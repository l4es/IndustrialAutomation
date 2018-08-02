#ifndef __ATLCTRLXP_H__
#define __ATLCTRLXP_H__

/////////////////////////////////////////////////////////////////////////////
// CCommandBarXPCtrl - Command Bar (XP look) and others
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2001-2002 Bjarke Viksoe.
// Alex Kamenev provided chevron support.
// Thanks to Ramon Casellas for plenty for suggestions.
// Nicola Tufarelli supplied fixes for button texts.
//
// Overrides the original WTL CCommandBarCtrl.
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
   #error atlctrlxp.h requires atlctrlw.h to be included first
#endif

#if (_WTL_VER < 0x0700)
   #error This file requires WTL version 7.0 or higher
#endif


// This hack allows us to have a SDI and MDI commandbar (but only one).
// Add this define if you're building a MDI app.
#ifndef _WTL_USE_MDI
   #define COMMANDBAR_CLASS CCommandBarCtrlImpl
#else
   #define COMMANDBAR_CLASS CMDICommandBarCtrlImpl
#endif


/////////////////////////////////////////////////////////////////////////////
// CCommandBarXPCtrl - The Command Bar

// Standard XP Command Bar styles
#define ATL_SIMPLE_XP_CMDBAR_PANE_STYLE \
   (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CBRWS_NODIVIDER | CBRWS_NORESIZE | CBRWS_NOPARENTALIGN)

// Additional Extended CommandBar styles
#define CBR_EX_NOWIGGLE  0x10000000


class CFlatMenuWindow : 
   public CWindowImpl<CFlatMenuWindow>
#ifdef __DIALOGSHADOWS_H__
   , public CDialogShadows<CFlatMenuWindow>
#endif // __DIALOGSHADOWS_H__
{
public:
   DECLARE_WND_SUPERCLASS(_T("WTL_XpMenu"), GetWndClassName())

   BEGIN_MSG_MAP(CFlatMenuWindow)
      MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
#ifdef __DIALOGSHADOWS_H__
      CHAIN_MSG_MAP( CDialogShadows<CFlatMenuWindow> )
#endif // __DIALOGSHADOWS_H__
   END_MSG_MAP()

   COLORREF m_clrHighlightBorder;
   COLORREF m_clrBackground;
   COLORREF m_clrMenu;

   SIZE m_sizeBorder;
   int m_cxMenuButton;

   CFlatMenuWindow(int cxMenuButton, COLORREF clrBorder, COLORREF clrBack, COLORREF clrColor)
      : m_cxMenuButton(cxMenuButton), 
        m_clrHighlightBorder(clrBorder), 
        m_clrBackground(clrBack), 
        m_clrMenu(clrColor)
   {
      m_sizeBorder.cx = ::GetSystemMetrics(SM_CXDLGFRAME);
      m_sizeBorder.cy = ::GetSystemMetrics(SM_CYDLGFRAME);
   }

   virtual void OnFinalMessage(HWND /*hWnd*/)
   {
      delete this;
   }

   LRESULT OnNcPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      //CDCHandle dc = GetDCEx( (HRGN) wParam, DCX_WINDOW | DCX_INTERSECTRGN | 0x10000 );
      CWindowDC dc(m_hWnd);
      if( wParam > 1 ) dc.SelectClipRgn( (HRGN)wParam );
      RECT rcWin;
      GetWindowRect(&rcWin);
      ::OffsetRect(&rcWin, -rcWin.left, -rcWin.top);

      // Paint frame
      CBrush brushBack;
      brushBack.CreateSolidBrush(m_clrBackground);
      CPen pen;
      pen.CreatePen(PS_SOLID, 1, m_clrHighlightBorder);
      HPEN hOldPen = dc.SelectPen(pen);
      HBRUSH hOldBrush = dc.SelectBrush(brushBack);
      dc.Rectangle(rcWin.left, rcWin.top, rcWin.right, rcWin.bottom);

      // Fill area to the left with grey color
      CBrush brushColor;
      brushColor.CreateSolidBrush(m_clrMenu);
      RECT rcLeft = { rcWin.left + 1, rcWin.top + m_sizeBorder.cy, rcWin.left + m_sizeBorder.cx, rcWin.bottom - m_sizeBorder.cy };
      dc.FillRect(&rcLeft, brushColor);

      // If this is a top-level dropdown menu, smooth the top/left area
      if( m_cxMenuButton > 0 ) {
         CPen penColor;
         penColor.CreatePen(PS_SOLID, 1, m_clrMenu);
         dc.SelectPen(penColor);
         dc.MoveTo(rcWin.left + 1, rcWin.top);
         dc.LineTo(rcWin.left + m_cxMenuButton, rcWin.top);
      }

      dc.SelectPen(hOldPen);
      dc.SelectBrush(hOldBrush);
      return 1;
   }
};


typedef struct tagXPSTYLE
{
   COLORREF clrFrame;
   COLORREF clrBackground;
   COLORREF clrMenu;
   COLORREF clrHighlightBorder;
   COLORREF clrHighlight;
   COLORREF clrHighlightDark;
   COLORREF clrPressed;
   COLORREF clrGreyText;
   COLORREF clrMenuText;
   COLORREF clrSelMenuText;
   COLORREF clrButtonText;
   COLORREF clrSelButtonText;
   COLORREF clrShadow;
} XPSTYLE;


class CCommandBarXPCtrlBase : public CCommandBarCtrlBase
{
public:
   static RECT m_rcButton;
   static bool m_bIsMenuDropped;
   static XPSTYLE m_xpstyle;
};

template< class T, class TBase = CCommandBarXPCtrlBase, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CCommandBarXPCtrlImpl : public COMMANDBAR_CLASS< T, TBase, TWinTraits >
{
public:
   DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

   typedef COMMANDBAR_CLASS< T, TBase, TWinTraits > baseCtrlClass;

   enum 
   {      
      s_kcxTextMargin = 6,
      s_kcxDropDownArrowWidth = 14,
      s_kcxDropWholeArrowWidth = 10,
      s_kcxChevronItemWidth = 12,
      _nMaxMenuItemTextLength = 100,
   };

   // Declarations
  
   CSimpleValArray<HWND> m_Toolbars;

   // Message map and handlers

   BEGIN_MSG_MAP(CRecentCommandBarCtrlImpl)
      MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
      CHAIN_MSG_MAP( baseCtrlClass )
   ALT_MSG_MAP(1)      // Parent window messages
      NOTIFY_CODE_HANDLER(NM_CUSTOMDRAW, OnParentCustomDraw)
      NOTIFY_CODE_HANDLER(TBN_DROPDOWN, OnParentToolbarDropDown)
      CHAIN_MSG_MAP_ALT( baseCtrlClass, 1 )
   ALT_MSG_MAP(2)      // MDI client window messages
      CHAIN_MSG_MAP_ALT( baseCtrlClass, 2 )
   ALT_MSG_MAP(3)      // Message hook messages
      CHAIN_MSG_MAP_ALT( baseCtrlClass, 3 )
   END_MSG_MAP()

   CCommandBarXPCtrlImpl()
   {
   }

   // Operations

   BOOL AddToolbar(HWND hwndTB)
   {
      ATLASSERT(::IsWindow(hwndTB));
      m_Toolbars.Add(hwndTB);
#if (_WIN32_IE >= 0x0501)
      // WTL 7.1 attempts to set the clipped button style
      DWORD dwExStyle = (DWORD)::SendMessage(hwndTB, TB_GETEXTENDEDSTYLE, 0, 0L);
      ::SendMessage(hwndTB, TB_SETEXTENDEDSTYLE, 0, dwExStyle & ~TBSTYLE_EX_HIDECLIPPEDBUTTONS);
#endif //(_WIN32_IE >= 0x0501)
      return TRUE;
   }

#ifndef _WTL_USE_MDI
   BOOL SetMDIClient(HWND /*hWndMDIClient*/)
   {
      // Use CMDICommandBarCtrl for MDI support
      // and define _WTL_USE_MDI in stdafx.h
      ATLASSERT(false);
      return FALSE;
   }
#endif // _WTL_USE_MDI

   void Prepare()
   {
#ifndef RBBS_NOGRIPPER
      const UINT RBBS_NOGRIPPER = 0x00000100;
#endif // RBBS_NOGRIPPER
      // Update colors
      _GetSystemSettings();
      // Assume we are in a rebar
      HWND hWndReBar = GetParent();
      ATLASSERT(::IsWindow(hWndReBar));
      // Turn off gripper for menu and remove band edge
      int nCount = (int) ::SendMessage(hWndReBar, RB_GETBANDCOUNT, 0, 0L);
      for( int i = 0; i < nCount; i++ ) {
         REBARBANDINFO rbbi = { sizeof(REBARBANDINFO), RBBIM_CHILD | RBBIM_STYLE | RBBIM_COLORS };
         BOOL bRet = (BOOL)::SendMessage(hWndReBar, RB_GETBANDINFO, i, (LPARAM) &rbbi);
         if( bRet && rbbi.hwndChild == m_hWnd ) {
            // Menubar has no gripper.
            rbbi.fStyle |= RBBS_NOGRIPPER;
         }
         else {
            // Other toolbars have a special background colour
            rbbi.clrBack = m_xpstyle.clrMenu;
         }
         rbbi.fMask &= ~RBBIM_CHILD;
         ::SendMessage(hWndReBar, RB_SETBANDINFO, i, (LPARAM) &rbbi);
      }
      // A nasty hack to get rid of the small indent of the menu toolbar.
      // We hide & show the band, and presto - no paint/indent problems!?
      ::SendMessage(hWndReBar, RB_SHOWBAND, 0, 0L);
      ::SendMessage(hWndReBar, RB_SHOWBAND, 0, 1L);
   }

   // Implementation

   void _GetSystemSettings()
   {
#ifndef BlendRGB
#define BlendRGB(c1, c2, factor) \
   RGB( GetRValue(c1) + ((GetRValue(c2) - GetRValue(c1)) * factor / 100), \
        GetGValue(c1) + ((GetGValue(c2) - GetGValue(c1)) * factor / 100), \
        GetBValue(c1) + ((GetBValue(c2) - GetBValue(c1)) * factor / 100) )
#endif

      ::EnterCriticalSection(&_Module.m_csStaticDataInit);

      COLORREF clrWindow = ::GetSysColor(COLOR_WINDOW);
      COLORREF clrText = ::GetSysColor(COLOR_WINDOWTEXT);
      COLORREF clrButton = ::GetSysColor(COLOR_BTNFACE);

      CWindowDC dc(NULL);
      int nBitsPerPixel = dc.GetDeviceCaps(BITSPIXEL);
      if( nBitsPerPixel > 8 ) {
         m_xpstyle.clrBackground = BlendRGB(clrWindow, clrText, 3);
         m_xpstyle.clrMenu = BlendRGB(clrButton, clrWindow, 20);
         m_xpstyle.clrHighlightBorder = ::GetSysColor(COLOR_HIGHLIGHT);
         m_xpstyle.clrHighlight = BlendRGB(m_xpstyle.clrHighlightBorder, clrWindow, 70);
         m_xpstyle.clrHighlightDark = BlendRGB(m_xpstyle.clrHighlightBorder, clrWindow, 60);
         m_xpstyle.clrPressed = BlendRGB(m_xpstyle.clrHighlight, clrText, 20);
         m_xpstyle.clrShadow = BlendRGB(::GetSysColor(COLOR_3DFACE), ::GetSysColor(COLOR_WINDOWTEXT), 35);
      }
      else {
         m_xpstyle.clrBackground = clrWindow;
         m_xpstyle.clrMenu = clrButton;
         m_xpstyle.clrHighlightBorder = ::GetSysColor(COLOR_HIGHLIGHT);
         m_xpstyle.clrHighlight = clrWindow;
         m_xpstyle.clrHighlightDark = clrWindow;
         m_xpstyle.clrPressed = ::GetSysColor(COLOR_HIGHLIGHT);
         m_xpstyle.clrShadow = ::GetSysColor(COLOR_GRAYTEXT);
      }
      m_xpstyle.clrFrame = ::GetSysColor(COLOR_WINDOWFRAME);
      m_xpstyle.clrGreyText = ::GetSysColor(COLOR_GRAYTEXT);
      m_xpstyle.clrMenuText = ::GetSysColor(COLOR_WINDOWTEXT);
      m_xpstyle.clrSelMenuText = ::GetSysColor(COLOR_MENUTEXT);
      m_xpstyle.clrButtonText = ::GetSysColor(COLOR_BTNTEXT);
      m_xpstyle.clrSelButtonText = ::GetSysColor(COLOR_MENUTEXT);

      ::LeaveCriticalSection(&_Module.m_csStaticDataInit);
   }

#if (_WIN32_IE >= 0x0500)
   // Chevron support by Alex Kamenev, thanks.
   void _DrawChevron(HWND hwndRebar, UINT nItemId, CDCHandle& dc, RECT rc)
   {
      // Assume we are in a rebar
      CReBarCtrl wndRB = hwndRebar;
      // Get current band
      int nItem = wndRB.IdToIndex(nItemId);
      RECT rcTb; // child toolbar rect
      REBARBANDINFO rbbi = { sizeof(REBARBANDINFO),  RBBIM_STYLE | RBBIM_IDEALSIZE | RBBIM_HEADERSIZE | RBBIM_CHILD | RBBIM_COLORS };
      if( ::SendMessage(wndRB, RB_GETBANDINFO, nItem, (LPARAM) &rbbi) )
      {
         ::GetWindowRect(rbbi.hwndChild, &rcTb);
         if( (rbbi.fStyle & RBBS_USECHEVRON) != 0 && ( (UINT)(rcTb.right - rcTb.left) < rbbi.cxIdeal) ) 
         {
            POINT pt;
            ::GetCursorPos(&pt);
            ::ScreenToClient(wndRB, &pt);
            CBrush brush;
            RECT rcChevItem = { rc.left + rbbi.cxHeader + (rcTb.right - rcTb.left) + 4, rc.top, 0, 0 };
            RECT rcBand;
            wndRB.GetRect(nItem, &rcBand);
            rcChevItem.right = rcChevItem.left + s_kcxChevronItemWidth;
            rcChevItem.bottom = rcChevItem.top + (rcBand.bottom - rcBand.top);
#ifdef _CMDBAR_EXTRA_TRACE
            ATLTRACE2(atlTraceUI, 0, "CmdBar - DrawChevron1: (%d,%d; %d;%d), (%d;%d)\n", rcChevItem.left, rcChevItem.top, rcChevItem.right, rcChevItem.bottom, pt.x, pt.y);
#endif
            if( ::PtInRect(&rcChevItem, pt) )
            {
               CPen pen;
               brush.CreateSolidBrush(m_xpstyle.clrHighlight);
               pen.CreatePen(PS_SOLID, 1, m_xpstyle.clrHighlightBorder);
               HPEN hOldPen = dc.SelectPen(pen);
               HBRUSH hOldBrush = dc.SelectBrush(brush);
               dc.Rectangle(&rcChevItem);
               dc.SelectPen(hOldPen);
               dc.SelectBrush(hOldBrush);
            }
            else
            {
               brush.CreateSolidBrush(rbbi.clrBack);
               dc.FillRect(&rcChevItem, brush);
            }
            // Draw chevron
            CFont fnt;
            CFontHandle fntOld;
            LOGFONT lf = { 0 };
            ::lstrcpy(lf.lfFaceName, _T("Verdana"));
            lf.lfHeight = -MulDiv(8, dc.GetDeviceCaps(LOGPIXELSY), 72); 
            lf.lfWeight = FW_BOLD;
            fnt.CreateFontIndirect(&lf);
            fntOld = dc.SelectFont(fnt);          
            WCHAR wszChevron[] = { 0x00bb, 0x0000 }; //TCHAR szChevron[] = _T(">");
            dc.SetBkMode(TRANSPARENT);
            ::TextOutW(dc, rc.left + rbbi.cxHeader + (rcTb.right - rcTb.left) + 5, rc.top + 1, wszChevron, 1);
            dc.SelectFont(fntOld);
         }
      }
   }
#endif //(_WIN32_IE >= 0x0500)

   // Implementation - overriding original functionality
   
   BOOL DoTrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, LPTPMPARAMS lpParams = NULL)
   {
#ifdef _CMDBAR_EXTRA_TRACE
      ATLTRACE2(atlTraceUI, 0, "CmdBar - DoTrackPopupMenu, m_bContextMenu: %s\n", m_bContextMenu ? "yes" : "no");
#endif

      // Remember to call Prepare()
      // Add toolbars with AddToolbar()
      ATLASSERT(m_xpstyle.clrBackground!=m_xpstyle.clrMenuText);

      // Added by Tom Furuya
      // Make flat menu overlap with menu button at its bottom frame line
      if( lpParams ) {
         lpParams->rcExclude.bottom -= 1; // don't exclude bottom frame
         y -= 1;                          // and move up the menu
      }

#ifndef TPM_HORPOSANIMATION
      const UINT TPM_HORPOSANIMATION = 0x0400;
      const UINT TPM_HORNEGANIMATION = 0x0800;
      const UINT TPM_VERPOSANIMATION = 0x1000;
      const UINT TPM_VERNEGANIMATION = 0x2000;
#endif // TPM_HORPOSANIMATION
#ifndef TPM_NOANIMATION
      const UINT TPM_NOANIMATION = 0x4000;
#endif // TPM_NOANIMATION
      OSVERSIONINFO ovi;
      ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
      BOOL bRet = ::GetVersionEx(&ovi);
      if( bRet && 
          ovi.dwPlatformId == VER_PLATFORM_WIN32_NT && 
          ovi.dwMajorVersion <= 4 ) 
      {
         // There are no menu-animations on Windows NT 4, so we
         // don't tinker with the flags. NT4 might fail otherwise.
         // Thanks to Andy Karels for this tip.
      }
      else {
         // We need to turn off animations because they mess up the first menu paint.
         // Thanks to Ramon Casellas for this tip.
         uFlags |= TPM_NOANIMATION;
         uFlags &= ~( TPM_HORPOSANIMATION | TPM_HORNEGANIMATION | TPM_VERPOSANIMATION | TPM_VERNEGANIMATION );
      }

      // Toggle menu fading/animation mode (Windows2K or later)
#ifndef SPI_GETMENUFADE
      const UINT SPI_GETMENUFADE = 0x1012;
      const UINT SPI_SETMENUFADE = 0x1013;
#endif // SPI_GETMENUFADE
#ifndef SPI_GETMENUANIMATION
      const UINT SPI_GETMENUANIMATION = 0x1002;
      const UINT SPI_SETMENUANIMATION = 0x1003;
#endif // SPI_GETMENUANIMATION
#ifndef SPI_GETANIMATION
      const UINT SPI_GETANIMATION = 0x0048;
      const UINT SPI_SETANIMATION = 0x0049;
#endif // SPI_GETANIMATION
      BOOL bMenuFading = FALSE;
      BOOL bMenuAnim = FALSE;
      if( ovi.dwMajorVersion >= 5 )
      {
         bRet = ::SystemParametersInfo(SPI_GETMENUFADE, 0, &bMenuFading, 0);
         bRet = ::SystemParametersInfo(SPI_GETMENUANIMATION, 0, &bMenuAnim, 0);
         bRet = ::SystemParametersInfo(SPI_SETMENUFADE, 0, (LPVOID) FALSE, SPIF_SENDWININICHANGE);
         bRet = ::SystemParametersInfo(SPI_SETMENUANIMATION, 0, (LPVOID) FALSE, SPIF_SENDWININICHANGE);
      }
#ifdef _CMDBAR_EXTRA_TRACE
      ATLTRACE2(atlTraceUI, 0, "CmdBar - DoTrackPopupMenu:, bMenuFading = %s, bMenuAnim = %s\n",
         bMenuFading ? "true" : "false", bMenuAnim ? "true" : "false");
#endif

      // Figure out the size of the pressed button
      if( !m_bContextMenu ) GetItemRect(m_nPopBtn, &m_rcButton);

      CMenuHandle menuPopup = hMenu;

      ::EnterCriticalSection(&_Module.m_csWindowCreate);

      ATLASSERT(s_hCreateHook == NULL);

      s_pCurrentBar = static_cast<CCommandBarCtrlBase*>(this);

      s_hCreateHook = ::SetWindowsHookEx(WH_CBT, MyCreateHookProc, _Module.GetModuleInstance(), GetCurrentThreadId());
      ATLASSERT(s_hCreateHook != NULL);

      m_bPopupItem = false;
      m_bMenuActive = true;

      BOOL bTrackRet = menuPopup.TrackPopupMenuEx(uFlags, x, y, m_hWnd, lpParams);
      m_bMenuActive = false;

      ::UnhookWindowsHookEx(s_hCreateHook);

      s_hCreateHook = NULL;
      s_pCurrentBar = NULL;

      ::LeaveCriticalSection(&_Module.m_csWindowCreate);

      // Cleanup - convert menus back to original state
#ifdef _CMDBAR_EXTRA_TRACE
      ATLTRACE2(atlTraceUI, 0, "CmdBar - TrackPopupMenu - cleanup\n");
#endif

      ATLASSERT(m_stackMenuWnd.GetSize() == 0);

      // These updates are also from the original WTL CommandBar control
      // but they actually solves some paint problems with submenus in this
      // control as well.
      UpdateWindow();
      CWindow wndTL = GetTopLevelParent();
      wndTL.UpdateWindow();

      // Restore menu fade/animation mode (Windows2K or later)
      if( ovi.dwMajorVersion >= 5 )
      {
         bRet = ::SystemParametersInfo(SPI_SETMENUFADE, 0, (LPVOID) bMenuFading, SPIF_SENDWININICHANGE);
         bRet = ::SystemParametersInfo(SPI_SETMENUANIMATION, 0, (LPVOID) bMenuAnim, SPIF_SENDWININICHANGE);
      }

      // Restore the menu items to the previous state for all menus that were converted
      if( m_bImagesVisible )
      {
         HMENU hMenuSav;
         while( (hMenuSav = m_stackMenuHandle.Pop()) != NULL ) {
            menuPopup = hMenuSav;
            BOOL bRet = FALSE;
            // Restore state and delete menu item data
            for( int i = 0; i < menuPopup.GetMenuItemCount(); i++ ) {
               CMenuItemInfo mii;
               mii.fMask = MIIM_DATA | MIIM_TYPE | MIIM_ID;
               bRet = menuPopup.GetMenuItemInfo(i, TRUE, &mii);
               ATLASSERT(bRet);

               _MenuItemData* pMI = (_MenuItemData*)mii.dwItemData;
               if( pMI != NULL && pMI->IsCmdBarMenuItem() )
               {
                  mii.fMask = MIIM_DATA | MIIM_TYPE | MIIM_STATE;
                  mii.fType = pMI->fType;
                  mii.fState = pMI->fState;
                  mii.dwTypeData = pMI->lpstrText;
                  mii.cch = lstrlen(pMI->lpstrText);
                  mii.dwItemData = NULL;

                  bRet = menuPopup.SetMenuItemInfo(i, TRUE, &mii);
                  // This one triggers WM_MEASUREITEM
                  menuPopup.ModifyMenu(i, MF_BYPOSITION | mii.fType | mii.fState, mii.wID, pMI->lpstrText);
                  ATLASSERT(bRet);

                  delete [] pMI->lpstrText;
                  delete pMI;
               }
            }
         }
      }
      return bTrackRet;
   }

   // Implementation - Hook procs
   
   static LRESULT CALLBACK MyCreateHookProc(int nCode, WPARAM wParam, LPARAM lParam)
   {
      LRESULT lRet = 0;
      TCHAR szClassName[7];

      if( nCode == HCBT_CREATEWND )
      {
         HWND hWndMenu = (HWND)wParam;
#ifdef _CMDBAR_EXTRA_TRACE
         ATLTRACE2(atlTraceUI, 0, "CmdBar - HCBT_CREATEWND (HWND = %8.8X)\n", hWndMenu);
#endif

         ::GetClassName(hWndMenu, szClassName, 7);
         if( ::lstrcmp(_T("#32768"), szClassName) == 0 ) {
            s_pCurrentBar->m_stackMenuWnd.Push(hWndMenu);

            // Subclass to a flat-looking menu
            CFlatMenuWindow* wnd = new CFlatMenuWindow(m_rcButton.right - m_rcButton.left, m_xpstyle.clrFrame, m_xpstyle.clrBackground, m_xpstyle.clrMenu);
            wnd->SubclassWindow(hWndMenu);
            wnd->SetWindowPos(HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_DRAWFRAME);
            ::SetRectEmpty(&m_rcButton);
         }
      }
      else if( nCode == HCBT_DESTROYWND )
      {
         HWND hWndMenu = (HWND)wParam;
#ifdef _CMDBAR_EXTRA_TRACE
         ATLTRACE2(atlTraceUI, 0, "CmdBar - HCBT_DESTROYWND (HWND = %8.8X)\n", hWndMenu);
#endif

         ::GetClassName(hWndMenu, szClassName, 7);
         if( ::lstrcmp(_T("#32768"), szClassName) == 0 )
         {
            ATLASSERT(hWndMenu == s_pCurrentBar->m_stackMenuWnd.GetCurrent());
            s_pCurrentBar->m_stackMenuWnd.Pop();
         }
      }
      else if( nCode < 0 )
      {
         lRet = ::CallNextHookEx(s_hCreateHook, nCode, wParam, lParam);
      }
      return lRet;
   }

   // Implementation - ownerdraw overridables and helpers

   LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      // Update colors
      _GetSystemSettings();
      // Allow WTL Command Bar to set its settings...
      bHandled = FALSE;
      return 0;
   }

   // Ownerdrawn menu

   void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
   {
      _MenuItemData* pmd = (_MenuItemData*)lpMeasureItemStruct->itemData;

      if( (pmd->fType & MFT_SEPARATOR) != 0 ) // separator - is really small
      {
         lpMeasureItemStruct->itemHeight = 5;
         lpMeasureItemStruct->itemWidth  = 0;
      }
      else
      {
         // ** Removed 'static' as suggested by Ilya Kheifets.
         CFont fontBold;
         // Compute size of text - use DrawText with DT_CALCRECT
         CWindowDC dc = m_hWnd;
         HFONT hOldFont;
         if( (pmd->fState & MFS_DEFAULT) != 0 ) {
            // Need bold version of font
            LOGFONT lf;
            m_fontMenu.GetLogFont(lf);
            lf.lfWeight += 200;
            fontBold.CreateFontIndirect(&lf);
            ATLASSERT(!fontBold.IsNull());
            hOldFont = dc.SelectFont(fontBold);
         }
         else {
            hOldFont = dc.SelectFont(m_fontMenu);
         }

         RECT rcText = { 0 };
         dc.DrawText(pmd->lpstrText, -1, &rcText, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_CALCRECT);
         dc.SelectFont(hOldFont);

         int cx = rcText.right - rcText.left;

         LOGFONT lf;
         m_fontMenu.GetLogFont(lf);
         int cy = lf.lfHeight;
         if( cy < 0 ) cy = -cy;
         cy += 10;

         // Height of item is the bigger of these two
         lpMeasureItemStruct->itemHeight = max(cy, (int) m_szButton.cy + 2);

         // Width is width of text plus a bunch of stuff
         cx += 2 * s_kcxTextMargin;       // L/R margin for readability
         cx += s_kcxGap;                  // Space between button and menu text
         cx += 2 * m_szButton.cx;         // Button width (L=button; R=empty margin)

         // Windows adds 1 to returned value
         cx -= ::GetSystemMetrics(SM_CXMENUCHECK) - 1;
         lpMeasureItemStruct->itemWidth = cx;      // done deal
      }
   }

   void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
   {
      _MenuItemData* pmd = (_MenuItemData*) lpDrawItemStruct->itemData;
      CDCHandle dc = lpDrawItemStruct->hDC;
      const RECT& rcItem = lpDrawItemStruct->rcItem;

      bool bSelected = (lpDrawItemStruct->itemState & ODS_SELECTED) != 0;
      bool bDisabled = (lpDrawItemStruct->itemState & ODS_GRAYED) != 0;
      bool bChecked = (lpDrawItemStruct->itemState & ODS_CHECKED) != 0;

      if( LOWORD(lpDrawItemStruct->itemID) == (WORD) -1 ) bSelected = false;
      
      RECT rcButn = { rcItem.left, rcItem.top, rcItem.left + m_szButton.cx, rcItem.top + m_szButton.cy }; // button rect
      ::OffsetRect(&rcButn, 0, ((rcItem.bottom - rcItem.top) - (rcButn.bottom - rcButn.top)) / 2); // center vertically

      int cxButn = m_szButton.cx;
      RECT rcIcon = rcItem;
      RECT rcText = rcItem;
      rcText.left += cxButn + s_kcxGap;
      rcIcon.right = rcText.left;

      // Draw background

      if( !bDisabled && bSelected ) 
      {
         CBrush brush;
         CPen pen;
         brush.CreateSolidBrush(m_xpstyle.clrHighlight);
         pen.CreatePen(PS_SOLID, 1, m_xpstyle.clrHighlightBorder);
         HPEN hOldPen = dc.SelectPen(pen);
         HBRUSH hOldBrush = dc.SelectBrush(brush);
         dc.Rectangle(&rcItem);
         dc.SelectPen(hOldPen);
         dc.SelectBrush(hOldBrush);
      }
      else 
      {
         CBrush brushIcon;
         CBrush brushText;
         brushIcon.CreateSolidBrush(m_xpstyle.clrMenu);
         brushText.CreateSolidBrush(m_xpstyle.clrBackground);      
         dc.FillRect(&rcIcon, brushIcon);
         dc.FillRect(&rcText, brushText);
      }

      // Draw item

      if( (pmd->fType & MFT_SEPARATOR) != 0 ) 
      {
         // Draw separator

         CPen pen;
         pen.CreatePen(PS_SOLID, 1, m_xpstyle.clrMenu);
         HPEN hOldPen = dc.SelectPen(pen);

         RECT rc = rcText;
         rc.top += (rc.bottom - rc.top) / 2;   // vertical center
         rc.left += s_kcxTextMargin + 4;
         dc.MoveTo(rc.left, rc.top);
         dc.LineTo(rc.right, rc.top);

         dc.SelectPen(hOldPen);
      }
      else  // Not a separator
      {      
         
         // Draw icon

         int iButton = pmd->iButton;
         if( iButton >= 0 ) {
            // Calc drawing point
            SIZE sz = { rcButn.right - rcButn.left - m_szBitmap.cx, rcButn.bottom - rcButn.top - m_szBitmap.cy };
            sz.cx /= 2;
            sz.cy /= 2;
            POINT point = { rcButn.left + sz.cx, rcButn.top + sz.cy };
            if( bDisabled ) {
               DrawShadowIcon(dc, m_hImageList, point, iButton);
            }
            else {
               if( bSelected ) {
                  // Draw shadow
                  if( (m_dwExtendedStyle & CBR_EX_NOWIGGLE) == 0 ) {
                     point.x++;
                     point.y++;
                     DrawShadowIcon(dc, m_hImageList, point, iButton);
                     point.x -= 2;
                     point.y -= 2;
                  }
                  else {
                     DrawShadowIcon(dc, m_hImageList, point, iButton);
                  }
               }
               // Draw the image
               ImageList_Draw(m_hImageList, iButton, dc, point.x, point.y, ILD_TRANSPARENT);
            }
         }
         else {
            // No image - look for custom checked/unchecked bitmaps
            CMenuItemInfo info;
            info.fMask = MIIM_CHECKMARKS | MIIM_TYPE;
            ::GetMenuItemInfo( (HMENU)lpDrawItemStruct->hwndItem, lpDrawItemStruct->itemID, MF_BYCOMMAND, &info );
            if( bChecked || info.hbmpUnchecked != NULL ) {
               bool bRadio = ((info.fType & MFT_RADIOCHECK) != 0);
               DrawCheckmark(dc, rcButn, bSelected, bDisabled, bRadio, bChecked);
            }
         }

         // Draw text

         COLORREF clrText;
         if( bDisabled ) clrText = m_xpstyle.clrGreyText;
         else if( bSelected ) clrText = m_xpstyle.clrSelMenuText;
         else clrText = m_xpstyle.clrMenuText;

         rcText.right -= cxButn;
         rcText.left += s_kcxTextMargin;
         dc.SetBkMode(TRANSPARENT);
         DrawMenuText(dc, rcText, pmd->lpstrText, clrText); // finally!        
      }
   }

   void DrawMenuText(CDCHandle& dc, RECT& rc, LPCTSTR lpstrText, COLORREF color) const
   {
#ifndef DT_HIDEPREFIX
      const UINT DT_HIDEPREFIX = 0x00100000;
#endif // DT_HIDEPREFIX
      int nTab = -1;
      for( int i = 0; i < ::lstrlen(lpstrText); i++ ) {
         if( lpstrText[i] == '\t' ) {
            nTab = i;
            break;
         }
      }
      dc.SetTextColor(color);
      UINT uFlags = DT_SINGLELINE | DT_VCENTER;
      if( !m_bShowKeyboardCues ) uFlags |= DT_HIDEPREFIX;
      dc.DrawText(lpstrText, nTab, &rc, DT_LEFT | uFlags);
      if( nTab >= 0 ) dc.DrawText(&lpstrText[nTab + 1], -1, &rc, DT_RIGHT | uFlags);
   }

   static void DrawShadowIcon(CDCHandle& dc, HIMAGELIST hImageList, POINT pt, int nImage)
   {
#if (_ATL_VER >= 0x0700)
      HICON hIcon = ImageList_ExtractIcon(ATL::_AtlBaseModule.GetResourceInstance(), hImageList, nImage);
#else
      HICON hIcon = ImageList_ExtractIcon(_Module.GetResouceInstance(), hImageList, nImage);
#endif
      CBrush brush;
      brush.CreateSolidBrush( m_xpstyle.clrShadow );
      SIZE sz = { 0, 0 };
      dc.DrawState( pt, sz, hIcon, DSS_MONO, brush);
      ::DestroyIcon(hIcon);
   }

   static void DrawCheckmark(CDCHandle& dc, RECT rc, bool bSelected, bool /*bDisabled*/, bool /*bRadio*/, bool bChecked)
   {
      ::InflateRect(&rc, -3, -3);
      COLORREF clrBackground = bSelected ? m_xpstyle.clrHighlightDark : m_xpstyle.clrHighlight;
      CBrush brush;
      CPen pen;
      brush.CreateSolidBrush(clrBackground);
      pen.CreatePen(PS_SOLID, 1, m_xpstyle.clrHighlightBorder);
      HPEN hOldPen = dc.SelectPen(pen);
      HBRUSH hOldBrush = dc.SelectBrush(brush);
      dc.Rectangle(&rc);
      dc.SelectPen(hOldPen);
      dc.SelectBrush(hOldBrush);

      if( bChecked ) {
         // Draw checkmark...
         // This is really a job for DrawFrameControl(), but it's a little
         // complex (see atlctrlw.h Draw3DCheckmark()... )
#ifndef OEMRESOURCE
         const UINT OBM_CHECK = 32760;
#endif // OEMRESOURCE
         CBitmap bmp;
         bmp.LoadOEMBitmap(OBM_CHECK);
         SIZE szBitmap = { 0 };
         bmp.GetSize(szBitmap);
         SIZE size = { rc.right - rc.left, rc.bottom - rc.top };
         POINT pt = { rc.left + 1 + ((size.cx - szBitmap.cx) / 2), rc.top + ((size.cy - szBitmap.cy) / 2) };
         dc.SetBkColor(clrBackground);
         dc.DrawState(pt, size, bmp, DSS_NORMAL);
      }
   }

   static void DrawArrow(CDCHandle& dc, RECT rc, bool /*bSelected*/)
   {
      // Draw arrow
      HPEN hOldPen = dc.SelectStockPen( BLACK_PEN );
      HBRUSH hOldBrush = dc.SelectStockBrush( BLACK_BRUSH );
      int xMiddle = rc.left + (rc.right - rc.left) / 2;
      int yMiddle = rc.top + (rc.bottom - rc.top) / 2;
      const int ARROW_SIZE = 2;
      POINT pt[] = {
         { xMiddle - ARROW_SIZE, yMiddle - ARROW_SIZE },
         { xMiddle + ARROW_SIZE, yMiddle - ARROW_SIZE },
         { xMiddle, yMiddle + 1 }
      };
      dc.Polygon(pt, 3);
      dc.SelectPen(hOldPen);
      dc.SelectBrush(hOldBrush);
   }

   // Ownerdrawn toolbar/menubar

   LRESULT OnParentCustomDraw(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
   {
      LRESULT lRet = CDRF_DODEFAULT;
      bHandled = FALSE;

      // Figure out if we're drawing on a known toolbar
      bool bIsMenubar = false;
      bool bKnownToolbar = false;
      if( pnmh->hwndFrom == m_hWnd ) bIsMenubar = true;
      int cnt = m_Toolbars.GetSize();
      for( int i = 0; i < cnt; i++ ) {
         if( m_Toolbars[i] == pnmh->hwndFrom ) {
            bKnownToolbar = true;
            break;
         }
      }
      // Ok, custom draw toolbar then...
      if( bIsMenubar || bKnownToolbar )
      {
         LPNMTBCUSTOMDRAW lpTBCustomDraw = (LPNMTBCUSTOMDRAW) pnmh;
         if( lpTBCustomDraw->nmcd.dwDrawStage == CDDS_PREPAINT )
         {
            lRet = CDRF_NOTIFYITEMDRAW;
            bHandled = TRUE;
         }
         else if( lpTBCustomDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
         {  
            // Get item information
            CDCHandle dc(lpTBCustomDraw->nmcd.hdc);
            HFONT hOldFont = dc.SelectFont(m_fontMenu);
            _DrawToolbarButton(lpTBCustomDraw, m_dwExtendedStyle, m_bShowKeyboardCues, bIsMenubar);
            dc.SelectFont(hOldFont);
            lRet = CDRF_SKIPDEFAULT;
         }
         bHandled = TRUE;
      }
      // Paint gripper bar
      else if( pnmh->hwndFrom == GetParent() ) 
      {
         LPNMCUSTOMDRAW lpCustomDraw = (LPNMCUSTOMDRAW) pnmh;
         if( lpCustomDraw->dwDrawStage == CDDS_PREPAINT )
         {
            lRet = CDRF_NOTIFYITEMDRAW;
            bHandled = TRUE;
         }
         else if( lpCustomDraw->dwDrawStage == CDDS_ITEMPREPAINT ) 
         {
            CDCHandle dc(lpCustomDraw->hdc);
            RECT rc = lpCustomDraw->rc;
            dc.FillSolidRect(&rc, m_xpstyle.clrMenu);
            ::InflateRect(&rc, -2, -4);
            rc.left += 2;
            if( rc.left < rc.right ) {
               COLORREF clrTextOld = dc.SetTextColor(::GetSysColor(COLOR_BTNSHADOW));
               COLORREF clrBkOld = dc.SetBkColor(::GetSysColor(COLOR_BTNFACE));
               CBrush brDither = _GetGripperBrush();
               dc.FillRect(&rc, brDither);
               dc.SetTextColor(clrTextOld);
               dc.SetBkColor(clrBkOld);
#if (_WIN32_IE >= 0x0500)
               _DrawChevron(pnmh->hwndFrom, lpCustomDraw->dwItemSpec, dc, lpCustomDraw->rc);
#endif //(_WIN32_IE >= 0x0500)
            }
            lRet = CDRF_SKIPDEFAULT;
            bHandled = TRUE;
         }
      }
      return lRet;
   }

   LRESULT OnParentToolbarDropDown(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
   {
      LPNMTOOLBAR lptb = (LPNMTOOLBAR) pnmh;
      CToolBarCtrl tb = lptb->hdr.hwndFrom;
      tb.GetItemRect(tb.CommandToIndex(lptb->iItem), &m_rcButton);
      m_bIsMenuDropped = true;
      bHandled = FALSE;
      return 0;
   }

   // Draw helpers

   static CBrushHandle _GetGripperBrush()
   {
      HBRUSH hBrush = NULL;
      WORD grayPattern[8] = { 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00 };
      HBITMAP hBitmap = CreateBitmap(8, 8, 1, 1, &grayPattern);
      if( hBitmap != NULL ) {
         hBrush = ::CreatePatternBrush(hBitmap);
         ::DeleteObject(hBitmap);
      }
      return CBrushHandle(hBrush);
   }
   static void _DrawToolbarButton(LPNMTBCUSTOMDRAW lpTBCustomDraw, 
                                  DWORD dwExtendedStyle = 0,
                                  bool bShowKeyboardCues = true, 
                                  bool bIsMenuBar = false)
   {
      CDCHandle dc(lpTBCustomDraw->nmcd.hdc);

      CToolBarCtrl tb = lpTBCustomDraw->nmcd.hdr.hwndFrom;

      TBBUTTONINFO tbi = { 0 };
      TCHAR szText[_nMaxMenuItemTextLength] = { 0 };
      tbi.cbSize = sizeof(TBBUTTONINFO);
      tbi.dwMask = TBIF_TEXT | TBIF_STYLE | TBIF_STATE | TBIF_IMAGE;
      tbi.pszText = szText;
      tbi.cchText = (sizeof(szText) / sizeof(TCHAR)) - 1;
      tb.GetButtonInfo((int)lpTBCustomDraw->nmcd.dwItemSpec, &tbi);

      // Retrieve text string from string pool
      // Thanks to Nicola Tufarelli for fixing this...
      if( ::lstrlen(szText) == 0 ) tb.GetButtonText((int)lpTBCustomDraw->nmcd.dwItemSpec, szText);

      // Get state information

#ifndef ODS_HOTLIGHT
      const UINT ODS_HOTLIGHT = 0x0040;
#endif // ODS_HOTLIGHT
#ifndef BTNS_WHOLEDROPDOWN
      const UINT BTNS_WHOLEDROPDOWN = 0x0080;
#endif // BTNS_WHOLEDROPDOWN
      UINT uItemState = lpTBCustomDraw->nmcd.uItemState;
      bool bSelected = (uItemState & ODS_SELECTED) != 0;
      bool bHotlight = (uItemState & ODS_HOTLIGHT) != 0;
      bool bChecked = (tbi.fsState & TBSTATE_CHECKED) != 0;
      bool bPressed = (tbi.fsState & TBSTATE_PRESSED) != 0;
      bool bDisabled = (tbi.fsState & TBSTATE_ENABLED) == 0;
      bool bDropDown = (tbi.fsStyle & TBSTYLE_DROPDOWN) != 0;
      bool bDropWhole = (tbi.fsStyle & BTNS_WHOLEDROPDOWN) != 0;
      bool bList = ((tb.GetStyle() & TBSTYLE_LIST) == TBSTYLE_LIST);
#if (_WIN32_IE >= 0x0400)
      RECT rcItem = lpTBCustomDraw->nmcd.rc;
#else //!(_WIN32_IE >= 0x0400)
      RECT rcItem;
      tb.GetItemRect(0, &rcItem);
#endif //!(_WIN32_IE >= 0x0400)
      
#ifdef _CMDBAR_EXTRA_TRACE
      ATLTRACE2(atlTraceUI, 0, "CmdBar - DrawToolbarButton State:%ld Style:%ld ItemState:%ld\n", tbi.fsState, tbi.fsStyle, uItemState);
#endif

      // Draw highlight

      if( (bSelected || bHotlight || bPressed || bChecked) && !bDisabled ) {
         if( m_bIsMenuDropped ) {
            CBrush brushIcon;
            brushIcon.CreateSolidBrush(m_xpstyle.clrMenu);
            dc.FillRect(&rcItem, brushIcon);
            dc.DrawEdge(&rcItem, EDGE_RAISED, BF_TOP | BF_LEFT | BF_RIGHT | BF_FLAT | BF_MONO);
            m_bIsMenuDropped = false;
         }
         else if( bDropDown && bIsMenuBar && (bSelected || bPressed) ) {
            // Menu bar button
            CBrush brushIcon;
            brushIcon.CreateSolidBrush(m_xpstyle.clrMenu);
            dc.FillRect(&rcItem, brushIcon);
            dc.DrawEdge(&rcItem, EDGE_RAISED, BF_TOP | BF_LEFT | BF_RIGHT | BF_FLAT | BF_MONO);
         }
         else {
            // Ordinary toolbar or unselected menu
            CBrush brush;
            CPen pen;
            brush.CreateSolidBrush(bSelected ? m_xpstyle.clrPressed : m_xpstyle.clrHighlight);
            pen.CreatePen(PS_SOLID, 1, m_xpstyle.clrHighlightBorder);
            HPEN hOldPen = dc.SelectPen(pen);
            HBRUSH hOldBrush = dc.SelectBrush(brush);
            dc.Rectangle(&rcItem);
            if( bDropDown && !bIsMenuBar ) {
               int cxThumb = s_kcxDropDownArrowWidth;
               RECT rcThumb = rcItem;
               rcThumb.left = rcThumb.right - cxThumb;
               // Draw additional border to separate arrow
               dc.Rectangle(&rcThumb);
            }
            dc.SelectBrush(hOldBrush);
            dc.SelectPen(hOldPen);
         }
      }
      
      if( bDropDown && !bIsMenuBar ) {
         // Paint arrow
         int cxThumb = s_kcxDropDownArrowWidth;
         RECT rcThumb = rcItem;
         rcThumb.left = rcThumb.right - cxThumb;
         DrawArrow(dc, rcThumb, false);
      }
      else if( bDropWhole && !bIsMenuBar ) {
         // Paint arrow
         int cxThumb = s_kcxDropWholeArrowWidth;
         RECT rcThumb = rcItem;
         rcThumb.left = rcThumb.right - cxThumb;
         DrawArrow(dc, rcThumb, false);
      }
      
      // Draw image

      UINT uTextFlags = DT_CENTER;

      if( tbi.iImage >= 0 ) {
         // Get ImageList
         HIMAGELIST hImageList = NULL;
         if( bHotlight ) hImageList = tb.GetHotImageList();
         if( bDisabled ) hImageList = tb.GetDisabledImageList();
         if( hImageList == NULL ) hImageList = tb.GetImageList();
         // Draw icon
         if( hImageList != NULL ) {
            int cxIcon, cyIcon;
            ImageList_GetIconSize(hImageList, &cxIcon, &cyIcon);
            SIZE sizeButton;
            tb.GetButtonSize(sizeButton);
            //SIZE sizePadding;
            //tb.GetPadding(&sizePadding);
            SIZE sizePadding = { (sizeButton.cx - cxIcon)/2, (sizeButton.cy - cyIcon)/2 };
            // Fix by Nicola Tufarelli:
            //   If the button has text to diplay make room for it
            //     on the right of the image if TBSTYLE_LIST is set or
            //     at the bottom of the image if not
            //POINT point = { rcItem.left + sizePadding.cx, rcItem.top + sizePadding.cy + 1 };
            POINT point = { rcItem.left + (bList ? 4 : sizePadding.cx),
                            rcItem.top + (::lstrlen(szText) > 0 ? 4 : sizePadding.cy + 1) };
            // Draw the image
            if( bDisabled ) {
               // Draw disabled icon (shadow, really)
               DrawShadowIcon(dc, hImageList, point, tbi.iImage);
            }
            else if( bHotlight || bSelected ) {
               if( !bSelected ) {
                  // Draw selected icon shadow
                  if( (dwExtendedStyle & CBR_EX_NOWIGGLE) == 0 ) {
                     point.x++;
                     point.y++;
                     DrawShadowIcon(dc, hImageList, point, tbi.iImage);
                     point.x -= 2;
                     point.y -= 2;
                  }
                  else {
                     DrawShadowIcon(dc, hImageList, point, tbi.iImage);
                  }
               }
               // Finally draw the icon above
               ImageList_Draw(hImageList, tbi.iImage, dc, point.x, point.y, ILD_TRANSPARENT);
            }
            else {
               // Draw normal icon
               ImageList_DrawEx(hImageList, tbi.iImage, dc, point.x, point.y, 0, 0, CLR_NONE, CLR_NONE, ILD_TRANSPARENT);
            }

            if( bList ) {
               uTextFlags = DT_LEFT;
               rcItem.left += cxIcon + 8;
            }
            else if( ::lstrlen(szText) > 0 ) {
               rcItem.top += sizePadding.cy + 6;
            }
         }
      }

      // Draw text

      if( ::lstrlen(szText) > 0 ) {
#ifndef DT_HIDEPREFIX
         const UINT DT_HIDEPREFIX = 0x00100000;
#endif // DT_HIDEPREFIX
         COLORREF clrText;
         if( bDisabled ) clrText = m_xpstyle.clrGreyText;
         else if( bHotlight ) clrText = m_xpstyle.clrSelButtonText;
         else clrText = m_xpstyle.clrButtonText;

         uTextFlags |= DT_SINGLELINE | DT_VCENTER;
         if( !bShowKeyboardCues ) uTextFlags |= DT_HIDEPREFIX;
         RECT rcText = rcItem;

         dc.SetBkMode(TRANSPARENT);
         dc.SetTextColor(clrText);
         dc.DrawText(szText, -1, &rcText, uTextFlags);
      }
   }
};

__declspec(selectany) RECT CCommandBarXPCtrlBase::m_rcButton = { 0 };
__declspec(selectany) XPSTYLE CCommandBarXPCtrlBase::m_xpstyle = { 0 };
__declspec(selectany) bool CCommandBarXPCtrlBase::m_bIsMenuDropped = false;


class CCommandBarXPCtrl : public CCommandBarXPCtrlImpl<CCommandBarXPCtrl>
{
public:
   DECLARE_WND_SUPERCLASS(_T("WTL_CommandBarXP"), GetWndClassName())
};

// The _WTL_USE_MDI define takes care of this.
typedef CCommandBarXPCtrl CMDICommandBarXPCtrl;


#endif // __ATLCTRLWXP_H__
