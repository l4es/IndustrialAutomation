#if !defined(AFX_CSVFILEIMPORTDLG_H__3890B814_575B_4FE5_85E8_11105A59A384__INCLUDED_)
#define AFX_CSVFILEIMPORTDLG_H__3890B814_575B_4FE5_85E8_11105A59A384__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

// CSVFileImportDlg.h : header file
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
// CREDIT: uses a edit control by JP Arendse
// http://www.codeproject.com/KB/edit/fileeditctrl.aspx

/////////////////////////////////////////////////////////////////////////////
// CCSVFileImportDlg dialog

class CCSVFileImportDlg : public CDialog
{
// Construction
public:
	CCSVFileImportDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCSVFileImportDlg)
	enum { IDD = IDD_CSV_IMPORTDLG };
	BOOL	m_csvImportEnable;
	CString	m_importFolder;
	CString	m_logFileName;
	//}}AFX_DATA
   CFileEditCtrl m_FileEditCtrl;
   CFileEditCtrl m_FolderEditCtrl;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCSVFileImportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCSVFileImportDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnQuickImport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CSVFILEIMPORTDLG_H__3890B814_575B_4FE5_85E8_11105A59A384__INCLUDED_)
