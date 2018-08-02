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

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiHGauge.h"
#include "hmiVGauge.h"
#include "oapc_libio.h"


hmiVGauge::hmiVGauge(BasePanel *parent,flowObject *obj)
          :hmiHGauge(parent,obj)
{
   this->data.type=HMI_TYPE_VGAUGE;
   this->data.stdIN =OAPC_DIGI_IO0|OAPC_NUM_IO7;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_NUM_IO7;
#ifdef ENV_EDITOR
   noUIFlags=OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_FONT;
   m_userPriviEnable=OAPC_USERPRIVI_HIDE;
   if (obj==NULL)
   {
       this->data.msizex=30*1000;
       this->data.msizey=150*1000;
       MinValue=0;
       MaxValue=100.0;
   }
#endif
}


wxWindow *hmiVGauge::createUIElement()
{
   uiElement=new wxGauge(m_parent,wxID_ANY,MaxValue-MinValue,wxDefaultPosition,wxDefaultSize,wxGA_VERTICAL|wxGA_SMOOTH);
   hmiObject::createUIElement();
   return uiElement;
}



hmiVGauge::~hmiVGauge()
{
}



#ifdef ENV_EDITOR
wxString hmiVGauge::getDefaultName()
{
    return _("Vertical Gauge");
}



wxInt32 hmiVGauge::save(wxFile *FHandle)
{
   wxInt32 length;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_VGAG,4);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData);
   length=htonl(length);
   FHandle->Write(&length,4);
   length=hmiObject::save(FHandle);

   return length;
}
#endif




