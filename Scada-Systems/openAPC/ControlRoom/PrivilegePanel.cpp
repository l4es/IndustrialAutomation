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
#include <wx/radiobut.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/gbsizer.h>

#include "PrivilegePanel.h"
#include "hmiUserMgmntPanel.h"



BEGIN_EVENT_TABLE(PrivilegePanel, wxPanel)
   EVT_BUTTON(1,PrivilegePanel::ButtonClicked)
   EVT_BUTTON(2,PrivilegePanel::ButtonClicked)
   EVT_BUTTON(3,PrivilegePanel::ButtonClicked)
   EVT_BUTTON(4,PrivilegePanel::ButtonClicked)
END_EVENT_TABLE()


PrivilegePanel::PrivilegePanel(wxWindow* parent, wxWindowID id,wxUint32 priviFlags,wxUint32 priviFlagsAvailable)
               :wxPanel(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL,wxEmptyString)
{
   wxInt32  i,useI;

   wxGridBagSizer *fSizer=new wxGridBagSizer(4,4); 

   SetSizer(fSizer); 

   wxStaticText *text=new wxStaticText(this,wxID_ANY,_("Privilege")); 
   fSizer->Add(text,wxGBPosition(0,0)); 

   text=new wxStaticText(this,wxID_ANY,_("State of user element")); 
   fSizer->Add(text,wxGBPosition(0,1),wxGBSpan(1,3)); 

   text=new wxStaticText(this,wxID_ANY,_("Supervision")); 
   fSizer->Add(text,wxGBPosition(1,0)); 
   wxRadioButton *svRB=new wxRadioButton(this,wxID_ANY,_("enabled"));
   svRB->Enable(false);
   svRB->SetValue(true);
   fSizer->Add(svRB,wxGBPosition(1,1),wxGBSpan(1,1),wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL); 
   svRB=new wxRadioButton(this,wxID_ANY,_("disabled"));
   svRB->Enable(false);
   if (!(priviFlagsAvailable & OAPC_USERPRIVI_DISABLE)) svRB->Show(false);
   fSizer->Add(svRB,wxGBPosition(1,2),wxGBSpan(1,1),wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL); 
   svRB=new wxRadioButton(this,wxID_ANY,_("invisible"));
   svRB->Enable(false);
   if (!(priviFlagsAvailable & OAPC_USERPRIVI_HIDE)) svRB->Show(false);
   fSizer->Add(svRB,wxGBPosition(1,3),wxGBSpan(1,1),wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL); 
   svRB=new wxRadioButton(this,wxID_ANY,_("ignore"));
   svRB->Enable(false);
   fSizer->Add(svRB,wxGBPosition(1,4),wxGBSpan(1,1),wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL); 

   for (i=0; i<=MAX_PRIVI_NUM; i++)
   {
      if (i==MAX_PRIVI_NUM) text=new wxStaticText(this,wxID_ANY,_("None of them")); 
      else text=new wxStaticText(this,wxID_ANY,g_userPriviData.priviName[i]); 
      fSizer->Add(text,wxGBPosition(i+2,0)); 

      rb[0][i]=new wxRadioButton(this,wxID_ANY,_("enabled"),wxDefaultPosition,wxDefaultSize,wxRB_GROUP);
      fSizer->Add(rb[0][i],wxGBPosition(i+2,1),wxGBSpan(1,1),wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL); 
      Connect(rb[0][i]->GetId(),wxEVT_COMMAND_RADIOBUTTON_SELECTED,wxCommandEventHandler(PrivilegePanel::RadiobuttonSelected));
      Connect(rb[0][i]->GetId(),wxEVT_COMMAND_LEFT_CLICK,wxCommandEventHandler(PrivilegePanel::RadiobuttonSelected));

      rb[1][i]=new wxRadioButton(this,wxID_ANY,_("disabled"));
      fSizer->Add(rb[1][i],wxGBPosition(i+2,2),wxGBSpan(1,1),wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL); 
      Connect(rb[1][i]->GetId(),wxEVT_COMMAND_RADIOBUTTON_SELECTED,wxCommandEventHandler(PrivilegePanel::RadiobuttonSelected));
      Connect(rb[1][i]->GetId(),wxEVT_COMMAND_LEFT_CLICK,wxCommandEventHandler(PrivilegePanel::RadiobuttonSelected));
      if (!(priviFlagsAvailable & OAPC_USERPRIVI_DISABLE)) rb[1][i]->Show(false);

      rb[2][i]=new wxRadioButton(this,wxID_ANY,_("invisible"));
      fSizer->Add(rb[2][i],wxGBPosition(i+2,3),wxGBSpan(1,1),wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL); 
      Connect(rb[2][i]->GetId(),wxEVT_COMMAND_RADIOBUTTON_SELECTED,wxCommandEventHandler(PrivilegePanel::RadiobuttonSelected));
      Connect(rb[2][i]->GetId(),wxEVT_COMMAND_LEFT_CLICK,wxCommandEventHandler(PrivilegePanel::RadiobuttonSelected));
      if (!(priviFlagsAvailable & OAPC_USERPRIVI_HIDE)) rb[2][i]->Show(false);

      rb[3][i]=new wxRadioButton(this,wxID_ANY,_("ignore"));
      fSizer->Add(rb[3][i],wxGBPosition(i+2,4),wxGBSpan(1,1),wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL); 
      Connect(rb[3][i]->GetId(),wxEVT_COMMAND_RADIOBUTTON_SELECTED,wxCommandEventHandler(PrivilegePanel::RadiobuttonSelected));
      Connect(rb[3][i]->GetId(),wxEVT_COMMAND_LEFT_CLICK,wxCommandEventHandler(PrivilegePanel::RadiobuttonSelected));
      if (i==MAX_PRIVI_NUM) rb[3][i]->Enable(false);

      if (i==MAX_PRIVI_NUM) useI=0; // set the "other" privilege to index pos 0
      else useI=i+1;
      if (((g_userPriviData.privisEnabled & hmiUserMgmntPanel::priviFlagMask[useI])!=hmiUserMgmntPanel::priviFlagEnabled[useI]) && 
          (i<MAX_PRIVI_NUM))
      {
         text->Enable(false);
         rb[0][i]->Enable(false);
         rb[1][i]->Enable(false);
         rb[2][i]->Enable(false);
         rb[3][i]->Enable(false);
      }
      if ((priviFlags & hmiUserMgmntPanel::priviFlagMask[useI])==hmiUserMgmntPanel::priviFlagEnabled[useI]) rb[0][i]->SetValue(true);
      else if ((priviFlags & hmiUserMgmntPanel::priviFlagMask[useI])==hmiUserMgmntPanel::priviFlagDisabled[useI]) rb[1][i]->SetValue(true);
      else if ((priviFlags & hmiUserMgmntPanel::priviFlagMask[useI])==hmiUserMgmntPanel::priviFlagInvisible[useI]) rb[2][i]->SetValue(true);
      else if ((priviFlags & hmiUserMgmntPanel::priviFlagMask[useI])==hmiUserMgmntPanel::priviFlagIgnore[useI]) rb[3][i]->SetValue(true);
      if (i==MAX_PRIVI_NUM) break;

   }

   for (i=0; i<4; i++)
   {
      setButton[i]=new wxButton(this,i+1,_("Set all")); 
      if ((i==1) && (!(priviFlagsAvailable & OAPC_USERPRIVI_DISABLE))) setButton[i]->Show(false);
      else if ((i==2) && (!(priviFlagsAvailable & OAPC_USERPRIVI_HIDE))) setButton[i]->Show(false);
      fSizer->Add(setButton[i],wxGBPosition(MAX_PRIVI_NUM+3,i+1),wxGBSpan(1,1),wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL); 
   }

   depSelectCB=new wxCheckBox(this,wxID_ANY,_("Select visibility privilege priority dependent"));
   fSizer->Add(depSelectCB,wxGBPosition(MAX_PRIVI_NUM+4,0),wxGBSpan(1,4));

   fSizer->AddGrowableCol(1,2); 
   fSizer->AddGrowableCol(2,2); 
   fSizer->AddGrowableCol(3,2); 
   fSizer->AddGrowableCol(4,2); 

}



PrivilegePanel::~PrivilegePanel(void)
{
}



wxUint32 PrivilegePanel::getPriviFlags()
{
   wxUint32 priviFlags=0;
   wxInt32  i,useI;

   for (i=0; i<=MAX_PRIVI_NUM; i++)
   {
      if (i==MAX_PRIVI_NUM) useI=0; // set the "other" privilege to index pos 0
      else useI=i+1;
      if (rb[0][i]->GetValue()) priviFlags|=hmiUserMgmntPanel::priviFlagEnabled[useI];
      else if (rb[1][i]->GetValue()) priviFlags|=hmiUserMgmntPanel::priviFlagDisabled[useI];
      else if (rb[2][i]->GetValue()) priviFlags|=hmiUserMgmntPanel::priviFlagInvisible[useI];
      else if (rb[3][i]->GetValue()) priviFlags|=hmiUserMgmntPanel::priviFlagIgnore[useI];
      if (i==MAX_PRIVI_NUM) break;
   }
   return priviFlags;
}



void PrivilegePanel::ButtonClicked(wxCommandEvent &event)
{
   wxInt32 i;

   for (i=0; i<=MAX_PRIVI_NUM; i++)
   {
      if ((rb[event.GetId()-1][i]->IsEnabled()) && (rb[event.GetId()-1][i]->IsShown()))
       rb[event.GetId()-1][i]->SetValue(true);
   }
}


void PrivilegePanel::RadiobuttonSelected(wxCommandEvent &event)
{
   wxInt32 j,i,s;

   if (!depSelectCB->GetValue()) return;
   for (j=0; j<4; j++)
    for (i=0; i<MAX_PRIVI_NUM; i++)
     if (rb[j][i]->GetId()==event.GetId())
   {
      for (s=i+1; s<=MAX_PRIVI_NUM; s++)
       if ((rb[j][s]->IsEnabled()) && (rb[j][s]->IsShown())) rb[j][s]->SetValue(true);
   }
}
