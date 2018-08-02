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
#include <wx/artprov.h>
#include <wx/gbsizer.h>

#include "DlgUserpriviSettings.h"
#include "hmiUserMgmntPanel.h"
#include "UserList.h"

BEGIN_EVENT_TABLE(DlgUserpriviSettings, wxDialog)
   EVT_BUTTON(wxID_ANY, DlgUserpriviSettings::OnButtonClick)
   EVT_CHECKBOX(wxID_ANY,DlgUserpriviSettings::OnCheckboxCheck)
END_EVENT_TABLE()



DlgUserpriviSettings::DlgUserpriviSettings(wxWindow* parent,const wxString & title,struct userprivi_data *userPriviData)
       : wxDialog(parent,wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
   wxInt32  i;
   wxString s;

   m_userPriviData=*userPriviData;
   wxBoxSizer *bSizer=new wxBoxSizer(wxVERTICAL);
   this->SetSizer(bSizer);

   wxPanel *rootPanel=new wxPanel(this);

   wxStaticBox *group=new wxStaticBox (this, -1, _("User Privileges"));
   wxStaticBoxSizer *gSizer=new wxStaticBoxSizer (group, wxVERTICAL);
   gSizer->Add(rootPanel,1,wxEXPAND);
   bSizer->Add(gSizer,1,wxEXPAND);

   g_createDialogButtons(this,bSizer,wxOK|wxCANCEL);

   wxGridBagSizer *fSizer=new wxGridBagSizer(4,4);

   rootPanel->SetSizer(fSizer);

   enableGlobalCB=new wxCheckBox(rootPanel,wxID_ANY,_("Enable User Privilege Management"));
   enableGlobalCB->SetValue(m_userPriviData.enabled);
   fSizer->Add(enableGlobalCB,wxGBPosition(0,0),wxGBSpan(1,4));

   title1=new wxStaticText(rootPanel,wxID_ANY,_("User Privilege"));
   fSizer->Add(title1,wxGBPosition(1,1),wxGBSpan(1,1));

   title2=new wxStaticText(rootPanel,wxID_ANY,_("Priority"));
   fSizer->Add(title2,wxGBPosition(1,2),wxGBSpan(1,2),wxALIGN_CENTER_HORIZONTAL);

   wxCheckBox *cb=new wxCheckBox(rootPanel,wxID_ANY,wxEmptyString);
   cb->Enable(false);
   cb->SetValue(true);
   fSizer->Add(cb,wxGBPosition(2,0),wxGBSpan(1,1),wxALIGN_CENTER);

   wxTextCtrl *txtField=new wxTextCtrl(rootPanel,wxID_ANY,_("Supervision"));
   fSizer->Add(txtField,wxGBPosition(2,1),wxGBSpan(1,1),wxEXPAND);
   txtField->Enable(false);

   wxBitmapButton *bmButton=new wxBitmapButton(rootPanel,wxID_ANY,wxArtProvider::GetBitmap(wxART_GO_DOWN));
   fSizer->Add(bmButton,wxGBPosition(2,3),wxGBSpan(1,1),wxALIGN_LEFT);
   bmButton->Enable(false);

   for (i=0; i<MAX_PRIVI_NUM; i++)
   {
      enableCB[i]=new wxCheckBox(rootPanel,wxID_ANY,wxEmptyString);

      fSizer->Add(enableCB[i],wxGBPosition(3+i,0),wxGBSpan(1,1),wxALIGN_CENTER);
      nameField[i]=new wxTextCtrl(rootPanel,wxID_ANY,_(""));
      nameField[i]->SetValue(m_userPriviData.priviName[i]);
      fSizer->Add(nameField[i],wxGBPosition(3+i,1),wxGBSpan(1,1),wxEXPAND);
      nameField[i]->SetSize(200,nameField[i]->GetSize().y);
    
      upBtn[i]=new wxBitmapButton(rootPanel,wxID_ANY,wxArtProvider::GetBitmap(wxART_GO_UP));
      upBtn[i]->Enable(false);
      fSizer->Add(upBtn[i],wxGBPosition(3+i,2),wxGBSpan(1,1),wxALIGN_RIGHT);

      if (i<MAX_PRIVI_NUM-1)
      {
         downBtn[i]=new wxBitmapButton(rootPanel,wxID_ANY,wxArtProvider::GetBitmap(wxART_GO_DOWN));
         fSizer->Add(downBtn[i],wxGBPosition(3+i,3),wxGBSpan(1,1),wxALIGN_LEFT);
      }
      else downBtn[i]=NULL;
   }
   fSizer->AddGrowableCol(1,12);
   fSizer->AddGrowableCol(2,1);
   fSizer->AddGrowableCol(3,1);

   updateUI();
   SetSize(DIALOG_X_SIZE,bSizer->Fit(this).y);
   Center();
}




DlgUserpriviSettings::~DlgUserpriviSettings()
{
}



void DlgUserpriviSettings::updateUI()
{
   wxInt32  i;
   bool     enabled=false,gEnabled=false;

   gEnabled=enableGlobalCB->GetValue();
   for (i=0; i<MAX_PRIVI_NUM; i++)
   {
      enableCB[i]->Enable(gEnabled);
      if ((i>0) || (!gEnabled)) upBtn[i]->Enable(gEnabled);
      title1->Enable(gEnabled);
      title2->Enable(gEnabled);
      if (downBtn[i]) downBtn[i]->Enable(gEnabled);
      if (!gEnabled) nameField[i]->Enable(false);
   }

   for (i=0; i<MAX_PRIVI_NUM; i++)
   {
      if ((m_userPriviData.privisEnabled & hmiUserMgmntPanel::priviFlagMask[i+1])==hmiUserMgmntPanel::priviFlagEnabled[i+1])
       enabled=true;
      else enabled=false;
      nameField[i]->Enable(enabled & gEnabled);
      enableCB[i]->SetValue(enabled);
   }
}



wxUint32 DlgUserpriviSettings::exchangeUserPriviFlags(wxUint32 privisEnabled,wxInt32 privi1,wxInt32 privi2)
{
   bool enabled;

   enabled=((privisEnabled & hmiUserMgmntPanel::priviFlagMask[privi1])==hmiUserMgmntPanel::priviFlagEnabled[privi1]); // store if privi 1 is set
   privisEnabled&=~hmiUserMgmntPanel::priviFlagMask[privi1];                                                          // clear privi 1 flags
   if ((privisEnabled & hmiUserMgmntPanel::priviFlagMask[privi2])==hmiUserMgmntPanel::priviFlagEnabled[privi2])      // set privi 1 flags when privi 2 is set
    privisEnabled|=hmiUserMgmntPanel::priviFlagEnabled[privi1];
   privisEnabled&=~hmiUserMgmntPanel::priviFlagMask[privi2];                                                          // clear privi 2 flags
   if (enabled)
    privisEnabled|=hmiUserMgmntPanel::priviFlagEnabled[privi2];                                                       // set privi 2 flags when privi 1 have been set

   return privisEnabled;
}



void DlgUserpriviSettings::exchangePrivis(wxInt32 privi1,wxInt32 privi2)
{
   wxString    name;
   flowObject *object;
   wxNode     *node;
   UserData   *userData;

   name=nameField[privi1]->GetValue();
   nameField[privi1]->SetValue(nameField[privi2]->GetValue());
   nameField[privi2]->SetValue(name);

   name=m_userPriviData.priviName[privi1];
   m_userPriviData.priviName[privi1]=m_userPriviData.priviName[privi2];
   m_userPriviData.priviName[privi2]=name;

   m_userPriviData.privisEnabled=exchangeUserPriviFlags(m_userPriviData.privisEnabled,privi1+1,privi2+1);

   node=g_objectList.getObject((wxNode*)NULL);
   while (node)
   {
      object=(flowObject*)node->GetData();
      object->data.userPriviFlags=exchangeUserPriviFlags(object->data.userPriviFlags,privi1+1,privi2+1);
      node=g_objectList.getObject(node);
   }

   if (!g_userList) g_userList=new UserList();
   userData=g_userList->getUserData(true);
   while (userData)
   {
      userData->m_canDo=exchangeUserPriviFlags(userData->m_canDo,privi1+1,privi2+1);
      userData=g_userList->getUserData(false);
   }
   updateUI();
}



void DlgUserpriviSettings::OnCheckboxCheck(wxCommandEvent &event)
{
   wxInt32 i;

   if (event.GetId()==enableGlobalCB->GetId())
   {
      updateUI();
      return;
   }
   for (i=0; i<MAX_PRIVI_NUM; i++)
   {
      if (event.GetId()==enableCB[i]->GetId())
      {
         m_userPriviData.privisEnabled&=~hmiUserMgmntPanel::priviFlagMask[i+1];;
         if (enableCB[i]->GetValue()) m_userPriviData.privisEnabled|=hmiUserMgmntPanel::priviFlagEnabled[i+1];
         updateUI();
         break;
      }
   }
}



void DlgUserpriviSettings::OnButtonClick(wxCommandEvent &event)
{
   wxInt32 i;

   for (i=0; i<MAX_PRIVI_NUM; i++)
   {
      if ((upBtn[i]) && (upBtn[i]->GetId()==event.GetId()))
      {
         exchangePrivis(i,i-1);
         return;
      }
      else if ((downBtn[i]) && (downBtn[i]->GetId()==event.GetId()))
      {
         exchangePrivis(i,i+1);
         return;
      }
   }
   if (event.GetId()==wxID_OK)
   {
      for (i=0; i<MAX_PRIVI_NUM; i++) m_userPriviData.priviName[i]=nameField[i]->GetValue();

      g_userPriviData=m_userPriviData;
      g_userPriviData.enabled=enableGlobalCB->GetValue();

      if (g_userPriviData.enabled)
      {
         if (!g_userList) g_userList=new UserList();
         g_userMgmntButton->Enable(true);
      }
      else
      {
         if (g_userList) delete g_userList;
         g_userList=NULL;
         g_userMgmntButton->Enable(false);
      }
      Destroy();
   }
   else if (event.GetId()==wxID_CANCEL)
   {
       Destroy();
   }
}


