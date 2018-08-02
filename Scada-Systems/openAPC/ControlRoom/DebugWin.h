#ifdef ENV_INT
 #include "../DebugWin.h"
#endif

#ifndef DEBUGWIN_H
#define DEBUGWIN_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/wx.h>
#include <wx/frame.h>

#ifdef ENV_DEBUGGER
#define DEBUG_STOP_COND                                             0x1000
#define DEBUG_STOP_COND_THREAD_ID                  (DEBUG_STOP_COND|0x0001)
#define DEBUG_STOP_COND_ILLEGAL_IO                 (DEBUG_STOP_COND|0x0002)
#define DEBUG_STOP_COND_RUN_TIMEOUT                (DEBUG_STOP_COND|0x0004)
#define DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT  (DEBUG_STOP_COND|0x0008)
#define DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_OUTPUT (DEBUG_STOP_COND|0x0010)
#define DEBUG_STOP_COND_LIB_FAILED                 (DEBUG_STOP_COND|0x0020)
#define DEBUG_STOP_COND_INCOMPATIBLE_DATATYPE      (DEBUG_STOP_COND|0x0040)
#define DEBUG_STOP_COND_NO_MEMORY                  (DEBUG_STOP_COND|0x0080)
#define DEBUG_STOP_COND_LIB_RETURNED_ERROR         (DEBUG_STOP_COND|0x0100)

#define DEBUG_INFO                                                  0x2000
#define DEBUG_INFO_FOCUS_LOST                           (DEBUG_INFO|0x0001)
#define DEBUG_INFO_DATA_FROM_IS_REJECTED                (DEBUG_INFO|0x0002)

#define DEBUG_ERROR                                                 0x4000
#define DEBUG_ERROR_CONVERSION_ERROR                   (DEBUG_ERROR|0x0001)
#define DEBUG_ERROR_EXECUTION_LIMIT                    (DEBUG_ERROR|0x0002)
#endif

#include "HMICanvas.h"
#include "ObjectList.h"

class PlayerMainApp;

class DebugWin : public wxFrame
{
public:
   DebugWin(wxWindow *parent,const wxChar *title, int xpos, int ypos, int width, int height,HMICanvas *hmiCanvas);
   ~DebugWin();
   void setDebugInformation(flowObject *object,wxUint32 debugFlags,wxString addData);

   static const wxInt32 RUN_STATE_STOPPED;
   static const wxInt32 RUN_STATE_RUNNING;
   static const wxInt32 RUN_STATE_STEP;

private:
   void OnLoadProject(wxCommandEvent& event);
   void OnExit(wxCommandEvent& event);
   void OnMenu(wxCommandEvent& event);   
   void OnCredits(wxCommandEvent& event);   
   void OnToolbar(wxCommandEvent& event);
   void OnTimer(wxTimerEvent& event);
   void OnClose(wxCloseEvent& WXUNUSED(event));

   void updateThreadNum();
   void setRunState(wxInt32 runState);
   void appendLogText(wxString text);

   wxInt32            runState;
   wxUint32           flags;
   HMICanvas         *hmiCanvas;
   wxWindow          *parent;
   wxCheckBox        *stopWarnCB,*displayNamesCB;
   wxToolBar         *mainTB;
   wxStatusBar       *mainSB;
   wxToolBarToolBase *runTool;
   wxTextCtrl        *mainLog;
   wxTimer           *stateTimer;

   DECLARE_EVENT_TABLE()

};

#endif
