#pragma once

#include <atlprint.h>
#include <atlmisc.h>
#include <vector>
#include <string>
#include <functional>

using namespace std;

class CGraph
	: public CWindowImpl<CGraph, CStatic>
	, CPrintJobInfo
{
public:
	struct sPoint
	{
		double x,y;
		struct fCompareByX:binary_function<const sPoint&,const sPoint&, bool>
		{
			bool operator()(const sPoint& lhs,const sPoint& rhs){return lhs.x < rhs.x;}
		};
		struct fCompareByY:binary_function<const sPoint&,const sPoint&, bool>
		{
			bool operator()(const sPoint& lhs,const sPoint& rhs){return lhs.y < rhs.y;}
		};
		void Print(){ATLTRACE("x:%.3f\ty:%.3f\n", x,y);};
	};
	typedef vector<sPoint>	TVPoints;
	
	struct sTrend
	{
		wstring		Name;				//Название тренда
		TVPoints	Data;				//Данные
		TVPoints	ScreenData;			//Данные для вывода на экран
		COLORREF	Color;				//Цвет
		double		Width;				//Толщина линии
		BYTE		InterpLevel;		//Уровень интерполирования [2,255]
	};
	typedef vector<sTrend>	TVTrends;

	class cLegendCallback
	{
	public:
		virtual wstring GetString(double val)=0;
	};

	CGraph(void);
	~CGraph(void);

	DECLARE_WND_SUPERCLASS(NULL, CStatic::GetWndClassName())

	BEGIN_MSG_MAP(CGraph)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		
		DEFAULT_REFLECTION_HANDLER();
	END_MSG_MAP()

protected:
	struct sColors
	{
		COLORREF	Background;
	}_Colors;

	struct sAxis
	{
		COLORREF	Color;			//Цвет линий
		double		LeftSpaces,		//Отступ от левого края
					RightSpaces,	//Отступ от правого края
					TopSpaces,		//Отступ от верхнего края
					BottomSpaces;	//Отступ от нижнего края
		double		Width;			//Толщина линии
		double		SegIntervalX,	//Интервал между делениями по оси X
					SegIntervalY;	//Интервал между делениями по оси Y
		CFont		Font;
		CFont		LegendFont;
		COLORREF	FontColor;
		double		LegendSize;
		int			LineSize;
	}_Axis;

	struct sGrid
	{
		COLORREF	Color;
		double		Width;
		bool		Show;
	}_Grid;

	struct
	{
		bool Draw;
		int Left,Top;
		int OldX,OldY;
	}_DragRect;

	struct
	{
		double Left,Right;
		double Top,Bottom;
	}_VisibleRect;

	TVTrends			_Trends;
	
	UINT				_InterMethod;//Метод интерполяции

	vector<COLORREF>	_BrightColors;
	double				_KX;	//Коэффициент пропорциональности по X
	double				_KY;	//Коэффициент пропорциональности по Y
	double				_XStart; //Начальное знчение по оси X
	double				_YStart; //Начальное знчение по оси Y
	
	cLegendCallback*	_FuncX;
	cLegendCallback*	_FuncY;

	int					_PointsOnScreen;
	CPrinter			_Printer;
	short				_Orientation;
	
public:
	//InterpLevel - Степень интерполяционного полинома (1-линейная;2-квадратичная;3-кубическая и т.д.)
	//FuncX и FuncY - callback функции для получения подписей к осям
	void AddTrend(wstring Name, TVPoints& Data, BYTE InterpLevel=3);
	void DeleteAllTrends();
	void SetLegendCallback(cLegendCallback* FuncX = NULL, cLegendCallback* FuncY = NULL)
	{
		_FuncX = FuncX;
		_FuncY = FuncY;
	};

	void Print();
	inline void SetPointsOnScreen(int val){_PointsOnScreen = val;};
	inline void SetIntervalX(double val){_Axis.SegIntervalX = val;};
	inline void SetIntervalY(double val){_Axis.SegIntervalY = val;};
	inline void SetRightSpaces(double val){_Axis.RightSpaces = val;};
	inline void SetInterMethod(UINT val){_InterMethod = val;};

private:
	void _DrawAxies(HDC dc, CRect& rc);
	void _DrawGrid(HDC dc, CRect& rc);
	void _DrawTrends(HDC dc, CRect& rc);
	void _UpdateScreenData();
	bool PrintPage(UINT /*nPage*/, HDC /*hDC*/);
	void InitializeParams();

public:
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
