#include "StdAfx.h"
#include "../Primitives/BaseObject.h"
#include "../MainFrm.h"
#include ".\objectpropertywindow.h"

CObjectPropertyWindow*	CObjectPropertyWindow::_Self		= NULL;
DWORD					CObjectPropertyWindow::_RefCount	= NULL;


LRESULT CObjectPropertyWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	_Properties.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LBS_OWNERDRAWVARIABLE);
	_Properties.SetExtendedListStyle(PLS_EX_CATEGORIZED);
	_Actions.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	PostMessage(WM_POSTCREATE);
	bHandled = FALSE;
	return 0;
}

LRESULT CObjectPropertyWindow::OnPostCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	AddTab(_Properties, L"Properties");
	AddTab(_Actions, L"Actions");
	return 0;
}

LRESULT CObjectPropertyWindow::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	if(wParam != SIZE_MINIMIZED)
	{
		UpdateLayout();
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CObjectPropertyWindow::OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	MINMAXINFO* mmi = (MINMAXINFO*)lParam;

	mmi->ptMinTrackSize.x = 220;
	mmi->ptMaxTrackSize.x = 400;

	return 0;
}

//Обновление графического окна
LRESULT CObjectPropertyWindow::OnUpdateView(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	if(g_MainFrame)
		g_MainFrame->SendMessage(WM_UPDATEVIEW);
	return 0;
}






LRESULT CObjectPropertyWindow::OnWindowPosChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	if(g_MainFrame)
	{
		LPWINDOWPOS Pos = (LPWINDOWPOS)lParam;
		if(Pos->flags & SWP_SHOWWINDOW)
			g_MainFrame->UISetCheck(ID_VIEW_PROPERTY, TRUE);
		if(Pos->flags & SWP_HIDEWINDOW)
			g_MainFrame->UISetCheck(ID_VIEW_PROPERTY, FALSE);
	}

	bHandled = FALSE;
	return 0;
}

