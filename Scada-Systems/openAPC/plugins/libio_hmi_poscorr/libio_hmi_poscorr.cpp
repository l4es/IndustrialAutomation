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
#include "oapcFloatCtrl.h"
#include "libio_hmi_poscorr.h"

#pragma pack(8)


class PinPad;

#define MAX_XML_SIZE   4000

struct instData
{
   wxByte                          m_digi;
   PinPad                         *m_pinpad;
   wxInt32                         m_callbackID;
   struct oapc_bin_struct_pos_corr posCorr;
   struct oapc_bin_head           *m_bin;
};



class PinPad : public wxPanel
{
public:
   void OnButtonClicked(wxCommandEvent& event);

   struct instData *data;
   
   oapcFloatCtrl  *m_distField,*m_rotField;
   wxBitmapButton *m_button[10];
   wxButton       *m_resetButton;
   wxInt32         m_callbackID;
   wxComboBox     *m_sizeCombo;
};

static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
<flowimage>%s</flowimage>\n\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyBAMAAABG0lM+AAAAMFBMVEUEAgT88gSEgoTEwsTk5vwEAoQEgvz8/vzgAMo2ADQhAJ8AAHdQAKjGAD4gHQAAAAAXNps4AAAACXBIWXMAABJ0AAASdAHeZh94AAAA/0lEQVR42t2WQQqDMBBFp2bAbe0J7KrbgB5gwFyhzQ26li7M9auGliBJzAzFRWcxC/Fh/ucbPoBo0FTuhYY1NFPajT2foltr+NRQKwFlKpRQupdQZP6KIkIBhZj71n2eR+T5ksE0ZZ1zE1uXdV2UyuuyDqJUXpcdoZ4EukCkK04RSCg0Eufz1HMZ9gl/m97UCX0Cw11CeR/CXaLr8z5EqZQu6+qAGjcUUoLSAF8KQG0oxBy1ppdBZU9IOx52TDckzvsEhrs8vdxE7SX3wBtb9qfgcVT+PtxzY60q5QDASrG6DV2blt9thlOjBN2muqCg2+hzL+g21Eq6zaC8kYJ5A+D9l5cOft9zAAAAAElFTkSuQmCC";
static char                 libname[]="Position Correction";
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
   return  OAPC_DIGI_IO0|OAPC_BIN_IO7;

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
   return OAPC_DIGI_IO0|OAPC_BIN_IO7;
   // avoid it to submit the same IO number for different data types, that would cause displaying problems for the flow symbol
}

/**
When the OAPC_HAS_XML_CONFIGURATION capability flag was set this function is called to retrieve an XML
structure that describes which configuration information have to be displayed within the main application.
@param[out] data pointer to the beginning of an char-array that contains the XML data
@return the length of the XML structure or 0 in case of an error
*/
OAPC_EXT_API char *oapc_get_config_data(void* /*instanceData*/)
{
/*   struct instData *data;

   data=(struct instData*)instanceData;*/

   snprintf(xmldescr,MAX_XML_SIZE,xmltempl,flowImage);
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
   if (!data) return;
   if (event.GetId()==m_button[0]->GetId())
   {
      data->posCorr.uPosX-=m_distField->GetValue(0.001,100.0)*1000.0;
      data->posCorr.uPosY-=m_distField->GetValue(0.001,100.0)*1000.0;
      m_oapc_io_callback(OAPC_BIN_IO7,m_callbackID);
   }
   else if (event.GetId()==m_button[1]->GetId())
   {
      data->posCorr.uPosY-=m_distField->GetValue(0.001,100.0)*1000.0;
      m_oapc_io_callback(OAPC_BIN_IO7,m_callbackID);
   }
   else if (event.GetId()==m_button[2]->GetId())
   {
      data->posCorr.uPosX+=m_distField->GetValue(0.001,100.0)*1000.0;
      data->posCorr.uPosY-=m_distField->GetValue(0.001,100.0)*1000.0;
      m_oapc_io_callback(OAPC_BIN_IO7,m_callbackID);
   }
   else if (event.GetId()==m_button[3]->GetId())
   {
      data->posCorr.uPosX-=m_distField->GetValue(0.001,100.0)*1000.0;
      m_oapc_io_callback(OAPC_BIN_IO7,m_callbackID);
   }
   else if (event.GetId()==m_button[4]->GetId())
   {
      data->posCorr.mZAngle-=m_rotField->GetValue(0.001,100.0)*1000.0;
      m_oapc_io_callback(OAPC_BIN_IO7,m_callbackID);
   }
   else if (event.GetId()==m_button[5]->GetId())
   {
      data->posCorr.mZAngle+=m_rotField->GetValue(0.001,100.0)*1000.0;
      m_oapc_io_callback(OAPC_BIN_IO7,m_callbackID);
   }
   else if (event.GetId()==m_button[6]->GetId())
   {
      data->posCorr.uPosX+=m_distField->GetValue(0.001,100.0)*1000.0;
      m_oapc_io_callback(OAPC_BIN_IO7,m_callbackID);
   }
   else if (event.GetId()==m_button[7]->GetId())
   {
      data->posCorr.uPosX-=m_distField->GetValue(0.001,100.0)*1000.0;
      data->posCorr.uPosY+=m_distField->GetValue(0.001,100.0)*1000.0;
      m_oapc_io_callback(OAPC_BIN_IO7,m_callbackID);
   }
   else if (event.GetId()==m_button[8]->GetId())
   {
      data->posCorr.uPosY+=m_distField->GetValue(0.001,100.0)*1000.0;
      m_oapc_io_callback(OAPC_BIN_IO7,m_callbackID);
   }
   else if (event.GetId()==m_button[9]->GetId())
   {
      data->posCorr.uPosX+=m_distField->GetValue(0.001,100.0)*1000.0;
      data->posCorr.uPosY+=m_distField->GetValue(0.001,100.0)*1000.0;
      m_oapc_io_callback(OAPC_BIN_IO7,m_callbackID);
   }
   else if (event.GetId()==m_resetButton->GetId())
   {
      memset(&data->posCorr,0,sizeof(struct oapc_bin_struct_pos_corr));
      m_oapc_io_callback(OAPC_BIN_IO7,m_callbackID);
   }
   else if (event.GetId()==m_sizeCombo->GetId())
   {
      double d;

      switch (m_sizeCombo->GetSelection())
      {
         case 0:
            d=0.001;
            break;
         case 1:
            d=0.005;
            break;
         case 2:
            d=0.01;
            break;
         case 3:
            d=0.05;
            break;
         case 4:
            d=0.1;
            break;
         case 5:
            d=0.5;
            break;
         case 6:
            d=1.0;
            break;
         case 7:
            d=5.0;
            break;
         case 8:
         default:
            d=1.0;
            break;
      }
      m_rotField->SetValue(d);
      m_distField->SetValue(d);
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
      data->m_pinpad->data=data;
      data->m_pinpad->m_callbackID=data->m_callbackID;
      
      data->m_pinpad->m_button[0]=new wxBitmapButton(canvas,wxID_ANY,wxBitmap(direction_1_xpm));
      gbSizer->Add(data->m_pinpad->m_button[0],wxGBPosition(0,0),wxGBSpan(1,1),wxEXPAND);
      data->m_pinpad->m_button[0]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);

      data->m_pinpad->m_button[1]=new wxBitmapButton(canvas,wxID_ANY,wxBitmap(direction_U_xpm));
      gbSizer->Add(data->m_pinpad->m_button[1],wxGBPosition(0,1),wxGBSpan(1,2),wxEXPAND);
      data->m_pinpad->m_button[1]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);

      data->m_pinpad->m_button[2]=new wxBitmapButton(canvas,wxID_ANY,wxBitmap(direction_2_xpm));
      gbSizer->Add(data->m_pinpad->m_button[2],wxGBPosition(0,3),wxGBSpan(1,1),wxEXPAND);
      data->m_pinpad->m_button[2]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);

      data->m_pinpad->m_sizeCombo=new wxComboBox(canvas,wxID_ANY,_T(""),wxDefaultPosition,wxSize(30,-1)/*DefaultSize*/,0,NULL,wxCB_DROPDOWN|wxCB_READONLY);
      gbSizer->Add(data->m_pinpad->m_sizeCombo,wxGBPosition(0,4),wxGBSpan(1,4),wxEXPAND);
      data->m_pinpad->m_sizeCombo->Append(_("0.001"));
      data->m_pinpad->m_sizeCombo->Append(_("0.005"));
      data->m_pinpad->m_sizeCombo->Append(_("0.010"));
      data->m_pinpad->m_sizeCombo->Append(_("0.050"));
      data->m_pinpad->m_sizeCombo->Append(_("0.100"));
      data->m_pinpad->m_sizeCombo->Append(_("0.500"));
      data->m_pinpad->m_sizeCombo->Append(_("1.000"));
      data->m_pinpad->m_sizeCombo->Append(_("5.000"));
      data->m_pinpad->m_sizeCombo->Append(_("10.00"));
      data->m_pinpad->m_sizeCombo->Select(4);
      data->m_pinpad->m_sizeCombo->Connect(data->m_pinpad->m_sizeCombo->GetId(),wxEVT_COMMAND_COMBOBOX_SELECTED,wxCommandEventHandler(PinPad::OnButtonClicked));
      //--------------------------------------
      data->m_pinpad->m_button[3]=new wxBitmapButton(canvas,wxID_ANY,wxBitmap(direction_L_xpm));
      gbSizer->Add(data->m_pinpad->m_button[3],wxGBPosition(1,0),wxGBSpan(2,1),wxEXPAND);
      data->m_pinpad->m_button[3]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);

      data->m_pinpad->m_button[4]=new wxBitmapButton(canvas,wxID_ANY,wxBitmap(rotate_L_xpm));
      gbSizer->Add(data->m_pinpad->m_button[4],wxGBPosition(1,1),wxGBSpan(2,1),wxEXPAND);
      data->m_pinpad->m_button[4]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);

      data->m_pinpad->m_button[5]=new wxBitmapButton(canvas,wxID_ANY,wxBitmap(rotate_R_xpm));
      gbSizer->Add(data->m_pinpad->m_button[5],wxGBPosition(1,2),wxGBSpan(2,1),wxEXPAND);
      data->m_pinpad->m_button[5]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);

      data->m_pinpad->m_button[6]=new wxBitmapButton(canvas,wxID_ANY,wxBitmap(direction_R_xpm));
      gbSizer->Add(data->m_pinpad->m_button[6],wxGBPosition(1,3),wxGBSpan(2,1),wxEXPAND);
      data->m_pinpad->m_button[6]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);

      data->m_pinpad->m_distField=new oapcFloatCtrl(canvas,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTE_RIGHT,1.0);
      gbSizer->Add(data->m_pinpad->m_distField,wxGBPosition(1,4),wxGBSpan(1,3),wxEXPAND);  

      wxStaticText *text=new wxStaticText(canvas,wxID_ANY,_T("mm"));
      gbSizer->Add(text,wxGBPosition(1,7),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER);

      data->m_pinpad->m_rotField=new oapcFloatCtrl(canvas,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTE_RIGHT,0.5);
      gbSizer->Add(data->m_pinpad->m_rotField,wxGBPosition(2,4),wxGBSpan(1,3),wxEXPAND);

      text=new wxStaticText(canvas,wxID_ANY,_("deg"));
      gbSizer->Add(text,wxGBPosition(2,7),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER);
      //--------------------------------------
      data->m_pinpad->m_button[7]=new wxBitmapButton(canvas,wxID_ANY,wxBitmap(direction_4_xpm));
      gbSizer->Add(data->m_pinpad->m_button[7],wxGBPosition(3,0),wxGBSpan(1,1),wxEXPAND);
      data->m_pinpad->m_button[7]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);

      data->m_pinpad->m_button[8]=new wxBitmapButton(canvas,wxID_ANY,wxBitmap(direction_D_xpm));
      gbSizer->Add(data->m_pinpad->m_button[8],wxGBPosition(3,1),wxGBSpan(1,2),wxEXPAND);
      data->m_pinpad->m_button[8]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);

      data->m_pinpad->m_button[9]=new wxBitmapButton(canvas,wxID_ANY,wxBitmap(direction_3_xpm));
      gbSizer->Add(data->m_pinpad->m_button[9],wxGBPosition(3,3),wxGBSpan(1,1),wxEXPAND);
      data->m_pinpad->m_button[9]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);

      data->m_pinpad->m_resetButton=new wxButton(canvas,wxID_ANY,_("Reset"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
      gbSizer->Add(data->m_pinpad->m_resetButton,wxGBPosition(3,4),wxGBSpan(1,3),wxEXPAND);
      data->m_pinpad->m_resetButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(PinPad::OnButtonClicked),NULL,data->m_pinpad);

      for (i=0; i<=4; i++) gbSizer->AddGrowableRow(i,1);
      for (i=0; i<=6; i++) gbSizer->AddGrowableCol(i,1);

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
   *x=320;
   *y=160;
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
      data->m_bin=oapc_util_alloc_bin_data(OAPC_BIN_TYPE_STRUCT,OAPC_BIN_SUBTYPE_STRUCT_POS_CORR,OAPC_COMPRESS_NONE,sizeof(struct oapc_bin_struct_pos_corr));
      if (!data->m_bin) return OAPC_ERROR_NO_MEMORY;
      memcpy(&data->m_bin->data,&data->posCorr,sizeof(struct oapc_bin_struct_pos_corr));
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
