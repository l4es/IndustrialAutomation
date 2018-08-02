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
#include "../DebugWin.cpp"
#else //ENV_INT

#include <wx/stockitem.h>
#include <wx/artprov.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "DebugWin.h"
#include "WatchWin.h"
#include "DBWatchWin.h"
#include "globals.h"
#include "PlayerMainApp.h"
#include "ObjectList.h"
#include "HMICanvas.h"
#include "DlgDebugParams.h"
#include "flowTimer.h"
#include "globals.h"
#include "oapcMutexLocker.h"

DECLARE_APP( PlayerMainApp)

#define HMI_STOP_ON_WARNING 0x1001
#define HMI_START_DEBUG     0x1002
#define HMI_ID_TIMER        0x1003
#define HMI_ID_CREDITS      0x1004
#define HMI_ID_STEP_DEBUG   0x1005
#define HMI_ID_WATCH        0x1006
#define HMI_ID_DBWATCH      0x1007

BEGIN_EVENT_TABLE(DebugWin, wxFrame)
EVT_MENU(wxID_OPEN, DebugWin::OnLoadProject)
EVT_MENU(wxID_ABOUT, DebugWin::OnMenu)
EVT_MENU(HMI_ID_CREDITS,     DebugWin::OnCredits)
EVT_MENU(wxID_EXIT,          DebugWin::OnExit)
EVT_MENU(wxID_PREFERENCES, DebugWin::OnToolbar)
EVT_MENU(HMI_START_DEBUG, DebugWin::OnToolbar)
EVT_MENU(HMI_ID_STEP_DEBUG, DebugWin::OnToolbar)
EVT_MENU(HMI_ID_WATCH, DebugWin::OnToolbar)
EVT_MENU(HMI_ID_DBWATCH, DebugWin::OnToolbar)
EVT_CHECKBOX(wxID_ANY, DebugWin::OnToolbar)
EVT_TIMER(HMI_ID_TIMER, DebugWin::OnTimer)
EVT_CLOSE( DebugWin::OnClose)
END_EVENT_TABLE()

const wxInt32 DebugWin::RUN_STATE_STOPPED=1;
const wxInt32 DebugWin::RUN_STATE_RUNNING = 2;
const wxInt32 DebugWin::RUN_STATE_STEP = 3;

DebugWin::DebugWin(wxWindow *parent, const wxChar *title, int xpos, int ypos,int width, int height, HMICanvas *hmiCanvas) 
         :wxFrame(parent, wxID_ANY, title, wxPoint(xpos, ypos), wxSize(width, height))
{
   SetTitle(HMI_TITLE);
   SetIcon(wxIcon(icon_xpm));

   this->parent = parent;
   this->hmiCanvas = hmiCanvas;

   mainLog
         = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition,
               wxDefaultSize,
               wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxHSCROLL);

   wxMenuBar *menuBar = new wxMenuBar;
   wxMenu *menuFile = new wxMenu;

   menuFile->Append(wxID_OPEN, _("Load project") + _T("\tCtrl-L"));
   menuFile->AppendSeparator();
   menuFile->Append(wxID_EXIT, _("Quit"),
         _("Quits the application"));

   menuBar->Append(menuFile, _("File"));

   wxMenu *menuHelp = new wxMenu;
   menuHelp->Append(wxID_ABOUT, _("About"),_("Displays the program information"));
   menuHelp->Append(HMI_ID_CREDITS, _("Credits"));

   menuBar->Append(menuHelp, _("Help"));

   SetMenuBar(menuBar);

   mainTB = CreateToolBar();
#if wxCHECK_VERSION(3,0,0)
   mainTB->AddTool(wxID_OPEN, _("Load project"), wxArtProvider::GetBitmap(wxART_FILE_OPEN));
   mainTB->AddTool(wxID_EXIT, _("Quit application"), wxArtProvider::GetBitmap(wxART_QUIT));
#else
   mainTB->AddTool(wxID_OPEN, wxArtProvider::GetBitmap(wxART_FILE_OPEN),         _("Load project"), _("Load project"));
   mainTB->AddTool(wxID_EXIT, wxArtProvider::GetBitmap(wxART_QUIT),         _("Quit application"), _("Quit application"));
#endif
   mainTB->AddSeparator();

   wxBitmap bugbm(_T(DATA_PATH"icons/bug.png"),wxBITMAP_TYPE_PNG);
   if (!bugbm.Ok())
      bugbm = wxArtProvider::GetBitmap(wxART_ERROR);
   runTool = mainTB->AddCheckTool(HMI_START_DEBUG, _T(""), bugbm, wxNullBitmap,
         _("Start/stop program flow"), _("Start/stop program flow"));

   wxBitmap inspectbm(_T(DATA_PATH"icons/inspect.png"),wxBITMAP_TYPE_PNG);
   if (!inspectbm.Ok())
      inspectbm = wxArtProvider::GetBitmap(wxART_ERROR);
#if wxCHECK_VERSION(3,0,0)
   mainTB->AddTool(HMI_ID_WATCH, _("Open watch window"), inspectbm);
#else
   mainTB->AddTool(HMI_ID_WATCH, inspectbm, _("Open watch window"));
#endif

   wxBitmap dbbm(_T(DATA_PATH"icons/database.png"),wxBITMAP_TYPE_PNG);
   if (!dbbm.Ok())
      dbbm = wxArtProvider::GetBitmap(wxART_ERROR);
#if wxCHECK_VERSION(3,0,0)
   /*   wxToolBarToolBase* tool =*/ mainTB->AddTool(HMI_ID_DBWATCH, _("Open interlock database watch window"), dbbm);
#else
/*   wxToolBarToolBase* tool =*/ mainTB->AddTool(HMI_ID_DBWATCH, dbbm,_("Open interlock database watch window"));
#endif
//   if ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK) == 0) tool->Enable(0);

   wxBitmap stepbm(_T(DATA_PATH"icons/step.png"),wxBITMAP_TYPE_PNG);
   if (!stepbm.Ok())
      stepbm = wxArtProvider::GetBitmap(wxART_ERROR);
#if wxCHECK_VERSION(3,0,0)
   mainTB->AddTool(HMI_ID_STEP_DEBUG, _("Single Step"), stepbm);
#else
   mainTB->AddTool(HMI_ID_STEP_DEBUG, stepbm, _("Single Step"),_("Single Step"));
#endif

   wxBitmap configbm(_T(DATA_PATH"icons/config.png"),wxBITMAP_TYPE_PNG);
   if (!configbm.Ok())
      configbm = wxArtProvider::GetBitmap(wxART_ERROR);
#if wxCHECK_VERSION(3,0,0)
   mainTB->AddTool(wxID_PREFERENCES, _("Configure debug parameters"), configbm);
#else
   mainTB->AddTool(wxID_PREFERENCES, configbm, _("Configure debug parameters"),_("Configure debug parameters"));
#endif

   stopWarnCB = new wxCheckBox(mainTB, wxID_ANY, _("Stop on warning"));
   stopWarnCB->SetValue(1);
   mainTB->AddControl(stopWarnCB);

   displayNamesCB
         = new wxCheckBox(mainTB, wxID_ANY, _("Display control names"));
   displayNamesCB->SetValue(0);
   mainTB->AddControl(displayNamesCB);

   mainTB->Realize();

   mainSB = CreateStatusBar();
   mainSB->SetFieldsCount(2);
   stateTimer = new wxTimer(this, HMI_ID_TIMER);
   stateTimer->Start(200);
   flags = 0xFFFFFFFF;
   setRunState(DebugWin::RUN_STATE_STOPPED);
   Show(true);
}

DebugWin::~DebugWin()
{
   oapcMutexLocker lock(g_watchWinLock, __FILE__, __LINE__);
   if (g_watchWin)
   {
      g_watchWin->Close(true);
      delete g_watchWin;
   }
   if (g_DBwatchWin)
   {
      g_DBwatchWin->Close(true);
      delete g_DBwatchWin;
   }
   delete stateTimer;
}



void DebugWin::appendLogText(wxString text)
{
   mainLog->AppendText(text);
}

void DebugWin::setRunState(wxInt32 runState)
{
   wxDateTime now = wxDateTime::Now();
   wxString stamp = now.Format(_T("%X: "));

   if (runState == DebugWin::RUN_STATE_STOPPED)
   {
      g_threadsDisabled = 1;
      if (this->runState != runState)
      {
         appendLogText(stamp + _("...stopped!") + _T("\n"));
         if (runTool)
            mainTB->ToggleTool(runTool->GetId(), false);
      }
   }
   else if (runState == DebugWin::RUN_STATE_RUNNING)
   {
      appendLogText(stamp + _("Running...") + _T("\n"));
      g_threadsDisabled = 0;
   }
   else if (runState == DebugWin::RUN_STATE_STEP)
   {
      g_debugStepMode = DEBUG_STEP_DO_STEP;
      appendLogText(stamp + _("Performing single step") + _T("\n"));
   }
   if (runState != DebugWin::RUN_STATE_STEP)
      this->runState = runState;
   updateThreadNum();
}

void DebugWin::updateThreadNum()
{
   wxString txt;
   if (g_flowDispatcher)
   {
      txt = txt.Format(_("Active Flows") + _T(": %d"),g_flowDispatcher->getThreadNum());
      mainSB->SetStatusText(txt, 1);
   }
}

void DebugWin::setDebugInformation(flowObject *object, wxUint32 debugFlags,wxString addData)
{
   wxString txt;

   if (g_breakThreads) return;
   if (object) txt = _("Flow stopped at") + _T(" ") + object->name + _T(": ");
   else txt = _("Flow stopped at unknown position") + _T(": ");
   switch (debugFlags)
   {
      case DEBUG_STOP_COND_LIB_FAILED:
         txt << _("Plug-in call failed");
         break;
      case DEBUG_STOP_COND_LIB_RETURNED_ERROR:
          txt << _("Error message returned from plug-in") << _T(": ") << addData;
          break;
      case DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT:
         txt << _("Plug-in doesn't provides required input function");
         break;
      case DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_OUTPUT:
         txt << _("Plug-in doesn't provides required output function");
         break;
      case DEBUG_STOP_COND_THREAD_ID:
         txt << _("Flow looped to itself");
         break;
      case DEBUG_STOP_COND_ILLEGAL_IO:
         txt << _("Unknown IO defined");
         break;
   case DEBUG_STOP_COND_RUN_TIMEOUT:
      txt << _("Flow run timeout exceeded");
      break;
   case DEBUG_STOP_COND_INCOMPATIBLE_DATATYPE:
      txt << _("Incompatible binary datatype");
      break;
   case DEBUG_STOP_COND_NO_MEMORY:
      txt << _("Out of memory");
      break;

   case DEBUG_INFO_FOCUS_LOST:
      txt = _("Focus lost/value changed event at") + _T(" ") + object->name;
      break;
   case DEBUG_INFO_DATA_FROM_IS_REJECTED:
      txt = _("Data from Interlock Server rejected at") + _T(" ")
            + object->name;
      break;

   case DEBUG_ERROR_CONVERSION_ERROR:
      txt = _T("\"") + addData + _T("\" ") + _("Data conversion error at")
            + _T(" ") + object->name;
      break;
   case DEBUG_ERROR_EXECUTION_LIMIT:
      txt = _("Maximum flow element execution time exceeded at") + _T(" ")
            + object->name;
      break;
   default:
      break;
   }
   wxDateTime now = wxDateTime::Now();
   wxString stamp = now.Format(_T("%X: "));

   appendLogText(stamp + txt + _T("\n"));
   if ((flags & debugFlags & DEBUG_STOP_COND)==DEBUG_STOP_COND)
   {
      if (stopWarnCB->GetValue()) setRunState(DebugWin::RUN_STATE_STOPPED);
   }
}

void DebugWin::OnTimer(wxTimerEvent& WXUNUSED(event))
{
   updateThreadNum();
}

void DebugWin::OnToolbar(wxCommandEvent& event)
{
   if (event.GetId() == HMI_START_DEBUG)
   {
      if (runTool->IsToggled())
         setRunState(DebugWin::RUN_STATE_RUNNING);
      else
         setRunState(DebugWin::RUN_STATE_STOPPED);
   }
   else if (event.GetId() == wxID_PREFERENCES)
   {
      DlgDebugParams dlg(flags, this, _("Debugging Settings"));
      dlg.ShowModal();
      flags = dlg.flags;
   }
   else if (event.GetId() == HMI_ID_STEP_DEBUG)
   {
      wxNode *node;
      flowTimer *timer;

      setRunState(DebugWin::RUN_STATE_STEP);
      node = g_objectList.getObject((wxNode*) NULL);
      while (node)
      {
         if (((flowObject*) node->GetData())->data.type
               == flowObject::FLOW_TYPE_FLOW_TIMER)
         {
            timer = (flowTimer*) node->GetData();
            timer->m_allowOneTimer = true;
         }
         node = g_objectList.getObject(node);
      }
      wxMilliSleep(25); // needs to be much bigger than 10 msec wait time of delayed threads
      g_debugStepMode = 0;

   }
   else if (event.GetId() == displayNamesCB->GetId())
   {
      hmiCanvas->displayControlNames(displayNamesCB->GetValue());
   }
   else if (event.GetId() == HMI_ID_WATCH)
   {
      if (!g_watchWin)
      {
         g_watchWin = new WatchWin(g_projectFile);
         g_watchWin->Show();
      }
      g_watchWin->Raise();
   }
   else if (event.GetId() == HMI_ID_DBWATCH)
   {
      if (!g_DBwatchWin)
      {
         g_DBwatchWin = new DBWatchWin(g_projectFile);
         g_DBwatchWin->Show();
      }
      g_DBwatchWin->Raise();
   }
}

void DebugWin::OnClose(wxCloseEvent& WXUNUSED(event))
{
   wxDateTime now = wxDateTime::Now();
   wxString stamp = now.Format(_T("%X: "));

   appendLogText(stamp + _("Exiting application...") + _T("\n"));

   g_flowPool.exitApplication();
   g_mainWin->Destroy();
   wxWindow::Destroy();
#ifndef _DEBUG
   exit(0); // TODO: remove this exit() for clean shutdown!!!
#endif
}

void DebugWin::OnExit(wxCommandEvent& WXUNUSED(event))
{
   wxCloseEvent evt;

   OnClose(evt);
}



void DebugWin::OnMenu(wxCommandEvent& event)
{
   if (event.GetId()==wxID_ABOUT) g_About();
}



void DebugWin::OnCredits(wxCommandEvent& WXUNUSED(event))
{
   g_Credits();
}

void DebugWin::OnLoadProject(wxCommandEvent& WXUNUSED(event))
{
   oapcMutexLocker lock(g_watchWinLock, __FILE__, __LINE__);
   g_fileLoadWarned = false;
   if (g_watchWin)
      g_watchWin->Close(true);
   hmiCanvas->loadProject(&g_projectDir, &g_projectFile);
}

#endif //ENV_INT