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
#include "../PlayerMainApp.cpp"
#else //ENV_INT

#include <wx/wx.h>
#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/dialog.h>
//#include <wx/splash.h>
#include <wx/filename.h>

#ifdef __WXMSW__
//#include "vld.h"
#endif

#include <wx/socket.h>

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif
#include "globals.h"
#include "PlayerMainApp.h"
#include "HMICanvas.h"
#include "globals.h"
#include "oapcResourceBundle.h"


static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
#if wxCHECK_VERSION(2,9,0)
 #ifndef ENV_DEBUGGER 
   { wxCMD_LINE_OPTION,"x",NULL,"window x position",    wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},
   { wxCMD_LINE_OPTION,"y",NULL,"window y position",    wxCMD_LINE_VAL_NUMBER,wxCMD_LINE_PARAM_OPTIONAL},
   { wxCMD_LINE_SWITCH,"b",NULL,"show window title bar",wxCMD_LINE_VAL_NONE,  wxCMD_LINE_PARAM_OPTIONAL},
 #endif // ENV_DEBUGGER 
   { wxCMD_LINE_PARAM, NULL, NULL,"project file",     wxCMD_LINE_VAL_STRING,wxCMD_LINE_OPTION_MANDATORY},
#else
 #ifndef ENV_DEBUGGER 
   { wxCMD_LINE_OPTION,_T("x"),NULL,_T("window x position"),wxCMD_LINE_VAL_NUMBER,  wxCMD_LINE_PARAM_OPTIONAL},
   { wxCMD_LINE_OPTION,_T("y"),NULL,_T("window y position"),wxCMD_LINE_VAL_NUMBER,  wxCMD_LINE_PARAM_OPTIONAL},
   { wxCMD_LINE_SWITCH,_T("b"),NULL,_T("show window title bar"),wxCMD_LINE_VAL_NONE,wxCMD_LINE_PARAM_OPTIONAL},
 #endif // ENV_DEBUGGER 
   { wxCMD_LINE_PARAM, NULL,   NULL,_T("project file"),wxCMD_LINE_VAL_STRING,     wxCMD_LINE_OPTION_MANDATORY},
#endif
   { wxCMD_LINE_NONE }
};



IMPLEMENT_APP(PlayerMainApp)


bool PlayerMainApp::OnInit()
{
//   wxSplashScreen *splash=NULL;
//   wxBitmap        bitmap;

   SDBG
#ifndef ENV_DEBUGGER
   m_posX=0;
   m_posY=0;
   m_titleBar=false;
#endif
   if (!wxApp::OnInit()) return false;
   g_imagePaintMutex=oapc_thread_mutex_create();
   if (!g_imagePaintMutex) return false;

#ifndef ENV_DEBUGGER
   wxLog::EnableLogging(false);
#else
   wxLog::EnableLogging(true);
#endif
   wxInitAllImageHandlers();
   wxSocketBase::Initialize();
   g_res=new oapcResourceBundle(_T(DATA_PATH"translations/openplayer"));
#ifdef ENV_DEBUGGER
   g_res->appendResource(_T(DATA_PATH"translations/openapc"));
#endif
   g_res->appendResource(_T(DATA_PATH"translations/common"));
   g_res->appendResource(_T(DATA_PATH"translations/custom"));
   g_projectPath=file;
   wxInt32 pos=file.Find(wxFileName::GetPathSeparator(),true);
   if (pos==wxNOT_FOUND) g_projectFile=file;
   else
   {
      g_projectFile=file.Mid(pos+1);
      g_projectDir=file.Mid(0,pos);
   }
	//   if (bitmap.LoadFile(_T("icons/splash.png"), wxBITMAP_TYPE_PNG))
	//    splash = new wxSplashScreen(bitmap,wxSPLASH_CENTRE_ON_SCREEN/*|wxSPLASH_TIMEOUT*/,6000,NULL, -1, wxDefaultPosition, wxDefaultSize,wxSIMPLE_BORDER|wxSTAY_ON_TOP);

   wxString err=g_objectList.loadProject(file,false,true);
   if (err.length()>0)
   {
      wxMessageBox(_("Project file could not be loaded, exiting!"),_("Error"),wxOK|wxICON_ERROR,NULL);
      exitApp();
      return false;
   }
#ifndef ENV_DEBUGGER
   if (!m_titleBar) g_mainWin=new wxFrame(NULL,wxID_ANY,wxEmptyString,wxPoint(m_posX,m_posY),wxSize(g_objectList.m_projectData->totalW,g_objectList.m_projectData->totalH),0);
   else g_mainWin=new wxFrame(NULL,wxID_ANY,wxEmptyString,wxPoint(m_posX,m_posY),wxSize(g_objectList.m_projectData->totalW+6,g_objectList.m_projectData->totalH+24),wxCAPTION);
#else
   g_mainWin=new wxFrame(NULL,wxID_ANY,wxEmptyString,wxPoint(0,0),wxSize(g_objectList.m_projectData->totalW+6,g_objectList.m_projectData->totalH+24),wxCAPTION);
#endif
   g_mainWin->SetIcon(icon_xpm);
   g_hmiCanvas=new HMICanvas(g_mainWin,0);
   if (!g_hmiCanvas->loadProject(file))
   {
       exitApp();
       return FALSE;
   }

   SetTopWindow(g_mainWin);
   g_mainWin->Show(TRUE);
#ifdef ENV_DEBUGGER   
   g_debugWin=new DebugWin(g_mainWin,_T("OpenDebugger"),wxSystemSettings::GetMetric(wxSYS_SCREEN_X)-482,20,480,450,g_hmiCanvas);
#endif
//   if (splash) splash->Destroy();
   g_mainWin->SetSize(g_objectList.m_projectData->totalW,g_objectList.m_projectData->totalH);
   g_mainWin->Raise();
   g_flowDispatcher=new FlowDispatcherThread();
   g_flowDispatcher->Create();
   g_flowDispatcher->Run();
   g_flowPool.startInitialFlows(&g_objectList);
   g_flowPool.startTimerFlows(&g_objectList);
   return TRUE;
}



void PlayerMainApp::exitApp()
{
//   delete g_hmiCanvas;
   g_objectList.deleteAll(0);
   if (g_externalIOLibs) delete g_externalIOLibs;
   if (g_externalHMILibs) delete g_externalHMILibs;

   delete g_res;
   if (g_imagePaintMutex) oapc_thread_mutex_release(g_imagePaintMutex);
}



int PlayerMainApp::OnExit()
{
   SDBG
   exitApp();
#ifdef ENV_WINDOWS
//   _CrtDumpMemoryLeaks();
#endif
   return 0;
}



void PlayerMainApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    SDBG
    parser.SetDesc (g_cmdLineDesc);
    // must refuse '/' as parameter starter or cannot use "/path" style paths
    parser.SetSwitchChars (wxT("-"));
}




bool PlayerMainApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
   SDBG
   for (wxUint32 i = 0; i < parser.GetParamCount(); i++)
   {
      file=parser.GetParam(i);
   }

#ifndef ENV_DEBUGGER
   parser.Found(wxT("x"),&m_posX);
   parser.Found(wxT("y"),&m_posY);
   m_titleBar=parser.Found(wxT("b"));
#endif
   return true;
}

#endif //ENV_INT