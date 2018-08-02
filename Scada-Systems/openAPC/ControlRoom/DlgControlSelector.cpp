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

#include <wx/gbsizer.h>
#include <wx/artprov.h>

#include "globals.h"
#include "DlgControlSelector.h"
#include "MainWin.h"

IMPLEMENT_CLASS(DlgControlSelector, wxDialog)

BEGIN_EVENT_TABLE(DlgControlSelector, wxDialog)
  EVT_BUTTON  (wxID_ANY,DlgControlSelector::OnButtonClick)
  EVT_LISTBOX_DCLICK(wxID_ANY, DlgControlSelector::OnDoubleClick)
  EVT_LISTBOX(wxID_ANY,DlgControlSelector::OnButtonClick)
END_EVENT_TABLE()



DlgControlSelector::DlgControlSelector(wxWindow* parent, const wxString& title,wxUint32 flags) //,wxByte showFlowState,bool showAll)
                   :wxDialog(parent,wxID_ANY,title,wxDefaultPosition,wxSize(300,400),wxRESIZE_BORDER|wxCAPTION,wxEmptyString),
                    m_flags(flags)
{
   wxInt32  i=0;
   wxNode  *node=NULL;

   wxBoxSizer *bSizer=new wxBoxSizer(wxVERTICAL);
   this->SetSizer(bSizer); 

   wxPanel *rootPanel=new wxPanel(this);
   bSizer->Add(rootPanel,1,wxEXPAND);

   g_createDialogButtons(this,bSizer,wxOK|wxCANCEL);

   wxGridBagSizer *gbSizer=new wxGridBagSizer(4,4);
   gbSizer->AddGrowableCol(0,1);
   gbSizer->AddGrowableRow(0,1);
   rootPanel->SetSizer(gbSizer);

   node=g_objectList.getObject((wxNode*)NULL);
   while (node)
   {
      flowObject *current=(flowObject*)node->GetData();
      if ((m_flags & DLGCONTROLSELECTOR_FLAG_SHOWALL) || (current->data.type & HMI_TYPE_MASK))
      {
         m_items.Add(current->name);
         if ((m_flags & DLGCONTROLSELECTOR_FLAG_SHOWFLOWSTATE) && (current->getFlowPos().x>-1)) m_items[i]=m_items[i]+_T(" *");
         i++;
      }
      node=g_objectList.getObject(node);
   }

   m_list=new wxListBox(rootPanel,wxID_ANY,wxDefaultPosition,wxDefaultSize,m_items);
   gbSizer->Add(m_list,wxGBPosition(0,0),wxGBSpan(12,3),wxEXPAND);

#ifdef ENV_EDITOR
   if (flags & DLGCONTROLSELECTOR_FLAG_ALLOWSORT)
   {
      m_upBtn=new wxBitmapButton(rootPanel,wxID_ANY,wxArtProvider::GetBitmap(wxART_GO_UP));
      m_upBtn->Enable(false);
      gbSizer->Add(m_upBtn,wxGBPosition(12,1),wxGBSpan(1,1),wxALIGN_RIGHT);

      m_downBtn=new wxBitmapButton(rootPanel,wxID_ANY,wxArtProvider::GetBitmap(wxART_GO_DOWN));
      m_downBtn->Enable(false);
      gbSizer->Add(m_downBtn,wxGBPosition(12,2),wxGBSpan(1,1),wxALIGN_LEFT);
   }
   else
   {
      m_upBtn=NULL;
      m_downBtn=NULL;
   }
#endif

   selectedObject=NULL;

   wxSize size=bSizer->Fit(this);
   if (size.y<260) size.y=260;
   if (size.x<DIALOG_X_SIZE) size.x=DIALOG_X_SIZE;
   SetSize(size);
   Center();
}



DlgControlSelector::~DlgControlSelector()
{
}



flowObject *DlgControlSelector::getSelectedControl()
{
    return selectedObject;
}



void DlgControlSelector::OnButtonClick(wxCommandEvent &event)
{
   if (event.GetId()==wxID_OK)
   {
      if (m_list->GetSelection()!=wxNOT_FOUND)
      {
         wxList::compatibility_iterator node=NULL;
         wxInt32                        cnt;

         cnt=m_list->GetSelection();
         node=g_objectList.getObject((wxObjectList::compatibility_iterator)NULL);
         while (node)
         { 
            flowObject *current=(flowObject*)node->GetData();
            if ((m_flags & DLGCONTROLSELECTOR_FLAG_SHOWALL) || (current->data.type & HMI_TYPE_MASK))
            {
               cnt--;
               if (cnt<0)
               {
                  selectedObject=(flowObject*)node->GetData();
                  break;
               }
            }
            node=g_objectList.getObject(node);
         }
      }
#ifdef ENV_EDITOR
      g_isSaved=false;
#endif
      Destroy();
   }
   else if (event.GetId()==wxID_CANCEL)
   {
      Destroy();
   }
#ifdef ENV_EDITOR
   else if ((m_upBtn) && (event.GetId()==m_upBtn->GetId())) changePos(-1);
   else if ((m_downBtn) && (event.GetId()==m_downBtn->GetId())) changePos(1);
   else if (event.GetId()==m_list->GetId()) updateUI();
#endif
}


#ifdef ENV_EDITOR
void DlgControlSelector::changePos(wxInt32 change)
{
   wxInt32     selected;
   wxString    item,otherItem;
   wxNode     *node,*otherNode;
   flowObject *moveObject;

   selected=m_list->GetSelection();
   if (selected==wxNOT_FOUND) return;
   item=m_list->GetString(selected);
   otherItem=m_list->GetString(selected+change);
   m_list->Delete(selected);
   m_list->Insert(item,selected+change);
   m_list->SetSelection(selected+change,true);
   updateUI();
   node=g_objectList.findObjectByName(item,NULL,1);
   otherNode=g_objectList.findObjectByName(otherItem,NULL,1);
   moveObject=(flowObject*)node->GetData();
   g_objectList.deleteObject(moveObject,false,false);
   if (change<0) g_objectList.InsertBefore(otherNode,moveObject);
   else g_objectList.InsertAfter(otherNode,moveObject);
}


void DlgControlSelector::updateUI()
{
   if ((m_upBtn) && (m_downBtn))
   {
      if ((m_list->GetCount()<=1) || (m_list->GetSelection()==wxNOT_FOUND))
      {
         m_upBtn->Enable(false);
         m_downBtn->Enable(false);
      }
      else
      {
         if (m_list->GetSelection()==0)
         {
            m_upBtn->Enable(false);
            m_downBtn->Enable(true);
         }
         else if (m_list->GetSelection()==(wxInt32)(m_list->GetCount()-1))
         {
            m_upBtn->Enable(true);
            m_downBtn->Enable(false);
         }
         else
         {
            m_upBtn->Enable(true);
            m_downBtn->Enable(true);
         }
      }
   }
}
#endif // ENV_EDITOR


void DlgControlSelector::OnDoubleClick(wxCommandEvent &WXUNUSED(event))
{
    wxCommandEvent myEvent(wxEVT_NULL,wxID_OK);

    OnButtonClick(myEvent);
}


