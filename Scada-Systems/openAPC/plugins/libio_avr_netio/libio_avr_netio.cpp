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
#include <stdlib.h>
#include <string.h>
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
 typedef int socklen_t;
#endif

#define MAX_IP_SIZE   100
#define MAX_XML_SIZE 3500

#include "oapc_libio.h"
#include "liboapc.h"


struct libio_config
{
   unsigned short version,length;
   char           ip[MAX_IP_SIZE];
   unsigned short port,incoming;
   int            pollDelay;
};



struct instData
{
   struct libio_config   config;
   unsigned char         DO[8];                     // Digital outputs
   unsigned char         DI[4];                     // Digital input data
   int                   AI[4];                     // Analogue input data
   int                   m_sock,m_callbackID;
   bool                  m_running;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
 <flowimage>%s</flowimage>\n\
 <dialogue>\n\
  <general>\n\
   <param>\n\
    <name>ip</name>\n\
    <text>IP</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>3</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>port</name>\n\
    <text>Port</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>1</min>\n\
    <max>65535</max>\n\
   </param>\n\
  </general>\n\
 <stdpanel>\n\
  <name>Configuration</name>\n\
  <param>\n\
   <name>polldelay</name>\n\
   <text>Poll Delay</text>\n\
   <type>integer</type>\n\
   <default>%d</default>\n\
   <min>25</min>\n\
   <max>1000</max>\n\
   <unit>msec</unit>\n\
  </param>\n\
 </stdpanel>\n\
 <helppanel>\n\
  <in0>SET - digital output 1</in0>\n\
  <in1>SET - digital output 2</in1>\n\
  <in2>SET - digital output 3</in2>\n\
  <in3>...</in3>\n\
  <out0>GET - digital input 1</out0>\n\
  <out1>GET - digital input 2</out1>\n\
  <out2>GET - digital input 3</out2>\n\
  <out3>GET - digital input 4</out3>\n\
  <out4>GET - analogue input 1</out4>\n""\
  <out5>GET - analogue input 2</out5>\n\
  <out6>GET - analogue input 3</out6>\n\
  <out7>GET - analogue input 4</out7>\n\
 </helppanel>\n\
 </dialogue>\n\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="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";
static char                 libname[]="AVR Net-IO Ethernet Interface";
static char                 xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports
static struct libio_config  save_config;



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
          OAPC_NUM_IO4| OAPC_NUM_IO5| OAPC_NUM_IO6| OAPC_NUM_IO7;
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

   sprintf(xmldescr,xmltempl,flowImage,data->config.ip,MAX_IP_SIZE,
                                       data->config.port,
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

   if (strcmp(name,"ip")==0)             strncpy(data->config.ip,value,MAX_IP_SIZE);
   else if (strcmp(name,"port")==0)      data->config.port=(unsigned short)atoi(value);
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
   save_config.version    =htons(data->config.version);
   save_config.length     =htons(data->config.length);
   strncpy(save_config.ip,data->config.ip,MAX_IP_SIZE);
   save_config.port    =htons(data->config.port);
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
   strncpy(data->config.ip,save_config.ip,MAX_IP_SIZE);
   data->config.port     =ntohs(save_config.port);
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
   int              i;

   data=(struct instData*)malloc(sizeof(struct instData));
   if (!data) return NULL;

   memset(data,0,sizeof(struct instData));
   data->config.version=1;
   data->config.length=sizeof(struct libio_config);
   strcpy(data->config.ip,"192.168.0.90");
   data->config.port=50290;
   data->config.pollDelay=250;
   data->m_sock=-1;
   data->m_callbackID=0;
   data->m_running=true;
   for (i=0; i<8; i++) data->DO[i]=2;
   for (i=0; i<4; i++)
   {
   	data->DI[i]=2;
   	data->AI[i]=1000000000;
   }
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



void *AVRLoop(void *arg)
{
	#define MAX_BUFLEN 100
	
	struct instData *data;
   int              i;
   char             lastDO[8]={2,2,2,2,2,2,2,2},lastDI[4]={2,2,2,2};
   int              lastAI[4]={1000000000,1000000000,1000000000,1000000000};
   char             sendBuf[MAX_BUFLEN+1];
	
	data=(struct instData*)arg;

	while (data->m_running)
	{
		for (i=0; i<8; i++) if (lastDO[i]!=data->DO[i])
		{
			sprintf(sendBuf,"SETPORT %d.%d\r\n",i+1,data->DO[i]);
			oapc_tcp_send(data->m_sock,sendBuf,(int)strlen(sendBuf),250);
			if ((oapc_tcp_recv(data->m_sock,sendBuf,MAX_BUFLEN,"\n",250)>0) &&
				 (strstr(sendBuf,"\n")))
		    lastDO[i]=data->DO[i];
		}
      for (i=0; i<4; i++)
      {
  			sprintf(sendBuf,"GETPORT %d\r\n",i+1);
 			oapc_tcp_send(data->m_sock,sendBuf,(int)strlen(sendBuf),250);
  			if ((oapc_tcp_recv(data->m_sock,sendBuf,MAX_BUFLEN,"\n",250)>0) &&
       		 (strstr(sendBuf,"\n")))
         {
         	data->DI[i]=(unsigned char)atoi(sendBuf);
	         if (lastDI[i]!=data->DI[i])
	         {
	         	if (i==0) m_oapc_io_callback(OAPC_DIGI_IO0,data->m_callbackID);
	         	else if (i==1) m_oapc_io_callback(OAPC_DIGI_IO1,data->m_callbackID);
	         	else if (i==2) m_oapc_io_callback(OAPC_DIGI_IO2,data->m_callbackID);
	         	else if (i==3) m_oapc_io_callback(OAPC_DIGI_IO3,data->m_callbackID);
	         }
        	}
  			sprintf(sendBuf,"GETADC %d\r\n",i+1);
 			oapc_tcp_send(data->m_sock,sendBuf,(int)strlen(sendBuf),250);
  			if ((oapc_tcp_recv(data->m_sock,sendBuf,MAX_BUFLEN,"\n",250)>0) &&
       		 (strstr(sendBuf,"\n")))
         {
         	data->AI[i]=atoi(sendBuf);
	         if (lastAI[i]!=data->AI[i])
	         {
	         	if (i==0) m_oapc_io_callback(OAPC_NUM_IO4,data->m_callbackID);
	         	else if (i==1) m_oapc_io_callback(OAPC_NUM_IO5,data->m_callbackID);
	         	else if (i==2) m_oapc_io_callback(OAPC_NUM_IO6,data->m_callbackID);
	         	else if (i==3) m_oapc_io_callback(OAPC_NUM_IO7,data->m_callbackID);
               lastAI[i]=data->AI[i];
	         }
         }
      }
      oapc_thread_sleep(data->config.pollDelay);
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

#ifdef ENV_WINDOWS
   WSADATA wsa;

   WSAStartup(MAKEWORD(2,0),&wsa);
#endif
   data->m_sock=oapc_tcp_connect_to(data->config.ip,data->config.port);
   if (data->m_sock<=0) return OAPC_ERROR_CONNECTION;

   // set socket to non-blocking in order to avoid deadlocks when sending fails
   oapc_tcp_set_blocking(data->m_sock,false);

   if (!oapc_thread_create(AVRLoop,data))
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
   int              ctr=0;

   data=(struct instData*)instanceData;

   data->m_running=false;
   while ((!data->m_running) && (ctr<10)) // wait for thread to finish
   {
      oapc_thread_sleep(250);
      ctr++;
   }
   oapc_tcp_closesocket(data->m_sock);
#ifdef ENV_WINDOWS
   WSACleanup();
#endif
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

   data=(struct instData*)instanceData;
   data->DO[input]=value;
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

   if (output>3) return OAPC_ERROR_NO_SUCH_IO;
   data=(struct instData*)instanceData;

   *value=data->DI[output];
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

   if (output<4) return OAPC_ERROR_NO_SUCH_IO;
   data=(struct instData*)instanceData;

   *value=data->AI[output-4];      
   return OAPC_OK;
}


