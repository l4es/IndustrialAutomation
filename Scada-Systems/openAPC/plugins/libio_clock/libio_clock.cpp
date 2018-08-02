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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef ENV_WINDOWSCE
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include "time_ce.h"
#endif

#ifdef ENV_WINDOWS
#ifdef ENV_WINDOWSCE
#include "Winsock2.h"
#endif
#endif

#ifndef ENV_WINDOWS
#include <sys/io.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#else
#include <winsock2.h>
#endif

#ifdef ENV_LINUX
#include <termios.h>
#include <unistd.h>
#endif

#include "oapc_libio.h"

#define MAX_XML_SIZE       3500
#define MAX_FMTSTRING_SIZE  100

struct libio_config
{
   unsigned short version,length;
   unsigned char  dout[2],nout[5];
   char           cout7[MAX_FMTSTRING_SIZE+1];
   unsigned short reserved;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<oapc-config>\
<flowimage>%s</flowimage>\
<dialogue>\
 <general>\
  <param>\
   <name>dout0</name>\
   <text>OUT0 Clock</text>\
   <type>option</type>\
   <value>Every Second</value>\
   <value>Every Minute</value>\
   <value>Every Hour</value>\
   <value>Every Day</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>dout1</name>\
   <text>OUT1 Clock</text>\
   <type>option</type>\
   <value>Every Second</value>\
   <value>Every Minute</value>\
   <value>Every Hour</value>\
   <value>Every Day</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>nout2</name>\
   <text>OUT2 Value</text>\
   <type>option</type>\
   <value>Second</value>\
   <value>Minute</value>\
   <value>Hour</value>\
   <value>Day</value>\
   <value>Month</value>\
   <value>Year</value>\
   <value>Unixtime</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>nout3</name>\
   <text>OUT3 Value</text>\
   <type>option</type>\
   <value>Second</value>\
   <value>Minute</value>\
   <value>Hour</value>\
   <value>Day</value>\
   <value>Month</value>\
   <value>Year</value>\
   <value>Unixtime</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>nout4</name>\
   <text>OUT4 Value</text>\
   <type>option</type>\
   <value>Second</value>\
   <value>Minute</value>\
   <value>Hour</value>\
   <value>Day</value>\
   <value>Month</value>\
   <value>Year</value>\
   <value>Unixtime</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>nout5</name>\
   <text>OUT5 Value</text>\
   <type>option</type>\
   <value>Second</value>\
   <value>Minute</value>\
   <value>Hour</value>\
   <value>Day</value>\
   <value>Month</value>\
   <value>Year</value>\
   <value>Unixtime</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>nout6</name>\
   <text>OUT6 Value</text>\
   <type>option</type>\
   <value>Second</value>\
   <value>Minute</value>\
   <value>Hour</value>\
   <value>Day</value>\
   <value>Month</value>\
   <value>Year</value>\
   <value>Unixtime</value>\
   <default>%d</default>\
  </param>\
  <param>\n" // an ugly hack to avoid errors with MS Visual C++
"  <name>cout7</name>\
   <text>OUT7 Format</text>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
  </param>\
 </general>\
 <helppanel>\
  <in2>OFFS - offset to current time</in2>\
  <out0>CLK1 - configurable output clock</out0>\
  <out1>CLK2 - configurable output clock</out1>\
  <out2>VAL1 - configurable output time value</out2>\
  <out3>...</out3>\
  <out7>FMT - formatted date/time string</out7>\
 </helppanel>\
</dialogue>\
</oapc-config>\n";



struct instData
{
   struct libio_config   config;
#ifndef ENV_WINDOWSCE
   time_t                timeOffset,clockTime[2],prev_clockTime[2];
#else
   time_t_ce             timeOffset,clockTime[2],prev_clockTime[2];
#endif
   char                  digi[2],prevDigi[2];
   int                   num[5],prevNum[5];
   char                  txt[MAX_FMTSTRING_SIZE*2+1],prevTxt[MAX_FMTSTRING_SIZE*2+1];
};



/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyBAMAAABG0lM+AAAAMFBMVEUEAgSFh4UE/vz8AvxJSUnFx8Wpqalqamr8/vw1NjUYGBiZmZlYWFjZ2tm5ubl4eHizsfUAAAAACXBIWXMAAA7EAAAOxAGVKw4bAAACNklEQVR42q2WTUsbQRjHdy2BOQYlECiC5hNY/ARCUfDieql7CZ7ULRjFQ2+tTA7bPKFseim5KOTiR1CJiIhgvPgCHkyn1JlToQoGT4WClDY7s9nsxh2bGZzTPsP89nnbZ/ZvGFoLQWyZudGenYSFeykrN2KrU3gwDeqUO5DSoMBEOpRl61AYdCjQoTDo5IUx1okQ4WfN6+frqYI8L8nam9jf/etI80qGyjOty4OrNSKhDCOR2lzJNwuMvVPKq3w6u9M8YOSOJFPJr9nLsDz1PPj4W1KNxMQ2Xw5TSgEWTxR8VWr3byglcPlrSSGvUpXtNCgtNjLXpH+qfNGi386bVXBLNwpUbZdeTW8RYMsqVHWdeLeTNnxXo9qloP7TtaNQjQ1mg18H8tXun6rU5nl7vQ9zPTWMTXFgoM7Wl1UH3Dx4mbVOv4IPFvEjcSOcgLd/6HA70Bv3POyyeCESbqNG9/MqbRDnU72SnXOUZvl4hRTr3sJROCkY8aAMjKLTjOKj/bnWYgW2Xe/OV/RckJf4Q0VH9MfM4cP7RvcGiB2Ur9mzs4z0jhKvCPYl1wEOahWGGNRaNEp29TyOsH9fOn+HoJdIRJjg8f+UrF+PIzSCToGKL728hFRpV9LgTvowfMqEom9mIR1qGzyYkxlD42lOWYDbY+0iSIXaxh3IyYwX4ylRwyyXM2YRdbWNOSo1XiFOuYjLGcv32NE21ojUGLNFDYWcwemItnna4JSQM24qom2eNrCeqPwHRV3Oj2dQBvoAAAAASUVORK5CYII=";
static char                libname[]="Clock";
static char                xmldescr[MAX_XML_SIZE+1];
static struct libio_config save_config;

/**
This function has to return the name that is used to display this library within the main application.
The name returned here has to be short and descriptive. It is displayed within the popup menu of the flow
editor, therefore it should not be too long!
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
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|
          OAPC_FLOWCAT_DATA;
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
   return OAPC_NUM_IO2;
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
   return OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6|OAPC_CHAR_IO7;
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
   sprintf(xmldescr,xmltempl,flowImage,data->config.dout[0],data->config.dout[1],
                             data->config.nout[0],data->config.nout[1],data->config.nout[2],data->config.nout[3],data->config.nout[4],
                             data->config.cout7,MAX_FMTSTRING_SIZE);
#ifdef _DEBUG
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

   if (strcmp(name,"dout0")==0) data->config.dout[0]=(unsigned char)atoi(value);
   else if (strcmp(name,"dout1")==0) data->config.dout[1]=(unsigned char)atoi(value);
   else if (strcmp(name,"nout2")==0) data->config.nout[0]=(unsigned char)atoi(value);
   else if (strcmp(name,"nout3")==0) data->config.nout[1]=(unsigned char)atoi(value);
   else if (strcmp(name,"nout4")==0) data->config.nout[2]=(unsigned char)atoi(value);
   else if (strcmp(name,"nout5")==0) data->config.nout[3]=(unsigned char)atoi(value);
   else if (strcmp(name,"nout6")==0) data->config.nout[4]=(unsigned char)atoi(value);
   else if (strcmp(name,"cout7")==0) strncpy(data->config.cout7,value,MAX_FMTSTRING_SIZE);
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
   save_config.dout[0]    =data->config.dout[0];
   save_config.dout[1]    =data->config.dout[1];
   save_config.nout[0]    =data->config.nout[0];
   save_config.nout[1]    =data->config.nout[1];
   save_config.nout[2]    =data->config.nout[2];
   save_config.nout[3]    =data->config.nout[3];
   save_config.nout[4]    =data->config.nout[4];
   strncpy(save_config.cout7,data->config.cout7,MAX_FMTSTRING_SIZE);
   save_config.reserved   =htons(data->config.reserved);

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
@param[in] loadedData the configuration data that are loaded for this library
*/
OAPC_EXT_API void oapc_set_loaded_data(void* instanceData,unsigned long length,char *loadedData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (length>sizeof(struct libio_config)) length=sizeof(struct libio_config);
   memcpy(&save_config,loadedData,length);
   data->config.version    =ntohs(save_config.version);
   data->config.length     =ntohs(save_config.length);
   data->config.dout[0]    =save_config.dout[0];
   data->config.dout[1]    =save_config.dout[1];
   data->config.nout[0]    =save_config.nout[0];
   data->config.nout[1]    =save_config.nout[1];
   data->config.nout[2]    =save_config.nout[2];
   data->config.nout[3]    =save_config.nout[3];
   data->config.nout[4]    =save_config.nout[4];
   strncpy(data->config.cout7,save_config.cout7,MAX_FMTSTRING_SIZE);
   data->config.reserved   =ntohs(save_config.reserved);
}



/**
This function handles all internal data initialisation and has to allocate a memory area where all
data are stored into that are required to operate this Plug-In. This memory area can be used by the
Plug-In freely, it is handed over with every function call so that the Plug-In cann access its
values. The memory area itself is released by the main application, therefore it has to be allocated
using malloc().
@return pointer where the allocated and pre-initialized memory area starts
*/
OAPC_EXT_API void* oapc_create_instance2(unsigned long flags)
{
   flags=flags; // removing "unused" warning

   struct instData *data;

   data=(struct instData*)malloc(sizeof(struct instData));
   data->config.version=1;
   data->config.length=sizeof(struct libio_config);
   data->config.dout[0]=1; data->config.dout[1]=2;
   data->config.nout[0]=2; data->config.nout[1]=3; data->config.nout[2]=4; data->config.nout[3]=5; data->config.nout[4]=6;   
   strncpy(data->config.cout7,"%A, %d. %B %Y, %H:%M:%S",MAX_FMTSTRING_SIZE);
   data->timeOffset=0;
   data->clockTime[0]=0;                  data->clockTime[1]=0;
#ifndef ENV_WINDOWSCE
   data->prev_clockTime[0]=time(NULL);    data->prev_clockTime[1]=time(NULL);
#else
   data->prev_clockTime[0]=time_ce(NULL); data->prev_clockTime[1]=time_ce(NULL);
#endif
   data->digi[0]=0;                       data->digi[1]=0;
   data->prevDigi[0]=0;                   data->prevDigi[1]=0;
   data->num[0]=0;                        data->num[1]=0;                     data->num[2]=0;         data->num[3]=0;         data->num[4]=0;
   data->prevNum[0]=0;                    data->prevNum[1]=0;                 data->prevNum[2]=0;     data->prevNum[3]=0;     data->prevNum[4]=0;
   data->txt[0]=0;                        data->prevTxt[0]=0;
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



/**
This is an internally used function that fetches the relevant part of the current time and returns it
@param[in] tSec the time where a part has to be fetched from
@param[in] unit the unit of the whole-numbered part that has to be fetched, 1 for seconds, 2 for
           minutes, 3 for hours, 4 for days, 5 for month and 6 for year
@return    return the whole-numbered part of the time according to the desired unit
*/
#ifdef ENV_WINDOWSCE
static time_t getTimeUnit(time_t_ce tSec,unsigned char unit)
#else
static time_t getTimeUnit(time_t tSec,unsigned char unit)
#endif
{
   struct tm *tm_time;

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
   tm_time=localtime_ce(&tSec);
 #else
   tm_time=localtime(&tSec);
 #endif
#else
   tm_time=gmtime(&tSec);
#endif
   if (unit==1) return tm_time->tm_sec;
   else if (unit==2) return tm_time->tm_min;
   else if (unit==3) return tm_time->tm_hour;
   else if (unit==4) return tm_time->tm_mday;
   else if (unit==5) return tm_time->tm_mon+1;
   else if (unit==6) return tm_time->tm_year+1900;
#ifndef ENV_WINDOWSCE
   else if (unit==7) return time(NULL);
#else
   else if (unit==7) return time_ce(NULL);
#endif
   else return 0; // should never happen
}



/**
This function is called by the main application periodically in order to poll the state of the related
output. It has to return the data that are available for that input or - in case there are none available -
the error code OAPC_ERROR_NO_DATA_AVAILABLE to notify the main application, that there is nothing new.
@param[in] output specifies the output where the data are fetched from, here not the OAPC_DIGI_IO...-flag is used
           but the plain, 0-based output number
@param[out] value if there are new data available, they are stored within that variable, if there are no new
           data, the returned data are ignored by the main application
@return an error code OAPC_ERROR_... in case of an error, OAPC_ERROR_NO_DATA_AVAILABLE in case no new data are
           available or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_get_digi_value(void* instanceData,unsigned long output,unsigned char *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if ((output!=0) && (output!=1)) return OAPC_ERROR_NO_SUCH_IO;
#ifdef ENV_WINDOWSCE
   data->clockTime[output]=time_ce(NULL)+data->timeOffset;
#else
   data->clockTime[output]=time(NULL)+data->timeOffset;
#endif
   if (getTimeUnit(data->clockTime[output],data->config.dout[output])!=getTimeUnit(data->prev_clockTime[output],data->config.dout[output]))
   {
      data->digi[output]=1;
      data->prev_clockTime[output]=data->clockTime[output];
   }
   else data->digi[output]=0;
   if (data->prevDigi[output]!=data->digi[output])
   {
      data->prevDigi[output]=data->digi[output];
      if (data->digi[output])
      {
         *value=data->digi[output];
         return OAPC_OK;
      }
   }
   return OAPC_ERROR_NO_DATA_AVAILABLE; // there are no new data available
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
   struct instData *data;

   data=(struct instData*)instanceData;

   if (input!=2) return OAPC_ERROR_NO_SUCH_IO;
   if (data->timeOffset!=(time_t)value)
   {
      data->timeOffset=(time_t)value;
#ifdef ENV_WINDOWSCE
      data->prev_clockTime[0]=time_ce(NULL)+data->timeOffset;
      data->prev_clockTime[1]=time_ce(NULL)+data->timeOffset;
#else
      data->prev_clockTime[0]=time(NULL)+data->timeOffset;
      data->prev_clockTime[1]=time(NULL)+data->timeOffset;
#endif
   }
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

   if ((output<2) || (output>6)) return OAPC_ERROR_NO_SUCH_IO;
#ifdef ENV_WINDOWSCE
   data->num[output-2]=getTimeUnit(time_ce(NULL)+data->timeOffset,data->config.nout[output-2]);
#else
   data->num[output-2]=(int)getTimeUnit(time(NULL)+data->timeOffset,data->config.nout[output-2]);
#endif
   if (data->num[output-2]!=data->prevNum[output-2])
   {
      data->prevNum[output-2]=data->num[output-2];
      *value=(float)data->num[output-2];
      return OAPC_OK;
   }
   return OAPC_ERROR_NO_DATA_AVAILABLE; // there are no new data available
}



/**
This function is called by the main application periodically in order to poll the state of the related
output. It has to return the data that are available for that input or - in case there are none available -
the error code OAPC_ERROR_NO_DATA_AVAILABLE to notify the main application, that there is nothing new.
@param[in] output specifies the output where the data are fetched from, here not the OAPC_CHAR_IO...-flag is used
           but the plain, 0-based output number
@param[out] value if there are new data available, they are stored within that variable, if there are no new
           data, the returned data are ignored by the main application
@return an error code OAPC_ERROR_... in case of an error, OAPC_ERROR_NO_DATA_AVAILABLE in case no new data are
           available or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_get_char_value(void* instanceData,unsigned long output,unsigned long length,char *value)
{
   struct tm       *tm_time;
#ifdef ENV_WINDOWSCE
   time_t_ce        sec_time;
#else
   time_t           sec_time;
#endif
   struct instData *data;

   data=(struct instData*)instanceData;

   if (output!=7) return OAPC_ERROR_NO_SUCH_IO;

#ifndef ENV_WINDOWSCE
   sec_time=time(NULL)+data->timeOffset;
 #ifndef ENV_WINDOWS
   tm_time=gmtime(&sec_time);
   strftime(data->txt,MAX_FMTSTRING_SIZE*2,data->config.cout7,tm_time);
 #else
   tm_time=localtime(&sec_time);
   strftime(data->txt,MAX_FMTSTRING_SIZE*2,data->config.cout7,tm_time);
 #endif
#else
   sec_time=time_ce(NULL)+data->timeOffset;
   tm_time=localtime_ce(&sec_time);
   strftime_ce(data->txt,MAX_FMTSTRING_SIZE*2,data->config.cout7,tm_time);
#endif
   if (strncmp(data->txt,data->prevTxt,MAX_FMTSTRING_SIZE*2))
   {
      strncpy(data->prevTxt,data->txt,MAX_FMTSTRING_SIZE*2);
      strncpy(value,data->txt,length);
      return OAPC_OK;
   }
   return OAPC_ERROR_NO_DATA_AVAILABLE;
}


