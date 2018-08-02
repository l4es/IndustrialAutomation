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

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif
#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "globals.h"
#include "oapc_libio.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "LogDataCtrl.h"
#include "hmiSpecialPanel.h"
#include "hmiUserMgmntPanel.h"


hmiSpecialPanel::hmiSpecialPanel(BasePanel *parent,flowObject *obj,wxInt32 type)
                :hmiObject(parent,obj)
{
   this->data.type=type;
   this->data.stdIN=OAPC_DIGI_IO0;
   this->data.stdOUT=OAPC_DIGI_IO0;
#ifdef ENV_EDITOR
   if (data.type==HMI_TYPE_SP_USERMANAGEMENT) g_hmiUserMgmntPanel=this;
   this->data.logFlags=0;
   noUIFlags=OAPC_HMI_NO_UI_MINMAX|OAPC_HMI_NO_UI_RO|OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_FG;
   if (obj==NULL)
   {
       this->data.msizex=490*1000;
       this->data.msizey=420*1000;
       text[0]=getDefaultName();
       text[1]=getDefaultName();
       this->data.bgcolor[0]=g_objectList.m_projectData->bgCol;
       this->data.bgcolor[1]=g_objectList.m_projectData->bgCol;
   }
#endif
}



wxWindow *hmiSpecialPanel::createUIElement()
{
   uiElement=new hmiUserMgmntPanel(m_parent);
   hmiObject::createUIElement();
#ifdef ENV_PLAYER
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(HMICanvas::OnButtonClicked));
#endif
   return uiElement;
}



hmiSpecialPanel::~hmiSpecialPanel()
{
#ifdef ENV_EDITOR
   if (data.type==HMI_TYPE_SP_USERMANAGEMENT) g_hmiUserMgmntPanel=NULL;
#endif
}



#ifdef ENV_EDITOR
wxString hmiSpecialPanel::getDefaultName()
{
   if (data.type==HMI_TYPE_SP_USERMANAGEMENT) return _T("User Management Panel");
   else wxASSERT(0);
   return _T("###undefined###");
}



wxInt32 hmiSpecialPanel::save(wxFile *FHandle)
{
   wxInt32 length;

   if (!FHandle) return 0;

   if (data.type==HMI_TYPE_SP_USERMANAGEMENT) FHandle->Write(CHUNK_HOBL_UMGM,4);
   else wxASSERT(0);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData);
   length=htonl(length);
   FHandle->Write(&length,4);
   length=hmiObject::save(FHandle);

   return length;
}
#endif



void hmiSpecialPanel::enable(wxWindow *ui,bool doEnable)
{
   wxWindowList      winList;
   wxWindowListNode *node;

   if (!doEnable)
   {
      ui->Enable(doEnable);
      winList=ui->GetChildren();
      node=winList.GetFirst();
      while (node)
      {
         enable((wxWindow*)node->GetData(),doEnable);
         node=node->GetNext();
      }
   }
   else
   {
      if (data.type==HMI_TYPE_SP_USERMANAGEMENT) ((hmiUserMgmntPanel*)uiElement)->updateUI();
      else wxASSERT(0);
   }
}



void hmiSpecialPanel::doApplyData(const wxByte all)
{
   if (all)
   {
      if (!applyLayoutData())
      {
         setSize(wxRealPoint(data.msizex,data.msizey));
         setPos(wxRealPoint(data.mposx,data.mposy));
      }
   }
   uiElement->SetBackgroundColour(wxColour(data.bgcolor[digi[0]] & 0xFFFFFF));
   applyFont(uiElement);
#ifdef ENV_EDITOR
   enable(uiElement,false);
#else
   enable(uiElement,(data.state[digi[0]] & HMI_STATE_FLAG_DISABLED)==0);
#endif
}



void hmiSpecialPanel::applyFont(wxWindow *ui)
{
   wxFont font;

   hmiObject::applyFont(ui);
   font=ui->GetFont();
   g_setFont(ui,font);
   ui->Layout();
}

