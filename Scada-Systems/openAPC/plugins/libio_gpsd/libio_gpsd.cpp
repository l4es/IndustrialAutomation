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
#include <math.h>
#include <ctype.h>
#include <assert.h>

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
 #include <termios.h>
 #include <unistd.h>
 #include <stdio_ext.h>
#endif

#ifdef ENV_QNX
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <arpa/inet.h>
 #include <termios.h>
 #include <unistd.h>
 #include <devctl.h>
 #include <sys/types.h>
 #include <sys/dcmd_chr.h>
#endif

#ifdef ENV_WINDOWS
 #include <windows.h>
#endif

#include <gps.h>

#ifndef WATCH_ENABLE
extern "C" // a small hack to make the functions of gpsdclient-lib linkable from cpp-file
{
 #include <gpsd.h>
}
#endif

#include "oapc_libio.h"
#include "liboapc.h"



struct libio_config
{
   unsigned short       version,length;
   int                  pollDelay;
};



struct instData
{
   struct libio_config   config;
#ifndef WATCH_ENABLE
//   struct fixsource_t    source;
#endif
   struct gps_data_t *gpsdata;
   bool               m_signalValid;
   double             m_lat,m_lon,m_height,m_speed,m_timestamp;
   int                m_callbackID;
   bool               m_running;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
<flowimage>%s</flowimage>\n\
<dialogue>\n\
 <general>\n\
  <param>\n\
   <name>polldelay</name>\n\
   <text>Poll Delay</text>\n\
   <type>integer</type>\n\
   <default>%d</default>\n\
   <min>1000</min>\n\
   <max>30000</max>\n\
   <unit>msec</unit>\n\
  </param>\n\
 </general>\n\
 <helppanel>\n\
  <out0>OK - position signal new/valid</out0>\n\
  <out1>LAT - Degree of latitude</out1>\n\
  <out2>LON - Degree of longitude</out2>\n\
  <out3>HGT - Height</out3>\n\
  <out4>SPD - Speed</out4>\n""\
  <out5>TIME - time (UTC)</out5>\n\
 </helppanel>\n\
</dialogue>\n\
</oapc-config>\n";

#define MAX_XML_SIZE 5500

/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUEAgSEgoQiNIFBSTkE/vxUYzlQXHwhKEmopKAnQadESGLFw8Xo6OgeL204R4drZ12MiqSmloolO5c8XrRXW2LV1NWsqrw2O05+c2t8eoEiJjIsP4BcQlRselQvOnC8trH8/vw8ZtRcisxUcKhPW0dEOzZ5ak4uTqxQZJ45UJmIh5RcerQUFiRtVUt0WmHR0Mw8RlCZnaxYZE6/u79MeuRXVYcsUsAeLFY/R3BSWHKmnJdoZm6trK98eY/c3N2ch3wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAyHMX5AAACh0lEQVR42u2YeXeiMBDAA17VDB6sWa4tTrfi0V2lmrWgYvz+36pRay22UH2I+495CowvyW+uMImEXLHhldoNdUPdUDfU/0FBpwPXQIEA6guQLW+UAGSbu4+7e0bUtv9bPdiXhd0MQk5vmop8otK+BcuKIu9XgiT248KSl2hBaXT31CqDr0BuqOUd4KTrjcJ5OB/XKAKnWVGbAQcHvl+F8EUzmLe7tV7bcYwVIu+xzFZ9sGf3LD8ADCf13qwxbfee65obPCGjkAtKCARXNZfr3n1ZVdVQswX6cIkM3D++O5AxbKrOqnHfmA4dVw2DioVM5LGuADrUcYbDxu9/j31dVbWwYlD/aCVfBiV8L9Kc4UP7z+OPQV160KjY0YjlgfIZB1XvvzwMlv1q3VE117aBAssBteYIml6v9l/6w3LDcjXD9mRStnJAcYGirju6XtV102z0aoZhgOA8l7RAMXFHAxmlWalUKtRGFeV4WV3qzc5QtGa/Bq7jhNNutxB6howT5IISyEcrTRrlauHYK9i2Jc5HxU4QJHGsQAqRJlsQPHv2X8VswikOJIniJy45RIsh5bUgqNiebXT8n9DyT0HFjkIkSYl4NxaBrIncWloW39RhiBLq1bYynDBhSltQ1mFydyFfvJtvSsE/abptsU9yNPrrMafm2he8k7KNOfLZkYBfhod8YgFjsviKxacwpaQFwa9nx2OFzlkp+96xUYlWXWJ3S06L1nGyJ8ftDFSSQOJ+Jmd48ysHxnVPFGJmnW7V9U4isT9LMgvpqM1WW9kPU4rFYhYhFUW3vLedLyVyWBbhmwxU8KCTVDGbkIqSZh10kipmE75J9o86ZRZSUfSjTpmF5FfNldorB9k8LH4+CgYAAAAASUVORK5CYII=";
static char                 libname[]="GPS (GPSd)";
static char                 xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports
static struct libio_config  save_config;



#ifdef TEST_MODE
static void test_io_callback(unsigned long outputs,unsigned long callbackID)
{
//   printf("Callback: %ld - %ld\n",outputs,callbackID);
}
#endif



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
   return OAPC_HAS_OUTPUTS|
          OAPC_HAS_XML_CONFIGURATION|
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|
          OAPC_ACCEPTS_IO_CALLBACK|
          OAPC_FLOWCAT_IO;
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
   return OAPC_DIGI_IO0|
          OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5;
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
                             data->config.pollDelay
                             );
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

   if (strcmp(name,"polldelay")==0)   data->config.pollDelay=(unsigned short)atoi(value);
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
   save_config.pollDelay=htons(data->config.pollDelay);
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
   data->config.pollDelay=ntohs(save_config.pollDelay);
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
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API void* oapc_create_instance2(unsigned long flags)
{
   struct instData *data;

   flags=flags; // removing "unused" warning
   data=(struct instData*)malloc(sizeof(struct instData));
   if (!data) return NULL;
   memset(data,0,sizeof(struct instData));

   data->config.version=1;
   data->config.length=sizeof(struct libio_config);
   
   data->config.pollDelay=1000;
   data->m_callbackID=0;
#ifndef WATCH_ENABLE
//   gpsd_source_spec(NULL,&data->source);
#endif
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



void* GPSdLoop( void*  arg )
{
   #define MAX_BUFLEN 100

   struct instData *data;
   bool             lastSignalValid=true;
   double           lastLat=-1000,lastLon=-1000,lastHeight=-10000,lastSpeed=-1,lastTimestamp=0;
   int              delayLeft,lastClock=(int)(clock()/(CLOCKS_PER_SEC/1000.0)),currClock;

   data=(struct instData*)arg;

#ifdef WATCH_ENABLE // new gpds style
   gps_stream(data->gpsdata, WATCH_ENABLE, NULL);
#else
   gps_query(data->gpsdata,"w+x\n");
#endif
   while (data->m_running)
   {
#ifdef WATCH_ENABLE // new gpds style
      if (gps_waiting(data->gpsdata,1000))
#else
      if (gps_poll(data->gpsdata)==0)
#endif
      {
         if (data->gpsdata->fix.mode >= MODE_2D && isnan(data->gpsdata->fix.latitude) == 0) data->m_lat=data->gpsdata->fix.latitude;
         if (data->gpsdata->fix.mode >= MODE_2D && isnan(data->gpsdata->fix.longitude) == 0) data->m_lon=data->gpsdata->fix.longitude;
         if (data->gpsdata->fix.mode == MODE_3D && isnan(data->gpsdata->fix.altitude) == 0) data->m_height=data->gpsdata->fix.altitude;
         if (data->gpsdata->fix.mode >= MODE_2D && isnan(data->gpsdata->fix.speed) == 0) data->m_speed=data->gpsdata->fix.speed;
         if (isnan(data->gpsdata->fix.time) == 0) data->m_timestamp=data->gpsdata->fix.time;
         if (data->gpsdata->online!=0) 
         {
            data->m_signalValid=true;
            if (data->m_lat!=lastLat)
            {
               m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
               lastLat=data->m_lat;
            }
            if (data->m_lon!=lastLon)
            {
               m_oapc_io_callback(OAPC_NUM_IO2,data->m_callbackID);
               lastLon=data->m_lon;
            }
            if (data->m_height!=lastHeight)
            {
               m_oapc_io_callback(OAPC_NUM_IO3,data->m_callbackID);
               lastHeight=data->m_height;
            }
            if (data->m_speed!=lastSpeed)
            {
               m_oapc_io_callback(OAPC_NUM_IO4,data->m_callbackID);
               lastSpeed=data->m_speed;
            }
            if (data->m_timestamp!=lastTimestamp)
            {
               m_oapc_io_callback(OAPC_NUM_IO5,data->m_callbackID);
               lastTimestamp=data->m_timestamp;
            }
         }
         else data->m_signalValid=false;
      }
      else data->m_signalValid=false;
      if ((data->m_signalValid!=lastSignalValid) || (data->m_signalValid==true)) // send the valid-signal whenever new data are collected or once when it is not valid 
      {
         m_oapc_io_callback(OAPC_DIGI_IO0,data->m_callbackID);
         lastSignalValid=data->m_signalValid;
      }
      currClock=(int)(clock()/(CLOCKS_PER_SEC/1000.0));
      delayLeft=data->config.pollDelay-(currClock-lastClock);
      if (delayLeft>0) oapc_thread_sleep(delayLeft);
      lastClock=(int)(clock()/(CLOCKS_PER_SEC/1000.0));
   }
   data->m_running=true;
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
   int              ret;

   data=(struct instData*)instanceData;
/*#ifndef WATCH_ENABLE
   data->gpsdata=gps_open(data->source.server,data->source.port);
#else*/
   ret=gps_open(NULL,NULL,data->gpsdata);
//#endif
   if (ret==-1) 
   {
   	  printf("Connecting to GPSd failed\n");
   	  oapc_exit(instanceData);
   	  return OAPC_ERROR_CONNECTION;
   }
   
   data->m_running=true;
   if (!oapc_thread_create(GPSdLoop,data))
   {
      printf("create thread failed\n");
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
	int              ctr=0;
   struct instData *data;

   data=(struct instData*)instanceData;

   if (data->m_running)
   {
      data->m_running=false;
      while ((!data->m_running) && (ctr<10)) // wait for thread to finish
      {
         oapc_thread_sleep(500);
         ctr++;
      }
   }
   if (data->gpsdata) gps_close(data->gpsdata);
   return OAPC_OK;
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

   if (output!=0) return OAPC_ERROR_NO_SUCH_IO;
   data=(struct instData*)instanceData;

   *value=data->m_signalValid;
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
OAPC_EXT_API unsigned long  oapc_get_num_value(void* instanceData,unsigned long output,double* value)
{
   struct instData *data;

   
   data=(struct instData*)instanceData;
   
   if (output==1) *value=data->m_lat;
   else if (output==2) *value=data->m_lon;
   else if (output==3) *value=data->m_height;
   else if (output==4) *value=data->m_speed;
   else if (output==5) *value=data->m_timestamp;
   else return OAPC_ERROR_NO_SUCH_IO;      
   return OAPC_OK;
}


#ifdef TEST_MODE
int main(int argc, char* argv[])
{
   struct instData *data;
   int              i,j;
   double           lat,lon,height,speed,time;
   unsigned char    ok;

   data=(struct instData*)oapc_create_instance2(0);
   if (data)
   {
      oapc_set_io_callback(data,test_io_callback,42);
      if (oapc_init(data)==OAPC_OK)
      {
      	for (i=0; i<10; i++)
      	{
      		oapc_get_digi_value(data,0,&ok);
   			oapc_get_num_value(data,1,&lat);
   			oapc_get_num_value(data,2,&lon);
   			oapc_get_num_value(data,3,&height);
   			oapc_get_num_value(data,4,&speed);
   			oapc_get_num_value(data,5,&time);
   			if (ok) printf("Latitude: %f Longitude: %f Height: %f Speed: %f Time: %d\n",lat,lon,height,speed,(int)time);
   			else printf("No position!\n");
      		oapc_thread_sleep(2000);
      	}
      }
      else printf("init failed\n");
      oapc_delete_instance(data);
   }
   return 0;
}
#endif

