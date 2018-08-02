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

#include "DlgUsers.h"
#include "hmiUserMgmntPanel.h"


DlgUsers::DlgUsers(wxWindow* parent,const wxString & title)
         :wxDialog(parent,wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
   wxString s;

   wxBoxSizer *bSizer=new wxBoxSizer(wxVERTICAL);
   this->SetSizer(bSizer);

   hmiUserMgmntPanel *rootPanel=new hmiUserMgmntPanel(this);

   wxStaticBox *group=new wxStaticBox (this, -1, _("Users"));
   wxStaticBoxSizer *gSizer=new wxStaticBoxSizer (group, wxVERTICAL);
   gSizer->Add(rootPanel,1,wxEXPAND);
   bSizer->Add(gSizer,1,wxEXPAND);

   g_createDialogButtons(this,bSizer,wxOK);
   SetSize(bSizer->Fit(this));
   Center();
}




DlgUsers::~DlgUsers()
{
}



