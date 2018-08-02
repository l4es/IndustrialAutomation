#if !defined(AFX_AUTOHIDE_H__20010516_C3E9_46AD_95FE_0080AD509054__INCLUDED_)
#define AFX_AUTOHIDE_H__20010516_C3E9_46AD_95FE_0080AD509054__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// AutoHideXp.h - An AutoHide control
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2001-2003 Bjarke Viksoe.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#ifndef __cplusplus
   #error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
   #error AutoHideXP.h requires atlapp.h to be included first
#endif


#define ATL_SIMPLE_AUTOHIDEVIEW_STYLE \
   (WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS)

// Sliding window animation delay
#ifndef AUTOHIDE_DELAY_IN
   #define AUTOHIDE_DELAY_IN  200
   #define AUTOHIDE_DELAY_OUT 100
#endif

#define AUTOHIDE_LEFT    0
#define AUTOHIDE_BOTTOM  1

typedef struct
{
   HWND hWnd;
   TCHAR szTitle[80];
   int iImage;
   int iDirection;
   RECT rc;
} AUTOPANE;

#define WM_AUTOHIDE_SETPANE    WM_USER + 360
#define WM_AUTOHIDE_VIEWCLOSE  WM_USER + 361


///////////////////////////////////////////////////////
// CAutoFloatWindow

typedef CWinTraits<WS_POPUP|WS_CAPTION|WS_THICKFRAME|WS_SYSMENU, WS_EX_TOOLWINDOW> CAutoFloatWinTraits;

template< class T, class TBase = CWindow, class TWinTraits = CAutoFloatWinTraits >
class ATL_NO_VTABLE CAutoFloatWindowImpl : 
   public CWindowImpl< T, TBase, TWinTraits >
{
public:
   DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS, NULL)

   typedef CAutoFloatWindowImpl< T , TBase, TWinTraits > thisClass;

#ifndef AW_SLIDE
   // AnimateWindow() constants defined in Platform SDK
   enum
   {
      AW_HOR_POSITIVE = 0x00000001,
      AW_HOR_NEGATIVE = 0x00000002,
      AW_VER_POSITIVE = 0x00000004,
      AW_VER_NEGATIVE = 0x00000008,
      AW_HIDE         = 0x00010000,
      AW_ACTIVATE     = 0x00020000,
      AW_SLIDE        = 0x00040000,
   };
#endif // AW_SLIDE

   BEGIN_MSG_MAP(CAutoFloatWindowImpl)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
      MESSAGE_HANDLER(WM_PRINT, OnPrint)
      MESSAGE_HANDLER(WM_NCHITTEST, OnHitTest)
      MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
      MESSAGE_HANDLER(WM_NCACTIVATE, OnNcActivate)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
      MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
      MESSAGE_HANDLER(WM_AUTOHIDE_SETPANE, OnSetPane);
   END_MSG_MAP()

   CAutoFloatWindowImpl() : 
      m_hwndOwner(NULL)
   {
      ::ZeroMemory(&m_pane, sizeof(m_pane));
   }
   
   HWND m_hwndOwner;
   AUTOPANE m_pane;
   SIZE m_sizeBorder;

   // Operations

   BOOL _AnimateWindow(DWORD dwTime, DWORD dwFlags)
   {
      if( !AtlIsOldWindows() ) {
         typedef BOOL (CALLBACK* LPFNANIMATEWINDOW)(HWND,DWORD,DWORD);
         LPFNANIMATEWINDOW lpfnAnimateWindow = (LPFNANIMATEWINDOW)
            ::GetProcAddress(::GetModuleHandle(_T("user32.dll")), "AnimateWindow");
         if( lpfnAnimateWindow != NULL ) return lpfnAnimateWindow( m_hWnd, dwTime, dwFlags );
      }
      // For incompatible AnimateWindow() Windows versions
      if( dwFlags & AW_ACTIVATE ) {
         SetWindowPos(HWND_TOP, 0,0,0,0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER);
      }
      if( dwFlags & AW_HIDE ) {
         SetWindowPos(HWND_TOP, 0,0,0,0, SWP_HIDEWINDOW|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER);
      }
      return FALSE;
   }

   void _DrawFrame(HDC hDC) const
   {
      // Repaint frame borders (except the border on the right)
      CDCHandle dc = hDC;
      HBRUSH hBrushBorder = ::GetSysColorBrush(COLOR_ACTIVEBORDER);
      HBRUSH hBrushGrey = ::GetSysColorBrush(COLOR_BTNFACE);
      RECT rcWin;
      GetWindowRect(&rcWin);
      RECT rcLeft = { 0, m_sizeBorder.cy, m_sizeBorder.cx, (rcWin.bottom - rcWin.top) - m_sizeBorder.cy };
      dc.FillRect(&rcLeft, m_pane.iDirection == AUTOHIDE_LEFT ? hBrushGrey : hBrushBorder);
      RECT rcBottom = { 0, (rcWin.bottom - rcWin.top) - m_sizeBorder.cy, (rcWin.right - rcWin.left) - m_sizeBorder.cx, (rcWin.bottom - rcWin.top) };
      dc.FillRect(&rcBottom, m_pane.iDirection == AUTOHIDE_LEFT ? hBrushBorder : hBrushGrey);
      if( m_pane.iDirection == AUTOHIDE_LEFT ) {
         RECT rcTop = { 0, 0, (rcWin.right - rcWin.left) - m_sizeBorder.cx, m_sizeBorder.cy };
         dc.FillRect(&rcTop, hBrushBorder);
      }
      else {
         RECT rcRight = { (rcWin.right - rcWin.left) - m_sizeBorder.cx, 0, (rcWin.right - rcWin.left), (rcWin.bottom - rcWin.top) };
         dc.FillRect(&rcRight, hBrushBorder);
      }
   }

   // Message handlers

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      SendMessage(WM_SETTINGCHANGE);
      return 0;
   }
   LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      m_sizeBorder.cx = ::GetSystemMetrics(SM_CYSIZEFRAME);
      m_sizeBorder.cy = ::GetSystemMetrics(SM_CYSIZEFRAME);
      return 0;
   }
   LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      return 1; // handled, no background painting needed
   }
   LRESULT OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc();
      CWindowDC dc(m_hWnd);
      _DrawFrame(dc);
      return lRes;
   }
   LRESULT OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
   {
      // When it becomes inactive, close the view
      if( (BOOL) wParam == FALSE ) {
         DWORD dwFlags = m_pane.iDirection == AUTOHIDE_LEFT ? AW_HOR_NEGATIVE : AW_VER_POSITIVE;
         _AnimateWindow(AUTOHIDE_DELAY_OUT, AW_SLIDE|dwFlags|AW_HIDE);
         RECT rc;
         GetWindowRect(&rc);
         WPARAM iSize = m_pane.iDirection == AUTOHIDE_LEFT ? rc.right - rc.left : rc.bottom - rc.top;
         ::SendMessage(m_hwndOwner, WM_AUTOHIDE_VIEWCLOSE, iSize, 0L);
      }
      bHandled = FALSE;
      return 0;
   }
   LRESULT OnHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc();
      switch( lRes ) {
      case HTBOTTOM:
      case HTBOTTOMLEFT:
      case HTLEFT:
      case HTCAPTION:
      case HTTOPRIGHT:
         lRes = HTCLIENT;
         break;
      case HTTOP:
      case HTTOPLEFT:
         lRes = m_pane.iDirection == AUTOHIDE_BOTTOM ? HTTOP : HTCLIENT;
         break;
      case HTRIGHT:
      case HTBOTTOMRIGHT:
         lRes = m_pane.iDirection == AUTOHIDE_LEFT ? HTRIGHT : HTCLIENT;
         break;
      }
      return lRes;
   }
   LRESULT OnSysCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
   {
      switch( wParam & 0xFFF0 ) {
      case SC_CLOSE:
         ::SetFocus(m_hwndOwner); // Kill focus
         return 0;
      }
      bHandled = FALSE;
      return 0;
   }
   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      if( !::IsWindow(m_pane.hWnd) ) return 0;
      RECT rc;
      GetClientRect(&rc);
      ::SetWindowPos(m_pane.hWnd, HWND_TOP, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER|SWP_SHOWWINDOW);
      return 0;
   }
   LRESULT OnPrint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc();
      if( lParam & PRF_NONCLIENT ) _DrawFrame( (HDC) wParam );
      return lRes;
   }
   LRESULT OnSetPane(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      ATLASSERT(::IsWindow((HWND)wParam));
      ATLASSERT(lParam!=0);
      if( m_pane.hWnd != NULL ) ::ShowWindow(m_pane.hWnd, SW_HIDE);
      m_hwndOwner = (HWND) wParam;
      m_pane = *(reinterpret_cast<AUTOPANE *>(lParam));
      // Re-parent
      ::SetParent(m_pane.hWnd, m_hWnd);
      // Set title
      SetWindowText(m_pane.szTitle);
      // Place view inside pane
      // NOTE: OnSize() also calls ShowWindow() to restore an invisible child
      BOOL bDummy;
      OnSize(WM_SIZE, 0,0, bDummy);
      UpdateWindow();
      // Entré...
      DWORD dwFlags = m_pane.iDirection == AUTOHIDE_LEFT ? AW_HOR_POSITIVE : AW_VER_NEGATIVE;
      _AnimateWindow(AUTOHIDE_DELAY_IN, AW_SLIDE|dwFlags|AW_ACTIVATE);
      // Make it top-of-the-world
      RECT rcWin;
      GetWindowRect(&rcWin);
      SetWindowPos(HWND_TOP, &rcWin, SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER);
      // Set focus to child view
      ::SetFocus(m_pane.hWnd);
      //::InvalidateRect(m_pane.hWnd, NULL, TRUE);
      return 0;
   }
};

class CAutoFloatWindow : public CAutoFloatWindowImpl<CAutoFloatWindow>
{
public:
   DECLARE_WND_CLASS_EX(_T("WTL_AutoFloatWindow"), CS_DBLCLKS, NULL)
};


///////////////////////////////////////////////////////
// CAutoPaneWindow

template< class T, class TBase = CWindow, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CAutoPaneWindowImpl : 
   public CWindowImpl< T, TBase, TWinTraits >
{
public:
   DECLARE_WND_CLASS_EX(NULL, CS_HREDRAW|CS_VREDRAW, NULL)

   typedef CAutoPaneWindowImpl< T , TBase, TWinTraits > thisClass;

   BEGIN_MSG_MAP(thisClass)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_PAINT, OnPaint)
      MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
      MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
      MESSAGE_HANDLER(WM_MOUSEHOVER, OnMouseHover)
      MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
      MESSAGE_HANDLER(WM_LBUTTONDOWN, OnButtonClick)
      MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
      MESSAGE_HANDLER(WM_AUTOHIDE_VIEWCLOSE, OnCloseView)
   END_MSG_MAP()

   enum { DEFAULT_AUTOHIDE_SIZE = 140 };

   CSimpleArray<AUTOPANE> m_panes;
   //
   CFont m_font;
   int m_iFontHeight;
   CBrush m_hbrBack;
   CImageList m_Images;
   //
   CAutoFloatWindow m_wndFloat;
   int m_cxy;
   int m_iCurPaneShown;
   int m_iDirection;
   //
   bool m_bMouseTracked;
   int m_iPaneTracked;

   CAutoPaneWindowImpl() : 
      m_bMouseTracked(false), 
      m_iPaneTracked(0), 
      m_iCurPaneShown(-1), 
      m_iDirection(AUTOHIDE_LEFT),
      m_cxy(DEFAULT_AUTOHIDE_SIZE)
   {
   }

   // Operations

   void AddPane(AUTOPANE& Pane)
   {
      m_panes.Add(Pane);  
      T* pT = static_cast<T*>(this);
      pT->UpdateLayout();
      Invalidate();
   }
   void RemovePane(AUTOPANE *pPane)
   {
      ATLASSERT(pPane);
      for( int i = 0; i < m_panes.GetSize(); i++ ) {
         if( pPane->hWnd == m_panes[i].hWnd ) {
            m_panes.RemoveAt(i);
            break;
         }
      }
      T* pT = static_cast<T*>(this);
      pT->UpdateLayout();
      Invalidate();
   }
   AUTOPANE* FindPane(HWND hWnd)
   {
      for( int i = 0; i < m_panes.GetSize(); i++ ) if( m_panes[i].hWnd == hWnd ) return &m_panes[i];
      return NULL;
   }
   BOOL ActivatePane(HWND hWnd)
   {
      const AUTOPANE* pPane = FindPane(hWnd);
      if( pPane == NULL ) return FALSE;
      SendMessage(WM_LBUTTONDOWN, 0, MAKELPARAM(pPane->rc.left, pPane->rc.top));
      return TRUE;
   }
   void SetImageList(HIMAGELIST hImageList)
   {
      m_Images = hImageList;
   }

   // Implementation

   int _HitTest(POINT pt) const
   {
      for( int i = 0; i < m_panes.GetSize(); i++ ) if( ::PtInRect(&m_panes[i].rc, pt) ) return i;
      return -1;
   }

   // Overridables

   void UpdateLayout()
   {
      enum 
      { 
         PADDING_X = 16,
         PADDING_Y = 16,
      };
      CClientDC dc(m_hWnd);
      HFONT hOldFont = dc.SelectFont(m_font);

      RECT rcClient;
      GetClientRect(&rcClient);

      int cnt = m_panes.GetSize();
      POINT pt = { 0, 8 };
      for( int i = 0; i < cnt; i++ ) {
         AUTOPANE pane = m_panes[i];
         RECT rc = { 0 };
         ::DrawText(dc, pane.szTitle, ::lstrlen(pane.szTitle), &rc, DT_SINGLELINE|DT_CALCRECT);
         if( m_iDirection == AUTOHIDE_LEFT ) {
            pane.rc.left = rc.top;
            pane.rc.top = rc.left;
            pane.rc.right = rc.bottom;
            pane.rc.bottom = rc.right;
            pane.rc.right = (rcClient.right - rc.left) - 2;
            pane.rc.bottom += (PADDING_X * 2);
            ::OffsetRect(&pane.rc, pt.x, pt.y);
            pt.y += (pane.rc.bottom - pane.rc.top) + 6;
         }
         else {
            // NOTE: The point 'pt' is sort of reversed here, so don't
            //       mind the strange use of 'x' and 'y'.
            pane.rc.top = rc.top + 2;
            pane.rc.left = rc.left;
            pane.rc.bottom = (rcClient.bottom - rc.top);
            pane.rc.right = rc.right + (PADDING_X * 2);
            ::OffsetRect(&pane.rc, pt.y, pt.x);
            pt.y += (pane.rc.right - pane.rc.left) + 6;
         }
         m_panes.SetAtIndex(i, pane);
      }

      dc.SelectFont(hOldFont);
   }

   // Message handlers

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      m_wndFloat.Create(m_hWnd, rcDefault, NULL);
      ATLASSERT(::IsWindow(m_wndFloat));
      SendMessage(WM_SETTINGCHANGE);
      return 0;
   }
   LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      CPaintDC dc(m_hWnd);

      HFONT hOldFont = dc.SelectFont(m_font);
      dc.SetBkMode(TRANSPARENT);
      dc.SetBkColor(::GetSysColor(COLOR_BTNFACE));
      dc.SetTextColor(::GetSysColor(COLOR_BTNTEXT));

      int cnt = m_panes.GetSize();
      for( int i = 0; i < cnt; i++ ) {
         AUTOPANE pane = m_panes[i];
         RECT rcBorder = pane.rc;
         dc.FillRect(&rcBorder, ::GetSysColorBrush(COLOR_BTNFACE));
         dc.Draw3dRect(&rcBorder, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNSHADOW));
         POINT ptImage = { pane.rc.left + 4, pane.rc.top + 3 };
         m_Images.Draw(dc, pane.iImage, ptImage, ILD_TRANSPARENT);
         if( m_iDirection == AUTOHIDE_LEFT ) {
            POINT ptText = { pane.rc.left + m_iFontHeight + 12, pane.rc.top + 16 + 8 };
            dc.TextOut(ptText.x, ptText.y, pane.szTitle, ::lstrlen(pane.szTitle));
         }
         else {
            POINT ptText = { pane.rc.left + 16 + 8, pane.rc.top + 4 };
            dc.TextOut(ptText.x, ptText.y, pane.szTitle, ::lstrlen(pane.szTitle));
         }
      }
      
      dc.SelectFont(hOldFont);

      return 0;
   }
   LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      CDCHandle dc = (HDC) wParam;
      RECT rc;
      GetClientRect(&rc);
      HBRUSH hOldBrush = dc.SelectBrush(m_hbrBack);
      dc.PatBlt(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, PATCOPY);
      dc.SelectBrush(hOldBrush);
      return 1;
   }
   LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      CWindowDC dc(NULL);

      // Initialize skewed font
      if( !m_font.IsNull() ) m_font.DeleteObject();
      NONCLIENTMETRICS ncm = { 0 };
      ncm.cbSize = sizeof(ncm);
      ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
      LOGFONT lf = ncm.lfMenuFont;
      if( m_iDirection == AUTOHIDE_LEFT ) {
         lf.lfEscapement = 2700;                 // Rotate 270 degrees
         lf.lfClipPrecision |= CLIP_LH_ANGLES;   // For Win95 compatibility (Q82932)
         lf.lfOutPrecision = OUT_TT_ONLY_PRECIS; // Choose a truetype font (Q72021)
      }
      m_font.CreateFontIndirect(&lf);
      ATLASSERT(!m_font.IsNull());
      ::GetObject(m_font, sizeof(LOGFONT), &lf);
      m_iFontHeight = ::MulDiv(-lf.lfHeight, 72, ::GetDeviceCaps(dc, LOGPIXELSY));

      // Background brush
      if( !m_hbrBack.IsNull() ) m_hbrBack.DeleteObject();
      int nBitsPerPixel = dc.GetDeviceCaps(BITSPIXEL);
      if( nBitsPerPixel > 8 ) {
         COLORREF clrBtnHilite = ::GetSysColor(COLOR_BTNHILIGHT);
         COLORREF clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
         COLORREF clrLight = 
            RGB( GetRValue(clrBtnFace) + ((GetRValue(clrBtnHilite) - GetRValue(clrBtnFace)) / 3 * 2),
                 GetGValue(clrBtnFace) + ((GetGValue(clrBtnHilite) - GetGValue(clrBtnFace)) / 3 * 2),
                 GetBValue(clrBtnFace) + ((GetBValue(clrBtnHilite) - GetBValue(clrBtnFace)) / 3 * 2)
               );
         m_hbrBack.CreateSolidBrush(clrLight);
      }
      else {
         m_hbrBack =  CDCHandle::GetHalftoneBrush();
      }
      Invalidate();
      return 0;
   }
   LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
      if( !m_bMouseTracked ) {
         int iHit = _HitTest(pt);
         if( iHit != -1 ) {
            TRACKMOUSEEVENT tme = { 0 };
            tme.cbSize = sizeof(tme);
            tme.hwndTrack = m_hWnd;
            tme.dwFlags = TME_HOVER|TME_LEAVE;
            tme.dwHoverTime = HOVER_DEFAULT;
            ::_TrackMouseEvent(&tme);
            //
            m_iPaneTracked = iHit;
            m_bMouseTracked = true;
         }
      }
      return 0;
   }
   LRESULT OnMouseHover(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      BOOL bDummy;
      OnButtonClick(WM_LBUTTONDOWN, wParam, lParam, bDummy);
      m_bMouseTracked = false;
      return 0;
   }
   LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      m_bMouseTracked = false;
      return 0;
   }
   LRESULT OnButtonClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
      int iHit = _HitTest(pt);
      if( iHit != -1 && iHit != m_iCurPaneShown ) {
         m_wndFloat.ShowWindow(SW_HIDE);
         RECT rc;
         GetWindowRect(&rc);
         if( !IsWindowVisible() ) {
            ::GetWindowRect(GetTopLevelParent(), &rc);
            m_iDirection == AUTOHIDE_LEFT ? rc.right = rc.left : rc.top = rc.bottom;
         }
         if( m_iDirection == AUTOHIDE_LEFT ) {
            rc.left = rc.right;
            rc.right = rc.left + m_cxy;
         }
         else {
            rc.bottom = rc.top;
            rc.top = rc.top - m_cxy;
         }
         m_wndFloat.MoveWindow(&rc);
         m_wndFloat.SendMessage(WM_AUTOHIDE_SETPANE, (WPARAM) m_hWnd, (LPARAM) &m_panes[iHit]);
         m_iCurPaneShown = iHit;
      }
      return 0;
   }
   LRESULT OnCloseView(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      m_iCurPaneShown = -1;
      m_cxy = wParam;
      return 0;
   }
};

class CAutoPaneWindow : public CAutoPaneWindowImpl<CAutoPaneWindow>
{
public:
   DECLARE_WND_CLASS_EX(_T("WTL_AutoPaneWindow"), 0, NULL)
};


///////////////////////////////////////////////////////
// CAutoHide

template< class T, class TBase = CWindow, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CAutoHideImpl : 
   public CWindowImpl< T, TBase, TWinTraits >
{
public:
   DECLARE_WND_CLASS_EX(NULL, CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, NULL)

   typedef CAutoHideImpl< T , TBase, TWinTraits > thisClass;

   BEGIN_MSG_MAP(thisClass)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
   END_MSG_MAP()

   CAutoPaneWindow m_wndPane[2];
   HWND m_hWndClient;
   SIZE m_sizeBorder;
   int m_cxyPane;

   // Message handlers

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      m_wndPane[AUTOHIDE_LEFT].m_iDirection = AUTOHIDE_LEFT;
      m_wndPane[AUTOHIDE_LEFT].Create(m_hWnd, rcDefault, NULL, WS_VISIBLE|WS_CHILD);
      m_wndPane[AUTOHIDE_BOTTOM].m_iDirection = AUTOHIDE_BOTTOM;
      m_wndPane[AUTOHIDE_BOTTOM].Create(m_hWnd, rcDefault, NULL, WS_VISIBLE|WS_CHILD);
      SendMessage(WM_SETTINGCHANGE);
      return 0;
   }

   LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      m_sizeBorder.cx = ::GetSystemMetrics(SM_CXEDGE);
      m_sizeBorder.cy = ::GetSystemMetrics(SM_CYEDGE);
      
      NONCLIENTMETRICS ncm = { 0 };
      ncm.cbSize = sizeof(ncm);
      ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
      m_cxyPane = ncm.iMenuHeight + 6;
      Invalidate();
      return 0;
   }

   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);
      pT->UpdateLayout();
      return 0;
   }

   LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      return 1; // handled, no background painting needed
   }

   // Overridables

   void UpdateLayout()
   {
      ATLASSERT(::IsWindow(m_hWnd));

      RECT rcClient;   
      GetClientRect(&rcClient);
     
      if( m_wndPane[AUTOHIDE_LEFT].m_panes.GetSize() > 0 ) {
         RECT rcPane = rcClient;
         rcPane.right = rcPane.left + m_cxyPane;
         rcClient.left += m_cxyPane;
         m_wndPane[AUTOHIDE_LEFT].SetWindowPos(HWND_TOP, &rcPane, SWP_NOACTIVATE|SWP_NOZORDER);
         m_wndPane[AUTOHIDE_LEFT].UpdateLayout();
      }
      if( m_wndPane[AUTOHIDE_BOTTOM].m_panes.GetSize() > 0 ) {
         RECT rcPane = rcClient;
         rcPane.top = rcPane.bottom - m_cxyPane;
         rcClient.bottom -= m_cxyPane;
         m_wndPane[AUTOHIDE_BOTTOM].SetWindowPos(HWND_TOP, &rcPane, SWP_NOACTIVATE|SWP_NOZORDER);
         m_wndPane[AUTOHIDE_BOTTOM].UpdateLayout();
      }
      if( ::IsWindow(m_hWndClient) ) {
         // Map client rectangle to windows's coord system
         ::MapWindowPoints(m_hWnd, ::GetParent(m_hWndClient), (LPPOINT) &rcClient, sizeof(rcClient)/sizeof(POINT));
         ::SetWindowPos(m_hWndClient, HWND_TOP, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, SWP_NOACTIVATE|SWP_NOZORDER);
      }
   }

   // Operations

   void SetClient(HWND hWnd)
   {
      ATLASSERT(::IsWindow(hWnd));
      ATLASSERT(::GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD);
      m_hWndClient = hWnd;
   }
   BOOL AddView(HWND hWnd, int iDirection, int iImage)
   {
      ATLASSERT(::IsWindow(hWnd));
      ATLASSERT(::IsWindow(m_hWndClient));
      ATLASSERT(iDirection==AUTOHIDE_LEFT || iDirection==AUTOHIDE_BOTTOM);
      AUTOPANE pane = { 0 };
      pane.hWnd = hWnd;
      ::ZeroMemory(pane.szTitle, sizeof(pane.szTitle));
      ::GetWindowText(hWnd, pane.szTitle, (sizeof(pane.szTitle)/sizeof(TCHAR))-1);
      pane.iImage = iImage;
      pane.iDirection = iDirection;
      m_wndPane[iDirection].AddPane(pane);
      T* pT = static_cast<T*>(this);
      pT->UpdateLayout();
      return TRUE;
   }
   BOOL RemoveView(HWND hWnd)
   {
      ATLASSERT(::IsWindow(hWnd));
      AUTOPANE* pPane = m_wndPane[AUTOHIDE_LEFT].FindPane(hWnd);
      if( pPane ) m_wndPane[AUTOHIDE_LEFT].RemovePane(pPane);
      pPane = m_wndPane[AUTOHIDE_BOTTOM].FindPane(hWnd);
      if( pPane ) m_wndPane[AUTOHIDE_BOTTOM].RemovePane(pPane);
      T* pT = static_cast<T*>(this);
      pT->UpdateLayout();
      return TRUE;
   }
   BOOL ActivateView(HWND hWnd)
   {
      ATLASSERT(::IsWindow(hWnd));
      BOOL bRes = m_wndPane[AUTOHIDE_LEFT].ActivatePane(hWnd);
      if( !bRes ) bRes |= m_wndPane[AUTOHIDE_BOTTOM].ActivatePane(hWnd);
      return bRes;
   }
   void SetPaneSize(int iDirection, int cx)
   {
      ATLASSERT(iDirection==AUTOHIDE_LEFT || iDirection==AUTOHIDE_BOTTOM);
      m_wndPane[iDirection].m_cxy = cx;
   }
   int GetPaneSize(int iDirection) const
   {
      ATLASSERT(iDirection==AUTOHIDE_LEFT || iDirection==AUTOHIDE_BOTTOM);
      return m_wndPane[iDirection].m_cxy;
   }
   void SetImageList(HIMAGELIST hImageList)
   {
      m_wndPane[AUTOHIDE_LEFT].SetImageList(hImageList);
      m_wndPane[AUTOHIDE_BOTTOM].SetImageList(hImageList);
   }
};

class CAutoHideXP : public CAutoHideImpl<CAutoHideXP>
{
public:
   DECLARE_WND_CLASS_EX(_T("WTL_AutoHideXP"), 0, NULL)
};


#endif // !defined(AFX_AUTOHIDE_H__20010516_C3E9_46AD_95FE_0080AD509054__INCLUDED_)
