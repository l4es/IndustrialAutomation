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
#endif

#include <wx/wx.h>
#include <wx/mstream.h>

#ifndef ENV_WINDOWS
 #include <arpa/inet.h>
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
 #define closesocket close
#else
 #include <windows.h>
 typedef int socklen_t;
#endif

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#define MAX_XML_SIZE  3000
#define MAX_TEXT_SIZE  250
#define MAX_IP_SIZE    100

#include "oapc_libio.h"
#include "liboapc.h"


struct libio_config
{
   unsigned short version,length;
   char           ip[MAX_IP_SIZE+1];
   unsigned short port;
   char           res[MAX_TEXT_SIZE+1];
};



struct instData
{
   struct libio_config   config;
   int                   sock;
   struct oapc_bin_head *retBin;
   int                   retCode,m_callbackID;
   char                  retStr[MAX_TEXT_SIZE+1];
   char                  buffer[MAX_TEXT_SIZE+1];
   bool                  running;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\
<oapc-config>\
 <flowimage>%s</flowimage>\
 <dialogue>\
  <general>\
   <param>\
    <name>ip</name>\
    <text>IP</text>\
    <type>string</type>\
    <default>%s</default>\
    <min>3</min>\
    <max>%d</max>\
   </param>\
   <param>\
    <name>port</name>\
    <text>Port</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>1</min>\
    <max>65535</max>\
   </param>\
   <param>\
    <name>res</name>\
    <text>Resource</text>\
    <type>string</type>\
    <default>%s</default>\
    <min>0</min>\
    <max>%d</max>\
   </param>\
  </general>\
  <helppanel>\
   <in0>IP - host address</in0>\
   <in1>PORT - port number</in1>\
   <in2>RES - URL resource</in2>\
   <in3>GET - get the data</in3>\
   <out0>DATA - retrieved binary data</out0>\
   <out1>TXT - retrieved plain text data</out1>\
   <out2>ERR - HTTP error/return code</out2>\
  </helppanel>\
 </dialogue>\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUEAgQcijwshowE/vwEMqL8/gROis+s0oysyuwsjnQEQpQ9ri5psoyCr90kYpDI4pAIV23U5tSEylRqnNM1cckQalw0mU2EgoR0vlz8AvykyqzU6vQUULecznRcnpTE3qRssqxgvED8/vxUonS+1+WUvuQ0bqQkniwUekx8vnyMupzEwsRMiqS83nSc1mTU6rQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABnwOt/AAACUklEQVR42tWY7XaiMBCGYw1oVQTlw2rCWthQdCl6/3e3kyCFuGITJO7p/PA45phn3swkgUHoiZamKdpst9sN/yac8XjcONIIfNSOtlWoAsGEqKh+KhCgGkca4f8oHkLx4JtoQVbbkUb6i6pREHwTLchqO9JIf1E1CuJtRavo9EQV7WgVnZ6oVIpW0emJMm9fubojAw2hqSn29iz1xk7rEXRzpC9Knr1L1hCqnpgreWVgySQht51bRlg2ZRqq5MRJuZLHri0LKKUYU2eljJIS1+lcG+MgwZpN9kZzlQXCOGsZl7+YGkr68SpxXaEByfZ93wbWx7Es4yNTy1Uqpyf9viwYkJwsy5zdn89FWZ5OpwXRVXVVm12iBImtmOuWJSet12/vZnI1pYCakvNZoDhpnbwSFVXaKJ+j2OoiCUhJkswLzX2lZpSj9vuzW8axkMRR75r7ShVF7Z3rsl18XLxdSPMXI7kCVRNYvYnjNyRTKPwp0sRFXUB5/moI9fJVeck8Gh3y3PPMoAJ+GFWkCFgeDkfYWxhBORgvf1eVF0XzEE7cAw7PRlAEznO8rOohP2BhgfZpgVKVaEAWnlUFkXuCRJkuSnU/2zB71CJhX/u+Ut3PJOAoKPKwgzTg1UhsHPLt5IUzfuf3uYXVciWO9yDKR97Mw9QnfS58rbOXZHAR+xnp9xz40APt/349uLOADz4/6+XqWaifm6umWXLXGQS1sSyrowHTas0MgSoQoDoaMK3WzDDvVxurswEzkKimG2N1NmAGEqXbjRkCZaYB8+8OfZL9BcouYxUX6Xs1AAAAAElFTkSuQmCC";
static char                 libname[]="HTTP Client";
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
   return OAPC_HAS_OUTPUTS|
          OAPC_HAS_INPUTS|
          OAPC_HAS_XML_CONFIGURATION|
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|
          OAPC_ACCEPTS_IO_CALLBACK|
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
   return OAPC_CHAR_IO0|
          OAPC_NUM_IO1|
          OAPC_CHAR_IO2|
          OAPC_DIGI_IO3;
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
   return OAPC_BIN_IO0|
          OAPC_CHAR_IO1|
          OAPC_NUM_IO2;
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
                                       data->config.res,MAX_TEXT_SIZE);
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

   if (strcmp(name,"ip")==0)         strncpy(data->config.ip,value,MAX_IP_SIZE);
   else if (strcmp(name,"port")==0)  data->config.port=(unsigned short)atoi(value);
   else if (strcmp(name,"res")==0)   strncpy(data->config.res,value,MAX_TEXT_SIZE);
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
   save_config.version =htons(data->config.version);
   save_config.length  =htons(data->config.length);
   strncpy(save_config.ip,data->config.ip,MAX_IP_SIZE);
   save_config.port    =htons(data->config.port);
   strncpy(save_config.res,data->config.res,MAX_TEXT_SIZE);
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
   data->config.version =ntohs(save_config.version);
   data->config.length  =ntohs(save_config.length);
   strncpy(data->config.ip,save_config.ip,MAX_IP_SIZE);
   data->config.port    =ntohs(save_config.port);
   strncpy(data->config.res,save_config.res,MAX_IP_SIZE);
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

   memset(data,0,sizeof(struct instData));
   data->config.version=1;
   data->config.length=sizeof(struct libio_config);
   strcpy(data->config.ip,"192.168.1.1");
   data->config.port=80;
   strcpy(data->config.res,"/index.php");

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



void *recvLoop(void *arg)
{
   struct instData *data;
   char             line[1001],mime[101];;
   char            *payloadBuffer=NULL;
   int              payloadLen=-1;

   data=(struct instData*)arg;

 	sprintf(line,"GET %s HTTP/1.1\r\nHost: %s\r\nAccept: */*\r\nUser-Agent: OpenAPC HTTP PlugIn\r\n\r\n",data->config.res,data->config.ip);
   oapc_tcp_send(data->sock,line,strlen(line),2000);
   memset(line,0,sizeof(line));
   while ((data->running) && (oapc_tcp_recv(data->sock,line,1000,"\n",4000)>0))
   {
   	if (strstr(line,"HTTP/1.")==line)
   	{
   		data->retCode=atoi(line+9);
         m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
         if (data->retCode!=200) break;
   	}
   	else if (strstr(line,"Content-Length:")==line)
   	{
   		payloadLen=atoi(line+16);
   		if (payloadLen<=0) break;
   	}
   	else if (strstr(line,"Content-Type:")==line)
   	{
         strncpy(mime,line+14,100);
   	}
   	else if ((line[0]=='\r') || (line[0]=='\n')) // the empty line that terminates the header
   	{
         if (!strncmp(mime,"image/",6))
         {
            wxImage             *image;
            wxMemoryInputStream *memStream;
         
   	   	payloadBuffer=(char*)malloc(payloadLen);
   		   if (!payloadBuffer)
      		{
      			data->retCode=-4;
               m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
               break;
      		}
      		if (oapc_tcp_recv(data->sock,payloadBuffer,payloadLen,NULL,10000)<payloadLen)
   	   	{
   		   	data->retCode=-3;
               m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
               break;
      		}
   	  	
   		   memStream=new wxMemoryInputStream(payloadBuffer,payloadLen);
      		image=new wxImage(*memStream,wxBITMAP_TYPE_ANY);
      		delete memStream;
            if ((!image) || (!image->Ok()))
            {
      			data->retCode=-5;
               m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
               break;
            }
         
            data->retBin=(struct oapc_bin_head*)malloc(sizeof(struct oapc_bin_head)+(3*image->GetWidth()*image->GetHeight()));
            if (!data->retBin)
            {
   		   	data->retCode=-6;
               m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
               break;
            }
            memset(data->retBin,0,sizeof(struct oapc_bin_head));
            data->retBin->version=OAPC_BIN_HEAD_CURR_VERSION;
            data->retBin->param1=image->GetWidth();
            data->retBin->param2=image->GetHeight();
            data->retBin->param3=24;
            data->retBin->type=OAPC_BIN_TYPE_IMAGE;
            data->retBin->subType=OAPC_BIN_SUBTYPE_IMAGE_RGB24;
            data->retBin->sizeData=3*image->GetWidth()*image->GetHeight();
            memcpy(&data->retBin->data,image->GetData(),data->retBin->sizeData);
            delete image;
            m_oapc_io_callback(OAPC_BIN_IO0,data->m_callbackID);
         }
         else if (!strncmp(mime,"text/plain",10))
         {
         	memset(line,0,sizeof(line));
            while ((data->running) && (oapc_tcp_recv(data->sock,data->retStr,MAX_TEXT_SIZE,"\n",4000)>0))
            {
               if (strlen(data->retStr)>0)
               {
                  if (data->retStr[strlen(data->retStr)-1]=='\n') data->retStr[strlen(data->retStr)-1]=0;
                  if (data->retStr[strlen(data->retStr)-1]=='\r') data->retStr[strlen(data->retStr)-1]=0;
               }
               m_oapc_io_callback(OAPC_CHAR_IO1,data->m_callbackID);
               memset(line,0,sizeof(line));
            }
         }
   				
   		break;
   	} 
      memset(line,0,sizeof(line));
   }
   oapc_tcp_closesocket(data->sock);
   if (payloadBuffer) free(payloadBuffer);

   data->sock=0;
   data->running=true;
   return NULL;
}


/**
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API unsigned long oapc_init(void* /*instanceData*/)
{
/*   struct instData   *data;

   data=(struct instData*)instanceData;*/

#ifdef ENV_WINDOWS
   WSADATA wsa;

   WSAStartup(MAKEWORD(2,0),&wsa);
#endif
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

   data->running=false;
#ifdef ENV_WINDOWS
   WSACleanup();
#endif
   while ((!data->running) && (ctr<10)) // wait for thread to finish
   {
      oapc_thread_sleep(250);
      ctr++;
   }
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

   if (input!=3) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   if (data->sock>0) return OAPC_ERROR_STILL_IN_PROGRESS;   
   
   data->sock=oapc_tcp_connect_to(data->config.ip,data->config.port);
   if (data->sock<=0) return OAPC_ERROR_CONNECTION;

   // set socket to non-blocking in order to avoid deadlocks when sending fails
   oapc_tcp_set_blocking(data->sock,false);
   data->running=true;
   if (!oapc_thread_create(recvLoop,data)) return OAPC_ERROR_NO_MEMORY;
   
   return OAPC_OK;
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

   if (input!=2) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   data->config.port=value;
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

   if (output!=1) return OAPC_ERROR_NO_SUCH_IO;
   data=(struct instData*)instanceData;

   *value=data->retCode;
   return OAPC_OK;
}



/**
This function is called by the main application when the library provides an character input (marked
using the digital input flags OAPC_CHAR_IO...), a connection was edited to this input and a data
flow reaches the input.
@param[in] input specifies the input where the data are send to, here not the OAPC_CHAR_IO...-flag is used
           but a plain, 0-based input number
@param[in] value specifies the string that is set to that input
@return an error code OAPC_ERROR_... in case of an error or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_set_char_value(void* instanceData,unsigned long input,char *value)
{
   // here we allocate the send buffer dynamically because we do not know how big the string is
   struct instData *data;

   data=(struct instData*)instanceData;

   if ((input!=0) && (input!=2)) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   if (input==0) // ip
    strncpy(data->config.ip,value,MAX_IP_SIZE);
   else if (input==2) // resource
   {
      strncpy(data->config.res,value,MAX_TEXT_SIZE);
      oapc_set_digi_value(data,3,1);
   }
   return OAPC_OK;
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
   struct instData *data;

   data=(struct instData*)instanceData;

   if (output!=1) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   if (length>MAX_TEXT_SIZE) length=MAX_TEXT_SIZE;
   strncpy(value,data->retStr,length);
   return OAPC_OK;
}



/**
This function is called by the main application periodically in order to poll the state of the related
output. It has to return the data that are available for that input or - in case there are none available -
the error code OAPC_ERROR_NO_DATA_AVAILABLE to notify the main application, that there is nothing new.
@param[in] output specifies the output where the data are fetched from, here not the OAPC_BIN_IO...-flag is used
           but the plain, 0-based output number
@param[out] value if there are new data available, they have to be stored within that variable, if there are
           no new data, the returned data are ignored by the main application
@return an error code OAPC_ERROR_... in case of an error, OAPC_ERROR_NO_DATA_AVAILABLE in case no new data are
           available or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_get_bin_value(void *instanceData,unsigned long output,struct oapc_bin_head **value)
{
   struct instData *data;

   if (output!=0) return OAPC_ERROR_NO_SUCH_IO;
   data=(struct instData*)instanceData;

   *value=data->retBin;
   return OAPC_OK;
}





OAPC_EXT_API void oapc_release_bin_data(void *instanceData,unsigned long output)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   	
   free(data->retBin);
   data->retBin=NULL;
}




#ifdef TEST_MODE
static void test_io_callback(unsigned long outputs,unsigned long callbackID)
{
   printf("Callback: %ld - %ld\n",outputs,callbackID);
}



int main(int argc, char* argv[])
{
   struct instData *data;
   double           speed,pos;

   data=(struct instData*)oapc_create_instance2(0);
   if (data)
   {
      strcpy(data->config.ip,"62.112.159.249");
      strcpy(data->config.res,"/");
      oapc_set_io_callback(data,test_io_callback,42);
      if (oapc_init(data)==OAPC_OK)
      {
         oapc_set_digi_value(data,1,4);   // home
         oapc_thread_sleep(750);
         oapc_exit(data);
      }
      oapc_delete_instance(data);
   }
   return 0;
}
#endif
