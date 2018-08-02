#if !defined(AFX_ESERVERSETTINGSDLG_H__5EE467B0_3A96_41BA_BEC2_27A1C3528C4E__INCLUDED_)
#define AFX_ESERVERSETTINGSDLG_H__5EE467B0_3A96_41BA_BEC2_27A1C3528C4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerSettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CServerSettingsEthDlg dialog

class CServerSettingsEthDlg : public CDialog
{
// Construction
public:
	CServerSettingsEthDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CServerSettingsEthDlg)
	enum { IDD = IDD_SERVERSETTINGS_ETH };
	UINT	m_localPort;
   UINT  m_localId;
	UINT	m_responseDelay;
	BOOL	m_linger;
	UINT	m_numServers;
	CString	m_localIPStr;
	CString	m_remoteIPStr;
	DWORD	m_other;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerSettingsEthDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

   SetCurrentTopic  m_topic;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CServerSettingsEthDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ESERVERSETTINGSDLG_H__5EE467B0_3A96_41BA_BEC2_27A1C3528C4E__INCLUDED_)
