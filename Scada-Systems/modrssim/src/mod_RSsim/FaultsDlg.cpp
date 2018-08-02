// FaultsDlg.cpp : implementation file
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

#include "stdafx.h"
#include "mod_rssim.h"
#include "FaultsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// control ID's to gray/ungray
int controlArray[] =
{
   IDC_INSERTS,
   IDC_DELETES,
   IDC_CORRUPTS,
   IDC_MISFRAMES,
   IDC_PARITY,
   IDC_IGNORES,
   IDC_DELAYS,
   IDC_DELAYPERIOD,
   IDC_FREQ,
   IDC_BEEP
};

BOOL ethernetControl[] = 
{
   FALSE,     // IDC_INSERTS,        
   FALSE,     //   IDC_DELETES,        
   FALSE,     //   IDC_CORRUPTS,       
   FALSE,  //   IDC_MISFRAMES,      
   FALSE,     //   IDC_PARITY,         
   TRUE,      //   IDC_IGNORES,        
   TRUE,      //   IDC_DELAYS,         
   TRUE,      //   IDC_DELAYPERIOD,    
   TRUE,      //   IDC_FREQ,           
   TRUE       //   IDC_BEEP            
};


/////////////////////////////////////////////////////////////////////////////
// CFaultsDlg dialog


CFaultsDlg::CFaultsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFaultsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFaultsDlg)
	m_beep = FALSE;
	m_delaysPeriod = 0;
	m_errorFrequency = 0;
	m_insertCharacters = FALSE;
	m_removeCharacters = FALSE;
	m_modifyCharacters = FALSE;
	m_ignoreReq = FALSE;
	m_corruptFraming = FALSE;
	m_parityFaults = FALSE;
	//}}AFX_DATA_INIT
   m_errors = FALSE;
   m_etherNet = FALSE;
}


void CFaultsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFaultsDlg)
	DDX_Control(pDX, IDC_FREQ, m_freqSlider);
	DDX_Check(pDX, IDC_BEEP, m_beep);
	DDX_Text(pDX, IDC_DELAYPERIOD, m_delaysPeriod);
	DDV_MinMaxInt(pDX, m_delaysPeriod, 0, 10000);
	DDX_Slider(pDX, IDC_FREQ, m_errorFrequency);
	DDX_Check(pDX, IDC_INSERTS, m_insertCharacters);
	DDX_Check(pDX, IDC_DELETES, m_removeCharacters);
	DDX_Check(pDX, IDC_CORRUPTS, m_modifyCharacters);
	DDX_Check(pDX, IDC_IGNORES, m_ignoreReq);
	DDX_Check(pDX, IDC_MISFRAMES, m_corruptFraming);
	DDX_Check(pDX, IDC_PARITY, m_parityFaults);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFaultsDlg, CDialog)
   ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnTTN_NeedText )
	//{{AFX_MSG_MAP(CFaultsDlg)
	ON_BN_CLICKED(IDC_BEEP, OnBeepClicked)
	ON_BN_CLICKED(IDC_DELAYS, OnDelaysClicked)
	ON_BN_CLICKED(IDC_ENABLE, OnEnableClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFaultsDlg message handlers

BOOL CFaultsDlg::OnInitDialog() 
{
CString title;
	CDialog::OnInitDialog();
	
	GetWindowText(title);
   if (m_etherNet)
      title += " : [Ethernet]";
   else
      title += " : [Serial]";
   SetWindowText(title);

   CheckDlgButton(IDC_ENABLE, (m_errorFrequency?1:0));
   CheckDlgButton(IDC_DELAYS, (m_errors?1:0));
   m_freqSlider.SetRange(0, 100);
   m_freqSlider.SetPos(m_errorFrequency);
   OnEnableClicked();   //perform graying

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

   // disabled in v7.7
   MessageBox("WARNING: The error simulation function is currently disabled, please contact the author.","Work in progress", MB_ICONEXCLAMATION |MB_OK);

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ---------------------------- OnBeepClicked ----------------------
void CFaultsDlg::OnBeepClicked() 
{
	// TODO: Add your control notification handler code here
   	
}

// ---------------------------- OnDelaysClicked --------------------
void CFaultsDlg::OnDelaysClicked() 
{
//BOOL enabled;
	
   UpdateData(TRUE);
   m_errors = IsDlgButtonChecked(IDC_DELAYS);
   GetDlgItem(IDC_DELAYPERIOD)->EnableWindow(m_errors);
   if (!m_errors)
   {
      m_delaysPeriod = 0;
      UpdateData(FALSE);
   }
}


// ------------------------------- OnEnableClicked -------------------
void CFaultsDlg::OnEnableClicked() 
{
BOOL enableCtl;
	
   // gray/ungray all of the controls
   m_errors = IsDlgButtonChecked(IDC_ENABLE);
   for (int i=0; i<(sizeof(controlArray)/sizeof(controlArray[0]));i++)
   {
//      enableCtl = m_errors && ((ethernetControl[i] && !m_etherNet));
      enableCtl = (m_etherNet? (ethernetControl[i] && m_errors): m_errors);
      GetDlgItem(controlArray[i])->EnableWindow(enableCtl);
   }

   //
   // handle special ungraying cases
   OnDelaysClicked();

} // OnEnableClicked

// ------------------------ OnTTN_NeedText ---------------------------------
// TTN_NEEDTEXT message handler for TOOLTIPS
//
BOOL CFaultsDlg::OnTTN_NeedText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
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

BOOL CFaultsDlg::PreTranslateMessage(MSG* pMsg) 
{
    // TOOLTIPS START
    if (m_hWnd)
    {
        m_ToolTip.RelayEvent (pMsg);
        return CDialog::PreTranslateMessage(pMsg);
    }
    return (FALSE);
    // TOOLTIPS END
}

void CFaultsDlg::OnOK() 
{
	// TODO: Add extra validation here
   m_errors = IsDlgButtonChecked(IDC_DELAYS);
	
	CDialog::OnOK();

   // feature disabled in v7.7
   m_errors = FALSE;
}
