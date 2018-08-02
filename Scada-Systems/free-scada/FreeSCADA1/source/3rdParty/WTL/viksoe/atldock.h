#ifndef __ATL_DOCK_H__
#define __ATL_DOCK_H__

/////////////////////////////////////////////////////////////////////////////
// atldock.h - Docking framework for the WTL library
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Thanks to Mike Simon for adding the ability to close a view.
// Copyright (c) 2000-2002 Bjarke Viksoe.
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

#pragma once

#ifndef __cplusplus
   #error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
   #error atldock.h requires atlapp.h to be included first
#endif


// Dock positions
#define DOCK_LEFT      0
#define DOCK_TOP       1
#define DOCK_RIGHT     2
#define DOCK_BOTTOM    3
#define DOCK_FLOAT     4
#define DOCK_HIDDEN    5
#define DOCK_LASTKNOWN 6

// Extended dock styles
#define DCK_EX_DESTROYONCLOSE 0x00000001
#define DCK_EX_REMEMBERSIZE   0x00000002

// Control style flags
#define DCK_NOLEFT      1<<DOCK_LEFT
#define DCK_NOTOP       1<<DOCK_TOP
#define DCK_NORIGHT     1<<DOCK_RIGHT
#define DCK_NOBOTTOM    1<<DOCK_BOTTOM
#define DCK_NOSPLITTER  0x00000100L
#define DCK_NOFLOAT     0x00000200L
#define DCK_NOHIDE      0x00000400L

#define ATL_SIMPLE_DOCKVIEW_STYLE \
   (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS)

// Docked child command chaining macro
#define CHAIN_DOCK_CHILD_COMMANDS(hwnd) \
   if(uMsg == WM_COMMAND) \
   { \
      if(::IsWindowVisible(hwnd)) \
         ::SendMessage(hwnd, uMsg, wParam, lParam); \
   }

// Docking position helpers
inline bool IsDockedVertically(short Side) { return (Side == DOCK_LEFT) || (Side == DOCK_RIGHT); };
inline bool IsDocked(short Side) { return (Side == DOCK_TOP) || (Side == DOCK_BOTTOM) || (Side == DOCK_LEFT) || (Side == DOCK_RIGHT); };


#define DOCK_INFO_CHILD 0x1000

// Minimum size of docking pane
#define MIN_DOCKPANE_SIZE 28
// Default width/height of docking pane
#define DEFAULT_DOCKPANE_SIZE 170
// Default size of floating window rectangle
#define DEFAULT_FLOAT_SIZE 120
// The splitter size in pixels
#define DEFAULT_SPLITTER_SIZE 6

#define WM_DOCK_QUERYRECT         WM_USER+840
#define WM_DOCK_QUERYTRACK        WM_USER+841
#define WM_DOCK_UNDOCK            WM_USER+842
#define WM_DOCK_UNFLOAT           WM_USER+843
#define WM_DOCK_DOCK              WM_USER+844
#define WM_DOCK_FLOAT             WM_USER+845
#define WM_DOCK_UPDATELAYOUT      WM_USER+846
#define WM_DOCK_REPOSITIONWINDOW  WM_USER+847
#define WM_DOCK_SETSPLITTER       WM_USER+848
#define WM_DOCK_CLIENT_CLOSE      WM_USER+849

class CDockingPaneChildWindow;
class CFloatingWindow;

struct DOCKCONTEXT 
{
   HWND hwndDocked;   // The docked pane
   HWND hwndFloated;  // The floating pane
   HWND hwndChild;    // The view window
   HWND hwndOrigPrnt; // The original parent window
   short Side;        // Dock state
   short LastSide;    // Last dock state
   RECT rcWindow;     // Preferred window size
   SIZE sizeFloat;    // Last window size (floating)
   HWND hwndRoot;     // Main dock window
   //
   DWORD dwFlags;     // Extra flags
   bool bKeepSize;    // Recommend using current size and avoid rescale
};

typedef CSimpleValArray<DOCKCONTEXT*> CDockMap;

struct TRACKINFO 
{
   HWND hWnd;
   DOCKCONTEXT* pCtx;
   POINT ptPos;
   POINT ptStart;
   RECT rc;
   short Side;
};


///////////////////////////////////////////////////////
// CSplitterBar

#pragma warning(disable : 4100)

template< class T >
class CSplitterBar
{
public:
   LONG m_cxySplitter;
   bool m_bTracking;
   bool m_bDragging;
   static HCURSOR s_hVertCursor;
   static HCURSOR s_hHorizCursor;

   CDCHandle m_dc;
   POINT m_ptStartDragPoint;
   POINT m_ptEndDragPoint;
   POINT m_ptDeltaDragPoint;
   RECT  m_rcTracker;
   SIZE  m_sizeTracker;
   RECT  m_rcTrackerBounds;   

   CSplitterBar() :
      m_bTracking(false), m_bDragging(false)
   {
      if( s_hVertCursor == NULL ) {
         ::EnterCriticalSection(&_Module.m_csStaticDataInit);
         s_hVertCursor = ::LoadCursor(NULL, IDC_SIZENS);
         s_hHorizCursor = ::LoadCursor(NULL, IDC_SIZEWE);
         ::LeaveCriticalSection(&_Module.m_csStaticDataInit);
      }
      m_sizeTracker.cx = ::GetSystemMetrics(SM_CXFRAME);
      m_sizeTracker.cy = ::GetSystemMetrics(SM_CYFRAME);
      m_cxySplitter = DEFAULT_SPLITTER_SIZE;
   }

   LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      T* pT = static_cast<T*>(this);
      DWORD dwPos = ::GetMessagePos();
      POINT ptPos = { GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos) };
      pT->ScreenToClient(&ptPos);
      if( ::PtInRect(&pT->m_rcSplitter, ptPos) ) return 1;
      bHandled = FALSE;
      return 0;
   }

   void DrawGhostBar()
   {
      ATLASSERT(!m_dc.IsNull());
      RECT rect = m_rcTracker;
      if( !::IsRectEmpty(&rect) ) {
         // Invert the brush pattern (looks just like frame window sizing)
         CBrush brush = CDCHandle::GetHalftoneBrush();
         if( brush.m_hBrush != NULL ) {
            ATLASSERT(!m_dc.IsNull());
            CBrushHandle brushOld = m_dc.SelectBrush(brush);
            m_dc.PatBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, PATINVERT);
            m_dc.SelectBrush(brushOld);
         }
      }
   }

   void DrawDragBar()
   {
      ATLASSERT(!m_dc.IsNull());
      RECT rect = m_rcTracker;
      if( !::IsRectEmpty(&rect) ) {
         // Invert the brush pattern (looks just like frame window sizing)
         CBrush brush = CDCHandle::GetHalftoneBrush();
         if( brush.m_hBrush != NULL ) {
            ATLASSERT(!m_dc.IsNull());
            CBrushHandle brushOld = m_dc.SelectBrush(brush);
            m_dc.PatBlt(rect.left + m_sizeTracker.cx, rect.top, rect.right - rect.left - (m_sizeTracker.cx * 2), m_sizeTracker.cy, PATINVERT);
            m_dc.PatBlt(rect.left, rect.bottom - m_sizeTracker.cy, rect.right - rect.left, m_sizeTracker.cy, PATINVERT);
            m_dc.PatBlt(rect.left, rect.top, m_sizeTracker.cx, rect.bottom - rect.top - m_sizeTracker.cy, PATINVERT);
            m_dc.PatBlt(rect.right - m_sizeTracker.cx, rect.top, m_sizeTracker.cx, rect.bottom - rect.top - m_sizeTracker.cy, PATINVERT);
            m_dc.SelectBrush(brushOld);
         }
      }
   }

   void DrawSplitterBar(CDCHandle dc, bool bVertical, RECT& rect)
   {      
      if( ::IsRectEmpty(&rect) ) return;
      dc.FillRect(&rect, ::GetSysColorBrush(COLOR_3DFACE));
      dc.DrawEdge(&rect, EDGE_RAISED, (bVertical ? (BF_TOP|BF_BOTTOM) : (BF_LEFT|BF_RIGHT)));
   }

   bool PtInSplitter(POINT& pt, short Side, DWORD dwFlags, RECT& rcSplitter)
   {
      if( !IsDocked(Side) ) return false;
      if( m_bTracking ) return false;
      if( (dwFlags & DCK_NOSPLITTER) != 0 ) return false;
      if( !::PtInRect(&rcSplitter, pt) ) return false;
      return true;
   }

   // Track loop

   bool Track(bool bDragging)
   {
      T* pT = static_cast<T*>(this);
      StartTracking(bDragging);
      // Get messages until capture lost or cancelled/accepted
      while( ::GetCapture() == pT->m_hWnd ) {
         MSG msg;
         if( !::GetMessage(&msg, NULL, 0, 0) ) {
            ::PostQuitMessage(msg.wParam);
            break;
         }
         switch( msg.message ) {
         case WM_LBUTTONUP:
            if( m_bDragging ) pT->OnEndDrag(); else pT->OnEndResize();
            CancelTracking();
            return true;
         case WM_MOUSEMOVE:
            if( m_bDragging ) pT->OnMove(msg.pt); else pT->OnStretch(msg.pt);
            break;
         case WM_KEYUP:
            if( m_bDragging ) pT->OnKey((int) msg.wParam, false) ;
            break;
         case WM_KEYDOWN:
            if( m_bDragging ) pT->OnKey((int) msg.wParam, true);
            if( msg.wParam == VK_ESCAPE ) {
               CancelTracking();
               return false;
            }
            break;
         case WM_SYSKEYDOWN:
         case WM_LBUTTONDOWN:
         case WM_RBUTTONDOWN:
            CancelTracking();
            return false;      
         default:
            // Just dispatch rest of the messages
            ::DispatchMessage(&msg);
            break;
         }
      }

      CancelTracking();
      return false;
   }

   void StartTracking(bool bDragging)
   {
      ATLASSERT(!m_bTracking);
      T* pT = static_cast<T*>(this);
      // Capture window
      m_bTracking = true;
      pT->SetCapture();
      // Make sure no updates are pending
      pT->RedrawWindow(NULL, NULL, RDW_ALLCHILDREN|RDW_UPDATENOW);
      // Lock Window update while dragging over desktop
      ATLASSERT(m_dc.IsNull());
      HWND hWnd = ::GetDesktopWindow();
      m_dc = ::GetDCEx(hWnd, NULL, ::LockWindowUpdate(hWnd) ? DCX_WINDOW|DCX_CACHE|DCX_LOCKWINDOWUPDATE : DCX_WINDOW|DCX_CACHE);
      ATLASSERT(!m_dc.IsNull());
      // Draw the initial focus rect
      m_bDragging = bDragging;
      if( m_bDragging ) DrawDragBar(); else DrawGhostBar();
      return;
   }

   void CancelTracking()
   {
      ATLASSERT(m_bTracking);
      if( !m_bTracking ) return;
      // Erase the focus rect
      if( m_bDragging ) DrawDragBar(); else DrawGhostBar();
      // Let window updates free
      ::LockWindowUpdate(NULL);
      HWND hWnd = ::GetDesktopWindow();
      if( !m_dc.IsNull() ) ::ReleaseDC(hWnd, m_dc.Detach());
      // Release the capture
      ::ReleaseCapture();
      m_bTracking = false;
   }

   // Overridables

   void OnEndDrag() { };
   void OnEndResize() { };
   void OnKey(int nCode, bool bDown) { };
   void OnMove(POINT& pt) { };
   void OnStretch(POINT& pt) { };
};

template<class T> HCURSOR CSplitterBar<T>::s_hVertCursor = NULL;
template<class T> HCURSOR CSplitterBar<T>::s_hHorizCursor = NULL;

#pragma warning(default : 4100)


///////////////////////////////////////////////////////
// CFloatingWindow

typedef CWinTraits<WS_OVERLAPPED|WS_CAPTION|WS_THICKFRAME|WS_SYSMENU, WS_EX_TOOLWINDOW|WS_EX_WINDOWEDGE> CFloatWinTraits;

template< class T, class TBase = CWindow, class TWinTraits = CFloatWinTraits >
class ATL_NO_VTABLE CFloatingWindowImpl : 
   public CWindowImpl< T, TBase, TWinTraits >,
   public CSplitterBar<CFloatingWindowImpl>
{
public:
   DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS, NULL)

   typedef CFloatingWindowImpl< T , TBase, TWinTraits > thisClass;
   
   BEGIN_MSG_MAP(CFloatingWindowImpl)
      MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
      MESSAGE_HANDLER(WM_MENUCHAR, OnMenuChar)
      MESSAGE_HANDLER(WM_GETMINMAXINFO, OnMsgForward)
      MESSAGE_HANDLER(WM_DOCK_UPDATELAYOUT, OnSize)
      MESSAGE_HANDLER(WM_NCACTIVATE, OnNcActivate)
      MESSAGE_HANDLER(WM_NCLBUTTONDOWN, OnLeftButtonDown)
      MESSAGE_HANDLER(WM_NCRBUTTONDOWN, OnRightButtonDown)
      MESSAGE_HANDLER(WM_NCLBUTTONDBLCLK, OnButtonDblClick)
   END_MSG_MAP()

   DOCKCONTEXT* m_pCtx;

   CFloatingWindowImpl(DOCKCONTEXT* pCtx) :
      m_pCtx(pCtx)
   { 
   }

   HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
         DWORD dwStyle = 0, DWORD dwExStyle = 0,
         UINT nID = 0, LPVOID lpCreateParam = NULL)
   {
      ATLASSERT(m_pCtx);
      if( m_pCtx->dwFlags & DCK_NOHIDE ) dwStyle = T::GetWndStyle(dwStyle) & ~WS_SYSMENU;
      return CWindowImpl< T, TBase, TWinTraits >::Create(hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
   }

   virtual void OnFinalMessage(HWND /*hWnd*/)
   {
      delete (T*) this;
   }

   // Message handlers

   LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      return 1; // handled, no background painting needed
   }

   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);
      pT->UpdateLayout();
      return 0;
   }

   LRESULT OnNcActivate(UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      return DefWindowProc(uMsg, IsWindowEnabled(), lParam);
   }

   LRESULT OnLeftButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   {
      SetFocus();
      if( wParam == HTCAPTION ) {
         // Get cursor point and start a tracking look
         POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
         m_ptStartDragPoint = m_ptEndDragPoint = pt;
         GetWindowRect(&m_rcTracker);
         // Enter tracking loop
         bool res = Track(true);
         if( res ) {
            // Determine if we landed over a docking pane or just moved around...
            TRACKINFO ti = { m_hWnd, m_pCtx, m_ptEndDragPoint.x, m_ptEndDragPoint.y, m_ptStartDragPoint.x, m_ptStartDragPoint.y };
            ::SendMessage(m_pCtx->hwndRoot, WM_DOCK_QUERYTRACK, 0, (LPARAM) &ti);
            if( ti.Side == DOCK_FLOAT ) {
               MoveWindow(&ti.rc, TRUE);
            }
            else {
               ::SendMessage(m_pCtx->hwndRoot, WM_DOCK_UNFLOAT, 0, (LPARAM) m_pCtx);
               ::SendMessage(m_pCtx->hwndRoot, WM_DOCK_DOCK, ti.Side, (LPARAM) m_pCtx);
            }
            return 0;
         }
         return 1;
      }
      bHandled = FALSE;
      return 0;
   }

   LRESULT OnRightButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
   {
      // We don't particular like the right - mousebutton. We need to
      // disable the system menu because it causes problems...
      if( wParam == HTCAPTION ) return 1;
      bHandled = FALSE;
      return 0;
   }

   LRESULT OnButtonDblClick(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
   {
      // Don't like double-clicks either. We need to disable
      // maximize on double-click for floating windows as well.
      if( wParam == HTCAPTION ) return 1;
      bHandled = FALSE;
      return 0;
   }

   LRESULT OnMenuChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      // Forward a failed menu-key strike to main window.
      // Perhaps it will show the menu for this key.
      LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
      if( HIWORD(lRes) == MNC_IGNORE ) {
         CWindow wndRoot = m_pCtx->hwndRoot;
         return ::SendMessage(wndRoot.GetTopLevelParent(), uMsg, wParam, lParam);
      }
      return lRes;
   }

   LRESULT OnSysCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
   {
      switch( wParam & 0xFFF0 ) {
      case SC_CLOSE:
         if( m_pCtx->dwFlags & DCK_NOHIDE ) return 0;
         // Use post message so it comes async
         ::PostMessage(m_pCtx->hwndRoot, WM_DOCK_CLIENT_CLOSE, WM_DOCK_UNFLOAT, (LPARAM) m_pCtx);
         return 0;
      }
      bHandled = FALSE;
      return 0;
   }

   LRESULT OnMsgForward(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      // Forward to view
      return ::SendMessage(m_pCtx->hwndChild, uMsg, wParam, lParam);
   }

   // Track methods

   void OnMove(POINT& pt)
   {
      TRACKINFO ti = { m_hWnd, m_pCtx, pt.x, pt.y, m_ptStartDragPoint.x, m_ptStartDragPoint.y };
      ::SendMessage(m_pCtx->hwndRoot, WM_DOCK_QUERYTRACK, 0, (LPARAM) &ti);
      if( !::EqualRect(&m_rcTracker, &ti.rc) ) {
         DrawDragBar();
         m_rcTracker = ti.rc;
         DrawDragBar();
      }
      m_ptEndDragPoint = pt;
   }

   // Overridables

   void UpdateLayout()
   {
      RECT rc;
      GetWindowRect(&rc);
      m_pCtx->sizeFloat.cx = rc.right - rc.left;
      m_pCtx->sizeFloat.cy = rc.bottom - rc.top;
      GetClientRect(&rc);
      ::SetWindowPos(m_pCtx->hwndChild, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
   }
};

class CFloatingWindow : public CFloatingWindowImpl<CFloatingWindow>
{
public:
   DECLARE_WND_CLASS_EX(_T("WTL_FloatingWindow"), CS_DBLCLKS, NULL)

   CFloatingWindow(DOCKCONTEXT* ctx) :
      CFloatingWindowImpl<CFloatingWindow>(ctx)
   { 
   }
};


///////////////////////////////////////////////////////
// CDockingPaneChildWindow

template< class T, class TBase = CWindow, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CDockingPaneChildWindowImpl : 
   public CWindowImpl< T, TBase, TWinTraits >,
   public CSplitterBar<CDockingPaneChildWindowImpl>
{
public:
   DECLARE_WND_CLASS_EX(NULL, CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, NULL)

   typedef CDockingPaneChildWindowImpl< T , TBase, TWinTraits > thisClass;
   
   BEGIN_MSG_MAP(CDockingPaneChildWindowImpl)
      MESSAGE_HANDLER(WM_PAINT, OnPaint)
      MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
      MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
      MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
      MESSAGE_HANDLER(WM_LBUTTONDOWN, OnButtonDown)
      MESSAGE_HANDLER(WM_LBUTTONUP, OnButtonUp)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_DOCK_UPDATELAYOUT, OnSize)
      MESSAGE_HANDLER(WM_DOCK_SETSPLITTER, OnSetSplitter)
   END_MSG_MAP()

   DOCKCONTEXT* m_pCtx;
   RECT m_rcChild;
   RECT m_rcSplitter;
   RECT m_rcGripper;
   RECT m_rcCloseButton;
   int m_cxyGripper;
   int m_cxyCloseButton;
   bool m_fCloseCapture;
   bool m_fCloseDown;

   CDockingPaneChildWindowImpl(DOCKCONTEXT* pCtx) :
      m_pCtx(pCtx), 
      m_fCloseDown(false), 
      m_fCloseCapture(false)
   {
      ::SetRectEmpty(&m_rcChild);
      ::SetRectEmpty(&m_rcGripper);
      ::SetRectEmpty(&m_rcSplitter);
      ::SetRectEmpty(&m_rcCloseButton);
      //m_cxyGripper = ::GetSystemMetrics(SM_CYCAPTION)*4/5;
      //m_cxyCloseButton = m_cxyGripper-2;
      m_cxyGripper = ::GetSystemMetrics(SM_CYSMCAPTION);
      m_cxyCloseButton = ::GetSystemMetrics(SM_CYSMSIZE)-4;

   }

   virtual void OnFinalMessage(HWND /*hWnd*/)
   {
      delete (T*) this;
   }

   LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      return 1; // handled, no background painting needed
   }

   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);
      pT->UpdateLayout();
      return 0;
   }

   LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);
      CPaintDC dc(m_hWnd);
      dc.ExcludeClipRect(&m_rcChild);
      RECT rc;
      GetClientRect(&rc);
      dc.FillRect(&rc, ::GetSysColorBrush(COLOR_3DFACE));
      short Side = m_pCtx->Side;
      bool bVertical = IsDockedVertically(Side);
      if( m_cxySplitter>0 ) pT->DrawSplitterBar((HDC) dc, bVertical, m_rcSplitter);
      pT->DrawGripperBar((HDC) dc, Side, m_rcGripper, m_rcCloseButton, m_fCloseDown);
      pT->DrawPaneFrame((HDC) dc, Side, rc);
      return 0;
   }

   LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
   {
      POINT ptPos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
      if( m_fCloseCapture ) {
         // Mouse capture because we're in a close-button loop
         bool fCloseDown = ::PtInRect(&m_rcCloseButton, ptPos) == TRUE;
         if( m_fCloseDown != fCloseDown ) {
            m_fCloseDown = fCloseDown;
            InvalidateRect(&m_rcCloseButton);
         }
      }
      else if( PtInSplitter(ptPos, m_pCtx->Side, m_pCtx->dwFlags, m_rcSplitter) ) {
         bool bVertical = IsDockedVertically(m_pCtx->Side);
         ::SetCursor( bVertical ? s_hVertCursor : s_hHorizCursor);
         bHandled = FALSE;
      }
      return 0;
   }

   LRESULT OnButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      POINT ptPos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
      if( ::PtInRect(&m_rcCloseButton, ptPos) ) {
         // Clicked on close button? Start a mouse capture look because you
         // can still move the mouse away from the button...
         m_fCloseCapture = m_fCloseDown = true;
         InvalidateRect(&m_rcCloseButton);
         SetCapture();
      }
      else if( PtInSplitter(ptPos, m_pCtx->Side, m_pCtx->dwFlags, m_rcSplitter) ) {
         // Clicked on splitter. Start dragging then...
         ::ClientToScreen(m_hWnd,& ptPos);
         m_ptStartDragPoint = m_ptEndDragPoint = m_ptDeltaDragPoint = ptPos;
         //
         RECT rcWin;
         GetWindowRect(&rcWin);
         m_rcTracker = m_rcSplitter;
         ::OffsetRect(&m_rcTracker, rcWin.left, rcWin.top);
         //
         ::GetWindowRect(::GetParent(m_hWnd),& m_rcTrackerBounds);
         ::InflateRect(&m_rcTrackerBounds, -MIN_DOCKPANE_SIZE, -MIN_DOCKPANE_SIZE);
         // Enter tracking loop
         bool res = Track(false);
         if( res ) {
            if( IsDockedVertically(m_pCtx->Side) ) {
               m_pCtx->rcWindow.bottom += m_ptEndDragPoint.y - m_ptStartDragPoint.y;
            }
            else {
               m_pCtx->rcWindow.right += m_ptEndDragPoint.x - m_ptStartDragPoint.x;
            }
            m_pCtx->bKeepSize = true;
            ::SendMessage(GetParent(), WM_DOCK_UPDATELAYOUT, 0,0);
         }
      }
      else if( ::PtInRect(&m_rcGripper, ptPos) ) {
         // Clicked on gripper? Allow user to move/drag the window out...
         ::ClientToScreen(m_hWnd,& ptPos);
         m_ptStartDragPoint = m_ptEndDragPoint = m_ptDeltaDragPoint = ptPos;
         GetWindowRect(&m_rcTracker);
         // Enter tracking loop
         bool res = Track(true);
         if( res ) {
            // Done dragging! Let's determine where the window went...
            TRACKINFO ti = { m_hWnd, m_pCtx, m_ptEndDragPoint.x, m_ptEndDragPoint.y, m_ptStartDragPoint.x, m_ptStartDragPoint.y };
            ::SendMessage(m_pCtx->hwndRoot, WM_DOCK_QUERYTRACK, 0, (LPARAM)&ti);
            if( ti.Side == m_pCtx->Side ) {
               // Dragged window to same side. Move the window to top position...
               RECT rc;
               GetWindowRect(&rc);
               if( !::PtInRect(&rc, m_ptEndDragPoint) ) ::SendMessage(m_pCtx->hwndRoot, WM_DOCK_REPOSITIONWINDOW, 0, (LPARAM) m_pCtx);
            }
            else {
               // Move the window out in the open or to another pane...
               ::SendMessage(m_pCtx->hwndRoot, WM_DOCK_UNDOCK, 0, (LPARAM) m_pCtx);
               if( ti.Side == DOCK_FLOAT ) m_pCtx->rcWindow = ti.rc;
               ::SendMessage(m_pCtx->hwndRoot, ti.Side == DOCK_FLOAT ? WM_DOCK_FLOAT : WM_DOCK_DOCK, ti.Side, (LPARAM) m_pCtx);
            }
         }
      }
      return 0;
   }

   LRESULT OnButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      if( m_fCloseCapture ) {
         // We're in a mouse capture loop. That means we're trying to figure
         // out if the close button is being hit...
         m_fCloseCapture = m_fCloseDown = false;
         ::ReleaseCapture();
         InvalidateRect(&m_rcCloseButton);
         POINT ptPos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
         if( ::PtInRect(&m_rcCloseButton, ptPos) ) {
            ::PostMessage(m_pCtx->hwndRoot, WM_DOCK_CLIENT_CLOSE, WM_DOCK_UNDOCK, (LPARAM) m_pCtx);
         }
      }
      return 0;
   }

   // Track methods

   void OnMove(POINT& pt)
   {
      TRACKINFO ti = { m_hWnd, m_pCtx, pt.x, pt.y, m_ptStartDragPoint.x, m_ptStartDragPoint.y };
      ::SendMessage(m_pCtx->hwndRoot, WM_DOCK_QUERYTRACK, 0, (LPARAM)&ti);
      if( !::EqualRect(&m_rcTracker, &ti.rc) ) {
         DrawDragBar();
         m_rcTracker = ti.rc;
         DrawDragBar();
      }
      m_ptDeltaDragPoint = pt;
   }

   void OnStretch(POINT& pt)
   {
      DrawGhostBar();
      if( IsDockedVertically(m_pCtx->Side) ) {
         int nOffset = pt.y - m_ptDeltaDragPoint.y;
         if( m_rcTracker.top + nOffset <= m_rcTrackerBounds.top ) nOffset = m_rcTrackerBounds.top - m_rcTracker.top;
         if( m_rcTracker.bottom + nOffset >= m_rcTrackerBounds.bottom ) nOffset = m_rcTrackerBounds.bottom - m_rcTracker.bottom;
         ::OffsetRect(&m_rcTracker, 0,nOffset);
         m_ptDeltaDragPoint.y += nOffset;
      }
      else {
         int nOffset = pt.x - m_ptDeltaDragPoint.x;
         if( m_rcTracker.left + nOffset <= m_rcTrackerBounds.left ) nOffset = m_rcTrackerBounds.left - m_rcTracker.left;
         if( m_rcTracker.right + nOffset >= m_rcTrackerBounds.right ) nOffset = m_rcTrackerBounds.right - m_rcTracker.right;
         ::OffsetRect(&m_rcTracker, nOffset,0);
         m_ptDeltaDragPoint.x += nOffset;
      }
      DrawGhostBar();
   }

   void OnEndResize()
   {
      m_ptEndDragPoint = m_ptDeltaDragPoint;
   }

   void OnEndDrag()
   {
      m_ptEndDragPoint = m_ptDeltaDragPoint;
   }

   LRESULT OnSetSplitter(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      m_cxySplitter = wParam;
      return 0;
   }

   // Overridables

   void UpdateLayout()
   {     
      const int CHILD_GAP = 4;
      bool bVertical = IsDockedVertically(m_pCtx->Side);

      // Reposition splitter and gripper bars
      RECT rect;   
      GetClientRect(&rect);
      if( bVertical ) {
         int nGap = 0;
         if( (m_pCtx->dwFlags & DCK_NOHIDE) == 0 ) {
            nGap = CHILD_GAP + m_cxyCloseButton;
            m_rcCloseButton.left = rect.right-nGap;
            m_rcCloseButton.top = rect.top + 2 + (m_cxyGripper - m_cxyCloseButton)/2;
            m_rcCloseButton.right = m_rcCloseButton.left + m_cxyCloseButton;
            m_rcCloseButton.bottom = m_rcCloseButton.top + m_cxyCloseButton;
         }
         ::SetRect(&m_rcGripper, rect.left, rect.top, rect.right-nGap, rect.top + m_cxyGripper);
         ::SetRect(&m_rcSplitter, rect.left, rect.bottom - m_cxySplitter, rect.right, rect.bottom);
         rect.top += m_cxyGripper;
         rect.bottom -= m_cxySplitter;
      }
      else {
         int nGap = 0;
         if( (m_pCtx->dwFlags & DCK_NOHIDE) == 0 ) {
            nGap = CHILD_GAP + m_cxyCloseButton;
            m_rcCloseButton.left = rect.left + 4;
            m_rcCloseButton.top = rect.top + 2 + (m_cxyGripper - m_cxyCloseButton)/2;
            m_rcCloseButton.right = m_rcCloseButton.left + m_cxyCloseButton;
            m_rcCloseButton.bottom = m_rcCloseButton.top + m_cxyCloseButton;
         }
         ::SetRect(&m_rcGripper, rect.left, rect.top+nGap, rect.left + m_cxyGripper, rect.bottom);
         ::SetRect(&m_rcSplitter, rect.right - m_cxySplitter, rect.top, rect.right, rect.bottom);
         rect.left += m_cxyGripper;
         rect.right -= m_cxySplitter;
      }
      // Calculate the client window area
      ::InflateRect(&rect, -CHILD_GAP, -CHILD_GAP);
      m_rcChild = rect;

      ::SetWindowPos(m_pCtx->hwndChild, NULL, m_rcChild.left, m_rcChild.top, m_rcChild.right - m_rcChild.left, m_rcChild.bottom - m_rcChild.top, SWP_NOZORDER | SWP_NOACTIVATE);
   }

   void DrawPaneFrame(CDCHandle /*dc*/, short /*Side*/, const RECT& /*rc*/)
   {
   }

   void DrawGripperBar(CDCHandle dc, short Side, const RECT& rcBar, RECT& rcCloseButton, bool bCloseDown)
   {
      if( ::IsRectEmpty(&rcBar) ) return;
      const int INSET = 4;
      const int LINE_GAP = 4;
      bool bVertical = IsDockedVertically(Side);
      RECT rcLine;
      if( bVertical ) {
         ::SetRect(&rcLine, rcBar.left+INSET, rcBar.top+6, rcBar.right-INSET, rcBar.top+8);
      }
      else {
         ::SetRect(&rcLine, rcBar.left+6, rcBar.top+INSET, rcBar.left+8, rcBar.bottom-INSET);
      }
      dc.Draw3dRect(&rcLine, ::GetSysColor(COLOR_BTNHIGHLIGHT), ::GetSysColor(COLOR_BTNSHADOW));
      ::OffsetRect(&rcLine, bVertical ? 0 : LINE_GAP, bVertical ? LINE_GAP : 0);
      dc.Draw3dRect(&rcLine, ::GetSysColor(COLOR_BTNHIGHLIGHT), ::GetSysColor(COLOR_BTNSHADOW));
      if( !::IsRectEmpty(&rcCloseButton) ) {
         dc.DrawFrameControl(&rcCloseButton, DFC_CAPTION, bCloseDown ? DFCS_CAPTIONCLOSE|DFCS_PUSHED : DFCS_CAPTIONCLOSE);
      }
   }
};

class CDockingPaneChildWindow : public CDockingPaneChildWindowImpl<CDockingPaneChildWindow>
{
public:
   DECLARE_WND_CLASS_EX(_T("WTL_DockingChildWindow"), CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, NULL)

   CDockingPaneChildWindow(DOCKCONTEXT* ctx) : 
      CDockingPaneChildWindowImpl<CDockingPaneChildWindow>(ctx) 
   { 
   }
};


///////////////////////////////////////////////////////
// CDockingPaneWindow

template< class T, class TBase = CWindow, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CDockingPaneWindowImpl : 
   public CWindowImpl< T, TBase, TWinTraits >,
   public CSplitterBar<CDockingPaneWindowImpl>
{
public:
   DECLARE_WND_CLASS_EX(NULL, CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, COLOR_WINDOW)

   typedef CDockingPaneWindowImpl< T , TBase, TWinTraits > thisClass;
   
   BEGIN_MSG_MAP(CDockingPaneWindowImpl)
      MESSAGE_HANDLER(WM_PAINT, OnPaint)
      MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
      MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
      MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
      MESSAGE_HANDLER(WM_LBUTTONDOWN, OnButtonDown)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_DOCK_UPDATELAYOUT, OnRecalcSpace)
   END_MSG_MAP()

   CDockMap m_map;       // Panels docking in this pane
                         // Pointers are owned by the parent window
   DWORD m_dwExtStyle;   // Mirror of extended style of parent window (dock window)
   int m_cy;             // Current size of panel
   int m_cyOld;          // Last size when panel was visible
   short m_Side;         // The side the panel is shown the
   RECT m_rcSplitter;    // Rectangle of the splitter

   CDockingPaneWindowImpl() : 
      m_cy(0), 
      m_cyOld(DEFAULT_DOCKPANE_SIZE), 
      m_dwExtStyle(0L)
   {
      ::SetRectEmpty(&m_rcSplitter);
   }

   // Implementation

   void DockWindow(DOCKCONTEXT* ctx)
   {
      ATLASSERT(ctx);
      if( m_map.GetSize() == 0 ) {
         m_cy = m_cyOld;
         ShowWindow(SW_SHOWNOACTIVATE);
      }
      ctx->Side = m_Side;
      ctx->LastSide = m_Side;
      ::SetParent(ctx->hwndDocked, m_hWnd);
      ::SetParent(ctx->hwndChild, ctx->hwndDocked);
      ::ShowWindow(ctx->hwndDocked, SW_SHOWNOACTIVATE);
      m_map.Add(ctx);
      PostMessage(WM_DOCK_UPDATELAYOUT);
   }

   void UnDockWindow(DOCKCONTEXT* ctx)
   {
      ATLASSERT(ctx);
      for( int i = 0; i < m_map.GetSize(); i++ ) {
         if( m_map[i] == ctx ) {
            m_map.RemoveAt(i);
            ::ShowWindow(ctx->hwndDocked, SW_HIDE);
            break;
         }
      }
      if( m_map.GetSize() == 0 ) {
         if( m_dwExtStyle & DCK_EX_REMEMBERSIZE ) {
            m_cyOld = m_cy; // remember pane size to until next time
         }
         m_cy = 0;
         ShowWindow(SW_HIDE);
      }
      else {
         SendMessage(WM_DOCK_UPDATELAYOUT);
      }
   }

   void RepositionWindow(DOCKCONTEXT* ctx, int iPos)
   {
      if( iPos >= m_map.GetSize() ) return;
      for( int i = 0; i < m_map.GetSize(); i++ ) {
         if( m_map[i] == ctx ) {
            DOCKCONTEXT* pCtx = m_map[iPos];
            m_map.SetAtIndex(iPos, ctx);
            m_map.SetAtIndex(i, pCtx);
            break;
         }
      }
      SendMessage(WM_DOCK_UPDATELAYOUT);
      Invalidate();
   }

   // Message handlers

   LRESULT OnRecalcSpace(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      if( m_map.GetSize() == 0 ) return 0;
      for( int i = 0; i < m_map.GetSize(); i++ ) {
         ::SendMessage(m_map[i]->hwndDocked, WM_DOCK_UPDATELAYOUT, 0,0);
      }
      OnSize(0,0,0,bHandled);
      return 0;
   }

   LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      CPaintDC dc(m_hWnd);
      // Draw splitter along the pane side
      T* pT = static_cast<T*>(this);
      pT->DrawSplitterBar((HDC) dc, !IsDockedVertically(m_Side), m_rcSplitter);
      return 0;
   }

   LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      return 1; // handled, no background painting needed
   }

   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      bHandled = FALSE;
      if( m_map.GetSize() == 0 ) return 1;
      T* pT = static_cast<T*>(this);
      pT->UpdateLayout();
      HDWP hdwp = BeginDeferWindowPos(m_map.GetSize());
      for( int i = 0; i < m_map.GetSize(); i++ ) {
         RECT& rc = m_map[i]->rcWindow;
         ::DeferWindowPos(hdwp, m_map[i]->hwndDocked, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
      }
      EndDeferWindowPos(hdwp);
      return 1;
   }

   LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
   {
      POINT ptPos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
      if( PtInSplitter(ptPos, m_Side, 0, m_rcSplitter) ) {
         bool bVertical = !IsDockedVertically(m_Side);
         ::SetCursor( bVertical ? s_hVertCursor : s_hHorizCursor);
         bHandled = FALSE;
      }
      return 0;
   }

   LRESULT OnButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      POINT ptPos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
      if( PtInSplitter(ptPos, m_Side, 0, m_rcSplitter) ) {
         // Click on the spiltter? Determine how har we can drag the pane and
         // start tracking...
         ::ClientToScreen(m_hWnd,& ptPos);
         m_ptStartDragPoint = m_ptEndDragPoint = m_ptDeltaDragPoint = ptPos;
         //
         RECT rcWin;
         GetWindowRect(&rcWin);
         m_rcTracker = m_rcSplitter;
         ::OffsetRect(&m_rcTracker, rcWin.left, rcWin.top);
         //
         ::GetWindowRect(GetParent(),& m_rcTrackerBounds);
         RECT rcLimit;
         ::SendMessage(GetParent(), WM_DOCK_QUERYRECT, DOCK_INFO_CHILD, (LPARAM)&rcLimit);
         switch( m_Side ) {
         case DOCK_LEFT:   m_rcTrackerBounds.right = rcLimit.right; break;
         case DOCK_RIGHT:  m_rcTrackerBounds.left = rcLimit.left; break;
         case DOCK_TOP:    m_rcTrackerBounds.bottom = rcLimit.bottom; break;
         case DOCK_BOTTOM: m_rcTrackerBounds.top = rcLimit.top; break;
         }
         // The actual dragging area is slightly smaller than the client area
         ::InflateRect(&m_rcTrackerBounds, -MIN_DOCKPANE_SIZE, -MIN_DOCKPANE_SIZE);
         // Enter tracking loop
         bool res = Track(false);
         if( res ) {
            int nOffset = ( !IsDockedVertically(m_Side) ? (m_ptEndDragPoint.y - m_ptStartDragPoint.y) : (m_ptEndDragPoint.x - m_ptStartDragPoint.x) );
            if( m_Side == DOCK_RIGHT || m_Side == DOCK_BOTTOM ) nOffset = -nOffset;
            m_cy += nOffset;
            ::SendMessage(GetParent(), WM_DOCK_UPDATELAYOUT, 0,0);
         }
      }
      return 0;
   }

   // Track methods

   void OnStretch(POINT& pt)
   {
      DrawGhostBar();
      if( !IsDockedVertically(m_Side) ) {
         int nOffset = pt.y - m_ptDeltaDragPoint.y;
         if( m_rcTracker.top + nOffset <= m_rcTrackerBounds.top ) nOffset = m_rcTrackerBounds.top - m_rcTracker.top;
         if( m_rcTracker.bottom + nOffset >= m_rcTrackerBounds.bottom ) nOffset = m_rcTrackerBounds.bottom - m_rcTracker.bottom;
         ::OffsetRect(&m_rcTracker, 0,nOffset);
         m_ptDeltaDragPoint.y += nOffset;
      }
      else {
         int nOffset = pt.x - m_ptDeltaDragPoint.x;
         if( m_rcTracker.left + nOffset <= m_rcTrackerBounds.left ) nOffset = m_rcTrackerBounds.left - m_rcTracker.left;
         if( m_rcTracker.right + nOffset >= m_rcTrackerBounds.right ) nOffset = m_rcTrackerBounds.right - m_rcTracker.right;
         ::OffsetRect(&m_rcTracker, nOffset,0);
         m_ptDeltaDragPoint.x += nOffset;
      }
      DrawGhostBar();
   }

   void OnEndResize()
   {
      m_ptEndDragPoint = m_ptDeltaDragPoint;
   }

   // Overridables

   void UpdateLayout()
   {
      if( m_map.GetSize() == 0 ) return;
      if( (GetStyle() & WS_VISIBLE) == 0 ) return;

      int nPanes = m_map.GetSize();
      bool bVertical = IsDockedVertically(m_Side);

      // Place side splitter for this docking area
      RECT rect; 
      GetClientRect(&rect);
      switch( m_Side ) {
      case DOCK_LEFT:
         ::SetRect(&m_rcSplitter, rect.right - m_cxySplitter, rect.top, rect.right, rect.bottom);
         rect.right -= m_cxySplitter;
         break;
      case DOCK_TOP:
         ::SetRect(&m_rcSplitter, rect.left, rect.bottom - m_cxySplitter, rect.right, rect.bottom);
         rect.bottom -= m_cxySplitter;
         break;
      case DOCK_RIGHT:
         ::SetRect(&m_rcSplitter, rect.left, rect.top, rect.left + m_cxySplitter, rect.bottom);
         rect.left += m_cxySplitter;
         break;
      case DOCK_BOTTOM:
         ::SetRect(&m_rcSplitter, rect.left, rect.top, rect.right, rect.top + m_cxySplitter);
         rect.top += m_cxySplitter;
         break;
      }

      // Place splitters in each child panel (except in the last one)
      for( int i = 0; i < nPanes - 1; i++ ) {
         ::SendMessage(m_map[i]->hwndDocked, WM_DOCK_SETSPLITTER, DEFAULT_SPLITTER_SIZE, 0L);
      }
      // The last panel does not have a splitter
      ::SendMessage(m_map[i]->hwndDocked, WM_DOCK_SETSPLITTER, 0, 0L);

      // Get actual height of all panels
      int nActualHeight = 0;
      for( i = 0; i < nPanes; i++ ) {
         const RECT& rc = m_map[i]->rcWindow;
         int iPaneHeight = (bVertical ? rc.bottom - rc.top : rc.right - rc.left);
         if( iPaneHeight < 10 ) iPaneHeight = 30;
         nActualHeight += iPaneHeight;
      }
      // Get height of docking area
      int nTop, nHeight;
      if( bVertical ) {
         nTop = rect.top;
         nHeight = rect.bottom - rect.top;
      }
      else {
         nTop = rect.left;
         nHeight = rect.right - rect.left;
      }
      // Distribute the difference among panels
      int nDelta = ((nHeight - nActualHeight) / nPanes);
      for( i = 0; i < nPanes; i++ ) {
         const RECT& rc = m_map[i]->rcWindow;
         int nSize = (bVertical ? rc.bottom - rc.top : rc.right - rc.left);
         if( !m_map[i]->bKeepSize ) nSize += nDelta;
         if( nSize < MIN_DOCKPANE_SIZE ) nSize = MIN_DOCKPANE_SIZE;
         if( nTop + nSize >= nHeight ) nSize /= 2;
         if( bVertical ) {
            ::SetRect(&m_map[i]->rcWindow, rect.left, nTop, rect.right, nTop + nSize);
         }
         else {
            ::SetRect(&m_map[i]->rcWindow, nTop, rect.top, nTop + nSize, rect.bottom);
         }
         nTop += nSize;
         m_map[i]->bKeepSize = false;
      }
      // Stretch the last window to the size of the docking window
      (bVertical ? m_map[nPanes - 1]->rcWindow.bottom : m_map[nPanes - 1]->rcWindow.right ) = nHeight;
   }
};

class CDockingPaneWindow : public CDockingPaneWindowImpl<CDockingPaneWindow>
{
public:
   DECLARE_WND_CLASS_EX(_T("WTL_DockingPaneWindow"), CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, COLOR_WINDOW)
};


///////////////////////////////////////////////////////
// CDockingWindow

template< class T, 
          class TPaneWindow = CDockingPaneWindow,
          class TDockWindow = CDockingPaneChildWindow,
          class TFloatWindow = CFloatingWindow,
          class TBase = CWindow, 
          class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CDockingWindowImpl : 
   public CWindowImpl< T, TBase, TWinTraits >
{
public:
   DECLARE_WND_CLASS_EX(NULL, CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, NULL)

   typedef CDockingWindowImpl< T , TBase, TWinTraits > thisClass;
   
   BEGIN_MSG_MAP(CDockingWindowImpl)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_DOCK_QUERYRECT, OnQueryRect)
      MESSAGE_HANDLER(WM_DOCK_QUERYTRACK, OnQueryTrack)
      MESSAGE_HANDLER(WM_DOCK_DOCK, OnDock)
      MESSAGE_HANDLER(WM_DOCK_FLOAT, OnFloat)
      MESSAGE_HANDLER(WM_DOCK_UNDOCK, OnUnDock)
      MESSAGE_HANDLER(WM_DOCK_UNFLOAT, OnUnFloat)
      MESSAGE_HANDLER(WM_DOCK_UPDATELAYOUT, OnSize)
      MESSAGE_HANDLER(WM_DOCK_REPOSITIONWINDOW, OnRepositionWindow)
      MESSAGE_HANDLER(WM_DOCK_CLIENT_CLOSE, OnClientClose)
   END_MSG_MAP()

   TPaneWindow m_panes[4];  // The 4 panel windows (one for each side)
   CDockMap m_map;          // The master map of dockable windows
   HWND m_hwndClient;       // The client window contained in the center
   DWORD m_dwExtStyle;      // Optional styles
   SIZE m_sizeBorder;       // Size of window borders

   // Operations

   void SetClient(HWND hWnd)
   {
      ATLASSERT(::IsWindow(hWnd));
      ATLASSERT(::GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD);
      m_hwndClient = hWnd;
   }

   DWORD GetExtendedDockStyle() const
   {
      return m_dwExtStyle;
   }

   void SetExtendedDockStyle(DWORD dwStyle)
   {
      ATLASSERT(::IsWindow(m_hWnd));
      m_dwExtStyle = dwStyle;
      // Duplicate style settings to panels
      for( short i = 0; i < 4; i++ ) m_panes[i].m_dwExtStyle = dwStyle;
   }

   BOOL IsDockPane(HWND hWnd) const
   {
      ATLASSERT(::IsWindow(hWnd));
      for( int i = 0; i < m_map.GetSize(); i++ ) if( m_map[i]->hwndChild == hWnd ) return TRUE;      
      return FALSE;
   }

   void AddWindow(HWND hWnd, DWORD dwFlags = 0)
   {
      ATLASSERT(::IsWindow(hWnd));

      // Create docking context
      DOCKCONTEXT* ctx;
      ATLTRY(ctx = new DOCKCONTEXT);
      if( ctx == NULL ) return;
      ctx->Side = DOCK_HIDDEN;
      ctx->LastSide = DOCK_LEFT;
      ctx->hwndChild = hWnd;
      ctx->hwndRoot = m_hWnd;
      ::SetRect(&ctx->rcWindow, 0, 0, MIN_DOCKPANE_SIZE, MIN_DOCKPANE_SIZE);
      ctx->sizeFloat.cx = ctx->sizeFloat.cy = DEFAULT_FLOAT_SIZE;
      ctx->dwFlags = dwFlags;
      ctx->bKeepSize = false;
      ctx->hwndOrigPrnt = ::GetParent(hWnd);

      // Create docking child
      TDockWindow* wndDock;
      ATLTRY(wndDock = new TDockWindow(ctx));
      if( wndDock == NULL ) return;
      wndDock->Create(m_hWnd, rcDefault, NULL);
      ATLASSERT(::IsWindow(wndDock->m_hWnd));
      ctx->hwndDocked = *wndDock;

      // Create floating child
      TFloatWindow* wndFloat;
      TCHAR szCaption[128];    // max text length is 127 for floating caption
      ::GetWindowText(hWnd, szCaption, sizeof(szCaption)/sizeof(TCHAR));
      ATLTRY(wndFloat = new TFloatWindow(ctx));
      if( wndFloat == NULL ) return;
      wndFloat->Create(m_hWnd, rcDefault, szCaption);
      ATLASSERT(::IsWindow(wndFloat->m_hWnd));
      ctx->hwndFloated = *wndFloat;

      ::SetParent(ctx->hwndChild, ctx->hwndDocked);

      // Add Context to master list
      m_map.Add(ctx);
   }

   BOOL RemoveWindow(HWND hWnd)
   {
      ATLASSERT(::IsWindow(hWnd));

      DOCKCONTEXT* pCtx = _GetContext(hWnd);
      ATLASSERT(pCtx);
      if( pCtx == NULL ) return FALSE;

      // Hide and destrow panel
      HideWindow(hWnd);

      // Remove context from map in case of repaint
      m_map.Remove(pCtx);

      ATLASSERT(::IsWindow(pCtx->hwndOrigPrnt));
      ::SetParent( pCtx->hwndChild, pCtx->hwndOrigPrnt );
      if( ::IsWindow(pCtx->hwndDocked )) ::DestroyWindow(pCtx->hwndDocked);
      if( ::IsWindow(pCtx->hwndFloated )) ::DestroyWindow(pCtx->hwndFloated);

      // Destroy context. Allocated in AddWindow()...
      delete pCtx;

      return TRUE;
   }

   BOOL DockWindow(HWND hWnd, short Side, int iRequestedSize = 0)
   {
      T* pT = static_cast<T*>(this);
      DOCKCONTEXT* pCtx = _GetContext(hWnd);
      ATLASSERT(pCtx);
      if( pCtx == NULL ) return FALSE;
      ::ShowWindow(pCtx->hwndChild, SW_SHOWNOACTIVATE);
      if( Side == DOCK_LASTKNOWN ) Side = pCtx->LastSide;
      if( !IsDocked(Side) ) return FALSE;
      return pT->_DockWindow(pCtx, Side, iRequestedSize);
   }

   BOOL FloatWindow(HWND hWnd, const RECT& rc)
   {
      T* pT = static_cast<T*>(this);
      DOCKCONTEXT* pCtx = _GetContext(hWnd);
      ATLASSERT(pCtx);
      if( pCtx == NULL ) return FALSE;
      ::ShowWindow(pCtx->hwndChild, SW_SHOWNOACTIVATE);
      pCtx->rcWindow = rc;
      return pT->_FloatWindow(pCtx);
   }

   BOOL HideWindow(HWND hWnd)
   {
      T* pT = static_cast<T*>(this);
      DOCKCONTEXT* pCtx = _GetContext(hWnd);
      ATLASSERT(pCtx);
      if( pCtx == NULL ) return FALSE;
      ::ShowWindow(pCtx->hwndChild, SW_HIDE);
      if( pCtx->Side == DOCK_FLOAT ) return pT->_UnFloatWindow(pCtx);
      else if( IsDocked(pCtx->Side) ) return pT->_UnDockWindow(pCtx);
      return TRUE;
   }

   void GetWindowState(HWND hWnd, int& DockState, RECT& rect) const
   {
      const DOCKCONTEXT* pCtx = _GetContext(hWnd);
      ATLASSERT(pCtx);
      if( pCtx == NULL ) return;
      DockState = pCtx->Side;
      ::GetWindowRect(::GetParent(hWnd), &rect);
   }

   int GetPaneSize(short Side) const
   {
      ATLASSERT(IsDocked(Side));
      int cy = m_panes[Side].m_cy;
      if( cy == 0 ) cy = m_panes[Side].m_cyOld;
      return cy;
   }

   void SetPaneSize(short Side, int iSize)
   {
      ATLASSERT(IsDocked(Side));
      if( iSize == 0 ) iSize = DEFAULT_DOCKPANE_SIZE;
      if( iSize < MIN_DOCKPANE_SIZE ) iSize = MIN_DOCKPANE_SIZE;
      m_panes[Side].m_cy = ( m_panes[Side].m_map.GetSize() == 0 ? 0 : iSize );
      m_panes[Side].m_cyOld = iSize;
      T* pT = static_cast<T*>(this);
      pT->UpdateLayout();
   }

   void EnableModeless(BOOL bEnable)
   {
      for( int i = 0; i < m_map.GetSize(); i++ ) {
         if( ::IsWindow(m_map[i]->hwndFloated) ) ::EnableWindow(m_map[i]->hwndFloated, bEnable);
      }
   }

   // Call this method from an message idle-handler (such as the
   // WTL CIdleHandler::OnIdle() handler). It makes sure to
   // keep the docked panes' caption updated with respect to current
   // keyboard focus.
   virtual BOOL OnIdle()
   {
      static HWND s_hwndLastActive = NULL;
      HWND hwndFocus = ::GetFocus();
      HWND hwndCurrent = NULL;
      for( int i = 0; i < m_map.GetSize(); i++ ) {
         const DOCKCONTEXT* pCtx = m_map[i];
         if( IsDocked(pCtx->Side) && ::IsChild(pCtx->hwndDocked, hwndFocus) ) hwndCurrent = pCtx->hwndDocked;
      }
      if( s_hwndLastActive != hwndCurrent ) {
         if( ::IsWindow(s_hwndLastActive) ) ::InvalidateRect(s_hwndLastActive, NULL, TRUE);
         if( ::IsWindow(hwndCurrent) ) ::InvalidateRect(hwndCurrent, NULL, TRUE);
         s_hwndLastActive = hwndCurrent;
      }
      return TRUE;
   }

   // Message handlers

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      for( short i = 0; i < 4; i++ ) {
         m_panes[i].m_Side = i;
         m_panes[i].Create(m_hWnd, rcDefault, NULL, WS_CHILD|WS_VISIBLE);
      }
      m_sizeBorder.cx = ::GetSystemMetrics(SM_CXEDGE);
      m_sizeBorder.cy = ::GetSystemMetrics(SM_CYEDGE);
      return 0;
   }

   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      for( int i = 0; i < m_map.GetSize(); i++ ) {
         if( ::IsWindow(m_map[i]->hwndDocked) ) ::DestroyWindow(m_map[i]->hwndDocked);
         if( ::IsWindow(m_map[i]->hwndFloated) ) ::DestroyWindow(m_map[i]->hwndFloated);
         delete m_map[i];
      }
      m_map.RemoveAll();
      return 0;
   }

   LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      return 1; // handled, no background painting needed
   }

   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);
      pT->UpdateLayout();
      return 0;
   }

   // Custom defined messages

   LRESULT OnQueryRect(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      // wParam: Side
      // lParam: LPRECT
      LPRECT prc = (LPRECT) lParam;
      ATLASSERT(prc);
      switch( (short) wParam ) {
      case DOCK_LEFT:
      case DOCK_TOP:
      case DOCK_RIGHT:
      case DOCK_BOTTOM:
         ::GetWindowRect(m_panes[wParam], prc);
         break;
      case DOCK_INFO_CHILD:
         ::GetWindowRect(m_hwndClient, prc);
         break;
      default:
         ATLASSERT(false);
      }
      return 0;
   }

   LRESULT OnQueryTrack(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      // lParam: TRACKINFO*
      TRACKINFO* pTI = (TRACKINFO*) lParam;
      ATLASSERT(pTI);
      POINT& pt = pTI->ptPos;
      RECT rc;

      if( pTI->pCtx->dwFlags & DCK_NOFLOAT ) {
         // If we're not allowed to float, we're still docked to where we came from
         ::GetWindowRect(pTI->hWnd, &rc);
         pTI->rc = rc;
         ::OffsetRect(&pTI->rc, pt.x - pTI->ptStart.x, pt.y - pTI->ptStart.y);
         pTI->Side = pTI->pCtx->Side;
      }
      else {
         // But by default, we're floating in the skies
         ::GetWindowRect(pTI->hWnd, &rc);
         ::SetRect(&pTI->rc, rc.left, rc.top, rc.left + pTI->pCtx->sizeFloat.cx, rc.top + pTI->pCtx->sizeFloat.cy);
         ::OffsetRect(&pTI->rc, pt.x - pTI->ptStart.x, pt.y - pTI->ptStart.y);
         pTI->Side = DOCK_FLOAT;
      }
      // Pressing CTRL key gives default floating
      if( ::GetKeyState(VK_CONTROL) < 0 ) return 0;
      // Are we perhaps hovering over the tracked window?
      ::GetWindowRect(pTI->hWnd, &rc);
      if( ::PtInRect(&rc, pt) ) {
         pTI->rc = rc;
         ::OffsetRect(&pTI->rc, pt.x - pTI->ptStart.x, pt.y - pTI->ptStart.y);
         pTI->Side = pTI->pCtx->Side;
         return 0;
      }
      // Or is the point inside one of the other docking areas?
      for( short i = 0; i < 4; i++ ) {
         if( pTI->pCtx->dwFlags & (1<<i) ) continue; // DCK_NOxxx flag?
         if( m_panes[i].m_cy == 0 ) {
            // Simulate docking areas that are currently invisible
            ::GetWindowRect(m_hWnd, &rc);
            switch( m_panes[i].m_Side ) {
            case DOCK_LEFT:   rc.right = rc.left + DEFAULT_DOCKPANE_SIZE; break;
            case DOCK_RIGHT:  rc.left = rc.right - DEFAULT_DOCKPANE_SIZE; break;
            case DOCK_TOP:    rc.bottom = rc.top + DEFAULT_DOCKPANE_SIZE; break;
            case DOCK_BOTTOM: rc.top = rc.bottom - DEFAULT_DOCKPANE_SIZE; break;
            }
         }
         else {
            ::GetWindowRect(m_panes[i], &rc);
         }
         if( ::PtInRect(&rc, pt) ) {
            pTI->Side = i;
            pTI->rc = rc;
            return 0;
         }
      }
      return 0;
   }

   LRESULT OnDock(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);
      pT->_DockWindow((DOCKCONTEXT*) lParam, (short) wParam,0);
      return 0;
   }

   LRESULT OnUnDock(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);
      pT->_UnDockWindow((DOCKCONTEXT*) lParam);
      return 0;
   }

   LRESULT OnFloat(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);
      pT->_FloatWindow((DOCKCONTEXT*) lParam);
      return 0;
   }

   LRESULT OnUnFloat(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);
      pT->_UnFloatWindow((DOCKCONTEXT*) lParam);
      return 0;
   }

   LRESULT OnClientClose(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      DOCKCONTEXT* pCtx = (DOCKCONTEXT*) lParam;
      // Make sure to bring focus back to this app!
      ::SetForegroundWindow(m_hWnd);
      SetFocus();
      // Hide or destroy view
      if( m_dwExtStyle & DCK_EX_DESTROYONCLOSE ) {
         HWND hWnd = pCtx->hwndChild;
         // Commit suicide
         RemoveWindow(hWnd);
         ::DestroyWindow(hWnd);
      }
      else {
         // Send the actual dock/float message instead
         SendMessage((UINT) wParam, 0, (LPARAM) pCtx);
         // HACK: Send notification to child (note the funky message type)
         ::SendMessage(pCtx->hwndChild, WM_COMPACTING, 0, 0L);
      }
      return 0;
   }

   LRESULT OnRepositionWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);
      pT->_RepositionWindow((DOCKCONTEXT*) lParam, (int) wParam);
      return 0;
   }

   // Overridables

   void UpdateLayout()
   {
      RECT rect;   
      GetClientRect(&rect);
      RECT rcClient = rect;

      if( m_panes[DOCK_TOP].m_cy > 0 ) {
         ::SetWindowPos(m_panes[DOCK_TOP], NULL, rect.left, rect.top, rect.right - rect.left, m_panes[DOCK_TOP].m_cy, SWP_NOZORDER | SWP_NOACTIVATE);
         rcClient.top += m_panes[DOCK_TOP].m_cy;
      }
      if( m_panes[DOCK_LEFT].m_cy > 0 ) {
         ::SetWindowPos(m_panes[DOCK_LEFT], NULL, rect.left, rcClient.top, m_panes[DOCK_LEFT].m_cy, rect.bottom - rcClient.top, SWP_NOZORDER | SWP_NOACTIVATE);
         rcClient.left += m_panes[DOCK_LEFT].m_cy;
      }
      if( m_panes[DOCK_RIGHT].m_cy > 0 ) {
         int left = max( (int) rect.right - m_panes[DOCK_RIGHT].m_cy, (int) rcClient.left );
         ::SetWindowPos(m_panes[DOCK_RIGHT], NULL, left, rcClient.top, m_panes[DOCK_RIGHT].m_cy, rect.bottom - rcClient.top, SWP_NOZORDER | SWP_NOACTIVATE);
         rcClient.right -= m_panes[DOCK_RIGHT].m_cy;
      }
      if( m_panes[DOCK_BOTTOM].m_cy > 0 ) {
         int top = max( (int) rcClient.bottom - m_panes[DOCK_BOTTOM].m_cy, (int) rcClient.top );
         ::SetWindowPos(m_panes[DOCK_BOTTOM], NULL, rcClient.left, top, rcClient.right - rcClient.left, m_panes[DOCK_BOTTOM].m_cy, SWP_NOZORDER | SWP_NOACTIVATE);
         rcClient.bottom -= m_panes[DOCK_BOTTOM].m_cy;
      }

      if( ::IsWindow(m_hwndClient) ) {
         // Adjust borders around docking panes
         DWORD dwExtStyle = (DWORD)::GetWindowLong(m_hwndClient, GWL_EXSTYLE);
         bool bClientEdge = ((dwExtStyle & WS_EX_CLIENTEDGE) != 0);
         if( bClientEdge ) {
            if( m_panes[DOCK_TOP].m_cy > 0 ) rcClient.top -= m_sizeBorder.cy;
            if( m_panes[DOCK_LEFT].m_cy > 0 ) rcClient.left -= m_sizeBorder.cx;
            if( m_panes[DOCK_RIGHT].m_cy > 0 ) rcClient.right += m_sizeBorder.cx;
            if( m_panes[DOCK_BOTTOM].m_cy > 0 ) rcClient.bottom += m_sizeBorder.cy;
         }
         // Map client rectangle to windows's coord system
         ::MapWindowPoints(m_hWnd, ::GetParent(m_hwndClient), (LPPOINT)&rcClient, sizeof(rcClient)/sizeof(POINT));
         ::SetWindowPos(m_hwndClient, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, SWP_NOZORDER);
      }
   }

   BOOL _DockWindow(DOCKCONTEXT* ctx, short Side, int iRequestedSize)
   {
      ATLASSERT(ctx);
      ATLASSERT(IsDocked(Side));
      ATLASSERT(ctx->Side==DOCK_HIDDEN); // Undock before redock
      if( !IsDocked(Side) ) return FALSE;
      if( ctx->Side != DOCK_HIDDEN ) return FALSE;
      bool bVertical = IsDockedVertically(Side);
      // Make up a new panel size
      if( iRequestedSize <= 0 ) {
         RECT rc;
         ::GetClientRect(m_panes[Side], &rc);
         iRequestedSize = bVertical ? rc.bottom - rc.top : rc.right - rc.left;
         if( m_panes[Side].m_map.GetSize() > 0 ) iRequestedSize /= m_panes[Side].m_map.GetSize();
      }
      if( iRequestedSize < 10 ) iRequestedSize = 0;
      // Set the new size of the pane (subject to readjustment)
      ::SetRectEmpty(&ctx->rcWindow);
      (bVertical ? ctx->rcWindow.bottom : ctx->rcWindow.right) = iRequestedSize;
      // Signal that we would like to retain this size after
      // first being laid out...
      ctx->bKeepSize = iRequestedSize > 0;
      // Dock
      m_panes[Side].DockWindow(ctx);
      PostMessage(WM_DOCK_UPDATELAYOUT);
      return TRUE;
   }

   BOOL _FloatWindow(DOCKCONTEXT* ctx)
   {
      ATLASSERT(ctx);
      ATLASSERT(ctx->Side==DOCK_HIDDEN); // Undock before float
      if( ctx->Side != DOCK_HIDDEN ) return FALSE;
      ctx->Side = DOCK_FLOAT;
      ::SetParent(ctx->hwndChild, ctx->hwndFloated);
      ::SetWindowPos(ctx->hwndFloated, NULL, ctx->rcWindow.left, ctx->rcWindow.top, ctx->rcWindow.right - ctx->rcWindow.left, ctx->rcWindow.bottom - ctx->rcWindow.top, SWP_NOZORDER);
      ::SendMessage(ctx->hwndFloated, WM_DOCK_UPDATELAYOUT, 0,0);
      ::ShowWindow(ctx->hwndFloated, SW_SHOWNORMAL);
      return TRUE;
   }

   BOOL _UnDockWindow(DOCKCONTEXT* ctx)
   {
      ATLASSERT(ctx);
      ATLASSERT(IsDocked(ctx->Side));
      if( !IsDocked(ctx->Side) ) return FALSE;
      m_panes[ctx->Side].UnDockWindow(ctx);
      ctx->Side = DOCK_HIDDEN;
      SendMessage(WM_DOCK_UPDATELAYOUT);
      return TRUE;
   }

   BOOL _UnFloatWindow(DOCKCONTEXT* ctx)
   {
      ATLASSERT(ctx);
      ATLASSERT(ctx->Side==DOCK_FLOAT);
      if( ctx->Side != DOCK_FLOAT ) return FALSE;
      ::ShowWindow(ctx->hwndFloated, SW_HIDE);
      ctx->Side = DOCK_HIDDEN;
      return TRUE;
   }

   BOOL _RepositionWindow(DOCKCONTEXT* ctx, int iPos)
   {
      ATLASSERT(ctx);
      ATLASSERT(IsDocked(ctx->Side));
      if( !IsDocked(ctx->Side) ) return FALSE;
      m_panes[ctx->Side].RepositionWindow(ctx,iPos);
      return TRUE;
   }

   DOCKCONTEXT* _GetContext(HWND hWnd) const
   {
      ATLASSERT(::IsWindow(hWnd));
      for( int i = 0; i < m_map.GetSize(); i++ ) if( m_map[i]->hwndChild == hWnd ) return m_map[i];
      ATLASSERT(!"Docking Window not found; use AddWindow() to add it");
      return NULL;
   }
};

class CDockingWindow : public CDockingWindowImpl<CDockingWindow>
{
public:
   DECLARE_WND_CLASS_EX(_T("WTL_DockingWindow"), CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, NULL)
};


#endif // __ATL_DOCK_H__
