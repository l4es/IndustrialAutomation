/////////////////////////////////////////////////////////////////////////////
//
// FILE: MOD_simDlg.h : headder file
//
// Code is: (c) Embedded Intelligence Ltd. 1993,2009
// Author: Conrad Braam. 
// Web: www.plcsimulator.org
// See: _README.CPP
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TI_SIMDLG_H__C69D2C79_8562_4428_A549_A4D258F9CF4F__INCLUDED_)
#define AFX_TI_SIMDLG_H__C69D2C79_8562_4428_A549_A4D258F9CF4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMOD_simDlg;
class CControllerMemory;
class CRS232Noise;

#include "SimCmdLine.h"
#include "About.h"

extern CMOD_simDlg * pGlobalDialog;    // global ptr to the dialog
extern CControllerMemory PLCMemory;
extern WORD PLCMemPrefixes[MAX_MOD_MEMTYPES];

#define STATIONTICKBOXES      52    // 56
#define MAX_SCROLLINDEX       5     // 0 thur 5 = 256/6

#define PROTOCOL_SELMOD232       0
#define PROTOCOL_SELMODETH       1
#define PROTOCOL_SELAB232        2
#define PROTOCOL_SELABMASTER232  3

#define PROTOCOL_SELMAX       (PROTOCOL_SELABMASTER232) // last protocol
#define PROTOCOL_SELNONE      -1



// This function is a patch that overcomes MFC bug of
// tooltips that don't show up on in disabled controls 
// if modal dialogs. Call this function in your overidable of
// PreTranslateMessage, before calling the inherited one.
void HandleTooltipsActivation(MSG *pMsg, CWnd *This, CWnd *disabledCtrls[], int numOfCtrls, CToolTipCtrl *pTooltip);


// Vinay
// this class implements register get/set for various protocols (in this case modbus)
class CRegisterUpdaterIMP : public CRegisterUpdaterIF
{
public:
   CRegisterUpdaterIMP(CMOD_simDlg *Dlg) :pDlg(Dlg) {}
   void DebugMessage(LPCTSTR message);
   BOOL SetRegister(LONG index, WORD value);

   BOOL ModbusClone();

private:
   CMOD_simDlg *pDlg;
};


/////////////////////////////////////////////////////////////////////////////
// CMOD_simDlg dialog

class CMOD_simDlg : public CDialog
{
// Construction
public:
	CMOD_simDlg(CWnd* pParent = NULL);	// standard constructor
   ~CMOD_simDlg();

   void InitGlobals();
   
   // GUI methods
   void FillComboBox(UINT id, char ** strTable, const DWORD currentsetting);
   BOOL MyProcessingLoop(int loops);
   void SetupMySystemMenu(CMenu *pSysMenu);
   void SetupMyToolBar();
   void DoPlantSimulation();
   BOOL InPlantSimulation() {return(m_plantAnimation);};
   void GetRegisterName(LONG area, LONG wordNumber, CString &registerName);

   void UpdateStatusLine();
   void DoAnimations();
   void ZeroRegisterValues() { m_zeroValues = TRUE;};

   BOOL RunAnimationScript(LPCTSTR moduleName, LPCTSTR moduleText = NULL);
   void RedrawListItems(int area, int nFirst, int nLast) 
      {if (area == m_listCtrlData.GetAreaDisplayed()) m_listCtrlData.RedrawItems(nFirst, nLast);};
   int  GetListDisplayedWidth() {return (m_listCtrlData.GetDataColumnCount());};
   BOOL SetAlphaBlendSolid();
   BOOL SetAlphaBlendHigh();
   BOOL SetAlphaBlendLow();

   LONG GetNumMemoryAreas();

   WORD PLCMemDisplayWidths(LONG typeIx);
   BOOL PLCIsBitMemory(LONG typeIx);
   void InitiateCommsProcessors(DWORD protocol);
   void UnloadCommsProcessors();
   void SetMemAreaSizes();
   void GetConnectionIPAddresses(CString &local, CString &remote);


   void SetupListViewControl(const DWORD cols);
   void LoadApplicationSettings();
   void SaveApplicationSettings();
   BOOL SendToClipboard(const CString & s);

   DWORD GetWordValue(const LONG area, const DWORD startBit, const DWORD numBits);
   void  SetWordValue(const LONG area, const DWORD startBit, const DWORD dwValue, const DWORD numBits);
   WORD GetAnimationOnPeriod() { return((WORD)m_animationPeriod);};

   void SetAddressFormatHex(BOOL formatHEX = TRUE);
   BOOL IsAddressFormatHex();
   BOOL IsClone() { return (m_isClone);};

   void SetDisplayToggleButton() {
      SetDlgItemText(IDB_TOGGLEDISP, (m_registerShow ?"Comms":"Registers"));};

   //////////////////////////////////////////////////////////
   // SCRIPTING object support functions
   //
   LPCTSTR GetAnimationScriptFile() {return (m_animationScriptFile);};
   BOOL ScriptIsRunning() { return(m_ScriptProxy.IsRunning());};
   LONG GetLastScriptRuntime() { return(m_lastRuntime);};
   void SetLastRuntime(LONG run) { m_lastRuntime = run;};
   void SetLastScriptErrorString(LPCTSTR str) { m_lastErrorString=str; };
   LONG MasterTransmit(short sourceStation, 
                       short destinationStation, 
                       long file_areaNumber, 
                       long startRegister, 
                       long numRegisters);
   LONG MasterBusy();

   void AddCommsDebugString(LPCTSTR string);
   
   // enables it if the station is visible, and sets the state
   void EnablePLCStation(int station, int state);

   void SendModemStatusUpdate(DWORD status) { OnModemStatusUpdate(status);}
   void UpdateCharactersSent() {OnCharactersSent();}
   void UpdateCharactersReceived() {OnCharactersReceived();}
   // comms methods
   void PacketsReceivedInc();
   void PacketsSentInc();

private:
   // event handlers
   void OnCharactersReceived() { m_portActivityRX = TRUE;};
   void OnCharactersSent() { m_portActivityTX = TRUE;};
   void OnModemStatusUpdate(DWORD status) { m_portModemStatus = status;}


   //////////////////////////////////////////////////////////////
   // DATA members
public:
   CString m_lastErrorString;
	CResizer m_resizer;
   BOOL     m_Unloading;   // unloading comms

   DWORD GetPLCMemoryLimit(DWORD area);
   DWORD GetPLCMemoryValue(DWORD area,DWORD offset, WORD type);
	DWORD m_numSeperate;       // number of registers allocated per station ID starting at 1
	DWORD	m_seperateRegisters; // seperate registers enabled/disabled?
   DWORD m_selectedProtocol;    // PROTOCOL_SELMOD232 etc values
   DWORD m_loadedProtocol;      // last used protocol

   BOOL StationEnabled(int stationID); 
   int  GetFirstVisibleStation();          // 1st visible station indicator
   int  GetLastVisibleStation();           // last indicator (always <=255)
   void SetTickBorderState(int stationID, BOOL active);  // activity on/off
   void InvalidateTick(int stationID);    // repaints the station if it is visible
   LPCTSTR GetToolTipForStation(int index);      // return tick-mark's tooltip

   CString &LogFileName();
   LPCTSTR GetScriptFileName() {return(m_animationScriptFile);};

   void SetEnableHTMLGUI(bool en) { m_enableHTMLGUI = en;};

   LONG        m_microTicksCountDown[STATIONTICKBOXESMAX];  // "GUI" delay on activity
   BOOL        m_microTicksBackState[STATIONTICKBOXESMAX];  // station was active, back-light it
   BOOL        m_microTicksEnableState[STATIONTICKBOXESMAX];// station enabled/disabled

   COLORREF    m_mTickColorEnabled;    // border color
   COLORREF    m_mTickColorDisabled;   // border color
   COLORREF    m_mTickColorActive;     // background color
   COLORREF    m_mTickColorInactive;   // background color
   COLORREF    m_mTickColorText;       // text color for enable/disable
   COLORREF    m_mTickDisColorText;

   //CControllerMemory m_TestRAM;

   CCommsLamp m_lampRX, m_lampTX;   // communication activity indicator lamps
   CCommsLamp m_lampTraining;       // training sim on/off
   CCommsLamp m_lampCTS, m_lampDSR, m_lampRING;

   // RS232 members
   CServerRS232Array * m_pServerRS232Array;  // 
   CRS232Noise *m_pNoiseSettings;

   DWORD m_ServerRSState;           // last incomming state
	DWORD	m_responseDelay;           // delay to simulate
   DWORD m_MOSCADchecks;            // moscad unit
   DWORD m_modifyThenRespond;
   DWORD m_disableWrites;            // test
   BOOL  m_isClone;
   DWORD m_PLCMaster;
   DWORD m_useBCC;
   // CDB added: ver 3.1
   DWORD m_autoLoad;
   DWORD m_startAsOff;
   //
   DWORD m_ABMasterSourceStation;     // SCC station ID
   DWORD m_ABMasterDestStation;       // destination (SCADA) station ID
   DWORD m_ABMasterNumFiles;        // # shields starting from AB file #101
   DWORD m_ABMasterRunMode;           // emulate ON/OFF
   DWORD m_ABMasterReadVariables;
   DWORD m_ABMasterWriteVariables;
   DWORD m_ABmasterIDLETime;
   DWORD m_ABtimeoutValue;

   CHAR  m_portNameShort[MAX_COMPORT_NAME];
   DWORD m_baud;
   DWORD m_byteSize;
   DWORD m_parity;
   DWORD m_stopBits;
   DWORD m_rts;

   // Ethernet members
   CServerSocketArray * m_pServerSockArray;  // the listen sockets live in here
   DWORD  m_ServerSocketState;       // last incomming state
	DWORD  m_localPort;               // local port to server from
   DWORD  m_otherPort;               // Secondary for switch over.
   DWORD  m_localId;                 // local Id
   DWORD  m_linger;                  // linger on socket closes Y/N
   DWORD  m_numServers;              // # listens/threads
   BOOL   m_busyCreatingServers;     // interlocking flag =TRUE during server creation

   CRegistrationTest m_registration;
   CString m_registeredUser, m_registeredKey;

   CMemoryEditorList m_listCtrlData;// the big listview control

   CSimCmdLine    m_commandLine; // CCommandLineInfo object

protected:
   CRITICAL_SECTION  dispCritSection;

   CDragSizerBmp  m_dragCorner;        // bottom right corner

   LONG  alphaBlendFactor;
   BOOL  alphaBlendON;
   LONG  m_portToggleMnuIndex;

   BOOL  m_portActivityRX, m_portActivityTX;    // activity lamps
   DWORD m_portModemStatus;                     // modem status indicators.

   DWORD m_PacketsReceived;         // stats
   DWORD m_PacketsSent;             // stats
   //BOOL  m_addressFormatHEX;        // display addreses in HEX/decimal
   WORD  m_dataFormat;              // data : decimal/..hex/long.../float etc...
   DWORD  m_startUpRegisterArea;
   // PLC sim
   DWORD	m_numInputs;
	DWORD	m_numOutputs;
	DWORD	m_numHolding;
	DWORD	m_numExtended;
	DWORD	m_numAnalogs;
   DWORD m_PDUSize;

   // accelerators
   HACCEL m_hAccel;
   LRESULT OnGetDefID(WPARAM wp, LPARAM lp); // handle <ENTER> key

   // TOOLTIPS
   CToolTipCtrl    m_ToolTip;
   // TOOLBAR
   CToolBar m_ToolBar;
   BOOL     OnTTN_NeedText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
    
   void  ToggleOnTop();    // "stay on top" done in here
   BOOL  m_stayOnTop;
   LONG  m_appSettingAdressHex;  // load/save only

   // Vinay
	CString	m_importFolder;
	CString	m_logFileName;
   LONG     m_csvImportEnable;
   CCSVTextImporter  m_CSVImporter;


   // STATION Activity indicators
private:
   void ConfigureStationButtons();
   //data
   CMicroTick  m_microTicks[STATIONTICKBOXES];
   
protected:
//   BOOL  m_busyCreatingServers;     // interlocking flag =TRUE during server creation
   BOOL  m_registerShow;            // default mode, show registers else show comms
   BOOL  m_commsTimes;
   
   CTypedPtrList<CPtrList, CString*> m_debugDataQueue; // array of new debug msgs
   CRITICAL_SECTION  debuggerCritSection;             // comms debugger

   LONG  m_animationIncValue;       // data SIM
   BOOL  m_animationON;             // data SIM
   BOOL  m_plantAnimation;
   BOOL  m_animationBYTE;           // data SIM
   BOOL  m_animationWORD;           // data SIM
   WORD  m_guardByte1;
   BOOL  m_zeroValues;              // data SIM

   LONG  m_animationCounter;        // data SIM
   LONG  m_animationRefreshes;      // data SIM
   DWORD m_animationPeriod;
   CString m_animationScriptFile;
   // HTML GUI params
   DWORD   m_enableHTMLGUI;     
   DWORD   m_HTMLUpdateRate;    
   CString m_outputHTMLFilename;
   CString m_inputHTMLFilename; 
   
   BOOL m_scriptEngineInitilized;
	CMyHostProxy	m_ScriptProxy;
   CString m_animationScriptText;
   BOOL    m_reloadAnimationScript;
   LONG    m_lastRuntime;


protected:
   void UpdateCommsList();

   BOOL m_paused;
   BOOL m_commsTobottom;
// Dialog Data
	//{{AFX_DATA(CMOD_simDlg)
	enum { IDD = IDD_TI_SIM_DIALOG };
	CComboBox	m_protocolCombo;
	CScrollBar	m_vScrollBarStations;
	CListBox	   m_commsListBox;
	CComboBox	m_dataFormatCombo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMOD_simDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	void OnMyContextMenu(CWnd*, CPoint point);
	//HICON m_hIconMOD;
	HICON m_hIconMB;
	HICON m_hIconETH;
	HICON m_hIconDF1;
	HICON m_hIconJOY;

public:
	// Generated message map functions
	//{{AFX_MSG(CMOD_simDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSettings();
	afx_msg void OnViewhex();
	afx_msg void OnViewdec();
	afx_msg void OnSelchangeComboArea();
	afx_msg void OnSimulate();
	afx_msg void OnZeroes();
	// VC 6 MFC version
	afx_msg void OnLoad();
#if     _MSC_VER > 1200	//VC 9
	afx_msg LRESULT OnLoad_(WPARAM,LPARAM);
#endif
	afx_msg void OnSave();
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnClone();
	afx_msg void OnSelchangeDataFormat();
	afx_msg void OnAboutboxBn();
	afx_msg void OnAlphablendFlip();
	afx_msg void OnNoise();
	afx_msg void OnEmulation();
	afx_msg void OnAbout();
	afx_msg void OnTransparencyToggle();
	afx_msg void OnToggleDisplay();
   afx_msg void OnCopyToClipBoard();
	afx_msg void OnPausecomms();
	afx_msg void OnTrackcomms();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeProtocol();
	afx_msg void OnTraining();
	afx_msg void OnClearcomms();
	afx_msg void OnShowcommstimeClicked();
	afx_msg void OnInJecterror();
	afx_msg void OnOpenPort();
	afx_msg void OnClosePort();
	afx_msg void OnTogglePortState();
	afx_msg void OnCsvImportPop();
	//}}AFX_MSG
   afx_msg void OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI );

public:
   CHTMLOutput m_htmlOutput;
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TI_SIMDLG_H__C69D2C79_8562_4428_A549_A4D258F9CF4F__INCLUDED_)
