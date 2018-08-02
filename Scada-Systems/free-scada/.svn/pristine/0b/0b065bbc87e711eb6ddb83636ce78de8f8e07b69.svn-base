// VisualizatorView.cpp : implementation of the CVisualizatorView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MainFrm.h"

#include "VisualizatorView.h"
#include ".\visualizatorview.h"

#define TIMER_GUI			1
#define TIMER_ACTION		2
#define TIMER_CONNECTION	3
#define TIMER_REFRESH_CHANNELS	4

BOOL CVisualizatorView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

void CVisualizatorView::OnInitialUpdate()
{
	//Интервал обновления окна
	SetTimer(TIMER_GUI, GUI_UPDATE_ITERVAL);

	//Интервал обновления действий
	SetTimer(TIMER_ACTION, ACTIONS_UPDATE_ITERVAL);

	SetTimer(TIMER_CONNECTION, 5000);

}

void CVisualizatorView::OnUpdate()
{
	CDoc* pDoc = GetDoc();
	ATLASSERT(pDoc != NULL);

	//Отображать на фоне изображение?
	if(pDoc->m_SchemaSettings.HasBackgrd)
	{
		CDCHandle dc = GetDC();
		::CImage img;
		pDoc->m_Archive.GetImage(pDoc->m_SchemaSettings.BackGrdName, img);

		//Удаляем старую картинку
		if(!_Background.IsNull())
			_Background.DeleteObject();

		//Создать bitmap
		_Background.CreateCompatibleBitmap(dc, img.GetWidth(), img.GetHeight());
		_Background.SetBitmapDimension(img.GetWidth(), img.GetHeight());
		CDC bmpDC;
		bmpDC.CreateCompatibleDC(dc);
		CBitmapHandle OldBmp = bmpDC.SelectBitmap(_Background);
		
		//Заполняем bitmap
		img.DrawOnDC(bmpDC);

		bmpDC.SelectBitmap(OldBmp);
		bmpDC.DeleteDC();
		ReleaseDC(dc);
	}

	sProjectSettings ps;
	pDoc->m_Archive.LoadProjectSettings(ps);
	//KillTimer(TIMER_REFRESH_CHANNELS);
	//SetTimer(TIMER_REFRESH_CHANNELS, ps.IntervalOPC);

	Invalidate();
	UpdateWindow();
}

LRESULT CVisualizatorView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CRect rc;
	CDC bmpDC;
	CDoc* pDoc = GetDoc();
	CPaintDC dc(m_hWnd);

	ATLASSERT(pDoc != NULL);
	ATLASSERT(!_ViewportBuffer.IsNull());

	//Рисуем на внеэкранном буфере
	bmpDC.CreateCompatibleDC(dc);
	
	
	CBitmapHandle OldBmp = bmpDC.SelectBitmap(_ViewportBuffer);
	GetClientRect(&rc);

	//Есть ли изображение на фоне?
	if(pDoc->m_SchemaSettings.HasBackgrd)
	{
		ATLASSERT(!_Background.IsNull());

		CDC bgDC;
		CSize bgSize;
		_Background.GetBitmapDimension(&bgSize);
		bgDC.CreateCompatibleDC(dc);
		CBitmapHandle oldBG = bgDC.SelectBitmap(_Background);

		if(pDoc->m_SchemaSettings.StretchBackGrd)
		{
			//Растягивать изображение по размеру окна
			bmpDC.SetStretchBltMode(HALFTONE);
			bmpDC.StretchBlt(0,0,rc.Width(),rc.Height(), bgDC, 0, 0, bgSize.cx, bgSize.cy, SRCCOPY);
		}
		else
		{
			//Рисовать изображение как есть (под изображением рисуется стандартный фон)
			bmpDC.FillSolidRect(0,0,rc.Width(),rc.Height(), pDoc->m_SchemaSettings.BackGrdColor.AsCOLORREF());
			const int	w = min(rc.Width(),bgSize.cx),
						h = min(rc.Height(),bgSize.cy);
			bmpDC.BitBlt(0,0,w,h, bgDC,0,0,SRCCOPY);
		}
		bgDC.SelectBitmap(oldBG);
	}
	else
	{
		//Закрашиваем фон монотонным цветом
		bmpDC.FillSolidRect(0,0,rc.Width(),rc.Height(), pDoc->m_SchemaSettings.BackGrdColor.AsCOLORREF());
	}

	//Рисуем объекты
	CObjectMenagerPtr om;
	om->DrawObjects(bmpDC);

	//Копируем изображение из буфера на экран
	dc.BitBlt(0,0,rc.Width(),rc.Height(), bmpDC,0,0,SRCCOPY);

	bmpDC.SelectBitmap(OldBmp);
	return 0;
}

LRESULT CVisualizatorView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	//На изменение размеров окна пересоздаем внеэкранный буфер
	//(он должен быть равен по размеру видимой части окна)

	CSize sz(lParam);
	CDCHandle dc = GetDC();

	if(!_ViewportBuffer.IsNull())
		_ViewportBuffer.DeleteObject();
	_ViewportBuffer.CreateCompatibleBitmap(dc, sz.cx, sz.cy);
	_ViewportBuffer.SetBitmapDimension(sz.cx,sz.cy);

	ReleaseDC(dc);
	return 0;
}

LRESULT CVisualizatorView::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//Не рисуем стандартный фон
	return TRUE;
}

LRESULT CVisualizatorView::OnTimer(UINT id, TIMERPROC lpTimerProc)
{
	//Обновить действия
	if(id == TIMER_ACTION)
	{
		CActionMenagerPtr am;
		am->UpdateActions();
	}
	
	//Обновить окно
	if(id == TIMER_GUI)
	{	//KillTimer(TIMER_GUI);
		Invalidate();
		//SetTimer(TIMER_GUI, GUI_UPDATE_ITERVAL);
	}

	if(id == TIMER_CONNECTION)
	{
		CDoc* pDoc = GetDoc();
		if(pDoc)
		{
			
			if(!pDoc->TestCon())
			{
				//Получить строку статуса
				CStatusBarCtrl bar = g_MainFrame->m_hWndStatusBar;
				bar.SetWindowText(L"OPC connection lost. Reconnecting...");
				pDoc->ConnectToOPC();
			}
			
		}
	}

	/*if(id == TIMER_REFRESH_CHANNELS)
	{
		CDoc* pDoc = GetDoc();
		if(pDoc)
			pDoc->RefreshOPC();
	}*/
	return 0;
	
}

LRESULT CVisualizatorView::OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//Передать сообщение менеджеру объектов
	CObjectMenagerPtr mng;
	mng->ProcessMouseLButtonDown(wParam, CPoint(lParam));

	//Перерисовать окно
	Invalidate();

	bHandled = FALSE;
	return 0;
}


LRESULT CVisualizatorView::OnLButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//Передать сообщение менеджеру объектов
	CObjectMenagerPtr mng;
	mng->ProcessMouseLButtonUp(wParam, CPoint(lParam));

	//Перерисовать окно
	Invalidate();

	bHandled = FALSE;
	return 0;
}
