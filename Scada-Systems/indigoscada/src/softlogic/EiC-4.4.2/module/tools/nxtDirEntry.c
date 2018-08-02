#include <dirent.h>

char * nxtDirEntry(DIR * dirp)
{
  /*
   * nxtDirEntry reads the next directory
   * entry from the the directory dirp.
   *
   * Returns a static char *
   * on success, otherwise NULL
   *
   * Example:
   *        #include tools/nxtDirEntry.c
   *        DIR *dirp = opendir(".");
   *        char *p;
   *        while((p=nxtDirEntry(dirp))) printf("%s,\n",p);
   *        closedir(dirp);
   */
  static struct dirent *dp;
  dp = readdir(dirp);

  if(dp) 
    return dp->d_name;
  else
    return NULL;
}



