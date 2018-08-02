#if !defined(AFX_EASTERDLG_H__80DE8125_6D9F_40C0_84B1_4E45FD9D9447__INCLUDED_)
#define AFX_EASTERDLG_H__80DE8125_6D9F_40C0_84B1_4E45FD9D9447__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EasterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEasterDlg dialog

class CEasterDlg : public CDialog
{
// Construction
public:
	CEasterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEasterDlg)
	enum { IDD = IDD_EASTEREGG };
	CStarWarsCtrl	m_starsCtrl;
	//}}AFX_DATA

   CString m_userName;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEasterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEasterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EASTERDLG_H__80DE8125_6D9F_40C0_84B1_4E45FD9D9447__INCLUDED_)
