#ifdef ENV_INT
#include "../MainWin.h"
#endif

#ifndef MAINWIN_H
#define MAINWIN_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/bookctrl.h>
#include <wx/colordlg.h>
#include <wx/splitter.h>

class MainWin;
class HMICanvas;
class ObjectList;

#include "HMICanvas.h"
#include "FlowCanvas.h"
#include "ObjectList.h"
#include "FlowObjects.h"

#define HMI_SCROLL_UNIT 10



extern wxColourDialog       *g_colourDialog;



class MainWin : public wxFrame
{
public:
	MainWin( const wxChar *title, int xpos, int ypos, int width, int height );
	~MainWin();
	virtual int  OnKeyDown(wxKeyEvent& event);
	virtual void OnKeyUp(wxKeyEvent& event);
	virtual bool Destroy();
           void loadProject(wxString projectPath);
           void setup(void);
           void addUndoStep(wxString operation,bool resetBuffer=true);

private:
    void OnNewProject(wxCommandEvent& event);
    void OnLoadProject(wxCommandEvent& event);
    void OnSaveProject(wxCommandEvent& event);
    void OnSaveAsProject(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnMenu(wxCommandEvent& event);
    void OnCredits(wxCommandEvent& event);
    void OnTabChanged(wxBookCtrlEvent& event);
    void EnableMenuItem(wxInt32 id,bool enable);
    void findElement(wxString findString);
    void removeUndoStep();
    void resetRedoBuffer();
    void resetUndoBuffer();
    void addRedoStep(wxString operation);
    void removeRedoStep();

    wxScrolledWindow *flowScroller;
    wxBookCtrl       *m_book;
    wxString          m_projectFile,m_projectDir; // project path after a file dialogue has been used
    wxString          m_findString;
    wxScrolledWindow *m_hmiScroller;
    wxNode           *m_findObject;
    wxToolBar        *m_mainTB;
    wxSplitterWindow *m_hmiSplitter,*m_flowSplitter,*m_plugSplitter;
    wxMenuItem       *m_undoMenu,*m_redoMenu;
    wxMenuBar        *m_menuBar;

    DECLARE_EVENT_TABLE()

};

#endif
