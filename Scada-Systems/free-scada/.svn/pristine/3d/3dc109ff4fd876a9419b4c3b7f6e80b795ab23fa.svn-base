#ifndef __ATLCTRLXP2_H__
#define __ATLCTRLXP2_H__

/////////////////////////////////////////////////////////////////////////////
// Various VisualStudio.NET wanna-be controls
//
// Contains:
//   CMultiPaneStatusBarXPCtrl
//   CToolBarXPCtrl
//   CComboBoxXPCtrl
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2002 Bjarke Viksoe.
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
   #error WTL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLCTRLW_H__
   #error atlctrlxp2.h requires atlctrlw.h to be included first
#endif

#ifndef __ATLCTRLXP_H__
   #error atlctrlxp2.h requires atlctrlxp.h to be included first
#endif

#if (_WTL_VER < 0x0700)
   #error This file requires WTL version 7.0 or higher
#endif


/////////////////////////////////////////////////////////////////////////////
// CMultiPaneStatusBarXPCtrl - The Status Bar

#ifdef __ATLCTRLX_H__

class CMultiPaneStatusBarXPCtrl : public CMultiPaneStatusBarCtrlImpl<CMultiPaneStatusBarXPCtrl>
{
public:
   DECLARE_WND_SUPERCLASS(_T("WTL_MultiPaneStatusBarXP"), GetWndClassName())

   BEGIN_MSG_MAP(CPaneContainerImpl)
      MESSAGE_HANDLER(WM_PAINT, OnPaint)
      CHAIN_MSG_MAP( CMultiPaneStatusBarCtrlImpl<CMultiPaneStatusBarXPCtrl> )
   END_MSG_MAP()

   LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc();

      if( IsSimple() ) return lRes;

      CClientDC dc(m_hWnd);
      CPen pen;
      pen.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
      HPEN hOldPen = dc.SelectPen(pen);
      HBRUSH hOldBrush = dc.SelectStockBrush(NULL_BRUSH);
      int nCount = (int) GetParts(0, NULL);
      for( int i = 0; i < nCount; i++ ) {
         RECT rcItem;
         GetRect(i, &rcItem);
         dc.Rectangle(&rcItem); 
      }
      dc.SelectBrush(hOldBrush);
      dc.SelectPen(hOldPen);

      return lRes;
   }
};

#endif // __ATLCTRLX_H__


/////////////////////////////////////////////////////////////////////////////
// CToolBarXPCtrl - The ToolBar control

class CToolBarXPCtrl : 
   public CWindowImpl<CToolBarXPCtrl, CToolBarCtrl>,
   public CCustomDraw<CToolBarXPCtrl>
{
public:
   DECLARE_WND_SUPERCLASS(_T("WTL_ToolBarXP"), GetWndClassName())

   BEGIN_MSG_MAP(CToolBarXPCtrl)
      CHAIN_MSG_MAP_ALT(CCustomDraw<CToolBarXPCtrl>, 1)
      DEFAULT_REFLECTION_HANDLER()
   END_MSG_MAP()

   DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
   {
      CDCHandle dc(lpNMCustomDraw->hdc);
      dc.FillSolidRect(&lpNMCustomDraw->rc, CCommandBarXPCtrl::m_xpstyle.clrMenu);
      return CDRF_NOTIFYITEMDRAW;   // We need per-item notifications
   }
   DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
   {
      CDCHandle dc(lpNMCustomDraw->hdc);
      HFONT hOldFont = dc.SelectFont(GetFont());
      CCommandBarXPCtrl::_DrawToolbarButton( (LPNMTBCUSTOMDRAW) lpNMCustomDraw );
      dc.SelectFont(hOldFont);
      return CDRF_SKIPDEFAULT;
   }
};


/////////////////////////////////////////////////////////////////////////////
// CComboBoxXPCtrl - The ComboBox control

#ifdef __ATLGDIX_H__

template< class T, class TBase = CComboBox, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CComboBoxXPImpl : 
   public CWindowImpl< T, TBase, TWinTraits >,
   public CMouseHover< T >
{
public:
   DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

   CContainedWindowT<CEdit> m_ctrlEdit;
   bool m_fMouseOverEdit;
   COLORREF m_clrHighlight;
   COLORREF m_clrHighlightDark;
   COLORREF m_clrBorder;

   CComboBoxXPImpl() : 
      m_ctrlEdit(this, 1), 
      m_fMouseOverEdit(false)
   {
   }

   // Operations

   BOOL SubclassWindow(HWND hWnd)
   {
      ATLASSERT(m_hWnd==NULL);
      ATLASSERT(::IsWindow(hWnd));
#ifdef _DEBUG
      // Check class
      TCHAR szBuffer[16] = { 0 };
      if( ::GetClassName(hWnd, szBuffer, (sizeof(szBuffer)/sizeof(TCHAR))-1) ) {
         ATLASSERT(::lstrcmpi(szBuffer, TBase::GetWndClassName())==0);
      }
#endif
      BOOL bRet = CWindowImpl< T, TBase, TWinTraits >::SubclassWindow(hWnd);
      if( bRet ) _Init();
      return bRet;
   }

   // Implementation

   void _Init()
   {
      ATLASSERT(::IsWindow(m_hWnd));
      
      // Calculate XP colours
      CWindowDC dc(NULL);
      int nBitsPerPixel = dc.GetDeviceCaps(BITSPIXEL);
      m_clrBorder = ::GetSysColor(COLOR_HIGHLIGHT);
      if( nBitsPerPixel > 8 ) {
         m_clrHighlight = BlendRGB(m_clrBorder, ::GetSysColor(COLOR_WINDOW), 70);
         m_clrHighlightDark = BlendRGB(m_clrBorder, ::GetSysColor(COLOR_WINDOW), 60);
      }
      else {
         m_clrHighlight = ::GetSysColor(COLOR_HIGHLIGHT);
         m_clrHighlightDark = ::GetSysColor(COLOR_HIGHLIGHT);
      }

      // Subclass child (edit) control if present
      if( GetWindow(GW_CHILD) ) {
         m_ctrlEdit.SubclassWindow(GetWindow(GW_CHILD));
      }
   }

   // Message map and handlers

   BEGIN_MSG_MAP(CComboBoxXPImpl)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_PAINT, OnPaint)
      REFLECTED_COMMAND_CODE_HANDLER(CBN_CLOSEUP, OnFocusChange);
      REFLECTED_COMMAND_CODE_HANDLER(CBN_KILLFOCUS, OnFocusChange);
      REFLECTED_COMMAND_CODE_HANDLER(CBN_SETFOCUS, OnFocusChange);
      CHAIN_MSG_MAP( CMouseHover< T > )
      DEFAULT_REFLECTION_HANDLER()
   ALT_MSG_MAP(1)
      MESSAGE_HANDLER(WM_MOUSEMOVE, OnEditMouseMove)
      MESSAGE_HANDLER(WM_MOUSELEAVE, OnEditMouseLeave)
   END_MSG_MAP()

   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
      _Init();
      return lRes;
   }
   LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      CPaintDC dc(m_hWnd);

      RECT rc;
      GetClientRect(&rc);
      RECT rcButton = { rc.right - ::GetSystemMetrics(SM_CXHTHUMB), rc.top, rc.right, rc.bottom };
      ValidateRect(&rcButton);

      LRESULT lRes = DefWindowProc(WM_PRINTCLIENT, (WPARAM) (HDC) dc, PRF_CLIENT);

      COLORREF clrBorder = ::GetSysColor(COLOR_WINDOW);
      COLORREF clrBack = ::GetSysColor(COLOR_3DFACE);
      if( IsWindowEnabled() ) {
         if( m_fMouseOver || 
             m_fMouseOverEdit ||
             ::GetFocus() == m_hWnd || 
             ::GetParent(::GetFocus()) == m_hWnd )
         {
            clrBorder = m_clrBorder;
            clrBack = m_clrHighlight;
         }
         if( GetDroppedState() ) {
            clrBorder = m_clrBorder;
            clrBack = m_clrHighlightDark;
         }
      }

      // Draw the border
      CPen pen;
      pen.CreatePen(PS_SOLID, 1, clrBorder);
      HPEN hOldPen = dc.SelectPen(pen);
      HBRUSH hOldBrush = dc.SelectBrush(::GetSysColorBrush(COLOR_WINDOW));
      dc.Rectangle(&rc);

      // Paint the button
      CBrush brush;
      brush.CreateSolidBrush(clrBack);
      dc.SelectBrush(brush);
      dc.Rectangle(&rcButton);

      // Draw dropdown arrow
      // Need this because all the OEM bitmaps render grey backgrounds...
      RECT rcArrow = { rcButton.left + 5, rcButton.top + 8, rcButton.right - 5, rcButton.top + 11 };
      POINT points[3] = 
      {
         { rcArrow.left, rcArrow.top },
         { rcArrow.right, rcArrow.top },
         { rcArrow.left + ((rcArrow.right - rcArrow.left) / 2), rcArrow.bottom }
      };
      int iFillMode = dc.SetPolyFillMode(WINDING);
      dc.SelectStockPen(BLACK_PEN);
      dc.SelectStockBrush(BLACK_BRUSH);
      dc.Polygon(points, 3);
      dc.SetPolyFillMode(iFillMode);
      
      dc.SelectPen(hOldPen);
      dc.SelectBrush(hOldBrush);
      
      return lRes;
   }
   LRESULT OnFocusChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc();
      Invalidate();
      return lRes;
   }

   // Edit control

   LRESULT OnEditMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      if( !m_fMouseOverEdit )   {
         m_fMouseOverEdit = true;
         ::InvalidateRect(m_hWnd, NULL, FALSE);
         ::UpdateWindow(m_hWnd);
         // Let us know when the mouse leaves
         TRACKMOUSEEVENT tme = { 0 };
         tme.cbSize = sizeof(tme);
         tme.dwFlags = TME_LEAVE;
         tme.hwndTrack = m_ctrlEdit;
         _TrackMouseEvent(&tme);
      }
      bHandled = FALSE;
      return 0;
   }
   LRESULT OnEditMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      if( m_fMouseOverEdit ) {
         m_fMouseOverEdit = false;
         ::InvalidateRect(m_hWnd, NULL, FALSE);
         ::UpdateWindow(m_hWnd);
      }
      bHandled = FALSE;
      return 0;
   }
};

class CComboBoxXPCtrl : public CComboBoxXPImpl<CComboBoxXPCtrl>
{
public:
   DECLARE_WND_SUPERCLASS(_T("WTL_ComboBoxXP"), GetWndClassName())  
};


#endif // __ATLGDIX_H__


#endif // __ATLCTRLWXP2_H__

