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
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libaio.h>


/* prototypes */
int open_write(char const*);
void* make_write_buffer(size_t);


#define NBUFS 4    /* number of buffers to use */
#define PERMS 0666 /* file perms */

/* performance testing */
struct timeval t0, t1;



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

  long pagesize = sysconf(_SC_PAGESIZE);
  printf("Pagesize is %ld\n", pagesize);
  printf("Buffer size is %zuMB\n", size/1024/1024);
  void* buf = 0;
  int ret = posix_memalign((void**)&buf, pagesize, size);
  if (ret < 0 || buf == 0) {
    perror("Failed to create write buffer");
    exit(1);
  }
  memset(buf, 'a', size);
  return buf;
}

int main(int argc, char **argv) {
  
  /* buffer size */
  static const size_t SIZE = 32 * 4096 * 4096;

  printf("Writing %s\n", argv[1]);

  /* start timer */
  gettimeofday(&t0, NULL);

  /* prepare write buffers and open file */
  int fd = open_write(argv[1]);
  void *bufs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    void* buf = make_write_buffer(SIZE); 
    bufs[i] = buf;
  }

  /* prepare aio */
  io_context_t ctx;
  memset(&ctx, 0, sizeof(ctx));
  const int maxEvents = 32;
  io_setup(maxEvents, &ctx);

  /* setup request array */
  struct iocb *iocbs[NBUFS];
  for (int i = 0; i < NBUFS; i++) {
    struct iocb *io = (struct iocb*) malloc(sizeof(struct iocb));
    iocbs[i] = io;
  }


  /* prep requests */ 
  off_t offset = 0;
  io_prep_pwrite(iocbs[0], fd, bufs[0], SIZE, offset);
  offset += (int)SIZE;
  io_prep_pwrite(iocbs[1], fd, bufs[1], SIZE, offset);
  offset += (int)SIZE;
  io_prep_pwrite(iocbs[0], fd, bufs[2], SIZE, offset);
  offset += (int)SIZE;
  io_prep_pwrite(iocbs[0], fd, bufs[3], SIZE, offset);


  /* submit request */
  int status = io_submit(ctx, 2, iocbs);
  if (status < 0) {
    //int errno = -status;
    perror("iosubmit failed");
    exit(1);
  }

  /* check events */
  struct io_event events[10];
  long min_nr = 1;
  long nr = 10;
  int n_complete = io_getevents(ctx, min_nr, nr, events, NULL);
  printf("Completed %d events\n", n_complete);


  // wrap up
  if (close(fd) == -1) perror("Cannot close file");

  /* end timer */
  gettimeofday(&t1, NULL);
  printf("Took %.2g seconds\n", t1.tv_sec - t0.tv_sec + 1E-6 * (double)(t1.tv_usec - t0.tv_usec));

  io_destroy(ctx);
  for (int i = 0; i < NBUFS; i++) {
    free(bufs[i]);
  }

  printf("Done.\n");
  return 0;

}
