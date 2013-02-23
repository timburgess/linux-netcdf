/*
 *                                                            
 *  aiowrite.c                                                  
 *
 *    Writes a large binary file using the libaio
*     asynchronous library
 *
 *    2013-02   timburgess@mac.com
 */

/* enable O_DIRECT */
#define _GNU_SOURCE

/* includes, system */
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libaio.h>

#include "all.h"

/* prototypes */
int open_write(char const*);
void* make_write_buffer(size_t);


/* file perms */
#define PERMS 0666

/* performance testing */
struct timeval t0, t1;


/* allocate memory on the heap */
//float sst[NFLOATS];

/* open new file for write */
int open_write(char const* file) {

  int fd;
  if ((fd = open(file, O_DIRECT|O_CREAT|O_WRONLY|O_TRUNC, PERMS)) == -1) {
    perror("Could not open for write");
    exit(1);
  }
  return fd;

}

void* make_write_buffer(size_t size) {

  void* buf = 0;
  int ret = posix_memalign((void**)&buf, sysconf(_SC_PAGESIZE), size);
  if (ret < 0 || buf == 0) {
    perror("Failed to create write buffer");
    exit(1);
  }
  memset(buf, 'a', size);
  return buf;
}

int main(int argc, char **argv) {
  
  /* buffer size */
  static const size_t SIZE = 16 * 1024;

  printf("Writing %s\n", argv[1]);

  //printf("Allocated %dMB\n", (int) sizeof(sst)/1000000);
  //printf("Allocated %ld bytes\n", sizeof(sst));

  /* init array values
  for (int i = 0; i < NFLOATS; i++) {
    sst[i] = 23.4f;
  }*/


  /* start timer */
  gettimeofday(&t0, NULL);

  /* prepare file and write buffer */
  int fd = open_write(argv[1]);
  void* buf = make_write_buffer(SIZE); 

  /* prepare aio */
  io_context_t ctx;
  memset(&ctx, 0, sizeof(ctx));
  const int maxEvents = 32;
  io_setup(maxEvents, &ctx);


  /* setup request 
  struct iocb *iocbpp = (struct iocb*) malloc(sizeof(struct iocb));
  io_prep_pwrite(iocbpp, fd, buf, SIZE, 0);


  int status = io_submit(ctx, 1, &iocbpp);
  if (status < 0) {
    int errno = -status;
    perror("iosubmit failed");
    exit(1);
  }

  io_event events[10];
  int n = io_getevents(ctx, 1, 10, events, NULL);
*/

  // wrap up
  if (close(fd) == -1) perror("Cannot clode file");

  /* end timer */
  gettimeofday(&t1, NULL);
  printf("Took %.2g seconds\n", t1.tv_sec - t0.tv_sec + 1E-6 * (double)(t1.tv_usec - t0.tv_usec));

  io_destroy(ctx);
  //delete iocbpp;
  free(buf);

  printf("Done.\n");
  return 0;

}
