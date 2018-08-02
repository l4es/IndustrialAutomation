#ifndef __ROLLOUTCTRL__H
#define __ROLLOUTCTRL__H

#pragma once

/////////////////////////////////////////////////////
//	Rollout control and container implementation
//	Based on the CodeProject Bjoern Graf article
//
//	Written by Alexey Shirshov (qqqaqa@mail.ru)
//	Copyright (c) 2003 Alexey Shirshov
//
//


#if (WINVER < 0x0500) && ((_WIN32_WINNT < 0x0400) || !defined(_WIN32_WINNT))
#include <ZMOUSE.H>
#endif


#ifndef __ATLCTRLS_H__
#error rolloutctrl.h requires atlctrls.h to be included first
#endif

#ifndef __ATLSCRL_H__
#error rolloutctrl.h requires atlscrl.h to be included first
#endif

#ifndef __ATLMISC_H__
#error rolloutctrl.h requires atlmisc.h to be included first
#endif

#include "chf.h"

namespace AWTL
{

/////////////////////////////////////////////////////
// CRolloutCtrlButton
//
//

class CRolloutCtrlButton : public CWindowImpl<CRolloutCtrlButton, CButton>
{
	typedef CWindowImpl<CRolloutCtrlButton, CButton> _baseClass;
	typedef CRolloutCtrlButton _thisClass;

private:
	// Internal states
	bool m_fChecked;
	bool m_fPressed;
	bool m_fEnabled;
	COLORREF clrTextColor;
	COLORREF clrTextColorShadow;

public:
	DECLARE_WND_SUPERCLASS(_T("AWTL_RolloutButton"), GetWndClassName())

	// Construction
	CRolloutCtrlButton() : m_fChecked(true), m_fPressed(false),
		m_fEnabled(false)
	{}

/*	// overridden to provide proper initialization
	HWND Create(HWND hWndParent, LPCTSTR szWindowName,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			UINT nID = 0U, LPVOID lpCreateParam = NULL)
	{
		CWindowImpl<T, TBase, TWinTraits>::Create(hWndParent, rcDefault, szWindowName,
			TWinTraits::GetWndStyle(dwStyle), TWinTraits::GetWndExStyle(dwExStyle),
			nID, lpCreateParam);
		if (IsWindow()){
			Init();
		}
		return m_hWnd;
	}
*/
	BOOL SubclassWindow(HWND hWnd)
	{
		BOOL bRet = _baseClass::SubclassWindow(hWnd);
		if (bRet) Init();
		return bRet;
	}

// Attributes
	bool IsChecked() const
	{
		return m_fChecked;
	}

	void SetChecked(bool fChecked)
	{
		m_fChecked = fChecked;
	}

private:
	// Initialization
	void Init()
	{
		// We need this style to prevent Windows from painting the button
		ModifyStyle(0, WS_GROUP|WS_TABSTOP|BS_OWNERDRAW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
		m_fEnabled = IsWindowEnabled() ? true : false;
		clrTextColor = ::GetSysColor(COLOR_3DHILIGHT);
		clrTextColorShadow = ::GetSysColor(COLOR_3DSHADOW);
	}

// Overrideables
	void DoPaint(CDCHandle dc)
	{
		CRect rect;
		CRect rcClient;
		GetClientRect(&rcClient);
		rect = rcClient;

		const int title_len = GetWindowTextLength();
		PTSTR szTitle = (PTSTR)_alloca(title_len+1);
		GetWindowText(szTitle, title_len+1);

		const HFONT hOldFont = dc.SelectFont(GetFont());

		//CSize ptTextExtent;
		//dc.GetTextExtent(szTitle, title_len, &ptTextExtent);

		dc.DrawFrameControl(&rcClient, DFC_BUTTON,DFCS_BUTTONPUSH|DFCS_FLAT|
			(m_fPressed?DFCS_PUSHED:0)|(m_fChecked?DFCS_CHECKED:0)|
			(m_fEnabled?DFCS_INACTIVE:0));
		
		rect.top += 2 * ::GetSystemMetrics(SM_CYBORDER);
		
		if (m_fPressed)
			rect.OffsetRect(1, 1);

		const int oldBkMode = dc.SetBkMode(TRANSPARENT);

		if (m_fEnabled){
			dc.DrawText(szTitle, title_len, &rect, DT_CENTER|DT_TOP|DT_SINGLELINE);
		}
		else{
			COLORREF clrOldText = dc.SetTextColor(clrTextColor);
			rect.OffsetRect(1, 1);
			dc.DrawText(szTitle, title_len, &rect, DT_CENTER|DT_TOP|DT_SINGLELINE);
			dc.SetTextColor(clrTextColorShadow);
			rect.OffsetRect(-1, -1);
			dc.DrawText(szTitle, title_len, &rect, DT_CENTER|DT_TOP|DT_SINGLELINE);
			dc.SetTextColor(clrOldText);
		}

		rect.top -= 2 * ::GetSystemMetrics(SM_CYBORDER);
		// draw expand/collapse button
/*		if (m_fChecked)
			lstrcpy(szTitle, _T("-"));
		else
			lstrcpy(szTitle, _T("+"));
		
		rect.left += 2 * ::GetSystemMetrics(SM_CXEDGE);
		
		if (m_fEnabled){
			dc.DrawText(szTitle, 1, &rect, DT_LEFT|DT_TOP|DT_SINGLELINE);
		}
		else{
			COLORREF clrOldText = dc.SetTextColor(clrTextColor);
			rect.OffsetRect(1, 1);
			dc.DrawText(szTitle, 1, &rect, DT_LEFT|DT_TOP|DT_SINGLELINE);
			dc.SetTextColor(clrTextColorShadow);
			rect.OffsetRect(-1, -1);
			dc.DrawText(szTitle, 1, &rect, DT_LEFT|DT_TOP|DT_SINGLELINE);
			dc.SetTextColor(clrOldText);
		}
*/
		rect.left += 2 * ::GetSystemMetrics(SM_CXEDGE);
		rect.top = rect.Height()/2-4;
		rect.right = rect.left + 9;
		rect.bottom = rect.top + 9;
		
		AWTL::DrawFrameControl(dc,&rect,DFC_CATEGORY,m_fChecked?DFCS_MINUS:DFCS_PLUS);
		
		dc.SelectFont(hOldFont);

		// draw focus rect
		if (::GetFocus() == m_hWnd){
			rect = rcClient;
			rect.DeflateRect(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));
			dc.DrawFocusRect(&rect);
		}
	}
	
protected:
// Message map and handlers
	BEGIN_MSG_MAP(_thisClass)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		MESSAGE_HANDLER(WM_ENABLE, OnEnable)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
//		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
		MESSAGE_HANDLER(WM_SETFOCUS, OnFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnFocus)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
//		MESSAGE_HANDLER(WM_MOVE, OnMove)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		Init();
		return 1;
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1;	// no background needed
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(wParam != NULL){
			//Handle WM_PRINTCLIENT
			DoPaint((HDC)wParam);
		}
		else{
			CPaintDC dc(m_hWnd);
			DoPaint(dc.m_hDC);
		}
		return 0;
	}

	LRESULT OnEnable(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		m_fEnabled = wParam?true:false;
		return 0;
	}

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
//		if(::GetCapture() == m_hWnd)
		{
			SetFocus();
			SetCapture();
			m_fPressed = true;
			Invalidate();
//			UpdateWindow();
		}
		return 0;
	}

	LRESULT OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
/*		if(::GetCapture() != m_hWnd)
			SetCapture();*/
		if(!m_fPressed){
			m_fPressed = true;
			Invalidate();
//			UpdateWindow();
		}
		return 0;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(m_fPressed){
			m_fChecked = !m_fChecked;
			m_fPressed = false;
			Invalidate();
			::SendMessage(GetParent(), WM_COMMAND,
				MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
		}
		::ReleaseCapture();
		return 0;
	}

	LRESULT OnCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(m_fPressed){
			m_fPressed = false;
			Invalidate();
			UpdateWindow();
		}
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnFocus(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		Invalidate();
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(wParam == VK_SPACE && !m_fPressed){
			m_fPressed = true;
			Invalidate();
		}
		return 0;
	}

	LRESULT OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(wParam == VK_SPACE && m_fPressed)
		{
			m_fPressed = false;
			m_fChecked = !m_fChecked;
			::SendMessage(GetParent(), WM_COMMAND,
				MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
			Invalidate();
		}
		return 0;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if(::GetCapture() == m_hWnd)
		{
			CPoint ptCursor(lParam);
			CRect rc;
			GetClientRect(&rc);
			const bool uPressed = rc.PtInRect(ptCursor) == TRUE;
			if(m_fPressed != uPressed){
				m_fPressed = uPressed;
				Invalidate();
			}
		}
		bHandled = FALSE;
		return 1;
	}

};	//end of CRolloutCtrlButton


/////////////////////////////////////////////////////
// Rollout - one pane holder
//
//

struct Rollout
{
	~Rollout()
	{
		if (rloButton.IsWindow())
			rloButton.DestroyWindow();
		else
			rloButton.m_hWnd = NULL;
	}

	HWND hWnd;
	//DWORD dwDefButton;
	CRolloutCtrlButton rloButton;
	
	Rollout(HWND Wnd,DWORD /*dwBtn*/)
	{
		hWnd = Wnd;
		//dwDefButton = dwBtn;
	}

	bool CreateButton(HWND hParent,CRect rc,HFONT hFont,PCTSTR szTitle)
	{		
		rc.DeflateRect(10,0);
		rc.top = 0;
		rc.bottom = 18;
		
		rloButton.Create(hParent,rc,szTitle,
			WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON|BS_NOTIFY);
		
		ATLASSERT(rloButton.IsWindow());

		rloButton.SetFont(hFont);

		return rloButton.IsWindow() == TRUE;
	}

/*	void Move(int dy)
	{
		CRect rc;
		GetClientRect(hWnd,&rc);
		MapWindowPoints(hWnd,GetParent(hWnd),(LPPOINT)&rc,2);
		SetWindowPos(hWnd,NULL,rc.left,rc.top+dy,0,0,
			SWP_NOZORDER|SWP_NOSIZE);
		//::SendMessage(hWnd,WM_MOVE,0,MAKELPARAM(rc.left,rc.top+dy));
	}
*/
};	//end of Rollout

/////////////////////////////////////////////////////
// CRolloutHolder - panes manager
//
//

class CRolloutHolder
{
private:
	int xMax;

public:

	CRolloutHolder()
	{
		xMax = 0;
	}
	
	~CRolloutHolder()
	{
		for(int i = 0;i < m_RolloutCtrls.GetSize();i++)
		{
			delete m_RolloutCtrls[i];
		}
	}

protected:
	CSimpleArray<Rollout*> m_RolloutCtrls;

	//Return the maximum width among all controls
	int GetMaxWidth() const
	{
		CRect r;
		::GetWindowRect(m_RolloutCtrls[0]->hWnd,&r);
		int maxim = r.Width();
		for(int i = 1;i < m_RolloutCtrls.GetSize();i++)
		{
			::GetWindowRect(m_RolloutCtrls[i]->hWnd,&r);
			if (maxim < r.Width())
				maxim = r.Width();
		}

		return maxim;
	}

public:
	// Return -1 if not found
	int FindByHandle(HWND hWndRollout) const
	{
		for(int i = 0;i < m_RolloutCtrls.GetSize();i++)
		{
			if (hWndRollout == m_RolloutCtrls[i]->hWnd)
				return i;
		}
		return -1;
	}
	
	// Return -1 if not found
	int FindByBtnHandle(HWND hBtnWnd) const
	{
		for(int i = 0;i < m_RolloutCtrls.GetSize();i++)
		{
			if (hBtnWnd == m_RolloutCtrls[i]->rloButton)
				return i;
		}
		return -1;
	}

	//Return -1, if error occur or number of controls in array
	int AddRollout(HWND hParent,HWND hWndRollout,PCTSTR szTitle,
		const CRect& rc,DWORD dwDefBtn = 0)
	{
		CWindow w(hWndRollout);

		_DASSERT(w.IsWindow()){
			return -1;
		}
		
		const DWORD style = w.GetWindowLongPtr(GWL_STYLE);
		const bool required = (style & WS_CHILD) == WS_CHILD;
		const bool nrequired = (style & DS_CONTROL) == DS_CONTROL &&
			(style & DS_MODALFRAME) == DS_MODALFRAME;

		ATLASSERT(required);
		ATLASSERT(!nrequired);

		_DASSERT(::IsWindow(hParent)){
			return -1;
		}

		Rollout* r = new Rollout(hWndRollout,dwDefBtn);
		const bool res = m_RolloutCtrls.Add(r) == TRUE;
		
		if (res){
			const int iCurIndx = m_RolloutCtrls.GetSize()-1;

			// Create button for rollout
			// see CreateButton function in Rollout struct
			if (m_RolloutCtrls[iCurIndx]->CreateButton(hParent,rc,
				AtlGetDefaultGuiFont(),szTitle)){
				xMax = GetMaxWidth();
			}
			return iCurIndx;
		}

		return -1;
	}

	//Return true, if control was deleted
	bool RemoveRollout(HWND hWndRollout)
	{
		_DASSERT(::IsWindow(hWndRollout)){
			return false;
		}

		const int idx = FindByHandle(hWndRollout);
		if (idx == -1)
			return false;

		return RemoveRollout(idx);
	}

	//Remove rollout by index
	bool RemoveRollout(int nIndex)
	{
		_DASSERT(nIndex >= 0 && nIndex < m_RolloutCtrls.GetSize()){
			return false;
		}

		HWND hWnd = m_RolloutCtrls[nIndex]->hWnd;

		_DASSERT(::IsWindow(hWnd)){
			return false;
		}

		delete m_RolloutCtrls[nIndex];
		const bool res = m_RolloutCtrls.RemoveAt(nIndex) == TRUE;
		if (res){
			xMax = GetMaxWidth();
		}

		return res;
	}

	//Return true, if all controls was deleted
	void RemoveAllRollouts()
	{
		m_RolloutCtrls.RemoveAll();
		xMax = 0;
	}
/*
	//Return true if set
	bool SetRolloutDefButton(HWND hRolloutCtrl,DWORD dwDefBtn)
	{
//		ATLASSERT(::IsWindow(hRolloutCtrl));
		const int idx = FindByHandle(hRolloutCtrl);
		if (idx == -1) return false;
		//m_RolloutCtrls[idx]->dwDefButton = dwDefBtn;
		return true;
	}

	//Return 0 if control not found
	DWORD GetRolloutDefButton(HWND hRolloutCtrl) const
	{
//		ATLASSERT(::IsWindow(hRolloutCtrl));
		const int idx = FindByHandle(hRolloutCtrl);
		if (idx == -1) return 0;
		return m_RolloutCtrls[idx]->dwDefButton;
	}
*/
	int GetRolloutCount() const
	{
		return m_RolloutCtrls.GetSize();
	}

	HWND GetRollout(int nIndex)
	{
		return m_RolloutCtrls[nIndex]->hWnd;
	}

	void ExpandRollout(HWND hWndRollout, bool fExpand = true, bool fUpdate = true)
	{
//		ATLASSERT(::IsWindow(hWndRollout));

		const int idx = FindByHandle(hWndRollout);
		if (idx != -1){
			ExpandRollout(idx,fExpand,fUpdate);
		}
	}

	void ExpandRollout(int nIndex, bool fExpand = true, bool fUpdate = true)
	{
		m_RolloutCtrls[nIndex]->rloButton.SetChecked(fExpand);

		if (fUpdate)
			m_RolloutCtrls[nIndex]->rloButton.Invalidate();
	}

	void ExpandAllRollouts(bool fExpand = true,bool fUpdate = true)
	{
		for(int i = 0; i < m_RolloutCtrls.GetSize(); i++)
			ExpandRollout(i, fExpand, fUpdate);
	}

	bool IsRolloutCollapsed(HWND hWndRollout) const
	{
		bool res = false;
		const int idx = FindByHandle(hWndRollout);
		if (idx != -1){
			res = IsRolloutCollapsed(idx);
		}
		return res;
	}

	bool IsRolloutCollapsed(int nIndex) const
	{
		return m_RolloutCtrls[nIndex]->rloButton.IsChecked();
	}

	void EnableRollout(HWND hWndRollout, bool fEnable)
	{
		ATLASSERT(::IsWindow(hWndRollout));
		const int idx = FindByHandle(hWndRollout);
		if (idx != -1){
			EnableRollout(idx, fEnable);
		}
	}

	void EnableRollout(int nIndex, bool fEnable)
	{
		::EnableWindow(m_RolloutCtrls[nIndex]->hWnd, fEnable);
	}

	bool IsRolloutEnabled(HWND hWndRollout) const
	{
		bool res = false;
		ATLASSERT(::IsWindow(hWndRollout));
		const int idx = FindByHandle(hWndRollout);
		if (idx != -1){
			res = IsRolloutEnabled(idx) == TRUE;
		}
		return res;
	}
	
	bool IsRolloutEnabled(int nIndex) const
	{
		return ::IsWindowEnabled(m_RolloutCtrls[nIndex]->hWnd) == TRUE;
	}

	int GetWidth() const
	{
		return xMax;
	}

};	//end of CRolloutHolder

// Messages
#define RCM_BASE			(WM_USER + 2)

// Request expand from parent
// WPARAM unused
// LPARAM unused
#define RCM_ISEXPANDED		(RCM_BASE + 0)

class ATL_NO_VTABLE CRolloutContainerT: public CRolloutHolder
{

protected:
	CSize outer_margins,inner_margins;
	int ySpacing;
	int yOffset;
	bool fTrans;
	//bool fResize;
	
public:
	
	// Construction
	CRolloutContainerT()
	{
		outer_margins.SetSize(4,4);
		inner_margins.SetSize(10,10);
		ySpacing = 6;
		yOffset = 0;
		fTrans = false;
		//fResize = false;
	}

	BOOL SubclassWindow(HWND)
	{
		return FALSE;
	}

	bool AddDialog(HWND hWnd,PCTSTR pszTitle,bool fExpand = false)
	{
		CWindow wnd = hWnd;
		_DASSERT(wnd.IsWindow()){
			return false;
		}

		CRect rc;
		
		wnd.GetClientRect(&rc);
		if (GetWidth() < rc.Width())
			RecalcWidth(rc.Width());
		else{
			rc.right = GetWidth();
			wnd.SetWindowPos(NULL,0,0,rc.right,rc.bottom,
				SWP_NOZORDER|SWP_NOMOVE);
		}

		rc.right += inner_margins.cx*2;
		rc.bottom += inner_margins.cy*2;
		rc.OffsetRect(outer_margins.cx,outer_margins.cy);
		
		const int idx = AddRollout(_GetThisWindow(),hWnd,pszTitle,rc);
		if (idx != -1){
			if (fExpand)
				return Expand(idx);
			else
				return Collapse(idx);
		}

		return false;
	}

	bool RemoveDialog(HWND hWnd)
	{
		bool b = RemoveRollout(hWnd);
		if (b){
			_Invalidate();
			RecalcButtons();
		}
		return b;
	}

	//Implementation in rolloutctrl.cpp
	bool Expand(int idx);

	//Implementation in rolloutctrl.cpp
	bool Collapse(int idx);

	//Implementation in rolloutctrl.cpp
	//called by CScrollWindowImpl
	void _DoPaint(CDCHandle dc);

protected:

	virtual void Init() = 0;

	//Implementation in rolloutctrl.cpp
	int GetTopForButton(int idx) const;
	
	//Implementation in rolloutctrl.cpp
	virtual int RecalcButtons() const;
	
	virtual BOOL _Invalidate() = 0;

	virtual HWND _GetThisWindow() = 0;

	//Implementation in rolloutctrl.cpp
	int RecalcWidth(int iWidth) const;

	int GetHeight() const
	{
		const int idx = GetRolloutCount()-1;
		
		int top = GetTopForButton(idx);
		
		Rollout& ro = *m_RolloutCtrls[idx];
		CRect r;
		//ro.rloButton.GetClientRect(&r);
		top += 9;//r.Height();

		if (IsRolloutCollapsed(idx)){
			::GetClientRect(ro.hWnd,&r);
		}
		else{
			r.SetRect(0,0,0,0);
		}
		
		r.bottom += inner_margins.cy*2;

		top += r.Height();

		//border 4*1
		top += 4;

		top += ySpacing;

		return top;
	}
};	//end of CRolloutContainerT

class CRolloutContainer:
	public CScrollWindowImpl<CRolloutContainer>,
	public CRolloutContainerT
{

	typedef CRolloutContainer _thisClass;
	typedef CScrollWindowImpl<CRolloutContainer> _baseClass;
public:
	DECLARE_WND_CLASS_EX(_T("AWTL_RolloutView"),0,COLOR_BTNFACE);
	
	BOOL PreTranslateMessage(PMSG pMsg)
	{
		return ::IsDialogMessage(m_hWnd, pMsg);
	}

	void DoPaint(CDCHandle dc)
	{
		_DoPaint(dc);
	}

protected:

	void Init(){}
	
	BOOL _Invalidate()
	{
		return Invalidate();
	}

	HWND _GetThisWindow()
	{
		return m_hWnd;
	}

	int RecalcButtons() const
	{
		int top = CRolloutContainerT::RecalcButtons();

		SIZE sz = {
			GetWidth() + outer_margins.cx*2+inner_margins.cx*2,
			top
		};
		const_cast<_thisClass*>(this)->SetScrollSize(sz,TRUE);

		return top;
	}

private:	
// Message map and handlers
	BEGIN_MSG_MAP(_thisClass)
		CHAIN_MSG_MAP(_baseClass)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		COMMAND_CODE_HANDLER(BN_CLICKED, OnRolloutCtrlClicked)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		Init();
		return 1;
	}

	LRESULT OnRolloutCtrlClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& bHandled)
	{
		const int idx = FindByBtnHandle(hWndCtl);
		if (idx != -1){
			if (!::SendMessage(m_RolloutCtrls[idx]->hWnd, RCM_ISEXPANDED, 0, 0)){
				if (IsRolloutCollapsed(idx))	
					Expand(idx);
				else
					Collapse(idx);
			}
		}
		return 1;
	}
};	//end of CRolloutContainer

class C3DStudioRolloutContainer:
	public CWindowImpl<C3DStudioRolloutContainer>,
	public CRolloutContainerT
{
private:
	typedef C3DStudioRolloutContainer _thisClass;
	typedef CWindowImpl<C3DStudioRolloutContainer> _baseClass;

	mutable int _height;
	int prev,cur;
	CPoint _ptOffset;
	CPoint _ptStartDrag;
	HBRUSH clrActiveCaption;
	HCURSOR _hCursorHand;
#if (WINVER < 0x0500) && ((_WIN32_WINNT < 0x0400) || !defined(_WIN32_WINNT))
	UINT m_uMsgMouseWheel;
#endif

public:
	DECLARE_WND_CLASS_EX(_T("AWTL_Rollout3DStudioView"),0,COLOR_BTNFACE);
	
	C3DStudioRolloutContainer()
	{
		_height = 0;
		_ptOffset.SetPoint(0,0);
		prev = 0;
		cur = 0;
		fTrans = true;
	}

	BOOL PreTranslateMessage(PMSG pMsg)
	{
		return ::IsDialogMessage(m_hWnd, pMsg);
	}

protected:
	
	RECT GetSliderRect() const
	{
		const int right = GetWidth()+outer_margins.cx*2+
			inner_margins.cx*2+4;

		CRect r;
		GetClientRect(&r);
		return CRect(right,2,right+6,r.Height()-2);
	}

	void InvalidateSlider()
	{
		CRect rc(GetSliderRect());
		CRect r;
		GetClientRect(&r);
		rc.top = 0;
		rc.bottom = r.Height();
		InvalidateRect(&rc);
	}

	//Implementation in rolloutctrl.cpp
	void Scroll(int newY);

	//Implementation in rolloutctrl.cpp
	void DoPaint(CDCHandle dc);

	void Init()
	{
		clrActiveCaption = ::GetSysColorBrush(COLOR_ACTIVECAPTION);

#if WINVER >= 0x0500
		_hCursorHand = ::LoadCursor(NULL, IDC_HAND);
#else
		_hCursorHand = ::LoadCursor(NULL, IDC_SIZENS);
#endif
#if (WINVER < 0x0500) && ((_WIN32_WINNT < 0x0400) || !defined(_WIN32_WINNT))
		m_uMsgMouseWheel = ::RegisterWindowMessage(MSH_MOUSEWHEEL);
#endif
	}

	BOOL _Invalidate()
	{
		return Invalidate();
	}

	HWND _GetThisWindow()
	{
		return m_hWnd;
	}

	int RecalcButtons() const
	{
		int top = CRolloutContainerT::RecalcButtons();

		_height = top-yOffset;

/*		SIZE sz = {
			GetWidth() + outer_margins.cx*2+inner_margins.cx*2,
			top
		};
		const_cast<_thisClass*>(this)->SetScrollSize(sz,TRUE);
*/
		return top;
	}

private:	
// Message map and handlers
	BEGIN_MSG_MAP(_thisClass)
		//CHAIN_MSG_MAP(_baseClass)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
#if (WINVER < 0x0500) && ((_WIN32_WINNT < 0x0400) || !defined(_WIN32_WINNT))
		MESSAGE_HANDLER(m_uMsgMouseWheel, OnMouseWheel)
#endif
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		COMMAND_CODE_HANDLER(BN_CLICKED, OnRolloutCtrlClicked)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		Init();
		return 1;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		CPaintDC dc(m_hWnd);
//		dc.SetViewportOrg(-_ptOffset.x, -_ptOffset.y);
		DoPaint((HDC)dc);
		return 1;
	}

	//Implementation in rolloutctrl.cpp
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);

	//Implementation in rolloutctrl.cpp
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);
	
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		CPoint p(lParam);
		CRect r(GetSliderRect());
		r.InflateRect(4,0);
		if (r.PtInRect(p)){
			SetCursor(_hCursorHand);
			CRect rc;
			GetClientRect(&rc);
			if (_height >= rc.Height()){
				SetCapture();
				_ptStartDrag = p;
			}
		}
		SetFocus();
		return 1;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (GetCapture() == m_hWnd){
			prev = _ptOffset.y;
			cur = 0;
			ReleaseCapture();
		}
		
		return 0;
	}

	//GET_WHEEL_DELTA_WPARAM
	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
#if (WINVER < 0x0500) && ((_WIN32_WINNT < 0x0400) || !defined(_WIN32_WINNT))
		const int iDelta = (uMsg == WM_MOUSEWHEEL)?(short)HIWORD(wParam):wParam;
#else
		const int iDelta = GET_WHEEL_DELTA_WPARAM(wParam);
#endif
		Scroll(-iDelta/5);
		prev = _ptOffset.y;
		cur = 0;
		return 1;
	}

	LRESULT OnRolloutCtrlClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& bHandled)
	{
		const int idx = FindByBtnHandle(hWndCtl);
		if (idx != -1){
			if (!::SendMessage(m_RolloutCtrls[idx]->hWnd, RCM_ISEXPANDED, 0, 0)){
				if (IsRolloutCollapsed(idx))	
					Expand(idx);
				else
					Collapse(idx);
			}
		}
		return 1;
	}
};	//end of C3DStudioRolloutContainer

};	//end of AWTL namespace

#endif // !defined(__ROLLOUTCTRL__H)
