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
#include <ctype.h>

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
 #include <sys/socket.h>
 #include <netinet/in.h>
#endif

#ifdef ENV_WINDOWS
 #include <windows.h>
 #define snprintf _snprintf
#endif

#include <queue>
#include <string>

#include "oapc_libio.h"
#include "liboapc.h"

#define MAX_XML_SIZE           25000
#define MAX_CMD_SIZE             256

#define FLAG_HANDLE_POWER    0x0001
#define FLAG_HANDLE_WAVEFORM 0x0002

#ifdef _DEBUG
// #define TEST_MODE
#endif

struct libio_config
{
   unsigned short       version,length;
   struct serial_params serialParams;
   char                 m_devOpenCmd[MAX_CMD_SIZE+8],m_devCloseCmd[MAX_CMD_SIZE+8];
   char                 m_markReadyCmd[MAX_CMD_SIZE+8],m_endMarkReadyCmd[MAX_CMD_SIZE+8];
   char                 m_jobStartCmd[MAX_CMD_SIZE+8],m_jobEndCmd[MAX_CMD_SIZE+8];
   int                  m_devOpenDelay,m_jobStartDelay,m_markReadyDelay;
   unsigned int         m_flags;
};


struct instData
{
   struct libio_config                config;
   int                                m_callbackID;
   unsigned int                       m_prevWaveform,m_prevPower;
   bool                               m_running,m_busy;
   void                              *m_ctrlMutex,*m_signal;
   std::queue<struct oapc_bin_head*> *m_ctrlList;
#ifdef ENV_WINDOWS
   HANDLE                             m_hCommPort;
#else
   int                                m_hCommPort;
#endif
};


/** the xml data that define the behaviour and user interface of the plugin within the main application */
static const char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<oapc-config>\
<flowimage>%s</flowimage>\
<dialogue>\
 <general>\
  <param text=\"Interface\">\
   <name>port</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>4</min>\
   <max>12</max>\
  </param>\
  <param text=\"Data Rate\">\
   <name>brate</name>\
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
  <param text=\"Data Bits\">\
   <name>databits</name>\
   <type>option</type>\
   <value>5</value>\
   <value>6</value>\
   <value>7</value>\
   <value>8</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Parity\">\
   <name>parity</name>\
   <type>option</type>\
   <value>None</value>\
   <value>Even</value>\
   <value>Odd</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Stop Bits\">\
   <name>stopbits</name>\
   <type>option</type>\
   <value>1</value>\
   <value>1.5</value>\
   <value>2</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Flow Control\">\
   <name>flowcontrol</name>\
   <type>option</type>\
   <value>None</value>\
   <value>Xon / Xoff</value>\
   <value>CTS / RTS</value>\
   <default>%d</default>\
  </param>\
 </general>\
 <stdpanel text=\"Commands\">\
  <param text=\"Device Open Command\">\
   <name>devopencmd</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
  </param>\
  <param text=\"Device Open Delay\">\
   <name>devopendelay</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>0</min>\
   <max>%d</max>\
   <unit>msec</unit>\
  </param>\
  <param text=\"Ready for Marking Command\">\
   <name>markreadycmd</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
  </param>""\
  <param text=\"Ready for Marking Delay\">\
   <name>markreadydelay</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>0</min>\
   <max>%d</max>\
   <unit>msec</unit>\
  </param>\
  <param text=\"Job Start Command\">\
   <name>jobstartcmd</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
  </param>""\
  <param text=\"Job Start Delay\">\
   <name>jobstartdelay</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>0</min>\
   <max>%d</max>\
   <unit>msec</unit>\
  </param>\
  <param text=\"Control power during process\">\
   <name>power</name>\
   <type>checkbox</type>\
   <default>%d</default>\
  </param>\
  <param text=\"Control waveform during process\">\
   <name>waveform</name>\
   <type>checkbox</type>\
   <default>%d</default>\
  </param>\
  <param text=\"Job End Command\">\
   <name>jobendcmd</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
  </param>\
  <param text=\"End Ready for Marking Command\">\
   <name>endmarkreadycmd</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
  </param>\
  <param text=\"Device Close Command\">\
   <name>devclosecmd</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
  </param>\
 </stdpanel>\
 <helppanel>\
  <in3>L - Laser on/off</in3>\
  <in4>PWR - Laser power in %%</in4>\
  <in7>CTRL - Control data</in7>\
  <out6>BSY - Marking active</out6>\
 </helppanel>\
</dialogue>\
</oapc-config>";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                     flowImage[]="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";
static char                     libname[]="SPI(R) G4 Laser RS232";
static char                     xmldescr[MAX_XML_SIZE+1];
static struct libio_config      save_config;
static lib_oapc_io_callback     m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports


/** has to be called after create instance and load data */
OAPC_EXT_API unsigned long oapc_get_config_info_data(void* instanceData,struct config_info *fillStruct)
{
   struct config_info *cfgInfo;

   if (!fillStruct) return OAPC_ERROR;
   cfgInfo=(struct config_info*)fillStruct;
   memset(cfgInfo,0,sizeof(struct config_info));

   cfgInfo->version=OAPC_CONFIG_INFO_VERSION;
   cfgInfo->length=sizeof(struct config_info);
   cfgInfo->configType=OAPC_CONFIG_TYPE_LASERCONTROLLER;

   cfgInfo->laserController.version=OAPC_CONFIG_LASER_CONTROLLER_VERSION;
   cfgInfo->laserController.length=sizeof(struct config_laser_controller);
   cfgInfo->laserController.capabilities=OAPC_LC_HAS_POWER;
   return OAPC_OK;
}


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
          OAPC_FLOWCAT_LASER;
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
   return OAPC_DIGI_IO3|OAPC_NUM_IO4|
          OAPC_BIN_IO7;
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
   return OAPC_DIGI_IO6;
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

   snprintf(xmldescr,MAX_XML_SIZE,xmltempl,flowImage,
                                           data->config.serialParams.port,data->config.serialParams.brate,
                                           data->config.serialParams.databits,data->config.serialParams.parity,
                                           data->config.serialParams.stopbits,data->config.serialParams.flowcontrol,
                                           data->config.m_devOpenCmd,MAX_CMD_SIZE,data->config.m_devOpenDelay,5000,
                                           data->config.m_markReadyCmd,MAX_CMD_SIZE,data->config.m_markReadyDelay,5000,
                                           data->config.m_jobStartCmd,MAX_CMD_SIZE,data->config.m_jobStartDelay,5000,
                                           (data->config.m_flags & FLAG_HANDLE_POWER)==FLAG_HANDLE_POWER,
                                           (data->config.m_flags & FLAG_HANDLE_WAVEFORM)==FLAG_HANDLE_WAVEFORM,
                                           data->config.m_jobEndCmd,MAX_CMD_SIZE,
                                           data->config.m_endMarkReadyCmd,MAX_CMD_SIZE,
                                           data->config.m_devCloseCmd,MAX_CMD_SIZE
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

   if (strcmp(name,"port")==0)                 strncpy(data->config.serialParams.port,value,MAX_TTY_SIZE);
   else if (strcmp(name,"brate")==0)           data->config.serialParams.brate=(unsigned short)atoi(value);
   else if (strcmp(name,"parity")==0)          data->config.serialParams.parity=(unsigned short)atoi(value);
   else if (strcmp(name,"databits")==0)        data->config.serialParams.databits=(unsigned short)atoi(value);
   else if (strcmp(name,"flowcontrol")==0)     data->config.serialParams.flowcontrol=(unsigned short)atoi(value);
   else if (strcmp(name,"stopbits")==0)        data->config.serialParams.stopbits=(unsigned short)atoi(value);
   else if (strcmp(name,"devopencmd")==0)      strncpy(data->config.m_devOpenCmd,value,MAX_CMD_SIZE);
   else if (strcmp(name,"devclosecmd")==0)     strncpy(data->config.m_devCloseCmd,value,MAX_CMD_SIZE);
   else if (strcmp(name,"markreadycmd")==0)    strncpy(data->config.m_markReadyCmd,value,MAX_CMD_SIZE);
   else if (strcmp(name,"endmarkreadycmd")==0) strncpy(data->config.m_endMarkReadyCmd,value,MAX_CMD_SIZE);
   else if (strcmp(name,"jobstartcmd")==0)     strncpy(data->config.m_jobStartCmd,value,MAX_CMD_SIZE);
   else if (strcmp(name,"jobendcmd")==0)       strncpy(data->config.m_jobEndCmd,value,MAX_CMD_SIZE);
   else if (strcmp(name,"devopendelay")==0)    data->config.m_devOpenDelay=atoi(value);
   else if (strcmp(name,"markreadydelay")==0)  data->config.m_markReadyDelay=atoi(value);
   else if (strcmp(name,"jobstartdelay")==0)   data->config.m_jobStartDelay=atoi(value);
   else if (strcmp(name,"power")==0)
   {
      if (atoi(value)>0) data->config.m_flags|=FLAG_HANDLE_POWER;
      else data->config.m_flags&=~FLAG_HANDLE_POWER;
   }
   else if (strcmp(name,"waveform")==0)
   {
      if (atoi(value)>0) data->config.m_flags|=FLAG_HANDLE_WAVEFORM;
      else data->config.m_flags&=~FLAG_HANDLE_WAVEFORM;
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
   save_config.version                 =htons(3);
   save_config.length                  =htons((unsigned short)*length);
   strncpy(save_config.serialParams.port,data->config.serialParams.port,MAX_TTY_SIZE);
   save_config.serialParams.brate      =htons(data->config.serialParams.brate);
   save_config.serialParams.parity     =htons(data->config.serialParams.parity);
   save_config.serialParams.databits   =htons(data->config.serialParams.databits);
   save_config.serialParams.flowcontrol=htons(data->config.serialParams.flowcontrol);
   save_config.serialParams.stopbits   =htons(data->config.serialParams.stopbits);
   strncpy(save_config.m_devOpenCmd,data->config.m_devOpenCmd,MAX_CMD_SIZE);
   strncpy(save_config.m_devCloseCmd,data->config.m_devCloseCmd,MAX_CMD_SIZE);
   strncpy(save_config.m_markReadyCmd,data->config.m_markReadyCmd,MAX_CMD_SIZE);
   strncpy(save_config.m_endMarkReadyCmd,data->config.m_endMarkReadyCmd,MAX_CMD_SIZE);
   strncpy(save_config.m_jobStartCmd,data->config.m_jobStartCmd,MAX_CMD_SIZE);
   strncpy(save_config.m_jobEndCmd,data->config.m_jobEndCmd,MAX_CMD_SIZE);
   save_config.m_devOpenDelay   =htonl(data->config.m_devOpenDelay);
   save_config.m_markReadyDelay =htonl(data->config.m_markReadyDelay);
   save_config.m_jobStartDelay  =htonl(data->config.m_jobStartDelay);
   save_config.m_flags                 =htonl(data->config.m_flags);

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
   memset(&data->config,0,sizeof(struct libio_config));
   data->config.version                 =ntohs(save_config.version);
   data->config.length                  =ntohs(save_config.length);
   strncpy(data->config.serialParams.port,save_config.serialParams.port,MAX_TTY_SIZE);
   data->config.serialParams.brate      =ntohs(save_config.serialParams.brate);
   data->config.serialParams.parity     =ntohs(save_config.serialParams.parity);
   data->config.serialParams.databits   =ntohs(save_config.serialParams.databits);
   data->config.serialParams.flowcontrol=ntohs(save_config.serialParams.flowcontrol);
   data->config.serialParams.stopbits   =ntohs(save_config.serialParams.stopbits);
   strncpy(data->config.m_devOpenCmd,save_config.m_devOpenCmd,MAX_CMD_SIZE);
   strncpy(data->config.m_devCloseCmd,save_config.m_devCloseCmd,MAX_CMD_SIZE);
   strncpy(data->config.m_markReadyCmd,save_config.m_markReadyCmd,MAX_CMD_SIZE);
   strncpy(data->config.m_endMarkReadyCmd,save_config.m_endMarkReadyCmd,MAX_CMD_SIZE);
   strncpy(data->config.m_jobStartCmd,save_config.m_jobStartCmd,MAX_CMD_SIZE);
   strncpy(data->config.m_jobEndCmd,save_config.m_jobEndCmd,MAX_CMD_SIZE);
   data->config.m_flags                 =ntohl(save_config.m_flags);
   data->config.m_devOpenDelay=ntohl(save_config.m_devOpenDelay);
   data->config.m_jobStartDelay=ntohl(save_config.m_jobStartDelay);
   data->config.m_markReadyDelay=ntohl(save_config.m_markReadyDelay);
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
   data->config.serialParams.brate=7;
   data->config.serialParams.databits=4;
   data->config.serialParams.flowcontrol=1;
   data->config.serialParams.parity=1;
   data->config.serialParams.stopbits=1;
   data->config.m_flags=FLAG_HANDLE_POWER;

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


static void *listLoop(void *arg)
{
   #define MAXBUFSIZE 2000

   struct instData      *data;
   struct oapc_bin_head *bin=NULL;

   data=(struct instData*)arg;
   while (data->m_running)
   {
      oapc_thread_mutex_lock(data->m_ctrlMutex);
      if (!data->m_ctrlList->empty())
      {
         bin=data->m_ctrlList->front();
         data->m_ctrlList->pop();
         oapc_thread_mutex_unlock(data->m_ctrlMutex);

         if ((bin) && (bin->type==OAPC_BIN_TYPE_STRUCT)) switch (bin->subType)
         {
            case OAPC_BIN_SUBTYPE_STRUCT_MARKREADY:
            {
               data->m_busy=1;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               if (data->config.m_markReadyCmd[0])
               {
                  char cmdBuffer[20+1];

#ifndef TEST_MODE
                  oapc_serial_send(data->m_hCommPort,data->config.m_markReadyCmd,(int)strlen(data->config.m_markReadyCmd),500);
                  oapc_serial_recv(data->m_hCommPort,cmdBuffer,20,NULL,100);
#endif
                  if (data->config.m_markReadyDelay>=0)
                   oapc_thread_sleep(data->config.m_markReadyDelay);
               }
               data->m_busy=0;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               break;
            }
            case OAPC_BIN_SUBTYPE_STRUCT_ENDMARKREADY:
            {
               data->m_busy=1;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               if (data->config.m_endMarkReadyCmd[0])
               {
                  char cmdBuffer[20+1];

#ifndef TEST_MODE
                  oapc_serial_send(data->m_hCommPort,data->config.m_endMarkReadyCmd,(int)strlen(data->config.m_endMarkReadyCmd),500);
                  oapc_serial_recv(data->m_hCommPort,cmdBuffer,20,NULL,100);
#endif
               }
               data->m_busy=0;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               break;
            }
            case OAPC_BIN_SUBTYPE_STRUCT_JOBSTART:
            {
               data->m_busy=1;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               if (data->config.m_jobStartCmd[0])
               {
                  char cmdBuffer[20+1];

#ifndef TEST_MODE
                  oapc_serial_send(data->m_hCommPort,data->config.m_jobStartCmd,(int)strlen(data->config.m_jobStartCmd),500);
                  oapc_serial_recv(data->m_hCommPort,cmdBuffer,20,NULL,100);
#endif
                  if (data->config.m_jobStartDelay>=0)
                   oapc_thread_sleep(data->config.m_jobStartDelay);
               }
               data->m_busy=0;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               break;
            }
            case OAPC_BIN_SUBTYPE_STRUCT_JOBEND:
            {
               data->m_busy=1;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               if (data->config.m_jobEndCmd[0])
               {
                  char cmdBuffer[20+1];

#ifndef TEST_MODE
                  oapc_serial_send(data->m_hCommPort,data->config.m_jobEndCmd,(int)strlen(data->config.m_jobEndCmd),500);
                  oapc_serial_recv(data->m_hCommPort,cmdBuffer,20,NULL,100);
#endif
               }
               data->m_busy=0;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               break;
            }
            case OAPC_BIN_SUBTYPE_STRUCT_SLICESTART:
            {
               data->m_busy=1;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);

               data->m_busy=0;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               break;
            }
            case OAPC_BIN_SUBTYPE_STRUCT_SLICEEND:
            {
               data->m_busy=1;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);

               data->m_busy=0;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               break;
            }
            case OAPC_BIN_SUBTYPE_STRUCT_LASERCTRL:
            {
               data->m_busy=1;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);

               struct oapc_bin_struct_laserctrl *ctrl;

               ctrl=(struct oapc_bin_struct_laserctrl*)&bin->data;

               if ((data->config.m_flags & FLAG_HANDLE_WAVEFORM)==FLAG_HANDLE_WAVEFORM)
               {
                  ctrl->waveformNum=ntohl(ctrl->waveformNum);

                  if (ctrl->waveformNum!=data->m_prevWaveform)
                  {
                	 char cmdBuffer[20+1];

                	 data->m_prevWaveform=ctrl->waveformNum;
                	 snprintf(cmdBuffer,20,"SW %d\r\n",ctrl->waveformNum);
                     oapc_serial_send(data->m_hCommPort,cmdBuffer,(int)strlen(cmdBuffer),500);
                     oapc_serial_recv(data->m_hCommPort,cmdBuffer,20,NULL,100);
                  }
               }

               data->m_busy=0;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               break;
            }
            case OAPC_BIN_SUBTYPE_STRUCT_CTRL:
            {
               unsigned int validityFlags;

               data->m_busy=1;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);

               struct oapc_bin_struct_ctrl *ctrl;

               ctrl=(struct oapc_bin_struct_ctrl*)&bin->data;

               validityFlags=ntohl(ctrl->validityFlags);
               if (validityFlags & TOOLPARAM_VALIDFLAG_POWER)
               {
                   unsigned int power=ntohl(ctrl->power);

                   if ((data->config.m_flags & FLAG_HANDLE_POWER)==FLAG_HANDLE_POWER)
                   {
                      if (power!=data->m_prevPower)
                      {
                         char cmdBuffer[20+1];

                         data->m_prevPower=power;
                         snprintf(cmdBuffer,20,"SH %d\r\n",(unsigned int)OAPC_ROUND(power/100.0,0));
                         oapc_serial_send(data->m_hCommPort,cmdBuffer,(int)strlen(cmdBuffer),500);
                         oapc_serial_recv(data->m_hCommPort,cmdBuffer,20,NULL,100);
                      }
                   }

               }

               data->m_busy=0;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               break;
            }
            default:
               assert(0);
               break;
         }
         if (bin) free(bin);         
      }
      else
      {
         oapc_thread_mutex_unlock(data->m_ctrlMutex);
         oapc_thread_signal_wait(data->m_signal,500);
      }
   };
   data->m_running=true;
   return NULL;
}


static void fullReplace(std::string& source,std::string const& find,std::string const& replace)
{
   for(std::string::size_type i = 0; (i = source.find(find, i)) != std::string::npos;)
   {
      source.replace(i, find.length(), replace);
      i += replace.length() - find.length() + 1;
   }
}


static void convertCommand(char *cstr)
{
   std::string str;

   str=std::string(cstr);
   fullReplace(str,"[CR]","\r");
   fullReplace(str,"[LF]","\n");
   strncpy(cstr,str.c_str(),MAX_CMD_SIZE);
}


static void convertCommands(struct instData *data)
{
   convertCommand(data->config.m_devOpenCmd);
   convertCommand(data->config.m_devCloseCmd);
   convertCommand(data->config.m_markReadyCmd);
   convertCommand(data->config.m_endMarkReadyCmd);
   convertCommand(data->config.m_jobStartCmd);
   convertCommand(data->config.m_jobEndCmd);
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

#ifndef TEST_MODE
   ret=oapc_serial_port_open(&data->config.serialParams,&data->m_hCommPort);
   if (ret!=OAPC_OK) return ret;
#endif
   convertCommands(data);

   data->m_ctrlMutex=oapc_thread_mutex_create();
   if (!data->m_ctrlMutex) return OAPC_ERROR_NO_MEMORY;
   data->m_ctrlList=new std::queue<struct oapc_bin_head*>;
   if (!data->m_ctrlList) return OAPC_ERROR_NO_MEMORY;
   data->m_signal=oapc_thread_signal_create();
   if (!data->m_signal) return OAPC_ERROR_NO_MEMORY;

   if (data->config.m_devOpenCmd[0])
   {
#ifndef TEST_MODE
       oapc_serial_send(data->m_hCommPort,data->config.m_devOpenCmd,(int)strlen(data->config.m_devOpenCmd),500);
#endif
       if (data->config.m_devOpenDelay>=0)
        oapc_thread_sleep(data->config.m_devOpenDelay);
   }
   data->m_running=true;
   if (!oapc_thread_create(listLoop,data)) return OAPC_ERROR_NO_MEMORY;
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
   if (data->m_running)
   {
      data->m_running=false;
      oapc_thread_signal_send(data->m_signal);
      while ((!data->m_running) && (ctr<10)) // wait for thread to finish
      {
         oapc_thread_sleep(250);
         ctr++;
      }
   }
   data->m_running=false;

   if ((data->m_hCommPort) && (data->config.m_devCloseCmd[0]))
   {
#ifndef TEST_MODE
      oapc_serial_send(data->m_hCommPort,data->config.m_devCloseCmd,(int)strlen(data->config.m_devCloseCmd),500);
#endif
   }
#ifndef TEST_MODE
   oapc_serial_port_close(&data->m_hCommPort);
#endif

   if (data->m_signal) oapc_thread_signal_release(data->m_signal);
   data->m_signal=NULL;

   if (data->m_ctrlList) delete data->m_ctrlList;
   data->m_ctrlList=NULL;

   if (data->m_ctrlMutex) oapc_thread_mutex_release(data->m_ctrlMutex);
   data->m_ctrlMutex=NULL;

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
OAPC_EXT_API unsigned long  oapc_set_digi_value(void* instanceData,unsigned long input,unsigned char value)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   if (input==3)
   {
      struct oapc_bin_head        *bin;
      struct oapc_bin_struct_ctrl *ctrl;

      bin=oapc_util_alloc_bin_data(OAPC_BIN_TYPE_STRUCT,OAPC_BIN_SUBTYPE_STRUCT_CTRL,OAPC_COMPRESS_NONE,sizeof(struct oapc_bin_struct_ctrl));
      if (!bin) return OAPC_ERROR_NO_MEMORY;
      ctrl=(struct oapc_bin_struct_ctrl*)&bin->data;
      ctrl->validityFlags=htonl(TOOLPARAM_VALIDFLAG_ON);
      ctrl->on=value;
      oapc_thread_mutex_lock(data->m_ctrlMutex);
      data->m_ctrlList->push(bin);
      oapc_thread_mutex_unlock(data->m_ctrlMutex);
      oapc_thread_signal_send(data->m_signal);
   }
   else return OAPC_ERROR_NO_SUCH_IO;
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

   if (output!=6) return OAPC_ERROR_NO_SUCH_IO;
   data=(struct instData*)instanceData;

   *value=data->m_busy;
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
   int              power=0,frequency=0,validityFlags;

   data=(struct instData*)instanceData;
   if (input==4)
   {
      power=htonl((int)(value*1000.0));
      validityFlags=TOOLPARAM_VALIDFLAG_POWER;
   }
   else if (input==5)
   {
      frequency=htonl((int)value);
      validityFlags=TOOLPARAM_VALIDFLAG_FREQ;
   }
   else return OAPC_ERROR_NO_SUCH_IO; // check for valid IO

   struct oapc_bin_head        *bin;
   struct oapc_bin_struct_ctrl *ctrl;

   bin=oapc_util_alloc_bin_data(OAPC_BIN_TYPE_STRUCT,OAPC_BIN_SUBTYPE_STRUCT_CTRL,OAPC_COMPRESS_NONE,sizeof(struct oapc_bin_struct_ctrl));
   if (!bin) return OAPC_ERROR_NO_MEMORY;
   ctrl=(struct oapc_bin_struct_ctrl*)&bin->data;
   ctrl->validityFlags=htonl(validityFlags);
   ctrl->power=power;
   ctrl->frequency=frequency;
   oapc_thread_mutex_lock(data->m_ctrlMutex);
   data->m_ctrlList->push(bin);
   oapc_thread_mutex_unlock(data->m_ctrlMutex);
   oapc_thread_signal_send(data->m_signal);
   return OAPC_OK;
}


OAPC_EXT_API unsigned long  oapc_set_bin_value(void* instanceData,unsigned long input,struct oapc_bin_head *value)
{
   struct instData      *data;
   struct oapc_bin_head *bin;

   if (input!=7) return OAPC_ERROR_NO_SUCH_IO;
   if (value->type!=OAPC_BIN_TYPE_STRUCT) return OAPC_ERROR_INVALID_INPUT;
   if (value->subType==OAPC_BIN_SUBTYPE_STRUCT_CTRL)
   {
      // copy only the structure but no vector data - they aren't used here
      data=(struct instData*)instanceData;
      bin=(struct oapc_bin_head*)malloc(sizeof(struct oapc_bin_head)+sizeof(struct oapc_bin_struct_ctrl));
      if (!bin) return OAPC_ERROR_NO_MEMORY;
      memcpy(bin,value,sizeof(struct oapc_bin_head)+sizeof(struct oapc_bin_struct_ctrl)); // copy only the head and the structure, appended vector data are not interesting for this plug-in
   }
   else if ((value->subType==OAPC_BIN_SUBTYPE_STRUCT_LASERCTRL) ||
            (value->subType==OAPC_BIN_SUBTYPE_STRUCT_MARKREADY) ||
            (value->subType==OAPC_BIN_SUBTYPE_STRUCT_ENDMARKREADY) ||
            (value->subType==OAPC_BIN_SUBTYPE_STRUCT_JOBSTART) ||
            (value->subType==OAPC_BIN_SUBTYPE_STRUCT_JOBEND) ||
            (value->subType==OAPC_BIN_SUBTYPE_STRUCT_SLICESTART) ||
            (value->subType==OAPC_BIN_SUBTYPE_STRUCT_SLICEEND))
   {
      data=(struct instData*)instanceData;
      bin=(struct oapc_bin_head*)malloc(sizeof(struct oapc_bin_head)+value->sizeData);
      if (!bin) return OAPC_ERROR_NO_MEMORY;
      memcpy(bin,value,sizeof(struct oapc_bin_head)+value->sizeData);
   }
   else return OAPC_ERROR_INVALID_INPUT;

   oapc_thread_mutex_lock(data->m_ctrlMutex);
   data->m_ctrlList->push(bin);
   oapc_thread_mutex_unlock(data->m_ctrlMutex);
   oapc_thread_signal_send(data->m_signal);

   return OAPC_OK;
}



