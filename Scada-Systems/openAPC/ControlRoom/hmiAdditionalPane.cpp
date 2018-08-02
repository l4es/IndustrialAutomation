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

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif
#include "iff.h"
#include "globals.h"
#include "oapc_libio.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiTabbedPane.h"
#include "hmiStackedPane.h"
#include "hmiAdditionalPane.h"
#include "BasePanel.h"
#include "wx/wxFlatNotebook/wxFlatNotebook.h"

#include <wx/panel.h>
#include <wx/event.h>


hmiAdditionalPane::hmiAdditionalPane(BasePanel *parent,flowObject *obj)
                  :hmiStackedPane(parent,obj)
{
   this->data.type=HMI_TYPE_ADDITIONALPANE;
   this->data.stdIN=OAPC_DIGI_IO0;
   this->data.stdOUT=OAPC_DIGI_IO0;
#ifdef ENV_EDITOR
   noUIFlags=OAPC_HMI_NO_UI_MINMAX|OAPC_HMI_NO_UI_RO|OAPC_HMI_NO_UI_FG|OAPC_HMI_NO_UI_FONT|OAPC_HMI_NO_UI_LAYOUT;
   this->name=wxString::Format(_T("%s %d"),getDefaultName().GetData(),this->data.id); // name is added only when the ID is assigned
   if (obj==NULL)
   {
       data.msizex=300*1000;
       data.msizey=200*1000;
       data.bgcolor[0]=0xF0F0F0; data.bgcolor[1]=0xF0F0F0;
       text[0]=getDefaultName(); 
       text[1]=getDefaultName();
   }
#endif
}



void hmiAdditionalPane::setPos(wxRealPoint WXUNUSED(pos))
{
}



wxWindow *hmiAdditionalPane::createUIElement()
{
   basePane=(hmiTabbedPane*)g_objectList.getObject(m_parent->m_myID);
#ifdef ENV_EDITOR
   if (!basePane) // here we had a context menu from the base HMICanvas but it was related to a selected sub-pane
   {
      flowObject *object;

      object=g_selectedList.getFirstObject();
      wxASSERT(object->data.type==HMI_TYPE_TABBEDPANE||object->data.type==HMI_TYPE_STACKEDPANE);
      if ((object->data.type!=HMI_TYPE_TABBEDPANE) && (object->data.type!=HMI_TYPE_STACKEDPANE)) return NULL;
      basePane=(hmiTabbedPane*)object;
      data.isChildOf=object->data.id;
   }
#endif
   wxASSERT(basePane);
   if (!basePane) return NULL;
   if ((basePane->data.type==HMI_TYPE_TABBEDPANE) || (basePane->data.type==HMI_TYPE_STACKEDPANE))
    uiElement=new HMICanvas(basePane->uiElement,data.id);
#ifdef ENV_EDITOR 
   else if (basePane->data.type==HMI_TYPE_ADDITIONALPANE) // why only in editor?
   {
      basePane=(hmiTabbedPane*)g_objectList.getObject(basePane->data.isChildOf);
      uiElement=new HMICanvas(basePane->uiElement,data.id);
   }
#endif
   else wxASSERT(0);
   setSize(basePane->getSize());
   hmiObject::setPos(wxRealPoint(basePane->getPanel()->GetPosition().x-1,basePane->getPanel()->GetPosition().y-1));
   uiElement->SetPosition(basePane->getPanel()->GetPosition());
   panelNumber=basePane->getPanelCount();
   ((hmiTabbedPane*)basePane)->addPanel(uiElement);

   hmiObject::createUIElement();
   applyData(1);
   return uiElement;
}



hmiAdditionalPane::~hmiAdditionalPane()
{

}



BasePanel *hmiAdditionalPane::getPanel()
{
   return (BasePanel*)uiElement;
}



#ifndef ENV_PLAYER
wxString hmiAdditionalPane::getDefaultName()
{
    return _T("Additional Pane");
}



wxInt32 hmiAdditionalPane::save(wxFile *FHandle)
{
   wxInt32 length;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_ADPA,4);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData);
   length=htonl(length);
   FHandle->Write(&length,4);
   length=hmiObject::save(FHandle);

   return length;
}
#endif



void hmiAdditionalPane::doApplyData(const wxByte WXUNUSED(all))
{
   uiElement->SetBackgroundColour(wxColour(data.bgcolor[digi[0]] & 0xFFFFFF));
   uiElement->SetForegroundColour(wxColour(data.fgcolor[digi[0]] & 0xFFFFFF));
   uiElement->Enable((data.state[digi[0]] & HMI_STATE_FLAG_DISABLED)==0);
   if (basePane->data.type==HMI_TYPE_TABBEDPANE)
    ((wxFlatNotebook*)basePane->uiElement)->SetPageText(panelNumber,g_res->getString(text[digi[0]]));
#ifdef ENV_EDITOR
   ((HMICanvas*)uiElement)->displayName(name);
#endif
}


void hmiAdditionalPane::setPageSelected(wxWindow *panel)
{
	basePane->setPageSelected(panel);
}



#ifdef ENV_EDITOR
void hmiAdditionalPane::setPageSelected(wxInt32 panelNum)
{
	basePane->setPageSelected(panelNum);
}



wxInt32 hmiAdditionalPane::getPageSelected()
{
	return basePane->getPageSelected();
}
#else



void hmiAdditionalPane::setUIElementState(wxInt32 state)
{   
   wxASSERT(basePane);
   if (!basePane) return;
   wxASSERT(basePane->uiElement);
   if (!basePane->uiElement) return;
   if (state==2) ((wxFlatNotebook*)basePane->uiElement)->Enable(panelNumber,true);
   else if (state==1) ((wxFlatNotebook*)basePane->uiElement)->Enable(panelNumber,false);
}
#endif



wxInt32 hmiAdditionalPane::getPanelCount()
{
	return basePane->getPanelCount();
}



