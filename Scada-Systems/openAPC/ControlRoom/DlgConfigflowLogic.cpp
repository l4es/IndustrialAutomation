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
#include "DlgConfigflowLogic.h"
#include "flowLogic.h"



IMPLEMENT_CLASS(DlgConfigflowLogic, wxDialog)

BEGIN_EVENT_TABLE(DlgConfigflowLogic, wxDialog)
  EVT_BUTTON  (wxID_ANY,DlgConfigflowLogic::OnButtonClick)
  EVT_CHECKBOX(wxID_ANY,DlgConfigflowLogic::OnCheckBoxCheck)
END_EVENT_TABLE()


DlgConfigflowLogic::DlgConfigflowLogic(flowLogic *object,wxWindow* parent, const wxString& title,bool hideISConfig)
                   :DlgConfigflowName(object,parent,title,hideISConfig)
{  
   wxInt32 i,j,bit;

   for (i=0; i<MAX_NUM_IOS; i++) m_modeInvert[i]=NULL;
   m_isNumericMode=0;
   m_isMathMode=false;

   if ((object->data.type==flowObject::FLOW_TYPE_LOGI_NUMOR) || 
       (object->data.type==flowObject::FLOW_TYPE_LOGI_NUMAND) ||
       (object->data.type==flowObject::FLOW_TYPE_LOGI_NUMXOR) ||
       (object->data.type==flowObject::FLOW_TYPE_LOGI_NUMMUL) ||
       (object->data.type==flowObject::FLOW_TYPE_LOGI_NUMDIV) ||
       (object->data.type==flowObject::FLOW_TYPE_LOGI_NUMSUB) ||
       (object->data.type==flowObject::FLOW_TYPE_LOGI_NUMADD))
    m_isNumericMode=1; // WARNING: this "1" is used for calculations, so it is not only a true-flag!
   if ((object->data.type==flowObject::FLOW_TYPE_LOGI_NUMMUL) ||
       (object->data.type==flowObject::FLOW_TYPE_LOGI_NUMDIV) ||
       (object->data.type==flowObject::FLOW_TYPE_LOGI_NUMSUB) ||
       (object->data.type==flowObject::FLOW_TYPE_LOGI_NUMADD))
    m_isMathMode=true;

   m_object=object;

   wxPanel *panelOUT= new wxPanel(m_book);
   m_book->AddPage(panelOUT,_("Output"),true);

   wxFlexGridSizer *panelBaseSizer=new wxFlexGridSizer(11-m_isNumericMode,4,4);
   panelBaseSizer->AddGrowableCol(0,1);
   panelBaseSizer->AddGrowableCol(1,1);
   panelBaseSizer->AddGrowableCol(10-m_isNumericMode,1);
   panelOUT->SetSizer(panelBaseSizer);

   wxStaticText *text=new wxStaticText(panelOUT,wxID_ANY,_(""));           panelBaseSizer->Add(text,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   text=new wxStaticText(panelOUT,wxID_ANY,_("Use Clock"));  panelBaseSizer->Add(text,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
   for (j=m_isNumericMode; j<MAX_NUM_IOS; j++)
   {
      text=new wxStaticText(panelOUT,wxID_ANY,wxString::Format(_T("IN%d"),j));
      panelBaseSizer->Add(text,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
   }

   if (m_isMathMode) text=new wxStaticText(panelOUT,wxID_ANY,_("Constant"));
   else text=new wxStaticText(panelOUT,wxID_ANY,_("Invert"));
   panelBaseSizer->Add(text,1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);

   for (i=m_isNumericMode; i<MAX_NUM_IOS; i++) // outputs
   {
      text=new wxStaticText(panelOUT,wxID_ANY,wxString::Format(_T("OUT%d"),i));
      panelBaseSizer->Add(text,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

      m_modeOnClock[i]=new wxCheckBox(panelOUT,wxID_ANY,_T(""));
      panelBaseSizer->Add(m_modeOnClock[i],1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
      if (m_object->convData.outData[i].flags & flowLogic::LOGIC_FLAGS_USE_CLOCK) m_modeOnClock[i]->SetValue(1);

      bit=1+m_isNumericMode;
      for (j=m_isNumericMode; j<MAX_NUM_IOS; j++) // inputs
      {
         m_maskBox[i][j]=new wxCheckBox(panelOUT,wxID_ANY,_T(""));
         panelBaseSizer->Add(m_maskBox[i][j],1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
         if (bit & object->convData.outData[i].inputMask) m_maskBox[i][j]->SetValue(true);
         bit=bit<<1;
      }

      if (m_isMathMode)
      {
         m_constNum[i]=new oapcFloatCtrl(panelOUT,wxID_ANY);
         m_constNum[i]->SetValue(object->ConstValue[i]);
         panelBaseSizer->Add(m_constNum[i],1,wxEXPAND);
      }
      else
      {
         m_modeInvert[i]=new wxCheckBox(panelOUT,wxID_ANY,_T(""));
         panelBaseSizer->Add(m_modeInvert[i],1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
         if (m_object->convData.outData[i].flags & flowLogic::LOGIC_FLAGS_INVERT) m_modeInvert[i]->SetValue(1);
      }

   }


   text=new wxStaticText(panelOUT,wxID_ANY,wxString::Format(_("Allow Loop")));
   panelBaseSizer->Add(text,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

   text=new wxStaticText(panelOUT,wxID_ANY,wxString::Format(_T("")));
   panelBaseSizer->Add(text,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

   for (j=m_isNumericMode; j<MAX_NUM_IOS; j++) // inputs
   {
      m_modeAllowLoop[j]=new wxCheckBox(panelOUT,wxID_ANY,_T(""));
      panelBaseSizer->Add(m_modeAllowLoop[j],1,wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
      if (m_object->convData.outData[j].flags & flowLogic::LOGIC_FLAGS_ALLOW_LOOPBACK) m_modeAllowLoop[j]->SetValue(1);
   }

   updateUI();
   SetSize(DIALOG_X_SIZE,m_bSizer->Fit(this).y);
   returnOK=1;
}



DlgConfigflowLogic::~DlgConfigflowLogic()
{
}



void DlgConfigflowLogic::updateUI()
{
   if (!m_isNumericMode)
   {
     wxInt32 i;
     bool    clockUsed=false,in0Used=false;

     for (i=0; i<MAX_NUM_IOS; i++)
     {
        if (m_modeOnClock[i]->GetValue())
        {
           clockUsed=true;
           break;
        }
		else if (m_maskBox[i][0]->GetValue())
        {
           in0Used=true;
           break;
        }
     }

     for (i=0; i<MAX_NUM_IOS; i++)
     {
        if (clockUsed) m_maskBox[i][0]->SetValue(0);
        m_maskBox[i][0]->Enable(!clockUsed);
        if (in0Used) m_modeOnClock[i]->SetValue(0);
        m_modeOnClock[i]->Enable(!in0Used);
     }
   }
}



void DlgConfigflowLogic::OnButtonClick(wxCommandEvent &event)
{
   if (event.GetId()==wxID_OK)
   {
      wxInt32 o,i;
      wxByte  bit;

      for (o=m_isNumericMode; o<MAX_NUM_IOS; o++)
      {
         bit=1+m_isNumericMode;
         m_object->convData.outData[o].inputMask=0;

         if (m_modeOnClock[o]->GetValue()) m_object->convData.outData[o].flags|=flowLogic::LOGIC_FLAGS_USE_CLOCK;
         else m_object->convData.outData[o].flags&=~flowLogic::LOGIC_FLAGS_USE_CLOCK;
         if (m_modeAllowLoop[o]->GetValue()) m_object->convData.outData[o].flags|=flowLogic::LOGIC_FLAGS_ALLOW_LOOPBACK;
         else m_object->convData.outData[o].flags&=~flowLogic::LOGIC_FLAGS_ALLOW_LOOPBACK;
         if (m_modeInvert[o])
         {
            if (m_modeInvert[o]->GetValue()) m_object->convData.outData[o].flags|=flowLogic::LOGIC_FLAGS_INVERT;
            else m_object->convData.outData[o].flags&=~flowLogic::LOGIC_FLAGS_INVERT;
         }
         if (m_isMathMode) m_object->ConstValue[o]=m_constNum[o]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE);
         for (i=m_isNumericMode; i<MAX_NUM_IOS; i++)
         {
            if (m_maskBox[o][i]->GetValue()) m_object->convData.outData[o].inputMask|=bit;
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
}



void DlgConfigflowLogic::OnCheckBoxCheck(wxCommandEvent &WXUNUSED(event))
{
   updateUI();
}


