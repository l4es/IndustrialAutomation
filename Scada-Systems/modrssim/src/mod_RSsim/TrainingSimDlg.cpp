// TrainingSimDlg.cpp : implementation file
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
#include "resource.h"

#include "TrainingSimDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ------------------------------ FillCharCBoxS ----------------------
// fill a combo-box control from a string array. the user-Data in the 
// combo is filled from an array of DWORDs.
void FillCharCBoxS(CComboBox * cBox, 
                  char ** strTable,
                  WORD tableLen, 
                  DWORD currentsetting)
{
DWORD count;
CHAR ** strTablePtr = strTable;

   cBox->ResetContent();
   for (count = 0; count < tableLen; count++)
   {
      cBox->AddString(strTablePtr[count]);
      cBox->SetItemData(count,  count);
      if (count == currentsetting)
         cBox->SetCurSel(count);
   }
} // FillCharCBoxS


/////////////////////////////////////////////////////////////////////////////
// CTrainingSimDlg dialog


CTrainingSimDlg::CTrainingSimDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrainingSimDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrainingSimDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
   m_numDisabledControls = 0;

   m_topic.SetHelpTopic("Animation");
}


void CTrainingSimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrainingSimDlg)
	DDX_Control(pDX, IDC_CBREGISTERNAMES, m_registerNames);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrainingSimDlg, CDialog)
	//{{AFX_MSG_MAP(CTrainingSimDlg)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_CBREGISTERNAMES, OnSelchangeCbregisternames)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_CHECK2, OnCheck2)
	ON_BN_CLICKED(IDC_CHECK3, OnCheck3)
	ON_BN_CLICKED(IDC_CHECK4, OnCheck4)
	ON_BN_CLICKED(IDC_CHECK5, OnCheck5)
	ON_BN_CLICKED(IDC_CHECK6, OnCheck6)
	ON_BN_CLICKED(IDC_CHECK7, OnCheck7)
	ON_BN_CLICKED(IDC_CHECK8, OnCheck8)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrainingSimDlg message handlers

BOOL CTrainingSimDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	FillCharCBoxS(&m_registerNames, plantSim_registerNames, PS_LASTREGISTER+1, 0);
   m_selectedRegister = 0;

   OnSelchangeCbregisternames();

   //TOOLTIPS START                                          
   
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

   SetTimer(0, 250, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ---------------------------- OnSet ---------------------------------
// "SET" register value button clicked
void CTrainingSimDlg::OnSet() 
{
int val;

   // lock memory for writting
   CMemWriteLock lk(PLCMemory.GetMutex());

   val = GetDlgItemInt(IDC_REGVALUESET);
   PLCMemory.SetAt(MODBUS_MEM_REGISTERS, m_selectedRegister, (WORD)val);
	// [m_selectedRegister];
}

// -------------------------------- OnTimer -----------------------------
void CTrainingSimDlg::OnTimer(UINT nIDEvent) 
{
WORD val, controlVal;
CString statusText;

	// Fill all controls values (except the SET edit control)

   // status line  text
   if (pGlobalDialog->InPlantSimulation())
   {
      if (strlen(pGlobalDialog->GetAnimationScriptFile()))
         statusText.Format("Run script file '%s' (%d ms) %s", 
                  pGlobalDialog->GetAnimationScriptFile(),
                  pGlobalDialog->GetLastScriptRuntime(),
                  (pGlobalDialog->ScriptIsRunning()?"RUN":"IDLE")
                          );
      else
         statusText = "Build-in Tranining animation running...";
   }
   else
   {
      statusText = "Animation disabled";
   }
   SetDlgItemText(IDC_STATUSLINE, statusText);

   // handle PLC register values

   // drop-list register
   val = PLCMemory[MODBUS_MEM_REGISTERS][m_selectedRegister];
   SetDlgItemInt(IDC_REGVALUE, val);

   // digitals
   val = PLCMemory[MODBUS_MEM_REGISTERS][PS_FEEDBACKBITS];
   controlVal = PLCMemory[MODBUS_MEM_REGISTERS][PS_CONTROLBITS];
   
   CheckDlgButton(IDC_CHECK1, controlVal & PS_MASK_INPUT1  );
   CheckDlgButton(IDC_CHECK2, controlVal & PS_MASK_INPUT2  );
   CheckDlgButton(IDC_CHECK3, controlVal & PS_MASK_DRAIN   );
   CheckDlgButton(IDC_CHECK4, controlVal & PS_MASK_HEATER  );
   CheckDlgButton(IDC_CHECK5, controlVal & PS_MASK_RESET   );
   CheckDlgButton(IDC_CHECK6, controlVal & PS_MASK_LOWLEVEL);
   CheckDlgButton(IDC_CHECK7, controlVal & PS_MASK_AUTO    );
   CheckDlgButton(IDC_CHECK8, controlVal & PS_STIRRER_START);

   CheckDlgButton(IDC_CHECK9, val & PS_MASK_INPUT1  );
   CheckDlgButton(IDC_CHECK10,val & PS_MASK_INPUT2  );
   CheckDlgButton(IDC_CHECK11,val & PS_MASK_DRAIN   );
   CheckDlgButton(IDC_CHECK12,val & PS_MASK_HEATER  );
   CheckDlgButton(IDC_CHECK13,val & PS_MASK_RESET   );
   CheckDlgButton(IDC_CHECK14,val & PS_MASK_LOWLEVEL);
   CheckDlgButton(IDC_CHECK15,val & PS_MASK_AUTO    );
   CheckDlgButton(IDC_CHECK16,val & PS_STIRRER_START);


   CDialog::OnTimer(nIDEvent);
}

// ------------------------------ OnSelchangeCbregisternames ----------------
// register selection drop-list changed
void CTrainingSimDlg::OnSelchangeCbregisternames() 
{
WORD val;
   m_selectedRegister = m_registerNames.GetCurSel();
   // TODO: Add your control notification handler code here
   val = PLCMemory[MODBUS_MEM_REGISTERS][m_selectedRegister];
   SetDlgItemInt(IDC_REGVALUE, val);
   SetDlgItemInt(IDC_REGVALUESET, val);
   // [m_selectedRegister];
}

// -------------------------- ToggleCheckOnItem --------------------------
// toggle a bit
void CTrainingSimDlg::ToggleCheckOnItem(int checkID, int bitNum)
{
// lock memory for writting
CMemWriteLock lk(PLCMemory.GetMutex());
WORD val;
	
   val = PLCMemory[MODBUS_MEM_REGISTERS][PS_CONTROLBITS];
   if (IsDlgButtonChecked(checkID))
      val |= bitNum;
   else
      val &= (~bitNum);
   PLCMemory.SetAt(MODBUS_MEM_REGISTERS, PS_CONTROLBITS, val);

}

// user clicked on a tick-box
void CTrainingSimDlg::OnCheck1() 
{
   ToggleCheckOnItem(IDC_CHECK1, PS_MASK_INPUT1);
}

void CTrainingSimDlg::OnCheck2() 
{
   ToggleCheckOnItem(IDC_CHECK2, PS_MASK_INPUT2);
}

void CTrainingSimDlg::OnCheck3() 
{
   ToggleCheckOnItem(IDC_CHECK3, PS_MASK_DRAIN);
}

void CTrainingSimDlg::OnCheck4() 
{
   ToggleCheckOnItem(IDC_CHECK4, PS_MASK_HEATER);
}

void CTrainingSimDlg::OnCheck5() 
{
   ToggleCheckOnItem(IDC_CHECK5, PS_MASK_RESET);
}

void CTrainingSimDlg::OnCheck6() 
{
   ToggleCheckOnItem(IDC_CHECK6, PS_MASK_LOWLEVEL);
}

void CTrainingSimDlg::OnCheck7() 
{
   ToggleCheckOnItem(IDC_CHECK7, PS_MASK_AUTO);
}

void CTrainingSimDlg::OnCheck8() 
{
   ToggleCheckOnItem(IDC_CHECK8, PS_STIRRER_START);
}

BOOL CTrainingSimDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
    // TOOLTIPS START                                      
    if (m_hWnd)                                               
    {  
      // catter for disabled controls too      
      HandleTooltipsActivation(pMsg, this, m_pDisabledControls, m_numDisabledControls, &m_ToolTip);
      //m_ToolTip.RelayEvent (pMsg);                       
      return CDialog::PreTranslateMessage(pMsg);         
    }                                                      
    return (FALSE);                                        
    // TOOLTIPS END                                        
}
