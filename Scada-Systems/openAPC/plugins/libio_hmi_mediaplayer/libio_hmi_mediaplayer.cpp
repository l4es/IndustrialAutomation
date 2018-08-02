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


#ifdef ENV_WINDOWS
#pragma warning (disable: 4018)
#pragma warning (disable: 4100)
#pragma warning (disable: 4663)
#pragma warning (disable: 4284)
#endif



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <vector>

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/event.h>

#if defined (ENV_LINUX) || defined (ENV_QNX)
 #include <sys/io.h>
 #include <arpa/inet.h>
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
#else
 #define snprintf _snprintf

 #ifdef ENV_WINDOWSCE
  #include <winsock2.h>
 #endif
#endif

#ifdef ENV_LINUX
#include <termios.h>
#include <unistd.h>
#endif

#include "oapc_libio.h"
#include "libio_hmi_mediaplayer.h"
#include "mpWindow.h"


#define MAX_XML_SIZE    5000


/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<oapc-config>\
<flowimage>%s</flowimage>\
<dialogue>\
 <stdpanel text=\"Default\">\
  <param text=\"Initial File\">\
   <name>file</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>5</min>\
   <max>%d</max>\
  </param>\
  <param text=\"Volume\">\
   <name>volume</name>\
   <type>float</type>\
   <default>%f</default>\
   <min>0</min>\
   <max>100</max>\
  </param>\
  <param text=\"Speed\">\
   <name>speed</name>\
   <type>float</type>\
   <default>%f</default>\
   <min>0.1</min>\
   <max>10</max>\
  </param>\
 </stdpanel>\
 <helppanel>\
  <in0>FILE - Path to file to be played</in0>\
  <in1>PLAY - Start playing</in1>\
  <in2>PAU - Pause playing</in2>\
  <in3>STP - Stop playing</in3>""\
  <in4>VOL - Audio volume [%%]></in4>\
  <in5>SPD - Replay speed [1x]</in5>\
  <out0>RDY - Ready for replaying</out0>\
  <out1>PLAY - Playing</out1>\
  <out2>POS - Play position</out2>\
 </helppanel>\
</dialogue>\
</oapc-config>";


static char                 libname[]="Mediaplayer";
static char                 xmldescr[MAX_XML_SIZE+1];
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUEAgSEgoQE/vz8AvxMRkzEwsTk5uQsKiykpqRkYmT8/vwcGRyUlpRcVlTU1tTk5vxAPDy0trR8enz08vSMjowMCgzMysw0MjSsrqxsamycnpy8vrxUTkzs6uzc2txcWlwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABIO5fMAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAB9UlEQVR42s2YC4+DIAyAqzt8jKHzzXTC//+Xx7ZsSjJYBdxdE82qxK+lj7ACfFHKsoTocDhEt1935Xg8YpQ4jt8qBnmgCCgUkMcjAuqDGEV9/a1iRd3cWmxSxuOU2KBYUcqtxSZlPE6JDYoVpaxa2eStWFFkbZO3YkWVmk3eihW1vzwz0GjWs/T8bX6hwLIDCBR8XvZEGW1HeoVY9v1YWbYJGyt/FJQotwGd7NpWg4NXrrGCXetKN2qt4LjweeFHr7QgNBkbzbECdLcwtLa1v2L4qarr0DaCFJvdWnULhPRtdhropWrTYUx43fvHyphNjegnPvLonLZV3qVZkbmVMLZGmqTop+okZTam591QPGs4G2fOTnPFooSnuSfKvIGSRtNPV09MXbK7kFz6ooySQ8dY113l/YJr3gZAvX/KKWVMSna/SXnuxt1QhNZN8pK5pw4owLXOG2qei2SeZ6FuTihkp1OoRIi+EEIU6kaoU12ZOqzWcAi9iH6RyQll7LBaH1WofiIv4S4oMJfw2i1Cc1ItklHvxmRJi/NpJbXbBhpLef2GDFSXKujpVnsz1ppsyfXPR85wJ8U/OHKqrAPnA/vGWIH7WTJUrLB/ev5hrJZhiVUJgsLNXEKgkDOXMGmBm7mEQCFnLmFK2GPMshHlM2bBo74mv1rXDhQ+eyKxAAAAAElFTkSuQmCC";
static struct configData    saveConfig;
       lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports


#define OWN_EVENT_FILE_CHANGED 10001
#define OWN_EVENT_PLAY         10002
#define OWN_EVENT_PAUSE        10003
#define OWN_EVENT_STOP         10004
#define OWN_EVENT_SET_VOLUME   10005
#define OWN_EVENT_SET_SPEED    10006

class EventHandler : public wxEvtHandler
{
public:
   EventHandler(struct instData *data):wxEvtHandler()
      ,m_data(data)
   {
      Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(EventHandler::OnCommandEvent),NULL,this);
   }

   void triggerEvent(int id)
   {
      wxCommandEvent* evt = new wxCommandEvent();

      evt->SetEventType(wxEVT_COMMAND_BUTTON_CLICKED);
      evt->SetId(id);
      AddPendingEvent(*evt);
   }

private:
   void OnCommandEvent(wxCommandEvent &evt)
   {
      switch (evt.GetId())
      {
         case OWN_EVENT_FILE_CHANGED:
            if (strlen(m_data->m_newFilename)>0)
            {
               m_data->m_mpWindow->load(m_data,m_data->m_newFilename);
               m_data->m_newFilename[0]=0;
            }
            break;
         case OWN_EVENT_PLAY:
            m_data->m_mpWindow->Play();
            break;
         case OWN_EVENT_PAUSE:
            m_data->m_mpWindow->Pause();
            break;
         case OWN_EVENT_STOP:
            m_data->m_mpWindow->Stop();
            break;
         case OWN_EVENT_SET_VOLUME:
            m_data->m_mpWindow->SetVolume(m_data->config.m_mVolume/100000.0);
            break;
         case OWN_EVENT_SET_SPEED:
            m_data->m_mpWindow->SetPlaybackRate(m_data->config.m_mSpeed/100.0);
            break;
         default:
            assert(0);
            break;
      }
   }

   struct instData *m_data;
};


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
          OAPC_HAS_XML_CONFIGURATION|           // for the custom HMI configuration panel
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|OAPC_ACCEPTS_IO_CALLBACK|
          OAPC_HMICAT_DISPLAY|
          OAPC_USERPRIVI_HIDE;
}



/**
This function returns a set of OAPC_HMI_NO_... flags that modify the general configuration dialogue
within the HMI Editor. These flags decide which parts of that dialogue have NOT to be accessible and
can be taken from oapc_libio.h
@return or-concatenated flags that describe which user interface properties are not supported by this
        type of HMI object
*/
OAPC_EXT_API unsigned long oapc_get_no_ui_flags(void)
{
   return OAPC_HMI_NO_UI_RO|OAPC_HMI_NO_UI_DISABLED|OAPC_HMI_NO_UI_FG|OAPC_HMI_NO_UI_BG|OAPC_HMI_NO_UI_FONT|OAPC_HMI_NO_UI_MINMAX|OAPC_HMI_NO_UI_TEXT;
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
   return OAPC_CHAR_IO0|
          OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|
          OAPC_NUM_IO4|OAPC_NUM_IO5;
   // avoid it to submit the same IO number for different data types, that would cause displaying problems for the flow symbol
}



OAPC_EXT_API unsigned long oapc_get_output_flags(void)
{
   return OAPC_DIGI_IO0|OAPC_DIGI_IO1|
          OAPC_NUM_IO2;
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
                                   data->config.m_filename,MAX_FILENAME_LEN,
                                   data->config.m_mVolume/1000.0,data->config.m_mSpeed/1000.0);
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

   if (strcmp(name,"file")==0)        strncpy(data->config.m_filename,value,MAX_FILENAME_LEN);
   else if (strcmp(name,"volume")==0) data->config.m_mVolume=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"speed")==0)  data->config.m_mSpeed=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
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

   *length=sizeof(struct configData);
   saveConfig.version    =htons(1);
   saveConfig.length     =htons(sizeof(struct configData));
   strncpy(saveConfig.m_filename,data->config.m_filename,MAX_FILENAME_LEN);
   saveConfig.m_mVolume  =htonl(data->config.m_mVolume);
   saveConfig.m_mSpeed   =htonl(data->config.m_mSpeed);
   saveConfig.res1=0;
   saveConfig.res2=0;
   return (char*)&saveConfig;
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

   if (length>sizeof(struct configData)) length=sizeof(struct configData);
   memcpy(&saveConfig,loadedData,length);

   strncpy(data->config.m_filename,saveConfig.m_filename,MAX_FILENAME_LEN);
   data->config.m_mVolume=ntohl(saveConfig.m_mVolume);
   data->config.m_mSpeed =ntohl(saveConfig.m_mSpeed);
   data->config.res1=0;
   data->config.res2=0;
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
   data->config.m_mSpeed=1*1000;
   data->config.m_mVolume=100*1000;
   return data;
}



/**
This function is called finally, it has to be used to release the instance data structure that was created
during the call of oapc_create_instance()
*/
OAPC_EXT_API void oapc_delete_instance(void* instanceData)
{
/*   struct instData *data;

   data=(struct instData*)instanceData;*/

   if (instanceData) free(instanceData);
}


static void *posLoop(void *arg)
{
   struct instData             *data;

   data=(struct instData*)arg;
   while (data->m_running)
   {
       oapc_thread_sleep(500);
       if (data->m_mpWindow) m_oapc_io_callback(OAPC_NUM_IO2,data->m_callbackID);
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

   data=(struct instData*)instanceData;
   data->m_eventHandler=new EventHandler(data);
   if (!data->m_eventHandler) return OAPC_ERROR_NO_MEMORY;
   data->m_running=true;
   if (!oapc_thread_create(posLoop,data)) return OAPC_ERROR_NO_MEMORY;
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
   if (data->m_mpWindow) data->m_mpWindow->Stop();
   if (data->m_running)
   {
      data->m_running=false;
      while ((!data->m_running) && (ctr<10)) // wait for thread to finish
      {
         oapc_thread_sleep(250);
         ctr++;
      }
   }

   if (data->m_eventHandler) delete data->m_eventHandler;
   data->m_eventHandler=NULL;

   return OAPC_OK;
}



/**
This function is called by the application whenever the element has to be repainted. That may happen
because a new value was set that has to be displayed or because the element was hidden and has to be
redrawn now. So the Plug-In does not take care about the states and conditions under which a repaint
is necessary, this function is called automatically.
@param[in] dc the wxWidgets drawing context structure that has to be used for painting the HMI element
@param[in] canvas the wxWindow element that was created by the main application in order to allocate
           a private area of defined size for the Plug-In to draw within
*/
OAPC_EXT_API void oapc_paint(void* instanceData,wxAutoBufferedPaintDC *dc,wxPanel *canvas)
{
   wxInt32          w,h;
   struct instData *data;

   data=(struct instData*)instanceData;

   canvas->GetSize(&w,&h);
   if (!data->m_mpWindow)
   {
      data->m_mpWindow=new mpWindow(canvas,wxSize(w,h),data);
      if (strlen(data->config.m_filename)>0) data->m_mpWindow->load(data,data->config.m_filename);
   }
   else
   {
      wxInt32 ow,oh;

      data->m_mpWindow->GetSize(&ow,&oh);
      if ((w!=ow) || (h!=oh)) data->m_mpWindow->SetSize(w,h);
   }
}




/**
Here the default size of the element has to be returned; this size is used when it is created
newly
@param[out] x the default size in x-direction (width)
@param[out] y the default size in y-direction (height)
*/
OAPC_EXT_API void oapc_get_defsize(wxFloat32 *x,wxFloat32 *y)
{
   *x=320;
   *y=180;
}



/**
This function specifies the minimum size that is allowed for this HMI element, no scaling
operation of the main application will set smaller sizes to the related canvas than specified by
this function
@param[out] x the minimum size in x-direction (width)
@param[out] y the minimum size in y-direction (height)
*/
OAPC_EXT_API void oapc_get_minsize(void* WXUNUSED(instanceData),wxFloat32 *x,wxFloat32 *y)
{
   *x=32;
   *y=18;
}



/**
This function specifies the maximum size that is allowed for this HMI element, no scaling
operation of the main application will set bigger sizes to the related canvas than specified by
this function
@param[out] x the maximum size in x-direction (width)
@param[out] y the maximum size in y-direction (height)
*/
OAPC_EXT_API void oapc_get_maxsize(void* WXUNUSED(instanceData),wxFloat32 *x,wxFloat32 *y)
{
   *x=1920;
   *y=1080;
}



/**
The foreground and background colour of an element are handled by the standard HMI properties dialogue.
This function is called once during first setup of this control and has to return the default colour
values for it. Afterwards these values are stored by the main application, the Plug-In does not to
take care about them. Within the painting-function (please see above) you can get these colours by
calling canvas->GetBackgroundColour() and canvas->GetForegroundColour()
@param[out] background the default background colour
@param[out] foreground the default foreground colour
*/
OAPC_EXT_API void oapc_get_colours(wxUint32 *background,wxUint32 *foreground)
{
   *foreground=0x110000;
   *background=0xFFF0F0;
}


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

   if (value==1)
   {
      if (input==1) data->m_eventHandler->triggerEvent(OWN_EVENT_PLAY);
      else if (input==2) data->m_eventHandler->triggerEvent(OWN_EVENT_PAUSE);
      else if (input==3) data->m_eventHandler->triggerEvent(OWN_EVENT_STOP);
      else return OAPC_ERROR_NO_SUCH_IO;
   }
   return OAPC_OK;
}


OAPC_EXT_API unsigned long  oapc_get_digi_value(void* instanceData,unsigned long output,unsigned char *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (output==0) *value=1;
   else if (output==1) *value=data->m_digi1;
   else return OAPC_ERROR_NO_SUCH_IO;
   return OAPC_OK;
}


OAPC_EXT_API unsigned long  oapc_get_num_value(void* instanceData,unsigned long output,double *value)
{
    struct instData *data;

    data=(struct instData*)instanceData;
    if (output==2) *value=data->m_mpWindow->Tell();
    else return OAPC_ERROR_NO_SUCH_IO;   
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
   if (input==4)
   {
      if ((value<0) || (value>100)) return OAPC_ERROR_INVALID_INPUT;
      data->config.m_mVolume=value*1000;
      data->m_eventHandler->triggerEvent(OWN_EVENT_SET_VOLUME);
   }
   else if (input==5)
   {
       if ((value<0.1) || (value>10)) return OAPC_ERROR_INVALID_INPUT;
       data->config.m_mSpeed=value*1000;
       data->m_eventHandler->triggerEvent(OWN_EVENT_SET_SPEED);
   }
   else return OAPC_ERROR_NO_SUCH_IO;
   return OAPC_OK;
}


OAPC_EXT_API unsigned long  oapc_set_char_value(void* instanceData,unsigned long input,char *value)
{
   struct instData           *data;

   data=(struct instData*)instanceData;
   if (input==0)
   {
      data->m_mpWindow->Stop();
      strncpy(data->m_newFilename,value,MAX_FILENAME_LEN);
      data->m_eventHandler->triggerEvent(OWN_EVENT_FILE_CHANGED);
   }
   else return OAPC_ERROR_NO_SUCH_IO;
   return OAPC_OK;
}
