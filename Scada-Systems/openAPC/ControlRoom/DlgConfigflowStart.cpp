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
#include <wx/spinctrl.h>

#include "globals.h"
#include "hmiObject.h"
#include "flowGroup.h"
#include "DlgConfigflowStart.h"
#include "flowStart.h"



IMPLEMENT_CLASS(DlgConfigflowStart, wxDialog)

BEGIN_EVENT_TABLE(DlgConfigflowStart, wxDialog)
  EVT_BUTTON  (wxID_ANY,DlgConfigflowStart::OnButtonClick)
END_EVENT_TABLE()


DlgConfigflowStart::DlgConfigflowStart(flowStart *object,wxWindow* parent, const wxString& title,bool hideISConfig)
                   :DlgConfigflowName(object,parent,title,hideISConfig)
{  
   wxInt32 i;
   
   m_object=object;

   wxPanel *panelOUT= new wxPanel(m_book);
   m_book->AddPage(panelOUT,_("Output"),true);

   wxFlexGridSizer *panelBaseSizer=new wxFlexGridSizer(2,4,4);
   panelBaseSizer->AddGrowableCol(1,5);
   panelOUT->SetSizer(panelBaseSizer);

   for (i=0; i<2; i++) // outputs
   {
      wxStaticText *text=new wxStaticText(panelOUT,wxID_ANY,wxString::Format(_T("OUT%d"),i+1));
      panelBaseSizer->Add(text,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

      m_digiField[i]=new wxSpinCtrl(panelOUT,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,0,1,object->convData.digi[i]);
      panelBaseSizer->Add(m_digiField[i],1,wxEXPAND);
   }
   for (i=0; i<2; i++) // outputs
   {
      wxStaticText *text=new wxStaticText(panelOUT,wxID_ANY,wxString::Format(_T("OUT%d"),i+3));
      panelBaseSizer->Add(text,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

      m_numField[i]=new wxTextCtrl(panelOUT,wxID_ANY);
      ((wxTextCtrl*)m_numField[i])->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
      *m_numField[i] << object->convData.mnum[i]/1000.0;
      panelBaseSizer->Add(m_numField[i],1,wxEXPAND);
   }
   for (i=0; i<2; i++) // outputs
   {
      wxStaticText *text=new wxStaticText(panelOUT,wxID_ANY,wxString::Format(_T("OUT%d"),i+5));
      panelBaseSizer->Add(text,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

      m_textField[i]=new wxTextCtrl(panelOUT,wxID_ANY);
      m_textField[i]->SetValue(object->txt[i]);
      panelBaseSizer->Add(m_textField[i],1,wxEXPAND);
   }
   SetSize(DIALOG_X_SIZE,m_bSizer->Fit(this).y);
   returnOK=1;
}



DlgConfigflowStart::~DlgConfigflowStart()
{
}



void DlgConfigflowStart::OnButtonClick(wxCommandEvent &event)
{
   if (event.GetId()==wxID_OK)
   {
      wxInt32 o;
      double d;

      for (o=0; o<2; o++)
      {
         m_object->convData.digi[o]=m_digiField[o]->GetValue();
         m_numField[o]->GetValue().ToDouble(&d);
         if (d<-2100000000) d=-2100000000;
         else if (d>2100000000) d=2100000000;
         m_object->convData.mnum[o]=d*1000.0;
         m_object->txt[o]=m_textField[o]->GetValue();
      }
      handleStdElements();
      Destroy();
   }
   else if (event.GetId()==wxID_CANCEL)
   {
      Destroy();
   }
}


