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

#include "iff.h"
#include "oapc_libio.h"
#include "MainWin.h"
#include "globals.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiHGauge.h"
#include "hmiAngularMeter.h"
#include "kwxCtrl/AngularMeter.h"

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif



hmiAngularMeter::hmiAngularMeter(BasePanel *parent,flowObject *obj)
                :hmiHGauge(parent,obj)
{
#ifdef ENV_EDITOR
   wxInt32 i;

   noUIFlags=OAPC_HMI_NO_FREE_SIZE_RATIO;
   m_userPriviEnable=OAPC_USERPRIVI_HIDE;
   if (obj==NULL)
   {
      data.msizey=data.msizex;
      meterData.startAngle=-450;
      meterData.endAngle=2250;
      meterData.numSectors=10;
      meterData.numTicks=9;
      meterData.reserved1=0;
      meterData.reserved2=0;
      for (i=0; i<2; i++)
      {
         data.fgcolor[i]=0x000000;
         data.bgcolor[i]=g_objectList.m_projectData->bgCol; 
         meterData.sectorColour[i][0]=0xFFFFFF;
         meterData.sectorColour[i][1]=0x00FF00;
         meterData.sectorColour[i][2]=0x00FF00;
         meterData.sectorColour[i][3]=0x00FF00;
         meterData.sectorColour[i][4]=0x00FF00;
         meterData.sectorColour[i][5]=0x00FF00;
         meterData.sectorColour[i][6]=0x00FF00;
         meterData.sectorColour[i][7]=0x00FF00;
         meterData.sectorColour[i][8]=0x0000FF;
         meterData.sectorColour[i][9]=0x0000FF;
      }
      for (i=10; i<MAX_NUM_ANGULARMETER_SECTORS; i++)
      {
         meterData.sectorColour[0][i]=0xFFFFFF;
         meterData.sectorColour[1][i]=0xFFFFFF;
      }
   }
#endif
   this->data.type=HMI_TYPE_ANGULARMETER;
   this->data.stdIN =OAPC_DIGI_IO0|OAPC_NUM_IO7;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_NUM_IO7;
}



wxWindow *hmiAngularMeter::createUIElement()
{
   uiElement=new kwxAngularMeter(m_parent,wxID_ANY);
   hmiObject::createUIElement();
   return uiElement;
}



void hmiAngularMeter::doApplyData(const wxByte all)
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
   ((kwxAngularMeter*)uiElement)->SetNumSectors(meterData.numSectors);
   for (wxInt32 i=0; i<meterData.numSectors; i++)
    ((kwxAngularMeter*)uiElement)->SetSectorColor(i,wxColour(meterData.sectorColour[digi[0]][i]));
   ((kwxAngularMeter*)uiElement)->SetNumTick(meterData.numTicks);
   ((kwxAngularMeter*)uiElement)->SetAngle(meterData.startAngle/10.0,meterData.endAngle/10.0);
   ((kwxAngularMeter*)uiElement)->SetRange(MinValue,MaxValue);
   applyFont(uiElement);
   ((kwxAngularMeter*)uiElement)->SetValue(num);
}



hmiAngularMeter::~hmiAngularMeter()
{
}



wxInt32 hmiAngularMeter::load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset)
{
   struct hmiAngularMeterData convData;
   wxUint32                   myChunkSize,i;

   if (!FHandle) return 0;

   myChunkSize=chunkSize;
   if (myChunkSize>sizeof(struct hmiAngularMeterData)) myChunkSize=sizeof(struct hmiAngularMeterData);

   myChunkSize=FHandle->Read(&convData,myChunkSize);
   meterData.endAngle=ntohl(convData.endAngle);
   meterData.startAngle=ntohl(convData.startAngle);
   meterData.numSectors=ntohl(convData.numSectors);
   meterData.numTicks=ntohl(convData.numTicks);
   meterData.reserved1=ntohl(convData.reserved1);
   meterData.reserved2=ntohl(convData.reserved2);
   for (i=0; i<MAX_NUM_ANGULARMETER_SECTORS; i++)
   {
      meterData.sectorColour[0][i]=ntohl(convData.sectorColour[0][i]);
      meterData.sectorColour[1][i]=ntohl(convData.sectorColour[1][i]);
   }
   return hmiObject::load(FHandle,chunkSize-myChunkSize,IDOffset)+myChunkSize;
}



#ifdef ENV_EDITOR
void hmiAngularMeter::setData(flowObject *object)
{
   memcpy(&meterData,&((hmiAngularMeter*)object)->meterData,sizeof(struct hmiAngularMeterData));
   flowObject::setData(object);
}



wxString hmiAngularMeter::getDefaultName()
{
    return _T("Angular Meter");
}



wxPoint hmiAngularMeter::getMinSize()
{
   return wxPoint(60,60);
}




wxInt32 hmiAngularMeter::save(wxFile *FHandle)
{
   wxInt32                    length,i;
   struct hmiAngularMeterData convData;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_ANME,4);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData)+sizeof(struct hmiAngularMeterData);
   length=htonl(length);
   FHandle->Write(&length,4);

   convData.endAngle=htonl(meterData.endAngle);
   convData.startAngle=htonl(meterData.startAngle);
   convData.numSectors=htonl(meterData.numSectors);
   convData.numTicks=htonl(meterData.numTicks);
   convData.reserved1=htonl(meterData.reserved1);
   convData.reserved2=htonl(meterData.reserved2);
   for (i=0; i<MAX_NUM_ANGULARMETER_SECTORS; i++)
   {
      convData.sectorColour[0][i]=htonl(meterData.sectorColour[0][i]);
      convData.sectorColour[1][i]=htonl(meterData.sectorColour[1][i]);
   }
   length=FHandle->Write(&convData,sizeof(struct hmiAngularMeterData));

   length=length+hmiObject::save(FHandle);

   return length;
}



wxPanel* hmiAngularMeter::getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num)
{
   if (num==0)
   {
      wxPanel *panel=new wxPanel(parent);

      wxFlexGridSizer *fSizer=new wxFlexGridSizer(2,4,4);

      fSizer->Add(new wxStaticText(panel,wxID_ANY,_("Number of Sectors")/*+_T(":")*/),1,wxALIGN_CENTER_VERTICAL);
      m_numSectors=new wxSpinCtrl(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,1,12);
      m_numSectors->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
      m_numSectors->SetValue(meterData.numSectors);
      fSizer->Add(m_numSectors,1,wxEXPAND);
      m_numSectors->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED,wxCommandEventHandler(hmiAngularMeter::OnButtonClick),NULL,this);

      fSizer->Add(new wxStaticText(panel,wxID_ANY,_("Number of Ticks")/*+_T(":")*/),1,wxALIGN_CENTER_VERTICAL);
      m_numTicks=new wxSpinCtrl(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,0,360);
      m_numTicks->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
      m_numTicks->SetValue(meterData.numTicks);
      fSizer->Add(m_numTicks,1,wxEXPAND);
      m_numTicks->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED,wxCommandEventHandler(hmiAngularMeter::OnButtonClick),NULL,this);

      fSizer->Add(new wxStaticText(panel,wxID_ANY,_("Startangle")/*+_T(":")*/),1,wxALIGN_CENTER_VERTICAL);
      m_sAngle=new wxTextCtrl(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
      m_sAngle->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
      m_sAngle->SetValue(wxString::Format(_T("%.1f"),meterData.startAngle/10.0));
      fSizer->Add(m_sAngle,1,wxEXPAND);
      m_sAngle->Connect(wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(hmiAngularMeter::OnButtonClick),NULL,this);

      fSizer->Add(new wxStaticText(panel,wxID_ANY,_("Endangle")/*+_T(":")*/),1,wxALIGN_CENTER_VERTICAL);
      m_eAngle=new wxTextCtrl(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
      m_eAngle->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
      m_eAngle->SetValue(wxString::Format(_T("%.1f"),meterData.endAngle/10.0));
      fSizer->Add(m_eAngle,1,wxEXPAND);
      m_eAngle->Connect(wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(hmiAngularMeter::OnButtonClick),NULL,this);

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

      for (wxInt32 i=0; i<12 /*MAX_NUM_ANGULARMETER_SECTORS*/; i++)
      {
         bgText[i]=new wxStaticText(panel,wxID_ANY,_("Sector")+wxString::Format(_T(" %d:"),i+1));
         if (i<meterData.numSectors) bgText[i]->Enable(true);
         else bgText[i]->Enable(false);
         fSizer->Add(bgText[i],0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

         bgLow[i]=new wxButton(panel,wxID_ANY);
         if (i<meterData.numSectors) bgLow[i]->Enable(true);
         else bgLow[i]->Enable(false);
         bgLow[i]->SetBackgroundColour(wxColour(meterData.sectorColour[0][i]));
         fSizer->Add(bgLow[i],1,wxEXPAND);
         bgLow[i]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(hmiAngularMeter::OnButtonClick),NULL,this);

         bgHigh[i]=new wxButton(panel,wxID_ANY);
         if (i<meterData.numSectors) bgHigh[i]->Enable(true);
         else bgHigh[i]->Enable(false);
         bgHigh[i]->SetBackgroundColour(wxColour(meterData.sectorColour[1][i]));
         fSizer->Add(bgHigh[i],1,wxEXPAND);
         bgHigh[i]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(hmiAngularMeter::OnButtonClick),NULL,this);
      }

      panel->SetSizer(fSizer);
      fSizer->AddGrowableCol(1,3);
      fSizer->AddGrowableCol(2,3);

      *name=_("Sectors");
      return panel;
   }

   return NULL;
}




void hmiAngularMeter::OnButtonClick(wxCommandEvent &event)
{
   wxInt32      i;
   wxColourData colour;

   if (event.GetId()==m_numSectors->GetId())
   {
      for (i=0; i<12; i++)
      {
         if (i<m_numSectors->GetValue()) 
         {
            bgText[i]->Enable(true);
            bgLow[i]->Enable(true);
            bgHigh[i]->Enable(true);
         }
         else
         {
            bgText[i]->Enable(false);
            bgLow[i]->Enable(false);
            bgHigh[i]->Enable(false);
         }
      }
   }
   for (i=0; i<12/*MAX_NUM_ANGULARMETER_SECTORS*/; i++)
   {
      if (event.GetId()==bgLow[i]->GetId())
      {
         colour.SetColour(bgLow[i]->GetBackgroundColour());
         g_colourDialog->Create(g_mainWin,&colour);
         g_colourDialog->Centre();
         g_colourDialog->SetTitle(_("Choose the sectors colour"));
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
         g_colourDialog->SetTitle(_("Choose the sectors colour"));
         if (g_colourDialog->ShowModal() == wxID_OK)
         {
            colour=g_colourDialog->GetColourData();
            bgHigh[i]->SetBackgroundColour(colour.GetColour());
         }
         return;
      }
   }
}



void hmiAngularMeter::setConfigData()
{
   double  d;
   wxInt32 i;

   m_eAngle->GetValue().ToDouble(&d);   
   if (d<-360) d=-360;   
   else if (d>360) d=360;
   meterData.endAngle=d*10;

   m_sAngle->GetValue().ToDouble(&d);   
   if (d<-360) d=-360;   
   else if (d>360) d=360;
   meterData.startAngle=d*10;

   meterData.numSectors=m_numSectors->GetValue();
   meterData.numTicks=m_numTicks->GetValue();

   for (i=0; i<12/*MAX_NUM_ANGULARMETER_SECTORS*/; i++)
   {
      meterData.sectorColour[0][i]=bgLow[i]->GetBackgroundColour().Red() |
                                   bgLow[i]->GetBackgroundColour().Green()<<8 |
                                   bgLow[i]->GetBackgroundColour().Blue()<<16;
      meterData.sectorColour[1][i]=bgHigh[i]->GetBackgroundColour().Red() |
                                   bgHigh[i]->GetBackgroundColour().Green()<<8 |
                                   bgHigh[i]->GetBackgroundColour().Blue()<<16;
   }
}



#else



#endif
