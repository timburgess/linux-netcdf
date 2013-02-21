/*******************************************************************
 *  write_sst.c                                                
 *                                                                 
 *    Writes a 4096x8192 array of short ints to a file
 *                                                                 
 *       tim.burgess@noaa.gov                                      
 */

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "gapfill.h"


#define DEBUG 0

extern short *target;

void write_sst(int julian_day) {

  // determine year & day of year
  int year, month, day, yearday;
  jdnl_to_ymd(julian_day, &year, &month, &day, 0);
  yearday = day_of_year(year, month, day);

  // create new filename 
  size_t size = 50;
  char* filename = (char*)malloc(size);
  snprintf(filename, size, "/output/sst/%d/PFv5_d4km_sst_%d%03d.dat", year, year, yearday);

  int fp;
	ssize_t bytes_written;

  /* output and ensure buffer is flushed */
  fprintf(stderr, "Writing sst...");
  fp = creat(filename, 0664 );
//  posix_fallocate(fp, 0, sizeof(float) * 4096 * 8192);
  if (fp < 0) {
    printf("Failed to write %s...exiting\n", filename);
    printf("Error: %d\n", fp);    
    exit(1);
  }
  else {
    bytes_written = write(fp, target, sizeof(short) * NPIXELS);
  }  
	close(fp);
  printf("wrote %s\n", filename);
}
