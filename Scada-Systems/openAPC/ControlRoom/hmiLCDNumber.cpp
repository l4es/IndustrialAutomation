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

#include "iff.h"
#include "globals.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiHGauge.h"
#include "hmiLCDNumber.h"
#include "kwxCtrl/LCDWindow.h"
#include "oapc_libio.h"

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif



hmiLCDNumber::hmiLCDNumber(BasePanel *parent,flowObject *obj)
             :hmiHGauge(parent,obj)
{
   this->data.type=HMI_TYPE_LCDNUMBER;
   this->data.stdIN =OAPC_DIGI_IO0|OAPC_NUM_IO7;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_NUM_IO7;
#ifdef ENV_EDITOR
   this->data.logFlags=LOG_TYPE_FLOATNUM;
   noUIFlags=OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_FONT|OAPC_HMI_NO_UI_DISABLED|OAPC_HMI_NO_UI_RO;
   m_userPriviEnable=OAPC_USERPRIVI_HIDE;
   if (obj==NULL)
   {
      data.fgcolor[0]=0x10A0FF; data.fgcolor[1]=0x10A0FF;
      data.bgcolor[0]=0x000000; data.bgcolor[1]=0x000000;
      data.msizex=104*1000;
      data.msizey=31*1000;
      lcdData.digits=6;
      lcdData.decimalPlaceDigits=2;
      num=1.25;
      lcdData.reserved1=0;
      lcdData.reserved2=0;
      lcdData.reserved3=0;
      lcdData.pad2=0;
   }
#endif
}



wxWindow *hmiLCDNumber::createUIElement()
{
   uiElement=new kwxLCDDisplay(m_parent);
   hmiObject::createUIElement();
   return uiElement;
}



hmiLCDNumber::~hmiLCDNumber()
{
}



void hmiLCDNumber::doApplyData(const wxByte all)
{
   wxInt32 gColourR,gColourG,gColourB;

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
   ((kwxLCDDisplay*)uiElement)->SetLightColour(wxColour(data.fgcolor[digi[0]]));
   gColourR=(((data.bgcolor[digi[0]] & 0x0000FF)*6.0)      +(data.fgcolor[digi[0]] & 0x0000FF))/7.0;
   gColourG=((((data.bgcolor[digi[0]] & 0x00FF00)>>8)*6.0) +((data.fgcolor[digi[0]] & 0x00FF00)>>8))/7.0;
   gColourB=((((data.bgcolor[digi[0]] & 0xFF0000)>>16)*6.0)+((data.fgcolor[digi[0]] & 0xFF0000)>>16))/7.0;
   ((kwxLCDDisplay*)uiElement)->SetGrayColour(wxColour((wxByte)gColourR,(wxByte)gColourG,(wxByte)gColourB));
   ((kwxLCDDisplay*)uiElement)->SetNumberDigits(lcdData.digits);

   wxString fmtString=wxString::Format(_T("%%.%df"),lcdData.decimalPlaceDigits);
   ((kwxLCDDisplay*)uiElement)->SetValue(wxString::Format(fmtString,num));
   uiElement->Refresh();
}



wxInt32 hmiLCDNumber::load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset)
{
   struct hmiLCDNumberData convData;
   wxUint32                myChunkSize;

   if (!FHandle) return 0;

   myChunkSize=chunkSize;
   if (myChunkSize>sizeof(struct hmiLCDNumberData)) myChunkSize=sizeof(struct hmiLCDNumberData);

   myChunkSize=FHandle->Read(&convData,myChunkSize);
   lcdData.digits=convData.digits;
   lcdData.decimalPlaceDigits=convData.decimalPlaceDigits;
   lcdData.reserved1=ntohl(convData.reserved1);
   lcdData.reserved2=ntohl(convData.reserved2);
   lcdData.reserved3=ntohl(convData.reserved3);
   lcdData.pad2     =ntohs(convData.pad2);
   return hmiObject::load(FHandle,chunkSize-myChunkSize,IDOffset)+myChunkSize;
}



#ifdef ENV_EDITOR
void hmiLCDNumber::setData(flowObject *object)
{
   memcpy(&lcdData,&((hmiLCDNumber*)object)->lcdData,sizeof(struct hmiLCDNumberData));
   flowObject::setData(object);
}



wxString hmiLCDNumber::getDefaultName()
{
    return _T("LC Numeric Display");
}



wxInt32 hmiLCDNumber::save(wxFile *FHandle)
{
   wxInt32                 length;
   struct hmiLCDNumberData convData;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_LCDN,4);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData)+sizeof(struct hmiLCDNumberData);
   length=htonl(length);
   FHandle->Write(&length,4);

   convData.digits=lcdData.digits;
   convData.decimalPlaceDigits=lcdData.decimalPlaceDigits;
   convData.reserved1=htonl(lcdData.reserved1);
   convData.reserved2=htonl(lcdData.reserved2);
   convData.reserved3=htonl(lcdData.reserved3);
   convData.pad2     =htons(lcdData.pad2);
   length=FHandle->Write(&convData,sizeof(struct hmiLCDNumberData));

   length=length+hmiObject::save(FHandle);

   return length;
}



wxPanel* hmiLCDNumber::getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num)
{
   if (num==0)
   {
      wxPanel *panel=new wxPanel(parent);

      wxFlexGridSizer *fSizer=new wxFlexGridSizer(2,4,4);

      fSizer->Add(new wxStaticText(panel,wxID_ANY,_("Digits")/*+_T(":")*/),1,wxALIGN_CENTER_VERTICAL);
      m_numDigits=new wxSpinCtrl(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,1,10);
      m_numDigits->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
      m_numDigits->SetValue(lcdData.digits);
      fSizer->Add(m_numDigits,1,wxEXPAND);
      m_numDigits->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED,wxCommandEventHandler(hmiLCDNumber::OnButtonClick),NULL,this);

      fSizer->Add(new wxStaticText(panel,wxID_ANY,_("Decimal Place Digits")/*+_T(":")*/),1,wxALIGN_CENTER_VERTICAL);
      m_numDecimalPlaceDigits=new wxSpinCtrl(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,0,9);
      m_numDecimalPlaceDigits->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
      m_numDecimalPlaceDigits->SetValue(lcdData.decimalPlaceDigits);
      fSizer->Add(m_numDecimalPlaceDigits,1,wxEXPAND);
      m_numDecimalPlaceDigits->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED,wxCommandEventHandler(hmiLCDNumber::OnButtonClick),NULL,this);

      fSizer->AddGrowableCol(1,6);
      panel->SetSizer(fSizer);

      *name=_("Configuration");
      return panel;
   }

   return NULL;
}




void hmiLCDNumber::OnButtonClick(wxCommandEvent &event)
{
   if (event.GetId()==m_numDecimalPlaceDigits->GetId())
   {
      if (m_numDecimalPlaceDigits->GetValue()>=m_numDigits->GetValue()) m_numDigits->SetValue(m_numDecimalPlaceDigits->GetValue()+1);
   }
   else if (event.GetId()==m_numDigits->GetId())
   {
      if (m_numDecimalPlaceDigits->GetValue()>=m_numDigits->GetValue()) m_numDecimalPlaceDigits->SetValue(m_numDigits->GetValue()-1);
   }
}



void hmiLCDNumber::setConfigData()
{
   lcdData.digits=m_numDigits->GetValue();
   lcdData.decimalPlaceDigits=m_numDecimalPlaceDigits->GetValue();
}
#endif
