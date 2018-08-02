#ifndef __COOLTABCTRLS_H__
#define __COOLTABCTRLS_H__

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CCoolTabCtrls - A set of Tab Controls with different appearances
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Several improvements done by Daniel Bowen.
// Copyright (c) 2001-2004 Bjarke Viksoe.
//
// Add the following macro to the parent's message map:
//   REFLECT_NOTIFICATIONS()
// CFolderTabCtrl code based on a Paul DiLascia MSJ 1999 article.
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
  #error WTL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
  #error CoolTabCtrls.h requires atlapp.h to be included first
#endif

#ifndef __ATLCTRLS_H__
  #error CoolTabCtrls.h requires atlctrls.h to be included first
#endif

#ifndef __ATLGDIX_H__
  #error CoolTabCtrls.h requires atlgdix.h to be included first
#endif

#if (_WIN32_IE < 0x0400)
  #error CoolTabCtrls.h requires _WIN32_IE >= 0x0400
#endif


// New tab notifications
#ifndef TCN_INITIALIZE
   #define TCN_INITIALIZE TCN_FIRST-10
   #define TCN_INSERTITEM TCN_FIRST-11
   #define TCN_DELETEITEM TCN_FIRST-12
#endif // TCN_INITIALIZE

// New masks
#define TCIF_TEXTCOLOR     0x1000 
#define TCIF_BKCOLOR       0x2000 
#define TCIF_TOOLTIP       0x4000
#define TCIF_WIDTH         0x8000

// New tab states
#define TCIS_DISABLED      0x1000
#define TCIS_HIDDEN        0x2000

// Extended tab styles
#define TCS_EX_SCROLLBUTTONS  0x00000010      // Displays scroll-buttons when needed
#define TCS_EX_FLATSCROLL     0x00000020      // Scroll-buttons are shown as flat
#define TCS_EX_CLOSEBUTTON    0x00000040      // Displays close-button to close tabs
#define TCS_EX_SELHIGHLIGHT   0x00000080      // Highlight selected tab with special font
#define TCS_EX_COMPRESSLINE   0x00000100      // Compress tabs if larger than client area

// Layout structure for tab
typedef struct
{
   int cxIndent;                              // Start indent
   int cxButtonSpacing;                       // Gap between buttons
   int cxPadding;                             // Padding between text and button
   int cxMargin;                              // Gap between button and content
   int cxSelMargin;                           // Additional gap for curr. sel. button
   int cxImagePadding;                        // Gap between text and image
   int cxOverlap;                             // Expand currently sel. button
} TCMETRICS;

typedef struct tagCOOLTCITEM : TCITEM
{
   int cx;
   RECT rcSize;
   COLORREF clrText;
   COLORREF clrBkgnd;
   LPTSTR pszTipText;
} COOLTCITEM;


template< class T, class TBase = CTabCtrl, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CCustomTabCtrl : 
   public CWindowImpl< T, TBase, TWinTraits >,
   public COffscreenDrawRect< T >
{
public:
   DECLARE_WND_CLASS(TBase::GetWndClassName())

   CCustomTabCtrl() : 
      m_iCurSel(-1), 
      m_dwExtStyle(0UL), 
      m_hFont(NULL), 
      m_hSelFont(NULL),
      m_nMinWidth(-1)
   {
      ::ZeroMemory(&m_metrics, sizeof(TCMETRICS));
   }

   // Data members

   int m_iCurSel;
   TCMETRICS m_metrics;
   CSimpleValArray< COOLTCITEM* > m_Items;
   CWindow m_wndNotify;
   CToolTipCtrl m_Tip;
   CImageList m_ImageList;
   DWORD m_dwExtStyle;
   UINT m_idDlgCtrl;
   HFONT m_hFont;
   HFONT m_hSelFont;
   int m_nMinWidth;

   // Operations

   BOOL SubclassWindow(HWND hWnd)
   {
      ATLASSERT(m_hWnd==NULL);
      ATLASSERT(::IsWindow(hWnd));
      BOOL bRet = CWindowImpl< T, TBase, TWinTraits >::SubclassWindow(hWnd);
      if( bRet ) _Init();
      return bRet;
   }

   int InsertItem(int iItem, LPCTSTR pstrText, int iImage = -1)
   {
      TCITEM tci = { 0 };
      tci.mask = TCIF_TEXT | TCIF_IMAGE;
      tci.pszText = const_cast<LPTSTR>(pstrText);
      tci.iImage = iImage;
      return InsertItem(iItem, &tci);
   }
   int InsertItem(int iItem, const LPTCITEM pItem)
   {
      COOLTCITEM tci;
      ::ZeroMemory(&tci, sizeof(tci));
      ::CopyMemory(&tci, pItem, sizeof(TCITEM));
      return InsertItem(iItem, &tci);
   }
   int InsertItem(int iItem, const COOLTCITEM* pItem)
   {
      ATLASSERT(::IsWindow(m_hWnd));
      ATLASSERT(pItem);
      ATLASSERT(iItem==m_Items.GetSize()); // We only support appending right now
      if( iItem < 0 || iItem > m_Items.GetSize() ) iItem = m_Items.GetSize();
      COOLTCITEM* pNewItem;
      ATLTRY( pNewItem = new COOLTCITEM );
      if( pNewItem == NULL ) return -1;
      ::ZeroMemory(pNewItem, sizeof(COOLTCITEM));
      m_Items.Add(pNewItem);
      int idx = m_Items.GetSize() - 1;
      if( !SetItem(idx, pItem) ) return FALSE;
      // Send notification
      NMHDR nmh = { m_hWnd, m_idDlgCtrl, TCN_INSERTITEM };
      m_wndNotify.SendMessage(WM_NOTIFY, nmh.idFrom, (LPARAM) &nmh);
      // Select if first tab
      if( m_Items.GetSize() == 1 ) SetCurSel(0);
      _Repaint();
      return idx;
   }

   BOOL DeleteAllItems()
   {
      ATLASSERT(::IsWindow(m_hWnd));
      while( GetItemCount() > 0 ) DeleteItem(0); // Slooow!!!
      return TRUE;
   }
   BOOL DeleteItem(int iItem)
   {
      ATLASSERT(::IsWindow(m_hWnd));
      if( iItem < 0 || iItem >= m_Items.GetSize() ) return FALSE;
      // If currently selected, select something else
      if( iItem < m_iCurSel ) {
         // The item being removed is before the current selection.
         // We still want the same item to be selected, but
         // the index needs to be adjusted to account for the missing item
         m_iCurSel--;
      }
      else if( iItem == m_iCurSel ) {
         SetCurSel( (m_Items.GetSize() > 1) ? 0 : -1 );
      }
      // Remove from structures
      COOLTCITEM* pItem = m_Items[iItem];
      m_Items.RemoveAt(iItem);
      if( pItem->mask & TCIF_TEXT ) ATLTRY( delete [] pItem->pszText );
      if( pItem->mask & TCIF_TOOLTIP ) ATLTRY( delete [] pItem->pszTipText );
      ATLTRY( delete pItem );
      // Send notification
      NMHDR nmh = { m_hWnd, m_idDlgCtrl, TCN_DELETEITEM };
      m_wndNotify.SendMessage(WM_NOTIFY, nmh.idFrom, (LPARAM) &nmh);
      _Repaint();
      return TRUE;
   }

   int SetItem(int iItem, const LPTCITEM pItem)
   {
      COOLTCITEM tci;
      ::ZeroMemory(&tci, sizeof(tci));
      ::CopyMemory(&tci, pItem, sizeof(TCITEM));
      return SetItem(iItem, &tci);
   }
   BOOL SetItem(int iItem, const COOLTCITEM* pItem)
   {
      ATLASSERT(::IsWindow(m_hWnd));
      ATLASSERT(!::IsBadReadPtr(pItem,sizeof(TCITEM)));
      if( !_ValidateItem(iItem) ) return FALSE;

      // Copy caller's data to the internal structure
      COOLTCITEM* pDstItem = m_Items[iItem];
      UINT mask = pItem->mask;
      if( mask & TCIF_TEXT ) {
         ATLASSERT(!::IsBadStringPtr(pItem->pszText,(UINT)-1));
         if( pDstItem->mask & TCIF_TEXT ) ATLTRY( delete [] pDstItem->pszText );
         ATLTRY( pDstItem->pszText = new TCHAR[ ::lstrlen(pItem->pszText) + 1 ] );
         ::lstrcpy(pDstItem->pszText, pItem->pszText);
      }
      if( mask & TCIF_TOOLTIP ) {
         ATLASSERT(!::IsBadStringPtr(pItem->pszTipText,(UINT)-1));
         if( pDstItem->mask & TCIF_TOOLTIP ) ATLTRY( delete [] pDstItem->pszTipText );
         ATLTRY( pDstItem->pszTipText = new TCHAR[ ::lstrlen(pItem->pszTipText) + 1 ] );
         ::lstrcpy(pDstItem->pszTipText, pItem->pszTipText);
      }
      if( mask & TCIF_STATE ) pDstItem->dwState = pItem->dwState;
      if( mask & TCIF_IMAGE ) pDstItem->iImage = pItem->iImage;
      if( mask & TCIF_PARAM ) pDstItem->lParam = pItem->lParam;
      if( mask & TCIF_WIDTH ) pDstItem->cx = pItem->cx;
      if( mask & TCIF_TEXTCOLOR ) pDstItem->clrText = pItem->clrText;
      if( mask & TCIF_BKCOLOR ) pDstItem->clrBkgnd = pItem->clrBkgnd;
      pDstItem->mask |= mask;

      _Repaint();
      return TRUE;
   }

   BOOL GetItem(int iItem, LPTCITEM pItem) const
   {
      ATLASSERT((pItem->mask & (TCIF_WIDTH|TCIF_TOOLTIP|TCIF_TEXTCOLOR|TCIF_BKCOLOR))==0);
      return GetItem(iItem, reinterpret_cast<COOLTCITEM*>(pItem));
   }
   BOOL GetItem(int iItem, COOLTCITEM* pItem) const
   {
      ATLASSERT(::IsWindow(m_hWnd));
      ATLASSERT(!::IsBadWritePtr(pItem,sizeof(TCITEM)));
      if( !_ValidateItem(iItem) ) return FALSE;

      // Copy item data
      COOLTCITEM* pSrcItem = m_Items[iItem];
      const UINT mask = pItem->mask;
      const UINT maskSrc = pSrcItem->mask;

      pItem->mask = 0;
      if( mask & TCIF_TEXT ) {
         ATLASSERT(pItem->pszText);
         pItem->mask |= (maskSrc & TCIF_TEXT);
         ::lstrcpyn( pItem->pszText, pSrcItem->pszText == NULL ? _T("") : pSrcItem->pszText, pItem->cchTextMax );
      }
      if( mask & TCIF_IMAGE ) {
         pItem->mask |= (maskSrc & TCIF_IMAGE);
         pItem->iImage = pSrcItem->iImage;
      }
      if( mask & TCIF_PARAM ) {
         pItem->mask |= (maskSrc & TCIF_PARAM);
         pItem->lParam = pSrcItem->lParam;
      }
      if( mask & TCIF_STATE ) {
         pItem->mask |= (maskSrc & TCIF_STATE);
         pItem->dwState = pSrcItem->dwState;
      }
      if( mask & TCIF_TEXTCOLOR ) {
         pItem->mask |= (maskSrc & TCIF_TEXTCOLOR);
         pItem->clrText = pSrcItem->clrText;
      }
      if( mask & TCIF_BKCOLOR ) {
         pItem->mask |= (maskSrc & TCIF_BKCOLOR);
         pItem->clrBkgnd = pSrcItem->clrBkgnd;
      }
      return 0;
   }
   
   int SetCurSel(int iItem)
   {
      ATLASSERT(::IsWindow(m_hWnd));
      // Selecting same tab? Not worth it!
      if( iItem == m_iCurSel ) return m_iCurSel;
      if( iItem < -1 || iItem >= m_Items.GetSize() ) return m_iCurSel;

      RECT rc = { 0 };
      int iOldSel = m_iCurSel;
      // Send notification
      NMHDR nmh = { m_hWnd, m_idDlgCtrl, TCN_SELCHANGING };
      if( m_wndNotify.SendMessage(WM_NOTIFY, nmh.idFrom, (LPARAM) &nmh) == TRUE ) return -1;
      // Repaint old button area
      GetItemRect(iItem, &rc); InvalidateRect(&rc);
      GetItemRect(iOldSel, &rc); InvalidateRect(&rc);

      // Change tab
      m_iCurSel = iItem;
      // Recalc new positions
      T* pT = static_cast<T*>(this);
      if( (pT->GetStyle() & WS_VISIBLE) != 0 ) pT->UpdateLayout();

      // Repaint whole tab
      GetClientRect(&rc); InvalidateRect(&rc);

      // Send notification
      nmh.code = TCN_SELCHANGE;
      m_wndNotify.SendMessage(WM_NOTIFY, nmh.idFrom, (LPARAM) &nmh);

      return iOldSel;
   }
   int GetCurSel() const
   {
      ATLASSERT(::IsWindow(m_hWnd));
      return m_iCurSel;
   }

   int GetItemCount() const
   {
      ATLASSERT(::IsWindow(m_hWnd));
      return m_Items.GetSize();
   }
   
   void SetImageList(HIMAGELIST hImageList)
   {
      m_ImageList = hImageList;
   }
   CImageList GetImageList() const
   {
      return m_ImageList;
   }
   
   int HitTest(POINT pt) const
   {
      TCHITTESTINFO tchti = { 0 };
      tchti.pt = pt;
      return HitTest(&tchti);
   }
   int HitTest(LPTCHITTESTINFO pinfo) const
   {
      ATLASSERT(!::IsBadWritePtr(pinfo,sizeof(TCHITTESTINFO)));
      for( int i = 0; i < m_Items.GetSize(); i++ ) {
         if( ::PtInRect(&m_Items[i]->rcSize, pinfo->pt) ) {
            pinfo->flags = TCHT_ONITEM;
            return i;
         }
      }
      return -1;
   }

   DWORD GetExtendedStyle() const
   {
      ATLASSERT(::IsWindow(m_hWnd));
      return m_dwExtStyle;
   }
   DWORD SetExtendedStyle(DWORD dwExMask, DWORD dwExStyle)
   {
      ATLASSERT(::IsWindow(m_hWnd));
      DWORD dwOldStyle = m_dwExtStyle;
      m_dwExtStyle = (m_dwExtStyle & ~dwExMask) | dwExStyle;
      _Repaint();
      return dwOldStyle;
   }

   int GetRowCount() const
   {
      return 1;
   }
   CToolTipCtrl GetTooltips() const
   {
      return m_Tip;
   }

   void SetNotify(HWND hWnd)
   {
      ATLASSERT(::IsWindow(hWnd));
      m_wndNotify = hWnd;
   }
   void SetMinTabWidth(int nWidth = -1)
   {
      m_nMinWidth = nWidth;
   }
   DWORD SetItemSize(int iItem, int cx, int cy)
   {
      ATLASSERT(::IsWindow(m_hWnd));
      if( !_ValidateItem(iItem) ) return 0;
      RECT rcOld = m_Items[iItem]->rcSize;     
      m_Items[iItem].cx = cx;
      m_Items[iItem].mask |= TCIF_WIDTH;
      _Repaint();
      return MAKELONG(rcOld.right - rcOld.left, rcOld.bottom - rcOld.top);
   }

   BOOL GetItemRect(int iItem, LPRECT prcItem) const
   {
      ATLASSERT(::IsWindow(m_hWnd));
      ATLASSERT(prcItem);
      if( prcItem == NULL ) return FALSE;
      ::SetRectEmpty(prcItem);
      if( iItem < 0 || iItem >= m_Items.GetSize() ) return FALSE;
      *prcItem = m_Items[iItem]->rcSize;
      return TRUE;
   }

   void GetMetrics(TCMETRICS *pMetrics) const
   {
      ATLASSERT(!::IsBadWritePtr(pMetrics,sizeof(TCMETRICS)));
      *pMetrics = m_metrics;
   }
   void SetMetrics(const TCMETRICS *pMetrics)
   {
      ATLASSERT(::IsWindow(m_hWnd));
      ATLASSERT(!::IsBadReadPtr(pMetrics,sizeof(TCMETRICS)));
      m_metrics = *pMetrics;
      _Repaint();
   }

   void SetSelFont(HFONT hFont)
   {
      m_hSelFont = hFont;
   }
   HFONT GetSelFont() const
   {
      return (m_dwExtStyle & TCS_EX_SELHIGHLIGHT) != 0 ? m_hSelFont : m_hFont;
   }

   int FindItem(LPTCITEM pFindInfo, int nStart = -1) const
   {
      ATLASSERT(::IsWindow(m_hWnd));
      if( nStart < 0 ) nStart = -1;     
      // Find the next item matching the criteria specified
      const UINT maskFind = pFindInfo->mask;
      int nCount = m_Items.GetSize();
      for( int i = nStart + 1; i < nCount; i++ ) {
         LPTCITEM pItem = m_Items[i];
         UINT mask = pItem->mask;
         if( (maskFind & mask) != maskFind ) continue;
         if( (maskFind & TCIF_PARAM) != 0 && pItem->lParam != pFindInfo->lParam ) continue;
         if( (maskFind & TCIF_TEXT) != 0 && pItem->pszText != NULL && pFindInfo->pszText != NULL && ::lstrcmp(pItem->pszText, pFindInfo->pszText) != 0 ) continue;
         if( (maskFind & TCIF_IMAGE) != 0 && pItem->iImage != pFindInfo->iImage ) continue;
         if( (maskFind & TCIF_STATE) != 0 && pItem->dwState != pFindInfo->dwState ) continue;
         return i;
      }
      return -1;
   }

   // Implementation

   void _Init()
   {
      ATLASSERT(::IsWindow(m_hWnd));
      ATLASSERT(GetStyle() & WS_CHILD);

      m_idDlgCtrl = GetDlgCtrlID();
      m_wndNotify = GetParent();

      SendMessage(WM_SETTINGCHANGE);

      // This is a little WTL subclass helper notification
      NMHDR nmh = { m_hWnd, m_idDlgCtrl, TCN_INITIALIZE };
      m_wndNotify.SendMessage(WM_NOTIFY, nmh.idFrom, (LPARAM) &nmh);
   }

   ATLINLINE bool _ValidateItem(int iItem) const
   {
      ATLASSERT(iItem>=0 && iItem<m_Items.GetSize());
      if( iItem < 0 || iItem >= m_Items.GetSize() ) return false;
      return true;
   }
   ATLINLINE void _Repaint()
   {
      T* pT = static_cast<T*>(this);
      if( (pT->GetStyle() & WS_VISIBLE) == 0 ) return;
      pT->UpdateLayout();
      Invalidate();
   }

   // Message map and handlers

   BEGIN_MSG_MAP(CCustomTabCtrl)
      CHAIN_MSG_MAP(COffscreenDrawRect< T >)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
      MESSAGE_HANDLER(WM_GETFONT, OnGetFont)
      MESSAGE_HANDLER(WM_SETFONT, OnSetFont)
      MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
      MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
      MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
      MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonClick)
      MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonClick)
      MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage) 
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc();
      _Init();
      return lRes;
   }
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {      
      ShowWindow(SW_HIDE);   // Repaint checks WS_VISIBLE; so we won't need repaints.
      DeleteAllItems();      // Make sure to clean up memory
      bHandled = FALSE;
      return 0;
   }
   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      T* pT = static_cast<T*>(this);
      pT->UpdateLayout();
      bHandled = FALSE;
      return 0;
   }
   LRESULT OnNcHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
      ScreenToClient(&pt);
      T* pT = static_cast<T*>(this);
      int iItem = pT->HitTest(pt);
      if( iItem == -1 ) return HTTRANSPARENT;
      return HTCLIENT;
   }
   LRESULT OnGetFont(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      return (LRESULT) (HFONT)( m_hFont != NULL ? m_hFont : AtlGetDefaultGuiFont() );
   }
   LRESULT OnSetFont(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      m_hFont = (HFONT) wParam;
      if( lParam != 0 ) _Repaint();
      return 0;
   }

   LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      _Repaint();
      return 0;
   }
   LRESULT OnGetDlgCode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      return DefWindowProc(uMsg, wParam, lParam) | DLGC_WANTARROWS;
   }

   LRESULT OnLButtonClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
      // Send click notification
      NMHDR nmh = { m_hWnd, m_idDlgCtrl, NM_CLICK };
      m_wndNotify.SendMessage(WM_NOTIFY, nmh.idFrom, (LPARAM) &nmh);
      // Select tab below
      T* pT = static_cast<T*>(this);
      int iItem = pT->HitTest(pt);
      if( iItem != -1 ) {
         SetFocus();
         SetCurSel(iItem);
      }
      return 0;
   }
   LRESULT OnRButtonClick(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      SendMessage(WM_LBUTTONDOWN, wParam, lParam); // BUG: Triggers NM_CLICK as well!
      NMHDR nmh = { m_hWnd, m_idDlgCtrl, NM_RCLICK };
      m_wndNotify.SendMessage(WM_NOTIFY, nmh.idFrom, (LPARAM) &nmh);
      return 0;
   }

   LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   {
      MSG msg = { m_hWnd, uMsg, wParam, lParam };
      if( m_Tip.IsWindow() ) m_Tip.RelayEvent(&msg);
      bHandled = FALSE;
      return 1;
   }

   LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
   {
      switch( wParam ) {
      case VK_LEFT:
         if( m_iCurSel > 0 ) SetCurSel(m_iCurSel - 1);
         return 0;
      case VK_RIGHT:
         if( m_iCurSel < m_Items.GetSize() - 1 ) SetCurSel(m_iCurSel + 1);
         return 0;
      }
      bHandled = FALSE;
      return 0;
   }

   // Overridables

   void UpdateLayout()
   {
      int nCount = m_Items.GetSize();
      if( nCount == 0 ) return;

      CClientDC dc(m_hWnd);
      HFONT hOldFont = dc.SelectStockFont(DEFAULT_GUI_FONT);

      RECT rcClient;
      GetClientRect(&rcClient);

      SIZE szIcon = { 0 };
      if( !m_ImageList.IsNull() ) m_ImageList.GetIconSize(szIcon);

      HFONT hFont = GetFont();
      HFONT hSelFont = GetSelFont();
      DWORD dwStyle = GetStyle();

      // Reposition buttons
      int xpos = m_metrics.cxIndent;
      for( int i = 0; i < nCount; i++ ) {
         COOLTCITEM* pItem = m_Items[i];
         // Hidden button?
         if( (pItem->dwState & TCIS_HIDDEN) != 0 ) continue;
         // Determine width...
         int cx = 0;
         // Expand width according to decorations
         if( pItem->iImage >= 0 && (pItem->mask & TCIF_IMAGE) != 0 && !m_ImageList.IsNull() ) {
            cx += szIcon.cx + (m_metrics.cxImagePadding * 2);
         }
         if( pItem->mask & TCIF_TEXT ) {
            RECT rcText = { 0 };
            dc.SelectFont(i == m_iCurSel ? hSelFont : hFont);
            dc.DrawText(pItem->pszText, ::lstrlen(pItem->pszText), &rcText, DT_SINGLELINE|DT_CALCRECT);
            cx += (rcText.right - rcText.left) + (m_metrics.cxPadding * 2);
         }
         // Add margins
         cx += m_metrics.cxMargin * 2;
         // Selected button is allowed to grow further
         if( m_iCurSel == i ) cx += m_metrics.cxSelMargin * 2;
         // Need separators?
         if( m_dwExtStyle & TCS_EX_FLATSEPARATORS ) cx += 2;
         // Fixed width?
         if( pItem->mask & TCIF_WIDTH ) cx = pItem->cx;
         // Minimum width?
         if( cx < m_nMinWidth ) cx = m_nMinWidth;
         // Finally...
         RECT& rc = pItem->rcSize;
         rc.top = 0;
         rc.bottom = rcClient.bottom - rcClient.top;
         rc.left = xpos;
         rc.right = xpos + cx;
         xpos += cx + m_metrics.cxButtonSpacing;
      }

      // Allow buttons to fill entire row
      int cx = (rcClient.right - rcClient.left) - xpos;
      if( dwStyle & TCS_RAGGEDRIGHT ) {
         if( cx > 0 ) {
            int iDiff = cx / m_Items.GetSize();
            for( int i = 0; i < nCount; i++ ) {
               m_Items[i]->rcSize.right += iDiff;
               if( i > 0 ) m_Items[i]->rcSize.left += iDiff;
               iDiff *= 2;
            }
         }
      }
      // Compress buttons on same line
      if( cx < 0 && (m_dwExtStyle & TCS_EX_COMPRESSLINE) != 0 ) {
         int xpos = m_metrics.cxIndent;
         int iWidth = (rcClient.right - rcClient.left) / nCount;
         for( int i = 0; i < nCount; i++ ) {
            COOLTCITEM* pItem = m_Items[i];
            int cx = min( iWidth, pItem->rcSize.right - pItem->rcSize.left );
            pItem->rcSize.right = xpos + cx;
            pItem->rcSize.left = xpos;
            xpos += cx;
         }
      }
    
      // Expand currently selected button to overlap other buttons.
      // NOTE: To make sense, take the cxIndent/cxMargin/cxSelMargin into
      //       account when choosing a value.
      if( m_iCurSel != - 1 ) ::InflateRect(&m_Items[m_iCurSel]->rcSize, m_metrics.cxOverlap, 0);

      dc.SelectFont(hOldFont);

      // Remove tooltips
      if( m_Tip.IsWindow() ) {
         TOOLINFO ti = { 0 };
         ti.cbSize = sizeof(ti);
         while( m_Tip.EnumTools(0, &ti) ) m_Tip.DelTool(&ti);
      }
      // Recreate tooltip rects
      for( int j = 0; j < nCount; j++ ) {
         if( m_Items[j]->mask & TCIF_TOOLTIP ) {
            if( !m_Tip.IsWindow() ) m_Tip.Create(m_hWnd);
            m_Tip.AddTool(m_hWnd, m_Items[j]->pszTipText, &m_Items[j]->rcSize, j + 1);
         }
         else if( dwStyle & TCS_TOOLTIPS ) {
            if( !m_Tip.IsWindow() ) m_Tip.Create(m_hWnd);
            m_Tip.AddTool(m_hWnd, m_Items[j]->pszText, &m_Items[j]->rcSize, j + 1);
         }
      }
      // Reactivate tooltips
      if( m_Tip.IsWindow() ) m_Tip.Activate(m_Tip.GetToolCount() > 0);
   }

   void DoPaint(CDCHandle dc, RECT &rcClip)
   {
      // NOTE: The handling of NM_CUSTOMDRAW is probably not entirely correct
      //       in the code below. But at least it makes a brave attempt to
      //       implement all the states described in MSDN docs.

      // Save current DC selections
      int save = dc.SaveDC();
      ATLASSERT(save!=0);

      // Make sure we don't paint outside client area (possible with paint dc)
      RECT rcClient;
      GetClientRect(&rcClient);
      ::IntersectClipRect(dc, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

      dc.FillRect(&rcClient, ::GetSysColorBrush(COLOR_3DFACE));

      // Prepare DC
      dc.SelectFont(GetFont());

      T* pT = static_cast<T*>(this);
      LRESULT lResStage;
      NMCUSTOMDRAW nmc = { 0 };
      nmc.hdr.hwndFrom = m_hWnd;
      nmc.hdr.idFrom = m_idDlgCtrl;
      nmc.hdr.code = NM_CUSTOMDRAW;
      nmc.hdc = dc;

      nmc.dwDrawStage = CDDS_PREPAINT;
      lResStage = m_wndNotify.SendMessage(WM_NOTIFY, nmc.hdr.idFrom, (LPARAM) &nmc);
      if( lResStage == CDRF_NOTIFYITEMDRAW || lResStage == CDRF_DODEFAULT ) {
         RECT rc;
         int nCount = m_Items.GetSize();
         // Draw the list items, except the selected one. It is drawn last
         // so it can cover the tabs below it.
         RECT rcIntersect;
         for( int i = 0; i < nCount; i++ ) {
            rc = m_Items[i]->rcSize;
            if( rc.bottom - rc.top == 0 ) pT->UpdateLayout();
            if( i != m_iCurSel ) {
               if( ::IntersectRect(&rcIntersect, &rc, &rcClip) ) {
                  nmc.dwItemSpec = i;
                  nmc.uItemState = 0;
                  if( (m_Items[i]->dwState & TCIS_DISABLED) != 0 ) nmc.uItemState |= CDIS_DISABLED;
                  nmc.rc = rc;
                  pT->ProcessItem(lResStage, nmc);
               }
            }
         }
         if( m_iCurSel != -1 ) {
            rc = m_Items[m_iCurSel]->rcSize;
            if( ::IntersectRect(&rcIntersect, &rc, &rcClip) ) {
               nmc.dwItemSpec = m_iCurSel;
               nmc.uItemState = CDIS_SELECTED;
               nmc.rc = rc;
               pT->ProcessItem(lResStage, nmc);
            }
         }
      }

      if( lResStage == CDRF_NOTIFYPOSTPAINT ) {
         nmc.dwItemSpec = 0;
         nmc.uItemState = 0;
         nmc.dwDrawStage = CDDS_POSTPAINT;
         m_wndNotify.SendMessage(WM_NOTIFY, nmc.hdr.idFrom, (LPARAM) &nmc);
      }

      dc.RestoreDC(save);
   }

   void ProcessItem(LRESULT lResStage, NMCUSTOMDRAW &nmc)
   {
      LRESULT lResItem = CDRF_DODEFAULT;
      if( lResStage == CDRF_NOTIFYITEMDRAW ) {
         nmc.dwDrawStage = CDDS_ITEMPREPAINT;
         lResItem = m_wndNotify.SendMessage(WM_NOTIFY, nmc.hdr.idFrom, (LPARAM) &nmc);
      }
      if( lResItem != CDRF_SKIPDEFAULT ) {
         // Do default item-drawing
         T* pT = static_cast<T*>(this);
         pT->DoItemPaint(nmc);
      }
      if( lResStage == CDRF_NOTIFYITEMDRAW && lResItem == CDRF_NOTIFYPOSTPAINT ) {
         nmc.dwDrawStage = CDDS_ITEMPOSTPAINT;
         m_wndNotify.SendMessage(WM_NOTIFY, nmc.hdr.idFrom, (LPARAM) &nmc);
      }
   }

   void DoItemPaint(NMCUSTOMDRAW &/*nmc*/)
   {
   }
};


/////////////////////////////////////////////////////////////////////////////
//
// The sample tab controls
//
// The follwing samples derive directly from CCustomTabCtrl.
// This means that they can actually use the internal members
// of this class. But they will not! To keep the code clean, I'm only
// using public member methods to access all variables.
//
// You need to add the...
//   REFLECT_NOTIFICATIONS()
// macro to the parent's message map.
//

class CButtonTabCtrl : 
   public CCustomTabCtrl<CButtonTabCtrl>,
   public CCustomDraw<CButtonTabCtrl>
{
public:
   DECLARE_WND_CLASS(_T("WTL_CoolButtonTabCtrl"))

   BEGIN_MSG_MAP(CButtonTabCtrl)
      REFLECTED_NOTIFY_CODE_HANDLER(TCN_INITIALIZE, OnInitialize)
      CHAIN_MSG_MAP(CCustomTabCtrl<CButtonTabCtrl>)
      CHAIN_MSG_MAP_ALT(CCustomDraw<CButtonTabCtrl>, 1)
      DEFAULT_REFLECTION_HANDLER()
   END_MSG_MAP()

   LRESULT OnInitialize(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
   {
      TCMETRICS metrics;
      GetMetrics(&metrics);
      metrics.cxButtonSpacing = 3;
      metrics.cxMargin = 10;
      SetMetrics(&metrics);
      return 0;
   }

   DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
   {
      return CDRF_NOTIFYITEMDRAW;   // We need per-item notifications
   }

   DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
   {
      ::SetBkMode(lpNMCustomDraw->hdc, TRANSPARENT);
      ::SetTextColor(lpNMCustomDraw->hdc, ::GetSysColor(COLOR_BTNTEXT));
      
      UINT state = 0;
      if( lpNMCustomDraw->uItemState & CDIS_SELECTED ) state |= DFCS_PUSHED;
      if( lpNMCustomDraw->uItemState & CDIS_DISABLED ) state |= DFCS_INACTIVE;
      ::DrawFrameControl(lpNMCustomDraw->hdc, &lpNMCustomDraw->rc, DFC_BUTTON, DFCS_BUTTONPUSH | state );
      
      TCITEM item = { 0 };
      TCHAR szText[128] = { 0 };
      item.mask = TCIF_TEXT;
      item.pszText = szText;
      item.cchTextMax = (sizeof(szText)/sizeof(TCHAR)) - 1;
      GetItem(lpNMCustomDraw->dwItemSpec, &item);
      if( lpNMCustomDraw->uItemState & CDIS_SELECTED ) {
         lpNMCustomDraw->rc.left += 2;
         lpNMCustomDraw->rc.top += 2;
      }
      ::DrawText(lpNMCustomDraw->hdc, item.pszText, ::lstrlen(item.pszText), &lpNMCustomDraw->rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

      return CDRF_SKIPDEFAULT;
   }
};

class CFolderTabCtrl : 
   public CCustomTabCtrl<CFolderTabCtrl>,
   public CCustomDraw<CFolderTabCtrl>
{
public:
   DECLARE_WND_CLASS(_T("WTL_CoolFolderTabCtrl"))

   CFont m_font;

   enum { CXOFFSET = 8 };     // defined pitch of trapezoid slant
   enum { CXMARGIN = 2 };     // left/right text margin
   enum { CYMARGIN = 1 };     // top/bottom text margin
   enum { CYBORDER = 1 };     // top border thickness
 
   BEGIN_MSG_MAP(CFolderTabCtrl)
      MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
      REFLECTED_NOTIFY_CODE_HANDLER(TCN_INITIALIZE, OnInitialize)
      CHAIN_MSG_MAP(CCustomTabCtrl<CFolderTabCtrl>)
      CHAIN_MSG_MAP_ALT(CCustomDraw<CFolderTabCtrl>, 1)
      DEFAULT_REFLECTION_HANDLER()
   END_MSG_MAP()

   LRESULT OnInitialize(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
   {
      TCMETRICS metrics;
      GetMetrics(&metrics);
      metrics.cxIndent = CXOFFSET;
      metrics.cxPadding = CXOFFSET + 3;
      metrics.cxOverlap = CXOFFSET;
      metrics.cxImagePadding = 2;
      SetMetrics(&metrics);
      return 0;
   }

   DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
   {
      return CDRF_NOTIFYITEMDRAW;   // We need per-item notifications
   }

   DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
   {
      CDCHandle dc( lpNMCustomDraw->hdc );
      bool bSelected = lpNMCustomDraw->uItemState & CDIS_SELECTED;

      COLORREF bgColor = ::GetSysColor(bSelected ? COLOR_WINDOW     : COLOR_3DFACE);
      COLORREF fgColor = ::GetSysColor(bSelected ? COLOR_WINDOWTEXT : COLOR_BTNTEXT);

      CBrush brush;
      brush.CreateSolidBrush(bgColor);     // background brush
      dc.SetBkColor(bgColor);              // text background
      dc.SetTextColor(fgColor);            // text color = fg color

      CPen shadowPen;
      shadowPen.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));

      // Fill trapezoid
      POINT pts[4];
      RECT rc = lpNMCustomDraw->rc;
      _GetTrapezoid(rc, pts);
      CPenHandle hOldPen = dc.SelectStockPen(BLACK_PEN);   
      CRgn rgn;
      rgn.CreatePolygonRgn(pts, 4, WINDING);
      dc.FillRgn(rgn, brush);

      // Draw edges. This requires two corrections:
      // 1) Trapezoid dimensions don't include the right and bottom edges,
      //    so must use one pixel less on bottom (cybottom)
      // 2) the endpoint of LineTo is not included when drawing the line, so
      //    must add one pixel (cytop)
      pts[1].y--;       // correction #1: true bottom edge y-coord
      pts[2].y--;       // ...ditto
      pts[3].y--;       // correction #2: extend final LineTo
      dc.MoveTo(pts[0]);              // upper left
      dc.LineTo(pts[1]);              // bottom left
      dc.SelectPen(shadowPen);        // bottom line is shadow color
      dc.MoveTo(pts[1]);              // line is inside trapezoid bottom
      dc.LineTo(pts[2]);              // ...
      dc.SelectStockPen(BLACK_PEN);   // upstroke is black
      dc.LineTo(pts[3]);              // y-1 to include endpoint
      if( !bSelected ) {
         // If not highlighted, upstroke has a 3D shadow, one pixel inside
         pts[2].x--;    // offset left one pixel
         pts[3].x--;    // ...ditto
         dc.SelectPen(shadowPen);
         dc.MoveTo(pts[2]);
         dc.LineTo(pts[3]);
      }
      dc.SelectPen(hOldPen);

      TCITEM item = { 0 };
      TCHAR szText[128] = { 0 };
      item.mask = TCIF_TEXT;
      item.pszText = szText;
      item.cchTextMax = (sizeof(szText) / sizeof(TCHAR)) - 1;
      GetItem(lpNMCustomDraw->dwItemSpec, &item);

      // Draw text
      ::InflateRect(&rc, -(CXOFFSET + CXMARGIN), -CYMARGIN);
      HFONT hOldFont = dc.SelectFont(bSelected ? GetSelFont() : GetFont());
      dc.DrawText(item.pszText, ::lstrlen(item.pszText), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
      dc.SelectFont(hOldFont);

      return CDRF_SKIPDEFAULT;
   }

   void _GetTrapezoid(const RECT& rc, POINT* pts) const
   {
      pts[0].x = rc.left;
      pts[0].y = rc.top;
      pts[1].x = rc.left + CXOFFSET;
      pts[1].y = rc.bottom;
      pts[2].x = rc.right - CXOFFSET - 1;
      pts[2].y = rc.bottom;
      pts[3].x = rc.right - 1, rc.top;
      pts[3].y = rc.top;
   }

   LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      // Initialize font
      if( !m_font.IsNull() ) m_font.DeleteObject();
      LOGFONT lf = { 0 };      
      lf.lfHeight = ::GetSystemMetrics(SM_CYHSCROLL) - CYMARGIN;
      lf.lfWeight = FW_NORMAL;
      lf.lfCharSet = DEFAULT_CHARSET;
      ::lstrcpy(lf.lfFaceName, _T("Arial"));
      m_font.CreateFontIndirect(&lf);
      SetFont(m_font);

      bHandled = FALSE;
      return 0;
   }
};

class CDotNetTabCtrl : 
   public CCustomTabCtrl<CDotNetTabCtrl>,
   public CCustomDraw<CDotNetTabCtrl>
{
public:
   DECLARE_WND_CLASS(_T("WTL_DotNetTabCtrl")) 

   CFont m_font;
   CFont m_fontBold;
   CBrush m_hbrBack;

   BEGIN_MSG_MAP(CDotNetTabCtrl)
      MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
      REFLECTED_NOTIFY_CODE_HANDLER(TCN_INITIALIZE, OnInitialize)
      CHAIN_MSG_MAP(CCustomTabCtrl<CDotNetTabCtrl>)
      CHAIN_MSG_MAP_ALT(CCustomDraw<CDotNetTabCtrl>, 1)
      DEFAULT_REFLECTION_HANDLER()
   END_MSG_MAP()

   LRESULT OnInitialize(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
   {
      TCMETRICS metrics;
      GetMetrics(&metrics);
      metrics.cxIndent = 6;
      metrics.cxPadding = 0;
      metrics.cxMargin = 4;
      metrics.cxSelMargin = 2;
      metrics.cxOverlap = 2;
      metrics.cxImagePadding = 2;
      SetMetrics(&metrics);
      return 0;
   }

   DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
   {
      CDCHandle dc( lpNMCustomDraw->hdc );

      RECT rc;
      GetClientRect(&rc);
      HBRUSH hOldBrush = dc.SelectBrush(m_hbrBack);
      dc.PatBlt(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, PATCOPY);
      if( GetStyle() & TCS_BOTTOM ) 
      {
         dc.SelectStockPen(BLACK_PEN);
         dc.MoveTo(rc.left, rc.top + 1);
         dc.LineTo(rc.right, rc.top + 1);
      }
      else
      {
         dc.SelectStockPen(WHITE_PEN);
         dc.MoveTo(rc.left, rc.bottom - 1);
         dc.LineTo(rc.right, rc.bottom - 1);
      }
      dc.SelectBrush(hOldBrush);

      dc.SetBkMode(TRANSPARENT);

      return CDRF_NOTIFYITEMDRAW;   // We need per-item notifications
   }

   DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
   {
      CDCHandle dc( lpNMCustomDraw->hdc );
      RECT rc = lpNMCustomDraw->rc;
      bool bSelected = (lpNMCustomDraw->uItemState & CDIS_SELECTED) != 0;

      int iCurSel = GetCurSel();
      DWORD dwStyle = GetStyle();
      DWORD dwExtStyle = GetExtendedStyle();

      if( dwStyle & TCS_BUTTONS ) /* */;
      else if( dwStyle & TCS_BOTTOM ) rc.bottom -= 3; 
      else rc.top += 3;

      // Paint separators if needed
      if( dwExtStyle & TCS_EX_FLATSEPARATORS ) 
      {
         if( (int) lpNMCustomDraw->dwItemSpec != iCurSel 
             && (int) lpNMCustomDraw->dwItemSpec != iCurSel - 1 
             && (int) lpNMCustomDraw->dwItemSpec != GetItemCount() - 1 ) 
         {
            RECT rcLine = { rc.right - 1, rc.top + 2, rc.right, rc.bottom - 2 };
            dc.FillRect(&rcLine, ::GetSysColorBrush(COLOR_GRAYTEXT));
            rc.right -= 2;
         }
      }

      rc.right--;
      RECT rcText = rc;

      // Tab is selected, so paint tab folder
      if( dwStyle & TCS_BUTTONS ) 
      {
         ::InflateRect(&rc, -4, -3);
         ::InflateRect(&rcText, -2, -2);
         if( bSelected ) 
         {
            dc.FillRect(&rc, ::GetSysColorBrush(COLOR_HIGHLIGHTTEXT));
            dc.Draw3dRect(&rc, ::GetSysColor(COLOR_HIGHLIGHT), ::GetSysColor(COLOR_HIGHLIGHT));
         }
      }
      else if( dwStyle & TCS_BOTTOM ) 
      {
         if( bSelected ) 
         {
            dc.FillRect(&rc, ::GetSysColorBrush(COLOR_BTNFACE));
            dc.SelectStockPen(WHITE_PEN);
            dc.MoveTo(rc.left, rc.top);
            dc.LineTo(rc.left, rc.bottom);
            dc.SelectStockPen(BLACK_PEN);
            dc.LineTo(rc.right, rc.bottom);
            dc.LineTo(rc.right, rc.top);
         }
         rcText.top += 4;
         rcText.bottom -= 1;
      }
      else
      {
         if( bSelected ) 
         {
            dc.FillRect(&rc, ::GetSysColorBrush(COLOR_BTNFACE));
            dc.SelectStockPen(WHITE_PEN);
            dc.MoveTo(rc.left, rc.bottom);
            dc.LineTo(rc.left, rc.top);
            dc.LineTo(rc.right, rc.top);
            dc.SelectStockPen(BLACK_PEN);
            dc.LineTo(rc.right, rc.bottom);
         }
         rcText.top += 1;
      }

      // Get some item information
      COOLTCITEM item;
      ::ZeroMemory(&item, sizeof(item));
      TCHAR szText[128] = { 0 };
      item.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_TEXTCOLOR;
      item.pszText = szText;
      item.cchTextMax = (sizeof(szText) / sizeof(TCHAR)) - 1;
      GetItem(lpNMCustomDraw->dwItemSpec, &item);

      TCMETRICS metrics;
      GetMetrics(&metrics);

      CImageList Images = GetImageList();

      rcText.left += 2;
      ::InflateRect(&rcText, -metrics.cxPadding, 0);

      if( item.iImage >= 0 && (item.mask & TCIF_IMAGE) != 0 && !Images.IsNull() ) 
      {
         SIZE szIcon;
         Images.GetIconSize(szIcon);
         if( szIcon.cx * 2 < rcText.right - rcText.left ) {
            Images.Draw(dc, item.iImage, rcText.left + metrics.cxImagePadding, rcText.top + (((rcText.bottom - rcText.top)/2) - (szIcon.cy/2)), ILD_TRANSPARENT);
            rcText.left += szIcon.cx + (metrics.cxImagePadding * 2);
         }
      }
      if( item.mask & TCIF_TEXT )
      {
         dc.SetTextColor(::GetSysColor(COLOR_BTNTEXT));
         if( item.mask & TCIF_TEXTCOLOR ) dc.SetTextColor(item.clrText);
         HFONT hOldFont = dc.SelectFont(bSelected ? GetSelFont() : GetFont());
         dc.DrawText(item.pszText, -1, &rcText, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS);
         dc.SelectFont(hOldFont);
      }

      return CDRF_SKIPDEFAULT;
   }

   LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      // Initialize font
      if( !m_font.IsNull() ) m_font.DeleteObject();
      NONCLIENTMETRICS ncm = { 0 };
      ncm.cbSize = sizeof(ncm);
      ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
      ncm.lfSmCaptionFont.lfWeight = FW_NORMAL;
      m_font.CreateFontIndirect(&ncm.lfSmCaptionFont);
      SetFont(m_font);
      if( !m_fontBold.IsNull() ) m_fontBold.DeleteObject();
      ncm.lfSmCaptionFont.lfWeight = FW_BOLD;
      m_fontBold.CreateFontIndirect(&ncm.lfSmCaptionFont);
      SetSelFont(m_fontBold);

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
                 GetBValue(clrBtnFace) + ((GetBValue(clrBtnHilite) - GetBValue(clrBtnFace)) / 2) );
         m_hbrBack.CreateSolidBrush(clrLight);
      }
      else {
         m_hbrBack =  CDCHandle::GetHalftoneBrush();
      }
      
      bHandled = FALSE;
      return 0;
   }
};


#endif // __COOLTABCTRLS_H__
