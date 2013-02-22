/*
 *                                                            
 *  readbin.c                                                  
 *
 *    Reads a large binary file
 *
 *    2013-02   timburgess@mac.com
 */

/* includes, system */
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <stdint.h>
#include <unistd.h>

#include "all.h"



/* performance testing */
struct timeval t0, t1;


/* allocate memory on the heap */
float sst[NFLOATS];

int main(int argc, char **argv) {
  

  char *crw_dir;
  crw_dir = getenv("CRWDIR");

  printf("Reading %s\n", argv[1]);


  /* start timer */
  gettimeofday(&t0, NULL);

  // open file
  int fd;
  if ((fd = open(argv[1], O_RDONLY)) == -1) {
    perror("Could not open for read");
    exit(1);
  }

  //  read from file
  ssize_t n_read;
  n_read = read(fd, sst, sizeof(sst));
  printf("Read %d bytes\n", (int)n_read);

  // close file
  close(fd);


  /* end timer */
  gettimeofday(&t1, NULL);
  printf("Took %.2g seconds\n", t1.tv_sec - t0.tv_sec + 1E-6 * (double)(t1.tv_usec - t0.tv_usec));

  printf("Done.\n");
  return 0;
}
