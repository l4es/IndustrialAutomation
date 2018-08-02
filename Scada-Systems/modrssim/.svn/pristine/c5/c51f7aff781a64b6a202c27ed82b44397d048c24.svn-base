#if !defined(AFX_JOYSCCEMULATION_H__B0688A3C_3519_489A_98EB_FEB88DFA5B32__INCLUDED_)
#define AFX_JOYSCCEMULATION_H__B0688A3C_3519_489A_98EB_FEB88DFA5B32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JoySCCEmulation.h : header file
//

#include "AdvEmulationDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CJoySCCEmulationDlg dialog

class CJoySCCEmulationDlg : public CDialog
{
// Construction
public:
	CJoySCCEmulationDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CJoySCCEmulationDlg)
	enum { IDD = IDD_JOYSCC_EMULATION };
	UINT	m_destAddr;
	UINT	m_sourceAddr;
	BOOL	m_runMode;
	UINT	m_numShields;
	BOOL	m_readVariables;
	BOOL	m_writeVariables;
	UINT	m_ABidletime;
	UINT	m_ABtimeout;
	//}}AFX_DATA

   CAdvEmulationDlg  *m_emuDlgPtr;
   BOOL  m_emulationOK;

   SetCurrentTopic  m_topic;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJoySCCEmulationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJoySCCEmulationDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnEmu();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JOYSCCEMULATION_H__B0688A3C_3519_489A_98EB_FEB88DFA5B32__INCLUDED_)
