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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <wx/wx.h>
#include <wx/dcbuffer.h>

#if defined (ENV_LINUX) || (ENV_QNX)
#include <sys/io.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#else
#include <wx/msw/private.h>

#ifdef ENV_WINDOWSCE
#include <winsock2.h>
#endif
#endif

#ifdef ENV_LINUX
#include <termios.h>
#include <unistd.h>
#endif

#include "oapc_libio.h"
#include "liboapc.h"



#define MAX_XML_SIZE   2500
#define MAX_TEXT_SIZE   200

#pragma pack(8)


class ListBox;

struct instData
{
   wxUint8               m_digi[MAX_NUM_IOS]; // we use an array for all possible digital IOs
   wxInt32               m_num[MAX_NUM_IOS];
   char                  m_text[MAX_NUM_IOS][MAX_TEXT_SIZE+1];
   ListBox              *m_box;
   wxInt32               m_callbackID;
};



class ListBox : public wxListBox
{
public:
   ListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n=0, const wxString choices[] = NULL, long style = 0);//:wxListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int n, const wxString choices[], long style, const wxValidator& validator, const wxString& name);

   void OnClick(wxCommandEvent& event);
   void OnDblClick(wxCommandEvent& event);

   struct instData *data;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]={"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
 <flowimage>%s</flowimage>\n\
</oapc-config>\n"};
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyBAMAAABG0lM+AAAAMFBMVEUEAgSEgoQE/vzEwsQEAoT8AvwEgvzk5vz8/vwAAAAAAAAAAAAAAAAAAAAAAAAAAAAVBugtAAAB60lEQVR42p2WS2rDMBCG5ZoiLUOh0GXiE3jnC5imy2ThG5Q2ywEvtFWhFB27Gj0s2dHLmZDAJPn4Z0by8BPyUFDeyB/KObx0B85tkg54nQ4cFNVLMXA+P3Wt+tYk6Zifp1ZT8IYivDmhiE3S0VyppmaGIrw/oohN0tFfBk3xRncCRqSheQr/pqledzIbkX7IU/g3TYHJjQjwQtA1FcTXNr7TlBajRH3cZBh/Up7zFOjkJpggkghRpPCa4BdaiylCCMYqtHzUV0iDId6IZBgVFA0pwYh6y3197Zhh0BdRUpLV9AWmPl0hTlBstfpkX5aKVdg3EQrMDHV3aobqxQhjZKEsNGf6YkzgQUsmHAVuZQy1p/z74bV0WQnKTMOemKECLFmh0F2tKDtDvqvCcAukTllp7af81XXUgzN024bmZohqREQqzJ4y3g37qFRT9xUCactauG7wpe7jcntL1OcYxvs4nnm4bBNUfMOXzmu7pOk6KVAAgYpPSlo01KL7tLZJVV92N/gkT5nl6PaQTwpathNC18me81o/X8Ts6kiYX/DJWieweJuot5iu1NoZlXQnZ4G8t4l6i+kyWDujku7oLFDgbaKm6eDsDKosFijwNlHT1Do7gyqLBQq8TbSxe9ez8TbRxu5dj/E2D8U/Vz4Kk8oH5LIAAAAASUVORK5CYII=";
static char                 libname[]="Text Listbox";
static char                 xmldescr[MAX_XML_SIZE+1];

static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports



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
          OAPC_HAS_LOG_TYPE_INTNUM|           // to let the main application log the change of data
          OAPC_ACCEPTS_IO_CALLBACK|
          OAPC_HMICAT_CONTROL|
          OAPC_USERPRIVI_DISABLE|OAPC_USERPRIVI_HIDE;
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
   return OAPC_HMI_NO_UI_RO|OAPC_HMI_NO_UI_TEXT;
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
   return  OAPC_DIGI_IO0|OAPC_NUM_IO1|OAPC_DIGI_IO2|OAPC_CHAR_IO7;

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
   return OAPC_DIGI_IO0|OAPC_CHAR_IO4|OAPC_CHAR_IO5|OAPC_NUM_IO6|OAPC_NUM_IO7;
   // avoid it to submit the same IO number for different data types, that would cause displaying problems for the flow symbol
}



/**
When the OAPC_HAS_XML_CONFIGURATION capability flag was set this function is called to retrieve an XML
structure that describes which configuration information have to be displayed within the main application.
When there are no extended flow configuration possibilities but the flag was set in order to provide
extended HMI possibilities (please see function below) this function has to return NULL.
@return an char-array that contains the XML data; this char array has to be released only when the 
        library is unloaded!
*/
OAPC_EXT_API char *oapc_get_config_data(void* /*instanceData*/)
{
/*   struct instData *data;

   data=(struct instData*)instanceData;*/

   sprintf(xmldescr,xmltempl,flowImage);
   assert(strlen(xmldescr)<MAX_XML_SIZE);
   return xmldescr;
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
OAPC_EXT_API unsigned long oapc_init(void* /*instanceData*/)
{
/*   struct instData *data;

   data=(struct instData*)instanceData;*/

//   data->num[7]=0;
   return OAPC_OK;
}



/**
This function is called before the application unloads everything, it has to be used to deinitialize
everything and to release used resources.
*/
OAPC_EXT_API unsigned long oapc_exit(void* WXUNUSED(instanceData))
{
   return OAPC_OK;
}



ListBox::ListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int n, const wxString choices[], long style)
        :wxListBox(parent,id,pos,size,n,choices,style,wxDefaultValidator,_T(""))
{
}



void ListBox::OnClick(wxCommandEvent& WXUNUSED(event))
{
   wxMBConvUTF8  conv;

   conv.WC2MB(data->m_text[4],data->m_box->GetStringSelection(),MAX_TEXT_SIZE);
   data->m_num[6]=data->m_box->GetSelection();
   m_oapc_io_callback(OAPC_CHAR_IO4,data->m_callbackID);
   m_oapc_io_callback(OAPC_NUM_IO6,data->m_callbackID);
}



void ListBox::OnDblClick(wxCommandEvent& WXUNUSED(event))
{
   wxMBConvUTF8  conv;

   conv.WC2MB(data->m_text[5],data->m_box->GetStringSelection(),MAX_TEXT_SIZE);
   this->data->m_num[7]=data->m_box->GetSelection();
   m_oapc_io_callback(OAPC_NUM_IO5,data->m_callbackID);
   m_oapc_io_callback(OAPC_NUM_IO7,data->m_callbackID);
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
OAPC_EXT_API void oapc_paint(void* instanceData,wxAutoBufferedPaintDC *WXUNUSED(dc),wxPanel *canvas)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (!data->m_box)
   {
      wxGridSizer *gSizer=new wxGridSizer(1,1,0,0);
      canvas->SetSizer(gSizer);

      data->m_box=new ListBox(canvas,wxID_ANY,wxDefaultPosition,canvas->GetSize(),0,NULL,wxLB_SINGLE|wxLB_NEEDED_SB);
      data->m_box->data=data;

      gSizer->Add(data->m_box,1,wxEXPAND);

      data->m_box->Connect(data->m_box->GetId(),wxEVT_COMMAND_LISTBOX_SELECTED,wxCommandEventHandler(ListBox::OnClick));
      data->m_box->Connect(data->m_box->GetId(),wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,wxCommandEventHandler(ListBox::OnDblClick));
   }
   data->m_box->SetBackgroundColour(canvas->GetBackgroundColour());
   data->m_box->SetForegroundColour(canvas->GetForegroundColour());
   data->m_box->SetFont(canvas->GetFont());
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
Here the default size of the element has to be returned; this size is used when it is created
newly
@param[out] x the default size in x-direction (width)
@param[out] y the default size in y-direction (height)
*/
OAPC_EXT_API void oapc_get_defsize(wxFloat32 *x,wxFloat32 *y)
{
   *x=160;
   *y=140;
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
   *x=60;
   *y=60;
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
   *x=6000;
   *y=6000;
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
   *foreground=0x000011;
   *background=0xFFFFFF;
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

   if ((input!=0) && (input!=2)) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   if (input==0) data->m_digi[input]=value;
   else if (data->m_box) data->m_box->Clear();
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

   data=(struct instData*)instanceData;

   if (output!=0) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   *value=data->m_digi[output];
   return OAPC_OK; // there are no new data available
}



OAPC_EXT_API unsigned long  oapc_set_num_value(void* instanceData,unsigned long input,double value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (input!=1) return OAPC_ERROR_NO_SUCH_IO;
   if (data->m_box->GetCount()>(unsigned int)value)
    data->m_box->SetSelection((int)value);
   return 1;
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
OAPC_EXT_API unsigned long  oapc_get_num_value(void* instanceData,unsigned long output,double *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if ((output!=6) && (output!=7)) return OAPC_ERROR_NO_SUCH_IO;
   *value=data->m_num[output];
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

   if (input!=7) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   if (data->m_box)
   {
      wxMBConvUTF8          conv;
      wchar_t               wc[300];

      conv.MB2WC(wc,value,300);

      data->m_box->Append(wc);
   }

   return OAPC_OK;
}



/**
This function is called by the main application periodically in order to poll the state of the related
output. It has to return the data that are available for that input or - in case there are none available -
the error code OAPC_ERROR_NO_DATA_AVAILABLE to notify the main application, that there is nothing new.
@param[in] output specifies the output where the data are fetched from, here not the OAPC_CHAR_IO...-flag is used
           but the plain, 0-based output number
@param[out] value if there are new data available, they are stored within that variable, if there are no new
           data, the returned data are ignored by the main application
@return an error code OAPC_ERROR_... in case of an error, OAPC_ERROR_NO_DATA_AVAILABLE in case no new data are
           available or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_get_char_value(void *instanceData,unsigned long output,unsigned long length,char* value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if ((output!=4) && (output!=5)) return OAPC_ERROR_NO_SUCH_IO;
   if (length>MAX_TEXT_SIZE) length=MAX_TEXT_SIZE;
   strncpy(value,data->m_text[output],length);
   return OAPC_OK;
}

