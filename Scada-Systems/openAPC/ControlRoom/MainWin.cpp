/******************************************************************************

This file is part of ControlRoom process control / HMI software.

ControlRoom is free software: you can redistribute it and / or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

ControlRoom is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
ControlRoom. If not, see http://www.gnu.org/licenses/

*******************************************************************************

For different licensing and / or usage of the sources apart from GPL or any other
open source license, please contact us at https ://openapc.com/contact.php

*******************************************************************************/

#ifdef ENV_INT
#include "../MainWin.cpp"
#else //ENV_INT

#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/stockitem.h>
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/ffile.h>

#include "wx/foldbar/foldpanelbar.h"

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "globals.h"
#include "MainWin.h"
#include "ObjectList.h"
#include "HMICanvas.h"
#include "FlowCanvas.h"
#include "DlgProjectSettings.h"
#include "DlgUserpriviSettings.h"
#include "DlgUsers.h"
#include "hmiImage.h"
#include "hmiUserMgmntPanel.h"
#include "PlugInPanel.h"
#include "PCompiler.h"
#include "flowExternalIOLib.h"
#include "UserList.h"


BEGIN_EVENT_TABLE(MainWin, wxFrame)
    EVT_MENU(wxID_NEW,                   MainWin::OnNewProject)
    EVT_MENU(wxID_OPEN,                  MainWin::OnLoadProject)
    EVT_MENU(wxID_SAVE,                  MainWin::OnSaveProject)
    EVT_MENU(wxID_SAVEAS,                MainWin::OnSaveAsProject)
    EVT_MENU(HMI_ID_COMPILEAS,           MainWin::OnMenu)
    EVT_MENU(wxID_EXIT,                  MainWin::OnExit)
    EVT_MENU(wxID_FIND,                  MainWin::OnMenu)
    EVT_MENU(wxID_ABOUT,                 MainWin::OnMenu)
    EVT_MENU(wxID_UNDO,                  MainWin::OnMenu)
    EVT_MENU(wxID_REDO,                  MainWin::OnMenu)
    EVT_MENU(HMI_ID_CREDITS,             MainWin::OnCredits)
    EVT_MENU(wxID_PREFERENCES,           MainWin::OnMenu)
    EVT_MENU(HMI_ID_USERPRIVI_SETTINGS,  MainWin::OnMenu)
    EVT_MENU(HMI_ID_USERS,               MainWin::OnMenu)
    EVT_MENU(HMI_ID_START_PLAYER,        MainWin::OnMenu)
    EVT_MENU(HMI_ID_START_DEBUGGER,      MainWin::OnMenu)
    EVT_MENU(HMI_ID_FINDNEXT,            MainWin::OnMenu)
    EVT_MENU(HMI_ID_INSPECT,             MainWin::OnMenu)
    EVT_MENU(HMI_ID_EDIT,                MainWin::OnMenu)
    EVT_MENU(wxID_DELETE,                MainWin::OnMenu)
    EVT_BOOKCTRL_PAGE_CHANGED(HMI_ID_TAB,MainWin::OnTabChanged)
END_EVENT_TABLE()

wxColourDialog        *g_colourDialog;



MainWin::MainWin( const wxChar *title, int xpos, int ypos, int width, int height)
: wxFrame( (wxFrame*) NULL, -1, title, wxPoint(xpos, ypos), wxSize(width, height) )
{
   SetTitle(HMI_TITLE);
   SetIcon(wxIcon(icon_xpm));

   m_menuBar=new wxMenuBar();
   g_mainMenuFile = new wxMenu;
   wxMenu *editFile = new wxMenu;
   wxMenuItem *item;

   g_mainMenuFile->Append(wxID_NEW,        _("New project")/*_("New project")*/);
   g_mainMenuFile->Append(wxID_OPEN,       _("Open project")/*_("Load project")*/+_T("\tCtrl-L"));
   g_mainMenuFile->Append(wxID_SAVE,       _("Save project")/*_("Save project")*/+_T("\tCtrl-S"));
   g_mainMenuFile->Append(wxID_SAVEAS,     _("Save project as")/*_("Save project as")*/+_T("...\tCtrl-A"));
   g_mainMenuFile->Append(HMI_ID_COMPILEAS,_("Compile project")+_T("..."));
   g_mainMenuFile->Append(wxID_PREFERENCES,_("Project settings")+_T("..."));
   g_mainMenuFile->Append(HMI_ID_USERPRIVI_SETTINGS,_("User Privileges")+_T("..."));
   item=new wxMenuItem(g_mainMenuFile,HMI_ID_USERS ,_("Users")+_T("..."));
   g_mainMenuFile->Append(item);
   item->Enable(false);
   g_mainMenuFile->AppendSeparator();
   g_mainMenuFile->Append(wxID_EXIT, _("Quit"),_("Quits the application"));

   m_menuBar->Append(g_mainMenuFile,_("File"));

   editFile->Append(wxID_FIND,               _("&Find element")+_T("\tCtrl-F"));
   editFile->Append(HMI_ID_FINDNEXT,         _("Find &next element")+_T("\tCtrl-N"));

   m_undoMenu=new wxMenuItem(editFile,wxID_UNDO,_("Undo")+_T("\tCtrl-Z"),wxEmptyString,wxITEM_NORMAL);
   editFile->Append(m_undoMenu);
   m_redoMenu=new wxMenuItem(editFile,wxID_REDO,_("Redo")+_T("\tCtrl-Y"),wxEmptyString,wxITEM_NORMAL);
   editFile->Append(m_redoMenu);

   m_menuBar->Append(editFile,_("Edit"));

   wxMenu *menuHelp = new wxMenu;
   menuHelp->Append(wxID_ABOUT,_("About"),_("Displays the program information"));
   menuHelp->Append(HMI_ID_CREDITS,_("Credits"));

   m_menuBar->Append(menuHelp,_("Help"));

   SetMenuBar(m_menuBar);

   EnableMenuItem(wxID_UNDO,false);
   EnableMenuItem(wxID_REDO,false);

   m_mainTB=CreateToolBar();
#if wxCHECK_VERSION(3,0,0)
   m_mainTB->AddTool(wxID_NEW,   _("New project"),   wxArtProvider::GetBitmap(wxART_NEW));
   m_mainTB->AddTool(wxID_OPEN,  _("Load project"),  wxArtProvider::GetBitmap(wxART_FILE_OPEN));
   m_mainTB->AddTool(wxID_SAVE,  _("Save project"),  wxArtProvider::GetBitmap(wxART_FILE_SAVE));
   m_mainTB->AddTool(wxID_SAVEAS,_("Save project as"),wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS));
   m_mainTB->AddTool(wxID_EXIT,  _("Quit application"),  wxArtProvider::GetBitmap(wxART_QUIT));
#else
   m_mainTB->AddTool(wxID_NEW,   wxArtProvider::GetBitmap(wxART_NEW),         _("New project"));
   m_mainTB->AddTool(wxID_OPEN,  wxArtProvider::GetBitmap(wxART_FILE_OPEN),   _("Load project"));
   m_mainTB->AddTool(wxID_SAVE,  wxArtProvider::GetBitmap(wxART_FILE_SAVE),   _("Save project"));
   m_mainTB->AddTool(wxID_SAVEAS,wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS),_("Save project as"));
   m_mainTB->AddTool(wxID_EXIT,  wxArtProvider::GetBitmap(wxART_QUIT),        _("Quit application"));
#endif
   m_mainTB->AddSeparator();

   wxBitmap editbm(_T(DATA_PATH"icons/edit.png"),wxBITMAP_TYPE_PNG);
   if (!editbm.Ok()) editbm=wxArtProvider::GetBitmap(wxART_ERROR);
#if wxCHECK_VERSION(3,0,0)
   m_mainTB->AddTool(HMI_ID_EDIT,_("Edit"),editbm);
   m_mainTB->AddTool(wxID_DELETE,      _("Delete"),wxArtProvider::GetBitmap(wxART_DELETE));
#else
   m_mainTB->AddTool(HMI_ID_EDIT,editbm,_("Edit"));
   m_mainTB->AddTool(wxID_DELETE,wxArtProvider::GetBitmap(wxART_DELETE),      _("Delete"));
#endif
   m_mainTB->AddSeparator();

   wxBitmap inspectbm(_T(DATA_PATH"icons/inspect.png"),wxBITMAP_TYPE_PNG);
   if (!inspectbm.Ok()) inspectbm=wxArtProvider::GetBitmap(wxART_ERROR);
#if wxCHECK_VERSION(3,0,0)
   m_mainTB->AddTool(HMI_ID_INSPECT,_("Inspect project for possible problems"),inspectbm);
#else
   m_mainTB->AddTool(HMI_ID_INSPECT,inspectbm,_("Inspect project for possible problems"));
#endif

   wxBitmap bugbm(_T(DATA_PATH"icons/bug.png"),wxBITMAP_TYPE_PNG);
   if (!bugbm.Ok()) bugbm=wxArtProvider::GetBitmap(wxART_ERROR);
#if wxCHECK_VERSION(3,0,0)
   m_mainTB->AddTool(HMI_ID_START_DEBUGGER,_("Start Debugger"),bugbm);
#else
   m_mainTB->AddTool(HMI_ID_START_DEBUGGER, bugbm, _("Start Debugger"));
#endif
    
   wxBitmap playbm(_T(DATA_PATH"icons/play.png"),wxBITMAP_TYPE_PNG);
   if (!playbm.Ok()) playbm=wxArtProvider::GetBitmap(wxART_ERROR);
#if wxCHECK_VERSION(3,0,0)
   m_mainTB->AddTool(HMI_ID_START_PLAYER,_("Start Player"),playbm);
#else
   m_mainTB->AddTool(HMI_ID_START_PLAYER,playbm,_("Start Player"));
#endif
   m_mainTB->Realize();

   m_book = new wxBookCtrl(this,HMI_ID_TAB,wxDefaultPosition,wxSize(width,height));
   m_book->SetBackgroundColour(*wxLIGHT_GREY);

   m_hmiSplitter=new wxSplitterWindow(m_book,wxID_ANY);
   m_book->AddPage(m_hmiSplitter, _("HMI Editor"), true);
   m_hmiScroller = new wxScrolledWindow(m_hmiSplitter, wxID_ANY);
   g_hmiCanvas=new HMICanvas(m_hmiScroller);
   g_isSaved=false;
   wxFoldPanelBar *flowBar=g_hmiCanvas->createFoldBar(m_hmiSplitter);
   m_hmiSplitter->SplitVertically(m_hmiScroller,flowBar,-220);
   m_hmiSplitter->SetSashGravity(1.0);
#ifdef ENV_LINUX
   m_hmiSplitter->SetMinimumPaneSize(190);
#else
   m_hmiSplitter->SetMinimumPaneSize(10);
#endif


   m_flowSplitter=new wxSplitterWindow(m_book,wxID_ANY);
   m_book->AddPage(m_flowSplitter,_("Flow Editor"), false);
   flowScroller = new wxScrolledWindow(m_flowSplitter,wxID_ANY);
   g_flowCanvas=new FlowCanvas(flowScroller);
   flowBar=g_flowCanvas->createFoldBar(m_flowSplitter);
   m_flowSplitter->SplitVertically(flowScroller,flowBar,-220);
   m_flowSplitter->SetSashGravity(1.0);
#ifdef ENV_LINUX
   m_flowSplitter->SetMinimumPaneSize(190);
#else
   m_flowSplitter->SetMinimumPaneSize(10);
#endif

   m_plugSplitter=new wxSplitterWindow(m_book,wxID_ANY);
   m_book->AddPage(m_plugSplitter,_("Plugged Devices"), false);
   g_plugInPanel=new PlugInPanel(m_plugSplitter);
   flowBar=g_plugInPanel->createFoldBar(m_plugSplitter);
   m_plugSplitter->SplitVertically(g_plugInPanel,flowBar,-220);
   m_plugSplitter->SetSashGravity(1.0);
#ifdef ENV_LINUX
   m_plugSplitter->SetMinimumPaneSize(190);
#else
   m_plugSplitter->SetMinimumPaneSize(10);
#endif
   m_projectFile.Clear();
   m_projectDir.Clear();

   g_colourDialog=new wxColourDialog();
   resetUndoBuffer();
}


MainWin::~MainWin()
{
   resetUndoBuffer();
   delete g_colourDialog;
}


void MainWin::resetRedoBuffer()
{
    unsigned int    i;
    wxString        path;

    EnableMenuItem(wxID_REDO,false);
    m_undoMenu->SetItemLabel(_("Redo")+_T("\tCtrl-Y"));
    if (!g_redoSteps.empty())
    {
        for (i=0; i<g_redoSteps.size(); i++)
        {
#if wxCHECK_VERSION(2,9,0)
            path << g_undoPath << i << "R";
#else
            path=wxString::Format(_T("%s%dR"),g_undoPath.c_str(),i);
#endif
            ::wxRemoveFile(path);
        }
    }
    g_redoSteps.clear();
}


void MainWin::resetUndoBuffer()
{
    unsigned int    i;
    wxString        path;
    wxFFile         file;

    EnableMenuItem(wxID_UNDO,false);
    m_undoMenu->SetItemLabel(_("Undo")+_T("\tCtrl-Z"));
    if (!g_undoSteps.empty())
    {
        for (i=0; i<g_undoSteps.size(); i++)
        {
#if wxCHECK_VERSION(2,9,0)
            path << g_undoPath << i;
#else
            path=wxString::Format(_T("%s%d"),g_undoPath.c_str(),i);
#endif
            ::wxRemoveFile(path);
        }
        g_undoSteps.clear();
    }
    g_undoPath=wxFileName::CreateTempFileName(wxStandardPaths::Get().GetTempDir()+wxFileName::GetPathSeparator());
    ::wxRemoveFile(g_undoPath);
    file.Close();
}


void MainWin::addUndoStep(wxString operation,bool resetBuffer)
{
    wxString    path;

    if (resetBuffer) resetRedoBuffer();
    EnableMenuItem(wxID_UNDO,true);
    m_undoMenu->SetItemLabel(_("Undo")+_T(" ")+operation+_T("\tCtrl-Z"));
#if wxCHECK_VERSION(2,9,0)
    path << g_undoPath << g_undoSteps.size();
#else
    path=wxString::Format(_T("%s%d"),g_undoPath.c_str(),g_undoSteps.size());
#endif
    g_hmiCanvas->saveProject(path);
    g_undoSteps.push_back(operation);
}


void MainWin::addRedoStep(wxString operation)
{
    wxString    path;

    EnableMenuItem(wxID_REDO,true);
    m_redoMenu->SetItemLabel(_("Redo")+_T(" ")+operation+_T("\tCtrl-Y"));
#if wxCHECK_VERSION(2,9,0)
    path << g_undoPath << g_redoSteps.size() << "R";
#else
    path=wxString::Format(_T("%s%dR"),g_undoPath.c_str(),g_redoSteps.size());
#endif
    g_hmiCanvas->saveProject(path);
    g_redoSteps.push_back(operation);
}


void MainWin::removeUndoStep()
{
    wxString    path;

    assert(g_undoSteps.size()>0);
    addRedoStep(g_undoSteps[g_undoSteps.size()-1]);
    g_undoSteps.pop_back();
#if wxCHECK_VERSION(2,9,0)
    path << g_undoPath << g_undoSteps.size();
#else
    path=wxString::Format(_T("%s%d"),g_undoPath.c_str(),g_undoSteps.size());
#endif

    g_hmiCanvas->newProject();
    g_hmiCanvas->loadProject(path);
    g_flowCanvas->refreshProject(1);
    m_hmiScroller->Scroll(0,0);
    ::wxRemoveFile(path);

    if (g_undoSteps.empty())
    {
        EnableMenuItem(wxID_UNDO,false);
        m_undoMenu->SetItemLabel(_("Undo")+_T("\tCtrl-Z"));
    }
    else m_undoMenu->SetItemLabel(_("Undo")+_T(" ")+g_undoSteps[g_undoSteps.size()-1]+_T("\tCtrl-Z"));
}


void MainWin::removeRedoStep()
{
    wxString    path;

    assert(g_undoSteps.size()>0);
    addUndoStep(g_redoSteps[g_redoSteps.size()-1],false);
    g_redoSteps.pop_back();
#if wxCHECK_VERSION(2,9,0)
    path << g_undoPath << g_redoSteps.size() << "R";
#else
    path=wxString::Format(_T("%s%dR"),g_undoPath.c_str(),g_redoSteps.size());
#endif

    g_hmiCanvas->newProject();
    g_hmiCanvas->loadProject(path);
    g_flowCanvas->refreshProject(1);
    m_hmiScroller->Scroll(0,0);
    ::wxRemoveFile(path);

    if (g_redoSteps.empty())
    {
        EnableMenuItem(wxID_REDO,false);
        m_redoMenu->SetItemLabel(_("Redo")+_T("\tCtrl-Y"));
    }
    else m_redoMenu->SetItemLabel(_("Redo")+_T(" ")+g_redoSteps[g_redoSteps.size()-1]+_T("\tCtrl-Y"));
}


void MainWin::EnableMenuItem(wxInt32 id,bool enable)
{
   m_menuBar->Enable(id,enable);
}


void MainWin::OnTabChanged(wxBookCtrlEvent& WXUNUSED(event))
{
   bool enable;

   if (m_book->GetSelection()==0) enable=true;
   else enable=false;
   m_mainTB->EnableTool(HMI_ID_EDIT,enable);
   m_mainTB->EnableTool(wxID_DELETE,enable);
}



void MainWin::setup()
{
   if (g_objectList.m_projectData) free (g_objectList.m_projectData);
   g_objectList.m_projectData=NULL;
   g_objectList.m_projectData=g_initProjectSettings(true);
   g_hmiCanvas->refreshProject();
   g_flowCanvas->refreshProject(1);
   g_hmiCanvas->setMainWin(this);
}


int MainWin::OnKeyDown(wxKeyEvent &event)
{
   if (m_book->GetSelection()==0) return g_hmiCanvas->OnKeyDown(event);
   return g_flowCanvas->OnKeyDown(event);
}



void MainWin::OnKeyUp(wxKeyEvent&WXUNUSED(event))
{
}



void MainWin::OnExit(wxCommandEvent& WXUNUSED(event))
{
   Close();
}


bool MainWin::Destroy()
{
   if (g_userList) delete g_userList;
   if ((!g_isEmpty) && (!g_isSaved))
   {
      if (wxMessageBox(_("Current changes have not been saved!\nSave project before exiting?"),_("Question"),wxICON_QUESTION|wxYES_NO)==wxYES)
      {
         wxCommandEvent event;
         OnSaveProject(event);
      }
   }
   g_hmiCanvas->newProject();
   g_plugInPanel->newProject();
   return wxFrame::Destroy();
}


void MainWin::OnMenu(wxCommandEvent& event)
{
   if (event.GetId()==wxID_ABOUT) g_About();
   else if (event.GetId()==wxID_UNDO) removeUndoStep();
   else if (event.GetId()==wxID_REDO) removeRedoStep();
   else if (event.GetId()==HMI_ID_COMPILEAS)
   {
      wxString   compiledPath,compiledFile,error;
      ObjectList compileList;
      PCompiler  compiler;

      if (!g_isSaved)
      {
         if (wxMessageBox(_("Project has to be saved before compiling.\nDo you want to save it now?"),_("Question"),wxICON_QUESTION|wxYES_NO|wxCENTRE)==wxNO) return;
         OnSaveProject(event);
      }
      compiledFile=m_projectFile.Mid(0,m_projectFile.Length()-5)+_T(".apcx");
      wxFileDialog* openFileDialog=new wxFileDialog( this,_("Save compiled project as")+_T("..."),m_projectDir,compiledFile,COMPILED_FILETYPES,wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
      if ( openFileDialog->ShowModal() == wxID_OK )
      {
         compiledPath=openFileDialog->GetPath();
d_isCompiling=true;
         if (m_projectDir.Length()<=0) error=compileList.loadProject(m_projectFile,false,false);
         else error=compileList.loadProject(m_projectDir+wxFileName::GetPathSeparator()+m_projectFile,false,false);
         if (error.Length()==0)
         {
            compiler.compile(&compileList);
            compileList.saveProject(compiledPath,true);
            compileList.deleteAll(true);
         }
         else wxMessageBox(m_projectDir+wxFileName::GetPathSeparator()+m_projectFile+_T(":\n")+error,_("Error"),wxICON_ERROR|wxOK|wxCENTRE);
d_isCompiling=false;
      }
      delete openFileDialog;
   }
   else if (event.GetId()==HMI_ID_EDIT)
   {
      wxCommandEvent evt;

      g_hmiCanvas->OnEditControl(evt);
   }
   else if (event.GetId()==wxID_DELETE)
   {
      wxCommandEvent evt;

      addUndoStep(_("Delete"));
      g_hmiCanvas->OnDelControl(evt);
   }
   else if (event.GetId()==wxID_PREFERENCES)
   {
      DlgProjectSettings dlg(this,_("Project settings"));

      addUndoStep(_("Project settings"));
      dlg.ShowModal();
      g_hmiCanvas->refreshProject();
   }
   else if (event.GetId()==HMI_ID_USERPRIVI_SETTINGS)
   {
      DlgUserpriviSettings dlg(this,_("User Privileges"),&g_userPriviData);

      addUndoStep(_("User Privileges"));
      dlg.ShowModal();
   }
   else if (event.GetId()==HMI_ID_USERS)
   {
      DlgUsers dlg(this,_("Users"));

      addUndoStep(_("Users"));
      dlg.ShowModal();
   }
   else if ((event.GetId()==HMI_ID_START_DEBUGGER) || (event.GetId()==HMI_ID_START_PLAYER))
   {
      if (!g_isSaved)
      {
         wxCommandEvent e;
         if (wxMessageBox(_("The current project has been changed and needs to be saved before execution.\nDo you want to save the project now?"),_("Question"), wxICON_QUESTION|wxYES_NO)==wxNO) return;
         OnSaveProject(e);
         if (!g_isSaved) return;
      }
       if (m_projectFile.IsEmpty())
       {
          #ifdef ENV_WINDOWS
          if (event.GetId()==HMI_ID_START_DEBUGGER) wxExecute(wxGetCwd()+wxFileName::GetPathSeparator()+_T("OpenDebugger ")+m_projectDir+wxFileName::GetPathSeparator()+m_projectFile);
          else wxExecute(wxGetCwd()+wxFileName::GetPathSeparator()+_T("OpenPlayer ")+m_projectDir+wxFileName::GetPathSeparator()+m_projectFile);
          #else
          if (event.GetId()==HMI_ID_START_DEBUGGER) wxExecute(_T("OpenDebugger ")+m_projectDir+wxFileName::GetPathSeparator()+m_projectFile);
          else wxExecute(_T("OpenPlayer ")+m_projectDir+wxFileName::GetPathSeparator()+m_projectFile);
          #endif
       }
       else
       {
//          wxMessageBox(_T("\"")+wxGetCwd()+wxFileName::GetPathSeparator()+_T("OpenDebugger.exe\" ")+_T("\"")+projectDir+wxFileName::GetPathSeparator()+projectFile+_T("\""));
          #ifdef ENV_WINDOWS
          if (event.GetId()==HMI_ID_START_DEBUGGER) wxExecute(_T("\"")+wxGetCwd()+wxFileName::GetPathSeparator()+_T("OpenDebugger.exe\" ")+_T("\"")+m_projectDir+wxFileName::GetPathSeparator()+m_projectFile+_T("\""));
          else wxExecute(_T("\"")+wxGetCwd()+wxFileName::GetPathSeparator()+_T("OpenPlayer.exe\" ")+_T("\"")+m_projectDir+wxFileName::GetPathSeparator()+m_projectFile+_T("\""));
          #else
          {
             wxString execParam;

             execParam=m_projectDir+wxFileName::GetPathSeparator()+m_projectFile;
             execParam.Replace(_T(" "),_T("\\ "));
             if (event.GetId()==HMI_ID_START_DEBUGGER) wxExecute(_T("OpenDebugger ")+execParam);
             else wxExecute(_T("OpenPlayer ")+execParam);
          }
          #endif
       }
    }
    else if (event.GetId()==wxID_FIND)
    {
       m_findObject=NULL; // start new search
       findElement(_T(""));
    }
    else if (event.GetId()==HMI_ID_FINDNEXT)
    {
       findElement(m_findString);
    }
    else if (event.GetId()==HMI_ID_INSPECT)
    {
       wxNode                   *node,*connectionNode=NULL;
       flowObject               *object;
       FlowConnection           *connection;
//p�-.8jmu9ik
       bool                      hasEndCondition=false,problemFound=false,hasLogFetchElement=false,isWeecoTarget=false;
       wxInt32                   usesLog=0;
       wxUint32                  totalLogFlags;
       wxString                  usingLogElements;
       list<hmiMutex*>::iterator it;
       hmiMutex                 *mutex;

       for ( it=g_objectList.m_mutexList.begin(); it!=g_objectList.m_mutexList.end(); it++)
       {
          mutex=*it;
          mutex->m_used=false;
          node=g_objectList.getObject((wxNode*)NULL);
          while (node)
          {
             object=(flowObject*)node->GetData();

             if ((object->data.id & HMI_TYPE_MASK)!=0)
             {
                if (mutex->hasObjectID(object->data.id))
                {
                   mutex->m_used=true;
                   break;
                }
             }
             node=g_objectList.getObject(node);
          }
          if (!mutex->m_used)
          {
             if (wxMessageBox(mutex->m_name+_T(":\n")+_("This mutual exclusion group is unused!\nDelete it from the list of mutual exclusions?"),_("Warning"),wxICON_WARNING|wxYES_NO)==wxYES)
             {
                g_objectList.m_mutexList.remove(mutex);
                delete mutex;
                it=g_objectList.m_mutexList.begin();
             }
          }
       }

       node=g_objectList.getObject((wxNode*)NULL);
       while (node)
       {
          object=(flowObject*)node->GetData();
          if (object->data.type==flowObject::FLOW_TYPE_FLOW_STOP) hasEndCondition=true;
          else if (object->data.type==HMI_TYPE_IMAGE)
          {
             if (((hmiImage*)object)->imageNormal.length()<=0)
             {
                if (wxMessageBox(object->name+_T(":\n")+_("There is an image object placed in the HMI Editor that is currently invisible!\nGo to this object?"),_("Warning"),wxICON_WARNING|wxYES_NO)==wxYES)
                {
                   g_hmiCanvas->showElement((hmiObject*)object);
                   return;
                }
             }
          }
          else if (object->data.type==FLOW_TYPE_EXTERNAL_LIB)
          {
             flowExternalIOLib *extObject;
             wxString           libname;

             extObject=(flowExternalIOLib*)object;
             libname=extObject->lib->libname.MakeLower();

             if (libname.Contains(_T("weeco"))) isWeecoTarget=true;
          }
          else if (object->data.type==flowObject::FLOW_TYPE_MISC_LOG) hasLogFetchElement=true;
          totalLogFlags=(object->data.log[0].flags | object->data.log[1].flags |
        		         object->data.log[2].flags | object->data.log[3].flags);
          if ((totalLogFlags & LOG_FLAG_MASK) ||
        	  (object->data.type==flowObject::FLOW_TYPE_MISC_LOG_REC))
          {
             if (usesLog<10)
             {
                usesLog++;
                usingLogElements=usingLogElements+_T("\n")+object->name;
             }
          }
          connectionNode=object->getFlowConnection(connectionNode);
          while (connectionNode)
          {
             connection=(FlowConnection*)connectionNode->GetData();
             if ((connection->data.targetID==0) || (!g_objectList.getObject(connection->data.targetID)))
             {
                connection->data.targetID=0;
                problemFound=true;
                if (wxMessageBox(object->name+_T(":\n")+_("Open flow connections found that are not connected with a target!\nGo to this object?"),_("Warning"),wxICON_WARNING|wxYES_NO)==wxYES)
                {
                   m_book->SetSelection(1);
                   g_flowCanvas->showElement(object);
                   return;
                }
             }
             connectionNode=object->getFlowConnection(connectionNode);
          }
          node=g_objectList.getObject(node);
       }
       if (!hasEndCondition)
        wxMessageBox(_("There is no end condition/no exit application flow object!\nYou will have no chance to stop the player regularly without that!"),_("Warning"),wxICON_WARNING|wxOK);
       else if (!problemFound)
        wxMessageBox(_("No problems could be found within that project"),_("Information"),wxICON_INFORMATION|wxOK);

       if ((hasLogFetchElement) && (usesLog==0))
        wxMessageBox(_("The project uses a Log Output but does not has any element, that collects logging data!"),_("Information"),wxICON_INFORMATION|wxOK);
       else if ((!hasLogFetchElement) && (usesLog>0))
        wxMessageBox(_("The project collects logging data but never use them! This may cause a memory overflow on your target system. At least the following elements are affected:")+usingLogElements,_("Warning"),wxICON_WARNING|wxOK);

       if ((isWeecoTarget) && (g_objectList.m_projectData->timerResolution<200))
        wxMessageBox(_("Your timer resolution may be too high for your intended target platform. Consider to set the projects timer resolution to 200 msec or more."),_("Warning"),wxICON_WARNING|wxOK);
    }
}



void MainWin::findElement(wxString findString)
{
   wxString res;

   if (findString.Length()<1) res=wxGetTextFromUser(_("Enter String to search for"),_("Element name"),m_findString,this);
   else res=findString;

   if (res.Length()>0)
   {
      bool isHMICanvasOpen;
      
      isHMICanvasOpen=(m_book->GetSelection()==0);
      m_findString=res;
      m_findObject=g_objectList.findObjectByName(m_findString,m_findObject,isHMICanvasOpen);
      if (m_findObject)
      {
         if (isHMICanvasOpen) g_hmiCanvas->showElement((hmiObject*)m_findObject->GetData());
         else
         {
            if (((flowObject*)m_findObject->GetData())->getFlowPos().x<0) findElement(m_findString);
            else g_flowCanvas->showElement((flowObject*)m_findObject->GetData());
         }
      }
      else wxMessageBox(_("Could not find object!"),_("Error"),wxICON_ERROR|wxOK);
   }
}



void MainWin::OnCredits(wxCommandEvent& WXUNUSED(event))
{
   g_Credits();
}



void MainWin::OnNewProject(wxCommandEvent& WXUNUSED(event))
{
   if (g_isEmpty) return;
   if (!g_isSaved)
   {
      wxMessageDialog dialog(this,_("Really create a new project?\nThis will delete all the existing data!"),_T("Question"), wxYES_NO|wxICON_QUESTION);
      if (dialog.ShowModal()!=wxID_YES) return;
   }
   g_objectList.m_imageList.clear();
   resetUndoBuffer();
   SetTitle(HMI_TITLE);
   if (g_objectList.m_projectData) free (g_objectList.m_projectData);
   g_objectList.m_projectData=NULL;
   g_objectList.m_projectData=g_initProjectSettings(true);
   g_hmiCanvas->newProject();
   g_plugInPanel->newProject();
   m_projectFile=_T("");
   g_flowLog=NULL;
   g_flowCanvas->refreshProject(1);
}



void MainWin::OnSaveProject(wxCommandEvent& event)
{
   g_objectList.m_projectData->editorX=GetPosition().x;
   g_objectList.m_projectData->editorY=GetPosition().y;
   g_objectList.m_projectData->editorW=GetSize().x;
   g_objectList.m_projectData->editorH=GetSize().y;

   if (m_projectFile.IsEmpty()) OnSaveAsProject(event);
   else
   {
      if (m_projectDir.Length()>0) g_hmiCanvas->saveProject(m_projectDir+wxFileName::GetPathSeparator()+m_projectFile);
      else g_hmiCanvas->saveProject(m_projectFile);
   }
}



void MainWin::OnLoadProject(wxCommandEvent& WXUNUSED(event))
{
   g_fileLoadWarned=false;
   if (g_hmiCanvas->loadProject(&m_projectDir,&m_projectFile))
   {
      resetUndoBuffer();
      SetTitle(HMI_TITLE+m_projectFile);
      SetPosition(wxPoint(g_objectList.m_projectData->editorX,g_objectList.m_projectData->editorY));
      SetSize(wxSize(g_objectList.m_projectData->editorW,g_objectList.m_projectData->editorH));
      m_hmiSplitter->SetSashPosition(-220);
      m_flowSplitter->SetSashPosition(-220);
      m_plugSplitter->SetSashPosition(-220);
      g_flowCanvas->refreshProject(1);
      m_hmiScroller->Scroll(0,0);
   }
}



void MainWin::loadProject(wxString projectPath)
{
   if ((m_projectDir.Length()<=0) && (m_projectFile.Length()<=0))
   {
      wxInt32 pos;

      pos=projectPath.Find(wxFileName::GetPathSeparator(),true);
      if (pos==wxNOT_FOUND) m_projectFile=projectPath;
      else
      {
         m_projectDir=projectPath.Mid(0,pos);
         m_projectFile=projectPath.Mid(pos+1);
      }
   }
   g_hmiCanvas->loadProject(projectPath);
   resetUndoBuffer();
   SetTitle(HMI_TITLE+projectPath);
   if (g_objectList.m_projectData)
   {
      SetPosition(wxPoint(g_objectList.m_projectData->editorX,g_objectList.m_projectData->editorY));
      SetSize(wxSize(g_objectList.m_projectData->editorW,g_objectList.m_projectData->editorH));
   }
   g_flowCanvas->refreshProject(1);
   m_hmiScroller->Scroll(0,0);
}



void MainWin::OnSaveAsProject(wxCommandEvent& WXUNUSED(event))
{
   wxFileDialog* openFileDialog;

   openFileDialog=new wxFileDialog( this,_("Save project as")+_T("..."),m_projectDir,m_projectFile,PROJECT_FILETYPES,wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
   g_objectList.m_projectData->editorX=GetPosition().x;
   g_objectList.m_projectData->editorY=GetPosition().y;
   g_objectList.m_projectData->editorW=GetSize().x;
   g_objectList.m_projectData->editorH=GetSize().y;

   if ( openFileDialog->ShowModal() == wxID_OK )
   {
      wxString projectPath;

      projectPath=openFileDialog->GetPath();
      oapc_path_split(&projectPath,&m_projectDir,&m_projectFile,_T(".apcp"));

      g_hmiCanvas->saveProject(projectPath);
      SetTitle(HMI_TITLE+m_projectFile);
   }
   delete openFileDialog;
}

#endif //ENV_INT


