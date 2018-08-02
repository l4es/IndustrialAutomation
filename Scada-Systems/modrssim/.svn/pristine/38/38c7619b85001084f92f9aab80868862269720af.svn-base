#if !defined(AFX_STARWARSCTRL_H__B75B0B9E_367D_11D6_AB38_00D0B70C3D79__INCLUDED_)
#define AFX_STARWARSCTRL_H__B75B0B9E_367D_11D6_AB38_00D0B70C3D79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StarWarsCtrl.h : header file
//
// See "StarWarsCtrl.cpp" for details

#define NUM_STARS 100


class CStarWarsCtrl : public CStatic
{
	struct CStar
	{
		int x;
		int y;
		int z;
	};

	CStar m_StarArray[NUM_STARS];
		
// Construction
public:
	CStarWarsCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStarWarsCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void AddTextLine(LPCTSTR lpszText);
	void SetStarSpeed(int nSpeed);
	void SetScrollSpeed(int nSpeed);
	virtual ~CStarWarsCtrl();

	// Generated message map functions
protected:
	int m_nStarsSpeed;
	int m_nScrollSpeed;
	int m_nScrollPos;
	void DoScrollText(CDC *pDC);
	void DoStars(CDC *pDC);
	void InvalidateCtrl();

	CFont m_Font;

	CRect  m_rectClient;

	CDC     m_MainDC;
	CBitmap m_MainBitmap;

	CBitmap *m_pOldBitmap;

	CStringArray m_TextLines;
	//{{AFX_MSG(CStarWarsCtrl)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STARWARSCTRL_H__B75B0B9E_367D_11D6_AB38_00D0B70C3D79__INCLUDED_)
