#if !defined(AFX_COMMSLAMP_H__CC09A381_06D6_4D45_95C3_A6583D61501D__INCLUDED_)
#define AFX_COMMSLAMP_H__CC09A381_06D6_4D45_95C3_A6583D61501D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CommsLamp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCommsLamp window

class CCommsLamp : public CButton
{
// Construction
public:
	CCommsLamp();

   BOOL SetupLamp(DWORD onTimerDuration);
// Attributes
public:
   COLORREF GetONcolor() { return m_ONcolor;};
   COLORREF GetOFFcolor() { return m_OFFcolor;};
   BOOL GetState() { return m_state;};

// Operations
public:
   void SetONcolor(COLORREF color) {m_ONcolor = color; UpdateMyCtrl();};
   void SetOFFcolor(COLORREF color) {m_OFFcolor = color; UpdateMyCtrl();};
   void SetState(BOOL state);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommsLamp)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCommsLamp();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCommsLamp)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

   // methods
   void UpdateMyCtrl();

   // variables
   BOOL m_state;
   COLORREF m_ONcolor, m_OFFcolor;

   DWORD m_timerDuration;
   DWORD m_timerID;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMSLAMP_H__CC09A381_06D6_4D45_95C3_A6583D61501D__INCLUDED_)
