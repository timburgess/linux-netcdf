/** \file
Write a file demonstrating some of the features of netCDF-4.

We create two shared dimensions, "x" and "y", in a parent group, and
some netCDF variables in different subgroups. The variables will
include a compound and an enum type, as well as some of the new atomic
types, like the unsigned 64-bit integer.

This is part of the netCDF package. Copyright 2006-2011 University
Corporation for Atmospheric Research/Unidata. See COPYRIGHT file for
conditions of use. Full documentation of the netCDF can be found at
http://www.unidata.ucar.edu/software/netcdf/docs.
*/

/* Mods by timburgess:
 - first argument is filename to write to
*/

#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>
#include <sys/time.h>

/* This is the name of the data file we will create. */
//#define FILE_NAME "simple_nc4.nc"

/* We are writing 2D data, a 6 x 12 grid. */
#define NDIMS 2
#define NX 16384
#define NY 16384

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

/* performance testing */
struct timeval t0, t1;


int main(int argc, char **argv) {
 
   /* When we create netCDF variables, groups, dimensions, or types,
    * we get back an ID for each one. */
   int ncid, x_dimid, y_dimid, varid1;
   int dimids[NDIMS];

   /* This is the data array we will write. It will be filled with a
    * progression of numbers for this example. */
   static float data[NX][NY];

   /* Loop indexes, and error handling. */
   int x, y, retval;


   /* start timer */
   gettimeofday(&t0, NULL);


   /* Create some pretend data. */
   for (x = 0; x < NX; x++)
      for (y = 0; y < NY; y++)
      {
         data[x][y] = (float) (x * NY + y);
      }

   /* Create the file. The NC_NETCDF4 flag tells netCDF to
    * create a netCDF-4/HDF5 file.*/
   if ((retval = nc_create(argv[1], NC_NETCDF4|NC_CLOBBER, &ncid)))
      ERR(retval);

   /* Define the dimensions in the root group. Dimensions are visible
    * in all subgroups. */
   if ((retval = nc_def_dim(ncid, "x", NX, &x_dimid)))
      ERR(retval);
   if ((retval = nc_def_dim(ncid, "y", NY, &y_dimid)))
      ERR(retval);

   /* The dimids passes the IDs of the dimensions of the variable. */
   dimids[0] = x_dimid;
   dimids[1] = y_dimid;

   /* Define an unsigned 64bit integer variable in grp1, using dimensions
    * in the root group. */
   if ((retval = nc_def_var(ncid, "data", NC_UINT64, NDIMS, 
                            dimids, &varid1)))
      ERR(retval);

   /* Write unsigned long long data to the file. For netCDF-4 files,
    * nc_enddef will be called automatically. */
   if ((retval = nc_put_var_float(ncid, varid1, &data[0][0])))
      ERR(retval);


   /* Close the file. */
   if ((retval = nc_close(ncid)))
      ERR(retval);

   /* end timer */
   gettimeofday(&t1, NULL);
   printf("Took %.2g seconds\n", t1.tv_sec - t0.tv_sec + 1E-6 * (double)(t1.tv_usec - t0.tv_usec));


   printf("*** wrote example file\n");
   return 0;
}
