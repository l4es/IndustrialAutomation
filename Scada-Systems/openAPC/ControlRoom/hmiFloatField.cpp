/******************************************************************************

This file is part of ControlRoom process control/HMI software.

ControlRoom is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

ControlRoom is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
ControlRoom. If not, see http://www.gnu.org/licenses/

*******************************************************************************

For different licensing and/or usage of the sources apart from GPL or any other
open source license, please contact us at https://openapc.com/contact.php

*******************************************************************************/

#include <wx/wx.h>
#include <wx/spinctrl.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif
#include "oapcFloatCtrl.h"
#include "iff.h"
#include "oapc_libio.h"
#include "globals.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiHSlider.h"
#include "hmiNumField.h"
#include "hmiFloatField.h"


hmiFloatField::hmiFloatField(BasePanel *parent,flowObject *obj)
              :hmiNumField(parent,obj)
{
   this->data.type=HMI_TYPE_FLOATFIELD;
#ifdef ENV_EDITOR   
   this->data.logFlags=LOG_TYPE_FLOATNUM;
   noUIFlags=OAPC_HMI_NO_UI_TEXT;
   if (!obj)
   {
       this->data.msizex=60*1000;
       this->data.msizey=20*1000;
   }
#else
   m_lastVal=0.0;
#endif
}



wxWindow *hmiFloatField::createUIElement()
{
   uiElement=new oapcFloatCtrl(m_parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTE_RIGHT|wxTE_PROCESS_ENTER,0.0);
   if (data.floatAccuracy>4) data.floatAccuracy=4;
   else if (data.floatAccuracy<1) data.floatAccuracy=1;
   ((oapcFloatCtrl*)uiElement)->SetAccuracy(data.floatAccuracy);
   hmiObject::createUIElement();
#ifdef ENV_PLAYER
   // commands for "value has changed"
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(HMICanvas::OnNumberChanged));
   uiElement->Connect(uiElement->GetId(),wxEVT_KILL_FOCUS,wxFocusEventHandler(HMICanvas::OnNumberFocusChanged),NULL,g_hmiCanvas);
#endif
   return uiElement;
}



hmiFloatField::~hmiFloatField()
{

}



#ifndef ENV_PLAYER
wxString hmiFloatField::getDefaultName()
{
    return _T("Floating Number Field");
}



wxInt32 hmiFloatField::save(wxFile *FHandle)
{
   wxInt32 length;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_FLTF,4);
   length=sizeof(struct hmiObjectData)+sizeof(struct hmiObjectHead);
   length=htonl(length);
   FHandle->Write(&length,4);
   length=hmiObject::save(FHandle);

   return length;
}
#endif



void hmiFloatField::doApplyData(const wxByte all)
{
    hmiObject::doApplyData(all);
    if (all)
    {
       if (!applyLayoutData())
       {
          setSize(wxRealPoint(data.msizex,data.msizey));
          setPos(wxRealPoint(data.mposx,data.mposy));
       }
    }
    uiElement->SetBackgroundColour(wxColour(data.bgcolor[digi[0]]));
    uiElement->SetForegroundColour(wxColour(data.fgcolor[digi[0]]));
//    if (((wxSlider*)uiElement)->GetMin()!=data.minValue) ((wxSlider*)uiElement)->SetMin(data.minValue);
//    if (((wxSlider*)uiElement)->GetMax()!=data.maxValue) ((wxSlider*)uiElement)->SetMax(data.maxValue);
    applyFont(uiElement);
#ifndef ENV_PLAYER
    ((oapcFloatCtrl*)uiElement)->Enable(false);
#else
    uiElement->Enable((data.state[digi[0]] & HMI_STATE_FLAG_DISABLED)==0);
#endif

    ((oapcFloatCtrl*)uiElement)->SetValue(num);
}



#ifdef ENV_PLAYER
bool hmiFloatField::valueHasChanged()
{
	return (m_lastVal!=((oapcFloatCtrl*)uiElement)->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE));
}



wxFloat64 hmiFloatField::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   if ((connection->sourceOutputNum==6) || (connection->sourceOutputNum==7))
   {
      m_lastVal=((oapcFloatCtrl*)uiElement)->GetValueUnlimited();
      if (m_lastVal<MinValue) m_lastVal=MinValue;
      else if (m_lastVal>MaxValue) m_lastVal=MaxValue;
      logDataCtrl->logNum(m_lastVal,this);
      *rcode=OAPC_OK;
      return m_lastVal;
   }
   else
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
#endif
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
}

#else

wxPanel* hmiFloatField::getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num)
{
   if (num>0) return NULL;
   wxPanel *panel=new wxPanel(parent);

   wxFlexGridSizer *fSizer=new wxFlexGridSizer(2,4,4);

   wxStaticText *text=new wxStaticText(panel,wxID_ANY,_("Number of decimal places")+_T(":"));
   fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   if (this->data.floatAccuracy>4) this->data.floatAccuracy=4;
   else if (this->data.floatAccuracy<1) this->data.floatAccuracy=1;
   m_numDecPlaces=new wxSpinCtrl(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,1,4,data.floatAccuracy);
   fSizer->Add(m_numDecPlaces,1,wxEXPAND);

   fSizer->AddGrowableCol(1);
   panel->SetSizer(fSizer);

   *name=_("Format");
   return panel;
}


void hmiFloatField::setConfigData()
{
   this->data.floatAccuracy=m_numDecPlaces->GetValue();
}

#endif
