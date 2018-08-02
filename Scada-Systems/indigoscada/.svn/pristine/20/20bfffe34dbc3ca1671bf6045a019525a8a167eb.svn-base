#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

void err(char *s)
{
    fprintf(stderr,"%s\n",s);
    exit(0);
}

void lockByteZero(int fd)
{
    struct flock lock;
    memset(&lock, 0, sizeof (lock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 1;
    if(fcntl(fd,F_SETLKW,&lock) < 0)
	err("failed to lock file");
}

int main()
{
    char * myfile = "myfile";
    char * str = "hello world\n";
    struct flock lock;
    int fd;

    errno = 0;

    fd = open(myfile,O_RDWR | O_CREAT, MODE);
    if(fd < 0)
	err("failed to creat myfile");

    write(fd,str,strlen(str));
    
    memset(&lock, 0, sizeof (lock));
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 1;

    lock.l_type = F_WRLCK;
    fcntl(fd,F_GETLK, &lock);
    assert(lock.l_type == F_UNLCK);

    lockByteZero(fd);

    lock.l_type = F_WRLCK;
    fcntl(fd,F_GETLK, &lock);
    assert(lock.l_type != F_UNLCK);

    if(remove(myfile) < 0)
	err("failed to remove myfile");
    return 0;
}
    
#ifdef EiCTeStS
main();
#endif
 









