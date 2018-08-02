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

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/generic/gridctrl.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/colour.h>

#include "WatchWin.h"
#include "DlgControlSelector.h"
#include "globals.h"


// ----------------------------------------------------------------------------
// GridFrame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE( WatchWin, wxFrame )
    EVT_GRID_CELL_LEFT_DCLICK( WatchWin::OnCellLeftDClick )
    EVT_GRID_LABEL_LEFT_DCLICK( WatchWin::OnCellLeftDClick )
    EVT_GRID_CELL_LEFT_CLICK( WatchWin::OnCellLeftClick )
#if wxCHECK_VERSION(2,9,0)
    EVT_GRID_CELL_CHANGED( WatchWin::OnCellValueChanged ) // 2.8: EVT_GRID_CELL_CHANGE
#else
    EVT_GRID_CELL_CHANGE( WatchWin::OnCellValueChanged) 
#endif
    EVT_CLOSE(WatchWin::OnClose)
    EVT_GRID_EDITOR_SHOWN(WatchWin::OnEditorShown)
END_EVENT_TABLE()


WatchWin::WatchWin(wxString projectName)
        : wxFrame(NULL, wxID_ANY,wxEmptyString,
                   wxDefaultPosition,wxSize(800,200))
{
   wxInt32   i;
   wxString  sTmp;
   wxFile *FHandle;

   SetTitle(_("Watch connected outputs"));
   SetIcon(wxIcon(icon_xpm));   

   m_projectName=projectName;
   m_nextNewRowNum=-1;
   grid = new wxGrid(this,wxID_ANY,wxPoint( 0, 0 ),wxSize( 500, 200 ) );
   grid->CreateGrid( 0, 0 );
   grid->AppendCols(8);
   grid->SetRowLabelSize(160);
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      sTmp=wxString::Format(_T("OUT%d"),i);
      grid->SetColLabelValue(i,sTmp);
   }
   createNewCol(-1,-1);

   sTmp=wxStandardPaths::Get().GetUserDataDir()+wxFileName::GetPathSeparator()+m_projectName+_T(".dbg");

   if (wxFile::Exists(sTmp))
   {
      FHandle=new wxFile(sTmp,wxFile::read);
      if (FHandle->IsOpened())
      {
		   wxInt32 x,y,id,height;

   		FHandle->Read(&x,sizeof(x)); FHandle->Read(&y,sizeof(y));
	   	if ((x>=0) && (y>=0)) Move(x,y);
		   FHandle->Read(&x,sizeof(x)); FHandle->Read(&y,sizeof(y));
   		if ((x>10) && (y>10)) SetSize(x,y);
      	for (i=0; i<8; i++)
         {
   		   if (FHandle->Read(&x,sizeof(x))==sizeof(x))
            {
   		      if (x>10) grid->SetColSize(i,x);
            }               
         }
   	   while (FHandle->Read(&id,sizeof(id))==sizeof(id))
         {
	   	   if (FHandle->Read(&height,sizeof(height))==sizeof(height))
   	   	 createNewCol(id,height);
         }

         FHandle->Close();
      }
      delete FHandle;
   }
   if (m_nextNewRowNum<0) createNewCol(-1,-1);
   wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL );
   this->SetSizer(topSizer);
   topSizer->Add(grid,1,wxEXPAND);
}



WatchWin::~WatchWin()
{
   m_watchList.DeleteContents(true);
}


wxInt32 WatchWin::getRowFromID(wxInt32 id)
{
	wxInt32           i;
	WatchListElement *elem;
	wxNode           *node;
	
	for (i=0; i<m_nextNewRowNum; i++)
	{
	   node=m_watchList.Item(i);
   	wxASSERT(node);
	  	if (node)
		{
		   elem=(WatchListElement*)node->GetData();
		   if (elem->m_id==id) return i;
		}
	}
	return -1;
}



void WatchWin::setNewValue(wxInt32 outputNum,wxByte digi,wxInt32 id)
{
	wxInt32  row=getRowFromID(id);
	wxString val;
   
   if (row<0) return;
   if (digi)
   {
      val=_T("HIGH");
      grid->SetCellTextColour(row,outputNum,*wxRED);
   }
   else
   {
      val=_T("LOW");
      grid->SetCellTextColour(row,outputNum,DARK_GREEN_COLOUR);
   }
   grid->SetCellValue(row,outputNum,val);
}



void WatchWin::setNewValue(wxInt32 outputNum,wxFloat64 num,wxInt32 id)
{
	wxInt32  row=getRowFromID(id);
	wxString val;
   
   if (row<0) return;
   val=val.Format(_T("%f"),num);
   grid->SetCellValue(row,outputNum,val);
}



void WatchWin::setNewValue(wxInt32 outputNum,wxString txt,wxInt32 id)
{
	wxInt32  row=getRowFromID(id);
   
   if (row<0) return;
   grid->SetCellValue(row,outputNum,txt);
}



void WatchWin::setNewValue(wxInt32 outputNum,oapcBinHeadSp &bin,wxInt32 id)
{
	wxInt32  row=getRowFromID(id);
	wxString val;
   
   if (row<0) return;
   val=val.Format(_T("0x%X"),bin->m_bin);
   grid->SetCellValue(row,outputNum,val);
}



void WatchWin::createNewCol(wxInt32 id,wxInt32 height)
{
	wxInt32     i;
   flowObject *object=NULL;

   if (m_nextNewRowNum>=0)
   {
      if (id<=0)
      {
         DlgControlSelector dlg(this,_("Flow element to watch"),DLGCONTROLSELECTOR_FLAG_SHOWALL);//0,true);

         dlg.ShowModal();
         object=dlg.getSelectedControl();
      }
      else object=g_objectList.getObject(id);
      if (object)
      {
         WatchListElement *elem;
         wxUint64          bmask=0x01010101;
   	
         grid->SetCellValue(m_nextNewRowNum,0,wxT(""));
         grid->SetRowLabelValue(m_nextNewRowNum,object->name);
         for (i=0; i<MAX_NUM_IOS; i++)
         {
            grid->SetCellAlignment(m_nextNewRowNum,i,wxALIGN_RIGHT, wxALIGN_CENTRE);
            grid->SetReadOnly(m_nextNewRowNum,i,false);
         	if (bmask & object->data.stdOUT & OAPC_DIGI_IO_MASK)
         	{
               grid->SetCellAlignment(m_nextNewRowNum,i,wxALIGN_CENTRE,wxALIGN_CENTRE);
               grid->SetReadOnly(m_nextNewRowNum,i,true);
               grid->SetCellBackgroundColour(m_nextNewRowNum,i,*wxWHITE);
         	}
         	else if (bmask & object->data.stdOUT & OAPC_NUM_IO_MASK)
         	{
               grid->SetCellEditor(m_nextNewRowNum,i,new wxGridCellFloatEditor());
               grid->SetCellRenderer(m_nextNewRowNum,i,new wxGridCellFloatRenderer());
               grid->SetCellBackgroundColour(m_nextNewRowNum,i,*wxCYAN);
         	}
         	else if (bmask & object->data.stdOUT & OAPC_CHAR_IO_MASK)
         	{
               grid->SetCellAlignment(m_nextNewRowNum,i,wxALIGN_LEFT, wxALIGN_CENTRE);
               grid->SetCellEditor(m_nextNewRowNum,i,new wxGridCellAutoWrapStringEditor());
               grid->SetCellRenderer(m_nextNewRowNum,i,new wxGridCellAutoWrapStringRenderer());
               grid->SetCellBackgroundColour(m_nextNewRowNum,i,MAGENTA_COLOUR);
         	}
         	else if (bmask & object->data.stdOUT & OAPC_BIN_IO_MASK)
         	{
               grid->SetReadOnly(m_nextNewRowNum,i,true); // modification of binary data is not allowed
               grid->SetCellBackgroundColour(m_nextNewRowNum,i,YELLOW_COLOUR);
         	}            
         	else // unused output
         	{
               grid->SetReadOnly(m_nextNewRowNum,i,true);
               grid->SetCellBackgroundColour(m_nextNewRowNum,i,*wxLIGHT_GREY);
         	}
            bmask=bmask<<1;
         }
         grid->ClearSelection();
#if wxCHECK_VERSION(3,1,0)
         if (height>8) grid->SetRowSize(m_nextNewRowNum,height);
#else
         if (height>8) grid->SetRowHeight(m_nextNewRowNum,height);
#endif
         elem=new WatchListElement();
         elem->m_id=object->data.id;
         elem->m_object=object;
         m_watchList.Append(elem);
      }
   }
   if ((object) || (m_nextNewRowNum<0))
   {
      m_nextNewRowNum++;
      grid->AppendRows(1);
      grid->SetCellAlignment(m_nextNewRowNum,-1,wxALIGN_LEFT, wxALIGN_CENTRE);
      grid->SetCellAlignment(m_nextNewRowNum,0,wxALIGN_LEFT, wxALIGN_CENTRE);
      grid->SetRowLabelValue(m_nextNewRowNum,_("New watch element"));
      for (i=0; i<8; i++)
      {
//      grid->SetReadOnly(m_nextNewRowNum,i,true);
      }
   }
}



void WatchWin::OnEditorShown( wxGridEvent& ev )
{
    ev.Skip();
}



void WatchWin::OnCellLeftClick( wxGridEvent& ev )
{
   wxNode           *node;
   WatchListElement *elem;
   wxUint32          bmask=0x00000001; // binary data can't be changed, other data are handled below
   wxByte            nextDigi;
   wxString          nextChar;
   	
  	node=m_watchList.Item(ev.GetRow());
  	wxASSERT(node);
  	if (node)
  	{
      elem=(WatchListElement*)node->GetData();
      wxASSERT(elem->m_object);
      if (elem->m_object)
      {
         bmask=bmask<<ev.GetCol();
         if (bmask & elem->m_object->data.stdOUT & OAPC_DIGI_IO_MASK)
         {
            if (grid->GetCellValue(ev.GetRow(),ev.GetCol()).Cmp(_T("HIGH"))) nextDigi=1;
            else nextDigi=0;
            setNewValue(ev.GetCol(),nextDigi,elem->m_id);
            g_flowPool.startOverflowFlows(elem->m_object,bmask & OAPC_DIGI_IO_MASK,nextDigi,0.0,_T(""),oapcBinHeadSp(),wxGetLocalTimeMillis());
         }
         else ev.Skip(true);
      }
  	}
}



void WatchWin::OnCellLeftDClick( wxGridEvent& ev )
{
   if (ev.GetRow()==m_nextNewRowNum) createNewCol(-1,-1);
   else if ((m_nextNewRowNum>0) && (ev.GetCol()==-1))
   {
   	wxNode *node;
   	
   	grid->DeleteRows(ev.GetRow());
   	m_nextNewRowNum--;
   	node=m_watchList.Item(ev.GetRow());
   	wxASSERT(node);
   	if (node)
   	{
      	m_watchList.DeleteNode(node);
//      	delete (WatchListElement*)node->GetData();
   	}
   }
}



void WatchWin::OnCellValueChanged( wxGridEvent& ev )
{
   wxNode           *node;
   WatchListElement *elem;
   wxUint32          bmask=0x00010100; // binary data can't be changed, digi data are handled above
   wxString          nextChar;
   wxFloat64         nextNum;
   	
  	node=m_watchList.Item(ev.GetRow());
  	wxASSERT(node);
  	if (node)
  	{
      elem=(WatchListElement*)node->GetData();
      wxASSERT(elem->m_object);
      if (elem->m_object)
      {
         bmask=bmask<<ev.GetCol();
         if (bmask & elem->m_object->data.stdOUT & OAPC_NUM_IO_MASK)
         {
            double d;
            grid->GetCellValue(ev.GetRow(),ev.GetCol()).ToDouble(&d);
            nextNum=d;
            g_flowPool.startOverflowFlows(elem->m_object,bmask & OAPC_NUM_IO_MASK,0,nextNum,_T(""),oapcBinHeadSp(),wxGetLocalTimeMillis());
         }
         else if (bmask & elem->m_object->data.stdOUT & OAPC_CHAR_IO_MASK)
         {
            nextChar=grid->GetCellValue(ev.GetRow(),ev.GetCol());
            g_flowPool.startOverflowFlows(elem->m_object,bmask & OAPC_CHAR_IO_MASK,0,0.0,nextChar,oapcBinHeadSp(),wxGetLocalTimeMillis());
         }
      }
  	}
}



void WatchWin::OnClose(wxCloseEvent& WXUNUSED(event))
{
   wxInt32           i,v,x,y;
   WatchListElement *elem;
   wxNode           *node;
   wxFile           *FHandle;
   wxString          useProjectName;


   if (!wxFileName::DirExists(wxStandardPaths::Get().GetUserDataDir())) wxFileName::Mkdir(wxStandardPaths::Get().GetUserDataDir());
   useProjectName=m_projectName;
   useProjectName.Replace(_T("/"),_T("_"),true);
   useProjectName.Replace(_T("\\"),_T("_"),true);
   FHandle=new wxFile(wxStandardPaths::Get().GetUserDataDir()+wxFileName::GetPathSeparator()+m_projectName+_T(".dbg"),wxFile::write);
   wxASSERT(FHandle->IsOpened());
   if (FHandle->IsOpened())
   {
      GetPosition(&x,&y);
		FHandle->Write(&x,sizeof(x)); FHandle->Write(&y,sizeof(y));
		v=GetSize().x;                FHandle->Write(&v,sizeof(v));
		v=GetSize().y;	               FHandle->Write(&v,sizeof(v));
   	for (i=0; i<8; i++)
	   {
   		v=grid->GetColSize(i);               
   		FHandle->Write(&v,sizeof(v));
	   }
	   for (i=0; i<m_nextNewRowNum; i++)
	   {
		   node=m_watchList.Item(i);
   		wxASSERT(node);
	   	if (node)
		   {
			   elem=(WatchListElement*)node->GetData();
       		FHandle->Write(&elem->m_id,sizeof(elem->m_id));
       		v=grid->GetRowSize(i);
       		FHandle->Write(&v,sizeof(v));
		   }
		}
		FHandle->Close();
	}
	delete FHandle;
   g_watchWin=NULL;	
	
   Destroy();
}



