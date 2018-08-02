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

#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/dirdlg.h>
#include <wx/dialog.h>
#include <wx/filename.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef ENV_WINDOWSCE
#include <sys/types.h>
#endif

#ifndef ENV_WINDOWS
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#define closesocket close
#else
 #ifdef ENV_WINDOWSCE
  #include <winsock2.h>
 #endif
#endif

#include "oapc_libio.h"
#include "liboapc.h"
#include "libioFileDialogue.h"
#include "libioDirDialogue.h"

#define MAX_XML_SIZE        5000
#define MAX_BUTTONNAME_SIZE   31
#define MAX_NAME_SIZE         31
#define MAX_TEXT_SIZE        150

struct libio_config
{
   unsigned short version,length;
   unsigned int   reserved1;
   wxInt32        pointSize,style,weight,reserved2;
   char           faceName[MAX_NAME_SIZE+1];
   unsigned int   reserved3,reserved4,reserved5,reserved6;
   char           loadTitle[MAX_NAME_SIZE+1],loadWildcard[MAX_TEXT_SIZE+1];
   char           saveTitle[MAX_NAME_SIZE+1],saveWildcard[MAX_TEXT_SIZE+1];
   char           dirTitle[MAX_NAME_SIZE+1];
};


class EventHandler;

struct instData
{
   struct libio_config  config;
   int                  m_callbackID,m_lastResult,m_currentDialog;
   wxString            *loadDir,*loadFile,*loadPath,*saveDir,*saveFile,*savePath,*dirDir;
   EventHandler        *m_eventHandler;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
 <flowimage>%s</flowimage>\n\
 <dialogue>\n\
  <general>\n\
   <param>\n\
    <name>font</name>\n\
    <text>Font</text>\n\
    <type>fontbutton</type>\n\
    <pointsize>%d</pointsize>\n\
    <style>%d</style>\n\
    <weight>%d</weight>\n\
    <face>%s</face>\n\
   </param>\n\
   <param>\n\
    <name>loadtitle</name>\n\
    <text>Load Dialogue Title</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>3</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>loadwildcard</name>\n\
    <text>Load Dialogue Wildcard</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>3</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>savetitle</name>\n\
    <text>Save Dialogue Title</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>3</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>savewildcard</name>\n\
    <text>Save Dialogue Wildcard</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>3</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>dirtitle</name>\n\
    <text>Directory Dialogue Title</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>3</min>\n\
    <max>%d</max>\n\
   </param>\n\
  </general>\n\
  <helppanel>\n\
   <in0>LOAD - open file load dialogue</in0>\n\
   <in1>SAVE - open file save dialogue</in1>\n\
   <in2>DIR - open directory selection dialogue</in2>\n\
   <out0>LOAD - selected file</out0>\n\
   <out1>SAVE - choosen filename</out1>\n\
   <out2>DIR - selected directory</out2>\n\
  </helppanel>\n\
 </dialogue>\n\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyBAMAAABG0lM+AAAAMFBMVEUEAgSEgoTEwsQEAoQEgvz8Avy8vvz8/vzEwvwAAAAAAAAAAAAAAAAAAAAAAAAAAAByolOXAAABdklEQVR42s2WQW6DQAxFTaXI226ibNnlAEjNAVqFLtsF93Apkq/fwXbaAWbAQ1spXuFoXr79MWCAXYFc8Qcy04kfmS3ZCApUzXRm7pEfwg+aOCg6jSJciYgl21SPowjXImLJNsWVdEIqUqGTqqWTXkXqs5MivVYRYidVHEIpiSqG4KNIq0IU570VGgWQpF4m0f5QAPHfzCpsJ9eXuRuQbqjF0CnqqdeniLLzPSa1CBET1K2VPnkT4goTlInMb2BLIVirTFVoo0HzviAcIrkxEyoekyWVqfA2G5h2PuPh92yknc95qFRmkDcqzIR5KGrXpRtImdkQD2k5UdHMr1ZYQImHamNMmdnEuOIhzqn12HhSHB4WUFkP/77C6XtjjxaXVhh0hvCKKqRa/hyCVCH13DRD01z+t68gEsXlt9+Uu6dkVRmfMpA3MPi2lKp7H6ljJ7uNJJ7dpnsbd5tDJ7uNJJ5v5VF3GxGxxLHbHHS3ERFL7mm32RVfE4ATRZR5jcwAAAAASUVORK5CYII=";
static char                 libname[]="Filedialogue";
static char                 xmldescr[MAX_XML_SIZE+1];
static struct libio_config  save_config;
static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports


#define OWN_EVENT_MASK 10000

class EventHandler : public wxFrame
{
public:
   EventHandler(struct instData *data):wxFrame(NULL,wxID_ANY,_T("hiddenparent"),wxPoint(0,0),wxSize(1,1),wxSTAY_ON_TOP)
      ,m_data(data)
   {
      Hide();
      Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(EventHandler::OnCommandEvent),NULL,this);
   }

   void triggerEvent(int id)
   {
      wxCommandEvent* evt = new wxCommandEvent();

      evt->SetEventType(wxEVT_COMMAND_BUTTON_CLICKED);
      evt->SetId(id);
      AddPendingEvent(*evt);
   }

   void OnCommandEvent(wxCommandEvent &evt)
   {
      wxMBConvUTF8       conv;
      wchar_t            wc[MAX_TEXT_SIZE];
      wxString           title,wildcard;
      bool               dialogueCancelled=false;

      m_data->m_currentDialog=evt.GetId();
      if ((m_data->config.pointSize>3) && (m_data->config.pointSize<1000))
      {
         conv.MB2WC(wc,m_data->config.faceName, MAX_TEXT_SIZE);      
         SetFont(wxFont(m_data->config.pointSize, wxFONTFAMILY_DEFAULT,
                 m_data->config.style, m_data->config.weight,
                 false, wc));
      }

      if (evt.GetId()==OWN_EVENT_MASK+0)
      {
         libioFileDialogue *openFileDialog=NULL;

         conv.MB2WC(wc,m_data->config.loadWildcard,MAX_TEXT_SIZE);
         wildcard=wc;
         conv.MB2WC(wc,m_data->config.loadTitle,MAX_NAME_SIZE);
         title=wc;
         if (!m_data->loadDir)  m_data->loadDir=new wxString();
         if (!m_data->loadFile) m_data->loadFile=new wxString();
         if (!m_data->loadPath) m_data->loadPath=new wxString();
         openFileDialog=new libioFileDialogue(this,title,*m_data->loadDir,*m_data->loadFile,wildcard,wxID_OPEN, wxDefaultPosition);
         openFileDialog->Centre();
         openFileDialog->ShowModal();
         if (!openFileDialog->wasCancelled())
         {
            *m_data->loadPath=openFileDialog->GetPath();
            oapc_path_split(m_data->loadPath,m_data->loadDir,m_data->loadFile,wxEmptyString);
            dialogueCancelled=false;
         }
         else dialogueCancelled=true;
         delete openFileDialog;
         if (!dialogueCancelled) m_oapc_io_callback(OAPC_CHAR_IO0,m_data->m_callbackID);
//#endif
      }
      else if (evt.GetId()==OWN_EVENT_MASK+1)
      {
         libioFileDialogue *saveFileDialog=NULL;

         conv.MB2WC(wc,m_data->config.saveWildcard,MAX_TEXT_SIZE);
         wildcard=wc;
         conv.MB2WC(wc,m_data->config.saveTitle,MAX_TEXT_SIZE);
         title=wc;
         if (!m_data->saveDir) m_data->saveDir=new wxString();
         if (!m_data->saveFile) m_data->saveFile=new wxString();
         if (!m_data->savePath) m_data->savePath=new wxString();

         saveFileDialog=new libioFileDialogue(this,title,*m_data->saveDir,*m_data->saveFile,wildcard,wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
         saveFileDialog->Centre();
         saveFileDialog->ShowModal();
         if (!saveFileDialog->wasCancelled())
         {
            *m_data->savePath=saveFileDialog->GetPath();
            oapc_path_split(m_data->savePath,m_data->saveDir,m_data->saveFile,wxEmptyString);
            dialogueCancelled=false;
         }
         else dialogueCancelled=true;
         delete saveFileDialog;
         if (!dialogueCancelled) m_oapc_io_callback(OAPC_CHAR_IO1,m_data->m_callbackID);
      }
      else if (evt.GetId()==OWN_EVENT_MASK+2)
      {
         libioDirDialogue  *dirDialog=NULL;

         conv.MB2WC(wc,m_data->config.dirTitle,MAX_TEXT_SIZE);
         title=wc;
         if (!m_data->dirDir) m_data->dirDir=new wxString();
         dirDialog=new libioDirDialogue(this,title,*m_data->dirDir);
         dirDialog->Centre();
         dirDialog->ShowModal();
         if (!dirDialog->wasCancelled())
         {
            *m_data->dirDir=dirDialog->GetPath();
            dialogueCancelled=false;
         }
         else dialogueCancelled=true;
         delete dirDialog;
         if (!dialogueCancelled) m_oapc_io_callback(OAPC_CHAR_IO2,m_data->m_callbackID);
      }
      m_data->m_currentDialog=0;
   }

private:
   struct instData *m_data;
};



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
          OAPC_FLOWCAT_FLOW;
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
   return OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2;
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
   return OAPC_CHAR_IO0|OAPC_CHAR_IO1|OAPC_CHAR_IO2;
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

   sprintf(xmldescr,xmltempl,flowImage,data->config.pointSize,data->config.style,data->config.weight,data->config.faceName,
                                       data->config.loadTitle,MAX_NAME_SIZE,data->config.loadWildcard,MAX_TEXT_SIZE,
                                       data->config.saveTitle,MAX_NAME_SIZE,data->config.saveWildcard,MAX_TEXT_SIZE,
                                       data->config.dirTitle,MAX_NAME_SIZE
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

   if (strcmp(name,"font_pointsize")==0) data->config.pointSize=atoi(value);
   else if (strcmp(name,"font_style")==0)    data->config.style=atoi(value);
   else if (strcmp(name,"font_weight")==0)   data->config.weight=atoi(value);
   else if (strcmp(name,"font_facename")==0) strncpy(data->config.faceName,value,MAX_NAME_SIZE);
   else if (strcmp(name,"loadtitle")==0)     strncpy(data->config.loadTitle,value,MAX_NAME_SIZE);
   else if (strcmp(name,"loadwildcard")==0)  strncpy(data->config.loadWildcard,value,MAX_NAME_SIZE);
   else if (strcmp(name,"savetitle")==0)     strncpy(data->config.saveTitle,value,MAX_NAME_SIZE);
   else if (strcmp(name,"savewildcard")==0)  strncpy(data->config.saveWildcard,value,MAX_NAME_SIZE);
   else if (strcmp(name,"dirtitle")==0)      strncpy(data->config.dirTitle,value,MAX_NAME_SIZE);
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
   save_config.version=htons(data->config.version);
   save_config.length =htons(data->config.length);
   save_config.pointSize   =htonl(data->config.pointSize);
   save_config.style       =htonl(data->config.style);
   save_config.weight      =htonl(data->config.weight);
   strncpy(save_config.faceName,data->config.faceName,MAX_NAME_SIZE);
   strncpy(save_config.loadTitle,data->config.loadTitle,MAX_NAME_SIZE);
   strncpy(save_config.loadWildcard,data->config.loadWildcard,MAX_NAME_SIZE);
   strncpy(save_config.saveTitle,data->config.saveTitle,MAX_NAME_SIZE);
   strncpy(save_config.saveWildcard,data->config.saveWildcard,MAX_NAME_SIZE);
   strncpy(save_config.dirTitle,data->config.dirTitle,MAX_NAME_SIZE);
   save_config.reserved1   =htonl(data->config.reserved1);
   save_config.reserved2   =htonl(data->config.reserved2);
   save_config.reserved3   =htonl(data->config.reserved3);
   save_config.reserved4   =htonl(data->config.reserved4);
   save_config.reserved5   =htonl(data->config.reserved5);
   save_config.reserved6   =htonl(data->config.reserved6);

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
   data->config.version=ntohs(save_config.version);
   data->config.length =ntohs(save_config.length);
   data->config.pointSize   =ntohl(save_config.pointSize);
   data->config.style       =ntohl(save_config.style);
   data->config.weight      =ntohl(save_config.weight);
   strncpy(data->config.faceName,save_config.faceName,MAX_NAME_SIZE);
   strncpy(data->config.loadTitle,save_config.loadTitle,MAX_NAME_SIZE);
   strncpy(data->config.loadWildcard,save_config.loadWildcard,MAX_NAME_SIZE);
   strncpy(data->config.saveTitle,save_config.saveTitle,MAX_NAME_SIZE);
   strncpy(data->config.saveWildcard,save_config.saveWildcard,MAX_NAME_SIZE);
   strncpy(data->config.dirTitle,save_config.dirTitle,MAX_NAME_SIZE);
   data->config.reserved1=ntohl(save_config.reserved1);
   data->config.reserved2=ntohl(save_config.reserved2);
   data->config.reserved3=ntohl(save_config.reserved3);
   data->config.reserved4=ntohl(save_config.reserved4);
   data->config.reserved5=ntohl(save_config.reserved5);
   data->config.reserved6=ntohl(save_config.reserved6);
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
   struct instData *data;

   flags=flags; // removing "unused" warning
   data=(struct instData*)malloc(sizeof(struct instData));
   memset(data,0,sizeof(struct instData));

   data->config.version=1;
   data->config.length=sizeof(struct libio_config);
   data->config.pointSize=10;
   data->config.style=wxFONTSTYLE_NORMAL;
   data->config.weight=wxFONTWEIGHT_NORMAL;
   data->config.faceName[0]=0;
   data->config.reserved1=0; data->config.reserved2=0;
   data->config.reserved3=0; data->config.reserved4=0;
   data->config.reserved5=0; data->config.reserved6=0;
   data->m_callbackID=0;
   strcpy(data->config.loadTitle,"Load File");
   strcpy(data->config.saveTitle,"Save File");
   strcpy(data->config.dirTitle,"Select Directory");
   strcpy(data->config.loadWildcard,"Text files|*.txt|All files|*");
   strcpy(data->config.saveWildcard,"Text files|*.txt|All files|*");
   return data;
}



/**
This function is called finally, it has to be used to release the instance data structure that was created
during the call of oapc_create_instance()
*/
OAPC_EXT_API void oapc_delete_instance(void* instanceData)
{
   if (instanceData)
   {
      struct instData *data;
 
      data=(struct instData*)instanceData;
      if (data->loadDir)  delete data->loadDir;
      if (data->loadFile) delete data->loadFile;
      if (data->loadPath) delete data->loadPath;
      if (data->saveDir)  delete data->saveDir;
      if (data->saveFile) delete data->saveFile;
      if (data->savePath) delete data->savePath;
      if (data->dirDir)   delete data->dirDir;
      free(instanceData);
   }
}


/**
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API unsigned long oapc_init(void *instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   data->m_eventHandler=new EventHandler(data);
   if (!data->m_eventHandler) return OAPC_ERROR_NO_MEMORY;
   return OAPC_OK;
}



/**
This function is called before the application unloads everything, it has to be used to deinitialize
everything and to release used resources.
*/
OAPC_EXT_API unsigned long oapc_exit(void* instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (data->m_eventHandler) delete data->m_eventHandler;
   data->m_eventHandler=NULL;

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



OAPC_EXT_API unsigned long oapc_set_digi_value(void* instanceData,unsigned long input,unsigned char value)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   if (data->m_currentDialog!=0) return OAPC_ERROR_STILL_IN_PROGRESS;
   data->m_eventHandler->triggerEvent(OWN_EVENT_MASK+input);
   return OAPC_OK;
}



OAPC_EXT_API unsigned long  oapc_get_char_value(void* instanceData,unsigned long output,unsigned long length,char *value)
{
   struct instData *data;
   wxString         returnValue;
   wxMBConvUTF8     conv;

   data=(struct instData*)instanceData;

   if (output==0) returnValue=*data->loadPath;
   else if (output==1) returnValue=*data->savePath;
   else if (output==2) returnValue=*data->dirDir;
   else return OAPC_ERROR_NO_SUCH_IO;

   conv.WC2MB(value,returnValue,length);
   return OAPC_OK;
}

