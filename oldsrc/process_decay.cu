/*
 * process_decay.cu
 *
 * Performs processing by copying data from host to device memory
 * and then running an exponential decay-based gapfill.
 *
 * All variables in GPU device memory are suffixed with '_d'
 *
 * tim.burgess@noaa.gov 
 */


// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <cuda.h>

#include "gapfill.h"

#define SEA_AIR_DELTA 0.75
#define DECAY 0.887

#define THREADS_PER_BLOCK 512


// error check
void checkCUDAError(const char *msg) {

  cudaError_t err = cudaGetLastError();
  if (cudaSuccess != err) {
    fprintf(stderr, "Cuda error: %s: %s.\n", msg, cudaGetErrorString(err));
    exit(EXIT_FAILURE);
  }  
  
}

// report on available device memory
void checkCUDAMemory() {

  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, 0);
  printf("-----------------------\n");
  printf("Name: %s\n", prop.name);
  printf("Compute capability: %d.%d\n", prop.major, prop.minor);
  printf("Total Global Memory: %ld Kbytes\n", prop.totalGlobalMem/1024);
  printf("Total Constant Memory: %ld Kbytes\n", prop.totalConstMem/1024);
  printf("Max threads per block: %d\n", prop.maxThreadsPerBlock);
  printf("-----------------------\n");
  
  //size_t freeMem, totalMem;
  //cudaMemGetInfo(&freeMem, &totalMem);

}

// init workarea  kernel
__global__ void init_workarea(float *workarea, unsigned short *daycount, float *airtemp) {

  int index = blockDim.x * blockIdx.x + threadIdx.x;
  if (index < NPIXELS) {
    *(workarea + index) = -3.0;
    *(daycount + index) = 0;
    *(airtemp + index) = -3.0;
  }
}


// gapfill kernel
__global__ void gapfill(short *target, float *workarea, unsigned short *sst, unsigned char *quality,
             unsigned char *lmask, unsigned short *daycount, float *airtemp, int indexcount) {
  
  int index = (blockDim.x * blockIdx.x + threadIdx.x) + indexcount;

  if (index < NPIXELS) { // if valid index
    
    // in landmask, 0 is land and 1 is ocean
    if (*(lmask+index) == 0) { // for land, set to -3
      *(target+index) = 0;
    }
    else { // if quality is > 3, update sst value in workarea
      if (*(quality + index) > 3) {
        *(daycount + index) = 0;
        *(workarea + index) = (*(sst + index)) * 0.075 - 3.0;
        *(airtemp + index) = (*(workarea + index)) * SEA_AIR_DELTA;
      }
      else {
        if (*(airtemp + index) != -3) { // no valid calc if a retrieval has not yet occurred
          ++*(daycount + index);
          *(workarea + index) = *(airtemp + index) + SEA_AIR_DELTA * __expf(-1 * (*(daycount + index)))/DECAY;
        }
      }

      // load workarea value into target
      *(target + index) = (short) (*(workarea + index) + 3) * 100;
    }
  }
}

extern unsigned char *landmask;
extern short *target;

// use device constant memory for landmask
__device__ __constant__ unsigned char *landmask_d;
__device__ unsigned short *sst_d;
__device__ unsigned char *quality_d;
__device__ unsigned short *daycount_d;
__device__ float *airtemp_d;
__device__ float *workarea_d;
__device__ short *target_d;

// memory sizes
size_t sstMemSize, qualityMemSize, targetMemSize;

void preprocess() {

  // report on free mem
  checkCUDAMemory();

  // allocate device memory for landmask
  size_t landMemSize = NPIXELS * sizeof(char);
  cudaMalloc((void **)&landmask_d, landMemSize);
  printf("allocating %ld device Kbytes for landmask\n", landMemSize/1024);
  checkCUDAError("memory allocation");
    
  // copy landmask from host to device
  cudaMemcpy(landmask_d, landmask, landMemSize, cudaMemcpyHostToDevice);
  checkCUDAError("memory copy");
 
  // allocate device memory for sst
  sstMemSize = NPIXELS * sizeof(unsigned short);
  cudaMalloc((void **)&sst_d, sstMemSize);
  printf("allocating %ld device Kbytes for sst\n", sstMemSize/1024);
  checkCUDAError("memory allocation");

  // allocate device memory for quality 
  qualityMemSize = NPIXELS * sizeof(char);
  cudaMalloc((void **)&quality_d, qualityMemSize);
  printf("allocating %ld device Kbytes for quality\n", qualityMemSize/1024);
  checkCUDAError("memory allocation");

  // allocate memory for daycount
  size_t dayMemSize = NPIXELS * sizeof(short);
  cudaMalloc((void **)&daycount_d, dayMemSize);
  printf("allocating %ld device Kbytes for daycount\n", dayMemSize/1024);

  // allocate memory for airtemp
  size_t airMemSize = NPIXELS * sizeof(float);
  cudaMalloc((void **)&airtemp_d, airMemSize);
  printf("allocating %ld device Kbytes for airtemp\n", airMemSize/1024);

  // allocate memory for workarea
  size_t workMemSize = NPIXELS * sizeof(float);
  cudaMalloc((void **)&workarea_d, workMemSize);
  printf("allocating %ld device Kbytes for workarea\n", workMemSize/1024);

  // init working values 
  init_workarea <<<32768, 1024>>> (workarea_d, daycount_d, airtemp_d);

  // allocate memory for target sst
  targetMemSize = NPIXELS * sizeof(short);
  target = (short*)malloc(targetMemSize);
  cudaMalloc((void **)&target_d, targetMemSize);
  printf("allocating %ld device Kbytes for target sst\n", targetMemSize/1024);
}

// main
void process(unsigned short* sst, unsigned char* quality) {
 
  // copy sst from host to device
  cudaMemcpy(sst_d, sst, sstMemSize, cudaMemcpyHostToDevice);
  checkCUDAError("memory copy");
    
  // copy quality from host to device
  cudaMemcpy(quality_d, quality, qualityMemSize, cudaMemcpyHostToDevice);
  checkCUDAError("memory copy");

  // run gafill twice due to memory constraints
  //gapfill <<<32768, 1024>>> (target_d, workarea_d, sst_d, quality_d, landmask_d, daycount_d, airtemp_d);
  gapfill <<<32768, THREADS_PER_BLOCK>>> (target_d, workarea_d, sst_d, quality_d, landmask_d, daycount_d, airtemp_d, 0);
  gapfill <<<32768, THREADS_PER_BLOCK>>> (target_d, workarea_d, sst_d, quality_d, landmask_d, daycount_d, airtemp_d, NPIXELS/2);

  // block until the device has completed
  cudaThreadSynchronize();
  
  // check for errors
  checkCUDAError("kernel invocation");
  
  // retrieve target data from device
  cudaMemcpy(target, target_d, targetMemSize, cudaMemcpyDeviceToHost);  
  
}

// wrapup after processing
void postprocess() {

  free(target);
  cudaFree(landmask_d);
  cudaFree(sst_d);
  cudaFree(quality_d);
  cudaFree(daycount_d);
  cudaFree(airtemp_d);
  cudaFree(workarea_d);
  cudaFree(target_d);

}



  
