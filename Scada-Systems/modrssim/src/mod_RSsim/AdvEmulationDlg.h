#if !defined(AFX_ADVEMULATIONDLG_H__4AC409B4_17EB_4295_84E9_814AF3E31BB4__INCLUDED_)
#define AFX_ADVEMULATIONDLG_H__4AC409B4_17EB_4295_84E9_814AF3E31BB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvEmulationDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdvEmulationDlg dialog
#include "resource.h"

class CAdvEmulationDlg : public CDialog
{
// Construction
public:
	CAdvEmulationDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAdvEmulationDlg)
	enum { IDD = IDD_ADVEMULATION };
	CStatic	m_staticBmpMode;
	DWORD	m_numInputs;
	DWORD	m_numOutputs;
	DWORD	m_numHolding;
	DWORD	m_numExtended;
	DWORD	m_numAnalogs;
	BOOL	m_MOSCADchecks;
	DWORD	m_PDUSize;
	BOOL	m_modifyThenRespond;
	BOOL	m_disableWrites;
	UINT	m_numSeperate;
	BOOL	m_seperateRegisters;
	//}}AFX_DATA

   CWnd* SetParentWnd(CWnd*pWndNew) {CWnd *pOld = pWndNew; m_pParentWnd = pWndNew; return(pOld);}
   
   CBitmap          m_bmpModeBitmap;
   SetCurrentTopic  m_topic;     // help topic thingy

   // TOOLTIPS
   CToolTipCtrl    m_ToolTip;
   int m_numDisabledControls;
   CWnd *m_pDisabledControls[256];      //only can get 256 controls


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvEmulationDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdvEmulationDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDefaults();
	afx_msg void OnMaximum();
	afx_msg void OnSeperatedClicked();
	afx_msg void OnBmpmodeClicked();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVEMULATIONDLG_H__4AC409B4_17EB_4295_84E9_814AF3E31BB4__INCLUDED_)
