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

#ifndef __ATLCTRLX_H__
#define __ATLCTRLX_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
	#error atlctrlx.h requires atlapp.h to be included first
#endif

#ifndef __ATLCTRLS_H__
	#error atlctrlx.h requires atlctrls.h to be included first
#endif

#ifndef WM_UPDATEUISTATE
  #define WM_UPDATEUISTATE                0x0128
#endif // !WM_UPDATEUISTATE


///////////////////////////////////////////////////////////////////////////////
// Classes in this file:
//
// CBitmapButtonImpl<T, TBase, TWinTraits>
// CBitmapButton
// CCheckListViewCtrlImpl<T, TBase, TWinTraits>
// CCheckListViewCtrl
// CHyperLinkImpl<T, TBase, TWinTraits>
// CHyperLink
// CWaitCursor
// CCustomWaitCursor
// CMultiPaneStatusBarCtrlImpl<T, TBase>
// CMultiPaneStatusBarCtrl
// CPaneContainerImpl<T, TBase, TWinTraits>
// CPaneContainer
// CSortListViewImpl<T>
// CSortListViewCtrlImpl<T, TBase, TWinTraits>
// CSortListViewCtrl

namespace WTL
{

///////////////////////////////////////////////////////////////////////////////
// CBitmapButton - bitmap button implementation

#ifndef _WIN32_WCE

// bitmap button extended styles
#define BMPBTN_HOVER		0x00000001
#define BMPBTN_AUTO3D_SINGLE	0x00000002
#define BMPBTN_AUTO3D_DOUBLE	0x00000004
#define BMPBTN_AUTOSIZE		0x00000008
#define BMPBTN_SHAREIMAGELISTS	0x00000010
#define BMPBTN_AUTOFIRE		0x00000020

template <class T, class TBase = CButton, class TWinTraits = ATL::CControlWinTraits>
class ATL_NO_VTABLE CBitmapButtonImpl : public ATL::CWindowImpl< T, TBase, TWinTraits>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

	enum
	{
		_nImageNormal = 0,
		_nImagePushed,
		_nImageFocusOrHover,
		_nImageDisabled,

		_nImageCount = 4,
	};

	enum
	{
		ID_TIMER_FIRST = 1000,
		ID_TIMER_REPEAT = 1001
	};

	// Bitmap button specific extended styles
	DWORD m_dwExtendedStyle;

	CImageList m_ImageList;
	int m_nImage[_nImageCount];

	CToolTipCtrl m_tip;
	LPTSTR m_lpstrToolTipText;

	// Internal states
	unsigned m_fMouseOver:1;
	unsigned m_fFocus:1;
	unsigned m_fPressed:1;


// Constructor/Destructor
	CBitmapButtonImpl(DWORD dwExtendedStyle = BMPBTN_AUTOSIZE, HIMAGELIST hImageList = NULL) : 
			m_ImageList(hImageList), m_dwExtendedStyle(dwExtendedStyle), 
			m_lpstrToolTipText(NULL),
			m_fMouseOver(0), m_fFocus(0), m_fPressed(0)
	{
		m_nImage[_nImageNormal] = -1;
		m_nImage[_nImagePushed] = -1;
		m_nImage[_nImageFocusOrHover] = -1;
		m_nImage[_nImageDisabled] = -1;
	}

	~CBitmapButtonImpl()
	{
		if((m_dwExtendedStyle & BMPBTN_SHAREIMAGELISTS) == 0)
			m_ImageList.Destroy();
		delete [] m_lpstrToolTipText;
	}

	// overridden to provide proper initialization
	BOOL SubclassWindow(HWND hWnd)
	{
#if (_MSC_VER >= 1300)
		BOOL bRet = ATL::CWindowImpl< T, TBase, TWinTraits>::SubclassWindow(hWnd);
#else // !(_MSC_VER >= 1300)
		typedef ATL::CWindowImpl< T, TBase, TWinTraits>   _baseClass;
		BOOL bRet = _baseClass::SubclassWindow(hWnd);
#endif // !(_MSC_VER >= 1300)
		if(bRet)
			Init();
		return bRet;
	}

// Attributes
	DWORD GetBitmapButtonExtendedStyle() const
	{
		return m_dwExtendedStyle;
	}

	DWORD SetBitmapButtonExtendedStyle(DWORD dwExtendedStyle, DWORD dwMask = 0)
	{
		DWORD dwPrevStyle = m_dwExtendedStyle;
		if(dwMask == 0)
			m_dwExtendedStyle = dwExtendedStyle;
		else
			m_dwExtendedStyle = (m_dwExtendedStyle & ~dwMask) | (dwExtendedStyle & dwMask);
		return dwPrevStyle;
	}

	HIMAGELIST GetImageList() const
	{
		return m_ImageList;
	}

	HIMAGELIST SetImageList(HIMAGELIST hImageList)
	{
		HIMAGELIST hImageListPrev = m_ImageList;
		m_ImageList = hImageList;
		if((m_dwExtendedStyle & BMPBTN_AUTOSIZE) != 0 && ::IsWindow(m_hWnd))
			SizeToImage();
		return hImageListPrev;
	}

	int GetToolTipTextLength() const
	{
		return (m_lpstrToolTipText == NULL) ? -1 : lstrlen(m_lpstrToolTipText);
	}

	bool GetToolTipText(LPTSTR lpstrText, int nLength) const
	{
		ATLASSERT(lpstrText != NULL);
		if(m_lpstrToolTipText == NULL)
			return false;
		return (lstrcpyn(lpstrText, m_lpstrToolTipText, min(nLength, lstrlen(m_lpstrToolTipText) + 1)) != NULL);
	}

	bool SetToolTipText(LPCTSTR lpstrText)
	{
		if(m_lpstrToolTipText != NULL)
		{
			delete [] m_lpstrToolTipText;
			m_lpstrToolTipText = NULL;
		}
		if(lpstrText == NULL)
		{
			if(m_tip.IsWindow())
				m_tip.Activate(FALSE);
			return true;
		}
		ATLTRY(m_lpstrToolTipText = new TCHAR[lstrlen(lpstrText) + 1]);
		if(m_lpstrToolTipText == NULL)
			return false;
		bool bRet = (lstrcpy(m_lpstrToolTipText, lpstrText) != NULL);
		if(bRet && m_tip.IsWindow())
		{
			m_tip.Activate(TRUE);
			m_tip.AddTool(m_hWnd, m_lpstrToolTipText);
		}
		return bRet;
	}

// Operations
	void SetImages(int nNormal, int nPushed = -1, int nFocusOrHover = -1, int nDisabled = -1)
	{
		if(nNormal != -1)
			m_nImage[_nImageNormal] = nNormal;
		if(nPushed != -1)
			m_nImage[_nImagePushed] = nPushed;
		if(nFocusOrHover != -1)
			m_nImage[_nImageFocusOrHover] = nFocusOrHover;
		if(nDisabled != -1)
			m_nImage[_nImageDisabled] = nDisabled;
	}

	BOOL SizeToImage()
	{
		ATLASSERT(::IsWindow(m_hWnd) && m_ImageList.m_hImageList != NULL);
		int cx = 0;
		int cy = 0;
		if(!m_ImageList.GetIconSize(cx, cy))
			return FALSE;
		return ResizeClient(cx, cy);
	}

// Overrideables
	void DoPaint(CDCHandle dc)
	{
		ATLASSERT(m_ImageList.m_hImageList != NULL);   // image list must be set
		ATLASSERT(m_nImage[0] != -1);                  // main bitmap must be set

		// set bitmap according to the current button state
		int nImage = -1;
		bool bHover = IsHoverMode();
		if(m_fPressed == 1)
			nImage = m_nImage[_nImagePushed];
		else if((!bHover && m_fFocus == 1) || (bHover && m_fMouseOver == 1))
			nImage = m_nImage[_nImageFocusOrHover];
		else if(!IsWindowEnabled())
			nImage = m_nImage[_nImageDisabled];
		if(nImage == -1)   // not there, use default one
			nImage = m_nImage[_nImageNormal];

		// draw the button image
		int xyPos = 0;
		if((m_fPressed == 1) && ((m_dwExtendedStyle & (BMPBTN_AUTO3D_SINGLE | BMPBTN_AUTO3D_DOUBLE)) != 0) && (m_nImage[_nImagePushed] == -1))
			xyPos = 1;
		m_ImageList.Draw(dc, nImage, xyPos, xyPos, ILD_NORMAL);

		// draw 3D border if required
		if((m_dwExtendedStyle & (BMPBTN_AUTO3D_SINGLE | BMPBTN_AUTO3D_DOUBLE)) != 0)
		{
			RECT rect;
			GetClientRect(&rect);

			if(m_fPressed == 1)
				dc.DrawEdge(&rect, ((m_dwExtendedStyle & BMPBTN_AUTO3D_SINGLE) != 0) ? BDR_SUNKENOUTER : EDGE_SUNKEN, BF_RECT);
			else if(!bHover || m_fMouseOver == 1)
				dc.DrawEdge(&rect, ((m_dwExtendedStyle & BMPBTN_AUTO3D_SINGLE) != 0) ? BDR_RAISEDINNER : EDGE_RAISED, BF_RECT);

			if(!bHover && m_fFocus == 1)
			{
				::InflateRect(&rect, -2 * ::GetSystemMetrics(SM_CXEDGE), -2 * ::GetSystemMetrics(SM_CYEDGE));
				dc.DrawFocusRect(&rect);
			}
		}
	}

// Message map and handlers
	BEGIN_MSG_MAP(CBitmapButtonImpl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		MESSAGE_HANDLER(WM_SETFOCUS, OnFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnFocus)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
		MESSAGE_HANDLER(WM_ENABLE, OnEnable)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_UPDATEUISTATE, OnUpdateUiState)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		Init();
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(m_tip.IsWindow())
		{
			m_tip.DestroyWindow();
			m_tip.m_hWnd = NULL;
		}
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MSG msg = { m_hWnd, uMsg, wParam, lParam };
		if(m_tip.IsWindow())
			m_tip.RelayEvent(&msg);
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1;   // no background needed
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		if(wParam != NULL)
		{
			pT->DoPaint((HDC)wParam);
		}
		else
		{
			CPaintDC dc(m_hWnd);
			pT->DoPaint(dc.m_hDC);
		}
		return 0;
	}

	LRESULT OnFocus(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		m_fFocus = (uMsg == WM_SETFOCUS) ? 1 : 0;
		Invalidate();
		UpdateWindow();
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LRESULT lRet = 0;
		if(IsHoverMode())
			SetCapture();
		else
			lRet = DefWindowProc(uMsg, wParam, lParam);
		if(::GetCapture() == m_hWnd)
		{
			m_fPressed = 1;
			Invalidate();
			UpdateWindow();
		}
		if((m_dwExtendedStyle & BMPBTN_AUTOFIRE) != 0)
		{
			int nElapse = 250;
			int nDelay = 0;
			if(::SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &nDelay, 0))
				nElapse += nDelay * 250;   // all milli-seconds
			SetTimer(ID_TIMER_FIRST, nElapse);
		}
		return lRet;
	}

	LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LRESULT lRet = 0;
		if(!IsHoverMode())
			lRet = DefWindowProc(uMsg, wParam, lParam);
		if(::GetCapture() != m_hWnd)
			SetCapture();
		if(m_fPressed == 0)
		{
			m_fPressed = 1;
			Invalidate();
			UpdateWindow();
		}
		return lRet;
	}

	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LRESULT lRet = 0;
		bool bHover = IsHoverMode();
		if(!bHover)
			lRet = DefWindowProc(uMsg, wParam, lParam);
		if(::GetCapture() == m_hWnd)
		{
			if(bHover && m_fPressed == 1)
				::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
			::ReleaseCapture();
		}
		return lRet;
	}

	LRESULT OnCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(m_fPressed == 1)
		{
			m_fPressed = 0;
			Invalidate();
			UpdateWindow();
		}
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnEnable(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		Invalidate();
		UpdateWindow();
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if(::GetCapture() == m_hWnd)
		{
			POINT ptCursor = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ClientToScreen(&ptCursor);
			RECT rect = { 0 };
			GetWindowRect(&rect);
			unsigned int uPressed = ::PtInRect(&rect, ptCursor) ? 1 : 0;
			if(m_fPressed != uPressed)
			{
				m_fPressed = uPressed;
				Invalidate();
				UpdateWindow();
			}
		}
		else if(IsHoverMode() && m_fMouseOver == 0)
		{
			m_fMouseOver = 1;
			Invalidate();
			UpdateWindow();
			StartTrackMouseLeave();
		}
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(m_fMouseOver == 1)
		{
			m_fMouseOver = 0;
			Invalidate();
			UpdateWindow();
		}
		return 0;
	}

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(wParam == VK_SPACE && IsHoverMode())
			return 0;   // ignore if in hover mode
		if(wParam == VK_SPACE && m_fPressed == 0)
		{
			m_fPressed = 1;
			Invalidate();
			UpdateWindow();
		}
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(wParam == VK_SPACE && IsHoverMode())
			return 0;   // ignore if in hover mode
		if(wParam == VK_SPACE && m_fPressed == 1)
		{
			m_fPressed = 0;
			Invalidate();
			UpdateWindow();
		}
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		ATLASSERT((m_dwExtendedStyle & BMPBTN_AUTOFIRE) != 0);
		switch(wParam)   // timer ID
		{
		case ID_TIMER_FIRST:
			KillTimer(ID_TIMER_FIRST);
			if(m_fPressed == 1)
			{
				::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
				int nElapse = 250;
				int nRepeat = 40;
				if(::SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &nRepeat, 0))
					nElapse = 10000 / (10 * nRepeat + 25);   // milli-seconds, approximated
				SetTimer(ID_TIMER_REPEAT, nElapse);
			}
			break;
		case ID_TIMER_REPEAT:
			if(m_fPressed == 1)
				::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
			else if(::GetCapture() != m_hWnd)
				KillTimer(ID_TIMER_REPEAT);
			break;
		default:	// not our timer
			break;
		}
		return 0;
	}

	LRESULT OnUpdateUiState(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// If the control is subclassed or superclassed, this message can cause
		// repainting without WM_PAINT. We don't use this state, so just do nothing.
		return 0;
	}

// Implementation
	void Init()
	{
		// We need this style to prevent Windows from painting the button
		ModifyStyle(0, BS_OWNERDRAW);

		// create a tool tip
		m_tip.Create(m_hWnd);
		ATLASSERT(m_tip.IsWindow());
		if(m_tip.IsWindow() && m_lpstrToolTipText != NULL)
		{
			m_tip.Activate(TRUE);
			m_tip.AddTool(m_hWnd, m_lpstrToolTipText);
		}

		if(m_ImageList.m_hImageList != NULL && (m_dwExtendedStyle & BMPBTN_AUTOSIZE) != 0)
			SizeToImage();
	}

	BOOL StartTrackMouseLeave()
	{
		TRACKMOUSEEVENT tme = { 0 };
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		return _TrackMouseEvent(&tme);
	}

	bool IsHoverMode() const
	{
		return ((m_dwExtendedStyle & BMPBTN_HOVER) != 0);
	}
};


class CBitmapButton : public CBitmapButtonImpl<CBitmapButton>
{
public:
	DECLARE_WND_SUPERCLASS(_T("WTL_BitmapButton"), GetWndClassName())

	CBitmapButton(DWORD dwExtendedStyle = BMPBTN_AUTOSIZE, HIMAGELIST hImageList = NULL) : 
		CBitmapButtonImpl<CBitmapButton>(dwExtendedStyle, hImageList)
	{ }
};

#endif // !_WIN32_WCE


///////////////////////////////////////////////////////////////////////////////
// CCheckListCtrlView - list view control with check boxes

template <DWORD t_dwStyle, DWORD t_dwExStyle, DWORD t_dwExListViewStyle>
class CCheckListViewCtrlImplTraits
{
public:
	static DWORD GetWndStyle(DWORD dwStyle)
	{
		return (dwStyle == 0) ? t_dwStyle : dwStyle;
	}

	static DWORD GetWndExStyle(DWORD dwExStyle)
	{
		return (dwExStyle == 0) ? t_dwExStyle : dwExStyle;
	}

	static DWORD GetExtendedLVStyle()
	{
		return t_dwExListViewStyle;
	}
};

typedef CCheckListViewCtrlImplTraits<WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT>   CCheckListViewCtrlTraits;

template <class T, class TBase = CListViewCtrl, class TWinTraits = CCheckListViewCtrlTraits>
class ATL_NO_VTABLE CCheckListViewCtrlImpl : public ATL::CWindowImpl<T, TBase, TWinTraits>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

// Attributes
	static DWORD GetExtendedLVStyle()
	{
		return TWinTraits::GetExtendedLVStyle();
	}

// Operations
	BOOL SubclassWindow(HWND hWnd)
	{
#if (_MSC_VER >= 1300)
		BOOL bRet = ATL::CWindowImplBaseT< TBase, TWinTraits>::SubclassWindow(hWnd);
#else // !(_MSC_VER >= 1300)
		typedef ATL::CWindowImplBaseT< TBase, TWinTraits>   _baseClass;
		BOOL bRet = _baseClass::SubclassWindow(hWnd);
#endif // !(_MSC_VER >= 1300)
		if(bRet)
		{
			T* pT = static_cast<T*>(this);
			pT;
			ATLASSERT((pT->GetExtendedLVStyle() & LVS_EX_CHECKBOXES) != 0);
			SetExtendedListViewStyle(pT->GetExtendedLVStyle());
		}
		return bRet;
	}

	void CheckSelectedItems(int nCurrItem)
	{
		// first check if this item is selected
		LVITEM lvi = { 0 };
		lvi.iItem = nCurrItem;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_STATE;
		lvi.stateMask = LVIS_SELECTED;
		GetItem(&lvi);
		// if item is not selected, don't do anything
		if(!(lvi.state & LVIS_SELECTED))
			return;
		// new check state will be reverse of the current state,
		BOOL bCheck = !GetCheckState(nCurrItem);
		int nItem = -1;
		int nOldItem = -1;
		while((nItem = GetNextItem(nOldItem, LVNI_SELECTED)) != -1)
		{
			if(nItem != nCurrItem)
				SetCheckState(nItem, bCheck);
			nOldItem = nItem;
		}
	}

// Implementation
	BEGIN_MSG_MAP(CCheckListViewCtrlImpl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDown)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		// first let list view control initialize everything
		LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);
		T* pT = static_cast<T*>(this);
		pT;
		ATLASSERT((pT->GetExtendedLVStyle() & LVS_EX_CHECKBOXES) != 0);
		SetExtendedListViewStyle(pT->GetExtendedLVStyle());
		return lRet;
	}

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		POINT ptMsg = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		LVHITTESTINFO lvh = { 0 };
		lvh.pt = ptMsg;
		if(HitTest(&lvh) != -1 && lvh.flags == LVHT_ONITEMSTATEICON && ::GetKeyState(VK_CONTROL) >= 0)
		{
			T* pT = static_cast<T*>(this);
			pT->CheckSelectedItems(lvh.iItem);
		}
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(wParam == VK_SPACE)
		{
			int nCurrItem = GetNextItem(-1, LVNI_FOCUSED);
			if(nCurrItem != -1  && ::GetKeyState(VK_CONTROL) >= 0)
			{
				T* pT = static_cast<T*>(this);
				pT->CheckSelectedItems(nCurrItem);
			}
		}
		bHandled = FALSE;
		return 1;
	}
};

class CCheckListViewCtrl : public CCheckListViewCtrlImpl<CCheckListViewCtrl>
{
public:
	DECLARE_WND_SUPERCLASS(_T("WTL_CheckListView"), GetWndClassName())
};


///////////////////////////////////////////////////////////////////////////////
// CHyperLink - hyper link control implementation

#if (WINVER < 0x0500) && !defined(_WIN32_WCE)
__declspec(selectany) struct
{
	enum { cxWidth = 32, cyHeight = 32 };
	int xHotSpot;
	int yHotSpot;
	unsigned char arrANDPlane[cxWidth * cyHeight / 8];
	unsigned char arrXORPlane[cxWidth * cyHeight / 8];
} _AtlHyperLink_CursorData = 
{
	5, 0, 
	{
		0xF9, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 
		0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF, 0xF0, 0x07, 0xFF, 0xFF, 0xF0, 0x01, 0xFF, 0xFF, 
		0xF0, 0x00, 0xFF, 0xFF, 0x10, 0x00, 0x7F, 0xFF, 0x00, 0x00, 0x7F, 0xFF, 0x00, 0x00, 0x7F, 0xFF, 
		0x80, 0x00, 0x7F, 0xFF, 0xC0, 0x00, 0x7F, 0xFF, 0xC0, 0x00, 0x7F, 0xFF, 0xE0, 0x00, 0x7F, 0xFF, 
		0xE0, 0x00, 0xFF, 0xFF, 0xF0, 0x00, 0xFF, 0xFF, 0xF0, 0x00, 0xFF, 0xFF, 0xF8, 0x01, 0xFF, 0xFF, 
		0xF8, 0x01, 0xFF, 0xFF, 0xF8, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	},
	{
		0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 
		0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0xC0, 0x00, 0x00, 0x06, 0xD8, 0x00, 0x00, 
		0x06, 0xDA, 0x00, 0x00, 0x06, 0xDB, 0x00, 0x00, 0x67, 0xFB, 0x00, 0x00, 0x77, 0xFF, 0x00, 0x00, 
		0x37, 0xFF, 0x00, 0x00, 0x17, 0xFF, 0x00, 0x00, 0x1F, 0xFF, 0x00, 0x00, 0x0F, 0xFF, 0x00, 0x00, 
		0x0F, 0xFE, 0x00, 0x00, 0x07, 0xFE, 0x00, 0x00, 0x07, 0xFE, 0x00, 0x00, 0x03, 0xFC, 0x00, 0x00, 
		0x03, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	}
};
#endif // (WINVER < 0x0500) && !defined(_WIN32_WCE)

#define HLINK_UNDERLINED      0x00000000
#define HLINK_NOTUNDERLINED   0x00000001
#define HLINK_UNDERLINEHOVER  0x00000002
#define HLINK_COMMANDBUTTON   0x00000004
#define HLINK_NOTIFYBUTTON    0x0000000C
#define HLINK_USETAGS         0x00000010
#define HLINK_USETAGSBOLD     0x00000030
#define HLINK_NOTOOLTIP       0x00000040

// Notes:
// - HLINK_USETAGS and HLINK_USETAGSBOLD are always left-aligned
// - When HLINK_USETAGSBOLD is used, the underlined styles will be ignored

template <class T, class TBase = ATL::CWindow, class TWinTraits = ATL::CControlWinTraits>
class ATL_NO_VTABLE CHyperLinkImpl : public ATL::CWindowImpl< T, TBase, TWinTraits >
{
public:
	LPTSTR m_lpstrLabel;
	LPTSTR m_lpstrHyperLink;

	HCURSOR m_hCursor;
	HFONT m_hFont;
	HFONT m_hFontNormal;

	RECT m_rcLink;
#ifndef _WIN32_WCE
	CToolTipCtrl m_tip;
#endif // !_WIN32_WCE

	COLORREF m_clrLink;
	COLORREF m_clrVisited;

	DWORD m_dwExtendedStyle;   // Hyper Link specific extended styles

	bool m_bPaintLabel:1;
	bool m_bVisited:1;
	bool m_bHover:1;
	bool m_bInternalLinkFont:1;


// Constructor/Destructor
	CHyperLinkImpl(DWORD dwExtendedStyle = HLINK_UNDERLINED) : 
			m_lpstrLabel(NULL), m_lpstrHyperLink(NULL),
			m_hCursor(NULL), m_hFont(NULL), m_hFontNormal(NULL),
			m_clrLink(RGB(0, 0, 255)), m_clrVisited(RGB(128, 0, 128)),
			m_dwExtendedStyle(dwExtendedStyle),
			m_bPaintLabel(true), m_bVisited(false),
			m_bHover(false), m_bInternalLinkFont(false)
	{
		::SetRectEmpty(&m_rcLink);
	}

	~CHyperLinkImpl()
	{
		free(m_lpstrLabel);
		free(m_lpstrHyperLink);
		if(m_bInternalLinkFont && m_hFont != NULL)
			::DeleteObject(m_hFont);
#if (WINVER < 0x0500) && !defined(_WIN32_WCE)
		// It was created, not loaded, so we have to destroy it
		if(m_hCursor != NULL)
			::DestroyCursor(m_hCursor);
#endif // (WINVER < 0x0500) && !defined(_WIN32_WCE)
	}

// Attributes
	DWORD GetHyperLinkExtendedStyle() const
	{
		return m_dwExtendedStyle;
	}

	DWORD SetHyperLinkExtendedStyle(DWORD dwExtendedStyle, DWORD dwMask = 0)
	{
		DWORD dwPrevStyle = m_dwExtendedStyle;
		if(dwMask == 0)
			m_dwExtendedStyle = dwExtendedStyle;
		else
			m_dwExtendedStyle = (m_dwExtendedStyle & ~dwMask) | (dwExtendedStyle & dwMask);
		return dwPrevStyle;
	}

	bool GetLabel(LPTSTR lpstrBuffer, int nLength) const
	{
		if(m_lpstrLabel == NULL)
			return false;
		ATLASSERT(lpstrBuffer != NULL);
		if(nLength > lstrlen(m_lpstrLabel) + 1)
		{
			lstrcpy(lpstrBuffer, m_lpstrLabel);
			return true;
		}
		return false;
	}

	bool SetLabel(LPCTSTR lpstrLabel)
	{
		free(m_lpstrLabel);
		m_lpstrLabel = NULL;
		ATLTRY(m_lpstrLabel = (LPTSTR)malloc((lstrlen(lpstrLabel) + 1) * sizeof(TCHAR)));
		if(m_lpstrLabel == NULL)
			return false;
		lstrcpy(m_lpstrLabel, lpstrLabel);
		T* pT = static_cast<T*>(this);
		pT->CalcLabelRect();

		if(m_hWnd != NULL)
			SetWindowText(lpstrLabel);   // Set this for accessibility

		return true;
	}

	bool GetHyperLink(LPTSTR lpstrBuffer, int nLength) const
	{
		if(m_lpstrHyperLink == NULL)
			return false;
		ATLASSERT(lpstrBuffer != NULL);
		if(nLength > lstrlen(m_lpstrHyperLink) + 1)
		{
			lstrcpy(lpstrBuffer, m_lpstrHyperLink);
			return true;
		}
		return false;
	}

	bool SetHyperLink(LPCTSTR lpstrLink)
	{
		free(m_lpstrHyperLink);
		m_lpstrHyperLink = NULL;
		ATLTRY(m_lpstrHyperLink = (LPTSTR)malloc((lstrlen(lpstrLink) + 1) * sizeof(TCHAR)));
		if(m_lpstrHyperLink == NULL)
			return false;
		lstrcpy(m_lpstrHyperLink, lpstrLink);
		if(m_lpstrLabel == NULL)
		{
			T* pT = static_cast<T*>(this);
			pT->CalcLabelRect();
		}
#ifndef _WIN32_WCE
		if(m_tip.IsWindow())
		{
			m_tip.Activate(TRUE);
			m_tip.AddTool(m_hWnd, m_lpstrHyperLink, &m_rcLink, 1);
		}
#endif // !_WIN32_WCE
		return true;
	}

	HFONT GetLinkFont() const
	{
		return m_hFont;
	}

	void SetLinkFont(HFONT hFont)
	{
		if(m_bInternalLinkFont && m_hFont != NULL)
		{
			::DeleteObject(m_hFont);
			m_bInternalLinkFont = false;
		}
		m_hFont = hFont;
	}

	int GetIdealHeight() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		if(m_lpstrLabel == NULL && m_lpstrHyperLink == NULL)
			return -1;
		if(!m_bPaintLabel)
			return -1;

		CClientDC dc(m_hWnd);
		RECT rect = { 0 };
		GetClientRect(&rect);
		HFONT hFontOld = dc.SelectFont(m_hFontNormal);
		RECT rcText = rect;
		dc.DrawText(_T("NS"), -1, &rcText, DT_LEFT | DT_WORDBREAK | DT_CALCRECT);
		dc.SelectFont(m_hFont);
		RECT rcLink = rect;
		dc.DrawText(_T("NS"), -1, &rcLink, DT_LEFT | DT_WORDBREAK | DT_CALCRECT);
		dc.SelectFont(hFontOld);
		return max(rcText.bottom - rcText.top, rcLink.bottom - rcLink.top);
	}

	bool GetIdealSize(SIZE& size) const
	{
		int cx = 0, cy = 0;
		bool bRet = GetIdealSize(cx, cy);
		if(bRet)
		{
			size.cx = cx;
			size.cy = cy;
		}
		return bRet;
	}

	bool GetIdealSize(int& cx, int& cy) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		if(m_lpstrLabel == NULL && m_lpstrHyperLink == NULL)
			return false;
		if(!m_bPaintLabel)
			return false;

		CClientDC dc(m_hWnd);
		RECT rcClient = { 0 };
		GetClientRect(&rcClient);
		RECT rcAll = rcClient;

		if(IsUsingTags())
		{
			// find tags and label parts
			LPTSTR lpstrLeft = NULL;
			int cchLeft = 0;
			LPTSTR lpstrLink = NULL;
			int cchLink = 0;
			LPTSTR lpstrRight = NULL;
			int cchRight = 0;

			const T* pT = static_cast<const T*>(this);
			pT->CalcLabelParts(lpstrLeft, cchLeft, lpstrLink, cchLink, lpstrRight, cchRight);

			// get label part rects
			HFONT hFontOld = dc.SelectFont(m_hFontNormal);
			RECT rcLeft = rcClient;
			dc.DrawText(lpstrLeft, cchLeft, &rcLeft, DT_LEFT | DT_WORDBREAK | DT_CALCRECT);

			dc.SelectFont(m_hFont);
			RECT rcLink = { rcLeft.right, rcLeft.top, rcClient.right, rcClient.bottom };
			dc.DrawText(lpstrLink, cchLink, &rcLink, DT_LEFT | DT_WORDBREAK | DT_CALCRECT);

			dc.SelectFont(m_hFontNormal);
			RECT rcRight = { rcLink.right, rcLink.top, rcClient.right, rcClient.bottom };
			dc.DrawText(lpstrRight, cchRight, &rcRight, DT_LEFT | DT_WORDBREAK | DT_CALCRECT);

			dc.SelectFont(hFontOld);

			int cyMax = max(rcLeft.bottom, max(rcLink.bottom, rcRight.bottom));
			::SetRect(&rcAll, rcLeft.left, rcLeft.top, rcRight.right, cyMax);
		}
		else
		{
			HFONT hOldFont = NULL;
			if(m_hFont != NULL)
				hOldFont = dc.SelectFont(m_hFont);
			LPTSTR lpstrText = (m_lpstrLabel != NULL) ? m_lpstrLabel : m_lpstrHyperLink;
			DWORD dwStyle = GetStyle();
			int nDrawStyle = DT_LEFT;
			if (dwStyle & SS_CENTER)
				nDrawStyle = DT_CENTER;
			else if (dwStyle & SS_RIGHT)
				nDrawStyle = DT_RIGHT;
			dc.DrawText(lpstrText, -1, &rcAll, nDrawStyle | DT_WORDBREAK | DT_CALCRECT);
			if(m_hFont != NULL)
				dc.SelectFont(hOldFont);
			if (dwStyle & SS_CENTER)
			{
				int dx = (rcClient.right - rcAll.right) / 2;
				::OffsetRect(&rcAll, dx, 0);
			}
			else if (dwStyle & SS_RIGHT)
			{
				int dx = rcClient.right - rcAll.right;
				::OffsetRect(&rcAll, dx, 0);
			}
		}

		cx = rcAll.right - rcAll.left;
		cy = rcAll.bottom - rcAll.top;

		return true;
	}

	// for command buttons only
	bool GetToolTipText(LPTSTR lpstrBuffer, int nLength) const
	{
		ATLASSERT(IsCommandButton());
		return GetHyperLink(lpstrBuffer, nLength);
	}

	bool SetToolTipText(LPCTSTR lpstrToolTipText)
	{
		ATLASSERT(IsCommandButton());
		return SetHyperLink(lpstrToolTipText);
	}

// Operations
	BOOL SubclassWindow(HWND hWnd)
	{
		ATLASSERT(m_hWnd == NULL);
		ATLASSERT(::IsWindow(hWnd));
#if (_MSC_VER >= 1300)
		BOOL bRet = ATL::CWindowImpl< T, TBase, TWinTraits>::SubclassWindow(hWnd);
#else // !(_MSC_VER >= 1300)
		typedef ATL::CWindowImpl< T, TBase, TWinTraits>   _baseClass;
		BOOL bRet = _baseClass::SubclassWindow(hWnd);
#endif // !(_MSC_VER >= 1300)
		if(bRet)
		{
			T* pT = static_cast<T*>(this);
			pT->Init();
		}
		return bRet;
	}

	bool Navigate()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		bool bRet = true;
		if(IsNotifyButton())
		{
			NMHDR nmhdr = { m_hWnd, GetDlgCtrlID(), NM_CLICK };
			::SendMessage(GetParent(), WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nmhdr);
		}
		else if(IsCommandButton())
		{
			::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
		}
		else
		{
			ATLASSERT(m_lpstrHyperLink != NULL);
#ifndef _WIN32_WCE
			DWORD_PTR dwRet = (DWORD_PTR)::ShellExecute(0, _T("open"), m_lpstrHyperLink, 0, 0, SW_SHOWNORMAL);
#else // CE specific
			SHELLEXECUTEINFO shExeInfo = { sizeof(SHELLEXECUTEINFO), 0, 0, L"open", m_lpstrHyperLink, 0, 0, SW_SHOWNORMAL, 0, 0, 0, 0, 0, 0, 0 };
			::ShellExecuteEx(&shExeInfo);
			DWORD_PTR dwRet = (DWORD_PTR)shExeInfo.hInstApp;
#endif // _WIN32_WCE
			bRet = (dwRet > 32);
			ATLASSERT(bRet);
			if(bRet)
			{
				m_bVisited = true;
				Invalidate();
			}
		}
		return bRet;
	}

// Message map and handlers
	BEGIN_MSG_MAP(CHyperLinkImpl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
#ifndef _WIN32_WCE
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
#endif // !_WIN32_WCE
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
#ifndef _WIN32_WCE
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
#endif // !_WIN32_WCE
		MESSAGE_HANDLER(WM_SETFOCUS, OnFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnFocus)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
#ifndef _WIN32_WCE
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
#endif // !_WIN32_WCE
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_ENABLE, OnEnable)
		MESSAGE_HANDLER(WM_GETFONT, OnGetFont)
		MESSAGE_HANDLER(WM_SETFONT, OnSetFont)
		MESSAGE_HANDLER(WM_UPDATEUISTATE, OnUpdateUiState)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->Init();
		return 0;
	}

#ifndef _WIN32_WCE
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(m_tip.IsWindow())
		{
			m_tip.DestroyWindow();
			m_tip.m_hWnd = NULL;
		}
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MSG msg = { m_hWnd, uMsg, wParam, lParam };
		if(m_tip.IsWindow() && IsUsingToolTip())
			m_tip.RelayEvent(&msg);
		bHandled = FALSE;
		return 1;
	}
#endif // !_WIN32_WCE

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1;   // no background painting needed (we do it all during WM_PAINT)
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(!m_bPaintLabel)
		{
			bHandled = FALSE;
			return 1;
		}

		T* pT = static_cast<T*>(this);
		if(wParam != NULL)
		{
			pT->DoEraseBackground((HDC)wParam);
			pT->DoPaint((HDC)wParam);
		}
		else
		{
			CPaintDC dc(m_hWnd);
			pT->DoEraseBackground(dc.m_hDC);
			pT->DoPaint(dc.m_hDC);
		}

		return 0;
	}

	LRESULT OnFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(m_bPaintLabel)
			Invalidate();
		else
			bHandled = FALSE;
		return 0;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		if((m_lpstrHyperLink != NULL  || IsCommandButton()) && ::PtInRect(&m_rcLink, pt))
		{
			::SetCursor(m_hCursor);
			if(IsUnderlineHover())
			{
				if(!m_bHover)
				{
					m_bHover = true;
					InvalidateRect(&m_rcLink);
					UpdateWindow();
#ifndef _WIN32_WCE
					StartTrackMouseLeave();
#endif // !_WIN32_WCE
				}
			}
		}
		else
		{
			if(IsUnderlineHover())
			{
				if(m_bHover)
				{
					m_bHover = false;
					InvalidateRect(&m_rcLink);
					UpdateWindow();
				}
			}
			bHandled = FALSE;
		}
		return 0;
	}

#ifndef _WIN32_WCE
	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(IsUnderlineHover() && m_bHover)
		{
			m_bHover = false;
			InvalidateRect(&m_rcLink);
			UpdateWindow();
		}
		return 0;
	}
#endif // !_WIN32_WCE

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		if(::PtInRect(&m_rcLink, pt))
		{
			SetFocus();
			SetCapture();
		}
		return 0;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if(GetCapture() == m_hWnd)
		{
			ReleaseCapture();
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			if(::PtInRect(&m_rcLink, pt))
			{
				T* pT = static_cast<T*>(this);
				pT->Navigate();
			}
		}
		return 0;
	}

	LRESULT OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(wParam == VK_RETURN || wParam == VK_SPACE)
		{
			T* pT = static_cast<T*>(this);
			pT->Navigate();
		}
		return 0;
	}

	LRESULT OnGetDlgCode(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return DLGC_WANTCHARS;
	}

	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		POINT pt = { 0, 0 };
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		if((m_lpstrHyperLink != NULL  || IsCommandButton()) && ::PtInRect(&m_rcLink, pt))
		{
			return TRUE;
		}
		bHandled = FALSE;
		return FALSE;
	}

	LRESULT OnEnable(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		Invalidate();
		UpdateWindow();
		return 0;
	}

	LRESULT OnGetFont(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return (LRESULT)m_hFontNormal;
	}

	LRESULT OnSetFont(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		m_hFontNormal = (HFONT)wParam;
		if((BOOL)lParam)
		{
			Invalidate();
			UpdateWindow();
		}
		return 0;
	}

	LRESULT OnUpdateUiState(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// If the control is subclassed or superclassed, this message can cause
		// repainting without WM_PAINT. We don't use this state, so just do nothing.
		return 0;
	}

// Implementation
	void Init()
	{
		ATLASSERT(::IsWindow(m_hWnd));

		// Check if we should paint a label
		const int cchBuff = 8;
		TCHAR szBuffer[cchBuff] = { 0 };
		if(::GetClassName(m_hWnd, szBuffer, cchBuff))
		{
			if(lstrcmpi(szBuffer, _T("static")) == 0)
			{
				ModifyStyle(0, SS_NOTIFY);   // we need this
				DWORD dwStyle = GetStyle() & 0x000000FF;
#ifndef _WIN32_WCE
				if(dwStyle == SS_ICON || dwStyle == SS_BLACKRECT || dwStyle == SS_GRAYRECT || 
						dwStyle == SS_WHITERECT || dwStyle == SS_BLACKFRAME || dwStyle == SS_GRAYFRAME || 
						dwStyle == SS_WHITEFRAME || dwStyle == SS_OWNERDRAW || 
						dwStyle == SS_BITMAP || dwStyle == SS_ENHMETAFILE)
#else // CE specific
				if(dwStyle == SS_ICON || dwStyle == SS_BITMAP)
#endif // _WIN32_WCE
					m_bPaintLabel = false;
			}
		}

		// create or load a cursor
#if (WINVER >= 0x0500) || defined(_WIN32_WCE)
		m_hCursor = ::LoadCursor(NULL, IDC_HAND);
#else
  #if (_ATL_VER >= 0x0700)
		m_hCursor = ::CreateCursor(ATL::_AtlBaseModule.GetModuleInstance(), _AtlHyperLink_CursorData.xHotSpot, _AtlHyperLink_CursorData.yHotSpot, _AtlHyperLink_CursorData.cxWidth, _AtlHyperLink_CursorData.cyHeight, _AtlHyperLink_CursorData.arrANDPlane, _AtlHyperLink_CursorData.arrXORPlane);
  #else // !(_ATL_VER >= 0x0700)
		m_hCursor = ::CreateCursor(_Module.GetModuleInstance(), _AtlHyperLink_CursorData.xHotSpot, _AtlHyperLink_CursorData.yHotSpot, _AtlHyperLink_CursorData.cxWidth, _AtlHyperLink_CursorData.cyHeight, _AtlHyperLink_CursorData.arrANDPlane, _AtlHyperLink_CursorData.arrXORPlane);
  #endif // !(_ATL_VER >= 0x0700)
#endif
		ATLASSERT(m_hCursor != NULL);

		// set font
		if(m_bPaintLabel)
		{
			ATL::CWindow wnd = GetParent();
			m_hFontNormal = wnd.GetFont();
			if(m_hFontNormal == NULL)
				m_hFontNormal = (HFONT)::GetStockObject(SYSTEM_FONT);
			if(m_hFontNormal != NULL && m_hFont == NULL)
			{
				LOGFONT lf = { 0 };
				CFontHandle font = m_hFontNormal;
				font.GetLogFont(&lf);
				if(IsUsingTagsBold())
					lf.lfWeight = FW_BOLD;
				else if(!IsNotUnderlined())
					lf.lfUnderline = TRUE;
				m_hFont = ::CreateFontIndirect(&lf);
				m_bInternalLinkFont = true;
				ATLASSERT(m_hFont != NULL);
			}
		}

#ifndef _WIN32_WCE
		// create a tool tip
		m_tip.Create(m_hWnd);
		ATLASSERT(m_tip.IsWindow());
#endif // !_WIN32_WCE

		// set label (defaults to window text)
		if(m_lpstrLabel == NULL)
		{
			int nLen = GetWindowTextLength();
			if(nLen > 0)
			{
				LPTSTR lpszText = (LPTSTR)_alloca((nLen + 1) * sizeof(TCHAR));
				if(GetWindowText(lpszText, nLen + 1))
					SetLabel(lpszText);
			}
		}

		T* pT = static_cast<T*>(this);
		pT->CalcLabelRect();

		// set hyperlink (defaults to label), or just activate tool tip if already set
		if(m_lpstrHyperLink == NULL && !IsCommandButton())
		{
			if(m_lpstrLabel != NULL)
				SetHyperLink(m_lpstrLabel);
		}
#ifndef _WIN32_WCE
		else
		{
			m_tip.Activate(TRUE);
			m_tip.AddTool(m_hWnd, m_lpstrHyperLink, &m_rcLink, 1);
		}
#endif // !_WIN32_WCE

		// set link colors
		if(m_bPaintLabel)
		{
			ATL::CRegKey rk;
			LONG lRet = rk.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\Settings"));
			if(lRet == 0)
			{
				const int cchBuff = 12;
				TCHAR szBuff[cchBuff] = { 0 };
#if (_ATL_VER >= 0x0700)
				ULONG ulCount = cchBuff;
				lRet = rk.QueryStringValue(_T("Anchor Color"), szBuff, &ulCount);
#else
				DWORD dwCount = cchBuff * sizeof(TCHAR);
				lRet = rk.QueryValue(szBuff, _T("Anchor Color"), &dwCount);
#endif
				if(lRet == 0)
				{
					COLORREF clr = pT->_ParseColorString(szBuff);
					ATLASSERT(clr != CLR_INVALID);
					if(clr != CLR_INVALID)
						m_clrLink = clr;
				}

#if (_ATL_VER >= 0x0700)
				ulCount = cchBuff;
				lRet = rk.QueryStringValue(_T("Anchor Color Visited"), szBuff, &ulCount);
#else
				dwCount = cchBuff * sizeof(TCHAR);
				lRet = rk.QueryValue(szBuff, _T("Anchor Color Visited"), &dwCount);
#endif
				if(lRet == 0)
				{
					COLORREF clr = pT->_ParseColorString(szBuff);
					ATLASSERT(clr != CLR_INVALID);
					if(clr != CLR_INVALID)
						m_clrVisited = clr;
				}
			}
		}
	}

	static COLORREF _ParseColorString(LPTSTR lpstr)
	{
		int c[3] = { -1, -1, -1 };
		LPTSTR p = NULL;
		for(int i = 0; i < 2; i++)
		{
			for(p = lpstr; *p != _T('\0'); p = ::CharNext(p))
			{
				if(*p == _T(','))
				{
					*p = _T('\0');
					c[i] = T::_xttoi(lpstr);
					lpstr = &p[1];
					break;
				}
			}
			if(c[i] == -1)
				return CLR_INVALID;
		}
		if(*lpstr == _T('\0'))
			return CLR_INVALID;
		c[2] = T::_xttoi(lpstr);

		return RGB(c[0], c[1], c[2]);
	}

	bool CalcLabelRect()
	{
		if(!::IsWindow(m_hWnd))
			return false;
		if(m_lpstrLabel == NULL && m_lpstrHyperLink == NULL)
			return false;

		CClientDC dc(m_hWnd);
		RECT rcClient = { 0 };
		GetClientRect(&rcClient);
		m_rcLink = rcClient;
		if(!m_bPaintLabel)
			return true;

		if(IsUsingTags())
		{
			// find tags and label parts
			LPTSTR lpstrLeft = NULL;
			int cchLeft = 0;
			LPTSTR lpstrLink = NULL;
			int cchLink = 0;
			LPTSTR lpstrRight = NULL;
			int cchRight = 0;

			T* pT = static_cast<T*>(this);
			pT->CalcLabelParts(lpstrLeft, cchLeft, lpstrLink, cchLink, lpstrRight, cchRight);
			ATLASSERT(lpstrLink != NULL);
			ATLASSERT(cchLink > 0);

			// get label part rects
			HFONT hFontOld = dc.SelectFont(m_hFontNormal);

			RECT rcLeft = rcClient;
			if(lpstrLeft != NULL)
				dc.DrawText(lpstrLeft, cchLeft, &rcLeft, DT_LEFT | DT_WORDBREAK | DT_CALCRECT);

			dc.SelectFont(m_hFont);
			RECT rcLink = rcClient;
			if(lpstrLeft != NULL)
				rcLink.left = rcLeft.right;
			dc.DrawText(lpstrLink, cchLink, &rcLink, DT_LEFT | DT_WORDBREAK | DT_CALCRECT);

			dc.SelectFont(hFontOld);

			m_rcLink = rcLink;
		}
		else
		{
			HFONT hOldFont = NULL;
			if(m_hFont != NULL)
				hOldFont = dc.SelectFont(m_hFont);
			LPTSTR lpstrText = (m_lpstrLabel != NULL) ? m_lpstrLabel : m_lpstrHyperLink;
			DWORD dwStyle = GetStyle();
			int nDrawStyle = DT_LEFT;
			if (dwStyle & SS_CENTER)
				nDrawStyle = DT_CENTER;
			else if (dwStyle & SS_RIGHT)
				nDrawStyle = DT_RIGHT;
			dc.DrawText(lpstrText, -1, &m_rcLink, nDrawStyle | DT_WORDBREAK | DT_CALCRECT);
			if(m_hFont != NULL)
				dc.SelectFont(hOldFont);
			if (dwStyle & SS_CENTER)
			{
				int dx = (rcClient.right - m_rcLink.right) / 2;
				::OffsetRect(&m_rcLink, dx, 0);
			}
			else if (dwStyle & SS_RIGHT)
			{
				int dx = rcClient.right - m_rcLink.right;
				::OffsetRect(&m_rcLink, dx, 0);
			}
		}

		return true;
	}

	void CalcLabelParts(LPTSTR& lpstrLeft, int& cchLeft, LPTSTR& lpstrLink, int& cchLink, LPTSTR& lpstrRight, int& cchRight) const
	{
		lpstrLeft = NULL;
		cchLeft = 0;
		lpstrLink = NULL;
		cchLink = 0;
		lpstrRight = NULL;
		cchRight = 0;

		LPTSTR lpstrText = (m_lpstrLabel != NULL) ? m_lpstrLabel : m_lpstrHyperLink;
		int cchText = lstrlen(lpstrText);
		bool bOutsideLink = true;
		for(int i = 0; i < cchText; i++)
		{
			if(lpstrText[i] != _T('<'))
				continue;

			if(bOutsideLink)
			{
				if(::CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, &lpstrText[i], 3, _T("<A>"), 3) == CSTR_EQUAL)
				{
					if(i > 0)
					{
						lpstrLeft = lpstrText;
						cchLeft = i;
					}
					lpstrLink = &lpstrText[i + 3];
					bOutsideLink = false;
				}
			}
			else
			{
				if(::CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, &lpstrText[i], 4, _T("</A>"), 4) == CSTR_EQUAL)
				{
					cchLink = i - 3 - cchLeft;
					if(lpstrText[i + 4] != 0)
					{
						lpstrRight = &lpstrText[i + 4];
						cchRight = cchText - (i + 4);
						break;
					}
				}
			}
		}

	}

	void DoEraseBackground(CDCHandle dc)
	{
		HBRUSH hBrush = (HBRUSH)::SendMessage(GetParent(), WM_CTLCOLORSTATIC, (WPARAM)dc.m_hDC, (LPARAM)m_hWnd);
		if(hBrush != NULL)
		{
			RECT rect = { 0 };
			GetClientRect(&rect);
			dc.FillRect(&rect, hBrush);
		}
	}

	void DoPaint(CDCHandle dc)
	{
		if(IsUsingTags())
		{
			// find tags and label parts
			LPTSTR lpstrLeft = NULL;
			int cchLeft = 0;
			LPTSTR lpstrLink = NULL;
			int cchLink = 0;
			LPTSTR lpstrRight = NULL;
			int cchRight = 0;

			T* pT = static_cast<T*>(this);
			pT->CalcLabelParts(lpstrLeft, cchLeft, lpstrLink, cchLink, lpstrRight, cchRight);

			// get label part rects
			RECT rcClient = { 0 };
			GetClientRect(&rcClient);

			dc.SetBkMode(TRANSPARENT);
			HFONT hFontOld = dc.SelectFont(m_hFontNormal);

			if(lpstrLeft != NULL)
				dc.DrawText(lpstrLeft, cchLeft, &rcClient, DT_LEFT | DT_WORDBREAK);

			COLORREF clrOld = dc.SetTextColor(IsWindowEnabled() ? (m_bVisited ? m_clrVisited : m_clrLink) : (::GetSysColor(COLOR_GRAYTEXT)));
			if(m_hFont != NULL && (!IsUnderlineHover() || (IsUnderlineHover() && m_bHover)))
				dc.SelectFont(m_hFont);
			else
				dc.SelectFont(m_hFontNormal);

			dc.DrawText(lpstrLink, cchLink, &m_rcLink, DT_LEFT | DT_WORDBREAK);

			dc.SetTextColor(clrOld);
			dc.SelectFont(m_hFontNormal);
			if(lpstrRight != NULL)
			{
				RECT rcRight = { m_rcLink.right, m_rcLink.top, rcClient.right, rcClient.bottom };
				dc.DrawText(lpstrRight, cchRight, &rcRight, DT_LEFT | DT_WORDBREAK);
			}

			if(GetFocus() == m_hWnd)
				dc.DrawFocusRect(&m_rcLink);

			dc.SelectFont(hFontOld);
		}
		else
		{
			dc.SetBkMode(TRANSPARENT);
			COLORREF clrOld = dc.SetTextColor(IsWindowEnabled() ? (m_bVisited ? m_clrVisited : m_clrLink) : (::GetSysColor(COLOR_GRAYTEXT)));

			HFONT hFontOld = NULL;
			if(m_hFont != NULL && (!IsUnderlineHover() || (IsUnderlineHover() && m_bHover)))
				hFontOld = dc.SelectFont(m_hFont);
			else
				hFontOld = dc.SelectFont(m_hFontNormal);

			LPTSTR lpstrText = (m_lpstrLabel != NULL) ? m_lpstrLabel : m_lpstrHyperLink;

			DWORD dwStyle = GetStyle();
			int nDrawStyle = DT_LEFT;
			if (dwStyle & SS_CENTER)
				nDrawStyle = DT_CENTER;
			else if (dwStyle & SS_RIGHT)
				nDrawStyle = DT_RIGHT;

			dc.DrawText(lpstrText, -1, &m_rcLink, nDrawStyle | DT_WORDBREAK);

			if(GetFocus() == m_hWnd)
				dc.DrawFocusRect(&m_rcLink);

			dc.SetTextColor(clrOld);
			dc.SelectFont(hFontOld);
		}
	}

#ifndef _WIN32_WCE
	BOOL StartTrackMouseLeave()
	{
		TRACKMOUSEEVENT tme = { 0 };
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		return _TrackMouseEvent(&tme);
	}
#endif // !_WIN32_WCE

// Implementation helpers
	bool IsUnderlined() const
	{
		return ((m_dwExtendedStyle & (HLINK_NOTUNDERLINED | HLINK_UNDERLINEHOVER)) == 0);
	}

	bool IsNotUnderlined() const
	{
		return ((m_dwExtendedStyle & HLINK_NOTUNDERLINED) != 0);
	}

	bool IsUnderlineHover() const
	{
		return ((m_dwExtendedStyle & HLINK_UNDERLINEHOVER) != 0);
	}

	bool IsCommandButton() const
	{
		return ((m_dwExtendedStyle & HLINK_COMMANDBUTTON) != 0);
	}

	bool IsNotifyButton() const
	{
		return ((m_dwExtendedStyle & HLINK_NOTIFYBUTTON) == HLINK_NOTIFYBUTTON);
	}

	bool IsUsingTags() const
	{
		return ((m_dwExtendedStyle & HLINK_USETAGS) != 0);
	}

	bool IsUsingTagsBold() const
	{
		return ((m_dwExtendedStyle & HLINK_USETAGSBOLD) == HLINK_USETAGSBOLD);
	}

	bool IsUsingToolTip() const
	{
		return ((m_dwExtendedStyle & HLINK_NOTOOLTIP) == 0);
	}

	static int _xttoi(const TCHAR* nptr)
	{
#ifndef _ATL_MIN_CRT
		return _ttoi(nptr);
#else // _ATL_MIN_CRT
		while(*nptr == _T(' '))   // skip spaces
			++nptr;

		int c = (int)(_TUCHAR)*nptr++;
		int sign = c;   // save sign indication
		if (c == _T('-') || c == _T('+'))
			c = (int)(_TUCHAR)*nptr++;   // skip sign

		int total = 0;
		while((TCHAR)c >= _T('0') && (TCHAR)c <= _T('9'))
		{
			total = 10 * total + ((TCHAR)c - _T('0'));   // accumulate digit
			c = (int)(_TUCHAR)*nptr++;        // get next char
		}

		// return result, negated if necessary
		return ((TCHAR)sign != _T('-')) ? total : -total;
#endif // _ATL_MIN_CRT
	}
};


class CHyperLink : public CHyperLinkImpl<CHyperLink>
{
public:
	DECLARE_WND_CLASS(_T("WTL_HyperLink"))
};


///////////////////////////////////////////////////////////////////////////////
// CWaitCursor - displays a wait cursor

class CWaitCursor
{
public:
// Data
	HCURSOR m_hWaitCursor;
	HCURSOR m_hOldCursor;
	bool m_bInUse;

// Constructor/destructor
	CWaitCursor(bool bSet = true, LPCTSTR lpstrCursor = IDC_WAIT, bool bSys = true) : m_hOldCursor(NULL), m_bInUse(false)
	{
#if (_ATL_VER >= 0x0700)
		HINSTANCE hInstance = bSys ? NULL : ATL::_AtlBaseModule.GetResourceInstance();
#else // !(_ATL_VER >= 0x0700)
		HINSTANCE hInstance = bSys ? NULL : _Module.GetResourceInstance();
#endif // !(_ATL_VER >= 0x0700)
		m_hWaitCursor = ::LoadCursor(hInstance, lpstrCursor);
		ATLASSERT(m_hWaitCursor != NULL);

		if(bSet)
			Set();
	}

	~CWaitCursor()
	{
		Restore();
	}

// Methods
	bool Set()
	{
		if(m_bInUse)
			return false;
		m_hOldCursor = ::SetCursor(m_hWaitCursor);
		m_bInUse = true;
		return true;
	}

	bool Restore()
	{
		if(!m_bInUse)
			return false;
		::SetCursor(m_hOldCursor);
		m_bInUse = false;
		return true;
	}
};


///////////////////////////////////////////////////////////////////////////////
// CCustomWaitCursor - for custom and animated cursors

class CCustomWaitCursor : public CWaitCursor
{
public:
// Constructor/destructor
	CCustomWaitCursor(ATL::_U_STRINGorID cursor, bool bSet = true, HINSTANCE hInstance = NULL) : 
			CWaitCursor(false, IDC_WAIT, true)
	{
		if(hInstance == NULL)
		{
#if (_ATL_VER >= 0x0700)
			hInstance = ATL::_AtlBaseModule.GetResourceInstance();
#else // !(_ATL_VER >= 0x0700)
			hInstance = _Module.GetResourceInstance();
#endif // !(_ATL_VER >= 0x0700)
		}
		m_hWaitCursor = (HCURSOR)::LoadImage(hInstance, cursor.m_lpstr, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE);

		if(bSet)
			Set();
	}

	~CCustomWaitCursor()
	{
		Restore();
#if !defined(_WIN32_WCE) || ((_WIN32_WCE >= 0x400) && !(defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)))
		::DestroyCursor(m_hWaitCursor);
#endif // !defined(_WIN32_WCE) || ((_WIN32_WCE >= 0x400) && !(defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)))
	}
};


///////////////////////////////////////////////////////////////////////////////
// CMultiPaneStatusBarCtrl - Status Bar with multiple panes

template <class T, class TBase = CStatusBarCtrl>
class ATL_NO_VTABLE CMultiPaneStatusBarCtrlImpl : public ATL::CWindowImpl< T, TBase >
{
public:
	DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

// Data
	enum { m_cxPaneMargin = 3 };

	int m_nPanes;
	int* m_pPane;

// Constructor/destructor
	CMultiPaneStatusBarCtrlImpl() : m_nPanes(0), m_pPane(NULL)
	{ }

	~CMultiPaneStatusBarCtrlImpl()
	{
		delete [] m_pPane;
	}

// Methods
	HWND Create(HWND hWndParent, LPCTSTR lpstrText, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | SBARS_SIZEGRIP, UINT nID = ATL_IDW_STATUS_BAR)
	{
#if (_MSC_VER >= 1300)
		return ATL::CWindowImpl< T, TBase >::Create(hWndParent, rcDefault, lpstrText, dwStyle, 0, nID);
#else // !(_MSC_VER >= 1300)
		typedef ATL::CWindowImpl< T, TBase >   _baseClass;
		return _baseClass::Create(hWndParent, rcDefault, lpstrText, dwStyle, 0, nID);
#endif // !(_MSC_VER >= 1300)
	}

	HWND Create(HWND hWndParent, UINT nTextID = ATL_IDS_IDLEMESSAGE, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | SBARS_SIZEGRIP, UINT nID = ATL_IDW_STATUS_BAR)
	{
		const int cchMax = 128;   // max text length is 127 for status bars (+1 for null)
		TCHAR szText[cchMax];
		szText[0] = 0;
#if (_ATL_VER >= 0x0700)
		::LoadString(ATL::_AtlBaseModule.GetResourceInstance(), nTextID, szText, cchMax);
#else // !(_ATL_VER >= 0x0700)
		::LoadString(_Module.GetResourceInstance(), nTextID, szText, cchMax);
#endif // !(_ATL_VER >= 0x0700)
		return Create(hWndParent, szText, dwStyle, nID);
	}

	BOOL SetPanes(int* pPanes, int nPanes, bool bSetText = true)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nPanes > 0);

		m_nPanes = nPanes;
		delete [] m_pPane;
		m_pPane = NULL;

		ATLTRY(m_pPane = new int[nPanes]);
		ATLASSERT(m_pPane != NULL);
		if(m_pPane == NULL)
			return FALSE;
		memcpy(m_pPane, pPanes, nPanes * sizeof(int));

		int* pPanesPos = (int*)_alloca(nPanes * sizeof(int));

		// get status bar DC and set font
		CClientDC dc(m_hWnd);
		HFONT hOldFont = dc.SelectFont(GetFont());

		// get status bar borders
		int arrBorders[3] = { 0 };
		GetBorders(arrBorders);

		const int cchBuff = 128;
		TCHAR szBuff[cchBuff] = { 0 };
		SIZE size = { 0, 0 };
		int cxLeft = arrBorders[0];

		// calculate right edge of each part
		for(int i = 0; i < nPanes; i++)
		{
			if(pPanes[i] == ID_DEFAULT_PANE)
			{
				// make very large, will be resized later
				pPanesPos[i] = INT_MAX / 2;
			}
			else
			{
#if (_ATL_VER >= 0x0700)
				::LoadString(ATL::_AtlBaseModule.GetResourceInstance(), pPanes[i], szBuff, cchBuff);
#else // !(_ATL_VER >= 0x0700)
				::LoadString(_Module.GetResourceInstance(), pPanes[i], szBuff, cchBuff);
#endif // !(_ATL_VER >= 0x0700)
				dc.GetTextExtent(szBuff, lstrlen(szBuff), &size);
				T* pT = static_cast<T*>(this);
				pT;
				pPanesPos[i] = cxLeft + size.cx + arrBorders[2] + 2 * pT->m_cxPaneMargin;
			}
			cxLeft = pPanesPos[i];
		}

		BOOL bRet = SetParts(nPanes, pPanesPos);

		if(bRet && bSetText)
		{
			for(int i = 0; i < nPanes; i++)
			{
				if(pPanes[i] != ID_DEFAULT_PANE)
				{
#if (_ATL_VER >= 0x0700)
					::LoadString(ATL::_AtlBaseModule.GetResourceInstance(), pPanes[i], szBuff, cchBuff);
#else // !(_ATL_VER >= 0x0700)
					::LoadString(_Module.GetResourceInstance(), pPanes[i], szBuff, cchBuff);
#endif // !(_ATL_VER >= 0x0700)
					SetPaneText(m_pPane[i], szBuff);
				}
			}
		}

		dc.SelectFont(hOldFont);
		return bRet;
	}

	bool GetPaneTextLength(int nPaneID, int* pcchLength = NULL, int* pnType = NULL) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return false;

		int nLength = GetTextLength(nIndex, pnType);
		if(pcchLength != NULL)
			*pcchLength = nLength;

		return true;
	}

	BOOL GetPaneText(int nPaneID, LPTSTR lpstrText, int* pcchLength = NULL, int* pnType = NULL) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		int nLength = GetText(nIndex, lpstrText, pnType);
		if(pcchLength != NULL)
			*pcchLength = nLength;

		return TRUE;
	}

	BOOL SetPaneText(int nPaneID, LPCTSTR lpstrText, int nType = 0)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		return SetText(nIndex, lpstrText, nType);
	}

	BOOL GetPaneRect(int nPaneID, LPRECT lpRect) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		return GetRect(nIndex, lpRect);
	}

	BOOL SetPaneWidth(int nPaneID, int cxWidth)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nPaneID != ID_DEFAULT_PANE);   // Can't resize this one
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		// get pane positions
		int* pPanesPos = (int*)_alloca(m_nPanes * sizeof(int));
		GetParts(m_nPanes, pPanesPos);
		// calculate offset
		int cxPaneWidth = pPanesPos[nIndex] - ((nIndex == 0) ? 0 : pPanesPos[nIndex - 1]);
		int cxOff = cxWidth - cxPaneWidth;
		// find variable width pane
		int nDef = m_nPanes;
		for(int i = 0; i < m_nPanes; i++)
		{
			if(m_pPane[i] == ID_DEFAULT_PANE)
			{
				nDef = i;
				break;
			}
		}
		// resize
		if(nIndex < nDef)   // before default pane
		{
			for(int i = nIndex; i < nDef; i++)
				pPanesPos[i] += cxOff;
				
		}
		else			// after default one
		{
			for(int i = nDef; i < nIndex; i++)
				pPanesPos[i] -= cxOff;
		}
		// set pane postions
		return SetParts(m_nPanes, pPanesPos);
	}

#if (_WIN32_IE >= 0x0400) && !defined(_WIN32_WCE)
	BOOL GetPaneTipText(int nPaneID, LPTSTR lpstrText, int nSize) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		GetTipText(nIndex, lpstrText, nSize);
		return TRUE;
	}

	BOOL SetPaneTipText(int nPaneID, LPCTSTR lpstrText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		SetTipText(nIndex, lpstrText);
		return TRUE;
	}

	BOOL GetPaneIcon(int nPaneID, HICON& hIcon) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		hIcon = GetIcon(nIndex);
		return TRUE;
	}

	BOOL SetPaneIcon(int nPaneID, HICON hIcon)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int nIndex  = GetPaneIndexFromID(nPaneID);
		if(nIndex == -1)
			return FALSE;

		return SetIcon(nIndex, hIcon);
	}
#endif // (_WIN32_IE >= 0x0400) && !defined(_WIN32_WCE)

// Message map and handlers
	BEGIN_MSG_MAP(CMultiPaneStatusBarCtrlImpl< T >)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
	END_MSG_MAP()

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);
		if(wParam != SIZE_MINIMIZED && m_nPanes > 0)
		{
			T* pT = static_cast<T*>(this);
			pT->UpdatePanesLayout();
		}
		return lRet;
	}

// Implementation
	BOOL UpdatePanesLayout()
	{
		// get pane positions
		int* pPanesPos = pPanesPos = (int*)_alloca(m_nPanes * sizeof(int));
		int nRet = GetParts(m_nPanes, pPanesPos);
		ATLASSERT(nRet == m_nPanes);
		if(nRet != m_nPanes)
			return FALSE;
		// calculate offset
		RECT rcClient = { 0 };
		GetClientRect(&rcClient);
		int cxOff = rcClient.right - pPanesPos[m_nPanes - 1];
#ifndef _WIN32_WCE
		// Move panes left if size grip box is present
		if((GetStyle() & SBARS_SIZEGRIP) != 0)
			cxOff -= ::GetSystemMetrics(SM_CXVSCROLL) + ::GetSystemMetrics(SM_CXEDGE);
#endif // !_WIN32_WCE
		// find variable width pane
		int i;
		for(i = 0; i < m_nPanes; i++)
		{
			if(m_pPane[i] == ID_DEFAULT_PANE)
				break;
		}
		// resize all panes from the variable one to the right
		if((i < m_nPanes) && (pPanesPos[i] + cxOff) > ((i == 0) ? 0 : pPanesPos[i - 1]))
		{
			for(; i < m_nPanes; i++)
				pPanesPos[i] += cxOff;
		}
		// set pane postions
		return SetParts(m_nPanes, pPanesPos);
	}

	int GetPaneIndexFromID(int nPaneID) const
	{
		for(int i = 0; i < m_nPanes; i++)
		{
			if(m_pPane[i] == nPaneID)
				return i;
		}

		return -1;   // not found
	}
};

class CMultiPaneStatusBarCtrl : public CMultiPaneStatusBarCtrlImpl<CMultiPaneStatusBarCtrl>
{
public:
	DECLARE_WND_SUPERCLASS(_T("WTL_MultiPaneStatusBar"), GetWndClassName())
};


///////////////////////////////////////////////////////////////////////////////
// CPaneContainer - provides header with title and close button for panes

// pane container extended styles
#define PANECNT_NOCLOSEBUTTON	0x00000001
#define PANECNT_VERTICAL	0x00000002

template <class T, class TBase = ATL::CWindow, class TWinTraits = ATL::CControlWinTraits>
class ATL_NO_VTABLE CPaneContainerImpl : public ATL::CWindowImpl< T, TBase, TWinTraits >, public CCustomDraw< T >
{
public:
	DECLARE_WND_CLASS_EX(NULL, 0, -1)

// Constants
	enum
	{
		m_cxyBorder = 2,
		m_cxyTextOffset = 4,
		m_cxyBtnOffset = 1,

		m_cchTitle = 80,

		m_cxImageTB = 13,
		m_cyImageTB = 11,
		m_cxyBtnAddTB = 7,

		m_cxToolBar = m_cxImageTB + m_cxyBtnAddTB + m_cxyBorder + m_cxyBtnOffset,

		m_xBtnImageLeft = 6,
		m_yBtnImageTop = 5,
		m_xBtnImageRight = 12,
		m_yBtnImageBottom = 11,

		m_nCloseBtnID = ID_PANE_CLOSE
	};

// Data members
	CToolBarCtrl m_tb;
	ATL::CWindow m_wndClient;
	int m_cxyHeader;
	TCHAR m_szTitle[m_cchTitle];
	DWORD m_dwExtendedStyle;   // Pane container specific extended styles


// Constructor
	CPaneContainerImpl() : m_cxyHeader(0), m_dwExtendedStyle(0)
	{
		m_szTitle[0] = 0;
	}

// Attributes
	DWORD GetPaneContainerExtendedStyle() const
	{
		return m_dwExtendedStyle;
	}

	DWORD SetPaneContainerExtendedStyle(DWORD dwExtendedStyle, DWORD dwMask = 0)
	{
		DWORD dwPrevStyle = m_dwExtendedStyle;
		if(dwMask == 0)
			m_dwExtendedStyle = dwExtendedStyle;
		else
			m_dwExtendedStyle = (m_dwExtendedStyle & ~dwMask) | (dwExtendedStyle & dwMask);
		if(m_hWnd != NULL)
		{
			T* pT = static_cast<T*>(this);
			bool bUpdate = false;

			if(((dwPrevStyle & PANECNT_NOCLOSEBUTTON) != 0) && ((m_dwExtendedStyle & PANECNT_NOCLOSEBUTTON) == 0))   // add close button
			{
				pT->CreateCloseButton();
				bUpdate = true;
			}
			else if(((dwPrevStyle & PANECNT_NOCLOSEBUTTON) == 0) && ((m_dwExtendedStyle & PANECNT_NOCLOSEBUTTON) != 0))   // remove close button
			{
				pT->DestroyCloseButton();
				bUpdate = true;
			}

			if((dwPrevStyle & PANECNT_VERTICAL) != (m_dwExtendedStyle & PANECNT_VERTICAL))   // change orientation
			{
				pT->CalcSize();
				bUpdate = true;
			}

			if(bUpdate)
				pT->UpdateLayout();
		}
		return dwPrevStyle;
	}

	HWND GetClient() const
	{
		return m_wndClient;
	}

	HWND SetClient(HWND hWndClient)
	{
		HWND hWndOldClient = m_wndClient;
		m_wndClient = hWndClient;
		if(m_hWnd != NULL)
		{
			T* pT = static_cast<T*>(this);
			pT->UpdateLayout();
		}
		return hWndOldClient;
	}

	BOOL GetTitle(LPTSTR lpstrTitle, int cchLength) const
	{
		ATLASSERT(lpstrTitle != NULL);
		return (lstrcpyn(lpstrTitle, m_szTitle, cchLength) != NULL);
	}

	BOOL SetTitle(LPCTSTR lpstrTitle)
	{
		ATLASSERT(lpstrTitle != NULL);
		BOOL bRet = (lstrcpyn(m_szTitle, lpstrTitle, m_cchTitle) != NULL);
		if(bRet && m_hWnd != NULL)
		{
			T* pT = static_cast<T*>(this);
			pT->UpdateLayout();
		}
		return bRet;
	}

	int GetTitleLength() const
	{
		return lstrlen(m_szTitle);
	}

// Methods
	HWND Create(HWND hWndParent, LPCTSTR lpstrTitle = NULL, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			DWORD dwExStyle = 0, UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		if(lpstrTitle != NULL)
			lstrcpyn(m_szTitle, lpstrTitle, m_cchTitle);
#if (_MSC_VER >= 1300)
		return ATL::CWindowImpl< T, TBase, TWinTraits >::Create(hWndParent, rcDefault, NULL, dwStyle, dwExStyle, nID, lpCreateParam);
#else // !(_MSC_VER >= 1300)
		typedef ATL::CWindowImpl< T, TBase, TWinTraits >   _baseClass;
		return _baseClass::Create(hWndParent, rcDefault, NULL, dwStyle, dwExStyle, nID, lpCreateParam);
#endif // !(_MSC_VER >= 1300)
	}

	HWND Create(HWND hWndParent, UINT uTitleID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			DWORD dwExStyle = 0, UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		if(uTitleID != 0U)
#if (_ATL_VER >= 0x0700)
			::LoadString(ATL::_AtlBaseModule.GetResourceInstance(), uTitleID, m_szTitle, m_cchTitle);
#else // !(_ATL_VER >= 0x0700)
			::LoadString(_Module.GetResourceInstance(), uTitleID, m_szTitle, m_cchTitle);
#endif // !(_ATL_VER >= 0x0700)
#if (_MSC_VER >= 1300)
		return ATL::CWindowImpl< T, TBase, TWinTraits >::Create(hWndParent, rcDefault, NULL, dwStyle, dwExStyle, nID, lpCreateParam);
#else // !(_MSC_VER >= 1300)
		typedef ATL::CWindowImpl< T, TBase, TWinTraits >   _baseClass;
		return _baseClass::Create(hWndParent, rcDefault, NULL, dwStyle, dwExStyle, nID, lpCreateParam);
#endif // !(_MSC_VER >= 1300)
	}

	BOOL EnableCloseButton(BOOL bEnable)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		T* pT = static_cast<T*>(this);
		pT;   // avoid level 4 warning
		return (m_tb.m_hWnd != NULL) ? m_tb.EnableButton(pT->m_nCloseBtnID, bEnable) : FALSE;
	}

	void UpdateLayout()
	{
		RECT rcClient = { 0 };
		GetClientRect(&rcClient);
		T* pT = static_cast<T*>(this);
		pT->UpdateLayout(rcClient.right, rcClient.bottom);
	}

// Message map and handlers
	BEGIN_MSG_MAP(CPaneContainerImpl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
#ifndef _WIN32_WCE
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
#endif // !_WIN32_WCE
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->CalcSize();

		if((m_dwExtendedStyle & PANECNT_NOCLOSEBUTTON) == 0)
			pT->CreateCloseButton();

		return 0;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->UpdateLayout(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(m_wndClient.m_hWnd != NULL)
			m_wndClient.SetFocus();
		return 0;
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1;   // no background needed
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		if(wParam != NULL)
		{
			pT->DrawPaneTitle((HDC)wParam);

			if(m_wndClient.m_hWnd == NULL)   // no client window
				pT->DrawPane((HDC)wParam);
		}
		else
		{
			CPaintDC dc(m_hWnd);
			pT->DrawPaneTitle(dc.m_hDC);

			if(m_wndClient.m_hWnd == NULL)   // no client window
				pT->DrawPane(dc.m_hDC);
		}

		return 0;
	}

	LRESULT OnNotify(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if(m_tb.m_hWnd == NULL)
		{
			bHandled = FALSE;
			return 1;
		}

		T* pT = static_cast<T*>(this);
		pT;
		LPNMHDR lpnmh = (LPNMHDR)lParam;
		LRESULT lRet = 0;

		// pass toolbar custom draw notifications to the base class
		if(lpnmh->code == NM_CUSTOMDRAW && lpnmh->hwndFrom == m_tb.m_hWnd)
			lRet = CCustomDraw< T >::OnCustomDraw(0, lpnmh, bHandled);
#ifndef _WIN32_WCE
		// tooltip notifications come with the tooltip window handle and button ID,
		// pass them to the parent if we don't handle them
		else if(lpnmh->code == TTN_GETDISPINFO && lpnmh->idFrom == pT->m_nCloseBtnID)
			bHandled = pT->GetToolTipText(lpnmh);
#endif // !_WIN32_WCE
		// only let notifications not from the toolbar go to the parent
		else if(lpnmh->hwndFrom != m_tb.m_hWnd && lpnmh->idFrom != pT->m_nCloseBtnID)
			bHandled = FALSE;

		return lRet;
	}

	LRESULT OnCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// if command comes from the close button, substitute HWND of the pane container instead
		if(m_tb.m_hWnd != NULL && (HWND)lParam == m_tb.m_hWnd)
			return ::SendMessage(GetParent(), WM_COMMAND, wParam, (LPARAM)m_hWnd);

		bHandled = FALSE;
		return 1;
	}

// Custom draw overrides
	DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
	{
		return CDRF_NOTIFYITEMDRAW;   // we need per-item notifications
	}

	DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
	{
		CDCHandle dc = lpNMCustomDraw->hdc;
#if (_WIN32_IE >= 0x0400)
		RECT& rc = lpNMCustomDraw->rc;
#else // !(_WIN32_IE >= 0x0400)
		RECT rc;
		m_tb.GetItemRect(0, &rc);
#endif // !(_WIN32_IE >= 0x0400)

		dc.FillRect(&rc, COLOR_3DFACE);

		return CDRF_NOTIFYPOSTPAINT;
	}

	DWORD OnItemPostPaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
	{
		CDCHandle dc = lpNMCustomDraw->hdc;
#if (_WIN32_IE >= 0x0400)
		RECT& rc = lpNMCustomDraw->rc;
#else // !(_WIN32_IE >= 0x0400)
		RECT rc = { 0 };
		m_tb.GetItemRect(0, &rc);
#endif // !(_WIN32_IE >= 0x0400)

		RECT rcImage = { m_xBtnImageLeft, m_yBtnImageTop, m_xBtnImageRight + 1, m_yBtnImageBottom + 1 };
		::OffsetRect(&rcImage, rc.left, rc.top);
		T* pT = static_cast<T*>(this);

		if((lpNMCustomDraw->uItemState & CDIS_DISABLED) != 0)
		{
			RECT rcShadow = rcImage;
			::OffsetRect(&rcShadow, 1, 1);
			CPen pen1;
			pen1.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_3DHILIGHT));
			pT->DrawButtonImage(dc, rcShadow, pen1);
			CPen pen2;
			pen2.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_3DSHADOW));
			pT->DrawButtonImage(dc, rcImage, pen2);
		}
		else
		{
			if((lpNMCustomDraw->uItemState & CDIS_SELECTED) != 0)
				::OffsetRect(&rcImage, 1, 1);
			CPen pen;
			pen.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_BTNTEXT));
			pT->DrawButtonImage(dc, rcImage, pen);
		}

		return CDRF_DODEFAULT;   // continue with the default item painting
	}

// Implementation - overrideable methods
	void UpdateLayout(int cxWidth, int cyHeight)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		RECT rect = { 0 };

		if(IsVertical())
		{
			::SetRect(&rect, 0, 0, m_cxyHeader, cyHeight);
			if(m_tb.m_hWnd != NULL)
				m_tb.SetWindowPos(NULL, m_cxyBorder, m_cxyBorder + m_cxyBtnOffset, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

			if(m_wndClient.m_hWnd != NULL)
				m_wndClient.SetWindowPos(NULL, m_cxyHeader, 0, cxWidth - m_cxyHeader, cyHeight, SWP_NOZORDER);
			else
				rect.right = cxWidth;
		}
		else
		{
			::SetRect(&rect, 0, 0, cxWidth, m_cxyHeader);
			if(m_tb.m_hWnd != NULL)
				m_tb.SetWindowPos(NULL, rect.right - m_cxToolBar, m_cxyBorder + m_cxyBtnOffset, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

			if(m_wndClient.m_hWnd != NULL)
				m_wndClient.SetWindowPos(NULL, 0, m_cxyHeader, cxWidth, cyHeight - m_cxyHeader, SWP_NOZORDER);
			else
				rect.bottom = cyHeight;
		}

		InvalidateRect(&rect);
	}

	void CreateCloseButton()
	{
		ATLASSERT(m_tb.m_hWnd == NULL);
		// create toolbar for the "x" button
		m_tb.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN | CCS_NOMOVEY | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT, 0);
		ATLASSERT(m_tb.IsWindow());

		if(m_tb.m_hWnd != NULL)
		{
			T* pT = static_cast<T*>(this);
			pT;   // avoid level 4 warning

			m_tb.SetButtonStructSize();

			TBBUTTON tbbtn = { 0 };
			tbbtn.idCommand = pT->m_nCloseBtnID;
			tbbtn.fsState = TBSTATE_ENABLED;
			tbbtn.fsStyle = TBSTYLE_BUTTON;
			m_tb.AddButtons(1, &tbbtn);

			m_tb.SetBitmapSize(m_cxImageTB, m_cyImageTB);
			m_tb.SetButtonSize(m_cxImageTB + m_cxyBtnAddTB, m_cyImageTB + m_cxyBtnAddTB);

			if(IsVertical())
				m_tb.SetWindowPos(NULL, m_cxyBorder + m_cxyBtnOffset, m_cxyBorder + m_cxyBtnOffset, m_cxImageTB + m_cxyBtnAddTB, m_cyImageTB + m_cxyBtnAddTB, SWP_NOZORDER | SWP_NOACTIVATE);
			else
				m_tb.SetWindowPos(NULL, 0, 0, m_cxImageTB + m_cxyBtnAddTB, m_cyImageTB + m_cxyBtnAddTB, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		}
	}

	void DestroyCloseButton()
	{
		if(m_tb.m_hWnd != NULL)
			m_tb.DestroyWindow();
	}

	void CalcSize()
	{
		T* pT = static_cast<T*>(this);
		CFontHandle font = pT->GetTitleFont();
		LOGFONT lf = { 0 };
		font.GetLogFont(lf);
		if(IsVertical())
		{
			m_cxyHeader = m_cxImageTB + m_cxyBtnAddTB + m_cxyBorder;
		}
		else
		{
			int cyFont = abs(lf.lfHeight) + m_cxyBorder + 2 * m_cxyTextOffset;
			int cyBtn = m_cyImageTB + m_cxyBtnAddTB + m_cxyBorder + 2 * m_cxyBtnOffset;
			m_cxyHeader = max(cyFont, cyBtn);
		}
	}

	HFONT GetTitleFont() const
	{
		return AtlGetDefaultGuiFont();
	}

#ifndef _WIN32_WCE
	BOOL GetToolTipText(LPNMHDR /*lpnmh*/)
	{
		return FALSE;
	}
#endif // !_WIN32_WCE

	void DrawPaneTitle(CDCHandle dc)
	{
		RECT rect = { 0 };
		GetClientRect(&rect);

		if(IsVertical())
		{
			rect.right = rect.left + m_cxyHeader;
			dc.DrawEdge(&rect, EDGE_ETCHED, BF_LEFT | BF_TOP | BF_BOTTOM | BF_ADJUST);
			dc.FillRect(&rect, COLOR_3DFACE);
		}
		else
		{
			rect.bottom = rect.top + m_cxyHeader;
			dc.DrawEdge(&rect, EDGE_ETCHED, BF_LEFT | BF_TOP | BF_RIGHT | BF_ADJUST);
			dc.FillRect(&rect, COLOR_3DFACE);
			// draw title only for horizontal pane container
			dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
			dc.SetBkMode(TRANSPARENT);
			T* pT = static_cast<T*>(this);
			HFONT hFontOld = dc.SelectFont(pT->GetTitleFont());
			rect.left += m_cxyTextOffset;
			rect.right -= m_cxyTextOffset;
			if(m_tb.m_hWnd != NULL)
				rect.right -= m_cxToolBar;;
#ifndef _WIN32_WCE
			dc.DrawText(m_szTitle, -1, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
#else // CE specific
			dc.DrawText(m_szTitle, -1, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
#endif // _WIN32_WCE
			dc.SelectFont(hFontOld);
		}
	}

	// called only if pane is empty
	void DrawPane(CDCHandle dc)
	{
		RECT rect = { 0 };
		GetClientRect(&rect);
		if(IsVertical())
			rect.left += m_cxyHeader;
		else
			rect.top += m_cxyHeader;
		if((GetExStyle() & WS_EX_CLIENTEDGE) == 0)
			dc.DrawEdge(&rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);
		dc.FillRect(&rect, COLOR_APPWORKSPACE);
	}

	// drawing helper - draws "x" button image
	void DrawButtonImage(CDCHandle dc, RECT& rcImage, HPEN hPen)
	{
#if !defined(_WIN32_WCE) || (_WIN32_WCE >= 400)
		HPEN hPenOld = dc.SelectPen(hPen);

		dc.MoveTo(rcImage.left, rcImage.top);
		dc.LineTo(rcImage.right, rcImage.bottom);
		dc.MoveTo(rcImage.left + 1, rcImage.top);
		dc.LineTo(rcImage.right + 1, rcImage.bottom);

		dc.MoveTo(rcImage.left, rcImage.bottom - 1);
		dc.LineTo(rcImage.right, rcImage.top - 1);
		dc.MoveTo(rcImage.left + 1, rcImage.bottom - 1);
		dc.LineTo(rcImage.right + 1, rcImage.top - 1);

		dc.SelectPen(hPenOld);
#else // (_WIN32_WCE < 400)
		rcImage;
		hPen;
		// no support for the "x" button image
#endif // (_WIN32_WCE < 400)
	}

	bool IsVertical() const
	{
		return ((m_dwExtendedStyle & PANECNT_VERTICAL) != 0);
	}
};

class CPaneContainer : public CPaneContainerImpl<CPaneContainer>
{
public:
	DECLARE_WND_CLASS_EX(_T("WTL_PaneContainer"), 0, -1)
};


///////////////////////////////////////////////////////////////////////////////
// CSortListViewCtrl - implements sorting for a listview control

// sort listview extended styles
#define SORTLV_USESHELLBITMAPS	0x00000001

// Notification sent to parent when sort column is changed by user clicking header.  
#define SLVN_SORTCHANGED	(LVN_FIRST-201)

// A LPNMSORTLISTVIEW is sent with the SLVN_SORTCHANGED notification
typedef struct tagNMSORTLISTVIEW
{
    NMHDR hdr;
    int iNewSortColumn;
    int iOldSortColumn;
} NMSORTLISTVIEW, *LPNMSORTLISTVIEW;

// Column sort types. Can be set on a per-column basis with the SetColumnSortType method.
enum
{
	LVCOLSORT_NONE,
	LVCOLSORT_TEXT,   // default
	LVCOLSORT_TEXTNOCASE,
	LVCOLSORT_LONG,
	LVCOLSORT_DOUBLE,
	LVCOLSORT_DECIMAL,
	LVCOLSORT_DATETIME,
	LVCOLSORT_DATE,
	LVCOLSORT_TIME,
	LVCOLSORT_CUSTOM,
	LVCOLSORT_LAST = LVCOLSORT_CUSTOM
};


template <class T>
class CSortListViewImpl
{
public:
	enum
	{
		m_cchCmpTextMax = 32, // overrideable
		m_cxSortImage = 16,
		m_cySortImage = 15,
		m_cxSortArrow = 11,
		m_cySortArrow = 6,
		m_iSortUp = 0,        // index of sort bitmaps
		m_iSortDown = 1,
		m_nShellSortUpID = 133
	};

	// passed to LVCompare functions as lParam1 and lParam2 
	struct LVCompareParam
	{
		int iItem;
		DWORD_PTR dwItemData;
		union
		{
			long lValue;
			double dblValue;
			DECIMAL decValue;
			LPCTSTR pszValue;
		};
	};
	
	// passed to LVCompare functions as the lParamSort parameter
	struct LVSortInfo
	{
		T* pT;
		int iSortCol;
		bool bDescending;
	};

	bool m_bSortDescending;
	bool m_bCommCtrl6;
	int m_iSortColumn;
	CBitmap m_bmSort[2];
	int m_fmtOldSortCol;
	HBITMAP m_hbmOldSortCol;
	DWORD m_dwSortLVExtendedStyle;
	ATL::CSimpleArray<WORD> m_arrColSortType;
	bool m_bUseWaitCursor;
	
	CSortListViewImpl() :
			m_bSortDescending(false),
			m_bCommCtrl6(false),
			m_iSortColumn(-1), 
			m_fmtOldSortCol(0),
			m_hbmOldSortCol(NULL),
			m_dwSortLVExtendedStyle(SORTLV_USESHELLBITMAPS),
			m_bUseWaitCursor(true)
	{
#ifndef _WIN32_WCE
		DWORD dwMajor = 0;
		DWORD dwMinor = 0;
		HRESULT hRet = ATL::AtlGetCommCtrlVersion(&dwMajor, &dwMinor);
		m_bCommCtrl6 = SUCCEEDED(hRet) && dwMajor >= 6;
#endif // !_WIN32_WCE
	}
	
// Attributes
	void SetSortColumn(int iCol)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		CHeaderCtrl header = pT->GetHeader();
		ATLASSERT(header.m_hWnd != NULL);
		ATLASSERT(iCol >= -1 && iCol < m_arrColSortType.GetSize());

		int iOldSortCol = m_iSortColumn;
		m_iSortColumn = iCol;
		if(m_bCommCtrl6)
		{
#ifndef HDF_SORTUP
			const int HDF_SORTUP = 0x0400;	
#endif // HDF_SORTUP
#ifndef HDF_SORTDOWN
			const int HDF_SORTDOWN = 0x0200;	
#endif // HDF_SORTDOWN
			const int nMask = HDF_SORTUP | HDF_SORTDOWN;
			HDITEM hditem = { HDI_FORMAT };
			if(iOldSortCol != iCol && iOldSortCol >= 0 && header.GetItem(iOldSortCol, &hditem))
			{
				hditem.fmt &= ~nMask;
				header.SetItem(iOldSortCol, &hditem);
			}
			if(iCol >= 0 && header.GetItem(iCol, &hditem))
			{
				hditem.fmt &= ~nMask;
				hditem.fmt |= m_bSortDescending ? HDF_SORTDOWN : HDF_SORTUP;
				header.SetItem(iCol, &hditem);
			}
			return;
		}

		if(m_bmSort[m_iSortUp].IsNull())
			pT->CreateSortBitmaps();

		// restore previous sort column's bitmap, if any, and format
		HDITEM hditem = { HDI_BITMAP | HDI_FORMAT };
		if(iOldSortCol != iCol && iOldSortCol >= 0)
		{
			hditem.hbm = m_hbmOldSortCol;
			hditem.fmt = m_fmtOldSortCol;
			header.SetItem(iOldSortCol, &hditem);
		}

		// save new sort column's bitmap and format, and add our sort bitmap
		if(iCol >= 0 && header.GetItem(iCol, &hditem))
		{
			if(iOldSortCol != iCol)
			{
				m_fmtOldSortCol = hditem.fmt;
				m_hbmOldSortCol = hditem.hbm;
			}
			hditem.fmt &= ~HDF_IMAGE;
			hditem.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT;
			int i = m_bSortDescending ? m_iSortDown : m_iSortUp;
			hditem.hbm = m_bmSort[i];
			header.SetItem(iCol, &hditem);
		}
	}

	int GetSortColumn() const
	{
		return m_iSortColumn;
	}

	void SetColumnSortType(int iCol, WORD wType)
	{
		ATLASSERT(iCol >= 0 && iCol < m_arrColSortType.GetSize());
		ATLASSERT(wType >= LVCOLSORT_NONE && wType <= LVCOLSORT_LAST);
		m_arrColSortType[iCol] = wType;
	}

	WORD GetColumnSortType(int iCol) const
	{
		ATLASSERT((iCol >= 0) && iCol < m_arrColSortType.GetSize());
		return m_arrColSortType[iCol];
	}

	int GetColumnCount() const
	{
		const T* pT = static_cast<const T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		CHeaderCtrl header = pT->GetHeader();
		return header.m_hWnd != NULL ? header.GetItemCount() : 0;
	}

	bool IsSortDescending() const
	{
		return m_bSortDescending;
	}

	DWORD GetSortListViewExtendedStyle() const
	{
		return m_dwSortLVExtendedStyle;
	}

	DWORD SetSortListViewExtendedStyle(DWORD dwExtendedStyle, DWORD dwMask = 0)
	{
		DWORD dwPrevStyle = m_dwSortLVExtendedStyle;
		if(dwMask == 0)
			m_dwSortLVExtendedStyle = dwExtendedStyle;
		else
			m_dwSortLVExtendedStyle = (m_dwSortLVExtendedStyle & ~dwMask) | (dwExtendedStyle & dwMask);
		return dwPrevStyle;
	}

// Operations
	bool DoSortItems(int iCol, bool bDescending = false)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		ATLASSERT(iCol >= 0 && iCol < m_arrColSortType.GetSize());

		WORD wType = m_arrColSortType[iCol];
		if(wType == LVCOLSORT_NONE)
			return false;

		int nCount = pT->GetItemCount();
		if(nCount < 2)
		{
			m_bSortDescending = bDescending;
			SetSortColumn(iCol);
			return true;
		}

		CWaitCursor waitCursor(false);
		if(m_bUseWaitCursor)
			waitCursor.Set();

		LVCompareParam* pParam = NULL;
		ATLTRY(pParam = new LVCompareParam[nCount]);
		PFNLVCOMPARE pFunc = NULL;
		TCHAR pszTemp[pT->m_cchCmpTextMax];
		bool bStrValue = false;

		switch(wType)
		{
		case LVCOLSORT_TEXT:
			pFunc = (PFNLVCOMPARE)pT->LVCompareText;
		case LVCOLSORT_TEXTNOCASE:
			if(pFunc == NULL)
				pFunc = (PFNLVCOMPARE)pT->LVCompareTextNoCase;
		case LVCOLSORT_CUSTOM:
			{
				if(pFunc == NULL)
					pFunc = (PFNLVCOMPARE)pT->LVCompareCustom;

				for(int i = 0; i < nCount; i++)
				{
					pParam[i].iItem = i;
					pParam[i].dwItemData = pT->GetItemData(i);
					pParam[i].pszValue = new TCHAR[pT->m_cchCmpTextMax];
					pT->GetItemText(i, iCol, (LPTSTR)pParam[i].pszValue, pT->m_cchCmpTextMax);
					pT->SetItemData(i, (DWORD_PTR)&pParam[i]);
				}
				bStrValue = true;
			}
			break;
		case LVCOLSORT_LONG:
			{
				pFunc = (PFNLVCOMPARE)pT->LVCompareLong;
				for(int i = 0; i < nCount; i++)
				{
					pParam[i].iItem = i;
					pParam[i].dwItemData = pT->GetItemData(i);
					pT->GetItemText(i, iCol, pszTemp, pT->m_cchCmpTextMax);
					pParam[i].lValue = pT->StrToLong(pszTemp);
					pT->SetItemData(i, (DWORD_PTR)&pParam[i]);
				}
			}
			break;
		case LVCOLSORT_DOUBLE:
			{
				pFunc = (PFNLVCOMPARE)pT->LVCompareDouble;
				for(int i = 0; i < nCount; i++)
				{
					pParam[i].iItem = i;
					pParam[i].dwItemData = pT->GetItemData(i);
					pT->GetItemText(i, iCol, pszTemp, pT->m_cchCmpTextMax);
					pParam[i].dblValue = pT->StrToDouble(pszTemp);
					pT->SetItemData(i, (DWORD_PTR)&pParam[i]);
				}
			}
			break;
		case LVCOLSORT_DECIMAL:
			{
				pFunc = (PFNLVCOMPARE)pT->LVCompareDecimal;
				for(int i = 0; i < nCount; i++)
				{
					pParam[i].iItem = i;
					pParam[i].dwItemData = pT->GetItemData(i);
					pT->GetItemText(i, iCol, pszTemp, pT->m_cchCmpTextMax);
					pT->StrToDecimal(pszTemp, &pParam[i].decValue);
					pT->SetItemData(i, (DWORD_PTR)&pParam[i]);
				}
			}
			break;
		case LVCOLSORT_DATETIME:
		case LVCOLSORT_DATE:
		case LVCOLSORT_TIME:
			{
				pFunc = (PFNLVCOMPARE)pT->LVCompareDouble;
				DWORD dwFlags = LOCALE_NOUSEROVERRIDE;
				if(wType == LVCOLSORT_DATE)
					dwFlags |= VAR_DATEVALUEONLY;
				else if(wType == LVCOLSORT_TIME)
					dwFlags |= VAR_TIMEVALUEONLY;
				for(int i = 0; i < nCount; i++)
				{
					pParam[i].iItem = i;
					pParam[i].dwItemData = pT->GetItemData(i);
					pT->GetItemText(i, iCol, pszTemp, pT->m_cchCmpTextMax);
					pParam[i].dblValue = pT->DateStrToDouble(pszTemp, dwFlags);
					pT->SetItemData(i, (DWORD_PTR)&pParam[i]);
				}
			}
			break;
		default:
			ATLTRACE2(atlTraceUI, 0, _T("Unknown value for sort type in CSortListViewImpl::DoSortItems()\n"));
			break;
		} // switch(wType)

		ATLASSERT(pFunc != NULL);
		LVSortInfo lvsi = { pT, iCol, bDescending };
		bool bRet = ((BOOL)pT->DefWindowProc(LVM_SORTITEMS, (WPARAM)&lvsi, (LPARAM)pFunc) != FALSE);
		for(int i = 0; i < nCount; i++)
		{
			DWORD_PTR dwItemData = pT->GetItemData(i);
			LVCompareParam* p = (LVCompareParam*)dwItemData;
			ATLASSERT(p != NULL);
			if(bStrValue)
				delete [] (TCHAR*)p->pszValue;
			pT->SetItemData(i, p->dwItemData);
		}
		delete [] pParam;

		if(bRet)
		{
			m_bSortDescending = bDescending;
			SetSortColumn(iCol);
		}

		if(m_bUseWaitCursor)
			waitCursor.Restore();

		return bRet;
	}

	void CreateSortBitmaps()
	{
		if((m_dwSortLVExtendedStyle & SORTLV_USESHELLBITMAPS) != 0)
		{
			bool bFree = false;
			LPCTSTR pszModule = _T("shell32.dll"); 
			HINSTANCE hShell = ::GetModuleHandle(pszModule);

			if (hShell == NULL)		
			{
				hShell = ::LoadLibrary(pszModule);
				bFree = true;
			}
 
			if (hShell != NULL)
			{
				bool bSuccess = true;
				for(int i = m_iSortUp; i <= m_iSortDown; i++)
				{
					if(!m_bmSort[i].IsNull())
						m_bmSort[i].DeleteObject();
					m_bmSort[i] = (HBITMAP)::LoadImage(hShell, MAKEINTRESOURCE(m_nShellSortUpID + i), 
#ifndef _WIN32_WCE
						IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
#else // CE specific
						IMAGE_BITMAP, 0, 0, 0);
#endif // _WIN32_WCE
					if(m_bmSort[i].IsNull())
					{
						bSuccess = false;
						break;
					}
				}
				if(bFree)
					::FreeLibrary(hShell);
				if(bSuccess)
					return;
			}
		}

		T* pT = static_cast<T*>(this);
		for(int i = m_iSortUp; i <= m_iSortDown; i++)
		{
			if(!m_bmSort[i].IsNull())
				m_bmSort[i].DeleteObject();

			CDC dcMem;
			CClientDC dc(::GetDesktopWindow());
			dcMem.CreateCompatibleDC(dc.m_hDC);
			m_bmSort[i].CreateCompatibleBitmap(dc.m_hDC, m_cxSortImage, m_cySortImage);
			HBITMAP hbmOld = dcMem.SelectBitmap(m_bmSort[i]);
			RECT rc = {0,0,m_cxSortImage, m_cySortImage};
			pT->DrawSortBitmap(dcMem.m_hDC, i, &rc);
			dcMem.SelectBitmap(hbmOld);
			dcMem.DeleteDC();
		}
	}

	void NotifyParentSortChanged(int iNewSortCol, int iOldSortCol)
	{
		T* pT = static_cast<T*>(this);
		int nID = pT->GetDlgCtrlID();
		NMSORTLISTVIEW nm = { { pT->m_hWnd, nID, SLVN_SORTCHANGED }, iNewSortCol, iOldSortCol };
		::SendMessage(pT->GetParent(), WM_NOTIFY, (WPARAM)nID, (LPARAM)&nm);
	}

// Overrideables
	int CompareItemsCustom(LVCompareParam* /*pItem1*/, LVCompareParam* /*pItem2*/, int /*iSortCol*/)
	{
		// pItem1 and pItem2 contain valid iItem, dwItemData, and pszValue members.
		// If item1 > item2 return 1, if item1 < item2 return -1, else return 0.
		return 0;
	}

	void DrawSortBitmap(CDCHandle dc, int iBitmap, LPRECT prc)
	{
		dc.FillRect(prc, ::GetSysColorBrush(COLOR_BTNFACE));	
		HBRUSH hbrOld = dc.SelectBrush(::GetSysColorBrush(COLOR_BTNSHADOW));
		CPen pen;
		pen.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_BTNSHADOW));
		HPEN hpenOld = dc.SelectPen(pen);
		POINT ptOrg = { (m_cxSortImage - m_cxSortArrow) / 2, (m_cySortImage - m_cySortArrow) / 2 };
		if(iBitmap == m_iSortUp)
		{
			POINT pts[3] = 
			{
				{ ptOrg.x + m_cxSortArrow / 2, ptOrg.y },
				{ ptOrg.x, ptOrg.y + m_cySortArrow - 1 }, 
				{ ptOrg.x + m_cxSortArrow - 1, ptOrg.y + m_cySortArrow - 1 }
			};
			dc.Polygon(pts, 3);
		}
		else
		{
			POINT pts[3] = 
			{
				{ ptOrg.x, ptOrg.y },
				{ ptOrg.x + m_cxSortArrow / 2, ptOrg.y + m_cySortArrow - 1 },
				{ ptOrg.x + m_cxSortArrow - 1, ptOrg.y }
			};
			dc.Polygon(pts, 3);
		}
		dc.SelectBrush(hbrOld);
		dc.SelectPen(hpenOld);
	}

	double DateStrToDouble(LPCTSTR lpstr, DWORD dwFlags)
	{
		ATLASSERT(lpstr != NULL);
		if(lpstr == NULL || lpstr[0] == _T('\0'))
			return 0;

		USES_CONVERSION;
		HRESULT hRet = E_FAIL;
		DATE dRet = 0;
		if (FAILED(hRet = ::VarDateFromStr((LPOLESTR)T2COLE(lpstr), LANG_USER_DEFAULT, dwFlags, &dRet)))
		{
			ATLTRACE2(atlTraceUI, 0, _T("VarDateFromStr failed with result of 0x%8.8X\n"), hRet);
			dRet = 0;
		}
		return dRet;
	}

	long StrToLong(LPCTSTR lpstr)
	{
		ATLASSERT(lpstr != NULL);
		if(lpstr == NULL || lpstr[0] == _T('\0'))
			return 0;
		
		USES_CONVERSION;
		HRESULT hRet = E_FAIL;
		long lRet = 0;
		if (FAILED(hRet = ::VarI4FromStr((LPOLESTR)T2COLE(lpstr), LANG_USER_DEFAULT, LOCALE_NOUSEROVERRIDE, &lRet)))
		{
			ATLTRACE2(atlTraceUI, 0, _T("VarI4FromStr failed with result of 0x%8.8X\n"), hRet);
			lRet = 0;
		}
		return lRet;
	}

	double StrToDouble(LPCTSTR lpstr)
	{
		ATLASSERT(lpstr != NULL);
		if(lpstr == NULL || lpstr[0] == _T('\0'))
			return 0;

		USES_CONVERSION;
		HRESULT hRet = E_FAIL;
		double dblRet = 0;
		if (FAILED(hRet = ::VarR8FromStr((LPOLESTR)T2COLE(lpstr), LANG_USER_DEFAULT, LOCALE_NOUSEROVERRIDE, &dblRet)))
		{
			ATLTRACE2(atlTraceUI, 0, _T("VarR8FromStr failed with result of 0x%8.8X\n"), hRet);
			dblRet = 0;
		}
		return dblRet;
	}

	bool StrToDecimal(LPCTSTR lpstr, DECIMAL* pDecimal)
	{
		ATLASSERT(lpstr != NULL);
		ATLASSERT(pDecimal != NULL);
		if(lpstr == NULL || pDecimal == NULL)
			return false;

		USES_CONVERSION;
		HRESULT hRet = E_FAIL;
		if (FAILED(hRet = ::VarDecFromStr((LPOLESTR)T2COLE(lpstr), LANG_USER_DEFAULT, LOCALE_NOUSEROVERRIDE, pDecimal)))
		{
			ATLTRACE2(atlTraceUI, 0, _T("VarDecFromStr failed with result of 0x%8.8X\n"), hRet);
			pDecimal->Lo64 = 0;
			pDecimal->Hi32 = 0;
			pDecimal->signscale = 0;
			return false;
		}
		return true;
	}

// Overrideable PFNLVCOMPARE functions
	static int CALLBACK LVCompareText(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		ATLASSERT(lParam1 != NULL && lParam2 != NULL && lParamSort != NULL);

		LVCompareParam* pParam1 = (LVCompareParam*)lParam1;
		LVCompareParam* pParam2 = (LVCompareParam*)lParam2;
		LVSortInfo* pInfo = (LVSortInfo*)lParamSort;
		
		int nRet = lstrcmp(pParam1->pszValue, pParam2->pszValue);
		return pInfo->bDescending ? -nRet : nRet;
	}

	static int CALLBACK LVCompareTextNoCase(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		ATLASSERT(lParam1 != NULL && lParam2 != NULL && lParamSort != NULL);

		LVCompareParam* pParam1 = (LVCompareParam*)lParam1;
		LVCompareParam* pParam2 = (LVCompareParam*)lParam2;
		LVSortInfo* pInfo = (LVSortInfo*)lParamSort;
		
		int nRet = lstrcmpi(pParam1->pszValue, pParam2->pszValue);
		return pInfo->bDescending ? -nRet : nRet;
	}

	static int CALLBACK LVCompareLong(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		ATLASSERT(lParam1 != NULL && lParam2 != NULL && lParamSort != NULL);

		LVCompareParam* pParam1 = (LVCompareParam*)lParam1;
		LVCompareParam* pParam2 = (LVCompareParam*)lParam2;
		LVSortInfo* pInfo = (LVSortInfo*)lParamSort;
		
		int nRet = 0;
		if(pParam1->lValue > pParam2->lValue)
			nRet = 1;
		else if(pParam1->lValue < pParam2->lValue)
			nRet = -1;
		return pInfo->bDescending ? -nRet : nRet;
	}

	static int CALLBACK LVCompareDouble(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		ATLASSERT(lParam1 != NULL && lParam2 != NULL && lParamSort != NULL);

		LVCompareParam* pParam1 = (LVCompareParam*)lParam1;
		LVCompareParam* pParam2 = (LVCompareParam*)lParam2;
		LVSortInfo* pInfo = (LVSortInfo*)lParamSort;
		
		int nRet = 0;
		if(pParam1->dblValue > pParam2->dblValue)
			nRet = 1;
		else if(pParam1->dblValue < pParam2->dblValue)
			nRet = -1;
		return pInfo->bDescending ? -nRet : nRet;
	}

	static int CALLBACK LVCompareCustom(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		ATLASSERT(lParam1 != NULL && lParam2 != NULL && lParamSort != NULL);

		LVCompareParam* pParam1 = (LVCompareParam*)lParam1;
		LVCompareParam* pParam2 = (LVCompareParam*)lParam2;
		LVSortInfo* pInfo = (LVSortInfo*)lParamSort;
		
		int nRet = pInfo->pT->CompareItemsCustom(pParam1, pParam2, pInfo->iSortCol);
		return pInfo->bDescending ? -nRet : nRet;
	}

#ifndef _WIN32_WCE
	static int CALLBACK LVCompareDecimal(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		ATLASSERT(lParam1 != NULL && lParam2 != NULL && lParamSort != NULL);

		LVCompareParam* pParam1 = (LVCompareParam*)lParam1;
		LVCompareParam* pParam2 = (LVCompareParam*)lParam2;
		LVSortInfo* pInfo = (LVSortInfo*)lParamSort;
		
		int nRet = (int)::VarDecCmp(&pParam1->decValue, &pParam2->decValue);
		nRet--;
		return pInfo->bDescending ? -nRet : nRet;
	}
#else
	// Compare mantissas, ignore sign and scale
	static int CompareMantissas(const DECIMAL& decLeft, const DECIMAL& decRight)
	{
		if (decLeft.Hi32 < decRight.Hi32)
		{
			return -1;
		}
		if (decLeft.Hi32 > decRight.Hi32)
		{
			return 1;
		}
		// Here, decLeft.Hi32 == decRight.Hi32
		if (decLeft.Lo64 < decRight.Lo64)
		{
			return -1;
		}
		if (decLeft.Lo64 > decRight.Lo64)
		{
			return 1;
		}
		return 0;
	}

	// return values: VARCMP_LT, VARCMP_EQ, VARCMP_GT, VARCMP_NULL
	static HRESULT VarDecCmp(const DECIMAL* pdecLeft, const DECIMAL* pdecRight)
	{
		static const ULONG powersOfTen[] =
		{
			10ul,
			100ul,
			1000ul,
			10000ul,
			100000ul,
			1000000ul,
			10000000ul,
			100000000ul,
			1000000000ul
		};
		static const int largestPower = sizeof(powersOfTen) / sizeof(powersOfTen[0]);
		if (!pdecLeft || !pdecRight)
		{
			return VARCMP_NULL;
		}
		
		// Degenerate case - at least one comparand is of the form
		// [+-]0*10^N (denormalized zero)
		bool bLeftZero = (!pdecLeft->Lo64 && !pdecLeft->Hi32);
		bool bRightZero = (!pdecRight->Lo64 && !pdecRight->Hi32);
		if (bLeftZero && bRightZero)
		{
			return VARCMP_EQ;
		}
		bool bLeftNeg = ((pdecLeft->sign & DECIMAL_NEG) != 0);
		bool bRightNeg = ((pdecRight->sign & DECIMAL_NEG) != 0);
		if (bLeftZero)
		{
			return (bRightNeg ? VARCMP_GT : VARCMP_LT);
		}
		// This also covers the case where the comparands have different signs
		if (bRightZero || bLeftNeg != bRightNeg)
		{
			return (bLeftNeg ? VARCMP_LT : VARCMP_GT);
		}

		// Here both comparands have the same sign and need to be compared
		// on mantissa and scale. The result is obvious when
		// 1. Scales are equal (then compare mantissas)
		// 2. A number with smaller scale is also the one with larger mantissa
		//    (then this number is obviously larger)
		// In the remaining case, we would multiply the number with smaller
		// scale by 10 and simultaneously increment its scale (which amounts to
		// adding trailing zeros after decimal point), until the numbers fall under
		// one of the two cases above
		DECIMAL temp;
		bool bInvert = bLeftNeg; // the final result needs to be inverted
		if (pdecLeft->scale < pdecRight->scale)
		{
			temp = *pdecLeft;
		}
		else
		{
			temp = *pdecRight;
			pdecRight = pdecLeft;
			bInvert = !bInvert;
		}

		// Now temp is the number with smaller (or equal) scale, and
		// we can modify it freely without touching original parameters
		int comp;
		while ((comp = CompareMantissas(temp, *pdecRight)) < 0 &&
			temp.scale < pdecRight->scale)
		{
			// Multiply by an appropriate power of 10
			int scaleDiff = pdecRight->scale - temp.scale;
			if (scaleDiff > largestPower)
			{
				// Keep the multiplier representable in 32bit
				scaleDiff = largestPower;
			}
			DWORDLONG power = powersOfTen[scaleDiff - 1];
			// Multiply temp's mantissa by power
			DWORDLONG product = temp.Lo32 * power;
			ULONG carry = static_cast<ULONG>(product >> 32);
			temp.Lo32  = static_cast<ULONG>(product);
			product = temp.Mid32 * power + carry;
			carry = static_cast<ULONG>(product >> 32);
			temp.Mid32 = static_cast<ULONG>(product);
			product = temp.Hi32 * power + carry;
			if (static_cast<ULONG>(product >> 32))
			{
				// Multiplication overflowed - pdecLeft is clearly larger
				break;
			}
			temp.Hi32 = static_cast<ULONG>(product);
			temp.scale = (BYTE)(temp.scale + scaleDiff);
		}
		if (temp.scale < pdecRight->scale)
		{
			comp = 1;
		}
		if (bInvert)
		{
			comp = -comp;
		}
		return (comp > 0 ? VARCMP_GT : comp < 0 ? VARCMP_LT : VARCMP_EQ);
	}

	static int CALLBACK LVCompareDecimal(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		ATLASSERT(lParam1 != NULL && lParam2 != NULL && lParamSort != NULL);

		LVCompareParam* pParam1 = (LVCompareParam*)lParam1;
		LVCompareParam* pParam2 = (LVCompareParam*)lParam2;
		LVSortInfo* pInfo = (LVSortInfo*)lParamSort;
		
		int nRet = (int)VarDecCmp(&pParam1->decValue, &pParam2->decValue);
		nRet--;
		return pInfo->bDescending ? -nRet : nRet;
	}
#endif // !_WIN32_WCE

	BEGIN_MSG_MAP(CSortListViewImpl)
		MESSAGE_HANDLER(LVM_INSERTCOLUMN, OnInsertColumn)
		MESSAGE_HANDLER(LVM_DELETECOLUMN, OnDeleteColumn)
		NOTIFY_CODE_HANDLER(HDN_ITEMCLICKA, OnHeaderItemClick)
		NOTIFY_CODE_HANDLER(HDN_ITEMCLICKW, OnHeaderItemClick)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
	END_MSG_MAP()

	LRESULT OnInsertColumn(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)	
	{
		T* pT = static_cast<T*>(this);
		LRESULT lRet = pT->DefWindowProc(uMsg, wParam, lParam);
		if(lRet == -1)
			return -1;

		WORD wType = 0;
		m_arrColSortType.Add(wType);
		int nCount = m_arrColSortType.GetSize();
		ATLASSERT(nCount == GetColumnCount());

		for(int i = nCount - 1; i > lRet; i--)
			m_arrColSortType[i] = m_arrColSortType[i - 1];
		m_arrColSortType[(int)lRet] = LVCOLSORT_TEXT;

		if(lRet <= m_iSortColumn)
			m_iSortColumn++;

		return lRet;
	}

	LRESULT OnDeleteColumn(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)	
	{
		T* pT = static_cast<T*>(this);
		LRESULT lRet = pT->DefWindowProc(uMsg, wParam, lParam);
		if(lRet == 0)
			return 0;

		int iCol = (int)wParam; 
		if(m_iSortColumn == iCol)
			m_iSortColumn = -1;
		else if(m_iSortColumn > iCol)
			m_iSortColumn--;
		m_arrColSortType.RemoveAt(iCol);

		return lRet;
	}

	LRESULT OnHeaderItemClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
	{
		LPNMHEADER p = (LPNMHEADER)pnmh;
		if(p->iButton == 0)
		{
			int iOld = m_iSortColumn;
			bool bDescending = (m_iSortColumn == p->iItem) ? !m_bSortDescending : false;
			if(DoSortItems(p->iItem, bDescending))
				NotifyParentSortChanged(p->iItem, iOld);				
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
#ifndef _WIN32_WCE
		if(wParam == SPI_SETNONCLIENTMETRICS)
			GetSystemSettings();
#else  // CE specific
		wParam; // avoid level 4 warning
		GetSystemSettings();
#endif // _WIN32_WCE
		bHandled = FALSE;
		return 0;
	}

	void GetSystemSettings()
	{
		if(!m_bCommCtrl6 && !m_bmSort[m_iSortUp].IsNull())
		{
			T* pT = static_cast<T*>(this);
			pT->CreateSortBitmaps();
			if(m_iSortColumn != -1)
				SetSortColumn(m_iSortColumn);
		}
	}

};


typedef ATL::CWinTraits<WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | LVS_REPORT | LVS_SHOWSELALWAYS , WS_EX_CLIENTEDGE>   CSortListViewCtrlTraits;

template <class T, class TBase = CListViewCtrl, class TWinTraits = CSortListViewCtrlTraits>
class ATL_NO_VTABLE CSortListViewCtrlImpl: public ATL::CWindowImpl<T, TBase, TWinTraits>, public CSortListViewImpl<T>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

	bool SortItems(int iCol, bool bDescending = false)
	{
		return DoSortItems(iCol, bDescending);
	}
		
	BEGIN_MSG_MAP(CSortListViewCtrlImpl)
		MESSAGE_HANDLER(LVM_INSERTCOLUMN, CSortListViewImpl<T>::OnInsertColumn)
		MESSAGE_HANDLER(LVM_DELETECOLUMN, CSortListViewImpl<T>::OnDeleteColumn)
		NOTIFY_CODE_HANDLER(HDN_ITEMCLICKA, CSortListViewImpl<T>::OnHeaderItemClick)
		NOTIFY_CODE_HANDLER(HDN_ITEMCLICKW, CSortListViewImpl<T>::OnHeaderItemClick)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, CSortListViewImpl<T>::OnSettingChange)
	END_MSG_MAP()
};

class CSortListViewCtrl : public CSortListViewCtrlImpl<CSortListViewCtrl>
{
public:
	DECLARE_WND_SUPERCLASS(_T("WTL_SortListViewCtrl"), GetWndClassName())
};

}; // namespace WTL

#endif // __ATLCTRLX_H__
