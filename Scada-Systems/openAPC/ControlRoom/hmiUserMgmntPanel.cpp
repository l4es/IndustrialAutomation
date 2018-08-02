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
#include <wx/statline.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/filename.h>

#include "common/oapcResourceBundle.h"

#ifdef ENV_BEAM
 #include "../CNConstruct/iff.h"
 #include "../CNConstruct/cncoProject.h"
#endif
#include "hmiUserMgmntPanel.h"
#include "ChangePwdDlg.h"
#include "UserList.h"
#include "UserData.h"
#include "iff.h"

const wxUint32 hmiUserMgmntPanel::priviFlagEnabled[MAX_PRIVI_NUM+1]=  {0x00000002,0x00000008,0x00000020,0x00000080,0x00000200,0x00000800,0x00002000,0x00008000,
                                                                       0x00020000,0x00080000,0x00200000,0x00800000,0x02000000,0x08000000,0x20000000,0x80000000};                                           
const wxUint32 hmiUserMgmntPanel::priviFlagDisabled[MAX_PRIVI_NUM+1]= {0x00000001,0x00000004,0x00000010,0x00000040,0x00000100,0x00000400,0x00001000,0x00004000,
                                                                       0x00010000,0x00040000,0x00100000,0x00400000,0x01000000,0x04000000,0x10000000,0x40000000};                                           
const wxUint32 hmiUserMgmntPanel::priviFlagInvisible[MAX_PRIVI_NUM+1]={0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
                                                                       0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000};
const wxUint32 hmiUserMgmntPanel::priviFlagIgnore[MAX_PRIVI_NUM+1]=   {0x00000003,0x0000000C,0x00000030,0x000000C0,0x00000300,0x00000C00,0x00003000,0x0000C000,
                                                                       0x00030000,0x000C0000,0x00300000,0x00C00000,0x03000000,0x0C000000,0x30000000,0xC0000000};
const wxUint32 hmiUserMgmntPanel::priviFlagMask[MAX_PRIVI_NUM+1]=     {0x00000003,0x0000000C,0x00000030,0x000000C0,0x00000300,0x00000C00,0x00003000,0x0000C000,
                                                                       0x00030000,0x000C0000,0x00300000,0x00C00000,0x03000000,0x0C000000,0x30000000,0xC0000000};



BEGIN_EVENT_TABLE(hmiUserMgmntPanel, wxPanel)
   EVT_LISTBOX(wxID_ANY,hmiUserMgmntPanel::OnListBoxSelected)
   EVT_BUTTON(wxID_ANY,hmiUserMgmntPanel::OnButtonClicked)
   EVT_CHECKBOX(wxID_ANY,hmiUserMgmntPanel::OnCheckBoxChecked)
   EVT_TEXT(wxID_ANY,hmiUserMgmntPanel::OnCheckBoxChecked)
   EVT_COMBOBOX(wxID_ANY,hmiUserMgmntPanel::OnCheckBoxChecked)
END_EVENT_TABLE()


hmiUserMgmntPanel::hmiUserMgmntPanel(wxWindow* parent)
                  :wxPanel(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL,wxEmptyString)
{
   wxInt32  i,cnt;
   UserData *userData;

   m_userData=NULL;
   m_wasChanged=false;
   wxGridBagSizer *fSizer=new wxGridBagSizer(4,4); 

   SetSizer(fSizer); 

   m_userText=new wxStaticText(this,wxID_ANY,_("User")); 
   fSizer->Add(m_userText,wxGBPosition(0,0)); 
   m_privilegeText=new wxStaticText(this,wxID_ANY,_("Privilege")); 
   fSizer->Add(m_privilegeText,wxGBPosition(0,2),wxGBSpan(1,2)); 


   priviCB[0]=new wxCheckBox(this,wxID_ANY,_("Supervision"));
   fSizer->Add(priviCB[0],wxGBPosition(1,2)); 

   cnt=0;
   for (i=0; i<MAX_PRIVI_NUM; i++)
   {
      if ((g_userPriviData.privisEnabled & priviFlagMask[i+1])==priviFlagEnabled[i+1])
      {
         priviCB[i+1]=new wxCheckBox(this,wxID_ANY,g_userPriviData.priviName[i]);
         fSizer->Add(priviCB[i+1],wxGBPosition(1+((cnt+1)/2),2+((cnt+1)%2)),wxGBSpan(1,1)); 
         cnt++;
      }
      else priviCB[i+1]=NULL;
   }

   cnt/=2;
   fSizer->AddGrowableRow(cnt+1,1);
   m_userList=new wxListBox(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,0,NULL,wxLB_SINGLE);
   fSizer->Add(m_userList,wxGBPosition(1,0),wxGBSpan(cnt+2,2),wxEXPAND); 

   userData=g_userList->getUserData(true);
   while (userData)
   {
      m_userList->Append(userData->m_login);
      userData=g_userList->getUserData(false);
   }

   /*** user data panel ***************************************************************************/
   m_userBox=new wxStaticBox (this, -1,_("User Data"));
   wxStaticBoxSizer *userBoxSizer=new wxStaticBoxSizer(m_userBox,wxHORIZONTAL); // (4,4);
   wxPanel *userPanel=new wxPanel(this);
   userBoxSizer->Add(userPanel,1,wxEXPAND);
   wxGridBagSizer *userPanelSizer=new wxGridBagSizer(4,4);
   userPanel->SetSizer(userPanelSizer);

   m_loginText=new wxStaticText(userPanel,wxID_ANY,_("Login name"));
   userPanelSizer->Add(m_loginText,wxGBPosition(0,0),wxGBSpan(1,1),wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);
   m_loginField=new oapcTextCtrl(userPanel,wxID_ANY);
   userPanelSizer->Add(m_loginField,wxGBPosition(0,1),wxGBSpan(1,1),wxEXPAND);

   m_nameText=new wxStaticText(userPanel,wxID_ANY,_("Full name"));
   userPanelSizer->Add(m_nameText,wxGBPosition(1,0),wxGBSpan(1,1),wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);
   m_nameField=new oapcTextCtrl(userPanel,wxID_ANY);
   userPanelSizer->Add(m_nameField,wxGBPosition(1,1),wxGBSpan(1,1),wxEXPAND);

   m_stateText=new wxStaticText(userPanel,wxID_ANY,_("State"));
   userPanelSizer->Add(m_stateText,wxGBPosition(2,0),wxGBSpan(1,1),wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);
   m_stateCombo=new wxComboBox(userPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_READONLY);
   m_stateCombo->Append(_("Enabled"));
   m_stateCombo->Append(_("Disabled"));
   userPanelSizer->Add(m_stateCombo,wxGBPosition(2,1),wxGBSpan(1,1),wxEXPAND);

   m_commentText=new wxStaticText(userPanel,wxID_ANY,_("Comment"));
   userPanelSizer->Add(m_commentText,wxGBPosition(3,0),wxGBSpan(1,1),wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);
   m_commentField=new oapcTextCtrl(userPanel,wxID_ANY);
   userPanelSizer->Add(m_commentField,wxGBPosition(3,1),wxGBSpan(1,1),wxEXPAND);

   m_applyUserData=new wxButton(userPanel,wxID_ANY,_("Apply"));
   userPanelSizer->Add(m_applyUserData,wxGBPosition(4,0),wxGBSpan(1,2),wxALIGN_CENTER_HORIZONTAL);

   userPanelSizer->AddGrowableCol(1,1);

   fSizer->Add(userBoxSizer,wxGBPosition(2+cnt,2),wxGBSpan(1,2),wxEXPAND); 
   /*** end of user data panel *********************************************************************/

   m_addUser=new wxButton(this,wxID_ANY,_("Add new user"));
   fSizer->Add(m_addUser,wxGBPosition(3+cnt,0),wxGBSpan(1,1),wxEXPAND); 
   m_delUser=new wxButton(this,wxID_ANY,_("Delete user"));
   fSizer->Add(m_delUser,wxGBPosition(3+cnt,1),wxGBSpan(1,1),wxEXPAND); 
   m_setUserPwd=new wxButton(this,wxID_ANY,_("Set password"));
   fSizer->Add(m_setUserPwd,wxGBPosition(3+cnt,2),wxGBSpan(1,2),wxALIGN_CENTER_HORIZONTAL); 

   fSizer->AddGrowableCol(0,7);
   fSizer->AddGrowableCol(1,7);
   fSizer->AddGrowableCol(2,8);
   fSizer->AddGrowableCol(3,8);

   updateUI();
}



hmiUserMgmntPanel::~hmiUserMgmntPanel(void)
{
}


bool hmiUserMgmntPanel::Enable(bool enable)
{
   m_userText->Enable(enable);   m_privilegeText->Enable(enable);

   return wxPanel::Enable(enable);
}


void hmiUserMgmntPanel::OnListBoxSelected(wxCommandEvent &WXUNUSED(event))
{
   wxInt32 i;

   if ((m_userList->GetSelection()>-1) && (g_userPriviData.enabled))
    m_userData=g_userList->getUserDataByLogin(m_userList->GetStringSelection());
   for (i=0; i<=MAX_PRIVI_NUM; i++)
   {
      if (priviCB[i])
      {
         if (m_userData)
         {
            if (m_userData->m_canDo & hmiUserMgmntPanel::priviFlagEnabled[i]) priviCB[i]->SetValue(true);
            else priviCB[i]->SetValue(false);
         }
      }
   }
   if (!m_userData) return;
   m_loginField->ChangeValue(m_userData->m_login);
   m_nameField->ChangeValue(m_userData->m_fullname);
   m_commentField->ChangeValue(m_userData->m_comment);
   m_stateCombo->SetSelection(m_userData->m_state);
   m_wasChanged=false;
   updateUI();
}



void hmiUserMgmntPanel::OnCheckBoxChecked(wxCommandEvent &WXUNUSED(event))
{
   m_wasChanged=true;
   updateUI();
}



void hmiUserMgmntPanel::OnButtonClicked(wxCommandEvent &event)
{
   if (event.GetId()==m_delUser->GetId())
   {
      if (wxMessageBox(_("Really delete this user?"),_("Question"),wxYES_NO,this,wxICON_QUESTION)==wxYES)
      {
#ifdef ENV_EDITOR
         g_isSaved=false;
#endif
         m_userList->Delete(m_userList->GetSelection());
         g_userList->deleteUser(m_userData);
         delete m_userData;
         m_userData=NULL;
         m_wasChanged=false;
      }
   }
   else if (event.GetId()==m_addUser->GetId())
   {
      wxString newName=wxGetTextFromUser(_("Login name for the new user:"),_("Create new user"),wxEmptyString,this);
      if (newName.Length()>0)
      {
         wxInt32 i;
#ifdef ENV_EDITOR
         g_isSaved=false;
#endif
         if (m_userList->FindString(newName)!=wxNOT_FOUND)
         {
            wxMessageBox(_("A user with this login name already exists!"),_("Error"),wxOK,this,wxICON_ERROR);
            return;
         }
         m_userList->Append(newName);
         m_userList->Select(m_userList->FindString(newName));
         m_userData=new UserData(newName,wxEmptyString,wxEmptyString,0);
         m_userData->m_pwd=newName;
         g_userList->addUserData(m_userData);
         m_loginField->ChangeValue(newName);
         m_nameField->ChangeValue(wxEmptyString);
         m_commentField->ChangeValue(wxEmptyString);
         m_stateCombo->SetSelection(0);
         for (i=0; i<=MAX_PRIVI_NUM; i++) if (priviCB[i])
         {
            priviCB[i]->SetValue(false);
         }
      }
   }
   else if (event.GetId()==m_applyUserData->GetId())
   {
      wxUint32 i,sel;

#ifdef ENV_EDITOR
      g_isSaved=false;
#endif
      m_userData->m_comment=m_commentField->GetValue();
      m_userData->m_fullname=m_nameField->GetValue();
      m_userData->m_login=m_loginField->GetValue();
      sel=m_userList->GetSelection();
      m_userList->SetString(sel,m_userData->m_login);
      m_userData->m_state=m_stateCombo->GetSelection();
      m_userData->m_canDo=0;

      for (i=0; i<=MAX_PRIVI_NUM; i++)
      {
         if (priviCB[i])
         {
            if (priviCB[i]->GetValue())
            {
               m_userData->m_canDo|=priviFlagEnabled[i];
            }
         }
      }
      m_wasChanged=false;
#ifdef ENV_PLAYER
#ifndef ENV_BEAM
      saveUserData(this);
#endif //ENV_BEAM
#endif //ENV_PLAYER
   }
   else if (event.GetId()==m_setUserPwd->GetId())
   {
      ChangePwdDlg dlg(this,_("Change Password"),m_userData);

      dlg.ShowModal();
#ifdef ENV_PLAYER
#ifndef ENV_BEAM
      saveUserData(this);
#endif //ENV_BEAM
#endif //ENV_PLAYER
   }
   updateUI();
}


#ifdef ENV_BEAM
void hmiUserMgmntPanel::deleteUserData()
{
   wxString sTmp;

   sTmp=wxStandardPaths::Get().GetUserDataDir()+wxFileName::GetPathSeparator()+_T("beamhome.rtdat");
   wxRemoveFile(sTmp);
}
#endif // ENV_BEAM


#ifdef ENV_PLAYER
// TODO: move this into g_objectList completely
void hmiUserMgmntPanel::saveUserData(wxWindow *parent)
{
   wxFile  *rtHandle;
   wxUint32 l;
   wxString sTmp;

#ifdef ENV_BEAM
   if (!wxFileName::DirExists(wxStandardPaths::Get().GetUserDataDir()))
    wxFileName::Mkdir(wxStandardPaths::Get().GetUserDataDir());
   sTmp=wxStandardPaths::Get().GetUserDataDir()+wxFileName::GetPathSeparator()+_T("beamhome");
#else
 #ifndef ENV_WINDOWSCE
   if (!wxFileName::DirExists(wxStandardPaths::Get().GetUserDataDir()))
    wxFileName::Mkdir(wxStandardPaths::Get().GetUserDataDir());
   sTmp=wxStandardPaths::Get().GetUserDataDir()+wxFileName::GetPathSeparator()+g_projectFile;
 #else //ENV_WINDOWSCE
   if (!wxFileName::DirExists(_T("oapchome"))) wxFileName::Mkdir(_T("oapchome"));
   sTmp=_T("oapchome")+wxFileName::GetPathSeparator()+g_projectFile;
 #endif //ENV_WINDOWSCE
#endif // ENV_BEAM
   wxRenameFile(sTmp+_T(".rtdat"),sTmp+_T("rtbak"),true);
   rtHandle=new wxFile(sTmp+_T(".rtdat"),wxFile::write);
   if ((rtHandle) && (rtHandle->IsOpened()))
   {
      rtHandle->Write(CHUNK_FORM"    "CHUNK_APRT,12); /********************************************************/

#ifndef ENV_BEAM
      g_objectList.saveUserData(rtHandle);
#else
      cncoProject cnco;

      cnco.saveUserData(rtHandle);
#endif

      l=htonl(rtHandle->Tell()-8); // total size of file data
      rtHandle->Seek(4,wxFromStart);
      rtHandle->Write(&l,4);

      rtHandle->Flush();
      rtHandle->Close();         
      wxRemoveFile(sTmp+_T("rtbak")); // remove the backup only after the file has been saved fully
   }
   else wxMessageBox(_("Data error: could not store data, the changed values may not survive the next reboot!"),_T("Error"),wxICON_ERROR,parent);
   if (rtHandle) delete rtHandle;
}
#endif



void hmiUserMgmntPanel::updateUI()
{
   bool      gEnabled;
   wxInt32   i;

   gEnabled=((m_userList->GetSelection()>-1) && (g_userPriviData.enabled));
   for (i=0; i<=MAX_PRIVI_NUM; i++)
   {
      if (priviCB[i])
      {
         priviCB[i]->Enable(gEnabled && 
                            ((i==0) || ((g_userPriviData.privisEnabled & priviFlagMask[0])!=priviFlagEnabled[0]))); // funny logic to have the supervision-box always enabled
      }
   }
   if (priviCB[0]->GetValue())
   {
      for (i=1; i<=MAX_PRIVI_NUM; i++) if (priviCB[i])
      {
         priviCB[i]->Enable(false); // when supervision privi is set all other privileges are set implicitely
         //priviCB[i]->SetValue(true); // ???
      }
   }

   m_userBox->Enable(gEnabled);
   m_loginText->Enable(gEnabled);   m_loginField->Enable(gEnabled);
   m_nameText->Enable(gEnabled);    m_nameField->Enable(gEnabled);
   m_stateText->Enable(gEnabled);   m_stateCombo->Enable(gEnabled);
   m_commentText->Enable(gEnabled); m_commentField->Enable(gEnabled);
   m_setUserPwd->Enable(gEnabled);
   m_delUser->Enable(gEnabled);
   m_applyUserData->Enable(m_wasChanged && gEnabled);
}





