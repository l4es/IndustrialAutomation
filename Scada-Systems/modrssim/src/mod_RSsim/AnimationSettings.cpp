/////////////////////////////////////////////////////////////////////////////
//
// FILE: AnimationSettings.cpp : implementation file
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
// Fixes: colors change-able
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_RSsim.h"
#include "AnimationSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define DLGTIMER_NUMBER    2
/////////////////////////////////////////////////////////////////////////////
// CAnimationSettingsDlg dialog


CAnimationSettingsDlg::CAnimationSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnimationSettingsDlg::IDD, pParent)
{
   m_firstPaint = TRUE;
	//{{AFX_DATA_INIT(CAnimationSettingsDlg)
	m_value        = 0;
	m_refreshes    = 0;
	m_animationPeriod = 0;
	m_inputHtmlFilename = _T("");
	m_outputHtmlFilename = _T("");
	m_enableHTMLGUI = FALSE;
	m_HtmlUpdateRate = 0;
	//}}AFX_DATA_INIT
}


void CAnimationSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnimationSettingsDlg)
	DDX_Text(pDX, IDC_VALUE, m_value);
	DDV_MinMaxLong(pDX, m_value, -9999, 9999);
	DDX_Text(pDX, IDC_REFRESHES, m_refreshes);
	DDV_MinMaxLong(pDX, m_refreshes, 1, 30);
	DDX_Text(pDX, IDC_ANIMATION_ON, m_animationPeriod);
	DDV_MinMaxDWord(pDX, m_animationPeriod, 1, 20);
	DDX_Text(pDX, IDC_INPUT_HTMLNAME, m_inputHtmlFilename);
	DDX_Text(pDX, IDC_OUTPUT_HTMLNAME, m_outputHtmlFilename);
	DDX_Check(pDX, IDC_ENABLEHTMLGUI, m_enableHTMLGUI);
	DDX_Text(pDX, IDC_HTML_UPDATERATE, m_HtmlUpdateRate);
	DDV_MinMaxDWord(pDX, m_HtmlUpdateRate, 1, 30);
	//}}AFX_DATA_MAP
   DDX_FileEditCtrl(pDX, IDC_SCRIPTNAME, m_ScriptNameEditCtrl, FEC_FILEOPEN);
   DDX_FileEditCtrl(pDX, IDC_INPUT_HTMLNAME, m_HtmlInEditCtrl, FEC_FILEOPEN);
   DDX_FileEditCtrl(pDX, IDC_OUTPUT_HTMLNAME, m_HtmlOutEditCtrl, FEC_FILESAVEAS);
}


BEGIN_MESSAGE_MAP(CAnimationSettingsDlg, CDialog)
   ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnTTN_NeedText )
	//{{AFX_MSG_MAP(CAnimationSettingsDlg)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_STN_ENABLED, OnSelchangeStnEnabled)
	ON_CBN_SELCHANGE(IDC_STN_DISABLED, OnSelchangeStnDisabled)
	ON_CBN_SELCHANGE(IDC_STN_ACTIVE, OnSelchangeStnActive)
	ON_CBN_SELCHANGE(IDC_STN_INACTIVE, OnSelchangeStnInactive)
	ON_CBN_SELCHANGE(IDC_STN_TEXT, OnSelchangeStnText)
	ON_CBN_SELCHANGE(IDC_STN_DISTEXT, OnSelchangeStnDisabledtext)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_ANIMATEBYTES, OnAnimatebytes)
	ON_BN_CLICKED(IDC_ANIMATEWORDS, OnAnimatewords)
	ON_BN_CLICKED(IDC_TRAINING, OnAnimTraining)
	ON_BN_CLICKED(IDC_NOANIMATE, OnNoanimate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimationSettingsDlg message handlers

BOOL CAnimationSettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	UpdateData(FALSE);
   SetDlgItemText(IDC_SCRIPTNAME, m_scriptFile);
	// TODO: Add extra initialization here
	if (!m_animationON)
   {
      CheckRadioButton(IDC_NOANIMATE, IDC_TRAINING, IDC_NOANIMATE);
      OnNoanimate();
   }
   else
   { // animation enabled
      if (m_plantAnimation)
      {
         CheckRadioButton(IDC_NOANIMATE, IDC_TRAINING, IDC_TRAINING);
         OnAnimTraining();
      }
      else
      {
      	OnAnimatewords();
         if (m_animationBYTE)
            CheckRadioButton(IDC_NOANIMATE, IDC_TRAINING, IDC_ANIMATEBYTES);
         else
            CheckRadioButton(IDC_NOANIMATE, IDC_TRAINING, IDC_ANIMATEWORDS);
      }
   }

   m_ctlEnabledColor.SubclassDlgItem(IDC_STN_ENABLED, this);
   m_ctlDisabledColor.SubclassDlgItem(IDC_STN_DISABLED, this);
   m_ctlActiveColor.SubclassDlgItem(IDC_STN_ACTIVE, this);
   m_ctlInactiveColor.SubclassDlgItem(IDC_STN_INACTIVE, this);
   m_ctlTextColor.SubclassDlgItem(IDC_STN_TEXT, this);
   m_ctlDisabledTextColor.SubclassDlgItem(IDC_STN_DISTEXT, this);
   // set the control's colors up
   m_ctlEnabledColor.SetSelectedColorValue(m_mTickColorEnabled);
   m_ctlDisabledColor.SetSelectedColorValue(m_mTickColorDisabled);
   m_ctlActiveColor.SetSelectedColorValue(m_mTickColorActive);
   m_ctlInactiveColor.SetSelectedColorValue(m_mTickColorInactive);
   m_ctlTextColor.SetSelectedColorValue(m_mTickColorText);
   m_ctlDisabledTextColor.SetSelectedColorValue(m_mTickDisColorText);


   m_demoTick.SubclassDlgItem(IDC_DEMOCHECK, this);
   m_demoTick.SetCheck(1);
   m_demoTick.SetBorderWidth(2);
   m_demoTick.SetCheckedColor(m_mTickColorEnabled);
   m_demoTick.SetUnCheckedColor(m_mTickColorDisabled);
   m_demoTick.SetActiveColor(m_mTickColorActive);
   m_demoTick.SetInactiveColor(m_mTickColorInactive);
   m_demoTick.SetTextColor(m_mTickColorText);
   m_demoTick.SetUnCheckedTextColor(m_mTickDisColorText);

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

   SetTimer(DLGTIMER_NUMBER, 2000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAnimationSettingsDlg::OnOK() 
{
   UpdateData(TRUE);
   m_plantAnimation = FALSE;

   POSITION pos = m_ScriptNameEditCtrl.GetStartPosition();
   if(pos)
   {
      m_scriptFile = m_ScriptNameEditCtrl.GetNextPathName(pos);
   }
   // HTML inpout template file
   pos = m_HtmlInEditCtrl.GetStartPosition();
   if(pos)
   {
      m_inputHtmlFilename = m_HtmlInEditCtrl.GetNextPathName(pos);
   }
   
   // HTML output file
   pos = m_HtmlOutEditCtrl.GetStartPosition();
   if(pos)
   {
      m_outputHtmlFilename = m_HtmlOutEditCtrl.GetNextPathName(pos);
   }

	if (IsDlgButtonChecked(IDC_NOANIMATE))
      m_animationON = FALSE;
   else
   {
      m_animationON = TRUE;

	   if (IsDlgButtonChecked(IDC_TRAINING))
         m_plantAnimation = TRUE;
      else
      {
         m_plantAnimation = FALSE;
	      if (IsDlgButtonChecked(IDC_ANIMATEBYTES))
         {
            m_animationBYTE = TRUE;
            m_animationWORD = FALSE;
         }
         else
         {
            m_animationBYTE = FALSE;
            m_animationWORD = TRUE;
         }
      }
   }
   // save our colors to the parent
   m_mTickColorEnabled  = m_ctlEnabledColor.GetSelectedColorValue();
   m_mTickColorDisabled = m_ctlDisabledColor.GetSelectedColorValue();
   m_mTickColorActive   = m_ctlActiveColor.GetSelectedColorValue();
   m_mTickColorInactive = m_ctlInactiveColor.GetSelectedColorValue();
   m_mTickColorText     = m_ctlTextColor.GetSelectedColorValue();
   m_mTickDisColorText  = m_ctlDisabledTextColor.GetSelectedColorValue();

   // store our HTML GUI parameters
	CDialog::OnOK();
}

void CAnimationSettingsDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
   if (DLGTIMER_NUMBER ==	nIDEvent)
   {
      // toggle the demo control's states
      m_demoTick.SetCheck(!m_demoTick.GetCheck());
      m_demoTick.SetBorderState(!m_demoTick.GetBorderState());
   }
	CDialog::OnTimer(nIDEvent);
}

void CAnimationSettingsDlg::OnSelchangeStnEnabled() 
{
	// TODO: Add your control notification handler code here
   m_demoTick.SetCheckedColor( m_ctlEnabledColor.GetSelectedColorValue() );
}

void CAnimationSettingsDlg::OnSelchangeStnDisabled() 
{
	// TODO: Add your control notification handler code here
   m_demoTick.SetUnCheckedColor( m_ctlDisabledColor.GetSelectedColorValue() );
}

void CAnimationSettingsDlg::OnSelchangeStnActive() 
{
	// TODO: Add your control notification handler code here
   m_demoTick.SetActiveColor( m_ctlActiveColor.GetSelectedColorValue() );
}

void CAnimationSettingsDlg::OnSelchangeStnInactive() 
{
	// TODO: Add your control notification handler code here
   m_demoTick.SetInactiveColor( m_ctlInactiveColor.GetSelectedColorValue() );
}

void CAnimationSettingsDlg::OnSelchangeStnText() 
{
	// TODO: Add your control notification handler code here
   m_demoTick.SetTextColor( m_ctlTextColor.GetSelectedColorValue() );
}

void CAnimationSettingsDlg::OnSelchangeStnDisabledtext() 
{
	// TODO: Add your control notification handler code here
   m_demoTick.SetUnCheckedTextColor( m_ctlDisabledTextColor.GetSelectedColorValue() );
}

void CAnimationSettingsDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
   if (m_firstPaint)
   {
      m_firstPaint = FALSE;
   }
	// Do not call CDialog::OnPaint() for painting messages
}

BOOL CAnimationSettingsDlg::PreTranslateMessage(MSG* pMsg) 
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

// ------------------------ OnTTN_NeedText ---------------------------------
// TTN_NEEDTEXT message handler for TOOLTIPS
//
BOOL CAnimationSettingsDlg::OnTTN_NeedText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
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


void CAnimationSettingsDlg::OnAnimatebytes() 
{
	// disable everything
   OnNoanimate();
   // then enable some controls afterward
}

void CAnimationSettingsDlg::OnAnimatewords() 
{
	// TODO: Add your control notification handler code here
   GetDlgItem(IDC_STATIC1)->EnableWindow(TRUE);
   GetDlgItem(IDC_STATIC2)->EnableWindow(TRUE);
   GetDlgItem(IDC_STATIC3)->EnableWindow(TRUE);
   GetDlgItem(IDC_VALUE)->EnableWindow(TRUE);
   GetDlgItem(IDC_REFRESHES)->EnableWindow(TRUE);
   // script and name
   GetDlgItem(IDC_STATIC5)->EnableWindow(FALSE);
   GetDlgItem(IDC_SCRIPTNAME)->EnableWindow(FALSE);
}

void CAnimationSettingsDlg::OnAnimTraining() 
{
	// disable everything
   OnNoanimate();
   // then enable some controls afterward
   // script and name enabled
   GetDlgItem(IDC_STATIC5)->EnableWindow(TRUE);
   GetDlgItem(IDC_SCRIPTNAME)->EnableWindow(TRUE);

   GetDlgItem(IDC_STATIC2)->EnableWindow(TRUE);
   GetDlgItem(IDC_REFRESHES)->EnableWindow(TRUE);
   GetDlgItem(IDC_STATIC3)->EnableWindow(TRUE);
} // OnAnimTraining

void CAnimationSettingsDlg::OnNoanimate() 
{
	// disable all the controls
   GetDlgItem(IDC_STATIC1)->EnableWindow(FALSE);
   GetDlgItem(IDC_STATIC2)->EnableWindow(FALSE);
   GetDlgItem(IDC_STATIC3)->EnableWindow(FALSE);
   GetDlgItem(IDC_VALUE)->EnableWindow(FALSE);
   GetDlgItem(IDC_REFRESHES)->EnableWindow(FALSE);
   //script and name
   GetDlgItem(IDC_STATIC5)->EnableWindow(FALSE);
   GetDlgItem(IDC_SCRIPTNAME)->EnableWindow(FALSE);

} // OnNoanimate
