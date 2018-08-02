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
#include <wx/filename.h>
#include <wx/artprov.h>
#include <wx/spinctrl.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "globals.h"
#include "MainWin.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiPrimitive.h"
#include "oapc_libio.h"
#include "globals.h"


hmiPrimitive::hmiPrimitive(BasePanel *parent,flowObject *obj,wxUint32 type)
         :hmiObject(parent,obj)
{
   this->data.type=type;
#ifdef ENV_EDITOR
   noUIFlags=OAPC_HMI_NO_UI_MINMAX|OAPC_HMI_NO_UI_RO|OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_FG|OAPC_HMI_NO_UI_FONT|OAPC_HMI_NO_UI_LAYOUT;
   m_userPriviEnable=OAPC_USERPRIVI_HIDE;
   if (obj==NULL)
   {
       this->data.msizex=100*1000;
       this->data.msizey=80*1000;
       memset(&freeLineData,0,sizeof(hmiPrimitiveData));
       freeLineData.m_lineSize=5;
       freeLineData.m_arrowSize[0]=12;
       freeLineData.m_arrowSize[1]=12;
       this->data.bgcolor[0]=0x000088;
   }
#endif
   m_isVisible=true;
}



hmiPrimitive::~hmiPrimitive()
{
}



#ifndef ENV_PLAYER
void hmiPrimitive::setData(flowObject *object)
{
   freeLineData.m_mode=((hmiPrimitive*)object)->freeLineData.m_mode;
   freeLineData.m_arrow[0]=((hmiPrimitive*)object)->freeLineData.m_arrow[0];
   freeLineData.m_arrow[1]=((hmiPrimitive*)object)->freeLineData.m_arrow[1];
   freeLineData.m_lineSize=((hmiPrimitive*)object)->freeLineData.m_lineSize;
   flowObject::setData(object);
}



wxString hmiPrimitive::getDefaultName()
{
   switch (data.type)
   {
      case HMI_TYPE_FRAME:
         return _T("Frame");
      case HMI_TYPE_ELLIPSE:
         return _T("Ellipse");
      default:
         assert(0);
      case HMI_TYPE_FREELINE:
         return _T("Free Line");
   }
}



wxInt32 hmiPrimitive::save(wxFile *FHandle)
{
   wxInt32                length;
   struct hmiPrimitiveData convData;

   if (!FHandle) return 0;

   if (data.type==HMI_TYPE_FREELINE) FHandle->Write(CHUNK_HOBL_FLIN,4);
   else if (data.type==HMI_TYPE_FRAME) FHandle->Write(CHUNK_HOBL_FRAM,4);
   else if (data.type==HMI_TYPE_ELLIPSE) FHandle->Write(CHUNK_HOBL_ELIP,4);
   else
   {
      assert(0);
      return 0;
   }
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData)+sizeof(struct hmiPrimitiveData);
   length=htonl(length);
   FHandle->Write(&length,4);

   convData.m_mode=freeLineData.m_mode;
   convData.m_arrow[0]=freeLineData.m_arrow[0];
   convData.m_arrow[1]=freeLineData.m_arrow[1];
   convData.m_lineSize=freeLineData.m_lineSize;
   convData.m_arrowSize[0]=freeLineData.m_arrowSize[0];
   convData.m_arrowSize[1]=freeLineData.m_arrowSize[1];
   convData.res1=0;
   convData.res2=0;
   length=FHandle->Write(&convData,sizeof(struct hmiPrimitiveData));

   length=length+hmiObject::save(FHandle);

   return length;
}
#endif



wxInt32 hmiPrimitive::load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset)
{
    struct hmiPrimitiveData convData;
    wxUint32               myChunkSize;

    myChunkSize=chunkSize;
    if (myChunkSize>sizeof(struct hmiPrimitiveData)) myChunkSize=sizeof(struct hmiPrimitiveData);

    myChunkSize=FHandle->Read(&convData,myChunkSize);
    freeLineData.m_mode=convData.m_mode;
    freeLineData.m_arrow[0]=convData.m_arrow[0];
    freeLineData.m_arrow[1]=convData.m_arrow[1];
    freeLineData.m_lineSize=convData.m_lineSize;
    freeLineData.m_arrowSize[0]=convData.m_arrowSize[0];
    freeLineData.m_arrowSize[1]=convData.m_arrowSize[1];
    freeLineData.res1=0;
    freeLineData.res2=0;

    return hmiObject::load(FHandle,chunkSize-myChunkSize,IDOffset)+myChunkSize;

}



wxWindow *hmiPrimitive::createUIElement()
{
   return NULL;
}



void hmiPrimitive::doApplyData(const wxByte all)
{
   if (all)
   {
      setSize(wxRealPoint(data.msizex,data.msizey));
      setPos(wxRealPoint(data.mposx,data.mposy));
   }
   hmiObject::doApplyData(all);
}


void hmiPrimitive::drawObject(wxAutoBufferedPaintDC *dc)
{
   wxPen  pen(wxColour(data.bgcolor[digi[0]]),freeLineData.m_lineSize);
   double x,y,w,h;

   dc->SetPen(pen);
   dc->SetBrush(*wxTRANSPARENT_BRUSH);
   switch (data.type)
   {
      case HMI_TYPE_FREELINE:
         x=getPos().x/1000.0;  y=getPos().y/1000.0;
         w=getSize().x/1000.0; h=getSize().y/1000.0;
         if (freeLineData.m_mode==0)
         {
            dc->DrawLine(x,y+h,x+w,y);
            pen.SetWidth(1);
            dc->SetPen(pen);
            dc->SetBrush(wxBrush(data.bgcolor[digi[0]]));
            if (freeLineData.m_arrow[0]==1) dc->DrawCircle(x,y+h,freeLineData.m_arrowSize[0]/2);
            else if (freeLineData.m_arrow[0]==2) dc->DrawRectangle(x-freeLineData.m_arrowSize[0]/2,y+h-freeLineData.m_arrowSize[0]/2,freeLineData.m_arrowSize[0],freeLineData.m_arrowSize[0]);
            if (freeLineData.m_arrow[1]==1) dc->DrawCircle(x+w,y,freeLineData.m_arrowSize[1]/2);
            else if (freeLineData.m_arrow[1]==2) dc->DrawRectangle(x+w-freeLineData.m_arrowSize[1]/2,y-freeLineData.m_arrowSize[1]/2,freeLineData.m_arrowSize[1],freeLineData.m_arrowSize[1]);
         }
         else
         {
            dc->DrawLine(x,y,x+w,y+h);
            pen.SetWidth(1);
            dc->SetPen(pen);
            dc->SetBrush(wxBrush(data.bgcolor[digi[0]]));
            if (freeLineData.m_arrow[0]==1) dc->DrawCircle(x,y,freeLineData.m_arrowSize[0]/2);
            else if (freeLineData.m_arrow[0]==2) dc->DrawRectangle(x-freeLineData.m_arrowSize[0]/2,y-freeLineData.m_arrowSize[0]/2,freeLineData.m_arrowSize[0],freeLineData.m_arrowSize[0]);
            if (freeLineData.m_arrow[1]==1) dc->DrawCircle(x+w,y+h,freeLineData.m_arrowSize[1]/2);
            else if (freeLineData.m_arrow[1]==2) dc->DrawRectangle(x+w-freeLineData.m_arrowSize[1]/2,y+h-freeLineData.m_arrowSize[1]/2,freeLineData.m_arrowSize[1],freeLineData.m_arrowSize[1]);
         }
         break;
      case HMI_TYPE_FRAME:
      case HMI_TYPE_ELLIPSE:
         x=(getPos().x/1000.0)+(freeLineData.m_lineSize/2.0);  y=(getPos().y/1000.0)+(freeLineData.m_lineSize/2.0);
         w=(getSize().x/1000.0)-freeLineData.m_lineSize;       h=(getSize().y/1000.0)-freeLineData.m_lineSize;
         if (data.type==HMI_TYPE_FRAME) dc->DrawRectangle(x,y,w,h);
         else dc->DrawEllipse(x,y,w,h);
         break;
      default:
         assert(0);
         break;
   }
}


#ifdef ENV_EDITOR
wxPoint hmiPrimitive::getMinSize()
{
   if (data.type==HMI_TYPE_FREELINE) return wxPoint(0,0);
   else
   {
      return wxPoint(freeLineData.m_lineSize*2,freeLineData.m_lineSize*2);
   }
}


wxPanel* hmiPrimitive::getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num)
{
   wxStaticText *text;
   wxInt32       ln=0,i;

   if (num>0) return NULL;
   wxPanel *panel=new wxPanel(parent);

   wxGridBagSizer *fSizer=new wxGridBagSizer(4,4);
   panel->SetSizer(fSizer);

   if (data.type==HMI_TYPE_FREELINE)
   {
      text=new wxStaticText(panel,wxID_ANY,_("Mode"));         fSizer->Add(text,wxGBPosition(ln,1),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
      text=new wxStaticText(panel,wxID_ANY,_("Size"));         fSizer->Add(text,wxGBPosition(ln,2),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
      ln++;

      text=new wxStaticText(panel,wxID_ANY,_("Line")+_T(":")); fSizer->Add(text,wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
      m_modeCombo=new wxComboBox(panel,wxID_ANY,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_DROPDOWN|wxCB_READONLY);
                                                               fSizer->Add(m_modeCombo,wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND);
      m_modeCombo->Append(_("Bottom to top"));
      m_modeCombo->Append(_("Top to bottom"));
      m_modeCombo->SetSelection(freeLineData.m_mode);

      m_lineSizeField=new wxSpinCtrl(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,1,50,freeLineData.m_lineSize);
                                                            fSizer->Add(m_lineSizeField,wxGBPosition(ln,2),wxGBSpan(1,1));
      m_lineSizeField->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
      m_lineSizeField->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED,wxCommandEventHandler(hmiPrimitive::OnCommandEvent),NULL,this);
      ln++;

      for (i=0; i<2; i++)
      {
         if (i==0) text=new wxStaticText(panel,wxID_ANY,_("Line Start")+_T(":"));
         else text=new wxStaticText(panel,wxID_ANY,_("Line End")+_T(":"));
                                                            fSizer->Add(text,wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
         m_arrowCombo[i]=new wxComboBox(panel,wxID_ANY,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_DROPDOWN|wxCB_READONLY);
                                                            fSizer->Add(m_arrowCombo[i],wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND);
         m_arrowCombo[i]->Append(_("None"));
         m_arrowCombo[i]->Append(_("Circle"));
         m_arrowCombo[i]->Append(_("Square"));
         m_arrowCombo[i]->SetSelection(freeLineData.m_arrow[i]);

         m_arrowSizeField[i]=new wxSpinCtrl(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,2,51,freeLineData.m_arrowSize[i]);
                                                            fSizer->Add(m_arrowSizeField[i],wxGBPosition(ln,2),wxGBSpan(1,1));
         m_arrowSizeField[i]->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
         m_arrowSizeField[i]->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED,wxCommandEventHandler(hmiPrimitive::OnCommandEvent),NULL,this);
         ln++;
      }
   }
   else
   {
      text=new wxStaticText(panel,wxID_ANY,_("Size"));         fSizer->Add(text,wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
      m_lineSizeField=new wxSpinCtrl(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,1,50,freeLineData.m_lineSize);
      fSizer->Add(m_lineSizeField,wxGBPosition(ln,1),wxGBSpan(1,1));
      m_lineSizeField->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
      m_lineSizeField->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED,wxCommandEventHandler(hmiPrimitive::OnCommandEvent),NULL,this);
      ln++;

   }

   fSizer->AddGrowableCol(1);
   fSizer->AddGrowableCol(2);

   *name=_("Configuration");
   return panel;
}


void hmiPrimitive::OnCommandEvent(wxCommandEvent &event)
{
   wxInt32 val;

   if (data.type==HMI_TYPE_FREELINE)
   {
      if (event.GetId()==m_lineSizeField->GetId())
      {
         val=m_lineSizeField->GetValue();
         if (m_arrowSizeField[0]->GetValue()<=val) m_arrowSizeField[0]->SetValue(val+1);
         if (m_arrowSizeField[1]->GetValue()<=val) m_arrowSizeField[1]->SetValue(val+1);
      }
      else if (event.GetId()==m_arrowSizeField[0]->GetId())
      {
         val=m_arrowSizeField[0]->GetValue();
         if (m_lineSizeField->GetValue()>=val) m_lineSizeField->SetValue(val-1);
      }
      else if (event.GetId()==m_arrowSizeField[1]->GetId())
      {
         val=m_arrowSizeField[1]->GetValue();
         if (m_lineSizeField->GetValue()>=val) m_lineSizeField->SetValue(val-1);
      }
   }
   else
   {
      if (event.GetId()==m_lineSizeField->GetId())
      {
         wxInt32 w,h;

         w=getSize().x/2000;
         h=getSize().y/2000;
         val=m_lineSizeField->GetValue();
         if (val>w) val=w;
         if (val>h) val=h;
         m_lineSizeField->SetValue(val);
      }
   }
}


void hmiPrimitive::setConfigData()
{
   freeLineData.m_lineSize=m_lineSizeField->GetValue();
   if (data.type==HMI_TYPE_FREELINE)
   {
      freeLineData.m_mode=m_modeCombo->GetSelection();
      freeLineData.m_arrowSize[0]=m_arrowSizeField[0]->GetValue();
      freeLineData.m_arrowSize[1]=m_arrowSizeField[1]->GetValue();
      freeLineData.m_arrow[0]=m_arrowCombo[0]->GetSelection();
      freeLineData.m_arrow[1]=m_arrowCombo[1]->GetSelection();
   }
}

#else


void hmiPrimitive::setUIElementState(wxInt32 state)
{
   if (state==0) m_isVisible=false;
   else m_isVisible=true;
}

#endif

