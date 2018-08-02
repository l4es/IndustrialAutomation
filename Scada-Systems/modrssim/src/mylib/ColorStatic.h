#if !defined(AFX_COLORSTATIC_H__38E16084_70AB_45D2_9CE9_D09537E6F278__INCLUDED_)
#define AFX_COLORSTATIC_H__38E16084_70AB_45D2_9CE9_D09537E6F278__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorStatic window

class CColorStatic : public CStatic
{
// Construction
public:
	CColorStatic();

// Attributes
public:

// Operations
public:

   void SetColor(COLORREF color) {m_color = color;};
   COLORREF GetColor() {return(m_color);};
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorStatic();

protected:
   CFont        m_font;            // underline font for text control
   COLORREF     m_color;           // color 

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorStatic)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

   afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CSampler window

class CSampler : public CEdit
{
// Construction
public:
	CSampler();

   void SetNewColor(COLORREF color);   // only call after window is created
   COLORREF CSampler::BrowseColor();       // open the colour browser dialog

   // Attributes
   COLORREF GetColor() {return(m_color);}
private:


   COLORREF m_color;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampler)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSampler();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSampler)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
}; // class CSampler


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORSTATIC_H__38E16084_70AB_45D2_9CE9_D09537E6F278__INCLUDED_)
