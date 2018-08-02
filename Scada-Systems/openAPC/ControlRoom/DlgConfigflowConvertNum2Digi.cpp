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
#include "DlgConfigflowConvertNum2Digi.h"
#include "flowConverterNum2Digi.h"

IMPLEMENT_CLASS(DlgConfigflowConvertNum2Digi, wxDialog)

BEGIN_EVENT_TABLE(DlgConfigflowConvertNum2Digi, wxDialog)
  EVT_BUTTON  (wxID_ANY,DlgConfigflowConvertNum2Digi::OnButtonClick)
  EVT_CHECKBOX(wxID_ANY,DlgConfigflowConvertNum2Digi::OnCheckBoxCheck)
  EVT_RADIOBUTTON(wxID_ANY,DlgConfigflowConvertNum2Digi::OnRadioButtonCheck)
END_EVENT_TABLE()


DlgConfigflowConvertNum2Digi::DlgConfigflowConvertNum2Digi(flowConverterNum2Digi *object,wxWindow* parent, const wxString& title,bool hideISConfig)
                             :DlgConfigflowName(object,parent,title,hideISConfig)
{  
   wxInt32  i,j;
   wxUint32 bit;

   m_object=object;

   for (i=1; i<MAX_NUM_IOS; i++)
   {
      wxPanel *panelOUT= new wxPanel(m_book);
      m_book->AddPage(panelOUT,wxString::Format(_T("OUT%d"),i),i==1);

      wxFlexGridSizer *panelBaseSizer=new wxFlexGridSizer(1,4,4);
      panelBaseSizer->AddGrowableRow(1);
      panelOUT->SetSizer(panelBaseSizer);

      // ********* Mode ********************************************

      wxPanel *panelMode=new wxPanel(panelOUT,wxID_ANY);
      panelBaseSizer->Add(panelMode,1,wxEXPAND);

      wxGridSizer *fSizerMode=new wxGridSizer(1,4,4);
      panelMode->SetSizer(fSizerMode);
      m_modeOnClock[i]=new wxCheckBox(panelMode,wxID_ANY,_("Convert on clock at IN0"));
      if (object->convData.outData[i].flags & flowConverter::CONVERTER_FLAGS_USE_CLOCK) m_modeOnClock[i]->SetValue(1);
      fSizerMode->Add(m_modeOnClock[i],1,wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);

      // ********* Direct Mapping **********************************

      wxPanel *panelMap=new wxPanel(panelOUT,wxID_ANY);
      panelBaseSizer->Add(panelMap,1,wxEXPAND);

      wxFlexGridSizer *fSizerMap=new wxFlexGridSizer(7,4,4);
      fSizerMap->AddGrowableCol(0,2);
      panelMap->SetSizer(fSizerMap);

      wxStaticText *text=new wxStaticText(panelMap,wxID_ANY,_(""));      fSizerMap->Add(text);
      text=new wxStaticText(panelMap,wxID_ANY,_(""));      fSizerMap->Add(text);
      m_inText[i]=new wxStaticText(panelMap,wxID_ANY,wxString::Format(_("Input Number"),i));
      fSizerMap->Add(m_inText[i],1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
      m_outText1[i]=new wxStaticText(panelMap,wxID_ANY,_("Level"));      fSizerMap->Add(m_outText1[i],1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
      m_outText2[i]=new wxStaticText(panelMap,wxID_ANY,_("Level"));      fSizerMap->Add(m_outText2[i],1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
      m_outText3[i]=new wxStaticText(panelMap,wxID_ANY,_("Pulse"));      fSizerMap->Add(m_outText3[i],1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
      m_outText4[i]=new wxStaticText(panelMap,wxID_ANY,_("Pulse"));      fSizerMap->Add(m_outText4[i],1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
      bit=1;
      for (j=0; j<CONVERTER_MAX_ASSIGNMENTS; j++)
      {
         m_enableBox[i][j]=new wxCheckBox(panelMap,wxID_ANY,wxString::Format(_T("IN%d"),i));
         fSizerMap->Add(m_enableBox[i][j],1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
         if ((object->convData.outData[i].enablePattern & bit)==bit) m_enableBox[i][j]->SetValue(1);

         m_cmpButton[i][j]=new wxButton(panelMap,wxID_ANY,wxEmptyString,wxDefaultPosition,CMP_BUTTON_SIZE);
         fSizerMap->Add(m_cmpButton[i][j],1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

         m_inNum[i][j]=new wxTextCtrl(panelMap,wxID_ANY);
         fSizerMap->Add(m_inNum[i][j],1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
         m_inNum[i][j]->SetValue(wxString::Format(_T("%f"),object->convData.outData[i].mInNum[j]/1000.0));

         bit=bit<<1;

         m_outLow[i][j]=new wxRadioButton(panelMap,wxID_ANY,_T("0"),wxDefaultPosition,wxDefaultSize,wxRB_GROUP);
         fSizerMap->Add(m_outLow[i][j],1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);            
         if ((object->convData.outData[i].outOperation[j] & flowConverter::CONVERTER_FLAGS_OUTPUT_MASK)==flowConverter::CONVERTER_FLAGS_OUTPUT_LOW) m_outLow[i][j]->SetValue(1);

         m_outHigh[i][j]=new wxRadioButton(panelMap,wxID_ANY,_T("1"));
         fSizerMap->Add(m_outHigh[i][j],1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);            
         if ((object->convData.outData[i].outOperation[j] & flowConverter::CONVERTER_FLAGS_OUTPUT_MASK)==flowConverter::CONVERTER_FLAGS_OUTPUT_HIGH) m_outHigh[i][j]->SetValue(1);

         m_outPulseLoHi[i][j]=new wxRadioButton(panelMap,wxID_ANY,_T("0/1"));
         fSizerMap->Add(m_outPulseLoHi[i][j],1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);            
         if ((object->convData.outData[i].outOperation[j] & flowConverter::CONVERTER_FLAGS_OUTPUT_MASK)==flowConverter::CONVERTER_FLAGS_OUTPUT_PULSE_LOHI) m_outPulseLoHi[i][j]->SetValue(1);

         m_outPulseHiLo[i][j]=new wxRadioButton(panelMap,wxID_ANY,_T("1/0"));
         fSizerMap->Add(m_outPulseHiLo[i][j],1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);            
         if ((object->convData.outData[i].outOperation[j] & flowConverter::CONVERTER_FLAGS_OUTPUT_MASK)==flowConverter::CONVERTER_FLAGS_OUTPUT_PULSE_HILO) m_outPulseHiLo[i][j]->SetValue(1);
      }
   }
   updateUI();
   SetSize(DIALOG_X_SIZE,m_bSizer->Fit(this).y);
   returnOK=1;
}



DlgConfigflowConvertNum2Digi::~DlgConfigflowConvertNum2Digi()
{
}



void DlgConfigflowConvertNum2Digi::updateUI()
{
   wxInt32 i,j;
   bool    enable=true;

   for (i=1; i<MAX_NUM_IOS; i++)
   {
      m_inText[i]->Enable(enable);
      m_outText1[i]->Enable(enable);
      m_outText2[i]->Enable(enable);
      m_outText3[i]->Enable(enable);
      m_outText4[i]->Enable(enable);
      for (j=0; j<CONVERTER_MAX_ASSIGNMENTS; j++)
      {
         if (m_object->convData.outData[i].compareFlag[j]==flowConverter::CONVERTER_FLAGS_CMP_EQUAL) m_cmpButton[i][j]->SetLabel(_T("=="));
         else if (m_object->convData.outData[i].compareFlag[j]==flowConverter::CONVERTER_FLAGS_CMP_NOTEQUAL) m_cmpButton[i][j]->SetLabel(_T("!="));
         else if (m_object->convData.outData[i].compareFlag[j]==flowConverter::CONVERTER_FLAGS_CMP_GREATER) m_cmpButton[i][j]->SetLabel(_T(">"));
         else if (m_object->convData.outData[i].compareFlag[j]==flowConverter::CONVERTER_FLAGS_CMP_SMALLER) m_cmpButton[i][j]->SetLabel(_T("<"));
         m_cmpButton[i][j]->Enable(enable & m_enableBox[i][j]->GetValue());
         m_enableBox[i][j]->Enable(enable);
         m_inNum[i][j]->Enable(enable & m_enableBox[i][j]->GetValue());
         m_outLow[i][j]->Enable(enable & m_enableBox[i][j]->GetValue());
         m_outHigh[i][j]->Enable(enable & m_enableBox[i][j]->GetValue());
         m_outPulseLoHi[i][j]->Enable(enable & m_enableBox[i][j]->GetValue());
         m_outPulseHiLo[i][j]->Enable(enable & m_enableBox[i][j]->GetValue());
      }
   }
}



void DlgConfigflowConvertNum2Digi::OnButtonClick(wxCommandEvent &event)
{
   if (event.GetId()==wxID_OK)
   {
      wxInt32   o,i;
      wxUint32  bit;
      double    d;

      for (o=1; o<CONVERTER_MAX_OUTPUTS; o++)
      {
         bit=1;

         if (m_modeOnClock[o]->GetValue()) m_object->convData.outData[o].flags|=flowConverter::CONVERTER_FLAGS_USE_CLOCK;
         else m_object->convData.outData[o].flags&=~flowConverter::CONVERTER_FLAGS_USE_CLOCK;

         for (i=0; i<CONVERTER_MAX_ASSIGNMENTS; i++)
         {
            m_object->convData.outData[o].outOperation[i]&=~flowConverter::CONVERTER_FLAGS_OUTPUT_MASK;
            if (m_outLow[o][i]->GetValue()) m_object->convData.outData[o].outOperation[i]=flowConverter::CONVERTER_FLAGS_OUTPUT_LOW;
            if (m_outHigh[o][i]->GetValue()) m_object->convData.outData[o].outOperation[i]=flowConverter::CONVERTER_FLAGS_OUTPUT_HIGH;
            if (m_outPulseLoHi[o][i]->GetValue()) m_object->convData.outData[o].outOperation[i]=flowConverter::CONVERTER_FLAGS_OUTPUT_PULSE_LOHI;
            if (m_outPulseHiLo[o][i]->GetValue()) m_object->convData.outData[o].outOperation[i]=flowConverter::CONVERTER_FLAGS_OUTPUT_PULSE_HILO;

            m_inNum[o][i]->GetValue().ToDouble(&d);
            m_object->convData.outData[o].mInNum[i]=d*1000;
            if (m_enableBox[o][i]->GetValue()) m_object->convData.outData[o].enablePattern|=bit;
            else m_object->convData.outData[o].enablePattern&=~bit;
            bit=bit<<1;
         }
      }
      handleStdElements();
      Destroy();
   }
   else if (event.GetId()==wxID_CANCEL)
   {
      Destroy();
   }
   else
   {
      wxInt32   o,a;

      for (o=1; o<MAX_NUM_IOS; o++)
      {
         for (a=0; a<CONVERTER_MAX_ASSIGNMENTS; a++)
         {
            if (m_cmpButton[o][a]->GetId()==event.GetId())
            {
               m_object->convData.outData[o].compareFlag[a]++;
               if (m_object->convData.outData[o].compareFlag[a]>flowConverter::CONVERTER_FLAGS_CMP_SMALLER)
                m_object->convData.outData[o].compareFlag[a]=1;
               updateUI();
               return;
            }
         }
      }
   }
}



void DlgConfigflowConvertNum2Digi::OnCheckBoxCheck(wxCommandEvent &WXUNUSED(event))
{
   updateUI();
}



void DlgConfigflowConvertNum2Digi::OnRadioButtonCheck(wxCommandEvent &WXUNUSED(event))
{
   updateUI();
}


