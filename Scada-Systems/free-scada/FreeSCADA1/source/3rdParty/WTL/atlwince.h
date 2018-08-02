// Windows Template Library - WTL version 8.0
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// This file is a part of the Windows Template Library.
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/licenses/cpl.php)
// which can be found in the file CPL.TXT at the root of this distribution.
// By using this software in any fashion, you are agreeing to be bound by
// the terms of this license. You must not remove this notice, or
// any other, from this software.

#ifndef __ATLWINCE_H__
#define __ATLWINCE_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
	#error atlwince.h requires atlapp.h to be included first
#endif

#ifndef __ATLWIN_H__
	#error atlwince.h requires atlwin.h to be included first
#endif

#ifndef _WIN32_WCE
	#error atlwince.h compiles under Windows CE only
#elif (_WIN32_WCE < 300)
	#error atlwince.h requires Windows CE 3.0 or higher.
#endif

#if defined(WIN32_PLATFORM_WFSP) &&  _MSC_VER < 1400 // EVC compiling SmartPhone code
  #if (WIN32_PLATFORM_WFSP < 200)
	#error atlwince.h requires Smartphone 2003 or higher
  #endif
#endif // WIN32_PLATFORM_WFSP

#if defined(WIN32_PLATFORM_PSPC) &&  _MSC_VER < 1400 // EVC compiling Pocket PC code
  #if (WIN32_PLATFORM_PSPC < 310)
	#error atlwince.h requires Pocket PC 2002 or higher
  #endif
#endif // WIN32_PLATFORM_PSPC

#if !defined(_AYGSHELL_H_) && !defined(__AYGSHELL_H__)
	#error atlwince.h requires aygshell.h to be included first
#else
	#if defined(WIN32_PLATFORM_WFSP) && !defined(_TPCSHELL_H_)
		#error SmartPhone dialog classes require tpcshell.h to be included first
	#endif
#endif

#if _MSC_VER >= 1400 // VS2005
	#include <DeviceResolutionAware.h>
	#define _WTL_CE_DRA
#endif // _MSC_VER >= 1400

#if !defined(_WTL_CE_NO_DIALOGS) &&  !defined(__ATLFRAME_H__)
	#error Orientation aware dialog classes require atlframe.h to be included first
#endif

#if !defined(_WTL_CE_NO_APPWINDOW) &&  !defined(__ATLFRAME_H__)
	#error Application window class require atlframe.h to be included first
#endif

#if !defined(_WTL_CE_NO_ZOOMSCROLL) &&  !defined(__ATLSCRL_H__)
	#error ZoomScroll implementation requires atlscrl.h to be included first
#endif

#if !defined(_WTL_CE_NO_ZOOMSCROLL) && !defined(__ATLTYPES_H__)
  #if !defined(__ATLMISC_H__) || defined(_WTL_NO_WTYPES)
	#error ZoomScroll WTL::CSize usage requires _WTL_NO_WTYPES to be undefined and atlmisc.h to be included first
  #elif defined(__ATLTYPES_H__)
    #if !defined(__ATLMISC_H__) || !defined(_WTL_NO_WTYPES)
	#error ZoomScroll ATL::CSize usage requires _WTL_NO_WTYPES to be defined and atlmisc.h to be included first
    #endif			
  #endif			
#endif

#if !defined(WIN32_PLATFORM_WFSP) && !defined(WIN32_PLATFORM_PSPC)
  #define _WTL_CE_NO_CONTROLS
#endif // !defined(WIN32_PLATFORM_WFSP) && !defined(WIN32_PLATFORM_PSPC)

#ifndef _WTL_CE_NO_CONTROLS
  #ifndef __ATLCTRLS_H__
	#error The PPC/SmartPhone controls classes require atlctrls.h to be included first
  #endif

  #include <htmlctrl.h>
  #pragma comment(lib, "htmlview.lib")

  #include <voicectl.h>
  #pragma comment(lib, "voicectl.lib")

  #ifdef WIN32_PLATFORM_PSPC
	#include <richink.h>
	#pragma comment(lib, "richink.lib")

	#include <inkx.h>
	#pragma comment(lib, "inkx.lib")

	#include <doclist.h>
	#pragma comment(lib, "doclist.lib")
  #endif
#endif


///////////////////////////////////////////////////////////////////////////////
// Classes in this file:
//
// CStdDialogBase<T, t_shidiFlags, t_bModal> : Standard dialog base class
// CStdDialogImpl<T, t_shidiFlags, t_bModal> : Standard dialog implementation
// CStdSimpleDialog<t_wDlgTemplateID, t_shidiFlags> : Standard simple dialog
// CStdDialogResizeBase<T, t_shidiFlags, t_bModal> : Orientation aware standard dialog base class
// CStdDialogResizeImpl<T, t_shidiFlags, t_bModal> : Orientation aware standard dialog implementation
// CStdSimpleDialogResizeImpl<T, t_wDlgTemplateID, t_shidiFlags> : Standard resizing simple dialog implementation
// CStdOrientedDialogBase<T, t_shidiFlags, t_bModal> : Orientable dialog base class
// CStdOrientedDialogImpl<T, t_shidiFlags, t_bModal> : Orientable dialog implementation
// CStdSimpleOrientedDialog<t_wDlgTemplateID, t_wDlgLandscapeID, t_shidiFlags> : Standard simple orientable dialog
//
// CAppInfoBase	 : Helper for application state save/restore to registry
// CAppInfoT<T> : CAppInfoBase constructed from a CAppWindow<T>
// CAppWindowBase<T> : Base class for PPC/SmartPhone well-behaved application window or dialog
// CAppWindow<T> : PPC/SmartPhone well-behaved application window class
// CAppDialog<T> : PPC/SmartPhone well-behaved application non-modal dialog class
// CAppStdDialogImpl<T, t_shidiFlags> : PPC/SmartPhone implementation of non-modal standard dialog application
//
// CFullScreenFrame<T, t_bHasSip> : Full screen frame class
//
// CZoomScrollImpl<T> : WinCE zooming implementation
//
// CHtmlCtrlT<TBase> - CHtmlCtrl
// CRichInkCtrlT<TBase> - CRichInkCtrl
// CInkXCtrlT<TBase> - CInkXCtrl
// CVoiceRecorderCtrlT<TBase> - CVoiceRecorderCtrl
// CDocListCtrlT<TBase> - CDocListCtrl
// CCapEditT<TBase> - CCapEdit
// CTTStaticT<TBase> - CTTStatic
// CTTButtonT<TBase> - CTTButton
//
// CSpinCtrlT<TBase> - CSpinCtrl : SmartPhone specific UpDown control
// CSpinned<TBase, t_bExpandOnly> : SmartPhone association of control and Spin
// CSpinListBox : SmartPhone spinned ListBox control
// CExpandListBox : SmartPhone expandable ListBox control
// CExpandEdit : SmartPhone expandable Edit control
// CExpandCapEdit : SmartPhone expandable CapEdit control
//
// Global functions:
//   AtlCreateMenuBar()
//   AtlCreateEmptyMenuBar()

namespace WTL
{

///////////////////////////////////////////////////////////////////////////////
// MenuBar creation functions for property sheets and dialogs
// Frame windows use CreateSimpleCEMenuBar

inline HWND AtlCreateMenuBar(SHMENUBARINFO& mbi)
{
	ATLASSERT(::IsWindow(mbi.hwndParent));
	ATLVERIFY(::SHCreateMenuBar(&mbi) != FALSE);
	return mbi.hwndMB;
};

inline HWND AtlCreateMenuBar(HWND hWnd, UINT nToolBarId = ATL_IDW_TOOLBAR, DWORD dwFlags = 0, int nBmpId = 0, int cBmpImages = 0, COLORREF clrBk = 0)
{
#if (_ATL_VER >= 0x0700)
	SHMENUBARINFO mbi = { sizeof(mbi), hWnd, dwFlags, nToolBarId, ATL::_AtlBaseModule.GetResourceInstance(), nBmpId, cBmpImages, 0, clrBk };
#else // !(_ATL_VER >= 0x0700)
	SHMENUBARINFO mbi = { sizeof(mbi), hWnd, dwFlags, nToolBarId, _Module.GetResourceInstance(), nBmpId, cBmpImages, 0, clrBk };
#endif // !(_ATL_VER >= 0x0700)
	return AtlCreateMenuBar(mbi);
}

inline HWND AtlCreateEmptyMenuBar(HWND hWnd, bool bSip = true)
{
	SHMENUBARINFO embi = { sizeof(SHMENUBARINFO), hWnd, SHCMBF_EMPTYBAR };
	if (!bSip)
		embi.dwFlags |= SHCMBF_HIDESIPBUTTON;
	
	return AtlCreateMenuBar(embi);
}


// --- Standard PPC/SmartPhone dialogs ---

#ifndef _WTL_CE_NO_DIALOGS
	
///////////////////////////////////////////////////////////////////////////////
// CStdDialogBase - base class for standard PPC/SmartPhone dialogs

#define WTL_STD_SHIDIF   SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN
#define WTL_SP_SHIDIF    SHIDIF_SIZEDLGFULLSCREEN

template <class T, UINT t_shidiFlags, bool t_bModal = true>
class CStdDialogBase
{
public:
// Pocket PC only Dialog title handling
#ifdef WIN32_PLATFORM_PSPC
	const int nTitleHeight;

	CStdDialogBase() : nTitleHeight(24)
	{ }

	// Title painting
	LRESULT OnPaintTitle(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		TCHAR sTitle[40];

		// Preparation
		CPaintDC dc(pT->m_hWnd);
		CFont fontTitle = AtlCreateBoldFont();
		CFontHandle fontOld = dc.SelectFont(fontTitle);
		dc.SetTextColor(RGB(0, 0, 156));
		int nLen = pT->GetWindowText(sTitle, 40);
		int nWidth = dc.GetDeviceCaps(HORZRES);

		// Display title text
		RECT rTitle = { 8, 0, nWidth, nTitleHeight };
		dc.DrawText(sTitle, nLen, &rTitle, DT_VCENTER | DT_SINGLELINE);
		dc.SelectFont(fontOld);

		// Draw bottom line
		CPenHandle penOld = dc.SelectStockPen(BLACK_PEN);
		POINT line[2] = { { 0, nTitleHeight }, { nWidth, nTitleHeight } };
		dc.Polyline(line, 2);
		dc.SelectPen(penOld);

		return bHandled = FALSE;
	}

	// Title preparation: move the dialog controls down to make room for title
	void DialogTitleInit()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));

		ATL::CWindow wCtl = pT->GetWindow(GW_CHILD);
		while (wCtl.IsWindow())
		{
			RECT rCtl = { 0 };
			wCtl.GetWindowRect(&rCtl);
			::MapWindowPoints(NULL, pT->m_hWnd, (LPPOINT)&rCtl, 2);
			::OffsetRect(&rCtl, 0, nTitleHeight);
			wCtl.MoveWindow(&rCtl, FALSE);
			wCtl = wCtl.GetWindow(GW_HWNDNEXT);
		}
	}
#endif // WIN32_PLATFORM_PSPC

#ifdef WIN32_PLATFORM_WFSP
// SmartPhone VK_TBACK key standard management
	LRESULT OnHotKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		UINT uModif = (UINT)LOWORD(lParam);
		UINT uVirtKey = (UINT)HIWORD(lParam);

		if(uVirtKey == VK_TBACK)
		{
			ATL::CWindow wCtrl = GetFocus();
			if (wCtrl.IsWindow())
			{
				TCHAR szClassName[8] = {0};
				ATLVERIFY(::GetClassName(wCtrl.m_hWnd, szClassName, 8));
				if (!_tcscmp(szClassName, _T("Edit")) || !_tcscmp(szClassName, WC_CAPEDIT))
				{
					::SHSendBackToFocusWindow(uMsg, wParam, lParam);
				}
				else
				{
					if (uModif & MOD_KEYUP)
						pT->PostMessage(WM_COMMAND, IDCANCEL, 0);
				}
			}
		}
		return 1;
	}

// Menu dialog ending
	LRESULT OnMenuClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->PostMessage(WM_COMMAND, wID == ID_MENU_CANCEL ? IDCANCEL : IDOK);
		return 0;
	}

	void SetStaticBold()
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));

		CFontHandle fontBold = AtlCreateBoldFont(pT->GetFont());

		ATL::CWindow wCtl = pT->GetWindow(GW_CHILD);

		while (wCtl.IsWindow())
		{
			if ((short int)wCtl.GetDlgCtrlID() == IDC_STATIC)
				wCtl.SetFont(fontBold);
			wCtl = wCtl.GetWindow(GW_HWNDNEXT);
		}
	}
#endif // WIN32_PLATFORM_WFSP

// Platform dependant initialization
	void StdPlatformInit()
	{
#ifdef WIN32_PLATFORM_PSPC // Pocket PC title initialization
		DialogTitleInit();
#elif defined(WIN32_PLATFORM_WFSP) // SmartPhone MenuBar and VK_TBACK key initialization
		T* pT = static_cast<T*>(this);
		HWND hMenuBar = NULL;

		if (t_shidiFlags & SHIDIF_DONEBUTTON)
			hMenuBar = AtlCreateMenuBar(pT->m_hWnd, ATL_IDM_MENU_DONE, SHCMBF_HMENU);
		else
			hMenuBar = ::SHFindMenuBar(pT->m_hWnd);

		if(hMenuBar != NULL)
			::SendMessage(hMenuBar, SHCMBM_OVERRIDEKEY, VK_TBACK,
			              MAKELPARAM(SHMBOF_NODEFAULT | SHMBOF_NOTIFY, SHMBOF_NODEFAULT | SHMBOF_NOTIFY));

		SetStaticBold();
#endif
	}

// Shell dialog layout initialization
	void StdShidInit()
	{
		T* pT = static_cast<T*>(this);
		SHINITDLGINFO shidi = { SHIDIM_FLAGS, pT->m_hWnd, t_shidiFlags };
		::SHInitDialog(&shidi);
	}

// IDC_INFOSTATIC background setting
	LRESULT OnColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (::GetDlgCtrlID((HWND)lParam) == IDC_INFOSTATIC)
		{
			::SetBkMode((HDC)wParam, TRANSPARENT);
			return (LRESULT)::GetSysColorBrush(COLOR_INFOBK);
		}
		return bHandled = FALSE;
	}

// Standard dialog ending: may be used with any command
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		if (t_bModal)
			::EndDialog(pT->m_hWnd, wID);
		else
			pT->DestroyWindow();
		return 0;
	}
};


///////////////////////////////////////////////////////////////////////////////
// CStdDialogImpl - implementation of standard PPC/SmartPhone dialog

template <class T, UINT t_shidiFlags = WTL_STD_SHIDIF, bool t_bModal = true>
class ATL_NO_VTABLE CStdDialogImpl :
		public ATL::CDialogImpl< T >,
		public CStdDialogBase<T, t_shidiFlags, t_bModal>
{
public:
	BEGIN_MSG_MAP(CStdDialogImpl)
#ifdef WIN32_PLATFORM_PSPC // Pocket PC title
		MESSAGE_HANDLER(WM_PAINT, OnPaintTitle)
#elif defined(WIN32_PLATFORM_WFSP) // SmartPhone VK_TBACK key
		MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
		COMMAND_RANGE_HANDLER(ID_MENU_OK, ID_MENU_CANCEL, OnMenuClose)
#endif
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorStatic)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_RANGE_HANDLER(IDOK, IDCANCEL, OnCloseCmd)
	END_MSG_MAP()
	
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
#ifdef _DEBUG // _DEBUG must be defined before atlwin.h inclusion
		T* pT = static_cast<T*>(this);
		ATLASSERT(t_bModal == pT->m_bModal);
#endif
		StdPlatformInit();
		StdShidInit();
		return bHandled = FALSE;
	}
};


///////////////////////////////////////////////////////////////////////////////
// CStdSimpleDialog - standard PPC/SmartPhone simple dialog with SHIDIF_xxx flags

template <WORD t_wDlgTemplateID, UINT t_shidiFlags = WTL_STD_SHIDIF>
class CStdSimpleDialog :
		public ATL::CSimpleDialog<t_wDlgTemplateID, FALSE>,
		public CStdDialogBase<CStdSimpleDialog<t_wDlgTemplateID, t_shidiFlags>, t_shidiFlags>
{
public:
	typedef CStdDialogBase<CStdSimpleDialog<t_wDlgTemplateID, t_shidiFlags>, t_shidiFlags> baseClass;

	BEGIN_MSG_MAP(CStdSimpleDialog)
#ifdef WIN32_PLATFORM_PSPC // Pocket PC title
		MESSAGE_HANDLER(WM_PAINT, OnPaintTitle)
#elif defined(WIN32_PLATFORM_WFSP) // SmartPhone VK_TBACK key
		MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
		COMMAND_RANGE_HANDLER(ID_MENU_OK, ID_MENU_CANCEL, OnMenuClose)
#endif
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorStatic)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_RANGE_HANDLER(IDOK, IDCANCEL, baseClass::OnCloseCmd)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		StdPlatformInit();
		StdShidInit();
		return bHandled = FALSE;
	}
};


///////////////////////////////////////////////////////////////////////////////
// CStdDialogResizeBase - base class for orientation aware standard PPC dialogs

template <class T, UINT t_shidiFlags, bool t_bModal = true>
class CStdDialogResizeBase :
		public CStdDialogBase<T, t_shidiFlags, t_bModal>,
		public CDialogResize< T >
{
public:
	// Note: BEGIN_DLGRESIZE_MAP is required in the derived class.
};


///////////////////////////////////////////////////////////////////////////////
// CStdDialogResizeImpl - implementation of orientation aware standard PPC dialog

template <class T, UINT t_shidiFlags = WTL_STD_SHIDIF, bool t_bModal = true>
class ATL_NO_VTABLE CStdDialogResizeImpl :
		public ATL::CDialogImpl< T >,
		public CStdDialogResizeBase<T, t_shidiFlags, t_bModal>
{
public:

	BEGIN_MSG_MAP(CStdResizeDialogImpl)
#ifdef WIN32_PLATFORM_PSPC // Pocket PC title
		MESSAGE_HANDLER(WM_PAINT, OnPaintTitle)
#elif defined(WIN32_PLATFORM_WFSP) // SmartPhone VK_TBACK key
		MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
		COMMAND_RANGE_HANDLER(ID_MENU_OK, ID_MENU_CANCEL, OnMenuClose)
#endif
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorStatic)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_RANGE_HANDLER(IDOK, IDCANCEL, OnCloseCmd)
		CHAIN_MSG_MAP(CDialogResize< T >)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
#ifdef _DEBUG // _DEBUG must be defined before atlwin.h inclusion
		T* pT = static_cast<T*>(this);
		ATLASSERT(t_bModal == pT->m_bModal);
#endif
		StdPlatformInit();
		DlgResize_Init(FALSE);
		StdShidInit();
		return bHandled = FALSE;
	}
};


///////////////////////////////////////////////////////////////////////////////
// CStdSimpleDialogResizeImpl - implementation of standard PPC resizing simple dialog with SHIDIF_xxx flags

// Usage:
//	class CMyDlg : public CStdSimpleDialogResize<CMyDlg,
//		IDD_MYDLG, SHIDIF_DONEBUTTON | SHIDIF_FULLSCREENNOMENUBAR>
//	{
//	public:
//		BEGIN_DLGRESIZE_MAP(CMyDlg)
//		...
//		END_DLGRESIZE_MAP()
//	};

template <class T, WORD t_wDlgTemplateID, UINT t_shidiFlags = WTL_STD_SHIDIF>
class ATL_NO_VTABLE CStdSimpleDialogResizeImpl :
		public ATL::CSimpleDialog<t_wDlgTemplateID, FALSE>,
		public CStdDialogResizeBase<T, t_shidiFlags>
{
public:
	typedef CStdDialogResizeBase<T, t_shidiFlags> baseClass;

	BEGIN_MSG_MAP(CStdSimpleDialogResizeImpl)
#ifdef WIN32_PLATFORM_PSPC // Pocket PC title
		MESSAGE_HANDLER(WM_PAINT, OnPaintTitle)
#elif defined(WIN32_PLATFORM_WFSP) // SmartPhone VK_TBACK key
		MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
		COMMAND_RANGE_HANDLER(ID_MENU_OK, ID_MENU_CANCEL, OnMenuClose)
#endif
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorStatic)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_RANGE_HANDLER(IDOK, IDCANCEL, baseClass::OnCloseCmd)
		CHAIN_MSG_MAP(CDialogResize< T >)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		StdPlatformInit();
		DlgResize_Init(FALSE);
		StdShidInit();
		return bHandled = FALSE;
	}
};

#if defined(_WTL_CE_DRA) && defined(WIN32_PLATFORM_PSPC)

///////////////////////////////////////////////////////////////////////////////
// CStdOrientedDialogBase - Orientable dialog base class

template <class T, UINT t_shidiFlags, bool t_bModal = true>
class CStdOrientedDialogBase : public CStdDialogBase<T, t_shidiFlags, t_bModal>
{
public:
// Operation
	BOOL SetOrientation(DRA::DisplayMode mode)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(pT->IsWindow());
		ATLASSERT(mode == DRA::GetDisplayMode());
		
		// Derived dialog must enumerate TWO dialog templates with the same control ids and types ie:
		// enum { IDD = IDD_MYDLG, IDD_LANDSCAPE = IDD_MYDLG_L };
		UINT iResource = (mode == DRA::Landscape)? T::IDD_LANDSCAPE : T::IDD;

#if (_ATL_VER >= 0x0700)
		BOOL bRes = DRA::RelayoutDialog(ATL::_AtlBaseModule.GetResourceInstance(), pT->m_hWnd, MAKEINTRESOURCE(iResource));
#else // !(_ATL_VER >= 0x0700)
		BOOL bRes = DRA::RelayoutDialog(_Module.GetResourceInstance(), pT->m_hWnd, MAKEINTRESOURCE(iResource));
#endif // !(_ATL_VER >= 0x0700)
		pT->OnOrientation(mode);
		return bRes;
	}

// Override
	void OnOrientation(DRA::DisplayMode /*mode*/)
	{}

// Message handlers
	LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (wParam & SETTINGCHANGE_RESET)
		{
			SetOrientation(DRA::GetDisplayMode());
			DialogTitleInit();
			StdShidInit();
		}
		return bHandled = FALSE;
	}
};


///////////////////////////////////////////////////////////////////////////////
// CStdOrientedDialogImpl - Orientable dialog implementation

template <class T, UINT t_shidiFlags = WTL_STD_SHIDIF, bool t_bModal = true>
class ATL_NO_VTABLE CStdOrientedDialogImpl :
		public ATL::CDialogImpl< T >,
		public CStdOrientedDialogBase<T, t_shidiFlags, t_bModal>
{
public:
	BEGIN_MSG_MAP(CStdOrientedDialogImpl)
		MESSAGE_HANDLER(WM_PAINT, OnPaintTitle)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorStatic)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_RANGE_HANDLER(IDOK, IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
#ifdef _DEBUG // _DEBUG must be defined before atlwin.h inclusion
		T* pT = static_cast<T*>(this);
		ATLASSERT(t_bModal == pT->m_bModal);
#endif
		if (DRA::GetDisplayMode() == DRA::Landscape)
			SetOrientation(DRA::Landscape);
		DialogTitleInit();
		StdShidInit();
		return bHandled = FALSE;
	}
};


///////////////////////////////////////////////////////////////////////////////
// CStdSimpleOrientedDialog - Standard simple orientable dialog

template <WORD t_wDlgTemplateID, WORD t_wDlgLandscapeID, UINT t_shidiFlags = WTL_STD_SHIDIF>
class CStdSimpleOrientedDialog :
		public ATL::CSimpleDialog<t_wDlgTemplateID, FALSE>,
		public CStdOrientedDialogBase<CStdSimpleOrientedDialog<t_wDlgTemplateID, t_wDlgLandscapeID, t_shidiFlags>, t_shidiFlags>
{
public:
	typedef CStdOrientedDialogBase<CStdSimpleOrientedDialog<t_wDlgTemplateID, t_wDlgLandscapeID, t_shidiFlags>, t_shidiFlags> baseClass;
	enum {IDD = t_wDlgTemplateID, IDD_LANDSCAPE = t_wDlgLandscapeID};

	BEGIN_MSG_MAP(CStdSimpleDialog)
		MESSAGE_HANDLER(WM_PAINT, OnPaintTitle)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorStatic)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_RANGE_HANDLER(IDOK, IDCANCEL, baseClass::OnCloseCmd)
	END_MSG_MAP()

		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (DRA::GetDisplayMode() == DRA::Landscape)
			SetOrientation(DRA::Landscape);
		DialogTitleInit();
		StdShidInit();
		return bHandled = FALSE;
	}
};

#endif // _WTL_CE_DRA

#endif // _WTL_CE_NO_DIALOGS


// --- PPC/SmartPhone application window and helpers ---

#ifndef _WTL_CE_NO_APPWINDOW

///////////////////////////////////////////////////////////////////////////////
// CAppInfoBase - Helper for application state save/restore to registry

class CAppInfoBase
{
public:
	ATL::CRegKey m_Key;

	CAppInfoBase(ATL::_U_STRINGorID sAppKey)
	{
		m_Key.Create(HKEY_CURRENT_USER, sAppKey.m_lpstr);
		ATLASSERT(m_Key.m_hKey);
	}

	template <class V>
	LONG Save(V& val, ATL::_U_STRINGorID sName)
	{
		return ::RegSetValueEx(m_Key, sName.m_lpstr, 0, REG_BINARY, (LPBYTE)&val, sizeof(V));
	}

	template <class V>
	LONG Save(int nb, V& val0, ATL::_U_STRINGorID sName)
	{
		return ::RegSetValueEx(m_Key, sName.m_lpstr, 0, REG_BINARY, (LPBYTE)&val0, nb * sizeof(V));
	}

	template <class V>
	LONG Restore(V& val, ATL::_U_STRINGorID sName)
	{
		DWORD valtype;
		DWORD bufSize = sizeof(V);
		return ::RegQueryValueEx(m_Key, sName.m_lpstr, 0, &valtype, (LPBYTE)&val, &bufSize);
	}

	template <class V>
	LONG Restore(int nb, V& val0, ATL::_U_STRINGorID sName)
	{
		DWORD valtype;
		DWORD bufSize = nb * sizeof(V);
		return ::RegQueryValueEx(m_Key, sName.m_lpstr, 0, &valtype, (LPBYTE)&val0, &bufSize);
	}

#if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
#if _ATL_VER < 0x800
	LONG Save(CString& sval, ATL::_U_STRINGorID sName)
	{
		return m_Key.SetValue(sval, sName.m_lpstr);
	}

	LONG Restore(CString& sval, ATL::_U_STRINGorID sName)
	{
		DWORD size = MAX_PATH;
		LONG res = m_Key.QueryValue(sval.GetBuffer(size), sName.m_lpstr, &size);
		sval.ReleaseBuffer();
		return res;
	}
#else
	LONG Save(CString& sval, ATL::_U_STRINGorID sName)
	{
		return m_Key.SetStringValue(sName.m_lpstr, sval/*,REG_SZ*/);
	}

	LONG Restore(CString& sval, ATL::_U_STRINGorID sName)
	{
		DWORD size = MAX_PATH;
		LONG res = m_Key.QueryStringValue(sName.m_lpstr, sval.GetBuffer(size), &size);
		sval.ReleaseBuffer();
		return res;
	}
#endif // _ATL_VER < 0x800
#else
  #pragma message("Warning: CAppInfoBase compiles without CString support. Do not use CString in Save or Restore.")
#endif // defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
	
#if _ATL_VER < 0x800
	LONG Save(LPCTSTR sval, ATL::_U_STRINGorID sName)
	{
		return m_Key.SetValue(sval, sName.m_lpstr);
	}

	LONG Restore(LPTSTR sval, ATL::_U_STRINGorID sName, DWORD *plength)
	{
		return m_Key.QueryValue(sval, sName.m_lpstr, plength);
	}
#else
	LONG Save(LPCTSTR sval, ATL::_U_STRINGorID sName)
	{
		return m_Key.SetStringValue(sName.m_lpstr, sval);
	}

	LONG Restore(LPTSTR sval, ATL::_U_STRINGorID sName, DWORD *plength)
	{
		return m_Key.QueryStringValue(sName.m_lpstr, sval, plength);
	}
#endif // !_ATL_VER < 0x800
	
	LONG Delete(ATL::_U_STRINGorID sName)
	{
		return  m_Key.DeleteValue(sName.m_lpstr);
	}
};


///////////////////////////////////////////////////////////////////////////////
// CAppInfoT - CAppInfoBase constructed from a class with T::GetAppKey() 

// Macro for declaring AppKey
#define DECLARE_APPKEY(uAppKey) \
	static LPCTSTR GetAppKey() \
	{ \
		static LPCTSTR sAppKey = ATL::_U_STRINGorID(uAppKey).m_lpstr; \
		return sAppKey; \
	}

template <class T>
class CAppInfoT : public CAppInfoBase
{
public:
	CAppInfoT() : CAppInfoBase(T::GetAppKey()){}
};


///////////////////////////////////////////////////////////////////////////////
// CAppWindowBase - Base class for PPC/SmartPhone "well-behaved" application window or dialog

// Macros for declaring frame WNDCLASS and AppKey
#define DECLARE_APP_FRAME_CLASS(WndClassName, uCommonResourceID, uAppKey) \
	DECLARE_FRAME_WND_CLASS(WndClassName, uCommonResourceID) \
	DECLARE_APPKEY(uAppKey)

#define DECLARE_APP_FRAME_CLASS_EX(WndClassName, uCommonResourceID, style, bkgnd, uAppKey) \
	DECLARE_FRAME_WND_CLASS_EX(WndClassName, uCommonResourceID, style, bkgnd) \
	DECLARE_APPKEY(uAppKey)

template <class T>
class CAppWindowBase
{
public:
	typedef class CAppInfoT< T > CAppInfo;

#ifndef WIN32_PLATFORM_WFSP
	SHACTIVATEINFO m_sai; // NoOp on SmartPhones
#endif // WIN32_PLATFORM_WFSP

	bool m_bHibernate;

	CAppWindowBase< T >() : m_bHibernate(false)
	{
#ifndef WIN32_PLATFORM_WFSP
		SHACTIVATEINFO sai = { sizeof(SHACTIVATEINFO) };
		m_sai = sai;
#endif // WIN32_PLATFORM_WFSP
	};

	// Same as WTL 7.1 AppWizard generated ActivatePreviousInstance + SendMessage WM_COPYDATA
	static HRESULT ActivatePreviousInstance(HINSTANCE hInstance, LPCTSTR  lpstrCmdLine, bool bDialog)
	{
		// requires T does DECLARE_APP_FRAME_CLASS, DECLARE_APP_FRAME_CLASS_EX or DECLARE_APP_DLG_CLASS
		CFrameWndClassInfo& classInfo = T::GetWndClassInfo();

		ATLVERIFY(::LoadString(hInstance, classInfo.m_uCommonResourceID, classInfo.m_szAutoName, sizeof(classInfo.m_szAutoName)/sizeof(classInfo.m_szAutoName[0])) != 0);

		classInfo.m_wc.lpszClassName = classInfo.m_szAutoName;

		const TCHAR* pszClass = classInfo.m_wc.lpszClassName;

		if(NULL == pszClass || '\0' == *pszClass)
		{
			return E_FAIL;
		}

		const DWORD dRetryInterval = 100;
		const int iMaxRetries = 25;

		for(int i = 0; i < iMaxRetries; ++i)
		{
			HANDLE hMutex = CreateMutex(NULL, FALSE, pszClass);

			DWORD dw = GetLastError();

			if(NULL == hMutex)
			{
				HRESULT hr;

				switch(dw)
				{
				case ERROR_INVALID_HANDLE:
					// A non-mutext object with this name already exists.
					hr = E_INVALIDARG;
					break;
				default:
					// This should never happen...
					hr = E_FAIL;
				}

				return hr;
			}

			// If the mutex already exists, then there should be another instance running
			if(dw == ERROR_ALREADY_EXISTS)
			{
				CloseHandle(hMutex);
				
				HWND hwnd = NULL;
				if (bDialog)
					hwnd = FindWindow(NULL, pszClass);
				else
					hwnd = FindWindow(pszClass, NULL);

				if(hwnd == NULL)
				{
					Sleep(dRetryInterval);
					continue;
				}
				else
				{
					// Transmit our params to previous instance
					if (lpstrCmdLine && *lpstrCmdLine)
					{
						COPYDATASTRUCT cd = { NULL, sizeof(TCHAR) * (wcslen(lpstrCmdLine) + 1), (PVOID)lpstrCmdLine };
						::SendMessage(hwnd, WM_COPYDATA, NULL, (LPARAM)&cd);
					}
					// Set the previous instance as the foreground window
					if(0 != SetForegroundWindow(reinterpret_cast<HWND>(reinterpret_cast<ULONG>(hwnd) | 0x1)))
						return S_FALSE;
				}
			}
			else
			{
				return S_OK;
			}
		}
		return S_OK;
	}

// Operations overriden in derived class
	bool AppHibernate(bool /*bHibernate*/)
	{
		return false;
	}

	bool AppNewInstance(LPCTSTR /*lpstrCmdLine*/)
	{
		return false;
	}

	void AppSave()
	{
	}

// Message map and handlers
	BEGIN_MSG_MAP(CAppWindowBase)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
#ifndef WIN32_PLATFORM_WFSP
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
#endif // WIN32_PLATFORM_WFSP
		MESSAGE_HANDLER(WM_HIBERNATE, OnHibernate)
		MESSAGE_HANDLER(WM_COPYDATA, OnNewInstance)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
	END_MSG_MAP()

	LRESULT OnActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		if (m_bHibernate)
			m_bHibernate = pT->AppHibernate(false);
#ifndef WIN32_PLATFORM_WFSP
		::SHHandleWMActivate(pT->m_hWnd, wParam, lParam, &m_sai, 0);
#else
		wParam;
		lParam;
#endif // WIN32_PLATFORM_WFSP
		 return bHandled = FALSE;
	}

#ifndef WIN32_PLATFORM_WFSP
	LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		bHandled = FALSE;
		return ::SHHandleWMSettingChange(pT->m_hWnd, wParam, lParam, &m_sai);
	}
#endif // WIN32_PLATFORM_WFSP

	LRESULT OnHibernate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		return m_bHibernate = pT->AppHibernate(true);
	}

	LRESULT OnNewInstance(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;
		return pT->AppNewInstance((LPCTSTR)pcds->lpData);
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		pT->AppSave();
		bHandled = FALSE;
		return 1;
	}
};


///////////////////////////////////////////////////////////////////////////////
// CAppWindow - PPC/SmartPhone "well-behaved" application window class

template <class T>
class CAppWindow : public CAppWindowBase< T >
{
public:
	// Same as WTL 7.1 AppWizard generated Run + lpstrCmdLine in CreateEx
	static int AppRun(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWNORMAL)
	{
		CMessageLoop theLoop;
		_Module.AddMessageLoop(&theLoop);

		T wndMain;

		if(wndMain.CreateEx(NULL, NULL, 0, 0, lpstrCmdLine) == NULL)
		{
			ATLTRACE2(atlTraceUI, 0, _T("Main window creation failed!\n"));
			return 0;
		}

		wndMain.ShowWindow(nCmdShow);

		int nRet = theLoop.Run();

		_Module.RemoveMessageLoop();
		return nRet;
	}

	static HRESULT ActivatePreviousInstance(HINSTANCE hInstance, LPCTSTR  lpstrCmdLine)
	{
		return CAppWindowBase< T >::ActivatePreviousInstance(hInstance, lpstrCmdLine, false);
	}
};


#ifndef _WTL_CE_NO_DIALOGS

///////////////////////////////////////////////////////////////////////////////
// CAppDialog - PPC/SmartPhone "well-behaved" non-modal dialog application class

// Macro for declaring dialog WNDCLASS and AppKey
#define DECLARE_APP_DLG_CLASS(WndClassName, uCommonResourceID, uAppKey) \
	static WTL::CFrameWndClassInfo& GetWndClassInfo() \
	{ \
		static WTL::CFrameWndClassInfo wc = \
		{ \
			{ 0, (WNDPROC)StartDialogProc, \
			0, 0, NULL, NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1), NULL, WndClassName }, \
			NULL, NULL, IDC_ARROW, TRUE, 0, _T(""), uCommonResourceID \
		}; \
		return wc; \
	}; \
	DECLARE_APPKEY(uAppKey)

template <class T>
class CAppDialog : public CAppWindowBase< T >
{
public:
	static int AppRun(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWNORMAL)
	{
		CMessageLoop theLoop;
		_Module.AddMessageLoop(&theLoop);

		T dlgMain;

		if(dlgMain.Create(NULL, (LPARAM)lpstrCmdLine) == NULL)
		{
			ATLTRACE2(atlTraceUI, 0, _T("Main dialog creation failed!\n"));
			return 0;
		}

		dlgMain.ShowWindow(nCmdShow);

		int nRet = theLoop.Run();

		_Module.RemoveMessageLoop();
		return nRet;
	}

	static HRESULT ActivatePreviousInstance(HINSTANCE hInstance, LPCTSTR  lpstrCmdLine)
	{
		return CAppWindowBase< T >::ActivatePreviousInstance(hInstance, lpstrCmdLine, true);
	};
};


///////////////////////////////////////////////////////////////////////////////
// CAppStdDialogImpl - PPC/SmartPhone implementation of non-modal standard dialog application

#ifdef WIN32_PLATFORM_WFSP
#define WTL_APP_SHIDIF WTL_SP_SHIDIF
#else
#define WTL_APP_SHIDIF WTL_STD_SHIDIF
#endif

template <class T, UINT t_shidiFlags = WTL_APP_SHIDIF>
class ATL_NO_VTABLE CAppStdDialogImpl :
		public ATL::CDialogImpl< T >,
		public CStdDialogBase<T, t_shidiFlags, false>, 
		public CAppDialog< T >
{
public:

	BEGIN_MSG_MAP(CAppStdDialogImpl)
#if defined(WIN32_PLATFORM_WFSP) // SmartPhone VK_TBACK key
		MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
		COMMAND_RANGE_HANDLER(ID_MENU_OK, ID_MENU_CANCEL, OnMenuClose)
#endif
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorStatic)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_RANGE_HANDLER(IDOK, IDCANCEL, OnCloseCmd)
		CHAIN_MSG_MAP(CAppDialog< T >)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
#ifdef WIN32_PLATFORM_WFSP
		StdPlatformInit();
#endif
		StdShidInit();
		return bHandled = FALSE;
	}
};

#endif // _WTL_CE_NO_DIALOGS

#endif // _WTL_CE_NO_APPWINDOW


// --- Full screen support ---

#ifndef _WTL_CE_NO_FULLSCREEN

///////////////////////////////////////////////////////////////////////////////
// CFullScreenFrame - PPC full screen frame implementation

#ifdef WIN32_PLATFORM_PSPC // Pocket PC code

template <class T, bool t_bHasSip = true>
class CFullScreenFrame
{
public:
	bool m_bFullScreen;

	CFullScreenFrame() : m_bFullScreen(false)
	{ }

// Operation	
	void SetFullScreen(bool bFull)
	{
		m_bFullScreen = bFull;
		ShowTaskBar(!bFull, false);
		ShowMenuBar(!bFull);
	}

// Manage TaskBar for modal dialogs and property sheets
	template <class D>
	int FSDoModal(D& dlg)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(pT->IsWindow());
		if (m_bFullScreen)   // Show taskbar if hidden
			ShowTaskBar(true, false);
		int iRet = dlg.DoModal();
		if (m_bFullScreen)   // Hide taskbar if restored
			ShowTaskBar(false);
		return iRet;
	}

// Implementation
	void ShowMenuBar(bool bShow)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(pT->IsWindow());
		ATL::CWindow MenuBar = pT->m_hWndCECommandBar;
		ATLASSERT(MenuBar.IsWindow());
		MenuBar.ShowWindow(bShow ? SW_SHOWNORMAL : SW_HIDE);
		pT->SizeToMenuBar();
	}
	
	void ShowTaskBar(bool bShow, bool bRepaint = true)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(pT->IsWindow());
		RECT rect = { 0 };
		SystemParametersInfo(SPI_GETWORKAREA, NULL, &rect, FALSE);
		if (!bShow)
			rect.top = 0;

		UINT uShow = t_bHasSip ? SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON : SHFS_SHOWTASKBAR | SHFS_HIDESIPBUTTON;		
		SHFullScreen(pT->m_hWnd, bShow ? uShow : SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON);

		pT->MoveWindow(&rect, bRepaint);
	}

// Message map and handler
	BEGIN_MSG_MAP(CFullScreenFrame)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
	END_MSG_MAP()

	LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
#ifndef SETTINGCHANGE_RESET // not defined for PPC 2002
	#define SETTINGCHANGE_RESET SPI_SETWORKAREA
#endif
		if (m_bFullScreen && (wParam & SETTINGCHANGE_RESET))
			SetFullScreen(m_bFullScreen);
		return bHandled = FALSE;
	}
};

#endif // WIN32_PLATFORM_PSPC

#endif // _WTL_CE_NO_FULLSCREEN


// --- WinCE zoom support ---

#ifndef _WTL_CE_NO_ZOOMSCROLL

///////////////////////////////////////////////////////////////////////////////
// CZoomScrollImpl - WinCE zooming implementation on top of CScrollImpl

template <class T>
class  CZoomScrollImpl: public CScrollImpl< T >
{
public:
// Data members
	CSize m_sizeTrue;
	double	m_fzoom;

// Creation
	CZoomScrollImpl() : m_sizeTrue(0), m_fzoom(1.)
	{ }

// Zoom operations and access
	void SetZoomScrollSize(CSize sizeTrue, double fzoom = 1., BOOL bRedraw = TRUE)
	{
		ATLASSERT(fzoom > 0.);
		m_sizeTrue = sizeTrue;
		m_fzoom = fzoom;

		CScrollImpl< T >::SetScrollSize(sizeTrue / fzoom, bRedraw);
	}

	void SetZoomScrollSize(int cx, int cy, double fzoom=1., BOOL bRedraw = TRUE)
	{
		SetZoomScrollSize(CSize(cx, cy), fzoom, bRedraw);
	}

	void SetZoom(double fzoom, BOOL bRedraw = TRUE)
	{
		CPoint ptCenter = WndtoTrue(m_sizeClient / 2);
		CSize sizePage = GetScrollPage();
		CSize sizeLine = GetScrollLine();

		SetZoomScrollSize(GetScrollSize(), fzoom, bRedraw);

		SetScrollLine(sizeLine);
		SetScrollPage(sizePage);
		CPoint ptOffset = ptCenter - (m_sizeClient / 2) * fzoom;
		SetScrollOffset(ptOffset, bRedraw);
	}

	double GetZoom()
	{
		return m_fzoom;
	}

// CScrollImpl overrides
	void SetScrollOffset(int x, int y, BOOL bRedraw = TRUE)
	{
		CScrollImpl< T >::SetScrollOffset((int)(x / m_fzoom), (int)(y / m_fzoom), bRedraw);
	}

	void SetScrollOffset(POINT ptOffset, BOOL bRedraw = TRUE)
	{
		SetScrollOffset(ptOffset.x, ptOffset.y, bRedraw);
	}

	void GetScrollOffset(POINT& ptOffset)
	{
		ptOffset.x = (LONG)(m_ptOffset.x * m_fzoom);
		ptOffset.y = (LONG)(m_ptOffset.y * m_fzoom);
	}

	void SetScrollSize(int cx, int cy, BOOL bRedraw = TRUE)
	{
		SetZoomScrollSize(cx, cy, GetZoom(), bRedraw);
	}

	void SetScrollSize(SIZE sizeTrue, BOOL bRedraw = TRUE)
	{
		SetZoomScrollSize(sizeTrue, GetZoom(), bRedraw);
	}

	void GetScrollSize(SIZE& sizeTrue) const
	{
		sizeTrue = m_sizeTrue;
	}

	void SetScrollPage(int cxPage, int cyPage)
	{
		SetScrollPage(CSize(cxPage, cyPage));
	}

	void SetScrollPage(SIZE sizePage)
	{
		CScrollImpl< T >::SetScrollPage(sizePage / m_fzoom);
	}

	void GetScrollPage(SIZE& sizePage) const
	{
		sizePage = m_sizePage * m_fzoom;
	}

	void SetScrollLine(int cxLine, int cyLine)
	{
		SetScrollLine(CSize(cxLine, cyLine));
	}

	void SetScrollLine(SIZE sizeLine)
	{
		CScrollImpl< T >::SetScrollLine(sizeLine / m_fzoom);
	}

	void GetScrollLine(SIZE& sizeLine) const
	{
		sizeLine = m_sizeLine * m_fzoom;
	}

// Data access complements
	CSize GetScrollSize()
	{
		return m_sizeTrue;
	}

	CSize GetScrollPage()
	{
		return m_sizePage * m_fzoom;
	}

	CSize GetScrollLine()
	{
		return m_sizeLine * m_fzoom;
	}

	CPoint GetScrollOffset()
	{
		return (CSize)m_ptOffset * m_fzoom;
	}

// Helper coordinate functions
	CPoint WndtoTrue(CPoint ptW)
	{
		return (CSize)ptW * GetZoom() + GetScrollOffset();
	}

	void WndtoTrue(LPPOINT aptW, int nPts)   // in place coord transformation
	{
		for (int i = 0 ; i < nPts ; i++)
			aptW[i] = WndtoTrue(aptW[i]);
	}

	void WndtoTrue(LPRECT prectW)   // in place coord transformation
	{
		WndtoTrue((LPPOINT)prectW, 2);
	}

	CPoint TruetoWnd(CPoint ptT)
	{
		return (ptT - GetScrollOffset()) / GetZoom();
	}

	void TruetoWnd(LPPOINT aptT, int nPts)   // in place coord transformation
	{
		for (int i = 0 ; i < nPts ; i++)
			aptT[i] = TruetoWnd(aptT[i]);
	}

	void TruetoWnd(LPRECT prectT)   // in place coord transformation
	{
		TruetoWnd((LPPOINT)prectT, 2);
	}

// Drawing operations : assume adequate setting of data members
	BOOL Draw(HBITMAP hbm, HDC hdestDC, DWORD dwROP = SRCCOPY)
	{
		CDC memDC = CreateCompatibleDC(hdestDC);
		CBitmapHandle bmpOld = memDC.SelectBitmap(hbm);
		BOOL bRes = Draw(memDC, hdestDC, dwROP);
		memDC.SelectBitmap(bmpOld);
		return bRes;
	}

	BOOL Draw(HDC hsourceDC, HDC hdestDC, DWORD dwROP = SRCCOPY)
	{
		CDCHandle destDC = hdestDC;
		destDC.SetViewportOrg(0,0);
		CPoint ptOffset = GetScrollOffset();
		CSize sizeZClient = m_sizeClient * GetZoom();
		return destDC.StretchBlt(0, 0, m_sizeClient.cx, m_sizeClient.cy, hsourceDC, ptOffset.x, ptOffset.y, sizeZClient.cx, sizeZClient.cy, dwROP);
	}

// Message map and handlers
	BEGIN_MSG_MAP(CZoomScrollImpl< T >)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		CHAIN_MSG_MAP(CScrollImpl< T >)
	END_MSG_MAP()
	
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		if ((GetScrollExtendedStyle() & SCRL_ERASEBACKGROUND))
		{
			CRect rect;
			pT->GetClientRect(rect);
			CSize sizeClient=rect.Size();

			if (m_sizeAll.cx < sizeClient.cx || m_sizeAll.cy < sizeClient.cy)
			{
				CDCHandle hdc = (HDC)wParam;
				HBRUSH hbr = GetSysColorBrush((int)T::GetWndClassInfo().m_wc.hbrBackground - 1);

				if (m_sizeAll.cx < sizeClient.cx)
				{
					CRect rectBG(CPoint(m_sizeAll.cx, 0), sizeClient);
					hdc.FillRect(rectBG, hbr);
				}

				if (m_sizeAll.cy < sizeClient.cy)
				{
					CRect rectBG(CPoint(0, m_sizeAll.cy), sizeClient);
					hdc.FillRect(rectBG, hbr);
				}
			}
		}
		else
		{
			bHandled = FALSE;
		}

		return 1;
 	}
};

#endif // _WTL_CE_NO_ZOOMSCROLL


// --- PPC/SmartPhone controls ---

#ifndef _WTL_CE_NO_CONTROLS

////////////////////////////////////////////////////////////////////////////////
// These are wrapper classes for the Pocket PC 2002/2003 and SmartPhone 2003 controls
// To implement a window based on a control, use following:
// Example: Implementing a window based on a Html control
//
// class CMyHtml : CWindowImpl<CMyHtml, CHtmlCtrl>
// {
// public:
//      BEGIN_MSG_MAP(CMyHtml)
//          // put your message handler entries here
//      END_MSG_MAP()
// };
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// CHtmlCtrl

template <class TBase>
class CHtmlCtrlT : public TBase
{
public:
// Constructors
	CHtmlCtrlT(HWND hWnd = NULL) : TBase(hWnd)
	{ }

	CHtmlCtrlT< TBase >& operator =(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
		HWND hWnd = TBase::Create(GetWndClassName(), hWndParent, rect.m_lpRect, szWindowName, dwStyle, dwExStyle, MenuOrID.m_hMenu, lpCreateParam);
		ATLASSERT(hWnd != NULL);   // Did you remember to call InitHTMLControl(hInstance) ??
		return hWnd;
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return WC_HTML;
	}

#if (_WIN32_WCE >= 400)
	void AddStyle(LPCWSTR pszStyle)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ADDSTYLE, 0, (LPARAM)pszStyle);
	}
#endif // (_WIN32_WCE >= 400)

	void AddText(BOOL bPlainText, LPCSTR pszText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ADDTEXT, (WPARAM)bPlainText, (LPARAM)pszText);
	}

	void AddHTML(LPCSTR pszHTML)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ADDTEXT, (WPARAM)FALSE, (LPARAM)pszHTML);
	}

	void AddText(BOOL bPlainText, LPCWSTR pszText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ADDTEXTW, (WPARAM)bPlainText, (LPARAM)pszText);
	}

	void AddHTML(LPCWSTR pszHTML)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ADDTEXTW, (WPARAM)FALSE, (LPARAM)pszHTML);
	}

	void Anchor(LPCSTR pszAnchor)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ANCHOR, 0, (LPARAM)pszAnchor);
	}

	void Anchor(LPCWSTR pszAnchor)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ANCHORW, 0, (LPARAM)pszAnchor);
	}

#if (_WIN32_WCE >= 400)
	void GetBrowserDispatch(IDispatch** ppDispatch)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(ppDispatch);
		ATLASSERT(*ppDispatch==NULL);
		::SendMessage(m_hWnd, DTM_BROWSERDISPATCH, 0, (LPARAM)ppDispatch);
	}
#endif // (_WIN32_WCE >= 400)

	void Clear()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_CLEAR, 0, 0L);
	}

	void EnableClearType(BOOL bEnable = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ENABLECLEARTYPE, 0, (LPARAM)bEnable);
	}

	void EnableContextMenu(BOOL bEnable = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ENABLECONTEXTMENU, 0, (LPARAM)bEnable);
	}

	void EnableScripting(BOOL bEnable = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ENABLESCRIPTING, 0, (LPARAM)bEnable);
	}

	void EnableShrink(BOOL bEnable = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ENABLESHRINK, 0, (LPARAM)bEnable);
	}

	void EndOfSource()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ENDOFSOURCE, 0, 0L);
	}

	void ImageFail(DWORD dwCookie)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_IMAGEFAIL, 0, (LPARAM)dwCookie);
	}

	int GetLayoutHeight() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, DTM_LAYOUTHEIGHT, 0, 0L);
	}

	int GetLayoutWidth() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, DTM_LAYOUTWIDTH, 0, 0L);
	}

	void Navigate(LPCTSTR pstrURL, UINT uFlags = 0)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(pstrURL);
		::SendMessage(m_hWnd, DTM_NAVIGATE, (WPARAM)uFlags, (LPARAM)pstrURL);
	}

	void SelectAll()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_SELECTALL, 0, 0L);
	}

	void SetImage(INLINEIMAGEINFO* pImageInfo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(pImageInfo);
		::SendMessage(m_hWnd, DTM_SETIMAGE, 0, (LPARAM)pImageInfo);
	}

	void ZoomLevel(int iLevel)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_ZOOMLEVEL, 0, (LPARAM)iLevel);
	}

#if (_WIN32_WCE >= 400)
	void Stop()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DTM_STOP, 0, 0L);
	}
#endif // (_WIN32_WCE >= 400)

	void GetScriptDispatch(IDispatch** ppDispatch)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(ppDispatch);
		ATLASSERT(*ppDispatch==NULL);
		::SendMessage(m_hWnd, DTM_SCRIPTDISPATCH, 0, (LPARAM)ppDispatch);
	}
};

typedef CHtmlCtrlT<ATL::CWindow> CHtmlCtrl;


#ifdef WIN32_PLATFORM_PSPC

///////////////////////////////////////////////////////////////////////////////
// CRichInkCtrl

template <class TBase>
class CRichInkCtrlT : public TBase
{
public:
// Constructors
	CRichInkCtrlT(HWND hWnd = NULL) : TBase(hWnd)
	{ }

	CRichInkCtrlT< TBase >& operator =(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
		HWND hWnd = TBase::Create(GetWndClassName(), hWndParent, rect.m_lpRect, szWindowName, dwStyle, dwExStyle, MenuOrID.m_hMenu, lpCreateParam);
		ATLASSERT(hWnd != NULL);   // Did you remember to call InitRichInkDLL() ??
		return hWnd;
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return WC_RICHINK;
	}

	BOOL CanPaste(UINT uFormat = 0) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_CANPASTE, (WPARAM)uFormat, 0L);
	}

	BOOL CanRedo() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_CANREDO, 0, 0L);
	}

	BOOL CanUndo() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_CANUNDO, 0, 0L);
	}

	void ClearAll(BOOL bRepaint = TRUE) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_CLEARALL, (WPARAM)bRepaint, 0L);
	}

	BOOL GetModify() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, EM_GETMODIFY, 0, 0L);
	}

	UINT GetPageStyle() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, EM_GETPAGESTYLE, 0, 0L);
	}

	UINT GetPenMode() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, EM_GETPENMODE, 0, 0L);
	}

	UINT GetViewStyle() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, EM_GETVIEW, 0, 0L);
	}

	UINT GetWrapMode() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, EM_GETWRAPMODE, 0, 0L);
	}

	UINT GetZoomPercent() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, EM_GETZOOMPERCENT, 0, 0L);
	}

	void InsertLinks(LPWSTR lpString, int cchLength = -1)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		if(cchLength == -1)
			cchLength = lstrlen(lpString);
		::SendMessage(m_hWnd, EM_INSERTLINKS, (WPARAM)cchLength, (LPARAM)lpString);
	}

	void RedoEvent()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_REDOEVENT, 0, 0L);
	}

	UINT SetInkLayer(UINT uLayer)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, EM_SETINKLAYER, (WPARAM)uLayer, 0L);
	}

	void SetPageStyle(UINT uStyle)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETPAGESTYLE, (WPARAM)uStyle, 0L);
	}

	void SetPenMode(UINT uMode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETPENMODE, (WPARAM)uMode, 0L);
	}

	void SetViewStyle(UINT uStyle)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETVIEW, (WPARAM)uStyle, 0L);
	}

	void SetViewAttributes(VIEWATTRIBUTES* pAttribs)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(pAttribs);
		::SendMessage(m_hWnd, EM_SETVIEWATTRIBUTES, 0, (LPARAM)pAttribs);
	}

	void SetWrapMode(UINT uMode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETWRAPMODE, (WPARAM)uMode, 0L);
	}

	void SetZoomPercent(UINT uPercent)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETZOOMPERCENT, (WPARAM)uPercent, 0L);
	}

	LONG StreamIn(UINT uFormat, EDITSTREAM& es)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LONG)::SendMessage(m_hWnd, EM_STREAMIN, (WPARAM)uFormat, (LPARAM)&es);
	}

	LONG StreamOut(UINT uFormat, EDITSTREAM& es)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LONG)::SendMessage(m_hWnd, EM_STREAMOUT, (WPARAM)uFormat, (LPARAM)&es);
	}

	void UndoEvent()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_UNDOEVENT, 0, 0L);
	}

// Standard EM_xxx messages
	DWORD GetSel() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (DWORD)::SendMessage(m_hWnd, EM_GETSEL, 0, 0L);
	}

	void GetSel(int& nStartChar, int& nEndChar) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_GETSEL, (WPARAM)&nStartChar, (LPARAM)&nEndChar);
	}

	void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo = FALSE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)lpszNewText);
	}

	void SetModify(BOOL bModified = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, EM_SETMODIFY, (WPARAM)bModified, 0L);
	}

	int GetTextLength() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, WM_GETTEXTLENGTH, 0, 0L);
	}

// Clipboard operations
	void Clear()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_CLEAR, 0, 0L);
	}

	void Copy()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_COPY, 0, 0L);
	}

	void Cut()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_CUT, 0, 0L);
	}

	void Paste()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_PASTE, 0, 0L);
	}
};

typedef CRichInkCtrlT<ATL::CWindow> CRichInkCtrl;


///////////////////////////////////////////////////////////////////////////////
// CInkXCtrl

template <class TBase>
class CInkXCtrlT : public TBase
{
public:
// Constructors
	CInkXCtrlT(HWND hWnd = NULL) : TBase(hWnd)
	{ }

	CInkXCtrlT< TBase >& operator =(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
		HWND hWnd = TBase::Create(GetWndClassName(), hWndParent, rect.m_lpRect, szWindowName, dwStyle, dwExStyle, MenuOrID.m_hMenu, lpCreateParam);
		ATLASSERT(hWnd != NULL);   // Did you remember to call InitInkX() ??
		return hWnd;
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return WC_INKX;
	}

	static UINT GetHotRecordingMessage()
	{
		return ::RegisterWindowMessage(szHotRecording);
	}

	void ClearAll()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, IM_CLEARALL, 0, 0L);
	}

	int GetData(BYTE* lpBuffer, INT cbBuffer) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(lpBuffer);
		return (int)::SendMessage(m_hWnd, IM_GETDATA, (WPARAM)cbBuffer, (LPARAM)lpBuffer);
	}

	int GetDataLen() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, IM_GETDATALEN, 0, 0L);
	}

	CRichInkCtrl GetRichInk() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HWND)::SendMessage(m_hWnd, IM_GETRICHINK, 0, 0L);
	}

	BOOL IsRecording() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, IM_RECORDING, 0, 0L);
	}

	void ReInit()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, IM_REINIT, 0, 0L);
	}

	void SetData(const BYTE* lpInkData, INT cbInkData)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(lpInkData);
		::SendMessage(m_hWnd, IM_SETDATA, (WPARAM)cbInkData, (LPARAM)lpInkData);
	}

	void VoicePlay()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, IM_VOICE_PLAY, 0, 0L);
	}

	BOOL IsVoicePlaying() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, IM_VOICE_PLAYING, 0, 0L);
	}

	BOOL VoiceRecord()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, IM_VOICE_RECORD, 0, 0L);
	}

	void VoiceStop()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, IM_VOICE_STOP, 0, 0L);
	}

	void ShowVoiceBar(BOOL bShow = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, IM_VOICEBAR, (WPARAM)bShow, 0L);
	}
};

typedef CInkXCtrlT<ATL::CWindow> CInkXCtrl;

#endif // WIN32_PLATFORM_PSPC


///////////////////////////////////////////////////////////////////////////////
// CVoiceRecorderCtrl

template <class TBase>
class CVoiceRecorderCtrlT : public TBase
{
public:
// Constructors
	CVoiceRecorderCtrlT(HWND hWnd = NULL) : TBase(hWnd)
	{ }

	CVoiceRecorderCtrlT< TBase >& operator =(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, const POINT pt, LPTSTR pstrFileName, UINT nID, DWORD dwStyle = 0)
	{
		ATLASSERT(pstrFileName != NULL);
		CM_VOICE_RECORDER cmvr = { 0 };
		cmvr.cb = sizeof(CM_VOICE_RECORDER);
		cmvr.dwStyle = dwStyle;
		cmvr.xPos = pt.x;
		cmvr.yPos = pt.y;
		cmvr.hwndParent = hWndParent;
		cmvr.id = nID;
		cmvr.lpszRecordFileName = pstrFileName;
		m_hWnd = VoiceRecorder_Create(&cmvr);
		return m_hWnd;
	}

	HWND Create(LPCM_VOICE_RECORDER pAttribs)
	{
		ATLASSERT(pAttribs);
		m_hWnd = VoiceRecorder_Create(pAttribs);
		return m_hWnd;
	}

// Attributes
	void Record()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, VRM_RECORD, 0, 0L);
	}

	void Play()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, VRM_PLAY, 0, 0L);
	}

	void Stop()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, VRM_STOP, 0, 0L);
	}

	void Cancel()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, VRM_CANCEL, 0, 0L);
	}

	void Done()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, VRM_OK, 0, 0L);
	}
};

typedef CVoiceRecorderCtrlT<ATL::CWindow> CVoiceRecorderCtrl;


#ifdef WIN32_PLATFORM_PSPC

///////////////////////////////////////////////////////////////////////////////
// CDocListCtrl

template <class TBase>
class CDocListCtrlT : public TBase
{
public:
// Attributes
	DOCLISTCREATE m_dlc;
	TCHAR m_szPath[MAX_PATH];

// Constructors
	CDocListCtrlT(HWND hWnd = NULL) : TBase(hWnd)
	{ }

	CDocListCtrlT< TBase >& operator =(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, WORD wId, LPCTSTR pszFolder = NULL, LPCTSTR pstrFilter = NULL,
			WORD wFilterIndex = 0, DWORD dwFlags = DLF_SHOWEXTENSION)
	{
		ATLASSERT(pstrFilter != NULL);   // It seems to need a filter badly!!
		::ZeroMemory(&m_dlc, sizeof(DOCLISTCREATE));
		::ZeroMemory(m_szPath, sizeof(m_szPath));
		if(pszFolder != NULL)
			::lstrcpyn(m_szPath, pszFolder, MAX_PATH - 1);
		m_dlc.dwStructSize = sizeof(DOCLISTCREATE);
		m_dlc.hwndParent = hWndParent;
		m_dlc.pszFolder = m_szPath;
		m_dlc.pstrFilter = pstrFilter;
		m_dlc.wFilterIndex = wFilterIndex;
		m_dlc.wId = wId;
		m_dlc.dwFlags = dwFlags;
		m_hWnd = DocList_Create(&m_dlc);
		return m_hWnd;
	}

	HWND Create(DOCLISTCREATE* pDlc)
	{
		m_dlc = *pDlc;
		m_hWnd = DocList_Create(&m_dlc);
		return m_hWnd;
	}

// Attributes
	void DeleteSel()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DLM_DELETESEL, 0, 0L);
	}

	void DisableUpdates()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DLM_DISABLEUPDATES, 0, 0L);
	}

	void EnableUpdates()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DLM_ENABLEUPDATES, 0, 0L);
	}

	int GetFilterIndex() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, DLM_GETFILTERINDEX, 0, 0L);
	}

	int GetItemCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, DLM_GETITEMCOUNT, 0, 0L);
	}

	int GetNextItem(int iIndex, DWORD dwRelation = LVNI_ALL) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, DLM_GETNEXTITEM, (WPARAM)iIndex, (LPARAM)dwRelation);
	}

	int GetFirstItem(DWORD dwRelation = LVNI_ALL) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, DLM_GETNEXTITEM, (WPARAM)-1, (LPARAM)dwRelation);
	}

	BOOL GetNextWave(int* pIndex) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(pIndex);
		return (BOOL)::SendMessage(m_hWnd, DLM_GETNEXTWAVE, 0, (LPARAM)pIndex);
	}

	BOOL GetPrevWave(int* pIndex) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(pIndex);
		return (BOOL)::SendMessage(m_hWnd, DLM_GETPREVWAVE, 0, (LPARAM)pIndex);
	}

	int GetSelCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, DLM_GETSELCOUNT, 0, 0L);
	}

	BOOL GetSelPathName(LPTSTR pstrPath, int cchMax) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(pstrPath);
		return (BOOL)::SendMessage(m_hWnd, DLM_GETSELPATHNAME, (WPARAM)cchMax, (LPARAM)pstrPath);
	}

	void ReceiveIR(LPCTSTR pstrPath) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(pstrPath);
		::SendMessage(m_hWnd, DLM_RECEIVEIR, 0, (LPARAM)pstrPath);
	}

	void Refresh()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DLM_REFRESH, 0, 0L);
	}

	BOOL RenameMoveSelectedItems()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, DLM_RENAMEMOVE, 0, 0L);
	}

	int SelectAll()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, DLM_SELECTALL, 0, 0L);
	}

	HRESULT SelectItem(LPCTSTR pstrPath, BOOL bVisible = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(pstrPath);
		return (HRESULT)::SendMessage(m_hWnd, DLM_SELECTITEM, (WPARAM)bVisible, (LPARAM)pstrPath);
	}

	void SendEMail(LPCTSTR pstrAttachment)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DLM_SENDEMAIL, 0, (LPARAM)pstrAttachment);
	}

	void SendIR(LPCTSTR pstrPath)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DLM_SENDIR, 0, (LPARAM)pstrPath);
	}

	HRESULT SetFilterIndex(int iIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HRESULT)::SendMessage(m_hWnd, DLM_SETFILTERINDEX, (WPARAM)iIndex, 0L);
	}

	void SetFolder(LPCTSTR pstrPath)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(pstrPath);
		::SendMessage(m_hWnd, DLM_SETFOLDER, 0, (LPARAM)pstrPath);
	}

	BOOL SetItemState(int iIndex, const LVITEM* pItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(pItem);
		return (BOOL)::SendMessage(m_hWnd, DLM_SETITEMSTATE, (WPARAM)iIndex, (LPARAM)pItem);
	}

	BOOL SetItemState(int iIndex, UINT uState, UINT uMask)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LV_ITEM lvi = { 0 };
		lvi.stateMask = uMask;
		lvi.state = uState;
		return (BOOL)::SendMessage(m_hWnd, DLM_SETITEMSTATE, (WPARAM)iIndex, (LPARAM)&lvi);
	}

	void SetOneItem(int iIndex, LPCVOID pPA)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DLM_SETONEITEM, (WPARAM)iIndex, (LPARAM)pPA);
	}

	void SetSelect(int iIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, DLM_SETSELECT, (WPARAM)iIndex, 0L);
	}

	void SetSelPathName(LPCTSTR pstrPath)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(pstrPath);
		::SendMessage(m_hWnd, DLM_SETSELPATHNAME, 0, (LPARAM)pstrPath);
	}

	BOOL SetSortOrder()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, DLM_SETSORTORDER, 0, 0L);
	}

	HRESULT Update()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HRESULT)::SendMessage(m_hWnd, DLM_UPDATE, 0, 0L);
	}

	BOOL ValidateFolder()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, DLM_VALIDATEFOLDER, 0, 0L);
	}

// Functions
	BOOL GetFirstSelectedWaveFile(int* pIndex, LPTSTR szPath, const size_t cchPath)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return DocList_GetFirstSelectedWaveFile(m_hWnd, pIndex, szPath, cchPath);
	}

	BOOL GetNextSelectedWaveFile(int* pIndex, LPTSTR szPath, const size_t cchPath)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return DocList_GetNextSelectedWaveFile(m_hWnd, pIndex, szPath, cchPath);
	}
};

typedef CDocListCtrlT<ATL::CWindow> CDocListCtrl;

#endif // WIN32_PLATFORM_PSPC


///////////////////////////////////////////////////////////////////////////////
// CCapEdit

template <class TBase>
class CCapEditT : public TBase
{
public:
// Constructors
	CCapEditT(HWND hWnd = NULL) : TBase(hWnd)
	{ }

	CCapEditT< TBase >& operator =(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
		HWND hWnd = /*TBase*/CWindow::Create(GetWndClassName(), hWndParent, rect.m_lpRect, szWindowName, dwStyle, dwExStyle, MenuOrID.m_hMenu, lpCreateParam);
		ATLASSERT(hWnd != NULL);   // Did you remember to call SHInitExtraControls() ??
		return hWnd;
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return WC_CAPEDIT;
	}
};

typedef CCapEditT<WTL::CEdit> CCapEdit;

///////////////////////////////////////////////////////////////////////////////
// CTTStatic

#ifndef WIN32_PLATFORM_WFSP // Tooltips not supported on SmartPhone

template <class TBase>
class CTTStaticT : public TBase
{
public:
// Constructors
	CTTStaticT(HWND hWnd = NULL) : TBase(hWnd)
	{ }

	CTTStaticT< TBase >& operator =(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
		HWND hWnd = TBase::Create(hWndParent, rect.m_lpRect, szWindowName, dwStyle, dwExStyle, MenuOrID.m_hMenu, lpCreateParam);
		ATLASSERT(hWnd != NULL);   // Did you remember to call SHInitExtraControls() ??
		return hWnd;
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return WC_TSTATIC;
	}

// Operations
	int SetToolTipText(LPCTSTR pstrTipText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(pstrTipText);
		ATLASSERT(lstrlen(pstrTipText)<= 253);
		LPTSTR pstr = (LPTSTR)_alloca((lstrlen(pstrTipText) + 3) * sizeof(TCHAR));
		::lstrcpy(pstr, _T("~~"));
		::lstrcat(pstr, pstrTipText);
		return SetWindowText(pstr);
	}
};

typedef CTTStaticT<WTL::CStatic> CTTStatic;


///////////////////////////////////////////////////////////////////////////////
// CTTButton

template <class TBase>
class CTTButtonT : public TBase
{
public:
// Constructors
	CTTButtonT(HWND hWnd = NULL) : TBase(hWnd)
	{ }

	CTTButtonT< TBase >& operator =(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
		HWND hWnd = TBase::Create(hWndParent, rect.m_lpRect, szWindowName, dwStyle, dwExStyle, MenuOrID.m_hMenu, lpCreateParam);
		ATLASSERT(hWnd != NULL);   // Did you remember to call SHInitExtraControls() ??
		return hWnd;
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return WC_TBUTTON;
	}

// Operations
	int SetToolTipText(LPCTSTR pstrTipText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(pstrTipText);
		ATLASSERT(lstrlen(pstrTipText)<= 253);
		LPTSTR pstr = (LPTSTR)_alloca((lstrlen(pstrTipText) + 3) * sizeof(TCHAR));
		::lstrcpy(pstr, _T("~~"));
		::lstrcat(pstr, pstrTipText);
		return SetWindowText(pstr);
	}
};

typedef CTTButtonT<WTL::CButton> CTTButton;

#endif // !WIN32_PLATFORM_WFSP


// --- SmartPhone specific controls ---

#ifdef WIN32_PLATFORM_WFSP

///////////////////////////////////////////////////////////////////////////////
// CSpinCtrlT - CSpinCtrl : SmartPhone adapted UpDown control

template <class TBase>
class CSpinCtrlT : public CUpDownCtrlT< TBase >
{
public:
// Constructors
	CSpinCtrlT(HWND hWnd = NULL) : CUpDownCtrlT< TBase >(hWnd)
	{ }

	CSpinCtrlT< TBase >& operator =(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, HWND hBuddy, DWORD dwStyle, int nID, LPCTSTR szExpandedName = NULL)
	{
		ATLASSERT(::IsWindow(hWndParent));
		CUpDownCtrlT< TBase >::Create(hWndParent, NULL, szExpandedName, dwStyle, 0, nID, NULL);
		ATLASSERT(m_hWnd != NULL);   // Did you remember to call AtlInitCommonControls(ICC_UPDOWN_CLASS)?
		if (hBuddy != NULL)
		{
			ATLASSERT(::IsWindow(hBuddy));
			SetBuddy(hBuddy);
		}
		return m_hWnd;
	}
};

typedef CSpinCtrlT<ATL::CWindow> CSpinCtrl;


///////////////////////////////////////////////////////////////////////////////
// CSpinned - SmartPhone association of control and Spin

template <class TBase, bool t_bExpandOnly>
class CSpinned : public TBase
{
public:
	CSpinCtrl m_SpinCtrl;
	DWORD m_dwSpinnedStyle;

// Constructors
	CSpinned(HWND hWnd = NULL) : TBase(hWnd)
	{
		m_dwSpinnedStyle = WS_VISIBLE | UDS_ALIGNRIGHT | UDS_EXPANDABLE;
		
		if (t_bExpandOnly == true)
			m_dwSpinnedStyle |= UDS_NOSCROLL;
		else
			m_dwSpinnedStyle |= UDS_HORZ | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_WRAP;

		if (hWnd != NULL)
			AttachOrCreateSpinCtrl();
	}

	CSpinned<TBase, t_bExpandOnly>& operator =(HWND hWnd)
	{
		Attach(hWnd);
		return *this;
	}

	void Attach(HWND hWnd)
	{
		ATLASSERT(!IsWindow());
		TBase* pT = static_cast<TBase*>(this);
		pT->m_hWnd = hWnd;
		if (hWnd != NULL)
			AttachOrCreateSpinCtrl();
	}

	HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCTSTR szExpandedName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{

		TBase* pT = static_cast<TBase*>(this);
		TBase::Create(hWndParent, rect, NULL, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
		ATLASSERT(pT->m_hWnd != NULL);

		m_SpinCtrl.Create(hWndParent, pT->m_hWnd, m_dwSpinnedStyle, ATL_IDW_SPIN_ID + (int)MenuOrID.m_hMenu, szExpandedName);

		ATLASSERT(m_SpinCtrl.m_hWnd != NULL);   // Did you remember to call AtlInitCommonControls(ICC_UPDOWN_CLASS)?

		return pT->m_hWnd;
	}

// Attributes
	CSpinCtrl& GetSpinCtrl()
	{
		return m_SpinCtrl;
	}

// Implementation
	// Attach our existing SpinCtrl or create one
	bool AttachOrCreateSpinCtrl()
	{
		TBase* pT = static_cast<TBase*>(this);

		HWND hSpin = ::GetDlgItem(pT->GetParent(), ATL_IDW_SPIN_ID + pT->GetDlgCtrlID());

		if (hSpin != NULL)
		{
			m_SpinCtrl.Attach(hSpin);
#ifdef DEBUG
			TCHAR sClassName[16];
			::GetClassName(hSpin, sClassName, 16);
			ATLASSERT(!_tcscmp(sClassName, UPDOWN_CLASS));
			ATLASSERT(m_SpinCtrl.GetBuddy().m_hWnd == pT->m_hWnd);
#endif // DEBUG
		}
		else
		{
			m_SpinCtrl.Create(pT->GetParent(), pT->m_hWnd, m_dwSpinnedStyle, ATL_IDW_SPIN_ID + pT->GetDlgCtrlID());
		}

		return m_SpinCtrl.m_hWnd != NULL;
	}
};


///////////////////////////////////////////////////////////////////////////////
// CSpinListBox - SmartPhone spinned ListBox control
// CExpandListBox - SmartPhone expandable ListBox control
// CExpandEdit - SmartPhone expandable Edit control
// CExpandCapEdit - SmartPhone expandable CapEdit control

typedef CSpinned<CListBox, false>   CSpinListBox;
typedef CSpinned<CListBox, true>    CExpandListBox;
typedef CSpinned<CEdit, true>       CExpandEdit;
typedef CSpinned<CCapEdit, true>    CExpandCapEdit;

#endif // WIN32_PLATFORM_WFSP

#endif // _WTL_CE_NO_CONTROLS

}; // namespace WTL

#endif // __ATLWINCE_H__
