// AdvEmulationDlg.cpp : implementation file
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

#include "stdafx.h"
#include "AdvEmulationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvEmulationDlg dialog


CAdvEmulationDlg::CAdvEmulationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdvEmulationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdvEmulationDlg)
	m_numInputs = 0;
	m_numOutputs = 0;
	m_numHolding = 0;
	m_numExtended = 0;
	m_numAnalogs = 0;
	m_MOSCADchecks = FALSE;
	m_PDUSize = 0;
	m_modifyThenRespond = FALSE;
	m_disableWrites = FALSE;
	m_numSeperate = 0;
	m_seperateRegisters = FALSE;
	//}}AFX_DATA_INIT
   m_numDisabledControls = 0;
   m_topic.SetHelpTopic("Animation");
}


void CAdvEmulationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvEmulationDlg)
	DDX_Control(pDX, IDC_BMPMODE, m_staticBmpMode);
	DDX_Text(pDX, IDC_INPUTS, m_numInputs);
	DDV_MinMaxDWord(pDX, m_numInputs, 0, 65535);
	DDX_Text(pDX, IDC_OUTPUTS, m_numOutputs);
	DDV_MinMaxDWord(pDX, m_numOutputs, 0, 65535);
	DDX_Text(pDX, IDC_HOLDING, m_numHolding);
	DDV_MinMaxDWord(pDX, m_numHolding, 0, 65535);
	DDX_Text(pDX, IDC_EXTENDED, m_numExtended);
	DDV_MinMaxDWord(pDX, m_numExtended, 0, 65535);
	DDX_Text(pDX, IDC_ANALOGS, m_numAnalogs);
	DDV_MinMaxDWord(pDX, m_numAnalogs, 0, 65535);
	DDX_Check(pDX, IDC_MOSCAD, m_MOSCADchecks);
	DDX_Text(pDX, IDC_PDUSIZE, m_PDUSize);
	DDV_MinMaxDWord(pDX, m_PDUSize, 1, 2048);
	DDX_Check(pDX, IDC_MODIFY_FIRST, m_modifyThenRespond);
	DDX_Check(pDX, IDC_DISABLEWRITES, m_disableWrites);
	DDX_Text(pDX, IDC_NUMSEPERATE, m_numSeperate);
	DDX_Check(pDX, IDC_SEPERATED, m_seperateRegisters);
	//}}AFX_DATA_MAP

   // MAX PDU is (MAX_RX_MESSAGELENGTH/2) = 2048
}


BEGIN_MESSAGE_MAP(CAdvEmulationDlg, CDialog)
	//{{AFX_MSG_MAP(CAdvEmulationDlg)
	ON_BN_CLICKED(IDC_DEFAULTS, OnDefaults)
	ON_BN_CLICKED(IDC_MAXIMUM, OnMaximum)
	ON_BN_CLICKED(IDC_SEPERATED, OnSeperatedClicked)
	ON_BN_CLICKED(IDC_BMPMODE, OnBmpmodeClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvEmulationDlg message handlers

BOOL CAdvEmulationDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

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
	
	UpdateData(FALSE);
   // gray/ungray
   GetDlgItem(IDC_NUMSEPERATE)->EnableWindow(m_seperateRegisters);
   m_bmpModeBitmap.LoadBitmap(m_seperateRegisters? IDB_MULTISLAVEMODE: IDB_SINGLESLAVEMODE);
   m_staticBmpMode.SetBitmap(m_bmpModeBitmap);




   // handle tooltip for disabled controls too...
   // (call this code again if U enable/disable any controls on the fly.)
   CWnd *pChildFirst,*pChild;

   m_numDisabledControls = 0;
   pChildFirst = GetWindow(GW_CHILD);
   pChild = pChildFirst;
   // enumerate all controls in this window 
   while (TRUE)
   {
      if (!pChild->IsWindowEnabled())
      { // add control to our list
         m_pDisabledControls[m_numDisabledControls] = pChild;
         m_numDisabledControls++;
      }
      pChild = pChild->GetNextWindow(GW_HWNDNEXT);
      if ((pChildFirst == pChild)||(NULL==pChild))
         break;
   }
   //TOOLTIPS END

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ------------------------------- OnOK -----------------------------------
void CAdvEmulationDlg::OnOK() 
{
	if (UpdateData(TRUE))
   	CDialog::OnOK();
   // else do nothing
}

// ------------------------------- OnDefaults --------------------------------
// load default settings
void CAdvEmulationDlg::OnDefaults() 
{
	m_numInputs = 10000;
	m_numOutputs = 10000;
	m_numHolding = 10000;
	m_numExtended = 10000;
	m_numAnalogs = 10000;
   m_PDUSize = 128;  // (Note : 128 = 256 bytes)
   m_disableWrites = FALSE;     // simple test

   m_MOSCADchecks = FALSE;
   m_modifyThenRespond = TRUE;
   UpdateData(FALSE);	
}

void CAdvEmulationDlg::OnMaximum() 
{
	// TODO: Add your control notification handler code here
	m_numInputs = 65535;
	m_numOutputs = 65535;
	m_numHolding = 65535;
	m_numExtended = 65535;
	m_numAnalogs = 65535;
   m_PDUSize = MAX_RX_MESSAGELENGTH/2; // (Note : 2048 = 4096 bytes)

   UpdateData(FALSE);	
	
}

// ------------------------ OnSeperatedClicked -------------------------------
// User wants to toggle modes between having seperate resgiters per PLC station 
// and having one register for all stations:
// Seperate registers are accomplished by taking the station ID, and multiplying 
// it by the number of registers per station. 
// Thus total registers = 256 * registers-per-station.
//
void CAdvEmulationDlg::OnSeperatedClicked() 
{

   UpdateData(TRUE); // get settings
   
   // gray/ungray
   GetDlgItem(IDC_NUMSEPERATE)->EnableWindow(m_seperateRegisters);
   // unload and the load correct bitmap.
   m_bmpModeBitmap.DeleteObject();
   m_bmpModeBitmap.LoadBitmap(m_seperateRegisters? IDB_MULTISLAVEMODE: IDB_SINGLESLAVEMODE);
   m_staticBmpMode.SetBitmap(m_bmpModeBitmap);

}

// -------------------------- OnBmpmodeClicked -------------------------------
// user clicks on the bitmap. NOT WORKING.
void CAdvEmulationDlg::OnBmpmodeClicked() 
{
	// Toggle mode:
   UpdateData(TRUE); // get settings

   CheckDlgButton(IDC_SEPERATED, !m_seperateRegisters);

   // call member to update things
	OnSeperatedClicked();
}

// -------------------------- PreTranslateMessage ----------------------------
// overridden so we can get tooltips to work
BOOL CAdvEmulationDlg::PreTranslateMessage(MSG* pMsg) 
{
    // TOOLTIPS START
    if (m_hWnd)
    {
      // catter for disabled controls too      
      HandleTooltipsActivation(pMsg, this, m_pDisabledControls, m_numDisabledControls, &m_ToolTip);
      // catch mouse down over the control
      return CDialog::PreTranslateMessage(pMsg);
    }
    return (FALSE);
    // TOOLTIPS END
}
