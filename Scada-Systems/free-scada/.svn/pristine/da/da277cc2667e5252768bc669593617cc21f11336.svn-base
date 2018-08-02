#include "StdAfx.h"
#include <atlstr.h>
#include ".\graph.h"
#include <algorithm>
#include <atlctrlx.h>


CGraph::CGraph(void)
{
	_DragRect.Draw = false;

	_KX = _KY = 1.0f;
	_XStart = 0;
	_YStart = 0;
	InitializeParams();
	_FuncX = NULL;
	_FuncY = NULL;

	_InterMethod = 0;
}

CGraph::~CGraph(void)
{
}

void CGraph::AddTrend(wstring Name, TVPoints& Data, BYTE InterpLevel)
{
	CWaitCursor wait;

	COLORREF color;
	if(_Trends.size() < _BrightColors.size())
		color = _BrightColors[_Trends.size()];
	else
		color = RGB(128+rand()%127, 128+rand()%127, 128+rand()%127);

	sTrend NewTrend			= {Name, Data};
	NewTrend.Color			= color;
	NewTrend.Width			= 2;
	NewTrend.InterpLevel	= InterpLevel+1;
	_Trends.push_back(NewTrend);

	_UpdateScreenData();
	RedrawWindow();
}

LRESULT CGraph::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CGraph::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd); // device context for painting
//////////////////////////////////////////////////////////////////////////
	wstring str = L"00:00 00:00:00";
	CSize sz;
	dc.GetTextExtent(str.c_str(), str.size(), &sz);
	_Axis.SegIntervalX = sz.cx*1.2;
//////////////////////////////////////////////////////////////////////////

	CRect rc;

	GetClientRect(rc);
	dc.FillSolidRect(rc, _Colors.Background);

	CPenHandle	OldPen;

	//Рисуем сетку
	if(_Grid.Show)
		_DrawGrid(dc, rc);
	//Рисуем оси координат
	_DrawAxies(dc, rc);
	//Рисуем графики
	_DrawTrends(dc, rc);

	return 0;
}
LRESULT CGraph::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return TRUE;
}

void CGraph::_DrawAxies(HDC hdc, CRect& rc)
{
	CDCHandle	dc(hdc);

	CPenHandle	OldPen;
	CPen		AxisPen;
	AxisPen.CreatePen(PS_SOLID, (int)_Axis.Width, _Axis.Color);

	OldPen = dc.SelectPen(AxisPen);


	dc.MoveTo((int)_Axis.LeftSpaces, (int)_Axis.TopSpaces);
	dc.LineTo((int)_Axis.LeftSpaces, rc.Height()-(int)_Axis.BottomSpaces);
	dc.LineTo(rc.Width()-(int)_Axis.RightSpaces, rc.Height()-(int)_Axis.BottomSpaces);
	
	//Верхняя левая стрелка
	dc.MoveTo((int)_Axis.LeftSpaces - (int)_Axis.Width, (int)_Axis.TopSpaces + (int)_Axis.Width*5);
	dc.LineTo((int)_Axis.LeftSpaces, (int)_Axis.TopSpaces);
	dc.LineTo((int)_Axis.LeftSpaces + (int)_Axis.Width, (int)_Axis.TopSpaces + (int)_Axis.Width*5);
	
	//Нижняя правая стрелка
	dc.MoveTo( rc.Width() - (int)_Axis.RightSpaces - (int)_Axis.Width*5, rc.Height() - (int)_Axis.BottomSpaces - (int)_Axis.Width);
	dc.LineTo( rc.Width() - (int)_Axis.RightSpaces, rc.Height() - (int)_Axis.BottomSpaces);
	dc.LineTo( rc.Width() - (int)_Axis.RightSpaces - (int)_Axis.Width*5, rc.Height() - (int)_Axis.BottomSpaces + (int)_Axis.Width);
	dc.SelectPen(OldPen);

	//Рисуем деления
	AxisPen.DeleteObject();
	AxisPen.CreatePen(PS_SOLID, (int)_Axis.Width/2, _Axis.Color);
	OldPen = dc.SelectPen(AxisPen);
	CFontHandle OldFont = dc.SelectFont(_Axis.LegendFont);
	const int LineSize = 6;
	dc.SetTextColor(_Axis.FontColor);
	//Ось Y
	for(	double i=rc.Height() - _Axis.BottomSpaces - _Axis.SegIntervalY;	//Начинаем с низа
			i>_Axis.TopSpaces+_Axis.Width*5;								//Рисуем черточки до начала стрелки
			i-=_Axis.SegIntervalY)
	{
		dc.MoveTo((int)(_Axis.LeftSpaces-LineSize/2.0f), (int)i);
		dc.LineTo((int)(_Axis.LeftSpaces+LineSize/2.0f), (int)i);
		//Добавляем подписи к осям
		if(_FuncY)
		{
			double val = (double)(i-_Axis.TopSpaces)/(rc.Height()-_Axis.TopSpaces-_Axis.BottomSpaces);
			val = 1-val;
			val /= _KY;
			val += _YStart;
			wstring str;
			if(!_Trends.empty())
				str = _FuncY->GetString(val);
			CSize sz;
			dc.GetTextExtent(str.c_str(), str.size(), &sz);
			dc.TextOut((int)(_Axis.LeftSpaces-sz.cx - _Axis.LegendSize), (int)i-sz.cy/2,str.c_str(),str.size());
		}
	}
	//Ось X
	double Interval = _Axis.SegIntervalX;
	for(	double i=_Axis.LeftSpaces + _Axis.SegIntervalY;	//Начинаем с левой стороны
		i<rc.Width() - _Axis.RightSpaces - _Axis.Width*5;	//Рисуем черточки до начала стрелки
		i+=Interval)
	{
	//	const int LineSize = 6;
		dc.MoveTo((int)i, rc.Height() - (int)_Axis.BottomSpaces - LineSize/2);
		dc.LineTo((int)i, rc.Height() - (int)_Axis.BottomSpaces + LineSize/2);
		//Добавляем подписи к осям
		if(_FuncX)
		{
			double val = (double)(i-_Axis.LeftSpaces)/(rc.Width()-_Axis.LeftSpaces-_Axis.RightSpaces);
			val /= _KX;
			val += _XStart;
			wstring str;
			if(!_Trends.empty())
				str = _FuncX->GetString(val);
			CSize sz;
			dc.GetTextExtent(str.c_str(), str.size(), &sz);
			dc.TextOut((int)i-sz.cx/2,rc.Height()-(int)_Axis.BottomSpaces+_Axis.LineSize, str.c_str(),str.size());
			//Interval = sz.cx*1.2f;
		}
	}
	dc.SelectFont(OldFont);
	dc.SelectPen(OldPen);
}

void CGraph::_DrawGrid(HDC hdc, CRect& rc)
{
	CDCHandle	dc(hdc);
	//dc.SetMapMode(MM_HIMETRIC);
	CPenHandle	OldPen;
	CPen		GridPen;
	GridPen.CreatePen(PS_SOLID, (int)_Grid.Width, _Grid.Color);

	OldPen = dc.SelectPen(GridPen);
	//Рисуем горизонтальные линии
	for(	double i=rc.Height() - _Axis.BottomSpaces - _Axis.SegIntervalY;	//Начинаем с низа
			i>_Axis.TopSpaces+_Axis.Width*5;								//Рисуем черточки до начала стрелки
			i-=_Axis.SegIntervalY)
	{
		dc.MoveTo((int)_Axis.LeftSpaces, (int)i);
		dc.LineTo(rc.Width()-(int)_Axis.RightSpaces, (int)i);
	}
	//Рисуем вертикальные линии
	CFontHandle OldFont = dc.SelectFont(_Axis.LegendFont);
	double Interval = _Axis.SegIntervalX;
	for(	double i=_Axis.LeftSpaces + _Axis.SegIntervalY;	//Начинаем с левой стороны
			i<rc.Width() - _Axis.RightSpaces - _Axis.Width*5;	//Рисуем черточки до начала стрелки
			i+=Interval)
	{
		dc.MoveTo((int)i, rc.Height() - (int)_Axis.BottomSpaces);
		dc.LineTo((int)i, (int)_Axis.TopSpaces);

		//Расчитываем интервал между линиями
		if(_FuncX)
		{
			double val = (double)(i-_Axis.LeftSpaces)/(rc.Width()-_Axis.LeftSpaces-_Axis.RightSpaces);
			val /= _KX;
			val += _XStart;
			/*
			wstring str = _FuncX->GetString(val);
			CSize sz;
			dc.GetTextExtent(str.c_str(), str.size(), &sz);
			Interval = sz.cx*1.2f;
			*/
		}
	}
	dc.SelectFont(OldFont);
	dc.SelectPen(OldPen);
}

void CGraph::_DrawTrends(HDC hdc, CRect& rc)
{
	CDCHandle	dc(hdc);
	//dc.SetMapMode(MM_HIMETRIC);
	typedef TVTrends::iterator trendIt;
	typedef TVPoints::iterator pointsIt;
	CFontHandle OldFont = dc.SelectFont(_Axis.LegendFont);
	CSize XSize;
	dc.GetTextExtent(L"X",1,&XSize);
	for(trendIt j=_Trends.begin();j!=_Trends.end();j++)
	{
		//Выводим легенду
		{
			double y = _Axis.TopSpaces+10+(int)(XSize.cy*1.5f)*(j-_Trends.begin());
			CRect Led;
			Led.left	= (int)(rc.Width() - _Axis.RightSpaces + 10);
			Led.right	= (int)(rc.Width() - _Axis.RightSpaces + 10+_Axis.LegendSize);
			Led.top		= (int)(y+XSize.cy/2-_Axis.LegendSize/2);
			Led.bottom	= (int)(y+_Axis.LegendSize+XSize.cy/2 - _Axis.LegendSize/2);
			CBrush brush;
			brush.CreateSolidBrush(j->Color);
			dc.FillRect(Led, brush);
			dc.SetTextColor(_Axis.FontColor);
			dc.TextOut(Led.right + (int)_Axis.LegendSize, (int)y, j->Name.c_str());
		}
		if(j->ScreenData.size() == 0)
			continue;
		//Рисуем график
		CPen TrendPen;
		TrendPen.CreatePen(PS_SOLID, (int)j->Width, j->Color);
		CPenHandle OldPen = dc.SelectPen(TrendPen);

		bool LinePreak = true;
		for(double i=_Axis.LeftSpaces;i<rc.Width()-_Axis.RightSpaces;i++)
		{
			double GraphWidth = rc.Width()-_Axis.RightSpaces-_Axis.LeftSpaces;
			double x = (double)(i-_Axis.LeftSpaces) / GraphWidth;
			double y;
			switch(_InterMethod) {
			case 0:
				y = LagrangeIterp(j->ScreenData, x, j->InterpLevel);
				break;
			case 1:
				y = MinSquareIterp(j->ScreenData, x, j->InterpLevel);
				break;
			case 2:
				y = LinearIterp(j->ScreenData, x);
				break;
			default:
				y = LagrangeIterp(j->ScreenData, x, j->InterpLevel);
			}
			//Переводим в экранные координаты
			double GraphHeight = rc.Height()-_Axis.TopSpaces-_Axis.BottomSpaces;
			x = (x*GraphWidth)+_Axis.LeftSpaces;
			y = GraphHeight - y*(GraphHeight) +_Axis.TopSpaces;
			if(y<_Axis.TopSpaces-1 || y>rc.Height()-_Axis.BottomSpaces+1)
			{
				LinePreak = true;
				continue;
			}
			if(LinePreak)
			{
				dc.MoveTo((int)x,(int)y);
				LinePreak = false;
			}
			else
				dc.LineTo((int)x,(int)y);
		}
		dc.SelectPen(OldPen);
	}
	dc.SelectFont(OldFont);
}
LRESULT CGraph::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	/*if(_DragRect.Draw == false)
	{
		_DragRect.Left	= GET_X_LPARAM(lParam); 
		_DragRect.Top	= GET_Y_LPARAM(lParam); 
		_DragRect.OldX = GET_X_LPARAM(lParam);
		_DragRect.OldY = GET_Y_LPARAM(lParam);
		_DragRect.Draw	= true;
	}*/

	return 0;
}

LRESULT CGraph::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//if(_DragRect.Draw)
	//{
	//	CDCHandle dc = GetDC();

	//	CRect rc,LastRc;
	//	rc.right	= 0; 
	//	rc.bottom	= 0;
	//	rc.left		= 0;
	//	rc.top		= 0;
	//
	//	LastRc.right	= max(_DragRect.Left, _DragRect.OldX);
	//	LastRc.bottom	= max(_DragRect.OldY, _DragRect.Top);
	//	LastRc.left		= min(_DragRect.Left, _DragRect.OldX);
	//	LastRc.top		= min(_DragRect.OldY, _DragRect.Top);

	//	//Расчитываем новое окно видимости
	//	CRect crc;
	//	GetClientRect(&crc);
	//	_VisibleRect.Left	= ((double)(LastRc.left-_Axis.LeftSpaces)/(crc.Width()-_Axis.LeftSpaces-_Axis.RightSpaces)) * _KX + _XStart;
	//	_VisibleRect.Right	= ((double)(LastRc.right-_Axis.LeftSpaces)/(crc.Width()-_Axis.LeftSpaces-_Axis.RightSpaces)) * _KX + _XStart;
	//	_VisibleRect.Top	= ((double)(LastRc.top-_Axis.TopSpaces)/(crc.Height()-_Axis.TopSpaces-_Axis.BottomSpaces)) * _KY;
	//	_VisibleRect.Bottom	= ((double)(LastRc.bottom-_Axis.TopSpaces)/(crc.Height()-_Axis.TopSpaces-_Axis.BottomSpaces)) * _KY;

	//	dc.DrawDragRect(&rc, CSize(1,1), LastRc, CSize(1,1));
	//	_DragRect.Draw = false;

	//	_UpdateScreenData();
	//	RedrawWindow();
	//}

	return 0;
}

LRESULT CGraph::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(_DragRect.Draw)
	{
		CDCHandle dc = GetDC();
		
		CRect rc,LastRc;
		rc.right	= max(GET_X_LPARAM(lParam), _DragRect.Left);
		rc.bottom	= max(GET_Y_LPARAM(lParam), _DragRect.Top);
		rc.left		= min(GET_X_LPARAM(lParam), _DragRect.Left);
		rc.top		= min(GET_Y_LPARAM(lParam), _DragRect.Top);

		LastRc.right	= max(_DragRect.Left, _DragRect.OldX);
		LastRc.bottom	= max(_DragRect.OldY, _DragRect.Top);
		LastRc.left		= min(_DragRect.Left, _DragRect.OldX);
		LastRc.top		= min(_DragRect.OldY, _DragRect.Top);
		
		dc.DrawDragRect(&rc, CSize(1,1), LastRc, CSize(1,1));
        
		_DragRect.OldX = GET_X_LPARAM(lParam);
		_DragRect.OldY = GET_Y_LPARAM(lParam);
	}
	return 0;
}


void CGraph::_UpdateScreenData()
{
	CWaitCursor wait;

	//Добавляем в "экранные данные" данные для отображения
	typedef TVTrends::iterator trendIt;
	typedef TVPoints::iterator pointsIt;
	
	for(trendIt j=_Trends.begin();j!=_Trends.end();j++)
	{
		j->ScreenData.clear();
		for(pointsIt i=j->Data.begin();i!=j->Data.end();i++)
		{
			//Если фильтр не используется отображаем все данные
			if( _VisibleRect.Left == _VisibleRect.Right &&
				_VisibleRect.Right == _VisibleRect.Bottom &&
				_VisibleRect.Bottom == _VisibleRect.Top &&
				_VisibleRect.Top == 0)
			{
				j->ScreenData.push_back(*i);
				continue;
			}

			if( i->x >= _VisibleRect.Left &&
				i->x <= _VisibleRect.Right &&
				i->y >= _VisibleRect.Top &&
				i->y <= _VisibleRect.Bottom)
			{
				j->ScreenData.push_back(*i);
			}
		}
	}

	double XMax=-999999,YMax=-99999;
	_XStart = 9999999;
	_YStart = 9999999;
	for(trendIt j=_Trends.begin();j!=_Trends.end();j++)
	{
		//Оставляем в экранных данных только несколько точек
		if(j->ScreenData.size()>=(size_t)_PointsOnScreen)
		{
			TVPoints tmp;
			tmp = j->ScreenData;
			j->ScreenData.clear();
			j->ScreenData.reserve(_PointsOnScreen);
			int x_last=-1;
			for(int i=0;i<_PointsOnScreen;i++)
			{
				int x = (int)(((double)i/_PointsOnScreen)*tmp.size());
				if(x == x_last)
					continue;
				x_last = x;
				j->ScreenData.push_back(tmp[x]);
			}
		}

		sort(j->ScreenData.begin(),j->ScreenData.end(),sPoint::fCompareByX());

		//Определяем максимальное значение X и Y
		pointsIt elem = max_element(j->ScreenData.begin(), j->ScreenData.end(), sPoint::fCompareByX());
		if(elem != j->ScreenData.end())
			XMax = max(XMax, elem->x);
		elem = max_element(j->ScreenData.begin(), j->ScreenData.end(), sPoint::fCompareByY());
		if(elem != j->ScreenData.end())
			YMax = max(YMax, elem->y);
		//Определяем минимальное значение X
		elem = min_element(j->ScreenData.begin(), j->ScreenData.end(), sPoint::fCompareByX());
		if(elem != j->ScreenData.end())		
			_XStart = min(_XStart, elem->x);
		//Определяем минимальное значение Y
		elem = min_element(j->ScreenData.begin(), j->ScreenData.end(), sPoint::fCompareByY());
		if(elem != j->ScreenData.end())		
			_YStart = min(_YStart, elem->y);
	}

	//Расчитываем коэффициента для нормализации данных графика
	if(XMax<_XStart)
		_KX = 1;
	else
		_KX = 1/(XMax-_XStart);
	
	if(YMax < _YStart)
		_KY = 1;
	else
		_KY = 1/(YMax-_YStart);
	for(trendIt j=_Trends.begin();j!=_Trends.end();j++)
	{
		for(pointsIt i=j->ScreenData.begin();i!=j->ScreenData.end();i++)
		{
			i->x = (i->x - _XStart) * _KX;
			i->y = (i->y - _YStart) * _KY;
		}
	}
}

void CGraph::Print()
{
	CPrintDialog	dlg;
	CPrintJob		pj;
	CDevMode		dm;
	if(dlg.DoModal() != IDOK)
		return;


	dm = dlg.m_pd.hDevMode;
	_Orientation = dm.m_pDevMode->dmOrientation;
	_Printer.OpenPrinter(dlg.m_pd.hDevNames, dm);
	pj.StartPrintJob(false, _Printer, dm, this, L"Graph printing", 0, 0);
	
}

bool CGraph::PrintPage(UINT /*nPage*/, HDC hDC)
{
	CDCHandle dc(hDC);
	CSize phys_offset(	dc.GetDeviceCaps(PHYSICALOFFSETX)+100, 
						dc.GetDeviceCaps(PHYSICALOFFSETY)+100);
	CSize phys_size(	dc.GetDeviceCaps(PHYSICALWIDTH)-phys_offset.cx,
						dc.GetDeviceCaps(PHYSICALHEIGHT)-phys_offset.cy);

	//CDevMode dm;
	//dm.CopyFromPrinter(_Printer);
	//Отступ в 2 см
	//double Offset = ((2/2.54f)*dm.m_pDevMode->dmYResolution)*2;
	CRect rc(	0,
				0,
				phys_size.cx,
				phys_size.cy);
	
	COLORREF Background			= _Colors.Background;
	COLORREF FontColor			= _Axis.FontColor;
	COLORREF AxisColor			= _Axis.Color;
	COLORREF GridColor			= _Grid.Color;

	CRect ClientRC;
	GetClientRect(&ClientRC);
	double	Kx = (double)rc.Width()/ClientRC.Width(),
			Ky = (double)rc.Height()/ClientRC.Height();
//////////////////////////////////////////////////////////////////////////

	if(_Orientation == DMORIENT_PORTRAIT)
		Ky = Kx;

	rc.bottom = (LONG)(ClientRC.Height()*Ky);
	rc.right = (LONG)(ClientRC.Width()*Kx);

//////////////////////////////////////////////////////////////////////////

	_Colors.Background	= RGB(255,255,255);
	_Axis.FontColor		= RGB(0,0,0);
	_Axis.Color			= RGB(0,0,0);
	_Axis.SegIntervalX	*= Kx;//*2;
	_Axis.SegIntervalY	*= Ky;
	_Grid.Color			= RGB(100,100,100);
	//////////////////////////////////////////////////////////////////////////
	_Axis.BottomSpaces	*= Kx;
	_Axis.TopSpaces	*= Kx;
	_Axis.RightSpaces *= Kx;
	//////////////////////////////////////////////////////////////////////////
	_Axis.LeftSpaces	*= Kx;//*1.1f;
	_Axis.Width			*= Kx;
	_Axis.LegendSize	*= Kx;
	_Axis.LineSize		= (int)(Kx*_Axis.LineSize);
	_Axis.Font.DeleteObject();
	_Axis.Font.CreateFont(	(int)(20*Ky/**0.5*/),
							0,
							0,
							0,
							FW_SEMIBOLD,
							FALSE,
							FALSE,
							FALSE,
							RUSSIAN_CHARSET,
							OUT_DEFAULT_PRECIS,
							CLIP_DEFAULT_PRECIS,
							ANTIALIASED_QUALITY,
							VARIABLE_PITCH,
							L"Arial");
	_Axis.LegendFont.DeleteObject();
	_Axis.LegendFont.CreateFont(	(int)(15*Ky/**0.5*/),
									0,
									0,
									0,
									FW_NORMAL,
									FALSE,
									FALSE,
									FALSE,
									RUSSIAN_CHARSET,
									OUT_DEFAULT_PRECIS,
									CLIP_DEFAULT_PRECIS,
									ANTIALIASED_QUALITY,
									VARIABLE_PITCH,
                                    L"Arial");
	for(TVTrends::iterator j=_Trends.begin();j!=_Trends.end();j++)
	{
		j->Width *= Kx;
	}

	_DrawGrid(dc, rc);
	_DrawAxies(dc, rc);
	_DrawTrends(dc, rc);

	_Colors.Background	= Background;
	_Axis.FontColor		= FontColor;
	_Axis.Color			= AxisColor;
	_Axis.SegIntervalX	/= Kx;//*2;
	_Axis.SegIntervalY	/= Ky;
	_Axis.LeftSpaces	/= Kx;//*1.1f;
	//////////////////////////////////////////////////////////////////////////
	_Axis.BottomSpaces	/= Kx;
	_Axis.TopSpaces	/= Kx;
	_Axis.RightSpaces /= Kx;
	//////////////////////////////////////////////////////////////////////////
	_Axis.Width			/= Kx;
	_Axis.LegendSize	/= Kx;
	_Axis.LineSize		= (int)(_Axis.LineSize/Kx);
	_Grid.Color			= GridColor;
	for(TVTrends::iterator j=_Trends.begin();j!=_Trends.end();j++)
	{
		j->Width /= Kx;
	}

	_Axis.Font.DeleteObject();
	_Axis.Font.CreateFont(	20,
							0,
							0,
							0,
							FW_SEMIBOLD,
							FALSE,
							FALSE,
							FALSE,
							RUSSIAN_CHARSET,
							OUT_DEFAULT_PRECIS,
							CLIP_DEFAULT_PRECIS,
							ANTIALIASED_QUALITY,
							VARIABLE_PITCH,
							L"Arial");
	_Axis.LegendFont.DeleteObject();
	_Axis.LegendFont.CreateFont(	15,
									0,
									0,
									0,
									FW_NORMAL,
									FALSE,
									FALSE,
									FALSE,
									RUSSIAN_CHARSET,
									OUT_DEFAULT_PRECIS,
									CLIP_DEFAULT_PRECIS,
									ANTIALIASED_QUALITY,
									VARIABLE_PITCH,
									L"Arial");
	return true;
}

void CGraph::InitializeParams()
{
	//Генерируем доступные цвета для трендов
	_BrightColors.clear();
	for(BYTE i=1;i<8;i++)
	{
		BYTE r = ((i>>0)&1)*255;
		BYTE g = ((i>>1)&1)*255;
		BYTE b = ((i>>2)&1)*255;
		_BrightColors.push_back(RGB(r,g,b));
	}

	_Colors.Background	= RGB(0,0,0);

	//Сетка
	_Grid.Color			= RGB(0,128,0);
	_Grid.Width			= 1;
	_Grid.Show			= true;

	//Границы графика
	_Axis.Color			= RGB(0,255,0);
	_Axis.LeftSpaces	= 50;
	_Axis.TopSpaces		= 10;
	_Axis.RightSpaces	= 60;
	_Axis.BottomSpaces	= 30;
	_Axis.Width			= 2;
	_Axis.SegIntervalY	= 30;
//	_Axis.SegIntervalX	= 35;
	_Axis.LegendSize	= 5;
	_Axis.LineSize = 6;
	_Axis.Font.CreateFont(	20,
		0,
		0,
		0,
		FW_SEMIBOLD,
		FALSE,
		FALSE,
		FALSE,
		RUSSIAN_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		VARIABLE_PITCH,
		L"Arial");
	_Axis.LegendFont.CreateFont(	15,
		0,
		0,
		0,
		FW_NORMAL,
		FALSE,
		FALSE,
		FALSE,
		RUSSIAN_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		VARIABLE_PITCH,
		L"Arial");
	_Axis.FontColor		= RGB(0,255,0);
//////////////////////////////////////////////////////////////////////////
	

//////////////////////////////////////////////////////////////////////////
	_KX = _KY = 1.0f;
	_XStart = 0;
	_YStart = 0;
	_PointsOnScreen = 20;
	ZeroMemory(&_VisibleRect, sizeof(_VisibleRect));
}

void CGraph::DeleteAllTrends()
{
	_Trends.clear();
	_UpdateScreenData();
	RedrawWindow();
}