#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "lockreg.c"
#include "locktest.c"

void err(char *s)
{
    fprintf(stderr,"%s\n",s);
    exit(0);
}
 

void test4lock(char *fname)
{
     int fd = open(fname,O_RDWR);
     if(fd < 0)
	 err("failed to open file");

     assert(is_writelock(fd,0,SEEK_SET,0) != 0);
     close(fd);
	    
}

int main(int argc, char ** argv)
{

    if(argc != 2)
	err("wrong number of arguments");
    test4lock(argv[1]);
    return 0;
}
    


