#ifndef __DTPICKER__H
#define __DTPICKER__H

#pragma once

/////////////////////////////////////////////////////
//	Masked edit controls
//
//	Written by Alexey Shirshov (qqqaqa@mail.ru)
//	Copyright (c) 2003 Alexey Shirshov
//
//
//
//

#ifndef __ATLWIN_H__
#error maskededit.h requires altwin.h to be included first
#endif

#ifndef __ATLMISC_H__
#error maskededit.h requires altmisc.h to be included first
#endif

#include "other\MiniCalendarCtrl.h"

namespace AWTL
{

class CDateTimeCtrl : public CWindowImpl<CDateTimeCtrl,CStatic>,
	public COwnerDraw<CDateTimeCtrl>
{
private:
	typedef CWindowImpl<CDateTimeCtrl,CStatic> _baseClass;

	struct CMaskItem
	{
		CMaskItem()
		{
			data[0] = 0;
			separators[0] = 0;
			rc.SetRectEmpty();
		}
		TCHAR _type[10];
		CRect rc;
		TCHAR data[100];
		TCHAR separators[10];
	};

public:
	DECLARE_WND_CLASS(_T("AWTL_DateTimeCtrl"))
		
	CDateTimeCtrl()
	{
		_xOffset = 0;
		_iActiveItemIdx = 0;
		_fDrawButton = 0;
		fCancel = false;
		fBorder = false;
		Cash[0] = 0;
	}

	BOOL SubclassWindow(HWND hWnd)
	{
#if (defined(_DEBUG) && (WINVER > 0x0400))
		TCHAR buf[100];
		RealGetWindowClass(hWnd,buf,100);
		ATLASSERT(!lstrcmpi(buf,_T("STATIC")));
#endif
		
		BOOL b = _baseClass::SubclassWindow(hWnd);
		Init();
		return b;
	}

	void OnFinalMessage(HWND)
	{
		if (_edit.IsWindow())
			_edit.DestroyWindow();
		if (m_wndCalendar.IsWindow())
			m_wndCalendar.DestroyWindow();
	}

	void SetDateTime(LPSYSTEMTIME pST = NULL)
	{
		Cash[0] = 0;
		for(int i = 0;i < _items.GetSize();i++)
		{
			GetDateFormat(LOCALE_USER_DEFAULT,0,pST,
				_items[i]._type,_items[i].data,100);
			GetTimeFormat(LOCALE_USER_DEFAULT,0,pST,
				_items[i].data,_items[i].data,100);
		}
		Invalidate();
	}

	bool GetDateTime(LPSYSTEMTIME pST)
	{
		ATLASSERT(pST);
		
		TCHAR buf[500];
		GetDateTimeText(buf,500);

		CComVariant v(buf);		
		bool b = SUCCEEDED(v.ChangeType(VT_DATE));
		if (b)
			b = VariantTimeToSystemTime(v.date,pST) == TRUE;
		
		return b;
	}

	bool SetFormat(PTSTR pFormat)
	{
		//TCHAR seps[] = _T(", :/-\\().");
		PCTSTR pCur = pFormat;
		size_t pos = _tcscspn(pCur,GetSeps());
		CMaskItem* mi = 0;
		while(pos){
			ATLASSERT(pos < 5);
			mi = CreateItem(pCur,pCur+pos-1);
			ATLASSERT(mi);
			PCTSTR pNext = _tcscspnn(pCur+pos,GetSeps());
			if (pNext){
				ATLASSERT(pNext-pCur-pos < 10);
				memcpy(mi->separators,pCur+pos,(pNext-pCur-pos)*sizeof(TCHAR));
				mi->separators[pNext-pCur-pos] = 0;
				pCur = pNext;
				pos = _tcscspn(pCur,GetSeps());
			}
			else
				pos = 0;
			_items.Add(*mi);
			delete mi;
		}
/*		mi = CreateItem(pCur,pCur+pos-1);
		if (mi){
			_items.Add(*mi);
			delete mi;
		}*/
		return true;
	}

	bool GetFormat(PTSTR pBuf,size_t& sz,PTSTR pPref = _T(""))
	{
		if (pBuf == NULL) sz = 0;
		size_t cur_sz = 0;

		const size_t pref_sz = lstrlen(pPref);
		for(int i = 0;i < _items.GetSize();i++)
		{
			const size_t fsz = lstrlen(_items[i]._type);
			const size_t ssz = lstrlen(_items[i].separators);
			cur_sz += fsz+ssz+pref_sz;
			if (cur_sz <= sz && pBuf != NULL){
				lstrcat(pBuf,pPref);
				lstrcat(pBuf,_items[i]._type);
				lstrcat(pBuf,_items[i].separators);
			}
		}
		bool b = sz >= cur_sz;
		sz = cur_sz;
		return b;
	}

	bool SetBorder(bool b)
	{
		bool t = fBorder;
		fBorder = b;
		return t;
	}

	virtual PCTSTR GetSeps() const
	{
		return _T(", :;/-\\().");
	}

	static PCTSTR _tcscspnn(PCTSTR pBuf,PCTSTR pSeps)
	{
		for(PCTSTR pF = pBuf;*pF;pF++)
		{
			if (!_tcschr(pSeps,*pF)) return pF;
		}
		return NULL;
	}

protected:
	virtual void Init()
	{		
		_edit.Create(this,2,m_hWnd,&CWindow::rcDefault,
			NULL,WS_CHILD|ES_RIGHT|WS_TABSTOP/*|WS_BORDER*/);
		
		ATLASSERT(_edit.IsWindow());

		_edit.SetMargins(0,0);
		
		//SetFont(AtlGetDefaultGuiFont());
		_edit.SetFont(GetFont());
		
		ModifyStyle(SS_ENHMETAFILE|WS_BORDER, 
			SS_OWNERDRAW|SS_NOTIFY|WS_CLIPSIBLINGS|
			WS_CLIPCHILDREN);

		const DWORD dwStyle = WS_POPUP | 
			MCX_3DBORDER | /*MCX_MULTISELECT | */MCX_HIGHLIGHTTODAY |
			MCX_SHOWNONMONTHDAYS/* | MCX_TODAYBUTTON | MCX_NOSELBUTTON*/;

		CRect r;
		GetClientRect(&r);
		r.OffsetRect(0,r.Height());
		ClientToScreen(&r);
		m_wndCalendar.Create(m_hWnd,r,0,dwStyle);
		m_wndCalendar.SetNotifyWindow(m_hWnd);
		//::SetForegroundWindow(m_wndCalendar);
		
		ATLASSERT(m_wndCalendar.IsWindow());
	}

	int GetDateTimeText(PTSTR pBuf,int iBufSz)
	{
		ATLASSERT(pBuf);
		pBuf[0] = 0;
		const int CashSz = lstrlen(Cash);
		if (CashSz > 0){
			if (CashSz > iBufSz) return CashSz;
			lstrcpy(pBuf,Cash);
			return CashSz;
		}

		int iRealBufSz = 0;
		CRect rc,r;
		GetClientRect(&rc);
		CWindowDC dc(m_hWnd);
		//dc.SelectFont(AtlGetDefaultGuiFont());
		HFONT hOldFont = dc.SelectFont(GetFont());
		int iOffset = 0;
		for(int i = 0;i < _items.GetSize();i++)
		{
			r.SetRectEmpty();
			_items[i].rc.SetRectEmpty();
			dc.DrawText(_items[i].data,-1,&_items[i].rc,
				DT_CALCRECT|DT_SINGLELINE|DT_VCENTER);
			_items[i].rc.top = rc.top;
			_items[i].rc.bottom = rc.bottom;
			_items[i].rc.OffsetRect(iOffset,0);
			dc.DrawText(_items[i].separators,-1,&r,
				DT_CALCRECT|DT_SINGLELINE|DT_VCENTER);
			iOffset = _items[i].rc.right+r.right;
			
			iRealBufSz += lstrlen(_items[i].data);
			const int SepSz = lstrlen(_items[i].separators);
			iRealBufSz += SepSz;
			if (iRealBufSz < iBufSz){
				lstrcat(pBuf,_items[i].data);
				if (SepSz){
					lstrcat(pBuf,_items[i].separators);
				}
			}
		}

		dc.SelectFont(hOldFont);
		
		ATLASSERT(iRealBufSz < 500);
		
		if (iRealBufSz < iBufSz)
			lstrcpy(Cash,pBuf);
		
		return iRealBufSz;
	}

	CMaskItem* CreateItem(PCTSTR pBeg,PCTSTR pEnd)
	{
		TCHAR words[] = _T("yMmdHhst");
		if (!_tcschr(words,pBeg[0])) return NULL;

		for(PCTSTR pf = &pBeg[1];pf <= pEnd;pf++)
		{
			if (*pf != pBeg[0]) return NULL;
		}

		CMaskItem* mi = new CMaskItem;
		const size_t sz = pEnd-pBeg+sizeof(TCHAR);
		memcpy(mi->_type,pBeg,sz*sizeof(TCHAR));
		mi->_type[sz] = 0;
		return mi;
	}

	void ClearAllItems()
	{
		_items.RemoveAll();
	}

	void ActivateItem(int iIdx)
	{
		ATLASSERT(iIdx >= 0 && iIdx < _items.GetSize());
		CRect rc;
		GetClientRect(&rc);
		CRect r = _items[iIdx].rc;
		if (fBorder){
			r.OffsetRect(2,-1);
			r.DeflateRect(0,1);
			r.bottom -= 2;
		}
		_xOffset = rc.right-r.right-rc.Height();
		if (_xOffset > 0) _xOffset = 0;
		Invalidate();
		//CRect r = _items[iIdx].rc;
		
/*		if (lstrlen(_items[iIdx].data) > 2)
			r.right -= 6;

		if (_xOffset < 0)
			r.OffsetRect(-2,0);
*/		
		r.OffsetRect(_xOffset,(rc.Height()<17)?1:2);
		//ClientToScreen(r);
		_edit.SetMargins(0,0);
		_edit.SetWindowPos(NULL,&r,SWP_SHOWWINDOW);
		_edit.SetWindowText(_items[iIdx].data);
//		_edit.ShowWindow(SW_SHOW);
		_edit.SetFocus();
		if (_items[iIdx]._type[0] != _T('t'))
			_edit.ModifyStyle(0,ES_NUMBER);
		else
			_edit.ModifyStyle(ES_NUMBER,0);
		
		_iActiveItemIdx = iIdx;
	}

	int GetItemIdxByPos(POINT p)
	{
		for(int i = 0;i < _items.GetSize();i++)
		{
			const CRect& r = _items[i].rc;
			if (r.PtInRect(p)) return i;
		}
		return -1;
	}

	void NextItem()
	{
		::SetFocus(GetParent());
		if (_iActiveItemIdx+1 == _items.GetSize())
			ActivateItem(0);
		else
			ActivateItem(++_iActiveItemIdx);
	}

	void PrevItem()
	{
		::SetFocus(GetParent());
		if (_iActiveItemIdx == 0)
			ActivateItem(_items.GetSize()-1);
		else
			ActivateItem(--_iActiveItemIdx);
	}

	virtual void ShowCal()
	{
		SetFocus();
		ATLASSERT(m_wndCalendar.IsWindow());
		
		SYSTEMTIME st;
		GetDateTime(&st);
		st.wHour = 0;
		st.wMilliseconds = 0;
		st.wSecond = 0;
		st.wMinute = 0;
		m_wndCalendar.EnsureVisible(st);

		CRect r;
		GetClientRect(&r);
		r.OffsetRect(0,r.Height());
		ClientToScreen(&r);
		m_wndCalendar.SetWindowPos(NULL,&r,SWP_SHOWWINDOW|SWP_NOSIZE);
	}

protected:
	BEGIN_MSG_MAP(CDateTimeCtrl)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		//MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkgnd)
		MESSAGE_HANDLER(WM_SETFOCUS,OnSetFocus)
		MESSAGE_HANDLER(WM_SETTEXT,OnSetText)
		MESSAGE_HANDLER(WM_GETTEXT,OnGetText)
		MESSAGE_HANDLER(WM_GETTEXTLENGTH, OnGetTextLength)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLBttonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP,OnLBttonUp)
		MESSAGE_HANDLER(WM_MOUSELEAVE,OnLBttonUp)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_SETFONT, OnSetFont)
		//MESSAGE_HANDLER(MCXN_LOSEFOCUS, OnCalendarKillFocus)
		//MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize)
		NOTIFY_CODE_HANDLER(MCXN_SELCHANGED,OnCalendarSelChange)
		NOTIFY_CODE_HANDLER(MCXN_LOSEFOCUS, OnCalendarKillFocus)
		CHAIN_MSG_MAP_ALT(COwnerDraw<CDateTimeCtrl>, 1)
		ALT_MSG_MAP(2)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnEditKillFocus)
		MESSAGE_HANDLER(WM_SETFOCUS, OnEditSetFocus)
		MESSAGE_HANDLER(WM_KEYDOWN, OnEditKeyDown)
		MESSAGE_HANDLER(WM_GETDLGCODE, OnEditGetDlgCode)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		Init();
		return 0;
	}

	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (_items.GetSize() > 0){
			ActivateItem(0);
		}
		return 0;
	}

	LRESULT OnCalendarKillFocus(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	{
		m_wndCalendar.ShowWindow(SW_HIDE);
		//SetDateTime(&m_wndCalendar.GetSelectedDateTime());
		::SetFocus(::GetNextWindow(m_hWnd,GW_HWNDNEXT));
		return 0;
	}

	LRESULT OnCalendarSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		NMMCXSELCHANGED* pnch = (NMMCXSELCHANGED*)pnmh;		
		SYSTEMTIME st;
		bool b = GetDateTime(&st);
		ATLASSERT(b);
		pnch->dateFrom.wHour = st.wHour;
		pnch->dateFrom.wMilliseconds = st.wMilliseconds;
		pnch->dateFrom.wMinute = st.wMinute;
		pnch->dateFrom.wSecond = st.wSecond;
		SetDateTime(&pnch->dateFrom);
		return 0;
	}

	LRESULT OnSetText(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnGetText(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return GetDateTimeText((PTSTR)lParam,wParam);
	}

	LRESULT OnGetTextLength(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		TCHAR buf[1000];		
		return GetDateTimeText(buf,1000);
	}
	
	LRESULT OnSetFont(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		_edit.SetFont((HFONT)wParam);
		bHandled = FALSE;
		return 0;
	}
	
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		return (LRESULT)::GetStockObject(WHITE_BRUSH);
	}
	
	LRESULT OnLBttonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		CPoint p(lParam);
		CRect rc;
		GetClientRect(&rc);
		rc.left = rc.right-rc.Height();
		if (rc.PtInRect(p)){
			_fDrawButton = 1;
			Invalidate();
			TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT),TME_LEAVE,m_hWnd};
			_TrackMouseEvent(&tme);
			ShowCal();
		}
		else{
			const int idx = GetItemIdxByPos(p);
			if (idx != -1){
				ActivateItem(idx);
			}
		}
		return 0;
	}

	LRESULT OnLBttonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		_fDrawButton = 2;
		Invalidate();
		_fDrawButton = 0;
		return 0;
	}

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		switch(wParam){
		case VK_F4:
			ShowCal();
			break;
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnNcPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcCalcSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		return 0;
	}
	
	void DrawItem(LPDRAWITEMSTRUCT lpDIS)
	{
		CDCHandle dc = lpDIS->hDC;
		const CRect& r = lpDIS->rcItem;
		
		CRect rcForCombo(r.right-r.Height(),r.top,r.right,r.bottom);
		CRect rcForMainArea(r.left,r.top,r.right-r.Height(),r.bottom);

		int nState = 0;
		if (_fDrawButton == 1)
			nState = DFCS_PUSHED;

		dc.DrawFrameControl(&rcForCombo,DFC_SCROLL,nState|DFCS_SCROLLDOWN);

		if (!_fDrawButton){
			CRect rc = rcForMainArea;
			if (fBorder){
				rc.DeflateRect(2,2);
			}
			dc.FillSolidRect(rc.left,rc.top,rc.right,rc.bottom,
				RGB(255,255,255));
			
			TCHAR buf[500];
			const int l = GetDateTimeText(buf,500);
			if (l > 0){
				dc.SetBkMode(TRANSPARENT);
				rc.left += _xOffset;
				dc.DrawText(buf,lstrlen(buf),&rc,DT_SINGLELINE|DT_VCENTER);
			}

			if (fBorder){
				dc.DrawEdge(&rcForMainArea,EDGE_SUNKEN,BF_RECT);
			}
		}
	}

///////////////////////////////////
//Handlers for edit control

	LRESULT OnEditKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		_xOffset = 0;
		_edit.ShowWindow(SW_HIDE);
		char buf[10];
		const int c = ::GetWindowTextA(_edit,buf,10);
		const int type_len = lstrlen(_items[_iActiveItemIdx]._type);
		if (c < type_len){
			if (_items[_iActiveItemIdx]._type[0] != _T('t')){
				TCHAR buf1[100] = _T("%0");
				buf1[2] = TCHAR(type_len)+_T('0');
				buf1[3] = _T('s');
				buf1[4] = 0;				
				//_stprintf(buf1,);
				wsprintf(_items[_iActiveItemIdx].data,buf1,buf);
				//lstrcpy(_items[_iActiveItemIdx].data,buf1);
			}
		}
		else{
			USES_CONVERSION;
			lstrcpy(_items[_iActiveItemIdx].data,A2T(buf));
		}
		Cash[0] = 0;
		Invalidate();
		
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnEditSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		_edit.SetSelAll();
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnEditKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (GetAsyncKeyState(VK_CONTROL) < 0){
			switch(wParam){
			case VK_LEFT:
				PrevItem();
				break;
			case VK_RIGHT:
				NextItem();
				break;
			}
			fCancel = false;
			return 0;
		}
		
		switch(wParam){
		case VK_TAB:
		case VK_RETURN:
			fCancel = false;
			break;
		case VK_ESCAPE:
			fCancel = true;
			break;
		case VK_F4:
			ShowCal();
		default:
			bHandled = FALSE;
			return 0;
		}
		
		::SetFocus(::GetNextWindow(m_hWnd,GW_HWNDNEXT));
//		::SetFocus(GetParent());
		return 0;
	}

	LRESULT OnEditGetDlgCode(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		return DLGC_WANTALLKEYS;
	}

protected:
	CSimpleArray<CMaskItem> _items;
	CMiniCalendarCtrl m_wndCalendar;
	TCHAR Cash[500];
	bool fCancel;

private:
	int _xOffset;
	int _iActiveItemIdx;
	int _fDrawButton;
	CContainedWindowT<CEdit> _edit;
	bool fBorder;
};	//end of CDateTimeCtrl

class CDateTimeRangeCtrl : public CDateTimeCtrl
{
private:
	typedef CDateTimeCtrl _baseClass;

public:
	DECLARE_WND_SUPERCLASS(_T("AWTL_DateTimeRangeCtrl"),GetWndClassName())

private:
	
	BEGIN_MSG_MAP(CDateTimeRangeCtrl)
		NOTIFY_CODE_HANDLER(MCXN_SELCHANGED,OnCalendarSelChange)
		CHAIN_MSG_MAP(_baseClass)
		ALT_MSG_MAP(2)
		CHAIN_MSG_MAP_ALT(_baseClass,2)
	END_MSG_MAP()

	LRESULT OnCalendarSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		NMMCXSELCHANGED* pnch = (NMMCXSELCHANGED*)pnmh;		
		SetDateTime(&pnch->dateFrom,&pnch->dateTo);
		return 0;
	}

	void Init()
	{
		_baseClass::Init();
		_baseClass::SetFormat(_T("yyyy/MM/dd - yyyy/MM/dd"));

		SYSTEMTIME st,st1;
		GetSystemTime(&st);
		st1 = st;
		st1.wYear = 1900;
		st.wYear = 2100;

		SetDateTime(&st1,&st);

		m_wndCalendar.ModifyStyle(0,MCX_MULTISELECT,0);
		//Invalidate();
	}

public:
	void SetDateTime(LPSYSTEMTIME)
	{
	}

	void SetDateTime(LPSYSTEMTIME From,LPSYSTEMTIME To)
	{
		for(int i = 0;i < _items.GetSize()/2;i++)
		{
			GetDateFormat(LOCALE_USER_DEFAULT,0,From,
				_items[i]._type,_items[i].data,100);
		}

		for(;i < _items.GetSize();i++)
		{
			GetDateFormat(LOCALE_USER_DEFAULT,0,To,
				_items[i]._type,_items[i].data,100);
		}
	
		Cash[0] = 0;
		Invalidate();
	}

	bool GetDateTime(LPSYSTEMTIME)
	{
		return false;
	}

	bool GetDateTimeRange(LPSYSTEMTIME dtFrom,LPSYSTEMTIME dtTo)
	{
		ATLASSERT(dtFrom);
		ATLASSERT(dtTo);
		
		TCHAR buf[500];
		GetDateTimeText(buf,500);

		TCHAR* p = _tcschr(buf,_T('-'));
		ATLASSERT(p);
		*p = 0;
		p++;

		CComVariant v(buf);
		bool b = SUCCEEDED(v.ChangeType(VT_DATE));
		if (b){
			b = VariantTimeToSystemTime(v.date,dtFrom) == TRUE;
			if (b){
				v = p;
				b = SUCCEEDED(v.ChangeType(VT_DATE));
				if (b)
					b = VariantTimeToSystemTime(v.date,dtTo) == TRUE;
			}
		}
		
		return b;
	}

	bool SetFormat(PTSTR)
	{
		return false;
	}
protected:
	void ShowCal()
	{
		SetFocus();
		ATLASSERT(m_wndCalendar.IsWindow());
		
		SYSTEMTIME from,to;
		GetDateTimeRange(&from,&to);
		m_wndCalendar.EnsureVisible(from,to);

		CRect r;
		GetClientRect(&r);
		r.OffsetRect(0,r.Height());
		ClientToScreen(&r);
		m_wndCalendar.SetWindowPos(NULL,&r,SWP_SHOWWINDOW|SWP_NOSIZE);
	}
};

}	//end of AWTL namespace

#endif	//__DTPICKER__H