#ifndef GLOBALS_H
#define GLOBALS_H

// global definitions used for editor, player and debugger only

#pragma pack(8)

#ifndef ENV_BEAM
 #include "wx/foldbar/foldpanelbar.h"
#endif

#include "ObjectList.h"
#include "FlowObjects.h"
#include "ExternalIOLibs.h"
#include "ExternalIOLib.h"
#include "FlowDataController.h"
#ifndef ENV_EDITOR
 #include "FlowDispatcherThread.h"
 #include "flowWalkerPool.h"
#endif
#include "flowLog.h"
#include "userprivi.h"

#ifndef ENV_HPLAYER
 #include "common/oapcResourceBundle.h"
 #ifndef ENV_BEAM
  #include "PlayerMainApp.h"
 #endif
 class MainWin;
 class UserList;
 class hmiSpecialPanel;
 class PlugInPanel;
#else
 #include "HPlayerMainApp.h"
#endif

#define DIALOG_X_SIZE      470
#ifdef _DEBUG
 #define MAX_RUNTIME_MILLIS 100
#else
 #define MAX_RUNTIME_MILLIS 175
#endif

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif

extern const wxChar *COMPILED_FILETYPES;

class FlowDispatcherThread;

extern void     g_fileVersionError(void);
extern void     g_OAPCMessageBox(const wxUint32 rcode,const ExternalIOLib *lib,const void *pluginData,wxWindow *parent,const wxString filename);
extern wxString g_OAPCMessage(const wxUint32 rcode,const ExternalIOLib *lib,const void *pluginData,const wxString filename,wxString *pluginName);
#ifndef ENV_HPLAYER
extern void     g_createDialogButtons(wxDialog *parent,wxSizer *bsizer,const int flags);
#endif
extern bool     g_fileLoadWarned;
#ifdef ENV_WINDOWS
extern int asprintf(char **str, const char *fmt, ...);
//extern /*__declspec(noinline)*/ wxString g_getLicenseFilename(void);
#else
extern wxString g_getLicenseFilename() __attribute__ ((noinline));
#endif

#ifdef ENV_EDITOR
 #define CMP_BUTTON_SIZE wxSize(38,22)
 #define HMI_ID_PROJECT_SETTINGS   0x3001
 #define HMI_ID_START_PLAYER       0x3003
 #define HMI_ID_START_DEBUGGER     0x3004
 #define HMI_ID_CREDITS            0x3005
 #define HMI_ID_FINDNEXT           0x3006
 #define HMI_ID_INSPECT            0x3007
 #define HMI_ID_EDIT               0x3008
 #define HMI_ID_TAB                0x3009
 #define HMI_ID_COMPILEAS          0x3010
 #define HMI_ID_USERPRIVI_SETTINGS 0x3011
 #define HMI_ID_USERS              0x3012
 #ifdef ENV_INT
  #define HMI_ID_LICENSEINFO        0x3013
  #define HMI_ID_LICENSERESET       0x3014
 #endif
 #define SDBG
#else
 #ifndef SDBG
  #ifdef _DEBUG
   extern void setDebugInfo(char *file,int line);
   #define SDBG setDebugInfo(__FILE__,__LINE__);
  #else
   #define SDBG
  #endif
 #endif
#endif

#define HMI_TITLE _T("ControlRoom ")

#ifndef BLUE_PEN
 #define BLUE_PEN          wxPen(0x00FF0000)
#endif
#if wxCHECK_VERSION(2,9,0)
 #define BLUE_PEN_DOT      wxPen(0x00FF0000,1,wxPENSTYLE_DOT)
#else
 #define BLUE_PEN_DOT      wxPen(0x00FF0000,1,wxDOT)
#endif
#define YELLOW_COLOUR     wxColour(0x0000FFFF)
#define MAGENTA_COLOUR    wxColour(0x00FF00FF)
#define DARK_GREEN_COLOUR wxColour(0x0000A000)
#define DARK_GREY_COLOUR  wxColour(0x00555555)
#define BLUE_COLOUR       wxColour(0x00FF3333)

extern const wxChar *PROJECT_FILETYPES;
extern const wxChar *GROUP_FILETYPES;
extern const wxChar *IMAGE_FILETYPES;

extern wxString              g_projectTouchFontFaceName;
extern flowLog              *g_flowLog;
extern ObjectList            g_objectList;
extern ExternalIOLibs       *g_externalIOLibs,*g_externalHMILibs;
extern bool                  d_isCompiling;
extern struct userprivi_data g_userPriviData;
#ifndef ENV_HPLAYER
 extern UserList             *g_userList;
 extern HMICanvas            *g_hmiCanvas;
#endif

#ifdef ENV_EDITOR
 class FlowCanvas;

 extern std::vector<wxString> g_undoSteps,g_redoSteps;
 extern wxString              g_undoPath;
 extern wxMenu               *g_mainMenuFile;
 extern FlowCanvas           *g_flowCanvas;
 extern PlugInPanel          *g_plugInPanel;
 extern ObjectList            g_selectedList,g_cuttedList;
 extern bool                  g_isSaved,g_isEmpty;
 extern FlowObjects           g_flowObjects;
 extern MainWin              *g_mainWin;
 extern hmiSpecialPanel      *g_hmiUserMgmntPanel;
 extern wxStaticText         *g_userMgmntButton;
 extern wxInt32               g_nextHMILeftClickOperation;
#else
 #ifndef ENV_CNCO
 #ifndef ENV_BEAMLOCK
  extern wxFrame              *g_mainWin;
 #else
  extern MainWin              *g_mainWin;
 #endif
 #endif
 extern void                 *g_ispaceHandle;
 #ifdef _DEBUG
  extern int                 dbg_line;
  extern char                dbg_file[200+1];
 #endif
#endif

#ifdef ENV_DEBUGGER
#define DEBUG_STEP_DO_STEP 1
#define DEBUG_STEP_DONE    2

class WatchWin;
class DBWatchWin;

extern wxByte                g_threadsDisabled;
extern DebugWin             *g_debugWin;
extern wxInt32               g_debugStepNum,g_debugStepMode;
extern WatchWin             *g_watchWin;
extern DBWatchWin           *g_DBwatchWin;
extern void                 *g_watchWinLock;
#endif

#ifdef ENV_PLAYER
 class OSNumpad;
 class OSKeyboard;
 class FlowWalkerPool;

 extern void                   *g_imagePaintMutex;
 extern FlowWalkerPool          g_flowPool;
 extern FlowDispatcherThread   *g_flowDispatcher;
 extern FlowDataController      g_dataCtrl;
 extern wxByte                  g_breakThreads;
 extern wxString                g_projectFile,g_projectDir,g_projectPath;
 extern OSNumpad               *g_numpad;
 extern struct oapc_ispace_auth g_auth;
 extern std::list<wxInt32>      g_pluggerIDList;
 extern bool                    g_allowInputPolling;
 #ifndef ENV_DEBUGGER
  extern bool                   g_invisibleMode;
 #endif
#endif

extern struct hmiProjectData *g_initProjectSettings(bool deleteUserList);
extern void                   g_setFont(wxWindow *ui,wxFont font);
extern void                   g_checkString(wxString &val);
extern wxBitmapType           g_getBitmapFlag(wxString path);
#if defined ENV_EDITOR || ENV_DEBUGGER
 extern void                   g_About(void);
 extern void                   g_Credits(void);
#endif
#ifdef ENV_PLAYER
 extern wxImage *g_getImageFromGrey8(oapcBinHeadSp &value);
#endif

/* XPM */
extern const char * icon_xpm[];

#endif

