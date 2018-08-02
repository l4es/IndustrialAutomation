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

#include <string>

#include "libio_coherent_avia.h"
#include "APC.h"
#include "AviaAPC.h"

#define MAX_XML_SIZE           25000
#define MAX_CMD_SIZE             128

#ifdef _DEBUG
// #define TEST_MODE
#endif

/** the xml data that define the behaviour and user interface of the plugin within the main application */
static const char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<oapc-config>\
<flowimage>%s</flowimage>\
<dialogue>\
 <general>\
  <param text=\"IP\">\
   <name>ip</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>16</max>\
  </param>\
  <param text=\"Port\">\
   <name>port</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>1</min>\
   <max>65535</max>\
  </param>\
  <param text=\"Control power during process\">\
   <name>ctrlpower</name>\
   <type>checkbox</type>""\
   <default>%d</default>\
  </param>\
  <param text=\"Control frequency during process\">\
   <name>ctrlfreq</name>\
   <type>checkbox</type>""\
   <default>%d</default>\
  </param>\
 </general>\
 <stdpanel text=\"Avia\">\
  <param text=\"Current\">\
   <name>curr</name>\
   <type>float</type>\
   <default>%f</default>\
   <min>0</min>\
   <max>100</max>\
   <unit>%%</unit>\
  </param>\
  <param text=\"Current Limit\">\
   <name>currlimit</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>10</min>\
   <max>60</max>\
   <unit>A</unit>\
  </param>\
  <param text=\"Power Limit\">\
   <name>powerlimit</name>\
   <type>float</type>\
   <default>%f</default>\
   <min>0.1</min>\
   <max>3.5</max>\
   <unit>W</unit>\
  </param>\
  <param text=\"Energy Limit\">\
   <name>energylimit</name>\
   <type>float</type>\
   <default>%f</default>\
   <min>0.1</min>\
   <max>150.0</max>\
   <unit>uJ</unit>\
  </param>\
  <param text=\"Thermatrack\">\
   <name>thermatrack</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>0</min>\
   <max>9000</max>\
  </param>\
  <param text=\"Frequency\">\
   <name>freq</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>5000</min>\
   <max>100000</max>\
   <unit>Hz</unit>\
  </param>\
  <param text=\"Trigger Mode\">\
   <name>triggermode</name>\
   <type>option</type>\
   <value>Internal</value>\
   <value>External</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Pulse Control\">\
   <name>pulsecontrol</name>\
   <type>option</type>\
   <value>ThermEQ</value>\
   <value>Pulse Track</value>\
   <default>%d</default>\
  </param>\
  <param text=\"SHG Temperature\">\
   <name>shgtemp</name>\
   <type>float</type>\
   <default>%f</default>\
   <min>50.0</min>\
   <max>170.0</max>\
   <unit>°C</unit>\
  </param>\
  <param text=\"FHG Temperature\">\
   <name>fhgtemp</name>\
   <type>float</type>\
   <default>%f</default>\
   <min>50.0</min>\
   <max>175.0</max>\
   <unit>°C</unit>\
  </param>\
  <param text=\"Timeout\">\
   <name>timeout</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>10</min>\
   <max>3600</max>\
   <unit>s</unit>\
  </param>\
 </stdpanel>\
 <helppanel>\
  <in3>L - Laser on/off</in3>\
  <in4>PWR - Laser power in %%</in4>\
  <in5>FREQ - Frequency in Hz</in5>\
  <in7>CTRL - Control data</in7>\
  <out4>STAT - Status messages</out4>""\
  <out5>ERR - In-process error codes</out5>\
  <out6>BSY - Marking active</out6>\
 </helppanel>\
</dialogue>""\
</oapc-config>";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                     flowImage[]="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";
static char                     libname[]="Coherent Avia(R) Ethernet";
static char                     xmldescr[MAX_XML_SIZE+1];
static struct libio_config      save_config;
       lib_oapc_io_callback     m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports


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
   cfgInfo->laserController.capabilities=0;
   if ((data->config.m_flags & OAPC_AVIA_FLAG_CONTROL_POWER)!=0) cfgInfo->laserController.capabilities|=OAPC_LC_HAS_POWER;
   if ((data->config.m_flags & OAPC_AVIA_FLAG_CONTROL_FREQ)!=0) cfgInfo->laserController.capabilities|=OAPC_LC_HAS_FREQ;

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
   return OAPC_CHAR_IO4|OAPC_NUM_IO5|OAPC_DIGI_IO6|OAPC_BIN_IO7;
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
                                           data->config.m_ip,data->config.m_port,
                                           data->config.m_flags & OAPC_AVIA_FLAG_CONTROL_POWER,data->config.m_flags & OAPC_AVIA_FLAG_CONTROL_FREQ,
                                           data->config.m_mCurr/1000.0,data->config.m_currLimit,
                                           data->config.m_mPowerLimit/1000.0,data->config.m_mEnergyLimit/1000.0,
                                           data->config.m_thermatrack,data->config.m_freq,
                                           data->config.m_triggerMode,data->config.m_pulseControl,
                                           data->config.m_mSHGTemp/1000.0,data->config.m_mFHGTemp/1000.0,
                                           data->config.m_timeout
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

   if (strcmp(name,"ip")==0)                 strncpy(data->config.m_ip,value,20);
   else if (strcmp(name,"port")==0)          data->config.m_port=(unsigned short)atoi(value);
   else if (strcmp(name,"currlimit")==0)     data->config.m_currLimit=(unsigned char)atoi(value);
   else if (strcmp(name,"powerlimit")==0)    data->config.m_mPowerLimit=(unsigned int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"energylimit")==0)   data->config.m_mEnergyLimit=(unsigned int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"curr")==0)          data->config.m_mCurr=(unsigned int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"thermatrack")==0)   data->config.m_thermatrack=(unsigned short)atoi(value);
   else if (strcmp(name,"freq")==0)          data->config.m_freq=(unsigned short)atoi(value);
   else if (strcmp(name,"triggermode")==0)   data->config.m_triggerMode=(unsigned char)atoi(value);
   else if (strcmp(name,"pulsecontrol")==0)  data->config.m_pulseControl=(unsigned char)atoi(value);
   else if (strcmp(name,"shgtemp")==0)       data->config.m_mSHGTemp=(unsigned int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"fhgtemp")==0)       data->config.m_mFHGTemp=(unsigned int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"timeout")==0)       data->config.m_timeout=(unsigned short)atoi(value);
   else if (strcmp(name,"ctrlpower")==0)
   {
      if (atoi(value)) data->config.m_flags|=OAPC_AVIA_FLAG_CONTROL_POWER;
      else data->config.m_flags&=~OAPC_AVIA_FLAG_CONTROL_POWER;
   }
   else if (strcmp(name,"ctrlfreq")==0)
   {
      if (atoi(value)) data->config.m_flags|=OAPC_AVIA_FLAG_CONTROL_FREQ;
      else data->config.m_flags&=~OAPC_AVIA_FLAG_CONTROL_FREQ;
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
   save_config.version            =htons(1);
   save_config.length             =htons((unsigned short)*length);
   strncpy(save_config.m_ip,data->config.m_ip,20);
   save_config.m_port             =htons(data->config.m_port);
   save_config.m_currLimit        =data->config.m_currLimit;
   save_config.m_mPowerLimit      =htonl(data->config.m_mPowerLimit);
   save_config.m_mEnergyLimit     =htonl(data->config.m_mEnergyLimit);
   save_config.m_mCurr            =htonl(data->config.m_mCurr);
   save_config.m_thermatrack      =htons(data->config.m_thermatrack);
   save_config.m_freq             =htons(data->config.m_freq);
   save_config.m_triggerMode      =data->config.m_triggerMode;
   save_config.m_pulseControl     =data->config.m_pulseControl;
   save_config.m_mSHGTemp         =htonl(data->config.m_mSHGTemp);
   save_config.m_mFHGTemp         =htonl(data->config.m_mFHGTemp);
   save_config.m_timeout          =htons(data->config.m_timeout);
   save_config.m_flags            =htonl(data->config.m_flags);

   save_config.res1=0;
   save_config.res2=0;

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
   data->config.version         =ntohs(save_config.version);
   data->config.length          =ntohs(save_config.length);
   strncpy(data->config.m_ip,save_config.m_ip,20);
   data->config.m_port          =ntohs(save_config.m_port);
   data->config.m_currLimit     =save_config.m_currLimit;
   data->config.m_mPowerLimit   =ntohl(save_config.m_mPowerLimit);
   data->config.m_mEnergyLimit  =ntohl(save_config.m_mEnergyLimit);
   data->config.m_mCurr         =ntohl(save_config.m_mCurr);
   data->config.m_thermatrack   =ntohs(save_config.m_thermatrack);
   data->config.m_freq          =ntohs(save_config.m_freq);
   data->config.m_triggerMode   =save_config.m_triggerMode;
   data->config.m_pulseControl  =save_config.m_pulseControl;
   data->config.m_mSHGTemp      =ntohl(save_config.m_mSHGTemp);
   data->config.m_mFHGTemp      =ntohl(save_config.m_mFHGTemp);
   data->config.m_timeout       =ntohs(save_config.m_timeout);
   data->config.m_flags         =ntohl(save_config.m_flags);
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

   strcpy(data->config.m_ip,PF_AVIA_HOST);
   data->config.m_port=PF_AVIA_PORT;
   data->config.m_currLimit=PF_CURRENT_LIMIT;
   data->config.m_mPowerLimit=(unsigned int)OAPC_ROUND(PF_POWER_LIMIT*1000.0,0);
   data->config.m_mEnergyLimit=PF_ENERGY_LIMIT*1000;
   data->config.m_mCurr=PF_CURRENT*1000;
   data->config.m_thermatrack=PF_THERMATRACK;
   data->config.m_freq=PF_FREQUENCY;
   data->config.m_triggerMode=1;
   data->config.m_pulseControl=1;
   data->config.m_mSHGTemp=(unsigned int)OAPC_ROUND(PF_SHG_TEMPERATURE*1000.0,0);
   data->config.m_mFHGTemp=(unsigned int)OAPC_ROUND(PF_FHG_TEMPERATURE*1000.0,0);
   data->config.m_timeout=PF_TIMEOUT;

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
   struct instData      *data;

   data=(struct instData*)arg;
   while (data->m_running)
   {
      oapc_thread_signal_wait(data->m_signal,-1);
      if (!data->m_running) break;
      if (data->m_ctrl)
      {
         if ((data->m_ctrl) && (data->m_ctrl->type==OAPC_BIN_TYPE_STRUCT)) switch (data->m_ctrl->subType)
         {
            case OAPC_BIN_SUBTYPE_STRUCT_MARKREADY:
            {
               data->m_busy=1;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);

               // BeamConstruct signals it is ready for marking so open the shutter               
               if (!data->m_laserInstance->Set_W(WR_SHUTTER, RD_SHUTTER, 1, data->config.m_timeout))
               {
                  // signal error to main application to let it stop the whole operation
                  data->m_errorCode=OAPC_ERROR_SEND_DATA;
                  m_oapc_io_callback(OAPC_NUM_IO5,data->m_callbackID);
               }

               data->m_busy=0;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               break;
            }
            case OAPC_BIN_SUBTYPE_STRUCT_ENDMARKREADY:
            {
               data->m_busy=1;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);

               // BeamConstruct signals marking is finished now so close the shutter
               if (!data->m_laserInstance->Set_W(WR_SHUTTER, RD_SHUTTER,0, data->config.m_timeout))
               {
                  // signal error to main application to let it stop the whole operation
                  data->m_errorCode=OAPC_ERROR_SEND_DATA;
                  m_oapc_io_callback(OAPC_NUM_IO5,data->m_callbackID);
               }

               data->m_busy=0;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               break;
            }
            case OAPC_BIN_SUBTYPE_STRUCT_JOBSTART:
            {
               data->m_busy=1;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);

               // do nothing special on job start

               data->m_busy=0;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               break;
            }
            case OAPC_BIN_SUBTYPE_STRUCT_JOBEND:
            {
               data->m_busy=1;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);

               // do nothing special on job end

               data->m_busy=0;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
               break;
            }
            case OAPC_BIN_SUBTYPE_STRUCT_LASERCTRL:
            {
               data->m_busy=1;
               m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);

               // do not use any of the special laser parameters

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

               ctrl=(struct oapc_bin_struct_ctrl*)&data->m_ctrl->data;

               validityFlags=ntohl(ctrl->validityFlags);
               if (validityFlags & TOOLPARAM_VALIDFLAG_POWER)
               {
                  int power;

                  power=(int)ntohl(ctrl->power);
                  if (power!=data->m_prevPower)
                  {
                     data->m_prevPower=power;
                     if ((data->config.m_flags & OAPC_AVIA_FLAG_CONTROL_POWER)!=0)
                     {
                        data->m_laserInstance->Set(WR_CURRENT,power/1000.0);
                        // TODO: manage error?
                        if (!data->m_laserInstance->Wait_Current(data, data->config.m_timeout))
                        {
                           // signal error to main application to let it stop the whole operation
                           data->m_errorCode=OAPC_ERROR_SEND_DATA;
                           m_oapc_io_callback(OAPC_NUM_IO5,data->m_callbackID);
                        }
                     }
                  }
               }
               if (validityFlags & TOOLPARAM_VALIDFLAG_FREQ)
               {
                  int freq;

                  freq=(int)ntohl(ctrl->frequency);
                  if (freq!=data->m_prevFreq)
                  {
                     data->m_prevFreq=freq;
                     if ((data->config.m_flags & OAPC_AVIA_FLAG_CONTROL_POWER)!=0)
                     {
                        data->m_laserInstance->Set(WR_FREQUENCY,freq);
                        // wait here for settlement of frequency?
                     }
                  }
               }
               if (validityFlags & TOOLPARAM_VALIDFLAG_ON)
               {
                  if (ctrl->on!=data->m_prevOn)
                  {
                     if (ctrl->on)
                     {
                         // turn laser on here -> do not forget to return the related OAPC_LC_LASERON-flag in this case
                     }
                     else if (!ctrl->on)
                     {
                         // turn laser on here -> do not forget to return the related OAPC_LC_LASEROFF-flag in this case
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
         oapc_util_release_bin_data(data->m_ctrl);
         data->m_ctrl=NULL;
      }
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

   data=(struct instData*)instanceData;

   try
   {
      data->m_laserInstance=new Avia(data->config.m_ip,data->config.m_port);
   }
   catch (...)
   {
      return OAPC_ERROR_DEVICE;
   }

   data->m_signal=oapc_thread_signal_create();
   if (!data->m_signal) return OAPC_ERROR_NO_MEMORY;

   data->m_laserInstance->Load(data);
   //	   pFocus->Load();

   data->m_laserInstance->Reset();
   if (!data->m_laserInstance->Set_W(WR_LOCK, RD_LOCK, 1, data->config.m_timeout)) return OAPC_ERROR_CUSTOM;
   if (!data->m_laserInstance->Wait_Temperature(data->config.m_timeout)) return OAPC_ERROR_CUSTOM;
   // turn AVIA on
   if (!data->m_laserInstance->Set_W(WR_PULSE, RD_PULSE, 1, data->config.m_timeout)) return OAPC_ERROR_CUSTOM;
   if (!data->m_laserInstance->Set_W(WR_DIODE, RD_DIODE, 1, data->config.m_timeout)) return OAPC_ERROR_CUSTOM;
   // wait for current
   if (!data->m_laserInstance->Wait_Current(data, data->config.m_timeout)) return OAPC_ERROR_CUSTOM;

   // wait for UV
   data->m_laserInstance->Set(WR_TRIGGER_MODE, 0);
   data->m_laserInstance->Set(WR_PULSE_CONTROL, 2);
   data->m_laserInstance->Set(WR_THERMATRACK, data->config.m_thermatrack);
   data->m_laserInstance->Set(WR_FREQUENCY, data->config.m_freq);
   if (!data->m_laserInstance->Set_W(WR_DIODE, RD_DIODE, 1, data->config.m_timeout)) return OAPC_ERROR_CUSTOM;
   if (!data->m_laserInstance->Set_W(WR_PULSE, RD_PULSE, 1, data->config.m_timeout)) return OAPC_ERROR_CUSTOM;
   if (!data->m_laserInstance->Set_W(WR_SHUTTER, RD_SHUTTER, 0, data->config.m_timeout)) return OAPC_ERROR_CUSTOM;
   if (!data->m_laserInstance->Wait_UV(data, data->config.m_timeout)) return OAPC_ERROR_CUSTOM;

   if (data->config.m_triggerMode==1) data->m_laserInstance->Set(WR_TRIGGER_MODE, 0);
   else data->m_laserInstance->Set(WR_TRIGGER_MODE, 1);
   if (data->config.m_pulseControl==1) data->m_laserInstance->Set(WR_PULSE_CONTROL, 2);
   else data->m_laserInstance->Set(WR_PULSE_CONTROL, 4);

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

   if (data->m_laserInstance)
   {
      data->m_laserInstance->Set_W(WR_SHUTTER, RD_SHUTTER, 0, data->config.m_timeout);
      delete data->m_laserInstance;
   }
   data->m_laserInstance=NULL;

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
   if (data->m_ctrl) return OAPC_ERROR_STILL_IN_PROGRESS;
   if (input==3)
   {
      struct oapc_bin_struct_ctrl *ctrl;

      data->m_ctrl=oapc_util_alloc_bin_data(OAPC_BIN_TYPE_STRUCT,OAPC_BIN_SUBTYPE_STRUCT_CTRL,OAPC_COMPRESS_NONE,sizeof(struct oapc_bin_struct_ctrl));
      if (!data->m_ctrl) return OAPC_ERROR_NO_MEMORY;
      ctrl=(struct oapc_bin_struct_ctrl*)&data->m_ctrl->data;
      ctrl->validityFlags=htonl(TOOLPARAM_VALIDFLAG_ON);
      ctrl->on=value;
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
   if (data->m_ctrl) return OAPC_ERROR_STILL_IN_PROGRESS;

   struct oapc_bin_struct_ctrl *ctrl;

   data->m_ctrl=oapc_util_alloc_bin_data(OAPC_BIN_TYPE_STRUCT,OAPC_BIN_SUBTYPE_STRUCT_CTRL,OAPC_COMPRESS_NONE,sizeof(struct oapc_bin_struct_ctrl));
   if (!data->m_ctrl) return OAPC_ERROR_NO_MEMORY;
   ctrl=(struct oapc_bin_struct_ctrl*)&data->m_ctrl->data;
   ctrl->validityFlags=htonl(validityFlags);
   ctrl->power=power;
   ctrl->frequency=frequency;
   oapc_thread_signal_send(data->m_signal);
   return OAPC_OK;
}


OAPC_EXT_API unsigned long  oapc_get_num_value(void* instanceData,unsigned long output,double *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   if (output==5)
   {
      *value=data->m_errorCode;
      return OAPC_OK;
   }
   else return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
}


OAPC_EXT_API unsigned long  oapc_get_char_value(void* instanceData,unsigned long output,unsigned long length,char *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   if (output==4)
   {
      strncpy(value,data->m_statusMessage,length);
      return OAPC_OK;
   }
   else return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
}


OAPC_EXT_API unsigned long  oapc_set_bin_value(void* instanceData,unsigned long input,struct oapc_bin_head *value)
{
   struct instData      *data;

   if (input!=7) return OAPC_ERROR_NO_SUCH_IO;
   if (value->type!=OAPC_BIN_TYPE_STRUCT) return OAPC_ERROR_INVALID_INPUT;
   data=(struct instData*)instanceData;
   if (data->m_ctrl) return OAPC_ERROR_STILL_IN_PROGRESS;
   if (value->subType==OAPC_BIN_SUBTYPE_STRUCT_CTRL)
   {
      // copy only the structure but no vector data - they aren't used here
      data->m_ctrl=oapc_util_alloc_bin_data(0,0,0,sizeof(struct oapc_bin_struct_ctrl));
      if (!data->m_ctrl) return OAPC_ERROR_NO_MEMORY;
      memcpy(data->m_ctrl,value,sizeof(struct oapc_bin_head)+sizeof(struct oapc_bin_struct_ctrl)); // copy only the head and the structure, appended vector data are not interesting for this plug-in
   }
   else if ((value->subType==OAPC_BIN_SUBTYPE_STRUCT_LASERCTRL) ||
            (value->subType==OAPC_BIN_SUBTYPE_STRUCT_MARKREADY) ||
            (value->subType==OAPC_BIN_SUBTYPE_STRUCT_ENDMARKREADY) ||
            (value->subType==OAPC_BIN_SUBTYPE_STRUCT_JOBSTART) ||
            (value->subType==OAPC_BIN_SUBTYPE_STRUCT_JOBEND))
   {
      data->m_ctrl=oapc_util_alloc_bin_data(0,0,0,value->sizeData);
      if (!data->m_ctrl) return OAPC_ERROR_NO_MEMORY;
      memcpy(data->m_ctrl,value,sizeof(struct oapc_bin_head)+value->sizeData);
   }
   else return OAPC_ERROR_INVALID_INPUT;

   oapc_thread_signal_send(data->m_signal);

   return OAPC_OK;
}

OAPC_EXT_API unsigned long  oapc_get_error_message(void* /*instanceData*/,unsigned long length,char* value) {
	strncpy( value, "Timeout. Check your AVIA limit values.", length );
	return OAPC_OK;
}
