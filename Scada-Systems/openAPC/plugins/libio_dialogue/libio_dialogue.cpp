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
#include <wx/dialog.h>

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

#define MAX_XML_SIZE        5000
#define MAX_BUTTONNAME_SIZE   31
#define MAX_NAME_SIZE         31
#define MAX_TEXT_SIZE        500

struct libio_config
{
   unsigned short version,length;
   unsigned int   reserved1;
   char           button1[MAX_BUTTONNAME_SIZE+1],button2[MAX_BUTTONNAME_SIZE+1],
                  button3[MAX_BUTTONNAME_SIZE+1],title[MAX_BUTTONNAME_SIZE+1];
   wxInt32        pointSize,style,weight,reserved2;
   char           faceName[MAX_NAME_SIZE+1];
   unsigned int   reserved3,reserved4,reserved5,reserved6;
};


class EventHandler;

struct instData
{
   struct libio_config  config;
   int                  m_callbackID,m_lastResult;
   char                 msgText[MAX_TEXT_SIZE+1];
   char                 msgNum;
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
 </general>\n\
 <helppanel>\n\
  <in0>Text for error message</in0>\n\
  <in1>Text for warning message</in1>\n\
  <in2>Text for information message</in2>\n\
  <in3>Text for yes/no/cancel question</in3>\n\
  <out0>HIGH out - message confirmed by user</out0>\n\
  <out1>HIGH out - message confirmed by user</out1>\n\
  <out2>HIGH out - message confirmed by user</out2>\n\
  <out3>1 - yes pressed, 2 - no pressed, 3 - cancel pressed</out3>\n\
 </helppanel>\n\
</dialogue>\n\
</oapc-config>\n";

/*  <stdpanel>\n\
   <name>IN4 Question</name>\n\
   <param>\n\
    <name>button1</name>\n\
    <text>Button 1 Text</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>1</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>button2</name>\n\
    <text>Button 2 Text</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>1</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>button3</name>\n\
    <text>Button 3 Text</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
  </stdpanel>\n\*/

/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUEAgSojRlwi60EAoQE/vwPVQ+byZrEwsTwzwyEgoS8VFKfBwXf6veWqMaoKyz76wuEekisyuzSBwf23gaMsoxUj1RcTki4usj37Yz3VFb8/vwkTqzp29z2cHH0OjnnGhkEgvz8AvxMPkzUukx8fkxfb5HnjY90AgQ0cjTMpKR8qHx0YkQMLpS0fnzs7jSeutwcEjQsRmyHhanefHrstrScorScHhysogz8/hzH2/L8/oyg0qDEy9z04iSMrtypXlwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA9lDtDAAAB4UlEQVR42u2YC1OCQBDHERFRTknwkSe+KjUtI/MVQfn9v1V3LCemwSAXNpZ7yt1/58bf7i0wzgrCCQ1jLBT6/X6BruKLow1QeYH8hpAHV0yREEXjDaKNKRKiSLxBtDFFQhSJdyfamCIhKr8bbUyREIW/RBtTJESlb4Ay5+12s42Y8+3J3W5AyCXDW06qYBMOlNkkpGanzFja6x1iIOy2ELZLVFRl2XEcWa5yoNqdcUcvzrtT8F0ba+MFljZGVgkhJALKuc/l+FDd7tCco6ZeBt/7+lnz07IRqjySC8uq5uQmXChd1xd4rusqJKUpdU2BtBoeyrZFdoC5Gl9Wql5EC7OsrjyXohkZQ7vy0mpY9kfLskszhqpx1mqjukV1OFanUClDWZMvpCWKVsUSZ0sfJZNS7aF6UXZ4B65UakVIipBuFMV4gA3LgTgY2Dj8DuxFJJL95rnajEajDXhuM2B1tmU5a+CI5+pIFI/16NuKDAHmlFGUw0baqNMdYJDSXmap1Ip+Dst13gd4QfGgIuyvHKD/QvendFGQjD+li8pSY9MZ1yobYb/67/aCuqD+DSpolsQXSVEFcgl6LuFCkiTubgxdbnsu4YKg+LsxeLfnEi6kH+jG4N2eS7iQLt2YA9TJ7BPpgeKoo0RZyQAAAABJRU5ErkJggg==";
static char                 libname[]="Dialogue";
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

private:
   wxDialog *createDialogue(wxWindow *parent,const wxString title,const wxString message,const wxFont font,const wxArtID id)
   { 
      wxDialog *dlg=new wxDialog(parent,wxID_ANY,title,wxDefaultPosition,wxDefaultSize,wxSTAY_ON_TOP|wxCAPTION);
      dlg->SetFont(font);

      wxBoxSizer *bSizer=new wxBoxSizer(wxVERTICAL);
      dlg->SetSizer(bSizer);
    
      wxPanel *rootPanel=new wxPanel(dlg);
      bSizer->Add(rootPanel,1,wxEXPAND);
    
      wxFlexGridSizer *fSizer=new wxFlexGridSizer(2,20,20);
      fSizer->AddGrowableCol(0,2);
      fSizer->AddGrowableCol(1,9);
      rootPanel->SetSizer(fSizer);

      wxStaticBitmap *bm=new wxStaticBitmap(rootPanel,wxID_ANY,wxArtProvider::GetBitmap(id));
      fSizer->Add(bm,1,wxALIGN_CENTER_VERTICAL);
 
      wxStaticText *text=new wxStaticText(rootPanel,wxID_ANY,_T("\n")+message+_T("\n"));
      fSizer->Add(text,1,wxEXPAND);
    
      wxSizer *sizerBtns =dlg->CreateButtonSizer(wxOK);
      if (sizerBtns) bSizer->Add(sizerBtns, wxSizerFlags().Expand().Border());
 
      dlg->SetSize(bSizer->Fit(dlg));
      dlg->Centre();
    
      return dlg;
   }


   void OnCommandEvent(wxCommandEvent &evt)
   {
      wxMBConvUTF8     conv;
      wchar_t          wc[MAX_TEXT_SIZE];
      wxString         msgStr,b1,b2,b3,t;

      m_data->msgNum=evt.GetId();
      if ((m_data->config.pointSize>3) && (m_data->config.pointSize<1000))
      {
         conv.MB2WC(wc,m_data->config.faceName, MAX_TEXT_SIZE);      
         SetFont(wxFont(m_data->config.pointSize, wxFONTFAMILY_DEFAULT,
                 m_data->config.style, m_data->config.weight,
                 false, wc));
      }
      conv.MB2WC(wc,m_data->msgText,MAX_TEXT_SIZE);
      msgStr=wc;
      msgStr.Replace(_T("\\r\\n"),_T("\n"));
      msgStr.Replace(_T("\\r"),_T("\n"));
      msgStr.Replace(_T("\\n"),_T("\n"));

      if (evt.GetId()==OWN_EVENT_MASK+0)
      {
         wxDialog *dialog;

         dialog=createDialogue(this,_T("Error"),msgStr,
                               wxFont(m_data->config.pointSize,wxFONTFAMILY_DEFAULT,m_data->config.style,m_data->config.weight,false,wc),
                               wxART_ERROR);
         dialog->ShowModal();
         m_oapc_io_callback(OAPC_DIGI_IO0,m_data->m_callbackID);
         delete dialog;
      }
      else if (evt.GetId()==OWN_EVENT_MASK+1)
      {
         wxDialog *dialog;

         dialog=createDialogue(this,_T("Warning"),msgStr,
                               wxFont(m_data->config.pointSize,wxFONTFAMILY_DEFAULT,m_data->config.style,m_data->config.weight,false,wc),
                               wxART_WARNING);
         dialog->ShowModal();
         m_oapc_io_callback(OAPC_DIGI_IO1,m_data->m_callbackID);
         delete dialog;
      }
      else if (evt.GetId()==OWN_EVENT_MASK+2)
      {
         wxDialog *dialog;

         dialog=createDialogue(this,_T("Information"),msgStr,
                               wxFont(m_data->config.pointSize,wxFONTFAMILY_DEFAULT,m_data->config.style,m_data->config.weight,false,wc),
                               wxART_INFORMATION);
         dialog->ShowModal();
         m_oapc_io_callback(OAPC_DIGI_IO2,m_data->m_callbackID);
         delete dialog;
      }
      else if (evt.GetId()==OWN_EVENT_MASK+3)
      {
         int ret;

         ret=wxMessageBox(msgStr,_T("Question"),wxICON_QUESTION|wxYES_NO|wxCANCEL,this);
         if (ret==wxYES) m_data->m_lastResult=1;
         else if (ret==wxNO) m_data->m_lastResult=2;
         else if (ret==wxCANCEL) m_data->m_lastResult=3;
         else m_data->m_lastResult=0;
         m_oapc_io_callback(OAPC_NUM_IO3,m_data->m_callbackID);
      }
      m_data->msgNum=0;
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
   return OAPC_CHAR_IO0|OAPC_CHAR_IO1|OAPC_CHAR_IO2|OAPC_CHAR_IO3;
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
   return OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_NUM_IO3;
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

   sprintf(xmldescr,xmltempl,flowImage,data->config.pointSize,data->config.style,data->config.weight,data->config.faceName/*,
                                       data->config.button1,MAX_BUTTONNAME_SIZE,
                                       data->config.button2,MAX_BUTTONNAME_SIZE,
                                       data->config.button3,MAX_BUTTONNAME_SIZE*/
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
OAPC_EXT_API void oapc_set_config_data(void* instanceData,char *name,char *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (strcmp(name,"button1")==0)             strncpy(data->config.button1,value,MAX_BUTTONNAME_SIZE);
   else if (strcmp(name,"button2")==0)        strncpy(data->config.button2,value,MAX_BUTTONNAME_SIZE);
   else if (strcmp(name,"button3")==0)        strncpy(data->config.button3,value,MAX_BUTTONNAME_SIZE);
   else if (strcmp(name,"title")==0)          strncpy(data->config.title,value,MAX_BUTTONNAME_SIZE);
   else if (strcmp(name,"font_pointsize")==0) data->config.pointSize=atoi(value);
   else if (strcmp(name,"font_style")==0)     data->config.style=atoi(value);
   else if (strcmp(name,"font_weight")==0)    data->config.weight=atoi(value);
   else if (strcmp(name,"font_facename")==0)  strncpy(data->config.faceName,value,MAX_NAME_SIZE);
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
   strncpy(save_config.button1,data->config.button1,MAX_BUTTONNAME_SIZE);
   strncpy(save_config.button2,data->config.button2,MAX_BUTTONNAME_SIZE);
   strncpy(save_config.button3,data->config.button3,MAX_BUTTONNAME_SIZE);
   strncpy(save_config.title,  data->config.title,MAX_BUTTONNAME_SIZE);
   save_config.pointSize   =htonl(data->config.pointSize);
   save_config.style       =htonl(data->config.style);
   save_config.weight      =htonl(data->config.weight);
   strncpy(save_config.faceName,data->config.faceName,MAX_NAME_SIZE);
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
   strncpy(data->config.button1,save_config.button1,MAX_BUTTONNAME_SIZE);
   strncpy(data->config.button2,save_config.button2,MAX_BUTTONNAME_SIZE);
   strncpy(data->config.button3,save_config.button3,MAX_BUTTONNAME_SIZE);
   strncpy(data->config.title,  save_config.title,  MAX_BUTTONNAME_SIZE);
   data->config.pointSize   =ntohl(save_config.pointSize);
   data->config.style       =ntohl(save_config.style);
   data->config.weight      =ntohl(save_config.weight);
   strncpy(data->config.faceName,save_config.faceName,MAX_NAME_SIZE);
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
   strcpy(data->config.button1,"Yes");
   strcpy(data->config.button2,"No");
   data->config.button3[0]=0;
   strcpy(data->config.title,"Question");
   data->config.pointSize=10;
   data->config.style=wxFONTSTYLE_NORMAL;
   data->config.weight=wxFONTWEIGHT_NORMAL;
   data->config.faceName[0]=0;
   data->config.reserved1=0; data->config.reserved2=0;
   data->config.reserved3=0; data->config.reserved4=0;
   data->config.reserved5=0; data->config.reserved6=0;
   data->m_callbackID=0;
   data->msgNum=0;

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



OAPC_EXT_API unsigned long  oapc_get_digi_value(void*,unsigned long output,unsigned char *value)
{
   if (output>=3) return OAPC_ERROR_NO_SUCH_IO;
   *value=1;
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

   data=(struct instData*)instanceData;

   if (output!=3) return OAPC_ERROR_NO_SUCH_IO;
   *value=(float)data->m_lastResult;
   return OAPC_OK;
}



/**
This function is called by the main application when the library provides an character input (marked
using the digital input flags OAPC_CHAR_IO...), a connection was edited to this input and a data
flow reaches the input.
@param[in] input specifies the input where the data are send to, here not the OAPC_CHAR_IO...-flag is used
           but a plain, 0-based input number
@param[in] value specifies the string that is set to that input
@return an error code OAPC_ERROR_... in case of an error or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_set_char_value(void* instanceData,unsigned long input,char *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (input<4) // filename
   {
      if (data->msgNum==0)
      {
         strncpy(data->msgText,value,MAX_TEXT_SIZE);
         data->m_eventHandler->triggerEvent(input+OWN_EVENT_MASK);
         return OAPC_OK;
      }
      else return OAPC_ERROR_STILL_IN_PROGRESS;
   }
   else return OAPC_ERROR_NO_SUCH_IO;
}



