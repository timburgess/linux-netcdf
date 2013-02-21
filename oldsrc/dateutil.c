/*
 * Useful date functions
 * 
 * 2010-08-14 tim.burgess@noaa.gov
 *
 */

int day_of_year(int year, int month, int day);
void month_day(int year, int yearday, int *pmonth, int *pday);

static char daytab[2][13] = {
  {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

/* return day of year from year, month & day */
int day_of_year(int year, int month, int day) {
  
  int i,leap;

  leap = year%4 == 0 && (year%100 != 0 || year%400 == 0);
  for (i = 1; i < month; i++)
    day += daytab[leap][i];
  return day;
}

/* return month & day from day of year */
void month_day(int year, int yearday, int *pmonth, int *pday) {

  int i,leap;

  leap = year%4 == 0 && (year%100 != 0 || year%400 == 0);
  for (i = 1; yearday > daytab[leap][i]; i++)
    yearday -= daytab[leap][i];
  *pmonth = i;
  *pday = yearday;
}


