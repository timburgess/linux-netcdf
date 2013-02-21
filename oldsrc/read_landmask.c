/*
 *  read_landmask.c                                                
 *                                                                 
 *    Given a filepath, opens the landmask file,                   
 *    and then returns an 4096x8192                                
 *    array of 8bit ints containing the quality flags.             
 *                                                                 
 *       tim.burgess@noaa.gov                                      
 */

#include <stdio.h>
#include <stdlib.h>
#include "gapfill.h"

/* posix_fadvise stuff */
#define _XOPEN_SOURCE 600
#include <unistd.h>
#include <fcntl.h>

#define DEBUG 0

extern unsigned char *landmask;

void read_landmask(char* crwdir) {

  landmask = (unsigned char*)malloc(NPIXELS * sizeof(char));

  size_t size = 53;
  char* filepath = (char*)malloc(size);
  snprintf(filepath, size, "%sPF4km/V5.0/landmask/pfv50_land_4km.dat", crwdir);


  FILE *fp;
	size_t bytes_read;

  fp = fopen(filepath, "rb");
  if (fp) {
    bytes_read = fread(landmask, sizeof(char), NPIXELS, fp);
    printf("Opened landmask %s\n", filepath);
  }
  else {
    printf("Failed to read %s...exiting\n", filepath);
    
  }  
	fclose(fp);
  free(filepath);

  int i = 0;
  while(*(landmask + i++) != 0);
  printf("first land pixel at %d\n", i);
}
