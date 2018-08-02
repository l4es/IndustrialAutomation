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
#include "hmiStackedPane.h"
#include "BasePanel.h"


hmiStackedPane::hmiStackedPane(BasePanel *parent,flowObject *obj)
               :hmiTabbedPane(parent,obj)
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
   this->data.type=HMI_TYPE_STACKEDPANE;
   noUIFlags=OAPC_HMI_NO_UI_MINMAX|OAPC_HMI_NO_UI_RO|OAPC_HMI_NO_UI_FG|OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_FONT;

   if (obj==NULL)
   {
       data.msizex=300*1000;           
       data.msizey=200*1000;
       data.bgcolor[0]=0xF0F0F0; data.bgcolor[1]=0xF0F0F0;
       text[0]=getDefaultName(); text[1]=getDefaultName();
   }
#endif
}



wxWindow *hmiStackedPane::createUIElement()
{
   uiElement=new wxFlatNotebook(m_parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxFNB_TABS_BORDER_SIMPLE|wxFNB_TABS_DISABLED);
#ifdef ENV_EDITOR
   m_firstPanel=new HMICanvas(uiElement,data.id);
#else
   m_firstPanel=new BasePanel(uiElement,data.id);
#endif
   ((wxFlatNotebook*)uiElement)->AddPage(m_firstPanel,g_res->getString(text[0]), true);

   hmiObject::createUIElement();
   applyData(1);
   return uiElement;
}



hmiStackedPane::~hmiStackedPane()
{

}



#ifndef ENV_PLAYER
wxString hmiStackedPane::getDefaultName()
{
    return _("Stacked Pane");
}



wxInt32 hmiStackedPane::save(wxFile *FHandle)
{
   wxInt32 length;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_STPA,4);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData);
   length=htonl(length);
   FHandle->Write(&length,4);
   length=hmiObject::save(FHandle);

   return length;
}
#endif




