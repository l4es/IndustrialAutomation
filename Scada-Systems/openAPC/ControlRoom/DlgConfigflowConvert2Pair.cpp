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
#include "flowConverter2Pair.h"
#include "DlgConfigflowConvert2Pair.h"

IMPLEMENT_CLASS(DlgConfigflowConvert2Pair, wxDialog)

BEGIN_EVENT_TABLE(DlgConfigflowConvert2Pair, wxDialog)
  EVT_BUTTON  (wxID_ANY,DlgConfigflowConvert2Pair::OnButtonClick)
END_EVENT_TABLE()


DlgConfigflowConvert2Pair::DlgConfigflowConvert2Pair(flowConverter2Pair *object,bool toPair,wxWindow* parent, const wxString& title,bool hideISConfig)
                          :DlgConfigflowName(object,parent,title,hideISConfig)
{  
   wxInt32  i;
   wxString tmpStr;

   m_object=object;

   wxPanel *panelOUT= new wxPanel(m_book);
   m_book->AddPage(panelOUT,_("Output"),true);

   wxFlexGridSizer *fSizer=new wxFlexGridSizer(2,4,4);
   fSizer->AddGrowableCol(0,3);
   fSizer->AddGrowableCol(1,12);
   panelOUT->SetSizer(fSizer);

   /* Command names *******************/
   wxStaticText *text=new wxStaticText(panelOUT,wxID_ANY,wxEmptyString);
   fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

   text=new wxStaticText(panelOUT,wxID_ANY,_("Command Name"));
   fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

   for (i=0; i<MAX_NUM_IOS; i++)
   {
      if (toPair) tmpStr=wxString::Format(_T("IN%d:"),i);
      else  tmpStr=wxString::Format(_T("OUT%d:"),i);
      text=new wxStaticText(panelOUT,wxID_ANY,tmpStr);
      fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

      m_cmdNameField[i]=new wxTextCtrl(panelOUT,wxID_ANY,m_object->cmdName[i]);
      m_cmdNameField[i]->Connect(m_cmdNameField[i]->GetId(),wxEVT_KILL_FOCUS,wxFocusEventHandler(DlgConfigflowConvert2Pair::OnNumberFocusChanged),NULL,this);
      fSizer->Add(m_cmdNameField[i],1,wxEXPAND);
   }

   SetSize(DIALOG_X_SIZE,m_bSizer->Fit(this).y);
   returnOK=1;
}



DlgConfigflowConvert2Pair::~DlgConfigflowConvert2Pair()
{
}



void DlgConfigflowConvert2Pair::OnNumberFocusChanged(wxFocusEvent &event)
{
   wxString tmp;
   wxInt32  i;

   for (i=0; i<MAX_NUM_IOS; i++)
   {
      tmp=m_cmdNameField[i]->GetValue();
      tmp.Replace(_T(" "),_T("_"));
      m_cmdNameField[i]->SetValue(tmp);
   }
   event.Skip(true);
}



void DlgConfigflowConvert2Pair::OnButtonClick(wxCommandEvent &event)
{
   wxInt32 i;

   if (event.GetId()==wxID_OK)
   {
      for (i=0; i<MAX_NUM_IOS; i++)
       m_object->cmdName[i]=m_cmdNameField[i]->GetValue();
      handleStdElements();
      Destroy();
   }
   else if (event.GetId()==wxID_CANCEL)
   {
      Destroy();
   }
}



