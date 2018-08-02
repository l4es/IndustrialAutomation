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
#include <wx/bookctrl.h>
#include <wx/combobox.h>

#include "globals.h"
#include "hmiObject.h"
#include "flowGroup.h"
#include "DlgConfigflowGroup.h"
#include "IOElementDefinition.h"
#include "oapc_libio.h"
#include "MainWin.h"
#include "oapcNoSpaceCtrl.h"

IMPLEMENT_CLASS(DlgConfigflowGroup, wxDialog)

BEGIN_EVENT_TABLE(DlgConfigflowGroup, wxDialog)
  EVT_BUTTON  (wxID_ANY,DlgConfigflowGroup::OnButtonClick)
//  EVT_CHECKBOX(wxID_ANY,DlgConfigflowGroup::OnCheckBoxCheck)
END_EVENT_TABLE()


DlgConfigflowGroup::DlgConfigflowGroup(flowGroup *group,wxWindow* parent, const wxString& title, const wxPoint& pos,const wxString& name)
                      :wxDialog(parent,wxID_ANY,title,pos,wxDefaultSize,wxRESIZE_BORDER|wxCAPTION,name)
{
   wxNode              *node;
   IOElementDefinition *IOdef;
   wxString newTitle;

   this->group=group;

   returnOK=0;
#if wxCHECK_VERSION(2,9,0)
   newTitle=wxString::Format(_T("%s (%d)"),title,group->data.id);
#else
   newTitle=wxString::Format(_T("%s (%d)"),title.c_str(),group->data.id);
#endif
   SetTitle(newTitle);

   wxBoxSizer *bSizer=new wxBoxSizer(wxVERTICAL);
   this->SetSizer(bSizer);

   wxBookCtrl *book=new wxBookCtrl(this,wxID_ANY);
   bSizer->Add(book,1,wxEXPAND);

   g_createDialogButtons(this,bSizer,wxOK|wxCANCEL);

   wxPanel *panelBasic = new wxPanel(book, wxID_ANY);
   book->AddPage(panelBasic,_("Basic"),true);

   wxFlexGridSizer *fSizerBasic=new wxFlexGridSizer(2,4,4);
   fSizerBasic->AddGrowableCol(0,2);
   fSizerBasic->AddGrowableCol(1,5);
   panelBasic->SetSizer(fSizerBasic);

   // Name
    wxStaticText *text=new wxStaticText(panelBasic,wxID_ANY,_("Name"));
    fSizerBasic->Add(text,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

    m_nameField=new oapcNoSpaceCtrl(panelBasic,wxID_ANY,group->name);
    fSizerBasic->Add(m_nameField,1,wxEXPAND);

   // Category
   text=new wxStaticText(panelBasic,wxID_ANY,_("Category"));
   fSizerBasic->Add(text,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

   m_catComboBox=new wxComboBox(panelBasic,wxID_ANY,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_DROPDOWN|wxCB_READONLY);
   m_catComboBox->Append(_("Data Conversion"));
   m_catComboBox->Append(_("Logic Operations"));
   m_catComboBox->Append(_("Mathematical"));
   m_catComboBox->Append(_("Flow Control"));
   m_catComboBox->Append(_("Input")+_T("/")+_("Output"));
   m_catComboBox->Append(_("Motion"));
   m_catComboBox->Append(_("Laser"));
   m_catComboBox->Append(_("Data"));
   m_catComboBox->Append(_("Miscellaneous"));

   if (group->m_categoryFlag<<24==OAPC_FLOWCAT_CONVERSION)  m_catComboBox->SetSelection(0);
   else if (group->m_categoryFlag<<24==OAPC_FLOWCAT_LOGIC)  m_catComboBox->SetSelection(1);
   else if (group->m_categoryFlag<<24==OAPC_FLOWCAT_CALC)   m_catComboBox->SetSelection(2);
   else if (group->m_categoryFlag<<24==OAPC_FLOWCAT_FLOW)   m_catComboBox->SetSelection(3);
   else if (group->m_categoryFlag<<24==OAPC_FLOWCAT_IO)     m_catComboBox->SetSelection(4);
   else if (group->m_categoryFlag<<24==OAPC_FLOWCAT_MOTION) m_catComboBox->SetSelection(5);
   else if (group->m_categoryFlag<<24==OAPC_FLOWCAT_LASER)  m_catComboBox->SetSelection(6);
   else if (group->m_categoryFlag<<24==OAPC_FLOWCAT_DATA)   m_catComboBox->SetSelection(7);
   else m_catComboBox->SetSelection(8);

   fSizerBasic->Add(m_catComboBox,1,wxEXPAND);

    wxScrolledWindow *scrollerIN = new wxScrolledWindow(book, wxID_ANY,wxDefaultPosition,wxDefaultSize,wxVSCROLL);
    book->AddPage(scrollerIN,_("Inputs"),false);

    wxFlexGridSizer *fSizerIN=new wxFlexGridSizer(2,4,4);
    fSizerIN->AddGrowableCol(0,3);
    fSizerIN->AddGrowableCol(1,1);
    scrollerIN->SetSizer(fSizerIN);

    text=new wxStaticText(scrollerIN,wxID_ANY,_("Name"));
    fSizerIN->Add(text);
    text=new wxStaticText(scrollerIN,wxID_ANY,_("Type"));
    fSizerIN->Add(text);
    
    node=group->getINObjectList()->getObject((wxNode*)NULL);
    while (node)
    {
       IOdef=(IOElementDefinition*)node->GetData();

       IOdef->m_nameField=new oapcNoSpaceCtrl(scrollerIN,wxID_ANY,wxEmptyString);
       IOdef->m_nameField->SetMaxLength(10);
       IOdef->m_nameField->SetValue(IOdef->name);
       fSizerIN->Add(IOdef->m_nameField,1,wxEXPAND);

       if (IOdef->data.IOFlag & OAPC_CHAR_IO_MASK) text=new wxStaticText(scrollerIN,wxID_ANY,_("characters"));
       else if (IOdef->data.IOFlag & OAPC_NUM_IO_MASK) text=new wxStaticText(scrollerIN,wxID_ANY,_("numeric"));
       else if (IOdef->data.IOFlag & OAPC_DIGI_IO_MASK) text=new wxStaticText(scrollerIN,wxID_ANY,_("digital"));
       fSizerIN->Add(text,1,wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);

       node=group->getINObjectList()->getObject(node);
    }

    wxScrolledWindow *scrollerOUT = new wxScrolledWindow(book, wxID_ANY,wxDefaultPosition,wxDefaultSize,wxVSCROLL);
    book->AddPage(scrollerOUT,_("Outputs"),false);

    wxFlexGridSizer *fSizerOUT=new wxFlexGridSizer(2,4,4);
    fSizerOUT->AddGrowableCol(0,3);
    fSizerOUT->AddGrowableCol(1,1);
    scrollerOUT->SetSizer(fSizerOUT);

    text=new wxStaticText(scrollerOUT,wxID_ANY,_("Name"));
    fSizerOUT->Add(text);
    text=new wxStaticText(scrollerOUT,wxID_ANY,_("Type"));
    fSizerOUT->Add(text);

   node=group->getOUTObjectList()->getObject((wxNode*)NULL);
   while (node)
   {
      IOdef=(IOElementDefinition*)node->GetData();

      IOdef->m_nameField=new oapcNoSpaceCtrl(scrollerOUT,wxID_ANY,wxEmptyString);
      IOdef->m_nameField->SetMaxLength(10);
      IOdef->m_nameField->SetValue(IOdef->name);
      fSizerOUT->Add(IOdef->m_nameField,1,wxEXPAND);
      if (IOdef->data.IOFlag & OAPC_CHAR_IO_MASK) text=new wxStaticText(scrollerOUT,wxID_ANY,_("characters"));
      else if (IOdef->data.IOFlag & OAPC_NUM_IO_MASK) text=new wxStaticText(scrollerOUT,wxID_ANY,_("numeric"));
      else if (IOdef->data.IOFlag & OAPC_DIGI_IO_MASK) text=new wxStaticText(scrollerOUT,wxID_ANY,_("digital"));
      else if (IOdef->data.IOFlag & OAPC_BIN_IO_MASK) text=new wxStaticText(scrollerOUT,wxID_ANY,_("binary"));
      else wxASSERT(0);
      fSizerOUT->Add(text,1,wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT);

      node=group->getOUTObjectList()->getObject(node);
   }

   updateUI();
   wxInt32 x,y=bSizer->Fit(this).y;

   if (y<200) y=200;
   else if (y>500) y=500;
   SetSize(DIALOG_X_SIZE,y);

   scrollerIN->GetViewStart(&x,&y);
   scrollerIN->SetScrollbars(HMI_SCROLL_UNIT,HMI_SCROLL_UNIT,scrollerIN->GetSize().x/10,scrollerIN->GetSize().y/10,x,y);
   scrollerOUT->GetViewStart(&x,&y);
   scrollerOUT->SetScrollbars(HMI_SCROLL_UNIT,HMI_SCROLL_UNIT,scrollerOUT->GetSize().x/10,scrollerOUT->GetSize().y/10,x,y);
   Center();
}



DlgConfigflowGroup::~DlgConfigflowGroup()
{

}



void DlgConfigflowGroup::updateUI()
{

}



void DlgConfigflowGroup::OnButtonClick(wxCommandEvent &event)
{
    if (event.GetId()==wxID_OK)
    {
       wxNode *node;
       IOElementDefinition *IODef;
       ObjectList          *list;
       wxUint32             i;

       g_isSaved=false;
       returnOK=1;

       list=group->getINObjectList();
       node=list->getObject((wxNode*)NULL);
       for (i=0; i<2; i++)
       {
          while (node)
          {
              IODef=(IOElementDefinition*)node->GetData();
              if (IODef->m_nameField) IODef->name=IODef->m_nameField->GetValue();
              IODef->m_nameField=NULL;
              node=list->getObject(node);
          }
          list=group->getOUTObjectList();
          node=list->getObject(node);
       }
       group->name=m_nameField->GetValue();

       if (m_catComboBox->GetSelection()==0)      group->m_categoryFlag=OAPC_FLOWCAT_CONVERSION>>24;
       else if (m_catComboBox->GetSelection()==1) group->m_categoryFlag=OAPC_FLOWCAT_LOGIC>>24;
       else if (m_catComboBox->GetSelection()==2) group->m_categoryFlag=OAPC_FLOWCAT_CALC>>24;
       else if (m_catComboBox->GetSelection()==3) group->m_categoryFlag=OAPC_FLOWCAT_FLOW>>24;
       else if (m_catComboBox->GetSelection()==4) group->m_categoryFlag=OAPC_FLOWCAT_IO>>24;
       else if (m_catComboBox->GetSelection()==5) group->m_categoryFlag=OAPC_FLOWCAT_MOTION>>24;
       else if (m_catComboBox->GetSelection()==6) group->m_categoryFlag=OAPC_FLOWCAT_LASER>>24;
       else if (m_catComboBox->GetSelection()==7) group->m_categoryFlag=OAPC_FLOWCAT_DATA>>24;
       else group->m_categoryFlag=0;

       g_isSaved=false;
       Destroy();
    }
    else if (event.GetId()==wxID_CANCEL)
    {
       Destroy();
    }

}





