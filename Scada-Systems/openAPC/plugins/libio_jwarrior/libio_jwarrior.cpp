/********************************************************************************************

These sources are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. All
the information given here, within the interface descriptions and within the specification
are subject to change without notice. Errors and omissions excepted.

These sources demonstrate the usage of the OpenAPC Plug-In programming interface. They can be
used freely according to the OpenAPC Dual License: As long as the sources and the resulting
applications/libraries/Plug-Ins are used together with the OpenAPC software, they are
licensed as freeware. When you use them outside the OpenAPC software they are licensed under
the terms of the GNU General Public License.

For more information please refer to http://www.openapc.com/oapc_license.php

*********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#ifndef ENV_WINDOWSCE
#include <sys/types.h>
#else
#include "Winsock2.h"
#endif

#ifdef ENV_LINUX
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <linux/joystick.h>
#include <unistd.h>
#endif

#ifdef ENV_QNX
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <termios.h>
#include <unistd.h>
#endif

#ifdef ENV_WINDOWS
#include <windows.h>
#endif


#define G_VAL 9.81


#include "oapc_libio.h"
#include "liboapc.h"

#define MAX_XML_SIZE    5500
#define MAX_PORTSIZE      50

struct libio_config
{
   unsigned short version,length;
   char           port[MAX_PORTSIZE+4];
   char           m_joynum,m_maxG;
};



struct instData
{
   struct libio_config         config;
   double                      m_spdX,m_spdY,m_spdZ;
   double                      m_accX,m_accY,m_accZ;
   double                      m_maxGVal;
#ifdef ENV_LINUX
   int                         m_handle;
#else
   bool                        m_running;
#endif
   int                         m_callbackID;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
<flowimage>%s</flowimage>\n\
<dialogue>\n\
 <general>\n"
#ifdef ENV_LINUX
" <param>\n\
   <name>port</name>\n\
   <text>Interface</text>\n\
   <type>fileload</type>\n\
   <default>%s</default>\n\
   <min>4</min>\n\
   <max>%d</max>\n\
   <ffilter>All devices|*</ffilter>\n\
  </param>\n"
#endif
#ifdef ENV_WINDOWS
" <param>\n\
   <name>joynum</name>\n\
   <text>Joystick Port</text>\n\
   <type>option</type>\n\
   <value>0</value>\n\
   <value>1</value>\n\
   <value>2</value>\n\
   <value>3</value>\n\
   <value>4</value>\n\
   <value>5</value>\n\
   <value>6</value>\n\
   <value>7</value>\n\
   <value>8</value>\n\
   <value>9</value>\n\
   <value>10</value>\n\
   <value>11</value>\n\
   <value>12</value>\n\
   <value>13</value>\n\
   <value>14</value>\n\
   <value>15</value>\n\
   <default>%d</default>\n\
  </param>\n"
#endif
" <param>\n\
   <name>maxg</name>\n\
   <text>Calibrated Maximum</text>\n\
   <unit>G</unit>\
   <type>option</type>\n\
   <value>1</value>\n\
   <value>1,5</value>\n\
   <value>2</value>\n\
   <value>3</value>\n\
   <value>4</value>\n\
   <value>8</value>\n\
   <value>16</value>\n\
   <default>%d</default>\n\
  </param>\n\
 </general>\n\
 <helppanel>\n\
  <in3>SPD X - reset to speed in X direction (m/sec)</in3>\n\
  <in4>SPD Y - reset to speed in Y direction (m/sec)</in4>\n\
  <in5>SPD Z - reset to speed in Z direction (m/sec)</in5>\n\
  <out0>ACC X - acceleration X direction</out0>\n\
  <out1>ACC Y - acceleration Y direction</out1>\n\
  <out2>ACC Z - acceleration Z direction</out2>\n\
  <out3>SPD X - calculated speed X direction (m/sec)</out3>\n\
  <out4>SPD Y - calculated speed Y direction (m/sec)</out4>\n\
  <out5>SPD Z - calculated speed Z direction (m/sec)</out5>\n\
 </helppanel>\n\
</dialogue>\n\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                        flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUEAgSCgoYuyt4rR11IZnscqMTBw8ev8fcZKTk0UW9HbZFXh6yWqbeouMUhO1HH2OZSeZoE/vzc5e144PB87vQOGSiMlqBumbf8/vxfcHlehqQkNUM0Wnh9qslsosQ83uwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADsuISOAAACBklEQVR42u2Y63KDIBCFUcxGVJSYeAXT93/LrrFttHUNGYjTznR/kJxkyMfCAckytmO01sGC0+kUsEeC6v0MChj+IINHwgdqHPx96LTwgcLB34dOCx8oHDyzET5QMB86LXygWmYnfKAc4h/1j/pLKDbfP8uOkuecCxMlSvV10wyDlIXbwTQHzHuWuqriWOdIM4nq06a5DkPp8bS4d4a4yrtOBYFSjcqkBABHFJkVVFU9TaQssukjNxS9Vl+oAuBjjYZr8RIHTiiJUfdjW74ahfZLErSFUpJEbbiYHtlcyAnV9yoRCltEXZv1rMj1/jGIdYFZ6QgMZmU47q1EFtAQKNLF4zs7VJdlAqdP5LiPTSCLJn06q9lXi3vjQiAqj2pujOA57mLRZzJNn1wrNiVmYQteAxdCcI0tz0ogUM5XC7RF0Laac65jPAw1ekSpwmkLM0LADRVrreMK2/jmfLsJpMQ2qusM5mWirsObWpKAC2rLFsH4egnD8O1yHN9G6yh6b1pO8Wj2EuMcHg7h+YjnUkmhrF3B1kUZV9MDC52OXsfA55brWq2Lth5Z6Ig8RwOOIaKsdUBtpNVCfYvsM74/772t1e+8MS3+FNPCA4rem6xlfrPaEbXjBO5ni0WxxFlso+zKLD6qMZZlFi/VGMsyi49qjGWZxUs1xrLM4qMaY1lmcazG7BbvWqWfDgVFKHYAAAAASUVORK5CYII=";
static char                        libname[]="JoyWarrior Accelerometer";
static char                        xmldescr[MAX_XML_SIZE+1];
static struct libio_config         save_config;
static lib_oapc_io_callback        m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports



/**
This function has to return the name that is used to display this library within the main application.
The name returned here has to be short and descriptive
@param[out] data pointer to the beginning of an char-array that contains the name
@return the length of the name structure or 0 in case of an error
*/
OAPC_EXT_API char *oapc_get_name(void)
{
   return libname;
}



/**
This function returns a set of OAPC_HAS_... flags that describe the general capabilities of this
library. These flags can be taken from oapc_libio.h
@return or-concatenated flags that describe the libraries capabilities
*/
OAPC_EXT_API unsigned long oapc_get_capabilities(void)
{
   return OAPC_HAS_INPUTS|OAPC_HAS_OUTPUTS|
          OAPC_HAS_XML_CONFIGURATION|
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|OAPC_ACCEPTS_IO_CALLBACK|
          OAPC_FLOWCAT_IO;
}



/**
When the OAPC_HAS_INPUTS flag is set, the application calls this function to get the configuration
for the inputs. Here "input" is defined from the libraries point of view, means data that are sent from
the application to the library are input data
@return or-concatenated OAPC_..._IO...-flags that describe which inputs and data types are used or 0 in
        case of an error
*/
OAPC_EXT_API unsigned long oapc_get_input_flags(void)
{
   return OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5;
   // avoid it to submit the same IO number for different data types, that would cause displaying problems for the flow symbol
}



/**
When the OAPC_HAS_OUTPUTS flag is set, the application calls this function to get the configuration
for the outputs. Here "output" is defined from the libraries point of view, means data that are sent from
the library to the application are output data
@return or-concatenated OAPC_..._IO...-flags that describe which outputs and data types are used or 0 in case
        of an error
*/
OAPC_EXT_API unsigned long oapc_get_output_flags(void)
{
   return OAPC_NUM_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|
          OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5;
   // avoid it to submit the same IO number for different data types, that would cause displaying problems for the flow symbol
}



/**
When the OAPC_HAS_XML_CONFIGURATION capability flag was set this function is called to retrieve an XML
structure that describes which configuration information have to be displayed within the main application.
@param[out] data pointer to the beginning of an char-array that contains the XML data
@return the length of the XML structure or 0 in case of an error
*/
OAPC_EXT_API char *oapc_get_config_data(void* instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   sprintf(xmldescr,xmltempl,flowImage,
#ifdef ENV_LINUX
                    data->config.port,MAX_PORTSIZE,
#endif
#ifdef ENV_WINDOWS
                    data->config.m_joynum,
#endif
                    data->config.m_maxG);
#ifdef _DEBUG
   long l=strlen(xmldescr);
   l=l;
   assert(strlen(xmldescr)<MAX_XML_SIZE);
#endif
   return xmldescr;
}



/**
When the OAPC_ACCEPTS_PLAIN_CONFIGURATION capability flag was set this function is called for every configurable parameter
to return the value that was set within the application.
@param[in] name the name of the parameter according to the unique identifier that was set within the <name></name> field of the
           XML config file
@param[in] value the value that was configured for this parameter; in case it is not a string but a number it has to be converted,
           the representation sent by the application is always a string
*/
OAPC_EXT_API void oapc_set_config_data(void* instanceData,const char *name,const char *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (strcmp(name,"port")==0)        strncpy(data->config.port,value,MAX_PORTSIZE);
   else if (strcmp(name,"maxg")==0)   data->config.m_maxG=atoi(value);
   else if (strcmp(name,"joynum")==0) data->config.m_joynum=atoi(value);
}



/**
This function delivers the data that are stored within the project file by the main application. It is
recommended to put two fields "version" and "length" into the data structure that is handed over here.
So when the data to be saved need to be changed it is easy to recognize which version of the data structure
is used, possible conversions can be done easier in oapc_set_loaded_data().<BR>
PLEASE NOTE: In order to keep the resulting project files compatible with all possible platforms the
             application is running at you have to store all values in network byteorder
@param[out] length the total effective length of the data that are returned by this function
@return the data that have to be saved
*/
OAPC_EXT_API char *oapc_get_save_data(void* instanceData,unsigned long *length)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   *length=sizeof(struct libio_config);
   save_config.version    =htons(data->config.version);
   save_config.length     =htons(data->config.length);
   strncpy(save_config.port,data->config.port,MAX_PORTSIZE);
   save_config.m_maxG=     data->config.m_maxG;
   save_config.m_joynum=   data->config.m_joynum;

   return (char*)&save_config;
}



/**
This function receives the data that have been stored within the project file by the main application. It is
recommended to check if the returned data are really what the library expects. To do that two fields
"version" and "length" within the saved data structure should be checked.<BR>
PLEASE NOTE: In order to keep the resulting project files compatible with all possible platforms the
             application is running at you have to convert all the values that have been stored in network
             byteorder back to the local byteorder
@param[in] length the total effective length of the data that are handed over to this function
@param[in] data the configuration data that are loaded for this library
*/
OAPC_EXT_API void oapc_set_loaded_data(void* instanceData,unsigned long length,char *loadedData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (length>sizeof(struct libio_config)) length=sizeof(struct libio_config);
   memcpy(&save_config,loadedData,length);
   data->config.version    =ntohs(save_config.version);
   data->config.length     =ntohs(save_config.length);
   strncpy(data->config.port,save_config.port,MAX_PORTSIZE);
   data->config.m_maxG     =save_config.m_maxG;
   data->config.m_joynum   =save_config.m_joynum;
}



/**
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API void* oapc_create_instance2(unsigned long flags)
{
   struct instData *data;

   flags=flags; // removing "unused" warning
   data=(struct instData*)malloc(sizeof(struct instData));
   memset(data,0,sizeof(struct instData));

   data->config.version=1;
   data->config.length=sizeof(struct libio_config);
#ifdef ENV_LINUX
   data->m_handle=-1;
#else
   data->m_running=true;
#endif
   strcpy(data->config.port,"/dev/input/js0");
   data->config.m_maxG=4;
   data->config.m_joynum=1;
   return data;
}



/**
This function is called finally, it has to be used to release the instance data structure that was created
during the call of oapc_create_instance()
*/
OAPC_EXT_API void oapc_delete_instance(void* instanceData)
{
   if (instanceData) free(instanceData);
}



static void *recvLoop(void *arg)
{
   struct instData *data;
   double           acc,delta_t;
   clock_t          tx,ty,tz,curr_t;
#ifdef ENV_WINDOWS
   JOYINFOEX        pji;
   unsigned int     prevX=100000,prevY=100000,prevZ=100000;
   bool             newData;
#else
   struct js_event  e;
#endif

printf("jwarrior thread func\n");
   data=(struct instData*)arg;
   tx=clock();
   ty=clock();
   tz=clock();
printf("Recv loop entered\n");
#ifdef ENV_WINDOWS
   while(data->m_running)
   {
      pji.dwSize=sizeof(pji);
      pji.dwFlags=JOY_CAL_READ3;
      if (joyGetPosEx(data->config.m_joynum-1,&pji)==JOYERR_NOERROR)
      {
         curr_t=clock();
         newData=false;
         if (prevX!=pji.dwXpos)
         {
            prevX=pji.dwXpos;
            acc=((pji.dwXpos/65535.0)-0.5)*data->m_maxGVal;
            data->m_accX=acc;
            m_oapc_io_callback(OAPC_NUM_IO0,data->m_callbackID);
            delta_t=double(curr_t-tx)/CLOCKS_PER_SEC;
            data->m_spdX+=(acc*delta_t);
            m_oapc_io_callback(OAPC_NUM_IO3,data->m_callbackID);
            tx=curr_t;
            newData=true;
         }
         if (prevY!=pji.dwYpos)
         {
            prevY=pji.dwYpos;
            acc=((pji.dwYpos/65535.0)-0.5)*data->m_maxGVal;
            data->m_accY=acc;
            m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
            delta_t=double(curr_t-ty)/CLOCKS_PER_SEC;
            data->m_spdY+=(acc*delta_t);
            m_oapc_io_callback(OAPC_NUM_IO4,data->m_callbackID);
            ty=curr_t;
            newData=true;
         }
         if (prevZ!=pji.dwZpos)
         {
            prevZ=pji.dwZpos;
            acc=((pji.dwZpos/65535.0)-0.5)*data->m_maxGVal;
            data->m_accZ=acc;
            m_oapc_io_callback(OAPC_NUM_IO2,data->m_callbackID);
            delta_t=double(curr_t-tz)/CLOCKS_PER_SEC;
            data->m_spdZ+=(acc*delta_t);
            m_oapc_io_callback(OAPC_NUM_IO5,data->m_callbackID);
            tz=curr_t;
            newData=true;
         }
         if (!newData) oapc_thread_sleep(10);
      }
#else
   while (read(data->m_handle, &e, sizeof(struct js_event))==sizeof(struct js_event))
   {
printf("Getting data (%d / %d)\n",e.type,e.number);
      switch(e.type)
      {
         case JS_EVENT_BUTTON:
            break;
         case JS_EVENT_AXIS:
            acc=(e.value/32767.0)*data->m_maxGVal;
            curr_t=clock();
            if (e.number==0)
            {
               data->m_accX=acc;
               m_oapc_io_callback(OAPC_NUM_IO0,data->m_callbackID);
               delta_t=double(curr_t-tx)/CLOCKS_PER_SEC;
               data->m_spdX+=(acc*delta_t);
               m_oapc_io_callback(OAPC_NUM_IO3,data->m_callbackID);
               tx=curr_t;
            }
            else if (e.number==1)
            {
               data->m_accY=acc;
               m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
               delta_t=double(curr_t-ty)/CLOCKS_PER_SEC;
               data->m_spdY+=(acc*delta_t);
               m_oapc_io_callback(OAPC_NUM_IO4,data->m_callbackID);
               ty=curr_t;
            }
            else if (e.number==2)
            {
               data->m_accZ=acc;
               m_oapc_io_callback(OAPC_NUM_IO2,data->m_callbackID);
               delta_t=double(curr_t-tz)/CLOCKS_PER_SEC;
               data->m_spdZ+=(acc*delta_t);
               m_oapc_io_callback(OAPC_NUM_IO5,data->m_callbackID);
               tz=curr_t;
            }
        case JS_EVENT_BUTTON | JS_EVENT_INIT:
	    	   break;
         case JS_EVENT_AXIS | JS_EVENT_INIT:
 		      break;
		}
#endif
   }
#ifdef ENV_WINDOWS
   data->m_running=true;
#endif
   return NULL;
}


/**
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API unsigned long oapc_init(void* instanceData)
{
   struct instData *data;
#ifdef ENV_WINDOWS
   JOYCAPS          pjc;
#endif

   data=(struct instData*)instanceData;

printf("jwarrior init\n");
#ifdef ENV_LINUX
   data->m_handle=open(data->config.port, O_RDONLY);
   if(data->m_handle==-1)
   {
      oapc_exit(instanceData);
      return OAPC_ERROR_DEVICE;
   }
#endif
#ifdef ENV_WINDOWS
   if (joyGetDevCaps(data->config.m_joynum-1,&pjc,sizeof(pjc))!=JOYERR_NOERROR)
   {
      oapc_exit(instanceData);
      return OAPC_ERROR_DEVICE;
   }
#endif
   if (data->config.m_maxG==1) data->m_maxGVal=G_VAL*1.0;
   else if (data->config.m_maxG==2) data->m_maxGVal=G_VAL*1.5;
   else if (data->config.m_maxG==3) data->m_maxGVal=G_VAL*2.0;
   else if (data->config.m_maxG==4) data->m_maxGVal=G_VAL*3.0;
   else if (data->config.m_maxG==5) data->m_maxGVal=G_VAL*4.0;
   else if (data->config.m_maxG==6) data->m_maxGVal=G_VAL*8.0;
   else if (data->config.m_maxG==7) data->m_maxGVal=G_VAL*16.0;

printf("jwarrior create thread\n");
   if (!oapc_thread_create(recvLoop,data))
   {
      oapc_exit(instanceData);
      return OAPC_ERROR_NO_MEMORY;
   }

   return OAPC_OK;
}



/**
This function is called before the application unloads everything, it has to be used to deinitialize
everything and to release used resources.
*/
OAPC_EXT_API unsigned long oapc_exit(void* instanceData)
{
   struct instData *data;
#ifndef ENV_LINUX
   int              i;
#endif

   data=(struct instData*)instanceData;
#ifdef ENV_LINUX
   if (data->m_handle>=0) close(data->m_handle);
   data->m_handle=-1;
#else
   data->m_running=false;
   for (i=0; i<100; i++)
   {
      oapc_thread_sleep(10);
      if (data->m_running) break;
   }
#endif
   return OAPC_OK;
}



/**
When the capability flag OAPC_ACCEPTS_IO_CALLBACK is set, the main application no longer cyclically polls
the outputs of a Plug-In and the related parameter within the flow configuration dialogue is turned off.
Instead of this the main application hands over a function pointer to a callback and an ID. Whenever something
changes within the scope of this Plug-In that influences the output state of it, the Plug-In jumps into
that callback function to notify the main application about the new output state. The callback function
"oapc_io_callback" expects two parameters. The first one "outputs" expects the Or-concatenated flags of
the outputs that have changed and the second one "callbackID" expects the ID that is handed over here to
identify the Plug-In. For a typedef of the callback function oapc_io_callback() that is called by the Plug-In
please refer to oapc_libio.h.<BR><BR>
Here the main application hands over a pointer to the callback function and a unique callback ID. Both have
to be stored for later use
@param[in] oapc_io_callback the callback function that has to be called whenever something changes at the
           outputs of this Plug-In
@param[in] callbackID a unique ID that identifies this Plug-In and that has to be used when the function
           oapc_io_callback is called
*/
OAPC_EXT_API void oapc_set_io_callback(void* instanceData,lib_oapc_io_callback oapc_io_callback,unsigned long callbackID)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   m_oapc_io_callback=oapc_io_callback;
   data->m_callbackID=callbackID;
}



/**
This function is called by the main application when the library provides an numerical input (marked
using the digital input flags OAPC_NUM_IO...), a connection was edited to this input and a data
flow reaches the input.
@param[in] input specifies the input where the data are send to, here not the OAPC_NUM_IO...-flag is used
           but the plain, 0-based input number
@param[in] value specifies the numerical floating-point value that is set to that input
@return an error code OAPC_ERROR_... in case of an error or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_set_num_value(void* instanceData,unsigned long input,double value)
{
   struct instData           *data;

   data=(struct instData*)instanceData;

   if (input==3) data->m_spdX=value;
   else if (input==4) data->m_spdY=value;
   else if (input==5) data->m_spdZ=value;
   else return OAPC_ERROR_NO_SUCH_IO;
   return OAPC_OK;
}



/**
This function is called by the main application periodically in order to poll the state of the related
output. It has to return the data that are available for that input or - in case there are none available -
the error code OAPC_ERROR_NO_DATA_AVAILABLE to notify the main application, that there is nothing new.
@param[in] output specifies the output where the data are fetched from, here not the OAPC_NUM_IO...-flag is used
           but the plain, 0-based output number
@param[out] value if there are new data available, they are stored within that variable, if there are no new
           data, the returned data are ignored by the main application
@return an error code OAPC_ERROR_... in case of an error, OAPC_ERROR_NO_DATA_AVAILABLE in case no new data are
           available or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_get_num_value(void* instanceData,unsigned long output,double *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   if (output==0) *value=data->m_accX;
   else if (output==1) *value=data->m_accY;
   else if (output==2) *value=data->m_accZ;
   else if (output==3) *value=data->m_spdX;
   else if (output==4) *value=data->m_spdY;
   else if (output==5) *value=data->m_spdZ;
   else return OAPC_ERROR_NO_SUCH_IO;
   return OAPC_OK;
}




