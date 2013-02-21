/*
 * get_sstfile.c
 *
 * Returns full pathname of sstfile for a given julian day
 *
 * The year/day dependent logic that determines the correct Pathfinder
 * filename is taken from the PFanalysis/get_pf_filename.pro code.
 *
 * tim.burgess@noaa.gov
 *
 * WARNING: This logic may need to change when yearly data changes from
 *          interim to final.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gapfill.h"


int get_sstfiles(long julianday, char *crwpath, char *sstfilepath, char *qualfilepath) {

  int year, month, day, day_of_year;
  long start_day; 

  /* get year and day of year from julian day */
  jdnl_to_ymd(julianday, &year, &month, &day, 0); 
  start_day = ymd_to_jdnl(year, 1, 1, 0); 
  day_of_year = (int)(julianday - start_day) + 1;

  /* determine appropriate suffixes */

  char sst_end[] = ".s04d1pfv50-sst-16b.hdf";
  char qual_end[] = ".m04d1pfv50-qual.hdf";
   
  if (year <= 1981 && day_of_year < 236) { /* date is before start of dataset */
      printf("Non-existent sst file date requested: %d %d\n", year, day_of_year);
      exit(1);
  }

  switch (year) {
    case 1981:
    case 1982: if (year == 1982 && ((day_of_year >= 148 && day_of_year <= 151) || 
                                   (day_of_year >= 268 && day_of_year <= 269))) {
                 return -1; /* missing days */
               }
    case 1983: if (year == 1983 && day_of_year >= 218 && day_of_year <= 219) {
                 return -1; /* missing days */
               }
    case 1984: if (year == 1984 && ((day_of_year >= 14 && day_of_year <= 15) ||
                                   day_of_year == 84 || day_of_year == 342)) {
                 return -1; /* missing days */
               }
    case 1985: 
               if (year == 1985 && day_of_year >= 40 && day_of_year <= 42) {
                 return -1; /* missing days */
               } else if (year == 1985 && day_of_year > 9) {
                 strcpy(sst_end, ".s04d1pfv50-sst-16b.hdf");
                 strcpy(qual_end, ".m04d1pfv50-qual.hdf");
               } else {
                 strcpy(sst_end, ".s04d1pfv51-sst.hdf");
                 strcpy(qual_end, ".m04d1pfv51-qual.hdf");
               }
               break;

    case 2002: strcpy(sst_end, ".s04d1pfv50-sst.hdf");
               strcpy(qual_end, ".m04d1pfv50-qual.hdf");
               break;
    case 2003: 
    case 2004: strcpy(sst_end, ".s04d4pfv50-sst.hdf");
               strcpy(qual_end, ".m04d4pfv50-qual.hdf"); 
               break;
    case 2005:
               if (day_of_year <= 155) {
                 strcpy(sst_end, ".s04d4pfv50-sst.hdf");
                 strcpy(qual_end, ".m04d4pfv50-qual.hdf");
               } else { /* 2AM satellite becomes available */
                 strcpy(sst_end, ".s04d1pfv50-sst.hdf");
                 strcpy(qual_end, ".m04d1pfv50-qual.hdf");
               }
               break;
    case 2006:
               strcpy(sst_end, ".s04d1pfv50-sst.hdf");
               strcpy(qual_end, ".m04d1pfv50-qual.hdf");
               break;
    case 2007:
    case 2008:
    case 2009:
               strcpy(sst_end, ".s04d1pfrt-sst.hdf");
               strcpy(qual_end, ".m04d1pfrt-qual.hdf");
               break;
  }

  /* add prefix of year and day */
  size_t size = 47; /* maximum size of filepath */
  char *buf = (char*)malloc(size);
  snprintf(buf, size, "%sPF4km/all/SST_daily_%d/%d%03d", crwpath, year, year, day_of_year);

  strcpy(sstfilepath, buf);
  strcat(sstfilepath, sst_end);
  strcpy(qualfilepath, buf);
  strcat(qualfilepath, qual_end);

  return 0; /* return SUCCESS */
}

