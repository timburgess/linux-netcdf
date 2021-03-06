/*******************************************************************/
/*  read_sst.c                                                     */
/*                                                                 */
/*    Given a filepath, opens a Pathfinder HDF4 file, reads the    */
/*    SDS named 'sst' and then returns an 4096x8192 array of       */
/*    floats containing the SST values.                            */
/*                                                                 */
/*       tim.burgess@noaa.gov                                      */
/*******************************************************************/

#include "mfhdf.h"
#include "gapfill.h"

#define DEBUG 0

#define SST_DATASET_NAME "sst"


void read_sst(char* sst_filename, unsigned short* data) {

	int32 sd_id, sds_index, sds_id, status;
	int32 start[2] = {0,0}, edges[2] = {DIMY, DIMX};
	int32 rank, dim_sizes[2], data_type, n_attrs;
	char name[MAX_NC_NAME];
	int i;

  
	/* Open hdf file and find the dataset id named 'sst' */
	sd_id = SDstart(sst_filename, DFACC_READ);
	if (sd_id != FAIL) {
	  printf("Opened %s\n", sst_filename);
  }
  else {
    printf("Unable to open %s\n", sst_filename);
    exit(0);
  }
  
	sds_index = SDnametoindex(sd_id, SST_DATASET_NAME);
	if (sds_index == FAIL) {
	  printf("Dataset with the name %s does not exist\n", SST_DATASET_NAME);
    exit(0);
  }
  
  sds_id = SDselect(sd_id, sds_index);
	
	/* Get info about the sst dataset */
	SDgetinfo (sds_id, name, &rank, dim_sizes, &data_type, &n_attrs);
  if (DEBUG) {
    printf("name = %s\n", name);
    printf("rank = %d\n", rank);
    printf("dimension sizes are : ");
    for (i=0; i< rank; i++) printf ("%d ", dim_sizes[i]); printf ("\n");
    /*  printf("number of attributes is %d\n", n_attrs);*/
  }
  
  /* Check that data type should be 23 - unsigned 16bit integer */
  if (data_type != 23) {
     printf("ERROR: data type should be 23 but is %d\n", data_type);
     exit(1);
  }

  /* Read the array data */
  status = SDreaddata(sds_id, start, NULL, edges, (VOIDP) data);
  if (status == FAIL) {
    printf("Failed to read data: %d\n", status);
  }
  
	/* Terminate access to the array. */
	status = SDendaccess(sds_id);

	/* Terminate access to the SD interface and close the file. */
	status = SDend(sd_id);
	
}
