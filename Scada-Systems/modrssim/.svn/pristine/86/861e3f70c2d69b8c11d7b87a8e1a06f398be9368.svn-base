// ServerSettingsEthDlg.cpp : implementation file
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
#include "mod_RSsim.h"
#include "ServerSettingsEthDlg.h"
#include "mod_RSsimDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMOD_simDlg * pGlobalDialog;


/////////////////////////////////////////////////////////////////////////////
// CServerSettingsEthDlg dialog


CServerSettingsEthDlg::CServerSettingsEthDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerSettingsEthDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CServerSettingsEthDlg)
	m_localPort = 0;		// these get changed by the caller
   m_localId   = 0;
	m_responseDelay = 0;
	m_linger = FALSE;
	m_numServers = 0;
	m_localIPStr = _T("");
	m_remoteIPStr = _T("");
	m_other = 0;
	//}}AFX_DATA_INIT
}


void CServerSettingsEthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerSettingsEthDlg)
	DDX_Text(pDX, IDC_LOCALPORT, m_localPort);
	DDV_MinMaxUInt(pDX, m_localPort, 0, 9999);
   DDX_Text(pDX, IDC_LOCALID, m_localId);
	DDV_MinMaxUInt(pDX, m_localId, 0, 9999);
	DDX_Text(pDX, IDC_RESPONSEDELAY, m_responseDelay);
	DDV_MinMaxUInt(pDX, m_responseDelay, 0, 10000);
	DDX_Check(pDX, IDC_LINGER, m_linger);
	DDX_Text(pDX, IDC_NUMSERVERS, m_numServers);
	DDV_MinMaxUInt(pDX, m_numServers, 1, 1000);
	DDX_Text(pDX, IDC_LOCALIP, m_localIPStr);
	DDX_Text(pDX, IDC_REMOTEIP, m_remoteIPStr);
	DDX_Text(pDX, IDC_LOCALPORT2, m_other);
	DDV_MinMaxDWord(pDX, m_other, 0, 65535);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServerSettingsEthDlg, CDialog)
	//{{AFX_MSG_MAP(CServerSettingsEthDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerSettingsEthDlg message handlers

void CServerSettingsEthDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

// ----------------------------- OnInitDialog -------------------------------
BOOL CServerSettingsEthDlg::OnInitDialog() 
{
CString format;
CString result;

	CDialog::OnInitDialog();
   m_topic.SetHelpTopic("Ethernet Setup");

   if (!pGlobalDialog->m_pServerSockArray) // offline
      result = "offline (not listening)";
   else
   {
      GetDlgItemText(IDC_SERVERLINE, format);

      result.Format(format, pGlobalDialog->m_numServers);
   }
   SetDlgItemText(IDC_SERVERLINE, result);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
