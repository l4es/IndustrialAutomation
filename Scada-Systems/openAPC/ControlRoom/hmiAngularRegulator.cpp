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
//#include <wx/colourdata.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif
#include "iff.h"
#include "oapc_libio.h"
#include "MainWin.h"
#include "globals.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiHGauge.h"
#include "hmiAngularRegulator.h"
#include "kwxCtrl/AngularRegulator.h"

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif



hmiAngularRegulator::hmiAngularRegulator(BasePanel *parent,flowObject *obj)
                    :hmiHSlider(parent,obj)
{
#ifdef ENV_EDITOR
   wxInt32 i;

   noUIFlags=OAPC_HMI_NO_UI_FG|OAPC_HMI_NO_UI_FONT|OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_FREE_SIZE_RATIO;
   noFlowFlags=HMI_NOFLOWFLAG_ON_CHANGED;
   if (obj==NULL)
   {
      data.dataFlowFlags=HMI_FLOWFLAG_ON_CHANGING_OUT7;
      data.msizex=120*1000;
      data.msizey=120*1000;

      meterData.numTags=11;
      meterData.startAngle=-450; // 1/10 degrees
      meterData.endAngle=2250;   // 1/10 degrees
      meterData.reserved1=0;
      meterData.reserved2=0;
      meterData.reserved3=0;

      for (i=0; i<2; i++)
      {
         data.fgcolor[i]=0x000000;
         data.bgcolor[i]=g_objectList.m_projectData->bgCol;
         meterData.extCircleColour[i]=0x500000; 
         meterData.intCircleColour[i]=0xBB2222; 
         meterData.knobBorderColour[i]=0x110000;
         meterData.knobColour[i]=0xFF5555;  
         meterData.limitsColour[i]=0x9999FF;
         meterData.tagsColour[i]=0xE0E0E0;
      }
      num=0;
   }
#endif
   this->data.type=HMI_TYPE_ANGULARREGULATOR;
}



wxWindow *hmiAngularRegulator::createUIElement()
{
   uiElement=new kwxAngularRegulator(m_parent,wxID_ANY);
   ((kwxAngularRegulator*)uiElement)->SetSize(data.msizex/1000.0,data.msizey/1000.0);
   hmiObject::createUIElement();
#ifdef ENV_PLAYER
   g_hmiCanvas->Connect(uiElement->GetId(),kwxEVT_ANGREG_CHANGE,wxCommandEventHandler(HMICanvas::OnNumberChanging));
#endif
   return uiElement;
}



void hmiAngularRegulator::setColours()
{
#ifdef ENV_PLAYER
   if (uiElement->IsEnabled())
#endif
   {
      uiElement->SetBackgroundColour(wxColour(data.bgcolor[digi[0]]));
      uiElement->SetForegroundColour(wxColour(data.fgcolor[digi[0]]));
      ((kwxAngularRegulator*)uiElement)->SetExtCircleColour(wxColour(meterData.extCircleColour[digi[0]]));
      ((kwxAngularRegulator*)uiElement)->SetIntCircleColour(wxColour(meterData.intCircleColour[digi[0]]));
      ((kwxAngularRegulator*)uiElement)->SetTagsColour(wxColour(meterData.tagsColour[digi[0]]));
      ((kwxAngularRegulator*)uiElement)->SetKnobBorderColour(wxColour(meterData.knobBorderColour[digi[0]]));
      ((kwxAngularRegulator*)uiElement)->SetKnobColour(wxColour(meterData.knobColour[digi[0]]));
      ((kwxAngularRegulator*)uiElement)->SetLimitsColour(wxColour(meterData.limitsColour[digi[0]]));
   }
#ifdef ENV_PLAYER
   else
   {
      uiElement->SetBackgroundColour(wxColour(disBgColour[digi[0]]));
      uiElement->SetForegroundColour(wxColour(disFgColour[digi[0]]));
      ((kwxAngularRegulator*)uiElement)->SetExtCircleColour(wxColour(disExtCircleColour[digi[0]]));
      ((kwxAngularRegulator*)uiElement)->SetIntCircleColour(wxColour(disIntCircleColour[digi[0]]));
      ((kwxAngularRegulator*)uiElement)->SetTagsColour(wxColour(disTagsColour[digi[0]]));
      ((kwxAngularRegulator*)uiElement)->SetKnobBorderColour(wxColour(disKnobBorderColour[digi[0]]));
      ((kwxAngularRegulator*)uiElement)->SetKnobColour(wxColour(disKnobColour[digi[0]]));
      ((kwxAngularRegulator*)uiElement)->SetLimitsColour(wxColour(disLimitsColour[digi[0]]));
   }
#endif
}



void hmiAngularRegulator::doApplyData(const wxByte all)
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
   ((kwxAngularRegulator*)uiElement)->SetRange(MinValue,MaxValue);
   ((kwxAngularRegulator*)uiElement)->SetAngle(meterData.startAngle/10,meterData.endAngle/10);
   ((kwxAngularRegulator*)uiElement)->SetValue(num);
   setColours();
   ((kwxAngularRegulator*)uiElement)->ResetTagNum();
   if (meterData.numTags>0)
   {
      wxFloat32 tagStepSize=((MaxValue-MinValue)/(meterData.numTags+1.0));
      for (wxInt32 i=0; i<meterData.numTags; i++) ((kwxAngularRegulator*)uiElement)->AddTag((MinValue+tagStepSize*(i+1.0)));
   }
}



hmiAngularRegulator::~hmiAngularRegulator()
{
}



wxInt32 hmiAngularRegulator::load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset)
{
   struct hmiAngularRegulatorData convData;
   wxUint32                       myChunkSize,i;
   wxInt32                        ret;

   if (!FHandle) return 0;

   myChunkSize=chunkSize;
   if (myChunkSize>sizeof(struct hmiAngularRegulatorData)) myChunkSize=sizeof(struct hmiAngularRegulatorData);

   myChunkSize=FHandle->Read(&convData,myChunkSize);
   for (i=0; i<2; i++)
   {
      meterData.extCircleColour[i] =ntohl(convData.extCircleColour[i]);
      meterData.intCircleColour[i] =ntohl(convData.intCircleColour[i]);
      meterData.knobBorderColour[i]=ntohl(convData.knobBorderColour[i]);
      meterData.knobColour[i]      =ntohl(convData.knobColour[i]);
      meterData.limitsColour[i]    =ntohl(convData.limitsColour[i]);
      meterData.tagsColour[i]      =ntohl(convData.tagsColour[i]);
   }

   meterData.endAngle        =ntohl(convData.endAngle);
   meterData.numTags         =ntohl(convData.numTags);
   meterData.startAngle      =ntohl(convData.startAngle);
   meterData.reserved1=ntohl(convData.reserved1);
   meterData.reserved2=ntohl(convData.reserved2);
   meterData.reserved3=ntohl(convData.reserved3);

   ret=hmiObject::load(FHandle,chunkSize-myChunkSize,IDOffset)+myChunkSize;

#ifdef ENV_PLAYER
   for (i=0; i<2; i++)
   {
      disExtCircleColour[i] =oapc_util_colour2gray(meterData.extCircleColour[i]);
      disIntCircleColour[i] =oapc_util_colour2gray(meterData.intCircleColour[i]);
      disKnobBorderColour[i]=oapc_util_colour2gray(meterData.knobBorderColour[i]);
      disKnobColour[i]      =oapc_util_colour2gray(meterData.knobColour[i]);
      disLimitsColour[i]    =oapc_util_colour2gray(meterData.limitsColour[i]);
      disTagsColour[i]      =oapc_util_colour2gray(meterData.tagsColour[i]);
      disBgColour[i]        =oapc_util_colour2gray(data.bgcolor[i]);
      disFgColour[i]        =oapc_util_colour2gray(data.fgcolor[i]);
   }
#endif
   return ret;
}



#ifdef ENV_PLAYER
wxFloat64 hmiAngularRegulator::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
#ifdef ENV_DEBUGGER
   if ((connection->sourceOutputNum<0) || (connection->sourceOutputNum>=MAX_NUM_IOS))
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
#endif
   if ((connection->sourceOutputNum==6) || (connection->sourceOutputNum==7))
   {
      wxInt32 l;

      l=((kwxAngularRegulator*)uiElement)->GetValue();
      logDataCtrl->logNum(l,this);
      *rcode=OAPC_OK;
      return l;
   }
   *rcode=OAPC_OK;
   return num;
}
#endif



#ifdef ENV_EDITOR
void hmiAngularRegulator::setData(flowObject *object)
{
   memcpy(&meterData,&((hmiAngularRegulator*)object)->meterData,sizeof(struct hmiAngularRegulatorData));
   flowObject::setData(object);
}



wxString hmiAngularRegulator::getDefaultName()
{
    return _T("Angular Regulator");
}



wxPoint hmiAngularRegulator::getMinSize()
{
   return wxPoint(60,60);
}




wxInt32 hmiAngularRegulator::save(wxFile *FHandle)
{
   wxInt32                    length,i;
   struct hmiAngularRegulatorData convData;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_ANRE,4);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData)+sizeof(struct hmiAngularRegulatorData);
   length=htonl(length);
   FHandle->Write(&length,4);

   for (i=0; i<2; i++)
   {
      convData.extCircleColour[i] =htonl(meterData.extCircleColour[i]);
      convData.intCircleColour[i] =htonl(meterData.intCircleColour[i]);
      convData.knobBorderColour[i]=htonl(meterData.knobBorderColour[i]);
      convData.knobColour[i]      =htonl(meterData.knobColour[i]);
      convData.limitsColour[i]    =htonl(meterData.limitsColour[i]);
      convData.tagsColour[i]      =htonl(meterData.tagsColour[i]);
   }
   convData.endAngle        =htonl(meterData.endAngle);
   convData.numTags         =htonl(meterData.numTags);
   convData.startAngle      =htonl(meterData.startAngle);
   convData.reserved1=htonl(meterData.reserved1);
   convData.reserved2=htonl(meterData.reserved2);
   convData.reserved3=htonl(meterData.reserved3);
   length=FHandle->Write(&convData,sizeof(struct hmiAngularRegulatorData));

   length=length+hmiObject::save(FHandle);

   return length;
}



wxPanel* hmiAngularRegulator::getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num)
{
   if (num==0)
   {
      wxPanel *panel=new wxPanel(parent);

      wxFlexGridSizer *fSizer=new wxFlexGridSizer(2,4,4);

      fSizer->Add(new wxStaticText(panel,wxID_ANY,_("Number of Ticks")/*+_T(":")*/),1,wxALIGN_CENTER_VERTICAL);
      m_numTicks=new wxSpinCtrl(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,0,20);
      m_numTicks->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
      m_numTicks->SetValue(meterData.numTags);
      fSizer->Add(m_numTicks,1,wxEXPAND);
      m_numTicks->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED,wxCommandEventHandler(hmiAngularRegulator::OnButtonClick),NULL,this);

      fSizer->Add(new wxStaticText(panel,wxID_ANY,_("Startangle")/*+_T(":")*/),1,wxALIGN_CENTER_VERTICAL);
      m_sAngle=new wxTextCtrl(panel,wxID_ANY);
      m_sAngle->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
      m_sAngle->SetValue(wxString::Format(_T("%.1f"),meterData.startAngle/10.0));
      fSizer->Add(m_sAngle,1,wxEXPAND);
      m_sAngle->Connect(wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(hmiAngularRegulator::OnButtonClick),NULL,this);

      fSizer->Add(new wxStaticText(panel,wxID_ANY,_("Endangle")/*+_T(":")*/),1,wxALIGN_CENTER_VERTICAL);
      m_eAngle=new wxTextCtrl(panel,wxID_ANY);
      m_eAngle->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
      m_eAngle->SetValue(wxString::Format(_T("%.1f"),meterData.endAngle/10.0));
      fSizer->Add(m_eAngle,1,wxEXPAND);
      m_eAngle->Connect(wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(hmiAngularRegulator::OnButtonClick),NULL,this);

      panel->SetSizer(fSizer);
      fSizer->AddGrowableCol(1,6);

      *name=_("Layout");
      return panel;
   }
   else if (num==1)
   {
      wxPanel *panel=new wxPanel(parent);

      wxFlexGridSizer *fSizer=new wxFlexGridSizer(3,4,4);

      fSizer->Add(new wxStaticText(panel,wxID_ANY,_("Digital IN 0")/*+_T(":")*/),0,0);
      fSizer->Add(new wxStaticText(panel,wxID_ANY,_("LOW State")),1,wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
      fSizer->Add(new wxStaticText(panel,wxID_ANY,_("HIGH State")),1,wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);

      for (wxInt32 i=0; i<6; i++)
      {
         if (i==0) bgText[i]=new wxStaticText(panel,wxID_ANY,_("Outer Circle")+_T(":"));
         else if (i==1) bgText[i]=new wxStaticText(panel,wxID_ANY,_("Inner Circle")+_T(":"));
         else if (i==2) bgText[i]=new wxStaticText(panel,wxID_ANY,_("Knob Border")+_T(":"));
         else if (i==3) bgText[i]=new wxStaticText(panel,wxID_ANY,_("Knob")+_T(":"));
         else if (i==4) bgText[i]=new wxStaticText(panel,wxID_ANY,_("Limits")+_T(":"));
         else if (i==5) bgText[i]=new wxStaticText(panel,wxID_ANY,_("Ticks")+_T(":"));
         fSizer->Add(bgText[i],0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

         bgLow[i]=new wxButton(panel,wxID_ANY);
         if (i==0) bgLow[i]->SetBackgroundColour(wxColour(meterData.extCircleColour[0]));
         else if (i==1) bgLow[i]->SetBackgroundColour(wxColour(meterData.intCircleColour[0]));
         else if (i==2) bgLow[i]->SetBackgroundColour(wxColour(meterData.knobBorderColour[0]));
         else if (i==3) bgLow[i]->SetBackgroundColour(wxColour(meterData.knobColour[0]));
         else if (i==4) bgLow[i]->SetBackgroundColour(wxColour(meterData.limitsColour[0]));
         else if (i==5) bgLow[i]->SetBackgroundColour(wxColour(meterData.tagsColour[0]));
         fSizer->Add(bgLow[i],1,wxEXPAND);
         bgLow[i]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(hmiAngularRegulator::OnButtonClick),NULL,this);

         bgHigh[i]=new wxButton(panel,wxID_ANY);
         if (i==0) bgHigh[i]->SetBackgroundColour(wxColour(meterData.extCircleColour[1]));
         else if (i==1) bgHigh[i]->SetBackgroundColour(wxColour(meterData.intCircleColour[1]));
         else if (i==2) bgHigh[i]->SetBackgroundColour(wxColour(meterData.knobBorderColour[1]));
         else if (i==3) bgHigh[i]->SetBackgroundColour(wxColour(meterData.knobColour[1]));
         else if (i==4) bgHigh[i]->SetBackgroundColour(wxColour(meterData.limitsColour[1]));
         else if (i==5) bgHigh[i]->SetBackgroundColour(wxColour(meterData.tagsColour[1]));
         fSizer->Add(bgHigh[i],1,wxEXPAND);
         bgHigh[i]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(hmiAngularRegulator::OnButtonClick),NULL,this);
      }

      fSizer->AddGrowableCol(0,2);
      fSizer->AddGrowableCol(1,3);
      fSizer->AddGrowableCol(2,3);
      panel->SetSizer(fSizer);

      *name=_("Colours");
      return panel;
   }
    return NULL;
}




void hmiAngularRegulator::OnButtonClick(wxCommandEvent &event)
{
   wxInt32      i;
   wxColourData colour;

   for (i=0; i<6; i++)
   {
      if (event.GetId()==bgLow[i]->GetId())
      {
         colour.SetColour(bgLow[i]->GetBackgroundColour());
         g_colourDialog->Create(g_mainWin,&colour);
         g_colourDialog->Centre();
         g_colourDialog->SetTitle(_("Choose the elements colour"));
         if (g_colourDialog->ShowModal() == wxID_OK)
         {
            colour=g_colourDialog->GetColourData();
            bgLow[i]->SetBackgroundColour(colour.GetColour());
         }
         return;
      }
      else if (event.GetId()==bgHigh[i]->GetId())
      {
         colour.SetColour(bgHigh[i]->GetBackgroundColour());
         g_colourDialog->Create(g_mainWin,&colour);
         g_colourDialog->Centre();
         g_colourDialog->SetTitle(_("Choose the elements colour"));
         if (g_colourDialog->ShowModal() == wxID_OK)
         {
            colour=g_colourDialog->GetColourData();
            bgHigh[i]->SetBackgroundColour(colour.GetColour());
         }
         return;
      }
   }
}



void hmiAngularRegulator::setConfigData()
{
   double  d;

   m_eAngle->GetValue().ToDouble(&d);   
   if (d<-360) d=-360;   
   else if (d>360) d=360;
   meterData.endAngle=d*10;

   m_sAngle->GetValue().ToDouble(&d);   
   if (d<-360) d=-360;   
   else if (d>360) d=360;
   meterData.startAngle=d*10;

   meterData.numTags=m_numTicks->GetValue();

   meterData.extCircleColour[0]= bgLow[0]->GetBackgroundColour().Red() | bgLow[0]->GetBackgroundColour().Green()<<8 | bgLow[0]->GetBackgroundColour().Blue()<<16;
   meterData.intCircleColour[0]= bgLow[1]->GetBackgroundColour().Red() | bgLow[1]->GetBackgroundColour().Green()<<8 | bgLow[1]->GetBackgroundColour().Blue()<<16;
   meterData.knobBorderColour[0]=bgLow[2]->GetBackgroundColour().Red() | bgLow[2]->GetBackgroundColour().Green()<<8 | bgLow[2]->GetBackgroundColour().Blue()<<16;
   meterData.knobColour[0]=      bgLow[3]->GetBackgroundColour().Red() | bgLow[3]->GetBackgroundColour().Green()<<8 | bgLow[3]->GetBackgroundColour().Blue()<<16;
   meterData.limitsColour[0]=    bgLow[4]->GetBackgroundColour().Red() | bgLow[4]->GetBackgroundColour().Green()<<8 | bgLow[4]->GetBackgroundColour().Blue()<<16;
   meterData.tagsColour[0]=      bgLow[5]->GetBackgroundColour().Red() | bgLow[5]->GetBackgroundColour().Green()<<8 | bgLow[5]->GetBackgroundColour().Blue()<<16;

   meterData.extCircleColour[1]= bgHigh[0]->GetBackgroundColour().Red() | bgHigh[0]->GetBackgroundColour().Green()<<8 | bgHigh[0]->GetBackgroundColour().Blue()<<16;
   meterData.intCircleColour[1]= bgHigh[1]->GetBackgroundColour().Red() | bgHigh[1]->GetBackgroundColour().Green()<<8 | bgHigh[1]->GetBackgroundColour().Blue()<<16;
   meterData.knobBorderColour[1]=bgHigh[2]->GetBackgroundColour().Red() | bgHigh[2]->GetBackgroundColour().Green()<<8 | bgHigh[2]->GetBackgroundColour().Blue()<<16;
   meterData.knobColour[1]=      bgHigh[3]->GetBackgroundColour().Red() | bgHigh[3]->GetBackgroundColour().Green()<<8 | bgHigh[3]->GetBackgroundColour().Blue()<<16;
   meterData.limitsColour[1]=    bgHigh[4]->GetBackgroundColour().Red() | bgHigh[4]->GetBackgroundColour().Green()<<8 | bgHigh[4]->GetBackgroundColour().Blue()<<16;
   meterData.tagsColour[1]=      bgHigh[5]->GetBackgroundColour().Red() | bgHigh[5]->GetBackgroundColour().Green()<<8 | bgHigh[5]->GetBackgroundColour().Blue()<<16;
}
#else

void hmiAngularRegulator::setUIElementState(wxInt32 state)
{
   hmiObject::setUIElementState(state);
   setColours();
}

#endif





