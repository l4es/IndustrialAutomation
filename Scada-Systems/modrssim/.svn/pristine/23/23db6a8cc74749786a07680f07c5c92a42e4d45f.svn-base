/////////////////////////////////////////////////////////////////////////////
//
// FILE: ServerSettingsDlg.cpp : implementation file
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
// This Class implements a "settings" dialog to allow the port to be configured
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_RSsim.h"
#include "ServerSettings232Dlg.h"
#include "mod_RSsimDlg.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMOD_simDlg * pGlobalDialog;

// contents of all the combo-boxes are here
//
DWORD    BaudTable[] = // please add any extra ones if U need in future
{
   110,     // CBR_110 
   300,
   600,
   1200,
   2400,
   4800,
   9600,
   14400,
   19200,
   38400, 
   56000,
   57600, //64800,
   115200,
   128000,
   256000 
};


DWORD    ParityTable[] =
{
   NOPARITY,
   ODDPARITY,
   EVENPARITY,
   MARKPARITY,
   SPACEPARITY
};

CHAR *   ParityTableStr[] =   // this table is tied to the one before it
{
   "None",
   "Odd",
   "Even",
   "Mark",
   "Space"
};

DWORD    DataBitTable[] =
{
   SEVENDATABITS,       // # defined as 0
   EIGHTDATABITS        // # defined as 2
};

CHAR *   DataBitTableStr[] = // this table is tied to the one before it
{
   "7",
   "8"
};

DWORD    StopBitTable[] =
{
   ONESTOPBIT,       // # defined as 0
   ONE5STOPBITS,
   TWOSTOPBITS       // # defined as 2
};

CHAR *   StopBitTableStr[] =  // this table is tied to the one before it
{
   "1",
   "1.5",
   "2"
};

DWORD    RTSLineTable[] =
         {
            RTS_CONTROL_DISABLE,       // 0
            RTS_CONTROL_ENABLE,        // 1
            RTS_CONTROL_HANDSHAKE,     // 2
            RTS_CONTROL_TOGGLE         // 3
         };

CHAR *   RTSLineTableStr[] =
{
   "Disable",
   "Enable",
   "h.shake",
   "toggle"
};



/////////////////////////////////////////////////////////////////////////////
// CServerSettings232Dlg dialog


CServerSettings232Dlg::CServerSettings232Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerSettings232Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CServerSettings232Dlg)
	m_responseDelay = 0;
	m_MOSCADchecks = FALSE;
	m_autoLoad = FALSE;
	m_startAsOff = FALSE;
	m_ABMastermode = FALSE;
	//}}AFX_DATA_INIT
}


void CServerSettings232Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerSettings232Dlg)
	DDX_Control(pDX, IDC_CHECKTYPE, m_checkTypeCombo);
	DDX_Control(pDX, IDC_RTS_COMBO, m_rtsCombo);
	DDX_Control(pDX, IDC_STOP_BITS, m_stopBitCombo);
	DDX_Control(pDX, IDC_PORT, m_portCombo);
	DDX_Control(pDX, IDC_PARITY, m_parityCombo);
	DDX_Control(pDX, IDC_DATA_BITS, m_dataBitsCombo);
	DDX_Control(pDX, IDC_BAUDRATE, m_baudCombo);
	DDX_Text(pDX, IDC_RESPONSEDELAY, m_responseDelay);
	DDV_MinMaxUInt(pDX, m_responseDelay, 0, 10000);
	DDX_Check(pDX, IDC_MOSCAD, m_MOSCADchecks);
	DDX_Check(pDX, IDC_AUTOLOAD, m_autoLoad);
	DDX_Check(pDX, IDC_STARTOFF, m_startAsOff);
	DDX_Check(pDX, IDC_ABMASTER, m_ABMastermode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServerSettings232Dlg, CDialog)
   ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnTTN_NeedText )
	//{{AFX_MSG_MAP(CServerSettings232Dlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerSettings232Dlg message handlers

void CServerSettings232Dlg::OnOK() 
{
//BOOL  errorFlag;
WORD  selection;
CHAR  shortPortName[MAX_COMPORT_NAME]; // displayable name for port
// COM ports >COM9 Windows uses a different naming format, hence this 
// little catch

	// TODO: Add extra validation here

	// copy all combo values into our data
   // get port name
   GetDlgItemText(IDC_PORT,
                  shortPortName,
                  sizeof(shortPortName) 
                  );
   ClearPortUseMark(shortPortName);
   m_portName = shortPortName;
   
   // get baud selection
   selection = (WORD)m_baudCombo.GetCurSel();
   m_baud = m_baudCombo.GetItemData(selection);

   // get parity selection
   selection = (WORD)m_parityCombo.GetCurSel();
   m_parity = m_parityCombo.GetItemData(selection);

   // get Data Bits selection
   selection = (WORD)m_dataBitsCombo.GetCurSel();
   m_dataBits = m_dataBitsCombo.GetItemData(selection);

   // get Stop Bits selection
   selection = (WORD)m_stopBitCombo.GetCurSel();
   m_stopBits = m_stopBitCombo.GetItemData(selection);

   // get Data Bits selection
   selection = (WORD)m_rtsCombo.GetCurSel();
   m_rts = m_rtsCombo.GetItemData(selection);

   // Some settings must only get "READ" is we are in the 
   // Allen-Bradley emulation mode.
   if ((PROTOCOL_SELABMASTER232 == pGlobalDialog->m_selectedProtocol) ||
       (PROTOCOL_SELAB232 == pGlobalDialog->m_selectedProtocol))
   { //Allen-Bradley stuff here
      selection = (WORD)m_checkTypeCombo.GetCurSel();
	   m_useBCC = (selection == 1);
   }
	CDialog::OnOK();
}

// ------------------------- OnInitDialog ---------------------------
//
BOOL CServerSettings232Dlg::OnInitDialog() 
{

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
   m_topic.SetHelpTopic("RS-232 Setup");

   // fill serial ports combo box
   FillSerialCBoxUsedResources(&m_portCombo, m_portName);
   // fill Baud combo box and make initial selection
   FillDWordCBox(&m_baudCombo, BaudTable, sizeof( BaudTable ) / sizeof( BaudTable[ 0 ] ),
         m_baud);

   // fill Parity combo box and make initial selection
   FillCharCBox(&m_parityCombo, ParityTable, ParityTableStr, sizeof( ParityTable ) / sizeof( ParityTable[ 0 ] ),
         m_parity);

   // fill DataBits combo box and make initial selection
   FillCharCBox(&m_dataBitsCombo, DataBitTable, DataBitTableStr, sizeof( DataBitTable ) / sizeof( DataBitTable[ 0 ] ),
         m_dataBits);

   // fill StopBits combo box and make initial selection
   FillCharCBox(&m_stopBitCombo, StopBitTable, StopBitTableStr, sizeof( StopBitTable ) / sizeof( StopBitTable[ 0 ] ),
         m_stopBits);


   // fill RTS combo box and make initial selection
   FillCharCBox(&m_rtsCombo, RTSLineTable, RTSLineTableStr, sizeof( RTSLineTable ) / sizeof( RTSLineTable[ 0 ] ),
         m_rts);

   ((CComboBox*)GetDlgItem(IDC_CHECKTYPE))->SetCurSel(m_useBCC? 1:0);

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

   // Allen-Bradley
   if ((PROTOCOL_SELABMASTER232 == pGlobalDialog->m_selectedProtocol) ||
       (PROTOCOL_SELAB232 == pGlobalDialog->m_selectedProtocol))
   {
   CString title;
   
      if (PROTOCOL_SELAB232 == pGlobalDialog->m_selectedProtocol)
         title.LoadString(IDS_ABDF1SETUPTITLE);
      else
         title.LoadString(IDS_JOYSETUPTITLE);

      SetWindowText(title);
      
      // disable modbus item controls
      GetDlgItem(IDC_MOSCAD)->EnableWindow(FALSE);
   }
   else
   {
      //disable non-modbus items
      GetDlgItem(IDC_ABMASTER)->EnableWindow(FALSE);
      GetDlgItem(IDC_CHECKTYPE)->EnableWindow(FALSE);
	  m_checkTypeCombo.SetCurSel(0);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CServerSettings232Dlg::PreTranslateMessage(MSG* pMsg) 
{
    // TOOLTIPS START
    if (m_hWnd)
    {
        m_ToolTip.RelayEvent (pMsg);
        return CDialog::PreTranslateMessage(pMsg);
    }
    return (FALSE);
    // TOOLTIPS END
	
	return CDialog::PreTranslateMessage(pMsg);
}

// ------------------------ OnTTN_NeedText ---------------------------------
// TTN_NEEDTEXT message handler for TOOLTIPS
//
BOOL CServerSettings232Dlg::OnTTN_NeedText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
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
    return(FALSE);
} // OnTTN_NeedText
