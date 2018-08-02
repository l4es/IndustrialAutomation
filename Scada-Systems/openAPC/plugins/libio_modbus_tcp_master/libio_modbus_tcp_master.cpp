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
#include <stdarg.h>

#ifndef ENV_WINDOWSCE
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

#ifdef ENV_QNX

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#define MAX_XML_SIZE         12000

#include "oapc_libio.h"
#include "liboapc.h"
#include "modbus_common.h"
#include "modbus_tcp_common.h"



struct libio_config
{
   unsigned short    version,length;
   char              ip[MAX_IP_SIZE];
   unsigned short    port;
	unsigned short    pollDelay;
	struct mod_config modConfig[MAX_NUM_IOS];
	unsigned int      flags;
	char              logfile[MAX_FILENAME_SIZE+4];
	//version 4
	unsigned char     modConfigUnitID[MAX_NUM_IOS];
};

struct instData
{
   struct libio_config config;
   struct mod_data     modData[MAX_NUM_IOS];
   int                 sock;
   int                 m_callbackID;
   FILE               *FHandle;
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
    <name>polldelay</name>\
    <text>Slave Poll Delay</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>50</min>\
    <max>10000</max>\
    <unit>msec</unit>\
   </param>\
   <param>\
    <name>addrbyteorder</name>\
    <text>Invert address byte order</text>\
    <type>checkbox</type>\
    <default>%d</default>\
   </param>\
   <param>\
    <name>databyteorder</name>\
    <text>Invert data byte order</text>\
    <type>checkbox</type>\
    <default>%d</default>\
   </param>\
   <param>\
    <name>logcom</name>\
    <text>Log Communication Data</text>\
    <type>checkbox</type>\
    <default>%d</default>\
   </param>\
   <param>\
    <name>logfile</name>\
    <text>Log Filename</text>\
    <type>filesave</type>\
    <default>%s</default>\
    <min>0</min>\
    <max>%d</max>\
   </param>\
   <param>\
    <name>1xcompat</name>\
    <text>OpenAPC 1.x compatibility mode</text>\
    <type>checkbox</type>\
    <default>%d</default>\
   </param>\
  </general>\
  <stdpanel>\
   <name>Access Parameters</name>\
   <param>\
    <name>mode0</name>\
    <text>IO0 Operation Mode</text>\
    <type>option</type>\
    <value>Disabled</value>\
    <value>Read/Write Coils</value>\
    <value>Read/Write Holding Register</value>\
    <value>Read Discrete Input</value>\
    <value>Read Input Register</value>\
    <value>Read IEEE754 Holding Register</value>\
    <default>%d</default>\
   </param>\
   <param>\
    <name>addroffset0</name>\
    <text>IO0 base address offset</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>65503</max>\
   </param>\
   <param>\
    <name>unitid0</name>\
    <text>Unit ID</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>255</max>""\
   </param>\
   <param>\
    <name>mode1</name>\
    <text>IO1 Operation Mode</text>\
    <type>option</type>\n\
    <value>Disabled</value>\
    <value>Read/Write Coils</value>\
    <value>Read/Write Holding Register</value>\
    <value>Read Discrete Input</value>\
    <value>Read Input Register</value>\
    <value>Read IEEE754 Holding Register</value>\
    <default>%d</default>\
   </param>\
   <param>\
    <name>addroffset1</name>\
    <text>IO1 base address offset</text>\n""\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>65503</max>\
   </param>\
   <param>\
    <name>unitid1</name>\
    <text>Unit ID</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>255</max>\
   </param>\
   <param>\
    <name>mode2</name>\
    <text>IO2 Operation Mode</text>\
    <type>option</type>\
    <value>Disabled</value>\
    <value>Read/Write Coils</value>\
    <value>Read/Write Holding Register</value>\
    <value>Read Discrete Input</value>\
    <value>Read Input Register</value>\
    <value>Read IEEE754 Holding Register</value>\
    <default>%d</default>\
   </param>\
   <param>\n""\
    <name>addroffset2</name>\n""\
    <text>IO2 base address offset</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>65503</max>\
   </param>\
   <param>\
    <name>unitid2</name>\
    <text>Unit ID</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>255</max>\
   </param>\
   <param>\
    <name>mode3</name>\
    <text>IO3 Operation Mode</text>\
    <type>option</type>\
    <value>Disabled</value>\
    <value>Read/Write Coils</value>\
    <value>Read/Write Holding Register</value>\
    <value>Read Discrete Input</value>\
    <value>Read Input Register</value>\
    <value>Read IEEE754 Holding Register</value>\
    <default>%d</default>\
   </param>\
   <param>\
    <name>addroffset3</name>\
    <text>IO3 base address offset</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>65503</max>\
   </param>\
   <param>\
    <name>unitid3</name>\
    <text>Unit ID</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>255</max>\
   </param>\
  </stdpanel>\
  <stdpanel>\
   <name>Access Parameters</name>\
   <param>\
    <name>mode4</name>\
    <text>IO4 Operation Mode</text>\
    <type>option</type>\
    <value>Disabled</value>\
    <value>Read/Write Coils</value>\
    <value>Read/Write Holding Register</value>\
    <value>Read Discrete Input</value>\
    <value>Read Input Register</value>\
    <value>Read IEEE754 Holding Register</value>\
    <default>%d</default>\
   </param>\
   <param>\
    <name>addroffset4</name>\
    <text>IO4 base address offset</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>65503</max>\
   </param>\
   <param>\
    <name>unitid4</name>\
    <text>Unit ID</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>255</max>\
   </param>\
   <param>\
    <name>mode5</name>\
    <text>IO5 Operation Mode</text>\
    <type>option</type>\
    <value>Disabled</value>\
    <value>Read/Write Coils</value>\
    <value>Read/Write Holding Register</value>\
    <value>Read Discrete Input</value>\
    <value>Read Input Register</value>\
    <value>Read IEEE754 Holding Register</value>""\
    <default>%d</default>\
   </param>\
   <param>\n""\
    <name>addroffset5</name>\
    <text>IO5 base address offset</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>65503</max>\
   </param>\
   <param>\
    <name>unitid5</name>\
    <text>Unit ID</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>255</max>\
   </param>\
   <param>\
    <name>mode6</name>\
    <text>IO6 Operation Mode</text>\
    <type>option</type>\
    <value>Disabled</value>\
    <value>Read/Write Coils</value>\
    <value>Read/Write Holding Register</value>\
    <value>Read Discrete Input</value>\
    <value>Read Input Register</value>\
    <value>Read IEEE754 Holding Register</value>\
    <default>%d</default>\
   </param>\
   <param>\n""\
    <name>addroffset6</name>\
    <text>IO6 base address offset</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>65503</max>\
   </param>\
   <param>\
    <name>unitid6</name>\
    <text>Unit ID</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>255</max>\
   </param>\
   <param>\
    <name>mode7</name>\
    <text>IO7 Operation Mode</text>\
    <type>option</type>\
    <value>Disabled</value>\
    <value>Read/Write Coils</value>\
    <value>Read/Write Holding Register</value>\
    <value>Read Discrete Input</value>\
    <value>Read Input Register</value>\
    <value>Read IEEE754 Holding Register</value>\
    <default>%d</default>\
   </param>\
   <param>\
    <name>addroffset7</name>\
    <text>IO7 base address offset</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>65503</max>\
   </param>\
   <param>\
    <name>unitid7</name>\
    <text>Unit ID</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>255</max>\
   </param>\
  </stdpanel>\
  <helppanel>\
   <in0>SET - output data</in0>\
   <in1>...</in1>\
   <out0>GET - input data</out0>\
   <out1>...</out1>\
  </helppanel>\
 </dialogue>\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUEAgRqh1EE/vwEKbRsgtmyxZ7Axe88VMji6daMpm+cpeQcRgRbcNA8Zhvy+OoNQLyEgoR8ktx0kkzEwsTR1fRFY8axtexXdzfi5fmgtIxyfts8Rsy80pwwWAj09fxsiFwMMbSEm2zU3ryctnR8gtx8mGRFaiVifUqYneKovJi8zq+kvOxMWNRsdtxUX9QkSry8yqzGy/SkrOgoUAjp7PyEitw0XhR8nlT8/vzW2/Q9SsycsoS4vOxUdtTU3syRleQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB4BFAfAAACPklEQVR42u2YAXeaMBCAg1SqhGo3RSjZrHag1lG3VunOrJv//1/tQgwFHvimr3F7G/c05CDmu7tcIgkhZxQAINetVuta1DQqEjUgqJOBfKhLkSjBVmZoU/YoZCsztCl7FLIzM3QpCjV4NUOXolCQM0OTkqH0SzZWOaSabZlNeaWmWalVVTN1gVLTQuekUoFita4ZKQewNpAEirbn+sj/RCgEqh5Vtz/PWDWoQ53l/i70ogZK/jvUheM4T2BjeQFjOwyC0N7AKBGyewRLVoZwYxhTiG5v76HXNeZzo30Cqm+apguXWPbHDkslHieywixbXvnQoPSq90zpFGY0lfbxaSFQk/FEoLac88uYMzZMsIYAtrM5j1cxY4sM1aF0PUOcUbW8HE52gTI/i6L/KCg+Qiz0yoGQswC9WmwWjNnvFeqO0tnyw7du+0SUu0dxtvWRJ1E7lqK4COA286on4/e8PAnlCNgEUdjpVnrFUxQP1FhZmVdws05ZM/i9t78iyhIhnMgAKq94FkAbLMaS17GKup/u2pR+PAm1+WL2N4gaIiVeYcQwLdLA8cTmLHZWjO0idOQKv99FWsw9gaoNHKkPoL8w3wnU+IFh9Hia7Dwdoxc7vcPZaCmHaN1RyT49HvXDdf2N+LgOwCh8cILEh6cgDIMwscAKhIQvAF8Nz/OinwCdaO55xj0cj3rbFb16Kr8NqrQgVS9QDeqvQJ0vLZqXs0NIrduDUh9atwfk39geFCaBLqU5jWlOY/74aczZ5Be51ksCujNepwAAAABJRU5ErkJggg==";
static char                 libname[]="Modbus TCP Master";
static char                 xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports
static struct libio_config  save_config;
static bool                 running=true;


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
                                       data->config.port,data->config.pollDelay,
                                       ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER),
                                       ((data->config.flags & MODBUS_TCP_FLAG_INVERT_DATABYTEORDER)==MODBUS_TCP_FLAG_INVERT_DATABYTEORDER),
                                       ((data->config.flags & MODBUS_TCP_FLAG_LOG_COMMUNICATION)==MODBUS_TCP_FLAG_LOG_COMMUNICATION),data->config.logfile,MAX_FILENAME_SIZE,
                                       ((data->config.flags & MODBUS_TCP_FLAG_1x_COMPATIBILITY)==MODBUS_TCP_FLAG_1x_COMPATIBILITY),
                                       data->config.modConfig[0].mode,data->config.modConfig[0].addrOffset,data->config.modConfigUnitID[0],
                                       data->config.modConfig[1].mode,data->config.modConfig[1].addrOffset,data->config.modConfigUnitID[1],
                                       data->config.modConfig[2].mode,data->config.modConfig[2].addrOffset,data->config.modConfigUnitID[2],
                                       data->config.modConfig[3].mode,data->config.modConfig[3].addrOffset,data->config.modConfigUnitID[3],
                                       data->config.modConfig[4].mode,data->config.modConfig[4].addrOffset,data->config.modConfigUnitID[4],
                                       data->config.modConfig[5].mode,data->config.modConfig[5].addrOffset,data->config.modConfigUnitID[5],
                                       data->config.modConfig[6].mode,data->config.modConfig[6].addrOffset,data->config.modConfigUnitID[6],
                                       data->config.modConfig[7].mode,data->config.modConfig[7].addrOffset,data->config.modConfigUnitID[7]);
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

   if (strcmp(name,"ip")==0)               strncpy(data->config.ip,value,MAX_IP_SIZE);
   else if (strcmp(name,"port")==0)        data->config.port=(unsigned short)atoi(value);
   else if (strcmp(name,"polldelay")==0)   data->config.pollDelay=(unsigned short)atoi(value);
   else if (strcmp(name,"addrbyteorder")==0)
   {
      if (atoi(value)!=0) data->config.flags|=MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER;
      else data->config.flags&=~MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER;
   }
   else if (strcmp(name,"databyteorder")==0)
   {
      if (atoi(value)!=0) data->config.flags|=MODBUS_TCP_FLAG_INVERT_DATABYTEORDER;
      else data->config.flags&=~MODBUS_TCP_FLAG_INVERT_DATABYTEORDER;
   }
   else if (strcmp(name,"logcom")==0)
   {
      if (atoi(value)!=0) data->config.flags|=MODBUS_TCP_FLAG_LOG_COMMUNICATION;
      else data->config.flags&=~MODBUS_TCP_FLAG_LOG_COMMUNICATION;
   }
   else if (strcmp(name,"1xcompat")==0)
   {
      if (atoi(value)!=0) data->config.flags|=MODBUS_TCP_FLAG_1x_COMPATIBILITY;
      else data->config.flags&=~MODBUS_TCP_FLAG_1x_COMPATIBILITY;
   }
   else if (strcmp(name,"logfile")==0)          strncpy(data->config.logfile,value,MAX_FILENAME_SIZE);
   else if (strcmp(name,"mode0")==0)       data->config.modConfig[0].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset0")==0) data->config.modConfig[0].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"unitid0")==0)     data->config.modConfigUnitID[0]=(unsigned char)atoi(value);
   else if (strcmp(name,"mode1")==0)       data->config.modConfig[1].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset1")==0) data->config.modConfig[1].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"unitid1")==0)     data->config.modConfigUnitID[1]=(unsigned char)atoi(value);
   else if (strcmp(name,"mode2")==0)       data->config.modConfig[2].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset2")==0) data->config.modConfig[2].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"unitid2")==0)     data->config.modConfigUnitID[2]=(unsigned char)atoi(value);
   else if (strcmp(name,"mode3")==0)       data->config.modConfig[3].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset3")==0) data->config.modConfig[3].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"unitid3")==0)     data->config.modConfigUnitID[3]=(unsigned char)atoi(value);
   else if (strcmp(name,"mode4")==0)       data->config.modConfig[4].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset4")==0) data->config.modConfig[4].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"unitid4")==0)     data->config.modConfigUnitID[4]=(unsigned char)atoi(value);
   else if (strcmp(name,"mode5")==0)       data->config.modConfig[5].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset5")==0) data->config.modConfig[5].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"unitid5")==0)     data->config.modConfigUnitID[5]=(unsigned char)atoi(value);
   else if (strcmp(name,"mode6")==0)       data->config.modConfig[6].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset6")==0) data->config.modConfig[6].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"unitid6")==0)     data->config.modConfigUnitID[6]=(unsigned char)atoi(value);
   else if (strcmp(name,"mode7")==0)       data->config.modConfig[7].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset7")==0) data->config.modConfig[7].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"unitid7")==0)     data->config.modConfigUnitID[7]=(unsigned char)atoi(value);
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
   save_config.version  =htons(5);
   save_config.length   =htons((unsigned short)*length);
   strncpy(save_config.ip,data->config.ip,MAX_IP_SIZE);
   save_config.port     =htons(data->config.port);
   save_config.pollDelay=htons(data->config.pollDelay);
   for (i=0; i<MAX_NUM_IOS; i++)
   {
   	save_config.modConfig[i].mode=data->config.modConfig[i].mode;
   	save_config.modConfig[i].addrOffset=htons(data->config.modConfig[i].addrOffset);
      save_config.modConfigUnitID[i]=data->config.modConfigUnitID[i];
   }
   // new since version 2 =============================================
   save_config.flags=htonl(data->config.flags);
   strncpy(save_config.logfile,data->config.logfile,MAX_FILENAME_SIZE);
   // =================================================================
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
   data->config.version  =ntohs(save_config.version);
   data->config.length   =ntohs(save_config.length);
   strncpy(data->config.ip,save_config.ip,MAX_IP_SIZE);
   data->config.port     =ntohs(save_config.port);
   data->config.pollDelay=ntohs(save_config.pollDelay);
   for (i=0; i<MAX_NUM_IOS; i++)
   {
   	data->config.modConfig[i].mode=save_config.modConfig[i].mode;
   	data->config.modConfig[i].addrOffset=ntohs(save_config.modConfig[i].addrOffset);
   }
   if (data->config.version>=2)
   {
      data->config.flags=ntohl(save_config.flags);
      strncpy(data->config.logfile,save_config.logfile,MAX_FILENAME_SIZE);
   }
   if (data->config.version<3)
   {
      data->config.flags|=MODBUS_TCP_FLAG_1x_COMPATIBILITY;
   }
   if (data->config.version>=4)
   {
      for (i=0; i<MAX_NUM_IOS; i++)
       data->config.modConfigUnitID[i]=save_config.modConfigUnitID[i];
   }
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
   strcpy(data->config.ip,"192.168.1.1");
   data->config.port=502;
   data->config.pollDelay=200;
   data->config.modConfig[0].mode=MOD_RW_COILS;
   data->config.modConfig[1].mode=MOD_UNUSED;
   data->config.modConfig[2].mode=MOD_UNUSED;
   data->config.modConfig[3].mode=MOD_UNUSED;
   data->config.modConfig[4].mode=MOD_UNUSED;
   data->config.modConfig[5].mode=MOD_UNUSED;
   data->config.modConfig[6].mode=MOD_UNUSED;
   data->config.modConfig[7].mode=MOD_UNUSED;
   data->config.flags=MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER|MODBUS_TCP_FLAG_INVERT_DATABYTEORDER;
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



int writeMultiCoils(struct instData *data,unsigned short addrOffset,unsigned char unitID,unsigned int value)
{
	struct modtcp_adu adu,recvAdu;
	unsigned char     recvBuffer[6];
	int               len,sendLen;
#ifdef ENV_WINDOWS
	int               errno;
#endif
	
   adu.head.transId=htons(0);
   adu.head.protId=htons(MODPROT_ID);
   sendLen=11;
   adu.head.length=htons((unsigned short)sendLen);
   adu.unitId=unitID;
   adu.pdu.funcCode=MODCMD_WRITE_MULTI_COILS;
   adu.pdu.addrOffset=htons(addrOffset);
   if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==0)
   {
      swapByteorder(&adu.head.transId);
      swapByteorder(&adu.head.protId);
      swapByteorder(&adu.head.length);
      swapByteorder(&adu.pdu.addrOffset);
   }
	
	adu.pdu.data[0]=0;
	adu.pdu.data[1]=32;
	
	adu.pdu.data[2]=4;
	
   if ((data->config.flags & MODBUS_TCP_FLAG_1x_COMPATIBILITY)==0)
   {
   	   adu.pdu.data[6]=(unsigned char)(value & 0x000000FF);
	   adu.pdu.data[5]=(unsigned char)((value & 0x0000FF00)>>8);
   	   adu.pdu.data[4]=(unsigned char)((value & 0x00FF0000)>>16);
	   adu.pdu.data[3]=(unsigned char)((value & 0xFF000000)>>24);
   }
   else
   {
      adu.pdu.data[3]=(unsigned char)(value & 0x000000FF);
      adu.pdu.data[4]=(unsigned char)((value & 0x0000FF00)>>8);
      adu.pdu.data[5]=(unsigned char)((value & 0x00FF0000)>>16);
      adu.pdu.data[6]=(unsigned char)((value & 0xFF000000)>>24);
   }
   writeLog(data,"writeMultiCoils %d %d",addrOffset,value);
	if (oapc_tcp_send(data->sock,(char*)&adu,sendLen+6,500)<sendLen+6)
	{
#ifdef ENV_WINDOWS
      errno=GetLastError();
#endif
	   writeLog(data,"ERR: sending of data failed (%d)",errno);
	   oapc_tcp_closesocket(data->sock);
	   data->sock=0;
		return -1;
	}
	len=oapc_tcp_recv(data->sock,(char*)&recvAdu,8,NULL,3500);
	if (len<8)
	{
#ifdef ENV_WINDOWS
      errno=GetLastError();
#endif
      writeLog(data,"ERR: receiving of response failed (%d bytes, %d)",len,errno);
		return -1;
	}
	if (recvAdu.pdu.funcCode==adu.pdu.funcCode)
	{
   	if (oapc_tcp_recv(data->sock,(char*)recvBuffer,4,NULL,500)<4)
	   {
#ifdef ENV_WINDOWS
         errno=GetLastError();
#endif
         writeLog(data,"ERR: receiving of response data failed (%d)",errno);
		   return -1;
   	}
	}
	else
	{
   	oapc_tcp_recv(data->sock,(char*)recvBuffer,1,NULL,500); // error code
      writeLog(data,"ERR: response error code %d",recvBuffer[0]);
	   return -1;
	}
	
	return 0;
}



int writeHoldReg(struct instData *data,unsigned short addrOffset,unsigned char unitID,unsigned short value)
{
	struct modtcp_adu adu,recvAdu;
	unsigned char     recvBuffer[6];
   int               len,sendLen;
#ifdef ENV_WINDOWS
   int               errno;
#endif
	
	adu.head.transId=htons(0);
	adu.head.protId=htons(MODPROT_ID);
   sendLen=6;
	adu.head.length=htons(6);
	adu.unitId=unitID;
	adu.pdu.funcCode=MODCMD_WRITE_SINGLE_HOLDREG;
	adu.pdu.addrOffset=htons(addrOffset);
	if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==0)
   {
      swapByteorder(&adu.head.transId);
      swapByteorder(&adu.head.protId);
      swapByteorder(&adu.head.length);
      swapByteorder(&adu.pdu.addrOffset);
   }
	
   if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_DATABYTEORDER)==0)
   {
      adu.pdu.data[0]=(unsigned char)((value & 0xFF00)>>8);
      adu.pdu.data[1]=(unsigned char)(value & 0x00FF);
   }
   else
   {
      adu.pdu.data[0]=(unsigned char)(value & 0x00FF);
      adu.pdu.data[1]=(unsigned char)((value & 0xFF00)>>8);
   }
	
   writeLog(data,"writeHoldReg %d %d",addrOffset,value);
   if (oapc_tcp_send(data->sock,(char*)&adu,sendLen+6,500)<sendLen+6)
   {
#ifdef ENV_WINDOWS
      errno=GetLastError();
#endif
      writeLog(data,"ERR: sending of data failed (%d)",errno);
      oapc_tcp_closesocket(data->sock);
      data->sock=0;
		return -1;
	}
	len=oapc_tcp_recv(data->sock,(char*)&recvAdu,8,NULL,3500);
	if (len<8)
	{
#ifdef ENV_WINDOWS
      errno=GetLastError();
#endif
      writeLog(data,"ERR: receiving of response failed (%d bytes, %d)",len,errno);
		return -1;
	}
	if (recvAdu.pdu.funcCode==adu.pdu.funcCode)
	{
   	if (oapc_tcp_recv(data->sock,(char*)recvBuffer,4,NULL,500)<4)
	   {
#ifdef ENV_WINDOWS
         errno=GetLastError();
#endif
         writeLog(data,"ERR: receiving of response data failed (%d)",errno);
		   return -1;
   	}
	}
	else
	{
   	oapc_tcp_recv(data->sock,(char*)recvBuffer,1,NULL,500); // error code
      writeLog(data,"ERR: response error code %d",recvBuffer[0]);
	   return -1;
	}
	
	return 0;
}



int readDisCoils(struct instData *data,unsigned short addrOffset,unsigned char unitID,unsigned int *value,unsigned char funcCode)
{
   struct modtcp_adu adu,recvAdu;
   unsigned char     recvBuffer[6];
   int               len,sendLen;
#ifdef ENV_WINDOWS
   int               errno;
#endif

   adu.head.transId=htons(0);
   adu.head.protId=htons(MODPROT_ID);
   sendLen=6;
   adu.head.length=htons(6);
   adu.unitId=unitID;
   adu.pdu.funcCode=funcCode;
   adu.pdu.addrOffset=htons(addrOffset);
   if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==0)
   {
      swapByteorder(&adu.head.transId);
      swapByteorder(&adu.head.protId);
      swapByteorder(&adu.head.length);
      swapByteorder(&adu.pdu.addrOffset);
   }
	
   adu.pdu.data[0]=0;
   adu.pdu.data[1]=32;
	
   writeLog(data,"readDisCoils %d %d",addrOffset,funcCode);
   if (oapc_tcp_send(data->sock,(char*)&adu,sendLen+6,500)<sendLen+6)
   {
#ifdef ENV_WINDOWS
      errno=GetLastError();
#endif
      writeLog(data,"ERR: sending of request failed (%d)",errno);
      oapc_tcp_closesocket(data->sock);
      data->sock=0;
		return OAPC_ERROR_SEND_DATA;
	}
	len=oapc_tcp_recv(data->sock,(char*)&recvAdu,8,NULL,3500);
	if (len<8)
	{
#ifdef ENV_WINDOWS
      errno=GetLastError();
#endif
      writeLog(data,"ERR: receiving of response failed (%d bytes, %d)",len,errno);
		return OAPC_ERROR_RECV_DATA;
	}
	if (recvAdu.pdu.funcCode==adu.pdu.funcCode)
	{
   	if (oapc_tcp_recv(data->sock,(char*)recvBuffer,5,NULL,500)<5)
	   {
#ifdef ENV_WINDOWS
         errno=GetLastError();
#endif
         writeLog(data,"ERR: receiving of response data failed (%d)",errno);
		   return OAPC_ERROR_RECV_DATA;
   	}
   	if ((data->config.flags & MODBUS_TCP_FLAG_1x_COMPATIBILITY)==0)
   	 *value=(recvBuffer[4]|(recvBuffer[3]<<8)|(recvBuffer[2]<<16)|(recvBuffer[1]<<24));
   	else
       *value=(recvBuffer[1]|(recvBuffer[2]<<8)|(recvBuffer[3]<<16)|(recvBuffer[4]<<24));
   }
   else
   {
      oapc_tcp_recv(data->sock,(char*)recvBuffer,1,NULL,500); // error code
      writeLog(data,"ERR: response error code %d",recvBuffer[0]);
      return OAPC_ERROR_RECV_DATA;
   }
	
   return OAPC_OK;
}



int readReg(struct instData *data,unsigned short addrOffset,unsigned char unitID,unsigned short *value,unsigned char funcCode)
{
   struct modtcp_adu adu,recvAdu;
   unsigned char     recvBuffer[6];
   int               len,sendLen;
#ifdef ENV_WINDOWS
   int               errno;
#endif
	
   adu.head.transId=htons(0);
   adu.head.protId=htons(MODPROT_ID);
   sendLen=6;
   adu.head.length=htons(6);
   adu.unitId=unitID;
   adu.pdu.funcCode=funcCode;
   adu.pdu.addrOffset=htons(addrOffset);
   if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==0)
   {
      swapByteorder(&adu.head.transId);
      swapByteorder(&adu.head.protId);
      swapByteorder(&adu.head.length);
      swapByteorder(&adu.pdu.addrOffset);
   }

   adu.pdu.data[0]=0;
   adu.pdu.data[1]=1;
	
   writeLog(data,"readReg %d %d",addrOffset,funcCode);
   if (oapc_tcp_send(data->sock,(char*)&adu,sendLen+6,500)<sendLen+6)
   {
#ifdef ENV_WINDOWS
      errno=GetLastError();
#endif
      writeLog(data,"ERR: sending of request failed (%d)",errno);
      oapc_tcp_closesocket(data->sock);
      data->sock=0;
      return OAPC_ERROR_SEND_DATA;
   }
   len=oapc_tcp_recv(data->sock,(char*)&recvAdu,8,NULL,3500);
   if (len<8)
   {
#ifdef ENV_WINDOWS
      errno=GetLastError();
#endif
      writeLog(data,"ERR: receiving of response failed (%d bytes, %d)",len,errno);
      return OAPC_ERROR_RECV_DATA;
   }
   if (recvAdu.pdu.funcCode==adu.pdu.funcCode)
   {
      if (oapc_tcp_recv(data->sock,(char*)recvBuffer,3,NULL,500)<3)
      {
#ifdef ENV_WINDOWS
         errno=GetLastError();
#endif
         writeLog(data,"ERR: receiving of response data failed (%d)",errno);
		 return OAPC_ERROR_RECV_DATA;
   	   }
       if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_DATABYTEORDER)==0)
        *value=(unsigned short)(recvBuffer[2]|(recvBuffer[1]<<8));
       else
        *value=(unsigned short)(recvBuffer[1]|(recvBuffer[2]<<8));
	}
	else
	{
       oapc_tcp_recv(data->sock,(char*)recvBuffer,1,NULL,500);
       writeLog(data,"ERR: response error code %d",recvBuffer[0]);
	   return OAPC_ERROR_RECV_DATA;
	}
	
	return OAPC_OK;
}



int readFloatReg(struct instData *data,unsigned short addrOffset,unsigned char unitID,double *value,unsigned char funcCode)
{
   struct modtcp_adu adu,recvAdu;
   unsigned char     recvBuffer[6];
   int               len,sendLen;
#ifdef ENV_WINDOWS
   int               errno;
#endif

   adu.head.transId=htons(0);
   adu.head.protId=htons(MODPROT_ID);
   sendLen=6;
   adu.head.length=htons(6);
   adu.unitId=unitID;
   adu.pdu.funcCode=funcCode;
   adu.pdu.addrOffset=htons(addrOffset);
   if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==0)
   {
      swapByteorder(&adu.head.transId);
      swapByteorder(&adu.head.protId);
      swapByteorder(&adu.head.length);
      swapByteorder(&adu.pdu.addrOffset);
   }

   adu.pdu.data[0]=0;
   adu.pdu.data[1]=2;

   writeLog(data,"readReg %d %d",addrOffset,funcCode);
   if (oapc_tcp_send(data->sock,(char*)&adu,sendLen+6,500)<sendLen+6)
   {
#ifdef ENV_WINDOWS
      errno=GetLastError();
#endif
      writeLog(data,"ERR: sending of request failed (%d)",errno);
      oapc_tcp_closesocket(data->sock);
      data->sock=0;
      return OAPC_ERROR_SEND_DATA;
   }
   len=oapc_tcp_recv(data->sock,(char*)&recvAdu,8,NULL,3500);
   if (len<8)
   {
#ifdef ENV_WINDOWS
      errno=GetLastError();
#endif
      writeLog(data,"ERR: receiving of response failed (%d bytes, %d)",len,errno);
      return OAPC_ERROR_RECV_DATA;
   }
   if (recvAdu.pdu.funcCode==adu.pdu.funcCode)
   {
      unsigned int  iVal,exp,mant;
      unsigned char sign;

      if (oapc_tcp_recv(data->sock,(char*)recvBuffer,5,NULL,500)<5)
      {
#ifdef ENV_WINDOWS
         errno=GetLastError();
#endif
         writeLog(data,"ERR: receiving of response data failed (%d)",errno);
         return OAPC_ERROR_RECV_DATA;
      }

      iVal=(recvBuffer[1]<<24)|(recvBuffer[2]<<16)|(recvBuffer[3]<<8)|(recvBuffer[4]);
      if (iVal& 0x80000000) sign=1;
      else sign=0;
      exp=(iVal & 0x7F800000)>>23;
      mant=(iVal & 0x007FFFFF);
      *value=pow(mant,(double)exp);
      if (sign) *value=*value*-1.0;
   }
   else
   {
      oapc_tcp_recv(data->sock,(char*)recvBuffer,1,NULL,500);
      writeLog(data,"ERR: response error code %d",recvBuffer[0]);
      return OAPC_ERROR_RECV_DATA;
   }

   return OAPC_OK;
}



void *recvLoop(void *arg)
{
   struct instData *data;
   int              i,len,sleepDelay;
   unsigned int     lData,ioFlag;
   unsigned short   sData;
   double           fData;
   char             tmpBuf[100];

   data=(struct instData*)arg;
   while (running)
   {
   	for (i=0; i<MAX_NUM_IOS; i++)
   	{
   		switch (i)
   		{
   			case 0:
   			   ioFlag=OAPC_NUM_IO0;
   			   break;
   			case 1:
   			   ioFlag=OAPC_NUM_IO1;
   			   break;
   			case 2:
   			   ioFlag=OAPC_NUM_IO2;
   			   break;
   			case 3:
   			   ioFlag=OAPC_NUM_IO3;
   			   break;
   			case 4:
   			   ioFlag=OAPC_NUM_IO4;
   			   break;
   			case 5:
   			   ioFlag=OAPC_NUM_IO5;
   			   break;
   			case 6:
   			   ioFlag=OAPC_NUM_IO6;
   			   break;
   			default:
   			   ioFlag=OAPC_NUM_IO7;
   			   break;
   		}
   		if (data->sock<=0)
   		{
            writeLog(data,"re-connecting to %s:%d",data->config.ip,data->config.port);
            data->sock=oapc_tcp_connect_to(data->config.ip,data->config.port);
   		}
   		if (data->sock>=0)
   		{
            len=oapc_tcp_recv(data->sock,tmpBuf,100,NULL,1);
            if (len>0)
            {
               writeLog(data,"WARNING: receiving %d bytes of unexpected data (0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X)",
                        len,tmpBuf[0],tmpBuf[1],tmpBuf[2],tmpBuf[3],tmpBuf[4],tmpBuf[5],tmpBuf[6],tmpBuf[7],tmpBuf[8],tmpBuf[9]);
            }
            switch (data->config.modConfig[i].mode)
            {
               case MOD_UNUSED:
                  data->modData[i].writeNewData=0;
                  break;
               case MOD_RW_COILS:
                  if (data->modData[i].writeNewData)
                  {
                     writeMultiCoils(data,data->config.modConfig[i].addrOffset,data->config.modConfigUnitID[i],data->modData[i].wData);
                     data->modData[i].writeNewData=0;
                  }
                  if (readDisCoils(data,data->config.modConfig[i].addrOffset,data->config.modConfigUnitID[i],&lData,MODCMD_READ_MULTI_COILS)==OAPC_OK)
                  {
                     if (lData!=data->modData[i].rData)
                     {
                        data->modData[i].rData=lData;
                        m_oapc_io_callback(ioFlag,data->m_callbackID);
                     }
                  }
                  break;
               case MOD_RW_HOLDREG:
                  if (data->modData[i].writeNewData)
                  {
                     writeHoldReg(data,data->config.modConfig[i].addrOffset,data->config.modConfigUnitID[i],(unsigned short)data->modData[i].wData);
                     data->modData[i].writeNewData=0;
                  }
                  if (readReg(data,data->config.modConfig[i].addrOffset,data->config.modConfigUnitID[i],&sData,MODCMD_READ_MULTI_HOLDREGS)==OAPC_OK)
                  {
                     if (sData!=data->modData[i].rData)
                     {
                        data->modData[i].rData=sData;
                        m_oapc_io_callback(ioFlag,data->m_callbackID);
                     }
                  }
                  break;
               case MOD_R_FLOAT_HOLDREG:
                  if (readFloatReg(data,data->config.modConfig[i].addrOffset,data->config.modConfigUnitID[i],&fData,MODCMD_READ_MULTI_HOLDREGS)==OAPC_OK)
                  {
                     if (fData!=data->modData[i].frData)
                     {
                        data->modData[i].frData=fData;
                        m_oapc_io_callback(ioFlag,data->m_callbackID);
                     }
                  }
                  break;
               case MOD_R_DISCRETE_IN:
                  data->modData[i].writeNewData=0;
                  if (readDisCoils(data,data->config.modConfig[i].addrOffset,data->config.modConfigUnitID[i],&lData,MODCMD_READ_MULTI_DISCRETE_INS)==OAPC_OK)
                  {
                     if (lData!=data->modData[i].rData)
                     {
                        data->modData[i].rData=lData;
                        m_oapc_io_callback(ioFlag,data->m_callbackID);
                     }
                  }
                  break;
               case MOD_R_INPUTREG:
                  data->modData[i].writeNewData=0;
                  if (readReg(data,data->config.modConfig[i].addrOffset,data->config.modConfigUnitID[i],&sData,MODCMD_READ_MULTI_INPUTREGS)==OAPC_OK)
                  {
                     if (sData!=data->modData[i].rData)
                     {
                        data->modData[i].rData=sData;
                        m_oapc_io_callback(ioFlag,data->m_callbackID);
                     }
                  }
                  break;
               default:
                  break;
            }
         }
         else oapc_thread_sleep(250);
   	}
      oapc_thread_sleep(0);
      sleepDelay=0;
      while ((!data->modData[0].writeNewData) && (!data->modData[1].writeNewData) &&
             (!data->modData[2].writeNewData) && (!data->modData[3].writeNewData) &&
             (!data->modData[4].writeNewData) && (!data->modData[5].writeNewData) &&
             (!data->modData[6].writeNewData) && (!data->modData[7].writeNewData) &&
             (sleepDelay<data->config.pollDelay))
       sleepDelay+=oapc_thread_sleep(10);
   }
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

   if (((data->config.flags & MODBUS_TCP_FLAG_LOG_COMMUNICATION)==MODBUS_TCP_FLAG_LOG_COMMUNICATION) &&
       (strlen(data->config.logfile)>0))
   {
       data->FHandle=fopen(data->config.logfile,"a");
       if (!data->FHandle)
       {
          oapc_exit(instanceData);
          return OAPC_ERROR_CREATE_FILE_FAILED;
       }
   }//-.,
   else data->FHandle=NULL;
   writeLog(data,"=====================================================");
   writeLog(data,"connecting to %s:%d",data->config.ip,data->config.port);
   data->sock=oapc_tcp_connect_to(data->config.ip,data->config.port);
   if (data->sock<=0)
   {
      oapc_exit(instanceData);
      return OAPC_ERROR_CONNECTION;
   }

   // set socket to non-blocking in order to avoid deadlocks when sending fails
   oapc_tcp_set_blocking(data->sock,false);

   writeLog(data,"creating receiver-thread");
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

   data=(struct instData*)instanceData;

   running=false;
   writeLog(data,"closing communication socket");
   oapc_tcp_closesocket(data->sock);
#ifdef ENV_WINDOWS
   WSACleanup();
#endif
   if (data->FHandle) fclose(data->FHandle);
   data->FHandle=NULL;
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

   if (data->config.modConfig[input].mode==MOD_UNUSED) return OAPC_ERROR_NO_SUCH_IO; // check for valid and configured IO
   if (data->config.modConfig[input].mode==MOD_R_DISCRETE_IN) return OAPC_ERROR_INVALID_INPUT;
   if (data->config.modConfig[input].mode==MOD_R_INPUTREG) return OAPC_ERROR_INVALID_INPUT;
   if (data->config.modConfig[input].mode==MOD_R_FLOAT_HOLDREG) return OAPC_ERROR_INVALID_INPUT;
   data->modData[input].wData=(unsigned short)value;
   data->modData[input].writeNewData=1;
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
   else if (data->config.modConfig[output].mode==MOD_R_FLOAT_HOLDREG) *value=data->modData[output].frData;
   else *value=(float)data->modData[output].rData;
   return OAPC_OK;
}

