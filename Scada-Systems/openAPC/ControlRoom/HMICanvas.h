#ifndef HMICANVAS_H
#define HMICANVAS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/wx.h>
#include <wx/scrolwin.h>

class MainWin;
class FlowWalkerPool;
class wxFoldPanel;

#include <wx/spinbutt.h>

#include "BasePanel.h"
#include "hmiObject.h"
#include "flowObject.h"
#include "ObjectList.h"
#ifndef ENV_EDITOR
 #include "flowWalkerPool.h"
#endif

class MainWin;
class wxFoldPanelBar;

#ifdef ENV_DEBUGGER
 class DebugWin;
#endif

#ifdef ENV_PLAYER
 #define FLOW_LOGIN_USER_CHANGE_PWD 0x0001
#endif

class HMICanvas: public BasePanel
{
public:
    HMICanvas(wxScrolledWindow *parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            wxInt32 style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxPanelNameStr);
    HMICanvas(wxWindow *parent,wxUint32 myID,wxInt32 style=wxTAB_TRAVERSAL | wxNO_BORDER);
    ~HMICanvas();

#ifndef ENV_HPLAYER
 #if wxCHECK_VERSION(2,9,0)
   virtual void AddPendingEvent(const wxEvent& event);
 #endif
#endif

#ifdef ENV_PLAYER
    void OnButtonClicked(wxCommandEvent &event);
    void OnSliderScrolled(wxScrollEvent &event);
    void OnSliderScrolling(wxScrollEvent &event);
    void DoOnSliderScrolled(flowObject *object,wxUint32 outflag,wxUint32 inheritThreadID,wxLongLong inheritCreationTime);
    void OnNumberChanged(wxCommandEvent &event);
    void OnNumberFocusChanged(wxFocusEvent &event);
    void OnNumberChanging(wxCommandEvent &event);
    void OnSpinNumberChanging(wxSpinEvent& event);
    void OnCharChanged(wxCommandEvent &event);
    void OnCharFocusChanged(wxFocusEvent &event);
    void OnHotKey(wxKeyEvent &event);
    void OnExternalEvent(wxCommandEvent& event);
#ifdef ENV_DEBUGGER
    void displayControlNames(wxByte enable);
 #endif
#else
    void            saveProject(wxString file);
    void            setMainWin(MainWin *mainWin);
    MainWin        *getMainWin();
    void            OnChildMouseEvent(wxMouseEvent& event);
    void            OnDelControl(wxCommandEvent& event);
    void            OnEditControl(wxCommandEvent& event);
    int             OnKeyDown(wxKeyEvent& event);
    void            showElement(hmiObject *object);
    wxFoldPanelBar *createFoldBar(wxWindow *parent);
#endif
#if defined ENV_DEBUGGER || ENV_EDITOR
    bool     loadProject(wxString *m_projectDir,wxString *m_projectFile);
    void     newProject();
#endif
    void     refreshProject();
    wxByte   loadProject(wxString projectPath);
    void     displayName(wxString Name);

private:
    void    init();
    void    OnPaint(wxPaintEvent& event);
#ifndef ENV_PLAYER
    void    OnMouseEvent(wxMouseEvent& event);
    void    OnNewControl(wxCommandEvent& event);
    void    OnDupControl(wxCommandEvent& event);
    void    OnMaxControl(wxCommandEvent& event);
    void    OnSelectControl(wxCommandEvent& event);
    void    OnSnapControlToGrid(wxCommandEvent& event);
    void    OnPopupMenu(wxCommandEvent& event);
    void    OnFoldbarClicked(wxMouseEvent& event);
    void    ShowContextMenu(wxContextMenuEvent &event);
    wxInt32 addFoldBarItem(wxFoldPanelBar *bar,wxFoldPanel *foldItem,wxInt32 id,wxString name);
#endif

#ifdef ENV_PLAYER
 #ifdef ENV_DEBUGGER
    wxByte              displayNames;
 #endif
#else
    wxInt32                  selectMode,m_lastElementID;
    MainWin                 *mainWin;
    wxPoint                  lastMousePos,startMousePos,rectStart,rectEnd;
    hmiObject               *highlightObject;
    std::list<wxStaticText*> m_foldBarList;
    wxFoldPanelBar          *m_hmiBar;
#endif
    wxScrolledWindow        *m_owner;
    wxString                 m_projectDir,m_projectFile;
    wxString                 displayedName;

//    DECLARE_CLASS(HMICanvas)
    DECLARE_EVENT_TABLE()
};

#endif
