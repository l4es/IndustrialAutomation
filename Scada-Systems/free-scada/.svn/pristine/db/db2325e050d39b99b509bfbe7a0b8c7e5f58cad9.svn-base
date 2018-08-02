#if !defined(__BARLINE_H__)
#define __BARLINE_H__

#include "graphics.h"
#include <string>
using namespace std;

//////////////////////////////////////////////////////////////////////////////////
////// class CBarLine
template <class T>
class CBarLine : public CGraphics
{
public:
	CBarLine();

	virtual bool SetRange(double, double, double, double) { return true; }

	enum TicksType { TICKS_NONE, TICKS_VERT, TICKS_HORZ, TICKS_BOTH };
    void SetData(T* pData, int Rows, int Cols, string* RowLables, string* ColLables);                 
	T GetMaxValue();
	void Line(bool HasGrid = true, int LineStyle = PS_SOLID, int LineWidth = 0);
	void Bar(bool HasGrid = true, bool bVert = true);
	void Bar3D(bool HasGrid = true, bool bVert = true);
	void GanttBar(bool HasGrid = true, bool bVert = true);

protected: 
	virtual void Legend(COLORREF cr, int Index, const char* Name);
	double GetTickValue(T nMaxValue, int nNumDiv);
	int GetBarLineMetrics(int nHDiv, int nVDiv, SIZE& sizeGraph, 
						  SIZE& sizePixelsPerTick, POINT& ptOrigin);
	void AxesTicks(POINT ptOrigin, SIZE sizePixelsPerTick,
			  int nHorzTicks, int nVertTicks, int nDrawTicks);
	void PlotCaptions(POINT ptOrigin, SIZE sizePixelsPerTick, T nValue, double nValuePerTick, 
					  int nNumTicks, int nCharHeight, bool bVert = true);
	void PlotGanttCaptions(POINT ptOrigin, SIZE sizePixelsPerTick, double nValuePerTick, 
					  int nNumTicks, int nCharHeight, bool bVert = true);
	void PlotCaptions(POINT ptOrigin, SIZE sizePixelsPerTick, int nNumTicks, 
					  int nCharHeight, BOOL bVert);
    
	bool	m_bHasGrid;
	T*		m_pData;
	int		m_nDataRows;
	int		m_nDataCols;
	int		m_nStartRow;
	int		m_nStartCol;
	string* m_pRowsLable;
	string* m_pColsLable; 
	double	Graduations[21];	
};

///////////////////////////////////////////////////////////////////////////////
// CBarLine operations
template<class T>
CBarLine<T>::CBarLine()
{                           
    m_nStartRow		= 1;
	m_nStartCol		= 1;
	m_bHasGrid		= false;
	m_pData			= NULL;
	m_pColsLable	= NULL;
	Graduations[0]	= 0.1;
	Graduations[1]	= 0.2;
	Graduations[2]	= 0.5;
	Graduations[3]	= 0.75;
	Graduations[4]	= 1;
	Graduations[5]	= 2;
	Graduations[6]	= 5;
	Graduations[7]	= 10;
	Graduations[8]	= 20;
	Graduations[9]	= 50;
	Graduations[10] = 100;
	Graduations[11] = 200;
	Graduations[12] = 500;
	Graduations[13] = 1000;
	Graduations[14] = 2000;
	Graduations[15] = 5000;
	Graduations[16] = 10000;
	Graduations[17] = 20000;
	Graduations[18] = 50000;
	Graduations[19] = 100000;
	Graduations[20] = 0;

	SetRange(0, 0, 0, 0);
} 

template<class T>
void CBarLine<T>::SetData(T *pData, int Rows, int Cols, string* RowsLable, string* ColsLable)
{        
	m_pData			= pData;
    m_nDataRows		= Rows;
	m_nDataCols		= Cols;
	m_pRowsLable	= RowsLable;
	m_pColsLable	= ColsLable;                      
}

template<class T>
T CBarLine<T>::GetMaxValue()
{                          
    T* pnData = m_pData;
    T nMax    = *pnData;

    for (int i = m_nDataRows * m_nDataCols - 1; i--;)
	{
		if (*pnData > nMax)
			nMax = *pnData;
		pnData++;
	}

    return nMax;
} 

template<class T>
double CBarLine<T>::GetTickValue(T nMaxValue, int nNumDiv)
{
	// Determine values for vertical ticks
	T nValuePerTick = (T)(((int)nMaxValue + nNumDiv - 1) / nNumDiv);
	for (int i = 0; Graduations[i] > 0.; i++)
	{
		if (Graduations[i] >= nValuePerTick)
		{
			return Graduations[i];
		}
	}
	return Graduations[i-1];
}

template<class T>
int CBarLine<T>::GetBarLineMetrics(int nHDiv, int nVDiv, SIZE& sizeGraph, 
								 SIZE& sizePixelsPerTick, POINT& ptOrigin)
{                       
	SIZE  sizeText;

	::GetTextExtentPoint32(m_hDC, "M", 1, &sizeText);

	sizeGraph.cx = PX;
	sizeGraph.cy = PY;
                                            
	sizePixelsPerTick.cx = sizeGraph.cx / nHDiv;
	sizePixelsPerTick.cy = sizeGraph.cy / nVDiv;

	ptOrigin.x = GL;
	ptOrigin.y = GB;

	return sizeText.cy;
}

template<class T>
void CBarLine<T>::AxesTicks(POINT ptOrigin, SIZE sizePixelsPerTick,
						   int nHorzTicks, int nVertTicks, int nDrawTicks)
{
	int nHeight = sizePixelsPerTick.cy * nVertTicks;
	int nWidth  = sizePixelsPerTick.cx * nHorzTicks;

	// Draw Axes
	if (m_bHasGrid)
		DrawRectangle(ptOrigin.x, ptOrigin.y - nHeight, ptOrigin.x + nWidth, ptOrigin.y);
	else
	{
		::MoveToEx(m_hDC, ptOrigin.x, ptOrigin.y - nHeight, NULL);
		::LineTo(m_hDC, ptOrigin.x, ptOrigin.y);
		::LineTo(m_hDC, ptOrigin.x + nWidth, ptOrigin.y);
	}
	
	int nTickSize = (sizePixelsPerTick.cx / 20);
	if (nTickSize < 6)
		nTickSize = 6;

	if (nDrawTicks & TICKS_VERT)
	{
		// Draw Vertical Ticks
		int nVPos = ptOrigin.y - sizePixelsPerTick.cy;
		int xLeft  = ptOrigin.x - nTickSize;
		int xRight = ptOrigin.x;

		for (int i = 1; i < nVertTicks; i++)
		{
			DrawLine(xLeft, nVPos, xRight, nVPos);
			nVPos -= sizePixelsPerTick.cy;
		}

		if (m_bHasGrid)
		{
			nVPos  = ptOrigin.y - sizePixelsPerTick.cy;
			HPEN  hPen, hOldPen;
			hPen = ::CreatePen(PS_DOT, 0, m_nGridColor);
			hOldPen = (HPEN)::SelectObject(m_hDC, hPen);
			for (i = 1; i < nVertTicks; i++)
			{
				DrawLine(ptOrigin.x + 1, nVPos, ptOrigin.x + nWidth - 1, nVPos);
				nVPos -= sizePixelsPerTick.cy;
			}
			::SelectObject(m_hDC, hOldPen);
			::DeleteObject(hPen);
		}
	}

	if (nDrawTicks & TICKS_HORZ)
	{
		// Draw Horizontal Ticks
		int nHPos   = ptOrigin.x + sizePixelsPerTick.cx;
		int yTop    = ptOrigin.y;
		int yBottom = ptOrigin.y + nTickSize + 1;
		for (int i = 1; i < nHorzTicks; i++)
		{
			DrawLine(nHPos, yTop, nHPos, yBottom);
			nHPos += sizePixelsPerTick.cx;
		}

		if (m_bHasGrid)
		{
			nHPos = ptOrigin.x + sizePixelsPerTick.cx;
			HPEN  hPen, hOldPen;
			hPen = ::CreatePen(PS_DOT, 0, m_nGridColor);
			hOldPen = (HPEN)::SelectObject(m_hDC, hPen);
			for (i = 1; i < nHorzTicks; i++)
			{
				DrawLine(nHPos, ptOrigin.y - nHeight + 1, nHPos, ptOrigin.y - 1);
				nHPos += sizePixelsPerTick.cx;
			}
			::SelectObject(m_hDC, hOldPen);
			::DeleteObject(hPen);
		}
	}
}

template<class T>
void CBarLine<T>::PlotCaptions(POINT ptOrigin, SIZE sizePixelsPerTick, T nValue, 
							   double nValuePerTick, int nNumTicks, int nCharHeight, bool bVert)
{                 
    char  buf[80];
    
	int nTickSize = (bVert ? sizePixelsPerTick.cx : sizePixelsPerTick.cy) / 30;
	if (nTickSize < 2)
		nTickSize = 2;

	int nPos  = bVert ? ptOrigin.y : ptOrigin.x;
	int nStep = bVert ? -sizePixelsPerTick.cy : sizePixelsPerTick.cx;

	int bm = ::SetBkMode(m_hDC, TRANSPARENT);
	SetStringAlign(bVert ? RIGHT : CENTER, TOP);
	m_LogFont.lfHeight = (int)(m_Size.cx / -30.0);
	if (m_LogFont.lfHeight > -10) m_LogFont.lfHeight = -10;
	m_LogFont.lfWeight     = 500;
	m_LogFont.lfOrientation= 0;
	m_LogFont.lfEscapement = 0;
	m_Font = ::CreateFontIndirect(&m_LogFont);
	if (m_Font)
	{
		HFONT hOldFont = (HFONT)::SelectObject(m_hDC, m_Font);
		SetTextColor(m_hDC, m_nTickColor); 
		for (int i = 0; i < nNumTicks; i++)
		{
			Format(3, buf, nValue);
			if (bVert)
				PrintString(GL - m_lM / 6, nPos - nCharHeight / 2, 0, buf);
			else
				PrintString(nPos, GB + m_bM / 5, 0, buf);

			nValue += nValuePerTick;
			nPos   += nStep;
		}
		::SelectObject(m_hDC, hOldFont);
		::DeleteObject(m_Font);
	}
	::SetBkMode(m_hDC, bm);
}

template<class T>
void CBarLine<T>::PlotCaptions(POINT ptOrigin, SIZE sizePixelsPerTick, 
		                       int nNumTicks, int nCharHeight, BOOL bVert)
{
	int nTickSize = (bVert ? sizePixelsPerTick.cx : sizePixelsPerTick.cy) / 30;
	if (nTickSize < 2)
		nTickSize = 2;

	int nPos  = bVert ? ptOrigin.y : ptOrigin.x;
	int nStep = bVert ? -sizePixelsPerTick.cy : sizePixelsPerTick.cx;

	int bm = ::SetBkMode(m_hDC, TRANSPARENT);
	SetStringAlign(bVert ? RIGHT : CENTER, TOP);
		m_LogFont.lfHeight = (int)(m_Size.cx / -30.0);
	if (m_LogFont.lfHeight > -10) m_LogFont.lfHeight = -10;
	m_LogFont.lfWeight     = 500;
	m_LogFont.lfOrientation= 0;
	m_LogFont.lfEscapement = 0;
	m_Font = ::CreateFontIndirect(&m_LogFont);
	if (m_Font)
	{
		HFONT hOldFont = (HFONT)::SelectObject(m_hDC, m_Font);
		SetTextColor(m_hDC, m_nTickColor); 
		for (int i = 0; i < nNumTicks; i++)
		{
			if (bVert)
				PrintString(GL - m_lM / 6, nPos - nCharHeight / 2, 0, m_pColsLable[i].c_str());
			else
				PrintString(nPos, GB + m_bM / 5, 0, m_pColsLable[i].c_str());
			nPos   += nStep;
		}
		::SelectObject(m_hDC, hOldFont);
		::DeleteObject(m_Font);
	}
	::SetBkMode(m_hDC, bm);
}

template<class T>
void CBarLine<T>::PlotGanttCaptions(POINT ptOrigin, SIZE sizePixelsPerTick, 
									double nValuePerTick, int nNumTicks, int nCharHeight, bool bVert)
{                 
	int nTickSize = (bVert ? sizePixelsPerTick.cx : sizePixelsPerTick.cy) / 30;
	if (nTickSize < 2)
		nTickSize = 2;

	int nPos  = bVert ? ptOrigin.y : ptOrigin.x;
	int nStep = bVert ? -sizePixelsPerTick.cy : sizePixelsPerTick.cx;

	int bm = ::SetBkMode(m_hDC, TRANSPARENT);
	SetStringAlign(bVert ? RIGHT : CENTER, TOP);
	m_LogFont.lfHeight = (int)(m_Size.cx / -30.0);
	if (m_LogFont.lfHeight > -10) m_LogFont.lfHeight = -10;
	m_LogFont.lfWeight     = 500;
	m_LogFont.lfOrientation= 0;
	m_LogFont.lfEscapement = 0;
	m_Font = ::CreateFontIndirect(&m_LogFont);
	if (m_Font)
	{
		HFONT hOldFont = (HFONT)::SelectObject(m_hDC, m_Font);
		SetTextColor(m_hDC, m_nTickColor); 
		for (int i = 0; i < nNumTicks; i++)
		{
			if (bVert)
				PrintString(GL - m_lM / 6, nPos - nCharHeight / 2, 0, m_pRowsLable[i].c_str());
			else
				PrintString(nPos, GB + m_bM / 5, 0, m_pRowsLable[i].c_str());
			nPos   += nStep;
		}
		::SelectObject(m_hDC, hOldFont);
		::DeleteObject(m_Font);
	}
	::SetBkMode(m_hDC, bm);
}

template<class T>
void CBarLine<T>::Line(bool HasGrid, int LineStyle, int LineWidth)
{                   
	m_bHasGrid			 = HasGrid;
	T nMaxValue			 = GetMaxValue();  
	double nValuePerTick = GetTickValue(nMaxValue, 5);
	
	SIZE  sizeGraph;
	SIZE  sizePixelsPerTick;
	POINT ptOrigin;
	int nCharHeight = GetBarLineMetrics(m_nDataCols - 1, 5, sizeGraph, sizePixelsPerTick, ptOrigin);
	AxesTicks(ptOrigin, sizePixelsPerTick, m_nDataCols - 1, 5, TICKS_BOTH);
                        
	PlotCaptions(ptOrigin, sizePixelsPerTick, (T)0, nValuePerTick, 6, nCharHeight, TRUE);					
	PlotCaptions(ptOrigin, sizePixelsPerTick, m_nDataCols, nCharHeight, FALSE);                 
		                          
	T* pData = m_pData;
	if (m_bLegendShadow && m_bEnableLegend)
		DrawShadow(m_nDataRows);

	for (int nRow = 0; nRow < m_nDataRows; nRow ++)
	{
        HPEN  hPen, hOldPen;
        hPen = ::CreatePen(LineStyle, LineWidth, crTable[nRow + 1]);
        hOldPen = (HPEN)::SelectObject(m_hDC, hPen);
		int nHPos = ptOrigin.x;
		for (int nCol = 0; nCol < m_nDataCols; nCol ++)
		{               
		    int nVPos;
			 nVPos = (int)((*pData++) * sizePixelsPerTick.cy / nValuePerTick);									
			if (nCol == 0)
				::MoveToEx(m_hDC, nHPos, ptOrigin.y - nVPos, NULL);
			else
				::LineTo(m_hDC, nHPos, ptOrigin.y - nVPos);
			nHPos += sizePixelsPerTick.cx;
		}
		if (m_bEnableLegend)
			CGraphics::Legend(crTable[nRow + 1], nRow + 1, m_pRowsLable[nRow].c_str());
		::SelectObject(m_hDC, hOldPen);
        ::DeleteObject(hPen);
	}
}

template<class T>
void CBarLine<T>::Bar(bool HasGrid, bool bVert)
{                
	m_bHasGrid			 = HasGrid;
	T nMaxValue			 = GetMaxValue();  
	double nValuePerTick = GetTickValue(nMaxValue, 5);
	int nVTicks = bVert ? 5 : m_nDataCols;
	int nHTicks = bVert ? m_nDataCols : 5;

	SIZE  sizeGraph;
	SIZE  sizePixelsPerTick;
    POINT ptOrigin;
	int nCharHeight = GetBarLineMetrics(nHTicks, nVTicks, sizeGraph, sizePixelsPerTick, ptOrigin);

	AxesTicks(ptOrigin, sizePixelsPerTick, nHTicks, nVTicks, 
			  bVert ? TICKS_VERT : TICKS_HORZ);       
	PlotCaptions(ptOrigin, sizePixelsPerTick, (T)0, nValuePerTick,
				 6, nCharHeight, bVert);			 

	// offset Origin to plot column letters in middle of Division
	POINT ptTemp;
	ptTemp.x = ptOrigin.x;
	ptTemp.y = ptOrigin.y;
	if (!bVert)
		ptTemp.y -= sizePixelsPerTick.cy / 2;
	else
		ptTemp.x += sizePixelsPerTick.cx / 2;
    
	PlotCaptions(ptTemp, sizePixelsPerTick,
				 m_nDataCols, nCharHeight, !bVert);
				 
	// m_nDataRows bars per division, plus 1 bar width for space
	int nDataSetWidth = bVert ? sizePixelsPerTick.cx : sizePixelsPerTick.cy;
	int nBarWidth     = nDataSetWidth / (m_nDataRows + 1);
	int nSpaceWidth   = (nDataSetWidth - (m_nDataRows * nBarWidth)) / 2;

	// in the loop, xPos means distance from origin along Data Set axis
	// not necessarily horizontal position
	if (m_bLegendShadow && m_bEnableLegend)
		DrawShadow(m_nDataRows);

	for (int nCol = 0; nCol < m_nDataCols; nCol++)
	{
		int xPos = nDataSetWidth * nCol + nSpaceWidth;
		for (int nRow = 0; nRow < m_nDataRows; nRow++)
		{          
		    T   Value;
		    int yPos;                                          
			Value = m_pData[nRow * m_nDataCols + nCol];  
			yPos = (int)(Value*(bVert ? sizePixelsPerTick.cy : sizePixelsPerTick.cx)
			       / nValuePerTick);
    
            HBRUSH hBrush = ::CreateSolidBrush(crTable[nRow+1]);
            HBRUSH hBrold = (HBRUSH)::SelectObject(m_hDC, hBrush);

			if (bVert)
				::Rectangle(m_hDC, ptOrigin.x + xPos, ptOrigin.y - yPos,
						  ptOrigin.x + xPos + nBarWidth, ptOrigin.y);
			else
				::Rectangle(m_hDC, ptOrigin.x, ptOrigin.y - xPos - nBarWidth,
						  ptOrigin.x + yPos, ptOrigin.y - xPos);

			if (m_bEnableLegend && nCol == m_nDataCols - 1)
				Legend(crTable[nRow + 1], nRow + 1, m_pRowsLable[nRow].c_str());

			::SelectObject(m_hDC, hBrold);
            ::DeleteObject(hBrush);
			xPos += nBarWidth;
		}
	}
}

template<class T>
void CBarLine<T>::Bar3D(bool HasGrid, bool bVert)
{         
	m_bHasGrid			 = HasGrid;
	T nMaxValue			 = GetMaxValue();  
	double nValuePerTick = GetTickValue(nMaxValue, 5);
	int nVTicks = bVert ? 5 : m_nDataCols;
	int nHTicks = bVert ? m_nDataCols : 5;

	// Tell Chart Metrics that there is an extra division in both directions
	SIZE  sizeGraph;
	SIZE  sizePixelsPerTick;
	POINT ptOrigin;
	int nCharHeight = GetBarLineMetrics(nHTicks + 1, nVTicks + 1, sizeGraph, sizePixelsPerTick, ptOrigin);
	int nColWidth   = bVert ? sizePixelsPerTick.cx : sizePixelsPerTick.cy;
	int nDivHeight  = bVert ? sizePixelsPerTick.cy : sizePixelsPerTick.cx;

	// Determine offsets of each column
	int deltaX = sizePixelsPerTick.cx / m_nDataRows;
	int deltaY = sizePixelsPerTick.cy / m_nDataRows;
	int nHeight = sizePixelsPerTick.cy * nVTicks;
	int nWidth  = sizePixelsPerTick.cx * nHTicks;
	::MoveToEx(m_hDC, ptOrigin.x, ptOrigin.y - nHeight, NULL);
	::LineTo(m_hDC, ptOrigin.x, ptOrigin.y);
	::LineTo(m_hDC, ptOrigin.x + nWidth, ptOrigin.y);
	
	POINT ptOffset;
	ptOffset.x = ptOrigin.x;
	ptOffset.y = ptOrigin.y;
	ptOffset.x += m_nDataRows * deltaX;
	ptOffset.y -= m_nDataRows * deltaY;
	::MoveToEx(m_hDC, ptOffset.x, ptOffset.y - nHeight, NULL);
	::LineTo(m_hDC, ptOffset.x, ptOffset.y);
	::LineTo(m_hDC, ptOffset.x + nWidth, ptOffset.y);

	// Draw top and right edges of back of grid
	int nTop   = ptOffset.y - sizePixelsPerTick.cy * nVTicks;
	int nRight = ptOffset.x + sizePixelsPerTick.cx * nHTicks;
	::MoveToEx(m_hDC, ptOrigin.x, nTop + m_nDataRows * deltaY, NULL);
	::LineTo(m_hDC, ptOffset.x, nTop);
	::LineTo(m_hDC, nRight, nTop);
	::LineTo(m_hDC, nRight, ptOffset.y);
	::LineTo(m_hDC, nRight - m_nDataRows * deltaX, ptOrigin.y);

	if (m_bHasGrid)
	{
		HPEN  hPen, hOldPen;
		hPen = ::CreatePen(PS_DOT, 0, m_nGridColor);
		hOldPen = (HPEN)::SelectObject(m_hDC, hPen);
		// Draw Value lines
		if (bVert)
		{           // Horizontal lines for vertical bars
			int yPos = 0;
			for (int nTick = 0; nTick < nVTicks; nTick++)
			{
				::MoveToEx(m_hDC, ptOrigin.x, ptOrigin.y - yPos, NULL);
				::LineTo(m_hDC, ptOffset.x, ptOffset.y - yPos);
				::LineTo(m_hDC, nRight,     ptOffset.y - yPos);
				yPos += sizePixelsPerTick.cy;
			}
		}
		else        // Vertical Lines for Horizontal Bars
		{
			int xPos = 0;
			for (int nTick = 0; nTick < nHTicks; nTick++)
			{
				::MoveToEx(m_hDC, ptOrigin.x + xPos, ptOrigin.y, NULL);
				::LineTo(m_hDC, ptOffset.x + xPos, ptOffset.y);
				::LineTo(m_hDC, ptOffset.x + xPos, nTop);
				xPos += sizePixelsPerTick.cx;
			}
		} 
		::SelectObject(m_hDC, hOldPen);
		::DeleteObject(hPen);
	}   
	PlotCaptions(ptOrigin, sizePixelsPerTick, (T)0, nValuePerTick,
				 6, nCharHeight, bVert);					

	// offset Origin to plot column letters in middle of Division
	POINT ptTemp;
	ptTemp.x = ptOrigin.x;
	ptTemp.y = ptOrigin.y;
	if (!bVert)
		ptTemp.y -= sizePixelsPerTick.cy / 2;
	else
		ptTemp.x += sizePixelsPerTick.cx / 2;
                                                      
    PlotCaptions(ptTemp, sizePixelsPerTick, m_nDataCols, nCharHeight, !bVert);
    
	// 2/3 of division used for bar--centered in division
	int nDivWidth     = bVert ? sizePixelsPerTick.cx : sizePixelsPerTick.cy;
	int nBarWidth     = nDivWidth * 2 / 3;
	int nSpaceWidth   = nDivWidth / 6;

	// in the loop, xPos means distance from origin along Column axis
	// not necessarily horizontal position

	if (m_bLegendShadow && m_bEnableLegend)
		DrawShadow(m_nDataRows);

	for (int nRow = m_nDataRows; nRow--;)
	{
		int xPos    = (bVert ? deltaX : deltaY) * nRow + nSpaceWidth;
		int yOffset = (bVert ? deltaY : deltaX) * nRow;
		HBRUSH  hBrush, hBrold;
    
        hBrush = ::CreateSolidBrush(crTable[nRow+1]);
        hBrold = (HBRUSH)::SelectObject(m_hDC, hBrush);
		for (int nCol = 0; nCol < m_nDataCols; nCol++)
		{     
		    int yPos;
			float Value = m_pData[nRow * m_nDataCols + nCol]; 
			yPos = (int)(Value * (bVert ? sizePixelsPerTick.cy : sizePixelsPerTick.cx)
			    	/nValuePerTick) + yOffset;
			RECT rcFace;
			if (bVert)
			{
				rcFace.left   = ptOrigin.x + xPos;
				rcFace.top    = ptOrigin.y - yPos;
				rcFace.right  = rcFace.left + nBarWidth + 1;
				rcFace.bottom = ptOrigin.y - yOffset;
			}
			else
			{
				rcFace.left   = ptOrigin.x + yOffset;
				rcFace.bottom = ptOrigin.y - xPos;
				rcFace.right  = ptOrigin.x + yPos + 1;
				rcFace.top    = rcFace.bottom - nBarWidth;
			}

			// Paint front face
			::Rectangle(m_hDC, rcFace.left, rcFace.top, rcFace.right, rcFace.bottom);
			rcFace.right--;

			// Paint top face
			POINT ptArray[4];
			ptArray[0].x = rcFace.left;
			ptArray[0].y = rcFace.top;
			ptArray[1].x = ptArray[0].x + deltaX;
			ptArray[1].y = ptArray[0].y - deltaY;
			ptArray[3].x = rcFace.right;
			ptArray[3].y = rcFace.top;
			ptArray[2].x = ptArray[3].x + deltaX;
			ptArray[2].y = ptArray[3].y - deltaY;
			::Polygon(m_hDC, ptArray, 4);

			// Leave points 2 & 3 the same
			// for right face
			ptArray[0].x = rcFace.right;
			ptArray[0].y = rcFace.bottom;
			ptArray[1].x = ptArray[0].x + deltaX;
			ptArray[1].y = ptArray[0].y - deltaY;
			::Polygon(m_hDC, ptArray, 4);

			xPos += nDivWidth;

			if (m_bEnableLegend && nCol == m_nDataCols - 1)
				Legend(crTable[nRow + 1], nRow + 1, m_pRowsLable[nRow].c_str());
		}
		::SelectObject(m_hDC, hBrold);
        ::DeleteObject(hBrush);
	}
}

template<class T>
void CBarLine<T>::GanttBar(bool HasGrid, bool bVert)
{      
	m_bHasGrid		  = HasGrid;
	double  nMaxValue = -1.0e-30;
	T* pData = m_pData;
	for (int nRow = 0; nRow < m_nDataRows; nRow++)
	{
		double nSum = 0.0;
		for (int nCol = 0; nCol < m_nDataCols; nCol++)
		   nSum += *pData++;
		if (nSum > nMaxValue)
		   nMaxValue = nSum;
	}
	double nValuePerTick = GetTickValue(nMaxValue, 5);  
	  
	int nVTicks = bVert ? 5 : m_nDataRows;
	int nHTicks = bVert ? m_nDataRows : 5;
	SIZE sizeGraph;
	SIZE sizePixelsPerTick;
	POINT ptOrigin;
	int nCharHeight = GetBarLineMetrics(nHTicks, nVTicks, sizeGraph, sizePixelsPerTick, ptOrigin);

	AxesTicks(ptOrigin, sizePixelsPerTick, nHTicks, nVTicks, bVert ? TICKS_VERT : TICKS_HORZ);
    PlotCaptions(ptOrigin, sizePixelsPerTick, (T)0, nValuePerTick,	6, nCharHeight, bVert);
	// offset Origin to plot row numbers in middle of Division
	POINT ptTemp;
	ptTemp.x = ptOrigin.x;
	ptTemp.y = ptOrigin.y;
	if (!bVert)
		ptTemp.y -= sizePixelsPerTick.cy / 2;
	else
		ptTemp.x += sizePixelsPerTick.cx / 2;
                        
    PlotGanttCaptions(ptTemp, sizePixelsPerTick, m_nStartRow, m_nDataRows, nCharHeight, !bVert);
	// 2/3 of division used for bar--centered in division
	int nDivWidth     = bVert ? sizePixelsPerTick.cx : sizePixelsPerTick.cy;
	int nBarWidth     = nDivWidth * 2 / 3;
	int nSpaceWidth   = nDivWidth / 6;

	// in the loop, xPos means distance from origin along Data Set axis
	// not necessarily horizontal position
	pData = m_pData;
	if (m_bLegendShadow && m_bEnableLegend)
		DrawShadow(m_nDataCols);

	for (nRow = 0; nRow < m_nDataRows; nRow++)
	{
		int xPos = nDivWidth * nRow + nSpaceWidth;

		int yPos = 0;
		for (int nCol = 0; nCol < m_nDataCols; nCol++)
		{
			int nValue = (int)((*pData++) * (bVert ? sizePixelsPerTick.cy : sizePixelsPerTick.cx) / nValuePerTick);		   
			HBRUSH hBrush = ::CreateSolidBrush(crTable[nCol+1]);
			HBRUSH hBrold = (HBRUSH)::SelectObject(m_hDC, hBrush);
			if (bVert)
				::Rectangle(m_hDC, ptOrigin.x + xPos, ptOrigin.y - yPos - nValue,
					ptOrigin.x + xPos + nBarWidth, ptOrigin.y - yPos + 1);
			else
				::Rectangle(m_hDC, ptOrigin.x + yPos, ptOrigin.y - xPos - nBarWidth,
					ptOrigin.x + yPos + nValue + 1, ptOrigin.y - xPos);

			if (m_bEnableLegend && nRow == m_nDataRows - 1)
				Legend(crTable[nCol + 1], nCol + 1, m_pColsLable[nCol].c_str());

			::SelectObject(m_hDC, hBrold);
			::DeleteObject(hBrush);
			yPos += nValue;
		}
	}
}

template<class T>
void CBarLine<T>::Legend(COLORREF cr, int Index, const char* Name)
{
	m_LogFont.lfHeight = (int)(m_Size.cx / -25.0);
	if (m_LogFont.lfHeight > -10) 
		m_LogFont.lfHeight = -10;
	m_LogFont.lfWeight	   = 700;
	m_LogFont.lfOrientation= 0;
	m_LogFont.lfEscapement = 0;
	m_Font = ::CreateFontIndirect(&m_LogFont);
	if (m_Font)
	{
		int n  = (m_Rect.right - GR) / 20 + 1;
		int xb = GR + 2 * n;
		int xe = xb + 4 * n;
		int y  = GT - 3 * Index * m_LogFont.lfHeight / 2;
		int y1 = y + m_LogFont.lfHeight / 3;
		int y2 = y - m_LogFont.lfHeight / 3;
		::Rectangle(m_hDC, xb, y1, xe, y2);
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

#endif
