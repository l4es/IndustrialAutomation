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
#include "../HPlayerMainApp.cpp"
#else //ENV_INT


#include <wx/wx.h>
#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/dialog.h>
#include <wx/filename.h>
#include <wx/socket.h>

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif
#include "globals.h"
#include "HPlayerMainApp.h"
#include "globals.h"


static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
#if wxCHECK_VERSION(2,9,0)
     { wxCMD_LINE_PARAM,  NULL, NULL,"project file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY},
#else
     { wxCMD_LINE_PARAM,  NULL, NULL,_T("project file"),wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY},
#endif
     { wxCMD_LINE_NONE }
};



IMPLEMENT_APP(PlayerMainApp)



bool PlayerMainApp::OnInit()
{
   wxString error;

   //TODO: call debug handler like in PlayerMainApp
   if (!wxAppConsole::OnInit()) return false;
   wxInitAllImageHandlers();
   wxSocketBase::Initialize();
   g_projectPath=file;
   wxInt32 pos=file.Find(wxFileName::GetPathSeparator(),true);
   if (pos==wxNOT_FOUND) g_projectFile=file;
   else
   {
      g_projectFile=file.Mid(pos+1);
      g_projectDir=file.Mid(0,pos);
   }

   if (!g_externalIOLibs) g_externalIOLibs=new ExternalIOLibs();
   error=g_objectList.loadProject(file,true,false);
   if (error.Length()!=0) return false;
   return true;
}



int PlayerMainApp::OnRun()
{
   g_flowPool.startTimer();
   g_flowDispatcher=new FlowDispatcherThread();
   g_flowDispatcher->Create();
   g_flowPool.startInitialFlows(&g_objectList);
   g_flowPool.startTimerFlows(&g_objectList);
   g_flowDispatcher->Entry();
   return 0;
}



void PlayerMainApp::exitApp()
{
//   delete g_hmiCanvas;
   g_objectList.deleteAll(0);
   delete g_externalIOLibs;
}



int PlayerMainApp::OnExit()
{
   exitApp();
#ifdef ENV_WINDOWS
//   _CrtDumpMemoryLeaks();
#endif
   return 0;
}



void PlayerMainApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetDesc (g_cmdLineDesc);
    // must refuse '/' as parameter starter or cannot use "/path" style paths
    parser.SetSwitchChars (wxT("-"));
}




bool PlayerMainApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    for (wxUint32 i = 0; i < parser.GetParamCount(); i++)
    {
        file=parser.GetParam();
    }
    return true;
}

#endif //ENV_INT
