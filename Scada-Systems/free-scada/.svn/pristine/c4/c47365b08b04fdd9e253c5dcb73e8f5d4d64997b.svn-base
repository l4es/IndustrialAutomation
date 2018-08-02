#if !defined(__GRAPHICS_H__)
#define __GRAPHICS_H__

#include <math.h>

#define  PiV     3.14159265358979324                       
#define  ConstV  180

//////////////////////////////////////////////////////////////////////////////////
////// class CGraphics
class CGraphics
{
public:
	CGraphics();
	~CGraphics() {if (m_Bitmap) ::DeleteObject(m_Bitmap); }

	enum   CTextAlign	{ LEFT = 0, CENTER = 1, RIGHT = 2, TOP = 0, BOTTOM = 2 };
	enum   CMarkerType	{ CROSS, XCROSS, STAR, CIRCLE, TRIANGLE, DIAMOND, 
						  SQUARE, FCIRCLE, FTRIANGLE, FDIAMOND, FSQUARE };

	// Operations
	void SetRatio(double xmin = 0, double ymin = 0, double xmax = 1, double ymax = 1);
	void SetPrintScale(HDC& hDC, RECT& rect);
	void RecalcRects(RECT& rt);
	void BeginDraw(HDC hDC);
	void EndDraw(HDC hDC);

	void SetBackColor(COLORREF cr)				{ m_nBackColor = cr; }
	COLORREF GetBackColor()						{ return m_nBackColor; }
	void SetGridColor(COLORREF cr)				{ m_nGridColor = cr; }
	COLORREF GetGridColor()						{ return m_nGridColor; }
	void SetBorderColor(COLORREF cr)			{ m_nBorderColor = cr; }
	COLORREF GetBorderColor()					{ return m_nBorderColor; }
	void SetTickColor(COLORREF cr)				{ m_nTickColor = cr; }
	COLORREF GetTickColor()						{ return m_nTickColor; }
	void SetTitleColor(COLORREF cr)				{ m_nTitleColor = cr; }
	COLORREF GetTitleColor()					{ return m_nTitleColor; }
	HDC GetHDC()								{ return m_hDC; }

	void SetXNumOfTicks(int x = 50)				{ XTicks = x; }
	void SetYNumOfTicks(int y = 50)				{ YTicks = y; }
	void SetXNumOfGridTicks(int x = 10)			{ XGridTicks = x; }
	void SetYNumOfGridTicks(int y = 10)			{ YGridTicks = y; }
	void SetXDecimal(int decimal)				{ m_nXDecimal = decimal; }
	int  GetXDecimal()							{ return m_nXDecimal; }
	void SetYDecimal(int decimal)				{ m_nYDecimal = decimal; }
	int  GetYDecimal()							{ return m_nYDecimal; }
	void SetStringAlign(int x, int y)			{ m_StrAlign.HAlign = x; m_StrAlign.VAlign = y; }
	void SetFontName(const wchar_t * FontName)		{ wcscpy(m_LogFont.lfFaceName, FontName); }

	void EnableLegend(bool bFlag = true)		{ m_bEnableLegend = bFlag; }
	void EnableLegendShadow(bool bFlag = true)	{ m_bLegendShadow = bFlag; }
	void EnableMemoryDraw(bool bFlag = false)	{ m_bMemoryDraw = bFlag; }
	
	void DrawBoundary(COLORREF cr, int size);
	void DrawShadow(int n);
	void Title(const wchar_t* Title, int Pos = TOP);
	void XAxisTitle(const wchar_t* Title, int Pos = BOTTOM);
	void YAxisTitle(const wchar_t* Title, int Pos = LEFT);
	void Axes();
	
	virtual bool SetRange(double xmin, double ymin, double xmax, double ymax) = 0;
	virtual void XAxis();
	virtual void YAxis(int num=0);
	virtual void Grid();

	RECT		m_PlotRect;
	bool		m_bMemoryDraw;
	bool		m_bPrinting;
	int			m_nPrintScale;
	COLORREF	crTable[16];

protected:
	enum   CAxesType	{ XY, XLOG, YLOG, XYLOG	};
	struct CScale		{ double dx, dy, xmin, ymin, xmax, ymax; };
	struct CRatio		{ double xmin, ymin, xmax, ymax; };
	struct StringAlign	{ int HAlign, VAlign; };

	CScale		m_Scale;
	CRatio		m_Ratio;
	LOGFONT		m_LogFont;
	COLORREF	m_nBackColor;
	COLORREF	m_nGridColor;
	COLORREF	m_nBorderColor;
	COLORREF	m_nTickColor;
	COLORREF	m_nTitleColor;
	HFONT		m_Font;		
	SIZE		m_Size;
	StringAlign m_StrAlign;
	int			m_nXDecimal;
	int			m_nYDecimal;
	bool		m_bEnableLegend;
	bool		m_bLegendShadow;
	int			m_nAxesType;
	int			m_nXStep;
	int			m_nYStep;

	int			m_lM;	// left margin
	int			m_tM;	// right margin
	int			m_rM;	// top margin
	int			m_bM;	// bottom margin

	RECT		m_Rect;
	RECT		m_ClipBox;
	RECT		m_ClientRect;
	HDC			m_hDC;	
	POINT		m_Pt;

	HBITMAP		m_Bitmap;
	HBITMAP		m_OldBitmap;

	int GL, GR, GT, GB, PX, PY;
	int XTicks, YTicks, XGridTicks, YGridTicks;

	virtual void Legend(COLORREF cr, int Index, const wchar_t* Name);
	void GetPixelRect(RECT& rt);
	void SetPixelRect(RECT rt);
	void DrawBkGround();
	void DrawCircle(int x, int y, int radius);
	void DrawFilledCircle(int x, int y, int radius);
	void DrawRectangle(int x1, int y1, int x2, int y2);
	void DrawMarker(int x, int y, int mode, int size = 6);
	void Format(int decimal, wchar_t* str, float value);
	void PrintString(int x, int y, int theta, const wchar_t* fmt);
	void Ticks();
	void RightYTick();
	
	void DrawLine(int x1, int y1, int x2, int y2)			{ ::MoveToEx(m_hDC, x1, y1, NULL); ::LineTo(m_hDC, x2, y2); }
	void DrawLine(POINT& pt1, POINT& pt2)					{ DrawLine(pt1.x, pt1.y, pt2.x, pt2.y); }
	void DrawCircle(POINT& pt, int radius)					{ DrawCircle(pt.x, pt.y, radius); }
	void DrawFilledCircle(POINT& pt, int radius)			{ DrawFilledCircle(pt.x, pt.y, radius); }
	void DrawRectangle(POINT& pt1, POINT& pt2)				{ DrawRectangle(pt1.x, pt1.y, pt2.x, pt2.y); }
	void DrawMarker(POINT& pt, int mode, int size = 6)		{ DrawMarker(pt.x, pt.y, mode, size); }
	void PrintString(POINT& pt, int theta, const wchar_t* fmt)	{ PrintString(pt.x, pt.y, theta, fmt); }
};

#endif
