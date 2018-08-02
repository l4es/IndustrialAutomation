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
#include "flowObject.h"
#include "DlgConfigflowBase.h"


DlgConfigflowBase::DlgConfigflowBase(flowObject *object,wxWindow* parent, const wxString& title,bool hideISConfig)
                  :wxDialog(parent,wxID_ANY,title,wxDefaultPosition,wxDefaultSize,wxRESIZE_BORDER|wxCAPTION)
{
   m_object=object;
   returnOK=0;
#ifndef ENV_BEAM
   m_hideISConfig=hideISConfig;

   ioModCB=NULL;
   mapOutputCB=NULL;
#else
   hideISConfig=hideISConfig;
#endif

/*   m_bSizer=new wxBoxSizer(wxVERTICAL);
   this->SetSizer(m_bSizer);

   m_book=new wxBookCtrl(this,wxID_ANY);
   m_bSizer->Add(m_book,1,wxEXPAND);

   wxSizer *sizerBtns = CreateButtonSizer(wxOK|wxCANCEL);
   if (sizerBtns) m_bSizer->Add(sizerBtns, wxSizerFlags().Expand().Border());

   wxPanel *panelBasic = new wxPanel(m_book, wxID_ANY);
   m_book->AddPage(panelBasic,_("Basic"),true);

   wxFlexGridSizer *fSizerBasic=new wxFlexGridSizer(2,4,4);
   fSizerBasic->AddGrowableCol(0,3);
   fSizerBasic->AddGrowableCol(1,12);
   panelBasic->SetSizer(fSizerBasic);

   wxStaticText *text=new wxStaticText(panelBasic,wxID_ANY,_("Name"));
   fSizerBasic->Add(text,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

   m_nameField=new oapcNoSpaceCtrl(panelBasic,wxID_ANY,m_object->name);
   fSizerBasic->Add(m_nameField,1,wxEXPAND);


   updateUI();
   wxInt32 y=m_bSizer->Fit(this).y;

   if (y<200) y=200;
   SetSize(DIALOG_X_SIZE,y);*/
}



DlgConfigflowBase::~DlgConfigflowBase()
{

}



#ifndef ENV_BEAM
void DlgConfigflowBase::addStdElements(wxSizer *fSizerBasic,wxPanel *panelBasic,bool buttonsOnly,bool threeRows)
{
   wxStaticText *text;

   if (m_hideISConfig) return;
   wxASSERT(m_object);
   if (m_object)
   {
      if (!buttonsOnly)
      {
         text=new wxStaticText(panelBasic,wxID_ANY,_("Interlock Server")+_T(":"));
         fSizerBasic->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
         if ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==0)
          text->Enable(false);
      }

      mapOutputCB=new wxCheckBox(panelBasic,wxID_ANY,_("Map outputs to server"));
      fSizerBasic->Add(mapOutputCB,1,wxEXPAND);
      if (((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==0) || (m_object->data.stdOUT==0))
       mapOutputCB->Enable(false);   
      else if (m_object->m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER) mapOutputCB->SetValue(1);
      if (m_object->getMode()==OBJECT_MODE_DEVICE)
      {
         mapOutputCB->Enable(false);   
         mapOutputCB->SetValue(true);   
      }
   
      if (!buttonsOnly)
      {
          fSizerBasic->Add(new wxStaticText(panelBasic,wxID_ANY,wxEmptyString),0,0);
          if (threeRows) fSizerBasic->Add(new wxStaticText(panelBasic,wxID_ANY,wxEmptyString),0,0);
      }
      ioModCB=new wxCheckBox(panelBasic,wxID_ANY,_("Allow modification"));
      fSizerBasic->Add(ioModCB,1,wxEXPAND);
      if (((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==0) || (m_object->data.stdIN==0))
       ioModCB->Enable(false);
      else if (m_object->m_flowFlags & FLAG_ALLOW_MOD_FROM_ISERVER) ioModCB->SetValue(1);
      if (m_object->getMode()==OBJECT_MODE_DEVICE)
      {
         ioModCB->Enable(false);   
         ioModCB->SetValue(true);   
      }
      if (threeRows) fSizerBasic->Add(new wxStaticText(panelBasic,wxID_ANY,wxEmptyString),0,0);
   }    
}
#endif
