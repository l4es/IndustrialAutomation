
#if !defined(AFX_SERVERSETTINGSDLG_H__5EE467B0_3A96_41BA_BEC2_27A1C3528C4E__INCLUDED_)
#define AFX_SERVERSETTINGSDLG_H__5EE467B0_3A96_41BA_BEC2_27A1C3528C4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////
//
// FILE: ServerSettingsDlg.h : headder file
//
// See _README.CPP
//
//
/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////
// CServerSettings232Dlg dialog

class CServerSettings232Dlg : public CDialog
{
// Construction
public:
	CServerSettings232Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CServerSettings232Dlg)
	enum { IDD = IDD_SERVERSETTINGS_232 };
	CComboBox	m_checkTypeCombo;
	CComboBox	m_rtsCombo;
	CComboBox	m_stopBitCombo;
	CComboBox	m_portCombo;
	CComboBox	m_parityCombo;
	CComboBox	m_dataBitsCombo;
	CComboBox	m_baudCombo;
	UINT	m_responseDelay;
	BOOL	m_MOSCADchecks;
	BOOL	m_autoLoad;
	BOOL	m_startAsOff;
	BOOL	m_ABMastermode;
	//}}AFX_DATA
   DWORD	   m_stopBits;
	CString	m_portName;
	DWORD	   m_parity;
	DWORD	   m_dataBits;
	DWORD	   m_baud;
   DWORD    m_rts;
   BOOL		m_useBCC;
   DWORD    m_protocol; // which 232 protcol to use

   SetCurrentTopic  m_topic;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerSettings232Dlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

   // TOOLTIPS
   CToolTipCtrl    m_ToolTip;
   BOOL     OnTTN_NeedText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );

	// Generated message map functions
	//{{AFX_MSG(CServerSettings232Dlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERSETTINGSDLG_H__5EE467B0_3A96_41BA_BEC2_27A1C3528C4E__INCLUDED_)
