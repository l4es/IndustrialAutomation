#ifndef __INPLACETT__H
#define __INPLACETT__H

#pragma once

/////////////////////////////////////////////////////
//	Inplace Tooltip for Edit control
//
//	Written by Alexey Shirshov (qqqaqa@mail.ru)
//	Copyright (c) 2003 Alexey Shirshov
//
//
//
//

#ifndef __ATLCTRLS_H__
#error inplacett.h requires altctrls.h to be included first
#endif

namespace AWTL 
{

class CInplaceEditTooltipCtrl :
	public CWindowImpl<CInplaceEditTooltipCtrl,CToolTipCtrl>
{
	typedef CWindowImpl<CInplaceEditTooltipCtrl,CToolTipCtrl> _baseClass;
	typedef CInplaceEditTooltipCtrl _thisClass;

public:
	
	CInplaceEditTooltipCtrl():_edit(this,1)
	{
	}
	
	DECLARE_WND_SUPERCLASS(_T("AWTL_InplaceEditTooltip"),_baseClass::GetWndClassName());

	bool AddTool(HWND hEdit,int iID, PTSTR pDefText = _T(""))
	{
		if (!::IsWindow(hEdit)) return false;

		CRect r;
		::GetClientRect(hEdit,&r);
		CToolInfo ti(TTF_SUBCLASS,hEdit,iID,&r,pDefText);
		BOOL b = _baseClass::AddTool(ti);
		ATLASSERT(b);

		_edit.SubclassWindow(hEdit);
		
		CRect rc;
		GetMargin(&rc);
		rc.InflateRect(0,1);
		rc.left = -2;
		SetMargin(&rc);
		
		SetFont(_edit.GetFont());
		
		return true;
	}
	
	void OnFinalMessage(HWND)
	{
		if (_edit.IsWindow())
			_edit.DestroyWindow();
	}

	bool UpdateText(int iID)
	{
		if (fHide){
			const int sz = _edit.GetWindowTextLength()+1;
			PTSTR pBuf = (PTSTR)_alloca(sz*sizeof(TCHAR));
			ATLASSERT(pBuf);
			_edit.GetWindowText(pBuf,sz);
			CWindowDC dc(_edit);
			dc.SelectFont(_edit.GetFont());
			CRect r(0,0,0,0),rc;
			dc.DrawText(pBuf,-1,&r,DT_CALCRECT|DT_SINGLELINE);
			_edit.GetClientRect(&rc);
			if (rc.Width() < r.Width()){
				UpdateTipText(pBuf,_edit,iID);
				return true;
			}
			else
				UpdateTipText(_T(""),_edit,iID);
		}
		return false;
	}

protected:
	BEGIN_MSG_MAP(_thisClass)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLBtnDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRBtnDown)
		ALT_MSG_MAP(1)
		NOTIFY_CODE_HANDLER(TTN_SHOW,OnShow)
		MESSAGE_HANDLER(WM_KEYDOWN, OnEditKeyDown)
		MESSAGE_HANDLER(WM_SETTEXT, OnEditSetText)
	END_MSG_MAP()

	LRESULT OnWindowPosChanging(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if (!fHide){
			LPWINDOWPOS wp = (LPWINDOWPOS)lParam;
			wp->flags &= ~SWP_HIDEWINDOW;
		}
		bHandled = FALSE;
		return 1;
	}
	
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		CPoint p(lParam);
		CRect rc;
		GetClientRect(&rc);
		if (!rc.PtInRect(p)){
			fHide = true;
			ReleaseCapture();
			ShowWindow(SW_HIDE);
		}
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnLBtnDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		fHide = true;
		ShowWindow(SW_HIDE);
		fHide = false;
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnRBtnDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		fHide = true;
		ShowWindow(SW_HIDE);
		fHide = false;
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnShow(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	{
		fHide = false;
		SetCapture();
		CRect r;
		_edit.GetRect(&r);
		_edit.ClientToScreen(&r);
		r.OffsetRect(-1,-1);
		BOOL b = SetWindowPos(NULL,&r, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
		ATLASSERT(b);
		return 1;
	}

	LRESULT OnEditKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (!fHide && IsWindowVisible()){
			fHide = true;
			ShowWindow(SW_HIDE);
			fHide = false;
		}
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnEditSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LRESULT lRes = _edit.DefWindowProc(uMsg,wParam,lParam);
		TOOLINFO ti = {sizeof(TOOLINFO)};
		for(int i = 0;i < GetToolCount();i++)
		{
			BOOL b = EnumTools(i,&ti);
			ATLASSERT(b);
			if (ti.hwnd == _edit){
				UpdateText(ti.uId);
				break;
			}
		}		
		return lRes;
	}

private:
	bool fHide;
	CContainedWindowT<CEdit> _edit;
};	//end of CInplaceEditTooltipCtrl

}	//end of AWTL namespace

#endif	//__INPLACETT__H