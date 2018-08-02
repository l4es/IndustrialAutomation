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

#ifdef ENV_INT
#include "../hmiImage.cpp"
#else //ENV_INT


#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/artprov.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "globals.h"
#include "MainWin.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiImage.h"
#include "oapc_libio.h"
#include "globals.h"


hmiImage::hmiImage(BasePanel *parent,flowObject *obj)
         :hmiObject(parent,obj)
{
   this->data.type=HMI_TYPE_IMAGE;
#ifdef ENV_EDITOR
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_BIN_IO3;
   noUIFlags=OAPC_HMI_NO_UI_MINMAX|OAPC_HMI_NO_UI_RO|OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_FG|OAPC_HMI_NO_UI_FONT|OAPC_HMI_NO_SIZE|OAPC_HMI_NO_UI_LAYOUT;
   m_userPriviEnable=OAPC_USERPRIVI_HIDE;
   if (obj==NULL)
   {
       this->data.msizex=84*1000;
       this->data.msizey=82*1000;
   }
#endif
   bitmap=NULL;
   m_isVisible=true;
}



hmiImage::~hmiImage()
{
   delete bitmap;
}



#ifndef ENV_PLAYER
void hmiImage::setData(flowObject *object)
{
   imageNormal=((hmiImage*)object)->imageNormal;
   flowObject::setData(object);
}



wxString hmiImage::getDefaultName()
{
    return _T("Image");
}



wxInt32 hmiImage::save(wxFile *FHandle)
{
   wxInt32             length=0,chunkLength;
   struct hmiImageData convData;
   wxMBConvUTF16BE     conv;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_IMAG,4);
   chunkLength=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData)+sizeof(struct hmiImageData);
   chunkLength=htonl(chunkLength);
   FHandle->Write(&chunkLength,4);

   conv.WC2MB(convData.store_imageNormal,imageNormal,sizeof(convData.store_imageNormal));

   length+=FHandle->Write(&convData,sizeof(struct hmiImageData));

   length+=hmiObject::save(FHandle);

   return length;
}
#endif



wxInt32 hmiImage::load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset)
{
    struct hmiImageData convData;
    wxMBConvUTF16BE     conv;
    wxUint32            myChunkSize;
    wchar_t             buf[MAX_PATH_LENGTH];

//    if (!FHandle) return 0;

    myChunkSize=chunkSize;
    if (myChunkSize>sizeof(struct hmiImageData)) myChunkSize=sizeof(struct hmiImageData);

    myChunkSize=FHandle->Read(&convData,myChunkSize);
    conv.MB2WC(buf,convData.store_imageNormal,sizeof(buf));
    imageNormal=buf;

    return hmiObject::load(FHandle,chunkSize-myChunkSize,IDOffset)+myChunkSize;

}



void hmiImage::setBitmapData()
{
   if (imageNormal.Length()>0)
   {
      std::map<wxString,wxBitmap>::iterator it;

      it=g_objectList.m_imageList.find(imageNormal);
      if (it!=g_objectList.m_imageList.end())
      {
         delete bitmap;
         bitmap=new wxBitmap(it->second);
      }
      else bitmap->LoadFile(imageNormal,g_getBitmapFlag(imageNormal));
      if ((bitmap->Ok()) && (bitmap->GetWidth()>0) && (bitmap->GetHeight()>0))
       setSize(wxRealPoint(bitmap->GetWidth()*1000,bitmap->GetHeight()*1000));
      else
       *bitmap=wxArtProvider::GetBitmap(wxART_MISSING_IMAGE);
   }
   else *bitmap=wxArtProvider::GetBitmap(wxART_MISSING_IMAGE);
}



wxWindow *hmiImage::createUIElement()
{
   bitmap=new wxBitmap();
   setBitmapData();
   return NULL;
}



void hmiImage::doApplyData(const wxByte all)
{
#ifdef ENV_EDITOR
   setBitmapData();
#endif
   if (all)
   {
      setSize(wxRealPoint(bitmap->GetWidth()*1000,bitmap->GetHeight()*1000));
      setPos(wxRealPoint(data.mposx,data.mposy));
   }
   hmiObject::doApplyData(all);
}


void hmiImage::drawObject(wxAutoBufferedPaintDC *dc)
{
#ifdef ENV_PLAYER
   oapc_thread_mutex_lock(g_imagePaintMutex);
#endif
   if ((bitmap->Ok()) && (m_isVisible)) dc->DrawBitmap(*bitmap,getPos().x/1000.0,getPos().y/1000.0,true);
#ifdef ENV_PLAYER
   oapc_thread_mutex_unlock(g_imagePaintMutex);
#endif
}


#ifdef ENV_EDITOR

wxPoint hmiImage::getMinSize()
{
    if (bitmap->Ok()) return wxPoint(bitmap->GetWidth(),bitmap->GetHeight());
    return hmiObject::getMinSize();
}



wxPoint hmiImage::getMaxSize()
{
   return getMinSize();
}



wxPanel* hmiImage::getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num)
{
   int ln=0;

   if (num>0) return NULL;
   wxPanel *panel=new wxPanel(parent);

   wxGridBagSizer *fSizer=new wxGridBagSizer(4,4);

   wxStaticText *text=new wxStaticText(panel,wxID_ANY,_("Normal Image")+_T(":"));     
   fSizer->Add(text,wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   normalPath=new wxTextCtrl(panel,wxID_ANY,imageNormal);                     
   fSizer->Add(normalPath,wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND);
   normalFCButton=new wxButton(panel,wxID_ANY,_T("..."));                     
   fSizer->Add(normalFCButton,wxGBPosition(ln,2),wxGBSpan(1,1),wxEXPAND);
   normalFCButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(hmiImage::OnButtonClick),NULL,this);
   ln++;

   fSizer->AddGrowableCol(1,3);
   fSizer->AddGrowableCol(2,1);
   panel->SetSizer(fSizer);

   *name=_("Image");
   return panel;
}




void hmiImage::OnButtonClick(wxCommandEvent &event)
{
   wxFileDialog dialog(NULL, _("Load new project"),_T(""),_T(""),IMAGE_FILETYPES,wxFD_OPEN, wxDefaultPosition);
   if (event.GetId()==normalFCButton->GetId())
   {
       dialog.SetFilename(normalPath->GetValue());
       if (dialog.ShowModal()==wxID_OK) normalPath->SetValue(dialog.GetPath());
   }
}



void hmiImage::setConfigData()
{
   imageNormal=normalPath->GetValue();
}

#else

void hmiImage::setUIElementState(wxInt32 state)
{
   if (state==0) m_isVisible=false;
   else m_isVisible=true;

}



wxByte hmiImage::setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   unsigned char *data;

   if (!threadIDOK(flowThreadID,false)) return 0;
#ifdef ENV_DEBUGGER
   if (connection->targetInputNum!=3)
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      return 0;
   }
#else
   connection=connection;
#endif
   if ((value->m_bin->type==OAPC_BIN_TYPE_IMAGE) &&
       ((value->m_bin->subType==OAPC_BIN_SUBTYPE_IMAGE_RGB24) ||
        (value->m_bin->subType==OAPC_BIN_SUBTYPE_IMAGE_GREY8)))
   {
   }
   else
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_INCOMPATIBLE_DATATYPE,_T(""));
#endif
      return 0;
   }
   data=(unsigned char*)malloc(value->m_bin->sizeData);
   wxASSERT(data);
   if (data)
   {
      wxImage *img;

      memcpy(data,(void*)&value->m_bin->data,value->m_bin->sizeData);
      if (value->m_bin->subType==OAPC_BIN_SUBTYPE_IMAGE_GREY8) img=g_getImageFromGrey8(value);
      else img=new wxImage(value->m_bin->param1,value->m_bin->param2,data);
      if (!img) return 0;
      oapc_thread_mutex_lock(g_imagePaintMutex);
      delete bitmap;
      bitmap=new wxBitmap(*img);
      oapc_thread_mutex_unlock(g_imagePaintMutex);
      delete img;
   }
   // TODO: refresh only the image, not the full canvas
   g_hmiCanvas->Refresh();
   return 1;
}

#endif

#endif //ENV_INT
