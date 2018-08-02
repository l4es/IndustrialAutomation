#if !defined(AFX_FAULTSDLG_H__03268BDB_A3F4_4A6D_8590_C7DB7326B890__INCLUDED_)
#define AFX_FAULTSDLG_H__03268BDB_A3F4_4A6D_8590_C7DB7326B890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FaultsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFaultsDlg dialog

class CFaultsDlg : public CDialog
{
// Construction
public:
	CFaultsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFaultsDlg)
	enum { IDD = IDD_FAULTSDLG };
	CSliderCtrl	m_freqSlider;
	BOOL	m_beep;
	int	m_delaysPeriod;
	int		m_errorFrequency;
	BOOL	m_insertCharacters;
	BOOL	m_removeCharacters;
	BOOL	m_modifyCharacters;
	BOOL	m_ignoreReq;
	BOOL	m_corruptFraming;
	BOOL	m_parityFaults;
	//}}AFX_DATA

   BOOL m_errors;
   BOOL m_etherNet;
protected:
   // TOOLTIPS
   CToolTipCtrl    m_ToolTip;
   BOOL     OnTTN_NeedText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFaultsDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFaultsDlg)
	afx_msg void OnBeepClicked();
	afx_msg void OnDelaysClicked();
	afx_msg void OnEnableClicked();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FAULTSDLG_H__03268BDB_A3F4_4A6D_8590_C7DB7326B890__INCLUDED_)
