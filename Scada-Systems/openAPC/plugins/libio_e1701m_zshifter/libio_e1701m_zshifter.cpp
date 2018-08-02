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
 #pragma warning (disable: 4244)
 #pragma warning (disable: 4100)
 #pragma warning (disable: 4616)
 #pragma warning (disable: 4018)
 #pragma warning (disable: 4663)
 #pragma warning (disable: 4710)
#endif

#ifdef _DEBUG
// #define TEST_MODE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#ifndef ENV_WINDOWSCE
 #include <sys/types.h>
#else
 #define STRSAFE_NO_DEPRECATE
 #define CONST const
 #include <windows.h>
 #include "../plugins/libio_clock/time_ce.h"
#endif

#ifndef ENV_WINDOWS
 #include <sys/io.h>
 #include <arpa/inet.h>
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
#else
 #include <winsock2.h>
 #define snprintf _snprintf
#endif

#ifdef ENV_LINUX
 #include <termios.h>
 #include <unistd.h>
#endif

#ifdef ENV_QNX
 #include <hw/inout.h>
 #include <sys/mman.h>
#endif

#include "oapc_libio.h"
#include "liboapc.h"
#include "libe1701m.h"

#include <queue>

#define MAX_XML_SIZE    20000
#define MAX_FILENAMESIZE  255

#define POS_UPDATE_TIME 250 //msec
#define DATA_LENGTH      48 //defined by contoller-firmware internal limitations

#define AXISFLAG_DIRVALUE  0x01 // invert direction signal
#define AXISFLAG_REFVALUE  0x02 // invert reference switch input logic
#define AXISFLAG_LIMVALUEA 0x04 // invert limit switch A input logic
#define AXISFLAG_LIMVALUEB 0x08 // invert limit switch B input logic

struct axis_config
{
   int           llimit,hlimit,factor;
   int           mAccel,mDecel,mStopDecel,res1,refspeedin,refspeedin2nd,refspeedin3rd,refpos; // int - 32 bit
   char          refmode,refin,flags,autoleavelim,m_accelmode,m_decelmode;
   unsigned int  m_axisMaxSpeed;
   unsigned char limitain,limitbin;
   unsigned char m_axisHomeTimeout;
   unsigned char m_useenc;
   unsigned int  m_mEncfactor;
};

struct instData;

struct axis_runconfig
{
   bool                running,moveToPos,doHome,doStop;
   double              newSpeed,currentAxisSpeed;
   long                currPos,newPos,lastPos;
   struct instData    *data;
   time_t              m_refTimeoutTime;
   time_t              m_refEndTime;
   void               *m_signal;              
};



struct libio_config
{
   unsigned short     version,length;
   unsigned short     reserved;
   struct axis_config axisConfig;
   char               m_ip[24];
   char               m_passwd[DATA_LENGTH];
   char               m_fwlogfile_unused[MAX_FILENAMESIZE+4];
};



struct instData
{
   struct libio_config                config;
   struct axis_runconfig              runconfig;
   int                                m_callbackID;
   unsigned char                      m_busy;
   bool                               running;
   unsigned char                      m_boardNum;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<oapc-config>\
<flowimage>%s</flowimage>\
<dialogue>\
 <general>\
  <param text=\"IP or serial interface\">\
   <name>ip</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>16</max>\
  </param>\
  <param text=\"Ethernet password\">\
   <name>passwd</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
  </param>\
 </general>\
 <stdpanel>\
  <name>Axis</name>\
  <param text=\"Resolution Factor\">\
   <name>factor1</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>1</min>\
   <max>100000</max>\
   <unit>inc/mm</unit>\
  </param>\
  <param text=\"Low Limit\">\
   <name>llimit1</name>\
   <type>integer</type>\
   <min>-2100000000</min>\
   <max>2100000000</max>\
   <default>%d</default>\
   <unit>mm</unit>\
  </param>\
  <param text=\"High Limit\">\
   <name>hlimit1</name>\
   <type>integer</type>\
   <min>-2100000000</min>\
   <max>2100000000</max>\
   <default>%d</default>\
   <unit>mm</unit>\
  </param>\
  <param text=\"Maximum Speed\">\
   <name>maxspeed1</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec</unit>\
  </param>\
  <param text=\"Acceleration Mode\">\
   <name>accelmode1</name>\
   <type>option</type>\
   <value>linear</value>\
   <value>exponential</value>\
   <value>s-shaped</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Acceleration\">\
   <name>accel1</name>\
   <type>float</type>""\
   <min>0</min>\
   <max>1000</max>\
   <default>%f</default>\
  </param>\
  <param text=\"Deceleration Mode\">\
   <name>decelmode1</name>\
   <type>option</type>\
   <value>linear</value>\
   <value>exponential</value>\
   <value>s-shaped</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Deceleration\">\
   <name>decel1</name>\
   <type>float</type>\
   <min>0</min>\
   <max>1000</max>\
   <default>%f</default>\
  </param>\
  <param text=\"Stop Deceleration\">\
   <name>stopdecel1</name>\
   <type>float</type>\
   <min>0</min>\
   <max>1000</max>\
   <default>%f</default>\
  </param>\
  <param text=\"Invert Direction\">\
   <name>dirvalue1</name>\
   <type>checkbox</type>\
   <default>%d</default>\
  </param>\
  <param text=\"Reference Mode\">\
   <name>refmode1</name>\n""\
   <type>option</type>\
   <value>No referencing</value>\
   <value>To switch in negative direction</value>\
   <value>To switch in positive direction</value>\
   <value>To switch in negative, leave in positive direction</value>\
   <value>To switch in positive, leave in negative direction</value>\
   <value>To switch in negative, leave in negative direction</value>\
   <value>To switch in positive, leave in positive direction</value>\
   <value>To switch in negative, to switch in positive, leave in negative direction</value>\
   <value>To switch in positive, to switch in negative, leave in positive direction</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Reference Timeout\">\
   <name>reftimeout1</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>250</max>\
   <default>%d</default>\
   <unit>sec</unit>\
  </param>\
  <param text=\"Reference Signal Input\">\
   <name>refin1</name>\
   <type>option</type>\
   <value>DIGI IN0</value>\
   <value>DIGI IN1</value>\
   <value>DIGI IN2</value>\
   <value>DIGI IN3</value>\
   <value>DIGI IN4</value>\
   <value>DIGI IN5</value>\
   <value>DIGI IN6</value>\
   <value>DIGI IN7</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Invert Input Logic\">\
   <name>refvalue1</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Reference Speed 1\">\
   <name>refspeedin1</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec</unit>\
  </param>\
  <param text=\"Reference Speed 2\">\
   <name>refspeedin2nd1</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec</unit>\
  </param>\
  <param text=\"Reference Speed 3\">\
   <name>refspeedin3rd1</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec</unit>\
  </param>\
  <param text=\"Referenced Position\">\
   <name>refpos1</name>\
   <type>float</type>\
   <min>-2100000000</min>\
   <max>2100000000</max>\
   <default>%f</default>\
   <unit>mm</unit>\
  </param>\
   <param text=\"Limit Switch (+) Input\">\
   <name>limitain1</name>\
   <type>option</type>\
   <value>DIGI IN0</value>\
   <value>DIGI IN1</value>""\
   <value>DIGI IN2</value>\
   <value>DIGI IN3</value>\
   <value>DIGI IN4</value>\
   <value>DIGI IN5</value>\
   <value>DIGI IN6</value>\
   <value>DIGI IN7</value>\
   <value>unused</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Invert Input Logic\">\
   <name>limvaluea1</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable1</enableon>\
  </param>\
   <param text=\"Limit Switch (-) Input\">\
   <name>limitbin1</name>\
   <type>option</type>\
   <value>DIGI IN0</value>\
   <value>DIGI IN1</value>\
   <value>DIGI IN2</value>\
   <value>DIGI IN3</value>\
   <value>DIGI IN4</value>\
   <value>DIGI IN5</value>\
   <value>DIGI IN6</value>\
   <value>DIGI IN7</value>\
   <value>unused</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Invert Input Logic\">\
   <name>limvalueb1</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Auto-leave Limit Switch\">\
   <name>autoleavelim1</name>\
   <type>checkbox</type>\
   <default>%d</default>\
  </param>\
  <param text=\"Encoder\">\
   <name>useenc1</name>\
   <type>option</type>\
   <value>0 (DIGI IN0/1)</value>\
   <value>1 (DIGI IN2/3)</value>\
   <value>None</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Encoder Factor\">\
   <name>encfactor1</name>\
   <type>float</type>\
   <default>%f</default>\
   <min>0.0001</min>\
   <max>100000</max>\
   <unit>steps/inc</unit>\
  </param>\
 </stdpanel>\
 <helppanel>\
  <in0>POS - absolute position to move to</in0>\
  <in1>MODE >0 speed, =0 stop, =-1 home</in1>\n\
  <in7>CTRL - Control data</in7>\
  <out0>POS - current absolute axis position</out0>\
  <out1>MODE >0 current speed, =0 has stopped</out1>\
  <out6>BSY - Motion active</out6>\
 </helppanel>\
</dialogue>\
</oapc-config>\n";

/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyBAMAAABG0lM+AAAAMFBMVEUEAgT8/gSEgoQE/vzEwsT8/vwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACigH0vAAAACXBIWXMAAA7EAAAOxAGVKw4bAAABQ0lEQVR42u2WQU7EMAxFTabeM5ygaMQe0Qtk0/0Xiu9/FVpnQpIRTZxKrMC7Gfn119+uZaJTweKWDxbBbZmlH3i9zoKN8sv7KhIuy2SgwtN1Ugo3FXG7Yj/cCysVLirid8V++OdVKXEqAktZMU0pryJhMlF7mlKIv01laZpSw/HnKT5DUZWnc0gUn8YtqUfq/gczj2ilx7Uo79efKWqUGKZiStnqxqaUxSoK1JzzJEYUKUQ7Gh6qTlGZUkCbijJ3MYBL6tiNvM7U6tQv2KaJy7q6blRa3/0iHqiLkxuCAQ+z8xjoF9dvaJ6NM3PIaaJidtP+PPOJgmGL5hfZyy8otBt9sAFadR1/y7+wo/73fJuK39vWNeqs+mI+zLeNk8+SMt42XrCWE2W7bfA2l3UZb5vAU+Wh8bZx9Y4y3jZ5B+DcUfkFvK//LmYewDkAAAAASUVORK5CYII=";
static char                 libname[]="E1701M Focus Shifter Controller";
static char                 xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports
static struct libio_config  save_config;



static long valueToInc(struct instData *data,double mm)
{
   long newPos;

   if (mm<data->config.axisConfig.llimit) mm=data->config.axisConfig.llimit;
   else if (mm>data->config.axisConfig.hlimit) mm=data->config.axisConfig.hlimit;
   newPos=(long)OAPC_ROUND((mm*data->config.axisConfig.factor),0);
   if (newPos==0) newPos=1;
   return newPos;
}



static unsigned int speedValueToInc(struct instData *data,const double spd,const bool checkMaxSpeed)
{
   unsigned long inc;

   inc=(unsigned int)OAPC_ROUND(spd*data->config.axisConfig.factor,0);
   if ((checkMaxSpeed) && (inc>data->config.axisConfig.m_axisMaxSpeed)) inc=data->config.axisConfig.m_axisMaxSpeed;
   if (inc<=2) inc=2;
   return inc;
}



static double incToValue(struct instData *data,long inc)
{
   return (1.0*inc)/data->config.axisConfig.factor;
}



static double speedIncToValue(struct instData *data,double inc)
{
   return (1.0*inc)/data->config.axisConfig.factor;
}



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



/**
This function has to return the name that is used to display this library within the main application.
The name returned here has to be short and descriptive. It is displayed within the popup menu of the flow
editor, therefore it should not be too long!
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
                     data->config.m_ip,data->config.m_passwd,DATA_LENGTH-1,
                     data->config.axisConfig.factor,
                     data->config.axisConfig.llimit,data->config.axisConfig.hlimit,
                     speedIncToValue(data,data->config.axisConfig.m_axisMaxSpeed),
                     data->config.axisConfig.m_accelmode,data->config.axisConfig.mAccel/1000.0,
                     data->config.axisConfig.m_decelmode,data->config.axisConfig.mDecel/1000.0,data->config.axisConfig.mStopDecel/1000.0,
                     (data->config.axisConfig.flags & AXISFLAG_DIRVALUE)!=0,
                     data->config.axisConfig.refmode,
                     data->config.axisConfig.m_axisHomeTimeout,
                     data->config.axisConfig.refin,
                     (data->config.axisConfig.flags & AXISFLAG_REFVALUE)!=0,
                     speedIncToValue(data,data->config.axisConfig.refspeedin),
                     speedIncToValue(data,data->config.axisConfig.refspeedin2nd),
                     speedIncToValue(data,data->config.axisConfig.refspeedin3rd),
                     incToValue(data,data->config.axisConfig.refpos),
                     data->config.axisConfig.limitain,(data->config.axisConfig.flags & AXISFLAG_LIMVALUEA)!=0,
                     data->config.axisConfig.limitbin,(data->config.axisConfig.flags & AXISFLAG_LIMVALUEB)!=0,
                     data->config.axisConfig.autoleavelim,
                     data->config.axisConfig.m_useenc,data->config.axisConfig.m_mEncfactor/1000.0
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
   else if (strcmp(name,"passwd")==0)        strncpy(data->config.m_passwd,value,DATA_LENGTH);

   else if (strcmp(name,"factor1")==0)        data->config.axisConfig.factor=atoi(value);
   else if (strcmp(name,"llimit1")==0)        data->config.axisConfig.llimit=atoi(value);
   else if (strcmp(name,"hlimit1")==0)        data->config.axisConfig.hlimit=atoi(value);
   else if (strcmp(name,"accel1")==0)         data->config.axisConfig.mAccel=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"decel1")==0)         data->config.axisConfig.mDecel=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"accelmode1")==0)     data->config.axisConfig.m_accelmode=(char)atoi(value);
   else if (strcmp(name,"decelmode1")==0)     data->config.axisConfig.m_decelmode=(char)atoi(value);
   else if (strcmp(name,"stopdecel1")==0)     data->config.axisConfig.mStopDecel=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"maxspeed1")==0)      data->config.axisConfig.m_axisMaxSpeed=speedValueToInc(data,oapc_util_atof(value),false);
   else if (strcmp(name,"reftimeout1")==0)    data->config.axisConfig.m_axisHomeTimeout=(unsigned char)atoi(value);
   else if (strcmp(name,"dirvalue1")==0)      
   {
      if (atoi(value)) data->config.axisConfig.flags|=AXISFLAG_DIRVALUE;
      else data->config.axisConfig.flags&=~AXISFLAG_DIRVALUE;
   }
   else if (strcmp(name,"refvalue1")==0)      
   {
      if (atoi(value)) data->config.axisConfig.flags|=AXISFLAG_REFVALUE;
      else data->config.axisConfig.flags&=~AXISFLAG_REFVALUE;
   }
   else if (strcmp(name,"limvaluea1")==0)      
   {
      if (atoi(value)) data->config.axisConfig.flags|=AXISFLAG_LIMVALUEA;
      else data->config.axisConfig.flags&=~AXISFLAG_LIMVALUEA;
   }
   else if (strcmp(name,"limvalueb1")==0)      
   {
      if (atoi(value)) data->config.axisConfig.flags|=AXISFLAG_LIMVALUEB;
      else data->config.axisConfig.flags&=~AXISFLAG_LIMVALUEB;
   }
   else if (strcmp(name,"autoleavelim1")==0)  data->config.axisConfig.autoleavelim=(char)atoi(value);
   else if (strcmp(name,"refmode1")==0)       data->config.axisConfig.refmode=(char)atoi(value);
   else if (strcmp(name,"refin1")==0)         data->config.axisConfig.refin=(char)atoi(value);
   else if (strcmp(name,"refspeedin1")==0)    data->config.axisConfig.refspeedin=speedValueToInc(data,oapc_util_atof(value),true);
   else if (strcmp(name,"refspeedin2nd1")==0) data->config.axisConfig.refspeedin2nd=speedValueToInc(data,oapc_util_atof(value),true);
   else if (strcmp(name,"refspeedin3rd1")==0) data->config.axisConfig.refspeedin3rd=speedValueToInc(data,oapc_util_atof(value),true);
   else if (strcmp(name,"refpos1")==0)        data->config.axisConfig.refpos=valueToInc(data,oapc_util_atof(value));
   else if (strcmp(name,"limitain1")==0)      data->config.axisConfig.limitain=(unsigned char)atoi(value);
   else if (strcmp(name,"limitbin1")==0)      data->config.axisConfig.limitbin=(unsigned char)atoi(value);
   else if (strcmp(name,"useenc1")==0)        data->config.axisConfig.m_useenc=(char)atoi(value);
   else if (strcmp(name,"encfactor1")==0)     data->config.axisConfig.m_mEncfactor=(unsigned int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
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
   save_config.length     =htons(sizeof(struct libio_config));
   save_config.reserved   =htons(data->config.reserved);
   strncpy(save_config.m_ip,data->config.m_ip,20);
   strncpy(save_config.m_passwd,data->config.m_passwd,DATA_LENGTH);

   save_config.axisConfig.mAccel      =htonl(data->config.axisConfig.mAccel);
   save_config.axisConfig.mDecel      =htonl(data->config.axisConfig.mDecel);
   save_config.axisConfig.m_accelmode =data->config.axisConfig.m_accelmode;
   save_config.axisConfig.m_decelmode =data->config.axisConfig.m_decelmode;
   save_config.axisConfig.mStopDecel  =htonl(data->config.axisConfig.mStopDecel);
   save_config.axisConfig.res1        =0;
   save_config.axisConfig.flags       =data->config.axisConfig.flags;
   save_config.axisConfig.autoleavelim=data->config.axisConfig.autoleavelim;
   save_config.axisConfig.factor      =htonl(data->config.axisConfig.factor);
   save_config.axisConfig.hlimit      =htonl(data->config.axisConfig.hlimit);
   save_config.axisConfig.llimit      =htonl(data->config.axisConfig.llimit);
   save_config.axisConfig.m_axisMaxSpeed=htonl(data->config.axisConfig.m_axisMaxSpeed);
   save_config.axisConfig.refin       =data->config.axisConfig.refin;
   save_config.axisConfig.refmode     =data->config.axisConfig.refmode;
   save_config.axisConfig.refpos      =htonl(data->config.axisConfig.refpos);
   save_config.axisConfig.limitain    =data->config.axisConfig.limitain;
   save_config.axisConfig.limitbin    =data->config.axisConfig.limitbin;
   save_config.axisConfig.refspeedin       =htonl(data->config.axisConfig.refspeedin);
   save_config.axisConfig.refspeedin2nd    =htonl(data->config.axisConfig.refspeedin2nd);
   save_config.axisConfig.refspeedin3rd    =htonl(data->config.axisConfig.refspeedin3rd);
   save_config.axisConfig.m_axisHomeTimeout=data->config.axisConfig.m_axisHomeTimeout;
   save_config.axisConfig.m_useenc         =data->config.axisConfig.m_useenc;
   save_config.axisConfig.m_mEncfactor     =htonl(data->config.axisConfig.m_mEncfactor);

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
   data->config.reserved   =ntohs(save_config.reserved);
   strncpy(data->config.m_ip,save_config.m_ip,20);
   strncpy(data->config.m_passwd,save_config.m_passwd,DATA_LENGTH);

   data->config.axisConfig.mAccel      =ntohl(save_config.axisConfig.mAccel);
   data->config.axisConfig.mDecel      =ntohl(save_config.axisConfig.mDecel);
   data->config.axisConfig.m_accelmode =save_config.axisConfig.m_accelmode;
   data->config.axisConfig.m_decelmode =save_config.axisConfig.m_decelmode;
   data->config.axisConfig.mStopDecel  =ntohl(save_config.axisConfig.mStopDecel);
   data->config.axisConfig.res1        =0;
   data->config.axisConfig.flags       =save_config.axisConfig.flags;
   data->config.axisConfig.autoleavelim=save_config.axisConfig.autoleavelim;

   data->config.axisConfig.factor=ntohl(save_config.axisConfig.factor);
   if (data->config.axisConfig.factor<=0) data->config.axisConfig.factor=123;

   data->config.axisConfig.hlimit=ntohl(save_config.axisConfig.hlimit);
   data->config.axisConfig.llimit=ntohl(save_config.axisConfig.llimit);
   data->config.axisConfig.refin=save_config.axisConfig.refin;
   data->config.axisConfig.refmode=save_config.axisConfig.refmode;
   data->config.axisConfig.refpos  =ntohl(save_config.axisConfig.refpos);
   data->config.axisConfig.limitain=save_config.axisConfig.limitain;
   data->config.axisConfig.limitbin=save_config.axisConfig.limitbin;
   data->config.axisConfig.refspeedin=ntohl(save_config.axisConfig.refspeedin);
   data->config.axisConfig.refspeedin2nd=ntohl(save_config.axisConfig.refspeedin2nd);
   data->config.axisConfig.refspeedin3rd=ntohl(save_config.axisConfig.refspeedin3rd);
   data->config.axisConfig.m_axisMaxSpeed=ntohl(save_config.axisConfig.m_axisMaxSpeed);
   data->config.axisConfig.m_axisHomeTimeout=save_config.axisConfig.m_axisHomeTimeout;

   data->config.axisConfig.m_useenc         =save_config.axisConfig.m_useenc;
   data->config.axisConfig.m_mEncfactor     =ntohl(save_config.axisConfig.m_mEncfactor);
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
   if (!data) return NULL;
   memset(data,0,sizeof(struct instData));

   data->config.version=1;
   data->config.length=sizeof(struct libio_config);
   data->config.reserved=0;
   strcpy(data->config.m_ip,"192.168.2.254");

   data->config.axisConfig.refin=3;
   data->config.axisConfig.mAccel=4*1000;
   data->config.axisConfig.mDecel=9*1000;
   data->config.axisConfig.m_accelmode=1;
   data->config.axisConfig.m_decelmode=1;
   data->config.axisConfig.mStopDecel=40*1000;

   data->config.axisConfig.m_axisHomeTimeout=120;
   data->config.axisConfig.m_axisMaxSpeed=2000000;

   data->config.axisConfig.factor=300;
   data->config.axisConfig.llimit=-100000; data->config.axisConfig.hlimit=100000;
   data->config.axisConfig.refmode=1;      
   data->config.axisConfig.refspeedin=300; data->config.axisConfig.refspeedin2nd=150; data->config.axisConfig.refspeedin3rd=80;
   data->config.axisConfig.limitain=9;
   data->config.axisConfig.limitbin=9;
   data->config.axisConfig.m_useenc=3;
   data->config.axisConfig.m_mEncfactor=1*1000;
   data->runconfig.running=true;
   data->runconfig.newSpeed=300;

   data->config.axisConfig.refin=1;
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


static void checkAxisData(struct instData *data,struct axis_runconfig *myParam,bool checkPositionOnly)
{
   long currPos;

   E1701M_get_axis_pos2(data->m_boardNum,0,&currPos);
   if ((data->config.axisConfig.flags & AXISFLAG_DIRVALUE)!=0) myParam->currPos=-currPos;
   else myParam->currPos=currPos;

   if (data->runconfig.currPos!=data->runconfig.lastPos)
   {
      m_oapc_io_callback(OAPC_NUM_IO0,data->m_callbackID);
      data->runconfig.lastPos=data->runconfig.currPos;
   }

   if (!checkPositionOnly)
   {
      myParam->currentAxisSpeed=E1701M_get_axis_speed(data->m_boardNum,0);
      m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
   }
}


static bool checkStop(struct instData *data,struct axis_runconfig *myParam)
{
   if (!myParam->doStop) return false;
   E1701M_stop(data->m_boardNum,(unsigned char)(1));
   myParam->doStop=0;

   // wait until movement has finished
   while (myParam->running)
   {
       if ((E1701M_get_axis_state(data->m_boardNum,0) & E1701M_AXIS_STATE_MOVING)==0) break;
       oapc_util_thread_sleep(10);
   }


   return true;
}


static void leaveLimitSwitch(struct instData *data,struct axis_runconfig *myParam,long prevPos)
{
   unsigned char stopCondEnter=0,stopCondLeave=0;

   if (data->config.axisConfig.limitain<=8)
   {
      if ((data->config.axisConfig.flags & AXISFLAG_LIMVALUEA)==0) stopCondEnter=(unsigned char)(1 << (data->config.axisConfig.limitain-1));
      else stopCondLeave=(unsigned char)(1 << (data->config.axisConfig.limitain-1));
   }
   if (data->config.axisConfig.limitbin<=8)
   {
      if ((data->config.axisConfig.flags & AXISFLAG_LIMVALUEB)==0) stopCondEnter|=(unsigned char)(1 << (data->config.axisConfig.limitbin-1));
      else stopCondLeave|=(unsigned char)(1 << (data->config.axisConfig.limitbin-1));
   }

   if ((stopCondEnter==0) && (stopCondLeave==0))
   {
      // we should never get here when no limit switches are set
      assert(0);
      return;
   }

   // stop when limit switches are free
   E1701M_set_stopcond(data->m_boardNum,0,stopCondLeave,stopCondEnter);

   // move to position we came from
   if ((data->config.axisConfig.flags & AXISFLAG_DIRVALUE)!=0) E1701M_move_abs(data->m_boardNum,1,-prevPos);
   else E1701M_move_abs(data->m_boardNum,1,prevPos);

   // wait until it starts moving
   while (myParam->running)
   {
      if (checkStop(data,myParam)) break;
      if ((E1701M_get_axis_state(data->m_boardNum,0) & E1701M_AXIS_STATE_MOVING)==E1701M_AXIS_STATE_MOVING) break;
      oapc_thread_sleep(10);
   }

   // wait until movement has stopped
   while (myParam->running)
   {
      if (checkStop(data,myParam)) break;
      if ((E1701M_get_axis_state(data->m_boardNum,0) & E1701M_AXIS_STATE_MOVING)==0) break;
      oapc_util_thread_sleep(10);
   }

   // reset to old condition
   E1701M_set_stopcond(data->m_boardNum,0,stopCondEnter,stopCondLeave);
}


extern "C"
{
static void* motorLoop( void*  arg )
{
   unsigned char                sendCnt;
   struct axis_runconfig       *myParam;
   struct instData             *data;
   unsigned char                prevBusy=0;
   long                         prevPos;

   myParam=(struct axis_runconfig*)arg;
   data=myParam->data;

   while (myParam->running)
   {
      checkStop(data,myParam); // workaround: sometimes stopped-event is received before axis is stopped and therefore does not check for stop in loop
      E1701M_get_axis_state(data->m_boardNum,0); // same workaround: sometimes there are more state-changes in queue than there should be (network connection only)
      checkAxisData(data,myParam,true); // check if position has changed meanwhile (may be detected when encoders are used for axes)
      if (myParam->moveToPos)
      {
         data->m_busy=1;
         m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
         myParam->moveToPos=0;

         prevPos=myParam->currPos;

         long currPos;
         
         E1701M_get_axis_pos2(data->m_boardNum,0,&currPos);
         if((data->config.axisConfig.flags & AXISFLAG_DIRVALUE)!=0) myParam->currPos=-currPos;
         else myParam->currPos=currPos;

/*         if ((((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) && (myParam->currPos!=-myParam->newPos)) ||
             (((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)==0) && (myParam->currPos!=myParam->newPos)))*/
         if (myParam->currPos!=myParam->newPos)
         {
            unsigned int lastAxisState=0;

            E1701M_set_speed(data->m_boardNum,1,myParam->newSpeed);
            if ((data->config.axisConfig.flags & AXISFLAG_DIRVALUE)==0) E1701M_move_abs(data->m_boardNum,1,-myParam->newPos);
            else E1701M_move_abs(data->m_boardNum,1,myParam->newPos);

            // wait until it starts moving
            while (myParam->running)
            {
               if (checkStop(data,myParam)) break;
               if ((E1701M_get_axis_state(data->m_boardNum,0) & E1701M_AXIS_STATE_MOVING)==E1701M_AXIS_STATE_MOVING) break;
               oapc_thread_sleep(10);
            }
            // wait movement has finished
            sendCnt=0;
            while (myParam->running)
            {
               if (checkStop(data,myParam)) break;
               lastAxisState=E1701M_get_axis_state(data->m_boardNum,0);
               if ((lastAxisState & E1701M_AXIS_STATE_MOVING)==0) break;
               oapc_util_thread_sleep(10);
               if (sendCnt>25)
               {
                  checkAxisData(data,myParam,false);
                  sendCnt=0;
               }
               sendCnt++;
            }
            if ((data->config.axisConfig.autoleavelim) &&
                ((lastAxisState & E1701M_AXIS_STATE_CONDSTOP)==E1701M_AXIS_STATE_CONDSTOP))
            {
                leaveLimitSwitch(data,myParam,prevPos);
            }

            checkAxisData(data,myParam,false);
            myParam->currentAxisSpeed=0;            
         }
         else oapc_thread_sleep(250);
         data->m_busy=0;
         m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
      }
      else if (myParam->doHome)
      {
         unsigned int refFlags=0;
         time_t       homeStopTime=time(NULL)+data->config.axisConfig.m_axisHomeTimeout;
         bool         refOK=true;

         myParam->doHome=0;
         data->m_busy=1;
         m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);

         if (data->config.axisConfig.refmode>1)
         {
            switch (data->config.axisConfig.refmode)
            {
               case 1:
                  if ((data->config.axisConfig.flags & AXISFLAG_DIRVALUE)==0) refFlags=E1701M_REFSTEP_1_ENTER_P;
                  else refFlags=E1701M_REFSTEP_1_ENTER_N;
                  break;
               case 2:
                  if ((data->config.axisConfig.flags & AXISFLAG_DIRVALUE)==0) refFlags=E1701M_REFSTEP_1_ENTER_N;
                  else refFlags=E1701M_REFSTEP_1_ENTER_P;
                  break;
               case 3:
                  if ((data->config.axisConfig.flags & AXISFLAG_DIRVALUE)==0) refFlags=E1701M_REFSTEP_1_ENTER_P|E1701M_REFSTEP_2_LEAVE_N;
                  else refFlags=E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_LEAVE_P;
                  break;
               case 4:
                  if ((data->config.axisConfig.flags & AXISFLAG_DIRVALUE)==0) refFlags=E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_LEAVE_P;
                  else refFlags=E1701M_REFSTEP_1_ENTER_P|E1701M_REFSTEP_2_LEAVE_N;
                  break;
               case 5:
                  if ((data->config.axisConfig.flags & AXISFLAG_DIRVALUE)==0) refFlags=E1701M_REFSTEP_1_ENTER_P|E1701M_REFSTEP_2_LEAVE_N;
                  else refFlags=E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_LEAVE_P;
                  break;
               case 6:
                  if ((data->config.axisConfig.flags & AXISFLAG_DIRVALUE)==0) refFlags=E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_LEAVE_P;
                  else refFlags=E1701M_REFSTEP_1_ENTER_P|E1701M_REFSTEP_2_LEAVE_N;
                  break;
               case 7:
                  if ((data->config.axisConfig.flags & AXISFLAG_DIRVALUE)==0) refFlags=E1701M_REFSTEP_1_ENTER_P|E1701M_REFSTEP_2_LEAVE_P;
                  else refFlags=E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_LEAVE_N;
                  break;
               case 8:
                  if ((data->config.axisConfig.flags & AXISFLAG_DIRVALUE)==0) refFlags=E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_LEAVE_N;
                  else refFlags=E1701M_REFSTEP_1_ENTER_P|E1701M_REFSTEP_2_LEAVE_P;
                  break;
               case 9:
                  if ((data->config.axisConfig.flags & AXISFLAG_DIRVALUE)==0) refFlags=E1701M_REFSTEP_1_ENTER_P|E1701M_REFSTEP_2_ENTER_N|E1701M_REFSTEP_3_LEAVE_P;
                  else refFlags=E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_ENTER_P|E1701M_REFSTEP_3_LEAVE_N;
                  break;
               case 10:
                  if ((data->config.axisConfig.flags & AXISFLAG_DIRVALUE)==0) refFlags=E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_ENTER_P|E1701M_REFSTEP_3_LEAVE_N;
                  else refFlags=E1701M_REFSTEP_1_ENTER_P|E1701M_REFSTEP_2_ENTER_N|E1701M_REFSTEP_3_LEAVE_P;
                  break;
               default:
                  assert(0);
                  break;
            }
            if ((data->config.axisConfig.flags & AXISFLAG_REFVALUE)!=0) refFlags|=E1701M_REFSTEP_INV_SWITCH;
            E1701M_reference(data->m_boardNum,1,refFlags,(unsigned char)(1 << (data->config.axisConfig.refin-1)),
                             data->config.axisConfig.refspeedin,
                             data->config.axisConfig.refspeedin2nd,
                             data->config.axisConfig.refspeedin3rd,
                             0);
            // wait until it starts moving
            while (myParam->running)
            {
               if (time(NULL)>homeStopTime) myParam->doStop=1;
               if (checkStop(data,myParam))
               {
                  refOK=false;
                  break;
               }
               if ((E1701M_get_axis_state(data->m_boardNum,0) & E1701M_AXIS_STATE_REFERENCING)==E1701M_AXIS_STATE_REFERENCING) break;
               oapc_thread_sleep(10);
            }
            checkAxisData(data,myParam,false);

            // wait movement has finished
            sendCnt=0;
            while (myParam->running)
            {
               if (time(NULL)>homeStopTime) myParam->doStop=1;
               if (checkStop(data,myParam))
               {
                  refOK=false;
                  break;
               }
               if ((E1701M_get_axis_state(data->m_boardNum,0) & (E1701M_AXIS_STATE_REFERENCING|E1701M_AXIS_STATE_MOVING))==0) break;
               oapc_util_thread_sleep(10);
               if (sendCnt>25)
               {
                  checkAxisData(data,myParam,false);
                  sendCnt=0;
               }
               sendCnt++;
            } 
            if (refOK) E1701M_set_pos(data->m_boardNum,1,valueToInc(data,data->config.axisConfig.refpos));
         }
         // for reference mode 1 - no referencing, just set new value
         else E1701M_set_pos(data->m_boardNum,1,valueToInc(data,data->config.axisConfig.refpos));

         // wait until new position was written only when no stop was received
         if (refOK) while (myParam->running)
         {
            if (time(NULL)>homeStopTime) myParam->doStop=1;
            if (checkStop(data,myParam))
            {
               refOK=false;
               break;
            }
            if ((E1701M_get_axis_state(data->m_boardNum,0) & E1701M_AXIS_STATE_SETPOS)==E1701M_AXIS_STATE_SETPOS) break;
            oapc_thread_sleep(10);
         }
         data->m_busy=0;
         checkAxisData(data,myParam,false);
         myParam->currentAxisSpeed=0;

         m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
      }
      else
      {
         if (prevBusy!=0) 
         {
            oapc_thread_sleep(0);
         }
         else
         {
            oapc_thread_signal_wait(myParam->m_signal,250);

            long currPos;
            
            E1701M_get_axis_pos2(data->m_boardNum,0,&currPos);
            if((data->config.axisConfig.flags & AXISFLAG_DIRVALUE)!=0) myParam->currPos=-currPos;
            else myParam->currPos=currPos;

         }
      }
   }
   // end while (myParam->running)
   myParam->running=true;
   return 0;
}
}


/**
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API unsigned long oapc_init(void* instanceData)
{
   struct instData *data;
   unsigned char    stopOnEnter=0,stopOnLeave=0;
   void            *threadHandle;

   data=(struct instData*)instanceData;

   data->m_boardNum=E1701M_set_connection(data->config.m_ip);
   E1701M_set_password(data->m_boardNum,data->config.m_passwd);

   if (E1701M_open_connection(data->m_boardNum)!=E1701M_OK) return OAPC_ERROR_CONNECTION;
   E1701M_stop(data->m_boardNum,E1701M_AXIS_0|E1701M_AXIS_1|E1701M_AXIS_2|E1701M_AXIS_3);

   E1701M_set_accels(data->m_boardNum,1,data->config.axisConfig.mAccel/1000.0,
                     data->config.axisConfig.mDecel/1000.0,
                     data->config.axisConfig.mStopDecel/1000.0);
   E1701M_set_limits(data->m_boardNum,1,valueToInc(data,data->config.axisConfig.llimit),
                     valueToInc(data,data->config.axisConfig.hlimit),
                     speedValueToInc(data,data->config.axisConfig.m_axisMaxSpeed,false));
   if (data->config.axisConfig.limitain<=8)
   {
      if ((data->config.axisConfig.flags & AXISFLAG_LIMVALUEA)==0) stopOnEnter=(unsigned char)(1 << (data->config.axisConfig.limitain-1));
      else stopOnLeave=(unsigned char)(1 << (data->config.axisConfig.limitain-1));
   }
   if (data->config.axisConfig.limitbin<=8)
   {
      if ((data->config.axisConfig.flags & AXISFLAG_LIMVALUEB)==0) stopOnEnter|=(unsigned char)(1 << (data->config.axisConfig.limitbin-1));
      else stopOnLeave|=(unsigned char)(1 << (data->config.axisConfig.limitbin-1));
   }
   E1701M_set_stopcond(data->m_boardNum,1,stopOnEnter,stopOnLeave);

   if (data->config.axisConfig.m_useenc==1) E1701M_set_enc(data->m_boardNum,0,0,data->config.axisConfig.m_mEncfactor/1000.0);
   else if (data->config.axisConfig.m_useenc==2) E1701M_set_enc(data->m_boardNum,0,1,data->config.axisConfig.m_mEncfactor/1000.0);

   E1701M_set_accel_modes(data->m_boardNum,0,data->config.axisConfig.m_accelmode,data->config.axisConfig.m_decelmode,0);

   long currPos;
   
   E1701M_get_axis_pos2(data->m_boardNum,0,&currPos);
   if((data->config.axisConfig.flags & AXISFLAG_DIRVALUE)!=0) data->runconfig.currPos=-currPos;
   else data->runconfig.currPos=currPos;

   data->runconfig.lastPos=data->runconfig.currPos;
   data->runconfig.doStop=0;
   m_oapc_io_callback(OAPC_NUM_IO0,data->m_callbackID);

   data->runconfig.data=data;
   data->runconfig.m_refTimeoutTime=data->config.axisConfig.m_axisHomeTimeout;
   data->runconfig.m_signal=oapc_thread_signal_create();
   if (!data->runconfig.m_signal) return OAPC_ERROR_NO_MEMORY;
   threadHandle=oapc_thread_create(motorLoop,(void*)&data->runconfig);
   if (!threadHandle) return OAPC_ERROR_NO_MEMORY;
   oapc_thread_set_prio(threadHandle,1);

   return OAPC_OK;
}



/**
This function is called before the application unloads everything, it has to be used to deinitialize
everything and to release used resources.
*/
OAPC_EXT_API unsigned long oapc_exit(void *instanceData)
{
   struct instData *data;
   int              j;
   
   data=(struct instData*)instanceData;
   data->running=false;
   E1701M_stop(data->m_boardNum,E1701M_AXIS_0|E1701M_AXIS_1|E1701M_AXIS_2|E1701M_AXIS_3);

   data->runconfig.doStop=1;
   data->runconfig.running=false;
   oapc_thread_signal_send(data->runconfig.m_signal);
   for (j=0; j<100; j++)
   {
      if (data->runconfig.running) continue;
      oapc_util_thread_sleep(10); // wait some time
   }
   oapc_thread_signal_release(data->runconfig.m_signal);

   E1701M_close(data->m_boardNum);

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
   
   data=(struct instData*)instanceData;

   if (input==0) // input 0,2,4
   {
      if (!data->m_busy)
      {
         data->runconfig.newPos=valueToInc(data,value);
         data->runconfig.moveToPos=true;
         return OAPC_OK;
      }
      else return OAPC_ERROR_STILL_IN_PROGRESS;
   }
   else if (input==1) // input 1,3,5
   {
      if ((value==0) || (value==-2)) data->runconfig.doStop=true;
      else if (value==-1) data->runconfig.doHome=true;
//      else if (value<0) data->runconfig[(input-1)/2].newSpeed=value; wtf was this???
      else if (value>0) data->runconfig.newSpeed=speedValueToInc(data,value,true);
      else return OAPC_ERROR_INVALID_INPUT;
      return OAPC_OK;
   }
   return OAPC_ERROR_NO_SUCH_IO;
}


OAPC_EXT_API unsigned long  oapc_get_num_value(void* instanceData,unsigned long output,double* value)
{
   struct instData *data;
   
   data=(struct instData*)instanceData;

   if (output==0)
   {
      *value=incToValue(data,data->runconfig.currPos);
      return OAPC_OK;
   }
   else if (output==1)
   {
      *value=speedIncToValue(data,data->runconfig.currentAxisSpeed);
      return OAPC_OK;
   }

   return OAPC_ERROR_NO_SUCH_IO;
}


OAPC_EXT_API unsigned long  oapc_set_bin_value(void* instanceData,unsigned long input,struct oapc_bin_head *value)
{
   struct instData      *data;

   if (input!=7) return OAPC_ERROR_NO_SUCH_IO;
   if (value->type!=OAPC_BIN_TYPE_STRUCT) return OAPC_ERROR_INVALID_INPUT;
   if (value->subType==OAPC_BIN_SUBTYPE_STRUCT_CTRLEND) return OAPC_OK; // nothing to do in case of an end signal

   data=(struct instData*)instanceData;
   if (value->subType==OAPC_BIN_SUBTYPE_STRUCT_MOTIONCTRL)
   {
      struct oapc_bin_struct_motionctrl *ctrl;
      double                             d;
      bool                               motionStarted=false;

      ctrl=(struct oapc_bin_struct_motionctrl*)&value->data;
      if ((ctrl->enableAxes & 0x01)==0x01)
      {
         motionStarted=true;
         data->runconfig.doStop=false;
         if ((ctrl->stopAxes & 0x01)==0x01) data->runconfig.doStop=true;
         else if (data->runconfig.currentAxisSpeed==0) // it is not moving
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
               if (d>2147483600) d=2147483600.0;
               else if (d<-2147483600) d=-2147483600.0;
               data->runconfig.newPos=valueToInc(data,d);
               data->runconfig.newSpeed=speedValueToInc(data,((int)ntohl(ctrl->speed[0]))/1000.0,true);
               data->runconfig.moveToPos=true;
               oapc_thread_signal_send(data->runconfig.m_signal);
            }
         }
         else return OAPC_ERROR_STILL_IN_PROGRESS;
      }
      if (!motionStarted) return OAPC_ERROR_INVALID_INPUT;
   }
   else return OAPC_ERROR_INVALID_INPUT;
   return OAPC_OK;
}



