/******************************************************************************

This file is part of ControlRoom process control/HMI software.

ControlRoom is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

ControlRoom is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
ControlRoom. If not, see http://www.gnu.org/licenses/

*******************************************************************************

For different licensing and/or usage of the sources apart from GPL or any other
open source license, please contact us at https://openapc.com/contact.php

*******************************************************************************/

/*
 * Copyright (c) 2004-2010
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "liboapc.h"
#include "platforms.h"

#ifdef ENV_WINDOWS

//#include <process.h>
//#include "Commctrl.h"



void showLog(const char *format,...)
{
   va_list  arglist;
   char     sText[1500];

   va_start(arglist,format);
   vsprintf(sText,format,arglist);
   va_end(arglist);
   printf("OpenIServer %s\r\n",sText);
}



#ifdef ENV_WINDOWSCE
int splitCmdLine(unsigned short *lpCmdLine,char *argv[])
{
int          ctr=1;
unsigned int i;
char        *c,*cprev,*cstart;
char        *cCmdLine;

cCmdLine=(char*)malloc(wcslen(lpCmdLine)+1);
if (!cCmdLine) return 0;
memset(cCmdLine,0,wcslen(lpCmdLine)+1);
for (i=0; i<wcslen(lpCmdLine); i++)
{
   cCmdLine[i]=(char)lpCmdLine[i];
}
if (strlen(cCmdLine)<2) return 0;
c=cCmdLine+1;
cprev=cCmdLine;
cstart=cCmdLine;
while (*c!=0)
   {
   if ((*c==' ') && (*cprev!='\\'))
      {
      *c=0;
      argv[ctr]=cstart;
      ctr++;
      cstart=c+1;
      }
   c++;
   cprev++;
   }
argv[ctr]=cstart;
free(cCmdLine);
return ctr;
}

#else

int splitCmdLine(char *cCmdLine,char *argv[])
{
   int          ctr=1;
   char        *c,*cprev,*cstart;

   if (strlen(cCmdLine)<2) return 0;
   c=cCmdLine+1;
   cprev=cCmdLine;
   cstart=cCmdLine;
   while (*c!=0)
   {
      if ((*c==' ') && (*cprev!='\\'))
      {
         *c=0;
         argv[ctr]=cstart;
         ctr++;
         cstart=c+1;
      }
      c++;
      cprev++;
   }
   argv[ctr]=cstart;
   return ctr;
}
#endif



bool LeaveServer(void)
{
   return false;
//TODO: implement end condition here
}


// ENV_WINDOWS
#else


#ifndef ENV_QNX
 #include <sys/signal.h>
#else
 #include <netdb.h>
#endif
#include <signal.h>
#include <errno.h>
#include <pthread.h>

static char quit=false;
extern bool verbose;


bool LeaveServer(void)
{
   return quit;
}



void signalh (const int sig)
{
//  setup_signal_traps ();
switch (sig) 
   {
   case SIGPIPE:
   case EPIPE:
      showLog("EPIPE received\n");
      break;
   case SIGHUP:
//      reloadSignal=true;
      break;
   case SIGBUS:
   case SIGFPE:
   case SIGIO:
   case SIGTERM:
      showLog("\nexiting\n");
      quit=true;
      break;
   case SIGINT:
      showLog("\nexiting\n");
      quit=1;
      break;
   case SIGQUIT:
      showLog("\nexiting\n");
      quit=1;
      break;
   }
}



void setupSignalTraps(void)
{
   signal (SIGINT, signalh);
   signal (SIGQUIT, signalh);
   signal (SIGFPE, signalh);
   signal (SIGBUS, signalh);
   signal (SIGTERM, signalh);
   signal (SIGHUP, signalh);
   signal (SIGIO, signalh);
   #ifdef ENV_SPARCSOLARIS
    signal (EPIPE, signalh);
    signal (SIGPIPE, SIG_IGN);
   #else
    signal (SIGPIPE, signalh);
   #endif
}



void showLog(const char *format,...)
{
   va_list arglist;
   char    sText[1500];
#ifdef FILELOG
   FILE   *FHandle;
#endif

   if (!verbose) return;
   va_start(arglist,format);
   vsprintf(sText,format,arglist);
   va_end(arglist);
   sText[strlen(sText)]='\0';
   printf("OpenIServer: %s\n",sText);
#ifdef FILELOG
   FHandle=fopen("/tmp/OpenIServer.log","a");
   if (FHandle)
   {
      fprintf(FHandle,"%s\n",sText);
      fclose(FHandle);
   }
#endif
}

#endif


