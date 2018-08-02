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



#ifdef _MSC_VER
#pragma warning (disable: 4100)
#pragma warning (disable: 4245)
#pragma warning (disable: 4505)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
#include <sys/ioctl.h>
#endif

#ifdef ENV_QNX
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <termios.h>
#include <unistd.h>
#include <devctl.h>
#include <sys/types.h>
#include <sys/dcmd_chr.h>
#endif

#ifdef ENV_WINDOWS
 #include <windows.h>
 #define snprintf _snprintf
#endif

#include "oapc_libio.h"
#include "liboapc.h" // contains sleep function
#include "APC.h"
#include "libio_sill_focus.h"

#define MAX_XML_SIZE   7000

#define MAX_NUM_AXES 3

/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
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
 </general>\
 <stdpanel text=\"Sill\">\
  <param text=\"Factor\">\
   <name>factor</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>1</min>\
   <max>100000000</max>\
   <unit>inc/µm</unit>\
  </param>\
  <param text=\"Low Limit\">\
   <name>llimit</name>\
   <type>integer</type>\
   <min>0</min>\
   <max>2100000000</max>\
   <default>%f</default>\
   <unit>µm</unit>\
  </param>\
  <param text=\"High Limit\">\
   <name>hlimit</name>\
   <type>integer</type>\
   <min>0</min>\
   <max>2100000000</max>\
   <default>%f</default>\
   <unit>µm</unit>\
  </param>\
 </stdpanel>\
 <helppanel>\
  <in0>POS - absolute position to move to</in0>\
  <in1>MODE >0 speed, =-1 home</in1>\
  <in7>CTRL - Control data</in7>\
  <out0>POS - current absolute axis position</out0>\
  <out1>MODE >0 current speed, =0 has stopped</out1>\
  <out6>BSY - Motion active</out6>""\
 </helppanel>\
</dialogue>\n""\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyBAMAAABG0lM+AAAAMFBMVEUEAgT8/gSEgoQE/vzEwsT8/vwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACigH0vAAAACXBIWXMAAA7EAAAOxAGVKw4bAAABQ0lEQVR42u2WQU7EMAxFTabeM5ygaMQe0Qtk0/0Xiu9/FVpnQpIRTZxKrMC7Gfn119+uZaJTweKWDxbBbZmlH3i9zoKN8sv7KhIuy2SgwtN1Ugo3FXG7Yj/cCysVLirid8V++OdVKXEqAktZMU0pryJhMlF7mlKIv01laZpSw/HnKT5DUZWnc0gUn8YtqUfq/gczj2ilx7Uo79efKWqUGKZiStnqxqaUxSoK1JzzJEYUKUQ7Gh6qTlGZUkCbijJ3MYBL6tiNvM7U6tQv2KaJy7q6blRa3/0iHqiLkxuCAQ+z8xjoF9dvaJ6NM3PIaaJidtP+PPOJgmGL5hfZyy8otBt9sAFadR1/y7+wo/73fJuK39vWNeqs+mI+zLeNk8+SMt42XrCWE2W7bfA2l3UZb5vAU+Wh8bZx9Y4y3jZ5B+DcUfkFvK//LmYewDkAAAAASUVORK5CYII=";
static char                 libname[]="Sill Focus(R) Ethernet";
static char                 xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports
static struct libio_config  save_config;



/** has to be called after create instance and load data */
OAPC_EXT_API unsigned long oapc_get_config_info_data(void *instanceData,struct config_info *fillStruct)
{
   struct config_info *cfgInfo;
   struct instData    *data;

   data=(struct instData*)instanceData;

   if (!fillStruct) return OAPC_ERROR;
   cfgInfo=(struct config_info*)fillStruct;
   memset(cfgInfo,0,sizeof(struct config_info));

   cfgInfo->version=OAPC_CONFIG_INFO_VERSION;
   cfgInfo->length=sizeof(struct config_info);
   cfgInfo->configType=OAPC_CONFIG_TYPE_ZSHIFTER;

   // zshifter makes use of motion-controller structure (but only one axis of it)
   cfgInfo->motionController.version=OAPC_CONFIG_MOTION_CONTROLLER_VERSION;
   cfgInfo->motionController.length=sizeof(struct config_motion_controller);

   cfgInfo->motionController.availableAxes=0x01;
   cfgInfo->motionController.uMinPos[0]=data->config.axisConfig.llimit*1000;
   cfgInfo->motionController.uMaxPos[0]=data->config.axisConfig.hlimit*1000;
   cfgInfo->motionController.uMaxSpeed[0]=1;
   return OAPC_OK;
}


static long valueToInc(struct instData *data,double um)
{
   long newPos;

   if (um<data->config.axisConfig.llimit) um=data->config.axisConfig.llimit;
   else if (um>data->config.axisConfig.hlimit) um=data->config.axisConfig.hlimit;
   newPos=(long)OAPC_ROUND((um*data->config.axisConfig.ufactor/1000.0),0);
   return newPos;
}


static double incToValue(struct instData *data,long inc)
{
   return inc/(data->config.axisConfig.ufactor/1000.0);
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
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|
          OAPC_ACCEPTS_IO_CALLBACK|
          OAPC_FLOWCAT_MOTION;
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
   return OAPC_NUM_IO0|OAPC_NUM_IO1|
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
   return OAPC_NUM_IO0|OAPC_NUM_IO1|
          OAPC_DIGI_IO6;
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
                                  data->config.axisConfig.ufactor/1000,data->config.axisConfig.llimit/1000.0,data->config.axisConfig.hlimit/1000.0
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

   if (strcmp(name,"ip")==0)          strncpy(data->config.m_ip,value,20);
   else if (strcmp(name,"port")==0)   data->config.m_port=(unsigned short)atoi(value);
   else if (strcmp(name,"factor")==0) data->config.axisConfig.ufactor=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"llimit")==0) data->config.axisConfig.llimit=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"hlimit")==0) data->config.axisConfig.hlimit=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
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
   save_config.version    =htons(1);
   save_config.length     =htons((unsigned short)*length);

   strncpy(save_config.m_ip,data->config.m_ip,20);
   save_config.m_port                =htons(data->config.m_port);
   save_config.res1=0;
   save_config.axisConfig.ufactor    =htonl(data->config.axisConfig.ufactor);
   save_config.axisConfig.hlimit     =htonl(data->config.axisConfig.hlimit);
   save_config.axisConfig.llimit     =htonl(data->config.axisConfig.llimit);
   save_config.axisConfig.res1=0;

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
   data->config.version    =ntohs(save_config.version);
   data->config.length     =ntohs(save_config.length);

   strncpy(data->config.m_ip,save_config.m_ip,20);
   data->config.m_port                =ntohs(save_config.m_port);
   data->config.res1=0;
   data->config.axisConfig.ufactor    =ntohl(save_config.axisConfig.ufactor);
   data->config.axisConfig.hlimit     =ntohl(save_config.axisConfig.hlimit);
   data->config.axisConfig.llimit     =ntohl(save_config.axisConfig.llimit);
   data->config.axisConfig.res1=0;
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
OAPC_EXT_API void* oapc_create_instance2(unsigned long flags)
{
   flags=flags; // removing "unused" warning

   struct instData *data;

   data=(struct instData*)malloc(sizeof(struct instData));
   if (!data) return NULL;
   memset(data,0,sizeof(struct instData));

   strcpy(data->config.m_ip,PF_FOCUS_HOST);
   data->config.m_port=PF_FOCUS_PORT;
   data->config.axisConfig.ufactor=51200*1000;
   data->config.axisConfig.llimit=0;
   data->config.axisConfig.hlimit=10000*1000;
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


static void* motorLoop( void*  arg )
{
   struct instData *data;
   int              targetPos=-1,currentPos;

   data=(struct instData*)arg;

   data->m_isInitialized=true;
   data->runconfig.targetPosReached=true;
   while (data->m_running)
   {
      if (data->runconfig.targetPosReached)
      {
	     if (data->runconfig.doHome)
         {
            data->m_busy=1;
            m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
			data->runconfig.targetPosReached=false;
            data->runconfig.currentAxisSpeed=1*data->config.axisConfig.ufactor/1000;
            m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
            data->runconfig.doHome=false;
            targetPos=0;
            data->runconfig.newPos=0;
            data->m_focusInstance->Reset();
         }
	   	 else if (data->runconfig.newSpeed>0)
         {
            data->m_busy=1;
            m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
			data->runconfig.targetPosReached=false;
            data->runconfig.currentAxisSpeed=data->runconfig.newSpeed;
            m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
            data->m_focusInstance->SetPosition(data->runconfig.newPos); // start movement to requested position here
            targetPos=data->runconfig.newPos;
            data->runconfig.newPos=0;
            data->runconfig.newSpeed=0;
         }
         else oapc_thread_sleep(100); // nothing urgent to do
      }

      // get the current position
      currentPos=data->m_focusInstance->Position();

      if (data->runconfig.currPos!=currentPos) // there is some movement so update the position
      {
          data->runconfig.currPos=currentPos;
          m_oapc_io_callback(OAPC_NUM_IO0,data->m_callbackID);
      }

      // device does not support a function to check if it is still moving so we're comparing the current position here
      if ((data->runconfig.currentAxisSpeed>0) && (currentPos==targetPos)) // target position was reached
      {
         data->runconfig.targetPosReached=true;
         // update output speed, set it to 0
         data->runconfig.currentAxisSpeed=0;
         m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
         data->m_busy=0;
         m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
      }
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
   struct instData *data;
   int              i;

   data=(struct instData*)instanceData;

   try
   {
       data->m_focusInstance=new Focus(data->config.m_ip,data->config.m_port);
   }
   catch (...)
   {
       return OAPC_ERROR_DEVICE;
   }

   data->m_running=true;
   if (!oapc_thread_create(motorLoop,data)) return OAPC_ERROR_NO_MEMORY;
   for (i=0; i<20; i++) // give up to two seconds for initialization
   {
   	if (data->m_isInitialized) break;
   	oapc_thread_sleep(100);
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

   if (data->m_running)
   {
      data->m_running=false;
      while ((!data->m_running) && (ctr<10)) // wait for thread to finish
      {
         oapc_thread_sleep(250);
         ctr++;
      }
   }

   if (data->m_focusInstance) delete data->m_focusInstance;
   data->m_focusInstance=NULL;

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
   
   if (input>1) return OAPC_ERROR_NO_SUCH_IO;
   data=(struct instData*)instanceData;
   if (input==0)
   {
      if (data->runconfig.currentAxisSpeed==0)
      {
         data->runconfig.newPos=valueToInc(data,value);
         return OAPC_OK;
      }
      else return OAPC_ERROR_STILL_IN_PROGRESS;
   }
   else if (input==1)
   {
      if (value==-1) data->runconfig.doHome=true;
      else if (value>0) data->runconfig.newSpeed=1; // hardware does not support speed so set it to something >0 here
      else return OAPC_ERROR_INVALID_INPUT;
      return OAPC_OK;
   }
   return OAPC_ERROR_NO_SUCH_IO;
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
   
   if (output>1) return OAPC_ERROR_NO_SUCH_IO;
   data=(struct instData*)instanceData;
   if (output==0)
   {
      *value=incToValue(data,data->runconfig.currPos);
      return OAPC_OK;
   }
   else if (output==1)
   {
      *value=data->runconfig.currentAxisSpeed;
      return OAPC_OK;
   }
   return OAPC_ERROR_NO_SUCH_IO;
}



OAPC_EXT_API unsigned long  oapc_set_bin_value(void* instanceData,unsigned long input,struct oapc_bin_head *value)
{
   struct instData      *data;

   if (input!=7) return OAPC_ERROR_NO_SUCH_IO;
   if (value->type!=OAPC_BIN_TYPE_STRUCT) return OAPC_ERROR_INVALID_INPUT;

   data=(struct instData*)instanceData;
   if (value->subType==OAPC_BIN_SUBTYPE_STRUCT_MOTIONCTRL)
   {
      struct oapc_bin_struct_motionctrl *ctrl;
      double                             d;

      ctrl=(struct oapc_bin_struct_motionctrl*)&value->data;

      if ((ctrl->enableAxes & 0x01)==0x01)
      {
         if (data->runconfig.currentAxisSpeed==0) // it is not moving
         {
            if ((ctrl->moveAxesToHome & 0x01)==0x01) data->runconfig.doHome=true;
            else
            {
               if ((ctrl->relativeMovement & 0x01)==0x01)
               {
                  d=incToValue(data,data->runconfig.currPos);
                  d=d+(((int)ntohl(ctrl->position[0]))/1000.0);
               }
               else d=((int)ntohl(ctrl->position[0]))/1000.0;
               data->runconfig.newPos=valueToInc(data,d);
               data->runconfig.newSpeed=1; // hardware does not support speed so set it to something >0 here
            }
         }
         else return OAPC_ERROR_STILL_IN_PROGRESS;
      }
   }
   else return OAPC_ERROR_INVALID_INPUT;
   return OAPC_OK;
}


