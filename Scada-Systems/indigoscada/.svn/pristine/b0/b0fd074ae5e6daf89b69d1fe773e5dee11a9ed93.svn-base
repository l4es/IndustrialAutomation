#ifndef FCNTLH_
#define FCNTLH_

#pragma push_safeptr

#include "sys/types.h"
#include "sys/fcntl.h"

#ifdef POWERPC
#define O_BINARY             0
#define O_TEXT               0
#endif



/* prototypes */

int open(const char *path, int access, ... /* mode_t mode */);
int creat(const char *path, mode_t amode);
int fcntl(int fd, int cmd, ... /* struct flock *flockptr*/);


#pragma pop_ptr

#endif






