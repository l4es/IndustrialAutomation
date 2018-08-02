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

#include <wx/spinctrl.h>

#include "globals.h"
#include "hmiObject.h"
#include "DlgDataFlowhmiHSlider.h"

BEGIN_EVENT_TABLE(DlgDataFlowhmiHSlider, DlgConfigflowBase)
  EVT_BUTTON  (wxID_ANY,DlgDataFlowhmiHSlider::OnButtonClick)
  EVT_CHECKBOX(wxID_ANY,DlgDataFlowhmiHSlider::OnCheckBoxCheck)
END_EVENT_TABLE()


DlgDataFlowhmiHSlider::DlgDataFlowhmiHSlider(hmiObject *object,wxWindow* parent,wxUint32 *cycleMicros,const wxString& title,wxUint32 noFlowFlags,bool hideISConfig)
                      :DlgConfigflowBase(object,parent,title,hideISConfig)
{
   wxPanel *rootPanel6=NULL;

   checkOnDigi1hi6=NULL; checkOnDigi1lo6=NULL;
   checkOnChanging6=NULL; checkOnChanged6=NULL;
   cycleTimeField=NULL;

   m_noFlowFlags=noFlowFlags;
   memcpy(&objectData,&object->data,sizeof(struct hmiObjectData));

   wxFlexGridSizer *bSizer=new wxFlexGridSizer(1,4,4);
   bSizer->AddGrowableCol(0,1);
   this->SetSizer(bSizer);

   if (cycleMicros)
   {
      m_cycleMicros=cycleMicros;

      wxPanel *cyclePanel=new wxPanel(this,wxID_ANY);
      bSizer->Add(cyclePanel,1,wxEXPAND);

      wxFlexGridSizer *fSizer=new wxFlexGridSizer(3,4,4);
      fSizer->AddGrowableCol(0,4);
      fSizer->AddGrowableCol(1,8);
      fSizer->AddGrowableCol(2,2);
      cyclePanel->SetSizer(fSizer);

      wxStaticText *text=new wxStaticText(cyclePanel,wxID_ANY,_("Input Poll Cycle Time")+_T(":"));
      fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

      cycleTimeField=new wxSpinCtrl(cyclePanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,25,60000,*cycleMicros/1000);
      cycleTimeField->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
      fSizer->Add(cycleTimeField,1,wxEXPAND);

      text=new wxStaticText(cyclePanel,wxID_ANY,_("msec"));
      fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   }

    if ((noFlowFlags & HMI_NOFLOWFLAG_OUT6)==0)
    {
       rootPanel6=new wxPanel(this,wxID_ANY);
       bSizer->Add(rootPanel6,1,wxEXPAND);
    }

    wxPanel *rootPanel7=new wxPanel(this,wxID_ANY);
    bSizer->Add(rootPanel7,1,wxEXPAND);

    if ((noFlowFlags & HMI_NOFLOWFLAG_OUT6)==0)
    {
       wxStaticBoxSizer *eSizer6 = new wxStaticBoxSizer (wxVERTICAL,rootPanel6,_("Send value at OUT 6 on event")+_T(":"));
       rootPanel6->SetSizer(eSizer6);

       checkOnDigi1hi6=new wxCheckBox(rootPanel6,wxID_ANY,_("On rising edge at digital IN 1"));
       eSizer6->Add(checkOnDigi1hi6,0,wxALIGN_LEFT|wxALIGN_TOP);

       checkOnDigi1lo6=new wxCheckBox(rootPanel6,wxID_ANY,_("On falling edge at digital IN 1"));
       eSizer6->Add(checkOnDigi1lo6,0,wxALIGN_LEFT|wxALIGN_TOP);

       checkOnChanging6=new wxCheckBox(rootPanel6,wxID_ANY,_("During value is changing"));
       eSizer6->Add(checkOnChanging6,0,wxALIGN_LEFT|wxALIGN_TOP);

       checkOnChanged6=new wxCheckBox(rootPanel6,wxID_ANY,_("When value has changed"));
       eSizer6->Add(checkOnChanged6,0,wxALIGN_LEFT|wxALIGN_TOP);
    }

    wxStaticBoxSizer *eSizer7 = new wxStaticBoxSizer (wxVERTICAL,rootPanel7,_("Send value at OUT 7 on event")+_T(":"));
    rootPanel7->SetSizer(eSizer7);

    checkOnDigi1hi7=new wxCheckBox(rootPanel7,wxID_ANY,_("On rising edge at digital IN 1"));
    eSizer7->Add(checkOnDigi1hi7,0,wxALIGN_LEFT|wxALIGN_TOP);

    checkOnDigi1lo7=new wxCheckBox(rootPanel7,wxID_ANY,_("On falling edge at digital IN 1"));
    eSizer7->Add(checkOnDigi1lo7,0,wxALIGN_LEFT|wxALIGN_TOP);

    checkOnChanging7=new wxCheckBox(rootPanel7,wxID_ANY,_("During value is changing"));
    eSizer7->Add(checkOnChanging7,0,wxALIGN_LEFT|wxALIGN_TOP);

    checkOnChanged7=new wxCheckBox(rootPanel7,wxID_ANY,_("When value has changed"));
    eSizer7->Add(checkOnChanged7,0,wxALIGN_LEFT|wxALIGN_TOP);

    if (noFlowFlags & HMI_NOFLOWFLAG_ON_DIGI_IN1)
    {
       if ((noFlowFlags & HMI_NOFLOWFLAG_OUT6)==0)
       {
          checkOnDigi1hi6->Enable(false);
          checkOnDigi1lo6->Enable(false);
       }
       checkOnDigi1hi7->Enable(false);
       checkOnDigi1lo7->Enable(false);
    }
    if (noFlowFlags & HMI_NOFLOWFLAG_ON_CHANGING)
    {
       if ((noFlowFlags & HMI_NOFLOWFLAG_OUT6)==0)
       {
          checkOnChanging6->Enable(false);
       }
       checkOnChanging7->Enable(false);
    }
    if (noFlowFlags & HMI_NOFLOWFLAG_ON_CHANGED)
    {
       if ((noFlowFlags & HMI_NOFLOWFLAG_OUT6)==0)
       {
          checkOnChanged6->Enable(false);
       }
       checkOnChanged7->Enable(false);
    }

   wxASSERT(m_object);
   if (m_object)
   {
      wxPanel *isPanel=new wxPanel(this,wxID_ANY);
      bSizer->Add(isPanel,1,wxEXPAND);

      wxStaticBoxSizer *fSizer = new wxStaticBoxSizer (wxVERTICAL,isPanel,_("Interlock Server")+_T(":"));
      isPanel->SetSizer(fSizer);

      addStdElements(fSizer,isPanel,true,false);
   }
   g_createDialogButtons(this,bSizer,wxOK|wxCANCEL);

   SetSize(DIALOG_X_SIZE,bSizer->Fit(this).y);
   updateUI();
}

DlgDataFlowhmiHSlider::~DlgDataFlowhmiHSlider()
{

}



void DlgDataFlowhmiHSlider::updateUI()
{
   if (objectData.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT6) checkOnDigi1hi6->SetValue(true);
   if (objectData.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT6) checkOnDigi1lo6->SetValue(true);
   if (objectData.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGING_OUT6)
   {
      objectData.dataFlowFlags|=HMI_FLOWFLAG_ON_CHANGED_OUT6;
      checkOnChanging6->SetValue(true);
      if ((m_noFlowFlags & HMI_NOFLOWFLAG_ON_CHANGED)==0)
      {
         checkOnChanged6->SetValue(true);
         checkOnChanged6->Enable(false);
      }
   }
   else if (((m_noFlowFlags & HMI_NOFLOWFLAG_ON_CHANGED)==0) &&
            ((m_noFlowFlags & HMI_NOFLOWFLAG_OUT6)==0)) checkOnChanged6->Enable(true);
   if ((objectData.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT6) &&
       ((m_noFlowFlags & HMI_NOFLOWFLAG_ON_CHANGED)==0) &&
       ((m_noFlowFlags & HMI_NOFLOWFLAG_OUT6)==0)) checkOnChanged6->SetValue(true);

   if (objectData.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT7) checkOnDigi1hi7->SetValue(true);
   if (objectData.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT7) checkOnDigi1lo7->SetValue(true);
   if (objectData.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGING_OUT7)
   {
      objectData.dataFlowFlags|=HMI_FLOWFLAG_ON_CHANGED_OUT7;
      checkOnChanging7->SetValue(true);
      if ((m_noFlowFlags & HMI_NOFLOWFLAG_ON_CHANGED)==0)
      {
         checkOnChanged7->SetValue(true);
         checkOnChanged7->Enable(false);
      }
   }
   else if ((m_noFlowFlags & HMI_NOFLOWFLAG_ON_CHANGED)==0) checkOnChanged7->Enable(true);
   if ((objectData.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT7) && ((m_noFlowFlags & HMI_NOFLOWFLAG_ON_CHANGED)==0)) checkOnChanged7->SetValue(true);
}



void DlgDataFlowhmiHSlider::OnButtonClick(wxCommandEvent &event)
{
   if (event.GetId()==wxID_OK)
   {
      wxASSERT(m_object);
      if (m_object)
      {
         if (mapOutputCB)
         {
            if (mapOutputCB->GetValue()) m_object->m_flowFlags|=FLAG_MAP_OUTPUT_TO_ISERVER;
            else m_object->m_flowFlags&=~(FLAG_MAP_OUTPUT_TO_ISERVER);
         }
         if (ioModCB)
         {
            if (ioModCB->GetValue()) m_object->m_flowFlags|=FLAG_ALLOW_MOD_FROM_ISERVER;
            else m_object->m_flowFlags&=~(FLAG_ALLOW_MOD_FROM_ISERVER);
         }
      }
      if (objectData.dataFlowFlags==0) wxMessageBox(_("Please note: when no events are defined, this element will never send out its value!"),_("Warning"),wxICON_WARNING);
      m_object->setData(&objectData);
      if (cycleTimeField) *m_cycleMicros=cycleTimeField->GetValue()*1000;
      g_isSaved=false;
      Destroy();
   }
   else if (event.GetId()==wxID_CANCEL)
   {
       Destroy();
   }
}



void DlgDataFlowhmiHSlider::OnCheckBoxCheck(wxCommandEvent &event)
{
    if ((checkOnDigi1hi6) && (event.GetId()==checkOnDigi1hi6->GetId()))
    {
        if (checkOnDigi1hi6->IsChecked()) objectData.dataFlowFlags|=HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT6;
        else objectData.dataFlowFlags&=~HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT6;
    }
    else if ((checkOnDigi1lo6) && (event.GetId()==checkOnDigi1lo6->GetId()))
    {
        if (checkOnDigi1lo6->IsChecked()) objectData.dataFlowFlags|=HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT6;
        else objectData.dataFlowFlags&=~HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT6;
    }
    else if ((checkOnChanging6) && (event.GetId()==checkOnChanging6->GetId()))
    {
        if (checkOnChanging6->IsChecked()) objectData.dataFlowFlags|=HMI_FLOWFLAG_ON_CHANGING_OUT6;
        else objectData.dataFlowFlags&=~HMI_FLOWFLAG_ON_CHANGING_OUT6;
    }
    else if ((checkOnChanged6) && (event.GetId()==checkOnChanged6->GetId()))
    {
        if (checkOnChanged6->IsChecked()) objectData.dataFlowFlags|=HMI_FLOWFLAG_ON_CHANGED_OUT6;
        else objectData.dataFlowFlags&=~HMI_FLOWFLAG_ON_CHANGED_OUT6;
    }
    else if (event.GetId()==checkOnDigi1hi7->GetId())
    {
        if (checkOnDigi1hi7->IsChecked()) objectData.dataFlowFlags|=HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT7;
        else objectData.dataFlowFlags&=~HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT7;
    }
    else if (event.GetId()==checkOnDigi1lo7->GetId())
    {
        if (checkOnDigi1lo7->IsChecked()) objectData.dataFlowFlags|=HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT7;
        else objectData.dataFlowFlags&=~HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT7;
    }
    else if (event.GetId()==checkOnChanging7->GetId())
    {
        if (checkOnChanging7->IsChecked()) objectData.dataFlowFlags|=HMI_FLOWFLAG_ON_CHANGING_OUT7;
        else objectData.dataFlowFlags&=~HMI_FLOWFLAG_ON_CHANGING_OUT7;
    }
    else if (event.GetId()==checkOnChanged7->GetId())
    {
        if (checkOnChanged7->IsChecked()) objectData.dataFlowFlags|=HMI_FLOWFLAG_ON_CHANGED_OUT7;
        else objectData.dataFlowFlags&=~HMI_FLOWFLAG_ON_CHANGED_OUT7;
    }
    updateUI();
}


