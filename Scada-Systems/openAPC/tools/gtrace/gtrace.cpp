/**
 * GTrace - GPS position tracing application
 * Copyright (C) 2010-2011 OpenAPC Inc. openapc(at)gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This software is not designed or intended for use in on-line control of
 * aircraft, air traffic, aircraft navigation or aircraft communications; or in
 * the design, construction, operation or maintenance of any nuclear
 * facility. Licensee represents and warrants that it will not use or
 * redistribute the Software for such purposes.
 */



#if defined (ENV_LINUX) || defined (ENV_QNX)
 #define ENV_POSIX
 #include <sys/socket.h>       /*  socket definitions        */
 #include <sys/types.h>        /*  socket types              */
 #include <arpa/inet.h>        /*  inet (3) funtions         */
 #include <unistd.h>           /*  misc. UNIX functions      */
 #include <pwd.h>
 #include <sched.h>
 #include <sys/select.h>       /* QNX only? */
#endif
#ifdef ENV_WINDOWS
 #include <windows.h>
#endif

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <stddef.h>
#include <pthread.h>

#ifndef ENV_QNX
#include "getopt.h"
#endif

#ifdef ENV_LINUX
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#endif

#ifdef ENV_POSIX
static bool              verbose=false,doBeep=true;
#endif

#ifdef ENV_QNX
#define MSG_NOSIGNAL 0
#endif

#define POLL_DELAY 500

#include "liboapc.h"
#include "oapc_libio.h"
#include "libjwarrior.h"

static void           *instData;
static double          lat=0.0,lon=0.0,height=0.0,speed=0.0,currTime=0.0;
static unsigned char   ok=0;
static FILE           *FHandle=NULL;
static int             logCnt=0;
static pthread_mutex_t dataMutex;
static void           *m_dynLib=NULL;

typedef void*          (*lib_oapc_create_instance2)(unsigned long flags);
typedef unsigned long  (*lib_oapc_init)            (void* instanceData);
typedef unsigned long  (*lib_oapc_exit)            (void* instanceData);
typedef void           (*lib_oapc_delete_instance) (void* instanceData);
typedef void           (*lib_oapc_set_io_callback) (void* instanceData,lib_oapc_io_callback oapc_io_callback,unsigned long callbackID);
typedef void           (*lib_oapc_set_config_data) (void* instanceData,const char *name,const char *value);
typedef unsigned long  (*lib_oapc_get_digi_value)(void* instanceData,unsigned long output,unsigned char *value);
typedef unsigned long  (*lib_oapc_get_num_value)(void* instanceData,unsigned long output,double *value);

static lib_oapc_create_instance2     gpsd_create_instance2;
static lib_oapc_init                 gpsd_init;
static lib_oapc_exit                 gpsd_exit;
static lib_oapc_delete_instance      gpsd_delete_instance;
static lib_oapc_set_io_callback      gpsd_set_io_callback;
static lib_oapc_set_config_data      gpsd_set_config_data;
static lib_oapc_get_digi_value       gpsd_get_digi_value;
static lib_oapc_get_num_value        gpsd_get_num_value;


static void beep(int freq)
{
#ifdef ENV_LINUX
   static int fd=-1;
   
   if (!doBeep) return;
   if (fd<=0) fd=open("/dev/tty10", O_RDONLY);
   if (fd>0) ioctl(fd, KDMKTONE,(750<<16)+(1193180/freq));
#else
#ifdef ENV_QNX
/* #include <stdlib.h>
#include <stdio.h>
#include <x86/inout.h>
#include <sys/neutrino.h>
#include <unistd.h>
#include <stdint.h>
#include <qnx/mma.h>

int main()
{
int system_control=0;
unsigned long i;
unsigned long kbd, clk;


ThreadCtl(_NTO_TCTL_IO, 0);

kbd = mmap_device_io(4,0x60);
clk = mmap_device_io(4,0x40);

// turn on bits 0 & 1 of kbd 0x61
system_control=in8(kbd + 1);
out8(kbd + 1,system_control | 0x03);

// load control word for 8254

out8(clk + 3, 0xb7); // bcd count, square wave,
// r/w lsb then msb, channel 2

// load counts
// to get 440Hz tone, divide 8254 clock (1.1892 MHz) by 2702
out8(clk + 2,0x02); // lsb count
out8(clk + 2,0x27); // msb count - remember, we're using BCD

// pause a bit...
sleep(1);

// shut it off
system_control=in8(kbd + 1);
out8(kbd + 1,system_control & 0xfc);
return(0);

}*/
#else
#error Not implemented!
#endif
#endif
}


/** definitions for the command line options */
static char shortopts[] = "vVhb";
#ifndef ENV_QNX
static struct option const longopts[] = {
  {"version",          no_argument, NULL, 'v'},
  {"verbose",          no_argument, NULL, 'V'},
  {"help",             no_argument, NULL, 'h'},
//  {"high-availability",no_argument, NULL, 'a'},
  {NULL, no_argument, NULL, 0}
};
#endif  



/** definitions for the command line help output */
#ifdef ENV_QNX
static char const *const option_help[] = {
  " -b     disable beep",
  " -V     print out some debugging information",
  " -v     show version info",
  " -h     show this help",
  0
};
#else
static char const *const option_help[] = {
  " -b     disable beep",
#ifdef ENV_POSIX
  " -V  --verbose              print out some debugging information",
#endif
  " -v  --version              show version info",
  " -h  --help                 show this help",
  0
};
#endif


/** print out version information */
static void version(void)
{
printf ("GTrace 0.6\n(c) 2010 by OpenAPC Int.\n");
}



/** print out command line help information */
static void usage (char *pname)
{
printf ("usage: %s [OPTIONS]\n",pname);
printf (" --help for more information\n");
return;
}



/**
 * Get the switches and related values out of the command line parameters
 * @param argc the number of arguments as handed over by main()
 * @param argv the arguments as handed over by main()
 */
static void getSwitches(int argc,char *argv[])
{
int                optc;
char const *const *p;

if (optind == argc) return;
#ifdef ENV_QNX
while ((optc = getopt(argc, argv, shortopts)) != -1)
#else
while ((optc = getopt_long (argc, argv, shortopts, longopts, (int *) 0)) != -1)
#endif
   {
   switch (optc)
      {
#ifndef ENV_WINDOWS
      case 'V':
         verbose=true;
         break;
#endif
      case 'b':
         doBeep=false;
         break;
      case 'v':
         version();
         exit (0);
         break;
      case 'h':
         usage (argv[0]);
         for (p = option_help; *p; p++) printf ("%s\n", *p);
         exit (0);
         break;
      default:
         usage (argv[0]);
      }
   }
}



static void rotateTracks()
{
   int i;
   char name1[200],name2[200];
   
   for (i=8; i>=0; i--)
   {
      sprintf(name1,"gtrace.00%d",i);
      sprintf(name2,"gtrace.00%d",i+1);
      rename(name1,name2);
   }
   rename("gtrace.curr","gtrace.000");
}



static void test_io_callback(unsigned long outputs,unsigned long callbackID)
{
   pthread_mutex_lock(&dataMutex);
   if (outputs==OAPC_DIGI_IO0) gpsd_get_digi_value(instData,0,&ok);
   else if (outputs==OAPC_NUM_IO1) gpsd_get_num_value(instData,1,&lat);
   else if (outputs==OAPC_NUM_IO2) gpsd_get_num_value(instData,2,&lon);
   else if (outputs==OAPC_NUM_IO3) gpsd_get_num_value(instData,3,&height);
   else if (outputs==OAPC_NUM_IO4) gpsd_get_num_value(instData,4,&speed);
   else if (outputs==OAPC_NUM_IO5) gpsd_get_num_value(instData,5,&currTime);
   else printf("Error: unknown output 0x%lX (%ld)!\n",outputs,callbackID);
   pthread_mutex_unlock(&dataMutex);
}



#ifdef ENV_WINDOWS
WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
WSADATA   wsaData;

#else
int main(int argc,char *argv[])
{
//void*          hInstance=NULL;
#endif
   int     ret,delayLeft,lastClock,currClock;
   char    param[10];
   bool    ljwIsOpen=false;
   
   beep(2500);
   if (pthread_mutex_init(&dataMutex,NULL)!=0)
   {
      printf("Error: could not create mutex (%d)\n",errno);
      exit(0);
   }
   getSwitches(argc,argv);
   rotateTracks();

   m_dynLib=oapc_dlib_load("libio_gpsd.so");
   if (!m_dynLib)
   {
      printf("Error: could not load GPSD Plug-In!");
      exit(0);
   }
   gpsd_set_config_data =(lib_oapc_set_config_data) oapc_dlib_get_symbol(m_dynLib,"oapc_set_config_data");
   gpsd_create_instance2=(lib_oapc_create_instance2)oapc_dlib_get_symbol(m_dynLib,"oapc_create_instance2");
   gpsd_delete_instance= (lib_oapc_delete_instance) oapc_dlib_get_symbol(m_dynLib,"oapc_delete_instance");
   gpsd_init=            (lib_oapc_init)            oapc_dlib_get_symbol(m_dynLib,"oapc_init");
   gpsd_exit=            (lib_oapc_exit)            oapc_dlib_get_symbol(m_dynLib,"oapc_exit");
   gpsd_set_io_callback= (lib_oapc_set_io_callback) oapc_dlib_get_symbol(m_dynLib,"oapc_set_io_callback");
   gpsd_get_digi_value=  (lib_oapc_get_digi_value)oapc_dlib_get_symbol(m_dynLib,"oapc_get_digi_value");
   gpsd_get_num_value=   (lib_oapc_get_num_value) oapc_dlib_get_symbol(m_dynLib,"oapc_get_num_value");


#ifdef ENV_POSIX
   if (verbose) printf("Initializing\n");
#endif
   instData=gpsd_create_instance2(0);
   if (instData)
   {
#ifdef ENV_POSIX
      if (verbose) printf("Connecting to gpsd and entering main loop\n");
#endif
      gpsd_set_io_callback(instData,test_io_callback,42);
      sprintf(param,"%d",POLL_DELAY);
      gpsd_set_config_data(instData,"polldelay",param);
      if (gpsd_init(instData)==OAPC_OK)
      {
         ret=ljw_open("/dev/input/js0",LJW_MAX_G_3);
         if (ret!=0) printf("ERROR: could not access accelerometer (%d)\n",ret);
         else
         {
            ljwIsOpen=true;
#ifdef ENV_POSIX
            if (verbose) printf("Accelerometer device opened successfully\n");
#endif
         }

         lastClock=(int)(clock()/(CLOCKS_PER_SEC/1000.0));
         while (1)
         {
//            if ((ok) || (ljwIsOpen))
            {
               pthread_mutex_lock(&dataMutex);
               if (!FHandle) FHandle=fopen("gtrace.curr","a");
               if (FHandle)
               {
                  char  *timeBuf;
                  time_t uTime;
                  double accX,accY,accZ,spdX,spdY,spdZ;
               
                  uTime=(time_t)currTime;
                  timeBuf=ctime(&uTime);
                  timeBuf[strlen(timeBuf)-1]=0;
                  if (!ok)
                  {
                     lat=0.0;
                     lon=0.0;
                  }
                  
                  if ((ok) && ((lat!=0) || (lon!=0)))
                  {
                     fprintf(FHandle,"\n%s\t%lf\t%lf\t%f",timeBuf,lat,lon,speed*3.6);
                     ljw_correct_speed(LJW_SPEED_X,speed);
                  }
                  else fprintf(FHandle,"\n------------------------\t---------\t---------\t---------");
                  
                  if ((ljwIsOpen) && (ljw_get_data(&accX,&accY,&accZ,&spdX,&spdY,&spdZ)==0))
                   fprintf(FHandle,"\t%f\t%f\t%f\t%f\t%f\t%f",accX,spdX*3.6,accY,spdY*3.6,accZ,spdZ*3.6);
                  fprintf(FHandle,"\n");
                  fflush(FHandle);
                  logCnt++;
                  if (logCnt>7500)
                  {
                     fclose(FHandle);
                     FHandle=NULL;
                     rotateTracks();
                     logCnt=0;
                  }
                  ok=false;
                  pthread_mutex_unlock(&dataMutex);
               }
               else printf("Error: could not create trackfile!\n");
            }
            currClock=(int)(clock()/(CLOCKS_PER_SEC/1000.0));
            delayLeft=POLL_DELAY-(currClock-lastClock);
            if (delayLeft>0) oapc_thread_sleep(delayLeft);
            lastClock=(int)(clock()/(CLOCKS_PER_SEC/1000.0));
         }
         gpsd_exit(instData);
      }
      else printf("Error: could not initialize!\n");
      gpsd_delete_instance(instData);
   }
   else printf("Error: could not create instance!\n");
   if (ljwIsOpen) ljw_close();
   if (FHandle) fclose(FHandle);
#ifdef ENV_POSIX
   if (verbose) printf("Exiting mainloop...\n");
#endif

#ifdef ENV_POSIX
   if (verbose) printf("Releasing buffered data...\n");
#endif
   pthread_mutex_destroy(&dataMutex);

   return 0;
}

