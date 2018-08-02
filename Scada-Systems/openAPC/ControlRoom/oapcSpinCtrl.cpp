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
#include <wx/spinctrl.h>

#include "oapcSpinCtrl.h"
#include "flowObject.h"
#ifdef ENV_PLAYER
 #include "OSNumpad.h"
 #include "HMICanvas.h"
 #include "globals.h"
#endif


BEGIN_EVENT_TABLE(oapcSpinCtrl,wxPanel)
   EVT_SPIN(wxID_ANY,oapcSpinCtrl::OnSpinEvent)
   EVT_TEXT(wxID_ANY,oapcSpinCtrl::OnTextChanged)
END_EVENT_TABLE()



oapcSpinCtrl::oapcSpinCtrl(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size,int min,int max,int initial)
{
   wxTextValidator textValidator;
   wxArrayString   charExcludes;

   m_min=min;
   m_max=max;
   Create(parent,id,pos,size);

   wxFlexGridSizer *fSizer=new wxFlexGridSizer(1,2,1,1);
   SetSizer(fSizer);

   m_numField=NULL;
   m_spinButton=NULL;

   m_numField=new oapcFloatCtrl(this,wxID_ANY,wxDefaultPosition,wxSize(1,1),wxTE_RIGHT|wxTE_PROCESS_ENTER);
   m_numField->SetAccuracy(0);
   m_numField->SetValue(initial);
   m_numField->m_me=this;

   charExcludes.Add(_T("0"));   charExcludes.Add(_T("1"));
   charExcludes.Add(_T("2"));   charExcludes.Add(_T("3"));
   charExcludes.Add(_T("4"));   charExcludes.Add(_T("5"));
   charExcludes.Add(_T("6"));   charExcludes.Add(_T("7"));
   charExcludes.Add(_T("8"));   charExcludes.Add(_T("9"));
   textValidator.SetStyle(wxFILTER_INCLUDE_LIST);
   textValidator.SetExcludes(charExcludes);
   m_numField->SetValidator(textValidator);
   fSizer->Add(m_numField,1,wxEXPAND);

   m_spinButton=new wxSpinButton(this,id,wxDefaultPosition,wxDefaultSize,wxSP_VERTICAL);
   m_spinButton->SetRange(m_min,m_max);
   m_spinButton->SetValue(initial);
   fSizer->Add(m_spinButton,1,wxEXPAND);

   fSizer->AddGrowableCol(0,1);
//   fSizer->AddGrowableRow(0,1);
}



oapcSpinCtrl::~oapcSpinCtrl()
{
}



bool oapcSpinCtrl::Enable(bool enable)
{
   m_spinButton->Enable(enable);
   return m_numField->Enable(enable);
}



void oapcSpinCtrl::SetEditable(bool editable)
{
   m_spinButton->Enable(editable);
   m_numField->SetEditable(editable);
}



wxInt32 oapcSpinCtrl::GetValue()
{
   return m_numField->GetValue(m_min,m_max);
}



void oapcSpinCtrl::SetValue(wxInt32 value)
{
   m_numField->SetValue(value);
}



void oapcSpinCtrl::OnSpinEvent(wxSpinEvent& event)
{
   m_numField->SetValue(m_spinButton->GetValue());
#ifdef ENV_PLAYER
   g_hmiCanvas->OnSpinNumberChanging(event);
#else
   event.GetId();
#endif
}



void oapcSpinCtrl::OnTextChanged(wxCommandEvent& WXUNUSED(event))
{
   if ((m_numField) && (m_spinButton))
   {
      static wxInt32 prevVal=0;
             wxInt32 val;

      val=m_numField->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE);
      if (val>m_max) val=m_max;
      else if (val<m_min) val=m_min;
      if (prevVal!=val)
      {
         m_spinButton->SetValue(val);
         prevVal=val;
      }
   }
}
