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
#include <wx/notebook.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif
#include "iff.h"
#include "globals.h"
#include "oapc_libio.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiSinglePanel.h"
#include "BasePanel.h"


hmiSinglePanel::hmiSinglePanel(BasePanel *parent,flowObject *obj)
               :hmiObject(parent,obj)
{
   if (data.id==0)
   {
      data.id=g_objectList.getUniqueID(); // this object actively retrieves the ID because it need to be available when the BasePanel is created
#ifdef ENV_EDITOR
      name=wxString::Format(_T("%s %d"),getDefaultName().GetData(),this->data.id); // name is added only when the ID is assigned
#endif
   }
   this->m_parent=parent;
   this->data.stdIN=OAPC_DIGI_IO0;
   this->data.stdOUT=OAPC_DIGI_IO0;
#ifdef ENV_EDITOR
   this->data.type=HMI_TYPE_SINGLEPANEL;
   noUIFlags=OAPC_HMI_NO_UI_MINMAX|OAPC_HMI_NO_UI_RO|OAPC_HMI_NO_UI_FONT|OAPC_HMI_NO_UI_TEXT;
   if (obj==NULL)
   {
      panelData.borderNum=1;
      panelData.reserved=0;
      data.msizex=300*1000;
      data.msizey=200*1000;
      data.bgcolor[0]=0xFFF0F0; data.bgcolor[1]=0xFFF0F0;
      data.fgcolor[0]=0x050000; data.fgcolor[1]=0x050000;
      text[0]=getDefaultName(); text[1]=getDefaultName();
   }
#endif
}



wxWindow *hmiSinglePanel::createUIElement()
{
#ifdef ENV_EDITOR
   uiElement=new HMICanvas(m_parent,data.id);
#else
   uiElement=new BasePanel(m_parent,data.id);
#endif
   updateBorder();       
   hmiObject::createUIElement();
   applyData(1);
   return uiElement;
}



hmiSinglePanel::~hmiSinglePanel()
{
}



#ifdef ENV_EDITOR
void hmiSinglePanel::setData(flowObject *object)
{
   memcpy(&panelData,&((hmiSinglePanel*)object)->panelData,sizeof(struct hmiSinglePanelData));
   flowObject::setData(object);
}



wxString hmiSinglePanel::getDefaultName()
{
    return _T("Single Panel");
}



wxInt32 hmiSinglePanel::save(wxFile *FHandle)
{
   wxInt32                   length;
   struct hmiSinglePanelData convData;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_SIPA,4);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData)+sizeof(struct hmiSinglePanelData);
   length=htonl(length);
   FHandle->Write(&length,4);

   convData.borderNum=panelData.borderNum; // byte
   convData.reserved=htonl(panelData.reserved);

   length=FHandle->Write(&convData,sizeof(struct hmiSinglePanelData));
   length=length+hmiObject::save(FHandle);

   return length;
}



wxPanel* hmiSinglePanel::getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num)
{
   if (num>0) return NULL;

   wxPanel *panel=new wxPanel(parent);

   wxFlexGridSizer *fSizer=new wxFlexGridSizer(2,4,4);

   wxStaticText *text=new wxStaticText(panel,wxID_ANY,_("Border")+_T(":"));
   fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

   comboBox=new wxComboBox(panel,wxID_ANY,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_DROPDOWN|wxCB_READONLY);
   comboBox->Append(_("No Border"));
   comboBox->Append(_("Sunken"));
   comboBox->Append(_("Raised"));
   comboBox->Append(_("Simple"));
   comboBox->Append(_("Double"));
   comboBox->Append(_("Dotted"));
   comboBox->Append(_("Short Dashes"));
   comboBox->Append(_("Dots and dashes"));
//   comboBox->Append(_("Long Dashes"));

   comboBox->Select(panelData.borderNum);
   fSizer->Add(comboBox,1,wxEXPAND);

   fSizer->AddGrowableCol(1,3);
   panel->SetSizer(fSizer);

   *name=_("Border");
   return panel;
}



void hmiSinglePanel::setConfigData()
{
    panelData.borderNum=comboBox->GetSelection();
    updateBorder();
}
#endif



void hmiSinglePanel::updateBorder()
{
   ((BasePanel*)uiElement)->setBorderNum(panelData.borderNum);
   uiElement->Refresh();
}



wxInt32 hmiSinglePanel::load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset)
{
   struct hmiSinglePanelData convData;
   wxUint32                  myChunkSize;

   myChunkSize=chunkSize;
   if (myChunkSize>sizeof(struct hmiSinglePanelData)) myChunkSize=sizeof(struct hmiSinglePanelData);
   myChunkSize=FHandle->Read(&convData,myChunkSize);
 

   panelData.borderNum=convData.borderNum; // byte
   panelData.reserved=ntohl(convData.reserved);

   return hmiObject::load(FHandle,chunkSize-myChunkSize,IDOffset)+myChunkSize;
}



void hmiSinglePanel::doApplyData(const wxByte all)
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
   uiElement->SetForegroundColour(wxColour(data.fgcolor[digi[0]] & 0xFFFFFF));
   applyFont(uiElement);
}


