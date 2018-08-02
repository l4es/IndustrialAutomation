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
#include <wx/tglbtn.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "globals.h"
#include "MainWin.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiSimpleButton.h"
#include "hmiImageButton.h"
#include "hmiToggleImageButton.h"
#include "oapc_libio.h"


hmiToggleImageButton::hmiToggleImageButton(BasePanel *parent,flowObject *obj)
                     :hmiImageButton(parent,obj)
{
   this->data.type=HMI_TYPE_TOGGLEIMAGEBUTTON;
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_DIGI_IO1;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_DIGI_IO1;
#ifdef ENV_EDITOR
   noUIFlags=OAPC_HMI_NO_UI_MINMAX|OAPC_HMI_NO_UI_RO|OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_FG|OAPC_HMI_NO_UI_FONT;
   if (obj==NULL)
   {
       this->data.msizex=100*1000;
       this->data.msizey=50*1000;
   }
#endif
}



wxWindow *hmiToggleImageButton::createUIElement()
{
//   uiElement=new wxBitmapToggleButton(m_parent,wxID_ANY,bitmap);
   hmiObject::createUIElement();

   return uiElement;
}



hmiToggleImageButton::~hmiToggleImageButton()
{

}



#ifndef ENV_PLAYER
void hmiToggleImageButton::setData(flowObject *object)
{
   flowObject::setData(object);
}



wxString hmiToggleImageButton::getDefaultName()
{
    return _("Image Toggle Button");
}



wxInt32 hmiToggleImageButton::save(wxFile *FHandle)
{
   wxInt32                   length;
   struct hmiImageButtonData convData;
   wxMBConvUTF16BE           conv;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_TOIB,4);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData)+sizeof(struct hmiImageButtonData);
   length=htonl(length);
   FHandle->Write(&length,4);

   conv.WC2MB(convData.store_imageNormal,imageNormal,sizeof(convData.store_imageNormal));
   conv.WC2MB(convData.store_imageSelected,imageSelected,sizeof(convData.store_imageSelected));
   conv.WC2MB(convData.store_imageDisabled,imageDisabled,sizeof(convData.store_imageDisabled));

   length=FHandle->Write(&convData,sizeof(struct hmiImageButtonData));

   length=length+hmiObject::save(FHandle);

   return length;
}
#endif



void hmiToggleImageButton::doApplyData(const wxByte all)
{
/*    if (imageNormal.Length()>0)
    {
        ((wxBitmapToggleButton*)uiElement)->SetBitmapLabel(wxBitmap(imageNormal,g_getBitmapFlag(imageNormal)));
        setSize(wxRealPoint(((wxBitmapToggleButton*)uiElement)->GetBitmapLabel().GetWidth(),((wxBitmapToggleButton*)uiElement)->GetBitmapLabel().GetHeight()));
    }
    if (imageSelected.Length()>0) ((wxBitmapToggleButton*)uiElement)->SetBitmapSelected(wxBitmap(imageSelected,g_getBitmapFlag(imageSelected)));
    if (imageDisabled.Length()>0) ((wxBitmapToggleButton*)uiElement)->SetBitmapDisabled(wxBitmap(imageDisabled,g_getBitmapFlag(imageDisabled)));*/
    hmiSimpleButton::doApplyData(all);
}



