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
#include <wx/gbsizer.h>

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

#pragma pack(8)


class PinPad;

struct instData
{
   wxByte                m_digi;
   PinPad               *m_pinpad;
   wxInt32               m_callbackID;
};



class PinPad : public wxPanel
{
public:
   void OnButtonClicked(wxCommandEvent& event);

   struct instData *data;
   
   wxTextCtrl      *m_inputField;
   wxButton        *m_numButton[10],*m_okButton,*m_clrButton;
   wxInt32          m_callbackID;
};

static char                 libname[]="Pinpad";

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
          OAPC_HAS_STANDARD_FLOW_CONFIGURATION|
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
   return OAPC_HMI_NO_UI_RO|OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_FG;
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
   return  OAPC_DIGI_IO0|OAPC_NUM_IO7;

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
   return OAPC_DIGI_IO0|OAPC_NUM_IO6|OAPC_NUM_IO7;
   // avoid it to submit the same IO number for different data types, that would cause displaying problems for the flow symbol
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
OAPC_EXT_API unsigned long oapc_init(void* instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   data->m_pinpad=NULL;
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



void PinPad::OnButtonClicked(wxCommandEvent& event)
{
   wxInt32 i;

   if (event.GetId()==m_clrButton->GetId())
   {
      m_inputField->SetValue(wxEmptyString);
   }
   else if (event.GetId()==m_okButton->GetId())
   {
      if (m_oapc_io_callback)
      {
         m_oapc_io_callback(OAPC_NUM_IO6,m_callbackID);
         m_oapc_io_callback(OAPC_NUM_IO7,m_callbackID);
      }
   }
   else for (i=0; i<10; i++)
   {
      if (event.GetId()==m_numButton[i]->GetId())
      {
         *m_inputField<<m_numButton[i]->GetLabel();
         break;
      }
   }
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
   struct instData *data;

   data=(struct instData*)instanceData;

   if (!data->m_pinpad)
   {
      wxInt32  i;
      wxString num;
      
      wxGridBagSizer *gbSizer=new wxGridBagSizer(4,4);
      canvas->SetSizer(gbSizer);
      data->m_pinpad=new PinPad();
      data->m_pinpad->m_callbackID=data->m_callbackID;
      
      data->m_pinpad->m_inputField=new wxTextCtrl(canvas,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_READONLY|wxTE_RIGHT);
      gbSizer->Add(data->m_pinpad->m_inputField,wxGBPosition(0,0),wxGBSpan(1,4),wxEXPAND);
      
      for (i=1; i<=3; i++)
      {
         num=wxString::Format(_T("%d"),i);
         data->m_pinpad->m_numButton[i]=new wxButton(canvas,wxID_ANY,num,wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
         gbSizer->Add(data->m_pinpad->m_numButton[i],wxGBPosition(1,i-1),wxGBSpan(1,1),wxEXPAND);
         data->m_pinpad->m_numButton[i]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);

         num=wxString::Format(_T("%d"),i+3);
         data->m_pinpad->m_numButton[i+3]=new wxButton(canvas,wxID_ANY,num,wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
         gbSizer->Add(data->m_pinpad->m_numButton[i+3],wxGBPosition(2,i-1),wxGBSpan(1,1),wxEXPAND);
         data->m_pinpad->Connect(data->m_pinpad->m_numButton[i+3]->GetId(),wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked));
         data->m_pinpad->m_numButton[i+3]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);

         num=wxString::Format(_T("%d"),i+6);
         data->m_pinpad->m_numButton[i+6]=new wxButton(canvas,wxID_ANY,num,wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
         gbSizer->Add(data->m_pinpad->m_numButton[i+6],wxGBPosition(3,i-1),wxGBSpan(1,1),wxEXPAND);
         data->m_pinpad->Connect(data->m_pinpad->m_numButton[i+6]->GetId(),wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked));
         data->m_pinpad->m_numButton[i+6]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);
      }
      data->m_pinpad->m_numButton[0]=new wxButton(canvas,wxID_ANY,_T("0"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
      gbSizer->Add(data->m_pinpad->m_numButton[0],wxGBPosition(4,1),wxGBSpan(1,1),wxEXPAND);
      data->m_pinpad->m_numButton[0]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);

      data->m_pinpad->m_clrButton=new wxButton(canvas,wxID_ANY,_T("CLR"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
      data->m_pinpad->m_clrButton->SetBackgroundColour(wxColour(0x00FFFF));
      gbSizer->Add(data->m_pinpad->m_clrButton,wxGBPosition(1,3),wxGBSpan(2,1),wxEXPAND);
      data->m_pinpad->m_clrButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);

      data->m_pinpad->m_okButton=new wxButton(canvas,wxID_ANY,_T("OK"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
      data->m_pinpad->m_okButton->SetBackgroundColour(*wxGREEN);
      gbSizer->Add(data->m_pinpad->m_okButton,wxGBPosition(3,3),wxGBSpan(2,1),wxEXPAND);
      data->m_pinpad->m_okButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);

      for (i=1; i<=4; i++) gbSizer->AddGrowableRow(i,1);
      for (i=0; i<=4; i++) gbSizer->AddGrowableCol(i,1);

      canvas->Layout();
   }
   dc->SetBackground(canvas->GetBackgroundColour());
   dc->Clear();
//   data->m_box->SetFont(canvas->GetFont());
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
   *x=180;
   *y=200;
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
   *x=40;
   *y=50;
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
   *x=4000;
   *y=5000;
}



/**
This function is relevant only to HMI controls that provide or handle numeric values, it specifies
the allowed range the numeric value can handle.
@param[out] minValue the minimum allowed value that can be handled by this control, this value can't
            be smaller than -2100000000
@param[out] maxValue the maximum allowed value that can be handled by this control, this value can't
            be bigger than 2100000000
*/
OAPC_EXT_API void oapc_get_numminmax(void* WXUNUSED(instanceData),wxFloat32 *minValue,wxFloat32 *maxValue)
{
   *minValue=0;
   *maxValue=10000;
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

   if (input!=0) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   data->m_digi=value;
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
   *value=data->m_digi;
   return OAPC_OK; // there are no new data available
}



OAPC_EXT_API unsigned long  oapc_set_num_value(void* instanceData,unsigned long input,double value)
{
   struct instData *data;

   data=(struct instData*)instanceData;
printf("set_num\n");
   if (input!=7) return OAPC_ERROR_NO_SUCH_IO;
   data->m_pinpad->m_inputField->SetValue(wxEmptyString);
   *data->m_pinpad->m_inputField<<value;
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
   data->m_pinpad->m_inputField->GetValue().ToDouble(value);
   return OAPC_OK;
}


