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

#include "DBWatchWin.h"
#include "DlgControlSelector.h"
#include "liboapc.h"
#include "globals.h"

// ----------------------------------------------------------------------------
// GridFrame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE( DBWatchWin, wxFrame )
    EVT_CLOSE(DBWatchWin::OnClose)
END_EVENT_TABLE()


static void db_recv_callback(void*,char *nodeName,unsigned int,unsigned int ios,void *values[MAX_NUM_IOS])
{
   values=values;
   if (g_DBwatchWin) g_DBwatchWin->setNodeInfo(nodeName,ios);
}



DBWatchWin::DBWatchWin(wxString projectName):wxFrame(NULL, wxID_ANY, wxEmptyString,wxPoint(g_objectList.m_projectData->totalW+5,470),wxSize(500,200))
{
  	wxString  sTmp;
	wxFile *FHandle;

   m_projectName=projectName;
   SetTitle(_("Watch Interlock Database"));
   SetIcon(wxIcon(icon_xpm));   

   m_list=new wxTextCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE|wxTE_READONLY);
   m_list->SetBackgroundColour(*wxBLACK);
   m_list->SetForegroundColour(*wxGREEN);
   m_list->SetFont(wxFont(9,wxFONTFAMILY_MODERN,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL));

   wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL );
   this->SetSizer(topSizer);
   topSizer->Add(m_list,1,wxEXPAND);

   sTmp=wxStandardPaths::Get().GetUserDataDir()+wxFileName::GetPathSeparator()+m_projectName+_T(".dbdbg");

   if (wxFile::Exists(sTmp))
   {
      FHandle=new wxFile(sTmp,wxFile::read);
      if (FHandle->IsOpened())
      {
		   wxInt32 x,y;

   		FHandle->Read(&x,sizeof(x)); FHandle->Read(&y,sizeof(y));
	   	if ((x>=0) && (y>=0)) Move(x,y);
		   FHandle->Read(&x,sizeof(x)); FHandle->Read(&y,sizeof(y));
   		if ((x>10) && (y>10)) SetSize(x,y);
         FHandle->Close();
         delete FHandle;
      }
   }
   m_ispaceHandle=oapc_ispace_get_instance();
   if (m_ispaceHandle)
   {
      if ((oapc_ispace_set_recv_callback(m_ispaceHandle,&db_recv_callback)!=OAPC_OK) ||
          (oapc_ispace_connect(m_ispaceHandle,NULL,0,NULL)!=OAPC_OK))
   	{
     		wxMessageBox(_("Could not establish connection to Interlock Server!"),_("Error"),wxOK|wxICON_ERROR);
        	m_ispaceHandle=NULL;
     	}
   }   	
   else
   {
      wxMessageBox(_("Not enough resources to communicate with Interlock Server!"),_("Error"),wxOK|wxICON_ERROR);
  		m_ispaceHandle=NULL;
   }  	
}



DBWatchWin::~DBWatchWin()
{
   oapc_ispace_disconnect(m_ispaceHandle);   
}



void DBWatchWin::setNodeInfo(char *nodeName,wxUint32 ios)
{
   wchar_t         wc[MAX_NODENAME_LENGTH+4];
   wxMBConvUTF8    conv;
   wxUint32        bitmask=0x01010101;
   wxInt32         i;
   wxString        ioStr,nodeStr,logStr;
   static wxUint32 maxStrLen=0;

   conv.MB2WC(wc,nodeName,MAX_NODENAME_LENGTH);
   nodeStr=wxString(wc);
   if (nodeStr.Length()>maxStrLen) maxStrLen=nodeStr.Length();
   else while (nodeStr.Length()<maxStrLen) nodeStr=nodeStr+_T(" ");
   for (i=0; i<MAX_NUM_IOS; i++)
   {
#if wxCHECK_VERSION(2,9,0)
      if      (ios & bitmask & OAPC_DIGI_IO_MASK) ioStr=wxString::Format(_T("%s D%d"),ioStr,i);
      else if (ios & bitmask & OAPC_NUM_IO_MASK)  ioStr=wxString::Format(_T("%s N%d"),ioStr,i);
      else if (ios & bitmask & OAPC_CHAR_IO_MASK) ioStr=wxString::Format(_T("%s C%d"),ioStr,i);
      else if (ios & bitmask & OAPC_BIN_IO_MASK)  ioStr=wxString::Format(_T("%s B%d"),ioStr,i);
      else ioStr=wxString::Format(_T("%s   "),ioStr);
#else
      if (ios & bitmask & OAPC_DIGI_IO_MASK) ioStr=wxString::Format(_T("%s D%d"),ioStr.c_str(),i);
      else if (ios & bitmask & OAPC_NUM_IO_MASK)  ioStr=wxString::Format(_T("%s N%d"),ioStr.c_str(),i);
      else if (ios & bitmask & OAPC_CHAR_IO_MASK) ioStr=wxString::Format(_T("%s C%d"),ioStr.c_str(),i);
      else if (ios & bitmask & OAPC_BIN_IO_MASK)  ioStr=wxString::Format(_T("%s B%d"),ioStr.c_str(),i);
      else ioStr=wxString::Format(_T("%s   "),ioStr.c_str());
#endif
      bitmask=bitmask<<1;
   }
#ifndef ENV_WINDOWS
   wxMutexGuiEnter();
#endif
   logStr=m_list->GetValue();
   if (logStr.Length()>5000)
   {
      wxInt32 pos;

      logStr=logStr.Mid(0,logStr.Length()-1);
      pos=logStr.Find('\n',true);
      logStr=logStr.Mid(0,pos);
   }
   m_list->Freeze();
   m_list->ChangeValue(nodeStr+_T(" ")+ioStr+_T("\n")+logStr);
   m_list->Thaw();
#ifndef ENV_WINDOWS
   wxMutexGuiLeave();
#endif
}



void DBWatchWin::OnClose(wxCloseEvent& WXUNUSED(event))
{
	wxFile         *FHandle;

   if (!wxFileName::DirExists(wxStandardPaths::Get().GetUserDataDir()))
	 wxFileName::Mkdir(wxStandardPaths::Get().GetUserDataDir());
	FHandle=new wxFile(wxStandardPaths::Get().GetUserDataDir()+wxFileName::GetPathSeparator()+m_projectName+_T(".dbdbg"),wxFile::write);
	wxASSERT(FHandle->IsOpened());
	if (FHandle->IsOpened())
	{
      wxInt32         x,y;
	
      GetPosition(&x,&y);
		FHandle->Write(&x,sizeof(x)); FHandle->Write(&y,sizeof(y));
      GetSize(&x,&y);
		FHandle->Write(&x,sizeof(x)); FHandle->Write(&y,sizeof(y));
		FHandle->Close();
	}
	delete FHandle;
   g_DBwatchWin=NULL;
   Destroy();
}



