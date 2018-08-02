// ----------------------------------------------------------------------------
// Written by Tony Ioanides (tonyi@bigpond.com)
// Copyright (c) 2003 Tony Ioanides.
//
// This code may be used in compiled form in any way you desire. This file may 
// be redistributed by any means PROVIDING it is not sold for profit without 
// the authors written consent, and providing that this notice and the authors 
// name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever.
// ----------------------------------------------------------------------------

#include "stdafx.h"
#include "MiniCalendarCtrl.h"

#include <algorithm>

#ifndef NUM_ELEMENTS
	#define NUM_ELEMENTS(a) (sizeof(a)/sizeof(*a))
#endif // NUM_ELEMENTS

// ----------------------------------------------------------------------------
static CRect
MC_GetArrowRect(CRect rectHeader, BOOL bLeftArrow)
{
	const int yMid = (rectHeader.top + rectHeader.bottom) / 2;
	const int cx = rectHeader.Height() / 5;

	rectHeader.top = yMid - cx;
	rectHeader.bottom = yMid + cx;

	if (bLeftArrow)
	{
		rectHeader.left += 6;
		rectHeader.right = rectHeader.left + cx;
	}
	else
	{
		rectHeader.right -= 6;
		rectHeader.left = rectHeader.right - cx;
	}

	return rectHeader;
}

// ----------------------------------------------------------------------------
static void
MC_DrawArrow(CDCHandle dc, CRect rectHeader, BOOL bLeftArrow)
{
	RECT rect = MC_GetArrowRect(rectHeader, bLeftArrow);

	if (bLeftArrow)
		std::swap(rect.left, rect.right);

	POINT ptTriangle[] = 
	{
		{ rect.left, rect.top },
		{ rect.left, rect.bottom },
		{ rect.right, (rect.top + rect.bottom) / 2 }
	};

	HBRUSH hbrOrig = dc.SelectStockBrush(BLACK_BRUSH);

	dc.Polygon(ptTriangle, NUM_ELEMENTS(ptTriangle));
	dc.SelectBrush(hbrOrig);
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Internal constants

enum
{
	nMonthsPerYear = 12,
	nDaysPerWeek = 7,
	nRowsPerCell = 6,
	nDaysPerCell = nRowsPerCell * nDaysPerWeek,
	nBorderSize = 4,
	nCellMargin = 10
};

#define ID_TIMER_FIRST  0xDEAD
#define ID_TIMER_REPEAT 0xC0DE

#define HEADER_FORMAT   _T("%B %Y")

// ----------------------------------------------------------------------------
CMiniCalendarCtrl::CMiniCalendarCtrl()
{
	m_crBack = GetSysColor(COLOR_WINDOW);

	m_bShow3dBorder = 1;
	m_bShowNonMonthDays = 1;
	m_bHighlightToday = 1;
	m_bMultiSelEnabled = 1;

	m_nCols = 1;
	m_nRows = 1;

	m_nFirstDayOfWeek = 1;
	m_nStartMonth = 1;
	m_nStartYear = 1970;
	m_nMonthsToScroll = 1;

	m_daySpace.cx = 4;
	m_daySpace.cy = 4;

	m_nActiveElement = 0;
}

// ----------------------------------------------------------------------------
BOOL
CMiniCalendarCtrl::IsDateSelected(const COleDateTime& date) const
{
	if (m_dtSelect.GetStatus() != COleDateTime::valid)
		return FALSE;

	if (date == m_dtSelect)
		return TRUE;
		
	if (m_bMultiSelEnabled && m_dtAnchor.GetStatus() == COleDateTime::valid)
		return m_dtSelect > m_dtAnchor ?
			(date >= m_dtAnchor && date <= m_dtSelect) :
			(date >= m_dtSelect && date <= m_dtAnchor);

	return FALSE;
}

// ----------------------------------------------------------------------------
SIZE
CMiniCalendarCtrl::GetMaxSize() const
{
	return GetMaxSize(m_nCols, m_nRows);
}

// ----------------------------------------------------------------------------
SIZE
CMiniCalendarCtrl::GetMaxSize(UINT nCols, UINT nRows) const
{
	const int cxyBorder = m_bShow3dBorder ? (nBorderSize * 2) : 0;

	return CSize(
		nCols * m_cellSize.cx + cxyBorder,
		nRows * m_cellSize.cy + cxyBorder);
}

// ----------------------------------------------------------------------------
BOOL 
CMiniCalendarCtrl::SetLayout(UINT nCols, UINT nRows)
{
	if (nCols > 0 && nRows > 0 && ((int) nCols != m_nCols || (int) nRows != m_nRows))
	{
		m_nCols = nCols; 
		m_nRows = nRows;

		AutoSize();
		return TRUE;
	}

	return FALSE;
}

// ----------------------------------------------------------------------------
BOOL
CMiniCalendarCtrl::SetFirstDayOfWeek(UINT nFirstDayOfWeek)
{
	if (nFirstDayOfWeek >= 1 && nFirstDayOfWeek <= nDaysPerWeek &&
		nFirstDayOfWeek != (UINT) m_nFirstDayOfWeek)
	{
		m_nFirstDayOfWeek = nFirstDayOfWeek;
		Invalidate();
		return TRUE;
	}

	return FALSE;
}

// ----------------------------------------------------------------------------
void
CMiniCalendarCtrl::AutoSize()
{
	const SIZE size = GetMaxSize();
	SetWindowPos(0, 0, 0, size.cx, size.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// ----------------------------------------------------------------------------
BOOL
CMiniCalendarCtrl::EnsureVisible(const COleDateTime& date)
{
	if (date.GetStatus() != COleDateTime::valid)
		return FALSE;

	if (date >= GetMonthFromCell(0, 0) &&
		date <= GetMonthFromCell(m_nCols - 1, m_nRows - 1))
		return FALSE;

	m_nStartMonth = date.GetMonth();
	m_nStartYear = date.GetYear();

	RedrawWindow();
	return TRUE;
}

// ----------------------------------------------------------------------------
BOOL
CMiniCalendarCtrl::ResetRange()
{
	if (m_dtAnchor.GetStatus() == COleDateTime::valid)
	{
		m_dtAnchor.SetStatus(COleDateTime::invalid);
		NotifySelChanged();

		Invalidate();
		return TRUE;
	}
	
	m_dtAnchor.SetStatus(COleDateTime::invalid);
	return FALSE;
}

// ----------------------------------------------------------------------------
LRESULT
CMiniCalendarCtrl::OnCreate(LPCREATESTRUCT lpcs) 
{
	hNotifyWindow = GetParent();

	m_font[FontHeader].bBold = TRUE;
	m_font[FontHeader].nFontSize = 10;

	m_font[FontSpecialDayNumber].bBold = TRUE;

	for (int i = 0; i < NUM_ELEMENTS(m_font); ++i)
		CreateFont(i);

	ApplyStyle(lpcs->style);
	
	RecalcLayout();
	AutoSize();
	EnsureVisible(COleDateTime::GetCurrentTime());

	SetMsgHandled(FALSE);
	return 0;
}

// ----------------------------------------------------------------------------
LRESULT
CMiniCalendarCtrl::OnStyleChanged(UINT nType, LPSTYLESTRUCT lpss)
{
	if (nType & GWL_STYLE)
		ApplyStyle(lpss->styleNew);

	SetMsgHandled(FALSE);
	return 0;
}

// ----------------------------------------------------------------------------
LRESULT 
CMiniCalendarCtrl::OnTimer(UINT nID, TIMERPROC /*fnProc*/)
{
	switch (nID)
	{
		case ID_TIMER_FIRST:
			KillTimer(ID_TIMER_FIRST);
			if (m_nActiveElement & htHeader)
			{
				DoScroll(m_nActiveElement, m_nMonthsToScroll);

				int nElapse = 250;
				DWORD nRepeat = 40;

				if (SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &nRepeat, 0))
					nElapse = 10000 / (5 * nRepeat + 25);	// milli-seconds, approximated
				SetTimer(ID_TIMER_REPEAT, nElapse);
			}
			break;

		case ID_TIMER_REPEAT:
			if (m_nActiveElement & htHeader)
				DoScroll(m_nActiveElement, m_nMonthsToScroll);
			else if (GetCapture() != m_hWnd)
				KillTimer(ID_TIMER_REPEAT);
			break;

		default:
			break;
	}

	return 0;
}

// ----------------------------------------------------------------------------
LRESULT
CMiniCalendarCtrl::OnMouseMove(UINT /*nFlags*/, CPoint point) 
{
	if (GetCapture() == m_hWnd)
	{
		m_nActiveElement &= ~htHeader;

		HitTestInfo ht;
		ht.ptHit = point;

		if (HitTest(ht))
		{
			if ((m_nActiveElement & htDate) && (ht.nFlags & htDate) && ht.dtHit != m_dtSelect)
			{
				m_dtSelect = ht.dtHit;

				if (ht.nFlags & (htBack | htNext))
					DoScroll(ht.nFlags);
				else
					RedrawWindow();

				NotifySelChanged();
			}

			if ((m_nActiveElement & (htBack | htNext)) &&
				ht.nFlags == UINT(htHeader | m_nActiveElement))
			{
				ATLTRACE("ResumeScroll\n");
				m_nActiveElement = ht.nFlags;
			}
		}
	}

	return 0;
}

// ----------------------------------------------------------------------------
LRESULT
CMiniCalendarCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_nActiveElement = 0;

	HitTestInfo ht;
	ht.ptHit = point;

	if (HitTest(ht)) 
	{
		SetCapture();
		m_nActiveElement = ht.nFlags;

		if (m_nActiveElement & htHeader)
		{
			if (m_nActiveElement & (htBack | htNext))
			{
				DoScroll(ht.nFlags, m_nMonthsToScroll);

				int nElapse = 250;
				int nDelay = 0;
				if (SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &nDelay, 0))
					nElapse += nDelay * 250;	// all milli-seconds
				SetTimer(ID_TIMER_FIRST, nElapse);
			}
			else
			{
				// Show month list here.
			}
		}
		else if (m_nActiveElement & htButton)
		{
		}
		else if (m_nActiveElement & htDate)
		{
			if (m_dtAnchor.GetStatus() != COleDateTime::valid || ! (nFlags & MK_SHIFT))
				m_dtAnchor = ht.dtHit;

			m_dtSelect = ht.dtHit;

			if (m_nActiveElement & (htBack | htNext))
				DoScroll(ht.nFlags);
			else
				RedrawWindow();

			NotifySelChanged();
		}
		else
			ResetRange();
	}

	return 0;
}

// ----------------------------------------------------------------------------
LRESULT
CMiniCalendarCtrl::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/) 
{
	ReleaseCapture();
	m_nActiveElement = 0;
	return 0;
}

// ----------------------------------------------------------------------------
LRESULT
CMiniCalendarCtrl::OnCaptureChanged(HWND /*hWnd*/) 
{
	m_nActiveElement = 0;
	return 0;
}

// ----------------------------------------------------------------------------
void
CMiniCalendarCtrl::DrawBorder(CDCHandle dc, CRect& rectClient)
{
	if (m_bShow3dBorder)
	{
		static const struct
		{
			COLORREF crTL;
			COLORREF crBR;
		} border[] =
		{
			{ COLOR_3DFACE,    COLOR_BTNTEXT },
			{ COLOR_3DHILIGHT, COLOR_3DSHADOW },
			{ COLOR_3DFACE,    COLOR_3DFACE },
			{ COLOR_3DSHADOW,  COLOR_3DHILIGHT }
		};

		for (int i = 0; i < NUM_ELEMENTS(border); ++i)
		{
			dc.Draw3dRect(rectClient, 
				GetSysColor(border[i].crTL), 
				GetSysColor(border[i].crBR));
			rectClient.DeflateRect(1, 1);
		}
	}
}

// ----------------------------------------------------------------------------
int
CMiniCalendarCtrl::DrawHeader(CDCHandle dc, CPoint pt, int nCol, int nRow)
{
	CRect rectHeader(0, 0, m_cellSize.cx, m_cyHeader);
	rectHeader.OffsetRect(pt);

	// Draw 'button' part
	dc.Draw3dRect(rectHeader, 
		GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
	rectHeader.DeflateRect(1, 1);

	dc.FillSolidRect(rectHeader, GetSysColor(COLOR_3DFACE));
	rectHeader.DeflateRect(1, 1);

	// Draw the text
	const COleDateTime dt = GetMonthFromCell(nCol, nRow);
	const CString str = dt.Format(HEADER_FORMAT);

	SelectFont(dc, FontHeader);

	dc.DrawText(str, str.GetLength(), rectHeader, 
		DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Draw arrow(s) (if required)
	if (nRow == 0 && (nCol == 0 || nCol == m_nCols - 1))
	{
		rectHeader.SetRect(0, 0, m_cellSize.cx, m_cyHeader);
		rectHeader.OffsetRect(pt);

		if (nCol == 0)
			MC_DrawArrow(dc, rectHeader, TRUE);

		if (nCol == m_nCols - 1)
			MC_DrawArrow(dc, rectHeader, FALSE);
	}

	return m_cyHeader;
}

// ----------------------------------------------------------------------------
int 
CMiniCalendarCtrl::DrawDaysOfWeek(CDCHandle dc, CPoint pt, int /*nCol*/, int /*nRow*/)
{
	const int cxColumn = m_dateSize.cx + m_daySpace.cx;
	const int xBeg = pt.x + m_xCol;

	SelectFont(dc, FontDayName);

	// Draw day names.
	CRect rectDate(xBeg, pt.y, xBeg + cxColumn, pt.y + m_cyDayNames);

	for (int i = 1; i <= nDaysPerWeek; ++i)
	{
		dc.DrawText(GetDayOfWeekName(i), 1, rectDate,  
			DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
		rectDate.OffsetRect(cxColumn, 0);
	}
	rectDate.bottom += 4;

	// Draw separator.
	dc.SelectBrush(GetSysColorBrush(COLOR_3DSHADOW));
	dc.PatBlt(xBeg, rectDate.bottom - 2, 
		cxColumn * nDaysPerWeek + m_daySpace.cx, 1, PATCOPY);

	return rectDate.bottom - pt.y;
}

// ----------------------------------------------------------------------------
int 
CMiniCalendarCtrl::DrawDays(CDCHandle dc, CPoint pt, int nCol, int nRow)
{
	const int cxColumn = m_dateSize.cx + m_daySpace.cx;
	const int xBeg = pt.x + m_xCol;

	COleDateTime dt = GetFirstDayInCell(nCol, nRow);

	const COleDateTime dtCell = GetMonthFromCell(nCol, nRow);
	const COleDateTime dtToday = COleDateTime::GetCurrentTime();

	const BOOL bBegRange = nCol == 0 && nRow == 0;
	const BOOL bEndRange = nCol == m_nCols - 1 && nRow == m_nRows - 1;
	const int nThisMonth = dtCell.GetMonth();

	// Determine position of 'today' in this cell (if required).
	CPoint ptToday(-1, -1);

	if (m_bHighlightToday && 
		dtToday.GetMonth() == nThisMonth &&	dtToday.GetYear() == dtCell.GetYear())
	{
		const int nOffset = (dtToday - dt).GetDays();
		ptToday.x = nOffset % nDaysPerWeek;
		ptToday.y = nOffset / nDaysPerWeek;
	}

	SelectFont(dc, FontDayNumber);

	CRect rectDate(xBeg, pt.y, xBeg + cxColumn, pt.y + m_dateSize.cy);

	for (int iRow = 0; iRow < nRowsPerCell; ++iRow)
	{
		rectDate.left = xBeg;
		rectDate.right = rectDate.left + cxColumn;

		for (int nCol = 0; nCol < nDaysPerWeek; ++nCol)
		{
			const int nMonth = dt.GetMonth();

			if (nMonth == nThisMonth ||	(m_bShowNonMonthDays &&
					((bBegRange && dt < dtCell) || (bEndRange && dt > dtCell)) ) )
			{
				const CRect rectDigits(
					rectDate.left + m_daySpace.cx / 2,
					rectDate.top,
					rectDate.right + m_daySpace.cx / 2,
					rectDate.bottom + 1);

				COLORREF crText = (nMonth == nThisMonth) ?
					GetSysColor(COLOR_BTNTEXT) : GetSysColor(COLOR_GRAYTEXT);

				if (IsDateSelected(dt))
				{
					crText = COLOR_GRAYTEXT;

					dc.FillSolidRect(
						rectDigits.left, 
						rectDigits.top,
						rectDigits.Width(), 
						rectDate.Height() + m_daySpace.cy,
						GetSysColor(COLOR_BTNFACE));
				}

				TCHAR szBuffer[10];
				_itot(dt.GetDay(), szBuffer, 10);

				dc.SetTextColor(crText);
				dc.DrawText(szBuffer, _tcslen(szBuffer), rectDate,
					DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);

				// Check if date should be hilighted as today.
				if (ptToday.x == nCol && ptToday.y == iRow){
					CRect r = rectDigits;
					r.bottom += 3;
					dc.Draw3dRect(r, RGB(0x80,0,0), RGB(0x80,0,0));
				}
			}

			rectDate.OffsetRect(cxColumn, 0);
			dt += 1;
		}

		rectDate.OffsetRect(0, m_dateSize.cy + m_daySpace.cy);
	}

	return rectDate.bottom - pt.y;
}

// ----------------------------------------------------------------------------
void
CMiniCalendarCtrl::DoPaint(CDCHandle dc)
{
	CRect rectClient;
	GetClientRect(rectClient);

	CRect rectClip;

	dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	dc.GetClipBox(rectClip);

	DrawBorder(dc, rectClient);

	for (int nRow = 0; nRow < m_nRows; ++nRow)
		for (int nCol = 0; nCol < m_nCols; ++nCol)
		{
			CRect rectCell(0, 0, m_cellSize.cx, m_cellSize.cy);

			rectCell.OffsetRect(
				nCol * m_cellSize.cx + rectClient.left,
				nRow * m_cellSize.cy + rectClient.top);

			if (rectClip & rectCell)
			{
				rectCell.top += DrawHeader(dc, rectCell.TopLeft(), nCol, nRow);
				dc.FillSolidRect(rectCell, m_crBack);

				rectCell.top += DrawDaysOfWeek(dc, rectCell.TopLeft(), nCol, nRow);
				DrawDays(dc, rectCell.TopLeft(), nCol, nRow);
			}
		}

	dc.RestoreDC(-1);
}

// ----------------------------------------------------------------------------
void
CMiniCalendarCtrl::NotifySelChanged()
{
	NMMCXSELCHANGED nm = {0};

	if (m_dtSelect.GetStatus() == COleDateTime::valid)
	{
		if (m_bMultiSelEnabled)
		{
			if (m_dtAnchor > m_dtSelect)
			{
				m_dtSelect.GetAsSystemTime(nm.dateFrom);
				m_dtAnchor.GetAsSystemTime(nm.dateTo);
			}
			else
			{
				m_dtAnchor.GetAsSystemTime(nm.dateFrom);
				m_dtSelect.GetAsSystemTime(nm.dateTo);
			}
		}
		else
		{
			m_dtSelect.GetAsSystemTime(nm.dateFrom);
			m_dtSelect.GetAsSystemTime(nm.dateTo);
		}
	}

	NotifyParent(MCXN_SELCHANGED, reinterpret_cast<NMHDR*>(&nm));
}

// ----------------------------------------------------------------------------
void
CMiniCalendarCtrl::NotifyParent(UINT nCode, LPNMHDR lpnmh)
{
	lpnmh->code = nCode;
	lpnmh->hwndFrom = m_hWnd;
	lpnmh->idFrom = GetDlgCtrlID();

	::SendMessage(hNotifyWindow,WM_NOTIFY, lpnmh->idFrom, (LPARAM) lpnmh);
}

// ----------------------------------------------------------------------------
BOOL
CMiniCalendarCtrl::HitTest(HitTestInfo& ht)
{
	ht.nFlags = 0;
	ht.nCol = -1;
	ht.nRow = -1;
	ht.dtHit.SetStatus(COleDateTime::invalid);

	CPoint ptHit = ht.ptHit;

	CRect rectClient;
	GetClientRect(rectClient);

	if (m_bShow3dBorder)
	{
		rectClient.DeflateRect(nBorderSize, nBorderSize);
		ptHit -= CSize(nBorderSize, nBorderSize); 
	}

	if (! rectClient.PtInRect(ht.ptHit))
		return FALSE;

	//rectClient.OffsetRect(-nBorderSize, -nBorderSize);

	// Determine which cell was hit.

	const int nCol = ptHit.x / m_cellSize.cx;
	const int nRow = ptHit.y / m_cellSize.cy;

	if (nCol >= m_nCols)
		return FALSE;

	// Determine if a button was hit.

	if (nRow >= m_nRows)
	{
		return FALSE;
	}

	ht.nCol = nCol;
	ht.nRow = nRow;

	// Determine which part of the cell was hit.

	ptHit.x %= m_cellSize.cx;
	ptHit.y %= m_cellSize.cy;

	if (ptHit.y < (int) m_cyHeader)
	{
		ht.nFlags = htHeader;

		const CRect rectHeader(0, 0, m_cellSize.cx, m_cyHeader);

		if (nRow == 0)
		{
			CRect rl = MC_GetArrowRect(rectHeader, TRUE);
			CRect rr = MC_GetArrowRect(rectHeader, FALSE);
			rl.InflateRect(4,4);
			rr.InflateRect(4,4);
			
			if (nCol == 0 && rl.PtInRect(ptHit))
				ht.nFlags |= htBack;

			else if (nCol == m_nCols - 1 && rr.PtInRect(ptHit))
				ht.nFlags |= htNext;
		}

		return TRUE;
	}

	ptHit.y -= m_cyHeader + m_dateSize.cy + 4;

	// Determine if a date was hit.

	const int cxColumn = m_dateSize.cx + m_daySpace.cx;

	CRect rectDate(CPoint(m_xCol, 0), m_dateSize);

	for (int r = 0; r < nRowsPerCell; ++r)
	{
		rectDate.left = m_xCol;
		rectDate.right = rectDate.left + cxColumn;

		for (int c = 0; c < nDaysPerWeek; ++c)
		{
			if (rectDate.PtInRect(ptHit))
			{
				const COleDateTime dtCell = GetMonthFromCell(nCol, nRow);
				//const COleDateTime dtCell = m_dtSelect;

				const int nDay = r * nDaysPerWeek + c;
				const int nMonth = dtCell.GetMonth();

				const COleDateTime dtBeg = GetFirstDayInCell(nCol, nRow);
				const COleDateTime dtHit = dtBeg + COleDateTimeSpan(nDay);

				if (dtHit.GetMonth() == nMonth)
				{
					ht.nFlags = htDate;
					ht.dtHit = dtHit;
					return TRUE;
				}

				if (m_bShowNonMonthDays)
				{
					if (nCol == 0 && nRow == 0 && dtHit < dtCell)
					{
						ht.nFlags = htDate | htBack;
						ht.dtHit = dtHit;
						return TRUE;
					}

					if (nCol == m_nCols - 1 && nRow == m_nRows - 1 && dtHit > dtCell)
					{
						ht.nFlags = htDate | htNext;
						ht.dtHit = dtHit;
						return TRUE;
					}
				}

				return FALSE;
			}

			rectDate.OffsetRect(cxColumn, 0);
		}

		rectDate.OffsetRect(0, m_dateSize.cy + m_daySpace.cy);
	}

	return FALSE;

}

// ----------------------------------------------------------------------------
void
CMiniCalendarCtrl::DoScroll(UINT nFlags, int nMonths)
{
	ATLASSERT(nMonths > 0);

	if (nFlags & htBack)
		nMonths = -m_nMonthsToScroll;
	else if (nFlags & htNext)
		nMonths = m_nMonthsToScroll;
	else
		nMonths = 0;

	if (nMonths)
	{
		m_nStartYear += nMonths / nMonthsPerYear;
		m_nStartMonth += nMonths % nMonthsPerYear;

		if (m_nStartMonth < 1)
		{
			m_nStartMonth += nMonthsPerYear;
			--m_nStartYear;
		}
		else if (m_nStartMonth > nMonthsPerYear)
		{
			m_nStartMonth -= nMonthsPerYear;
			++m_nStartYear;
		}

		RedrawWindow();
	}
}

// ----------------------------------------------------------------------------
void
CMiniCalendarCtrl::GetMaxTextExtent(CDCHandle dc, SIZE& size)
{
	// Determine the maximum size for the range of day numbers.
	// Single-digit day numbers are obviously smaller, so start at 10.

	for (int i = 10; i <= 31; ++i)
	{
		TCHAR sz[10];
		_itot(i, sz, 10);

		SIZE sizeDate = {0};
		dc.GetTextExtent(sz, _tcslen(sz), &sizeDate);

		size.cx = max(sizeDate.cx, size.cx);
		size.cy = max(sizeDate.cy, size.cy);
	}
}

// ----------------------------------------------------------------------------
void
CMiniCalendarCtrl::RecalcLayout()
{
	ATLASSERT(IsWindow());

	CClientDC dc(m_hWnd);
	HFONT hFontOrig = dc.SelectStockFont(DEFAULT_GUI_FONT);

	m_dateSize = CSize(0, 0);
	
	SelectFont(dc.m_hDC, FontDayNumber);
	GetMaxTextExtent(dc.m_hDC, m_dateSize);

	SelectFont(dc.m_hDC, FontSpecialDayNumber);
	GetMaxTextExtent(dc.m_hDC, m_dateSize);

	// Now check names of weekdays.

	m_cyDayNames = 0;
	SelectFont(dc.m_hDC, FontDayName);

	LPCTSTR szNames = _T("SMTWF");

	for (size_t i = 0; i < _tcslen(szNames); ++i)
	{
		SIZE size = {0};
		dc.GetTextExtent(szNames + i, 1, &size);

		m_dateSize.cx = max(size.cx, m_dateSize.cx);
		m_cyDayNames = max((UINT) size.cy, m_cyDayNames);
	}

	// Lastly, calculate the header size.

	const int cxWeek = (m_dateSize.cx + m_daySpace.cx) * nDaysPerWeek + m_daySpace.cx;
	const int cxCalendar = cxWeek + nCellMargin * 2;

	int cxHeader = 0;

	m_cyHeader = 0;
	SelectFont(dc.m_hDC, FontHeader);

	CString str;

	for (int nYear = 1990; nYear < 2020; ++nYear)
		for (int nMonth = 1; nMonth <= nMonthsPerYear; ++nMonth)
		{
			const COleDateTime dt(nYear, nMonth, 1, 0, 0, 0);
			str = dt.Format(HEADER_FORMAT);

			SIZE size = {0};
			dc.GetTextExtent(str, str.GetLength(), &size);

			cxHeader = max(size.cx, cxHeader);
			m_cyHeader = max((UINT) size.cy, m_cyHeader);
		}

	// Header padding.
	m_cyHeader += 6;

	m_cellSize.cx = max(cxHeader + 30, cxCalendar);

	m_cellSize.cy = m_cyHeader + (m_cyDayNames + 4) + 
		(m_dateSize.cy + m_daySpace.cy) * nRowsPerCell;

	m_xCol = (m_cellSize.cx - cxWeek) / 2;

	dc.SelectFont(hFontOrig);
}

// ----------------------------------------------------------------------------
void
CMiniCalendarCtrl::ApplyStyle(DWORD dwStyle)
{
	m_bShow3dBorder = (dwStyle & MCX_3DBORDER) != 0;
	m_bShowNonMonthDays = (dwStyle & MCX_SHOWNONMONTHDAYS) != 0;
	m_bHighlightToday = (dwStyle & MCX_HIGHLIGHTTODAY) != 0;
	m_bMultiSelEnabled = (dwStyle & MCX_MULTISELECT) != 0;

	if (! m_bMultiSelEnabled)
		ResetRange();

	Invalidate();
}

// ----------------------------------------------------------------------------
CString
CMiniCalendarCtrl::GetDayOfWeekName(int nDayOfWeek) const
{
	ATLASSERT(nDayOfWeek >= 1 && nDayOfWeek <= nDaysPerWeek);

	// NOTE: both m_nFirstDayOfWeek and nDayOfWeek are 1-based!

	// Apr 1, 2001 is known to be a Sunday, ie dayOfWeek == 1
	const COleDateTime dt =
		COleDateTime(2001, 4, 1, 0, 0, 0) + 
		COleDateTimeSpan((nDayOfWeek - 1) + (m_nFirstDayOfWeek - 1));

	return dt.Format(_T("%A"));
}

// ----------------------------------------------------------------------------
COleDateTime
CMiniCalendarCtrl::GetMonthFromCell(int nCol, int nRow) const
{
	ATLASSERT(nCol >= 0 && nRow >= 0);

	// NOTE: m_nMonth is 1-based!
	const int nMonth = (m_nStartMonth - 1) + nRow * m_nCols + nCol;
	
	return COleDateTime(m_nStartYear + nMonth / nMonthsPerYear, 
		nMonth % nMonthsPerYear + 1, 1, 0, 0, 0);
}

// ----------------------------------------------------------------------------
COleDateTime
CMiniCalendarCtrl::GetFirstDayInCell(int nCol, int nRow) const
{
	const COleDateTime dt = GetMonthFromCell(nCol, nRow);
	const int nPriorDays = 
		(dt.GetDayOfWeek() - m_nFirstDayOfWeek + nDaysPerWeek) % nDaysPerWeek;

	return dt - COleDateTimeSpan(nPriorDays);
}

// ----------------------------------------------------------------------------
void
CMiniCalendarCtrl::CreateFont(int nFont)
{
	ATLASSERT(nFont >= FontHeader && nFont <= FontSpecialDayNumber);

	FontInfo* pInfo = m_font + nFont;

	CFont font;
	font.CreatePointFont(pInfo->nFontSize * 10, pInfo->strFaceName);

	LOGFONT lf = {0};
	font.GetLogFont(&lf);	
	font.DeleteObject();

	{
		CClientDC dc(m_hWnd);
		lf.lfHeight	= -MulDiv(pInfo->nFontSize, dc.GetDeviceCaps(LOGPIXELSY), 72);
		lf.lfWeight = FW_NORMAL;
		lf.lfQuality = PROOF_QUALITY;
	}

	if (pInfo->bBold)
		lf.lfWeight = FW_BOLD;

	if (pInfo->bItalic)
		lf.lfItalic = TRUE;

	if (pInfo->bUnderline)
		lf.lfUnderline = TRUE;

	if (! pInfo->font.IsNull())
		pInfo->font.DeleteObject();

	pInfo->font.CreateFontIndirect(&lf);
}

// ----------------------------------------------------------------------------
void
CMiniCalendarCtrl::SelectFont(CDCHandle dc, int nFont)
{
	ATLASSERT(nFont >= FontHeader && nFont <= FontSpecialDayNumber);

	dc.SetTextColor(m_font[nFont].crColor);
	dc.SelectFont(m_font[nFont].font);
}

// ----------------------------------------------------------------------------
CMiniCalendarCtrl::FontInfo::FontInfo()
{
	strFaceName = _T("Tahoma");
	crColor = GetSysColor(COLOR_BTNTEXT);
	nFontSize = 9;
	bBold = FALSE;
	bItalic = FALSE;
	bUnderline = FALSE;
}

// ----------------------------------------------------------------------------
