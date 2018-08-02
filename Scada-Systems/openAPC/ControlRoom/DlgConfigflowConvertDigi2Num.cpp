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

#include <wx/bookctrl.h>

#include "globals.h"
#include "hmiObject.h"
#include "flowGroup.h"
#include "DlgConfigflowConvertDigi2Num.h"
#include "flowConverterDigi2Num.h"



IMPLEMENT_CLASS(DlgConfigflowConvertDigi2Num, wxDialog)

BEGIN_EVENT_TABLE(DlgConfigflowConvertDigi2Num, wxDialog)
  EVT_BUTTON  (wxID_ANY,DlgConfigflowConvertDigi2Num::OnButtonClick)
  EVT_CHECKBOX(wxID_ANY,DlgConfigflowConvertDigi2Num::OnCheckBoxCheck)
  EVT_RADIOBUTTON(wxID_ANY,DlgConfigflowConvertDigi2Num::OnRadioButtonCheck)
END_EVENT_TABLE()


DlgConfigflowConvertDigi2Num::DlgConfigflowConvertDigi2Num(flowConverterDigi2Num *object,wxWindow* parent, const wxString& title,bool hideISConfig)
                             :DlgConfigflowName(object,parent,title,hideISConfig)
{  
   wxInt32  i,j,bit;

   m_object=object;

   for (i=1; i<CONVERTER_MAX_OUTPUTS; i++)
   {
      wxPanel *panelOUT= new wxPanel(m_book);
      m_book->AddPage(panelOUT,wxString::Format(_T("OUT%d"),i),i==1);

      wxFlexGridSizer *panelBaseSizer=new wxFlexGridSizer(1,4,4);
      panelBaseSizer->AddGrowableCol(0,1);
      panelOUT->SetSizer(panelBaseSizer);

      // ********* Mask *******************************************

      wxPanel *panelMask=new wxPanel(panelOUT,wxID_ANY);
      panelBaseSizer->Add(panelMask,1,wxEXPAND);

      wxFlexGridSizer *fSizerMask=new wxFlexGridSizer(9,4,4);
      fSizerMask->AddGrowableRow(0,2);
      panelMask->SetSizer(fSizerMask);

      wxStaticText *text=new wxStaticText(panelMask,wxID_ANY,_(""));
      fSizerMask->Add(text,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

      for (j=0; j<MAX_NUM_IOS; j++)
      {
         text=new wxStaticText(panelMask,wxID_ANY,wxString::Format(_T("IN%d"),j));
         fSizerMask->Add(text,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
      }

      text=new wxStaticText(panelMask,wxID_ANY,_("Input Mask"));
      fSizerMask->Add(text,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

      bit=1;
      for (j=0; j<MAX_NUM_IOS; j++)
      {
         m_maskBox[i][j]=new wxCheckBox(panelMask,wxID_ANY,_T(""));
         fSizerMask->Add(m_maskBox[i][j],1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
         if (bit & object->convData.outData[i].inputMask) m_maskBox[i][j]->SetValue(true);
         bit=bit<<1;
      }

      // ********* Mode ********************************************

      wxPanel *panelMode=new wxPanel(panelOUT,wxID_ANY);
      panelBaseSizer->Add(panelMode,1,wxEXPAND);

      wxGridSizer *fSizerMode=new wxGridSizer(1,4,4);
      panelMode->SetSizer(fSizerMode);

      m_modeOnClock[i]=new wxCheckBox(panelMode,wxID_ANY,_("Convert on clock at IN0"));
      if (object->convData.outData[i].flags & flowConverter::CONVERTER_FLAGS_USE_CLOCK) m_modeOnClock[i]->SetValue(1);
      fSizerMode->Add(m_modeOnClock[i],1,wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);

      m_modeBinaryRB[i]=new wxRadioButton(panelMode,wxID_ANY,_("Direct Conversion"),wxDefaultPosition,wxDefaultSize,wxRB_GROUP);
      fSizerMode->Add(m_modeBinaryRB[i],wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
      m_modeDirectRB[i]=new wxRadioButton(panelMode,wxID_ANY,_("Assignment"));
      fSizerMode->Add(m_modeDirectRB[i],wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

      if ((object->convData.outData[i].flags & flowConverter::CONVERTER_FLAGS_OUTPUT_MASK)==flowConverter::CONVERTER_FLAGS_DIRECT_MODE)
       m_modeDirectRB[i]->SetValue(1);
      else m_modeBinaryRB[i]->SetValue(1); 

      // ********* Direct Mapping **********************************

      wxPanel *panelMap=new wxPanel(panelOUT,wxID_ANY);
      panelBaseSizer->Add(panelMap,1,wxEXPAND);

      wxFlexGridSizer *fSizerMap=new wxFlexGridSizer(3,4,4);
      fSizerMap->AddGrowableCol(1,1);
      fSizerMap->AddGrowableCol(2,1);
      panelMap->SetSizer(fSizerMap);

      text=new wxStaticText(panelMap,wxID_ANY,_(""));
      fSizerMap->Add(text);
      m_lowText[i]=new wxStaticText(panelMap,wxID_ANY,_("LOW State"));
      fSizerMap->Add(m_lowText[i],1,wxEXPAND);
      m_highText[i]=new wxStaticText(panelMap,wxID_ANY,_("HIGH State"));
      fSizerMap->Add(m_highText[i],1,wxEXPAND);
      for (j=0; j<CONVERTER_MAX_INPUTS; j++)
      {
         m_ValueText[i][j]=new wxStaticText(panelMap,wxID_ANY,wxString::Format(_T("IN%d"),j));
         fSizerMap->Add(m_ValueText[i][j]);

         m_lowValueField[i][j]=new wxTextCtrl(panelMap,wxID_ANY,_(""),wxDefaultPosition,wxDefaultSize,wxTE_RIGHT);
         ((wxTextCtrl*)m_lowValueField[i][j])->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
         fSizerMap->Add(m_lowValueField[i][j],1,wxEXPAND);
         m_lowValueField[i][j]->SetValue(wxString::Format(_T("%f"),object->convData.outData[i].mOutValueLow[j]/1000.0));

         m_highValueField[i][j]=new wxTextCtrl(panelMap,wxID_ANY,_(""),wxDefaultPosition,wxDefaultSize,wxTE_RIGHT);
         ((wxTextCtrl*)m_highValueField[i][j])->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
         fSizerMap->Add(m_highValueField[i][j],1,wxEXPAND);
         m_highValueField[i][j]->SetValue(wxString::Format(_T("%f"),object->convData.outData[i].mOutValueHigh[j]/1000.0));
      }
   }
   updateUI();
   SetSize(DIALOG_X_SIZE,m_bSizer->Fit(this).y);
   returnOK=1;
}



DlgConfigflowConvertDigi2Num::~DlgConfigflowConvertDigi2Num()
{
}



void DlgConfigflowConvertDigi2Num::updateUI()
{
   wxInt32 i,j;
   bool    enable,clockUsed=false;

   for (i=1; i<MAX_NUM_IOS; i++)
   {
      if (m_modeOnClock[i]->GetValue())
      {
         clockUsed=true;
         break;
      }
   }

   for (i=1; i<MAX_NUM_IOS; i++)
   {
      m_lowText[i]->Enable(m_modeDirectRB[i]->GetValue());
      m_highText[i]->Enable(m_modeDirectRB[i]->GetValue());

      if (clockUsed) m_maskBox[i][0]->SetValue(0);
      m_maskBox[i][0]->Enable(!clockUsed);
      for (j=0; j<MAX_NUM_IOS; j++)
      {
         if ((m_modeBinaryRB[i]->GetValue()) && (j>0)) m_maskBox[i][j]->Enable(true);
         else if (/*(m_modeOnClock[i]->GetValue()) && (m_modeDirectRB[i]->GetValue()) &&*/ (j>0) && (j!=i))
         {
            if (clockUsed) m_maskBox[i][j]->SetValue(0);
            m_maskBox[i][j]->Enable(!clockUsed);
         } 
         enable=m_modeDirectRB[i]->GetValue() & m_maskBox[i][j]->GetValue();
         m_ValueText[i][j]->Enable(enable);
         m_lowValueField[i][j]->Enable(enable);
         m_highValueField[i][j]->Enable(enable);
      }
   }
}



void DlgConfigflowConvertDigi2Num::OnButtonClick(wxCommandEvent &event)
{
   if (event.GetId()==wxID_OK)
   {
      wxInt32 o,i;
      wxByte  bit;
      double  d;

      for (o=1; o<MAX_NUM_IOS; o++)
      {
         bit=1;
         m_object->convData.outData[o].inputMask=0;

         if (m_modeOnClock[o]->GetValue()) m_object->convData.outData[o].flags|=flowConverter::CONVERTER_FLAGS_USE_CLOCK;
         else m_object->convData.outData[o].flags&=~flowConverter::CONVERTER_FLAGS_USE_CLOCK;

         for (i=0; i<MAX_NUM_IOS; i++)
         {
            if (m_maskBox[o][i]->GetValue()) m_object->convData.outData[o].inputMask|=bit;
            bit=bit<<1;

            m_lowValueField[o][i]->GetValue().ToDouble(&d);
            m_object->convData.outData[o].mOutValueLow[i]=d*1000.0;
            m_highValueField[o][i]->GetValue().ToDouble(&d);
            m_object->convData.outData[o].mOutValueHigh[i]=d*1000.0;
        
         }

         m_object->convData.outData[o].flags&=~flowConverter::CONVERTER_FLAGS_OUTPUT_MASK;
         if (m_modeBinaryRB[o]->GetValue()) m_object->convData.outData[o].flags|=flowConverter::CONVERTER_FLAGS_BINARY_MODE;
         if (m_modeDirectRB[o]->GetValue()) m_object->convData.outData[o].flags|=flowConverter::CONVERTER_FLAGS_DIRECT_MODE;
      }
      handleStdElements();
      Destroy();
   }
   else if (event.GetId()==wxID_CANCEL)
   {
      Destroy();
   }
}



void DlgConfigflowConvertDigi2Num::OnCheckBoxCheck(wxCommandEvent &WXUNUSED(event))
{
   updateUI();
}



void DlgConfigflowConvertDigi2Num::OnRadioButtonCheck(wxCommandEvent &WXUNUSED(event))
{
   updateUI();
}


