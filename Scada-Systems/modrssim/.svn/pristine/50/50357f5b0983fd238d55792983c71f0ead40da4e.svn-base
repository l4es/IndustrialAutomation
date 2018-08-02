// JoySCCEmulation.cpp : implementation file
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
#include "resource.h"
#include "JoySCCEmulation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJoySCCEmulationDlg dialog


CJoySCCEmulationDlg::CJoySCCEmulationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJoySCCEmulationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJoySCCEmulationDlg)
	m_destAddr = 0;
	m_sourceAddr = 0;
	m_runMode = FALSE;
	m_numShields = 0;
	m_readVariables = FALSE;
	m_writeVariables = FALSE;
	m_ABidletime = 0;
	m_ABtimeout = 0;
	//}}AFX_DATA_INIT

   m_emulationOK = TRUE;
   m_topic.SetHelpTopic("ProtocolsJoy");
}


void CJoySCCEmulationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJoySCCEmulationDlg)
	DDX_Text(pDX, IDC_SCC_DEST, m_destAddr);
	DDV_MinMaxUInt(pDX, m_destAddr, 0, 255);
	DDX_Text(pDX, IDC_SCC_SOURCE, m_sourceAddr);
	DDV_MinMaxUInt(pDX, m_sourceAddr, 0, 255);
	DDX_Check(pDX, IDC_RUN, m_runMode);
	DDX_Text(pDX, IDC_NUMSHIELDS, m_numShields);
	DDV_MinMaxUInt(pDX, m_numShields, 1, 154);
	DDX_Check(pDX, IDC_READVARIABLES, m_readVariables);
	DDX_Check(pDX, IDC_WRITEVARIABLES, m_writeVariables);
	DDX_Text(pDX, IDC_IDLE, m_ABidletime);
	DDV_MinMaxUInt(pDX, m_ABidletime, 0, 10000);
	DDX_Text(pDX, IDC_TIMEOUT, m_ABtimeout);
	DDV_MinMaxUInt(pDX, m_ABtimeout, 100, 20000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJoySCCEmulationDlg, CDialog)
	//{{AFX_MSG_MAP(CJoySCCEmulationDlg)
	ON_BN_CLICKED(IDC_EMU, OnEmu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJoySCCEmulationDlg message handlers

void CJoySCCEmulationDlg::OnOK() 
{
	// TODO: Add extra validation here
	CDialog::OnOK();
   if (!m_readVariables && !m_writeVariables)
   {
      m_writeVariables = TRUE;
   }
}

void CJoySCCEmulationDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CJoySCCEmulationDlg::OnEmu() 
{
void *pDialogRam;
DWORD ClsSize = sizeof(CAdvEmulationDlg);

   pDialogRam = new char[ClsSize];
   memcpy(pDialogRam, m_emuDlgPtr, ClsSize);
	
   // set the bookmark in the help!
   m_topic.SetHelpTopic("ProtocolsModbus");
   //
	m_emulationOK = m_emuDlgPtr->DoModal();
   if (IDOK != m_emulationOK)
   {
      // restore things to what they where, since user cancelled the dialog
      memcpy(m_emuDlgPtr, pDialogRam, ClsSize);
   }
   delete pDialogRam;

   // change help topic bookmark back again
   m_topic.SetHelpTopic("ProtocolsJoy");
}
