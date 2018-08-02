#include "uilog.h"
#include "liboapc.h"

#include <stdio.h>


static FILE *FHandle=NULL;
static void *lock=NULL;
static char  filename[250+1];


static void uilog_rotate()
{
   bool isOpen=(FHandle!=NULL);
   int  size=0;

   if (isOpen) size=ftell(FHandle);
   if ((!isOpen) || (size>1500000))
   {
      char   filenameA[250+1],filenameB[250+1];
      size_t len=strlen(filename);

      if (isOpen) fclose(FHandle);
      strncpy(filenameB,filename,250);
      strncpy(filenameA,filenameB,250);

      strncat(filenameA,"3",250); strncat(filenameB,"2",250);
      unlink(filenameA);
      rename(filenameB,filenameA);

      filenameA[len]='2'; filenameB[len]='1';
      rename(filenameB,filenameA);

      rename(filename,filenameB);

      if (isOpen) FHandle=fopen(filename,"at");
   }
}


void uilog_open(const enum uiLogLevel /*level*/, const char *file)
{
   strncpy(filename,file,250);
   uilog_rotate();
   FHandle=fopen(file,"at");
   if (!FHandle) return;
   lock=oapc_thread_mutex_create();
   if (!lock)
   {
      fclose(FHandle);
      FHandle=NULL;
   }
}


void uilog_close()
{
   if (!FHandle) return;
   oapc_thread_mutex_lock(lock);
   fclose(FHandle);
   FHandle=NULL;
   oapc_thread_mutex_unlock(lock);
   oapc_thread_mutex_release(lock);
}


void uilog(const enum uiLogLevel level,const char *format, ...)
{
   if (!FHandle) return;
   oapc_thread_mutex_lock(lock);

   va_list arglist;

   fprintf(FHandle,"%d %d\t",(int)time(NULL),level);
   va_start(arglist,format);
   vfprintf(FHandle,format,arglist);
   va_end(arglist);
   fprintf(FHandle,"\n");
   if (level<info) uilog_rotate();
   oapc_thread_mutex_unlock(lock);
}
