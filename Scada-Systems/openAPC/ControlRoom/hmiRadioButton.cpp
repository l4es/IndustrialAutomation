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
#include <wx/radiobut.h>
#include <wx/checkbox.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "globals.h"
#include "oapc_libio.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiRadioButton.h"


hmiRadioButton::hmiRadioButton(BasePanel *parent,flowObject *obj,wxUint32 type)
               :hmiToggleButton(parent,obj)
{
   this->data.type=type;
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_CHAR_IO3;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_DIGI_IO1;
#ifdef ENV_EDITOR
   noUIFlags=OAPC_HMI_NO_UI_MINMAX|OAPC_HMI_NO_UI_RO;
   if (obj==NULL)
   {
       this->data.msizey=20*1000;
       text[0]=getDefaultName();
       text[1]=getDefaultName();
       this->data.bgcolor[0]=g_objectList.m_projectData->bgCol;
       this->data.bgcolor[1]=g_objectList.m_projectData->bgCol;
   }
#endif
}



wxWindow *hmiRadioButton::createUIElement()
{
   if (data.type==HMI_TYPE_RADIOBUTTON) uiElement=new wxRadioButton(m_parent,wxID_ANY,g_res->getString(text[0]));
   else if (data.type==HMI_TYPE_CHECKBOX) uiElement=new wxCheckBox(m_parent,wxID_ANY,g_res->getString(text[0]));
   else wxASSERT(0);
   hmiObject::createUIElement();
#ifdef ENV_PLAYER
   if (data.type==HMI_TYPE_RADIOBUTTON) g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_COMMAND_RADIOBUTTON_SELECTED,wxCommandEventHandler(HMICanvas::OnButtonClicked));
   else if (data.type==HMI_TYPE_CHECKBOX) g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_COMMAND_CHECKBOX_CLICKED,wxCommandEventHandler(HMICanvas::OnButtonClicked));
   else wxASSERT(0);
#endif
   return uiElement;
}



hmiRadioButton::~hmiRadioButton()
{
}



#ifdef ENV_EDITOR
wxString hmiRadioButton::getDefaultName()
{
   if (data.type==HMI_TYPE_RADIOBUTTON) return _("Radio Button");
   else if (data.type==HMI_TYPE_CHECKBOX) return _("Checkbox");
   else wxASSERT(0);
   return _T("##undefined##");
}



wxInt32 hmiRadioButton::save(wxFile *FHandle)
{
   wxInt32 length;

   if (!FHandle) return 0;

   if (data.type==HMI_TYPE_RADIOBUTTON) FHandle->Write(CHUNK_HOBL_RABU,4);
   else if (data.type==HMI_TYPE_CHECKBOX) FHandle->Write(CHUNK_HOBL_CKBX,4);
   else wxASSERT(0);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData);
   length=htonl(length);
   FHandle->Write(&length,4);
   length=hmiObject::save(FHandle);

   return length;
}
#endif
