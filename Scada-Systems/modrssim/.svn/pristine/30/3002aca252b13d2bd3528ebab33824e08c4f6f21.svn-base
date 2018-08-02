/////////////////////////////////////////////////////////////////////////////
//
// FILE; MOD_simDlg.cpp ; implementation file
//
// Mod_RSSim (c) Embedded Intelligence Ltd. 1993,2009
// AUTHOR: Conrad Braam.  http://www.plcsimulator.org
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Affero General Public License as
//    published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Fixes: Tranparency
//        Activity ticks repainting cut down, reduced CPU load& flicker
// Added: Tooltips & Toolbar
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServerSettings232Dlg.h"
#include "ServerSettingsEthDlg.h"
#include "AnimationSettings.h"
#include "FaultsDlg.h"
#include "resource.h"
#include "Creditstatic.h"
#include "about.h"
#include "TrainingSimDlg.h"
#include "ABCommsProcessor.h"
#include "JoySCCEmulation.h"
#include "CSVFileImportDlg.h"
#include "updatecheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LOADREGMSGSTRING      "LOADREGISTERS"
#define ASSERTMICROBOX(a)     ASSERT(a >=0);ASSERT(a <STATIONTICKBOXES);
#define ANIMATION_ANTISTARVATION_MS (50)
#define MAX_COMMSDEBUGGER_BUFFER    (1000)   // max size for the pause buffer
#define MAX_COMMSDEBUGGER_ENTRIES   (2000)   // listbox


const UINT    wm_LoadRegisters = RegisterWindowMessage( LOADREGMSGSTRING );

WORD PLCMemPrefixes[MAX_MOD_MEMTYPES] =
{
   0, // outputs
   1, // in
   3, // analogs
   4, // registers
   6, // ext. registers
   
};

CHAR * PLCMemoryTypes[MAX_MOD_MEMTYPES] =
{
   "Coil Outputs        (00000)",// outputs
   "Digital Inputs      (10000)",// in
   "Analogue Inputs  (30000)",   // analogs
   "Holding Registers",          // registers
   "Extended Registers",         // ext. registers
   ""
};


CControllerMemory PLCMemory;

char * stateDescriptions [] =
{
   "Serv. starting.",   // RSPORTCURRENTLY_VOID       
   "Serv. read data.",  // RSPORTCURRENTLY_READING    
   "Serv. write data.", // RSPORTCURRENTLY_WRITTING   
   "Serv. close port.", // RSPORTCURRENTLY_CLOSING    
   "Serv. opening.",    // RSPORTCURRENTLY_OPENING
   "Serv. idle."        // RSPORTCURRENTLY_IDLE
};

char * sockStateDescriptions [] =
{
   "Serv. starting.",   // SOCKETCURRENTLY_VOID         
   "Serv. listening.",  // SOCKETCURRENTLY_LISTENING    
   "Serv. read data.",  // SOCKETCURRENTLY_READING      
   "Serv. write data.", // SOCKETCURRENTLY_WRITTING     
   "Serv. closing.",    // SOCKETCURRENTLY_CLOSING   
   "Serv. idle."        // SOCKETCURRENTLY_IDLE      
};

DWORD PLCDataFormatsTable[] =
{
   CMemoryEditorList::VIEWFORMAT_DECIMAL,
   CMemoryEditorList::VIEWFORMAT_HEX,
   CMemoryEditorList::VIEWFORMAT_WORD,
   CMemoryEditorList::VIEWFORMAT_DWORD,
   CMemoryEditorList::VIEWFORMAT_LONG,
   CMemoryEditorList::VIEWFORMAT_FLOAT,
   CMemoryEditorList::VIEWFORMAT_CHAR
};

extern CHAR * ViewerFormatNames[6];


/////////////////////////////////////////////////////////////////////////////
// CMOD_simDlg dialog

void CMOD_simDlg::InitGlobals()
{
DWORD major,minor;
   CUpdateCheck::GetFileVersion(major, minor);
   sprintf_s(lpsMyAppVersion, sizeof(lpsMyAppVersion), "%d.%d", HIWORD(major),LOWORD(major));

   pGlobalDialog = this;
}


CMOD_simDlg::CMOD_simDlg(CWnd* pParent /*=NULL*/)
: CDialog(CMOD_simDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMOD_simDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIconMB = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIconETH = AfxGetApp()->LoadIcon(IDR_MAINZETH);
	m_hIconDF1 = AfxGetApp()->LoadIcon(IDR_MAINZDF1);
	m_hIconJOY = AfxGetApp()->LoadIcon(IDR_MAINJOY);

   // set version # and other globals
   CMOD_simDlg::InitGlobals();

   // m_pNoiseSettings = new CRS232Noise;
   m_pNoiseSettings = NULL; // disabled in version 7.7
   //m_pNoiseSettings->SetErrorTypes(FALSE,0);   // init noise obj, copy it each time

   EnableAutomation();
   m_scriptEngineInitilized = FALSE;
   m_reloadAnimationScript = TRUE;
   m_lastRuntime = -1;

   m_PacketsReceived = m_PacketsSent = 0;
   m_portActivityRX = m_portActivityTX = 0;
   m_portModemStatus = 0;  // modem lamps off


   m_loadedProtocol = m_selectedProtocol = PROTOCOL_SELNONE;
   m_ServerSocketState = 0;

   m_zeroValues = FALSE;

   m_pServerRS232Array = NULL;
   m_pServerSockArray = NULL;
   m_ServerRSState = RSPORTCURRENTLY_VOID;
   m_responseDelay = 0;
   m_MOSCADchecks = FALSE;
   m_modifyThenRespond = TRUE;
   m_disableWrites = FALSE;
   m_PLCMaster = FALSE;
   m_useBCC = FALSE;

   m_autoLoad = FALSE;
   m_startAsOff = FALSE;

   m_ABMasterSourceStation = 20;
   m_ABMasterDestStation = 10;  
   m_ABMasterNumFiles = 22;   
   m_ABMasterRunMode = TRUE;      
   m_ABMasterReadVariables = FALSE;
   m_ABMasterWriteVariables = TRUE;
   m_ABmasterIDLETime = 200;
   m_ABtimeoutValue = 300;


   m_stayOnTop = FALSE;

   InitializeCriticalSection(&dispCritSection);
   InitializeCriticalSection(&debuggerCritSection);

   m_animationIncValue = 10;
   m_animationON = FALSE;
   m_plantAnimation = FALSE;
   m_animationBYTE = FALSE;
   m_animationWORD = TRUE;
   m_animationRefreshes = 5;
   m_animationCounter = m_animationRefreshes;
   m_busyCreatingServers = FALSE;
   m_animationPeriod = 5; //
   m_animationScriptFile = L"";

   m_enableHTMLGUI = FALSE;
   m_HTMLUpdateRate = 5;
   m_inputHTMLFilename = "input.html";
   m_outputHTMLFilename = "output.html";


   //m_startTime = GetTickCount();
   m_registerShow = TRUE;
   m_commsTimes = FALSE;
   m_paused = FALSE;
   m_commsTobottom = TRUE;
}

CMOD_simDlg::~CMOD_simDlg()
{

   // Free up memory used by the debugger queue if 
   // it has any un-processed messages at shutdown time.
CString *pDebugentry;
   while (!m_debugDataQueue.IsEmpty())
   {
      pDebugentry = m_debugDataQueue.RemoveHead();
      delete pDebugentry;
   }
}

// ----------------------------------- ToggleOnTop ---------------------------
// toggle the "stay on top" function.
void CMOD_simDlg::ToggleOnTop()
{
CMenu* pSysMenu = GetSystemMenu(FALSE);
DWORD state;

   state = pSysMenu->GetMenuState(IDM_ONTOP, MF_BYCOMMAND);
   pSysMenu->CheckMenuItem(IDM_ONTOP, (state&&MF_CHECKED==MF_CHECKED)? MF_UNCHECKED: MF_CHECKED);
   if (state&&MF_CHECKED==MF_CHECKED)
   { // user un-check the item
      SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
      m_stayOnTop = FALSE;
   } else
   {
      SetWindowPos(&wndTopMost,0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
      m_stayOnTop = TRUE;
   }
} // ToggleOnTop

// ---------------------------------- DoDataExchange ---------------------------
void CMOD_simDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMOD_simDlg)
	DDX_Control(pDX, IDC_PROTOCOL, m_protocolCombo);
	DDX_Control(pDX, IDC_VSCROLLSTATIONS, m_vScrollBarStations);
	DDX_Control(pDX, IDC_COMMSLIST, m_commsListBox);
	DDX_Control(pDX, IDC_DATAFORMAT, m_dataFormatCombo);
	//}}AFX_DATA_MAP
}

// ----------------------------------- OnGetDefID ------------------------------
LRESULT CMOD_simDlg::OnGetDefID(WPARAM wp, LPARAM lp) 
{
   // Do not return an control ID for the default control.
   // This prevents the <ENTER> key from closing the dialog
    return MAKELONG(0,DC_HASDEFID); 
}

// ------------------------- GetPLCMemoryLimit --------------------------------
DWORD CMOD_simDlg::GetPLCMemoryLimit(DWORD area)
{
   //if (m_busyCreatingServers)
   //   return(0);
   return PLCMemory[area].GetSize();
}

// ---------------------------- GetPLCMemoryValue -----------------------------
DWORD CMOD_simDlg::GetPLCMemoryValue(DWORD area, DWORD offset, WORD type)
{
DWORD dwValue=0;

   ASSERT(area < (DWORD)GetNumMemoryAreas());
   if (offset >= MAX_MOD_MEMWORDS)
      return (0);
   //ASSERT(offset < MAX_MOD_MEMWORDS);
   switch(type)
   {
   case CMemoryEditorList::VIEWFORMAT_DECIMAL:
   case CMemoryEditorList::VIEWFORMAT_HEX:
   case CMemoryEditorList::VIEWFORMAT_WORD:
      dwValue = PLCMemory[area][offset];
      break;
   case CMemoryEditorList::VIEWFORMAT_DWORD:
   case CMemoryEditorList::VIEWFORMAT_LONG:
      dwValue = PLCMemory[area][offset]<<16;
      if (offset < MAX_MOD_MEMWORDS)
         dwValue += PLCMemory[area][offset+1];
      break;
   case CMemoryEditorList::VIEWFORMAT_FLOAT:
      dwValue = PLCMemory[area][offset]<<16;
      if (offset < MAX_MOD_MEMWORDS)
         dwValue += PLCMemory[area][offset+1];
      if (pGlobalDialog->IsClone())
         SwopWords(&dwValue);          //clone PLC's have a swapped float
      break;
   case CMemoryEditorList::VIEWFORMAT_CHAR:
      dwValue = PLCMemory[area][offset];
      break;
   default:
      ASSERT(0);
      break;
   }
   return (dwValue);
} // GetPLCMemoryValue


BEGIN_MESSAGE_MAP(CMOD_simDlg, CDialog)
   ON_MESSAGE(DM_GETDEFID, OnGetDefID)
	ON_WM_CONTEXTMENU()
   ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnTTN_NeedText )
	//{{AFX_MSG_MAP(CMOD_simDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SETTINGS, OnSettings)
	ON_BN_CLICKED(IDC_VIEWHEX, OnViewhex)
	ON_BN_CLICKED(IDC_VIEWDEC, OnViewdec)
	ON_CBN_SELCHANGE(IDC_COMBOAREA, OnSelchangeComboArea)
	ON_BN_CLICKED(IDC_SIMULATE, OnSimulate)
	ON_BN_CLICKED(IDC_ZEROES, OnZeroes)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CLONE, OnClone)
	ON_CBN_SELCHANGE(IDC_DATAFORMAT, OnSelchangeDataFormat)
	ON_BN_CLICKED(IDS_ABOUTBOX, OnAboutboxBn)
	ON_COMMAND(IDB_ALPHABLEND, OnAlphablendFlip)
	ON_COMMAND(IDC_NOISE, OnNoise)
	ON_COMMAND(IDC_EMULATION, OnEmulation)
	ON_COMMAND(IDH_ABOUT, OnAbout)
	ON_COMMAND(IDH_TRANSPARENCY, OnTransparencyToggle)
	ON_COMMAND(IDH_TOGGLEDISP, OnToggleDisplay)
	ON_COMMAND(IDH_COPYCLIP, OnCopyToClipBoard)
	ON_BN_CLICKED(IDC_PAUSECOMMS, OnPausecomms)
	ON_BN_CLICKED(IDC_TRACKCOMMS, OnTrackcomms)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_PROTOCOL, OnSelchangeProtocol)
	ON_BN_CLICKED(IDC_TRAININGSIM, OnTraining)
	ON_BN_CLICKED(IDC_CLEARCOMMS, OnClearcomms)
	ON_BN_CLICKED(IDC_SHOWCOMMSTIME, OnShowcommstimeClicked)
	ON_COMMAND(IDH_INJECTERROR, OnInJecterror)
	ON_COMMAND(IDC_OPEN, OnOpenPort)
	ON_COMMAND(IDC_CLOSE, OnClosePort)
	ON_COMMAND(IDM_TOGGLEPORT, OnTogglePortState)
	ON_COMMAND(IDH_EMULATION, OnEmulation)
	ON_COMMAND(IDH_SETTINGS, OnSettings)
	ON_COMMAND(IDH_LOAD, OnLoad)
	ON_COMMAND(IDH_SAVE, OnSave)
	ON_COMMAND(IDH_NOISE, OnNoise)
	ON_COMMAND(IDH_ERASE, OnZeroes)
	ON_COMMAND(IDH_HELP, OnAbout)
	ON_BN_CLICKED(IDB_TOGGLEDISP, OnToggleDisplay)
	ON_COMMAND(IDC_CSVIMPORT, OnCsvImportPop)
	//}}AFX_MSG_MAP
   ON_WM_GETMINMAXINFO()
#if     _MSC_VER > 1200	//VC 9
   ON_REGISTERED_MESSAGE( wm_LoadRegisters, OnLoad_ )
#else
   ON_REGISTERED_MESSAGE( wm_LoadRegisters, OnLoad )
#endif
END_MESSAGE_MAP()

CMOD_simDlg *pGlobalDialog;

/////////////////////////////////////////////////////////////////////////////
// CMOD_simDlg message handlers


// ----------------------------- MyProcessingLoop----------------------------
// This is a little secondary message processing loop used when creating RS servers
// This routine allows the GUI to repaint itself and update as well as perform 
// some connections/comms on any servers threads that are already running. In the 
// Ethernet simulator version, this method would handle all the extra listen socket 
// creation etc..
//
// A HourGlass cursor during this time is used to precent anyone clicking buttons and 
// getting us into any killer loops.
BOOL CMOD_simDlg::MyProcessingLoop(int loops)
{
   MSG msg;
   
   // Secondary message loop
   while ((PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))&&(loops>=0))
   {
      loops--;
      if (msg.message == WM_QUIT)
      {
         // Repost the QUIT message so that it will be retrieved by the
         // primary GetMessage() loop.
         PostQuitMessage(msg.wParam);
         return FALSE;
      }
      // Pre-process message if required (TranslateAccelerator etc.)
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   return(TRUE);
}

// -------------------------- PacketsReceivedInc -------------------------
void CMOD_simDlg::PacketsReceivedInc()
{
   m_PacketsReceived++;
}

// ---------------------------- PacketsSentInc ----------------------------
void CMOD_simDlg::PacketsSentInc()
{
   m_PacketsSent++;
}

// ------------------------------- GetWordValue -------------------------------
// return (typically 16) bits as an integer value
DWORD CMOD_simDlg::GetWordValue(const LONG area, 
                                const DWORD startBit, 
                                DWORD numBits)
{
DWORD registerValue = 0;
DWORD maximumOffset = GetPLCMemoryLimit(area);

   if (maximumOffset < startBit + numBits)
      numBits = maximumOffset - startBit;
   if (IsDlgButtonChecked(IDC_CLONE))
   {
      for (DWORD i=0 ; i<numBits ; i++)  //gather bits in backwards
         registerValue += PLCMemory[area][startBit+i]<<i;
   }
   else
   {
      for (DWORD i=0 ; i<numBits ; i++)
         registerValue += PLCMemory[area][startBit+(numBits-i-1)]<<i;
   }
   return (registerValue);
} // GetWordValue

// ------------------------------- SetWordValue -------------------------------
// re-pack (typically 16) bits into their individual bits
void CMOD_simDlg::SetWordValue(const LONG area, const DWORD startBit, const DWORD dwValue, const DWORD numBits)
{
// lock the memory for writting
CMemWriteLock lk(PLCMemory.GetMutex());

   if (lk.IsLocked())
      return;
   if (IsDlgButtonChecked(IDC_CLONE))
   {
      for (DWORD i=0 ; i<numBits ; i++)  //gather bits in backwards
         //registerValue += PLCMemory[pGlobalDialog->m_memAreaDisplayed][startBit+i]<<i;
         PLCMemory.SetAt(area, startBit+i, ((dwValue & (0x0001<<i))?1:0));
   }
   else
   {
      for (DWORD i=0 ; i<numBits ; i++)
         //registerValue += PLCMemory[pGlobalDialog->m_memAreaDisplayed][startBit+(numBits-i-1)]<<i;
         PLCMemory.SetAt(area, startBit+(numBits-i-1), ((dwValue & (0x0001<<i))?1:0));
   }
} // SetWordValue

// ------------------------------ FillComboBox ----------------------
// load an array of strings into a combo-box, then select one of them.
void CMOD_simDlg::FillComboBox(UINT id,char ** strTable, const DWORD currentsetting)
{
DWORD count;
CComboBox *pCombo = ((CComboBox*)GetDlgItem(id));

   pCombo->ResetContent();
   for (count = 0; strlen(strTable[count]); count++)
   {
#ifdef _UI_DEBUGGING
      OutputDebugString(strTable[count]);
      OutputDebugString(" added\n");
#endif
      
      pCombo->AddString(strTable[count]);
      pCombo->SetItemData(count,  count);
      if (count == currentsetting)
         pCombo->SetCurSel(count);
   }
} // FillComboBox

// -------------------------------------- SetAddressFormatHex -----------------
// call the contained control
void CMOD_simDlg::SetAddressFormatHex(BOOL formatHEX /*= TRUE*/) 
{
   m_listCtrlData.SetAddressFormatHex(formatHEX);
}

// -------------------------------------- IsAddressFormatHex -------------------
// call the contained control
BOOL CMOD_simDlg::IsAddressFormatHex() 
{
   return (m_listCtrlData.IsAddressFormatHex());
}

// -------------------------------- SetupMySystemMenu --------------------------
// Just populate the little pop-up system menu, along with some other misc. stuff
//
void CMOD_simDlg::SetupMySystemMenu(CMenu *pSysMenu)
{
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	if (pSysMenu != NULL)
	{
   CString strAboutMenu, strOnTop, strEmulation;
   CString strToggle, strReadOnly;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
      strOnTop.LoadString(IDS_MNU_ONTOP);
      strEmulation.LoadString(IDS_EMULATION);

		if (!strAboutMenu.IsEmpty())
		{
         // about menu
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
         //always on top
		   pSysMenu->AppendMenu(MF_STRING, IDM_ONTOP, strOnTop);
         pSysMenu->CheckMenuItem(IDM_ONTOP, MF_UNCHECKED);

         // emu. settings
		   pSysMenu->AppendMenu(MF_STRING, IDM_EMULATE, strEmulation);
         pSysMenu->CheckMenuItem(IDM_EMULATE, MF_UNCHECKED);
         // Vinay
         strToggle.LoadString(IDC_CSVIMPORT);
         pSysMenu->AppendMenu(MF_STRING, IDM_CSVIMPORT, strToggle);

         
         strReadOnly.LoadString(IDM_READONLY);
         pSysMenu->AppendMenu(MF_STRING, IDM_READONLY, strReadOnly);
      
         m_portToggleMnuIndex = 8;  // index of the port open/close item
         // The window transparency code for XP and 2000 users comes in here....
         GetTNImport();
         if (NULL != m_pSetLayeredWindowAttributes)
         {
         CString alphaMenu;
		      alphaMenu.LoadString(IDS_ALPHA);
		      if (!alphaMenu.IsEmpty())
		      {
   			   pSysMenu->AppendMenu(MF_SEPARATOR);
			      pSysMenu->AppendMenu(MF_STRING, IDM_ALPHA, alphaMenu);
		      }
            // up-down blend setting not working at this time
		      alphaMenu.LoadString(IDS_ALPHA_60);
		      if (!alphaMenu.IsEmpty())
		      {
			      pSysMenu->AppendMenu(MF_STRING, IDM_ALPHA_60, alphaMenu);
		      }
		      alphaMenu.LoadString(IDS_ALPHA_70);
		      if (!alphaMenu.IsEmpty())
		      {
			      pSysMenu->AppendMenu(MF_STRING, IDM_ALPHA_70, alphaMenu);
		      }      
            m_portToggleMnuIndex+=3; // index of the port open/close item
         }
         alphaBlendON = FALSE;
         alphaBlendFactor = 70;
         // port open/close menu item
         strToggle.LoadString(IDS_PORTTOGGLE);
         pSysMenu->AppendMenu(MF_STRING, IDM_TOGGLEPORT, strToggle);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIconMB, TRUE);			// Set big icon
	SetIcon(m_hIconMB, FALSE);		   // Set small icon

} // SetupMySystemMenu


// ------------------------------ SetupMyToolBar ------------------------------
// populate the little toolbar with the controls and bitmaps
void CMOD_simDlg::SetupMyToolBar()
{
BOOL  ret;

   // TOOLBAR 
   // Tooltips for the buttons are handled by a MSG handler
   CWindowRect toolRect(GetDlgItem(IDB_TOOLBARID));
   ScreenToClient(toolRect);
   CRect borders(0,0,0,0);
   ret = m_ToolBar.CreateEx(this, /*TBSTYLE_FLAT*/TBSTYLE_BUTTON|TBSTYLE_TOOLTIPS, WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS, borders, IDB_TOOLBARID);
   
   m_ToolBar.LoadBitmap(IDR_TOOLBAR1);
   m_ToolBar.SetButtons(NULL, 11);
                       // BN position, control ID, bitmap index
   m_ToolBar.SetButtonInfo(0, IDC_SAVE,      TBBS_BUTTON, 0);
   m_ToolBar.SetButtonInfo(1, IDC_LOAD,      TBBS_BUTTON, 1);
   m_ToolBar.SetButtonInfo(2, IDC_ZEROES,    TBBS_BUTTON, 2);
   m_ToolBar.SetButtonInfo(3, IDC_SETTINGS,  TBBS_BUTTON, 3);
   m_ToolBar.SetButtonInfo(4, IDC_SIMULATE,  TBBS_BUTTON, 4);
   m_ToolBar.SetButtonInfo(5, IDC_NOISE,  TBBS_BUTTON, 10);       //bitmaps 10,11

   m_ToolBar.SetButtonInfo(6, ID_SEPARATOR,  TBBS_SEPARATOR , 5);  // seperator
   m_ToolBar.SetButtonInfo(7, IDS_ABOUTBOX,  TBBS_BUTTON, 5);


   if (NULL != m_pSetLayeredWindowAttributes)
      // HF fixed index of bitmaps  m_ToolBar.SetButtonInfo(8, IDB_ALPHABLEND,  TBBS_BUTTON, 8);   //bitmaps 7,8 & 9
      m_ToolBar.SetButtonInfo(8, IDB_ALPHABLEND,  TBBS_BUTTON, 7);   //bitmaps 7,8 & 9
   else // pleb O/S's just get a seperator not a button
      m_ToolBar.SetButtonInfo(8, ID_SEPARATOR,  TBBS_SEPARATOR , 5);    // seperator
   m_ToolBar.SetButtonInfo(9, IDC_EMULATION,      TBBS_BUTTON, 12);
   //m_ToolBar.SetButtonInfo(10, ID_SEPARATOR,  TBBS_SEPARATOR , 5);      // seperator
   m_ToolBar.SetButtonInfo(10, IDC_CLOSE,      TBBS_BUTTON, 13);
   
   //ret = m_ToolBar.LoadToolBar(IDR_TOOLBAR1);

   m_ToolBar.MoveWindow(EXPAND_RECT(toolRect));
} // SetupMyToolBar


static CResizer::CBorderInfo s_bi[] = {
   {IDC_SERVERSTATS,     {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight,IDC_MAIN, CResizer::eBottom}},// bottom

   {IDC_LAMPRX,          {CResizer::eFixed, IDC_SERVERSTATS, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_SERVERSTATS, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight,IDC_MAIN, CResizer::eTop}},// bottom
   {IDC_LAMPTX,          {CResizer::eFixed, IDC_SERVERSTATS, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_SERVERSTATS, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight,IDC_MAIN, CResizer::eTop}},// bottom

   {IDB_TOOLBARID,       {CResizer::eFixed, IDC_MAIN, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight,IDC_MAIN, CResizer::eTop}},// bottom

   {IDC_STATIC_1,        {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight,IDC_MAIN, CResizer::eBottom}},// bottom
   {IDC_VIEWHEX,         {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight, IDC_MAIN, CResizer::eBottom}},// bottom
   {IDC_VIEWDEC,         {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight, IDC_MAIN, CResizer::eBottom}},// bottom

   {IDC_STATIC_2,        {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight, IDC_MAIN, CResizer::eBottom}},// bottom
   {IDC_COMBOAREA,       {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight, IDC_MAIN, CResizer::eBottom}},// bottom
   
   {IDC_PLANTSIM,        {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight, IDC_MAIN, CResizer::eBottom}},// bottom
   {IDC_STATIC_3,        {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight, IDC_MAIN, CResizer::eBottom}},// bottom
   {IDC_DATAFORMAT,      {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight, IDC_MAIN, CResizer::eBottom}},// bottom

   {IDC_CLONE,           {CResizer::eFixed, IDC_MAIN, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight,IDC_MAIN, CResizer::eBottom}},// bottom
   // protocol selection
   {IDC_STATIC_4,        {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight, IDC_MAIN, CResizer::eBottom}},// bottom
   {IDC_PROTOCOL,        {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eProportional, IDC_CLONE, CResizer::eRight},  // right side
                         {CResizer::eHeight, IDC_MAIN, CResizer::eBottom}},// bottom

   // register data view
   {IDC_DATALIST,        {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eFixed, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom}},// bottom

   // RULER to determine control total width for the tick-boxes
   {IDC_RULER,           {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eProportional, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight, IDC_MAIN, CResizer::eBottom}},// bottom

   {IDC_STATION00,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION01,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION02,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION03,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION04,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION05,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION06,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION07,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION08,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION09,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION10,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION11,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION12,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION13,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION14,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION15,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION16,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION17,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION18,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION19,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION20,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION21,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION22,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION23,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION24,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   {IDC_STATION25,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side  
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top              
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side  
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom               
   // start of 2nd ROW of boxes
   {IDC_STATION26,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION27,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION28,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION29,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION30,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION31,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION32,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION33,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION34,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION35,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION36,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION37,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION38,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION39,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION40,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION41,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION42,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION43,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION44,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION45,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION46,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION47,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION48,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION49,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top            
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom             
   {IDC_STATION50,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom
   {IDC_STATION51,       {CResizer::eProportional, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top
                         {CResizer::eProportional, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eFixed,IDC_MAIN, CResizer::eBottom}},// bottom
   

   {IDC_VSCROLLSTATIONS, {CResizer::eFixed, IDC_RULER, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top
                         {CResizer::eFixed, IDC_RULER, CResizer::eRight},  // right side
                         {CResizer::eHeight,IDC_MAIN, CResizer::eBottom}},// bottom

   //comms debug view
   {IDC_COMMSLIST,       {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eFixed, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom}},// bottom

   {IDC_TRAININGSIM,     {CResizer::eFixed, IDC_MAIN, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight,IDC_MAIN, CResizer::eBottom}},// bottom
   {IDB_TOGGLEDISP,      {CResizer::eFixed, IDC_MAIN, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight,IDC_MAIN, CResizer::eBottom}},// bottom
   {IDC_PAUSECOMMS,      {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight,IDC_MAIN, CResizer::eBottom}},// bottom
   {IDC_TRACKCOMMS,      {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight,IDC_MAIN, CResizer::eBottom}},// bottom
   {IDC_CLEARCOMMS,      {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight,IDC_MAIN, CResizer::eBottom}},// bottom
   {IDC_SHOWCOMMSTIME,      {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight,IDC_MAIN, CResizer::eBottom}},// bottom
   // modem status lamps
   {IDC_LAMPCTS,         {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight,IDC_MAIN, CResizer::eBottom}},// bottom
   {IDC_LAMPDSR,         {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight,IDC_MAIN, CResizer::eBottom}},// bottom
   {IDC_LAMPRING,        {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eTop},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight,IDC_MAIN, CResizer::eBottom}},// bottom

   {IDC_DRAG,            {CResizer::eFixed, IDC_MAIN, CResizer::eRight},   // left side
                         {CResizer::eFixed, IDC_MAIN, CResizer::eBottom},    // top
                         {CResizer::eWidth, IDC_MAIN, CResizer::eRight},  // right side
                         {CResizer::eHeight, IDC_MAIN, CResizer::eBottom}} // bottom
};


// --------------------------------------- OnInitDialog ------------------------
BOOL CMOD_simDlg::OnInitDialog()
{
CString tickText;
BOOL ret;

	CDialog::OnInitDialog();

   CSplashWnd::EnableSplashScreen(TRUE);
   CSplashWnd::m_splashShowDelay = 1;
   CSplashWnd::ShowSplashScreen(this); 	
      
   // start preparing the window-sizer
   m_dragCorner.SubclassDlgItem(IDC_DRAG, this);
   m_dragCorner.SetCornerType(CDragSizerBmp::CORNER_BOTTOMRIGHT, IDB_DRAGSE, TRUE);

   CMenu* pSysMenu = GetSystemMenu(FALSE);
   SetupMySystemMenu(pSysMenu);
	
   const int nSize = sizeof(s_bi)/sizeof(s_bi[0]);
   //Second we initialize resizer with this array. 
   //At the same time it will store the original controls positions.
   m_resizer.Init(m_hWnd, NULL, s_bi, nSize);

   m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), 
                                 m_lpszTemplateName); 
   ASSERT(m_hAccel);
   // load user settings from registry
   LoadApplicationSettings();

   // parse the command-line
   m_commandLine.SetPortParams(m_baud, m_byteSize, m_parity, m_stopBits, m_rts); // pass serial settings in
   AfxGetApp()->ParseCommandLine( this->m_commandLine );

   m_commandLine.GetProtocol(m_selectedProtocol);
   // get any serial settings that changed on the commandline.
   m_commandLine.GetPortSettings(m_portNameShort, m_baud, m_byteSize, m_parity, m_stopBits, m_rts);
   m_commandLine.GetIPSettings(m_localPort);

   SaveApplicationSettings();

   // set mem sizes
   PLCMemory.SetSize(GetNumMemoryAreas());
   SetMemAreaSizes();
   m_listCtrlData.SubclassDlgItem(IDC_DATALIST, this);
   // set display to Hex
   SetAddressFormatHex(m_appSettingAdressHex);
   CheckRadioButton(IDC_VIEWHEX, IDC_VIEWDEC, (IsAddressFormatHex() ? IDC_VIEWHEX : IDC_VIEWDEC));

   if (m_enableHTMLGUI) // start up the GUI server thread
   {
      // start the GUI
      m_htmlOutput.Watch(m_HTMLUpdateRate, m_inputHTMLFilename, m_outputHTMLFilename);
   }


   // set protocol selection in the list-box
   m_protocolCombo.SetCurSel(m_selectedProtocol);

   //IDC_DATAFORMAT
   FillCharCBox(&m_dataFormatCombo, PLCDataFormatsTable, &ViewerFormatNames[1], sizeof(PLCDataFormatsTable)/sizeof(PLCDataFormatsTable[0]), m_listCtrlData.GetViewFormat());

   m_vScrollBarStations.SetScrollRange(0, MAX_SCROLLINDEX, FALSE);
   // set up the tick-boxes
   for (int stationIndex=0;stationIndex<STATIONTICKBOXESMAX;stationIndex++) 
   {
      //tickText.Format("%02d", stationIndex);
      if (stationIndex < STATIONTICKBOXES)
      {
         ret = m_microTicks[stationIndex].SubclassDlgItem(IDC_STATION00+stationIndex, this);
         //colors
         m_microTicks[stationIndex].SetCheckedColor(m_mTickColorEnabled);
         m_microTicks[stationIndex].SetUnCheckedColor(m_mTickColorDisabled);
         m_microTicks[stationIndex].SetActiveColor(m_mTickColorActive);
         m_microTicks[stationIndex].SetInactiveColor(m_mTickColorInactive);

         m_microTicks[stationIndex].SetTextColor(m_mTickColorText);
         m_microTicks[stationIndex].SetUnCheckedTextColor(m_mTickDisColorText);
         m_microTicks[stationIndex].SetBorderWidth(2);

      }
      
      m_microTicksCountDown[stationIndex] = 0;        // tick-box coloring
      m_microTicksBackState[stationIndex] = FALSE;    // inactive/recently active
      m_microTicksEnableState[stationIndex] = (m_startAsOff ? FALSE : TRUE);
   }

   // RX/TX diodes
   m_lampRX.SubclassDlgItem(IDC_LAMPRX ,this);
   m_lampTX.SubclassDlgItem(IDC_LAMPTX ,this);
   m_lampTraining.SubclassDlgItem(IDC_PLANTSIM, this);
   m_lampCTS.SubclassDlgItem(IDC_LAMPCTS, this);
   m_lampDSR.SubclassDlgItem(IDC_LAMPDSR, this);
   m_lampRING.SubclassDlgItem(IDC_LAMPRING, this);


   m_lampRX.SetupLamp(500);
   m_lampTX.SetupLamp(500);

   m_lampTraining.SetONcolor(RGB(0,200,0)); // set the ON color for this lamp to be GREEN
   m_lampTraining.SetOFFcolor(RGB(0,110,0));

   SetupMyToolBar();

   // start the comms side
   InitiateCommsProcessors(m_selectedProtocol);
   OnSelchangeProtocol();  // update icon

   OnSelchangeComboArea();

   //TOOLTIPS START
   m_ToolTip.Create (this);
   m_ToolTip.Activate (TRUE);

   CWnd*    pWnd = GetWindow (GW_CHILD);
   while (pWnd)
   {
       int nID = pWnd->GetDlgCtrlID ();
       if (nID != -1)
       {
           m_ToolTip.AddTool (pWnd, pWnd->GetDlgCtrlID ());
       }
       pWnd = pWnd->GetWindow (GW_HWNDNEXT);
   }
   //TOOLTIPS END

   ConfigureStationButtons();


   // Animation and updates timer
   SetTimer(0, 500, NULL);
	SetTimer(3, 30000,0); // 30 seconds
   PostMessage(WM_TIMER,3);


   // load registers if we have to now.
   if (m_autoLoad)
      PostMessage(wm_LoadRegisters); //calls OnLoad();

   SetDisplayToggleButton();

   SetDlgItemText(IDC_TRACKCOMMS, (m_commsTobottom? "Stop tracking" : "Track comms"));
   SetDlgItemText(IDC_PAUSECOMMS, (m_paused? "Resume" : "Pause"));



   { // position our window
   CString registryPath;
   DRegKey key;
   CRect rect;
   DWORD x=0,y=0;

      m_appSettingAdressHex = m_listCtrlData.IsAddressFormatHex();
      registryPath = APPREGISTRY_SETTINGSKEY;
      key.Open(DRegKey::current_user, registryPath);
      if (ERROR_SUCCESS != key.QueryValue("WindowPositionX", &x))	// gracefull upgrade
	  {
		  // re-open and retry using local-machine
		  key.Close();
		  key.Open(DRegKey::local_machine, registryPath);
		  key.QueryValue("WindowPositionX", &x);
	  }
      key.QueryValue("WindowPositionY", &y);//(rect.top+ rect.bottom<<16)); 
      rect.left = x & 0xFFFF;
      rect.right = x >> 16;
      rect.top = y & 0xFFFF;
      rect.bottom = y >> 16;

      if (x && y)
      {
      int cx,cy;
         CWnd *pTop = FindWindow(SIMULATOR_WINDOW_CLASSNAME, NULL);
         cx = GetSystemMetrics(SM_CXFULLSCREEN);
         cy = GetSystemMetrics(SM_CYFULLSCREEN);

         if ((rect.left >=0) && (rect.right <= cx) && (rect.top >=0) && (rect.bottom <= cy))
            SetWindowPos(&CWnd::wndTop, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, SWP_SHOWWINDOW);

      }
   }
     

	return TRUE;  // return TRUE  unless you set the focus to a control
} // OnInitDialog


// ----------------------------------- SetMemAreaSizes -------------------------
// set up how many bits/registers each modbus type has got
void CMOD_simDlg::SetMemAreaSizes()
{
   switch (m_selectedProtocol)
   {
      case PROTOCOL_SELMOD232:
      case PROTOCOL_SELMODETH:
         PLCMemory[0].SetSize(this->m_numOutputs);
         PLCMemory[1].SetSize(this->m_numInputs);
         PLCMemory[2].SetSize(this->m_numAnalogs);
         PLCMemory[3].SetSize(this->m_numHolding);
         PLCMemory[4].SetSize(this->m_numExtended);
         FillComboBox(IDC_COMBOAREA, PLCMemoryTypes, m_startUpRegisterArea);//m_listCtrlData.GetAreaDisplayed());
         {
            int count = ((CComboBox*)GetDlgItem(IDC_COMBOAREA))->GetCurSel();
            if (-1 == count)
               ((CComboBox*)GetDlgItem(IDC_COMBOAREA))->SetCurSel(0);
         }
         break;
      case PROTOCOL_SELABMASTER232:
      case PROTOCOL_SELAB232:
         {
            int i, max = GetNumMemoryAreas();
            for (i=0; i < max; i++)
               PLCMemory[i].SetSize(255);

            // init the memory
            if (PROTOCOL_SELABMASTER232 == m_selectedProtocol)
            {
               for (i=0; i < max; i++)
               {
                  PLCMemory.SetAt(i, 1, JOY_MINROOFPRESSURE);
                  PLCMemory.SetAt(i,2, JOY_MINROOFPRESSURE);
                  PLCMemory.SetAt(i,6, JOY_12VOLTS);
               }
            }
            // fill a combo box with AB file #/names
         LONG count;
         CComboBox *pCombo = ((CComboBox*)GetDlgItem(IDC_COMBOAREA));
         CString entry;
         BOOL noSel = TRUE;

            pCombo->ResetContent();
            for (count = 0; count < max; count++)
            {
               entry.Format("File %d", count);
               pCombo->AddString(entry);
               pCombo->SetItemData(count,  count);
               if (count == m_listCtrlData.GetAreaDisplayed())
                  pCombo->SetCurSel(count);
            }
            count = pCombo->GetCurSel();
            if (-1 == count)
               pCombo->SetCurSel(0);
         }
         break;
   }
   //populate the LB as well
}


// ---------------------------------- SetupListViewControl ---------------------
// PURPOSE; Set up the list-view control headdings and col. widths
// NOTES;
// Data on the control comes in via a callback func
void CMOD_simDlg::SetupListViewControl(const DWORD cols)
{

   m_isClone = IsDlgButtonChecked(IDC_CLONE);
   m_listCtrlData.SetupColumns(LONG (cols+1), IsClone());
} // SetupListViewControl



// ---------------------------- LoadApplicationSettings -----------------
void CMOD_simDlg::LoadApplicationSettings()
{
CString registryPath;
DRegKey key;
DWORD len = sizeof(m_portNameShort);
DWORD animationType;

   // 1. set default values
   strcpy_s(m_portNameShort, MAX_COMPORT_NAME, "COM1");
   m_baud = CBR_9600;
   m_byteSize = 8;
   m_parity = NOPARITY;
   m_stopBits = ONESTOPBIT;
   m_rts = RTS_CONTROL_ENABLE;

   // colors
   m_mTickColorActive =   RGB(0xFF, 0xFF, 0x00);   // yellow
   m_mTickColorInactive = RGB(0x80, 0x80, 0x80);   // gray
   m_mTickColorEnabled =  RGB(0x00, 0x80, 0x00);   // green
   m_mTickColorDisabled = RGB(0xFF, 0x00, 0x00);   // red
   m_mTickColorText =     RGB(0x00, 0x00, 0x00);   // black
   m_mTickDisColorText =  RGB(0x00, 0x00, 0x00);   // black

   m_appSettingAdressHex = FALSE;
   m_numInputs  = 65535;
   m_numOutputs = 65535;
   m_numHolding = 65535;
   m_numExtended= 65535;
   m_numAnalogs = 65535;
   m_PDUSize = 2048; // 4096 bytes max
   m_numSeperate = 16;
   m_seperateRegisters = FALSE;

   m_selectedProtocol = PROTOCOL_SELMODETH;  // default protocol
   m_isClone = FALSE;
   m_startUpRegisterArea = 3;

   m_localPort = 502;
   m_otherPort = 501;
   m_numServers = 10;
   m_linger = FALSE;
   m_localId = 1;

   m_animationON = FALSE;
   m_plantAnimation = FALSE;
   m_animationBYTE = FALSE;
   m_animationWORD = FALSE;
   // Vinay
   m_importFolder = "CSVdata";
   m_logFileName = "csreport.log";
   m_csvImportEnable = FALSE;

   //////////////////////////////////////////////////
   // 2. load the settings, baud rate etc
   registryPath = APPREGISTRY_SETTINGSKEY;

   key.Open(DRegKey::local_machine, registryPath);
   if (ERROR_SUCCESS != key.QueryValue("Port", m_portNameShort, &len))
   {
		key.Close();
      key.Open(DRegKey::current_user, registryPath);
      key.QueryValue("Port", m_portNameShort, &len);
   }
   key.QueryValue("BaudRate",    &m_baud);
   key.QueryValue("ByteSize",    &m_byteSize);
   key.QueryValue("Parity",      &m_parity);
   key.QueryValue("StopBits",    &m_stopBits);
   key.QueryValue("RTS",         &m_rts);

   key.QueryValue("ResponseDelays",    &m_responseDelay);
   key.QueryValue("MOSCAD",            &m_MOSCADchecks);
   key.QueryValue("AutoLoad",          &m_autoLoad);
   key.QueryValue("DevicesOfflineAtStart", &m_startAsOff);
   key.QueryValue("ModifyThenRespond", &m_modifyThenRespond);
   key.QueryValue("DisableWrites",     &m_disableWrites);
   key.QueryValue("PLCMaster",         &m_PLCMaster);
   key.QueryValue("UseBCCs",           &m_useBCC);
   // JOY
   key.QueryValue("ABMasterSource",    &m_ABMasterSourceStation);
   key.QueryValue("ABMasterDest",      &m_ABMasterDestStation);
   key.QueryValue("ABMasterFiles",     &m_ABMasterNumFiles);
   key.QueryValue("ABMasterRun",       &m_ABMasterRunMode);
   key.QueryValue("ABMasterRead",      &m_ABMasterReadVariables);
   key.QueryValue("ABMasterWrite",     &m_ABMasterWriteVariables);
   key.QueryValue("ABmasterIDLETime",  &m_ABmasterIDLETime);
   key.QueryValue("ABtimeoutValue",    &m_ABtimeoutValue);

   key.QueryValue("TickColorActive",   &m_mTickColorActive);
   key.QueryValue("TickColorInactive", &m_mTickColorInactive);
   key.QueryValue("TickColorEnabled",  &m_mTickColorEnabled);
   key.QueryValue("TickColorDisabled", &m_mTickColorDisabled);
   key.QueryValue("TickColorText",     &m_mTickColorText);
   key.QueryValue("TickDisabledColorText",  &m_mTickDisColorText);

   key.QueryValue("AddressHex",        &m_appSettingAdressHex);
   key.QueryValue("AnimationON",       &m_animationPeriod);
   key.QueryValue("RegisterSel",       &m_startUpRegisterArea);

   key.QueryValue("InputRegisters",    &m_numInputs  );
   key.QueryValue("OutputRegisters",   &m_numOutputs );
   key.QueryValue("HoldingRegisters",  &m_numHolding );
   key.QueryValue("ExtendedRegisters", &m_numExtended);
   key.QueryValue("AnalogRegisters",   &m_numAnalogs );
   key.QueryValue("PDUSize",           &m_PDUSize);

   key.QueryValue("Protocol",          &m_selectedProtocol);
   key.QueryValue("EthernetServers",   &m_numServers);
   key.QueryValue("LingerClose",       &m_linger);
   key.QueryValue("EthernetID",        &m_localId);
   key.QueryValue("EthernetPort",      &m_localPort);
   key.QueryValue("EthernetPort2",     &m_otherPort);

   key.QueryValue("AnimationIncrement", &m_animationIncValue);
   key.QueryValue("AnimationPeriod", &m_animationPeriod);

   key.QueryValue("RegistrationUserName", m_registeredUser);
   key.QueryValue("RegistrationKey",      m_registeredKey);
   key.SetValue("RegistrationUserName",   m_registeredUser);
   key.SetValue("RegistrationKey",        m_registeredKey);

   key.QueryValue("RegistersPerStation", &m_numSeperate);
   key.QueryValue("RegistersSeperated", &m_seperateRegisters);
   // HTML GUI parameters
   key.QueryValue("EnableHTMLGUI", &m_enableHTMLGUI);
   key.QueryValue("HTMLUpdateRate", &m_HTMLUpdateRate);
   key.QueryValue("InputHTMLFilename", m_inputHTMLFilename);
   key.QueryValue("OuputHTMLFilename", m_outputHTMLFilename);


   if (!m_registration.CheckRegistrationKey(m_registeredUser, m_registeredKey))
      CSplashWnd::SetWaitCount(9); // splash screen stays up longer if U not registered
   else
      CSplashWnd::GetSWnd()->InvalidateRect(NULL); // Force a splash re-paint with registration info

   key.QueryValue("AnimationType", &animationType);
   switch (animationType)
   {
      case 1: m_animationBYTE = TRUE;
         break;
      case 2: m_animationWORD = TRUE;
         break;
      case 3: m_plantAnimation = TRUE;
         break;
      default:
         break;
   }
   if (m_animationBYTE || m_animationWORD || m_plantAnimation)
      m_animationON = TRUE;
   key.QueryValue("AnimationScript", m_animationScriptFile);

   //Vinay
   key.QueryValue("CSVImportFolder", m_importFolder);
   key.QueryValue("CSVImportLogName", m_logFileName);
   key.QueryValue("CSVImportEnable", &m_csvImportEnable);


} // LoadApplicationSettings


// ---------------------------- SaveApplicationSettings -----------------
// save program settings to HKEY_LOCAL_MACHINE registry
void CMOD_simDlg::SaveApplicationSettings()
{
CString registryPath;
DRegKey key;
DWORD animationType;

   m_appSettingAdressHex = m_listCtrlData.IsAddressFormatHex();
   // Save the baud rate etc
   registryPath = APPREGISTRY_SETTINGSKEY;
   // for save, writting to current user
   key.Open(DRegKey::current_user, registryPath);
   if (ERROR_SUCCESS != key.SetValue("Port", m_portNameShort, strlen(m_portNameShort)))
   {
      pGlobalDialog->AddCommsDebugString("unable to save reg. values.");
   }
   
   key.SetValue("BaudRate", m_baud);
   key.SetValue("ByteSize", m_byteSize);
   key.SetValue("Parity", m_parity);
   key.SetValue("StopBits", m_stopBits);
   key.SetValue("RTS", m_rts);
   key.SetValue("ResponseDelays", m_responseDelay);
   key.SetValue("MOSCAD", m_MOSCADchecks);
   key.SetValue("AutoLoad", m_autoLoad);
   key.SetValue("DevicesOfflineAtStart", m_startAsOff);
   key.SetValue("ModifyThenRespond", m_modifyThenRespond);
   key.SetValue("DisableWrites", m_disableWrites);
   key.SetValue("PLCMaster", m_PLCMaster);
   key.SetValue("UseBCCs", m_useBCC);
   
   key.SetValue("ABMasterSource",m_ABMasterSourceStation);
   key.SetValue("ABMasterDest",  m_ABMasterDestStation);
   key.SetValue("ABMasterFiles", m_ABMasterNumFiles);
   key.SetValue("ABMasterRun",   m_ABMasterRunMode);
   key.SetValue("ABMasterRead",  m_ABMasterReadVariables);
   key.SetValue("ABMasterWrite", m_ABMasterWriteVariables);
   key.SetValue("ABmasterIDLETime", m_ABmasterIDLETime);
   key.SetValue("ABtimeoutValue", m_ABtimeoutValue);

   key.SetValue("TickColorActive",     m_mTickColorActive);
   key.SetValue("TickColorInactive",   m_mTickColorInactive);
   key.SetValue("TickColorEnabled",    m_mTickColorEnabled);
   key.SetValue("TickColorDisabled",   m_mTickColorDisabled);
   key.SetValue("TickColorText",       m_mTickColorText);
   key.SetValue("TickDisabledColorText", m_mTickDisColorText);

   key.SetValue("AddressHex",  m_appSettingAdressHex);
   key.SetValue("AnimationON", m_animationPeriod);
   key.SetValue("RegisterSel", m_startUpRegisterArea);

   key.SetValue("InputRegisters",    m_numInputs);
   key.SetValue("OutputRegisters",   m_numOutputs);
   key.SetValue("HoldingRegisters",  m_numHolding);
   key.SetValue("ExtendedRegisters", m_numExtended);
   key.SetValue("AnalogRegisters",   m_numAnalogs);
   key.SetValue("PDUSize",   m_PDUSize);

   key.SetValue("Protocol", m_selectedProtocol);
   key.SetValue("EthernetServers", m_numServers);
   key.SetValue("LingerClose", m_linger);
   key.SetValue("EthernetID", m_localId);

   key.SetValue("EthernetPort", m_localPort);
   key.SetValue("EthernetPort2", m_otherPort);

   key.SetValue("AnimationIncrement", m_animationIncValue);
   key.SetValue("AnimationPeriod", m_animationPeriod);
   animationType = 0;
   if (m_animationON)
   {
      if (m_animationBYTE) animationType = 1;
      if (m_animationWORD) animationType = 2;
      if (m_plantAnimation) animationType = 3;
   }
   key.SetValue("AnimationType", animationType);
   key.SetValue("AnimationScript", m_animationScriptFile);

   key.SetValue("RegistersPerStation", m_numSeperate);
   key.SetValue("RegistersSeperated", m_seperateRegisters);
   // HTML GUI settings
   key.SetValue("EnableHTMLGUI", m_enableHTMLGUI);
   key.SetValue("HTMLUpdateRate", m_HTMLUpdateRate);
   key.SetValue("InputHTMLFilename", m_inputHTMLFilename);
   key.SetValue("OuputHTMLFilename", m_outputHTMLFilename);

   key.SetValue("CSVImportFolder", m_importFolder);
   key.SetValue("CSVImportLogName", m_logFileName);
   key.SetValue("CSVImportEnable", m_csvImportEnable);

} // SaveApplicationSettings


// -------------------------- OnSysCommand -----------------------------
// Handle the "system menu"
void CMOD_simDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
   switch (nID & 0xFFF0) 
   {
   case ID_EDIT_COPY:
      OnAboutboxBn();
      break;
   case IDM_ABOUTBOX:
      OnAboutboxBn();
      break;
   case IDM_ALPHA_60:
      SetAlphaBlendLow();
      break;
   case IDM_ALPHA_70:
      SetAlphaBlendHigh();
      break;
   case IDM_ALPHA:
      SetAlphaBlendSolid();
      break;
   case IDM_ONTOP :
      ToggleOnTop();
      break;
   case IDM_EMULATE :
      OnEmulation();
      break;
   case IDM_TOGGLEPORT:
      OnTogglePortState();
      break;
      // Vinay
   case IDM_CSVIMPORT:
      OnCsvImportPop();
      break;
   case IDM_READONLY:
      {
         CWnd *start = GetWindow(GW_CHILD);
         int b = !start->IsWindowEnabled();

         CWnd *next = start;
         do {
            next->EnableWindow(b);
            next = next->GetNextWindow();
         }
         while (( next !=start) && (next));
         // turn menu items on/off
         CMenu* mnu = this->GetSystemMenu(FALSE);
         if (b)
         {
            mnu->InsertMenu(0, MF_BYPOSITION | MF_GRAYED, SC_RESTORE , "Restore");
            mnu->InsertMenu(1, MF_BYPOSITION, SC_MOVE , "Move");
            mnu->InsertMenu(2, MF_BYPOSITION,SC_SIZE ,  "Size");
            mnu->InsertMenu(3, MF_BYPOSITION, SC_MINIMIZE, "Minimize");
            mnu->InsertMenu(4, MF_BYPOSITION, SC_MAXIMIZE, "Maximize");
            mnu->InsertMenu(5, MF_BYPOSITION, MF_SEPARATOR );

            mnu->InsertMenu(5, MF_BYPOSITION, SC_CLOSE, "Close" );
            mnu->InsertMenu(5, MF_BYPOSITION, MF_SEPARATOR );
         }
         else
         {
            //mnu->DeleteMenu(5, MF_BYPOSITION);
            mnu->DeleteMenu(SC_CLOSE, MF_BYCOMMAND );
            mnu->DeleteMenu(SC_RESTORE ,MF_BYCOMMAND );
            mnu->DeleteMenu(SC_MOVE ,MF_BYCOMMAND );
            mnu->DeleteMenu(SC_SIZE ,MF_BYCOMMAND );
            mnu->DeleteMenu(SC_MINIMIZE,MF_BYCOMMAND );
            mnu->DeleteMenu(SC_MAXIMIZE ,MF_BYCOMMAND );
            mnu->DeleteMenu(0, MF_BYPOSITION);
            mnu->DeleteMenu(0, MF_BYPOSITION);
            WINDOWPLACEMENT wndpl;
            GetWindowPlacement( &wndpl );
            wndpl.showCmd = SW_MINIMIZE;
            SetWindowPlacement( &wndpl );

         }
      }
      break;

   default:
      CDialog::OnSysCommand(nID, lParam);
      break;
   }
} // OnSysCommand


// --------------------------------- OnPaint --------------------------------
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CMOD_simDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIconMB);
	}
	else
	{
		CDialog::OnPaint();
	}
} // OnPaint


// ------------------------------- OnQueryDragIcon --------------------------
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMOD_simDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIconMB;
}


// -------------------------------- UnloadCommsProcessors ----------------------
// Stop and load all comms processors, there is only 1, but can be many if on ethernet
void CMOD_simDlg::UnloadCommsProcessors()
{
CWaitCursor wait;    // put up a wait cursor

   //
   AddCommsDebugString("Stopping server(s)");
   m_Unloading = TRUE;
   switch (m_loadedProtocol)
   {
      case PROTOCOL_SELMOD232 :
      case PROTOCOL_SELABMASTER232 :
      case PROTOCOL_SELAB232 :
         // close all the ports
         if (NULL != m_pServerRS232Array)
         {
            m_pServerRS232Array->CloseAll();
            delete m_pServerRS232Array;
         }
         m_pServerRS232Array = NULL;
         break;
      case PROTOCOL_SELMODETH :
         // close all the sockets
         if (NULL != m_pServerSockArray)
         {
            m_pServerSockArray->kill = TRUE;
            m_pServerSockArray->CloseAll();
            delete m_pServerSockArray;
         }
         m_pServerSockArray = NULL;
         break;
      case PROTOCOL_SELNONE : // already unloaded, do nothing
         break;
      default:
         ASSERT(0);
         break;
   }
   m_loadedProtocol = PROTOCOL_SELNONE;
} // UnloadCommsProcessors


// -------------------------------- InitiateCommsProcessors ----------------------
// PURPOSE; start and initiate all comms processors (there is only 1 if serial)
// NOTES;
//     Maybe in future I will handle 2 serial ports or more in this program
//     We did that for Ethernet sockets, that is why this code is here.
//
void CMOD_simDlg::InitiateCommsProcessors(DWORD protocol)
{
CWaitCursor wait;    // put up a wait cursor to stop any user interaction with the window
CString csTitle;
CString csNewTitle, portName;


   AddCommsDebugString("Starting server(s)...");
   m_Unloading = FALSE;
   switch (protocol)
   {
      case PROTOCOL_SELMOD232 :
      case PROTOCOL_SELABMASTER232 :
      case PROTOCOL_SELAB232 :
         m_ServerRSState = RSPORTCURRENTLY_OPENING;
         m_pServerRS232Array = new CServerRS232Array();
         // Check port availability.
         if (!PortAvailable(m_portNameShort))
         {
         CString msg;
            msg.Format("Port '%s' not available, will try another port!", m_portNameShort);
            AfxMessageBox(msg, MB_OK|MB_ICONEXCLAMATION);
            GetFirstFreePort(m_portNameShort);
            m_ServerRSState = RSPORTCURRENTLY_VOID;
         }

         m_pServerRS232Array->AddServers(protocol,
                                         m_portNameShort, 
                                         m_baud, 
                                         m_byteSize, 
                                         m_parity, 
                                         m_stopBits, 
                                         m_rts,
                                         m_responseDelay, 
                                         m_MOSCADchecks,
                                         m_modifyThenRespond,
                                         m_disableWrites,
                                         m_PDUSize,
										           m_useBCC,
                                         m_ABMasterSourceStation,
                                         m_ABMasterDestStation,
                                         m_ABMasterNumFiles,
                                         m_ABMasterRunMode,
                                         m_ABMasterReadVariables,
                                         m_ABMasterWriteVariables,
                                         m_ABmasterIDLETime,
                                         m_ABtimeoutValue
                                        );
         //update title bar
         csTitle.LoadString(IDS_TITLE); //MODBUS %s PLC - Simulator (%s %s %d,%d,%c,%s)
         portName.Format("port: %s %d,%d,%c,%s,%s", m_portNameShort, 
                                              m_baud, 
                                              m_byteSize, 
                                              commsParityStr[m_parity], 
                                              commsStopStr[m_stopBits],
                                              commsRTSStr[m_rts]
                                              );
         //
         if (PROTOCOL_SELMOD232 == protocol)
            csNewTitle.Format(csTitle, ((CMOD232CommsProcessor*)m_pServerRS232Array->GetAt(0))->ProtocolName(),"RS-232", portName);
         else
         {
            portName += (((CAB232CommsProcessor*)m_pServerRS232Array->GetAt(0))->IsMaster()?" -Master":" -Slave");
            csNewTitle.Format(csTitle, ((CAB232CommsProcessor*)m_pServerRS232Array->GetAt(0))->ProtocolName(),"RS-232", portName);

         }
         break;
      case PROTOCOL_SELMODETH:
         m_pServerSockArray = new CServerSocketArray();
         m_pServerSockArray->AddServers(m_numServers, 
                                        m_localPort, 
                                        m_responseDelay, 
                                        m_linger, 
                                        m_MOSCADchecks,
                                        m_modifyThenRespond,
                                        m_disableWrites,
                                        m_PDUSize);
         //update title bar
         csTitle.LoadString(IDS_TITLE);
         portName.Format("port: %d", m_localPort);
         csNewTitle.Format(csTitle, ((CMODEthCommsProcessor*)m_pServerSockArray->GetAt(0))->ProtocolName(), "TCP/IP", portName);
         break;
      default: 
         ASSERT(0);
         break;
   }
   SetWindowText(csNewTitle);
   m_loadedProtocol = protocol;
} // InitiateCommsProcessors

CString &CMOD_simDlg::LogFileName()
{
   return(this->m_logFileName);
}


//----------------------------------------------------------------------------------------
// this 'callback' class implements the interface for setting registers and for debug strings
// Vinay


void CRegisterUpdaterIMP::DebugMessage(LPCTSTR message)
{
SYSTEMTIME  sTime;
CString fmt;

   GetLocalTime(&sTime);
   fmt.Format("%02d/%02d %02d:%02d:%02d - %s\r\n", sTime.wMonth, sTime.wDay, sTime.wHour, sTime.wMinute, sTime.wSecond, message);
   pDlg->AddCommsDebugString(message);
   // todo: log to the text file
   // CFile :: pDlg->m_logFileName
   CFile logFile;
   logFile.Open(pDlg->LogFileName(), CFile::modeWrite|CFile::modeNoTruncate|CFile::modeCreate);
#if     _MSC_VER > 1200	//VC 9
   if (logFile.m_hFile != INVALID_HANDLE_VALUE)
#else
   if (logFile.m_hFile != (DWORD)INVALID_HANDLE_VALUE)
#endif
   {
      logFile.SeekToEnd();
      logFile.Write(fmt, fmt.GetLength());
   }
}

BOOL CRegisterUpdaterIMP::SetRegister(LONG index, WORD value) 
{ 
   CMemWriteLock lk(PLCMemory.GetMutex());
   PLCMemory.SetAt(MODBUS_MEM_REGISTERS, index, value);

   return(true);
}

BOOL CRegisterUpdaterIMP::ModbusClone()
{
   return(pDlg->IsClone());
}

// end Vinay

// ------------------------------------ OnTimer ---------------------------------
void CMOD_simDlg::OnTimer(UINT nIDEvent) 
{
static int tickerCount=0;
int stationIndex, firstVisibleStation, lastVisibleStation;
static DWORD   lastTickCountOut=0, tickCount;

   // Vinay
   if (3== nIDEvent)
   {
      if ((m_csvImportEnable) &&  // CSV import only supported for modbus!
         ((m_selectedProtocol == PROTOCOL_SELMOD232 )||(m_selectedProtocol== PROTOCOL_SELMODETH)) ) 

      {
         CRegisterUpdaterIMP imp(this);
         if (m_CSVImporter.HandleTimer(m_importFolder, &imp))
         {
            // update modbus registers
            m_CSVImporter.UpdateRegisters();
         }
      }
      return;
   }

   tickerCount++;
   // update TX/RX lamps
   if (m_portActivityRX)
   {
      m_lampRX.SetState(TRUE);
      m_portActivityRX = 0;
   }
   if (m_portActivityTX)
   {
      m_lampTX.SetState(TRUE);
      m_portActivityTX = 0;
   }
   
   if (m_lampTraining.GetState() != (m_animationON & m_plantAnimation))
      m_lampTraining.SetState(m_animationON & m_plantAnimation);
   // update CTS,DSR and RING lamps
   m_lampCTS.SetState(m_portModemStatus & MS_CTS_ON);
   m_lampDSR.SetState(m_portModemStatus & MS_DSR_ON);
   m_lampRING.SetState(m_portModemStatus & MS_RING_ON);

   // update comms debugger
   UpdateCommsList();

   
   // Update active station thingy at bottom of the main window
   // update the tick-boxes
   EnterCriticalSection(&dispCritSection);

   if (0==tickerCount%5)
   {
      // if not registered ,then a Dialog appears
      m_registration.RegistrationReminder();

      firstVisibleStation = GetFirstVisibleStation();
      lastVisibleStation = GetLastVisibleStation();
      for (stationIndex=firstVisibleStation;stationIndex <lastVisibleStation; stationIndex++) 
      {
         ASSERTMICROBOX(stationIndex-firstVisibleStation)
         if (m_microTicks[stationIndex-firstVisibleStation].GetCheck() != m_microTicksEnableState[stationIndex])
         {
            m_microTicksEnableState[stationIndex] = !m_microTicksEnableState[stationIndex];
            //update tooltip
            ASSERTMICROBOX(stationIndex-firstVisibleStation)
            m_ToolTip.UpdateTipText(GetToolTipForStation(stationIndex), &m_microTicks[stationIndex-firstVisibleStation], 0);
         }
      }
   }

   // stations active tick-box array.
   for (stationIndex=0;stationIndex<STATIONTICKBOXESMAX;stationIndex++) 
   {
   BOOL invalidateCtl=FALSE;

      if (m_microTicksCountDown[stationIndex]>0)
      {
         // decrement the tick counter for that station
         if (!m_microTicksBackState[stationIndex])
            invalidateCtl = TRUE;
         m_microTicksCountDown[stationIndex]--;
         SetTickBorderState(stationIndex, TRUE);
         m_microTicksBackState[stationIndex] = TRUE;
      }
      else
      {
         if (m_microTicksBackState[stationIndex])
            invalidateCtl = TRUE;

         SetTickBorderState(stationIndex, FALSE);
         m_microTicksBackState[stationIndex] = FALSE;
      }
      if (invalidateCtl)
         InvalidateTick(stationIndex);
   }
   LeaveCriticalSection(&dispCritSection);

   UpdateStatusLine();
   tickCount = GetTickCount();   
   if (tickCount - lastTickCountOut > ANIMATION_ANTISTARVATION_MS)
   { // timer fires @500ms interval, if we took >450ms to service it, skip
      // do animations and value zeroing + execute simulation script
      DoAnimations();
      lastTickCountOut = GetTickCount();
   }
   CDialog::OnTimer(nIDEvent);
} // OnTimer

// ------------------------------ UpdateStatusLine ------------------------------
void CMOD_simDlg::UpdateStatusLine()
{
CString  statline;
LONG     conServers, totServers;

   // Figure out how many server sockets/ports are used/available
   totServers = 0;
   conServers = 0;
   if (m_pServerRS232Array)
   {
      totServers = m_pServerRS232Array->GetUpperBound()+1;
      conServers = m_pServerRS232Array->NumberConnected();
   }

   if (m_pServerSockArray)
   {
      totServers = m_pServerSockArray->GetUpperBound()+1;
      conServers = m_pServerSockArray->NumberConnected();
   }
   // also add in some info about how many packets were received/sent and the last state
   switch (m_loadedProtocol)
   {
      case PROTOCOL_SELMODETH:
         statline.Format("Connected (%d/%d) : (received/sent) (%ld/%ld) %s", 
                         conServers,
                         totServers,
                         m_PacketsReceived, 
                         m_PacketsSent, 
                         sockStateDescriptions[m_ServerSocketState]);
         break;
      case PROTOCOL_SELMOD232:
      case PROTOCOL_SELABMASTER232:
      case PROTOCOL_SELAB232:
         statline.Format("Connected (%d) : (received/sent) (%ld/%ld) %s", conServers, m_PacketsReceived, m_PacketsSent, 
                          stateDescriptions[m_ServerRSState]);
         break;
      case PROTOCOL_SELNONE :
         statline.Format("Unloaded, click open port to simulate");
         break;
      default:
         break;
   }
   if (m_MOSCADchecks)
      statline += " [MOSCAD]";
   SetDlgItemText(IDC_SERVERSTATS, statline);
}
  

//////////////////////////////////////////////////////////////////////////////////////
// Plant Simulation section
// The next bit of code performs PLC Simulation for training purposes
//
#include "plantsim.h"
#include "MOD_RSsimDlg.h"

#define getrandom(min,max) ((rand()%(int)(((max)+1)-(min)))+(min))

char *plantSim_registerNames[PS_LASTREGISTER+1] =
{
   "Plant I/O Controls    (r/w)",    // 0
   "Plant I/O feedback    (r)",
   "Mixer run-time        (r/w)",
   "Tank level            (r/w)",
   "Tank Max. level       (r/w)",
   "H20 Tank level        (r/w)",
   "H20 Tank max level    (r/w)",
   "Conc. Tank level      (r/w)",
   "Conc. Tank max level  (r/w)",
   "Tank temp. (x100)     (r/w)",
   "Temp. SP (for auto)   (r/w)",         //10
   "Heater current        (r)",
   "Heater eff.           (r)",
   "Time: (ms)            (r)",
   "Time: (sec)           (r)",
   "Time: (min)           (r)",
   "Time: (hour)          (r)",
   "Time: (day)           (r)",
   "Time:(weekday Sun=0)  (r)",
   "Random 0-65535        (r)"            // 19
};

float heatingEfficiencyTable[] = 
{
   1.0F,     // 100% (cold)
   0.98F,
   0.85F,
   0.73F,
   0.6F,
   0.5F,
   0.4F,
   0.2F,     
   0.1F,
   0.05F     // (hotter)
};

// ------------------------------------------- GetRegisterName ----------------------
void CMOD_simDlg::GetRegisterName(LONG area, LONG wordNumber, CString &registerName)
{
   if (MODBUS_MEM_REGISTERS == area)
   {
      if ((wordNumber <= PS_LASTREGISTER) && (wordNumber>=0))
         registerName = plantSim_registerNames[wordNumber];
   }
}


// ---------------------------------------- DoPlantSimulation ------------------------
void CMOD_simDlg::DoPlantSimulation()
{
DWORD valves, valveControls, alarms;
DWORD tankLevel, tankLevelMax, tankTemp, heaterCurrent, tempSetPoint, mixerDuration;
DWORD tank1Level, tank2Level;
DWORD controlMaskOffBits=0, controlMaskOnBits=0;
BOOL heaterON, elementCovered, input1, input2, drain, simReset, automatic, startMixer;
DWORD efficiency=0;
SYSTEMTIME  sysTime;
static DWORD mixerRunningStart = 0; // tick count



   {
   // lock the memory for writting
   CMemWriteLock lk(PLCMemory.GetMutex());

      // do the auto bit here
      valveControls = PLCMemory.GetAt(MODBUS_MEM_REGISTERS).GetAt(PS_CONTROLBITS);   // controls
      tankLevel     = PLCMemory.GetAt(MODBUS_MEM_REGISTERS).GetAt(PS_LEVEL);       // level
      tankLevelMax  = PLCMemory.GetAt(MODBUS_MEM_REGISTERS).GetAt(PS_MAXLEVEL);    // max level
      tank1Level    = PLCMemory.GetAt(MODBUS_MEM_REGISTERS).GetAt(PS_TANK1LEVEL);       // level
      tank2Level    = PLCMemory.GetAt(MODBUS_MEM_REGISTERS).GetAt(PS_TANK2LEVEL);       // level
      valves        = PLCMemory.GetAt(MODBUS_MEM_REGISTERS).GetAt(PS_FEEDBACKBITS);   // feedback
      tankTemp      = PLCMemory.GetAt(MODBUS_MEM_REGISTERS).GetAt(PS_TEMP);         // temp
      tempSetPoint  = PLCMemory.GetAt(MODBUS_MEM_REGISTERS).GetAt(PS_TEMPSETPOINT); // SP when in auto
      mixerDuration = PLCMemory.GetAt(MODBUS_MEM_REGISTERS).GetAt(PS_MIXERTIME);    // in milliseconds
   }
   input1 = (valveControls & PS_MASK_INPUT1);
   input2 = (valveControls & PS_MASK_INPUT2);
   drain = (valveControls & PS_MASK_DRAIN);
   heaterON = (valveControls & PS_MASK_HEATER);
   simReset = (valveControls & PS_MASK_RESET);
   automatic = (valveControls & PS_MASK_AUTO);
   startMixer = (valveControls & PS_STIRRER_START);

   elementCovered = (tankLevel > 200); // need 20 cm of liquid to cover the element, else it does nothing

   if (simReset)
   {
      controlMaskOffBits |=PS_MASK_RESET | PS_MASK_AUTO | PS_STIRRER_START| PS_MASK_HEATER;
      //valveControls = valveControls & (~PS_MASK_RESET);  // clear this bit
      //valveControls = valveControls & (~PS_MASK_AUTO);  // clear this bit too
      //valveControls = valveControls & (~PS_STIRRER_START);  // clear this bit too
      //valveControls = valveControls & (~PS_MASK_HEATER);  // clear this bit too
      tankTemp = AMBIENT;   // 21 degrees *100
      tankLevelMax = 2000;
   }
   else
   {
      ///////////////////////////////
      // Mixer
      if (startMixer)
      {
         mixerRunningStart = GetTickCount();

      }
      if (valves & PS_STIRRER_START)
      {
         // mixer is running
         // check if it is time to turn it off
         if ((GetTickCount() - mixerRunningStart) > mixerDuration)
         {
            mixerRunningStart = 0;  //clear the timer
         }
      }
      controlMaskOffBits |= PS_STIRRER_START;
      //valveControls &= (~PS_STIRRER_START);   // mask the start bit off

      ///////////////////////////////
      // AUTO
      if (automatic /*&& elementCovered*/)
      {
         heaterON = tankTemp < tempSetPoint + (heaterON?PS_AUTO_HYSTERESIS:0);
         if (!elementCovered)
            heaterON = FALSE; //kick out the heater automatically
         if (heaterON)
         {
            controlMaskOnBits |= PS_MASK_HEATER;
            //valveControls |= PS_MASK_HEATER;    // turn heater bit ON
         }
         else
         {
            controlMaskOffBits|= PS_MASK_HEATER;
            //valveControls &= (~PS_MASK_HEATER); // turn heater bit off
         }
      }
      else
      {
         // turn auto off
         if (!elementCovered)
         {
            controlMaskOffBits |= PS_MASK_AUTO;
            //valveControls &= (~PS_MASK_AUTO); // turn auto bit off
         }
      }
      
      if (0 == tankTemp)
         tankTemp = AMBIENT;   // 21 degrees *100
      
      ///////////////////////////////////////////
      // fill the tank
      if ((input1 || input2) && (tankLevel < tankLevelMax))
      {
      DWORD input1Flow,input2Flow;
         
         input1Flow = (input1 &&  (tank1Level>0)? 4 : 0);
         input2Flow = (input2 &&  (tank2Level>0)? 3 : 0);
         // 
         
         tankLevel += input1Flow;  
         tankLevel += input2Flow; 
         // subtract the vol. from the filler tanks
         tank1Level -= input1Flow;
         tank2Level -= input2Flow;
      }

      // empty the tank
      if (tankLevel>1)     // drain valve open  (randomly drain till 0 or 1)
         if (drain)
         {
            tankLevel -=2;
            if (tankLevel > tankLevelMax/2)
               tankLevel -=3;
            if (tankLevel > tankLevelMax/3)
               tankLevel -=4;
         }
         
      ///////////////////////////////////
      // work out tank temperature
      if (elementCovered && heaterON)
      { // heat up
         //float inc = (40*(float)tankLevel/400); // joules to add per 400 mm water
         float inc = (40*(1.0F/tankLevel*800)); // joules to add per 400 mm water
         float heatFactor;
         int heaterIndex, heaterIndexes;
         // based on the existing temp adjust the temp

         heaterCurrent = 40;
         heaterCurrent += (int)((TEMPMAX/(float)tankTemp)*4.0);

         //heatFactor = 0.5+(0.01/( ((float)tankTemp+1)/(TEMPMAX-AMBIENT)))/0.3;
         heaterIndexes = sizeof(heatingEfficiencyTable)/sizeof(heatingEfficiencyTable[0]);
         heaterIndex = (int)Round(((float)(tankTemp-AMBIENT))/TEMPMAX*(heaterIndexes-1));
         if (input1 || input2) 
            heaterIndex--; // add cold water
         if (heaterIndex<0) heaterIndex=0;
         if (heaterIndex >= heaterIndexes) heaterIndex = heaterIndexes-1;

         heatFactor = heatingEfficiencyTable[heaterIndex];
         efficiency = (int)(heatFactor*100);
         inc *= heatFactor;
         tankTemp += (int)inc;

      }
      else
      {  // cool off
      int cooling = 1;

         if (input1 || input2)
         {
            cooling += getrandom(1, ((input1 && input2)?3:2) );// 2 inflows will cool this lot faster
            cooling += (int)((1.0F/(tankLevel+1))*2000);
         }
         heaterCurrent = 0;
         if (heaterON)
            heaterCurrent = 40;  // element on, but not covered

         if (AMBIENT < tankTemp)
            tankTemp-=cooling;
         if (AMBIENT*2 < tankTemp)
            tankTemp-=cooling;
         if (AMBIENT*3 < tankTemp)
            tankTemp-=2*cooling;
         if (AMBIENT*4 < tankTemp)
            tankTemp-=4*cooling;
      }

   }

   if (heaterCurrent)
      heaterCurrent += getrandom(0,1);
   /////////////////////////////
   // Time
   GetLocalTime(&sysTime);
   
   /////////////////////////////
   // determine all built-in tank alarms
   alarms = 0;
   if (!elementCovered)
      alarms += PS_MASK_LOWLEVEL;
   
   valves = valveControls;
   if (mixerRunningStart)
      valves |= PS_STIRRER_START;   // stirrer feedback bit #1

   if (heaterON)
      valves |= PS_MASK_HEATER;
   else
      valves &= (~PS_MASK_HEATER);

   valves |= alarms; // add the alarms to the feedback bits

   {
   // lock the memory for writting
   CMemWriteLock lk(PLCMemory.GetMutex());

      valveControls &= (~controlMaskOffBits);
      valveControls |= controlMaskOnBits;

      // plug all values back into the sim
      PLCMemory.SetAt(MODBUS_MEM_REGISTERS, PS_CONTROLBITS, (WORD)valveControls);        // valve feedbacks
      PLCMemory.SetAt(MODBUS_MEM_REGISTERS, PS_FEEDBACKBITS, (WORD)valves);        // valve feedbacks

      PLCMemory.SetAt(MODBUS_MEM_REGISTERS, PS_LEVEL, (WORD)tankLevel);

      PLCMemory.SetAt(MODBUS_MEM_REGISTERS, PS_TANK1LEVEL, (WORD)tank1Level);
      PLCMemory.SetAt(MODBUS_MEM_REGISTERS, PS_TANK2LEVEL, (WORD)tank2Level);
   
      PLCMemory.SetAt(MODBUS_MEM_REGISTERS, PS_TEMP, (WORD)tankTemp);        // temp *10
      PLCMemory.SetAt(MODBUS_MEM_REGISTERS, PS_HEATERAMPS, (WORD)heaterCurrent);        // current Amps
   
      PLCMemory.SetAt(MODBUS_MEM_REGISTERS, PS_HEATEREFFECT, (WORD)efficiency);
      PLCMemory.SetAt(MODBUS_MEM_REGISTERS, PS_MILLISECONDS, sysTime.wMilliseconds);
      PLCMemory.SetAt(MODBUS_MEM_REGISTERS, PS_SECONDS, sysTime.wSecond);
      PLCMemory.SetAt(MODBUS_MEM_REGISTERS, PS_MINUTES, sysTime.wMinute);
      PLCMemory.SetAt(MODBUS_MEM_REGISTERS, PS_HOURS, sysTime.wHour);
      PLCMemory.SetAt(MODBUS_MEM_REGISTERS, PS_DAY, sysTime.wDay);
      PLCMemory.SetAt(MODBUS_MEM_REGISTERS, PS_WEEKDAY, sysTime.wDayOfWeek);

      PLCMemory.SetAt(MODBUS_MEM_REGISTERS, PS_RANDOM, getrandom(0, 65535));
   }
   if (m_listCtrlData.GetAreaDisplayed() == MODBUS_MEM_REGISTERS)
      m_listCtrlData.RedrawItems(0, 2);  // refresh 1st 3 rows
} // DoPlantSimulation

// --------------------------- RunAnimationScript ---------------------------
BOOL CMOD_simDlg::RunAnimationScript(LPCTSTR moduleName, LPCTSTR moduleText)
{
CString deb;
CString strScriptText;
DWORD dwRuntime = GetTickCount();

   if (m_ScriptProxy.IsRunning())
   {
      AddCommsDebugString("WARNING! Animation Script overrun!");

      SetLastRuntime(-1);
      return(TRUE);
   }

   if (moduleName)
   {
      if (m_reloadAnimationScript)
      {
      CFile file;
      CFileStatus moduleFileStatus;
      CString szBuf;
      long nBuf=0, lengthTest;

         deb.Format("Load script file '%s'", moduleName);
         //

         file.Open(moduleName, CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary);
         file.GetStatus( moduleFileStatus );
         nBuf = (long)(moduleFileStatus.m_size + 1);
#if     _MSC_VER > 1200	//VC 9
         if (file.m_hFile == INVALID_HANDLE_VALUE)
#else
         if (file.m_hFile == (DWORD)INVALID_HANDLE_VALUE)
#endif
         {
            deb.Format("Error opening script file '%s'", moduleName);
         }
         else
         {
            LPTSTR sBuf = szBuf.GetBufferSetLength( nBuf );
            file.Read(sBuf, (DWORD)nBuf); 
            //file.Read( sBuf, nBuf );
            file.Close();
            sBuf[nBuf - 1] = '\0';
            szBuf.ReleaseBuffer();
            lengthTest = szBuf.GetLength();
            ASSERT(lengthTest == nBuf-1);
            szBuf.TrimRight();
         }
         m_animationScriptText = szBuf;
         AddCommsDebugString(deb);
         m_reloadAnimationScript = FALSE;
      }
      //deb.Format("Run script '%s'", moduleName);
      deb = "";
   }
   else
   {
      //deb.Format("Run script TEXT");
      deb = "";
      m_animationScriptText = moduleText;
   }
   AddCommsDebugString(deb);
   /*
   strScriptText = "dim x\n\
      dim n\n\
      n=0\n\
      for n=0 to 240 \n\
      x = getregistervalue(3,n)\n\
      SetRegisterValue 3, n, x+1\n\
      next\n\
      'DisplayAbout";
   */

   
   strScriptText = m_animationScriptText;
	if (strScriptText.GetLength()>0)
	{
		BSTR bstrText = strScriptText.AllocSysString();
		m_ScriptProxy.AddScriptCode(bstrText);
		SysFreeString(bstrText);
	}
   // TODO !!! check error codes

   SetLastRuntime(GetTickCount() - dwRuntime);
   return(TRUE);
}

// ------------------------------- GetNumMemoryAreas -----------------------
// return the # of memory files/areas
LONG CMOD_simDlg::GetNumMemoryAreas()
{
   switch (m_selectedProtocol)
   {
      case PROTOCOL_SELMOD232:
      case PROTOCOL_SELMODETH:
         return(MAX_MOD_MEMTYPES);
         break;
      case PROTOCOL_SELABMASTER232:
      case PROTOCOL_SELAB232:
         return(MAX_AB_MEMFILES);
   }
   return(0);
}

// ------------------------------ PLCMemDisplayWidths ------------------------
WORD CMOD_simDlg::PLCMemDisplayWidths(LONG typeIx)
{
   switch(m_selectedProtocol)
   {
      case PROTOCOL_SELMOD232:
      case PROTOCOL_SELMODETH:
         switch(typeIx)
         {
         case 0: return   16;
            break;
         case 1: return   16;
            break;
         case 2: return   10;
            break;
         case 3: return   10;
            break;
         case 4: return   10;
            break;
         }
         break;
      case PROTOCOL_SELABMASTER232:
      case PROTOCOL_SELAB232:
         return(10);
         break;
   }
   return(10);
}

BOOL CMOD_simDlg::PLCIsBitMemory(LONG typeIx)
{
   switch(m_selectedProtocol)
   {
      case PROTOCOL_SELMOD232:
      case PROTOCOL_SELMODETH:
         return (typeIx < 2);
         break;
      case PROTOCOL_SELABMASTER232:
      case PROTOCOL_SELAB232:
         return(FALSE);
         break;
   }
   return(FALSE);
}


// --------------------------- DoAnimations --------------------------------
// Animate the simulation data every X ticks of the timer
// Zeroing values also uses the same loop code in here as OO re-use
void CMOD_simDlg::DoAnimations() 
{
   m_animationCounter--;
   if ( ((m_animationON) && (m_animationCounter<=0)) ||
        (m_zeroValues) || (m_plantAnimation)
      )
   {
   LONG areaIndex;
   LONG wordIndex;
   LONG wordINCValue = m_animationIncValue + (BYTE(m_animationIncValue)<<8);

      if ((m_plantAnimation) && (!m_zeroValues))
      {
      BOOL  scriptedOK = FALSE;
      BOOL  initOK = TRUE;

         if (m_animationScriptFile.GetLength())
         {
            if (!m_scriptEngineInitilized)
            {
               // check error, and do not bother loading text
             	initOK = m_ScriptProxy.CreateEngine( L"VBScript" );

               m_scriptEngineInitilized = TRUE; // do not try init things twice. It is not use.
            }

            // try to run the script
            if (initOK)
               scriptedOK = RunAnimationScript(m_animationScriptFile);
            // if it fails do the normal stuff anyway
            if (!scriptedOK)
               DoPlantSimulation();
         }
         else
            DoPlantSimulation();
      }
      else //  animation
      {
         // loop thru all mem areas
         for (areaIndex=0; areaIndex < GetNumMemoryAreas()/*MAX_MOD_MEMTYPES*/; areaIndex++)
         {
         // lock the memory for writting
         CMemWriteLock lk(PLCMemory.GetMutex());
         DWORD    maximumOffset = GetPLCMemoryLimit(areaIndex);

            if (!lk.IsLocked())
            {
               if (m_zeroValues)
               {
                  for (wordIndex=0; wordIndex < (LONG)maximumOffset; wordIndex++)
                     PLCMemory.SetAt(areaIndex, wordIndex, 0);
               }
               else
               {
                  // loop thru all registers (WORD)
                  for (wordIndex=0; wordIndex < (LONG)maximumOffset/*MAX_MOD_MEMWORDS*/; wordIndex++)
                  {
                     // perform animation of the values
                     if (PLCIsBitMemory(areaIndex))
                     {  // animate digitals by toggling
                        if (PLCMemory[areaIndex][wordIndex])   // digitals
                           PLCMemory.SetAt(areaIndex, wordIndex, 0);
                        else
                           PLCMemory.SetAt(areaIndex, wordIndex, 1);
                     }
                     else
                     {  // animate 16-bit types
                        if (m_animationBYTE)   // increment each byte on it's own
                           PLCMemory.IncrementAt(areaIndex, wordIndex, (WORD)wordINCValue);
                        else
                           PLCMemory.IncrementAt(areaIndex, wordIndex, (WORD)m_animationIncValue);
                     }
                  }
               }
            }
            else
               OutputDebugString("Failed to lock mem!\n");
            // refresh the listview control
            if (m_listCtrlData.GetAreaDisplayed() == areaIndex)
               m_listCtrlData.RedrawItems(0, maximumOffset);

            // we unlock the memory for a moment to allow comms writes to occur
            // during an animation without any waiting time (causes comms delay otherwise)
         }
         m_zeroValues = FALSE;

      }      
      // reset the count-down to the next animation cycle
      m_animationCounter = m_animationRefreshes;
   }
} // DoAnimations



// ------------------------------ GetConnectionIPAddresses ------------------
void CMOD_simDlg::GetConnectionIPAddresses(CString &local, CString &remote)
{
CHAR     nameBuffer[120];
SOCKET peerSock = INVALID_SOCKET;

   // the exception handlers are to cope if the socket or some pointers get nuked by 
   // the server threads while we are in here.
   if (!m_pServerSockArray)  // user has closed the port
   {
      local = remote = "no host";
      return;
   }

   // get our local name
   try
   {
      if (SOCKET_ERROR != gethostname(nameBuffer, sizeof(nameBuffer)))
      {
         local = nameBuffer;
      }

   }
   catch(...)
   {
      CString msg;
         msg.Format("INTERNAL APPLICATION ERROR FILE %s LINE: %d\n%s\n%s", 
                          __FILE__, __LINE__, __MY_APPVERSION__, __DATE__);
         MessageBox(msg);
   }

   // Get remote IP
   // search for a connected socket and get it's addresses
   try
   {
      // find a socket that is currently connected
      peerSock = m_pServerSockArray->FirstConnected();
	   SOCKADDR sockAddr;
	   memset(&sockAddr, 0, sizeof(sockAddr));

	   int nSockAddrLen = sizeof(sockAddr);
            
      if (INVALID_SOCKET != peerSock)
         // use it to get the peer name
         if (SOCKET_ERROR != getpeername(peerSock ,&sockAddr, &nSockAddrLen))
         {
		      //rPeerPort = ntohs(sockAddr.sin_port);
		      //remote = inet_ntoa(sockAddr.sin_addr);
            remote.Format("%d.%d.%d.%d", (BYTE)sockAddr.sa_data[2],
                (BYTE)sockAddr.sa_data[3],
                (BYTE)sockAddr.sa_data[4],
                (BYTE)sockAddr.sa_data[5]
               );
         }

   }
   catch(...)
   {
   CString msg;
      msg.Format("INTERNAL APPLICATION ERROR FILE %s LINE: %d\n%s\n%s", 
         __FILE__, __LINE__, __MY_APPVERSION__, __DATE__);
      MessageBox(msg);
   }
} // GetConnectionIPAddresses


// ---------------------------------- OnSettings -----------------------------
//
void CMOD_simDlg::OnSettings() 
{
//CMOD232CommsProcessor *pCPU;
CObject *pCPU = NULL;
int retCode;

   // a little code to stop us from going into here twice with disasterous effects.
   if (m_busyCreatingServers)
   {
      MessageBeep(MB_ICONEXCLAMATION);
      return;
   }
   m_busyCreatingServers = TRUE;

   switch (m_selectedProtocol)
   {
      case PROTOCOL_SELMOD232:
      case PROTOCOL_SELABMASTER232:
      case PROTOCOL_SELAB232:
      {
      CServerSettings232Dlg dlg;


         dlg.m_portName    = m_portNameShort;
         dlg.m_baud        = m_baud;
         dlg.m_dataBits    = m_byteSize;
         dlg.m_parity      = m_parity;
         dlg.m_stopBits    = m_stopBits;
         dlg.m_responseDelay = m_responseDelay;
         dlg.m_MOSCADchecks = m_MOSCADchecks;
         dlg.m_autoLoad    = m_autoLoad;
         dlg.m_startAsOff  = m_startAsOff;
         dlg.m_rts         = m_rts;
         dlg.m_ABMastermode = m_PLCMaster;

         dlg.m_protocol    = m_selectedProtocol;
         dlg.m_useBCC      = m_useBCC;

         /////////////////////////////////////////////////////////////////////////
         // pop the dialog
         retCode = dlg.DoModal();
         if (IDOK==retCode)
         {

            m_responseDelay = dlg.m_responseDelay;

            strcpy_s(m_portNameShort, MAX_COMPORT_NAME, dlg.m_portName);
            m_baud         = dlg.m_baud;
            m_byteSize     = dlg.m_dataBits;
            m_parity       = dlg.m_parity;
            m_stopBits     = dlg.m_stopBits;
            m_MOSCADchecks = dlg.m_MOSCADchecks;
            m_autoLoad     = dlg.m_autoLoad;
            m_startAsOff   = dlg.m_startAsOff;
            m_rts          = dlg.m_rts;
            m_PLCMaster    = dlg.m_ABMastermode;
            m_useBCC       = dlg.m_useBCC;

         }
      }
      break;
      case PROTOCOL_SELMODETH:
      {
      CServerSettingsEthDlg dlg;

         dlg.m_localPort = m_localPort;
         dlg.m_other     = m_otherPort;
         dlg.m_localId   = m_localId;
         dlg.m_responseDelay = m_responseDelay;
         dlg.m_linger = m_linger;
         dlg.m_numServers = m_numServers;
         // fill in IP addresses for the connected host
         GetConnectionIPAddresses(dlg.m_localIPStr, dlg.m_remoteIPStr);

         retCode = dlg.DoModal();
         if (IDOK == retCode)
         {
            m_localPort       = dlg.m_localPort;
            m_localId         = dlg.m_localId;
            m_responseDelay   = dlg.m_responseDelay;
            m_linger          = dlg.m_linger;
            m_numServers      = dlg.m_numServers;
         }
      }

   }

   if (IDOK == retCode)
   {
      UnloadCommsProcessors();
      InitiateCommsProcessors(m_selectedProtocol);
      m_PacketsReceived = 0;
      m_PacketsSent = 0;

      // clear all "noise" settings again
      switch (m_selectedProtocol)
      {
         case PROTOCOL_SELMOD232:
            pCPU = m_pServerRS232Array->GetAt(0);
            if (NULL!=pCPU)
               m_ToolBar.SetButtonInfo(5, IDC_NOISE,  TBBS_BUTTON, 
                                       (((CMOD232CommsProcessor *)pCPU)->m_NoiseSimulator.ErrorsEnabled()?11:10)       //bitmaps 10,11
                                      );
            break;
         case PROTOCOL_SELABMASTER232:
         case PROTOCOL_SELAB232:
            pCPU = m_pServerRS232Array->GetAt(0);
            if (NULL!=pCPU)
               m_ToolBar.SetButtonInfo(5, IDC_NOISE,  TBBS_BUTTON, 
                                       (((CAB232CommsProcessor *)pCPU)->m_NoiseSimulator.ErrorsEnabled()?11:10)       //bitmaps 10,11
                                      );
            break;
         case PROTOCOL_SELMODETH:
            if (!m_pServerSockArray) //creation failed, may be shutting down the app
               break;

            pCPU = m_pServerSockArray->GetAt(0);
            if (NULL!=pCPU)
               m_ToolBar.SetButtonInfo(5, IDC_NOISE,  TBBS_BUTTON, 
                                       (((CMODEthCommsProcessor *)pCPU)->m_NoiseSimulator.ErrorsEnabled()?11:10)       //bitmaps 10,11
                                      );
            break;
         default:
            ASSERT(0);
            break;
      }
   SaveApplicationSettings();
   }
   m_busyCreatingServers = FALSE;
   
} // OnSettings

// ------------------------------ OnViewhex ------------------------------------
// address format change
void CMOD_simDlg::OnViewhex() 
{
   // toggle over
   SetAddressFormatHex();
   CheckRadioButton(IDC_VIEWHEX, IDC_VIEWDEC, (IsAddressFormatHex() ? IDC_VIEWHEX : IDC_VIEWDEC));
   // re-paint (the control will call all the call-backs necessary)
   GetDlgItem(IDC_DATALIST)->InvalidateRect(NULL, TRUE);
} // OnViewhex

// ------------------------------ OnViewdec -------------------------------------
// address format change
void CMOD_simDlg::OnViewdec() 
{
	SetAddressFormatHex(FALSE);
   CheckRadioButton(IDC_VIEWHEX, IDC_VIEWDEC, (IsAddressFormatHex() ? IDC_VIEWHEX : IDC_VIEWDEC));
   // re-paint (the control will call all the call-backs necessary)
   GetDlgItem(IDC_DATALIST)->InvalidateRect(NULL, TRUE);
} // OnViewdec

// ------------------------------- OnSelchangeComboArea --------------------------
// User clicked on the MEM area selection list, set up the list-view, and 
// repaint it.
// This method is called in OnInitDialog also.
void CMOD_simDlg::OnSelchangeComboArea() 
{
DWORD width;
DWORD sel;
	
   sel = ((CComboBox*)GetDlgItem(IDC_COMBOAREA))->GetCurSel();
   m_listCtrlData.SetAreaDisplayed(sel);

   width = PLCMemDisplayWidths(sel);

   //if ( !PLCIsBitMemory[m_memAreaDisplayed])
   //   width = 8;
   //else
   //   width = BITS_IN_WORD;   // 16
   //
   SetupListViewControl(width);
} // OnSelchangeComboArea


// ------------------------------- OnZeroes ------------------------------
// This command is "fed" to the animation timer thread.
// The timer will zero the registers on the next tick.
void CMOD_simDlg::OnZeroes() 
{
   ZeroRegisterValues();
}

#if _MSC_VER > 1200
// compile for VS2008 (version 9)
LRESULT CMOD_simDlg::OnLoad_(WPARAM,LPARAM)
{
   OnLoad();
   return(0);
}
#endif



// --------------------------------- OnLoad ----------------------------
// load register values from file
// See note: OnSave()
void CMOD_simDlg::OnLoad() 
{
CWaitCursor wait;    // put up a wait cursor to stop any user interaction with 
                     // the window
   switch (m_selectedProtocol)
   {
      case PROTOCOL_SELMOD232 :
         {
         CMOD232CommsProcessor *pCPU;

            pCPU = (CMOD232CommsProcessor *)m_pServerRS232Array->GetAt(0);
            if (NULL!=pCPU)
               pCPU->LoadRegisters();
         }
         break;
      case PROTOCOL_SELABMASTER232 :
      case PROTOCOL_SELAB232 :
         {
         CAB232CommsProcessor *pCPU;

            pCPU = (CAB232CommsProcessor *)m_pServerRS232Array->GetAt(0);
            if (NULL!=pCPU)
               pCPU->LoadRegisters();
         }
         break;
      case PROTOCOL_SELMODETH :
         {
         CMODEthCommsProcessor *pCPU;

            pCPU = (CMODEthCommsProcessor *)m_pServerSockArray->GetAt(0);
            if (NULL!=pCPU)
               pCPU->LoadRegisters();
         }
         break;
   default:
      ASSERT(0);
      break;
   }
   // re-draw contents
   m_listCtrlData.RedrawItems(0, MAX_MOD_MEMWORDS);
}

// ------------------------------------ OnSave -------------------------
// save all the registers to file:
// Does not catter for different sizes, if U change the upper register limits
// so loading will give interresting results, but not crash.
//
void CMOD_simDlg::OnSave() 
{
CMODEthCommsProcessor *pEthCPU;
CWaitCursor wait;    // put up a wait cursor to stop any user interaction 
                     // with the window during this time

   switch (m_selectedProtocol)
   {
      case PROTOCOL_SELMOD232 :
         {
         CMOD232CommsProcessor *p232CPU;
            p232CPU = (CMOD232CommsProcessor *)m_pServerRS232Array->GetAt(0);
            if (NULL!=p232CPU)
               p232CPU->SaveRegisters();
         }
         break;
      case PROTOCOL_SELABMASTER232 :
      case PROTOCOL_SELAB232 :
         {
         CAB232CommsProcessor *p232CPU;
            p232CPU = (CAB232CommsProcessor *)m_pServerRS232Array->GetAt(0);
            if (NULL!=p232CPU)
               p232CPU->SaveRegisters();
         }
         break;
      case PROTOCOL_SELMODETH :
         pEthCPU = (CMODEthCommsProcessor *)m_pServerSockArray->GetAt(0);
         if (NULL!=pEthCPU)
            pEthCPU->SaveRegisters();
         break;
   }
   SaveApplicationSettings();

} // OnSave

// ----------------------------------- OnCancel ------------------------
// Close application button
void CMOD_simDlg::OnCancel() 
{
   m_startUpRegisterArea = ((CComboBox*)GetDlgItem(IDC_COMBOAREA))->GetCurSel();
   // save window size/position
   CRect rect;

   GetWindowRect(&rect);
   {
   CString registryPath;
   DRegKey key;

      m_appSettingAdressHex = m_listCtrlData.IsAddressFormatHex();
      // Save the baud rate etc
      registryPath = APPREGISTRY_SETTINGSKEY;
      
      // save to current user
      key.Open(DRegKey::current_user, registryPath);
      key.SetValue("WindowPositionX", (rect.left+ (rect.right<<16))); 
      key.SetValue("WindowPositionY", (rect.top+ (rect.bottom<<16))); 
   }
   // save
   SaveApplicationSettings();

   UnloadCommsProcessors();

   CDialog::OnCancel();
} // OnCancel

// ------------------------------------ OnClose ------------------------------
// close button on the system menu
void CMOD_simDlg::OnClose() 
{
   UnloadCommsProcessors();
	
   CDialog::OnClose();
} // OnClose


// ------------------------------------ OnClone ------------------------------
// User clicked on the pure/clone toggle-box
//
void CMOD_simDlg::OnClone() 
{
   if (PLCIsBitMemory(m_listCtrlData.GetAreaDisplayed()))
      // Force column headdings to change.
      OnSelchangeComboArea();	
}

// ------------------------------- PreTranslateMessage ---------------------
//
BOOL CMOD_simDlg::PreTranslateMessage(MSG* pMsg) 
{
   // MSDN Q190684: Pass the message to the splash screen component
   if (CSplashWnd::PreTranslateAppMessage(pMsg))
        return TRUE; 

   // when restoring the window, always turn the alpha-blend off
	if (pMsg->message == WM_SYSCOMMAND)
      if (pMsg->wParam == SC_RESTORE)
         if (alphaBlendON) // we can only do this on win2K onwards
            OnSysCommand(IDM_ALPHA, 0);
   
   HACCEL hAccel = m_hAccel; 
   if(hAccel && ::TranslateAccelerator(AfxGetApp()->m_pMainWnd->m_hWnd, hAccel,
                               pMsg))
      return TRUE;


   // TOOLTIPS START
   if (m_hWnd)
   {
      m_ToolTip.RelayEvent (pMsg);
      return CDialog::PreTranslateMessage(pMsg);
   }
   // TOOLTIPS END

   // call the base class as well
	return CDialog::PreTranslateMessage(pMsg);
} // PreTranslateMessage

// ----------------------------- OnSelchangeDataFormat ---------------------
//
void CMOD_simDlg::OnSelchangeDataFormat() 
{
DWORD selection = (WORD)m_dataFormatCombo.GetCurSel();
WORD  sel = (WORD)m_dataFormatCombo.GetItemData(selection);

	// tell the contained view to re-draw the items
   m_listCtrlData.SetDataViewFormat(CMemoryEditorList::ListViewFormats_(sel));
}

// ------------------------------ OnMyContextMenu ---------------------------
// If I want a menu sometime in future. I can use this code to pop it up
void CMOD_simDlg::OnMyContextMenu(CWnd*, CPoint point)
{
	// CG: This block was added by the Pop-up Menu component
	{
		if (point.x == -1 && point.y == -1){
			//keystroke invocation
			CRect rect;
			GetClientRect(rect);
			ClientToScreen(rect);

			point = rect.TopLeft();
			point.Offset(5, 5);
		}

		CMenu menu;
		VERIFY(menu.LoadMenu(CG_IDR_POPUP_APPMENU));

		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		CWnd* pWndPopupOwner = this;

		while (pWndPopupOwner->GetStyle() & WS_CHILD)
			pWndPopupOwner = pWndPopupOwner->GetParent();

       
       pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON/*TPM_RIGHTBUTTON*/, point.x, point.y,
			pWndPopupOwner);

	}
} // OnMyContextMenu

// ------------------------ OnTTN_NeedText ---------------------------------
// TTN_NEEDTEXT message handler for TOOLTIPS
//
BOOL CMOD_simDlg::OnTTN_NeedText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
static char toolTiptext[160];

    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    UINT nID =pNMHDR->idFrom;
    if (pTTT->uFlags & TTF_IDISHWND)
    {
        // idFrom is actually the HWND of the tool
        nID = ::GetDlgCtrlID((HWND)nID);
        if(nID)
        {
           pTTT->lpszText = MAKEINTRESOURCE(nID);
           pTTT->hinst = AfxGetResourceHandle();
           return(TRUE);
        }
    }
    else
    {
       // CDB : The controls in the toolbar do not have the TTF_IDISHWND flag  
       pTTT->lpszText = MAKEINTRESOURCE(nID);
       pTTT->hinst = AfxGetResourceHandle();
    }
    return(FALSE);
} // OnTTN_NeedText

// --------------------------------- OnAboutboxBn ----------------------------
// toolbar button handler
// this method is balled from the system-menu handler too.
void CMOD_simDlg::OnAboutboxBn() 
{
	// TODO: Add your control notification handler code here
	// pop the about box
	CAboutDlg dlgAbout;
   dlgAbout.m_registeredUser = m_registeredUser;
   dlgAbout.m_registeredKey = m_registeredKey;
   dlgAbout.m_registeredOK = m_registration.IsRegistered();

		dlgAbout.DoModal();
} // OnAboutboxBn

// ----------------------------------- OnAlphablendFlip ----------------------
// Toolbar button handler for alpha blending
void CMOD_simDlg::OnAlphablendFlip() 
{
   //Decide on which state to flip to
   // 1: none     graphic 8
   // 2: 70%      graphic 9/10
   // 3: 60%      graphic 9/10
   // ... back to 1
   if (!alphaBlendON)
      // go to 60%
      SetAlphaBlendLow();
   else
   {
      if (70==alphaBlendFactor)
         // toggle it off
         SetAlphaBlendSolid();
      else
         // was on 60%, go to 70%
         SetAlphaBlendHigh();
      
   }
} // OnAlphablendFlip

// ----------------------------------- SetAlphaBlendXXXX ---------------------
// Alpha- blend functions only available on XP & 2000 onwards, these members 
// will not be called if m_pSetLayeredWindowAttributes is NULL.
//
// --------------------------------- SetAlphaBlendSolid ------------------------------
BOOL CMOD_simDlg::SetAlphaBlendSolid()
{
   ASSERT(m_pSetLayeredWindowAttributes);
   SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) && (!WS_EX_LAYERED));
   alphaBlendON = FALSE;
   // HF fixed index of bitmaps m_ToolBar.SetButtonInfo(8, IDB_ALPHABLEND,  TBBS_BUTTON, 8);
   m_ToolBar.SetButtonInfo(8, IDB_ALPHABLEND,  TBBS_BUTTON, 7);
   return (TRUE);
} // SetAlphaBlendSolid

// --------------------------------- SetAlphaBlendHigh ------------------------------
BOOL CMOD_simDlg::SetAlphaBlendHigh()
{
   ASSERT(m_pSetLayeredWindowAttributes);
   alphaBlendFactor = 70;
   // Check the current state of the dialog, and then add the WS_EX_LAYERED attribute
   SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
   m_pSetLayeredWindowAttributes(m_hWnd, 0, (BYTE)((255 / alphaBlendFactor) * 100), LWA_ALPHA);
   alphaBlendON = TRUE;
   // HF fixed index of bitmaps  m_ToolBar.SetButtonInfo(8, IDB_ALPHABLEND,  TBBS_BUTTON, 7);
   m_ToolBar.SetButtonInfo(8, IDB_ALPHABLEND,  TBBS_BUTTON, 9);
   return (TRUE);
} // SetAlphaBlendHigh

// ---------------------------------- SetAlphaBlendLow ------------------------------
BOOL CMOD_simDlg::SetAlphaBlendLow()
{
   ASSERT(m_pSetLayeredWindowAttributes);
   alphaBlendFactor = 60;
   // Check the current state of the dialog, and then add the WS_EX_LAYERED attribute
   SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
   m_pSetLayeredWindowAttributes(m_hWnd, 0, (BYTE)((255 / alphaBlendFactor) * 100), LWA_ALPHA);
   alphaBlendON = TRUE;
   // HF fixed index of bitmaps  m_ToolBar.SetButtonInfo(8, IDB_ALPHABLEND,  TBBS_BUTTON, 9);
   m_ToolBar.SetButtonInfo(8, IDB_ALPHABLEND,  TBBS_BUTTON, 8);
   return (TRUE);
} // SetAlphaBlendLow

// -------------------------------- OnSimulate --------------------------------
// Open the "edit simulation settings" dialog
void CMOD_simDlg::OnSimulate() 
{
   CAnimationSettingsDlg dlg;
   
   //copy all setings over to the dialog
   dlg.m_value          = m_animationIncValue;
   dlg.m_animationON    = m_animationON;
   dlg.m_plantAnimation = m_plantAnimation;
   dlg.m_animationBYTE  = m_animationBYTE;
   dlg.m_animationWORD  = m_animationWORD;
   dlg.m_refreshes      = m_animationRefreshes;
   dlg.m_animationPeriod = m_animationPeriod;
   dlg.m_scriptFile      = m_animationScriptFile;

   // HTML GUI params
   dlg.m_enableHTMLGUI      = m_enableHTMLGUI;
   dlg.m_HtmlUpdateRate     = m_HTMLUpdateRate;
   dlg.m_outputHtmlFilename = m_outputHTMLFilename;
   dlg.m_inputHtmlFilename  = m_inputHTMLFilename;

   // colors
   dlg.m_mTickColorEnabled  = m_mTickColorEnabled;
   dlg.m_mTickColorDisabled = m_mTickColorDisabled;
   dlg.m_mTickColorActive   = m_mTickColorActive; 
   dlg.m_mTickColorInactive = m_mTickColorInactive;
   dlg.m_mTickColorText     = m_mTickColorText;
   dlg.m_mTickDisColorText  = m_mTickDisColorText;

   if (IDOK == dlg.DoModal())
   {
      // OK accept the new settings
      m_animationIncValue  = dlg.m_value;
      m_animationON        = dlg.m_animationON;
      m_plantAnimation     = dlg.m_plantAnimation;
      m_animationBYTE      = dlg.m_animationBYTE;
      m_animationWORD      = dlg.m_animationWORD;
      m_animationRefreshes = dlg.m_refreshes;
      m_animationPeriod    = dlg.m_animationPeriod;
      m_animationScriptFile = dlg.m_scriptFile;
      // HTML GUI params
      m_enableHTMLGUI      = dlg.m_enableHTMLGUI     ; 
      m_HTMLUpdateRate     = dlg.m_HtmlUpdateRate    ; 
      m_outputHTMLFilename = dlg.m_outputHtmlFilename; 
      m_inputHTMLFilename  = dlg.m_inputHtmlFilename ; 

      m_mTickColorEnabled  = dlg.m_mTickColorEnabled; 
      m_mTickColorDisabled = dlg.m_mTickColorDisabled; 
      m_mTickColorActive   = dlg.m_mTickColorActive; 
      m_mTickColorInactive = dlg.m_mTickColorInactive; 
      m_mTickColorText     = dlg.m_mTickColorText; 
      m_mTickDisColorText  = dlg.m_mTickDisColorText; 
      

      // apply the new settings
      m_reloadAnimationScript = TRUE;
      for (int stationIndex=0; stationIndex<STATIONTICKBOXES; stationIndex++) 
      {
         // change the activity-tick colors
         m_microTicks[stationIndex].SetCheckedColor(m_mTickColorEnabled);
         m_microTicks[stationIndex].SetUnCheckedColor(m_mTickColorDisabled);
         m_microTicks[stationIndex].SetActiveColor(m_mTickColorActive);
         m_microTicks[stationIndex].SetInactiveColor(m_mTickColorInactive);
         m_microTicks[stationIndex].SetTextColor(m_mTickColorText);
         m_microTicks[stationIndex].SetUnCheckedTextColor(m_mTickDisColorText);
      }
      ConfigureStationButtons();
      if (m_enableHTMLGUI)
      {
         // start the GUI
         m_htmlOutput.Watch(m_HTMLUpdateRate, m_inputHTMLFilename, m_outputHTMLFilename);
      }
      else
         m_htmlOutput.StopWatching();

   }
   SaveApplicationSettings();

} // OnSimulate

// -------------------------------- OnNoise ---------------------------------------
// pop the "noise" dialog.
void CMOD_simDlg::OnNoise() 
{
CFaultsDlg  faultDlg;
CMOD232CommsProcessor *pModCPU;
CAB232CommsProcessor *pABCPU;
CRS232Noise *pNoiseObject = NULL;

   if (NULL == m_pServerRS232Array)
   {
   CString msg, debugStr;
      // pop up a warning message to tell the user this function is not supported
      msg.LoadString(IDS_NOTSUPPORTEDETH);
      debugStr.Format("\n\nFILE:%s LINE:%d\n%s\n%s", __FILE__, __LINE__, __MY_APPVERSION__, __DATE__);
      msg += debugStr;
      MessageBox(msg);
      return;
   }
   pModCPU = (CMOD232CommsProcessor *)m_pServerRS232Array->GetAt(0);
   pABCPU = (CAB232CommsProcessor *)pModCPU;
   
   // insert our settings into the dialog
   if (NULL!=pModCPU)
   {
      if (pModCPU->IsKindOf(RUNTIME_CLASS(CMOD232CommsProcessor)))
         pNoiseObject = &pModCPU->m_NoiseSimulator;
      else
         pNoiseObject = &pABCPU->m_NoiseSimulator;
      pNoiseObject->GetErrorTypes(&faultDlg.m_errors,
                                  &faultDlg.m_errorFrequency,
                                  &faultDlg.m_delaysPeriod,
                                  &faultDlg.m_insertCharacters,
                                  &faultDlg.m_removeCharacters,
                                  &faultDlg.m_corruptFraming,
                                  &faultDlg.m_modifyCharacters,
                                  &faultDlg.m_ignoreReq,
                                  &faultDlg.m_parityFaults,
                                  &faultDlg.m_beep
                                 );
   }   
   if (IDOK==faultDlg.DoModal())
   {
      // read the dialog settings back into the NOISE class
      if (NULL != pNoiseObject)
         pNoiseObject->SetErrorTypes(faultDlg.m_errors,
                                     faultDlg.m_errorFrequency,   
                                     faultDlg.m_delaysPeriod,     
                                     faultDlg.m_insertCharacters, 
                                     faultDlg.m_removeCharacters, 
                                     faultDlg.m_corruptFraming,   
                                     faultDlg.m_modifyCharacters, 
                                     faultDlg.m_ignoreReq,        
                                     faultDlg.m_parityFaults,
                                     faultDlg.m_beep              
                                    );
      // set the toolbar button appropriately
      m_ToolBar.SetButtonInfo(5, IDC_NOISE,  TBBS_BUTTON, (faultDlg.m_errorFrequency?11:10));       //bitmaps 10,11
   }
   SaveApplicationSettings();
} // OnNoise


// ----------------------------- OnEmulation ------------------------------
// Display the emulation settings dialog.
void CMOD_simDlg::OnEmulation() 
{
CAdvEmulationDlg dlg;
BOOL dialogCompletion;
CJoySCCEmulationDlg masterDlg;

   // marshall the needed parameters
	dlg.m_numInputs =   m_numInputs;   
	dlg.m_numOutputs =  m_numOutputs;  
	dlg.m_numHolding =  m_numHolding;  
	dlg.m_numExtended = m_numExtended; 
	dlg.m_numAnalogs =  m_numAnalogs;  
   dlg.m_MOSCADchecks = m_MOSCADchecks;

   dlg.m_PDUSize = m_PDUSize;
   dlg.m_modifyThenRespond = m_modifyThenRespond;
   dlg.m_disableWrites = m_disableWrites;
   dlg.m_numSeperate = m_numSeperate;
   dlg.m_seperateRegisters = m_seperateRegisters;

   if (PROTOCOL_SELABMASTER232 == m_selectedProtocol)
   {
      masterDlg.m_emuDlgPtr = &dlg; // pointer to the old MODBUS dialog
      masterDlg.m_sourceAddr     = m_ABMasterSourceStation;
      masterDlg.m_destAddr       = m_ABMasterDestStation;
      masterDlg.m_numShields     = m_ABMasterNumFiles;
      masterDlg.m_runMode        = m_ABMasterRunMode;
      masterDlg.m_readVariables  = m_ABMasterReadVariables;
      masterDlg.m_writeVariables = m_ABMasterWriteVariables;
      masterDlg.m_ABidletime     = m_ABmasterIDLETime;
      masterDlg.m_ABtimeout      = m_ABtimeoutValue;

      dialogCompletion = masterDlg.DoModal(); // pop AB master dialog, and have the modbus parameter dialog as a child
   }
   else
      dialogCompletion = dlg.DoModal();

   if (IDOK == dialogCompletion)
   {

      // copy the parameters back
	   m_numInputs =   dlg.m_numInputs;   
	   m_numOutputs =  dlg.m_numOutputs;  
	   m_numHolding =  dlg.m_numHolding;  
	   m_numExtended = dlg.m_numExtended; 
	   m_numAnalogs =  dlg.m_numAnalogs;  
      m_MOSCADchecks = dlg.m_MOSCADchecks;
      m_PDUSize = dlg.m_PDUSize;
      m_modifyThenRespond = dlg.m_modifyThenRespond;
      m_disableWrites = dlg.m_disableWrites;

      m_numSeperate = dlg.m_numSeperate;
      m_seperateRegisters = dlg.m_seperateRegisters;

      switch (m_selectedProtocol)
      {
      case PROTOCOL_SELMOD232 :
         {
         CMOD232CommsProcessor *p232CPU;
            p232CPU = (CMOD232CommsProcessor *)m_pServerRS232Array->GetAt(0);
            p232CPU->SetPDUSize(m_PDUSize);
            p232CPU->SetEmulationParameters(m_MOSCADchecks, m_modifyThenRespond, m_disableWrites);
         }
         break;
      case PROTOCOL_SELABMASTER232 :
         {
         CAB232CommsProcessor *p232CPU;

            m_ABMasterSourceStation = masterDlg.m_sourceAddr;
            m_ABMasterDestStation   = masterDlg.m_destAddr;
            m_ABMasterNumFiles      = masterDlg.m_numShields;
            m_ABMasterRunMode       = masterDlg.m_runMode;
            m_ABMasterReadVariables = masterDlg.m_readVariables;
            m_ABMasterWriteVariables= masterDlg.m_writeVariables;

            m_ABmasterIDLETime = masterDlg.m_ABidletime;
            m_ABtimeoutValue = masterDlg.m_ABtimeout;

            p232CPU = (CAB232CommsProcessor *)m_pServerRS232Array->GetAt(0);
            p232CPU->SetPDUSize(m_PDUSize);
            p232CPU->m_ABmasterIDLETime = m_ABmasterIDLETime;
            p232CPU->m_ABtimeoutValue = m_ABtimeoutValue;
            p232CPU->SetEmulationParameters(m_MOSCADchecks, m_modifyThenRespond, m_disableWrites);
            p232CPU->SetJoyParameters( m_ABMasterSourceStation, m_ABMasterDestStation, 
                                       m_ABMasterNumFiles, m_ABMasterRunMode,
                                       m_ABMasterReadVariables,
                                       m_ABMasterWriteVariables);
            // more...
         }
         break;
      case PROTOCOL_SELAB232 :
         {
         CAB232CommsProcessor *p232CPU;
            p232CPU = (CAB232CommsProcessor *)m_pServerRS232Array->GetAt(0);
            p232CPU->SetPDUSize(m_PDUSize);
            p232CPU->m_ABmasterIDLETime = m_ABmasterIDLETime;
            p232CPU->m_ABtimeoutValue = m_ABtimeoutValue;
            p232CPU->SetEmulationParameters(m_MOSCADchecks, m_modifyThenRespond, m_disableWrites);
         }
         break;
      case PROTOCOL_SELMODETH :
         {
         CMODEthCommsProcessor *pEthCPU;
            pEthCPU = (CMODEthCommsProcessor *)m_pServerSockArray->GetAt(0);
            pEthCPU->SetPDUSize(m_PDUSize);
            pEthCPU->SetEmulationParameters(m_MOSCADchecks, m_modifyThenRespond, m_disableWrites);
         }
         break;
      }

      SetMemAreaSizes();
      // refresh the list view
      OnSelchangeComboArea();
      // 
      SaveApplicationSettings();
   }
} // OnEmulation

// ------------------------------ TextToFile -----------------------------------
BOOL TextToFile(LPCTSTR text, LPCTSTR fileName)
{
DWORD size;
HANDLE hFile;
DWORD written=0;

   hFile = CreateFile(fileName,
                        GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL, NULL
                       );
   if (INVALID_HANDLE_VALUE == hFile)  // error trap
   {
   //CString caption;
   //   caption.Format("File %s could not be written!", fileName);
   //   AfxMessageBox(caption, MB_OK|MB_ICONSTOP);
      return(FALSE);
   }
   else
   {
      // write to disk
      size = strlen(text);
      WriteFile(hFile, text, size, &written, NULL);
      CloseHandle(hFile);
   }
   return(TRUE);
}

// ----------------------------------- OnAbout --------------------------------
// invoked via hotkey
void CMOD_simDlg::OnAbout() 
{
      OnAboutboxBn();	
}


// ----------------------------------- ResourceToFile --------------------------
BOOL ResourceToFile(int resourceID, LPCTSTR fileName)
{
DWORD size;
char *fileBuffer;

   if (LoadDATAResource(resourceID, &fileBuffer, &size))
   {
   HANDLE hFile;
   DWORD written=0;

      hFile = CreateFile(fileName,
                           GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, NULL
                          );
      if (INVALID_HANDLE_VALUE == hFile)  // error trap
      {
      //CString caption;
      //   caption.Format("File %s could not be written!", fileName);
      //   AfxMessageBox(caption, MB_OK|MB_ICONSTOP);
         delete fileBuffer;
         return(FALSE);
      }
      else
      {
         // write to disk
         WriteFile(hFile, fileBuffer, size, &written, NULL);
         CloseHandle(hFile);
      }
      delete fileBuffer;
   }
   return(TRUE);
}

// -------------------------------------- OnHelp ---------------------------------
void CMOD_simApp::OnHelp() 
{
char currentfolder[MAX_PATH*2];
char fileName[MAX_PATH*2];
CString myPath, page;
CString bookMark = m_htmBookmark;

   // get our EXE filename
   GetModuleFileName(NULL, currentfolder, sizeof(currentfolder));
   myPath = currentfolder;
   int pos = myPath.ReverseFind('\\'); //strip the name back untill the '\mod_rssim,exe'
   currentfolder[pos+1] = '\0'; 

   // build a filename for the root HTML help file
   sprintf(fileName, "%sindex.html", currentfolder);
   myPath = fileName;

   // load the HTM resource from our EXE resources
   if (ResourceToFile(IDR_HELPHTM, fileName))
   {
      // build a folder name for the image files
      sprintf(fileName, "%s\\images", currentfolder);
      CreateDirectory(fileName, NULL); //create directory

      // build filenames for and save each image file
      sprintf(fileName, "%s\\images\\shot1.gif", currentfolder);
      ResourceToFile(IDR_HELPSHOT1, fileName);
      sprintf(fileName, "%s\\images\\shot2.gif", currentfolder);
      ResourceToFile(IDR_HELPSHOT2, fileName);

      sprintf(fileName, "%s\\images\\anim.gif", currentfolder);
      ResourceToFile(IDR_HELPANIM, fileName);

      sprintf(fileName, "%s\\images\\juice.gif", currentfolder);
      ResourceToFile(IDR_HELPJUICE, fileName);
      sprintf(fileName, "%s\\images\\portA.gif", currentfolder);
      ResourceToFile(IDR_HELPPORTA, fileName);

      sprintf(fileName, "%s\\images\\toolbar.gif", currentfolder);
      ResourceToFile(IDR_TOOLBARSHOT, fileName);
      sprintf(fileName, "%s\\images\\joyshot.gif", currentfolder);
      ResourceToFile(IDR_JOYSHOT, fileName);
      sprintf(fileName, "%s\\images\\advmbus.gif", currentfolder);
      ResourceToFile(IDR_ADVMBUS, fileName);



      page.Format("<HTML>\
<HEAD>\r\n\
</HEAD>\r\n\
<BODY leftmargin=0 topmargin=0 bottommargin=0 rightmargin=10>\r\n\
<IFRAME ID=IFrame1 FRAMEBORDER=0 WIDTH=\"100%%\" HEIGHT=\"100%%\" SCROLLING=YES SRC=\"index.html%s\"></IFRAME>\r\n\
</BODY>\r\n\
</HTML>\r\n", bookMark);

//<meta http-equiv=\"refresh\" content = \"index.html%s\">\
      
      sprintf(fileName, "%shome.html", currentfolder);
      TextToFile(page, fileName);
      // if the root file saved OK, open the help file
      HINSTANCE h = ShellExecute(NULL, "open", fileName , NULL, NULL, SW_SHOWNORMAL);
   }
}

// -------------------------------- OnTransparencyToggle ----------------------
void CMOD_simDlg::OnTransparencyToggle() 
{
	// TODO: Add your command handler code here
   OnAlphablendFlip();	
}

// ------------------------------ SendToClipboard ------------------------------
BOOL CMOD_simDlg::SendToClipboard(const CString & s)
{
   HGLOBAL mem = ::GlobalAlloc(GMEM_MOVEABLE, s.GetLength() + 1);
   if(mem == NULL)
      return FALSE;
   LPTSTR p = (LPTSTR)::GlobalLock(mem);
   lstrcpy(p, (LPCTSTR)s);
   ::GlobalUnlock(mem);

   if(!OpenClipboard())
   {
      ::GlobalFree(mem);
      return FALSE;
   }
   EmptyClipboard();
   SetClipboardData(CF_TEXT, mem);
   CloseClipboard();
   return TRUE;
}

// ---------------------------------- OnCopyToClipBoard ------------------------
// CTRL-Insert (Copy contents of the debugger list to the clipboard)
void CMOD_simDlg::OnCopyToClipBoard()
{
   CString data;
   CString itemBuff;
   CWaitCursor wait;    // display wait cursor - long running loop.

   EnterCriticalSection(&debuggerCritSection);
   // dump the comms debugger listbox into a string
   int items = m_commsListBox.GetCount();
   int itemIndex =0;
   while (itemIndex < items)
   {
      m_commsListBox.GetText( itemIndex, itemBuff);
      itemBuff += "\r\n";
      data += itemBuff;
      itemIndex++;
   }
   LeaveCriticalSection(&debuggerCritSection);

   SendToClipboard(data);
}

/////////////////////////////////////////////////////////////////////////////////
// The next couple of members handle the communications display list-box.

// -------------------------------- OnToggleDisplay ---------------------------
// hotkey CTRL-C
void CMOD_simDlg::OnToggleDisplay() 
{
	// TODO: Add your command handler code here
	m_registerShow = !m_registerShow;
   if (m_registerShow)
   {
      //
      GetDlgItem(IDC_COMMSLIST)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_PAUSECOMMS)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_TRACKCOMMS)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_CLEARCOMMS)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SHOWCOMMSTIME)->ShowWindow(SW_HIDE);
      
      GetDlgItem(IDC_LAMPCTS)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_LAMPDSR)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_LAMPRING)->ShowWindow(SW_HIDE);

      GetDlgItem(IDC_DATALIST)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_STATIC_1)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_STATIC_2)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_STATIC_3)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_STATIC_4)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_VIEWHEX)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_VIEWDEC)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_COMBOAREA)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_DATAFORMAT)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_PLANTSIM)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_PROTOCOL)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_CLONE)->ShowWindow(SW_SHOW);
   }
   else
   {
      GetDlgItem(IDC_DATALIST)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_STATIC_1)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_STATIC_2)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_STATIC_3)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_STATIC_4)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_VIEWHEX)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_VIEWDEC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_COMBOAREA)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_DATAFORMAT)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_PLANTSIM)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_PROTOCOL)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_CLONE)->ShowWindow(SW_HIDE);

      GetDlgItem(IDC_COMMSLIST)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_PAUSECOMMS)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_TRACKCOMMS)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_CLEARCOMMS)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_SHOWCOMMSTIME)->ShowWindow(SW_SHOW);

      if (NULL != m_pServerRS232Array)
      {
         GetDlgItem(IDC_LAMPCTS)->ShowWindow(SW_SHOW);
         GetDlgItem(IDC_LAMPDSR)->ShowWindow(SW_SHOW);
         GetDlgItem(IDC_LAMPRING)->ShowWindow(SW_SHOW);
      }
   }
   
   SetDisplayToggleButton();
}


// ---------------------------- AddCommsDebugString -----------------------
// call from any thread
void CMOD_simDlg::AddCommsDebugString(LPCTSTR string)
{
CString entry;
CString *pString;
int pos;

   if (!strlen(string))
      return;
   pString = new CString;
   // check if we wanted timestamps
   if (m_commsTimes)
   {
   SYSTEMTIME sysTime;
      // Fetch and put in time MM:SS:sss for all things on this screen
      // Times here, is the time the event comes into the Q, not the time it gets displayed at, so
      // pausing display does not affect the time shown!
      GetLocalTime(&sysTime);
      entry.Format("%02d:%02d.%03d %s", sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds, string);
   }
   else
      entry = string;

   // clean it up a bit
   pos = entry.FindOneOf("\r\n");
   if (pos>=0)
      entry.SetAt(pos, '\0');

   // interlock now
   EnterCriticalSection(&debuggerCritSection);
   *pString = entry;
   m_debugDataQueue.AddTail(pString);

   LeaveCriticalSection(&debuggerCritSection);
}

// ------------------------------- UpdateCommsList -----------------------
// call on the GUI thread only!!!!
// copies the Debugger queue messages to the list-box
void CMOD_simDlg::UpdateCommsList()
{
CString *pString;
int count=0;
int sel, killed=0;

   if (m_debugDataQueue.IsEmpty())
      return;

   if (m_paused)
   {
      //check that there are not too many entries in the list, 
      EnterCriticalSection(&debuggerCritSection);
      if (m_debugDataQueue.GetCount() > MAX_COMMSDEBUGGER_BUFFER)
      {
         // drop items
         for (killed=0; killed < (MAX_COMMSDEBUGGER_BUFFER/20) ; killed++)   // cull top 5% of entries
         {
           pString = m_debugDataQueue.RemoveHead();
           delete pString;
         }
         sel = m_commsListBox.GetCurSel();
         m_commsListBox.AddString("<< Debugger buffer overflow>>");   // CListBox
         m_commsListBox.SetCurSel(sel - 1);

      }
      LeaveCriticalSection(&debuggerCritSection);
      return;
   }

   EnterCriticalSection(&debuggerCritSection);
   
   // move all strings into the list-box control
   while (!m_debugDataQueue.IsEmpty())
   {
      pString = m_debugDataQueue.RemoveHead();
      //check when to "rotate" the list-box entries
      if (m_commsListBox.GetCount() > MAX_COMMSDEBUGGER_ENTRIES)
      {  // delete a couple of them from the top if the list so that the 
         // list never gets very long, as this affects list performance badly
         sel = m_commsListBox.GetCurSel();
         for (killed=0;killed < (MAX_COMMSDEBUGGER_ENTRIES/20) ; killed++) // cull topmost 5% of entries
            m_commsListBox.DeleteString(0); // 1 for now
         if (!m_commsTobottom)
            m_commsListBox.SetCurSel(sel - killed);
      }
      try {
         // OK, add the item now
         if ((pString) && (!pString->IsEmpty()))
         {
            m_commsListBox.AddString(*pString);   // CListBox
            delete pString;        // free up memory used
         }
      }
      catch(...)
      {
      CString msg;
         msg.Format("INTERNAL APPLICATION ERROR FILE %s LINE: %d\n%s\n%s", 
            __FILE__, __LINE__, __MY_APPVERSION__, __DATE__);
         m_commsListBox.AddString(msg);   // CListBox
      }
      count++;
   }
   if (m_commsTobottom)
      m_commsListBox.SetCurSel(m_commsListBox.GetCount()-1);
   LeaveCriticalSection(&debuggerCritSection);
}

// -------------------------------- OnPausecomms -----------------------------
// TOGGLE
// in pause, no new entries to the list are added, this stops the stuff from 
// scrolling off completely.
void CMOD_simDlg::OnPausecomms() 
{
	// TODO: Add your control notification handler code here
   EnterCriticalSection(&debuggerCritSection);
	m_paused = !m_paused;

   if ((m_commsTobottom)&&(!m_paused))
      m_commsListBox.SetCurSel(m_commsListBox.GetCount()-1);
   LeaveCriticalSection(&debuggerCritSection);

   SetDlgItemText(IDC_PAUSECOMMS, (m_paused? "Resume" : "Pause"));
   
   // gray/ungray the "tracking" button as needed
   GetDlgItem(IDC_TRACKCOMMS)->EnableWindow(!m_paused);
}


// ----------------------------------- OnTrackcomms -----------------------------
// TOGGLE
// turns the feature for selecting last row on/off
void CMOD_simDlg::OnTrackcomms() 
{
	// TODO: Add your control notification handler code here
   EnterCriticalSection(&debuggerCritSection);
	m_commsTobottom = !m_commsTobottom;

   if ((m_commsTobottom)&&(!m_paused))
      m_commsListBox.SetCurSel(m_commsListBox.GetCount()-1);
   LeaveCriticalSection(&debuggerCritSection);

   SetDlgItemText(IDC_TRACKCOMMS, (m_commsTobottom? "Stop tracking" : "Track comms"));
}

// ------------------------------- OnVScroll -----------------------------
// scroll thru the station tick-boxes
void CMOD_simDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
int min,max,pos;
int firstTick, lastTick, stationID;

   if ((nSBCode == SB_LINEUP)||(nSBCode == SB_LINEDOWN))
   {
      // retrieve all tick enable states
      firstTick = GetFirstVisibleStation();
	   lastTick = GetLastVisibleStation();
      for (stationID = firstTick; stationID<lastTick;stationID++)
      {
         ASSERTMICROBOX(stationID-firstTick)
         m_microTicksEnableState[stationID] = m_microTicks[stationID-firstTick].GetCheck();
      }
   }
   // now do the scroll
   pos = pScrollBar->GetScrollPos();
   switch(nSBCode)
   {
      case SB_LINEUP:
         if (pos)
            pScrollBar->SetScrollPos(pos-1);
         break;
      case SB_LINEDOWN:
         pScrollBar->GetScrollRange(&min,&max);
         if (pos < max)
            pScrollBar->SetScrollPos(pos+1);
         break;
      default:
         break;
   }
	
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
   if ((nSBCode == SB_LINEUP)||(nSBCode == SB_LINEDOWN))
   {
      // re-organize as needed
      ConfigureStationButtons();
   }
}

// -------------------------------- GetFirstVisibleStation ---------------------
int CMOD_simDlg::GetFirstVisibleStation()
{
int firstTick;
int pos;

   pos = m_vScrollBarStations.GetScrollPos();
   firstTick = (int)((1.0*STATIONTICKBOXESMAX/(MAX_SCROLLINDEX+1))*pos);
   return(firstTick);
}

// -------------------------------- GetLastVisibleStation -----------------------
int CMOD_simDlg::GetLastVisibleStation()
{
int lastTick;

   lastTick = GetFirstVisibleStation()+STATIONTICKBOXES-1; // 
   if (lastTick>=STATIONTICKBOXESMAX)
      lastTick = STATIONTICKBOXESMAX-1;
   return(lastTick);
}


// --------------------------------- EnablePLCStation ---------------------------
void CMOD_simDlg::EnablePLCStation(int station, int state)
{
int firstTick;
   m_microTicksEnableState[station] = state;
   firstTick = GetFirstVisibleStation();
   if ((station >= firstTick) && (station < GetLastVisibleStation()))
   {
      ASSERTMICROBOX(station-firstTick)
      m_microTicks[station-firstTick].SetCheck(state);
   }
};




// ----------------------------- GetToolTipForStation -----------------------------
LPCTSTR CMOD_simDlg::GetToolTipForStation(int index)
{
static char tip[80];

   if (!index)
      sprintf(tip, "Broadcast station %s", (m_microTicksEnableState[index]?"enabled":"disabled"));
   else
      if (m_seperateRegisters)
      { // tell user the start/end register # for this station
         sprintf(tip, "Station %d %s (I/O %d thru %d)", 
                         index, 
                         (m_microTicksEnableState[index]?"enabled":"disabled"),
                         index * m_numSeperate,
                         (1+index) * m_numSeperate -1
                );
      }
      else
         sprintf(tip, "Station %d %s", index, (m_microTicksEnableState[index]?"enabled":"disabled"));
   return(tip);
}

// ------------------------------- ConfigureStationButtons -------------------------
// Enables/disables sets description text inside, and even hides the station boxes.
// Sets tooltips for each box.
void CMOD_simDlg::ConfigureStationButtons()
{
int firstTick, lastTick;
int pos, count=0;
CString tickText;
int stationIndex;
   
   //work out first and last tick-box
   pos = m_vScrollBarStations.GetScrollPos();
   firstTick = GetFirstVisibleStation();
   lastTick = GetLastVisibleStation();

   for (stationIndex = firstTick; stationIndex<lastTick ; stationIndex++) 
   {
      ASSERTMICROBOX(stationIndex-firstTick)
      m_microTicks[stationIndex-firstTick].ShowWindow(SW_SHOW);
      tickText.Format("%02d", stationIndex);
      m_microTicks[stationIndex-firstTick].SetCheck(m_microTicksEnableState[stationIndex]); //ver 3.1
      m_microTicks[stationIndex-firstTick].SetWindowText(tickText);
      m_microTicks[stationIndex-firstTick].SetBorderState(m_microTicksBackState[stationIndex]);

      m_microTicks[stationIndex-firstTick].InvalidateRect(NULL);
      //tooltips
      m_ToolTip.UpdateTipText(GetToolTipForStation(stationIndex), &m_microTicks[stationIndex-firstTick], 0);
      count++;
   }
   while (count < STATIONTICKBOXES) // if the bottom row is only partial, hide some controls
   {
      ASSERTMICROBOX(stationIndex-firstTick)
      m_microTicks[stationIndex-firstTick].ShowWindow(SW_HIDE);
      m_microTicks[stationIndex-firstTick].InvalidateRect(NULL);
      stationIndex++;
      count++;
   }
}


// -------------------------- SetTickBorderState -------------------
// set enable/disable for this tick
// if the box is not visible, the function does nothing
void CMOD_simDlg::SetTickBorderState(int stationID, BOOL active)
{
int firstTick;
   firstTick = GetFirstVisibleStation();

   if ((stationID >= firstTick) && (stationID <=GetLastVisibleStation()))
   {
      ASSERTMICROBOX(stationID-firstTick) 
      m_microTicks[stationID-firstTick].SetBorderState(active);
   }
}

// ------------------------- InvalidateTick -----------------------------
// repaint station # X if visible only
void CMOD_simDlg::InvalidateTick(int stationID)
{
int firstTick;
   firstTick = GetFirstVisibleStation();

   if ((stationID >= firstTick) && (stationID <= GetLastVisibleStation()))
   {
      ASSERTMICROBOX(stationID-firstTick)
      m_microTicks[stationID-firstTick].InvalidateRect(NULL);
   }
}

// ------------------------ StationEnabled ------------------------------
// return state of any station
BOOL CMOD_simDlg::StationEnabled(int stationID)
{
int firstTick;
   // Return the station's control if in view, since the refresh of 'm_microTicksEnableState'
   // may not yet have occured.
   firstTick = GetFirstVisibleStation();
   if ((stationID >= firstTick) && (stationID <=GetLastVisibleStation()))
   {
      ASSERTMICROBOX(stationID-firstTick)
      return(m_microTicks[stationID-firstTick].GetCheck());
   }
   
   // station is not currently displayed, return it from the full listing
   return(m_microTicksEnableState[stationID]);
}

//
// handle dialog re-sizing
//

// --------------------------------------- OnSize -----------------------------
void CMOD_simDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
   //Move controls
   m_resizer.Move();

   // move the toobar, since it is created at runtime, and not part of the Dlg resource
   if (m_ToolBar.m_hWnd)
   {
      CWindowRect toolRect(GetDlgItem(IDB_TOOLBARID));
      ScreenToClient(toolRect);
      m_ToolBar.MoveWindow(toolRect);
      
      // make correct controls visible/invisible again
	   m_registerShow = !m_registerShow;   //invert, since the toggle will toggle back again
      OnToggleDisplay();
   }
}

// ----------------------------- OnGetMinMaxInfo ------------------------------------
// handle a WM_GETMINMAXINFO
void CMOD_simDlg::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI )
{
   // Return the minimum window size I allow
   lpMMI->ptMinTrackSize.x = 640;
   lpMMI->ptMinTrackSize.y = 220;
}

// ----------------------------- OnSelchangeProtocol -------------------------------
void CMOD_simDlg::OnSelchangeProtocol() 
{
DWORD sel;

   // cannot swap protocols while a protocol is being set up still
   if (m_busyCreatingServers)
   {
      MessageBeep(MB_ICONEXCLAMATION);
      m_protocolCombo.SetCurSel(m_selectedProtocol);
      return;
   }

   sel = m_protocolCombo.GetCurSel();
   if (sel == LB_ERR)
      return;

   if (sel != m_selectedProtocol)
   {
   CString msg, lbText;
      m_protocolCombo.GetLBText(sel, lbText);
      msg.Format("Change protocol to %s?", lbText);
      if (IDYES == MessageBox("Click 'Yes' to select the protocol, or 'No' to cancel.", msg, MB_YESNO))
      {
         m_selectedProtocol = sel;
         SaveApplicationSettings();

         // init the ports by re-initializing/creating the relevant CommsProcessor array
         UnloadCommsProcessors();

         PLCMemory.SetSize(GetNumMemoryAreas());
         SetMemAreaSizes();

         InitiateCommsProcessors(m_selectedProtocol);
      }
   }

   // It is currently not possible to change protocols in the comms view, 
   // but just for in future.
   if ((NULL == m_pServerRS232Array) && (!m_registerShow))
   {
      GetDlgItem(IDC_LAMPCTS)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_LAMPDSR)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_LAMPRING)->ShowWindow(SW_HIDE);
   }
   
   // determine which application icon to use
   switch (m_selectedProtocol)
   {
      case PROTOCOL_SELMOD232:
	      SetIcon(m_hIconMB, TRUE);			// Set big icon
	      SetIcon(m_hIconMB, FALSE);		   // Set small icon
         break;
      case PROTOCOL_SELABMASTER232:
	      SetIcon(m_hIconJOY, TRUE);			// Set big icon
	      SetIcon(m_hIconJOY, FALSE);		   // Set small icon
         break;
      case PROTOCOL_SELAB232:
	      SetIcon(m_hIconDF1, TRUE);			// Set big icon
	      SetIcon(m_hIconDF1, FALSE);		   // Set small icon
         break;
      case PROTOCOL_SELMODETH:
      default:
	      SetIcon(m_hIconETH, TRUE);			// Set big icon
	      SetIcon(m_hIconETH, FALSE);		   // Set small icon
         break;
   }
   m_protocolCombo.SetCurSel(m_selectedProtocol);
   // reset the port open/close toolbar button to be 'Close'
   m_ToolBar.SetButtonInfo(10, IDC_CLOSE,  TBBS_BUTTON, 13);


   // refresh the list-view control
   OnSelchangeComboArea();

}

// ----------------------------------- OnTraining -----------------------------
// Training sim "diagnostics" animating dialog.
void CMOD_simDlg::OnTraining() 
{
CTrainingSimDlg   dlg;

   dlg.DoModal();
}

// ------------------------------- MasterBusy ----------------------------
// implements the scripting host method.
// maps to: VT_I4 TransacationBusy (VTS_NONE)
// 
LONG CMOD_simDlg::MasterBusy()
{
CCommsProcessor *pCPU;

   pCPU = (CCommsProcessor *)m_pServerRS232Array->GetAt(0);
   if (!pCPU->IsKindOf(RUNTIME_CLASS(CCommsProcessor)))
      return(TRUE);        // pointer duff?
   
   if (!pCPU->SupportsMaster())
      return(TRUE);      // it is a fault to TX on this driver
   
   return(pCPU->IsMasterBusy());    // ask him if he is busy
   // non-masters will always return busy
}

// ------------------------------ MasterTransmit ----------------------------
// scripting host method:
// implements VT_I4,  TransmitRegisters(VTS_I2 VTS_I2 VTS_I4 VTS_I4 VTS_I4)
LONG CMOD_simDlg::MasterTransmit(short sourceStation, 
                                 short destinationStation, 
                                 long file_areaNumber, 
                                 long startRegister, 
                                 long numRegisters)
{
   // protocol sel specific
   if ((m_selectedProtocol == PROTOCOL_SELABMASTER232) ||
       (m_selectedProtocol == PROTOCOL_SELAB232))
   {
   CAB232CommsProcessor *p232CPU;
   DWORD startTicks;
   BOOL sendOK;

      p232CPU = (CAB232CommsProcessor *)m_pServerRS232Array->GetAt(0);
      if (!p232CPU->IsKindOf(RUNTIME_CLASS(CAB232CommsProcessor)))
         return(-2);    // internal SYSTEM fault
      if ((p232CPU->m_masterBusy) || (!p232CPU->IsMaster()))
         return(-3);    // invalid mode or busy

      // start the clock
      startTicks = GetTickCount();
      sendOK = p232CPU->SendPLCBlock((BYTE)sourceStation,
                                     (BYTE)destinationStation,
                                     p232CPU->GetNextTransactionID((BYTE)destinationStation),
                                     (WORD)file_areaNumber, 
                                     (WORD)startRegister, 
                                     (WORD)numRegisters);
      if (!sendOK)
         return(-4); // comms error occured
      
      while (p232CPU->m_masterBusy)
      {
         // keep waiting for comms to complete
         Sleep(20);
      }
      return ((LONG)(GetTickCount()- startTicks));
   }
   return(-1); // not possible with current protocol/settings
}

// ----------------------------- OnClearcomms -----------------------------
// Clear the "comms" list-box
void CMOD_simDlg::OnClearcomms() 
{
   EnterCriticalSection(&debuggerCritSection);
   m_commsListBox.ResetContent();
   LeaveCriticalSection(&debuggerCritSection);
}

// ------------------------- OnShowcommstimeClicked ------------------------
// Show event times on all "comms debug log" toggle
void CMOD_simDlg::OnShowcommstimeClicked() 
{
   EnterCriticalSection(&debuggerCritSection);

   if (IsDlgButtonChecked(IDC_SHOWCOMMSTIME))
      m_commsTimes = TRUE;
   else
      m_commsTimes = FALSE;
   LeaveCriticalSection(&debuggerCritSection);
}



// ---------------------------------- OnInJecterror --------------------------
// hotkey Ctrl-J inject any configured error into the system now
void CMOD_simDlg::OnInJecterror() 
{
CMOD232CommsProcessor *pModCPU;
CAB232CommsProcessor *pABCPU;
CRS232Noise *pNoiseObject = NULL;
CRS232Port  *pPort = NULL;
BYTE errBuffer[80];
CHAR debugBuffer[MAX_DEBUG_STR_LEN];

   if (NULL != m_pServerRS232Array)
   {
      pModCPU = (CMOD232CommsProcessor *)m_pServerRS232Array->GetAt(0);
      pABCPU = (CAB232CommsProcessor *)pModCPU;
      if (pModCPU->IsKindOf(RUNTIME_CLASS(CMOD232CommsProcessor)))
      {
         pNoiseObject = &pModCPU->m_NoiseSimulator;
      }
      else
      {
         pNoiseObject = &pABCPU->m_NoiseSimulator;
      }
      // ok
      pPort = (CRS232Port  *)m_pServerRS232Array->GetAt(0);

      // inject a random char or error into the buffer (does interlock with the main comms thread)
      
      //disabled in v7.7
      errBuffer[0] = getrandom(0,255);
      pNoiseObject->InjectErrors(pPort, errBuffer, 1, debugBuffer);
   }
}


void CMOD_simDlg::OnOpenPort() 
{

	if(m_loadedProtocol == PROTOCOL_SELNONE) // port not loaded
      InitiateCommsProcessors(m_selectedProtocol);

	// update GUI
   m_ToolBar.SetButtonInfo(10, IDC_CLOSE,  TBBS_BUTTON, 13);
   m_ToolBar.GetToolBarCtrl().EnableButton(3, TRUE);
   m_ToolBar.GetToolBarCtrl().EnableButton(4, TRUE);
   m_ToolBar.GetToolBarCtrl().EnableButton(5, TRUE);
   m_ToolBar.GetToolBarCtrl().EnableButton(9, TRUE);
}


void CMOD_simDlg::OnClosePort() 
{

	ASSERT(m_loadedProtocol != PROTOCOL_SELNONE);

   UnloadCommsProcessors();
   m_loadedProtocol = PROTOCOL_SELNONE;
	// update GUI
   m_ToolBar.SetButtonInfo(10, IDC_OPEN,  TBBS_BUTTON, 6);
   m_ToolBar.GetToolBarCtrl().EnableButton(3, FALSE);
   m_ToolBar.GetToolBarCtrl().EnableButton(4, FALSE);
   m_ToolBar.GetToolBarCtrl().EnableButton(5, FALSE);
   m_ToolBar.GetToolBarCtrl().EnableButton(9, FALSE);
}


/* Open/Close comm channel-server ports.
 * this is a port open/close toggle - not used by the toolbar, since the toolbar switches it's 
 * message-handler ID on the fly between OnClosePort() and OnOpenPort() .
 */
void CMOD_simDlg::OnTogglePortState() 
{
	if (m_loadedProtocol == PROTOCOL_SELNONE)
      OnOpenPort();
   else
      OnClosePort();
}

// ------------------------------- OnCsvImportPop --------------------------
// pop the CSV import dialog
// Vinay
void CMOD_simDlg::OnCsvImportPop() 
{
CCSVFileImportDlg dlg;

   dlg.m_importFolder = m_importFolder;//"..\\CSVdata";
   dlg.m_logFileName = m_logFileName;//"..\\CSVData\\csreport.log";
   dlg.m_csvImportEnable =  m_csvImportEnable;
   if (IDOK == dlg.DoModal())
   {
      AddCommsDebugString(dlg.m_importFolder);
      AddCommsDebugString(dlg.m_logFileName);

      m_importFolder = dlg.m_importFolder;
      m_logFileName = dlg.m_logFileName;
      m_csvImportEnable =  dlg.m_csvImportEnable;
   }   
}
