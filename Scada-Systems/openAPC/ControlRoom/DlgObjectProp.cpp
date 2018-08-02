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
#include "../DlgObjectProp.cpp"
#else //ENV_INT

#include <wx/colordlg.h>
#include <wx/fontdlg.h>
#include <wx/spinctrl.h>
#include <wx/app.h>

#include "oapcNoSpaceCtrl.h"
#include "MainWin.h"
#include "LogDefPanel.h"
#include "DlgObjectProp.h"
#include "hmiExternalIOLib.h"
#include "globals.h"
#include "oapc_libio.h"
#include "PrivilegePanel.h"
#include "MainApp.h"

DECLARE_APP(MainApp)

IMPLEMENT_CLASS(DlgObjectProp, wxDialog)

BEGIN_EVENT_TABLE(DlgObjectProp, wxDialog)
  EVT_BUTTON(wxID_ANY,DlgObjectProp::OnButtonClick)
  EVT_CHECKBOX(wxID_ANY,DlgObjectProp::OnButtonClick)
  EVT_RADIOBUTTON(wxID_ANY,DlgObjectProp::OnButtonClick)
  EVT_SPINCTRL(wxID_ANY,DlgObjectProp::OnSpinEvent)
END_EVENT_TABLE()


DlgObjectProp::DlgObjectProp(hmiObject *object,wxWindow* parent,const wxString& title, const wxPoint& pos,const wxString& name)
             :wxDialog(parent,wxID_ANY,title,pos,wxSize(400,430),wxRESIZE_BORDER|wxCAPTION,name)
{
   wxInt32 i,ln=0;
   wxInt32 valX,valY,valW,valH;

   this->object=object;
   memcpy(&objectData,&object->data,sizeof(struct hmiObjectData));

   wxBoxSizer *bSizer=new wxBoxSizer(wxVERTICAL);
   this->SetSizer(bSizer);

   book=new wxBookCtrl(this,wxID_ANY);
   bSizer->Add(book,1,wxEXPAND);

   g_createDialogButtons(this,bSizer,wxOK|wxCANCEL);

   wxPanel *rootPanel=new wxPanel(book);
   book->AddPage(rootPanel,_("Basic"), true);

   wxGridBagSizer *fSizer=new wxGridBagSizer(4,4);
   rootPanel->SetSizer(fSizer);

   wxStaticText *text=new wxStaticText(rootPanel,wxID_ANY,_("Name")+_T(":"));
   fSizer->Add(text,wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   nameField=new oapcNoSpaceCtrl(rootPanel,wxID_ANY,object->name);
   fSizer->Add(nameField,wxGBPosition(ln,1),wxGBSpan(1,2),wxEXPAND);
   ln++;

   text=new wxStaticText(rootPanel,wxID_ANY,_("Min/Max")+_T(":"));
   fSizer->Add(text,wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

   minField=new oapcFloatCtrl(rootPanel,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTE_RIGHT);
   minField->SetAccuracy(3);
   minField->SetValue(object->MinValue);
   fSizer->Add(minField,wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND);
   Connect(minField->GetId(),wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(DlgObjectProp::OnNumberChanged));
   minField->Connect(minField->GetId(),wxEVT_KILL_FOCUS,wxFocusEventHandler(DlgObjectProp::OnNumberFocusChanged),NULL,this);

   maxField=new oapcFloatCtrl(rootPanel,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTE_RIGHT);
   maxField->SetAccuracy(3);
   maxField->SetValue(object->MaxValue);
   fSizer->Add(maxField,wxGBPosition(ln,2),wxGBSpan(1,1),wxEXPAND);
   Connect(maxField->GetId(),wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(DlgObjectProp::OnNumberChanged));
   maxField->Connect(maxField->GetId(),wxEVT_KILL_FOCUS,wxFocusEventHandler(DlgObjectProp::OnNumberFocusChanged),NULL,this);
   if (object->noUIFlags & OAPC_HMI_NO_UI_MINMAX)
   {
       text->Enable(false);
       minField->Enable(false);
       maxField->Enable(false);
   }
   ln++;

   fSizer->Add(new wxStaticText(rootPanel,wxID_ANY,wxEmptyString),wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT);
   ln++;

   fSizer->Add(new wxStaticText(rootPanel,wxID_ANY,_T("X")),wxGBPosition(ln,1),wxGBSpan(1,1),wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
   fSizer->Add(new wxStaticText(rootPanel,wxID_ANY,_T("Y")),wxGBPosition(ln,2),wxGBSpan(1,1),wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
   ln++;

   if ((objectData.msizex>=0) || ((object->noUIFlags & OAPC_HMI_NO_UI_LAYOUT)!=0))
   {
      valX=objectData.mposx/1000.0;  valY=objectData.mposy/1000.0;
      valW=objectData.msizex/1000.0; valH=objectData.msizey/1000.0;
   }
   else
   {
      valX=20;                valY=10;
      valW=object->getDefSize().x; valH=object->getDefSize().y;
   }

   m_absPosText=new wxStaticText(rootPanel,wxID_ANY,_("Position")+_T(":"));
   fSizer->Add(m_absPosText,wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   posX=new wxSpinCtrl(rootPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,0,10000,valX);
   posX->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
   fSizer->Add(posX,wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND);
   posY=new wxSpinCtrl(rootPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,0,10000,valY);
   posY->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
   fSizer->Add(posY,wxGBPosition(ln,2),wxGBSpan(1,1),wxEXPAND);
   if (object->noUIFlags & OAPC_HMI_NO_POS)
   {
       m_absPosText->Enable(false);
       posX->Enable(false);
       posY->Enable(false);
   }
   ln++;

   m_absSizeText=new wxStaticText(rootPanel,wxID_ANY,_("Size")+_T(":"));
   fSizer->Add(m_absSizeText,wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   sizeX=new wxSpinCtrl(rootPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,object->getMinSize().x,object->getMaxSize().x,valW);
   sizeX->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
   fSizer->Add(sizeX,wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND);
   sizeY=new wxSpinCtrl(rootPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,object->getMinSize().y,object->getMaxSize().y,valH);
   sizeY->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
   fSizer->Add(sizeY,wxGBPosition(ln,2),wxGBSpan(1,1),wxEXPAND);
   m_sizeRatio=0;
   if (object->noUIFlags & OAPC_HMI_NO_SIZE)
   {
      m_absSizeText->Enable(false);
      sizeX->Enable(false);
      sizeY->Enable(false);
   }
   else if (object->noUIFlags & OAPC_HMI_NO_FREE_SIZE_RATIO)
   {
      m_sizeRatio=objectData.msizex/objectData.msizey;
      sizeY->Enable(false);
      Connect(sizeX->GetId(),wxEVT_COMMAND_SPINCTRL_UPDATED,wxCommandEventHandler(DlgObjectProp::OnNumberChanged));
      Connect(sizeX->GetId(),wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(DlgObjectProp::OnNumberChanged));
      sizeX->Connect(sizeX->GetId(),wxEVT_KILL_FOCUS,wxFocusEventHandler(DlgObjectProp::OnNumberFocusChanged),NULL,this);
   }
   ln++;

   fSizer->Add(new wxStaticText(rootPanel,wxID_ANY,wxEmptyString),wxGBPosition(ln,0),wxGBSpan(1,1),0);
   ln++;

   fSizer->Add(new wxStaticText(rootPanel,wxID_ANY,_("Digital IN 0")+_T(":")),wxGBPosition(ln,0),wxGBSpan(1,1),0);
   fSizer->Add(new wxStaticText(rootPanel,wxID_ANY,_("LOW State")),           wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
   fSizer->Add(new wxStaticText(rootPanel,wxID_ANY,_("HIGH State")),          wxGBPosition(ln,2),wxGBSpan(1,1),wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
   ln++;
    
   text=new wxStaticText(rootPanel,wxID_ANY,_("Text")+_T(":"));
   fSizer->Add(text,wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   textLowField=new wxTextCtrl(rootPanel,wxID_ANY,object->text[0]);
   fSizer->Add(textLowField,wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND);
   textHighField=new wxTextCtrl(rootPanel,wxID_ANY,object->text[1]);
   fSizer->Add(textHighField,wxGBPosition(ln,2),wxGBSpan(1,1),wxEXPAND);
   if (object->noUIFlags & OAPC_HMI_NO_UI_TEXT)
   {
      text->Enable(false);
      textLowField->Enable(false);
      textHighField->Enable(false);
   }
   ln++;

   text=new wxStaticText(rootPanel,wxID_ANY,_("Font")+_T(":"));
   fSizer->Add(text,wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   fontLow=new wxButton(rootPanel,wxID_ANY,_("AaBbCc123"));
   fontLow->SetBackgroundColour(*wxWHITE);
#if wxCHECK_VERSION(2,9,0)
   fontLow->SetFont(wxFont(objectData.font[0].pointSize,
                            wxFONTFAMILY_DEFAULT,
                            (wxFontStyle)objectData.font[0].style,
                            (wxFontWeight)objectData.font[0].weight,
                            false,
                            object->fontFaceName[0]));
#else
   fontLow->SetFont(wxFont(objectData.font[0].pointSize,
                            wxFONTFAMILY_DEFAULT,
                            objectData.font[0].style,
                            objectData.font[0].weight,
                            false,
                            object->fontFaceName[0]));
#endif
   fSizer->Add(fontLow,wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND);
   fontHigh=new wxButton(rootPanel,wxID_ANY,_("AaBbCc123"));
   fontHigh->SetBackgroundColour(*wxWHITE);
#if wxCHECK_VERSION(2,9,0)
   fontHigh->SetFont(wxFont(objectData.font[1].pointSize,
                            wxFONTFAMILY_DEFAULT,
                            (wxFontStyle)objectData.font[1].style,
                            (wxFontWeight)objectData.font[1].weight,
                            false,
                            object->fontFaceName[1]));
#else
   fontHigh->SetFont(wxFont(objectData.font[1].pointSize,
                            wxFONTFAMILY_DEFAULT,
                            objectData.font[1].style,
                            objectData.font[1].weight,
                            false,
                            object->fontFaceName[1]));
#endif
   fSizer->Add(fontHigh,wxGBPosition(ln,2),wxGBSpan(1,1),wxEXPAND);
   if (object->noUIFlags & OAPC_HMI_NO_UI_FONT)
   {
      text->Enable(false);
      fontLow->Enable(false);
      fontHigh->Enable(false);
   }
   ln++;

   text=new wxStaticText(rootPanel,wxID_ANY,_("Background Colour")+_T(":"));
   fSizer->Add(text,wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   bgLow=new wxButton(rootPanel,wxID_ANY);
   bgLow->SetBackgroundColour(wxColour(objectData.bgcolor[0]));
   fSizer->Add(bgLow,wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND);
   bgHigh=new wxButton(rootPanel,wxID_ANY);
   bgHigh->SetBackgroundColour(wxColour(objectData.bgcolor[1]));
   fSizer->Add(bgHigh,wxGBPosition(ln,2),wxGBSpan(1,1),wxEXPAND);
   if (object->noUIFlags & OAPC_HMI_NO_UI_BG)
   {
      text->Enable(false);
      bgLow->Enable(false);
      bgHigh->Enable(false);
   }
   ln++;

   text=new wxStaticText(rootPanel,wxID_ANY,_("Foreground Colour")+_T(":"));
   fSizer->Add(text,wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   fgLow=new wxButton(rootPanel,wxID_ANY);
   fgLow->SetBackgroundColour(wxColour(objectData.fgcolor[0] & 0xFFFFFF));
   fSizer->Add(fgLow,wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND);
   fgHigh=new wxButton(rootPanel,wxID_ANY);
   fgHigh->SetBackgroundColour(wxColour(objectData.fgcolor[1] & 0xFFFFFF));
   fSizer->Add(fgHigh,wxGBPosition(ln,2),wxGBSpan(1,1),wxEXPAND);
   if (object->noUIFlags & OAPC_HMI_NO_UI_FG)
   {
      text->Enable(false);
      fgLow->Enable(false);
      fgHigh->Enable(false);
   }
   ln++;

   fSizer->Add(new wxStaticText(rootPanel,wxID_ANY,_("State")+_T(":")),wxGBPosition(ln,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   disabledLow=new wxCheckBox(rootPanel,wxID_ANY,_T("Disabled"));
   fSizer->Add(disabledLow,wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND);
   disabledHigh=new wxCheckBox(rootPanel,wxID_ANY,_T("Disabled"));
   fSizer->Add(disabledHigh,wxGBPosition(ln,2),wxGBSpan(1,1),wxEXPAND);
   if (object->noUIFlags & OAPC_HMI_NO_UI_DISABLED)
   {
      disabledLow->Enable(false);
      disabledHigh->Enable(false);
   }
   else
   {
      if (objectData.state[0] & HMI_STATE_FLAG_DISABLED) disabledLow->SetValue(1);
      if (objectData.state[1] & HMI_STATE_FLAG_DISABLED) disabledHigh->SetValue(1);
   }
   ln++;

   fSizer->Add(new wxStaticText(rootPanel,wxID_ANY,wxEmptyString),wxGBPosition(ln,0),wxGBSpan(1,1),0);
   roLow=new wxCheckBox(rootPanel,wxID_ANY,_T("Read-only"));
   fSizer->Add(roLow,wxGBPosition(ln,1),wxGBSpan(1,1),wxEXPAND);
   roHigh=new wxCheckBox(rootPanel,wxID_ANY,_T("Read-only"));
   fSizer->Add(roHigh,wxGBPosition(ln,2),wxGBSpan(1,1),wxEXPAND);
   if (object->noUIFlags & OAPC_HMI_NO_UI_RO)
   {
      roLow->Enable(false);
      roHigh->Enable(false);
   }
   else
   {
      if (objectData.state[0] & HMI_STATE_FLAG_RO) roLow->SetValue(1);
      if (objectData.state[1] & HMI_STATE_FLAG_RO) roHigh->SetValue(1);
   }
   ln++;

   text=new wxStaticText(rootPanel,wxID_ANY,wxEmptyString);
   fSizer->Add(text,wxGBPosition(ln,0),wxGBSpan(1,1),wxEXPAND);
   fSizer->AddGrowableRow(ln,1);
   ln++;

   /*** The logging panels *******************************************/
   if (objectData.logFlags!=0)
   {
      logPanel=new LogDefPanel(objectData.log,objectData.logFlags,book);
      book->AddPage(logPanel,_("Logging"),false);
   }
   else logPanel=NULL;

   /*** The user privilege panel *************************************/
   if (((objectData.type & HMI_TYPE_MASK)!=0) && (object->m_userPriviEnable & OAPC_USERPRIVI_MASK))
   {
      priviPanel=new PrivilegePanel(book,wxID_ANY,this->object->data.userPriviFlags,object->m_userPriviEnable);
      book->AddPage(priviPanel,_("User Privileges"),false);
      if (!g_userPriviData.enabled) priviPanel->Enable(false);
   }
   else priviPanel=NULL;

   /*** The custom panels ********************************************/
   for (i=0; i<10; i++)
   {
      wxString custName;
      wxPanel *custPanel=object->getConfigPanel(&custName,book,i);
      if (custPanel) book->AddPage(custPanel,custName,false);
      else break;
   }

//   fSizer->AddGrowableCol(0,2);
   fSizer->AddGrowableCol(1,3);
   fSizer->AddGrowableCol(2,3);
   fSizer->AddGrowableRow(9,1);
   fSizer->AddGrowableRow(14,1);

   rootPanel->Layout();
   wxInt32 x,y;

   y=bSizer->Fit(this).y;
   x=bSizer->Fit(this).x;
   if (x<DIALOG_X_SIZE) x=DIALOG_X_SIZE;
   SetSize(x,y);
   updateUI();
   Centre();
}



DlgObjectProp::~DlgObjectProp()
{
}


void DlgObjectProp::updateUI()
{
   wxWindow  *okButton;


   okButton=wxWindow::FindWindowById(wxID_OK,this);
   okButton->Enable(true);
}


void DlgObjectProp::OnNumberChanged(wxCommandEvent &event)
{
   if (event.GetId()==minField->GetId())
   {
      minField->SetValue(minField->GetValue(MIN_NUM_VALUE,maxField->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE)));
   }
   else if (event.GetId()==maxField->GetId())
   {
      maxField->SetValue(maxField->GetValue(minField->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE),MAX_NUM_VALUE));
   }
   else if (event.GetId()==sizeX->GetId())
   {
      sizeY->SetValue(sizeX->GetValue()/m_sizeRatio);
   }
}



void DlgObjectProp::OnNumberFocusChanged(wxFocusEvent &event)
{
   wxCommandEvent cevent;

   cevent.SetId(event.GetId());
   OnNumberChanged(cevent);
   event.Skip(true);
}


void DlgObjectProp::OnSpinEvent(wxSpinEvent& /*event*/)
{
   updateUI();
}


void DlgObjectProp::OnButtonClick(wxCommandEvent &event)
{
    wxColourData colour;
    wxFontData   font;

    if (event.GetId()==bgLow->GetId())
    {
       colour.SetColour(bgLow->GetBackgroundColour());
       g_colourDialog->Create(this,&colour);
       g_colourDialog->Centre();
       g_colourDialog->SetTitle(_("Choose the background colour"));
       if (g_colourDialog->ShowModal() == wxID_OK)
       {
          colour=g_colourDialog->GetColourData();
          bgLow->SetBackgroundColour(colour.GetColour());
          objectData.bgcolor[0]=colour.GetColour().Red() |
                                        colour.GetColour().Green()<<8 |
                                        colour.GetColour().Blue()<<16;
       }
    }
    else if (event.GetId()==bgHigh->GetId())
    {
       colour.SetColour(bgHigh->GetBackgroundColour());
       g_colourDialog->Create(this,&colour);
       g_colourDialog->Centre();

       g_colourDialog->SetTitle(_("Choose the background colour"));
       if (g_colourDialog->ShowModal() == wxID_OK)
       {
          colour=g_colourDialog->GetColourData();
          bgHigh->SetBackgroundColour(colour.GetColour());
          objectData.bgcolor[1]=colour.GetColour().Red() |
                                        colour.GetColour().Green()<<8 |
                                        colour.GetColour().Blue()<<16;
       }
    }
    else if (event.GetId()==fgLow->GetId())
    {
       colour.SetColour(fgLow->GetBackgroundColour());
       g_colourDialog->Create(this,&colour);
       g_colourDialog->Centre();

       g_colourDialog->SetTitle(_("Choose the foreground colour"));
       if (g_colourDialog->ShowModal() == wxID_OK)
       {
          colour=g_colourDialog->GetColourData();
          fgLow->SetBackgroundColour(colour.GetColour());
          objectData.fgcolor[0]=colour.GetColour().Red() |
                                        colour.GetColour().Green()<<8 |
                                        colour.GetColour().Blue()<<16;
       }
    }
    else if (event.GetId()==fgHigh->GetId())
    {
       colour.SetColour(fgHigh->GetBackgroundColour());
       g_colourDialog->Create(this,&colour);
       g_colourDialog->Centre();

       g_colourDialog->SetTitle(_("Choose the foreground colour"));
       if (g_colourDialog->ShowModal() == wxID_OK)
       {
          colour=g_colourDialog->GetColourData();
          fgHigh->SetBackgroundColour(colour.GetColour());
          objectData.fgcolor[1]=colour.GetColour().Red() |
                                        colour.GetColour().Green()<<8 |
                                        colour.GetColour().Blue()<<16;
       }
    }
    else if (event.GetId()==fontLow->GetId())
    {
       font.SetInitialFont(fontLow->GetFont());
       font.EnableEffects(false);

       wxFontDialog *dialog = new wxFontDialog(this,font);
       if (dialog->ShowModal() == wxID_OK)
       {
          font=dialog->GetFontData();          
          fontLow->SetFont(font.GetChosenFont());
          
          objectData.font[0].pointSize=font.GetChosenFont().GetPointSize();
          objectData.font[0].style=font.GetChosenFont().GetStyle();
          objectData.font[0].weight=font.GetChosenFont().GetWeight();
       }
       dialog->Destroy();
    }
    else if (event.GetId()==fontHigh->GetId())
    {
       font.SetInitialFont(fontHigh->GetFont());
       font.EnableEffects(false);

       wxFontDialog *dialog = new wxFontDialog(this,font);
       if (dialog->ShowModal() == wxID_OK)
       {
          font=dialog->GetFontData();          
          fontHigh->SetFont(font.GetChosenFont());

          objectData.font[1].pointSize=font.GetChosenFont().GetPointSize();
          objectData.font[1].style=font.GetChosenFont().GetStyle();
          objectData.font[1].weight=font.GetChosenFont().GetWeight();
       }
       dialog->Destroy();
    }
   else if (event.GetId()==wxID_OK)
   {
      /*** Std Panel *******************/
      object->fontFaceName[0]=fontLow->GetFont().GetFaceName();
      object->fontFaceName[1]=fontHigh->GetFont().GetFaceName();
      object->name=nameField->GetValue();
      object->text[0]=textLowField->GetValue();
      object->text[1]=textHighField->GetValue();
      object->MinValue=minField->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE);
      object->MaxValue=maxField->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE);

      if (disabledLow->GetValue()) objectData.state[0]|=HMI_STATE_FLAG_DISABLED;
      else objectData.state[0]&=~(HMI_STATE_FLAG_DISABLED);
      if (disabledHigh->GetValue()) objectData.state[1]|=HMI_STATE_FLAG_DISABLED;
      else objectData.state[1]&=~(HMI_STATE_FLAG_DISABLED);
        
      if (roLow->GetValue()) objectData.state[0]|=HMI_STATE_FLAG_RO;
      else objectData.state[0]&=~(HMI_STATE_FLAG_RO);
      if (roHigh->GetValue()) objectData.state[1]|=HMI_STATE_FLAG_RO;
      else objectData.state[1]&=~(HMI_STATE_FLAG_RO);

      objectData.mposx=posX->GetValue()*1000.0;
      objectData.mposy=posY->GetValue()*1000.0;
      objectData.msizex=sizeX->GetValue()*1000.0;
      objectData.msizey=sizeY->GetValue()*1000.0;

      if (object->data.type==HMI_TYPE_EXTERNAL_LIB)
      {
         ((hmiExternalIOLib*)object)->xmlHMIDecoder->getValuesFromUI();
      }
      if (logPanel) logPanel->getConfigData(objectData.log); // let the LogDefPanel update the log-parts of the object structure
      if (priviPanel) objectData.userPriviFlags=priviPanel->getPriviFlags();
      object->setConfigData();        
      // custom fields of the data-structure can be set within setConfigData() so we have to copy them to objectData here
      objectData.floatAccuracy  =object->data.floatAccuracy;
      objectData.hmiFlags       =object->data.hmiFlags;
      objectData.custom3        =object->data.custom3;
      object->setSize(wxRealPoint(objectData.msizex,objectData.msizey));
      object->setData(&objectData);
      g_isSaved=0;
      Destroy();
   }
   else if (event.GetId()==wxID_CANCEL)
   {
      Destroy();
   }
   else if (object->data.type==HMI_TYPE_EXTERNAL_LIB)
   {
      ((hmiExternalIOLib*)object)->xmlHMIDecoder->OnButtonClick(event);
   }
   g_colourDialog->SetParent(NULL);
   updateUI();
}

#endif //ENV_INT


