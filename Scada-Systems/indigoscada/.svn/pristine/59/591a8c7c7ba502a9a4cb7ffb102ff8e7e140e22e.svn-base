#ifndef UNISTDH_
#define UNISTDH_

#pragma push_safeptr

#include "sys/types.h"

/* Symbolic constants for the "access" routine: */
#define R_OK    4       /* Test for Read permission */
#define W_OK    2       /* Test for Write permission */
#define X_OK    1       /* Test for eXecute permission */
#define F_OK    0       /* Test for existence of File */

/* Standard file descriptors.  */
#define STDIN_FILENO    0       /* Standard input.  */
#define STDOUT_FILENO   1       /* Standard output.  */
#define STDERR_FILENO   2       /* Standard error output.  */


#ifndef SEEK_SET
#define SEEK_SET        0       /* Set file pointer to "offset" */
#endif

#ifndef SEEK_CUR
#define SEEK_CUR        1       /* Set file pointer to current plus "offset" */
#endif

#ifndef SEEK_END
#define SEEK_END        2       /* Set file pointer to EOF plus "offset" */
#endif

#ifndef NULL
#define NULL    (void*)0
#endif

int pause(void);
unsigned int alarm(unsigned int);
int access(const char *path, int mode);
int close(int handle);
int dup(int fd);
int dup2(int oldfd, int newfd);
pid_t fork(void);
int getpid(void);
int link(const char *oldpath, const char *newpath);
off_t lseek(int fildes, off_t offset, int whence);
int pipe(int *a);
ssize_t read(int fd, void  *buf, size_t count);
int rmdir(const char * path);
unsigned int sleep(unsigned int seconds);
int unlink(const char *fname);
ssize_t write(int fd, const void *buf, size_t count);
int chdir(const char *path);
char *getcwd(char *buf, size_t size);
pid_t tcgetpgrp(int);
int tcsetpgrp (int, pid_t);

#ifndef _EiC
/** not installed yet into EiC */
int execl(const char *s1, const char *s2, ...);

#endif

#pragma pop_ptr

#endif  /* end  UNISTDH_ */




