#if !defined(AFX_AUTOHIDE_H__20010516_C3E9_46AD_95FE_0080AD509054__INCLUDED_)
#define AFX_AUTOHIDE_H__20010516_C3E9_46AD_95FE_0080AD509054__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// autohide.h - An AutoHide control
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2001 Bjarke Viksoe.
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
   #error autohide.h requires atlapp.h to be included first
#endif


#define ATL_SIMPLE_AUTOHIDEVIEW_STYLE \
   (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS)

// Sliding window animation delay
#ifndef AUTOHIDE_DELAY_IN
   #define AUTOHIDE_DELAY_IN  200
   #define AUTOHIDE_DELAY_OUT 100
#endif

// AnimateWindow() constants defined in Platform SDK
#ifndef AW_BLEND
   #define AW_HOR_POSITIVE             0x00000001
   #define AW_HOR_NEGATIVE             0x00000002
   #define AW_HIDE                     0x00010000
   #define AW_ACTIVATE                 0x00020000
   #define AW_SLIDE                    0x00040000
#endif

struct AUTOPANE
{
   HWND hWnd;
   TCHAR szTitle[80];
   RECT rc;
};

#define WM_AUTOHIDE_SETPANE   WM_USER+360
#define WM_AUTOHIDE_VIEWCLOSE WM_USER+361


///////////////////////////////////////////////////////
// CAutoFloatWindow

typedef CWinTraits<WS_OVERLAPPED|WS_CAPTION|WS_THICKFRAME|WS_SYSMENU, WS_EX_TOOLWINDOW> CAutoFloatWinTraits;

template <class T, class TBase = CWindow, class TWinTraits = CAutoFloatWinTraits>
class ATL_NO_VTABLE CAutoFloatWindowImpl : 
   public CWindowImpl< T, TBase, TWinTraits >
{
public:
   DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS, NULL)

   typedef CAutoFloatWindowImpl< T , TBase, TWinTraits > thisClass;
   BEGIN_MSG_MAP(thisClass)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
      MESSAGE_HANDLER(WM_PRINT, OnPrint)
      MESSAGE_HANDLER(WM_NCHITTEST, OnHitTest)
      MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
      MESSAGE_HANDLER(WM_NCACTIVATE, OnKillFocus)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
      MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
      MESSAGE_HANDLER(WM_AUTOHIDE_SETPANE, OnSetPane);
   END_MSG_MAP()

   CAutoFloatWindowImpl()
      : m_hwndOwner(NULL)
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
         if( lpfnAnimateWindow!=NULL ) return lpfnAnimateWindow( m_hWnd, dwTime, dwFlags );
      }
      // For incompatible AnimateWindow() Windows versions
      if( dwFlags & AW_ACTIVATE ) {
         SetWindowPos(HWND_TOP, 0,0,0,0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
      }
      if( dwFlags & AW_HIDE ) {
         SetWindowPos(HWND_TOP, 0,0,0,0, SWP_HIDEWINDOW|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
      }
      return FALSE;
   }

   void _DrawFrame(HDC hDC) const
   {
      // Repaint frame borders (except the border on the right)
      CDCHandle dc(hDC);
      RECT rcWin;
      GetWindowRect(&rcWin);
      RECT rcTop = { 0,0, (rcWin.right-rcWin.left)-m_sizeBorder.cx, m_sizeBorder.cy };
      dc.FillRect(&rcTop, ::GetSysColorBrush(COLOR_BTNFACE));
      RECT rcBottom = { 0, (rcWin.bottom-rcWin.top)-m_sizeBorder.cy, (rcWin.right-rcWin.left)-m_sizeBorder.cx, (rcWin.bottom-rcWin.top) };
      dc.FillRect(&rcBottom, ::GetSysColorBrush(COLOR_BTNFACE));
      RECT rcLeft = { 0, m_sizeBorder.cy, m_sizeBorder.cx, (rcWin.bottom-rcWin.top)-m_sizeBorder.cy };
      dc.FillRect(&rcLeft, ::GetSysColorBrush(COLOR_BTNFACE));
   }

   // Message handlers

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      BOOL bDummy;
      OnSettingChange(WM_SETTINGCHANGE, 0,0, bDummy);
      return 0;
   }

   LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      m_sizeBorder.cx = ::GetSystemMetrics(SM_CXSIZEFRAME);
      m_sizeBorder.cy = ::GetSystemMetrics(SM_CYSIZEFRAME);
      return 0;
   }

   LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      return 1; // handled, no background painting needed
   }

   LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
      CWindowDC dc(m_hWnd);
      //HDC hdc = ::GetDCEx(m_hWnd, (HRGN)wParam, DCX_WINDOW|DCX_INTERSECTRGN);
      //if( wParam>1 ) dc.SelectClipRgn((HRGN)wParam);
      _DrawFrame(dc);
      return lRes;
   }

   LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
   {
      if( (BOOL)wParam==FALSE ) {
         _AnimateWindow(AUTOHIDE_DELAY_OUT, AW_SLIDE|AW_HOR_NEGATIVE|AW_HIDE);
         RECT rc;
         GetWindowRect(&rc);
         ::SendMessage(m_hwndOwner, WM_AUTOHIDE_VIEWCLOSE, rc.right-rc.left,0);
      }
      bHandled = FALSE;
      return 0;
   }

   LRESULT OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
      switch( lRes ) {
      case HTBOTTOM:
      case HTBOTTOMLEFT:
      case HTLEFT:
      case HTTOPLEFT:
      case HTTOP:
      case HTCAPTION:
         lRes = HTCLIENT;
         break;
      case HTTOPRIGHT:
      case HTBOTTOMRIGHT:
         lRes = HTRIGHT;
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
      RECT rc;
      GetClientRect(&rc);
      ::SetWindowPos(m_pane.hWnd, HWND_TOP, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, SWP_NOZORDER | SWP_SHOWWINDOW);
      return 0;
   }

   LRESULT OnPrint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
      if( lParam & PRF_NONCLIENT ) _DrawFrame((HDC)wParam);
      return lRes;
   }

   LRESULT OnSetPane(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      ATLASSERT(::IsWindow((HWND)wParam));
      ATLASSERT(lParam!=0);
      if( m_pane.hWnd ) ::ShowWindow(m_pane.hWnd, SW_HIDE);
      m_hwndOwner = (HWND)wParam;
      m_pane = *(reinterpret_cast<AUTOPANE *>(lParam));
      // Re-parent
      ::SetParent(m_pane.hWnd, m_hWnd);
      // Set title
      SetWindowText(m_pane.szTitle);
      // Place view inside pane
      // NOTE: OnSize() also calls ShowWindow() to restore an invisible child
      BOOL bDummy;
      OnSize(WM_SIZE, 0,0, bDummy);
      // Entré...
      _AnimateWindow(AUTOHIDE_DELAY_IN, AW_SLIDE|AW_HOR_POSITIVE|AW_ACTIVATE);
      // Make it top-most
      RECT rc;
      GetWindowRect(&rc);
      SetWindowPos(HWND_TOP, &rc, SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOSIZE);
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

template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits>
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

   enum { DEFAULT_AUTOPANE_SIZE = 140 };

   CSimpleArray<AUTOPANE> m_panes;
   CFont m_font;
   CBrush m_hbrBack;
   //
   CAutoFloatWindow m_wndFloat;
   int m_cx;
   int m_iPaneShown;
   //
   bool m_bMouseTracked;
   int m_iPaneTracked;

   CAutoPaneWindowImpl() : 
      m_bMouseTracked(false), 
      m_iPaneTracked(0), 
      m_iPaneShown(-1), 
      m_cx(DEFAULT_AUTOPANE_SIZE)
   {
   }

   // Operations

   void AddPane(AUTOPANE *pPane)
   {
      ATLASSERT(pPane);
      m_panes.Add(*pPane);  
      T* pT = static_cast<T*>(this);
      pT->UpdateLayout();
      Invalidate();
   }

   void RemovePane(AUTOPANE *pPane)
   {
      ATLASSERT(pPane);
      for( int i=0; i<m_panes.GetSize(); i++ ) {
         if( pPane->hWnd==m_panes[i].hWnd ) {
            m_panes.RemoveAt(i);
            break;
         }
      }
   }

   // Implementation

   int _HitTest(POINT pt) const
   {
      for( int i=0; i<m_panes.GetSize(); i++ ) {
         if( ::PtInRect(&m_panes[i].rc, pt) ) return i;
      }
      return -1;
   }

   // Overridables

   enum { PADDING_X = 1 };
   enum { PADDING_Y = 4 };

   void UpdateLayout()
   {
      CClientDC dc(m_hWnd);
      HFONT hOldFont = dc.SelectFont(m_font);

      RECT rcClient;
      GetClientRect(&rcClient);
      int cnt = m_panes.GetSize();
      POINT pt = { 4,8 };
      for( int i=0; i<cnt; i++ ) {
         AUTOPANE pane = m_panes[i];
         RECT rc = { 0 };
         ::DrawText(dc, pane.szTitle, ::lstrlen(pane.szTitle), &rc, DT_SINGLELINE|DT_CALCRECT);
         pane.rc.left = rc.top;
         pane.rc.top = rc.left;
         pane.rc.right = rc.bottom;
         pane.rc.bottom = rc.right;
         ::InflateRect(&pane.rc, PADDING_X, PADDING_Y);
         ::OffsetRect(&pane.rc, pt.x, pt.y);
         m_panes.SetAtIndex(i, pane);

         pt.y += (pane.rc.bottom-pane.rc.top) + 6;
      }

      dc.SelectFont(hOldFont);
   }

   // Message handlers

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);
      m_wndFloat.Create(m_hWnd, rcDefault, NULL);
      BOOL bDummy;
      pT->OnSettingChange(0,0,0,bDummy);    
      return 0;
   }

   LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      CPaintDC dc(m_hWnd);

      HFONT hOldFont = dc.SelectFont(m_font);
      dc.SetBkMode(OPAQUE);
      dc.SetBkColor(::GetSysColor(COLOR_BTNFACE));
      dc.SetTextColor(::GetSysColor(COLOR_BTNTEXT));

      int cnt = m_panes.GetSize();
      for( int i=0; i<cnt; i++ ) {
         AUTOPANE pane = m_panes[i];
         RECT &rc = pane.rc;
         dc.FillRect(&rc, ::GetSysColorBrush(COLOR_BTNFACE));
         dc.TextOut(rc.right + (PADDING_X-1), rc.top + (PADDING_Y), pane.szTitle, ::lstrlen(pane.szTitle));
      }
      
      dc.SelectFont(hOldFont);

      return 0;
   }

   LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      CDCHandle dc((HDC)wParam);
      RECT rc;
      GetClientRect(&rc);
      HBRUSH hOldBrush = dc.SelectBrush(m_hbrBack);
      dc.PatBlt(rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, PATCOPY);
      dc.SelectBrush(hOldBrush);
      return 1;
   }

   LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      // Initialize skewed font
      if( !m_font.IsNull() ) m_font.DeleteObject();
      NONCLIENTMETRICS ncm = { 0 };
      ncm.cbSize = sizeof(ncm);
      ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
      LOGFONT &lf = ncm.lfMenuFont;
      lf.lfEscapement = 2700;                 // Rotate 270 degrees
      lf.lfClipPrecision |= CLIP_LH_ANGLES;   // For Win95 compatibility (Q82932)
      lf.lfOutPrecision = OUT_TT_ONLY_PRECIS; // Choose a truetype font (Q72021)
      m_font.CreateFontIndirect(&lf);
      ATLASSERT(!m_font.IsNull());

      // Background brush
      if( !m_hbrBack.IsNull() ) m_hbrBack.DeleteObject();
      CWindowDC dc(NULL);
      int nBitsPerPixel = dc.GetDeviceCaps(BITSPIXEL);
      if( nBitsPerPixel > 8 ) {
         COLORREF clrBtnHilite = ::GetSysColor(COLOR_BTNHILIGHT);
         COLORREF clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
         COLORREF clrLight = 
            RGB( GetRValue(clrBtnFace) + ((GetRValue(clrBtnHilite) - GetRValue(clrBtnFace)) / 2),
                 GetGValue(clrBtnFace) + ((GetGValue(clrBtnHilite) - GetGValue(clrBtnFace)) / 2),
                 GetBValue(clrBtnFace) + ((GetBValue(clrBtnHilite) - GetBValue(clrBtnFace)) / 2),
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
         if( iHit!=-1 ) {
            TRACKMOUSEEVENT tme = { 0 };
            tme.cbSize = sizeof(tme);
            tme.hwndTrack = m_hWnd;
            tme.dwFlags = TME_HOVER | TME_LEAVE;
            tme.dwHoverTime = HOVER_DEFAULT;
            ::_TrackMouseEvent(&tme);
            //
            m_iPaneTracked = iHit;
            m_bMouseTracked = true;
         }
      }
      return 0;
   }

   LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      BOOL bDummy;
      OnButtonClick(uMsg, wParam, lParam, bDummy);
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
      if( iHit!=-1 && iHit!=m_iPaneShown ) {
         m_wndFloat.ShowWindow(SW_HIDE);
         RECT rc;
         GetWindowRect(&rc);
         rc.left = rc.right;
         rc.right = rc.left + m_cx;
         m_wndFloat.MoveWindow(&rc);
         m_wndFloat.SendMessage(WM_AUTOHIDE_SETPANE, (WPARAM)m_hWnd, (LPARAM)&m_panes[iHit]);
         m_iPaneShown = iHit;
      }
      return 0;
   }

   LRESULT OnCloseView(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      m_iPaneShown = -1;
      m_cx = wParam;
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

template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits>
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

   CAutoPaneWindow m_wndPane;
   HWND m_hwndClient;
   SIZE m_sizeBorder;
   int m_cxPane;

   // Message handlers

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);
      m_wndPane.Create(m_hWnd, rcDefault, NULL, WS_VISIBLE|WS_CHILD);
      BOOL bDummy;
      pT->OnSettingChange(0,0,0,bDummy);    
      return 0;
   }

   LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      m_sizeBorder.cx = ::GetSystemMetrics(SM_CXEDGE);
      m_sizeBorder.cy = ::GetSystemMetrics(SM_CYEDGE);
      
      NONCLIENTMETRICS ncm = { 0 };
      ncm.cbSize = sizeof(ncm);
      ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
      m_cxPane = ncm.iSmCaptionHeight + 10;
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
      RECT rect;   
      GetClientRect(&rect);
      RECT rcClient = rect;
      RECT rcPane = rect;
      rcPane.right = rcPane.left + m_cxPane;
      rcClient.left += m_cxPane;

      m_wndPane.SetWindowPos(HWND_TOP, &rcPane, SWP_NOACTIVATE|SWP_NOZORDER);
      if( ::IsWindow(m_hwndClient) ) {
         // Map client rectangle to windows's coord system
         ::MapWindowPoints(m_hWnd, ::GetParent(m_hwndClient), (LPPOINT) &rcClient, sizeof(rcClient)/sizeof(POINT));
         ::SetWindowPos(m_hwndClient, HWND_TOP, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, SWP_NOACTIVATE|SWP_NOZORDER);
      }
   }

   // Operations

   void SetClient(HWND hWnd)
   {
      ATLASSERT(::IsWindow(hWnd));
      m_hwndClient = hWnd;
   }

   void AddWindow(HWND hWnd)
   {
      ATLASSERT(::IsWindow(hWnd));
      AUTOPANE pane = { 0 };
      pane.hWnd = hWnd;
      ::GetWindowText(hWnd, pane.szTitle, (sizeof(pane.szTitle)/sizeof(TCHAR))-1);
      m_wndPane.AddPane(&pane);
   }

   void SetPaneSize(int cx)
   {
      m_wndPane.m_cx = cx;
   }
};

class CAutoHide : public CAutoHideImpl<CAutoHide>
{
public:
   DECLARE_WND_CLASS_EX(_T("WTL_AutoHide"), 0, NULL)
};


#endif // !defined(AFX_AUTOHIDE_H__20010516_C3E9_46AD_95FE_0080AD509054__INCLUDED_)

