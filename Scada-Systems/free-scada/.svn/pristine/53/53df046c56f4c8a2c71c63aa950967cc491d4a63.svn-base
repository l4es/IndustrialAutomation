#ifndef __ATLGDIX_H__
#define __ATLGDIX_H__

/////////////////////////////////////////////////////////////////////////////
// Additional GDI/USER wrappers
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2001-2002 Bjarke Viksoe.
// Thanks to Daniel Bowen for COffscreenDrawRect.
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

#ifndef __ATLGDI_H__
   #error atlgdix.h requires atlgdi.h to be included first
#endif

namespace WTL
{

/////////////////////////////////////////////////////////////////////////////
// Macros

// The GetXValue macros below are badly designed and emit
// compiler warnings e.g. when using RGB(255,255,255)...
#pragma warning(disable : 4310)

#ifndef BlendRGB
   #define BlendRGB(c1, c2, factor) \
      RGB( GetRValue(c1) + ((GetRValue(c2) - GetRValue(c1)) * factor / 100L), \
           GetGValue(c1) + ((GetGValue(c2) - GetGValue(c1)) * factor / 100L), \
           GetBValue(c1) + ((GetBValue(c2) - GetBValue(c1)) * factor / 100L) )
#endif

#ifndef COLOR_INVALID
   #define COLOR_INVALID  (COLORREF) CLR_INVALID
#endif

/////////////////////////////////////////////////////////////////////////////
// CMemDC

class CMemDC : public CDC
{
public:
   CDCHandle     m_dc;          // Owner DC
   CBitmap       m_bitmap;      // Offscreen bitmap
   CBitmapHandle m_hOldBitmap;  // Originally selected bitmap
   RECT          m_rc;          // Rectangle of drawing area

   CMemDC(HDC hDC, LPRECT pRect = NULL)
   {
      ATLASSERT(hDC!=NULL);
      m_dc = hDC;
      if( pRect != NULL ) m_rc = *pRect; else m_dc.GetClipBox(&m_rc);

      CreateCompatibleDC(m_dc);
      ::LPtoDP(m_dc, (LPPOINT) &m_rc, sizeof(RECT) / sizeof(POINT));
      m_bitmap.CreateCompatibleBitmap(m_dc, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top);
      m_hOldBitmap = SelectBitmap(m_bitmap);
      ::DPtoLP(m_dc, (LPPOINT) &m_rc, sizeof(RECT) / sizeof(POINT));
      SetWindowOrg(m_rc.left, m_rc.top);
   }
   ~CMemDC()
   {
      // Copy the offscreen bitmap onto the screen.
      m_dc.BitBlt(m_rc.left, m_rc.top, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top,
                  m_hDC, m_rc.left, m_rc.top, SRCCOPY);
      // Swap back the original bitmap.
      SelectBitmap(m_hOldBitmap);
   }
};


/////////////////////////////////////////////////////////////////////////////
// COffscreenDraw

// To use it, derive from it and chain it in the message map.
template< class T >
class COffscreenDraw
{
public:
   BEGIN_MSG_MAP(COffscreenDraw)
      MESSAGE_HANDLER(WM_PAINT, OnPaint)
      MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
      MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
   END_MSG_MAP()

   LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);
      if( wParam != NULL )
      {
         CMemDC memdc( (HDC) wParam, NULL );
         pT->DoPaint(memdc.m_hDC);
      }
      else
      {
         RECT rc;
         pT->GetClientRect(&rc);
         CPaintDC dc(pT->m_hWnd);
         CMemDC memdc(dc.m_hDC, &rc);
         pT->DoPaint(memdc.m_hDC);
      }
      return 0;
   }
   LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      return 1; // handled; no need to erase background; do it in DoPaint();
   }
   void DoPaint(CDCHandle dc)
   {
      ATLASSERT(false); // must override this
   }
};

// To use it, derive from it and chain it in the message map.
template< class T >
class COffscreenDrawRect
{
public:
   BEGIN_MSG_MAP(COffscreenDrawRect)
      MESSAGE_HANDLER(WM_PAINT, OnPaint)
      MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
      MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
   END_MSG_MAP()

   LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);
      if( wParam != NULL )
      {
         CMemDC memdc( (HDC) wParam, NULL );
         pT->DoPaint(memdc.m_hDC, memdc.m_rc);
      }
      else
      {
         CPaintDC dc(pT->m_hWnd);
         CMemDC memdc(dc.m_hDC, &dc.m_ps.rcPaint);
         pT->DoPaint(memdc.m_hDC, dc.m_ps.rcPaint);
      }
      return 0;
   }
   LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      return 1; // handled; no need to erase background; do it in DoPaint();
   }
   void DoPaint(CDCHandle dc, RECT& rcClip)
   {
      ATLASSERT(false); // must override this
   }
};


/////////////////////////////////////////////////////////////////////////////
// CSaveDC

class CSaveDC
{
public:
   HDC m_hDC;
   int m_iState;

   CSaveDC(HDC hDC) : m_hDC(hDC)
   {
      ATLASSERT(::GetObjectType(m_hDC)==OBJ_DC || ::GetObjectType(m_hDC)==OBJ_MEMDC);
      m_iState = ::SaveDC(hDC);
      ATLASSERT(m_iState!=0);
   }
   ~CSaveDC()
   {
      Restore();
   }
   void Restore()
   {
      if( m_iState == 0 ) return;
      ATLASSERT(::GetObjectType(m_hDC)==OBJ_DC || ::GetObjectType(m_hDC)==OBJ_MEMDC);
      ::RestoreDC(m_hDC, m_iState);
      m_iState = 0;
   }
};


/////////////////////////////////////////////////////////////////////////////
// CHandle

#if (_ATL_VER < 0x0700)

class CHandle
{
public:
   HANDLE m_h;

   CHandle(HANDLE hSrc = INVALID_HANDLE_VALUE) : m_h(hSrc)
   { }

   ~CHandle()
   {
      Close();
   }

   operator HANDLE() const { return m_h; };
  
   LPHANDLE operator&()
   {
      ATLASSERT(!IsValid());
      return &m_h;
   }

   CHandle& operator=(HANDLE h)
   {
      ATLASSERT(!IsValid());
      m_h = h;
      return *this;
   }

   bool IsValid() const { return m_h != INVALID_HANDLE_VALUE; };
   
   void Attach(HANDLE h)
   {
      if( IsValid() ) ::CloseHandle(m_h);
      m_h = h;
   }   
   HANDLE Detach()
   {
      HANDLE h = m_h;
      m_h = INVALID_HANDLE_VALUE;
      return h;
   }
   
   BOOL Close()
   {
      BOOL bRes = FALSE;
      if( m_h != INVALID_HANDLE_VALUE ) {
         bRes = ::CloseHandle(m_h);
         m_h = INVALID_HANDLE_VALUE;
      }
      return bRes;
   }

   BOOL Duplicate(HANDLE hSource, bool bInherit = false)
   {
      ATLASSERT(!IsValid());
      HANDLE hOurProcess = ::GetCurrentProcess();
      BOOL b = ::DuplicateHandle(hOurProcess, 
         hSource,
         hOurProcess, 
         &m_h,
         DUPLICATE_SAME_ACCESS,
         bInherit,
         DUPLICATE_SAME_ACCESS);
      ATLASSERT(b);
      return b;
   }
};

#endif // _ATL_VER


/////////////////////////////////////////////////////////////////////////////
// Mouse Hover helper

#ifndef NOTRACKMOUSEEVENT

#ifndef WM_MOUSEENTER
   #define WM_MOUSEENTER WM_USER + 253
#endif // WM_MOUSEENTER

// To use it, derive from it and chain it in the message map.
// Make sure to set bHandled to FALSE when handling WM_MOUSEMOVE or
// the WM_MOUSELEAVE message!
template< class T >
class CMouseHover
{
public:   
   bool m_fMouseOver;          // Internal mouse-over state
   bool m_fMouseForceUpdate;   // Update window immediately on event

   CMouseHover() : 
      m_fMouseOver(false),
      m_fMouseForceUpdate(true)
   {
   }

   BEGIN_MSG_MAP(CMouseHover)
      MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
      MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
   END_MSG_MAP()

   LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   {
      T* pT = static_cast<T*>(this);
      if( !m_fMouseOver )   {
         m_fMouseOver = true;
         pT->SendMessage(WM_MOUSEENTER, wParam, lParam);
         pT->Invalidate();
         if( m_fMouseForceUpdate ) pT->UpdateWindow();
         _StartTrackMouseLeave(pT->m_hWnd);
      }
      bHandled = FALSE;
      return 0;
   }
   LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      T* pT = static_cast<T*>(this);
      if( m_fMouseOver ) {
         m_fMouseOver = false;
         pT->Invalidate();
         if( m_fMouseForceUpdate ) pT->UpdateWindow();
      }
      bHandled = FALSE;
      return 0;
   }
   BOOL _StartTrackMouseLeave(HWND hWnd) const
   {
      ATLASSERT(::IsWindow(hWnd));
      TRACKMOUSEEVENT tme = { 0 };
      tme.cbSize = sizeof(tme);
      tme.dwFlags = TME_LEAVE;
      tme.hwndTrack = hWnd;
      return _TrackMouseEvent(&tme);
   }
   BOOL _CancelTrackMouseLeave(HWND hWnd) const
   {
      TRACKMOUSEEVENT tme = { 0 };
      tme.cbSize = sizeof(tme);
      tme.dwFlags = TME_LEAVE | TME_CANCEL;
      tme.hwndTrack = hWnd;
      return _TrackMouseEvent(&tme);
   }
};

#endif // NOTRACKMOUSEEVENT


}; // namespace WTL

#endif // __ATLGDIX_H__
