#if !defined(AFX_TRAININGSIMDLG_H__CD8860EF_5F04_46CF_8D62_1F6F6DBAD2D8__INCLUDED_)
#define AFX_TRAININGSIMDLG_H__CD8860EF_5F04_46CF_8D62_1F6F6DBAD2D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrainingSimDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrainingSimDlg dialog

#include "plantsim.h"

class CTrainingSimDlg : public CDialog
{
// Construction
public:
	CTrainingSimDlg(CWnd* pParent = NULL);   // standard constructor

   void ToggleCheckOnItem(int checkID, int bitNum);
   int m_selectedRegister;	
   SetCurrentTopic  m_topic;

// Dialog Data
	//{{AFX_DATA(CTrainingSimDlg)
	enum { IDD = IDD_TRAININGSIM };
	CComboBox	m_registerNames;
	//}}AFX_DATA

   // TOOLTIPS
   CToolTipCtrl    m_ToolTip;
   int m_numDisabledControls;
   CWnd *m_pDisabledControls[256];      //only can get 256 controls

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrainingSimDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTrainingSimDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSet();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeCbregisternames();
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnCheck3();
	afx_msg void OnCheck4();
	afx_msg void OnCheck5();
	afx_msg void OnCheck6();
	afx_msg void OnCheck7();
	afx_msg void OnCheck8();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAININGSIMDLG_H__CD8860EF_5F04_46CF_8D62_1F6F6DBAD2D8__INCLUDED_)
