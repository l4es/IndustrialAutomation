// DesignerView.cpp : implementation of the CDesignerView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "Primitives/Polyline.h"
#include "DesignerView.h"
#include "Doc.h"
#include ".\designerview.h"
#include "MainFrm.h"

#define GRID_SIZE	8

BOOL CDesignerView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

//Рисование примитивов на Bitmap-е и наложение Bitmap-а с примитивами на фон
LRESULT CDesignerView::OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CDCHandle dc((HDC)wParam);
	CRect rc;
	GetClientRect(&rc);

	CDC buffDC;
	//Создаем вспомогательный DC
	buffDC.CreateCompatibleDC(dc);
	//Выбираем в DC вспомогательный Bitmap для рисования примитивов
	CBitmap bbold = buffDC.SelectBitmap(_ViewportBuffer);


	CDC bdc;
	//Создаем DC
	bdc.CreateCompatibleDC(dc);
	//Выбираем в DC Bitmap с фоном
	CBitmap bold = bdc.SelectBitmap(m_Viewport);


	CObjectMenagerPtr mng;
	//Копируем фон на вспомогательный Bitmap
	const int bgPosX = 0;
	const int bgPosY = 0;
	buffDC.BitBlt(bgPosX,bgPosY,rc.Width(),rc.Height(), bdc, 0, 0, SRCCOPY);
	//Рисуем сетку
	if(((CDoc*)GetDocument())->m_ShowGrid)
	{
        for(int x=GetScrollPos(SB_HORZ)%GRID_SIZE;x < rc.Width(); x+= GRID_SIZE)
			for(int y=GetScrollPos(SB_VERT)%GRID_SIZE;y < rc.Height(); y+= GRID_SIZE)
				buffDC.SetPixel(x,y, RGB(0,0,0));
	}

	//Рисуем примитивы на вспомогательном Bitmap-е
	mng->DrawObjects(buffDC);

	//Копируем вспомогательный Bitmap на экран
	dc.BitBlt(0,0,rc.Width(),rc.Height(),buffDC, 0, 0, SRCCOPY);

	bdc.SelectBitmap(bold);
	buffDC.SelectBitmap(bbold);
	return TRUE;
}

//Обработчик изменения размеров
LRESULT CDesignerView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	CSize sz(lParam);
	CDCHandle dc = GetDC();

	//создание Bitmap-а для рисования фона
	if(!m_Viewport.IsNull())
		m_Viewport.DeleteObject();
	m_Viewport.CreateCompatibleBitmap(dc, sz.cx, sz.cy);
	m_Viewport.SetBitmapDimension(sz.cx,sz.cy);
	
	//создание Bitmap-а для рисования примитивов
	if(!_ViewportBuffer.IsNull())
		_ViewportBuffer.DeleteObject();
	_ViewportBuffer.CreateCompatibleBitmap(dc, sz.cx, sz.cy);
	_ViewportBuffer.SetBitmapDimension(sz.cx,sz.cy);

	//Наложение Bitmap-а фонового изображения поверх заливки
	UpdateBackGround();
	ReleaseDC(dc);
	RedrawWindow();
	return 0;
}

//Создание Bitmap-а для фонового изображения
void CDesignerView::CreateBackground()
{
	CDCHandle dc = GetDC();
	CDC bdc;
	bdc.CreateCompatibleDC(dc);
	if(!_Background.IsNull())
		_Background.DeleteObject();
	_Background.CreateCompatibleBitmap(dc, m_BGImage.GetWidth(), m_BGImage.GetHeight());
	_Background.SetBitmapDimension(m_BGImage.GetWidth(), m_BGImage.GetHeight());
	CBitmapHandle bold = bdc.SelectBitmap(_Background);
	m_BGImage.DrawOnDC(bdc);
	bdc.SelectBitmap(bold);
	ReleaseDC(dc);
	UpdateBackGround();
}

//Наложение Bitmap-а фонового изображения поверх заливки
void CDesignerView::UpdateBackGround()
{
	CObjectMenagerPtr mng;
	CPoint LeftTop = mng->GlobalToScreen(sVector(0,0));
	CPoint RightBottom = mng->GlobalToScreen(m_WorkArea);

	CDCHandle dc = GetDC();
	
	CDC bdc;
	bdc.CreateCompatibleDC(dc);

	//Выбираем "чистый" Bitmap
	CBitmapHandle bold_view = bdc.SelectBitmap(m_Viewport);

	CBrush brush;
	brush.CreateSolidBrush(/*cBackgroundColor*/m_BackGrdColor.AsCOLORREF());

	CSize sz;
	m_Viewport.GetBitmapDimension(&sz);
	CRect rc(0,0,sz.cx,sz.cy);
	//Выполняем заливку на Bitmap-е
	bdc.FillRect(&rc, brush);
	//Если рисуем изображение
	if(m_IsBGImage)
	{
		CDC bitDC;
		bitDC.CreateCompatibleDC(dc);
		//Выбираем в DC Bitmap с изображением
		CBitmapHandle bold = bitDC.SelectBitmap(_Background);

		//Если есть границы области рисования и установлен флаг растягивания изображения 
		if(m_IsLimit && m_StrchBGImage)
		{
			//Масштабируем изображение до размеров области рисования
			bdc.SetStretchBltMode(HALFTONE);
			bdc.StretchBlt(LeftTop.x, LeftTop.y, RightBottom.x, RightBottom.y, bitDC,
				0, 0, m_BGImage.GetWidth() ,m_BGImage.GetHeight(), SRCCOPY );
		}
		else
		{
			//Копируем изображение без масштабирования
			bdc.BitBlt(LeftTop.x, LeftTop.y,m_BGImage.GetWidth() ,m_BGImage.GetHeight(), bitDC, 0, 0, SRCCOPY);
		}
	}
	
	//Рисуем границы области рисования
	if(m_IsLimit)
	{
		bdc.MoveTo(LeftTop);
		bdc.LineTo(RightBottom.x, LeftTop.y);
		bdc.LineTo(RightBottom);
		bdc.LineTo(LeftTop.x, RightBottom.y);
		bdc.LineTo(LeftTop);
	}

	bdc.SelectBitmap(bold_view );
	ReleaseDC(dc);

	Invalidate();
}

//Обработчик нажатия левой кнопки мыши
LRESULT CDesignerView::OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CDoc* pDoc = (CDoc*)GetDocument();
	ATLASSERT(pDoc != NULL);
	//Если не выбран примитив для создания
	if(pDoc->m_CurrentCreatingPrimitive == P_EMPTY)
	{
		CObjectMenagerPtr mng;
		mng->ProcessMouseLButtonDown(wParam, CPoint(lParam));
		//Если нет выбранных объектов - очищаем окно свойств
		if(mng->GetSelected() == NULL)
		{
			g_MainFrame->m_PropertyWindow->SetObject(NULL);
		}
		RedrawWindow();
		SetCapture();
	}
	//Нужно установить фокус, чтобы получать сообщения от клавиатуры
	SetFocus();
	bHandled = FALSE;
	return 0;
}

//Обработчик отпускания левой кнопки мыши
LRESULT CDesignerView::OnLButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ReleaseCapture();

	CDoc* pDoc = (CDoc*)GetDocument();
	ATLASSERT(pDoc != NULL);
	//Если выбран примитив для создания
	if(pDoc->m_CurrentCreatingPrimitive != P_EMPTY)
	{
		if(pDoc->GetCurrentSchemaName() == L"")
		{
			MessageBox(L"Current circuit is undefined", L"Error", MB_OK|MB_ICONINFORMATION);
			return 0;
		}

		if(wParam != MK_SHIFT)
		{
			//Создаем примитив
			pDoc->CreateNewPrimitive(pDoc->m_CurrentCreatingPrimitive, CPoint(lParam));
			pDoc->m_CurrentCreatingPrimitive = P_EMPTY;
			::PostMessage(GetParent(), WM_UPDATE_PRIMITIVES_TOOLBAR, 0, 0);
		}
		else
			pDoc->CreateNewPrimitive(pDoc->m_CurrentCreatingPrimitive, CPoint(lParam), TRUE);

	}
	else //Если не выбран примитив для создания
	{
		CObjectMenagerPtr mng;
		//Направляем сообщение в ObjectMenager
		mng->ProcessMouseLButtonUp(wParam, CPoint(lParam));
	}

	RedrawWindow();
	bHandled = FALSE;
	return 0;
}

//Обработчик перемещения мыши
LRESULT CDesignerView::OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CObjectMenagerPtr mng;
	//Направляем сообщение в ObjectMenager
	CPoint pt(lParam);
	if(((CDoc*)GetDocument())->m_ShowGrid)
	{
		pt.x = static_cast<int>((pt.x/GRID_SIZE)*GRID_SIZE);
		pt.y = static_cast<int>((pt.y/GRID_SIZE)*GRID_SIZE);
	}
	mng->ProcessMouseMove(wParam, pt);
	
	RedrawWindow();
	bHandled = FALSE;
	return 0;
}

//Обработчик нажатия правой кнопки мыши
LRESULT CDesignerView::OnRButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CObjectMenagerPtr mng;
	//Направляем сообщение в ObjectMenager
	mng->ProcessMouseRButtonDown(wParam, CPoint(lParam));

	RedrawWindow();
	bHandled = FALSE;
	return 0;
}

//Обработчик двойного нажатия левой кнопки мыши
LRESULT CDesignerView::OnLButtonDblClk(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CObjectMenagerPtr mng;
	//Направляем сообщение в ObjectMenager
	mng->ProcessMouseLButtonDblClk(wParam, CPoint(lParam));

	RedrawWindow();
	bHandled = FALSE;
	return 0;
}

//Обработчик нажатия кнопки клавиатуры
LRESULT CDesignerView::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CObjectMenagerPtr mng;
	//Направляем сообщение в ObjectMenager
	mng->ProcessKeyDown(wParam, lParam);

	if (wParam == (WPARAM)46)
		g_MainFrame->m_PropertyWindow->SetObject(NULL);

	RedrawWindow();
	bHandled = FALSE;
	return 0;
}

//Проверка есть ли в ObjectMenagere примитивы оределенного типа
bool CDesignerView::IsUniquePrimitive(enCreatePrimitive Primitive)
{
	CObjectMenagerPtr mng;
	
	list<CObject*> List;
	switch(Primitive) {
	case P_RECTANGLE : List = mng->FindObjects(_RECTANGLE);
		break;
	case P_ELLIPSE : List = mng->FindObjects(_ELLIPSE);
		break;
	case P_POLYLINE : List = mng->FindObjects(_POLYLINE);
		break;
	case P_IMAGE : List = mng->FindObjects(_POLYLINE);
		break;
	}

	if(!List.empty())
		return false;
	
	return true;
}

void CDesignerView::OnUpdate()
{
	SCROLLINFO si;
	si.cbSize	= sizeof(SCROLLINFO);
	si.nPage	= 100;
	si.nMin		= 0;
	si.nMax		= 0xfff;
	si.fMask = SIF_PAGE|SIF_RANGE;
	SetScrollInfo(SB_VERT, &si);
	SetScrollInfo(SB_HORZ, &si);

	Invalidate();
}
LRESULT CDesignerView::OnHScroll(int code, short pos, HWND hwndCtl)
{
	SCROLLINFO si;
	si.cbSize = sizeof (si);
	si.fMask  = SIF_ALL;
	GetScrollInfo(SB_HORZ, &si);
	switch (code)
	{
	case SB_LINELEFT:	si.nPos -= 1;			break;
	case SB_LINERIGHT: 	si.nPos += 1;			break;
	case SB_PAGELEFT:	si.nPos -= si.nPage;	break;
	case SB_PAGERIGHT:	si.nPos += si.nPage;	break;
	case SB_THUMBTRACK: si.nPos = si.nTrackPos;	break;
	default :
		break;
	}
	si.fMask = SIF_POS;
	SetScrollInfo(SB_HORZ, &si, TRUE);
	GetScrollInfo(SB_HORZ, &si);

	CObjectMenagerPtr mng;
	if(mng->m_ViewPort.TopLeft.x != si.nPos)
	{
		mng->m_ViewPort.TopLeft.x = si.nPos;
		UpdateBackGround();
		Invalidate();
	}
	return 0;
}

LRESULT CDesignerView::OnVScroll(int code, short pos, HWND hwndCtl)
{
	SCROLLINFO si;
	si.cbSize = sizeof (si);
	si.fMask  = SIF_ALL;
	GetScrollInfo(SB_VERT, &si);
	switch (code)
	{
	case SB_LINELEFT:	si.nPos -= 1;			break;
	case SB_LINERIGHT: 	si.nPos += 1;			break;
	case SB_PAGELEFT:	si.nPos -= si.nPage;	break;
	case SB_PAGERIGHT:	si.nPos += si.nPage;	break;
	case SB_THUMBTRACK: si.nPos = si.nTrackPos;	break;
	default :
		break;
	}
	si.fMask = SIF_POS;
	SetScrollInfo(SB_VERT, &si, TRUE);
	GetScrollInfo(SB_VERT, &si);

	CObjectMenagerPtr mng;
	if(mng->m_ViewPort.TopLeft.y != si.nPos)
	{
		mng->m_ViewPort.TopLeft.y = si.nPos;
		UpdateBackGround();
		Invalidate();
	}
	return 0;
}
