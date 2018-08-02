/////////////////////////////////////////////////////////////////////////////
//
// FILE: MicroTick.h : headder file
//
// See _README.CPP
//
// Defines a class derieved from CButton, it is a specialized check-box
// that uses colours and displays "text" inside the tick area, making it 
// smaller that a check-box. This is great if you have screen real-estate
// limitations.
// Works almost exactly like a tick-box.
//
// Fixes: Border box drawn correctly (and within client rect)
//        colors change-able
// There are 6 colors to supply:
//
//                   background           text                   border
// +---------------+-------------------+-----------------------+-------------------+
// | tick marked   | SetCheckedColor   | SetTextColor          |                   |
// | tick cleared  | SetUncheckedColor | SetUncheckedTextColor |                   |
// | activity      |                   |                       | SetActiveColor    |
// | no activity   |                   |                       | SetInactiveColor  |
// +---------------+-------------------+-----------------------+-------------------+
// The border is a non-user accessible GUI driver by the program to show internal 
// status, in our case to show activity.
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_MICROTICK_H__51526765_6F63_11D3_B17D_00A0248E2104__INCLUDED_)
#define AFX_MICROTICK_H__51526765_6F63_11D3_B17D_00A0248E2104__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CMicroTick window

class CMicroTick : public CButton
{
// Construction
public:
	CMicroTick();
	virtual ~CMicroTick();

// Operations
public:
   ////////////////////////////////////////////////////////////
   // text color
   void SetTextColor(COLORREF color) {m_textColor=color;};
   void SetUnCheckedTextColor(COLORREF color) {m_uncheckedTextColor=color;};
   // border 
   void SetBorderWidth(int width) { m_borderWidth=width;};
   void SetBorderState(BOOL active) {m_backgroundState = active;};
   BOOL GetBorderState() {return(m_backgroundState);};

   // check-box state members
   void SetCheck(int nCheck);
   void SetCheckedColor(COLORREF color) {m_checked=color;\
                                        InvalidateRect(NULL);};
   void SetUnCheckedColor(COLORREF color) {m_unchecked=color;\
                                        InvalidateRect(NULL);};
   int GetCheck();
   COLORREF GetCheckedColor() {return(m_checked);};
   COLORREF GetUnCheckedColor() {return(m_unchecked);};

   // background members
   void SetActiveColor(COLORREF color) {m_backgroundActive=color;\
                                        InvalidateRect(NULL);};
   void SetInactiveColor(COLORREF color) {m_backgroundInActive=color;\
                                        InvalidateRect(NULL);};
   
   COLORREF GetActiveColor() {return(m_backgroundActive);};
   COLORREF GetInactiveColor() {return(m_backgroundInActive);};

private:
   // tick-box members
   DWORD    m_microTickState;       // checked state
   COLORREF m_unchecked;            // unchecked color
   COLORREF m_checked;              // checked color

   // border members
   DWORD    m_borderWidth;          // border width
   DWORD    m_backgroundState;      // border on/off
   COLORREF m_backgroundInActive;   // idle color
   COLORREF m_backgroundActive;     // active color

   // text colors
   COLORREF m_textColor;            // normal text color
   COLORREF m_uncheckedTextColor;   // text color for in un-checked state

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMicroTick)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
private:
   CFont *m_pFont;

	// Generated message map functions
protected:
	//{{AFX_MSG(CMicroTick)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClicked();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MICROTICK_H__51526765_6F63_11D3_B17D_00A0248E2104__INCLUDED_)
