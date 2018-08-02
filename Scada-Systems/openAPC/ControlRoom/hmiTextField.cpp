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
#include "iff.h"
#include "oapc_libio.h"
#include "globals.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiHSlider.h"
#include "hmiNumField.h"
#include "hmiTextField.h"
#include "oapcTextCtrl.h"
#include "DlgDataFlowhmiTextField.h"


hmiTextField::hmiTextField(BasePanel *parent,flowObject *obj,wxInt32 type)
             :hmiNumField(parent,obj)
{
   this->data.type=type;
   this->data.stdIN =OAPC_DIGI_IO0|OAPC_DIGI_IO1|             OAPC_CHAR_IO7;
   this->data.stdOUT=OAPC_DIGI_IO0|             OAPC_CHAR_IO6|OAPC_CHAR_IO7;
#ifdef ENV_EDITOR
   this->data.logFlags=LOG_TYPE_CHAR;
   noUIFlags=OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_MINMAX;
   txt[7]=getDefaultName();
   if (obj==NULL)
   {
       this->data.msizex=90*1000;
       this->data.msizey=22*1000;
   }
#endif
}



wxWindow *hmiTextField::createUIElement()
{
   if (data.type==HMI_TYPE_TEXTFIELD) uiElement=new oapcTextCtrl(m_parent,wxID_ANY,_T(""),wxDefaultPosition,wxDefaultSize,wxTE_RIGHT|wxTE_PROCESS_ENTER);
   else uiElement=new oapcTextCtrl(m_parent,wxID_ANY,_T(""),wxDefaultPosition,wxDefaultSize,wxTE_RIGHT|wxTE_PROCESS_ENTER|wxTE_PASSWORD);
   hmiObject::createUIElement();
#ifdef ENV_PLAYER
   // commands for "value has changed"
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(HMICanvas::OnCharChanged));
   uiElement->Connect(uiElement->GetId(),wxEVT_KILL_FOCUS,wxFocusEventHandler(HMICanvas::OnCharFocusChanged),NULL,g_hmiCanvas);
#endif

   return uiElement;
}



hmiTextField::~hmiTextField()
{

}



#ifndef ENV_PLAYER
wxString hmiTextField::getDefaultName()
{
   if (data.type==HMI_TYPE_TEXTFIELD) return _("Text Field");
   else return _("Password Field");
}



wxInt32 hmiTextField::save(wxFile *FHandle)
{
   wxInt32 length;

   if (!FHandle) return 0;

   if (data.type==HMI_TYPE_TEXTFIELD) FHandle->Write(CHUNK_HOBL_TXTF,4);
   else FHandle->Write(CHUNK_HOBL_PWDF,4);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData);
   length=htonl(length);
   FHandle->Write(&length,4);
   length=hmiObject::save(FHandle);

   return length;
}
#endif



void hmiTextField::doApplyData(const wxByte all)
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
   applyFont(uiElement);
#ifndef ENV_PLAYER
   ((wxTextCtrl*)uiElement)->Enable(false);
#else
   uiElement->Enable((data.state[digi[0]] & HMI_STATE_FLAG_DISABLED)==0);
   ((wxTextCtrl*)uiElement)->SetEditable((data.state[digi[0]] & HMI_STATE_FLAG_RO)==0);
#endif
   ((wxTextCtrl*)uiElement)->SetValue(txt[7]);
}



#ifndef ENV_PLAYER
void hmiTextField::doDataFlowDialog(bool hideISConfig)
{
   DlgDataFlowhmiTextField dlg(this,m_parent,NULL,_("Data flow definition"),0,hideISConfig);
   dlg.ShowModal();
}
#endif


#ifdef ENV_PLAYER
bool hmiTextField::valueHasChanged()
{
   return (m_lastVal.Cmp(((wxTextCtrl*)uiElement)->GetValue())!=0);
}


/**
Sets an digital input, checks if the same thread has set that input before and uses the
input as trigger for sending the own value (if configured)
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input (0 or 1)
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte hmiTextField::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object)
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   if (connection->targetInputNum==1)
   {
       if ((data.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT6) && (value))
        ((HMICanvas*)m_parent)->DoOnSliderScrolled(this,OAPC_CHAR_IO6,*flowThreadID,wxGetLocalTimeMillis());
       else if ((data.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT6) && (!value))
        ((HMICanvas*)m_parent)->DoOnSliderScrolled(this,OAPC_CHAR_IO6,*flowThreadID,wxGetLocalTimeMillis());
       if ((data.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT7) && (value))
        ((HMICanvas*)m_parent)->DoOnSliderScrolled(this,OAPC_CHAR_IO7,*flowThreadID,wxGetLocalTimeMillis());
       else if ((data.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT7) && (!value))
        ((HMICanvas*)m_parent)->DoOnSliderScrolled(this,OAPC_CHAR_IO7,*flowThreadID,wxGetLocalTimeMillis());
       return 0; // return with 0 because afterwards no digi thread has to run but a numerical thread
   }
   return hmiObject::setDigitalInput(connection,value,flowThreadID,object);
}



/**
Sets an numerical input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte hmiTextField::setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   txt[connection->targetInputNum]=value;
   applyData(0);

   // continue to send these data only in case the changed flag is set
   if (data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT6)
    ((HMICanvas*)g_hmiCanvas)->DoOnSliderScrolled(this,OAPC_CHAR_IO6,*flowThreadID,wxGetLocalTimeMillis());
   if (data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT7)
    ((HMICanvas*)g_hmiCanvas)->DoOnSliderScrolled(this,OAPC_CHAR_IO7,*flowThreadID,wxGetLocalTimeMillis());

   return 0;
}



wxString hmiTextField::getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
#ifdef ENV_DEBUGGER
   if ((connection->sourceOutputNum<0) || (connection->sourceOutputNum>=MAX_NUM_IOS))
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return _T("");
   }
#endif
   if ((connection->sourceOutputNum==6) || (connection->sourceOutputNum==7))
   {
      m_lastVal=((wxTextCtrl*)uiElement)->GetValue();
      logDataCtrl->logChar(m_lastVal,this);

      *rcode=OAPC_OK;
      return m_lastVal;
   }
   *rcode=OAPC_OK;
   return txt[connection->sourceOutputNum];
}
#endif

