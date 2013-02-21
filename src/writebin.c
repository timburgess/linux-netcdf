/*
 *                                                            
 *  writebin.c                                                  
 *
 *    Writes a large binary file
 *
 *    2013-02   tim.burgess@noaa.gov                                      
 */

/* includes, system */
#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>

#include "all.h"

#include <sys/time.h>

/* beginning and end of Pathfinder data */
#define STARTYEAR 1981
#define STARTMONTH 8
#define STARTDAY   24
#define ENDYEAR    2009
#define ENDMONTH   12
#define ENDDAY     31

/* performance testing */
struct timeval t0, t1;


/* allocate memory on the heap */
unsigned short sst[DIMY][DIMX];
unsigned char quality[DIMY][DIMX];

int main() {
  
  int julday_start, julday_end;
  //julday_start = (int) ymd_to_jdnl(STARTYEAR, STARTMONTH, STARTDAY, 0);
  //julday_end = (int) ymd_to_jdnl(ENDYEAR, ENDMONTH, ENDDAY, 0);

  char *crw_dir;
  char sst_file[80], qual_file[80];
  
  crw_dir = getenv("CRWDIR");

  /* start timer */
  gettimeofday(&t0, NULL);


  //write_sst(current_day);


  /* end timer */
  gettimeofday(&t1, NULL);
  printf("Took %.2g seconds\n", t1.tv_sec - t0.tv_sec + 1E-6 * (double)(t1.tv_usec - t0.tv_usec));

  printf("Done.\n");
  return 0;
}
