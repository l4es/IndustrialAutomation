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
#include <wx/gbsizer.h>
#include <wx/statline.h>

#include "UserData.h"
#include "ChangePwdDlg.h"
#include "globals.h"


BEGIN_EVENT_TABLE(ChangePwdDlg, wxDialog)
   EVT_BUTTON(wxID_ANY, ChangePwdDlg::OnButtonClick)
END_EVENT_TABLE()

class UserData;

ChangePwdDlg::ChangePwdDlg(wxWindow *parent,wxString title,UserData *userData)
             :wxDialog(parent,wxID_ANY,title,wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE,wxEmptyString)
             , m_ok(false)
{
   wxBoxSizer *bSizer=new wxBoxSizer(wxVERTICAL);
   this->SetSizer(bSizer);

   m_userData=userData;
   wxPanel *rootPanel=new wxPanel(this);
   bSizer->Add(rootPanel,1,wxEXPAND);

   g_createDialogButtons(this,bSizer,wxOK|wxCANCEL);

   wxGridBagSizer *gbSizer=new wxGridBagSizer(4,4);

   rootPanel->SetSizer(gbSizer);

   wxStaticText *text=new wxStaticText(rootPanel,wxID_ANY,_("Old password"));
   gbSizer->Add(text,wxGBPosition(0,0),wxGBSpan(1,1),wxALIGN_CENTER_VERTICAL);
   oldPwd=new oapcTextCtrl(rootPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_PASSWORD);
   gbSizer->Add(oldPwd,wxGBPosition(0,1),wxGBSpan(1,1),wxEXPAND);
#ifdef ENV_EDITOR
   text->Enable(false);
   oldPwd->Enable(false);
#endif

   wxStaticLine *line=new wxStaticLine(rootPanel);
   gbSizer->Add(line,wxGBPosition(1,0),wxGBSpan(1,2),wxEXPAND);

   text=new wxStaticText(rootPanel,wxID_ANY,_("New password"));
   gbSizer->Add(text,wxGBPosition(2,0),wxGBSpan(1,1),wxALIGN_CENTER_VERTICAL);
   newPwd=new oapcTextCtrl(rootPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_PASSWORD);
   gbSizer->Add(newPwd,wxGBPosition(2,1),wxGBSpan(1,1),wxEXPAND);

   text=new wxStaticText(rootPanel,wxID_ANY,_("Repeat password"));
   gbSizer->Add(text,wxGBPosition(3,0),wxGBSpan(1,1),wxALIGN_CENTER_VERTICAL);
   repPwd=new oapcTextCtrl(rootPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_PASSWORD);
   gbSizer->Add(repPwd,wxGBPosition(3,1),wxGBSpan(1,1),wxEXPAND);
   gbSizer->AddGrowableCol(1,1);

   SetSize(250,bSizer->Fit(this).y);
   Center();
}



ChangePwdDlg::~ChangePwdDlg()
{
}



void ChangePwdDlg::OnButtonClick(wxCommandEvent &event)
{
   if (event.GetId()==wxID_OK)
   {
#ifndef ENV_EDITOR
      if (oldPwd->GetValue().Length()<=0)
      {
         wxMessageBox(_("Please enter your old password!"),_("Error"),wxOK|wxICON_ERROR);
         event.Skip(false);
         return;
      }
      else if (oldPwd->GetValue().Cmp(m_userData->m_pwd)!=0)
      {
         wxMessageBox(_("Wrong password entered!\nPlease enter your password that is currently used by you!"),_("Error"),wxOK|wxICON_ERROR);
         event.Skip(false);
         return;
      }
      else 
#endif
           if ((newPwd->GetValue().Length()<=0) || (repPwd->GetValue().Length()<=0))
      {
         wxMessageBox(_("Please enter your new password twice!"),_("Error"),wxOK|wxICON_ERROR);
         event.Skip(false);
         return;
      }
      else if (!newPwd->GetValue().Cmp(repPwd->GetValue())==0)
      {
         wxMessageBox(_("The new passwords do not match!"),_("Error"),wxOK|wxICON_ERROR);
         newPwd->SetValue(wxEmptyString);
         repPwd->SetValue(wxEmptyString);
         event.Skip(false);
         return;
      }
      m_userData->m_pwd=newPwd->GetValue();
      m_ok=true;
      Destroy();
   }
   else if (event.GetId()==wxID_CANCEL)
   {
      Destroy();
   }
}

