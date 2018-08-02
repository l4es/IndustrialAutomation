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
#include "../MainApp.cpp"
#else //ENV_INT

#include <wx/wx.h>
#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/splash.h>

#ifdef __WXMSW__
//#include "vld.h"
#endif

#include "MainApp.h"
#include "MainWin.h"
#include "globals.h"


static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
#if wxCHECK_VERSION(2,9,0)
   { wxCMD_LINE_PARAM,  NULL, NULL,"project file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE|wxCMD_LINE_PARAM_OPTIONAL },
#else
   { wxCMD_LINE_PARAM,  NULL, NULL,_T("project file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE|wxCMD_LINE_PARAM_OPTIONAL },
#endif
   { wxCMD_LINE_NONE }
};


IMPLEMENT_APP(MainApp)

static bool isInitialized=false;

bool MainApp::OnInit()
{
   if (!isInitialized)
   {
      isInitialized=true;
      if (!wxApp::OnInit()) return false;
      wxLog::EnableLogging(false);
      wxInitAllImageHandlers();
#ifndef NOSPLASH // for *nix makefiles only
#ifndef _DEBUG
      wxSplashScreen* splash=NULL;

      wxBitmap bitmap;
      if (bitmap.LoadFile(_T(DATA_PATH"icons/splash.png"), wxBITMAP_TYPE_PNG))
      splash = new wxSplashScreen(bitmap,wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,6000,NULL, -1, wxDefaultPosition, wxDefaultSize,wxSIMPLE_BORDER|wxSTAY_ON_TOP);
#endif
#endif
      g_res=new oapcResourceBundle(_T(DATA_PATH"translations/openplayer"));
      g_res->appendResource(_T(DATA_PATH"translations/openapc"));
      g_res->appendResource(_T(DATA_PATH"translations/common"));
      g_res->appendResource(_T(DATA_PATH"translations/custom"));

      g_mainWin = new MainWin(_T("Basic"), 50, 50, 800,600);
      SetTopWindow(g_mainWin);
      g_mainWin->Show(TRUE);
      g_mainWin->Raise();
      g_mainWin->setup();
      if (file.Length()>0)
      {
         g_mainWin->loadProject(file);
//         g_isSaved=false; // mark as unsaved for compiling process, elsewhere we do not have a valid pathname
      }
#ifndef NOSPLASH // for *nix makefiles only
#ifndef _DEBUG
      if (splash) splash->Destroy();
#endif
#endif
   }
   else return FALSE;
   return TRUE;
}



int MainApp::OnExit()
{
//   g_objectList.deleteAll(1);
   delete g_externalIOLibs;
   delete g_externalHMILibs;
   delete g_res;
   return 0;
}



int MainApp::FilterEvent(wxEvent& event)
{
    if ((event.GetEventType()==wxEVT_KEY_DOWN) && (g_mainWin))
    {
        return g_mainWin->OnKeyDown( (wxKeyEvent&)event );
    }
    else if ((event.GetEventType()==wxEVT_KEY_UP) && (g_mainWin))
    {
        g_mainWin->OnKeyUp( (wxKeyEvent&)event );
        return -1;
    }

    return -1;
}



void MainApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetDesc (g_cmdLineDesc);
    // must refuse '/' as parameter starter or cannot use "/path" style paths
    parser.SetSwitchChars (wxT("-"));
}




bool MainApp::OnCmdLineParsed(wxCmdLineParser& parser)
{

    for (wxUint32 i = 0; i < parser.GetParamCount(); i++)
    {
        file=parser.GetParam();
    }
    return true;
}

#endif //ENV_INT