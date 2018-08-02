#include <dirent.h>
#include <string.h>

char * getDirEntry(char *dir, int nth)
{
	/* This function will return
	 * the `nth' filename in directory
         * `dir'.
         * 
	 *  On success it returns a static
         *  char * containing the filename
         *  else it returns NULL;
	 * 
         * Example:
         *     char *p;
         *     int i = 0;
         *     while((p=getDirEntry(".",++i))
         *           printf("%s\n",p);
	 */    

	static char fname[256];
	DIR * dirp;
	struct dirent *dp;
	int i;


	dirp = opendir(dir);  

	if(!dirp)
	    return NULL;
	for (i=0,dp = readdir(dirp); dp != NULL && i< nth; i++, dp = readdir(dirp))
		;

	closedir (dirp);

	if(dp) {
	   strcpy(fname,dp->d_name);
	   return fname;
	} else
             return NULL;
}



