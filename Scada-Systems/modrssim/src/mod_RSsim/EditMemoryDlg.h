#if !defined(AFX_EDITMEMORYDLG_H__3B29C5BB_9C00_4EFD_B8B2_8E7E5D322F29__INCLUDED_)
#define AFX_EDITMEMORYDLG_H__3B29C5BB_9C00_4EFD_B8B2_8E7E5D322F29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////
//
// FILE: EditMemoryDlg.h : headder file
//
// See _README.CPP
//
/////////////////////////////////////////////////////////////////////////////

extern void ConvertWordToASCIICS(CString& csString, const WORD wValue);
extern void ConvertASCIIToWord(const char*String, WORD &wValue);
/////////////////////////////////////////////////////////////////////////////
// CEditMemoryDlg dialog

class CEditMemoryDlg : public CDialog
{
// Construction
public:
	CEditMemoryDlg(LPCTSTR formatting, LPCTSTR registerName, DWORD memoryValue, WORD valueType, LPCTSTR description, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditMemoryDlg)
	enum { IDD = IDD_EDITMEMCELL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

   CString m_description;
   DWORD  m_value;
   CString m_formatting;
   CString m_registerName;

   WORD  m_valueType;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditMemoryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditMemoryDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITMEMORYDLG_H__3B29C5BB_9C00_4EFD_B8B2_8E7E5D322F29__INCLUDED_)
