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
#include "flowObject.h"
#include "DlgConfigflowName.h"
#include "IOElementDefinition.h"

BEGIN_EVENT_TABLE(DlgConfigflowName, DlgConfigflowBase)
  EVT_BUTTON  (wxID_ANY,DlgConfigflowName::OnButtonClick)
END_EVENT_TABLE()


DlgConfigflowName::DlgConfigflowName(flowObject *object,wxWindow* parent, const wxString& title,bool hideISConfig)
                  :DlgConfigflowBase(object,parent,title,hideISConfig)
{
   m_bSizer=new wxBoxSizer(wxVERTICAL);
   this->SetSizer(m_bSizer);

   m_book=new wxBookCtrl(this,wxID_ANY);
   m_bSizer->Add(m_book,1,wxEXPAND);

   g_createDialogButtons(this,m_bSizer,wxOK|wxCANCEL);

   wxPanel *panelBasic = new wxPanel(m_book, wxID_ANY);
   m_book->AddPage(panelBasic,_("Basic"),true);

   wxFlexGridSizer *fSizerBasic=new wxFlexGridSizer(2,4,4);

   wxStaticText *text=new wxStaticText(panelBasic,wxID_ANY,_("Name"));
   fSizerBasic->Add(text,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

   m_nameField=new oapcNoSpaceCtrl(panelBasic,wxID_ANY,m_object->name);
   fSizerBasic->Add(m_nameField,1,wxEXPAND);

   addStdElements(fSizerBasic,panelBasic,false,false);

   updateUI();

   fSizerBasic->AddGrowableCol(0,4);
   fSizerBasic->AddGrowableCol(1,11);
   panelBasic->SetSizer(fSizerBasic);

   wxInt32 y=m_bSizer->Fit(this).y;

   if (y<200) y=200;
   SetSize(DIALOG_X_SIZE,y);
}



DlgConfigflowName::~DlgConfigflowName()
{

}



void DlgConfigflowName::updateUI()
{

}



void DlgConfigflowName::handleStdElements()
{
   wxASSERT(m_object);
   if (m_object)
   {
      if (mapOutputCB)
      {
         if (mapOutputCB->GetValue()) m_object->m_flowFlags|=FLAG_MAP_OUTPUT_TO_ISERVER;
         else m_object->m_flowFlags&=~(FLAG_MAP_OUTPUT_TO_ISERVER);
      }
      if (ioModCB)
      {
         if (ioModCB->GetValue()) m_object->m_flowFlags|=FLAG_ALLOW_MOD_FROM_ISERVER;
         else m_object->m_flowFlags&=~(FLAG_ALLOW_MOD_FROM_ISERVER);
      }
      m_object->name=m_nameField->GetValue();
   }
   g_isSaved=false;
}



void DlgConfigflowName::OnButtonClick(wxCommandEvent &event)
{
   if (event.GetId()==wxID_OK)
   {
      returnOK=1;

      handleStdElements();
      Destroy();
   }
   else if (event.GetId()==wxID_CANCEL)
   {
      Destroy();
   }
}





