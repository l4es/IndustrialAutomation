#include <stdarg.h>
#include <time.h>

#include "plclib.h"
#include "ui.h"
#include "util.h"

extern int UiReady;
FILE * ErrLog = NULL;

void plc_log(const char * msg, ...)
{
   va_list arg;
   time_t now;
   time(&now);
   char msgstr[MAXSTR];
   memset(msgstr,0,MAXSTR);
   va_start(arg, msg);
   vsprintf(msgstr,msg,arg);
   va_end(arg);
   if(!ErrLog)
      ErrLog = fopen(LOG,"w+");
   if(ErrLog){
      fprintf(ErrLog, "%s", msgstr);
      fprintf(ErrLog, ":%s", ctime(&now));
      fflush(ErrLog);
   }
   if(UiReady)
       ui_display_message(msgstr);
   else
       printf("%s\n",msgstr);
}

void close_log()
{
  if(ErrLog)
    fclose(ErrLog);
}

