/*
 * process_persist.cu
 *
 * Performs processing by copying data from host to device memory
 * and then running a persistence-based gapfill.
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

  size_t freeMem, totalMem;
  cudaMemGetInfo(&freeMem, &totalMem);
  printf("total: %ld Kbytes free: %ld Kbytes\n", totalMem/1024, freeMem/1024);

}

// init workarea  kernel
__global__ void init_workarea(float *workarea) {

  int index = 1024 * blockIdx.x + threadIdx.x;
  *(workarea + index) = -3.0;
}


// gapfill kernel
__global__ void gapfill(short *target, float *workarea, unsigned short* sst, unsigned char* quality,
             unsigned char *lmask) {
  
  int index = 1024 * blockIdx.x + threadIdx.x;
    
  // in landmask, 0 is land and 1 is ocean
  if (*(lmask+index) == 0) { // for land, set to -3
    *(target+index) = 0;
  }
  else { // if quality is > 3, update sst value in workarea
    if (*(quality+index) > 3) {
      *(workarea+index) = (*(sst+index))*0.075 - 3.0;
    }

    // simply load existing workarea value into target
    *(target+index) = (short) ((*(workarea+index)) + 3) * 100;
     
  }

}

extern unsigned char *landmask;
extern short *target;

// use device constant memory for landmask
__device__ __constant__ unsigned char *landmask_d;
__device__ unsigned short *sst_d;
__device__ unsigned char *quality_d;
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

  // allocate memory for workarea
  size_t workMemSize = NPIXELS * sizeof(float);
  cudaMalloc((void **)&workarea_d, workMemSize);
  printf("allocating %ld device Kbytes for workarea\n", workMemSize/1024);

  // init workarea values to -3
  init_workarea <<<32768, 1024>>> (workarea_d);

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

  // do calc on device
  //dim3 dimGrid(1024,128);
  //dim3 dimBlock(16,16);
  gapfill <<<32768, 1024>>> (target_d, workarea_d, sst_d, quality_d, landmask_d);

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
  cudaFree(workarea_d);
  cudaFree(target_d);

}
