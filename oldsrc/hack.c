/*
 *
 *  test.c
 *       tim.burgess@noaa.gov
 *
 *  A place for trying out various code
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
//#include <cuda.h>

#define NPIXELS 100

static char daytab[2][13] = {
  {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

int day_of_year(int year, int month, int day);

unsigned char *landmask_d;

int main() {

  /* allocate device memory for landmask
  size_t landMemSize = NPIXELS * sizeof(char);
  cudaMalloc((void **)&landmask_d, landMemSize);
  printf("allocating %ld device Kbytes for landmask\n", landMemSize/1024);
//  checkCUDAError("memory allocation"); */

 int foo = -1;
 if (foo) printf("foo\n");

  //printf("day of year:%d\n", day_of_year(2010, 8, 9) );

  return 0;
}


int day_of_year(int year, int month, int day) {

  int i, leap;


  leap = year%4 == 0 && (year%100 != 0 || year%400 == 0);

  for (i = 1; i < month; i++) 
    day += daytab[leap][i];

  return day;
}
