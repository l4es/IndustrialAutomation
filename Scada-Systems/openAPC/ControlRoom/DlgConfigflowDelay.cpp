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
#include "DlgConfigflowDelay.h"
#include "flowDelay.h"



IMPLEMENT_CLASS(DlgConfigflowDelay, wxDialog)

BEGIN_EVENT_TABLE(DlgConfigflowDelay, wxDialog)
  EVT_BUTTON  (wxID_ANY,DlgConfigflowDelay::OnButtonClick)
END_EVENT_TABLE()


DlgConfigflowDelay::DlgConfigflowDelay(flowDelay *object,wxWindow* parent, const wxString& title,bool hideISConfig)
                   :DlgConfigflowName(object,parent,title,hideISConfig)
{  
   wxInt32 i;
   
   m_object=object;

   wxPanel *panelOUT= new wxPanel(m_book);
   m_book->AddPage(panelOUT,_("Output"),true);

   wxFlexGridSizer *panelBaseSizer=new wxFlexGridSizer(3,4,4);
   panelBaseSizer->AddGrowableCol(1,5);
   panelOUT->SetSizer(panelBaseSizer);

   for (i=0; i<6; i++) // outputs
   {
      wxStaticText *text=new wxStaticText(panelOUT,wxID_ANY,wxString::Format(_T("OUT%d"),i));
      panelBaseSizer->Add(text,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

      m_delayField[i]=new wxSpinCtrl(panelOUT,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,1,10000,object->convData.delay[i]);
      panelBaseSizer->Add(m_delayField[i],1,wxEXPAND);

      text=new wxStaticText(panelOUT,wxID_ANY,wxString::Format(_T("msec"),i));
      panelBaseSizer->Add(text,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   }
   SetSize(DIALOG_X_SIZE,m_bSizer->Fit(this).y);
   returnOK=1;
}



DlgConfigflowDelay::~DlgConfigflowDelay()
{
}



void DlgConfigflowDelay::OnButtonClick(wxCommandEvent &event)
{
   if (event.GetId()==wxID_OK)
   {
      wxInt32 o;

      for (o=0; o<6; o++)
      {
         m_object->convData.delay[o]=m_delayField[o]->GetValue();
      }
      handleStdElements();
      Destroy();
   }
   else if (event.GetId()==wxID_CANCEL)
   {
      Destroy();
   }
}


