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
 
int main()
{
    char *tmpfile = "tmpfile";
    char *str = "hello world\n";
    int fd;

    if((fd =creat(tmpfile,FILE_MODE)) < 0)
	err("failed to creat");
    
    if(write(fd,str,strlen(str)) != strlen(str))
	err("failed to write");


    assert(writew_lock(fd,0,SEEK_SET,0) == 0);

    printf("system 1\n");
    system("eic filelock.c tmpfile");

    assert(un_lock(fd,0,SEEK_SET,0) == 0);

    printf("system 2\n");
    system("eic filelock.c tmpfile");
    
    close(fd);
    remove(tmpfile);

    return 0;
}
    


