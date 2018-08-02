#if !defined(AFX_ANIMATIONSETTINGS_H__788EF6C0_384E_4725_A2D6_A20B436C0981__INCLUDED_)
#define AFX_ANIMATIONSETTINGS_H__788EF6C0_384E_4725_A2D6_A20B436C0981__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////
//
// FILE: AnimationSettings.h : headder file
//
// See _README.CPP
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CAnimationSettingsDlg dialog

class CAnimationSettingsDlg : public CDialog
{
// Construction
public:
	CAnimationSettingsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnimationSettingsDlg)
	enum { IDD = IDD_ANIMATE };
	long	m_value;
	long	m_refreshes;
	DWORD	m_animationPeriod;
	CString	m_inputHtmlFilename;
	CString	m_outputHtmlFilename;
	BOOL	m_enableHTMLGUI;
	DWORD	m_HtmlUpdateRate;
	//}}AFX_DATA
   BOOL  m_animationON;
   BOOL  m_plantAnimation;
   BOOL  m_animationBYTE;
   BOOL  m_animationWORD;
   CString m_scriptFile;

   BOOL m_firstPaint;

   CColorPickerCB m_ctlEnabledColor;
   CColorPickerCB m_ctlDisabledColor;
   CColorPickerCB m_ctlActiveColor;
   CColorPickerCB m_ctlInactiveColor;
   CColorPickerCB m_ctlTextColor;
   CColorPickerCB m_ctlDisabledTextColor;

   CMicroTick     m_demoTick;
   // the colors
   COLORREF    m_mTickColorEnabled;
   COLORREF    m_mTickColorDisabled;
   COLORREF    m_mTickColorActive;
   COLORREF    m_mTickColorInactive;
   COLORREF    m_mTickColorText;
   COLORREF    m_mTickDisColorText;

   // file browse boxes
   CFileEditCtrl m_ScriptNameEditCtrl;
   CFileEditCtrl m_HtmlInEditCtrl;
   CFileEditCtrl m_HtmlOutEditCtrl;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimationSettingsDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

   // TOOLTIPS
   CToolTipCtrl    m_ToolTip;
   BOOL     OnTTN_NeedText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAnimationSettingsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeStnEnabled();
	afx_msg void OnSelchangeStnDisabled();
	afx_msg void OnSelchangeStnActive();
	afx_msg void OnSelchangeStnInactive();
	afx_msg void OnSelchangeStnText();
	afx_msg void OnSelchangeStnDisabledtext();
	afx_msg void OnPaint();
	afx_msg void OnAnimatebytes();
	afx_msg void OnAnimatewords();
	afx_msg void OnAnimTraining();
	afx_msg void OnNoanimate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMATIONSETTINGS_H__788EF6C0_384E_4725_A2D6_A20B436C0981__INCLUDED_)
