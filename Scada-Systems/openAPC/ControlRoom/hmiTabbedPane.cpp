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
#include "wx/wxFlatNotebook/wxFlatNotebook.h"

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
#include "hmiTabbedPane.h"
#include "BasePanel.h"


// TODO: make hmiTabbedPane inherit from hmiSinglePanel, getUniqueID() etc. has to be removed for that

hmiTabbedPane::hmiTabbedPane(BasePanel *parent,flowObject *obj)
              :hmiObject(parent,obj)
{
   if (data.id==0)
   {
      data.id=g_objectList.getUniqueID(); // this object actively retrieves the ID because it need to be available when the BasePanel is created
#ifdef ENV_EDITOR
      name=wxString::Format(_T("%s %d"),getDefaultName().GetData(),this->data.id); // name is added only when the ID is assigned
#endif
   }
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5;
   this->data.stdOUT=OAPC_DIGI_IO0;
#ifdef ENV_EDITOR
   this->data.type=HMI_TYPE_TABBEDPANE;
   noUIFlags=OAPC_HMI_NO_UI_MINMAX|OAPC_HMI_NO_UI_RO|OAPC_HMI_NO_UI_FG|OAPC_HMI_NO_UI_FONT;
   m_userPriviEnable=OAPC_USERPRIVI_DISABLE;
   if (obj==NULL)
   {
       data.msizex=300*1000;           
       data.msizey=200*1000;
       data.bgcolor[0]=0xF0F0F0; data.bgcolor[1]=0xF0F0F0;
       text[0]=getDefaultName(); text[1]=getDefaultName();
   }
#endif
}



wxWindow *hmiTabbedPane::createUIElement()
{
#ifdef ENV_EDITOR
   uiElement=new wxFlatNotebook(m_parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxFNB_TABS_BORDER_SIMPLE/*|wxFNB_TABS_DISABLED|wxFNB_DROPDOWN_TABS_LIST*/);
#else
   uiElement=new wxFlatNotebook(m_parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxFNB_TABS_BORDER_SIMPLE);
#endif
/*
#define wxFNB_TABS_BORDER_SIMPLE                0x00000004
#define wxFNB_BOTTOM                            0x00000040
#define wxFNB_BACKGROUND_GRADIENT               0x00000400
*/
   hmiObject::createUIElement();
#ifdef ENV_EDITOR
   m_firstPanel=new HMICanvas(uiElement,data.id);
#else
   m_firstPanel=new BasePanel(uiElement,data.id);
#endif
   ((wxFlatNotebook*)uiElement)->AddPage(m_firstPanel,g_res->getString(text[0]), true);

//   hmiObject::createUIElement();
   applyData(1);
   return uiElement;
}



hmiTabbedPane::~hmiTabbedPane()
{
//   while (((wxBookCtrl*)uiElement)->GetPageCount()>1)
//    ((wxBookCtrl*)uiElement)->RemovePage(1);
}



#ifndef ENV_PLAYER
wxString hmiTabbedPane::getDefaultName()
{
    return _("Tabbed Pane");
}



wxInt32 hmiTabbedPane::save(wxFile *FHandle)
{
   wxInt32 length;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_TAPA,4);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData);
   length=htonl(length);
   FHandle->Write(&length,4);
   length=hmiObject::save(FHandle);

   return length;
}
#endif



void hmiTabbedPane::doApplyData(const wxByte all)
{
   wxNotebookPage *page=((wxFlatNotebook*)uiElement)->GetPage(0);
   if (all)
   {
      if (!applyLayoutData())
      {
         setSize(wxRealPoint(data.msizex,data.msizey));
         setPos(wxRealPoint(data.mposx,data.mposy));
      }
   }
   page->SetBackgroundColour(wxColour(data.bgcolor[digi[0]] & 0xFFFFFF));
   uiElement->SetBackgroundColour(wxColour(data.bgcolor[digi[0]] & 0xFFFFFF));
   applyFont(uiElement);
   ((wxFlatNotebook*)uiElement)->SetPageText(0,g_res->getString(text[digi[0]]));
   page->Enable((data.state[digi[0]] & HMI_STATE_FLAG_DISABLED)==0);
}



BasePanel *hmiTabbedPane::getPanel()
{
   return m_firstPanel;
}



wxInt32 hmiTabbedPane::getPanelCount()
{
   return ((wxFlatNotebook*)uiElement)->GetPageCount();
}



void hmiTabbedPane::addPanel(wxWindow *panel)
{
   ((wxFlatNotebook*)uiElement)->AddPage(panel,_T(""),false);
}



void hmiTabbedPane::deletePanel(wxWindow *panel)
{
   wxInt32 i;

   for (i=0; i<((wxFlatNotebook*)uiElement)->GetPageCount(); i++)
   {
      if (((wxFlatNotebook*)uiElement)->GetPage(i)==panel)
      {
         ((wxFlatNotebook*)uiElement)->RemovePage(i);
         return;
      }
   }
}



void hmiTabbedPane::setPageSelected(wxWindow *panel)
{
   wxInt32 i;

   for (i=0; i<((wxFlatNotebook*)uiElement)->GetPageCount(); i++)
   {
      if (((wxFlatNotebook*)uiElement)->GetPage(i)==panel)
      {
         ((wxFlatNotebook*)uiElement)->SetSelection(i);
         return;
      }
   }
}



#ifdef ENV_EDITOR
void hmiTabbedPane::setPageSelected(wxInt32 panelNum)
{
   ((wxFlatNotebook*)uiElement)->SetSelection(panelNum);
}



int hmiTabbedPane::getPageSelected()
{
   return ((wxFlatNotebook*)uiElement)->GetSelection();
}
#endif



void hmiTabbedPane::setPageEnabled(wxWindow *panel,wxByte enable)
{
   wxInt32 i;

   for (i=0; i<((wxFlatNotebook*)uiElement)->GetPageCount(); i++)
   {
      if (((wxFlatNotebook*)uiElement)->GetPage(i)==panel)
      {
         ((wxFlatNotebook*)uiElement)->Enable(i,enable!=0);
         uiElement->Refresh();
         return;
      }
   }
}



#ifdef ENV_PLAYER
void hmiTabbedPane::setUIElementState(wxInt32 state)
{   
   if (state==2) ((wxFlatNotebook*)uiElement)->Enable(0,true);
   else if (state==1) ((wxFlatNotebook*)uiElement)->Enable(0,false);
   else if (state==0) {} // no possibility to disable a tabbed pane
   else wxASSERT(0);
}



/**
Sets an numerical input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte hmiTabbedPane::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   if (connection->targetInputNum==3) // SEL
    ((wxFlatNotebook*)uiElement)->SetSelection(value);
   else if (connection->targetInputNum==4) // EN
   {
      ((wxFlatNotebook*)uiElement)->Enable(value,true);
      uiElement->Refresh();
   }
   else if (connection->targetInputNum==5) // DIS
   {
      ((wxFlatNotebook*)uiElement)->Enable(value,false);
      uiElement->Refresh();
   }
   return 0;
}


#endif
