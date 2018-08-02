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
 #pragma warning (disable: 4018)
 #pragma warning (disable: 4663)
 #pragma warning (disable: 4710)
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
#endif

#include "oapc_libio.h"
#include "liboapc.h" // contains sleep function

#include <queue>

#define MAX_XML_SIZE   7000

#define MAX_NUM_AXES 3



#define FLIPFLAG_XY      0x0001
#define FLIPFLAG_MIRRORX 0x0002
#define FLIPFLAG_MIRRORY 0x0004

struct libio_config
{
   unsigned short       version,length;
   unsigned char        m_flipflags;
   int                  m_ondelay,m_offdelay;
};



struct axis_runconfig
{
   double newPos,currPos,newSpeed,currentAxisSpeed;
   bool   targetPosReached;
   void  *lastUpdateTime;
};



struct instData
{
   struct libio_config                config;
	struct axis_runconfig              runconfig[MAX_NUM_AXES];
   int                                m_callbackID;
   bool                               m_running;
   unsigned char                      m_toolOn,m_prevToolOn,m_busy;
   int                                prevJumpspeed,prevMarkspeed;
   void                              *m_mutex;
   std::queue<struct oapc_bin_head*> *m_ctrlList;
   double                             lastX,lastY,lastZ;
   struct oapc_bin_head              *m_bin;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
<flowimage>%s</flowimage>\n\
<dialogue>\n\
 <general>\n\
  <param>\n\
   <name>flipxy</name>\n\
   <text>Swap X and Y</text>\n\
   <type>checkbox</type>\n\
   <default>%d</default>\n\
  </param>\n\
 </general>\n\
 <stdpanel>\n\
  <name>Control Default</name>\n\
  <param>\n\
   <name>ondelay</name>\n\
   <text>On Delay</text>\n\
   <type>integer</type>\n\
   <default>%d</default>\n\
   <min>-10000</min>\n\
   <max>0</max>\n\
   <unit>msec</unit>\n\
  </param>\n\
  <param>\n\
   <name>offdelay</name>\n\
   <text>Off Delay</text>\n\
   <type>integer</type>\n\
   <default>%d</default>\n\
   <min>-10000</min>\n\
   <max>0</max>\n\
   <unit>msec</unit>\n\
  </param>\n\
 </stdpanel>\n\
 <helppanel>\n\
  <in7>CTRL - Control data</in7>\n\
  <out0>POS - current absolute axis position</out0>\n\
  <out1>MODE >0 current speed, =0 has stopped</out1>\n\
  <out2>...</out2>\n\
  <out6>BSY - Motion active</out6>\n\
  <out7>CTRL - Motion-synchronous control data</out7>\n\
 </helppanel>\n\
</dialogue>\n""\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAAAA3NCSVQICAjb4U/gAAAAFVBMVEUEAgT///+EgoT//wDEwsQAAAAA//8WIg75AAAACXBIWXMAAA7EAAAOxAGVKw4bAAABbklEQVRYhe2Y7Q7DIAhF0Wrf/5Gn9QtY2VirZFnGj2a3bB4RBokAhuaWGvht23yGLEcFSCgIFqgc1hGUASqFdQRlgEphFYQBKpSgLFCuEixQjfhH/VE/gVJMjGkopoMZKjUj/CZeQMX6szeo3PZUqJhNIEUN6miw7U1f7TSq89UKStgEXqW0cotc1aHR3+T9rUH18TRI9StnuXL4DHElcM/43FdpJDFXpBJI8pWiLdxJcq5wsjkXCepxT6hBQgfIUHcNOElb7EJFv/AAJ4lRRVYIpBI0HuAkMVcs306sBMkDnESiIgdISpo0Jp0HOElG3TXgJNQt6u56VDjfFwRw0otcST1BKfRT+DZKYbMO8AOUgb3tgUqPHkXneETVpPV8JYpMhwtCj7KwgiJ/ulWiPv2+774574v0GILexgRI32xdbILIC3fBbmPSrnxP2gThsKC3MWlXozVPEA4LdhsDHlXiGtFQZLKtESdzZpk9AOW/GIvvvNcsAAAAAElFTkSuQmCC";
static char                 libname[]="Control to Number";
static char                 xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports
static struct libio_config  save_config;



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
          OAPC_FLOWCAT_CONVERSION;
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
   return OAPC_BIN_IO7;
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

   sprintf(xmldescr,xmltempl,flowImage,
                             data->config.m_flipflags & FLIPFLAG_XY,
                             (int)OAPC_ROUND(data->config.m_ondelay/1000.0,0),(int)OAPC_ROUND(data->config.m_offdelay/1000.0,0)
                             );
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

   if (strcmp(name,"flipxy")==0)
   {
      if (atoi(value)) data->config.m_flipflags|=FLIPFLAG_XY;
      else data->config.m_flipflags&=~FLIPFLAG_XY;
   }
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
   save_config.version    =htons(1);
   save_config.length     =htons((unsigned short)*length);
   save_config.m_flipflags             =data->config.m_flipflags;
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
   data->config.m_flipflags=save_config.m_flipflags;
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
      data->runconfig[0].newPos=x;
      data->runconfig[1].newPos=y;
      data->runconfig[0].newSpeed=fabs(data->lastX-x)*fac;
      data->runconfig[1].newSpeed=fabs(data->lastY-y)*fac;
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
      data->runconfig[0].newPos=x;
      data->runconfig[1].newPos=y;
      data->runconfig[2].newPos=z;
      data->runconfig[0].newSpeed=fabs(data->lastX-x)*fac;
      data->runconfig[1].newSpeed=fabs(data->lastY-y)*fac;
      data->runconfig[2].newSpeed=fabs(data->lastZ-z)*fac;
   }
}



static void* convertLoop( void*  arg )
{
   struct instData             *data;
   int                          a;
   double                       currPos=0;
   unsigned char                allTargetPosReached,busy=0,prevBusy=0;
   struct oapc_bin_struct_ctrl *ctrl4List=NULL;
   int                          vecListCtr=0,polyDelay=0;
   void                        *currTime;

   data=(struct instData*)arg;
   for (a=0; a<MAX_NUM_AXES; a++)
   {
      data->runconfig[a].targetPosReached=true;
   }

   data->lastX=0;
   data->lastY=0;
   data->lastZ=0;

   while (data->m_running)
   {
      if (busy!=prevBusy)
      {
         data->m_busy=(unsigned char)busy;
         prevBusy=busy;
         m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
      }
      allTargetPosReached=true;
      for (a=0; a<MAX_NUM_AXES; a++)
      {
   		if (data->runconfig[a].targetPosReached)
	   	{
		   	if ((data->runconfig[a].newSpeed>0) && (data->runconfig[a].newPos!=0))
      		{
	   		   busy=1;
			      data->runconfig[a].targetPosReached=false;
			      allTargetPosReached=false;
		         // === start with your own, motion-specific implementation here ========================================
               // here the new target position and the speed to drive to that position have to be sent to the device
		         // === end with your own, motion-specific implementation here ==========================================
               data->runconfig[a].currentAxisSpeed=data->runconfig[a].newSpeed;
               data->runconfig[a].lastUpdateTime=oapc_util_get_time();
		      	data->runconfig[a].newSpeed=0;
               if (a==0) m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
               else if (a==1) m_oapc_io_callback(OAPC_NUM_IO3,data->m_callbackID);
               else if (a==2) m_oapc_io_callback(OAPC_NUM_IO5,data->m_callbackID);
            }
         }
         else allTargetPosReached=false;
   		// === start with your own, motion-specific implementation here ==========================================
   		// this block checks if the axes are still moving (in this example implementation it checks the simulated
   		// axes), in case they do and in case the current position is different to the preceding one the updated
   		// position is sent to the output
         if (OAPC_ROUND(data->runconfig[a].newPos,4)!=OAPC_ROUND(data->runconfig[a].currPos,4)) // is still moving
         {
            double diff;

          	currTime=oapc_util_get_time();
          	diff=oapc_util_diff_time(data->runconfig[a].lastUpdateTime,currTime);
          	data->runconfig[a].lastUpdateTime=oapc_util_get_time();
          	diff=diff*data->runconfig[a].currentAxisSpeed;
          	if (data->runconfig[a].newPos>data->runconfig[a].currPos)
          	{
          	   currPos=data->runconfig[a].currPos+diff;
          	   if (currPos>data->runconfig[a].newPos) currPos=data->runconfig[a].newPos;
          	}
          	else
          	{
          	   currPos=data->runconfig[a].currPos-diff;
               if (currPos<data->runconfig[a].newPos) currPos=data->runconfig[a].newPos;
          	}
         // === end with your own, motion-specific implementation here ============================================
            if (data->runconfig[a].currPos!=currPos)
            {
               data->runconfig[a].currPos=currPos;
               if (a==0) m_oapc_io_callback(OAPC_NUM_IO0,data->m_callbackID);
               else if (a==1) m_oapc_io_callback(OAPC_NUM_IO2,data->m_callbackID);
               else if (a==2) m_oapc_io_callback(OAPC_NUM_IO4,data->m_callbackID);
            }
         }
         else
         {
            data->runconfig[a].targetPosReached=true;

//            data->runconfig[a].newPos=0;
            if (data->m_ctrlList->empty())
             data->runconfig[a].currentAxisSpeed=0; // set speed to 0 only in case list is empty
            if (a==0) m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
            else if (a==1) m_oapc_io_callback(OAPC_NUM_IO3,data->m_callbackID);
            else if (a==2) m_oapc_io_callback(OAPC_NUM_IO5,data->m_callbackID);
         }
//         oapc_thread_sleep(0); // ???
      }
      if (data->m_running)
      {
         if (allTargetPosReached)
         {
            if (ctrl4List)
            {
               double x,y,z;

               if (data->config.m_flipflags & FLIPFLAG_XY)
               {
                  x=(int)ntohl(ctrl4List->coord[vecListCtr].y);
                  y=(int)ntohl(ctrl4List->coord[vecListCtr].x);
               }
               else
               {
                  x=(int)ntohl(ctrl4List->coord[vecListCtr].x);
                  y=(int)ntohl(ctrl4List->coord[vecListCtr].y);
               }
               z=(int)ntohl(ctrl4List->coord[vecListCtr].z);

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
            else if (!data->m_ctrlList->empty())
            {
               struct oapc_bin_struct_ctrl *ctrl=NULL;
               double                       x,y,z;
               bool                         switchingOn=false,switchingOff=false;

               busy=1;
               oapc_thread_mutex_lock(data->m_mutex);
               data->m_bin=data->m_ctrlList->front();
               data->m_ctrlList->pop();
               oapc_thread_mutex_unlock(data->m_mutex);

               ctrl=(struct oapc_bin_struct_ctrl*)&data->m_bin->data;
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
                  data->m_prevToolOn=ctrl->on;
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
                  polyDelay=(int)OAPC_ROUND(((int)ntohl(ctrl->toolParam[2]))/1000.0,0);
               }

               setMovementParams(data,ctrl,x/1000.0,y/1000.0,z/1000.0);
               data->m_toolOn=data->m_prevToolOn;

               if (ctrl->numCoords>1)
               {
                  if (ctrl4List) free(ctrl4List);

                  ctrl4List=(struct oapc_bin_struct_ctrl*)malloc(sizeof(struct oapc_bin_head)+data->m_bin->sizeData);
                  if (ctrl4List)
                  {
                     memcpy(ctrl4List,ctrl,sizeof(struct oapc_bin_head)+data->m_bin->sizeData);
                     vecListCtr=1; // 0 is already handled
                  }
               }

               m_oapc_io_callback(OAPC_BIN_IO7,data->m_callbackID); // this releases the bin-structure

               if ((switchingOn) && (data->config.m_ondelay<0))
                oapc_thread_sleep(data->config.m_ondelay/-1000);
               else if ((switchingOff) && (data->config.m_offdelay<0))
                oapc_thread_sleep(data->config.m_offdelay/-1000);

               data->lastX=x/1000.0;
               data->lastY=y/1000.0;
               data->lastZ=z/1000.0;
            }
            else
            {
               busy=0;
               if (busy==prevBusy)
                oapc_thread_sleep(250); // permanent loop to monitor external position changes
            }
         }
         else oapc_thread_sleep(0); // monitor positions faster when an axis is running
      }
      else break;
      oapc_thread_sleep(10); // have a delay of at least 10 milliseconds because of low resolution of windows time function
   }
   if (ctrl4List) free(ctrl4List);
   for (a=0; a<MAX_NUM_AXES; a++)
   {
      oapc_util_release_time(data->runconfig[a].lastUpdateTime);
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
   struct instData *data;

   data=(struct instData*)instanceData;

   data->m_mutex=oapc_thread_mutex_create();
   if (!data->m_mutex)
   {
      oapc_exit(instanceData);
      return OAPC_ERROR_RESOURCE;
   }
   data->m_ctrlList=new std::queue<struct oapc_bin_head*>;
   if (!data->m_ctrlList)
   {
      oapc_exit(instanceData);
      return OAPC_ERROR_NO_MEMORY;
   }
   data->m_running=true;
   if (!oapc_thread_create(convertLoop,data))
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
   if (data->m_ctrlList) delete data->m_ctrlList;
   data->m_ctrlList=NULL;
   if (data->m_mutex) oapc_thread_mutex_release(data->m_mutex);
   data->m_mutex=NULL;
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
   
   if (output>6) return OAPC_ERROR_NO_SUCH_IO;
   data=(struct instData*)instanceData;
   if (output%2==0) // input 0,2,4
   {
      *value=data->runconfig[output/2].currPos;
      return OAPC_OK;
   }
   else if (output%2==1) // input 0,2,4
   {
      *value=data->runconfig[(output-1)/2].currentAxisSpeed;
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
   if (value->subType==OAPC_BIN_SUBTYPE_STRUCT_CTRL)
   {
      bin=oapc_util_alloc_bin_data(0,0,0,value->sizeData);
      if (!bin) return OAPC_ERROR_NO_MEMORY;
      memcpy(bin,value,sizeof(struct oapc_bin_head)+value->sizeData-1); //-1 because head and data overlap by one byte at head-member "data"
      oapc_thread_mutex_lock(data->m_mutex);
      data->m_ctrlList->push(bin);
      oapc_thread_mutex_unlock(data->m_mutex);
   }
   else if (value->subType==OAPC_BIN_SUBTYPE_STRUCT_MOTIONCTRL)
   {
      int                                i;
      int                                bitmask=0x01;
      struct oapc_bin_struct_motionctrl *ctrl;
      double                             d;

      ctrl=(struct oapc_bin_struct_motionctrl*)&value->data;
      for (i=0; i<MAX_NUM_AXES; i++)
      {
         if ((ctrl->enableAxes & bitmask)==bitmask)
         {
            if (data->runconfig[i].currentAxisSpeed==0)
            {
               if ((ctrl->relativeMovement & bitmask)==bitmask) d=data->runconfig[i].currPos+(((int)ntohl(ctrl->position[i]))/1000.0);
               else d=((int)ntohl(ctrl->position[i]))/1000.0;
               data->runconfig[i].newPos=d;
               data->runconfig[i].newSpeed=((int)ntohl(ctrl->speed[i]))/1000.0;
            }
            else return OAPC_ERROR_STILL_IN_PROGRESS;
         }
         bitmask=bitmask<<1;
      }
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
      return OAPC_ERROR;
   }
   else return OAPC_ERROR_NO_SUCH_IO;
}



OAPC_EXT_API void oapc_release_bin_data(void *instanceData,unsigned long output)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   	
   oapc_util_release_bin_data(data->m_bin);
   data->m_bin=NULL;
}
