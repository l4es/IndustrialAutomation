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

#include "OSNumpad.h"

#include <wx/gbsizer.h>
#include <wx/artprov.h>

#include "oapcFloatCtrl.h"
#include "globals.h"
#include "MainWin.h"

#define ID_BACKSPACE 0x3004
#define ID_MINUS     0x3005
#define ID_ENTER     0x3006

BEGIN_EVENT_TABLE(OSNumpad, OSKeyboard)
   EVT_BUTTON(ID_BACKSPACE,OSNumpad::OnButtonClicked)
   EVT_BUTTON(ID_ENTER,OSNumpad::OnButtonClicked)
   EVT_BUTTON(ID_MINUS,OSNumpad::OnButtonClicked)
END_EVENT_TABLE()



OSNumpad::OSNumpad(oapcTextCtrl *textInput,wxWindowID id, const double value)
         :OSKeyboard(textInput,id)
{
   init(value);
   setOptPosition((wxWindow*)textInput);
}



void OSNumpad::init(wxFloat64 value)
{
   wxInt32 i,j,cnt=0;

   m_row=_("npad");
   if (m_row.Length()<10) m_row=_T("7894561230,");

   wxGridBagSizer *gbSizer=new wxGridBagSizer(2,2);

   m_inputField=new oapcFloatCtrl(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTE_READONLY|wxTE_CENTRE);
   ((oapcFloatCtrl*)m_inputField)->SetAccuracy(((oapcFloatCtrl*)m_input)->GetAccuracy());
   if (value!=0)
   {
      wxString str;
      wxInt32  pos;
      
      str=((wxTextCtrl*)m_input)->GetValue();
      pos=str.Find(_T(","));
      if (pos==wxNOT_FOUND) pos=str.Find(_T("."));
      if (pos!=wxNOT_FOUND) ((oapcFloatCtrl*)m_inputField)->SetAccuracy(str.Length()-pos);
      ((oapcFloatCtrl*)m_inputField)->SetValue(value);
   }
   else m_inputField->SetValue(wxEmptyString);
   gbSizer->Add(m_inputField,wxGBPosition(0,0),wxGBSpan(1,4),wxEXPAND);
   for (j=0; j<3; j++) for (i=0; i<3; i++) 
   {
      m_keyButton[i][j]=new wxButton(this,wxID_ANY,m_row.Mid(cnt,1),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
      m_keyButton[i][j]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(OSNumpad::OnButtonClicked),NULL,this);
      gbSizer->Add(m_keyButton[i][j],wxGBPosition(j+1,i),wxGBSpan(1,1),wxEXPAND);
      cnt++;
   }
   m_backspace=new wxBitmapButton(this,ID_BACKSPACE,wxArtProvider::GetBitmap(wxART_GO_BACK),wxDefaultPosition,wxDefaultSize,wxBU_AUTODRAW);
   gbSizer->Add(m_backspace,wxGBPosition(1,3),wxGBSpan(1,1),wxEXPAND);

   m_minus=new wxButton(this,ID_MINUS,_T("-"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
   m_minus->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(OSNumpad::OnButtonClicked),NULL,this);
   gbSizer->Add(m_minus,wxGBPosition(2,3),wxGBSpan(1,1),wxEXPAND);

   m_enter=new wxBitmapButton(this,ID_ENTER,wxArtProvider::GetBitmap(wxART_TICK_MARK),wxDefaultPosition,wxDefaultSize,wxBU_AUTODRAW);
   gbSizer->Add(m_enter,wxGBPosition(3,3),wxGBSpan(2,1),wxEXPAND);

   m_keyButton[3][0]=new wxButton(this,wxID_ANY,m_row.Mid(cnt,1),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
   m_keyButton[3][0]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(OSNumpad::OnButtonClicked),NULL,this);
   gbSizer->Add(m_keyButton[3][0],wxGBPosition(4,0),wxGBSpan(1,2),wxEXPAND);
   cnt++;

   if (((oapcFloatCtrl*)m_input)->m_accuracy==0)
   {
      m_keyButton[3][1]=NULL;
   }
   else
   {
      m_keyButton[3][1]=new wxButton(this,wxID_ANY,m_row.Mid(cnt,1),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
      m_keyButton[3][1]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(OSNumpad::OnButtonClicked),NULL,this);
      gbSizer->Add(m_keyButton[3][1],wxGBPosition(4,2),wxGBSpan(1,1),wxEXPAND);
   }
   cnt++;

   for (i=1; i<5; i++) gbSizer->AddGrowableRow(i,1);
   for (i=0; i<4; i++) gbSizer->AddGrowableCol(i,1);

#if wxCHECK_VERSION(2,9,0)
   g_setFont(this,wxFont(g_objectList.m_projectData->font.pointSize,
                         wxFONTFAMILY_DEFAULT,
                         (wxFontStyle)g_objectList.m_projectData->font.style,
                         (wxFontWeight)g_objectList.m_projectData->font.weight,
                         false,
                         g_projectTouchFontFaceName));
#else
   g_setFont(this,wxFont(g_objectList.m_projectData->font.pointSize,
                         wxFONTFAMILY_DEFAULT,
                         g_objectList.m_projectData->font.style,
                         g_objectList.m_projectData->font.weight,
                         false,
                         g_projectTouchFontFaceName));
#endif
   SetSizer(gbSizer);

   SetSize(gbSizer->Fit(this).x*g_objectList.m_projectData->touchFactor/1000.0,
           gbSizer->Fit(this).y*g_objectList.m_projectData->touchFactor/1000.0);
}


OSNumpad::~OSNumpad()
{

}



void OSNumpad::OnButtonClicked(wxCommandEvent &event)
{
   if (event.GetId()==ID_MINUS)
   {
      if (m_inputField->GetValue().Mid(0,1).Cmp(_T("-"))==0)
      {
         m_inputField->SetValue(m_inputField->GetValue().Mid(1));
      }
      else
      {
         m_inputField->SetValue(_T("-")+m_inputField->GetValue());
      }
   }
   else if (event.GetId()==ID_BACKSPACE)
   {
      wxString val;

      val=m_inputField->GetValue();
      val=val.Mid(0,val.Length()-1);
      m_inputField->SetValue(val);
   }
   else if (event.GetId()==ID_ENTER)
   {
      m_input->SetValue(m_inputField->GetValue());
      Destroy();
   }
   else
   {
      wxInt32 i,j;

      if ((m_keyButton[3][0]) && (m_keyButton[3][0]->GetId()==event.GetId())) *m_inputField<<m_keyButton[3][0]->GetLabel();
      else if ((m_keyButton[3][1]) && (m_keyButton[3][1]->GetId()==event.GetId())) *m_inputField<<m_keyButton[3][1]->GetLabel();
      else for (i=0; i<3; i++) for (j=0; j<3; j++) if (m_keyButton[j][i])
      {
         if (m_keyButton[j][i]->GetId()==event.GetId())
         {
            *m_inputField<<m_keyButton[j][i]->GetLabel();
            break;
         }
      }
   }
}

