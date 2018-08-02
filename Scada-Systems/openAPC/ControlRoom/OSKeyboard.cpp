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

#include "OSKeyboard.h"

#include <wx/gbsizer.h>
#include <wx/artprov.h>

#include "ObjectList.h"
#include "globals.h"
#include "liboapc.h"
#include "oapcTextCtrl.h"
#include "MainWin.h"


#define ID_SHIFT1    0x3001
#define ID_SHIFT2    0x3002
#define ID_CAPS_LOCK 0x3003
#define ID_BACKSPACE 0x3004
#define ID_SPACE     0x3005
#define ID_ENTER     0x3006

BEGIN_EVENT_TABLE(OSKeyboard, wxDialog)
   EVT_TOGGLEBUTTON(ID_SHIFT1,OSKeyboard::OnButtonClicked)
   EVT_TOGGLEBUTTON(ID_SHIFT2,OSKeyboard::OnButtonClicked)
   EVT_TOGGLEBUTTON(ID_CAPS_LOCK,OSKeyboard::OnButtonClicked)
   EVT_BUTTON(ID_BACKSPACE,OSKeyboard::OnButtonClicked)
   EVT_BUTTON(ID_SPACE,OSKeyboard::OnButtonClicked)
   EVT_BUTTON(ID_ENTER,OSKeyboard::OnButtonClicked)
END_EVENT_TABLE()


OSKeyboard::OSKeyboard(oapcTextCtrl* input, wxWindowID id)
           :wxDialog(g_mainWin,id,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxRESIZE_BORDER|wxSTAY_ON_TOP|wxCAPTION,wxEmptyString)
{
   m_input=input;
}



OSKeyboard::OSKeyboard(oapcTextCtrl* input, wxWindowID id, const wxString& value)
           :wxDialog(g_mainWin,id,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxRESIZE_BORDER|wxSTAY_ON_TOP|wxCAPTION,wxEmptyString)
{
   wxInt32 i,x,y,ex,ey;

   wxGridBagSizer *gbSizer=new wxGridBagSizer(2,2);

   m_input=input;
   m_row[0][0]=_("kbd1");  if (m_row[0][0].Length()<10) m_row[0][0]=_T("`1234567890-=");   
   m_row[1][0]=_("kbd1s"); if (m_row[1][0].Length()<10) m_row[1][0]=_T("~!@#$%^&*()_+");
   m_row[0][1]=_("kbd2");  if (m_row[0][1].Length()<10) m_row[0][1]=_T("qwertyuiop[]\\");  
   m_row[1][1]=_("kbd2s"); if (m_row[1][1].Length()<10) m_row[1][1]=_T("QWERTYUIOP{}|");
   m_row[0][2]=_("kbd3");  if (m_row[0][2].Length()<10) m_row[0][2]=_T("asdfghjkl;' ");    
   m_row[1][2]=_("kbd3s"); if (m_row[1][2].Length()<10) m_row[1][2]=_T("ASDFGHJKL:\" ");
   m_row[0][3]=_("kbd4");  if (m_row[0][3].Length()<10) m_row[0][3]=_T(" zxcvbnm,./");     
   m_row[1][3]=_("kbd4s"); if (m_row[1][3].Length()<10) m_row[1][3]=_T(" ZXCVBNM<>?");

   m_inputField=new wxTextCtrl(this,wxID_ANY,value,wxDefaultPosition,wxDefaultSize,wxTE_READONLY|wxTE_CENTRE);
   gbSizer->Add(m_inputField,wxGBPosition(0,0),wxGBSpan(1,30),wxEXPAND);
   for (i=0; i<13; i++)
   {
      m_keyButton[0][i]=new wxButton(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
      m_keyButton[0][i]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(OSKeyboard::OnButtonClicked),NULL,this);
      gbSizer->Add(m_keyButton[0][i],wxGBPosition(1,i*2),wxGBSpan(1,2),wxEXPAND);
   }
   m_backspace=new wxBitmapButton(this,ID_BACKSPACE,wxArtProvider::GetBitmap(wxART_GO_BACK),wxDefaultPosition,wxDefaultSize,wxBU_AUTODRAW);
   gbSizer->Add(m_backspace,wxGBPosition(1,26),wxGBSpan(1,4),wxEXPAND);

   wxStaticText *text=new wxStaticText(this,wxID_ANY,_("  "));
   gbSizer->Add(text,wxGBPosition(2,2),wxGBSpan(1,1),wxEXPAND);
   for (i=0; i<12; i++)
   {
      m_keyButton[1][i]=new wxButton(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
      m_keyButton[1][i]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(OSKeyboard::OnButtonClicked),NULL,this);
      gbSizer->Add(m_keyButton[1][i],wxGBPosition(2,i*2+3),wxGBSpan(1,2),wxEXPAND);
   }
   m_enter=new wxBitmapButton(this,ID_ENTER,wxArtProvider::GetBitmap(wxART_TICK_MARK),wxDefaultPosition,wxDefaultSize,wxBU_AUTODRAW);
   gbSizer->Add(m_enter,wxGBPosition(2,28),wxGBSpan(2,2),wxEXPAND);

   m_capsLock=new wxToggleButton(this,ID_CAPS_LOCK,_T("\\/"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
//   m_capsLock=new wxBitmapButton(this,ID_CAPS_LOCK,wxArtProvider::GetBitmap(wxART_GO_DOWN),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
   gbSizer->Add(m_capsLock,wxGBPosition(3,0),wxGBSpan(1,4),wxEXPAND);
   for (i=0; i<12; i++)
   {
      m_keyButton[2][i]=new wxButton(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
      m_keyButton[2][i]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(OSKeyboard::OnButtonClicked),NULL,this);
      gbSizer->Add(m_keyButton[2][i],wxGBPosition(3,i*2+4),wxGBSpan(1,2),wxEXPAND);
   }

   m_shiftOnce1=new wxToggleButton(this,ID_SHIFT1,_T("/\\"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
//   m_shiftOnce1=new wxBitmapButton(this,ID_SHIFT1,wxArtProvider::GetBitmap(wxART_GO_UP),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
   gbSizer->Add(m_shiftOnce1,wxGBPosition(4,0),wxGBSpan(1,3),wxEXPAND);
   for (i=0; i<11; i++)
   {
      m_keyButton[3][i]=new wxButton(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
      m_keyButton[3][i]->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(OSKeyboard::OnButtonClicked),NULL,this);
      gbSizer->Add(m_keyButton[3][i],wxGBPosition(4,i*2+3),wxGBSpan(1,2),wxEXPAND);
   }
   m_shiftOnce2=new wxToggleButton(this,ID_SHIFT2,_T("/\\"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
//   m_shiftOnce2=new wxBitmapButton(this,ID_SHIFT2,wxArtProvider::GetBitmap(wxART_GO_UP),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
   gbSizer->Add(m_shiftOnce2,wxGBPosition(4,25),wxGBSpan(1,5),wxEXPAND);

   m_space=new wxButton(this,ID_SPACE,_T(" "),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
   gbSizer->Add(m_space,wxGBPosition(5,7),wxGBSpan(1,16),wxEXPAND);
   setKbdValues(false);

   for (i=1; i<6; i++) gbSizer->AddGrowableRow(i,1);
   for (i=0; i<29; i++) gbSizer->AddGrowableCol(i,1);
   gbSizer->AddGrowableCol(29,2);
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
   setOptPosition((wxWindow*)m_input);
   m_keyButton[0][0]->GetSize(&x,&y);
   m_enter->GetSize(&ex,&ey);
   ex=abs(x-ex);
   GetSize(&x,&y);
   SetSize(x+ex,y);
}



OSKeyboard::~OSKeyboard()
{

}



void OSKeyboard::setOptPosition(wxWindow *win)
{
   wxInt32 inx,iny,myx,myy,mainx,mainy,myposx,myposy,inposx,inposy;

   win->GetSize(&inx,&iny);
   win->GetPosition(&inposx,&inposy);
   GetSize(&myx,&myy);
   GetPosition(&myposx,&myposy);
   g_mainWin->GetSize(&mainx,&mainy);

   myposx=inposx+(inx/2)-(myx/2);
   if (myposx<1) myposx=1;
   else if (myposx+myx>=mainx) myposx=mainx-myx-1;
   if (inposy+iny+myy<mainy) Move(myposx,inposy+iny+1);
   else if (inposy-myy>0) Move(myposx,inposy-myy-1);
}



void OSKeyboard::setKbdValues(bool shift)
{
   wxByte  shiftIdx=0;
   wxInt32 i;

   if (shift) shiftIdx=1;
   for (i=0; i<13; i++)
   {
      m_keyButton[0][i]->SetLabel(m_row[shiftIdx][0].Mid(i,1));
      if (m_keyButton[0][i]->GetLabel().Cmp(_T(" "))==0) m_keyButton[0][i]->Show(false);
   }
   for (i=0; i<12; i++)
   {
      m_keyButton[1][i]->SetLabel(m_row[shiftIdx][1].Mid(i,1));
      if (m_keyButton[1][i]->GetLabel().Cmp(_T(" "))==0) m_keyButton[1][i]->Show(false);
   }
   for (i=0; i<12; i++)
   {
      m_keyButton[2][i]->SetLabel(m_row[shiftIdx][2].Mid(i,1));
      if (m_keyButton[2][i]->GetLabel().Cmp(_T(" "))==0) m_keyButton[2][i]->Show(false);
   }
   for (i=0; i<11; i++)
   {
      m_keyButton[3][i]->SetLabel(m_row[shiftIdx][3].Mid(i,1));   
      if (m_keyButton[3][i]->GetLabel().Cmp(_T(" "))==0) m_keyButton[3][i]->Show(false);
   }
}



void OSKeyboard::OnButtonClicked(wxCommandEvent &event)
{
   if (event.GetId()==ID_SHIFT1)
   {
      m_shiftOnce2->SetValue(m_shiftOnce1->GetValue());
      setKbdValues(m_shiftOnce2->GetValue());
   }
   else if (event.GetId()==ID_SHIFT2)
   {
      m_shiftOnce1->SetValue(m_shiftOnce2->GetValue());
      setKbdValues(m_shiftOnce2->GetValue());
   }
   else if (event.GetId()==ID_CAPS_LOCK)
   {
      setKbdValues(m_capsLock->GetValue());
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
      wxInt32 i;

      if (event.GetId()==m_space->GetId())
      {
         *m_inputField<<_T(" ");
      }
      else
      {
         bool found=false;

         for (i=0; i<13; i++)
          if (m_keyButton[0][i]->GetId()==event.GetId())
         {
            *m_inputField<<m_keyButton[0][i]->GetLabel();
            found=true;
            break;
         }
         if (!found) for (i=0; i<12; i++)
          if (m_keyButton[1][i]->GetId()==event.GetId())
         {
            *m_inputField<<m_keyButton[1][i]->GetLabel();
            found=true;
            break;
         }
         if (!found) for (i=0; i<12; i++)
          if (m_keyButton[2][i]->GetId()==event.GetId())
         {
            *m_inputField<<m_keyButton[2][i]->GetLabel();
            found=true;
            break;
         }
         if (!found) for (i=0; i<11; i++)
          if (m_keyButton[3][i]->GetId()==event.GetId())
         {
            *m_inputField<<m_keyButton[3][i]->GetLabel();
            found=true;
            break;
         }
      
         if ((m_shiftOnce1->GetValue()) || (m_shiftOnce2->GetValue()))
         {
            m_shiftOnce1->SetValue(false);
            m_shiftOnce2->SetValue(false);
            m_capsLock->SetValue(false);
            setKbdValues(m_shiftOnce2->GetValue());
         }
      }
   }
}

