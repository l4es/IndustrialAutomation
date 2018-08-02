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
#include <wx/listctrl.h>
#include <wx/imaglist.h>

#include <vector>

#include "flowObject.h"
#include "flowExternalIOLib.h"
#include "PlugInPanel.h"
#include "FlowCanvas.h"
#include "MainWin.h"
#include "globals.h"

#include "common/wx/foldbar/foldpanelbar.h"


BEGIN_EVENT_TABLE(PlugInPanel, wxPanel)
  EVT_MENU(wxID_ANY,  PlugInPanel::OnPopupMenu)
  EVT_CONTEXT_MENU(   PlugInPanel::ShowContextMenu)
  EVT_BUTTON(wxID_ANY,PlugInPanel::OnPopupMenu)
END_EVENT_TABLE()

PlugInPanel::PlugInPanel(wxWindow *parent)
            :wxPanel(parent)
{
   m_item_list = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL);
   m_pImageList = new wxImageList(106,50,false);
   m_item_list->SetImageList(m_pImageList, wxIMAGE_LIST_SMALL);

   wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

   // Add first column       
   wxListItem col0;
   col0.SetId(0);
   col0.SetText( _("Device") );
   col0.SetWidth(110);
   m_item_list->InsertColumn(0, col0);
        
   // Add second column
   wxListItem col1;
   col1.SetId(1);
   col1.SetText( _("Name") );
   col1.SetWidth(250);
   m_item_list->InsertColumn(1, col1);
    
   // Add second column
   wxListItem col2;
   col2.SetId(2);
   col2.SetText( _("ID") );
   col2.SetWidth(40);
   col2.SetAlign(wxLIST_FORMAT_RIGHT);
   m_item_list->InsertColumn(2, col2);

   // Add second column
   wxListItem col3;
   col3.SetId(3);
   col3.SetText( _("Type") );
   col3.SetWidth(200);
   m_item_list->InsertColumn(3, col3);


   sizer->Add(m_item_list,1, wxEXPAND | wxALL);
   SetSizer(sizer);

   Connect(m_item_list->GetId(),wxEVT_COMMAND_LIST_ITEM_ACTIVATED,wxListEventHandler(PlugInPanel::OnListEditEvent),NULL,this);
//   Connect(m_item_list->GetId(),wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,wxListEventHandler(PlugInPanel::OnListEvent),NULL,this);
   
   newProject();
}


PlugInPanel::~PlugInPanel()
{
   delete m_pImageList;
}



wxFoldPanelBar *PlugInPanel::createFoldBar(wxWindow *parent)
{
   m_plugBar = new wxFoldPanelBar(parent,wxID_ANY,wxDefaultPosition,wxSize(200,500),wxFPB_DEFAULT_STYLE |wxFPB_VERTICAL,0);
   g_flowCanvas->addFoldBarSubmenu(m_plugBar,false);

   return m_plugBar;
}



void PlugInPanel::setEnabled(bool enabled)
{
   m_item_list->Enable(enabled);
   m_plugBar->Enable(enabled);
}



void PlugInPanel::OnListEditEvent(wxListEvent& WXUNUSED(event))
{
   flowObject *editObject;
   wxListItem  item;
   wxInt32     idx;

   idx=m_item_list->GetNextItem(-1,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED); 
   wxASSERT(idx>=0);
   editObject=g_objectList.getObject(itemID[idx]);
   wxASSERT(editObject);
   if (!editObject) return;
   
   g_mainWin->addUndoStep(_T("Edit Element"));
   editObject->doDataFlowDialog(true);
   setItemName(idx,editObject->name);
}



void PlugInPanel::ShowContextMenu(wxContextMenuEvent &WXUNUSED(event))
{
   wxInt32 idx;

   idx=m_item_list->GetNextItem(-1,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED); 
   DoShowContextMenu(idx==-1);
}


void PlugInPanel::DoShowContextMenu(bool isNew)
{
   wxMenu                         menu;
   wxMenuItem                    *item,*retItem;
   wxList::compatibility_iterator node;

   wxMenu *addMenu=new wxMenu();  
   g_flowCanvas->addDeviceSubmenu(addMenu);
   retItem=menu.AppendSubMenu(addMenu,_("Add Element"));
   if ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==0) retItem->Enable(false);

   item=new wxMenuItem(&menu,POPUP_EDIT_ELEM, _("Edit Element"));
   menu.Append(item);
   if ((isNew) ||
       ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==0)) item->Enable(false);
   
   item=new wxMenuItem(&menu,POPUP_DEL_ELEM, _("Delete Element"));
   menu.Append(item);
   if ((isNew) ||
       ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==0)) item->Enable(false);
   
   PopupMenu(&menu);
}


void PlugInPanel::OnFoldbarClicked(wxMouseEvent& event)
{
   wxCommandEvent cmdEvent;
   wxInt32        id;

   id=event.GetId();
   cmdEvent.SetId(id);
   OnPopupMenu(cmdEvent);
}



void PlugInPanel::OnPopupMenu(wxCommandEvent &event)
{
   switch (event.GetId())
   {
      case POPUP_EDIT_ELEM:
      {
         wxListEvent e;

         OnListEditEvent(e);
         break;
      }
      case POPUP_DEL_ELEM:
      {
         g_mainWin->addUndoStep(_T("Delete Element"));
         deleteItem(m_item_list->GetNextItem(-1,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED)); 
         break;
      }
      default:
      {
          ExternalIOLib *extIOLib;

          extIOLib=g_externalIOLibs->getLib(true);
          g_mainWin->addUndoStep(_T("Add Element"));
          while (extIOLib)
          {
             if ((extIOLib->uiID==event.GetId()) || (extIOLib->foldUiID==event.GetId()))
             {
                flowExternalIOLib *object;
                wxString           category;   

                object=new flowExternalIOLib(extIOLib,0);
                if (object->returnOK)
                {
//                   object->name=g_charToString(object->flowData.store_name);
                   object->setMode(OBJECT_MODE_DEVICE);
                   g_objectList.addObject(object,true,false);

                   addItem(object,extIOLib->getLibCapabilities());
                }
                else delete object;
                               
//                m_drawObjectList->addObject(object,true,false);
//                object->setFlowPos(m_drawObjectList,lastMousePos,1,1);
//                Refresh();
                break;
             }
             extIOLib=g_externalIOLibs->getLib(false);
          }
    	  break;
      }
   }
}



void PlugInPanel::addItem(flowExternalIOLib *object,wxUint32 capabilities)
{
   wxListItem item;
   wxInt32    idx;
   wxString   idStr;
   
   m_pImageList->Add(*object->flowBitmap);

   item.m_mask=0;
#ifdef ENV_LINUX
   item.SetId(m_item_list->GetItemCount());
   idx=m_item_list->InsertItem(item);
   itemID.push_back(object->data.id);
   m_item_list->SetItem(idx,1,wxEmptyString);
#endif
   item.SetImage(m_imgCnt);
   item.SetId(m_item_list->GetItemCount());
   idx=m_item_list->InsertItem(item);
   itemID.push_back(object->data.id);

   m_item_list->SetItem(idx,1,object->name);
   idStr=wxString::Format(_("%d"),object->data.id);
   m_item_list->SetItem(idx,2,idStr);
   if ((capabilities & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_IO) m_item_list->SetItem(idx,3,_("Input")+_T("/")+_("Output"));
   else if ((capabilities & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_MOTION) m_item_list->SetItem(idx,3,_("Motion"));
   else if ((capabilities & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_LASER) m_item_list->SetItem(idx,3,_("Laser"));

#ifdef ENV_LINUX
   item.m_mask=0;
   item.SetId(m_item_list->GetItemCount());
   idx=m_item_list->InsertItem(item);
   itemID.push_back(object->data.id);
   m_item_list->SetItem(idx,1,wxEmptyString);
#endif

   m_imgCnt++;
   g_isSaved=false;
}



void PlugInPanel::newProject()
{
   m_imgCnt=0;
   itemID.clear();
   m_pImageList->RemoveAll();
   m_item_list->DeleteAllItems();
}



void PlugInPanel::deleteItem(wxInt32 idx)
{
   if (idx<0) return;
   g_objectList.deleteObject(g_objectList.getObject(itemID[idx]),true,true);
#ifdef ENV_LINUX
   idx/=3;
   idx*=3;
   itemID.erase(itemID.begin()+idx);
   itemID.erase(itemID.begin()+idx);
   itemID.erase(itemID.begin()+idx);
   m_item_list->DeleteItem(idx);
   m_item_list->DeleteItem(idx);
   m_item_list->DeleteItem(idx);
#else
   m_item_list->DeleteItem(idx);
   itemID.erase(itemID.begin()+idx);
#endif
   g_isSaved=false;
}



void PlugInPanel::setItemName(wxInt32 idx,wxString name)
{
#ifdef ENV_LINUX
   idx/=3;
   idx*=3;
   idx++;
#endif
   m_item_list->SetItem(idx,1,name);
}


