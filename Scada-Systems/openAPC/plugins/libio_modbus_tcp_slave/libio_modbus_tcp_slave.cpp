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
#include <stdarg.h>
#include <assert.h>

#ifndef ENV_WINDOWSCE
 #include <errno.h>
 #include <sys/types.h>
#else
 #include <winsock2.h>
 #include "time_ce.h"
#endif

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

#define MAX_XML_SIZE          12000
#define MAX_INCOMING_CONNECTIONS 50
#define MAX_RECVBUF_SIZE         (sizeof(struct modtcp_adu)+10)
#define MAX_SENDBUF_SIZE         (MAX_RECVBUF_SIZE*2)

#include <list>

#include "oapc_libio.h"
#include "liboapc.h"
#include "modbus_common.h"
#include "modbus_tcp_common.h"


struct libio_config
{
   unsigned short    version,length;
   char              ip[MAX_IP_SIZE];
   unsigned short    port;
   struct mod_config modConfig[MAX_NUM_IOS];
   unsigned int      flags,incoming;
   char              logfile[MAX_FILENAME_SIZE+4];
};

struct client_dataset
{
   int  sock;
   int  received,toSend;
   char recvBuffer[MAX_RECVBUF_SIZE],sendBuffer[MAX_SENDBUF_SIZE];
};


struct instData
{
   struct libio_config   config;
   struct mod_data       modData[MAX_NUM_IOS];
   int                   sock;
   int                   m_callbackID;
   bool                  running;
   struct client_dataset client[MAX_INCOMING_CONNECTIONS];
   FILE                 *FHandle;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
 <flowimage>%s</flowimage>\n\
 <dialogue>\n\
  <general>\n\
   <param>\n\
    <name>ip</name>\n\
    <text>Local IP</text>\n\
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
   <param>\n\
    <name>incoming</name>\n\
    <text>Incoming Connections</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>1</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>byteorder</name>\n\
    <text>Invert byte order</text>\n\
    <type>checkbox</type>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n\
    <name>logcom</name>\n\
    <text>Log Communication Data</text>\n\
    <type>checkbox</type>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n\
    <name>logfile</name>\n\
    <text>Log Filename</text>\n\
    <type>filesave</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
  </general>\n\
  <stdpanel>\n\
   <name>Access Parameters</name>\n\
   <param>\n\
    <name>mode0</name>\n\
    <text>IO0 Operation Mode</text>\n\
    <type>option</type>\n\
    <value>Disabled</value>\n\
    <value>Read/Write Coils</value>\n\
    <value>Read/Write Holding Register</value>\n\
    <value>Set Discrete Input</value>\n\
    <value>Set Input Register</value>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n\
    <name>addroffset0</name>\n\
    <text>IO0 base address offset</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>65503</max>\n\
   </param>\n\
   <param>\n\
    <name>mode1</name>\n\
    <text>IO1 Operation Mode</text>\n\
    <type>option</type>\n\
    <value>Disabled</value>\n\
    <value>Read/Write Coils</value>\n\
    <value>Read/Write Holding Register</value>\n\
    <value>Set Discrete Input</value>\n\
    <value>Set Input Register</value>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n\
    <name>addroffset1</name>\n\
    <text>IO1 base address offset</text>\n""\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>65503</max>\n\
   </param>\n\
   <param>\n\
    <name>mode2</name>\n\
    <text>IO2 Operation Mode</text>\n\
    <type>option</type>\n\
    <value>Disabled</value>\n\
    <value>Read/Write Coils</value>\n\
    <value>Read/Write Holding Register</value>\n\
    <value>Set Discrete Input</value>\n\
    <value>Set Input Register</value>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n""\
    <name>addroffset2</name>\n""\
    <text>IO2 base address offset</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>65503</max>\n\
   </param>\n\
   <param>\n\
    <name>mode3</name>\n\
    <text>IO3 Operation Mode</text>\n\
    <type>option</type>\n\
    <value>Disabled</value>\n\
    <value>Read/Write Coils</value>\n\
    <value>Read/Write Holding Register</value>\n\
    <value>Set Discrete Input</value>\n\
    <value>Set Input Register</value>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n\
    <name>addroffset3</name>\n\
    <text>IO3 base address offset</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>65503</max>\n\
   </param>\n\
  </stdpanel>\n\
  <stdpanel>\n\
   <name>Access Parameters</name>\n\
   <param>\n\
    <name>mode4</name>\n\
    <text>IO4 Operation Mode</text>\n\
    <type>option</type>\n\
    <value>Disabled</value>\n\
    <value>Read/Write Coils</value>\n\
    <value>Read/Write Holding Register</value>\n\
    <value>Set Discrete Input</value>\n\
    <value>Set Input Register</value>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n\
    <name>addroffset4</name>\n\
    <text>IO4 base address offset</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>65503</max>\n\
   </param>\n\
   <param>\n\
    <name>mode5</name>\n\
    <text>IO5 Operation Mode</text>\n\
    <type>option</type>\n\
    <value>Disabled</value>\n\
    <value>Read/Write Coils</value>\n\
    <value>Read/Write Holding Register</value>\n\
    <value>Set Discrete Input</value>\n\
    <value>Set Input Register</value>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n\
    <name>addroffset5</name>\n\
    <text>IO5 base address offset</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>65503</max>\n\
   </param>\n\
   <param>\n\
    <name>mode6</name>\n\
    <text>IO6 Operation Mode</text>\n\
    <type>option</type>\n\
    <value>Disabled</value>\n\
    <value>Read/Write Coils</value>\n\
    <value>Read/Write Holding Register</value>\n\
    <value>Set Discrete Input</value>\n\
    <value>Set Input Register</value>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n""\
    <name>addroffset6</name>\n\
    <text>IO6 base address offset</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>65503</max>\n\
   </param>\n\
   <param>\n\
    <name>mode7</name>\n\
    <text>IO7 Operation Mode</text>\n\
    <type>option</type>\n\
    <value>Disabled</value>\n\
    <value>Read/Write Coils</value>\n\
    <value>Read/Write Holding Register</value>\n\
    <value>Set Discrete Input</value>\n\
    <value>Set Input Register</value>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n\
    <name>addroffset7</name>\n\
    <text>IO7 base address offset</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>65503</max>\n\
   </param>\n\
  </stdpanel>\n\
  <helppanel>\n\
   <in0>SET - output data</in0>\n\
   <in1>...</in1>\n\
   <out0>GET - input data</out0>\n\
   <out1>...</out1>\n\
  </helppanel>\n\
 </dialogue>\n\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                        flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUEAgRqh1EE/vwEKbRsgtmyxZ7Axe88VMji6daMpm+cpeQcRgRbcNA8Zhvy+OoNQLyEgoR8ktx0kkzEwsTR1fRFY8axtexXdzfi5fmgtIxyfts8Rsy80pwwWAj09fxsiFwMMbSEm2zU3ryctnR8gtx8mGRFaiVifUqYneKovJi8zq+kvOxMWNRsdtxUX9QkSry8yqzGy/SkrOgoUAjp7PyEitw0XhR8nlT8/vzW2/Q9SsycsoS4vOxUdtTU3syRleQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB4BFAfAAACPklEQVR42u2YAXeaMBCAg1SqhGo3RSjZrHag1lG3VunOrJv//1/tQgwFHvimr3F7G/c05CDmu7tcIgkhZxQAINetVuta1DQqEjUgqJOBfKhLkSjBVmZoU/YoZCsztCl7FLIzM3QpCjV4NUOXolCQM0OTkqH0SzZWOaSabZlNeaWmWalVVTN1gVLTQuekUoFita4ZKQewNpAEirbn+sj/RCgEqh5Vtz/PWDWoQ53l/i70ogZK/jvUheM4T2BjeQFjOwyC0N7AKBGyewRLVoZwYxhTiG5v76HXNeZzo30Cqm+apguXWPbHDkslHieywixbXvnQoPSq90zpFGY0lfbxaSFQk/FEoLac88uYMzZMsIYAtrM5j1cxY4sM1aF0PUOcUbW8HE52gTI/i6L/KCg+Qiz0yoGQswC9WmwWjNnvFeqO0tnyw7du+0SUu0dxtvWRJ1E7lqK4COA286on4/e8PAnlCNgEUdjpVnrFUxQP1FhZmVdws05ZM/i9t78iyhIhnMgAKq94FkAbLMaS17GKup/u2pR+PAm1+WL2N4gaIiVeYcQwLdLA8cTmLHZWjO0idOQKv99FWsw9gaoNHKkPoL8w3wnU+IFh9Hia7Dwdoxc7vcPZaCmHaN1RyT49HvXDdf2N+LgOwCh8cILEh6cgDIMwscAKhIQvAF8Nz/OinwCdaO55xj0cj3rbFb16Kr8NqrQgVS9QDeqvQJ0vLZqXs0NIrduDUh9atwfk39geFCaBLqU5jWlOY/74aczZ5Be51ksCujNepwAAAABJRU5ErkJggg==";
static char                        libname[]="Modbus TCP Slave";
static char                        xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback        m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports
static struct libio_config         save_config;
static unsigned char              *coilMem=NULL,*discreteMem=NULL;
static unsigned short             *regMem=NULL,*inputMem=NULL;
static void                       *listMutex=NULL;
static std::list<struct instData*> instList;
static int                         sockCnt=0;
static struct instData            *serverInstanceData=NULL;


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
   return OAPC_NUM_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|
          OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6|OAPC_NUM_IO7;
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
   return OAPC_NUM_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|
          OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6|OAPC_NUM_IO7;
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
                                       data->config.port,data->config.incoming,MAX_INCOMING_CONNECTIONS,
                                       ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER),
                                       ((data->config.flags & MODBUS_TCP_FLAG_LOG_COMMUNICATION)==MODBUS_TCP_FLAG_LOG_COMMUNICATION),data->config.logfile,MAX_FILENAME_SIZE,
                                       data->config.modConfig[0].mode,data->config.modConfig[0].addrOffset,
                                       data->config.modConfig[1].mode,data->config.modConfig[1].addrOffset,
                                       data->config.modConfig[2].mode,data->config.modConfig[2].addrOffset,
                                       data->config.modConfig[3].mode,data->config.modConfig[3].addrOffset,
                                       data->config.modConfig[4].mode,data->config.modConfig[4].addrOffset,
                                       data->config.modConfig[5].mode,data->config.modConfig[5].addrOffset,
                                       data->config.modConfig[6].mode,data->config.modConfig[6].addrOffset,
                                       data->config.modConfig[7].mode,data->config.modConfig[7].addrOffset);
   assert(strlen(xmldescr)<MAX_XML_SIZE);
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
   else if (strcmp(name,"incoming")==0)
   {
      data->config.incoming=(unsigned short)atoi(value);
      if (data->config.incoming>MAX_INCOMING_CONNECTIONS) data->config.incoming=MAX_INCOMING_CONNECTIONS;
   }
   else if (strcmp(name,"byteorder")==0)
   {
      if (atoi(value)!=0) data->config.flags|=MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER;
      else data->config.flags&=~MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER;
   }
   else if (strcmp(name,"logcom")==0)
   {
      if (atoi(value)!=0) data->config.flags|=MODBUS_TCP_FLAG_LOG_COMMUNICATION;
      else data->config.flags&=~MODBUS_TCP_FLAG_LOG_COMMUNICATION;
   }
   if (strcmp(name,"logfile")==0)          strncpy(data->config.logfile,value,MAX_FILENAME_SIZE);
   else if (strcmp(name,"mode0")==0)       data->config.modConfig[0].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset0")==0) data->config.modConfig[0].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"mode1")==0)       data->config.modConfig[1].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset1")==0) data->config.modConfig[1].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"mode2")==0)       data->config.modConfig[2].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset2")==0) data->config.modConfig[2].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"mode3")==0)       data->config.modConfig[3].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset3")==0) data->config.modConfig[3].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"mode4")==0)       data->config.modConfig[4].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset4")==0) data->config.modConfig[4].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"mode5")==0)       data->config.modConfig[5].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset5")==0) data->config.modConfig[5].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"mode6")==0)       data->config.modConfig[6].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset6")==0) data->config.modConfig[6].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"mode7")==0)       data->config.modConfig[7].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset7")==0) data->config.modConfig[7].addrOffset=(unsigned short)atoi(value);
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
   int              i;

   data=(struct instData*)instanceData;

   *length=sizeof(struct libio_config);
   save_config.version =htons(1);
   save_config.length  =htons((unsigned short)*length);
   strncpy(save_config.ip,data->config.ip,MAX_IP_SIZE);
   save_config.port    =htons(data->config.port);
   save_config.incoming=htonl(data->config.incoming);
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      save_config.modConfig[i].mode=data->config.modConfig[i].mode;
      save_config.modConfig[i].addrOffset=htons(data->config.modConfig[i].addrOffset);
   }
   save_config.flags=htonl(data->config.flags);
   strncpy(save_config.logfile,data->config.logfile,MAX_FILENAME_SIZE);
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
   int              i;

   data=(struct instData*)instanceData;

   if (length>sizeof(struct libio_config)) length=sizeof(struct libio_config);
   memcpy(&save_config,loadedData,length);
   data->config.version =ntohs(save_config.version);
   data->config.length  =ntohs(save_config.length);
   strncpy(data->config.ip,save_config.ip,MAX_IP_SIZE);
   data->config.port    =ntohs(save_config.port);
   data->config.incoming=ntohl(save_config.incoming);
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      data->config.modConfig[i].mode=save_config.modConfig[i].mode;
      data->config.modConfig[i].addrOffset=ntohs(save_config.modConfig[i].addrOffset);
   }
   data->config.flags=ntohl(save_config.flags);
   strncpy(data->config.logfile,save_config.logfile,MAX_FILENAME_SIZE);
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
   struct instData *data;

   if (!listMutex) listMutex=oapc_thread_mutex_create();
   flags=flags; // removing "unused" warning
   data=(struct instData*)malloc(sizeof(struct instData));
   if (!data) return NULL;

   memset(data,0,sizeof(struct instData));
   strcpy(data->config.ip,"0.0.0.0");
   data->config.port=502;
   data->sock=-1;
   data->config.incoming=3;
   data->config.modConfig[0].mode=MOD_RW_COILS;
   data->config.modConfig[1].mode=MOD_UNUSED;
   data->config.modConfig[2].mode=MOD_UNUSED;
   data->config.modConfig[3].mode=MOD_UNUSED;
   data->config.modConfig[4].mode=MOD_UNUSED;
   data->config.modConfig[5].mode=MOD_UNUSED;
   data->config.modConfig[6].mode=MOD_UNUSED;
   data->config.modConfig[7].mode=MOD_UNUSED;
   data->config.flags=MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER;
   oapc_thread_mutex_lock(listMutex);
   instList.push_back(data);
   oapc_thread_mutex_unlock(listMutex);
   return data;
}



/**
This function is called finally, it has to be used to release the instance data structure that was created
during the call of oapc_create_instance()
*/
OAPC_EXT_API void oapc_delete_instance(void* instanceData)
{
   struct instData *data;
   std::list<struct instData*>::iterator it;

   data=(struct instData*)instanceData;
   oapc_thread_mutex_lock(listMutex);
   for (it=instList.begin(); it!=instList.end(); it++)
   {
      if (*it==data)
      {
         instList.erase(it);
         break;
      }
   }
   oapc_thread_mutex_unlock(listMutex);

   if (instanceData) free(instanceData);
}



static void writeLog(struct instData *data,const char *format,...)
{
   va_list  arglist;
   char     sText[1500];

   if (!data->FHandle) return;
   va_start(arglist,format);
   vsprintf(sText,format,arglist);
   va_end(arglist);
#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
   fprintf(data->FHandle,"%d\t%s\r\n",time_ce(NULL),sText);
 #else
   fprintf(data->FHandle,"%d\t%s\r\n",time(NULL),sText);
 #endif
#else
   fprintf(data->FHandle,"%d\t%s\n",(int)time(NULL),sText);
#endif
}



static void swapByteorder(unsigned short *addr)
{
   unsigned char store;

   store=(unsigned char)(((*addr) & 0xFF00)>>8);
   *addr=(unsigned short)(((*addr)<<8) & 0xFF00);
   *addr|=store;
}



/**
This function checks if there are new incoming clients. If the number of the client does not
exceed the limit it is accepted, elsewhere an error message is sent and the connection is
closed. Additionally the successfully connected clients are polled for new data.
this function is not part of the external library interface.
*/
static bool check_all_clients(struct instData *data)
{
   int                c;
   unsigned int       i;
   unsigned long      ip;

   if (data->sock<=0) return false;
   c=oapc_tcp_accept_connection(data->sock,&ip);
   if (c>0)
   {
      for (i=0; i<data->config.incoming; i++) if (data->client[i].sock==-1)
      {
         data->client[i].sock=c;
         // set socket to non-blocking in order to avoid deadlocks when sending fails
         oapc_tcp_set_blocking(c,false);
         return true;
      }
      writeLog(data,"Incoming client would exceed maximum number of connections, rejected");
      oapc_tcp_closesocket(c); // still too much connections
   }
   return false;
}



static unsigned char *getCoilMem()
{
   if (!coilMem) coilMem=(unsigned char*)malloc(8192);
   return coilMem;
}



static unsigned short *getRegMem()
{
   if (!regMem) regMem=(unsigned short*)malloc(65535*2);
   return regMem;
}



static unsigned char *getDiscreteMem()
{
   if (!discreteMem) discreteMem=(unsigned char*)malloc(8192);
   return discreteMem;
}



static unsigned short *getInputMem()
{
   if (!inputMem) inputMem=(unsigned short*)malloc(65535*2);
   return inputMem;
}



static unsigned int getIOFlag(unsigned int num)
{
   switch (num)
   {
      case 0:
         return OAPC_NUM_IO0;
         break;
      case 1:
         return OAPC_NUM_IO1;
         break;
      case 2:
         return OAPC_NUM_IO2;
         break;
      case 3:
         return OAPC_NUM_IO3;
         break;
      case 4:
         return OAPC_NUM_IO4;
         break;
      case 5:
         return OAPC_NUM_IO5;
         break;
      case 6:
         return OAPC_NUM_IO6;
         break;
      default:
         return OAPC_NUM_IO7;
         break;
   }
}



static void checkForChangedCoilData(void)
{
   int                                   j;
   unsigned char                        *c;
   unsigned int                          lData=0;
   std::list<struct instData*>::iterator it;
   struct instData                      *data;

   c=getCoilMem();
   if (c)
   {
      oapc_thread_mutex_lock(listMutex);
      for (it=instList.begin(); it!=instList.end(); it++)
      {
         data=*it;
         for (j=0; j<MAX_NUM_IOS; j++)
         {
            if (data->config.modConfig[j].mode==MOD_RW_COILS)
            {
               unsigned int srcBit,trgBit=0x80000000,srcByte,count;

               srcByte=data->config.modConfig[j].addrOffset/8;
               srcBit=0x80>>(data->config.modConfig[j].addrOffset % 8);
               count=32;

               while (count>0)
               {
                  if ((c[srcByte] & srcBit)!=0) lData|=trgBit;

                  if (srcBit==0x01)
                  {
                     srcBit=0x80;
                     srcByte++;
                  }
                  else srcBit=srcBit>>1;
                  trgBit=trgBit>>1;
                  count--;
               }
               if (lData!=data->modData[j].rData)
               {
                  data->modData[j].rData=lData;
                  m_oapc_io_callback(getIOFlag(j),data->m_callbackID);
               }
            }
         }
      }
      oapc_thread_mutex_unlock(listMutex);
   }
}



static void checkForChangedRegData(int addrOffset,int len)
{
   unsigned short                       *w;
   int                                   j;
   std::list<struct instData*>::iterator it;
   struct instData                      *data;

   w=getRegMem();
   if (w)
   {
      oapc_thread_mutex_lock(listMutex);
      for (it=instList.begin(); it!=instList.end(); it++)
      {
         data=*it;

         for (j=0; j<MAX_NUM_IOS; j++)
         {
            if ((data->config.modConfig[j].mode==MOD_RW_HOLDREG) &&
                (data->config.modConfig[j].addrOffset>=addrOffset) &&
                (data->config.modConfig[j].addrOffset<addrOffset+len))
            {
               if (w[addrOffset]!=data->modData[j].rData)
               {
                  data->modData[j].rData=w[addrOffset];
                  m_oapc_io_callback(getIOFlag(j),data->m_callbackID);
               }
            }
         }
      }
   }
}



void *recvLoop(void *arg)
{
   struct instData *data;
   int              i;
   bool             dataReceived;
#ifdef ENV_WINDOWS
   int              err;
#endif

   data=(struct instData*)arg;
   while (data->running)
   {
      oapc_thread_sleep(0);
      dataReceived=check_all_clients(data);

      for (i=0; i<MAX_INCOMING_CONNECTIONS; i++)  if (data->client[i].sock!=-1)
      {
         if (!data->running) break;
         if (data->client[i].toSend>0)
         {
            int sentLen;

            sentLen=oapc_tcp_send(data->client[i].sock,data->client[i].sendBuffer,data->client[i].toSend,50);
            if (sentLen<=0)
            {
#ifdef ENV_WINDOWS
               err=GetLastError();
               if (
#ifndef ENV_WINDOWSCE
                  (err!=EAGAIN) && 
#endif
                  (err!=WSAEWOULDBLOCK) && (err!=0))
#else
               if ((errno!=EAGAIN) && (errno!=EINPROGRESS) && (errno!=0))
#endif
               {
                  oapc_tcp_closesocket(data->client[i].sock);
                  data->client[i].sock=-1;
                  writeLog(data,"Sending data failed, connection closed");
                  continue;
                  // connection failure
               }
            }
            else
            {
               // memmove sent data
               memmove(data->client[i].sendBuffer,data->client[i].sendBuffer+sentLen,MAX_SENDBUF_SIZE-sentLen);
               data->client[i].toSend-=sentLen;
            }
         }
   		if (data->client[i].received<(int)sizeof(struct modtcp_adu))
   		{
   		   int recvLen;

            recvLen=oapc_tcp_recv(data->client[i].sock,
                                  data->client[i].recvBuffer+data->client[i].received,sizeof(struct modtcp_adu)-data->client[i].received,
                                  NULL,50);
            if (recvLen<=0)
            {
#ifdef ENV_WINDOWS
               err=GetLastError();
               if (
#ifndef ENV_WINDOWSCE
                   (err!=EAGAIN) && 
#endif
                   (err!=WSAEWOULDBLOCK) && (err!=0))
#else
               if ((errno!=EAGAIN) && (errno!=EINPROGRESS) && (errno!=0))
#endif
               {
                  oapc_tcp_closesocket(data->client[i].sock);
                  data->client[i].sock=-1;
                  writeLog(data,"Receiving data failed, connection closed");
                  continue;
                  // connection failure
               }
            }
            else
            {
               data->client[i].received+=recvLen;
               dataReceived=true;
            }
   		}
         if (data->client[i].received>=(int)sizeof(struct modtcp_adu_head))
         {
            struct modtcp_adu_head *recvHead;
            unsigned short          length;

            recvHead=(struct modtcp_adu_head*)&data->client[i].recvBuffer;
            length=ntohs(recvHead->length);
            if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==0)
             swapByteorder(&length);
            if ((length+sizeof(struct modtcp_adu_head))>=(unsigned int)data->client[i].received)
            {
               struct modtcp_adu *recvAdu;
               unsigned char     *c;

               recvAdu=(struct modtcp_adu*)&data->client[i].recvBuffer;
               recvAdu->pdu.addrOffset=ntohs(recvAdu->pdu.addrOffset);
               if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==0)
                swapByteorder(&recvAdu->pdu.addrOffset);
               if (recvAdu->pdu.funcCode==MODCMD_WRITE_SINGLE_COIL)
               {
                  c=getCoilMem();
                  if (c)
                  {
                     int bit,byte;

                     byte=recvAdu->pdu.addrOffset/8;
                     bit=0x80>>(recvAdu->pdu.addrOffset % 8);
                     if (!recvAdu->pdu.data[0]) c[byte]&=~bit;
                     else c[byte]|=bit;
                     checkForChangedCoilData();
                  }
                  else
                  {
                     // send out of memory error code
                  }
               }
               else if (recvAdu->pdu.funcCode==MODCMD_WRITE_MULTI_COILS)
               {
                  unsigned char *c;

                  c=getCoilMem();
                  if (c)
                  {
                     int srcBit,trgBit,srcByte=3,trgByte,count;

                     trgByte=recvAdu->pdu.addrOffset/8;
                     trgBit=0x80>>(recvAdu->pdu.addrOffset % 8);
                     count=(recvAdu->pdu.data[0]<<8)|recvAdu->pdu.data[1];
                     srcBit=0x80;
                     while ((count>0) && (srcByte<=255))
                     {
                        if ((recvAdu->pdu.data[srcByte] & srcBit)==0) c[trgByte]&=~trgBit;
                        else c[trgByte]|=trgBit;

                        if (srcBit==0x01)
                        {
                           srcBit=0x80;
                           srcByte++;
                        }
                        else srcBit=srcBit>>1;
                        if (trgBit==0x01)
                        {
                           trgBit=0x80;
                           trgByte++;
                        }
                        else trgBit=trgBit>>1;
                        count--;
                     }
                     checkForChangedCoilData();
                  }
                  else
                  {
                     // send out of memory error code
                  }
               }
               else if ((recvAdu->pdu.funcCode==MODCMD_READ_MULTI_COILS) || (recvAdu->pdu.funcCode==MODCMD_READ_MULTI_DISCRETE_INS))
               {
                  if (MAX_SENDBUF_SIZE-data->client[i].toSend>=sizeof(struct modtcp_adu))
                  {
                     struct modtcp_adu *sendAdu;
                     unsigned char     *c,*rData;

                     if (recvAdu->pdu.funcCode==MODCMD_READ_MULTI_COILS) c=getCoilMem();
                     else c=getDiscreteMem();
                     if (c)
                     {
                        int srcBit,trgBit,srcByte,trgByte=1,count;

                        srcByte=recvAdu->pdu.addrOffset/8;
                        srcBit=0x80>>(recvAdu->pdu.addrOffset % 8);
                        count=(recvAdu->pdu.data[0]<<8)|recvAdu->pdu.data[1];
                        trgBit=0x80;

                        sendAdu=(struct modtcp_adu*)&data->client[i].sendBuffer[data->client[i].toSend];
                        sendAdu->head.protId=htons(MODPROT_ID);
                        sendAdu->pdu.funcCode=recvAdu->pdu.funcCode;
                        sendAdu->head.length=htons((unsigned short)(3+((count+7)/8)));
                        sendAdu->head.transId=recvAdu->head.transId;
                        sendAdu->unitId=recvAdu->unitId;
                        if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==0)
                        {
                           swapByteorder(&sendAdu->head.transId);
                           swapByteorder(&sendAdu->head.protId);
                           swapByteorder(&sendAdu->head.length);
                        }
                        data->client[i].toSend+=(8+1+((count+7)/8)); // head plus funcCode plus data
                        rData=(unsigned char*)&sendAdu->pdu.addrOffset; // start with data at position of address offset, this field is not used on return
                        rData[0]=(unsigned char)((count+7)/8);
                        while ((count>0) && (trgByte<=255))
                        {
                           if ((c[srcByte] & srcBit)==0) rData[trgByte]&=~trgBit;
                           else rData[trgByte]|=trgBit;

                           if (srcBit==0x01)
                           {
                              srcBit=0x80;
                              srcByte++;
                           }
                           else srcBit=srcBit>>1;
                           if (trgBit==0x01)
                           {
                              trgBit=0x80;
                              trgByte++;
                           }
                           else trgBit=trgBit>>1;
                           count--;
                        }
                     }
                     else
                     {
                        // send out of memory error code
                     }
                  }
                  else
                  {
                     //not enough space in send buffer
                  }
               }

               else if (recvAdu->pdu.funcCode==MODCMD_WRITE_SINGLE_HOLDREG)
               {
                  unsigned short *w;

                  w=getRegMem();
                  if (w)
                  {
                     w[recvAdu->pdu.addrOffset]=(unsigned short)((recvAdu->pdu.data[0]<<8)|recvAdu->pdu.data[1]);
                     checkForChangedRegData(recvAdu->pdu.addrOffset,1);
                  }
                  else
                  {
                     // send out of memory error code
                  }
               }
               else if (recvAdu->pdu.funcCode==MODCMD_WRITE_MULTI_HOLDREGS)
               {
                  unsigned short *w;
                  unsigned int    count;

                  w=getRegMem();
                  if (w)
                  {
                     int srcByte=3,trgByte;

                     trgByte=recvAdu->pdu.addrOffset;
                     count=(recvAdu->pdu.data[0]<<8)|recvAdu->pdu.data[1];
                     if (count>100) count=100;
                     while ((count>0) && (srcByte<=255))
                     {
                        w[trgByte]=(unsigned short)(recvAdu->pdu.data[srcByte]<<8);
                        srcByte++;
                        w[trgByte]|=recvAdu->pdu.data[srcByte];
                        srcByte++;
                        trgByte++;
                        count--;
                     }
                     checkForChangedRegData(recvAdu->pdu.addrOffset,(recvAdu->pdu.data[0]<<8)|recvAdu->pdu.data[1]);
                  }
                  else
                  {
                     // send out of memory error code
                  }
               }
               else if ((recvAdu->pdu.funcCode==MODCMD_READ_MULTI_HOLDREGS) || (recvAdu->pdu.funcCode==MODCMD_READ_MULTI_INPUTREGS))
               {
                  if (MAX_SENDBUF_SIZE-data->client[i].toSend>=sizeof(struct modtcp_adu))
                  {
                     struct modtcp_adu *sendAdu;
                     unsigned short    *w;
                     unsigned char     *rData;

                     if (recvAdu->pdu.funcCode==MODCMD_READ_MULTI_HOLDREGS) w=getRegMem();
                     else w=getInputMem();
                     if (w)
                     {
                        int srcByte,trgByte=1,count;

                        srcByte=recvAdu->pdu.addrOffset;
                        count=(recvAdu->pdu.data[0]<<8)|recvAdu->pdu.data[1];
                        if (count>125) count=125;

                        sendAdu=(struct modtcp_adu*)&data->client[i].sendBuffer[data->client[i].toSend];
                        sendAdu->head.protId=htons(MODPROT_ID);
                        sendAdu->pdu.funcCode=recvAdu->pdu.funcCode;
                        sendAdu->head.length=htons((unsigned short)(3+(count*2)));
                        sendAdu->head.transId=recvAdu->head.transId;
                        sendAdu->unitId=recvAdu->unitId;
                        if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==0)
                        {
                           swapByteorder(&sendAdu->head.transId);
                           swapByteorder(&sendAdu->head.protId);
                           swapByteorder(&sendAdu->head.length);
                        }
                        data->client[i].toSend+=(8+1+(count*2)); // head plus funcCode plus data
                        rData=(unsigned char*)&sendAdu->pdu.addrOffset; // start with data at position of address offset, this field is not used on return
                        rData[0]=(unsigned char)count;
                        while (count>0)
                        {
                           rData[trgByte]=(unsigned char)((w[srcByte] & 0xFF00)>>8);
                           trgByte++;
                           rData[trgByte]=(unsigned char)((w[srcByte] & 0x00FF));
                           trgByte++;
                           srcByte++;
                           count--;
                        }
                     }
                     else
                     {
                        // send out of memory error code
                     }
                  }
                  else
                  {
                     //not enough space in send buffer
                  }

               }
               if (length>0)
               {
                  if (length+sizeof(struct modtcp_adu_head)>sizeof(struct modtcp_adu)) data->client[i].received =0;
                  else
                  {
                     memmove(data->client[i].recvBuffer,data->client[i].recvBuffer+length,sizeof(struct modtcp_adu)-(length+sizeof(struct modtcp_adu_head)));
                     data->client[i].received-=(length+sizeof(struct modtcp_adu_head));
                     if (data->client[i].received<0)
                     {
                        assert(0);
                        data->client[i].received=0;
                     }
                  }
               }
            }
         }
         if (data->client[i].toSend) dataReceived=true;
   	}
   	if (!dataReceived) oapc_util_thread_sleep(50);
   }
   for (i=0; i<MAX_INCOMING_CONNECTIONS; i++)
   {
      if (data->client[i].sock>-1) closesocket(data->client[i].sock);
   }
   data->running=true;
   return NULL;
}


/**
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API unsigned long oapc_init(void* instanceData)
{
   int       i;
   struct instData *data;
#ifdef ENV_WINDOWS
   WSADATA wsa;

   WSAStartup(MAKEWORD(2,0),&wsa);
#endif
   data=(struct instData*)instanceData;

   if (sockCnt==0)
   {
      data->sock=oapc_tcp_listen_on_port(data->config.port,data->config.ip);
      if (data->sock<=0)
      {
         oapc_exit(instanceData);
         return OAPC_ERROR_RESOURCE;
      }
      oapc_tcp_set_blocking(data->sock,false);
      serverInstanceData=data;
   }
   for (i=0; i<MAX_INCOMING_CONNECTIONS; i++)
   {
      memset(&data->client[i],0,sizeof(struct client_dataset));
      data->client[i].sock=-1;
   }
   data->running=true;
   if (sockCnt==0)
   {
      if (!oapc_thread_create(recvLoop,serverInstanceData))
      {
         oapc_exit(instanceData);
         return OAPC_ERROR_NO_MEMORY;
      }
   }
   sockCnt++;

   return OAPC_OK;
}



/**
This function is called before the application unloads everything, it has to be used to deinitialize
everything and to release used resources.
*/
OAPC_EXT_API unsigned long oapc_exit(void* instanceData)
{
   struct instData *data;
   int              i;

   data=(struct instData*)instanceData;

   data->running=false;
   i=0;
   while ((data->sock>0) && (!data->running) && (i<500))
   {
      oapc_util_thread_sleep(10);
      i++;
   }
//   oapc_tcp_closesocket(data->sock);
   if (coilMem)     free(coilMem);
   if (regMem)      free(regMem);
   if (discreteMem) free(discreteMem);
   if (inputMem)    free(inputMem);
#ifdef ENV_WINDOWS
   WSACleanup();
#endif
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
   unsigned int     lValue;
   unsigned char   *c;
   unsigned short  *w;

   data=(struct instData*)instanceData;

   lValue=(unsigned int)value;
   if ((data->config.modConfig[input].mode==MOD_RW_COILS) || (data->config.modConfig[input].mode==MOD_R_DISCRETE_IN))
   {
      if (data->config.modConfig[input].mode==MOD_RW_COILS) c=getCoilMem();
      else c=getDiscreteMem();
      if (c)
      {
         unsigned int srcBit=0x80000000,trgBit,trgByte,count;

         trgByte=data->config.modConfig[input].addrOffset/8;
         trgBit=0x80>>(data->config.modConfig[input].addrOffset % 8);
         count=32;
         while (count>0)
         {
            if ((lValue & srcBit)==0) c[trgByte]&=~trgBit;
            else c[trgByte]|=trgBit;

            srcBit=srcBit>>1;
            if (trgBit==0x01)
            {
               trgBit=0x80;
               trgByte++;
            }
            else trgBit=trgBit>>1;
            count--;
         }
      }
      else return OAPC_ERROR_NO_MEMORY;
   }
   if ((data->config.modConfig[input].mode==MOD_RW_HOLDREG) || (data->config.modConfig[input].mode==MOD_R_INPUTREG))
   {
      if (data->config.modConfig[input].mode==MOD_RW_HOLDREG) w=getRegMem();
      else w=getInputMem();
      if (w) w[data->config.modConfig[input].addrOffset]=(unsigned short)(lValue & 0xFFFF);
      else return OAPC_ERROR_NO_MEMORY;
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

   if (data->config.modConfig[output].mode==MOD_UNUSED) return OAPC_ERROR_NO_SUCH_IO; // check for valid and configured IO
   *value=(float)data->modData[output].rData;
   return OAPC_OK;
}

