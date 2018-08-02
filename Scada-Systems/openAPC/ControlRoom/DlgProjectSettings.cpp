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
#include "../DlgProjectSettings.cpp"
#else //ENV_INT

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/gbsizer.h>
#include <wx/fontdlg.h>

#include "globals.h"
#include "MainWin.h"
#include "DlgProjectSettings.h"
#include "PlugInPanel.h"
#include "oapcFloatCtrl.h"


BEGIN_EVENT_TABLE(DlgProjectSettings, wxDialog)
   EVT_BUTTON(wxID_ANY, DlgProjectSettings::OnButtonClick)
   EVT_CHECKBOX(wxID_ANY, DlgProjectSettings::OnButtonClick)
END_EVENT_TABLE()



DlgProjectSettings::DlgProjectSettings(wxWindow* parent,const wxString & title)
       : wxDialog(parent,wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
   wxString s;

   wxBoxSizer *bSizer=new wxBoxSizer(wxVERTICAL);
   this->SetSizer(bSizer);

   wxNotebook *rootBook=new wxNotebook(this,wxID_ANY, wxDefaultPosition,wxDefaultSize,0/* wxNB_MULTILINE*/);

   wxNotebookPage *rootPanel=new wxNotebookPage(rootBook, -1);
   bSizer->Add(rootBook,wxEXPAND);

   g_createDialogButtons(this,bSizer,wxOK|wxCANCEL);

   wxGridBagSizer *gbSizer=new wxGridBagSizer(4,4);
/*   fSizer->AddGrowableCol(0,5);
   fSizer->AddGrowableCol(1,6);
   fSizer->AddGrowableCol(2,6);*/

   rootPanel->SetSizer(gbSizer);

    wxStaticText *text=new wxStaticText(rootPanel,wxID_ANY,_("Visual Size")+_T(":"));
    gbSizer->Add(text,wxGBPosition(0,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
    visW=new wxSpinCtrl(rootPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,160,16000,g_objectList.m_projectData->totalW);
    visW->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    gbSizer->Add(visW,wxGBPosition(0,1),wxGBSpan(1,2),wxEXPAND);
    visH=new wxSpinCtrl(rootPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,120,12000,g_objectList.m_projectData->totalH);
    visH->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    gbSizer->Add(visH,wxGBPosition(0,3),wxGBSpan(1,2),wxEXPAND);

    text=new wxStaticText(rootPanel,wxID_ANY,_("Visual Grid Size")+_T(":"));
    gbSizer->Add(text,wxGBPosition(1,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
    visGridW=new wxSpinCtrl(rootPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,2,100,g_objectList.m_projectData->gridW);
    visGridW->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    gbSizer->Add(visGridW,wxGBPosition(1,1),wxGBSpan(1,2),wxEXPAND);
    visGridH=new wxSpinCtrl(rootPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,2,100,g_objectList.m_projectData->gridH);
    visGridH->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    gbSizer->Add(visGridH,wxGBPosition(1,3),wxGBSpan(1,2),wxEXPAND);

    snapGridCB=new wxCheckBox(rootPanel,wxID_ANY,_("Snap To Grid"));   
    if (g_objectList.m_projectData->flags & PROJECT_FLAG_SNAPTOGRID) snapGridCB->SetValue(1);
    gbSizer->Add(snapGridCB,wxGBPosition(2,1),wxGBSpan(1,4),wxEXPAND);
    
    text=new wxStaticText(rootPanel,wxID_ANY,_("Background Colour")+_T(":"));
    gbSizer->Add(text,wxGBPosition(3,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
    visbgCol=new wxButton(rootPanel,wxID_ANY);   
    visbgCol->SetBackgroundColour(wxColour(g_objectList.m_projectData->bgCol));
    gbSizer->Add(visbgCol,wxGBPosition(3,1),wxGBSpan(1,4),wxEXPAND);
    
    text=new wxStaticText(rootPanel,wxID_ANY,_("Control Flow Time-Out")+_T(":"));
    gbSizer->Add(text,wxGBPosition(4,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
    flowT=new wxSpinCtrl(rootPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,50,10000,g_objectList.m_projectData->flowTimeout);
    flowT->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    gbSizer->Add(flowT,wxGBPosition(4,1),wxGBSpan(1,2),wxEXPAND);
    text=new wxStaticText(rootPanel,wxID_ANY,_("msec"));
    gbSizer->Add(text,wxGBPosition(4,3),wxGBSpan(1,2),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
    
    text=new wxStaticText(rootPanel,wxID_ANY,_("Timer Resolution")+_T(":"));
    gbSizer->Add(text,wxGBPosition(5,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
    timerRes=new wxSpinCtrl(rootPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,10,1000,g_objectList.m_projectData->timerResolution);
    timerRes->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    gbSizer->Add(timerRes,wxGBPosition(5,1),wxGBSpan(1,2),wxEXPAND);
    text=new wxStaticText(rootPanel,wxID_ANY,_("msec"));
    gbSizer->Add(text,wxGBPosition(5,3),wxGBSpan(1,2),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);    

    /** Interlock Server modes ****************************************************************/
    text=new wxStaticText(rootPanel,wxID_ANY,_("External applications")+_T(":"));
    gbSizer->Add(text,wxGBPosition(6,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

    locSingleIS=new wxCheckBox(rootPanel,wxID_ANY,_("Use local Interlock Server"));
    gbSizer->Add(locSingleIS,wxGBPosition(6,1),wxGBSpan(1,4),wxEXPAND);

    remSingleIS=new wxCheckBox(rootPanel,wxID_ANY,_("Use remote Interlock Server"));
    gbSizer->Add(remSingleIS,wxGBPosition(7,1),wxGBSpan(1,4),wxEXPAND);

    remSingleIPText=new wxStaticText(rootPanel,wxID_ANY,_("Server IP")+_T(":"));
    gbSizer->Add(remSingleIPText,wxGBPosition(8,1),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
    remSingleIP=new wxTextCtrl(rootPanel,wxID_ANY);
    oapc_unicode_charToStringUTF8(g_objectList.m_projectData->m_remSingleIP,sizeof(g_objectList.m_projectData->m_remSingleIP),&s);
    remSingleIP->SetValue(s);
    gbSizer->Add(remSingleIP,wxGBPosition(8,2),wxGBSpan(1,3),wxEXPAND);

    locDoubleIS=new wxCheckBox(rootPanel,wxID_ANY,_("Use local, mirrored Interlock Servers"));
    gbSizer->Add(locDoubleIS,wxGBPosition(9,1),wxGBSpan(1,4),wxEXPAND);

    remDoubleIS=new wxCheckBox(rootPanel,wxID_ANY,_("Use remote, redundant Interlock Servers"));
    gbSizer->Add(remDoubleIS,wxGBPosition(10,1),wxGBSpan(1,4),wxEXPAND);

    remDoubleIP1Text=new wxStaticText(rootPanel,wxID_ANY,_("Server IP")+_T(":"));
    gbSizer->Add(remDoubleIP1Text,wxGBPosition(11,1),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
    remDoubleIP1=new wxTextCtrl(rootPanel,wxID_ANY);
    oapc_unicode_charToStringUTF8(g_objectList.m_projectData->m_remSingleIP,sizeof(g_objectList.m_projectData->m_remSingleIP),&s);
    remDoubleIP1->SetValue(s);
    gbSizer->Add(remDoubleIP1,wxGBPosition(11,2),wxGBSpan(1,3),wxEXPAND);

    remDoubleIP2Text=new wxStaticText(rootPanel,wxID_ANY,_("Server IP")+_T(":"));
    gbSizer->Add(remDoubleIP2Text,wxGBPosition(12,1),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
    remDoubleIP2=new wxTextCtrl(rootPanel,wxID_ANY);
    oapc_unicode_charToStringUTF8(g_objectList.m_projectData->m_remDoubleIP,sizeof(g_objectList.m_projectData->m_remDoubleIP),&s);
    remDoubleIP2->SetValue(s);
    gbSizer->Add(remDoubleIP2,wxGBPosition(12,2),wxGBSpan(1,3),wxEXPAND);

    if ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_SINGLE_LOCAL) locSingleIS->SetValue(1);
    else if ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_SINGLE_REMOTE) remSingleIS->SetValue(1);
    else if ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_DOUBLE_LOCAL) locDoubleIS->SetValue(1);
    else if ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_DOUBLE_REMOTE) remDoubleIS->SetValue(1);
    /** end of Interlock Server modes ****************************************************************/

    text=new wxStaticText(rootPanel,wxID_ANY,_("Touchscreen Support")+_T(":"));
    gbSizer->Add(text,wxGBPosition(13,0),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
    touchCB=new wxCheckBox(rootPanel,wxID_ANY,_("Activate"));   
    if (g_objectList.m_projectData->flags & PROJECT_FLAG_TOUCHSCREEN_SUPPORT) touchCB->SetValue(1);
    gbSizer->Add(touchCB,wxGBPosition(13,1),wxGBSpan(1,4),wxEXPAND);

    touchFactorText=new wxStaticText(rootPanel,wxID_ANY,_("Keyboard Size Factor")+_T(":"));
    gbSizer->Add(touchFactorText,wxGBPosition(14,1),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
    touchFactor=new oapcFloatCtrl(rootPanel,wxID_ANY);
    touchFactor->SetValue(g_objectList.m_projectData->touchFactor/1000.0);
    gbSizer->Add(touchFactor,wxGBPosition(14,2),wxGBSpan(1,3),wxEXPAND);

    touchFontText=new wxStaticText(rootPanel,wxID_ANY,_("Keyboard Font")+_T(":"));
    gbSizer->Add(touchFontText,wxGBPosition(15,1),wxGBSpan(1,1),wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
    touchFont=new wxButton(rootPanel,wxID_ANY,_("AaBbCc123"));
    touchFont->SetBackgroundColour(*wxWHITE);
#if wxCHECK_VERSION(2,9,0)
    touchFont->SetFont(wxFont(g_objectList.m_projectData->font.pointSize,
                              wxFONTFAMILY_DEFAULT,
                              (wxFontStyle)g_objectList.m_projectData->font.style,
                              (wxFontWeight)g_objectList.m_projectData->font.weight,
                              false,
                              g_projectTouchFontFaceName));
#else
    touchFont->SetFont(wxFont(g_objectList.m_projectData->font.pointSize,
                              wxFONTFAMILY_DEFAULT,
                              g_objectList.m_projectData->font.style,
                              g_objectList.m_projectData->font.weight,
                              false,
                              g_projectTouchFontFaceName));
#endif
    gbSizer->Add(touchFont,wxGBPosition(15,2),wxGBSpan(1,3),wxEXPAND);

    gbSizer->AddGrowableRow(15,1);

   rootBook->AddPage(rootPanel,_("Project"));
   rootPanel->Layout();

   SetSize(bSizer->Fit(this).x,bSizer->Fit(this).y+14);//DIALOG_X_SIZE,bSizer->Fit(this).y);
   Center();
   updateUI();
}




DlgProjectSettings::~DlgProjectSettings()
{

}



void DlgProjectSettings::updateUI()
{
   bool enable;

   enable=remSingleIS->IsChecked();
   remSingleIPText->Enable(enable);
   remSingleIP->Enable(enable);

   enable=remDoubleIS->IsChecked();
   remDoubleIP1Text->Enable(enable);
   remDoubleIP1->Enable(enable);
   remDoubleIP2Text->Enable(enable);
   remDoubleIP2->Enable(enable);

   enable=touchCB->IsChecked();
   touchFont->Enable(enable);
   touchFontText->Enable(enable);
   touchFactor->Enable(enable);
   touchFactorText->Enable(enable);
}



void DlgProjectSettings::OnButtonClick(wxCommandEvent &event)
{
   if ((event.GetId()==locSingleIS->GetId()) && (locSingleIS->IsChecked()))
   {
      remSingleIS->SetValue(false);
      locDoubleIS->SetValue(false);
      remDoubleIS->SetValue(false);
   }
   else if ((event.GetId()==remSingleIS->GetId()) && (remSingleIS->IsChecked()))
   {
      locSingleIS->SetValue(false);
      locDoubleIS->SetValue(false);
      remDoubleIS->SetValue(false);
   }
   else if ((event.GetId()==locDoubleIS->GetId()) && (locDoubleIS->IsChecked()))
   {
      remSingleIS->SetValue(false);
      locSingleIS->SetValue(false);
      remDoubleIS->SetValue(false);
   }
   else if ((event.GetId()==remDoubleIS->GetId()) && (remDoubleIS->IsChecked()))
   {
      remSingleIS->SetValue(false);
      locSingleIS->SetValue(false);
      locDoubleIS->SetValue(false);
   }
   else if (event.GetId()==visbgCol->GetId())
   {
      wxColourData colour;

      colour.SetColour(visbgCol->GetBackgroundColour());

      g_colourDialog->Create(this,&colour);
      g_colourDialog->SetTitle(_("Choose the background colour"));
      g_colourDialog->Centre();
      if (g_colourDialog->ShowModal() == wxID_OK)
      {
         colour=g_colourDialog->GetColourData();
         visbgCol->SetBackgroundColour(colour.GetColour());
      }
   }
   else if (event.GetId()==touchFont->GetId())
   {
      wxFontData   font;

      font.SetInitialFont(touchFont->GetFont());
      font.EnableEffects(false);

      wxFontDialog *dialog = new wxFontDialog(this,font);
      if (dialog->ShowModal() == wxID_OK)
      {
         font=dialog->GetFontData();          
         touchFont->SetFont(font.GetChosenFont());
      }
      dialog->Destroy();
   }
   else if (event.GetId()==wxID_OK)
   {
      wxFont font;

      g_objectList.m_projectData->totalW=visW->GetValue();
      g_objectList.m_projectData->totalH=visH->GetValue();
      g_objectList.m_projectData->gridW=visGridW->GetValue();
      g_objectList.m_projectData->gridH=visGridH->GetValue();

      g_objectList.m_projectData->bgCol=visbgCol->GetBackgroundColour().Red() |
                          visbgCol->GetBackgroundColour().Green()<<8 |
                          visbgCol->GetBackgroundColour().Blue()<<16;

      g_objectList.m_projectData->flowTimeout=flowT->GetValue();
      g_objectList.m_projectData->timerResolution=timerRes->GetValue();
      if (snapGridCB->IsChecked()) g_objectList.m_projectData->flags|=PROJECT_FLAG_SNAPTOGRID;
      else g_objectList.m_projectData->flags&=~PROJECT_FLAG_SNAPTOGRID;

      g_objectList.m_projectData->flags&=~PROJECT_FLAG_ISPACE_MASK;
      if (locSingleIS->IsChecked()) g_objectList.m_projectData->flags|=PROJECT_FLAG_ISPACE_SINGLE_LOCAL;
      else if (remSingleIS->IsChecked())
      {
         g_objectList.m_projectData->flags|=PROJECT_FLAG_ISPACE_SINGLE_REMOTE;
         oapc_unicode_stringToCharUTF8(remSingleIP->GetValue(),g_objectList.m_projectData->m_remSingleIP,sizeof(g_objectList.m_projectData->m_remSingleIP));
      }
      else if (locDoubleIS->IsChecked()) g_objectList.m_projectData->flags|=PROJECT_FLAG_ISPACE_DOUBLE_LOCAL;
      else if (remDoubleIS->IsChecked())
      {
         g_objectList.m_projectData->flags|=PROJECT_FLAG_ISPACE_DOUBLE_REMOTE;
         oapc_unicode_stringToCharUTF8(remDoubleIP1->GetValue(),g_objectList.m_projectData->m_remSingleIP,sizeof(g_objectList.m_projectData->m_remSingleIP));
         oapc_unicode_stringToCharUTF8(remDoubleIP2->GetValue(),g_objectList.m_projectData->m_remDoubleIP,sizeof(g_objectList.m_projectData->m_remDoubleIP));
      }
      else if (locDoubleIS->IsChecked()) g_objectList.m_projectData->flags|=PROJECT_FLAG_ISPACE_DOUBLE_LOCAL;
      g_plugInPanel->setEnabled((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)!=0);

      if (touchCB->IsChecked())
      {
         g_objectList.m_projectData->flags|=PROJECT_FLAG_TOUCHSCREEN_SUPPORT;
         g_objectList.m_projectData->touchFactor=touchFactor->GetValue(1.0,20.0)*1000;
         font=touchFont->GetFont();
         g_objectList.m_projectData->font.pointSize=font.GetPointSize();
         g_objectList.m_projectData->font.style=font.GetStyle();
         g_objectList.m_projectData->font.weight=font.GetWeight();
         g_projectTouchFontFaceName=font.GetFaceName();
      }
      else g_objectList.m_projectData->flags&=~PROJECT_FLAG_TOUCHSCREEN_SUPPORT;

      g_isSaved=0;
      Destroy();
      return;
   }
   else if (event.GetId()==wxID_CANCEL)
   {
       Destroy();
       return;
   }
   updateUI();
}

#endif //ENV_INT
