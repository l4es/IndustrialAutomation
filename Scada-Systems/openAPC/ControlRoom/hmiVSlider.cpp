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
#include "globals.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiHSlider.h"
#include "hmiVSlider.h"
#include "oapc_libio.h"


hmiVSlider::hmiVSlider(BasePanel *parent,flowObject *obj)
           :hmiHSlider(parent,obj)
{
   this->data.type=HMI_TYPE_VSLIDER;
#ifdef ENV_EDITOR
   noUIFlags=OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_FONT;
   if (!obj)
   {
       this->data.msizex=30*1000;
       this->data.msizey=150*1000;
       MinValue=0;
       MaxValue=100.0;
   }
#endif
}



wxWindow *hmiVSlider::createUIElement()
{
   uiElement=new wxSlider(m_parent,wxID_ANY,(MinValue+MaxValue)/3.0,MinValue,MaxValue,wxDefaultPosition,wxDefaultSize,wxSL_RIGHT|wxSL_INVERSE|wxSL_VERTICAL);
   hmiObject::createUIElement();
#ifdef ENV_PLAYER
   // commands for "value has changed"
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_SCROLL_THUMBRELEASE,wxScrollEventHandler(HMICanvas::OnSliderScrolled));
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_SCROLL_LINEUP,wxScrollEventHandler(HMICanvas::OnSliderScrolled));
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_SCROLL_LINEDOWN,wxScrollEventHandler(HMICanvas::OnSliderScrolled));
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_SCROLL_PAGEUP,wxScrollEventHandler(HMICanvas::OnSliderScrolled));
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_SCROLL_PAGEDOWN,wxScrollEventHandler(HMICanvas::OnSliderScrolled));
   //commands for "value is changing"
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_SCROLL_THUMBTRACK,wxScrollEventHandler(HMICanvas::OnSliderScrolling));
#endif

   return uiElement;
}



hmiVSlider::~hmiVSlider()
{

}



#ifdef ENV_EDITOR
wxString hmiVSlider::getDefaultName()
{
    return _("Vertical Slider");
}



wxInt32 hmiVSlider::save(wxFile *FHandle)
{
   wxInt32 length;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_VSLD,4);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData);
   length=htonl(length);
   FHandle->Write(&length,4);
   length=hmiObject::save(FHandle);

   return length;
}
#endif

