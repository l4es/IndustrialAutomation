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
#include <wx/statline.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiSimpleButton.h"
#include "hmiLine.h"
#include "oapc_libio.h"


hmiLine::hmiLine(BasePanel *parent,flowObject *obj)
        :hmiObject(parent,obj)
{
   this->data.type=HMI_TYPE_LINE;
#ifdef ENV_EDITOR
   noUIFlags=OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_MINMAX|OAPC_HMI_NO_UI_RO|OAPC_HMI_NO_UI_FG|OAPC_HMI_NO_UI_FONT;
   m_userPriviEnable=OAPC_USERPRIVI_HIDE;
   if (obj==NULL)
   {
       this->data.msizex=100*1000;
       this->data.msizey=5*1000;
       this->data.bgcolor[0]=0x000088;
   }
#endif
}



wxWindow *hmiLine::createUIElement()
{
   uiElement=new wxPanel(m_parent,wxID_ANY);
   hmiObject::createUIElement();

   return uiElement;
}



hmiLine::~hmiLine()
{

}



#ifndef ENV_PLAYER
wxString hmiLine::getDefaultName()
{
    return _T("Rectangle");
}



wxInt32 hmiLine::save(wxFile *FHandle)
{
   wxInt32 length;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_LINE,4);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData);
   length=htonl(length);
   FHandle->Write(&length,4);
   length=hmiObject::save(FHandle);

   return length;
}



wxPoint hmiLine::getMinSize()
{
    return wxPoint(1,1);
}
#endif



void hmiLine::doApplyData(const wxByte all)
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
   uiElement->Refresh();
}
