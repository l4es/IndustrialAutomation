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

#ifndef ENV_WINDOWSCE
 #include <sys/types.h>
#else
 #include "Winsock2.h"
 #include "time_ce.h"
#endif

#ifdef ENV_LINUX
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <termios.h>
#include <unistd.h>
#endif

#ifdef ENV_QNX
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <termios.h>
#include <unistd.h>
#include <sched.h>
#endif

#ifdef ENV_WINDOWS
#include <windows.h>
#endif

#include "oapc_libio.h"
#include "liboapc.h"
#include "../libio_modbus_tcp_master/modbus_common.h"
#include "modbus_rtu_io.h"


/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\
<oapc-config>\
<flowimage>%s</flowimage>\
<dialogue>\
 <general>\
  <param>\
   <name>port</name>\
   <text>Interface</text>\
   <type>string</type>\
   <default>%s</default>\
   <min>4</min>\
   <max>12</max>\
  </param>\
  <param>\
   <name>brate</name>\
   <text>Data Rate</text>\
   <unit>bps</unit>\
   <type>option</type>\
   <value>110</value>\
   <value>300</value>\
   <value>1200</value>\
   <value>2400</value>\
   <value>4800</value>\
   <value>9600</value>\
   <value>19200</value>\
   <value>38400</value>\
   <value>57600</value>\
   <value>115200</value>\
   <value>230400</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>databits</name>\
   <text>Data Bits</text>\
   <type>option</type>\
   <value>5</value>\
   <value>6</value>\
   <value>7</value>\
   <value>8</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>parity</name>\
   <text>Parity</text>\
   <type>option</type>\
   <value>None</value>\
   <value>Even</value>\
   <value>Odd</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>stopbits</name>\
   <text>Stop Bits</text>\
   <type>option</type>\
   <value>1</value>\
   <value>1.5</value>\
   <value>2</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>flowcontrol</name>\
   <text>Flow Control</text>\
   <type>option</type>\
   <value>None</value>\
   <value>Xon / Xoff</value>\
   <value>CTS / RTS</value>\
   <default>%d</default>\
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
    <text>Log Filename</text>""\
    <type>filesave</type>\
    <default>%s</default>\
    <min>0</min>\
    <max>%d</max>\
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
    <name>slaveaddr0</name>\n""\
    <text>IO0 slave address</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>1</min>\
    <max>274</max>\
   </param>\
   <param>\
    <name>addroffset0</name>\n""\
    <text>IO0 base address offset</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>65503</max>\
   </param>\
   <param text=\"IO0 r/w length\">\
    <name>rwLength0</name>\
    <type>integer</type>\
    <default>%d</default>\
    <min>1</min>\
    <max>61</max>\
   </param>\
   <param>\
    <name>mode1</name>\
    <text>IO1 Operation Mode</text>\
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
    <name>slaveaddr1</name>\
    <text>IO1 slave address</text>\
    <type>integer</type>\
    <default>%d</default>\
    <min>1</min>\
    <max>274</max>\
   </param>\
   <param text=\"IO1 base address offset\">\
    <name>addroffset1</name>\
    <type>integer</type>\
    <default>%d</default>\
    <min>0</min>\
    <max>65503</max>\
   </param>\
   <param text=\"IO1 r/w length\">\
    <name>rwLength1</name>\
    <type>integer</type>\
    <default>%d</default>\
    <min>1</min>\
    <max>61</max>\
   </param>\
  </stdpanel>\
  <helppanel>\
   <in0>0 - output data</in0>\
   <in1>SL - slave address</in1>\
   <in2>AD - address offset</in2>\
   <in4>1 - output data</in4>\
   <in5>SL - slave address</in5>\
   <in6>AD - address offset</in6>\
   <out0>0 - input data</out0>\
   <out4>1 - input data</out4>\
  </helppanel>\
 </dialogue>\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                  flowImage[]="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";
static char                  libname[]="Modbus Addressable RTU Master";
static char                  xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback  m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports
static struct libio_config   save_config;
static bool                  running=true;



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
   return OAPC_NUM_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|
          OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6;
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
   return OAPC_NUM_IO0|
          OAPC_NUM_IO4;
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
   int              i;

   data=(struct instData*)instanceData;
   for (i=0; i<MAX_NUM_IOS; i++)
    if (data->config.modConfig[i].mode==MOD_R_FLOAT_HOLDREG)
     if (data->config.rwLength[i]>31) data->config.rwLength[i]=31;

   sprintf(xmldescr,xmltempl,flowImage,
                             data->config.serialParams.port,data->config.serialParams.brate,
                             data->config.serialParams.databits,data->config.serialParams.parity,
                             data->config.serialParams.stopbits,data->config.serialParams.flowcontrol,
                             data->config.pollDelay,
                             ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER),
                             ((data->config.flags & MODBUS_TCP_FLAG_INVERT_DATABYTEORDER)==MODBUS_TCP_FLAG_INVERT_DATABYTEORDER),
                             ((data->config.flags & MODBUS_TCP_FLAG_LOG_COMMUNICATION)==MODBUS_TCP_FLAG_LOG_COMMUNICATION),data->config.logfile,MAX_FILENAME_SIZE,
                             data->config.modConfig[0].mode,data->config.modConfig[0].slaveAddr,data->config.modConfig[0].addrOffset,data->config.rwLength[0],
                             data->config.modConfig[1].mode,data->config.modConfig[1].slaveAddr,data->config.modConfig[1].addrOffset,data->config.rwLength[0]);
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

   if (strcmp(name,"port")==0)             strncpy(data->config.serialParams.port,value,MAX_TTY_SIZE);
   else if (strcmp(name,"brate")==0)       data->config.serialParams.brate=(unsigned short)atoi(value);
   else if (strcmp(name,"parity")==0)      data->config.serialParams.parity=(unsigned short)atoi(value);
   else if (strcmp(name,"databits")==0)    data->config.serialParams.databits=(unsigned short)atoi(value);
   else if (strcmp(name,"flowcontrol")==0) data->config.serialParams.flowcontrol=(unsigned short)atoi(value);
   else if (strcmp(name,"stopbits")==0)    data->config.serialParams.stopbits=(unsigned short)atoi(value);
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
   else if (strcmp(name,"logfile")==0)     strncpy(data->config.logfile,value,MAX_FILENAME_SIZE);
   else if (strcmp(name,"mode0")==0)       data->config.modConfig[0].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"slaveaddr0")==0)  data->config.modConfig[0].slaveAddr=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset0")==0) data->config.modConfig[0].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"rwLength0")==0)   data->config.rwLength[0]=(unsigned short)atoi(value);
   else if (strcmp(name,"mode1")==0)       data->config.modConfig[1].mode=(unsigned char)atoi(value);
   else if (strcmp(name,"slaveaddr1")==0)  data->config.modConfig[1].slaveAddr=(unsigned char)atoi(value);
   else if (strcmp(name,"addroffset1")==0) data->config.modConfig[1].addrOffset=(unsigned short)atoi(value);
   else if (strcmp(name,"rwLength1")==0)   data->config.rwLength[1]=(unsigned short)atoi(value);
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
   strncpy(save_config.serialParams.port,data->config.serialParams.port,MAX_TTY_SIZE);
   save_config.serialParams.brate      =htons(data->config.serialParams.brate);
   save_config.serialParams.parity     =htons(data->config.serialParams.parity);
   save_config.serialParams.databits   =htons(data->config.serialParams.databits);
   save_config.serialParams.flowcontrol=htons(data->config.serialParams.flowcontrol);
   save_config.serialParams.stopbits   =htons(data->config.serialParams.stopbits);
   save_config.pollDelay=htons(data->config.pollDelay);
   for (i=0; i<2; i++)
   {
      save_config.modConfig[i].mode=data->config.modConfig[i].mode;
      save_config.modConfig[i].slaveAddr=data->config.modConfig[i].slaveAddr;
      save_config.modConfig[i].addrOffset=htons(data->config.modConfig[i].addrOffset);
      // new since version 5
      save_config.rwLength[i]=htons(data->config.rwLength[i]);
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
   strncpy(data->config.serialParams.port,save_config.serialParams.port,MAX_TTY_SIZE);
   data->config.serialParams.brate      =ntohs(save_config.serialParams.brate);
   data->config.serialParams.parity     =ntohs(save_config.serialParams.parity);
   data->config.serialParams.databits   =ntohs(save_config.serialParams.databits);
   data->config.serialParams.flowcontrol=ntohs(save_config.serialParams.flowcontrol);
   data->config.serialParams.stopbits   =ntohs(save_config.serialParams.stopbits);
   data->config.pollDelay=ntohs(save_config.pollDelay);
   for (i=0; i<2; i++)
   {
      data->config.modConfig[i].mode=save_config.modConfig[i].mode;
      data->config.modConfig[i].slaveAddr=save_config.modConfig[i].slaveAddr;
      data->config.modConfig[i].addrOffset=ntohs(save_config.modConfig[i].addrOffset);
   }
   if (data->config.version>=2)
   {
      data->config.flags=ntohl(save_config.flags);
      strncpy(data->config.logfile,save_config.logfile,MAX_FILENAME_SIZE);
   }
   if (data->config.version<5)
   {
      for (i=0; i<2; i++) data->config.rwLength[i]=1;
   }
   else
   {
      for (i=0; i<2; i++) data->config.rwLength[i]=ntohs(save_config.rwLength[i]);
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
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API void* oapc_create_instance2(unsigned long /*flags*/)
{
   struct instData *data;

   data=(struct instData*)malloc(sizeof(struct instData));
   if (!data) return NULL;

   memset(data,0,sizeof(struct instData));
   
#ifdef ENV_WINDOWS
   strcpy(data->config.serialParams.port,"COM1");
 #else
  #ifdef ENV_LINUX
   strcpy(data->config.serialParams.port,"/dev/ttyS0");
  #else
   #ifdef ENV_QNX
    strcpy(data->config.serialParams.port,"/dev/ser1");
   #else
    #error Not implemented!
   #endif
 #endif
#endif

   data->config.serialParams.brate=6;
   data->config.serialParams.databits=4;
   data->config.serialParams.flowcontrol=1;
   data->config.serialParams.parity=1;
   data->config.serialParams.stopbits=1;
   data->config.pollDelay=200;
   data->config.modConfig[0].mode=MOD_RW_COILS;
   data->config.modConfig[1].mode=MOD_UNUSED;
   data->config.rwLength[0]=1;
   data->config.rwLength[1]=1;
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



static void *recvLoop(void *arg)
{
   struct instData *data;
   int              i,sleepDelay;
   unsigned int     ioFlag;

   data=(struct instData*)arg;
   while (running)
   {
   	for (i=0; i<2; i++)
   	{
   		switch (i)
   		{
   			case 0:
   			   ioFlag=OAPC_NUM_IO0;
   			   break;
   			default:
   			   ioFlag=OAPC_NUM_IO4;
   			   break;
   		}
   		switch (data->useConfig[i].mode)
   		{
   			case MOD_UNUSED:
   			   data->modData[i].writeNewData=0;
   			   data->modData[i].readNewData=0;
   			   break;
            case MOD_RW_COILS:
               if (data->modData[i].writeNewData)
               {
                  writeMultiCoils(data,data->useConfig[i].slaveAddr,data->useConfig[i].addrOffset,data->modData[i].wData,data->config.rwLength[i]);
                  data->modData[i].writeNewData=0;
               }
               if (readDisCoils(data,data->useConfig[i].slaveAddr,data->useConfig[i].addrOffset,data->modData[i].lData,MODCMD_READ_MULTI_COILS,data->config.rwLength[i])==OAPC_OK)
               {
                  if ((!data->modData[i].noInitialCall) || (memcmp(data->modData[i].rData,data->modData[i].lData,data->config.rwLength[i]*sizeof(unsigned int))!=0))
                  {
                     data->modData[i].noInitialCall=true;
                     memcpy(data->modData[i].rData,data->modData[i].lData,data->config.rwLength[i]*sizeof(unsigned int));
                     for (data->modData[i].rLengthCtr=0; data->modData[i].rLengthCtr<data->config.rwLength[i]; data->modData[i].rLengthCtr++)
                      m_oapc_io_callback(ioFlag,data->m_callbackID);
                  }
               }
               break;
            case MOD_RW_HOLDREG:
               if (data->modData[i].writeNewData)
               {
                  writeHoldReg(data,data->useConfig[i].slaveAddr,data->useConfig[i].addrOffset,data->modData[i].wData,data->config.rwLength[i]);
                  data->modData[i].writeNewData=0;
               }
               if (readReg(data,data->useConfig[i].slaveAddr,data->useConfig[i].addrOffset,&data->modData[i].rData[0],MODCMD_READ_MULTI_HOLDREGS,data->config.rwLength[i])==OAPC_OK)
               {
                  if ((!data->modData[i].noInitialCall) || (arraycmp(data->modData[i].sData,data->modData[i].rData,data->config.rwLength[i]*sizeof(unsigned int))!=0))
                  {
                     data->modData[i].noInitialCall=true;
                     arraycpy(data->modData[i].sData,data->modData[i].rData,data->config.rwLength[i]*sizeof(unsigned int));
                     for (data->modData[i].rLengthCtr=0; data->modData[i].rLengthCtr<data->config.rwLength[i]; data->modData[i].rLengthCtr++)
                      m_oapc_io_callback(ioFlag,data->m_callbackID);
                  }
               }
               break;
            case MOD_R_FLOAT_HOLDREG:
               if (readFloatReg(data,data->config.modConfig[i].slaveAddr,data->config.modConfig[i].addrOffset,data->modData[i].fData,MODCMD_READ_MULTI_HOLDREGS,data->config.rwLength[i])==OAPC_OK)
               {
                  if ((!data->modData[i].noInitialCall) || (memcmp(data->modData[i].frData,data->modData[i].fData,data->config.rwLength[i]*sizeof(double))!=0))
                  {
                     data->modData[i].noInitialCall=true;
                     memcpy(data->modData[i].frData,data->modData[i].fData,data->config.rwLength[i]*sizeof(double));
                     for (data->modData[i].rLengthCtr=0; data->modData[i].rLengthCtr<data->config.rwLength[i]; data->modData[i].rLengthCtr++)
                      m_oapc_io_callback(ioFlag,data->m_callbackID);
                  }
               }
               break;
            case MOD_R_DISCRETE_IN:
   			   data->modData[i].writeNewData=0;
               if (readDisCoils(data,data->useConfig[i].slaveAddr,data->useConfig[i].addrOffset,data->modData[i].lData,MODCMD_READ_MULTI_DISCRETE_INS,data->config.rwLength[i])==OAPC_OK)
               {
                  if ((!data->modData[i].noInitialCall) || (memcmp(data->modData[i].rData,data->modData[i].lData,data->config.rwLength[i]*sizeof(unsigned int))!=0))
                  {
                     data->modData[i].noInitialCall=true;
                     memcpy(data->modData[i].rData,data->modData[i].lData,data->config.rwLength[i]*sizeof(unsigned int));
                     for (data->modData[i].rLengthCtr=0; data->modData[i].rLengthCtr<data->config.rwLength[i]; data->modData[i].rLengthCtr++)
               		  m_oapc_io_callback(ioFlag,data->m_callbackID);
               	  }
               }
               break;
            case MOD_R_INPUTREG:
   			   data->modData[i].writeNewData=0;
               if (readReg(data,data->useConfig[i].slaveAddr,data->useConfig[i].addrOffset,&data->modData[i].rData[0],MODCMD_READ_MULTI_INPUTREGS,data->config.rwLength[i])==OAPC_OK)
               {
                  if ((!data->modData[i].noInitialCall) || (arraycmp(data->modData[i].sData,data->modData[i].rData,data->config.rwLength[i]*sizeof(unsigned int))!=0))
                  {
                     data->modData[i].noInitialCall=true;
                     arraycpy(data->modData[i].sData,data->modData[i].rData,data->config.rwLength[i]*sizeof(unsigned int));
                     for (data->modData[i].rLengthCtr=0; data->modData[i].rLengthCtr<data->config.rwLength[i]; data->modData[i].rLengthCtr++)
                      m_oapc_io_callback(ioFlag,data->m_callbackID);
               	  }
               }
               break;
   		 }
   	  }
      oapc_thread_sleep(0);
      sleepDelay=0;
      while ((!data->modData[0].writeNewData) && (!data->modData[1].writeNewData) &&
             (sleepDelay<data->config.pollDelay))
      sleepDelay+=oapc_thread_sleep(10);
   }
   return NULL;
}



OAPC_EXT_API unsigned long oapc_init(void* instanceData)
{
   int                ret,i;
   struct instData   *data;

   data=(struct instData*)instanceData;

   if (((data->config.flags & MODBUS_TCP_FLAG_LOG_COMMUNICATION)==MODBUS_TCP_FLAG_LOG_COMMUNICATION) &&
       (strlen(data->config.logfile)>0))
   {
       data->FHandle=fopen(data->config.logfile,"a");
       if (!data->FHandle) return OAPC_ERROR_CREATE_FILE_FAILED;
   }
   else data->FHandle=NULL;
   writeLog(data,"=====================================================");
   writeLog(data,"opening serial port");
   ret=oapc_serial_port_open(&data->config.serialParams,&data->m_fd);
   if (ret!=OAPC_OK) return ret;

   for (i=0; i<2; i++)
   {
      data->useConfig[i]=data->config.modConfig[i];
      switch (data->useConfig[i].mode)
      {
         case MOD_UNUSED:
            break;
         case MOD_RW_COILS:
            data->modData[i].wData=(unsigned int*)malloc(data->config.rwLength[i]*sizeof(unsigned int));
            if (!data->modData[i].wData) return OAPC_ERROR_NO_MEMORY;
            data->modData[i].rData=(unsigned int*)malloc(data->config.rwLength[i]*sizeof(unsigned int));
            if (!data->modData[i].rData) return OAPC_ERROR_NO_MEMORY;

            data->modData[i].lData=(unsigned int*)malloc(data->config.rwLength[i]*sizeof(unsigned int));
            if (!data->modData[i].lData) return OAPC_ERROR_NO_MEMORY;
            break;
         case MOD_RW_HOLDREG:
            data->modData[i].wData=(unsigned int*)malloc(data->config.rwLength[i]*sizeof(unsigned int));
            if (!data->modData[i].wData) return OAPC_ERROR_NO_MEMORY;
            data->modData[i].rData=(unsigned int*)malloc(data->config.rwLength[i]*sizeof(unsigned int));
            if (!data->modData[i].rData) return OAPC_ERROR_NO_MEMORY;

            data->modData[i].sData=(unsigned short*)malloc(data->config.rwLength[i]*sizeof(unsigned short));
            if (!data->modData[i].sData) return OAPC_ERROR_NO_MEMORY;
            break;
         case MOD_R_FLOAT_HOLDREG:
            if (data->config.rwLength[i]>31) data->config.rwLength[i]=31;
            data->modData[i].frData=(double*)malloc(data->config.rwLength[i]*sizeof(double));
            if (!data->modData[i].frData) return OAPC_ERROR_NO_MEMORY;

            data->modData[i].fData=(double*)malloc(data->config.rwLength[i]*sizeof(double));
            if (!data->modData[i].fData) return OAPC_ERROR_NO_MEMORY;
            break;
         case MOD_R_DISCRETE_IN:
            data->modData[i].rData=(unsigned int*)malloc(data->config.rwLength[i]*sizeof(unsigned int));
            if (!data->modData[i].rData) return OAPC_ERROR_NO_MEMORY;

            data->modData[i].lData=(unsigned int*)malloc(data->config.rwLength[i]*sizeof(unsigned int));
            if (!data->modData[i].lData) return OAPC_ERROR_NO_MEMORY;
            break;
         case MOD_R_INPUTREG:
            data->modData[i].rData=(unsigned int*)malloc(data->config.rwLength[i]*sizeof(unsigned int));
            if (!data->modData[i].rData) return OAPC_ERROR_NO_MEMORY;

            data->modData[i].sData=(unsigned short*)malloc(data->config.rwLength[i]*sizeof(unsigned short));
            if (!data->modData[i].sData) return OAPC_ERROR_NO_MEMORY;
            break;
      }
   }
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
   int              i;

   data=(struct instData*)instanceData;
   running=false;
   writeLog(data,"closing serial port");
   if (data->m_fd) oapc_serial_port_close(&data->m_fd);
   data->m_fd=0;
   if (data->FHandle) fclose(data->FHandle);
   data->FHandle=NULL;
   for (i=0; i<2; i++)
   {
      if (data->modData[i].rData) free(data->modData[i].rData);
      data->modData[i].rData=NULL;
      if (data->modData[i].wData) free(data->modData[i].wData);
      data->modData[i].wData=NULL;
      if (data->modData[i].frData) free(data->modData[i].frData);
      data->modData[i].frData=NULL;

      if (data->modData[i].lData) free(data->modData[i].lData);
      data->modData[i].lData=NULL;
      if (data->modData[i].sData) free(data->modData[i].sData);
      data->modData[i].sData=NULL;
      if (data->modData[i].fData) free(data->modData[i].fData);
      data->modData[i].fData=NULL;
   }
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
   int              useInput;

   data=(struct instData*)instanceData;

   if (input<=3) useInput=0;
   else if ((input>=4) && (input<=6)) useInput=1;
   else return OAPC_ERROR_NO_SUCH_IO;
   if (data->config.modConfig[useInput].mode==MOD_UNUSED) return OAPC_ERROR_NO_SUCH_IO; // check for valid and configured IO
   if (data->config.modConfig[useInput].mode==MOD_R_INPUTREG) return OAPC_ERROR_INVALID_INPUT;
   if (data->config.modConfig[useInput].mode==MOD_R_FLOAT_HOLDREG) return OAPC_ERROR_INVALID_INPUT;
   if ((input==0) || (input==4))
   {
      data->modData[useInput].wData[data->modData[useInput].wLengthCtr]=(unsigned int)value;
      data->modData[useInput].wLengthCtr++;
      if (data->modData[useInput].wLengthCtr>=data->config.rwLength[useInput])
      {
         data->modData[useInput].wLengthCtr=0;
         data->modData[useInput].writeNewData=1;
      }
   }
   else if ((input==1) ||(input==5))
   {
      //TODO_ value range check
      data->useConfig[useInput].slaveAddr=(unsigned char)value;
   }
   else if ((input==2) ||(input==6))
   {
      //TODO_ value range check
      data->modData[useInput].readNewData=1;
      data->useConfig[useInput].addrOffset=(unsigned short)value;
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
   int              useOutput;

   data=(struct instData*)instanceData;

   if (output==0) useOutput=0;
   else if (output==4) useOutput=1;
   else return OAPC_ERROR_NO_SUCH_IO;
   if (data->config.modConfig[useOutput].mode==MOD_UNUSED) return OAPC_ERROR_NO_SUCH_IO; // check for valid and configured IO
   else if (data->config.modConfig[useOutput].mode==MOD_R_FLOAT_HOLDREG) *value=data->modData[useOutput].frData[data->modData[useOutput].rLengthCtr];
   else *value=(float)data->modData[useOutput].rData[data->modData[useOutput].rLengthCtr];
   return OAPC_OK;
}

