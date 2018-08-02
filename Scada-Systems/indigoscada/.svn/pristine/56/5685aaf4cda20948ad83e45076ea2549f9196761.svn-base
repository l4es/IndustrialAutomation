#ifndef _DIRENTH
#define _DIRENTH

#pragma push_safeptr

#include "sys/types.h"
#include "sys/dirent.h"

typedef struct
        {
        int     dd_fd;                  /* file descriptor */
        int     dd_loc;                 /* offset in block */
        int     dd_size;                /* amount of valid data */
        struct dirent *dd_buf;          /* -> directory block */
        }       DIR;                    /* stream data from opendir() */

/* POSIX.1 */
int closedir(DIR *dirp);
DIR *opendir(const char *filename);
struct dirent *readdir(DIR *dirp);
void rewinddir(DIR *dirp);

/* MISC */
void seekdir(DIR *dirp, long loc);
long telldir(DIR *dirp);

#pragma pop_ptr

#endif /* end _DIRENTH */
