
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

void dirCrawl(char *dir, int depth)
{
    DIR *dp;

    struct dirent *entry;
    struct stat statbuf;

    int cf, cd=0;
    
    if((dp = opendir(dir)) == NULL) {
	fprintf(stderr,"cannont open directory; %s\n", dir);
	return;
    }
    chdir(dir);
    while((entry = readdir(dp)) != NULL) {

	stat(entry->d_name,&statbuf);

	/* ignore directories for test code */
	if (S_ISDIR(statbuf.st_mode)) {
	    continue;
	}

	/* count files */
	cf++;
	/*
	    if(strcmp(".",entry->d_name) == 0 ||
	       strcmp("..",entry->d_name) == 0)
		continue;
		*/
	if(strcmp(entry->d_name,"tstdirent.c") == 0)
	    printf("%*s%s  %ld\n",depth,"",entry->d_name,(long)statbuf.st_mtime);
	else
	    
	
	if(S_ISDIR(statbuf.st_mode)) 
	    dirCrawl(entry->d_name,depth+4);
    }

    printf("Okay = %d\n", cf > 10); 
    chdir("..");
    closedir(dp);
}


int main()
{
    dirCrawl(".",0);
    return 0;
}

