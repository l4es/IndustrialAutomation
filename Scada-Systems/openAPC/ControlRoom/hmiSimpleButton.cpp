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

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif
#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "globals.h"
#include "oapc_libio.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "LogDataCtrl.h"
#include "hmiSimpleButton.h"


hmiSimpleButton::hmiSimpleButton(BasePanel *parent,flowObject *obj)
                :hmiObject(parent,obj)
{
   this->data.type=HMI_TYPE_SIMPLEBUTTON;
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_CHAR_IO3;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_DIGI_IO1;
#ifdef ENV_EDITOR
   this->data.logFlags=LOG_TYPE_DIGI;
   noUIFlags=OAPC_HMI_NO_UI_MINMAX|OAPC_HMI_NO_UI_RO;
   if (obj==NULL)
   {
       this->data.msizex=110*1000;
       this->data.msizey=30*1000;
       text[0]=getDefaultName();
       text[1]=getDefaultName();
   }
#endif
}



wxWindow *hmiSimpleButton::createUIElement()
{
   uiElement=new wxButton(m_parent,wxID_ANY,g_res->getString(text[0]));
   hmiObject::createUIElement();
#ifdef ENV_PLAYER
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(HMICanvas::OnButtonClicked));
#endif
   return uiElement;
}



hmiSimpleButton::~hmiSimpleButton()
{
}



#ifdef ENV_EDITOR
wxString hmiSimpleButton::getDefaultName()
{
    return _T("Simple Button");
}



wxInt32 hmiSimpleButton::save(wxFile *FHandle)
{
   wxInt32 length;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_SIBU,4);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData);
   length=htonl(length);
   FHandle->Write(&length,4);
   length=hmiObject::save(FHandle);

   return length;
}
#endif



void hmiSimpleButton::doApplyData(const wxByte all)
{
   if (all)
   {
      if (!applyLayoutData())
      {
         setSize(wxRealPoint(data.msizex,data.msizey));
         setPos(wxRealPoint(data.mposx,data.mposy));
      }
   }
   uiElement->SetBackgroundColour(wxColour(data.bgcolor[digi[0]] & 0xFFFFFF));
   uiElement->SetForegroundColour(wxColour(data.fgcolor[digi[0]] & 0xFFFFFF));
   applyFont(uiElement);
   uiElement->SetLabel(g_res->getString(text[digi[0]]));
   uiElement->Enable((data.state[digi[0]] & HMI_STATE_FLAG_DISABLED)==0);
}



wxInt32 hmiSimpleButton::load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset)
{
   wxInt32 res;

   res=hmiObject::load(FHandle,chunkSize,IDOffset);
   return res;
}




#ifdef ENV_PLAYER
/**
Sets an digital input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input (0 or 1)
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte hmiSimpleButton::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object)
{
   if (flowObject::setDigitalInput(connection,value,flowThreadID,object))
   {
      if (connection->targetInputNum==0) applyData(0);
      return 1;
   }
   else return 0;
}



wxByte hmiSimpleButton::getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime)
{
#ifdef ENV_DEBUGGER
   if ((connection->sourceOutputNum<0) || (connection->sourceOutputNum>=MAX_NUM_IOS))
   {
      wxASSERT(0);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
#endif
   if (connection->sourceOutputNum==1)
   {
      logDataCtrl->logDigi(1,this);
      logDataCtrl->logDigi(0,this);
      return 1;
   }
   return hmiObject::getDigitalOutput(connection,rcode,lastInput,origCreationTime);
}



/**
Sets an character input
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0; this method has to be overridden
*/
wxByte hmiSimpleButton::setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
#ifdef ENV_DEBUGGER
   if (connection->targetInputNum!=3)
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      return 0;
   }
#else
   connection=connection;
#endif
   text[digi[0]]=value;
   applyData(0);
   return 1;
}



wxUint64 hmiSimpleButton::getAssignedOutput(wxUint64 input)
{
   if (input==OAPC_DIGI_IO1) return OAPC_DIGI_IO1;
   else return hmiObject::getAssignedOutput(input);
}



#endif
