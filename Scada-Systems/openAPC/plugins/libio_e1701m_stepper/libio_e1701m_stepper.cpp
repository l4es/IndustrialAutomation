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

#define MAX_IP_SIZE       100
#define MAX_XML_SIZE    31000
#define MAX_FILENAMESIZE  255

#define MAX_NUM_AXES      4
#define USE_MAX_NUM_AXES  4
#define POS_UPDATE_TIME 250 //msec
#define DATA_LENGTH      48 //defined by contoller-firmware internal limitations

#define AXISFLAG_DIRVALUE  0x01 // invert direction signal
#define AXISFLAG_REFVALUE  0x02 // invert reference switch input logic
#define AXISFLAG_LIMVALUEA 0x04 // invert limit switch A input logic
#define AXISFLAG_LIMVALUEB 0x08 // invert limit switch B input logic

struct axis_config
{
   char          enable;
   int           llimit,hlimit,factor;
   int           mAccel,mDecel,mStopDecel,res1,refspeedin,refspeedin2nd,refspeedin3rd,refpos; // int - 32 bit
   char          refmode,refin,flags,autoleavelim,axisMode,m_accelmode,m_decelmode;
   unsigned int  m_axisMaxSpeed;
   unsigned char limitain,limitbin;
   unsigned char m_axisHomeTimeout;
   unsigned char m_useenc;
   unsigned int  m_mEncfactor;
};

struct instData;

struct axis_runconfig
{
   unsigned char       axisNum;

   bool                running,moveToPos,doHome,doStop;
   double              newSpeed,currentAxisSpeed;
   long                currPos,newPos,lastPos;
   unsigned char       m_busy;
   struct instData    *data;
   time_t              m_refTimeoutTime;
   time_t              m_refEndTime;
   void               *m_signal;          
   unsigned char       m_stopOnEnter,m_stopOnLeave;
};


#define FLIPFLAG_XY      0x01
#define FLIPFLAG_MIRRORX 0x02
#define FLIPFLAG_MIRRORY 0x04

struct libio_config
{
   unsigned short     version,length;
   unsigned short     reserved;
   struct axis_config axisConfig[8];
   unsigned char      m_flipflags;
   int                m_ondelay,m_offdelay;
   char               m_ip[MAX_IP_SIZE];
   char               m_passwd[DATA_LENGTH];
   char               m_fwlogfile_unused[MAX_FILENAMESIZE+4];
   // version 2
   unsigned short     m_acfgLimitSwitchDist[8]; // additional axis config
};


struct instData
{
   struct libio_config                config;
   struct axis_runconfig              runconfig[MAX_NUM_AXES];
   void                              *m_mutex,*m_busyStateMutex;
   int                                m_callbackID;
   unsigned char                      m_toolOn,m_busy;
   int                                prevJumpspeed,prevMarkspeed;
   std::queue<struct oapc_bin_head*> *m_ctrlList;
   double                             lastX,lastY,lastZ;
   struct oapc_bin_head              *m_bin;
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
  <param text=\"Swap X and Y\">\
   <name>flipxy</name>\
   <type>checkbox</type>\
   <default>%d</default>\
  </param>\
 </general>\
 <stdpanel>\
  <name>Axis 1</name>\
  <param text=\"Enable Axis\">\
   <name>enable1</name>\
   <type>checkbox</type>\
   <default>%d</default>\
  </param>\
  <param name=\"axismode1\" text=\"Axis Mode\">\
   <type>option</type>\
   <value>planar (mm)</value>\
   <value>rotational (deg)</value>\
   <default>%d</default>\
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Resolution Factor\">\
   <name>factor1</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>10</min>\
   <max>100000</max>\
   <unit>inc/mm | inc/rot</unit>\
   <enableon>enable1</enableon>\
  </param>\
  <param name=\"llimit1\" text=\"Low Limit\">\
   <type>integer</type>\
   <min>-2100000000</min>\
   <max>2100000000</max>\
   <default>%d</default>\
   <unit>mm</unit>\
   <enableon>enable1</enableon>\
  </param>\
  <param name=\"hlimit1\" text=\"High Limit\">\
   <type>integer</type>\
   <min>-2100000000</min>\
   <max>2100000000</max>\
   <default>%d</default>\
   <unit>mm</unit>\
   <enableon>enable1</enableon>\
  </param>\
  <param name=\"maxspeed1\" text=\"Maximum Speed\">\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec | deg/sec</unit>\
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Acceleration Mode\">\
   <name>accelmode1</name>\
   <type>option</type>\
   <value>linear</value>\
   <value>exponential</value>\
   <value>s-shaped</value>\
   <default>%d</default>\
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Acceleration\">\
   <name>accel1</name>\
   <type>float</type>""\
   <min>0</min>\
   <max>1000</max>\
   <default>%f</default>\
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Deceleration Mode\">\
   <name>decelmode1</name>\
   <type>option</type>\
   <value>linear</value>\
   <value>exponential</value>\
   <value>s-shaped</value>\
   <default>%d</default>\
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Deceleration\">\
   <name>decel1</name>\
   <type>float</type>\
   <min>0</min>\
   <max>1000</max>\
   <default>%f</default>\
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Stop Deceleration\">\
   <name>stopdecel1</name>\
   <type>float</type>\
   <min>0</min>\
   <max>1000</max>\
   <default>%f</default>\
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Invert Direction\">\
   <name>dirvalue1</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable1</enableon>\
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
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Reference Timeout\">\
   <name>reftimeout1</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>250</max>\
   <default>%d</default>\
   <unit>sec</unit>\
   <enableon>enable1</enableon>\
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
   <enableon>enable1</enableon>\
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
   <unit>mm/sec | deg/sec</unit>\
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Reference Speed 2\">\
   <name>refspeedin2nd1</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec | deg/sec</unit>\
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Reference Speed 3\">\
   <name>refspeedin3rd1</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec | deg/sec</unit>\
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Referenced Position\">\
   <name>refpos1</name>\
   <type>float</type>\
   <min>-2100000000</min>\
   <max>2100000000</max>\
   <default>%f</default>\
   <unit>mm | deg</unit>\
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Limit Switch (+) Input\">\
   <name>limitain1</name>\
   <type>option</type>\
   <value>DIGI IN0</value>""\
   <value>DIGI IN1</value>\
   <value>DIGI IN2</value>\
   <value>DIGI IN3</value>\
   <value>DIGI IN4</value>\
   <value>DIGI IN5</value>\
   <value>DIGI IN6</value>\
   <value>DIGI IN7</value>\
   <value>unused</value>\
   <default>%d</default>\
   <enableon>enable1</enableon>\
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
   <enableon>enable1</enableon>\
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
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Minimum Leave-Distance\">\
   <name>limitswitchdist1</name>\
   <type>float</type>\
   <min>0</min>\
   <max>50</max>\
   <default>%f</default>\
   <unit>mm | deg</unit>\
   <enableon>autoleavelim1</enableon>\
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Encoder\">\
   <name>useenc1</name>\
   <type>option</type>\
   <value>0 (DIGI IN0/1)</value>\
   <value>1 (DIGI IN2/3)</value>\
   <value>None</value>\
   <default>%d</default>\
   <enableon>enable1</enableon>\
  </param>\
  <param text=\"Encoder Factor\">\
   <name>encfactor1</name>\
   <type>float</type>\
   <default>%f</default>\
   <min>0.0001</min>\
   <max>100000</max>\
   <unit>steps/inc</unit>\
   <enableon>enable1</enableon>\
  </param>\
 </stdpanel>\
 <stdpanel>\
  <name>Axis 2</name>\
  <param text=\"Enable Axis\">\
   <name>enable2</name>\
   <type>checkbox</type>\
   <default>%d</default>\
  </param>\
  <param text=\"Axis Mode\">\
   <name>axismode2</name>\
   <type>option</type>\
   <value>planar (mm)</value>\
   <value>rotational (deg)</value>\
   <default>%d</default>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Resolution Factor\">\
   <name>factor2</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>10</min>\
   <max>100000</max>\
   <unit>inc/mm | inc/rot</unit>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Low Limit\">\
   <name>llimit2</name>\
   <type>integer</type>\
   <min>-2100000000</min>\
   <max>2100000000</max>\
   <default>%d</default>\
   <unit>mm</unit>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"High Limit\">\
   <name>hlimit2</name>\
   <type>integer</type>\
   <min>-2100000000</min>\
   <max>2100000000</max>\
   <default>%d</default>\
   <unit>mm</unit>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Maximum Speed\">\
   <name>maxspeed2</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec | deg/sec</unit>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Acceleration Mode\">\
   <name>accelmode2</name>\
   <type>option</type>\
   <value>linear</value>\
   <value>exponential</value>\
   <value>s-shaped</value>\
   <default>%d</default>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Acceleration\">\
   <name>accel2</name>\
   <type>float</type>\
   <min>0</min>\
   <max>1000</max>\
   <default>%f</default>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Deceleration Mode\">\
   <name>decelmode2</name>\
   <type>option</type>\
   <value>linear</value>\
   <value>exponential</value>\
   <value>s-shaped</value>\
   <default>%d</default>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Deceleration\">\
   <name>decel2</name>\
   <type>float</type>\
   <min>0</min>\
   <max>1000</max>\
   <default>%f</default>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Stop Deceleration\">\
   <name>stopdecel2</name>\
   <type>float</type>\
   <min>0</min>\
   <max>1000</max>\
   <default>%f</default>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Invert Direction\">\
   <name>dirvalue2</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Reference Mode\">\
   <name>refmode2</name>\
   <type>option</type>\
   <value>No referencing</value>\
   <value>To switch in negative direction</value>""\
   <value>To switch in positive direction</value>\
   <value>To switch in negative, leave in positive direction</value>\
   <value>To switch in positive, leave in negative direction</value>\
   <value>To switch in negative, leave in negative direction</value>\
   <value>To switch in positive, leave in positive direction</value>\
   <value>To switch in negative, to switch in positive, leave in negative direction</value>\
   <value>To switch in positive, to switch in negative, leave in positive direction</value>\
   <default>%d</default>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Reference Timeout\">\
   <name>reftimeout2</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>250</max>\
   <default>%d</default>\
   <unit>sec</unit>""\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Reference Signal Input\">\
   <name>refin2</name>\
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
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Invert Input Logic\">\
   <name>refvalue2</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Reference Speed 1\">\
   <name>refspeedin2</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec | deg/sec</unit>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Reference Speed 2\">\
   <name>refspeedin2nd2</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec | deg/sec</unit>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Reference Speed 3\">\
   <name>refspeedin3rd2</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec | deg/sec</unit>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Referenced Position\">\
   <name>refpos2</name>\
   <type>float</type>\
   <min>-2100000000</min>\
   <max>2100000000</max>\
   <default>%f</default>\
   <unit>mm | deg</unit>\
   <enableon>enable2</enableon>\
  </param>\
   <param text=\"Limit Switch (+) Input\">\
   <name>limitain2</name>\
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
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Invert Input Logic\">\
   <name>limvaluea2</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Limit Switch (-) Input\">\
   <name>limitbin2</name>\
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
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Invert Input Logic\">\
   <name>limvalueb2</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Auto-leave Limit Switch\">\
   <name>autoleavelim2</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Minimum Leave-Distance\">\
   <name>limitswitchdist2</name>\
   <type>float</type>\
   <min>0</min>\
   <max>50</max>\
   <default>%f</default>\
   <unit>mm | deg</unit>\
   <enableon>autoleavelim2</enableon>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Encoder\">\
   <name>useenc2</name>\
   <type>option</type>\
   <value>0 (DIGI IN0/1)</value>\
   <value>1 (DIGI IN2/3)</value>""\
   <value>None</value>\
   <default>%d</default>\
   <enableon>enable2</enableon>\
  </param>\
  <param text=\"Encoder Factor\">\
   <name>encfactor2</name>\
   <type>float</type>\
   <default>%f</default>\
   <min>0.0001</min>\
   <max>100000</max>\
   <unit>steps/inc</unit>\
   <enableon>enable2</enableon>\
  </param>\
 </stdpanel>\
 <stdpanel>\
  <name>Axis 3</name>\
  <param text=\"Enable Axis\">\
   <name>enable3</name>\
   <type>checkbox</type>\
   <default>%d</default>\
  </param>\
  <param text=\"Axis Mode\">\
   <name>axismode3</name>\
   <type>option</type>\
   <value>planar (mm)</value>\
   <value>rotational (deg)</value>\
   <default>%d</default>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Resolution Factor\">\
   <name>factor3</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>10</min>\
   <max>100000</max>\
   <unit>inc/mm | inc/rot</unit>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Low Limit\">\
   <name>llimit3</name>\
   <type>integer</type>\
   <min>-2100000000</min>\
   <max>2100000000</max>\
   <default>%d</default>\
   <unit>mm</unit>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"High Limit\">\
   <name>hlimit3</name>\
   <type>integer</type>\
   <min>-2100000000</min>\
   <max>2100000000</max>\
   <default>%d</default>\
   <unit>mm</unit>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Maximum Speed\">\
   <name>maxspeed3</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec | deg/sec</unit>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Acceleration Mode\">\
   <name>accelmode3</name>\
   <type>option</type>\
   <value>linear</value>\
   <value>exponential</value>\
   <value>s-shaped</value>\
   <default>%d</default>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Acceleration\">\
   <name>accel3</name>\
   <type>float</type>\
   <min>0</min>\
   <max>1000</max>\
   <default>%f</default>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Deceleration Mode\">\
   <name>decelmode3</name>\
   <type>option</type>\
   <value>linear</value>\
   <value>exponential</value>\
   <value>s-shaped</value>\
   <default>%d</default>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Deceleration\">\
   <name>decel3</name>\
   <type>float</type>\
   <min>0</min>\
   <max>1000</max>\
   <default>%f</default>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Stop Deceleration\">\
   <name>stopdecel3</name>\
   <type>float</type>""\
   <min>0</min>\
   <max>1000</max>\
   <default>%f</default>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Invert Direction\">\
   <name>dirvalue3</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Reference Mode\">\
   <name>refmode3</name>\n""\
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
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Reference Timeout\">\
   <name>reftimeout3</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>250</max>\
   <default>%d</default>\
   <unit>sec</unit>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Reference Signal Input\">\
   <name>refin3</name>\
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
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Invert Input Logic\">\
   <name>refvalue3</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Reference Speed 1\">\
   <name>refspeedin3</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec | deg/sec</unit>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Reference Speed 2\">\
   <name>refspeedin2nd3</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec | deg/sec</unit>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Reference Speed 3\">\
   <name>refspeedin3rd3</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec | deg/sec</unit>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Referenced Position\">\
   <name>refpos3</name>\
   <type>float</type>\
   <min>-2100000000</min>\
   <max>2100000000</max>\
   <default>%f</default>\
   <unit>mm | deg</unit>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Limit Switch (+) Input\">\
   <name>limitain3</name>\
   <type>option</type>""\
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
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Invert Input Logic\">\
   <name>limvaluea3</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Limit Switch (-) Input\">\
   <name>limitbin3</name>\
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
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Invert Input Logic\">\
   <name>limvalueb3</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Auto-leave Limit Switch\">\
   <name>autoleavelim3</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Minimum Leave-Distance\">\
   <name>limitswitchdist3</name>\
   <type>float</type>\
   <min>0</min>\
   <max>50</max>\
   <default>%f</default>\
   <unit>mm | deg</unit>\
   <enableon>autoleavelim3</enableon>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Encoder\">\
   <name>useenc3</name>\
   <type>option</type>\
   <value>0 (DIGI IN0/1)</value>\
   <value>1 (DIGI IN2/3)</value>\
   <value>None</value>\
   <default>%d</default>\
   <enableon>enable3</enableon>\
  </param>\
  <param text=\"Encoder Factor\">\
   <name>encfactor3</name>\
   <type>float</type>\
   <default>%f</default>\
   <min>0.0001</min>\
   <max>100000</max>\
   <unit>steps/inc</unit>\
   <enableon>enable3</enableon>\
  </param>\
 </stdpanel>\
 <stdpanel>\
  <name>Axis 4</name>\
  <param text=\"Enable Axis\">\
   <name>enable4</name>\
   <type>checkbox</type>\
   <default>%d</default>\
  </param>\
  <param text=\"Axis Mode\">\
   <name>axismode4</name>\
   <type>option</type>\
   <value>planar (mm)</value>\
   <value>rotational (deg)</value>\
   <default>%d</default>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Resolution Factor\">\
   <name>factor4</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>10</min>\
   <max>100000</max>\
   <unit>inc/mm | inc/rot</unit>\
   <enableon>enable4</enableon>\
  </param>\
  <param name=\"llimit4\" text=\"Low Limit\">\
   <type>integer</type>\
   <min>-2100000000</min>\
   <max>2100000000</max>\
   <default>%d</default>\
   <unit>mm</unit>\
   <enableon>enable4</enableon>\
  </param>\
  <param name=\"hlimit4\" text=\"High Limit\">\
   <type>integer</type>\
   <min>-2100000000</min>\
   <max>2100000000</max>\
   <default>%d</default>\
   <unit>mm</unit>\
   <enableon>enable4</enableon>\
  </param>\
  <param name=\"maxspeed4\" text=\"Maximum Speed\">\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec | deg/sec</unit>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Acceleration Mode\">\
   <name>accelmode4</name>\
   <type>option</type>\
   <value>linear</value>\
   <value>exponential</value>\
   <value>s-shaped</value>\
   <default>%d</default>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Acceleration\">\
   <name>accel4</name>\
   <type>float</type>\
   <min>0</min>\
   <max>1000</max>\
   <default>%f</default>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Deceleration Mode\">\
   <name>decelmode4</name>\
   <type>option</type>\
   <value>linear</value>\
   <value>exponential</value>\
   <value>s-shaped</value>\
   <default>%d</default>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Deceleration\">\
   <name>decel4</name>\
   <type>float</type>\
   <min>0</min>\
   <max>1000</max>\
   <default>%f</default>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Stop Deceleration\">\
   <name>stopdecel4</name>\
   <type>float</type>""\
   <min>0</min>\
   <max>1000</max>\
   <default>%f</default>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Invert Direction\">\
   <name>dirvalue4</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Reference Mode\">\
   <name>refmode4</name>\n""\
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
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Reference Timeout\">\
   <name>reftimeout4</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>250</max>\
   <default>%d</default>\
   <unit>sec</unit>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Reference Signal Input\">\
   <name>refin4</name>\
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
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Invert Input Logic\">\
   <name>refvalue4</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Reference Speed 1\">\
   <name>refspeedin4</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec | deg/sec</unit>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Reference Speed 2\">\
   <name>refspeedin2nd4</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec | deg/sec</unit>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Reference Speed 3\">\
   <name>refspeedin3rd4</name>\
   <type>float</type>\
   <min>0.1</min>\
   <max>200000</max>\
   <default>%f</default>\
   <unit>mm/sec | deg/sec</unit>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Referenced Position\">\
   <name>refpos4</name>\
   <type>float</type>\
   <min>-2100000000</min>\
   <max>2100000000</max>\
   <default>%f</default>\
   <unit>mm | deg</unit>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Limit Switch (+) Input\">\
   <name>limitain4</name>\
   <type>option</type>""\
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
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Invert Input Logic\">\
   <name>limvaluea4</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Limit Switch (-) Input\">\
   <name>limitbin4</name>\
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
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Invert Input Logic\">\
   <name>limvalueb4</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Auto-leave Limit Switch\">\
   <name>autoleavelim4</name>\
   <type>checkbox</type>\
   <default>%d</default>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Minimum Leave-Distance\">\
   <name>limitswitchdist4</name>\
   <type>float</type>\
   <min>0</min>\
   <max>50</max>\
   <default>%f</default>\
   <unit>mm | deg</unit>\
   <enableon>autoleavelim4</enableon>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Encoder\">\
   <name>useenc4</name>\
   <type>option</type>\
   <value>0 (DIGI IN0/1)</value>\
   <value>1 (DIGI IN2/3)</value>\
   <value>None</value>\
   <default>%d</default>\
   <enableon>enable4</enableon>\
  </param>\
  <param text=\"Encoder Factor\">\
   <name>encfactor4</name>\
   <type>float</type>\
   <default>%f</default>\
   <min>0.0001</min>\
   <max>100000</max>\
   <unit>steps/inc</unit>\
   <enableon>enable4</enableon>\
  </param>\
 </stdpanel>\
 <oapcstdpanel>\
  <name>Control Default</name>\
  <param text=\"On Delay\">\
   <name>ondelay</name>\
   <type>integer</type>\
   <default>%d</default>""\
   <min>-10000</min>\
   <max>0</max>\
   <unit>msec</unit>\
  </param>\
  <param text=\"Off Delay\">\
   <name>offdelay</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>-10000</min>\
   <max>0</max>\
   <unit>msec</unit>\
  </param>\
 </oapcstdpanel>\
 <helppanel>\
  <in0>POS - absolute position to move to</in0>\
  <in1>MODE >0 speed, =0 stop, =-1 home</in1>\n\
  <in2>...</in2>\
  <in7>CTRL - Control data</in7>\
  <out0>POS - current absolute axis position</out0>\
  <out1>MODE >0 current speed, =0 has stopped</out1>\
  <out2>...</out2>\
  <out6>BSY - Motion active</out6>\
  <out7>CTRL - Motion-synchronous control data</out7>\
 </helppanel>\
</dialogue>\
</oapc-config>\n";

/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAAAA3NCSVQICAjb4U/gAAAAFVBMVEUEAgT///+EgoQE/vz//wDEwsQAAAA8mNi4AAAACXBIWXMAAA7EAAAOxAGVKw4bAAABoElEQVRYhe2Y2xKFIAhFocL//+RjXhJIHT1YT/HQhNtxYRI1ALxoiAjbvu/bebfCEQbbcRxBiSgCPxMoimZHGIFHBSWizqhKTGZHbysqCeWjKjGZHb2tqCSUZ7OYzI7aVkIkFPGYzI6wrGQUipjMDlaUC/W85QwExgTkMQJztDIV5pUWfIwvMuhMoMTYiyj5NLmjFDPqIaui2nxLZDkDVc6JpBtRZlDivLGZCS0lGIWzbFWNKqp5+N20oEihFoy9V42nMaqUGOh2oI6jqvwphW+F1CyB4muo8jOggA6BDbjT2HSdWpMOWyVeqbL9Gmp+i1fRzhc+0aFrPsBqMF0lL+LPCOI5ldRw8aQGauCQcqXx+cUFPnQ/K+N3tyxCqFE8zjipteKQkt5aFjV/gD2UvB9QbhmInWQXFWfaSZWvmux6V0brvcJ8bAEKO4XJYa8G/se6YLLcPoBqfkRSVqz9txj6NL5hH+pDfSjbmqLo5z7ME6jNX0qfRnZj1lqo66VPo7oxa81vi/dpZDdmrYV/tOKobsxaq/ZpnkFV+zTwov0A7F0Ub4X51IwAAAAASUVORK5CYII=";
static char                 libname[]="E1701M Stepper Motion Controller";
static char                 xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports
static struct libio_config  save_config;



static long valueToInc(struct instData *data,int a,double mm)
{
   long newPos;

   if (data->config.axisConfig[a].axisMode==1) // planar mode
   {
      if (mm<data->config.axisConfig[a].llimit) mm=data->config.axisConfig[a].llimit;
      else if (mm>data->config.axisConfig[a].hlimit) mm=data->config.axisConfig[a].hlimit;
      newPos=(long)OAPC_ROUND((mm*data->config.axisConfig[a].factor),0);
   }
   else newPos=(long)OAPC_ROUND((data->config.axisConfig[a].factor*mm)/360.0,0);
   return newPos;
}



static unsigned int speedValueToInc(struct instData *data,const int a,const double spd,const bool checkMaxSpeed)
{
   unsigned long inc;

   if (data->config.axisConfig[a].axisMode==1) // planar mode
    inc=(unsigned int)OAPC_ROUND(spd*data->config.axisConfig[a].factor,0);
   else
    inc=(unsigned int)OAPC_ROUND((spd/360.0)*data->config.axisConfig[a].factor,0);
   if ((checkMaxSpeed) && (inc>data->config.axisConfig[a].m_axisMaxSpeed)) inc=data->config.axisConfig[a].m_axisMaxSpeed;
   if (inc<=2) inc=2;
   return inc;
}



static double incToValue(struct instData *data,int a,long inc)
{
   if (data->config.axisConfig[a].axisMode==1) // planar mode
   {
      return (1.0*inc)/data->config.axisConfig[a].factor;
   }
   return (inc*360.0)/data->config.axisConfig[a].factor;
}



static double speedIncToValue(struct instData *data,int a,double inc)
{
   if (data->config.axisConfig[a].axisMode==1) // planar mode
   {
      return (1.0*inc)/data->config.axisConfig[a].factor;
   }
   return (inc*360.0)/data->config.axisConfig[a].factor;
}



/** has to be called after create instance and load data */
OAPC_EXT_API unsigned long oapc_get_config_info_data(void *instanceData,struct config_info *fillStruct)
{
   struct config_info *cfgInfo;
   struct instData    *data;
   int                 i;
   unsigned int        bitfield=1;

   data=(struct instData*)instanceData;

   if (!fillStruct) return OAPC_ERROR;
   cfgInfo=(struct config_info*)fillStruct;
   memset(cfgInfo,0,sizeof(struct config_info));

   cfgInfo->version=OAPC_CONFIG_INFO_VERSION;
   cfgInfo->length=sizeof(struct config_info);
   cfgInfo->configType=OAPC_CONFIG_TYPE_MOTIONCONTROLLER;

   cfgInfo->motionController.version=OAPC_CONFIG_MOTION_CONTROLLER_VERSION;
   cfgInfo->motionController.length=sizeof(struct config_motion_controller);
   cfgInfo->motionController.flags=0;

   for (i=0; i<USE_MAX_NUM_AXES; i++)
   {
      if (data->config.axisConfig[i].enable)
      {
         cfgInfo->motionController.availableAxes|=bitfield;
         cfgInfo->motionController.uMinPos[i]=data->config.axisConfig[i].llimit*1000;
         cfgInfo->motionController.uMaxPos[i]=data->config.axisConfig[i].hlimit*1000;
         cfgInfo->motionController.uMaxSpeed[i]=(int)OAPC_ROUND(speedIncToValue(data,i,data->config.axisConfig[i].m_axisMaxSpeed)*1000.0,0);
         if (data->config.axisConfig[i].axisMode==2) cfgInfo->motionController.rotationalAxes|=bitfield;
      }
      bitfield=bitfield<<1;
   }
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
   return OAPC_NUM_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|
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
   return OAPC_NUM_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|
          OAPC_DIGI_IO6|OAPC_BIN_IO7;
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
                     data->config.m_flipflags & FLIPFLAG_XY,
                     data->config.axisConfig[0].enable,data->config.axisConfig[0].axisMode,data->config.axisConfig[0].factor,
                     data->config.axisConfig[0].llimit,data->config.axisConfig[0].hlimit,
                     speedIncToValue(data,0,data->config.axisConfig[0].m_axisMaxSpeed),
                     data->config.axisConfig[0].m_accelmode,data->config.axisConfig[0].mAccel/1000.0,
                     data->config.axisConfig[0].m_decelmode,data->config.axisConfig[0].mDecel/1000.0,data->config.axisConfig[0].mStopDecel/1000.0,
                     (data->config.axisConfig[0].flags & AXISFLAG_DIRVALUE)!=0,
                     data->config.axisConfig[0].refmode,
                     data->config.axisConfig[0].m_axisHomeTimeout,
                     data->config.axisConfig[0].refin,
                     (data->config.axisConfig[0].flags & AXISFLAG_REFVALUE)!=0,
                     speedIncToValue(data,0,data->config.axisConfig[0].refspeedin),
                     speedIncToValue(data,0,data->config.axisConfig[0].refspeedin2nd),
                     speedIncToValue(data,0,data->config.axisConfig[0].refspeedin3rd),
                     incToValue(data,0,data->config.axisConfig[0].refpos),
                     data->config.axisConfig[0].limitain,(data->config.axisConfig[0].flags & AXISFLAG_LIMVALUEA)!=0,
                     data->config.axisConfig[0].limitbin,(data->config.axisConfig[0].flags & AXISFLAG_LIMVALUEB)!=0,
                     data->config.axisConfig[0].autoleavelim,incToValue(data,0,data->config.m_acfgLimitSwitchDist[0]),
                     data->config.axisConfig[0].m_useenc,data->config.axisConfig[0].m_mEncfactor/1000.0,

                     data->config.axisConfig[1].enable,data->config.axisConfig[1].axisMode,data->config.axisConfig[1].factor,
                     data->config.axisConfig[1].llimit,data->config.axisConfig[1].hlimit,
                     speedIncToValue(data,1,data->config.axisConfig[1].m_axisMaxSpeed),
                     data->config.axisConfig[1].m_accelmode,data->config.axisConfig[1].mAccel/1000.0,
                     data->config.axisConfig[1].m_decelmode,data->config.axisConfig[1].mDecel/1000.0,data->config.axisConfig[1].mStopDecel/1000.0,
                     (data->config.axisConfig[1].flags & AXISFLAG_DIRVALUE)!=0,
                     data->config.axisConfig[1].refmode,
                     data->config.axisConfig[1].m_axisHomeTimeout,
                     data->config.axisConfig[1].refin,
                     (data->config.axisConfig[1].flags & AXISFLAG_REFVALUE)!=0,
                     speedIncToValue(data,1,data->config.axisConfig[1].refspeedin),
                     speedIncToValue(data,1,data->config.axisConfig[1].refspeedin2nd),
                     speedIncToValue(data,1,data->config.axisConfig[1].refspeedin3rd),
                     incToValue(data,1,data->config.axisConfig[1].refpos),
                     data->config.axisConfig[1].limitain,(data->config.axisConfig[1].flags & AXISFLAG_LIMVALUEA)!=0,
                     data->config.axisConfig[1].limitbin,(data->config.axisConfig[1].flags & AXISFLAG_LIMVALUEB)!=0,
                     data->config.axisConfig[1].autoleavelim,incToValue(data,1,data->config.m_acfgLimitSwitchDist[1]),
                     data->config.axisConfig[1].m_useenc,data->config.axisConfig[1].m_mEncfactor/1000.0,

                     data->config.axisConfig[2].enable,data->config.axisConfig[2].axisMode,data->config.axisConfig[2].factor,
                     data->config.axisConfig[2].llimit,data->config.axisConfig[2].hlimit,
                     speedIncToValue(data,2,data->config.axisConfig[2].m_axisMaxSpeed),
                     data->config.axisConfig[2].m_accelmode,data->config.axisConfig[2].mAccel/1000.0,
                     data->config.axisConfig[2].m_decelmode,data->config.axisConfig[2].mDecel/1000.0,data->config.axisConfig[2].mStopDecel/1000.0,
                     (data->config.axisConfig[2].flags & AXISFLAG_DIRVALUE)!=0,
                     data->config.axisConfig[2].refmode,
                     data->config.axisConfig[2].m_axisHomeTimeout,
                     data->config.axisConfig[2].refin,
                     (data->config.axisConfig[2].flags & AXISFLAG_REFVALUE)!=0,
                     speedIncToValue(data,2,data->config.axisConfig[2].refspeedin),
                     speedIncToValue(data,2,data->config.axisConfig[2].refspeedin2nd),
                     speedIncToValue(data,2,data->config.axisConfig[2].refspeedin3rd),
                     incToValue(data,2,data->config.axisConfig[2].refpos),
                     data->config.axisConfig[2].limitain,(data->config.axisConfig[2].flags & AXISFLAG_LIMVALUEA)!=0,
                     data->config.axisConfig[2].limitbin,(data->config.axisConfig[2].flags & AXISFLAG_LIMVALUEB)!=0,
                     data->config.axisConfig[2].autoleavelim,incToValue(data,2,data->config.m_acfgLimitSwitchDist[2]),
                     data->config.axisConfig[2].m_useenc,data->config.axisConfig[2].m_mEncfactor/1000.0,

                     data->config.axisConfig[3].enable,data->config.axisConfig[3].axisMode,data->config.axisConfig[3].factor,
                     data->config.axisConfig[3].llimit,data->config.axisConfig[3].hlimit,
                     speedIncToValue(data,3,data->config.axisConfig[3].m_axisMaxSpeed),
                     data->config.axisConfig[3].m_accelmode,data->config.axisConfig[3].mAccel/1000.0,
                     data->config.axisConfig[3].m_decelmode,data->config.axisConfig[3].mDecel/1000.0,data->config.axisConfig[3].mStopDecel/1000.0,
                     (data->config.axisConfig[3].flags & AXISFLAG_DIRVALUE)!=0,
                     data->config.axisConfig[3].refmode,
                     data->config.axisConfig[3].m_axisHomeTimeout,
                     data->config.axisConfig[3].refin,
                     (data->config.axisConfig[3].flags & AXISFLAG_REFVALUE)!=0,
                     speedIncToValue(data,3,data->config.axisConfig[3].refspeedin),
                     speedIncToValue(data,3,data->config.axisConfig[3].refspeedin2nd),
                     speedIncToValue(data,3,data->config.axisConfig[3].refspeedin3rd),
                     incToValue(data,3,data->config.axisConfig[3].refpos),
                     data->config.axisConfig[3].limitain,(data->config.axisConfig[3].flags & AXISFLAG_LIMVALUEA)!=0,
                     data->config.axisConfig[3].limitbin,(data->config.axisConfig[3].flags & AXISFLAG_LIMVALUEB)!=0,
                     data->config.axisConfig[3].autoleavelim,incToValue(data,3,data->config.m_acfgLimitSwitchDist[3]),
                     data->config.axisConfig[3].m_useenc,data->config.axisConfig[3].m_mEncfactor/1000.0,

                     data->config.m_ondelay/1000,data->config.m_offdelay/1000
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

   if (strcmp(name,"ip")==0)                 strncpy(data->config.m_ip,value,MAX_IP_SIZE);
   else if (strcmp(name,"passwd")==0)        strncpy(data->config.m_passwd,value,DATA_LENGTH);
   else if (strcmp(name,"flipxy")==0)
   {
      if (atoi(value)) data->config.m_flipflags|=FLIPFLAG_XY;
      else data->config.m_flipflags&=~FLIPFLAG_XY;
   }

   else if (strcmp(name,"enable1")==0)        data->config.axisConfig[0].enable=(char)atoi(value);
   else if (strcmp(name,"factor1")==0)        data->config.axisConfig[0].factor=atoi(value);
   else if (strcmp(name,"llimit1")==0)        data->config.axisConfig[0].llimit=atoi(value);
   else if (strcmp(name,"hlimit1")==0)        data->config.axisConfig[0].hlimit=atoi(value);
   else if (strcmp(name,"accel1")==0)         data->config.axisConfig[0].mAccel=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"decel1")==0)         data->config.axisConfig[0].mDecel=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"accelmode1")==0)     data->config.axisConfig[0].m_accelmode=(char)atoi(value);
   else if (strcmp(name,"decelmode1")==0)     data->config.axisConfig[0].m_decelmode=(char)atoi(value);
   else if (strcmp(name,"stopdecel1")==0)     data->config.axisConfig[0].mStopDecel=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"maxspeed1")==0)      data->config.axisConfig[0].m_axisMaxSpeed=speedValueToInc(data,0,oapc_util_atof(value),false);
   else if (strcmp(name,"reftimeout1")==0)    data->config.axisConfig[0].m_axisHomeTimeout=(unsigned char)atoi(value);
   else if (strcmp(name,"dirvalue1")==0)      
   {
      if (atoi(value)) data->config.axisConfig[0].flags|=AXISFLAG_DIRVALUE;
      else data->config.axisConfig[0].flags&=~AXISFLAG_DIRVALUE;
   }
   else if (strcmp(name,"refvalue1")==0)      
   {
      if (atoi(value)) data->config.axisConfig[0].flags|=AXISFLAG_REFVALUE;
      else data->config.axisConfig[0].flags&=~AXISFLAG_REFVALUE;
   }
   else if (strcmp(name,"limvaluea1")==0)      
   {
      if (atoi(value)) data->config.axisConfig[0].flags|=AXISFLAG_LIMVALUEA;
      else data->config.axisConfig[0].flags&=~AXISFLAG_LIMVALUEA;
   }
   else if (strcmp(name,"limvalueb1")==0)      
   {
      if (atoi(value)) data->config.axisConfig[0].flags|=AXISFLAG_LIMVALUEB;
      else data->config.axisConfig[0].flags&=~AXISFLAG_LIMVALUEB;
   }
   else if (strcmp(name,"autoleavelim1")==0)  data->config.axisConfig[0].autoleavelim=(char)atoi(value);
   else if (strcmp(name,"limitswitchdist1")==0) data->config.m_acfgLimitSwitchDist[0]=(unsigned short)valueToInc(data,0,oapc_util_atof(value));
   else if (strcmp(name,"refmode1")==0)       data->config.axisConfig[0].refmode=(char)atoi(value);
   else if (strcmp(name,"refin1")==0)         data->config.axisConfig[0].refin=(char)atoi(value);
   else if (strcmp(name,"refspeedin1")==0)    data->config.axisConfig[0].refspeedin=speedValueToInc(data,0,oapc_util_atof(value),true);
   else if (strcmp(name,"refspeedin2nd1")==0) data->config.axisConfig[0].refspeedin2nd=speedValueToInc(data,0,oapc_util_atof(value),true);
   else if (strcmp(name,"refspeedin3rd1")==0) data->config.axisConfig[0].refspeedin3rd=speedValueToInc(data,0,oapc_util_atof(value),true);
   else if (strcmp(name,"refpos1")==0)        data->config.axisConfig[0].refpos=valueToInc(data,0,oapc_util_atof(value));
   else if (strcmp(name,"limitain1")==0)      data->config.axisConfig[0].limitain=(unsigned char)atoi(value);
   else if (strcmp(name,"limitbin1")==0)      data->config.axisConfig[0].limitbin=(unsigned char)atoi(value);
   else if (strcmp(name,"axismode1")==0)      data->config.axisConfig[0].axisMode=(char)atoi(value);
   else if (strcmp(name,"useenc1")==0)        data->config.axisConfig[0].m_useenc=(char)atoi(value);
   else if (strcmp(name,"encfactor1")==0)     data->config.axisConfig[0].m_mEncfactor=(unsigned int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);

   else if (strcmp(name,"enable2")==0)        data->config.axisConfig[1].enable=(char)atoi(value);
   else if (strcmp(name,"factor2")==0)        data->config.axisConfig[1].factor=atoi(value);
   else if (strcmp(name,"llimit2")==0)        data->config.axisConfig[1].llimit=atoi(value);
   else if (strcmp(name,"hlimit2")==0)        data->config.axisConfig[1].hlimit=atoi(value);
   else if (strcmp(name,"accel2")==0)         data->config.axisConfig[1].mAccel=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"decel2")==0)         data->config.axisConfig[1].mDecel=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"accelmode2")==0)     data->config.axisConfig[1].m_accelmode=(char)atoi(value);
   else if (strcmp(name,"decelmode2")==0)     data->config.axisConfig[1].m_decelmode=(char)atoi(value);
   else if (strcmp(name,"stopdecel2")==0)     data->config.axisConfig[1].mStopDecel=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"maxspeed2")==0)      data->config.axisConfig[1].m_axisMaxSpeed=speedValueToInc(data,1,oapc_util_atof(value),false);
   else if (strcmp(name,"reftimeout2")==0)    data->config.axisConfig[1].m_axisHomeTimeout=(unsigned char)atoi(value);
   else if (strcmp(name,"dirvalue2")==0)      
   {
      if (atoi(value)) data->config.axisConfig[1].flags|=AXISFLAG_DIRVALUE;
      else data->config.axisConfig[1].flags&=~AXISFLAG_DIRVALUE;
   }
   else if (strcmp(name,"refvalue2")==0)      
   {
      if (atoi(value)) data->config.axisConfig[1].flags|=AXISFLAG_REFVALUE;
      else data->config.axisConfig[1].flags&=~AXISFLAG_REFVALUE;
   }
   else if (strcmp(name,"limvaluea2")==0)      
   {
      if (atoi(value)) data->config.axisConfig[1].flags|=AXISFLAG_LIMVALUEA;
      else data->config.axisConfig[1].flags&=~AXISFLAG_LIMVALUEA;
   }
   else if(strcmp(name,"limvalueb2")==0)
   {
       if(atoi(value)) data->config.axisConfig[1].flags|=AXISFLAG_LIMVALUEB;
       else data->config.axisConfig[1].flags&=~AXISFLAG_LIMVALUEB;
   }
   else if (strcmp(name,"autoleavelim2")==0)  data->config.axisConfig[1].autoleavelim=(char)atoi(value);
   else if (strcmp(name,"limitswitchdist2")==0) data->config.m_acfgLimitSwitchDist[1]=(unsigned short)valueToInc(data,1,oapc_util_atof(value));
   else if (strcmp(name,"refmode2")==0)       data->config.axisConfig[1].refmode=(char)atoi(value);
   else if (strcmp(name,"refin2")==0)         data->config.axisConfig[1].refin=(char)atoi(value);
   else if (strcmp(name,"refspeedin2")==0)    data->config.axisConfig[1].refspeedin=speedValueToInc(data,1,oapc_util_atof(value),true);
   else if (strcmp(name,"refspeedin2nd2")==0) data->config.axisConfig[1].refspeedin2nd=speedValueToInc(data,1,oapc_util_atof(value),true);
   else if (strcmp(name,"refspeedin3rd2")==0) data->config.axisConfig[1].refspeedin3rd=speedValueToInc(data,1,oapc_util_atof(value),true);
   else if (strcmp(name,"refpos2")==0)        data->config.axisConfig[1].refpos=valueToInc(data,1,oapc_util_atof(value));
   else if (strcmp(name,"limitain2")==0)      data->config.axisConfig[1].limitain=(unsigned char)atoi(value);
   else if (strcmp(name,"limitbin2")==0)      data->config.axisConfig[1].limitbin=(unsigned char)atoi(value);
   else if (strcmp(name,"axismode2")==0)      data->config.axisConfig[1].axisMode=(char)atoi(value);
   else if (strcmp(name,"useenc2")==0)        data->config.axisConfig[1].m_useenc=(char)atoi(value);
   else if (strcmp(name,"encfactor2")==0)     data->config.axisConfig[1].m_mEncfactor=(unsigned int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);

   else if (strcmp(name,"enable3")==0)        data->config.axisConfig[2].enable=(char)atoi(value);
   else if (strcmp(name,"factor3")==0)        data->config.axisConfig[2].factor=atoi(value);
   else if (strcmp(name,"llimit3")==0)        data->config.axisConfig[2].llimit=atoi(value);
   else if (strcmp(name,"hlimit3")==0)        data->config.axisConfig[2].hlimit=atoi(value);
   else if (strcmp(name,"accel3")==0)         data->config.axisConfig[2].mAccel=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"decel3")==0)         data->config.axisConfig[2].mDecel=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"accelmode3")==0)     data->config.axisConfig[2].m_accelmode=(char)atoi(value);
   else if (strcmp(name,"decelmode3")==0)     data->config.axisConfig[2].m_decelmode=(char)atoi(value);
   else if (strcmp(name,"stopdecel3")==0)     data->config.axisConfig[2].mStopDecel=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"maxspeed3")==0)      data->config.axisConfig[2].m_axisMaxSpeed=speedValueToInc(data,2,oapc_util_atof(value),false);
   else if (strcmp(name,"reftimeout3")==0)    data->config.axisConfig[2].m_axisHomeTimeout=(unsigned char)atoi(value);
   else if (strcmp(name,"dirvalue3")==0)      
   {
      if (atoi(value)) data->config.axisConfig[2].flags|=AXISFLAG_DIRVALUE;
      else data->config.axisConfig[2].flags&=~AXISFLAG_DIRVALUE;
   }
   else if (strcmp(name,"refvalue3")==0)      
   {
      if (atoi(value)) data->config.axisConfig[2].flags|=AXISFLAG_REFVALUE;
      else data->config.axisConfig[2].flags&=~AXISFLAG_REFVALUE;
   }
   else if (strcmp(name,"limvaluea3")==0)      
   {
      if (atoi(value)) data->config.axisConfig[2].flags|=AXISFLAG_LIMVALUEA;
      else data->config.axisConfig[2].flags&=~AXISFLAG_LIMVALUEA;
   }
   else if (strcmp(name,"limvalueb3")==0)      
   {
      if (atoi(value)) data->config.axisConfig[2].flags|=AXISFLAG_LIMVALUEB;
      else data->config.axisConfig[2].flags&=~AXISFLAG_LIMVALUEB;
   }
   else if (strcmp(name,"autoleavelim3")==0)  data->config.axisConfig[2].autoleavelim=(char)atoi(value);
   else if (strcmp(name,"limitswitchdist3")==0) data->config.m_acfgLimitSwitchDist[2]=(unsigned short)valueToInc(data,2,oapc_util_atof(value));
   else if (strcmp(name,"refmode3")==0)       data->config.axisConfig[2].refmode=(char)atoi(value);
   else if (strcmp(name,"refin3")==0)         data->config.axisConfig[2].refin=(char)atoi(value);
   else if (strcmp(name,"refspeedin3")==0)    data->config.axisConfig[2].refspeedin=speedValueToInc(data,2,oapc_util_atof(value),true);
   else if (strcmp(name,"refspeedin2nd3")==0) data->config.axisConfig[2].refspeedin2nd=speedValueToInc(data,2,oapc_util_atof(value),true);
   else if (strcmp(name,"refspeedin3rd3")==0) data->config.axisConfig[2].refspeedin3rd=speedValueToInc(data,2,oapc_util_atof(value),true);
   else if (strcmp(name,"refpos3")==0)        data->config.axisConfig[2].refpos=valueToInc(data,2,oapc_util_atof(value));
   else if (strcmp(name,"limitain3")==0)      data->config.axisConfig[2].limitain=(unsigned char)atoi(value);
   else if (strcmp(name,"limitbin3")==0)      data->config.axisConfig[2].limitbin=(unsigned char)atoi(value);
   else if (strcmp(name,"axismode3")==0)      data->config.axisConfig[2].axisMode=(char)atoi(value);
   else if (strcmp(name,"useenc3")==0)        data->config.axisConfig[2].m_useenc=(char)atoi(value);
   else if (strcmp(name,"encfactor3")==0)     data->config.axisConfig[2].m_mEncfactor=(unsigned int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);

   else if (strcmp(name,"enable4")==0)        data->config.axisConfig[3].enable=(char)atoi(value);
   else if (strcmp(name,"factor4")==0)        data->config.axisConfig[3].factor=atoi(value);
   else if (strcmp(name,"llimit4")==0)        data->config.axisConfig[3].llimit=atoi(value);
   else if (strcmp(name,"hlimit4")==0)        data->config.axisConfig[3].hlimit=atoi(value);
   else if (strcmp(name,"accel4")==0)         data->config.axisConfig[3].mAccel=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"decel4")==0)         data->config.axisConfig[3].mDecel=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"accelmode4")==0)     data->config.axisConfig[3].m_accelmode=(char)atoi(value);
   else if (strcmp(name,"decelmode4")==0)     data->config.axisConfig[3].m_decelmode=(char)atoi(value);
   else if (strcmp(name,"stopdecel4")==0)     data->config.axisConfig[3].mStopDecel=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"maxspeed4")==0)      data->config.axisConfig[3].m_axisMaxSpeed=speedValueToInc(data,3,oapc_util_atof(value),false);
   else if (strcmp(name,"reftimeout4")==0)    data->config.axisConfig[3].m_axisHomeTimeout=(unsigned char)atoi(value);
   else if (strcmp(name,"dirvalue4")==0)      
   {
      if (atoi(value)) data->config.axisConfig[3].flags|=AXISFLAG_DIRVALUE;
      else data->config.axisConfig[3].flags&=~AXISFLAG_DIRVALUE;
   }
   else if (strcmp(name,"refvalue4")==0)      
   {
      if (atoi(value)) data->config.axisConfig[3].flags|=AXISFLAG_REFVALUE;
      else data->config.axisConfig[3].flags&=~AXISFLAG_REFVALUE;
   }
   else if (strcmp(name,"limvaluea4")==0)      
   {
      if (atoi(value)) data->config.axisConfig[3].flags|=AXISFLAG_LIMVALUEA;
      else data->config.axisConfig[3].flags&=~AXISFLAG_LIMVALUEA;
   }
   else if (strcmp(name,"limvalueb4")==0)      
   {
      if (atoi(value)) data->config.axisConfig[3].flags|=AXISFLAG_LIMVALUEB;
      else data->config.axisConfig[3].flags&=~AXISFLAG_LIMVALUEB;
   }
   else if (strcmp(name,"autoleavelim4")==0)  data->config.axisConfig[3].autoleavelim=(char)atoi(value);
   else if (strcmp(name,"limitswitchdist4")==0) data->config.m_acfgLimitSwitchDist[3]=(unsigned short)valueToInc(data,3,oapc_util_atof(value));
   else if (strcmp(name,"refmode4")==0)       data->config.axisConfig[3].refmode=(char)atoi(value);
   else if (strcmp(name,"refin4")==0)         data->config.axisConfig[3].refin=(char)atoi(value);
   else if (strcmp(name,"refspeedin4")==0)    data->config.axisConfig[3].refspeedin=speedValueToInc(data,3,oapc_util_atof(value),true);
   else if (strcmp(name,"refspeedin2nd4")==0) data->config.axisConfig[3].refspeedin2nd=speedValueToInc(data,3,oapc_util_atof(value),true);
   else if (strcmp(name,"refspeedin3rd4")==0) data->config.axisConfig[3].refspeedin3rd=speedValueToInc(data,3,oapc_util_atof(value),true);
   else if (strcmp(name,"refpos4")==0)        data->config.axisConfig[3].refpos=valueToInc(data,3,oapc_util_atof(value));
   else if (strcmp(name,"limitain4")==0)      data->config.axisConfig[3].limitain=(unsigned char)atoi(value);
   else if (strcmp(name,"limitbin4")==0)      data->config.axisConfig[3].limitbin=(unsigned char)atoi(value);
   else if (strcmp(name,"axismode4")==0)      data->config.axisConfig[3].axisMode=(char)atoi(value);
   else if (strcmp(name,"useenc4")==0)        data->config.axisConfig[3].m_useenc=(char)atoi(value);
   else if (strcmp(name,"encfactor4")==0)     data->config.axisConfig[3].m_mEncfactor=(unsigned int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);

   else if (strcmp(name,"ondelay")==0)       data->config.m_ondelay=atoi(value)*1000;
   else if (strcmp(name,"offdelay")==0)      data->config.m_offdelay=atoi(value)*1000;
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
   save_config.version    =htons(2);
   save_config.length     =htons(sizeof(struct libio_config));
   save_config.reserved   =htons(data->config.reserved);
   strncpy(save_config.m_ip,data->config.m_ip,MAX_IP_SIZE);
   strncpy(save_config.m_passwd,data->config.m_passwd,DATA_LENGTH);
   save_config.m_flipflags=data->config.m_flipflags;

   for (int i=0; i<MAX_NUM_AXES; i++)
   {
      save_config.axisConfig[i].enable      =data->config.axisConfig[i].enable;
      save_config.axisConfig[i].mAccel      =htonl(data->config.axisConfig[i].mAccel);
      save_config.axisConfig[i].mDecel      =htonl(data->config.axisConfig[i].mDecel);
      save_config.axisConfig[i].m_accelmode =data->config.axisConfig[i].m_accelmode;
      save_config.axisConfig[i].m_decelmode =data->config.axisConfig[i].m_decelmode;
      save_config.axisConfig[i].mStopDecel  =htonl(data->config.axisConfig[i].mStopDecel);
      save_config.axisConfig[i].res1        =0;
      save_config.axisConfig[i].flags       =data->config.axisConfig[i].flags;
      save_config.axisConfig[i].autoleavelim=data->config.axisConfig[i].autoleavelim;
      save_config.axisConfig[i].factor      =htonl(data->config.axisConfig[i].factor);
      save_config.axisConfig[i].hlimit      =htonl(data->config.axisConfig[i].hlimit);
      save_config.axisConfig[i].llimit      =htonl(data->config.axisConfig[i].llimit);
      save_config.axisConfig[i].m_axisMaxSpeed=htonl(data->config.axisConfig[i].m_axisMaxSpeed);
      save_config.axisConfig[i].refin       =data->config.axisConfig[i].refin;
      save_config.axisConfig[i].refmode     =data->config.axisConfig[i].refmode;
      save_config.axisConfig[i].refpos      =htonl(data->config.axisConfig[i].refpos);
      save_config.axisConfig[i].limitain    =data->config.axisConfig[i].limitain;
      save_config.axisConfig[i].limitbin    =data->config.axisConfig[i].limitbin;
      save_config.axisConfig[i].refspeedin       =htonl(data->config.axisConfig[i].refspeedin);
      save_config.axisConfig[i].refspeedin2nd    =htonl(data->config.axisConfig[i].refspeedin2nd);
      save_config.axisConfig[i].refspeedin3rd    =htonl(data->config.axisConfig[i].refspeedin3rd);
      save_config.axisConfig[i].axisMode         =data->config.axisConfig[i].axisMode;
      save_config.axisConfig[i].m_axisHomeTimeout=data->config.axisConfig[i].m_axisHomeTimeout;
      save_config.axisConfig[i].m_useenc         =data->config.axisConfig[i].m_useenc;
      save_config.axisConfig[i].m_mEncfactor     =htonl(data->config.axisConfig[i].m_mEncfactor);

      //version 2
      save_config.m_acfgLimitSwitchDist[i]=htons(data->config.m_acfgLimitSwitchDist[i]);
   }
   save_config.m_ondelay  =htonl(data->config.m_ondelay);
   save_config.m_offdelay =htonl(data->config.m_offdelay);

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
   strncpy(data->config.m_ip,save_config.m_ip,MAX_IP_SIZE);
   strncpy(data->config.m_passwd,save_config.m_passwd,DATA_LENGTH);
   data->config.m_flipflags=save_config.m_flipflags;
   memset(data->config.m_fwlogfile_unused,0,MAX_FILENAMESIZE);

   for (int i=0; i<MAX_NUM_AXES; i++)
   {
      data->config.axisConfig[i].enable      =save_config.axisConfig[i].enable;
      data->config.axisConfig[i].mAccel      =ntohl(save_config.axisConfig[i].mAccel);
      data->config.axisConfig[i].mDecel      =ntohl(save_config.axisConfig[i].mDecel);
      data->config.axisConfig[i].m_accelmode =save_config.axisConfig[i].m_accelmode;
      data->config.axisConfig[i].m_decelmode =save_config.axisConfig[i].m_decelmode;
      data->config.axisConfig[i].mStopDecel  =ntohl(save_config.axisConfig[i].mStopDecel);
      data->config.axisConfig[i].res1        =0;
      data->config.axisConfig[i].flags       =save_config.axisConfig[i].flags;
      data->config.axisConfig[i].autoleavelim=save_config.axisConfig[i].autoleavelim;

      data->config.axisConfig[i].factor=ntohl(save_config.axisConfig[i].factor);
      if (data->config.axisConfig[i].factor<=0) data->config.axisConfig[i].factor=123;

      data->config.axisConfig[i].hlimit=ntohl(save_config.axisConfig[i].hlimit);
      data->config.axisConfig[i].llimit=ntohl(save_config.axisConfig[i].llimit);
      data->config.axisConfig[i].refin=save_config.axisConfig[i].refin;
      data->config.axisConfig[i].refmode=save_config.axisConfig[i].refmode;
      data->config.axisConfig[i].refpos  =ntohl(save_config.axisConfig[i].refpos);
      data->config.axisConfig[i].limitain=save_config.axisConfig[i].limitain;
      data->config.axisConfig[i].limitbin=save_config.axisConfig[i].limitbin;
      data->config.axisConfig[i].refspeedin=ntohl(save_config.axisConfig[i].refspeedin);
      data->config.axisConfig[i].refspeedin2nd=ntohl(save_config.axisConfig[i].refspeedin2nd);
      data->config.axisConfig[i].refspeedin3rd=ntohl(save_config.axisConfig[i].refspeedin3rd);
      data->config.axisConfig[i].axisMode=save_config.axisConfig[i].axisMode;
      data->config.axisConfig[i].m_axisMaxSpeed=ntohl(save_config.axisConfig[i].m_axisMaxSpeed);
      data->config.axisConfig[i].m_axisHomeTimeout=save_config.axisConfig[i].m_axisHomeTimeout;

      data->config.axisConfig[i].m_useenc         =save_config.axisConfig[i].m_useenc;
      data->config.axisConfig[i].m_mEncfactor     =ntohl(save_config.axisConfig[0].m_mEncfactor);

      //version 2
      if (data->config.version>=2) data->config.m_acfgLimitSwitchDist[i]=ntohs(save_config.m_acfgLimitSwitchDist[i]);
      else data->config.m_acfgLimitSwitchDist[i]=0;
   }
   data->config.m_ondelay  =ntohl(save_config.m_ondelay);
   data->config.m_offdelay =ntohl(save_config.m_offdelay);

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
   int i;

   flags=flags; // removing "unused" warning

   struct instData *data;

   data=(struct instData*)malloc(sizeof(struct instData));
   if (!data) return NULL;
   memset(data,0,sizeof(struct instData));

   data->config.version=1;
   data->config.length=sizeof(struct libio_config);
   data->config.reserved=0;
   strcpy(data->config.m_ip,"192.168.2.254");
   data->m_boardNum=0xFF;
   for (i=0; i<MAX_NUM_AXES; i++)
   {
      data->config.axisConfig[i].refin=3;
      data->config.axisConfig[i].mAccel=4*1000;
      data->config.axisConfig[i].mDecel=9*1000;
      data->config.axisConfig[i].m_accelmode=1;
      data->config.axisConfig[i].m_decelmode=1;
      data->config.axisConfig[i].mStopDecel=40*1000;

      data->config.axisConfig[i].m_axisHomeTimeout=120;
      data->config.axisConfig[i].m_axisMaxSpeed=2000000;

      data->config.axisConfig[i].factor=300;
      data->config.axisConfig[i].llimit=-100000; data->config.axisConfig[i].hlimit=100000;
      data->config.axisConfig[i].refmode=1;      
      data->config.axisConfig[i].refspeedin=300; data->config.axisConfig[i].refspeedin2nd=150; data->config.axisConfig[i].refspeedin3rd=80;
      data->config.axisConfig[i].axisMode=1;
      data->config.axisConfig[i].limitain=9;
      data->config.axisConfig[i].limitbin=9;
      data->config.axisConfig[i].m_useenc=3;
      data->config.axisConfig[i].m_mEncfactor=1*1000;
      data->runconfig[i].running=true;
      data->runconfig[i].newSpeed=300;
   }
   data->config.axisConfig[0].enable=1;

   data->config.axisConfig[0].refin=1;
   data->config.axisConfig[1].refin=3;
   data->config.axisConfig[2].refin=5;
   data->config.axisConfig[3].refin=7;
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


static void setMovementParams(struct instData *data,struct oapc_bin_struct_ctrl *ctrl,double x,double y,double z)
{
   double fac;
   double rdist;

   if (!(ctrl->validityFlags & TOOLPARAM_VALIDFLAG_Z))
   {
      rdist=sqrt(pow(data->lastX-x,2.0)+
                 pow(data->lastY-y,2.0));
      if (!data->m_toolOn) fac=(data->prevJumpspeed/1000.0)/rdist;
      else fac=(data->prevMarkspeed/1000.0)/rdist;
      data->runconfig[0].newPos=valueToInc(data,0,x);
      data->runconfig[0].newSpeed=speedValueToInc(data,0,fabs(data->lastX-x)*fac,true);
      data->runconfig[0].moveToPos=true;
      data->runconfig[1].newPos=valueToInc(data,1,y);
      data->runconfig[1].newSpeed=speedValueToInc(data,1,fabs(data->lastY-y)*fac,true);
      data->runconfig[1].moveToPos=true;
      data->runconfig[2].newSpeed=0;
   }
   else
   {
      //TODO: check if this calculation is really correct
      rdist=sqrt(pow(data->lastX-x,2.0)+
                 pow(data->lastY-y,2.0)+
                 pow(data->lastZ-z,2.0));
      if (!data->m_toolOn) fac=(data->prevJumpspeed/1000.0)/rdist;
      else fac=(data->prevMarkspeed/1000.0)/rdist;
      data->runconfig[0].newPos=valueToInc(data,0,x);
      data->runconfig[0].newSpeed=speedValueToInc(data,0,fabs(data->lastX-x)*fac,true);
      data->runconfig[0].moveToPos=true;
      data->runconfig[1].newPos=valueToInc(data,1,y);
      data->runconfig[1].newSpeed=speedValueToInc(data,1,fabs(data->lastY-y)*fac,true);
      data->runconfig[1].moveToPos=true;
      data->runconfig[2].newPos=valueToInc(data,2,z);
      data->runconfig[2].newSpeed=speedValueToInc(data,2,fabs(data->lastZ-z)*fac,true);
      data->runconfig[2].moveToPos=true;
   }
}


static void checkAxisData(struct instData *data,struct axis_runconfig *myParam,bool checkPositionOnly)
{
   long currPos;

   E1701M_get_axis_pos2(data->m_boardNum,myParam->axisNum,&currPos);
   if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) myParam->currPos=-currPos;
   else myParam->currPos=currPos;

   if (myParam->axisNum>2)
   {
      if ((data->runconfig[myParam->axisNum].currPos!=data->runconfig[myParam->axisNum].lastPos) ||
          (!checkPositionOnly))
      {
         data->runconfig[myParam->axisNum].lastPos=data->runconfig[myParam->axisNum].currPos;
         data->m_bin=oapc_util_alloc_bin_data(OAPC_BIN_TYPE_STRUCT,OAPC_BIN_SUBTYPE_STRUCT_AXISSTATE,OAPC_COMPRESS_NONE,0);
         if (data->m_bin)
         {
            data->m_bin->param1=htonl(myParam->axisNum);
            myParam->currentAxisSpeed=E1701M_get_axis_speed(data->m_boardNum,myParam->axisNum);
            data->m_bin->param2=htonl((int)OAPC_ROUND(incToValue(data,myParam->axisNum,myParam->currPos)*100.0,0));
            data->m_bin->param3=htonl((int)OAPC_ROUND(speedIncToValue(data,myParam->axisNum,myParam->currentAxisSpeed)*100.0,0));
            m_oapc_io_callback(OAPC_BIN_IO7,data->m_callbackID);
         }
      }
   }
   else
   {
      if (data->runconfig[myParam->axisNum].currPos!=data->runconfig[myParam->axisNum].lastPos)
      {
         if (myParam->axisNum==0) m_oapc_io_callback(OAPC_NUM_IO0,data->m_callbackID);
         else if (myParam->axisNum==1) m_oapc_io_callback(OAPC_NUM_IO2,data->m_callbackID);
         else if (myParam->axisNum==2) m_oapc_io_callback(OAPC_NUM_IO4,data->m_callbackID);
         else assert(0);
         data->runconfig[myParam->axisNum].lastPos=data->runconfig[myParam->axisNum].currPos;
      }

      if (!checkPositionOnly)
      {
         myParam->currentAxisSpeed=E1701M_get_axis_speed(data->m_boardNum,myParam->axisNum);
         if (myParam->axisNum==0) m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
         else if (myParam->axisNum==1) m_oapc_io_callback(OAPC_NUM_IO3,data->m_callbackID);
         else if (myParam->axisNum==2) m_oapc_io_callback(OAPC_NUM_IO5,data->m_callbackID);
         else assert(0);
      }
   }
}


static bool checkStop(struct instData *data,struct axis_runconfig *myParam)
{
   if (!myParam->doStop) return false;
   E1701M_stop(data->m_boardNum,(unsigned char)(1<<myParam->axisNum));
   myParam->doStop=false;

   // wait until movement has finished
   while (myParam->running)
   {
       if ((E1701M_get_axis_state(data->m_boardNum,myParam->axisNum) & E1701M_AXIS_STATE_MOVING)==0) break;
       oapc_util_thread_sleep(50);
   }


   return true;
}


static void leaveLimitSwitch(struct instData *data,struct axis_runconfig *myParam,unsigned char axisFlag,long prevPos)
{
   if ((myParam->m_stopOnEnter==0) && (myParam->m_stopOnLeave==0))
   {
      // we should never get here when no limit switches are set
      assert(0);
      return;
   }

   // stop when limit switches are free
   E1701M_set_stopcond(data->m_boardNum,(unsigned char)(1<<myParam->axisNum), myParam->m_stopOnLeave,myParam->m_stopOnEnter);

   // move to position we came from
   if (data->config.m_acfgLimitSwitchDist[myParam->axisNum]>0) // move some additional increments to leave the limit switch
   {
      long currPos;

      E1701M_get_axis_pos2(data->m_boardNum,myParam->axisNum,&currPos);
      if((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) currPos=-currPos;
      if (currPos>prevPos)
      {
         long dist=currPos-prevPos;

         if (dist<data->config.m_acfgLimitSwitchDist[myParam->axisNum]) dist=data->config.m_acfgLimitSwitchDist[myParam->axisNum];
         E1701M_move_rel(data->m_boardNum,axisFlag,-dist);
      }
      else if (currPos<prevPos)
      {
         long dist=prevPos-currPos;

         if (dist<data->config.m_acfgLimitSwitchDist[myParam->axisNum]) dist=data->config.m_acfgLimitSwitchDist[myParam->axisNum];
         E1701M_move_rel(data->m_boardNum,axisFlag,dist);
      }
      else // current and previous are equal, so axis was already at the limit before motion started
      {
         long dist=data->config.m_acfgLimitSwitchDist[myParam->axisNum];
         
         if (prevPos<myParam->newPos) dist=-dist;
         if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) dist=-dist;
         E1701M_move_rel(data->m_boardNum,axisFlag,dist);
      }
   }
   else
   {
      if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) E1701M_move_abs(data->m_boardNum,axisFlag,-prevPos);
      else E1701M_move_abs(data->m_boardNum,axisFlag,prevPos);
   }

   // wait until it starts moving
   while (myParam->running)
   {
      if (checkStop(data,myParam)) break;
      if ((E1701M_get_axis_state(data->m_boardNum,myParam->axisNum) & E1701M_AXIS_STATE_MOVING)==E1701M_AXIS_STATE_MOVING) break;
      oapc_thread_sleep(50);
   }

   // wait until movement has stopped
   while (myParam->running)
   {
      if (checkStop(data,myParam)) break;
      if ((E1701M_get_axis_state(data->m_boardNum,myParam->axisNum) & E1701M_AXIS_STATE_MOVING)==0) break;
      oapc_util_thread_sleep(50);
   }


   // reset to old condition
   E1701M_set_stopcond(data->m_boardNum,(unsigned char)(1<<myParam->axisNum),myParam->m_stopOnEnter,myParam->m_stopOnLeave);
}

#ifdef _is_new_stuff_
static void checkBusyState(struct instData *data,struct axis_runconfig *myParam,unsigned char busy)
{
   unsigned char lastBusy;

   oapc_thread_mutex_lock(data->m_busyStateMutex);
   lastBusy=myParam->m_busy;
   myParam->m_busy=(unsigned char)busy;
   if (busy==0)
   {
      if ((data->runconfig[0].moveToPos) || (data->runconfig[0].doHome) ||
          (data->runconfig[1].moveToPos) || (data->runconfig[1].doHome) ||
          (data->runconfig[2].moveToPos) || (data->runconfig[2].doHome) ||
          (data->runconfig[3].moveToPos) || (data->runconfig[3].doHome))
       busy=1;
       // there is a movement operation still pending for one of the other axes, so do not set busy state to 0
   }
   if (busy!=data->m_busy)
   {
      data->m_busy=busy;
      m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
   }
   oapc_thread_mutex_unlock(data->m_busyStateMutex);
}
#else
static void checkBusyState(struct instData *data,struct axis_runconfig *myParam,unsigned char busy,unsigned char *prevBusy,const bool force)
{
   oapc_thread_mutex_lock(data->m_busyStateMutex);
   if ((busy!=*prevBusy) || (force))
   {
//      if ((busy!=(data->runconfig[0].m_busy|data->runconfig[1].m_busy|data->runconfig[2].m_busy|data->runconfig[3].m_busy)) ||
//          (force))
      {
         static char prevUsedBusyState=0;

         myParam->m_busy=(unsigned char)busy;
         if (busy==0)
         {
            if ((data->runconfig[0].moveToPos) || (data->runconfig[0].doHome) ||
                (data->runconfig[1].moveToPos) || (data->runconfig[1].doHome) ||
                (data->runconfig[2].moveToPos) || (data->runconfig[2].doHome) ||
                (data->runconfig[3].moveToPos) || (data->runconfig[3].doHome))
             busy=1;
             // there is a movement operation still pending for one of the other axes, so do not set busy state to 0
         }

         if (prevUsedBusyState!=busy)
         {
            data->m_busy=busy;
            m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
            prevUsedBusyState=busy;
         }
      }
//      else myParam->m_busy=(unsigned char)busy;
      *prevBusy=busy;
   }
   oapc_thread_mutex_unlock(data->m_busyStateMutex);
}
#endif


static void stopAxis(struct axis_runconfig *myParam)
{
   myParam->doStop=true;
   myParam->doHome=false;
   myParam->newPos=myParam->currPos;
   myParam->moveToPos=false;
}


extern "C"
{
/**
This is the main thread which is started once for every axis which is configured. Every thread processes one axis exclusively,
checks incoming commands and invokes motion operations. The configuration parameters of every axis are handed over via parameter
"arg" which contains a data structure of type "struct axis_runconfig"
*/
static void* motorLoop( void*  arg )
{
   unsigned char                axisFlag,sendCnt;
   struct axis_runconfig       *myParam;
   struct instData             *data;
   unsigned char                prevBusy=0;
   struct oapc_bin_struct_ctrl *ctrl4List=NULL;
   int                          vecListCtr=0,polyDelay=0;
   long                         prevPos;

   myParam=(struct axis_runconfig*)arg;
   data=myParam->data;
   axisFlag=(unsigned char)(1 << myParam->axisNum);

   while (myParam->running)
   {
//      checkStop(data,myParam); // workaround: sometimes stopped-event is received before axis is stopped and therefore does not check for stop in loop
//      E1701M_get_axis_state(data->m_boardNum,myParam->axisNum); // same workaround: sometimes there are more state-changes in queue than there should be (network connection only)
      checkAxisData(data,myParam,true); // check if position has changed meanwhile (may be detected when encoders are used for axes)
      if (myParam->moveToPos)
      {
         checkBusyState(data,myParam,1,&prevBusy,false);

         prevPos=myParam->currPos;
         long currPos;

         E1701M_get_axis_pos2(data->m_boardNum,myParam->axisNum,&currPos);
         if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) myParam->currPos=-currPos;
         else myParam->currPos=currPos;

/*         if ((((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) && (myParam->currPos!=-myParam->newPos)) ||
             (((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)==0) && (myParam->currPos!=myParam->newPos)))*/
         if (myParam->currPos!=myParam->newPos)
         {
            unsigned int lastAxisState=0;

            E1701M_set_speed(data->m_boardNum,axisFlag,myParam->newSpeed);
            if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) E1701M_move_abs(data->m_boardNum,axisFlag,-myParam->newPos);
            else E1701M_move_abs(data->m_boardNum,axisFlag,myParam->newPos);

            // wait until it starts moving
            while (myParam->running)
            {
               if (checkStop(data,myParam))
                break;
               if ((E1701M_get_axis_state(data->m_boardNum,myParam->axisNum) & E1701M_AXIS_STATE_MOVING)==E1701M_AXIS_STATE_MOVING) break;
               oapc_thread_sleep(50);
            }
            // wait movement has finished
            sendCnt=0;
            while (myParam->running)
            {
               if (checkStop(data,myParam))
                break;
               lastAxisState&=~E1701M_AXIS_STATE_MOVING; // do only collect E1701M_AXIS_STATE_CONDSTOP since it happens exactly once
               lastAxisState|=E1701M_get_axis_state(data->m_boardNum,myParam->axisNum);
               if ((lastAxisState & E1701M_AXIS_STATE_MOVING)==0)
                break;
               oapc_util_thread_sleep(50);
               if (sendCnt>25)
               {
                  checkAxisData(data,myParam,false);
                  sendCnt=0;
               }
               sendCnt++;
            }
            if ((data->config.axisConfig[myParam->axisNum].autoleavelim) &&
                ((lastAxisState & E1701M_AXIS_STATE_CONDSTOP)==E1701M_AXIS_STATE_CONDSTOP))
            {
               leaveLimitSwitch(data,myParam,axisFlag,prevPos);
            }

            checkAxisData(data,myParam,false);
            myParam->currentAxisSpeed=0;            
         }
         else oapc_thread_sleep(250);
         myParam->moveToPos=0;
         if (data->m_ctrlList->empty())
          checkBusyState(data,myParam,0,&prevBusy,false);
      }
      else if (myParam->doHome)
      {
         unsigned int refFlags=0;
         time_t       homeStopTime=time(NULL)+data->config.axisConfig[myParam->axisNum].m_axisHomeTimeout;
         bool         refOK=true;

         checkBusyState(data,myParam,1,&prevBusy,false);

         if (data->config.axisConfig[myParam->axisNum].refmode>1)
         {
            switch (data->config.axisConfig[myParam->axisNum].refmode)
            {
               case 2:
                  if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) refFlags=E1701M_REFSTEP_1_ENTER_P;
                  else refFlags=E1701M_REFSTEP_1_ENTER_N;
                  break;
               case 3:
                  if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) refFlags=E1701M_REFSTEP_1_ENTER_N;
                  else refFlags=E1701M_REFSTEP_1_ENTER_P;
                  break;
               case 4:
                  if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) refFlags=E1701M_REFSTEP_1_ENTER_P|E1701M_REFSTEP_2_LEAVE_N;
                  else refFlags=E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_LEAVE_P;
                  break;
               case 5:
                  if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) refFlags=E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_LEAVE_P;
                  else refFlags=E1701M_REFSTEP_1_ENTER_P|E1701M_REFSTEP_2_LEAVE_N;
                  break;
               case 6:
                  if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) refFlags=E1701M_REFSTEP_1_ENTER_P|E1701M_REFSTEP_2_LEAVE_N;
                  else refFlags=E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_LEAVE_P;
                  break;
               case 7:
                  if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) refFlags=E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_LEAVE_P;
                  else refFlags=E1701M_REFSTEP_1_ENTER_P|E1701M_REFSTEP_2_LEAVE_N;
                  break;
               case 8:
                  if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) refFlags=E1701M_REFSTEP_1_ENTER_P|E1701M_REFSTEP_2_LEAVE_P;
                  else refFlags=E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_LEAVE_N;
                  break;
               case 9:
                  if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) refFlags=E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_LEAVE_N;
                  else refFlags=E1701M_REFSTEP_1_ENTER_P|E1701M_REFSTEP_2_LEAVE_P;
                  break;
               case 10:
                  if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) refFlags=E1701M_REFSTEP_1_ENTER_P|E1701M_REFSTEP_2_ENTER_N|E1701M_REFSTEP_3_LEAVE_P;
                  else refFlags=E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_ENTER_P|E1701M_REFSTEP_3_LEAVE_N;
                  break;
               case 11:
                  if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) refFlags=E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_ENTER_P|E1701M_REFSTEP_3_LEAVE_N;
                  else refFlags=E1701M_REFSTEP_1_ENTER_P|E1701M_REFSTEP_2_ENTER_N|E1701M_REFSTEP_3_LEAVE_P;
                  break;
               default:
                  assert(0);
                  break;
            }
            if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_REFVALUE)!=0) refFlags|=E1701M_REFSTEP_INV_SWITCH;
            E1701M_reference(data->m_boardNum,axisFlag,refFlags,(unsigned char)(1 << (data->config.axisConfig[myParam->axisNum].refin-1)),
                             data->config.axisConfig[myParam->axisNum].refspeedin,
                             data->config.axisConfig[myParam->axisNum].refspeedin2nd,
                             data->config.axisConfig[myParam->axisNum].refspeedin3rd,
                             0);
            // wait until it starts moving
            while (myParam->running)
            {
#ifndef _DEBUG
               if (time(NULL)>homeStopTime) stopAxis(myParam);
#endif
               if (checkStop(data,myParam))
               {
                  refOK=false;
                  break;
               }
               if ((E1701M_get_axis_state(data->m_boardNum,myParam->axisNum) & E1701M_AXIS_STATE_REFERENCING)==E1701M_AXIS_STATE_REFERENCING) break;
               oapc_thread_sleep(50);
            }
            checkAxisData(data,myParam,false);

            // wait movement has finished
            sendCnt=0;
            while (myParam->running)
            {
#ifndef _DEBUG
               if (time(NULL)>homeStopTime) stopAxis(myParam);
#endif
               if (checkStop(data,myParam))
               {
                  refOK=false;
                  break;
               }
               if ((E1701M_get_axis_state(data->m_boardNum,myParam->axisNum) & (E1701M_AXIS_STATE_REFERENCING|E1701M_AXIS_STATE_MOVING))==0) break;
               oapc_util_thread_sleep(50);
               if (sendCnt>25)
               {
                  checkAxisData(data,myParam,false);
                  sendCnt=0;
               }
               sendCnt++;
            } 
            if (refOK) E1701M_set_pos(data->m_boardNum,axisFlag,data->config.axisConfig[myParam->axisNum].refpos);
         }
         // for reference mode 1 - no referencing, just set new value
         else E1701M_set_pos(data->m_boardNum,axisFlag,data->config.axisConfig[myParam->axisNum].refpos);

         // wait until new position was written only when no stop was received
         if (refOK) while (myParam->running)
         {
#ifndef _DEBUG
            if (time(NULL)>homeStopTime) stopAxis(myParam);
#endif
            if (checkStop(data,myParam))
            {
               refOK=false;
               break;
            }
            if ((E1701M_get_axis_state(data->m_boardNum,myParam->axisNum) & E1701M_AXIS_STATE_SETPOS)==E1701M_AXIS_STATE_SETPOS) break;
            oapc_thread_sleep(50);
         }
         checkAxisData(data,myParam,false);
         myParam->currentAxisSpeed=0;
         myParam->doHome=0;
         if (data->m_ctrlList->empty())
          checkBusyState(data,myParam,0,&prevBusy,false);
      }
      if (ctrl4List)
      {
         double x,y,z;

         if (data->config.m_flipflags & FLIPFLAG_XY)
         {
            x=ctrl4List->coord[vecListCtr].y;
            y=ctrl4List->coord[vecListCtr].x;
         }
         else
         {
            x=ctrl4List->coord[vecListCtr].x;
            y=ctrl4List->coord[vecListCtr].y;
         }
         z=ctrl4List->coord[vecListCtr].z;
         if ((data->m_toolOn) && (polyDelay>0)) // tool stays on so we have to handle in-polygon delay
          oapc_thread_sleep(polyDelay);

         setMovementParams(data,ctrl4List,x/1000.0,y/1000.0,z/1000.0);

         data->lastX=x/1000.0;
         data->lastY=y/1000.0;
         data->lastZ=z/1000.0;

         vecListCtr++;
         if (vecListCtr>=ctrl4List->numCoords)
         {
            free(ctrl4List);
            ctrl4List=NULL;
         }
      }
      else if ((data->m_ctrlList) && (!data->m_ctrlList->empty()))
      {
         struct oapc_bin_struct_ctrl *ctrl=NULL;
         struct oapc_bin_head        *bin=NULL;
         double                       x,y,z;
         bool                         switchingOn=false,switchingOff=false;

         oapc_thread_mutex_lock(data->m_mutex);
         bin=data->m_ctrlList->front();
         data->m_ctrlList->pop();
         oapc_thread_mutex_unlock(data->m_mutex);

         ctrl=(struct oapc_bin_struct_ctrl*)&bin->data;
         ctrl->validityFlags=ntohl(ctrl->validityFlags);
         ctrl->numCoords=ntohs(ctrl->numCoords);

         if (data->config.m_flipflags & FLIPFLAG_XY)
         {
            x=(int)ntohl(ctrl->coord[0].y);
            y=(int)ntohl(ctrl->coord[0].x);
         }
         else
         {
            x=(int)ntohl(ctrl->coord[0].x);
            y=(int)ntohl(ctrl->coord[0].y);
         }
         z=(int)ntohl(ctrl->coord[0].z);

         if (ctrl->validityFlags & (TOOLPARAM_VALIDFLAG_ON))
         {
            if ((ctrl->on) && (!data->m_toolOn)) switchingOn=true;
            else if ((!ctrl->on) && (data->m_toolOn)) switchingOff=true;
            data->m_toolOn=ctrl->on;
         }
         if (ctrl->validityFlags & (TOOLPARAM_VALIDFLAG_OFFSPEED))
         {
            data->prevJumpspeed=(int)ntohl(ctrl->offSpeed);
         }
         if (ctrl->validityFlags & (TOOLPARAM_VALIDFLAG_ONSPEED))
         {
            data->prevMarkspeed=(int)ntohl(ctrl->onSpeed);
         }
         if (ctrl->validityFlags & (TOOLPARAM_VALIDFLAG_ONDELAY))
         {
            data->config.m_ondelay=(int)ntohl(ctrl->onDelay);
         }
         if (ctrl->validityFlags & (TOOLPARAM_VALIDFLAG_OFFDELAY))
         {
            data->config.m_offdelay=(int)ntohl(ctrl->offDelay);
         }
         if (ctrl->validityFlags & (TOOLPARAM_VALIDFLAG_PARAM3))
         {
            polyDelay=(int)OAPC_ROUND(ntohl(ctrl->toolParam[2])/1000.0,0);
         }

         setMovementParams(data,ctrl,x/1000.0,y/1000.0,z/1000.0);

         if (ctrl->numCoords>1)
         {
            if (ctrl4List) free(ctrl4List);

            ctrl4List=(struct oapc_bin_struct_ctrl*)malloc(sizeof(struct oapc_bin_head)+bin->sizeData);
            if (ctrl4List)
            {
               memcpy(ctrl4List,ctrl,sizeof(struct oapc_bin_head)+bin->sizeData);
               vecListCtr=1; // 0 is already handled
            }
         }
         free(bin);
         if ((switchingOn) && (data->config.m_ondelay<0)) oapc_thread_sleep(-data->config.m_ondelay);
         else if ((switchingOff) && (data->config.m_offdelay<0)) oapc_thread_sleep(-data->config.m_offdelay);

         data->lastX=x/1000.0;
         data->lastY=y/1000.0;
         data->lastZ=z/1000.0;
      }
      else
      {
         if ((data->m_busy==1) &&
             (data->runconfig[0].m_busy==0) &&
             (data->runconfig[1].m_busy==0) &&
             (data->runconfig[2].m_busy==0) &&
             (data->runconfig[3].m_busy==0) &&
             (data->runconfig[0].moveToPos==0) &&
             (data->runconfig[1].moveToPos==0) &&
             (data->runconfig[2].moveToPos==0) &&
             (data->runconfig[3].moveToPos==0) &&
             (data->runconfig[0].doHome==0) &&
             (data->runconfig[1].doHome==0) &&
             (data->runconfig[2].doHome==0) &&
             (data->runconfig[3].doHome==0)
             )
         {
            // this should never happen...
            assert(0);
            // ...but when it happens, we try to fix the situation
            checkBusyState(data,myParam,0,&prevBusy,true);
         }
         if (prevBusy!=0) 
         {
            oapc_thread_sleep(0);
         }
         else
         {
            long currPos;

            oapc_thread_signal_wait(myParam->m_signal,250);

            E1701M_get_axis_pos2(data->m_boardNum,myParam->axisNum,&currPos);
            if ((data->config.axisConfig[myParam->axisNum].flags & AXISFLAG_DIRVALUE)!=0) myParam->currPos=-currPos;
            else myParam->currPos=currPos;
         }
      }
   }
   if (ctrl4List) free(ctrl4List);
   ctrl4List=NULL;
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
   int              i;

   data=(struct instData*)instanceData;

   data->m_boardNum=E1701M_set_connection(data->config.m_ip);
   E1701M_set_password(data->m_boardNum,data->config.m_passwd);

   if (E1701M_open_connection(data->m_boardNum)!=E1701M_OK) return OAPC_ERROR_CONNECTION;
   E1701M_stop(data->m_boardNum,E1701M_AXIS_0|E1701M_AXIS_1|E1701M_AXIS_2|E1701M_AXIS_3);

   for (i=0; i<4; i++) if (data->config.axisConfig[i].enable)
   {
      E1701M_set_accels(data->m_boardNum,(unsigned char)(1<<i),data->config.axisConfig[i].mAccel/1000.0,
                                              data->config.axisConfig[i].mDecel/1000.0,
                                              data->config.axisConfig[i].mStopDecel/1000.0);
/*      E1701M_set_limits(data->m_boardNum,(unsigned char)(1<<i),valueToInc(data,i,data->config.axisConfig[i].llimit),
                                              valueToInc(data,i,data->config.axisConfig[i].hlimit),
                                              speedValueToInc(data,i,data->config.axisConfig[i].m_axisMaxSpeed,false));*/
      data->runconfig[i].m_stopOnEnter=0;
      data->runconfig[i].m_stopOnLeave=0;
      if (data->config.axisConfig[i].limitain<=8)
      {
         if ((data->config.axisConfig[i].flags & AXISFLAG_LIMVALUEA)==0) data->runconfig[i].m_stopOnEnter=(unsigned char)(1 << (data->config.axisConfig[i].limitain-1));
         else data->runconfig[i].m_stopOnLeave=(unsigned char)(1 << (data->config.axisConfig[i].limitain-1));
      }
      if (data->config.axisConfig[i].limitbin<=8)
      {
         if ((data->config.axisConfig[i].flags & AXISFLAG_LIMVALUEB)==0) data->runconfig[i].m_stopOnEnter|=(unsigned char)(1 << (data->config.axisConfig[i].limitbin-1));
         else data->runconfig[i].m_stopOnLeave|=(unsigned char)(1 << (data->config.axisConfig[i].limitbin-1));
      }
      E1701M_set_stopcond(data->m_boardNum,(unsigned char)(1<<i),data->runconfig[i].m_stopOnEnter,data->runconfig[i].m_stopOnLeave);

      if (data->config.axisConfig[i].m_useenc==1) E1701M_set_enc(data->m_boardNum,(unsigned char)i,0,data->config.axisConfig[i].m_mEncfactor/1000.0);
      else if (data->config.axisConfig[i].m_useenc==2) E1701M_set_enc(data->m_boardNum,(unsigned char)i,1,data->config.axisConfig[i].m_mEncfactor/1000.0);

      E1701M_set_accel_modes(data->m_boardNum,(unsigned char)i,data->config.axisConfig[i].m_accelmode,data->config.axisConfig[i].m_decelmode,0); // dirty hack, this assumes, option-numbers and mode-defines are the same :-(

      long currPos;
      
      E1701M_get_axis_pos2(data->m_boardNum, (unsigned char)i,&currPos);
      if ((data->config.axisConfig[i].flags & AXISFLAG_DIRVALUE)!=0) data->runconfig[i].currPos=-currPos;
      else data->runconfig[i].currPos=currPos;


      data->runconfig[i].lastPos=data->runconfig[i].currPos;
      data->runconfig[i].doStop=false;
      checkAxisData(data,&data->runconfig[i],true);
   }
   data->m_mutex=oapc_thread_mutex_create();
   if (!data->m_mutex) return OAPC_ERROR_NO_MEMORY;
   data->m_busyStateMutex=oapc_thread_mutex_create();
   if (!data->m_busyStateMutex) return OAPC_ERROR_NO_MEMORY;
   data->m_ctrlList=new std::queue<struct oapc_bin_head*>;
   if (!data->m_ctrlList) return OAPC_ERROR_NO_MEMORY;
   for (i=0; i<USE_MAX_NUM_AXES; i++) if (data->config.axisConfig[i].enable)
   {
      data->runconfig[i].axisNum=(unsigned char)i;
      data->runconfig[i].data=data;
      data->runconfig[i].m_refTimeoutTime=data->config.axisConfig[i].m_axisHomeTimeout;
      data->runconfig[i].m_signal=oapc_thread_signal_create();
      if (!data->runconfig[i].m_signal) return OAPC_ERROR_NO_MEMORY;
      oapc_thread_create(motorLoop,(void*)&data->runconfig[i]);
   }
   return OAPC_OK;
}



/**
This function is called before the application unloads everything, it has to be used to deinitialize
everything and to release used resources.
*/
OAPC_EXT_API unsigned long oapc_exit(void *instanceData)
{
   struct instData *data;
   int              i,j;
   
   data=(struct instData*)instanceData;
   data->running=false;
   E1701M_stop(data->m_boardNum,E1701M_AXIS_0|E1701M_AXIS_1|E1701M_AXIS_2|E1701M_AXIS_3);
   for (i=0; i<USE_MAX_NUM_AXES; i++)
   {
      data->runconfig[i].doStop=true;
      data->runconfig[i].running=false;
      oapc_thread_signal_send(data->runconfig[i].m_signal);
   }
   for (i=0; i<USE_MAX_NUM_AXES; i++)
   {
      for (j=0; j<100; j++)
      {
         if ((!data->config.axisConfig[i].enable) || (data->runconfig[i].running)) continue; // axis not used or has terminated
         oapc_util_thread_sleep(20); // wait some time
      }
      oapc_thread_signal_release(data->runconfig[i].m_signal);
   }
   if (data->m_ctrlList) delete data->m_ctrlList;
   data->m_ctrlList=NULL;
   if (data->m_mutex) oapc_thread_mutex_release(data->m_mutex);
   data->m_mutex=NULL;
   if (data->m_busyStateMutex) oapc_thread_mutex_release(data->m_busyStateMutex);
   data->m_busyStateMutex=NULL;

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

   if (input%2==0) // input 0,2,4
   {
      if (!data->runconfig[input/2].m_busy)
      {
         data->runconfig[input/2].newPos=valueToInc(data,input/2,value);
         data->runconfig[input/2].moveToPos=true;
         return OAPC_OK;
      }
      else return OAPC_ERROR_STILL_IN_PROGRESS;
   }
   else if (input%2==1) // input 1,3,5
   {
      if ((value==0) || (value==-2)) stopAxis(&data->runconfig[(input-1)/2]);
      else if (value==-1) data->runconfig[(input-1)/2].doHome=true;
//      else if (value<0) data->runconfig[(input-1)/2].newSpeed=value; wtf was this???
      else if (value>0) data->runconfig[(input-1)/2].newSpeed=speedValueToInc(data,(input-1)/2,value,true);
      else return OAPC_ERROR_INVALID_INPUT;
      return OAPC_OK;
   }
   return OAPC_ERROR_NO_SUCH_IO;
}


OAPC_EXT_API unsigned long  oapc_get_num_value(void* instanceData,unsigned long output,double* value)
{
   struct instData *data;
   
   data=(struct instData*)instanceData;

   if ((output%2==0) && (output<=4)) // input 0,2,4
   {
      *value=incToValue(data,output/2,data->runconfig[output/2].currPos);
      return OAPC_OK;
   }
   else if ((output%2==1) && (output<=5)) // input 1,3,5
   {
      *value=speedIncToValue(data,(output-1)/2,data->runconfig[(output-1)/2].currentAxisSpeed);
      return OAPC_OK;
   }

   return OAPC_ERROR_NO_SUCH_IO;
}


OAPC_EXT_API unsigned long  oapc_set_bin_value(void* instanceData,unsigned long input,struct oapc_bin_head *value)
{
   struct instData      *data;
   struct oapc_bin_head *bin;

   if (input!=7) return OAPC_ERROR_NO_SUCH_IO;
   if (value->type!=OAPC_BIN_TYPE_STRUCT) return OAPC_ERROR_INVALID_INPUT;
   if (value->subType==OAPC_BIN_SUBTYPE_STRUCT_CTRLEND) return OAPC_OK; // nothing to do in case of an end signal

   data=(struct instData*)instanceData;
   if (value->subType==OAPC_BIN_SUBTYPE_STRUCT_MOTIONCTRL)
   {
      int                                i;
      int                                bitmask=0x01;
      struct oapc_bin_struct_motionctrl *ctrl;
      double                             d;
      bool                               motionStarted=false;

      ctrl=(struct oapc_bin_struct_motionctrl*)&value->data;
      for (i=0; i<MAX_NUM_AXES; i++)
      {
         if ((ctrl->enableAxes & bitmask)==bitmask)
         {
            motionStarted=true;
            data->runconfig[i].doStop=false;
            if ((ctrl->stopAxes & bitmask)==bitmask) data->runconfig[i].doStop=true;
            else if (data->runconfig[i].currentAxisSpeed==0) // it is not moving
            {
               if ((ctrl->moveAxesToHome & bitmask)==bitmask) data->runconfig[i].doHome=true;
               else
               {
                  if ((ctrl->relativeMovement & bitmask)==bitmask)
                  {
                     d=incToValue(data,i,data->runconfig[i].currPos);
                     d=d+(((int)ntohl(ctrl->position[i]))/1000.0);
                  }
                  else d=((int)ntohl(ctrl->position[i]))/1000.0;
                  if (d>2147483600) d=2147483600.0;
                  else if (d<-2147483600) d=-2147483600.0;
                  data->runconfig[i].newPos=valueToInc(data,i,d);
                  data->runconfig[i].newSpeed=speedValueToInc(data,i,((int)ntohl(ctrl->speed[i]))/1000.0,true);
                  data->runconfig[i].moveToPos=true;
                  oapc_thread_signal_send(data->runconfig[i].m_signal);
               }
            }
            else return OAPC_ERROR_STILL_IN_PROGRESS;
         }
         bitmask=bitmask<<1;
      }
      if (!motionStarted) return OAPC_ERROR_INVALID_INPUT;
   }
   else if (value->subType==OAPC_BIN_SUBTYPE_STRUCT_CTRL)
   {
      int a;

      bin=(struct oapc_bin_head*)malloc(sizeof(struct oapc_bin_head)+value->sizeData);
      if (!bin) return OAPC_ERROR_NO_MEMORY;
      memcpy(bin,value,sizeof(struct oapc_bin_head)+value->sizeData-1); //-1 because head and data overlap by one byte at head-member "data"
      oapc_thread_mutex_lock(data->m_mutex);
      data->m_ctrlList->push(bin);
      oapc_thread_mutex_unlock(data->m_mutex);
      for (a=0; a<4; a++) if (data->runconfig[a].m_signal) oapc_thread_signal_send(data->runconfig[a].m_signal);
   }
   else if (value->subType==OAPC_BIN_SUBTYPE_STRUCT_AXISSTATE)
   {
      int axis=ntohl(value->param1);

      if ((axis>=0) && (axis<4))
       data->runconfig[axis].lastPos=data->runconfig[axis].currPos-1000000;
   }
   else if (value->subType==OAPC_BIN_SUBTYPE_STRUCT_STOPAXIS)
   {
      int axis=ntohl(value->param1);

      if ((axis>=0) && (axis<4))
       stopAxis(&data->runconfig[axis]);
   }
   else return OAPC_ERROR_INVALID_INPUT;
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
      *value=data->m_bin;
      return OAPC_OK;
   }
   else return OAPC_ERROR_NO_SUCH_IO;
}



OAPC_EXT_API void oapc_release_bin_data(void *instanceData,unsigned long /*output*/)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   oapc_util_release_bin_data(data->m_bin);
   data->m_bin=NULL;
}

