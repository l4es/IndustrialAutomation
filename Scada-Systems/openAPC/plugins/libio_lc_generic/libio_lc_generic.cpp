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
#define MAX_CMD_SIZE             128

#define INTERFACE_E1803U1  1
#define INTERFACE_ETHERNET 2
#define INTERFACE_TTY      3

#ifdef _DEBUG
// #define TEST_MODE
#endif

struct libio_config
{
   unsigned short       version,length;
   struct serial_params serialParams;
   char                 m_devOpenCmd[MAX_CMD_SIZE+8],m_devCloseCmd[MAX_CMD_SIZE+8];
   char                 m_jobStartCmd[MAX_CMD_SIZE+8],m_jobEndCmd[MAX_CMD_SIZE+8];
   char                 m_laserOnCmd[MAX_CMD_SIZE+8],m_laserOffCmd[MAX_CMD_SIZE+8];
   char                 m_markReadyCmd[MAX_CMD_SIZE+8],m_endMarkReadyCmd[MAX_CMD_SIZE+8];
   char                 resCmd1[MAX_CMD_SIZE+8],resCmd2[MAX_CMD_SIZE+8];
   unsigned char        m_responseChar,res1,res2,res3;
   // version 2
   int                  m_devOpenDelay,m_jobStartDelay,m_laserOnDelay,m_markReadyDelay;
   // version 3
   char                 m_sliceStartCmd[MAX_CMD_SIZE+8],m_sliceEndCmd[MAX_CMD_SIZE+8];
   int                  m_sliceStartDelay;
};


enum eBinState
{
   eBinStateUnused=0,
   eBinStateFilled=1,
   ebinStateSent=2
};

struct instData
{
   struct libio_config                config;
   int                                m_callbackID;
   int                                m_prevPower,m_prevFreq;
   bool                               m_running;
   unsigned char                      m_useInterface;
   bool                               m_busy;
   unsigned char                      m_prevOn;
   unsigned long                      m_lc_capabilities;
   void                              *m_ctrlMutex,*m_signal;
   std::queue<struct oapc_bin_head*> *m_ctrlList;
   struct oapc_bin_head              *m_uartBin;
   enum eBinState                     m_uartBinState;
#ifdef ENV_WINDOWS
   HANDLE                             m_hCommPort;
#else
   int                                m_hCommPort;
#endif
   int                                m_sock;
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
   <min>2</min>\
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
  <param text=\"Response End Character\">\
   <name>responsechar</name>\
   <type>option</type>\
   <value>None</value>\
   <value>CR/LF (\\r\\n, like Windows)</value>\
   <value>LF (\\n, like Linux)</value>\
   <default>%d</default>\
  </param>\
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
  <param text=\"Slice Start Command\">\
   <name>slicestartcmd</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
  </param>""\
  <param text=\"Slice Start Delay\">\
   <name>slicestartdelay</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>0</min>\
   <max>%d</max>\
   <unit>msec</unit>\
  </param>\
  <param text=\"Laser On Command\">\
   <name>laseroncmd</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
  </param>\
  <param text=\"Laser on delay\">\
   <name>laserondelay</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>0</min>\
   <max>%d</max>\
   <unit>msec</unit>\
  </param>\
  <param text=\"Laser Off Command\">\
   <name>laseroffcmd</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
  </param>\
  <param text=\"Slice End Command\">\
   <name>sliceendcmd</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
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
  <in5>FREQ - Frequency in Hz</in5>\
  <in7>CTRL - Control data</in7>\
  <out6>BSY - Marking active</out6>\
  <out7>CTRL - Control data</out7>\
 </helppanel>\
</dialogue>\
</oapc-config>";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                     flowImage[] = "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";
static char                     libname[]="Generic Laser Controller";
static char                     xmldescr[MAX_XML_SIZE+1];
static struct libio_config      save_config;
static lib_oapc_io_callback     m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports


/** has to be called after create instance and load data */
OAPC_EXT_API unsigned long oapc_get_config_info_data(void* instanceData,struct config_info *fillStruct)
{
   struct config_info *cfgInfo;
   struct instData    *data;

   data=(struct instData*)instanceData;

   if (!fillStruct) return OAPC_ERROR;
   cfgInfo=(struct config_info*)fillStruct;
   memset(cfgInfo,0,sizeof(struct config_info));

   cfgInfo->version=OAPC_CONFIG_INFO_VERSION;
   cfgInfo->length=sizeof(struct config_info);
   cfgInfo->configType=OAPC_CONFIG_TYPE_LASERCONTROLLER;

   cfgInfo->laserController.version=OAPC_CONFIG_LASER_CONTROLLER_VERSION;
   cfgInfo->laserController.length=sizeof(struct config_laser_controller);
   cfgInfo->laserController.capabilities=data->m_lc_capabilities;
   if (data->m_useInterface==INTERFACE_E1803U1)
    cfgInfo->laserController.flags=INT_FLAGS_ROUTE_BIN_OUT7_TO_SCANNER_BIN_IN7;

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
   return OAPC_DIGI_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|
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
   return OAPC_DIGI_IO6|OAPC_BIN_IO7;
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
                                           data->config.m_responseChar,
                                           data->config.m_devOpenCmd,MAX_CMD_SIZE,data->config.m_devOpenDelay,5000,
                                           data->config.m_markReadyCmd,MAX_CMD_SIZE,data->config.m_markReadyDelay,5000,
                                           data->config.m_jobStartCmd,MAX_CMD_SIZE,data->config.m_jobStartDelay,5000,
                                           data->config.m_sliceStartCmd,MAX_CMD_SIZE,data->config.m_sliceStartDelay,5000,
                                           data->config.m_laserOnCmd,MAX_CMD_SIZE,data->config.m_laserOnDelay,5000,
                                           data->config.m_laserOffCmd,MAX_CMD_SIZE,
                                           data->config.m_sliceEndCmd,MAX_CMD_SIZE,
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
   else if (strcmp(name,"slicestartcmd")==0)   strncpy(data->config.m_sliceStartCmd,value,MAX_CMD_SIZE);
   else if (strcmp(name,"sliceendcmd")==0)     strncpy(data->config.m_sliceEndCmd,value,MAX_CMD_SIZE);
   else if (strcmp(name,"laseroncmd")==0)      strncpy(data->config.m_laserOnCmd,value,MAX_CMD_SIZE);
   else if (strcmp(name,"laseroffcmd")==0)     strncpy(data->config.m_laserOffCmd,value,MAX_CMD_SIZE);
   else if (strcmp(name,"responsechar")==0)    data->config.m_responseChar=(unsigned char)atoi(value);
   else if (strcmp(name,"devopendelay")==0)    data->config.m_devOpenDelay=atoi(value);
   else if (strcmp(name,"markreadydelay")==0)  data->config.m_markReadyDelay=atoi(value);
   else if (strcmp(name,"jobstartdelay")==0)   data->config.m_jobStartDelay=atoi(value);
   else if (strcmp(name,"laserondelay")==0)    data->config.m_laserOnDelay=atoi(value);
   else if (strcmp(name,"slicestartdelay")==0) data->config.m_sliceStartDelay=atoi(value);
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
   strncpy(save_config.m_laserOnCmd,data->config.m_laserOnCmd,MAX_CMD_SIZE);
   strncpy(save_config.m_laserOffCmd,data->config.m_laserOffCmd,MAX_CMD_SIZE);
   memset(save_config.resCmd1,0,MAX_CMD_SIZE);
   memset(save_config.resCmd2,0,MAX_CMD_SIZE);
   save_config.m_responseChar          =data->config.m_responseChar;
   save_config.res1=0;
   save_config.res2=0;
   save_config.res3=0;
   //version 2
   save_config.m_devOpenDelay   =htonl(data->config.m_devOpenDelay);
   save_config.m_markReadyDelay =htonl(data->config.m_markReadyDelay);
   save_config.m_jobStartDelay  =htonl(data->config.m_jobStartDelay);
   save_config.m_laserOnDelay   =htonl(data->config.m_laserOnDelay);
   // version 3
   strncpy(save_config.m_sliceStartCmd,data->config.m_sliceStartCmd,MAX_CMD_SIZE);
   strncpy(save_config.m_sliceEndCmd,data->config.m_sliceEndCmd,MAX_CMD_SIZE);
   save_config.m_sliceStartDelay  =htonl(data->config.m_sliceStartDelay);

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
   strncpy(data->config.m_laserOnCmd,save_config.m_laserOnCmd,MAX_CMD_SIZE);
   strncpy(data->config.m_laserOffCmd,save_config.m_laserOffCmd,MAX_CMD_SIZE);
   memset(data->config.resCmd1,0,MAX_CMD_SIZE);
   memset(data->config.resCmd2,0,MAX_CMD_SIZE);
   data->config.m_responseChar          =save_config.m_responseChar;
   data->config.res1=0;
   data->config.res2=0;
   data->config.res3=0;
   if (data->config.version>=2)
   {
      data->config.m_devOpenDelay=ntohl(save_config.m_devOpenDelay);
      data->config.m_jobStartDelay=ntohl(save_config.m_jobStartDelay);
      data->config.m_laserOnDelay=ntohl(save_config.m_laserOnDelay);
      data->config.m_markReadyDelay=ntohl(save_config.m_markReadyDelay);
   }
   else
   {
      data->config.m_devOpenDelay=0;
      data->config.m_jobStartDelay=0;
      data->config.m_laserOnDelay=0;
      data->config.m_markReadyDelay=0;
   }
   if (data->config.version>=3)
   {
      strncpy(data->config.m_sliceStartCmd,save_config.m_sliceStartCmd,MAX_CMD_SIZE);
      strncpy(data->config.m_sliceEndCmd,save_config.m_sliceEndCmd,MAX_CMD_SIZE);
      data->config.m_sliceStartDelay=ntohl(save_config.m_sliceStartDelay);
   }
   else
   {
      data->config.m_sliceStartCmd[0]=0;
      data->config.m_sliceEndCmd[0]=0;
      data->config.m_sliceStartDelay=0;
   }
   data->m_lc_capabilities=0; // no frequency/power setting capabilities
   if (strlen(data->config.m_laserOnCmd)>0)  data->m_lc_capabilities|=OAPC_LC_HAS_LASERON;
   if (strlen(data->config.m_laserOffCmd)>0) data->m_lc_capabilities|=OAPC_LC_HAS_LASEROFF;
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
   data->config.m_responseChar=1;

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


static int serial_send(struct instData *data,const char *sendBuffer,const size_t length,const unsigned int timeout)
{
   if (data->m_useInterface==INTERFACE_TTY)
    return oapc_serial_send(data->m_hCommPort,sendBuffer,(int)strlen(sendBuffer),timeout);
   else if (data->m_useInterface==INTERFACE_ETHERNET)
    return oapc_tcp_send(data->m_sock,sendBuffer,(int)strlen(sendBuffer),timeout);
   else
   {
      if (data->m_uartBin)
      {
         assert(0);
         return 0;
      }
      data->m_uartBin=oapc_util_alloc_bin_data(OAPC_BIN_TYPE_DATA,OAPC_BIN_SUBTYPE_DATA_GENERIC_8S,OAPC_COMPRESS_NONE,(int)length);
      if (!data->m_uartBin) return 0;
      data->m_uartBin->param1=htonl((int)length);
      memcpy(&data->m_uartBin->data,sendBuffer,(int)length);
      data->m_uartBinState=eBinStateFilled;
      m_oapc_io_callback(OAPC_BIN_IO7,data->m_callbackID);
   }
   return (int)length;
}


#ifndef TEST_MODE
static void waitResponse(struct instData *data)
{
   char buf[200];

   if (data->m_useInterface==INTERFACE_TTY)
   {
      if (data->config.m_responseChar==1) oapc_serial_recv(data->m_hCommPort,buf,200,NULL,0);
      else if (data->config.m_responseChar==2) oapc_serial_recv(data->m_hCommPort,buf,200,"\r\n",750);
      else if (data->config.m_responseChar==3) oapc_serial_recv(data->m_hCommPort,buf,200,"\r",750);
      oapc_serial_recv(data->m_hCommPort,buf,200,NULL,0);      
   }
   else if (data->m_useInterface==INTERFACE_ETHERNET)
   {
      if (data->config.m_responseChar==1) oapc_tcp_recv(data->m_sock,buf,200,NULL,0);
      else if (data->config.m_responseChar==2) oapc_tcp_recv(data->m_sock,buf,200,"\r\n",750);
      else if (data->config.m_responseChar==3) oapc_tcp_recv(data->m_sock,buf,200,"\r",750);
      oapc_serial_recv(data->m_hCommPort,buf,200,NULL,0);      
   }
}
#endif


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
#ifndef TEST_MODE
                  serial_send(data,data->config.m_markReadyCmd,(int)strlen(data->config.m_markReadyCmd),500);
                  waitResponse(data);
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
#ifndef TEST_MODE
                  serial_send(data,data->config.m_endMarkReadyCmd,(int)strlen(data->config.m_endMarkReadyCmd),500);
                  waitResponse(data);
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
#ifndef TEST_MODE
                  serial_send(data,data->config.m_jobStartCmd,(int)strlen(data->config.m_jobStartCmd),500);
                  waitResponse(data);
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
#ifndef TEST_MODE
                  serial_send(data,data->config.m_jobEndCmd,(int)strlen(data->config.m_jobEndCmd),500);
                  waitResponse(data);
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
               if (data->config.m_sliceStartCmd[0])
               {
#ifndef TEST_MODE
                  serial_send(data,data->config.m_sliceStartCmd,(int)strlen(data->config.m_sliceStartCmd),500);
                  waitResponse(data);
#endif
                  if (data->config.m_sliceStartDelay>=0)
                   oapc_thread_sleep(data->config.m_sliceStartDelay);
               }
               data->m_busy=0;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               break;
            }
            case OAPC_BIN_SUBTYPE_STRUCT_SLICEEND:
            {
               data->m_busy=1;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               if (data->config.m_sliceEndCmd[0])
               {
#ifndef TEST_MODE
                  serial_send(data,data->config.m_sliceEndCmd,(int)strlen(data->config.m_sliceEndCmd),500);
                  waitResponse(data);
#endif
               }
               data->m_busy=0;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               break;
            }
            case OAPC_BIN_SUBTYPE_STRUCT_LASERCTRL:
            {
               data->m_busy=1;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);

/*               struct oapc_bin_struct_laserctrl *ctrl;

               ctrl=(struct oapc_bin_struct_laserctrl*)&bin->data;*/

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
                  int power;

                  power=(int)ntohl(ctrl->power);
                  if (power!=data->m_prevPower)
                  {
                     data->m_prevPower=power;
                     // send power to laser here
                  }
               }
               if (validityFlags & TOOLPARAM_VALIDFLAG_FREQ)
               {
                  int freq;

                  freq=(int)ntohl(ctrl->frequency);
                  if (freq!=data->m_prevFreq)
                  {
                     data->m_prevFreq=freq;
                     // send frequency to laser here
                  }
               }
               if (validityFlags & TOOLPARAM_VALIDFLAG_ON)
               {
                  if (ctrl->on!=data->m_prevOn)
                  {
                     if ((ctrl->on) && (data->config.m_laserOnCmd[0]))
                     {
#ifndef TEST_MODE
                        serial_send(data,data->config.m_laserOnCmd,(int)strlen(data->config.m_laserOnCmd),500);
                        waitResponse(data);
#endif
                        if (data->config.m_laserOnDelay>=0)
                         oapc_thread_sleep(data->config.m_laserOnDelay);
                     }
                     else if ((!ctrl->on) && (data->config.m_laserOffCmd[0]))
                     {
#ifndef TEST_MODE
                        serial_send(data,data->config.m_laserOffCmd,(int)strlen(data->config.m_laserOffCmd),500);
                        waitResponse(data);
#endif
                     }
                     data->m_prevOn=ctrl->on;
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
   convertCommand(data->config.m_laserOnCmd);
   convertCommand(data->config.m_laserOffCmd);
   convertCommand(data->config.m_sliceStartCmd);
   convertCommand(data->config.m_sliceEndCmd);
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

   if (strncmp(data->config.serialParams.port,"U1",sizeof(data->config.serialParams.port))==0)
   {
      data->m_useInterface=INTERFACE_E1803U1;
   }
#ifdef ENV_WINDOWS
   else if (strstr(data->config.serialParams.port,"COM"))
#else
   else if (strstr(data->config.serialParams.port,"tty"))
#endif
   {
#ifndef TEST_MODE
      ret=oapc_serial_port_open(&data->config.serialParams,&data->m_hCommPort);
      if (ret!=OAPC_OK) return ret;
      data->m_useInterface=INTERFACE_TTY;
   }
   else
   {
      data->m_sock=oapc_tcp_connect_to(data->config.serialParams.port,1234);
      if (data->m_sock<=0) return OAPC_ERROR_CONNECTION;
      data->m_useInterface=INTERFACE_ETHERNET;
   }
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
       serial_send(data,data->config.m_devOpenCmd,(int)strlen(data->config.m_devOpenCmd),500);
       waitResponse(data);
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
      serial_send(data,data->config.m_devCloseCmd,(int)strlen(data->config.m_devCloseCmd),500);
      waitResponse(data);
#endif
   }
   if (data->m_useInterface==INTERFACE_TTY)
   {
#ifndef TEST_MODE
      oapc_serial_port_close(&data->m_hCommPort);
#endif
   }
   else if (data->m_useInterface==INTERFACE_ETHERNET)
   {
       oapc_tcp_closesocket(data->m_sock);
   }

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



OAPC_EXT_API unsigned long  oapc_get_bin_value(void *instanceData,unsigned long output,struct oapc_bin_head **value)
{
   struct instData      *data;

   data=(struct instData*)instanceData;

   if (output==7)
   {
      if (data->m_uartBinState==eBinStateFilled)
      {
         *value=data->m_uartBin;
         data->m_uartBinState=ebinStateSent;
         return OAPC_OK;
      }      
      return OAPC_ERROR_NO_DATA_AVAILABLE;
   }
   else return OAPC_ERROR_NO_SUCH_IO;
}



OAPC_EXT_API void oapc_release_bin_data(void *instanceData,unsigned long /*output*/)
{
   struct instData *data;

   data=(struct instData*)instanceData;
}

