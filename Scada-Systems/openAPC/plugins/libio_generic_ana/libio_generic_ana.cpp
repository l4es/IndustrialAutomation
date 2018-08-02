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

#define INTERFACE_E1803U0  1
#define INTERFACE_E1803U1  2
#define INTERFACE_TTY      3

#define PROTOCOL_CTLT      1
#define PROTOCOL_CSLT      2

#ifdef _DEBUG
// #define TEST_MODE
#endif

#define FLAG_STOP_PROCESS_OUTSIDE_RANGE 0x0001
#define FLAG_PERMANENT_CONTROL          0x0002

struct libio_config
{
   unsigned short       version,length;
   int                  m_dLowValue,m_dHighValue;
   unsigned char        m_outport,m_outbit;
   unsigned char        m_inport[3];
   int                  m_mInMinTempValue[3],m_mInMaxTempValue[3];
   bool                 m_inEnabled[3];
   unsigned int         m_flags;
   unsigned int         res4;
   unsigned char        res3,res1;
   unsigned short       res2;
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
   bool                               m_running,m_enabled;
   unsigned char                      m_on;
   struct oapc_bin_head              *m_bin;
   void                              *m_signal;
   int                                m_cCurrTemp[3],m_cNewTemp[3];
   double                             m_factor[3];
   bool                               m_tempValid[3];
};


/** the xml data that define the behaviour and user interface of the plugin within the main application */
static const char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<oapc-config>\
<flowimage>%s</flowimage>\
<dialogue>\
 <general>\
  <param text=\"Analogue input port\" name=\"inport0\">\
   <type>option</type>\
   <value>Analogue 1 (10 Bit)</value>\
   <value>Analogue 2 (10 Bit)</value>\
   <value>Analogue 3 (10 Bit)</value>\
   <value>Analogue 1 (12 Bit)</value>\
   <value>Analogue 2 (12 Bit)</value>\
   <value>Analogue 3 (12 Bit)</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Temperature at 0\" name=\"inmintemp0\">\
   <type>float</type>\
   <default>%f</default>\
   <min>-100</min>\
   <max>500</max>\
   <accuracy>3</accuracy>\
   <unit>deg centigrade</unit>\
  </param>\
  <param text=\"Temperature at max\" name=\"inmaxtemp0\">\
   <type>float</type>\
   <default>%f</default>\
   <min>-100</min>\
   <max>500</max>\
   <accuracy>3</accuracy>\
   <unit>deg centigrade</unit>\
  </param>\
 </general>\
 <stdpanel text=\"Thresholds\">\
  <param text=\"Lower temperature\" name=\"lowvalue\">\
   <type>float</type>\
   <default>%f</default>\
   <min>-100</min>\
   <max>500</max>\
   <accuracy>1</accuracy>\
   <unit>deg centigrade</unit>\
  </param>\
  <param text=\"Upper temperature\" name=\"highvalue\">\
   <type>float</type>\
   <default>%f</default>\
   <min>-100</min>\
   <max>500</max>\
   <accuracy>1</accuracy>\
   <unit>deg centigrade</unit>\
  </param>\
  <param text=\"Additional Digital Output Port\" name=\"outport\">\
   <type>option</type>\
   <value>None</value>\
   <value>Laserport (8 Bit)</value>\
   <value>Digital (8 Bit)</value>\
   <value>Digital (16 Bit)</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Additional Digital Output Bit\" name=\"outbit\">\
   <type>option</type>\
   <value>Bit 0</value>\
   <value>Bit 1</value>\
   <value>Bit 2</value>""\
   <value>Bit 3</value>\
   <value>Bit 4</value>\
   <value>Bit 5</value>\
   <value>Bit 6</value>\
   <value>Bit 7</value>\
   <value>Bit 8</value>\
   <value>Bit 9</value>\
   <value>Bit 10</value>\
   <value>Bit 11</value>\
   <value>Bit 12</value>\
   <value>Bit 13</value>\
   <value>Bit 14</value>\
   <value>Bit 15</value>\
   <default>%d</default>\
  </param>\
  <param name=\"stopprocess\" text=\"Stop process on temperature deviation >10%%\">\
   <type>checkbox</type>""\
   <default>%d</default>\
  </param>\
  <param name=\"permcontrol\" text=\"Permanent temperature control\">\
   <type>checkbox</type>""\
   <default>%d</default>\
  </param>\
 </stdpanel>\
 <helppanel>\
  <in0>L - Lower temperature</in0>\
  <in1>H - Upper temperature</in1>\
  <in7>CTRL - Analogue input data</in7>\
  <out0>CURR - Current temperature</out0>\
  <out6>ON - Turn heater on/off</out6>\
  <out7>CTRL - Turn heater on/off</out7>\
 </helppanel>\
</dialogue>\
</oapc-config>";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                     flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyBAMAAABG0lM+AAAAMFBMVEUBAQH98wWFg4UF//3Fw8XtHyX9//0FAwX///8AAAAAAAAAAAAAAAAAAAAAAAAAAABgr2cgAAAACXBIWXMAAA7EAAAOxAGVKw4bAAABP0lEQVRIx72WQW7CMBBFs2rXdNN1hUT2zaI3iE8QdV9ZM93WJDM3QDk2eAzEIYixTeGvgjRPf74h5lecr1euCFzzSQC4bmrQhR+rGvBAmearBbDbpk+g7N+qFwrXYuK8oy73RkLZrZgY76jLvLdCgRMTTIkVxoQyYmL7JMqPCYXhc1IsgJcjla1AiRkzFVBA8HgKmYs2LDuNB1FYRBHdTdHTKP8FPSsXYomXf0ykhqJcQ1GuiKITRVywIWKJ1/H9+vdc2H1HXpSYa+g6uqT0XDGFnJpLqDD2O47jLieXrHSmUL+cZIJg7oVJd8d8Q+SMeyojV7Tm5JW4oYzdScHtXKPXbrGhojM1//Xy7X+TExXGDufPQoVuo1IOfmjRbVTKALbLbqNRuKmvdBuNstRf6TbqGU4VKOo2KjVVIKy4RHveGTIBx8T0dAAAAABJRU5ErkJggg==";
static char                     libname[]="Generic analogue temperature";
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
   cfgInfo->configType=OAPC_CONFIG_TYPE_PCONTROL;

   cfgInfo->pControl.version=OAPC_CONFIG_PCONTROL_VERSION;
   cfgInfo->pControl.length=sizeof(struct config_pcontrol);
   cfgInfo->pControl.flags=INT_FLAGS_ROUTE_BIN_OUT7_TO_SCANNER_BIN_IN7; // send all data of own output to scanner controller to be handled there
   cfgInfo->pControl.flags|=INT_FLAGS_ROUTE_BIN_IN7_FROM_SCANNER_BIN_OUT7;

   strncpy(cfgInfo->pControl.penPanelName,"Heater",sizeof(cfgInfo->pControl.penPanelName));

   cfgInfo->pControl.paramFlag[0]=OAPC_CONFIG_PCONTROL_FLAG_INT_TYPE; // additional pen parameter value
   cfgInfo->pControl.paramMin[0]=-100;
   cfgInfo->pControl.paramMax[0]=500;
   cfgInfo->pControl.paramDef[0]=30;
   strncpy(cfgInfo->pControl.paramName[0],"Lower temperature",sizeof(cfgInfo->pControl.paramName[0]));
   strncpy(cfgInfo->pControl.paramUnit[0],"°C",sizeof(cfgInfo->pControl.paramUnit[0]));

   cfgInfo->pControl.paramFlag[1]=OAPC_CONFIG_PCONTROL_FLAG_INT_TYPE; // additional pen parameter value
   cfgInfo->pControl.paramMin[1]=-100;
   cfgInfo->pControl.paramMax[1]=500;
   cfgInfo->pControl.paramDef[1]=35;
   strncpy(cfgInfo->pControl.paramName[1],"Upper temperature",sizeof(cfgInfo->pControl.paramName[1]));
   strncpy(cfgInfo->pControl.paramUnit[1],"°C",sizeof(cfgInfo->pControl.paramUnit[1]));

   cfgInfo->pControl.dispFlag[0]=OAPC_CONFIG_PCONTROL_FLAG_FLOAT_TYPE|OAPC_CONFIG_PCONTROL_FLAG_TEMPERATURE_STYLE; // display measured values as float and with temperature-symbol
   strncpy(cfgInfo->pControl.dispName[0],"Temperature",sizeof(cfgInfo->pControl.dispName[0]));
   strncpy(cfgInfo->pControl.dispUnit[0],"°C",sizeof(cfgInfo->pControl.dispUnit[0]));


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
   return OAPC_NUM_IO0|OAPC_NUM_IO1|OAPC_BIN_IO7;
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
   return OAPC_NUM_IO0|OAPC_DIGI_IO6|OAPC_BIN_IO7;
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
                                           data->config.m_inport[0],data->config.m_mInMinTempValue[0]/1000.0,data->config.m_mInMaxTempValue[0]/1000.0,
                                           data->config.m_dLowValue/10.0,data->config.m_dHighValue/10.0,
                                           data->config.m_outport,data->config.m_outbit,
                                           ((data->config.m_flags & FLAG_STOP_PROCESS_OUTSIDE_RANGE)!=0),
                                           ((data->config.m_flags & FLAG_PERMANENT_CONTROL)!=0)
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

   if (strcmp(name,"lowvalue")==0)        data->config.m_dLowValue=(int)OAPC_ROUND(oapc_util_atof(value)*10.0,0);
   else if (strcmp(name,"highvalue")==0)  data->config.m_dHighValue=(int)OAPC_ROUND(oapc_util_atof(value)*10.0,0);
   else if (strcmp(name,"outport")==0)    data->config.m_outport=(unsigned char)atoi(value);
   else if (strcmp(name,"outbit")==0)     data->config.m_outbit=(unsigned char)atoi(value);
   else if (strcmp(name,"inport0")==0)    data->config.m_inport[0]=(unsigned char)atoi(value);
   else if (strcmp(name,"inmintemp0")==0) data->config.m_mInMinTempValue[0]=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"inmaxtemp0")==0) data->config.m_mInMaxTempValue[0]=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"stopprocess")==0)
   {
      if (atoi(value)==0) data->config.m_flags&=~FLAG_STOP_PROCESS_OUTSIDE_RANGE;
      else data->config.m_flags|=FLAG_STOP_PROCESS_OUTSIDE_RANGE;
   }
   else if (strcmp(name,"permcontrol")==0)
   {
      if (atoi(value)==0) data->config.m_flags&=~FLAG_PERMANENT_CONTROL;
      else data->config.m_flags|=FLAG_PERMANENT_CONTROL;
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
   int              i;

   data=(struct instData*)instanceData;

   *length=sizeof(struct libio_config);
   save_config.version                 =htons(1);
   save_config.length                  =htons((unsigned short)*length);
   save_config.m_dLowValue             =htonl(data->config.m_dLowValue);
   save_config.m_dHighValue            =htonl(data->config.m_dHighValue);
   save_config.m_outport               =data->config.m_outport;
   save_config.m_outbit                =data->config.m_outbit;
   save_config.m_flags                 =htonl(data->config.m_flags);
   for (i=0; i<3; i++)
   {
      save_config.m_inport[i]=data->config.m_inport[i];
      save_config.m_mInMinTempValue[i]=htonl(data->config.m_mInMinTempValue[i]);
      save_config.m_mInMaxTempValue[i]=htonl(data->config.m_mInMaxTempValue[i]);
   }
   save_config.res1=0;
   save_config.res2=0;
   save_config.res3=0;

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
   memset(&data->config,0,sizeof(struct libio_config));
   data->config.version                 =ntohs(save_config.version);
   data->config.length                  =ntohs(save_config.length);
   data->config.m_dLowValue             =ntohl(save_config.m_dLowValue);
   data->config.m_dHighValue            =ntohl(save_config.m_dHighValue);
   data->config.m_outport               =save_config.m_outport;
   data->config.m_outbit                =save_config.m_outbit;
   data->config.m_flags                 =ntohl(save_config.m_flags);
   for (i=0; i<3; i++)
   {
      data->config.m_inport[i]=save_config.m_inport[i];
      data->config.m_mInMinTempValue[i]=ntohl(save_config.m_mInMinTempValue[i]);
      data->config.m_mInMaxTempValue[i]=ntohl(save_config.m_mInMaxTempValue[i]);
   }
}


/**
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API void* oapc_create_instance2(unsigned long /*flags*/)
{
   struct instData *data;
   int              i;

   data=(struct instData*)malloc(sizeof(struct instData));
   if (!data) return NULL;
   memset(data,0,sizeof(struct instData));

   data->config.m_dLowValue=30*10;
   data->config.m_dHighValue=35*10;
   data->config.m_outport=1;
   data->config.m_outbit=8;   
   data->config.m_inEnabled[0]=true;
   for (i=0; i<3; i++)
   {
      data->config.m_inport[i]=i+4;
      data->config.m_mInMinTempValue[i]=-10*1000;
      data->config.m_mInMaxTempValue[i]=100*1000;
      data->m_cCurrTemp[i]=-1000*100;
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


static char enable_output(struct instData *data,char halt)
{
   data->m_bin=oapc_util_alloc_bin_data(OAPC_BIN_TYPE_STRUCT,OAPC_BIN_SUBTYPE_STRUCT_ENABLE,OAPC_COMPRESS_NONE,0);
   if (data->m_bin)
   {
      data->m_bin->param1=halt;
      m_oapc_io_callback(OAPC_BIN_IO7,data->m_callbackID);
      return 1;
   }
   return 0;
}


// read the temperature from the pyrometer and react on this value according to current pen settings
static void *controlLoop(void *arg)
{
   struct instData *data;
   bool             processAllowed=true;

   data=(struct instData*)arg;

/*   if (data->config.m_protocol==PROTOCOL_CTLT) sendBuffer=0x81;
   else sendBuffer = 0x01; //PROTOCOL_CSLT*/

   data->m_on=2;
   while (data->m_running)
   {
      if ((data->config.m_flags & FLAG_PERMANENT_CONTROL)==0)
      {
         if (!data->m_enabled)
         {
            if (!processAllowed)
            {
               if (enable_output(data,1)) processAllowed=true;
            }
            oapc_thread_sleep(50);

            if ((data->m_cCurrTemp[0]!=data->m_cNewTemp[0]) && (data->m_tempValid[0]))
            {
               data->m_cCurrTemp[0]=data->m_cNewTemp[0];
               m_oapc_io_callback(OAPC_NUM_IO0,data->m_callbackID);
            }

            continue;
         }
      }

      if ((data->m_cCurrTemp[0]!=data->m_cNewTemp[0]) && (data->m_tempValid[0]))
      {
         if ((data->config.m_flags & FLAG_STOP_PROCESS_OUTSIDE_RANGE)==FLAG_STOP_PROCESS_OUTSIDE_RANGE)
         {
            if ((processAllowed) &&
                ((data->m_cNewTemp[0]*0.9<=data->config.m_dLowValue) || 
                 (data->m_cNewTemp[0]*1.1>=data->config.m_dHighValue)))
            {
               // out of allowed range, stop process
               if (enable_output(data,0)) processAllowed=false;
            }
            else if ((!processAllowed) &&
                     (data->m_cNewTemp[0]*0.9>data->config.m_dLowValue) && 
                     (data->m_cNewTemp[0]*1.1<data->config.m_dHighValue))
            {
               // back in allowed range, continue process
               if (enable_output(data,1)) processAllowed=true;
            }
         }

         if ((data->m_cNewTemp[0]<=data->config.m_dLowValue) && (data->m_on!=1))
         {
            data->m_on=1;
            m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
            if ((data->config.m_outport>1) && (!data->m_bin))
            {
               data->m_bin=oapc_util_alloc_bin_data(OAPC_BIN_TYPE_STRUCT,OAPC_BIN_SUBTYPE_STRUCT_FASTOUTPUTCTRL,OAPC_COMPRESS_NONE,sizeof(struct oapc_bin_struct_ioctrl));
               if (data->m_bin)
               {
                  struct oapc_bin_struct_ioctrl *ioctrl;

                  ioctrl=(struct oapc_bin_struct_ioctrl*)&data->m_bin->data;
                  switch (data->config.m_outport)
                  {
                     case 2:
                        ioctrl->enableFlags=IOCTRL_LASERPORT_8_1;
                        ioctrl->laserport8[0]=1 << (data->config.m_outbit-1);
                        break;
                     case 3:
                        ioctrl->enableFlags=IOCTRL_DIGITAL_8_1;
                        ioctrl->digital8[0]=1 << (data->config.m_outbit-1);
                        ioctrl->digital8mask[0]=1 << (data->config.m_outbit-1);
                        break;
                     case 4:
                        ioctrl->enableFlags=IOCTRL_DIGITAL_16_1;
                        ioctrl->digital16[0]=1 << (data->config.m_outbit-1);
                        ioctrl->digital16mask[0]=1 << (data->config.m_outbit-1);
                        break;
                     default:
                        assert(0);
                        break;
                  }
                  m_oapc_io_callback(OAPC_BIN_IO7,data->m_callbackID);
               }
            }
         }
         else if ((data->m_cNewTemp[0]>=data->config.m_dHighValue)  && (data->m_on!=0))
         {
            data->m_on=0;
            m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
            if ((data->config.m_outport>1) && (!data->m_bin))
            {
               data->m_bin=oapc_util_alloc_bin_data(OAPC_BIN_TYPE_STRUCT,OAPC_BIN_SUBTYPE_STRUCT_FASTOUTPUTCTRL,OAPC_COMPRESS_NONE,sizeof(struct oapc_bin_struct_ioctrl));
               if (data->m_bin)
               {
                  struct oapc_bin_struct_ioctrl *ioctrl;

                  ioctrl=(struct oapc_bin_struct_ioctrl*)&data->m_bin->data;
                  switch (data->config.m_outport)
                  {
                     case 2:
                        ioctrl->enableFlags=IOCTRL_LASERPORT_8_1;
                        ioctrl->laserport8[0]=0;
                        break;
                     case 3:
                        ioctrl->enableFlags=IOCTRL_DIGITAL_8_1;
                        ioctrl->digital8[0]=0;
                        ioctrl->digital8mask[0]=1 << (data->config.m_outbit-1);
                        break;
                     case 4:
                        ioctrl->enableFlags=IOCTRL_DIGITAL_16_1;
                        ioctrl->digital16[0]=0;
                        ioctrl->digital16mask[0]=1 << (data->config.m_outbit-1);
                        break;
                     default:
                        assert(0);
                        break;
                  }
                  m_oapc_io_callback(OAPC_BIN_IO7,data->m_callbackID);
               }
            }
            data->m_cCurrTemp[0]=data->m_cNewTemp[0];
            m_oapc_io_callback(OAPC_NUM_IO0,data->m_callbackID);
         }
      }
      oapc_thread_signal_wait(data->m_signal,100);
   };
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
   int              i;

   data=(struct instData*)instanceData;

   for (i = 0; i < 3; i++)
   {
      double diff,range=1.0;

      diff=(data->config.m_mInMaxTempValue[i]-data->config.m_mInMinTempValue[i])/1000.0;
      switch (data->config.m_inport[0])
      {
         case 1:
         case 2:
         case 3:
            range=1024.0;
            break;
         case 4:
         case 5:
         case 6:
            range=4096.0;
            break;
         default:
            assert(0);
            break;
      }
      data->m_factor[i]=(diff/range);
   }

   data->m_signal=oapc_thread_signal_create();
   if (!data->m_signal) return OAPC_ERROR_NO_MEMORY;
   data->m_running=true;
   if (!oapc_thread_create(controlLoop,data)) return OAPC_ERROR_NO_MEMORY;
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

   if (data->m_signal) oapc_thread_signal_release(data->m_signal);
   data->m_signal=NULL;
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

   *value=data->m_on;
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
   if (input==0)
   {
      data->config.m_dLowValue=(int)OAPC_ROUND(value*10.0,0);
   }
   else if (input==1)
   {
      data->config.m_dHighValue=(int)OAPC_ROUND(value*10.0,0);
   }
   else return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   return OAPC_OK;
}


OAPC_EXT_API unsigned long  oapc_get_num_value(void* instanceData,unsigned long output,double* value)
{
   struct instData *data;
   
   data=(struct instData*)instanceData;
   if (output==0)
   {
      *value=data->m_cCurrTemp[0]/100.0;
      return OAPC_OK;
   }
   return OAPC_ERROR_NO_SUCH_IO;
}



static int calculate_temp(struct instData *data,const int idx,const unsigned int value)
{
   double temp;

   data->m_tempValid[idx]=true;
   temp=data->m_factor[idx]*value;
   temp=temp+(data->config.m_mInMinTempValue[idx]/1000.0);
   return (int)OAPC_ROUND(temp*100.0,0);
}


OAPC_EXT_API unsigned long  oapc_set_bin_value(void* instanceData,unsigned long input,struct oapc_bin_head *value)
{
   struct instData      *data;
   int                  *dataArr;

   if (input!=7) return OAPC_ERROR_NO_SUCH_IO;
   data=(struct instData*)instanceData;
   if (value->type==OAPC_BIN_TYPE_STRUCT)
   {
      if (value->subType==OAPC_BIN_SUBTYPE_STRUCT_JOBSTART) data->m_enabled=true;
      else if (value->subType==OAPC_BIN_SUBTYPE_STRUCT_JOBEND)  data->m_enabled=false;
      else if (value->subType==OAPC_BIN_SUBTYPE_STRUCT_INPUTCTRL)
      {
         struct oapc_bin_struct_ioctrl ctrl;

         if (value->sizeData>sizeof(struct oapc_bin_struct_ioctrl))
         {
            assert(0);
            return OAPC_ERROR_INVALID_INPUT;
         }
         memcpy(&ctrl,&value->data,value->sizeData);
         ctrl.enableFlags=ntohl(ctrl.enableFlags);
         switch (data->config.m_inport[0])
         {
            case 1:
               if ((ctrl.enableFlags & IOCTRL_ANALOGUE_10_1)==0) return OAPC_OK;
               data->m_cNewTemp[0]=calculate_temp(data,0,ntohs(ctrl.analogue10[0]));
               break;
            case 2:
               if ((ctrl.enableFlags & IOCTRL_ANALOGUE_10_2)==0) return OAPC_OK;
               data->m_cNewTemp[0]=calculate_temp(data,0,ntohs(ctrl.analogue10[1]));
               break;
            case 3:
               if ((ctrl.enableFlags & IOCTRL_ANALOGUE_10_3)==0) return OAPC_OK;
               data->m_cNewTemp[0]=calculate_temp(data,0,ntohs(ctrl.analogue10[2]));
               break;
            case 4:
               if ((ctrl.enableFlags & IOCTRL_ANALOGUE_12_1)==0) return OAPC_OK;
               data->m_cNewTemp[0]=calculate_temp(data,0,ntohs(ctrl.analogue12[0]));
               break;
            case 5:
               if ((ctrl.enableFlags & IOCTRL_ANALOGUE_12_2)==0) return OAPC_OK;
               data->m_cNewTemp[0]=calculate_temp(data,0,ntohs(ctrl.analogue12[1]));
               break;
            case 6:
               if ((ctrl.enableFlags & IOCTRL_ANALOGUE_12_3)==0) return OAPC_OK;
               data->m_cNewTemp[0]=calculate_temp(data,0,ntohs(ctrl.analogue12[2]));
               break;
            default:
               assert(0);
               return OAPC_ERROR_INTERNAL;
         }

      }
      else return OAPC_ERROR_INVALID_INPUT;
      oapc_thread_signal_send(data->m_signal);
      return OAPC_OK;
   }
   if (value->type!=OAPC_BIN_TYPE_DATA) return OAPC_ERROR_INVALID_INPUT;
   else if (value->subType!=OAPC_BIN_SUBTYPE_DATA_GENERIC_32SLE) return OAPC_ERROR_INVALID_INPUT;
   // this function is called with these data types whenever pen changes and therefore new custom parameters apply to current process

   dataArr=(int*)&value->data;
   data->config.m_dLowValue =ntohl(dataArr[0])*10;
   data->config.m_dHighValue=ntohl(dataArr[1])*10;
   oapc_thread_signal_send(data->m_signal);
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
   struct instData      *data;

   data=(struct instData*)instanceData;

   if (output==7)
   {
      if (data->m_bin)
      {
         *value=data->m_bin;
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
   if (data->m_bin)
   {
      oapc_util_release_bin_data(data->m_bin);
      data->m_bin=NULL;
   }
}



