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

#ifdef ENV_WINDOWS
#pragma warning (disable: 4530)
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef ENV_WINDOWSCE
 #include <sys/types.h>
#else
 #include <winsock2.h>
#endif

#ifndef ENV_WINDOWS
 #include <arpa/inet.h>
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
 #define closesocket close
#else
 #define snprintf _snprintf
#endif

#include "oapc_libio.h"
#include "liboapc.h"
#include "libsmartfactory.h"

#define MAX_XML_SIZE             15000
#define MAX_IP_SIZE              128
#define FLAG_ALLOW_REMOTE_CONFIG 0x0001


struct libio_config
{
   unsigned short version,length;
   char           m_prevMachineIP[MAX_IP_SIZE];
   unsigned short m_prevMachinePort,m_nextMachinePort;
   unsigned int   m_flags;
   char           m_machineID[MAX_IP_SIZE];
   unsigned int   reserved1,reserved2,reserved3,reserved4;
   unsigned char  reserved5,reserved6,reserved7,reserved8;
};


struct instData
{
   struct libio_config config;
   void               *m_hermesInst;
   int                 m_callbackID;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\
<oapc-config>\
 <flowimage>%s</flowimage>\
 <dialogue>\
  <general>\
   <param name=\"previp\" text=\"Previous machine IP\">\
    <type>string</type>\
    <default>%s</default>\
    <min>3</min>\
    <max>%d</max>\
   </param>\
   <param name=\"prevport\" text=\"Previous machine port\">\
    <type>integer</type>\
    <default>%d</default>\
    <min>1</min>\
    <max>65535</max>\
   </param>\
   <param name=\"nextport\" text=\"Next machine port\">\
    <type>integer</type>\
    <default>%d</default>\
    <min>1</min>\
    <max>65535</max>\
   </param>\
  <param text=\"Allow remote configuration changes\" name=\"remotecfg\">\
   <type>checkbox</type>\
   <default>%d</default>\
  </param>\
  <param name=\"mid\" text=\"Own machine identifier\">\
    <type>string</type>\
    <default>%s</default>\
    <min>3</min>\
    <max>%d</max>\
   </param>\
  </general>\
  <helppanel>\
   <in0>RDY - MachineReady to previous machine</in0>\
   <in1>!RDY - RevokeMachineReady to previous machine</in1>\
   <in2>STA - StartTransport to previous machine</in2>\
   <in3>STO - StopTransport to previous machine</in3>\
   <in4>AV - BoardAvailable to next machine</in4>\
   <in5>!AV - RevokeBoardAvailable to next machine</in5>\
   <in6>TF - TransportFinished to next machine</in6>\
   <out0>AV - BoardAvailable from previous machine</out0>\
   <out1>!AV - RevokeBoardAvailable from previous machine</out1>\
   <out2>TF - TransportFinished from previous machine</out2>\
   <out4>RDY - MachineReady from next machine</out4>\
   <out5>!RDY - RevokeMachineReady from next machine</out5>\
   <out6>STA - StartTransport from next machine</out6>\
   <out7>STO - StopTransport from next machine</out7>\
  </helppanel>\
 </dialogue>\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUBAQEjmTXEw8Qvq0MVVx+Fg4UjozfT7dcDDQUnpz15yYXv+fEDBwMXZyMjnTc7r00rqT+N0Zkbcyfj8+X3+/kHFwkZbSf+//4zq0cFAwUjpTkbdSk1rUcBAwEjmzUDCQMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABY89sOAAAACXBIWXMAAA7EAAAOxAGVKw4bAAABxUlEQVRYw82Y61bCMAyAQwV1IgNFnesU3/8t3Q2adF2Srq7H/OAkI9mXNPRCoc4mUFdVVe/aj12n6Qxj1DFOBpTpeWZ4pDA6lDLGR3WJuDRko0NpY3xUm4hLQzZ6lDLGR7WJoDREo0dpY3yUwWmIxoBSxvioiqQhGQNKG+OjosSYBUEjCtAjSEYBh4pkiSjgBhDAfY31RSgIvWGVqv5Nr3ixosQMINuq8pnK5vRG5aivCpADTJ0vQGVz8l5zjOgVKjlQPkZ9J6IEwaiP7WMaqp11gGYgU1Vjy22ZMoCoO4G5TlGtFPscvepR9u5lv36vmnEmEdhCVFcTyChrPx8yVVU8nfMMIAGtiirOOaZwY+8PZZ7V4vXwnmth+sm3BqpWdrwKXNV1UHSdG/ekEGq6MNkLFWu/qBQJVcF6xxhvwweeKaGCpyJUFeiPjyIqtFGEqvJHenr6Sa1KOAf/Va/mm7W0VzDRAxu+4vwtowCd9YDOK38ipFUVLCv7X7nxumR8NGsQFOOGLadH3cZQ1LwbtpwedRtDUIwbtpwedxtDesW4Yeumx93GEBTjhq2bHnkbQ36BjFtNSqyX3MYQFONmSInmisomvyJarOc+oG+IAAAAAElFTkSuQmCC";
static char                 libname[]="Hermes interface";
static char                 xmldescr[MAX_XML_SIZE+1];
static struct libio_config  save_config;
static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports



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
   return OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|
          OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6;
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
   return OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|
          OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7;
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
                                           data->config.m_prevMachineIP,MAX_IP_SIZE,data->config.m_prevMachinePort,
	                                       data->config.m_nextMachinePort,
	                                       data->config.m_flags & FLAG_ALLOW_REMOTE_CONFIG,
	                                       data->config.m_machineID,MAX_IP_SIZE);
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

   if (strcmp(name,"previp")==0)        strncpy(data->config.m_prevMachineIP,value,MAX_IP_SIZE);
   else if (strcmp(name,"prevport")==0) data->config.m_prevMachinePort=atoi(value);
   else if (strcmp(name,"nextport")==0) data->config.m_nextMachinePort=atoi(value);
   else if (strcmp(name,"mid")==0)      strncpy(data->config.m_machineID,value,MAX_IP_SIZE);
   else if (strcmp(name,"remotecfg")==0)
   {
      if (atoi(value)) data->config.m_flags|=FLAG_ALLOW_REMOTE_CONFIG;
      else data->config.m_flags&=~FLAG_ALLOW_REMOTE_CONFIG;
   }
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
   save_config.version=htons(1);
   save_config.length =htons(sizeof(struct libio_config));
   strncpy(save_config.m_prevMachineIP,data->config.m_prevMachineIP,MAX_IP_SIZE);
   save_config.m_prevMachinePort=htons(data->config.m_prevMachinePort);
   save_config.m_nextMachinePort=htons(data->config.m_nextMachinePort);
   strncpy(save_config.m_machineID,data->config.m_machineID,MAX_IP_SIZE);
   save_config.m_flags          =htonl(data->config.m_flags);

   save_config.reserved1      =htonl(data->config.reserved1);
   save_config.reserved2      =htonl(data->config.reserved2);
   save_config.reserved3      =htonl(data->config.reserved3);
   save_config.reserved4      =htonl(data->config.reserved4);
   save_config.reserved5      =data->config.reserved5;
   save_config.reserved6      =data->config.reserved6;
   save_config.reserved7      =data->config.reserved7;
   save_config.reserved8      =data->config.reserved8;

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
   data->config.version=ntohs(save_config.version);
   data->config.length =ntohs(save_config.length);
   strncpy(data->config.m_prevMachineIP,save_config.m_prevMachineIP,MAX_IP_SIZE);
   data->config.m_prevMachinePort=ntohs(save_config.m_prevMachinePort);
   data->config.m_nextMachinePort=ntohs(save_config.m_nextMachinePort);
   strncpy(data->config.m_machineID,save_config.m_machineID,MAX_IP_SIZE);
   data->config.m_flags          =ntohl(save_config.m_flags);

   data->config.reserved1=ntohl(save_config.reserved1);
   data->config.reserved2=ntohl(save_config.reserved2);
   data->config.reserved3=ntohl(save_config.reserved3);
   data->config.reserved4=ntohl(save_config.reserved4);
   data->config.reserved5=save_config.reserved5;
   data->config.reserved6=save_config.reserved6;
   data->config.reserved7=save_config.reserved7;
   data->config.reserved8=save_config.reserved8;
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

   strncpy(data->config.m_prevMachineIP,"192.168.1.200",MAX_IP_SIZE);
   data->config.m_prevMachinePort=50101;
   data->config.m_nextMachinePort=50101;
   strncpy(data->config.m_machineID,"unidentified",MAX_IP_SIZE);

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


static void next_state_callback(const enum hermes_next_state newState, const bool error, const void* custData)
{
   struct instData *data;

   data=(struct instData*)custData;
   assert(m_oapc_io_callback);
   if (!m_oapc_io_callback) return;

   //<out4>RDY - MachineReady from next machine</out4>
   //<out5>!RDY - RevokeMachineReady from next machine</out5>
   //<out6>STA - StartTransport from next machine</out6>
   //<out7>STO - StopTransport from next machine</out7>
   switch (newState)
   {
      case eNEXT_MACHINE_READY:
	     m_oapc_io_callback(OAPC_DIGI_IO4,data->m_callbackID);
         break;
      case eNEXT_REVOKE_MACHINE_READY:
	     m_oapc_io_callback(OAPC_DIGI_IO5,data->m_callbackID);
         break;
	  case eNEXT_START_TRANSPORT:
         m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
         break;
	  case eNEXT_STOP_TRANSPORT:
	     m_oapc_io_callback(OAPC_DIGI_IO7,data->m_callbackID);
         break;
	  default:
		 assert(0);
         break;
   }
}


static void prev_state_callback(const enum hermes_prev_state newState, const bool error, const void* custData)
{
   struct instData *data;

   data=(struct instData*)custData;
   assert(m_oapc_io_callback);
   if (!m_oapc_io_callback) return;

   switch (newState)
   {
	  case ePREV_BOARD_AVAILABLE:
         //<out0>AV - BoardAvailable from previous machine</out0>
	     m_oapc_io_callback(OAPC_DIGI_IO0,data->m_callbackID);
         break;
	  case ePREV_REVOKE_BOARD_AVAILABLE:
         //<out1>!AV - RevokeBoardAvailable previous next machine</out1>
	     m_oapc_io_callback(OAPC_DIGI_IO1,data->m_callbackID);
         break;
	  case ePREV_TRANSPORT_FINISHED:
		  //<out2>TF - TransportFinished from previous machine</out2>
	     m_oapc_io_callback(OAPC_DIGI_IO2,data->m_callbackID);
         break;
	  default:
		 assert(0);
         break;
   }
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
   data->m_hermesInst=sf_hermes_create_instance(data->config.m_machineID,eHERMES_1_0);
   if (!data->m_hermesInst) return OAPC_ERROR_NO_MEMORY;
   ret=sf_hermes_open_connections(data->m_hermesInst,
	                              data->config.m_prevMachinePort,data->config.m_prevMachineIP,
	                              data->config.m_nextMachinePort,
	                              data->config.m_flags & FLAG_ALLOW_REMOTE_CONFIG);
   if (ret!=OAPC_OK) return ret;

   ret=sf_hermes_set_next_state_callback(data->m_hermesInst,next_state_callback,instanceData);
   if (ret!=OAPC_OK) return ret;

   return sf_hermes_set_prev_state_callback(data->m_hermesInst,prev_state_callback,instanceData);
}



/**
This function is called before the application unloads everything, it has to be used to deinitialize
everything and to release used resources.
*/
OAPC_EXT_API unsigned long oapc_exit(void* instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   if (data->m_hermesInst) sf_hermes_delete_instance(data->m_hermesInst);
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
This function is called by the main application when the library provides an digital input (marked
using the digital input flags OAPC_DIGI_IO...), a connection was edited to this input and a data
flow reaches the input.
@param[in] input specifies the input where the data are send to, here not the OAPC_DIGI_IO...-flag is used
           but the plain, 0-based input number
@param[in] value specifies the value (0 or 1) that is set to that input
@return an error code OAPC_ERROR_... in case of an error or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long oapc_set_digi_value(void *instanceData,unsigned long input,unsigned char value)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   switch (input)
   {
      case 0:
		 //<in0>RDY - MachineReady to previous machine< / in0>
		 sf_hermes_set_prev_MachineReady(data->m_hermesInst);
		 break;
	  case 1:
		 //<in1>!RDY - RevokeMachineReady to previous machine< / in1>
		 sf_hermes_set_prev_RevokeMachineReady(data->m_hermesInst);
         break;
	  case 2:
		 //<in2>STA - StartTransport to previous machine< / in2>
		 sf_hermes_set_prev_StartTransport(data->m_hermesInst);
         break;
	  case 3:
		 //<in3>STO - StopTransport to previous machine< / in3>
		 sf_hermes_set_prev_StopTransport(data->m_hermesInst);
         break;
	  case 4:
		 //<in4>AV - BoardAvailable to next machine< / in4>
		 sf_hermes_set_next_BoardAvailable(data->m_hermesInst);
         break;
	  case 5:
		 //<in5>!AV - RevokeBoardAvailable to next machine< / in5>
		 sf_hermes_set_next_RevokeBoardAvailable(data->m_hermesInst);
         break;
	  case 6:
		 //<in6>TF - TransportFinished to next machine< / in6>
		 sf_hermes_set_next_TransportFinished(data->m_hermesInst);
         break;
	  default:
		 return OAPC_ERROR_INVALID_INPUT;
   }
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
   switch (output)
   {
      case 0:
      case 1:
      case 2:
      case 4:
      case 5:
      case 6:
      case 7:
		 // these possibilities all are called from the related callback and only when they are logically 1
		 *value=1;
         break;
	  default:
		 assert(0);
		 return OAPC_ERROR_INVALID_INPUT;
   }

   return OAPC_OK;
}

