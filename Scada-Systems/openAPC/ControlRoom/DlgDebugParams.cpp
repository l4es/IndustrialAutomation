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

#include "DebugWin.h"
#include "globals.h"
#include "DlgDebugParams.h"


IMPLEMENT_CLASS(DlgDebugParams, wxDialog)

BEGIN_EVENT_TABLE(DlgDebugParams, wxDialog)
  EVT_BUTTON  (wxID_ANY,DlgDebugParams::OnButtonClick)
  EVT_CHECKBOX(wxID_ANY,DlgDebugParams::OnCheckBoxCheck)
END_EVENT_TABLE()



DlgDebugParams::DlgDebugParams(wxUint32 flags,wxWindow* parent, const wxString& title, const wxPoint& pos,const wxString& name)
                      :wxDialog(parent,wxID_ANY,title,pos,wxSize(300,250),wxRESIZE_BORDER|wxCAPTION,name)
{
    this->flags=flags;

    wxBoxSizer *bSizer=new wxBoxSizer(wxVERTICAL);
    this->SetSizer(bSizer);

    wxPanel *rootPanel1=new wxPanel(this,wxID_ANY);
    bSizer->Add(rootPanel1,1,wxEXPAND);

    g_createDialogButtons(this,bSizer,wxOK|wxCANCEL);

    wxStaticBoxSizer *eSizer1 = new wxStaticBoxSizer (wxVERTICAL,rootPanel1,_("Stop flow on events")+_T(":"));
    rootPanel1->SetSizer(eSizer1);

    stopOnThreadID=new wxCheckBox(rootPanel1,wxID_ANY,_("When flow loop was detected"));
    eSizer1->Add(stopOnThreadID,0,wxALIGN_LEFT|wxALIGN_TOP);

    stopOnIllegalIO=new wxCheckBox(rootPanel1,wxID_ANY,_("When unknown IO was defined"));
    eSizer1->Add(stopOnIllegalIO,0,wxALIGN_LEFT|wxALIGN_TOP);

    stopOnRunTimeout=new wxCheckBox(rootPanel1,wxID_ANY,_("When flow run timeout was reached"));
    eSizer1->Add(stopOnRunTimeout,0,wxALIGN_LEFT|wxALIGN_TOP);

/*    stopOnDataConvError=new wxCheckBox(rootPanel1,wxID_ANY,_("When a data conversion error occurs"));
    eSizer1->Add(stopOnDataConvError,0,wxALIGN_LEFT|wxALIGN_TOP);
*/
    stopOnExtLibError=new wxCheckBox(rootPanel1,wxID_ANY,_("When an error occurs in plug-in"));
    eSizer1->Add(stopOnExtLibError,0,wxALIGN_LEFT|wxALIGN_TOP);

    stopOnIncompData=new wxCheckBox(rootPanel1,wxID_ANY,_("When incoming binary data are not supported"));
    eSizer1->Add(stopOnIncompData,0,wxALIGN_LEFT|wxALIGN_TOP);

    updateUI();
    SetSize(bSizer->Fit(this));
}

DlgDebugParams::~DlgDebugParams()
{

}



void DlgDebugParams::updateUI()
{
   if ((flags & DEBUG_STOP_COND_THREAD_ID)==DEBUG_STOP_COND_THREAD_ID) stopOnThreadID->SetValue(true);
   else stopOnThreadID->SetValue(false);
   if ((flags & DEBUG_STOP_COND_ILLEGAL_IO)==DEBUG_STOP_COND_ILLEGAL_IO) stopOnIllegalIO->SetValue(true);
   else stopOnIllegalIO->SetValue(false);
   if ((flags & DEBUG_STOP_COND_RUN_TIMEOUT)==DEBUG_STOP_COND_RUN_TIMEOUT) stopOnRunTimeout->SetValue(true);
   else stopOnRunTimeout->SetValue(false);
   if ((flags & DEBUG_STOP_COND_INCOMPATIBLE_DATATYPE)==DEBUG_STOP_COND_INCOMPATIBLE_DATATYPE) stopOnIncompData->SetValue(true);
   else stopOnIncompData->SetValue(false);
   if ((flags & (DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT|DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_OUTPUT|DEBUG_STOP_COND_LIB_FAILED|DEBUG_STOP_COND_LIB_RETURNED_ERROR))==(DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT|DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_OUTPUT|DEBUG_STOP_COND_LIB_FAILED|DEBUG_STOP_COND_LIB_RETURNED_ERROR)) stopOnExtLibError->SetValue(true);
   else stopOnExtLibError->SetValue(false);
}



void DlgDebugParams::OnButtonClick(wxCommandEvent &event)
{
    if (event.GetId()==wxID_OK)
    {
        flags=0;
        if (stopOnThreadID->GetValue()) flags=DEBUG_STOP_COND_THREAD_ID;
        if (stopOnIllegalIO->GetValue()) flags|=DEBUG_STOP_COND_ILLEGAL_IO;
        if (stopOnRunTimeout->GetValue()) flags|=DEBUG_STOP_COND_RUN_TIMEOUT;
        if (stopOnIncompData->GetValue()) flags|=DEBUG_STOP_COND_INCOMPATIBLE_DATATYPE;
        if (stopOnExtLibError->GetValue()) flags|=(DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT|DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_OUTPUT|DEBUG_STOP_COND_LIB_FAILED);
        Destroy();
    }
    else if (event.GetId()==wxID_CANCEL)
    {
        Destroy();
    }

}



void DlgDebugParams::OnCheckBoxCheck(wxCommandEvent &WXUNUSED(event))
{
}


