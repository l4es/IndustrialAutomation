#include "common.h"
//#include <windows.h>
//#include <stdio.h>
#include "graphics.h"

///////////////////////////////////////////////////////////////////////////////
// class CGroupLines

CGraphics::CGraphics()
{
	m_Ratio.xmin	= 0;
	m_Ratio.ymin	= 0;
	m_Ratio.xmax	= 1;
	m_Ratio.ymax	= 1;
	m_nBackColor	= RGB(255, 255, 255);
	m_nGridColor	= RGB(192, 192, 192);
	m_nBorderColor	= RGB(0, 0, 0);
	m_nTickColor	= RGB(0, 0, 255);
	m_nTitleColor	= RGB(255, 0, 0);
	m_nXDecimal		= 0;
	m_nYDecimal		= 0;
	XGridTicks		= 10;
	YGridTicks		= 10;
	XTicks			= 50;
	YTicks			= 50;
	m_bEnableLegend = true;
	m_bPrinting		= false;
	m_bLegendShadow = true;
	m_bMemoryDraw	= true;
	m_nPrintScale	= 8;
	m_nAxesType		= XY;
	m_nXStep		= 0;
	m_nYStep		= 0;

	m_LogFont.lfWidth			= 0;
	m_LogFont.lfItalic			= false;
	m_LogFont.lfUnderline		= false;
	m_LogFont.lfStrikeOut		= false;
	m_LogFont.lfCharSet			= ANSI_CHARSET;
	m_LogFont.lfOutPrecision	= OUT_DEFAULT_PRECIS;
	m_LogFont.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	m_LogFont.lfQuality			= PROOF_QUALITY;
	m_LogFont.lfPitchAndFamily	= DEFAULT_PITCH;
	wcscpy(m_LogFont.lfFaceName,L"Ariel");

	crTable[0]  = RGB(255, 255, 255);     // White                  
    crTable[1]  = RGB(  0,   0,   0);     // Black
	crTable[2]  = RGB(255,   0,   0);     // Red
	crTable[3]  = RGB(  0, 255,   0);     // Green
	crTable[4]  = RGB(  0,   0, 255);     // Blue
	crTable[5]  = RGB(255,   0, 255);     // Magenta
	crTable[6]  = RGB(  0, 255, 255);     // Cyan
	crTable[7]  = RGB(128, 128, 128);     // Grey
	crTable[8]  = RGB(128, 128,   0);     // Brown
	crTable[9]  = RGB(128,   0, 128);     // Purple
	crTable[10] = RGB(  0, 128, 128);     // Aqua
	crTable[11] = RGB(128,   0,   0);     // 
	crTable[12] = RGB(  0, 128,   0);     // 
	crTable[13] = RGB(  0,   0, 128);     // 
	crTable[14] = RGB(192, 192, 192);     // 
	crTable[15] = RGB(255, 255,   0);     // Yellow

	m_Bitmap    = 0;
}

void CGraphics::SetPrintScale(HDC& hDC, RECT& rect)
{
	int Width     = ::GetDeviceCaps(hDC, HORZRES);
	int Height    = ::GetDeviceCaps(hDC, VERTRES);
	int wd		  = abs(rect.right - rect.left);
	int ht		  = abs(rect.bottom - rect.top);
	m_nPrintScale = (wd + ht) / (Width + Height);
	m_bPrinting	  = true;
	RecalcRects(rect);
}

void CGraphics::RecalcRects(RECT& rt)
{
	//m_Rect = rt;
	m_ClientRect = rt;
	SetPixelRect(rt);
	m_bM = (m_Rect.bottom - m_Rect.top) / 6;
	m_tM = (m_Rect.bottom - m_Rect.top) / 10;
	m_lM = m_rM = (m_Rect.right - m_Rect.left) / 8;

	m_PlotRect.left		= m_Rect.left + m_lM;
	if (m_bEnableLegend)
		m_PlotRect.right= m_Rect.right - 3 * m_rM / 2;
	else
		m_PlotRect.right= m_Rect.right - m_rM;
	m_PlotRect.top		= m_Rect.top + m_tM;
	m_PlotRect.bottom	= m_Rect.bottom - m_bM;

	GL = m_PlotRect.left;
	GR = m_PlotRect.right;
	GT = m_PlotRect.top;
	GB = m_PlotRect.bottom;
	PX = GR - GL;
	PY = GB - GT;
	m_Scale.dx   = (m_Scale.xmax- m_Scale.xmin) / PX;
	m_Scale.dy   = (m_Scale.ymax- m_Scale.ymin) / PY;
	m_Size.cx = (PY < PX) ? PY : PX;
}

void CGraphics::BeginDraw(HDC hDC)						
{ 
	if (m_bPrinting || !m_bMemoryDraw)
		m_hDC = hDC;
	else
	{
		::GetClipBox(hDC, &m_ClipBox);
		m_hDC    = ::CreateCompatibleDC(hDC);
		m_Bitmap = ::CreateCompatibleBitmap(hDC, m_ClipBox.right - m_ClipBox.left, 
											m_ClipBox.bottom - m_ClipBox.top);
		m_OldBitmap = (HBITMAP)::SelectObject(m_hDC, m_Bitmap);
		::SetWindowOrgEx(m_hDC, m_ClipBox.left, m_ClipBox.top, NULL);
	}
	DrawBkGround();
}

void CGraphics::EndDraw(HDC hDC)
{
	if (m_bPrinting || !m_bMemoryDraw)
	{
		m_bPrinting = false;
		return;
	}

	::BitBlt(hDC, m_ClipBox.left, m_ClipBox.top, m_ClipBox.right - m_ClipBox.left,
			 m_ClipBox.bottom - m_ClipBox.top, m_hDC, m_ClipBox.left, m_ClipBox.top, SRCCOPY);
	::SelectObject(m_hDC, m_OldBitmap);
	::DeleteObject(m_Bitmap);
	::DeleteDC(m_hDC);
}

void CGraphics::DrawBkGround()
{
	HBRUSH hBrush = ::CreateSolidBrush(m_nBackColor);
    HBRUSH hBrold = (HBRUSH)::SelectObject(m_hDC, hBrush);
	::Rectangle(m_hDC, m_ClientRect.left, m_ClientRect.top, 
				m_ClientRect.right, m_ClientRect.bottom);
    ::SelectObject(m_hDC, hBrold);
    ::DeleteObject(hBrush);
}

void CGraphics::SetRatio(double xmin, double ymin, double xmax, double ymax)
{
	m_Ratio.xmin = xmin;
	m_Ratio.ymin = ymin;
	m_Ratio.xmax = xmax;
	m_Ratio.ymax = ymax;
}

void CGraphics::GetPixelRect(RECT& rt)
{
	rt.left	  = m_Rect.left;
	rt.top	  = m_Rect.top;
	rt.right  = m_Rect.right;
	rt.bottom = m_Rect.bottom;
}

void CGraphics::SetPixelRect(RECT rt)
{
	LONG Width    = rt.right  - rt.left;
    LONG Height   = rt.bottom - rt.top; 
	m_Rect.left   = (LONG)(rt.left + m_Ratio.xmin * Width);
	m_Rect.top	  = (LONG)(rt.top + m_Ratio.ymin * Height);
	m_Rect.right  = (LONG)(rt.right - (1 - m_Ratio.xmax) * Width);
	m_Rect.bottom = (LONG)(rt.bottom - (1 - m_Ratio.ymax) * Height);
}

void CGraphics::Title(const wchar_t* Title, int Pos)
{
	m_LogFont.lfHeight = (int)(m_Size.cx / -15.0);
	if (m_LogFont.lfHeight > -10) 
		m_LogFont.lfHeight = -10;
	m_LogFont.lfWeight	   = 700;
	m_LogFont.lfOrientation= 0;
	m_LogFont.lfEscapement = 0;
	m_Font = ::CreateFontIndirect(&m_LogFont);
	if (m_Font)
	{
	  	int bm  = ::SetBkMode(m_hDC, TRANSPARENT);
		HFONT hOldFont = (HFONT)::SelectObject(m_hDC, m_Font);
		::SetTextColor(m_hDC, RGB(0, 0, 0)); 
		SetStringAlign(CENTER, CENTER);
		if (Pos == TOP)
		{
			PrintString((GL + GR) / 2, (m_Rect.top + GT ) / 2 + 1, 0, Title);
			::SetTextColor(m_hDC, m_nTitleColor); 
			PrintString((GL + GR) / 2, (m_Rect.top + GT ) / 2, 0, Title);
		}
		else
		{
			int n = m_Rect.bottom - (m_Rect.bottom - GB) / 3;
			PrintString((GL + GR) / 2, n + 1, 0, Title);
			::SetTextColor(m_hDC, m_nTitleColor); 
			PrintString((GL + GR) / 2, n, 0, Title);
		}
		::SelectObject(m_hDC, hOldFont);
		::DeleteObject(m_Font);
		::SetBkMode(m_hDC, bm);
	}
}

void CGraphics::XAxisTitle(const wchar_t* Title, int Pos)
{
	m_LogFont.lfHeight = (int)(m_Size.cx / -20.0);
	if (m_LogFont.lfHeight > -10) 
		m_LogFont.lfHeight = -10;
	m_LogFont.lfWeight	   = 700;
	m_LogFont.lfOrientation= 0;
	m_LogFont.lfEscapement = 0;
	m_Font = ::CreateFontIndirect(&m_LogFont);
	if (m_Font)
	{
	  	int bm  = ::SetBkMode(m_hDC, TRANSPARENT);
		HFONT hOldFont = (HFONT)::SelectObject(m_hDC, m_Font);
		::SetTextColor(m_hDC, m_nTitleColor); 
		SetStringAlign(CENTER, CENTER);
		if (Pos == TOP)
			PrintString((GL + GR) / 2, (m_Rect.top + GT ) / 2, 0, Title);
		else
			PrintString((GL + GR) / 2, m_Rect.bottom - (m_Rect.bottom - GB) / 3, 0, Title);
		::SelectObject(m_hDC, hOldFont);
		::DeleteObject(m_Font);
		::SetBkMode(m_hDC, bm);
	}
}

void CGraphics::YAxisTitle(const wchar_t* Title, int Pos)
{
	m_LogFont.lfHeight = (int)(m_Size.cx / -20.0);
	if (m_LogFont.lfHeight > -10) 
		m_LogFont.lfHeight = -10;
	m_LogFont.lfWeight	   = 700;
	if (Pos == LEFT)
	{
		m_LogFont.lfOrientation= 900;
		m_LogFont.lfEscapement = 900;
	}
	else
	{
		m_LogFont.lfOrientation= -900;
		m_LogFont.lfEscapement = -900;
	}
	m_Font = ::CreateFontIndirect(&m_LogFont);
	if (m_Font)
	{
	  	int bm  = ::SetBkMode(m_hDC, TRANSPARENT);
		HFONT hOldFont = (HFONT)::SelectObject(m_hDC, m_Font);
		::SetTextColor(m_hDC, m_nTitleColor); 
		SetStringAlign(CENTER, CENTER);
		if (Pos == LEFT)
			PrintString(m_Rect.left + (GL - m_Rect.left) / 3, (GT + GB) / 2, 90, Title);
		else
			PrintString(m_Rect.right - (m_Rect.right - GR) / 3, (GT + GB) / 2, -90, Title);
		::SelectObject(m_hDC, hOldFont);
		::DeleteObject(m_Font);
		::SetBkMode(m_hDC, bm);
	}
}

void CGraphics::DrawBoundary(COLORREF cr, int size)
{
	HPEN hPen	= ::CreatePen(PS_SOLID, size, cr);
	HPEN hOldPen = (HPEN)::SelectObject(m_hDC, hPen);
	
	int n = 5;
	if (m_bPrinting)	n *= m_nPrintScale;
	DrawRectangle(m_Rect.left + n, m_Rect.top + n, 
				  m_Rect.right - n, m_Rect.bottom - n);

    ::SelectObject(m_hDC, hOldPen);
    ::DeleteObject(hPen);  
}

void CGraphics::Legend(COLORREF cr, int Index, const wchar_t* Name)
{
	m_LogFont.lfHeight = (int)(m_Size.cx / -25.0);
	if (m_LogFont.lfHeight > -10) 
		m_LogFont.lfHeight = -10;
	m_LogFont.lfWeight	   = 500;
	m_LogFont.lfOrientation= 0;
	m_LogFont.lfEscapement = 0;
	m_Font = ::CreateFontIndirect(&m_LogFont);
	if (m_Font)
	{
		int n  = (m_Rect.right - GR) / 20 + 1;
		int xb = GR + 2 * n;
		int xe = xb + 4 * n;
		int y  = GT - 3 * Index * m_LogFont.lfHeight / 2;
		DrawLine(xb, y, xe, y);
	  	int bm  = ::SetBkMode(m_hDC, TRANSPARENT);
		HFONT hOldFont = (HFONT)::SelectObject(m_hDC, m_Font);
		::SetTextColor(m_hDC, cr); 
		SetStringAlign(LEFT, CENTER);
		PrintString(xe + n, y, 0, Name);
		::SelectObject(m_hDC, hOldFont);
		::DeleteObject(m_Font);
		::SetBkMode(m_hDC, bm);
	}
}

void CGraphics::DrawCircle(int x, int y, int radius)
{
    int x1 = x - radius;
    int y1 = y - radius;
    int x2 = x + radius;
    int y2 = y + radius;
	::Arc(m_hDC, x1, y1, x2, y2, x, y2, x, y2);
}

void CGraphics::DrawFilledCircle(int x, int y, int radius)
{
    int x1 = x - radius;
    int y1 = y - radius;
    int x2 = x + radius;
    int y2 = y + radius;
    ::Ellipse(m_hDC, x1, y1, x2, y2);
}

void CGraphics::DrawRectangle(int x1, int y1, int x2, int y2)
{
	::MoveToEx(m_hDC, x1, y1, NULL);
	::LineTo(m_hDC, x1, y2);
	::LineTo(m_hDC, x2, y2);
	::LineTo(m_hDC, x2, y1);
	::LineTo(m_hDC, x1, y1);
}

void CGraphics::PrintString(int x, int y, int theta, const wchar_t* fmt)
{
	WORD    Height, Width;
	UINT    PreSet;
	double  thta;
	
	PreSet = ::SetTextAlign(m_hDC, TA_LEFT|TA_TOP);

	SIZE  size;
	::GetTextExtentPoint32(m_hDC, fmt, lstrlen(fmt), &size);
	Height = (WORD)size.cy;
	Width  = (WORD)size.cx;
                                             
    thta   = PiV*theta/ConstV;
    if(m_StrAlign.HAlign == LEFT && m_StrAlign.VAlign == TOP)
	{
	} 
    else if(m_StrAlign.HAlign == LEFT && m_StrAlign.VAlign == CENTER)
	{     
		x = (int)(x - Height/2.*sin(thta));
		y = (int)(y - Height/2.*cos(thta));
	}
    else if(m_StrAlign.HAlign == CENTER && m_StrAlign.VAlign == TOP)
	{
		x = (int)(x - Width/2. * cos(thta));
		y = (int)(y + Width/2. * sin(thta));
	}
    else if(m_StrAlign.HAlign == CENTER && m_StrAlign.VAlign == CENTER)
	{
		x = (int)(x - Width/2. * cos(thta) - Height/2.*sin(thta));
		y = (int)(y + Width/2. * sin(thta) - Height/2.*cos(thta));
	}
    else if(m_StrAlign.HAlign == CENTER && m_StrAlign.VAlign == BOTTOM)
	{
		x = (int)(x - Width/2. * cos(thta) - Height*sin(thta));
		y = (int)(y + Width/2. * sin(thta) - Height*cos(thta));
	}
    else if(m_StrAlign.HAlign == RIGHT && m_StrAlign.VAlign == TOP)
	{
		x = (int)(x - Width * cos(thta));
		y = (int)(y + Width * sin(thta));
	}
    else if(m_StrAlign.HAlign == RIGHT && m_StrAlign.VAlign == CENTER)
	{
		x = (int)(x - Width * cos(thta) - Height/2.*sin(thta));
		y = (int)(y + Width * sin(thta) - Height/2.*cos(thta));
	}
    else if(m_StrAlign.HAlign == RIGHT && m_StrAlign.VAlign == BOTTOM)
	{
		x = (int)(x - Width * cos(thta) - Height*sin(thta));
		y = (int)(y + Width * sin(thta) - Height*cos(thta));
	}
    else //if(m_StrAlign.HAlign == LEFT && m_StrAlign.VAlign == BOTTOM)
	{             
		x = (int)(x - Height*sin(thta));
		y = (int)(y - Height*cos(thta));
	}                 
	::TextOut(m_hDC, x, y, fmt, lstrlen(fmt));
	::SetTextAlign(m_hDC, PreSet);
}

void CGraphics::Format(int decimal,wchar_t* str, float value)
{
	wchar_t tstr[24];
	switch(decimal)
	{
	case 0:
		swprintf(str,32, L"%.f", value);
		return;
	case 1:
		swprintf(tstr,32 ,L"%.1f", value);
		break;
	case 2:
		swprintf(tstr,32 ,L"%.2f", value);
		break;
	case 3:
		swprintf(tstr,32 ,L"%.3f", value);
		break;
	case 4:
		swprintf(tstr,32, L"%.4f", value);
		break;
	default:
		swprintf(tstr,32 ,L"%.5f", value);
		break;
	}

	if (fabs(value) > 0.99999)
	{
		char aa[32];
		_gcvt(atof(CW2A(tstr)), 12, aa);
		CA2W  ww(aa);
		
		wcscpy(str,ww);
		if (str[lstrlen(str)-1] == '.')
			str[lstrlen(str)-1] = '\0';
	}
	else
	{
		if (fabs(value) <1.0e-10)
			lstrcpy(str, L"0");
		else
		{
			lstrcpy(str, tstr);
			int n = lstrlen(str) - 1;
			while(str[n] == '0' && n >= 0)
				str[n--] = '\0';
		}
	}
	
}

void CGraphics::Grid()
{
	int i, j;                                    
                
	HPEN hPen	= ::CreatePen(PS_DOT, 0, m_nGridColor);
	HPEN hOldPen = (HPEN)::SelectObject(m_hDC, hPen);

	for(i = 1; i < XGridTicks; i ++)
	{
		j = GL + (int)(1.0 * i * (GR-GL) / XGridTicks);
		DrawLine(j, GT + 1, j, GB - 1);
	}

	for(i = 1; i < YGridTicks; i ++) 
	{
		j = GT + (int)(1.0 * i * (GB-GT) / YGridTicks);
		DrawLine(GL + 1, j, GR - 1, j);
	}

	::SelectObject(m_hDC, hOldPen);
	::DeleteObject(hPen);
}

void CGraphics::XAxis()
{
	int	  xb, yb, xe, ye;   
    int   i, j;                                  
	wchar_t  str[32];
	float value;

	yb=GB;
	j = XTicks / 10;
	for(i = 0; i <= XTicks; i ++) 
	{
		xb = xe = (int)(GL + 1.0 * PX * i / XTicks );
		if((i % j) == 0)
		{
			ye = GB + m_bM / 7;
			value = (float)(m_Scale.xmin + i * (m_Scale.xmax - m_Scale.xmin) / XTicks);
			Format(m_nXDecimal, str, value);
			PrintString(xb, GB + m_bM / 5, 0, str);
		}
		else 
			ye = GB + m_bM / 14;
		if (i > 0 && i < XTicks)
			DrawLine(xb, yb, xe, ye);
	}
}

void CGraphics::YAxis(int /*num*/)
{
    int	  xb, yb, xe, ye;   
    int   i, j;                                  
	wchar_t  str[32];
	float value;

	xe = GL;//+num*20;
	j = YTicks / 10;
	for(i = 0; i <= YTicks; i ++)     
	{
		yb = ye = (int)(GT + 1.0 * PY * i / YTicks );
		if((i % j) == 0) 
		{
			xb = GL - m_lM / 10;
			value = (float)(m_Scale.ymax - i * (m_Scale.ymax - m_Scale.ymin) / YTicks);
			Format(m_nYDecimal, str, value);
			PrintString(GL - m_lM / 6, yb, 0, str);
		}
		else
			xb = GL - m_lM / 20;
		if (i > 0 && i < YTicks)
			DrawLine(xb, yb, xe, ye);
	}    
}

void CGraphics::Ticks()
{
	SetStringAlign(CENTER, TOP);
	XAxis();
	SetStringAlign(RIGHT, CENTER);
	YAxis();
}

void CGraphics::RightYTick()
{
	if (m_bEnableLegend)
		return;

    int	  xb, yb=0, xe, ye;   
    int   i, j;                                  
	wchar_t  str[32];
	float value;

	xb = GR;
	SetStringAlign(LEFT, CENTER);
	if (m_nAxesType == XY || m_nAxesType == XLOG)
	{
		j = YTicks / 10;
		for(i = 0; i <= YTicks; i ++)     
		{
			yb = ye = (int)(GT + 1.0 * PY * i / YTicks );
			if((i % j) == 0) 
			{
				xe = GR + m_rM / 10;
				value = (float)(m_Scale.ymax - i * (m_Scale.ymax - m_Scale.ymin) / YTicks);
				Format(m_nYDecimal, str, value);
				PrintString(GR + m_rM / 6, yb, 0, str);
			}
			else
				xe = GR + m_rM / 20;
			if (i > 0 && i < YTicks)
				DrawLine(xb, yb, xe, ye);
		}
	}
	else
	{
		for(i = 1; i <= m_nYStep; i ++)
		{
			bool once=true;
			for(j = 1; j <= 10; j ++)
			{
				if (once == true)   
					xe = GR + m_rM / 10;
				else
					xe = GR + m_rM / 20;
				yb = ye = GB - (int)(log10((float)j) * PY / m_nYStep + 1.0 * (i - 1) * PY / m_nYStep);
				if (j == 1)
				{
					value = (float)(pow(10.0, m_Scale.ymin) * pow(10.0, i - 1));
					Format(m_nYDecimal, str, value);
					PrintString(GR + m_rM / 6, yb, 0, str);
				}
				if ((i != 1 || j != 1) && (i != m_nYStep || j != 10))
					DrawLine(xb, yb, xe, ye);
				once = false;
			}
		}

		value = (float)(pow(10.0, m_Scale.ymin) * pow(10.0, i - 1));
		Format(m_nYDecimal, str, value);
		PrintString(GR + m_rM / 6, yb, 0, str);
	}
}

void CGraphics::Axes()
{
  	int bm		 = ::SetBkMode(m_hDC, TRANSPARENT);
    HPEN hPen    = ::CreatePen(PS_SOLID, 0, m_nBorderColor);
    HPEN hOldPen = (HPEN)::SelectObject(m_hDC, hPen);

	DrawRectangle(GL, GT, GR, GB);
	m_LogFont.lfHeight = (int)(m_Size.cx / -25.0);
	if (m_LogFont.lfHeight > -10) 
		m_LogFont.lfHeight = -10;
	m_LogFont.lfWeight     = 500;
	m_LogFont.lfOrientation= 0;
	m_LogFont.lfEscapement = 0;
	m_Font = ::CreateFontIndirect(&m_LogFont);
	if (m_Font)
	{
		HFONT hOldFont = (HFONT)::SelectObject(m_hDC, m_Font);
		SetTextColor(m_hDC, m_nTickColor); 
		Ticks();
		RightYTick();
		::SelectObject(m_hDC, hOldFont);
		::DeleteObject(m_Font);
	}

    ::SelectObject(m_hDC, hOldPen);
    ::DeleteObject(hPen);  
	::SetBkMode(m_hDC, bm);
}

void CGraphics::DrawMarker(int x, int y, int mode, int n)
{
	if (m_bPrinting)	n *= m_nPrintScale;
    switch(mode)
	{
		case CROSS:
			DrawLine(x - n, y, x + n, y );
			DrawLine(x, y - n, x, y + n );
			break;

		case STAR:
			DrawLine(x - n, y, x + n, y );
			DrawLine(x - n / 2, (int)(y + n * sqrt(3.) / 2), x + n / 2, 
					(int)(y - n * sqrt(3.) / 2));
			DrawLine(x - n / 2, (int)(y - n * sqrt(3.) / 2), x + n / 2, 
					(int)(y + n * sqrt(3.) / 2));
			break;

		case FCIRCLE:
			DrawFilledCircle(x, y, n);
			break;

		case FTRIANGLE:
		{
			POINT  p[3];
			p[0].x = x - n;
			p[0].y = (int)(y + n * sqrt(3.) / 3);
			p[1].x = x + n;
			p[1].y = (int)(y + n * sqrt(3.) / 3);
			p[2].x = x;
			p[2].y = (int)(y - 2 * n * sqrt(3.) / 3);
			::Polygon(m_hDC, p, 3);
			break;
		}

		case XCROSS:
			DrawLine((int)(x + n * sqrt(2.) / 2),
					 (int)(y - n * sqrt(2.) / 2), 
					 (int)(x - n * sqrt(2.) / 2),
					 (int)(y + n * sqrt(2.) / 2));
			DrawLine((int)(x - n * sqrt(2.) / 2),
					 (int)(y - n * sqrt(2.) / 2), 
					 (int)(x + n * sqrt(2.) / 2),
					 (int)(y + n * sqrt(2.) / 2));
			break;

		case CIRCLE:
			DrawCircle(x, y, n);
			break;

		case TRIANGLE:
			DrawLine(x - n, (int)(y + n * sqrt(3.) / 3),
					 x + n, (int)(y + n * sqrt(3.) / 3));
			DrawLine(x + n, (int)(y + n * sqrt(3.) / 3),
					 x, (int)(y - 2 * n * sqrt(3.) / 3));
			DrawLine(x, (int)(y - 2 * n * sqrt(3.) / 3),
					 x - n, (int)(y + n * sqrt(3.) / 3));  
			break;

		case FSQUARE:
			::Rectangle(m_hDC, x - n, y - n, x + n, y + n);
			break;

		case SQUARE:      
			DrawRectangle(x - n, y - n, x + n, y + n );
			break;

		case FDIAMOND:
		{
			POINT  p[4];
			p[0].x = x - n;
			p[0].y = y;
			p[1].x = x;
			p[1].y = y + n;
			p[2].x = x + n;
			p[2].y = y;
			p[3].x = x;
			p[3].y = y - n;
			::Polygon(m_hDC, p, 4);
			break;
		}

		case DIAMOND:
			::MoveToEx(m_hDC, x - n, y, NULL);
			::LineTo(m_hDC, x, y + n);
			::LineTo(m_hDC, x + n, y);
			::LineTo(m_hDC, x, y - n);
			::LineTo(m_hDC, x - n, y);
			break;
	}
}

void CGraphics::DrawShadow(int n)
{
	HPEN m_CurPen = ::CreatePen(PS_SOLID, 0, RGB(127,127,127));
	HPEN m_OldPen = (HPEN)::SelectObject(m_hDC, m_CurPen);
	HBRUSH hBrush = ::CreateSolidBrush(RGB(127,127,127));
	HBRUSH hBrold = (HBRUSH)::SelectObject(m_hDC, hBrush);
	int w  = (m_Rect.right - GR) / 20 + 1;
	::Rectangle(m_hDC, GR + (int)(2.2 * w), GT - m_LogFont.lfHeight, 
				m_Rect.right - (int)(0.8 * w), GT - 2 * (n + 1) * m_LogFont.lfHeight);
	::SelectObject(m_hDC, hBrold);
	::DeleteObject(hBrush);
	::SelectObject(m_hDC, m_OldPen);
	::DeleteObject(m_CurPen);

	hBrush = ::CreateSolidBrush(RGB(255, 255, 255));
	hBrold = (HBRUSH)::SelectObject(m_hDC, hBrush);
	::Rectangle(m_hDC, GR + w, GT, m_Rect.right - 2 * w, 
				GT - 2 * (n + 1) * m_LogFont.lfHeight + m_LogFont.lfHeight);
	::SelectObject(m_hDC, hBrold);
	::DeleteObject(hBrush);
}
