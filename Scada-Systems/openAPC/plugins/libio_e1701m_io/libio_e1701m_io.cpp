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
#include <assert.h>

#ifndef ENV_WINDOWSCE
 #include <sys/types.h>
#endif

#ifndef ENV_WINDOWS
 #include <arpa/inet.h>
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
 #define closesocket close
#else
 #include <windows.h>
 #ifdef ENV_WINDOWSCE
  #include <winsock2.h>
 #endif
 #define snprintf _snprintf
 typedef int socklen_t;
#endif

#define MAX_IP_SIZE   100
#define MAX_XML_SIZE 3500

#include "oapc_libio.h"
#include "liboapc.h"
#include "libe1701m.h"

#define DATA_LENGTH      48 //defined by contoller-firmware internal limitations

struct libio_config
{
   unsigned short version,length;
   char           m_ip[MAX_IP_SIZE];
   char           m_passwd[DATA_LENGTH];
   unsigned short incoming;
   int            pollDelay;
};



struct instData
{
   struct libio_config config;
   unsigned int        DI,DO;
   unsigned char       m_boardNum;
   int                 m_callbackID;
   bool                m_running;
   void               *m_signal;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\
<oapc-config>\
 <flowimage>%s</flowimage>\
 <dialogue>\
  <general>\
  <param text=\"IP or serial interface\">\
    <name>ip</name>\
    <type>string</type>\
    <default>%s</default>\
    <min>3</min>\
    <max>%d</max>\
   </param>\
  <param text=\"Ethernet password\">\
   <name>passwd</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
  </param>\
  </general>\
 <stdpanel>\
  <name>Configuration</name>\
  <param>\
   <name>polldelay</name>\
   <text>Poll Delay</text>\
   <type>integer</type>\
   <default>%d</default>\
   <min>25</min>\
   <max>1000</max>\
   <unit>msec</unit>\
  </param>\
 </stdpanel>\
 <helppanel>\
  <in0>SET - digital output 1</in0>\
  <in1>SET - digital output 2</in1>\
  <in2>SET - digital output 3</in2>\
  <in3>...</in3>\
  <out0>GET - digital input 1</out0>\
  <out1>GET - digital input 2</out1>\
  <out2>GET - digital input 3</out2>\
  <out3>...</out3>\
 </helppanel>\
 </dialogue>\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="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";
static char                 libname[]="E1701M IO Interface";
static char                 xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports
static struct libio_config  save_config;



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
          OAPC_ACCEPTS_IO_CALLBACK|
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
   return OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|
          OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7;
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
   return OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|
          OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7;
   // avoid it to submit the same IO number for different data types, that would cause displaying problems for the flow symbol - and it would not make sense
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

   snprintf(xmldescr,MAX_XML_SIZE,xmltempl,flowImage,
                     data->config.m_ip,MAX_IP_SIZE,data->config.m_passwd,DATA_LENGTH-1,
                     data->config.pollDelay
                                       );
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

   if (strcmp(name,"ip")==0)             strncpy(data->config.m_ip,value,MAX_IP_SIZE);
   else if (strcmp(name,"passwd")==0)        strncpy(data->config.m_passwd,value,DATA_LENGTH);
   else if (strcmp(name,"polldelay")==0) data->config.pollDelay=(unsigned short)atoi(value);
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
   save_config.version    =htons(1);
   save_config.length     =htons(data->config.length);
   strncpy(save_config.m_ip,data->config.m_ip,MAX_IP_SIZE);
   strncpy(save_config.m_passwd,data->config.m_passwd,DATA_LENGTH);
   save_config.pollDelay=htonl(data->config.pollDelay);
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
   data->config.version  =ntohs(save_config.version);
   data->config.length   =ntohs(save_config.length);
   strncpy(data->config.m_ip,save_config.m_ip,MAX_IP_SIZE);
   strncpy(data->config.m_passwd,save_config.m_passwd,DATA_LENGTH);
   data->config.pollDelay=ntohl(save_config.pollDelay);
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
   if (!data) return NULL;
   memset(data,0,sizeof(struct instData));

   data->config.version=1;
   data->config.length=sizeof(struct libio_config);
   strcpy(data->config.m_ip,"192.168.2.254");
   data->config.pollDelay=250;
   data->m_boardNum=0xFF;
   data->m_callbackID=0;
   data->m_running=true;
   data->DO=0;
   data->DI=0;

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



void *IOLoop(void *arg)
{
   struct instData *data;
   unsigned int     lastDI=0xFFFFFFFF,currDI,lastDO=0xFFFFFFFF,maskDO,bitMask;
   int              i;

   data=(struct instData*)arg;
   oapc_thread_sleep(500);
   while (data->m_running)
   {
       E1701M_get_axis_state(data->m_boardNum, E1701M_AXIS_0);
      if (lastDO!=data->DO)
      {
         maskDO=lastDO^data->DO;
         if (E1701M_set_outputs(data->m_boardNum,0,data->DO,maskDO)==E1701M_OK)
          lastDO=data->DO;
      }
      currDI=E1701M_get_inputs(data->m_boardNum);
      if ((currDI!=0xFFFFFFFF) && (currDI!=lastDI))
      {
         bitMask=0x01;
         data->DI=currDI;
         for (i=0; i<8; i++)
         {
            if ((lastDI & bitMask)!=(currDI & bitMask)) switch (i)
            {
               case 0:
                  m_oapc_io_callback(OAPC_DIGI_IO0,data->m_callbackID);
                  break;
               case 1:
                  m_oapc_io_callback(OAPC_DIGI_IO1,data->m_callbackID);
                  break;
               case 2:
                  m_oapc_io_callback(OAPC_DIGI_IO2,data->m_callbackID);
                  break;
               case 3:
                  m_oapc_io_callback(OAPC_DIGI_IO3,data->m_callbackID);
                  break;
               case 4:
                  m_oapc_io_callback(OAPC_DIGI_IO4,data->m_callbackID);
                  break;
               case 5:
                  m_oapc_io_callback(OAPC_DIGI_IO5,data->m_callbackID);
                  break;
               case 6:
                  m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
                  break;
               case 7:
                  m_oapc_io_callback(OAPC_DIGI_IO7,data->m_callbackID);
                  break;
               default:
                  assert(0);
            }
            bitMask=bitMask<<1;
         }
         lastDI=currDI;
      }
      oapc_thread_signal_wait(data->m_signal,data->config.pollDelay);
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
   struct instData   *data;

   data=(struct instData*)instanceData;

   data->m_signal=oapc_thread_signal_create();
   if (!data->m_signal) return OAPC_ERROR_NO_MEMORY;

   data->m_boardNum=E1701M_set_connection(data->config.m_ip);
   E1701M_set_password(data->m_boardNum,data->config.m_passwd);

   if (E1701M_open_connection(data->m_boardNum)!=E1701M_OK) return OAPC_ERROR_CONNECTION;
   E1701M_stop(data->m_boardNum,E1701M_AXIS_0|E1701M_AXIS_1|E1701M_AXIS_2|E1701M_AXIS_3);


   if (!oapc_thread_create(IOLoop,data))
   {
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
   int              ctr=0;

   data=(struct instData*)instanceData;

   data->m_running=false;
   while ((!data->m_running) && (ctr<10)) // wait for thread to finish
   E1701M_stop(data->m_boardNum,E1701M_AXIS_0|E1701M_AXIS_1|E1701M_AXIS_2|E1701M_AXIS_3);
   {
      oapc_thread_sleep(250);
      ctr++;
   }
   E1701M_close(data->m_boardNum);

   if (data->m_signal) oapc_thread_signal_release(data->m_signal);
   data->m_signal=NULL;
   return OAPC_OK;
}



/**
This function is called by the main application when the library provides an digital input (marked
using the digital input flags OAPC_DIGI_IO...), a connection was edited to this input and a data
flow reaches the input.
@param[in] input specifies the input where the data are send to, here not the OAPC_DIGI_IO...-flag is used
           but the plain, 0-based input number
@param[in] value specifies the value (0 or 1) that is set to that input
@return an error code OAPC_ERROR_... in case of an error or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_set_digi_value(void* instanceData,unsigned long input,unsigned char value)
{
   struct instData *data;
   int              bitMask;

   data=(struct instData*)instanceData;
   bitMask=1<<input;
   if (value) data->DO|=bitMask;
   else data->DO&=~bitMask;
   oapc_thread_signal_send(data->m_signal);
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
   unsigned int     bitMask;

   data=(struct instData*)instanceData;
   bitMask=1<<output;
   if ((data->DI & bitMask)==bitMask) *value=1;
   else *value=0;
   return OAPC_OK;
}






