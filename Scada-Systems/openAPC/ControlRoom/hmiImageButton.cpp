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
#include "../hmiImageButton.cpp"
#else //ENV_INT

#include <wx/wx.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/artprov.h>
#include <wx/stockitem.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "globals.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiSimpleButton.h"
#include "hmiImageButton.h"
#include "oapc_libio.h"
#include "globals.h"
#include "libio_hmi_poscorr/general_images.h"

#define ICON_INTERNAL_THRES 60000
#define ICON_POINTDRAG      (ICON_INTERNAL_THRES+1)
#define ICON_ZOOMIN         (ICON_INTERNAL_THRES+2)
#define ICON_ZOOMOUT        (ICON_INTERNAL_THRES+3)
#define ICON_ZOOMWORKAREA   (ICON_INTERNAL_THRES+4)
#define ICON_ZOOMSELECTED   (ICON_INTERNAL_THRES+5)
#define ICON_ADD_HATCH      (ICON_INTERNAL_THRES+6)
#define ICON_INP_SERIAL     (ICON_INTERNAL_THRES+7)
#define ICON_INP_OAPC       (ICON_INTERNAL_THRES+8)
#define ICON_POST_CURVE     (ICON_INTERNAL_THRES+9)
#define ICON_POST_SINE      (ICON_INTERNAL_THRES+10)
#define ICON_PRI_BARCODE    (ICON_INTERNAL_THRES+11)
#define ICON_PRI_BEZIER     (ICON_INTERNAL_THRES+12)
#define ICON_PRI_CIRCLE     (ICON_INTERNAL_THRES+13)
#define ICON_PRI_DELAY      (ICON_INTERNAL_THRES+14)
#define ICON_PRI_DOT        (ICON_INTERNAL_THRES+15)
#define ICON_PRI_EXTTRIGGER (ICON_INTERNAL_THRES+16)
#define ICON_PRI_LASEROUT   (ICON_INTERNAL_THRES+17)
#define ICON_PRI_LINE       (ICON_INTERNAL_THRES+18)
#define ICON_PRI_MOTION     (ICON_INTERNAL_THRES+19)
#define ICON_PRI_POLYGON    (ICON_INTERNAL_THRES+20)
#define ICON_PRI_RECTANGLE  (ICON_INTERNAL_THRES+21)
#define ICON_PRI_SPIRAL     (ICON_INTERNAL_THRES+22)
#define ICON_PRI_STAR       (ICON_INTERNAL_THRES+23)
#define ICON_PRI_TEXT       (ICON_INTERNAL_THRES+24)
#define ICON_PRI_TRIANGLE   (ICON_INTERNAL_THRES+25)
#define ICON_PRI_ZSHIFTER   (ICON_INTERNAL_THRES+26)
#define ICON_VIDEO          (ICON_INTERNAL_THRES+27)
#define ICON_PRI_LASERIN    (ICON_INTERNAL_THRES+28)
#define ICON_PILOTLASER     (ICON_INTERNAL_THRES+29)
#define ICON_OPENMARKDLG    (ICON_INTERNAL_THRES+30)
#define ICON_GEARS          (ICON_INTERNAL_THRES+31)
#define ICON_DRAW           (ICON_INTERNAL_THRES+32)

static wxArtID artIDs[]={
        wxART_FILE_OPEN       ,
        wxART_NEW             ,
        wxART_FILE_SAVE       ,
        wxART_FILE_SAVE_AS    ,
        wxART_QUIT            ,
        wxART_UNDO            ,
        wxART_REDO            ,
        wxART_HELP            ,
        wxART_PRINT           ,
        wxART_CUT             ,
        wxART_COPY            ,
        wxART_PASTE           ,
        wxART_FIND            ,
        wxART_DELETE          ,
        wxART_FIND_AND_REPLACE,

        wxART_GO_FORWARD      ,
        wxART_GO_BACK         ,

        wxART_GO_UP,
        wxART_GO_DOWN,
        wxART_GO_HOME
};


static int imgIDs[]={
    wxID_OPEN,
    wxID_NEW,
    wxID_SAVE,
    wxID_SAVEAS,
    wxID_EXIT,
    wxID_UNDO,
    wxID_REDO,
    wxID_HELP,
    wxID_PRINT,

    wxID_CUT,
    wxID_COPY,
    wxID_PASTE,
    wxID_FIND,
    wxID_DELETE,
    wxID_REPLACE,

    wxID_FORWARD,
    wxID_BACKWARD,

    wxID_UP,
    wxID_DOWN,
    wxID_HOME,

    ICON_POINTDRAG,
    ICON_ZOOMIN,
    ICON_ZOOMOUT,
    ICON_ZOOMWORKAREA,
    ICON_ZOOMSELECTED,

    ICON_ADD_HATCH,
    ICON_INP_SERIAL,
    ICON_INP_OAPC   ,
    ICON_POST_CURVE  ,
    ICON_POST_SINE    ,
    ICON_PRI_BARCODE   ,
    ICON_PRI_BEZIER     ,
    ICON_PRI_CIRCLE,
    ICON_PRI_DELAY  ,
    ICON_PRI_DOT     ,
    ICON_PRI_EXTTRIGGER,
    ICON_PRI_LASEROUT,
    ICON_PRI_LINE,
    ICON_PRI_MOTION,
    ICON_PRI_POLYGON,
    ICON_PRI_RECTANGLE,
    ICON_PRI_SPIRAL    ,
    ICON_PRI_STAR,
    ICON_PRI_TEXT,
    ICON_PRI_TRIANGLE,
    ICON_PRI_ZSHIFTER,
    ICON_VIDEO,
    ICON_PRI_LASERIN,
    ICON_PILOTLASER,
    ICON_OPENMARKDLG,
    ICON_GEARS,
    ICON_DRAW,

    0};


hmiImageButton::hmiImageButton(BasePanel *parent,flowObject *obj)
               :hmiSimpleButton(parent,obj)
{
   this->data.type=HMI_TYPE_IMAGEBUTTON;
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_BIN_IO3|OAPC_BIN_IO4|OAPC_BIN_IO5;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_DIGI_IO1;
#ifdef ENV_EDITOR
   noUIFlags=OAPC_HMI_NO_UI_MINMAX|OAPC_HMI_NO_UI_RO|OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_FG|OAPC_HMI_NO_UI_FONT;
   if (obj==NULL)
   {
      data.msizex=100*1000;
      data.msizey=50*1000;
      data.custom3=0;
   }
#endif
}

void hmiImageButton::setBitmapData(bool all)
{
   if (data.custom3==0) // custom image
   {
      std::map<wxString,wxBitmap>::iterator it;

      if (imageNormal.Length()>0)
      {
         it=g_objectList.m_imageList.find(imageNormal);
         if (it!=g_objectList.m_imageList.end()) ((wxBitmapButton*)uiElement)->SetBitmapLabel(it->second);
         else ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(imageNormal,g_getBitmapFlag(imageNormal)));
#ifdef ENV_EDITOR
         if ((all) && (((wxBitmapButton*)uiElement)->GetBitmapLabel().Ok())) setSize(wxRealPoint(((wxBitmapButton*)uiElement)->GetBitmapLabel().GetWidth()*1000,((wxBitmapButton*)uiElement)->GetBitmapLabel().GetHeight()*1000));
#else
         all=all;
#endif
      }
      if (imageSelected.Length()>0) 
      {
         it=g_objectList.m_imageList.find(imageSelected);
         if (it!=g_objectList.m_imageList.end()) ((wxBitmapButton*)uiElement)->SetBitmapLabel(it->second);
         else ((wxBitmapButton*)uiElement)->SetBitmapSelected(wxBitmap(imageSelected,g_getBitmapFlag(imageSelected)));
      }
      if (imageDisabled.Length()>0) 
      {
         it=g_objectList.m_imageList.find(imageDisabled);
         if (it!=g_objectList.m_imageList.end()) ((wxBitmapButton*)uiElement)->SetBitmapLabel(it->second);
         else ((wxBitmapButton*)uiElement)->SetBitmapDisabled(wxBitmap(imageDisabled,g_getBitmapFlag(imageDisabled)));
      }
   }
   else
   {
       if (data.custom3<ICON_INTERNAL_THRES)
       {
          int i=0;

          while (imgIDs[i]!=0)
          {
             if (imgIDs[i]==data.custom3)
             {
                ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxArtProvider::GetBitmap(artIDs[i]));
                break;
             }
             i++;
          }
       }
       else switch (data.custom3)
       {
       case ICON_POINTDRAG:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(pointDragXPM));
           break;
       case ICON_OPENMARKDLG:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(openmarkdlgXPM));
           break;
       case ICON_ZOOMIN:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(zoominXPM));
           break;
       case ICON_ZOOMOUT:
          ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(zoomoutXPM));
          break;
       case ICON_ZOOMSELECTED:
          ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(zoomselectedXPM));
          break;
       case ICON_ZOOMWORKAREA:
          ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(zoomworkareaXPM));
          break;
       case ICON_ADD_HATCH:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(addHatchIcon_xpm));
           break;
       case ICON_INP_SERIAL:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(inputOAPCIcon_xpm));
           break;
       case ICON_INP_OAPC:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(inputSerialIcon_xpm));
           break;
       case ICON_POST_CURVE:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(postCurveIcon_xpm));
           break;
       case ICON_POST_SINE:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(postSineIcon_xpm));
           break;
       case ICON_PRI_BARCODE:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priBarcodeIcon_xpm));
           break;
       case ICON_PRI_BEZIER:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priBezierIcon_xpm));
           break;
       case ICON_PRI_CIRCLE:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priCircleIcon_xpm));
           break;
       case ICON_PRI_DELAY:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priDelayIcon_xpm));
           break;
       case ICON_PRI_DOT:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priDotIcon_xpm));
           break;
       case ICON_PRI_EXTTRIGGER:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priExttriggerIcon_xpm));
           break;
       case ICON_PRI_LASEROUT:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priLaseroutputIcon_xpm));
           break;
       case ICON_PRI_LASERIN:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priLaserinputIcon_xpm));
           break;
       case ICON_PRI_LINE:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priLineIcon_xpm));
           break;
       case ICON_PRI_MOTION:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priMotionIcon_xpm));
           break;
       case ICON_PRI_POLYGON:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priPolygonIcon_xpm));
           break;
       case ICON_PRI_RECTANGLE:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priRectangleIcon_xpm));
           break;
       case ICON_PRI_SPIRAL:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priSpiralIcon_xpm));
           break;
       case ICON_PRI_STAR:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priStarIcon_xpm));
           break;
       case ICON_PRI_TEXT:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priTextIcon_xpm));
           break;
       case ICON_PRI_TRIANGLE:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priTriangleIcon_xpm));
           break;
       case ICON_PRI_ZSHIFTER:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(priZShiftIcon_xpm));
           break;
       case ICON_VIDEO:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(video_xpm));
           break;
       case ICON_PILOTLASER:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(pilot_small_xpm));
           break;
       case ICON_GEARS:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(gearsXPM));
           break;
       case ICON_DRAW:
           ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(drawXPM));
           break;
       default:
          assert(0);
          break;
       }
   }
}

wxWindow *hmiImageButton::createUIElement()
{
   bitmap=wxArtProvider::GetBitmap(wxART_MISSING_IMAGE);
   uiElement=new wxBitmapButton(m_parent,wxID_ANY,bitmap,wxDefaultPosition,wxDefaultSize,wxBU_AUTODRAW);
   hmiObject::createUIElement();
#ifdef ENV_PLAYER
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(HMICanvas::OnButtonClicked));
#endif
   setBitmapData(true);
   return uiElement;
}



hmiImageButton::~hmiImageButton()
{

}



#ifdef ENV_EDITOR
void hmiImageButton::setData(flowObject *object)
{
   imageNormal=((hmiImageButton*)object)->imageNormal;
   imageDisabled=((hmiImageButton*)object)->imageDisabled;
   imageSelected=((hmiImageButton*)object)->imageSelected;
   flowObject::setData(object);
}



wxString hmiImageButton::getDefaultName()
{
    return _T("Image Button");
}



wxInt32 hmiImageButton::save(wxFile *FHandle)
{
   wxInt32                   length=0,chunkLength;
   struct hmiImageButtonData convData;
   wxMBConvUTF16BE           conv;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_IMBU,4);
   chunkLength=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData)+sizeof(struct hmiImageButtonData);
   chunkLength=htonl(chunkLength);
   FHandle->Write(&chunkLength,4);

   conv.WC2MB(convData.store_imageNormal,imageNormal,sizeof(convData.store_imageNormal));
   conv.WC2MB(convData.store_imageSelected,imageSelected,sizeof(convData.store_imageSelected));
   conv.WC2MB(convData.store_imageDisabled,imageDisabled,sizeof(convData.store_imageDisabled));

   length+=FHandle->Write(&convData,sizeof(struct hmiImageButtonData));

   length+=hmiObject::save(FHandle);

   return length;
}
#endif



wxInt32 hmiImageButton::load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset)
{
    struct hmiImageButtonData convData;
    wxUint32                   myChunkSize;
    wxMBConvUTF16BE           conv;
    wchar_t                   buf[MAX_PATH_LENGTH];

    if (!FHandle) return 0;

    myChunkSize=chunkSize;
    if (myChunkSize>sizeof(struct hmiImageButtonData)) myChunkSize=sizeof(struct hmiImageButtonData);

    myChunkSize=FHandle->Read(&convData,myChunkSize);
    conv.MB2WC(buf,convData.store_imageNormal,sizeof(buf));
    imageNormal=buf;
    conv.MB2WC(buf,convData.store_imageSelected,sizeof(buf));
    imageSelected=buf;
    conv.MB2WC(buf,convData.store_imageDisabled,sizeof(buf));
    imageDisabled=buf;

    return hmiObject::load(FHandle,chunkSize-myChunkSize,IDOffset)+myChunkSize;
}



void hmiImageButton::doApplyData(const wxByte all)
{
#ifdef ENV_EDITOR
   setBitmapData(all!=0);
#endif
   hmiSimpleButton::doApplyData(all);
}



#ifdef ENV_EDITOR
wxPanel* hmiImageButton::getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num)
{
   int      i=0,ln=0;

   if (num>0) return NULL;
   wxPanel *panel=new wxPanel(parent);

   wxGridBagSizer *fSizer=new wxGridBagSizer(4,4);

   wxStaticText *text=new wxStaticText(panel,wxID_ANY,_("Predefined Image")+_T(":")); 
   fSizer->Add(text,wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   m_predefComboBox=new wxComboBox(panel,wxID_ANY,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_DROPDOWN|wxCB_READONLY);
   fSizer->Add(m_predefComboBox,wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND);
   m_predefComboBox->Append(_("None"));
   m_predefComboBox->SetSelection(0);
   while (imgIDs[i]>0)
   {
      if (imgIDs[i]<ICON_INTERNAL_THRES)
      {
         wxString label;

         label=wxGetStockLabel(imgIDs[i]);
         label.Replace(_T("&"),_T(""),true);
         m_predefComboBox->Append(label);
      }
      else switch (imgIDs[i])
      {
         case ICON_POINTDRAG:
            m_predefComboBox->Append(_("Point drag mode"));
            break;
         case ICON_OPENMARKDLG:
            m_predefComboBox->Append(_("Open mark dialogue"));
            break;
         case ICON_ZOOMIN:
            m_predefComboBox->Append(_("Zoom into view"));
            break;
         case ICON_ZOOMOUT:
            m_predefComboBox->Append(_("Zoom out of view"));
            break;
         case ICON_ZOOMSELECTED:
            m_predefComboBox->Append(_("Zoom selected entity"));
            break;
         case ICON_ZOOMWORKAREA:
            m_predefComboBox->Append(_("Show full working area"));
            break;
         case ICON_ADD_HATCH:
             m_predefComboBox->Append(_("Additional / Hatch"));
             break;
         case ICON_INP_SERIAL:
             m_predefComboBox->Append(_("Input / Serial/Date/Time"));
             break;
         case ICON_INP_OAPC:
             m_predefComboBox->Append(_("Input / OpenAPC Data Input"));
             break;
         case ICON_POST_CURVE:
             m_predefComboBox->Append(_("Postprocessing / Curve"));
             break;
         case ICON_POST_SINE:
             m_predefComboBox->Append(_("Postprocessing / Sine"));
             break;
         case ICON_PRI_BARCODE:
             m_predefComboBox->Append(_("Primary / Barcode"));
             break;
         case ICON_PRI_BEZIER:
             m_predefComboBox->Append(_("Primary / Bezier Curve"));
             break;
         case ICON_PRI_CIRCLE:
             m_predefComboBox->Append(_("Primary / Circle"));
             break;
         case ICON_PRI_DELAY:
             m_predefComboBox->Append(_("Primary / Delay"));
             break;
         case ICON_PRI_DOT:
             m_predefComboBox->Append(_("Primary / Dot"));
             break;
         case ICON_PRI_EXTTRIGGER:
             m_predefComboBox->Append(_("Primary / External Trigger"));
             break;
         case ICON_PRI_LASEROUT:
             m_predefComboBox->Append(_("Primary / Custom Output"));
             break;
         case ICON_PRI_LASERIN:
             m_predefComboBox->Append(_("Primary / Custom Input"));
             break;
         case ICON_PRI_LINE:
             m_predefComboBox->Append(_("Primary / Single Line"));
             break;
         case ICON_PRI_MOTION:
             m_predefComboBox->Append(_("Primary / Motion"));
             break;
         case ICON_PRI_POLYGON:
             m_predefComboBox->Append(_("Primary / Polygon"));
             break;
         case ICON_PRI_RECTANGLE:
             m_predefComboBox->Append(_("Primary / Rectangle"));
             break;
         case ICON_PRI_SPIRAL:
             m_predefComboBox->Append(_("Primary / Spiral"));
             break;
         case ICON_PRI_STAR:
             m_predefComboBox->Append(_("Primary / Star"));
             break;
         case ICON_PRI_TEXT:
             m_predefComboBox->Append(_("Primary / Text"));
             break;
         case ICON_PRI_TRIANGLE:
             m_predefComboBox->Append(_("Primary / Triangle"));
             break;
         case ICON_PRI_ZSHIFTER:
             m_predefComboBox->Append(_("Primary / Z-Shifter"));
             break;
         case ICON_VIDEO:
             m_predefComboBox->Append(_("Video"));
             break;
         case ICON_PILOTLASER:
             m_predefComboBox->Append(_("Pilot-Laser"));
             break;
         case ICON_GEARS:
             m_predefComboBox->Append(_("Gears"));
             break;
         case ICON_DRAW:
             m_predefComboBox->Append(_("Draw"));
             break;

         default:
            assert(0);
            break;
      }
      if (imgIDs[i]==data.custom3) m_predefComboBox->SetSelection(i+1);
      i++;
   }
   m_predefComboBox->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED,wxCommandEventHandler(hmiImageButton::OnButtonClick),NULL,this);
   ln++;

   normalText=new wxStaticText(panel,wxID_ANY,_("Normal Image")+_T(":"));     
   fSizer->Add(normalText,wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   normalPath=new wxTextCtrl(panel,wxID_ANY,imageNormal);                     
   fSizer->Add(normalPath,wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND);
   normalFCButton=new wxButton(panel,wxID_ANY,_T("..."));                     
   fSizer->Add(normalFCButton,wxGBPosition(ln,2),wxGBSpan(1,1),wxEXPAND);
   normalFCButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(hmiImageButton::OnButtonClick),NULL,this);
   ln++;

   selectedText=new wxStaticText(panel,wxID_ANY,_("Selected Image")+_T(":")); 
   fSizer->Add(selectedText,wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   selectedPath=new wxTextCtrl(panel,wxID_ANY,imageSelected);                 
   fSizer->Add(selectedPath,wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND);
   selectedFCButton=new wxButton(panel,wxID_ANY,_T("..."));                   
   fSizer->Add(selectedFCButton,wxGBPosition(ln,2),wxGBSpan(1,1),wxEXPAND);
   selectedFCButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(hmiImageButton::OnButtonClick),NULL,this);
   ln++;

   disabledText=new wxStaticText(panel,wxID_ANY,_("Disabled Image")+_T(":")); 
   fSizer->Add(disabledText,wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   disabledPath=new wxTextCtrl(panel,wxID_ANY,imageDisabled);                 
   fSizer->Add(disabledPath,wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND);
   disabledFCButton=new wxButton(panel,wxID_ANY,_T("..."));                   
   fSizer->Add(disabledFCButton,wxGBPosition(ln,2),wxGBSpan(1,1),wxEXPAND);
   disabledFCButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(hmiImageButton::OnButtonClick),NULL,this);
   ln++;

   fSizer->AddGrowableCol(1,5);
   fSizer->AddGrowableCol(2,1);
   panel->SetSizer(fSizer);

   updateUI();

   *name=_("Images");
   return panel;
}


void hmiImageButton::updateUI()
{
    bool enable;

    if (m_predefComboBox->GetSelection()==0) enable=true;
    else enable=false;
    normalText->Enable(enable);   normalFCButton->Enable(enable);   normalPath->Enable(enable);
    selectedText->Enable(enable); selectedFCButton->Enable(enable); selectedPath->Enable(enable);
    disabledText->Enable(enable); disabledFCButton->Enable(enable); disabledPath->Enable(enable);
}


void hmiImageButton::OnButtonClick(wxCommandEvent &event)
{
   wxFileDialog dialog(NULL, _("Select image"),_T(""),_T(""),IMAGE_FILETYPES,wxFD_OPEN, wxDefaultPosition);

   if (event.GetId()==m_predefComboBox->GetId())
   {
      if (m_predefComboBox->GetSelection()==0) data.custom3=0;
      else data.custom3=imgIDs[m_predefComboBox->GetSelection()-1];
      updateUI();
   }
   else if (event.GetId()==normalFCButton->GetId())
   {
       dialog.SetFilename(normalPath->GetValue());
       if (dialog.ShowModal()==wxID_OK) normalPath->SetValue(dialog.GetPath());
   }
   else if (event.GetId()==selectedFCButton->GetId())
   {
       dialog.SetFilename(selectedPath->GetValue());
       if (dialog.ShowModal()==wxID_OK) selectedPath->SetValue(dialog.GetPath());
   }
   else if (event.GetId()==disabledFCButton->GetId())
   {
       dialog.SetFilename(disabledPath->GetValue());
       if (dialog.ShowModal()==wxID_OK) disabledPath->SetValue(dialog.GetPath());
   }
}


void hmiImageButton::setConfigData()
{
   imageNormal=normalPath->GetValue();
   imageSelected=selectedPath->GetValue();
   imageDisabled=disabledPath->GetValue();
}

#else



wxByte hmiImageButton::setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   unsigned char *data;

   if (!threadIDOK(flowThreadID,false)) return 0;
#ifdef ENV_DEBUGGER
   if ((connection->targetInputNum<3) || (connection->targetInputNum>5))
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      return 0;
   }
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
      if (connection->targetInputNum==3) ((wxBitmapButton*)uiElement)->SetBitmapLabel(wxBitmap(*img));
      else if (connection->targetInputNum==4) ((wxBitmapButton*)uiElement)->SetBitmapSelected(wxBitmap(*img));
      else if (connection->targetInputNum==5) ((wxBitmapButton*)uiElement)->SetBitmapDisabled(wxBitmap(*img));
      else wxASSERT(0);
      delete img;
   }
   applyData(0);
   return 1;
}



wxUint64 hmiImageButton::getAssignedOutput(wxUint64 input)
{
   if (input==OAPC_DIGI_IO1) return OAPC_DIGI_IO1;
   else return hmiObject::getAssignedOutput(input);
}



#endif

#endif //ENV_INT
