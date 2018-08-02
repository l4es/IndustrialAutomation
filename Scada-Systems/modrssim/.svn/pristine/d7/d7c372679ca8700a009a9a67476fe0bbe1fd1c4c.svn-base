#ifndef _STATLINK_CLASS_H_INCLUDED
#define _STATLINK_CLASS_H_INCLUDED
 ////////////////////////////////////////////////////////////////// 
 // CStaticLink 1997 Microsoft Systems Journal. 
 // If this program works, it was written by Paul DiLascia.
 // If not, I don't know who wrote it.
 ////////////////
 // CStaticLink implements a static control that's a hyperlink
 // to any file on your desktop or web. You can use it in dialog boxes
 // to create hyperlinks to web sites. When clicked, opens the file/URL
 //
 class CStaticLink : public CStatic {
 public:
     CStaticLink();

	//{{AFX_VIRTUAL(CStaticLink)
	public:
	//}}AFX_VIRTUAL
     
     // you can change these any time:
     COLORREF     m_colorUnvisited;         // color for unvisited
     COLORREF     m_colorVisited;           // color for visited
     BOOL         m_bVisited;               // whether visited or not
 
     // URL/filename for non-text controls (e.g., icon, bitmap) or when link is
     // different from window text. If you don't set this, CStaticLink will
     // use GetWindowText to get the link.
     CString     m_link;
     HCURSOR      m_hHandCursor;
 
     void SetHoverCursorHandle(HCURSOR handle) {m_hHandCursor = handle;}

 protected:
     DECLARE_DYNAMIC(CStaticLink)
     CFont         m_font;                  // underline font for text control

	//{{AFX_MSG(CStaticLink)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
     
     // message handlers
     afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
     afx_msg void    OnClicked();
     DECLARE_MESSAGE_MAP()
 };

#endif // _STATLINK_CLASS_H_INCLUDED