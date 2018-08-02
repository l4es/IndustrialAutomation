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
#include "oapc_libio.h"
#include "globals.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiHSlider.h"
#include "DlgDataFlowhmiHSlider.h"


hmiHSlider::hmiHSlider(BasePanel *parent,flowObject *obj)
           :hmiObject(parent,obj)
{
   data.type=HMI_TYPE_HSLIDER;
   data.stdIN =OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|             OAPC_NUM_IO7;
   data.stdOUT=OAPC_DIGI_IO0|                                        OAPC_NUM_IO6|OAPC_NUM_IO7;
   num=0;
#ifdef ENV_EDITOR
   data.logFlags=LOG_TYPE_INTNUM;
   data.dataFlowFlags|=HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT7;
   noFlowFlags=0; //HMI_NOFLOWFLAG_ON_DIGI_IN1;
   noUIFlags=OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_FONT;
   if (obj==NULL)
   {
//       num[1]=(wxFloat32)33.33;
       this->data.msizex=200*1000;
       this->data.msizey=30*1000;
       MinValue=0.0;
       MaxValue=100.0;
       data.bgcolor[0]=g_objectList.m_projectData->bgCol;
       data.bgcolor[1]=g_objectList.m_projectData->bgCol;
   }
#endif
/*#ifdef ENV_PLAYER
   lastThreadTime=0;
#endif*/
}



wxWindow *hmiHSlider::createUIElement()
{
   uiElement=new wxSlider(m_parent,wxID_ANY,(MinValue+MaxValue)/3.0,MinValue,MaxValue,wxDefaultPosition,wxDefaultSize,0);
   hmiObject::createUIElement();
#ifdef ENV_PLAYER
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_SCROLL_THUMBRELEASE,wxScrollEventHandler(HMICanvas::OnSliderScrolled));
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_SCROLL_LINEUP,wxScrollEventHandler(HMICanvas::OnSliderScrolled));
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_SCROLL_LINEDOWN,wxScrollEventHandler(HMICanvas::OnSliderScrolled));
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_SCROLL_PAGEUP,wxScrollEventHandler(HMICanvas::OnSliderScrolled));
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_SCROLL_PAGEDOWN,wxScrollEventHandler(HMICanvas::OnSliderScrolled));
   //commands for "value is changing"
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_SCROLL_THUMBTRACK,wxScrollEventHandler(HMICanvas::OnSliderScrolling));

   /*   // commands for "value has changed"
   uiElement->Connect(wxEVT_SCROLL_THUMBRELEASE,wxScrollEventHandler(HMICanvas::OnSliderScrolled),NULL,parent);
   uiElement->Connect(wxEVT_SCROLL_LINEUP,wxScrollEventHandler(HMICanvas::OnSliderScrolled),NULL,parent);
   uiElement->Connect(wxEVT_SCROLL_LINEDOWN,wxScrollEventHandler(HMICanvas::OnSliderScrolled),NULL,parent);
   uiElement->Connect(wxEVT_SCROLL_PAGEUP,wxScrollEventHandler(HMICanvas::OnSliderScrolled),NULL,parent);
   uiElement->Connect(wxEVT_SCROLL_PAGEDOWN,wxScrollEventHandler(HMICanvas::OnSliderScrolled),NULL,parent);
   //commands for "value is changing"
   uiElement->Connect(wxEVT_SCROLL_THUMBTRACK,wxScrollEventHandler(HMICanvas::OnSliderScrolling),NULL,parent);*/
#endif

   return uiElement;
}



hmiHSlider::~hmiHSlider()
{
}



#ifndef ENV_PLAYER
wxString hmiHSlider::getDefaultName()
{
    return _T("Horizontal Slider");
}



wxInt32 hmiHSlider::save(wxFile *FHandle)
{
   wxInt32 length;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_HSLD,4);
   length=sizeof(struct hmiObjectData)+sizeof(struct hmiObjectHead);
   length=htonl(length);
   FHandle->Write(&length,4);
   length=hmiObject::save(FHandle);

   return length;
}
#endif



void hmiHSlider::doApplyData(const wxByte all)
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
    if (((wxSlider*)uiElement)->GetMin()!=MinValue) ((wxSlider*)uiElement)->SetMin(MinValue);
    if (((wxSlider*)uiElement)->GetMax()!=MaxValue) ((wxSlider*)uiElement)->SetMax(MaxValue);
    uiElement->Enable((data.state[digi[0]] & HMI_STATE_FLAG_DISABLED)==0);
    ((wxSlider*)uiElement)->SetValue(num);
}



#ifndef ENV_PLAYER
void hmiHSlider::doDataFlowDialog(bool hideISConfig)
{
   DlgDataFlowhmiHSlider dlg(this,m_parent,NULL,_("Data flow definition"),noFlowFlags,hideISConfig);
   dlg.ShowModal();
}
#endif



#ifdef ENV_PLAYER
wxUint64 hmiHSlider::getAssignedOutput(wxUint64 input)
{
   if (input==OAPC_NUM_IO7) return OAPC_NUM_IO7;
   return hmiObject::getAssignedOutput(input);
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
wxByte hmiHSlider::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object)
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   if (connection->targetInputNum==1)
   {
       if ((data.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT6) && (value))
        ((HMICanvas*)m_parent)->DoOnSliderScrolled(this,OAPC_NUM_IO6,*flowThreadID,wxGetLocalTimeMillis());
       else if ((data.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT6) && (!value))
        ((HMICanvas*)m_parent)->DoOnSliderScrolled(this,OAPC_NUM_IO6,*flowThreadID,wxGetLocalTimeMillis());
       if ((data.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT7) && (value))
        ((HMICanvas*)m_parent)->DoOnSliderScrolled(this,OAPC_NUM_IO7,*flowThreadID,wxGetLocalTimeMillis());
       else if ((data.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT7) && (!value))
        ((HMICanvas*)m_parent)->DoOnSliderScrolled(this,OAPC_NUM_IO7,*flowThreadID,wxGetLocalTimeMillis());
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
wxByte hmiHSlider::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   bool changed=false;
   if (!threadIDOK(flowThreadID,false)) return 0;
   if (connection->targetInputNum==2) this->MinValue=value;
   else if (connection->targetInputNum==3) this->MaxValue=value;
   else if (connection->targetInputNum==7)
   {
      num=value;
      changed=true;
   }
   else 
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
#endif
      return 0;
   }
   if (num>MaxValue)
   {
      num=MaxValue;
      changed=true;
   }
   else if (num<MinValue)
   {
      num=MinValue;
      changed=true;
   }
   applyData(0);

   if (changed)
   {
      // continue to send these data only in case the changed flag is set
      if (data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT6)
       ((HMICanvas*)m_parent)->DoOnSliderScrolled(this,OAPC_NUM_IO6,*flowThreadID,wxGetLocalTimeMillis());
      if (data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT7)
       ((HMICanvas*)m_parent)->DoOnSliderScrolled(this,OAPC_NUM_IO7,*flowThreadID,wxGetLocalTimeMillis());
   }
   return 1;
}



wxFloat64 hmiHSlider::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   if ((connection->sourceOutputNum==6) || (connection->sourceOutputNum==7))
   {
      wxInt32 l;

      l=((wxSlider*)uiElement)->GetValue();
      if (l<MinValue) l=MinValue;
      else if (l>MaxValue) l=MaxValue;

      logDataCtrl->logNum(l,this);

      *rcode=OAPC_OK;
      return l;
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
#endif

