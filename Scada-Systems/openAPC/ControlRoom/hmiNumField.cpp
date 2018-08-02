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

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif
#include "iff.h"
#include "globals.h"
#include "oapc_libio.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiHSlider.h"
#include "hmiNumField.h"
#include "oapcSpinCtrl.h"


hmiNumField::hmiNumField(BasePanel *parent,flowObject *obj)
            :hmiHSlider(parent,obj)
{
   this->data.type=HMI_TYPE_NUMFIELD;
#ifdef ENV_EDITOR
   this->data.logFlags=LOG_TYPE_INTNUM;
   noUIFlags=OAPC_HMI_NO_UI_TEXT;
   noFlowFlags=0;
   if (obj==NULL)
   {
       this->data.msizex=80*1000;
       this->data.msizey=22*1000;
       MinValue=-2100000;
       MaxValue=2100000;
   }
#else
   m_lastVal=0;
#endif
}



wxWindow *hmiNumField::createUIElement()
{
   if (MaxValue>2100000000) MaxValue=2100000000; // limitation for spinCtrl
   uiElement=new oapcSpinCtrl(m_parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,MinValue,MaxValue,0);
   applyData(1);
   hmiObject::createUIElement();
#ifdef ENV_PLAYER
   // commands for "value has changed"
   g_hmiCanvas->Connect(((oapcSpinCtrl*)uiElement)->m_numField->GetId(),wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(HMICanvas::OnNumberChanged),NULL,g_hmiCanvas);
   ((oapcSpinCtrl*)uiElement)->m_numField->Connect(((oapcSpinCtrl*)uiElement)->m_numField->GetId(),wxEVT_KILL_FOCUS,wxFocusEventHandler(HMICanvas::OnNumberFocusChanged),NULL,g_hmiCanvas);

   //commands for "value is changing"
   //the spin button events are forwarded from the oapcSpinCtrl object to g_hmiCanvas directly
#endif
   return uiElement;
}



void hmiNumField::doApplyData(const wxByte all)
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
   if (((oapcSpinCtrl*)uiElement)->GetValue()<MinValue) ((oapcSpinCtrl*)uiElement)->SetValue(MinValue);
   else if (((oapcSpinCtrl*)uiElement)->GetValue()>MaxValue) ((oapcSpinCtrl*)uiElement)->SetValue(MaxValue);
   applyFont(uiElement);
#ifndef ENV_PLAYER
   ((oapcSpinCtrl*)uiElement)->Enable(false);
#else
   ((oapcSpinCtrl*)uiElement)->Enable((data.state[digi[0]] & HMI_STATE_FLAG_DISABLED)==0);
   ((oapcSpinCtrl*)uiElement)->SetEditable((data.state[digi[0]] & HMI_STATE_FLAG_RO)==0);
#endif
   ((oapcSpinCtrl*)uiElement)->SetValue(num);
   uiElement->Refresh();
}



hmiNumField::~hmiNumField()
{

}



#ifndef ENV_PLAYER
wxString hmiNumField::getDefaultName()
{
    return _T("Number Field");
}



wxInt32 hmiNumField::save(wxFile *FHandle)
{
   wxInt32 length;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_NUMF,4);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData);
   length=htonl(length);
   FHandle->Write(&length,4);
   length=hmiObject::save(FHandle);

   return length;
}
#else



bool hmiNumField::valueHasChanged()
{
	return (m_lastVal!=((oapcSpinCtrl*)uiElement)->GetValue());
}



wxFloat64 hmiNumField::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   if ((connection->sourceOutputNum==6) || (connection->sourceOutputNum==7))
   {
      m_lastVal=((oapcSpinCtrl*)uiElement)->GetValue();
      if (m_lastVal<MinValue) m_lastVal=MinValue;
      else if (m_lastVal>MaxValue) m_lastVal=MaxValue;
      logDataCtrl->logNum(m_lastVal,this);
      *rcode=OAPC_OK;
      return m_lastVal;
   }
#ifdef ENV_DEBUGGER
   g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
#endif
   *rcode=OAPC_ERROR_NO_SUCH_IO;
   return 0;
}
#endif




