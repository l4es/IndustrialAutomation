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
#include "DlgConfigflowConvertMixed2Char.h"
#include "flowConverterMixed2Char.h"

IMPLEMENT_CLASS(DlgConfigflowConvertMixed2Char, wxDialog)

BEGIN_EVENT_TABLE(DlgConfigflowConvertMixed2Char, wxDialog)
  EVT_BUTTON  (wxID_ANY,DlgConfigflowConvertMixed2Char::OnButtonClick)
END_EVENT_TABLE()


DlgConfigflowConvertMixed2Char::DlgConfigflowConvertMixed2Char(flowConverterMixed2Char *object,wxWindow* parent, const wxString& title,bool hideISConfig)
                               :DlgConfigflowName(object,parent,title,hideISConfig)
{  
   m_object=object;

   wxPanel *panelOUT= new wxPanel(m_book);
   m_book->AddPage(panelOUT,_T("OUT1"),true);

   wxFlexGridSizer *fSizer=new wxFlexGridSizer(2,4,4);
   fSizer->AddGrowableCol(0,3);
   fSizer->AddGrowableCol(1,12);
   panelOUT->SetSizer(fSizer);

   m_modeOnClock=new wxCheckBox(panelOUT,wxID_ANY,_("Convert on clock at IN0"));
   if (object->convData.flags & flowConverterMixed2Char::CONVERTER_FLAGS_USE_CLOCK) m_modeOnClock->SetValue(1);
   fSizer->Add(m_modeOnClock,1,wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);
   wxStaticText *text=new wxStaticText(panelOUT,wxID_ANY,_T(""));
   fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

   m_modeForceToDot=new wxCheckBox(panelOUT,wxID_ANY,_("Force Decimal Place to Dot"));
   if (object->convData.flags & flowConverterMixed2Char::CONVERTER_FLAGS_FORCE_TO_DOT) m_modeForceToDot->SetValue(1);
   fSizer->Add(m_modeForceToDot,1,wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);
   text=new wxStaticText(panelOUT,wxID_ANY,_T(""));
   fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

   text=new wxStaticText(panelOUT,wxID_ANY,_("Convert String")+_T(":"));
   fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   m_fmtChar=new wxTextCtrl(panelOUT,wxID_ANY,wxEmptyString);
   fSizer->Add(m_fmtChar,1,wxEXPAND);
   m_fmtChar->SetValue(object->outFormat);

   SetSize(DIALOG_X_SIZE,m_bSizer->Fit(this).y);
   returnOK=1;
}



DlgConfigflowConvertMixed2Char::~DlgConfigflowConvertMixed2Char()
{
}



void DlgConfigflowConvertMixed2Char::OnButtonClick(wxCommandEvent &event)
{
   if (event.GetId()==wxID_OK)
   {
      if (m_modeOnClock->GetValue()) m_object->convData.flags|=flowConverterMixed2Char::CONVERTER_FLAGS_USE_CLOCK;
      else m_object->convData.flags&=~flowConverterMixed2Char::CONVERTER_FLAGS_USE_CLOCK;
      if (m_modeForceToDot->GetValue()) m_object->convData.flags|=flowConverterMixed2Char::CONVERTER_FLAGS_FORCE_TO_DOT;
      else m_object->convData.flags&=~flowConverterMixed2Char::CONVERTER_FLAGS_FORCE_TO_DOT;
      m_object->outFormat=m_fmtChar->GetValue();
      handleStdElements();
      Destroy();
   }
   else if (event.GetId()==wxID_CANCEL)
   {
      Destroy();
   }
}



