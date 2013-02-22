/*
 *                                                            
 *  writebin.c                                                  
 *
 *    Writes a large binary file
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


/* write size */
#define PERMS 0666
#define NFLOATS 500000000

/* performance testing */
struct timeval t0, t1;


/* allocate memory on the heap */
float sst[NFLOATS];

int main(int argc, char **argv) {
  

  char *crw_dir;
  crw_dir = getenv("CRWDIR");

  printf("Writing %s\n", argv[1]);
  printf("Allocated %dMB\n", (int) sizeof(sst)/1000000);
  printf("Allocated %ld bytes\n", sizeof(sst));

  // init array values
  for (int i = 0; i < NFLOATS; i++) {
    sst[i] = 23.4f;
  }


  /* start timer */
  gettimeofday(&t0, NULL);

  // open file
  int fd;
  if ((fd = open(argv[1], O_CREAT|O_WRONLY|O_TRUNC, PERMS)) == -1) {
    perror("Could not open for write");
    exit(1);
  }

  // write to file
  int n_written;
  n_written = write(fd, sst, sizeof(sst));
  printf("    Wrote %d bytes\n", n_written);

  // close file
  close(fd);


  /* end timer */
  gettimeofday(&t1, NULL);
  printf("Took %.2g seconds\n", t1.tv_sec - t0.tv_sec + 1E-6 * (double)(t1.tv_usec - t0.tv_usec));

  printf("Done.\n");
  return 0;
}
