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
#include "hmiSimpleButton.h"
#include "hmiTextLabel.h"
#include "oapc_libio.h"
#include "common/oapcResourceBundle.h"
#include "ObjectList.h"
#include "globals.h"


hmiTextLabel::hmiTextLabel(BasePanel *parent,flowObject *obj)
             :hmiSimpleButton(parent,obj)
{
   this->data.type=HMI_TYPE_TEXTLABEL;
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_CHAR_IO3;
   this->data.stdOUT=OAPC_DIGI_IO0;
#ifdef ENV_EDITOR
   this->data.logFlags=LOG_TYPE_CHAR;
   noUIFlags=OAPC_HMI_NO_UI_MINMAX|OAPC_HMI_NO_UI_RO;
   if (obj==NULL)
   {
       this->data.msizex=100*1000;
       this->data.msizey=14*1000;
       text[0]=getDefaultName();
       text[1]=getDefaultName();
       this->data.bgcolor[0]=g_objectList.m_projectData->bgCol;
       this->data.bgcolor[1]=g_objectList.m_projectData->bgCol;
   }
#endif
}



wxWindow *hmiTextLabel::createUIElement()
{
   uiElement=new wxStaticText(m_parent,wxID_ANY,g_res->getString(text[0]),wxDefaultPosition,wxDefaultSize,wxST_NO_AUTORESIZE);
   
   hmiObject::createUIElement();
   updateAlignment();
   return uiElement;
}



hmiTextLabel::~hmiTextLabel()
{

}



#ifndef ENV_PLAYER
wxString hmiTextLabel::getDefaultName()
{
    return _("Text Label");
}



wxInt32 hmiTextLabel::save(wxFile *FHandle)
{
   wxInt32                 length;
   struct hmiTextLabelData convData;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_TELA,4);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData)+sizeof(struct hmiTextLabelData);
   length=htonl(length);
   FHandle->Write(&length,4);

   length=hmiObject::save(FHandle);

   convData.alignment=labelData.alignment; // byte
   convData.reserved=htonl(labelData.reserved);
   length+=FHandle->Write(&convData,sizeof(struct hmiTextLabelData));

   return length;
}



wxPanel* hmiTextLabel::getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num)
{
   if (num>0) return NULL;

   wxPanel *panel=new wxPanel(parent);

   wxFlexGridSizer *fSizer=new wxFlexGridSizer(2,4,4);

   wxStaticText *text=new wxStaticText(panel,wxID_ANY,_("Alignment")+_T(":"));
   fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

   comboBox=new wxComboBox(panel,wxID_ANY,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_DROPDOWN|wxCB_READONLY);
   comboBox->Append(_("Align to left"));
   comboBox->Append(_("Center horizontally"));
   comboBox->Append(_("Align to right"));

   comboBox->Select(labelData.alignment);
   fSizer->Add(comboBox,1,wxEXPAND);

   fSizer->AddGrowableCol(1,3);
   panel->SetSizer(fSizer);

   *name=_("Alignment");
   return panel;
}



void hmiTextLabel::setConfigData()
{
    labelData.alignment=comboBox->GetSelection();
    updateAlignment();
}
#endif



void hmiTextLabel::updateAlignment()
{
   if (labelData.alignment==0) ((wxStaticText*)uiElement)->SetWindowStyle(wxALIGN_LEFT|wxST_NO_AUTORESIZE);
   else if (labelData.alignment==1) ((wxStaticText*)uiElement)->SetWindowStyle(wxALIGN_CENTER|wxST_NO_AUTORESIZE);
   else if (labelData.alignment==2) ((wxStaticText*)uiElement)->SetWindowStyle(wxALIGN_RIGHT|wxST_NO_AUTORESIZE);
   uiElement->Refresh();
}



wxInt32 hmiTextLabel::load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset)
{
   struct hmiTextLabelData convData;
   wxUint32                loadedChunkSize,myChunkSize;

   loadedChunkSize=hmiObject::load(FHandle,chunkSize,IDOffset);

   myChunkSize=chunkSize-loadedChunkSize;
   if (myChunkSize>sizeof(struct hmiTextLabelData)) myChunkSize=sizeof(struct hmiTextLabelData);
   if (myChunkSize>0)
   {
      loadedChunkSize+=FHandle->Read(&convData,myChunkSize); 
      labelData.alignment=convData.alignment; // byte
      labelData.reserved=ntohl(convData.reserved);
   }
   else
   {
      labelData.alignment=1;
      labelData.reserved=0;
   }
   return loadedChunkSize;
}

