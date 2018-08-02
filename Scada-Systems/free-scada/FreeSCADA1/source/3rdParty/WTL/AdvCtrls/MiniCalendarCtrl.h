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
//
// Thanks to:
// Matt Gullet   - MFC version of MSOutlook-style miniature calendar control
// Bjarke Viksoe - for atlgdix.h
// Daniel Bowen  - for COffscreenDraw
// 
// ...and all the folks that make CodeProject possible.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// TODO (as at May, 2003)
// - 'month selection' ctrl
// - 'today' and 'none' buttons
// - extended selection
// - internationalisation
// - higlighting "special date"s
// ----------------------------------------------------------------------------

#pragma once

#include "atlgdix.h"

#include <atlcrack.h>

#if _ATL_VER < 0x0700

#ifndef __ATLMISC_H__
#error MiniCalendarCtrl.h requires altmisc.h to be included first
#endif

#include "atl3comtime.h"

#else
#include <atlcomtime.h>
#endif	//_ATL_VER

// ----------------------------------------------------------------------------
// Custom window styles
#define MCX_3DBORDER			0x0001L
#define MCX_SHOWNONMONTHDAYS	0x0002L
#define MCX_HIGHLIGHTTODAY		0x0004L		
#define MCX_MULTISELECT			0x0008L
#define MCX_TODAYBUTTON			0x0010L	
#define MCX_NOSELBUTTON			0x0020L

#define MCX_STYLEMASK			0x00FFL

// ----------------------------------------------------------------------------
// Notifications

#define MCXN_FIRST		0U - 00U
#define MCXN_LAST		0U - 09U

#define MCXN_SELCHANGED	(MCN_FIRST -  1)	// (LPARAM)(LPNMMCXSELCHANGED)lpnmh
#define MCXN_LOSEFOCUS (MCN_FIRST - 2)

typedef struct tagNMMCXSELCHANGED
{
	NMHDR	   hdr;
	SYSTEMTIME dateFrom;
	SYSTEMTIME dateTo;
} NMMCXSELCHANGED, * LPNMMCXSELCHANGED;

class CMiniCalendarCtrl :
	public CWindowImpl<CMiniCalendarCtrl>, 
	public COffscreenDraw<CMiniCalendarCtrl>
// ----------------------------------------------------------------------------
{
public:

	CMiniCalendarCtrl();

	BOOL IsDateSelected(const COleDateTime& date) const;
	SIZE GetMaxSize() const;
	SIZE GetMaxSize(UINT nCols, UINT nRows) const;

	SIZE GetLayout() const
		{ return CSize(m_nCols, m_nRows); }
	BOOL SetLayout(UINT nCols, UINT nRows);

	UINT GetFirstDayOfWeek() const
		{ return m_nFirstDayOfWeek; }
	BOOL SetFirstDayOfWeek(UINT nFirstDayOfWeek);

	void AutoSize();
	void AutoConfigure() { }

	BOOL EnsureVisible(const COleDateTime& date);
	
	BOOL EnsureVisible(const SYSTEMTIME& st)
	{
		m_dtSelect = st;
		return EnsureVisible(COleDateTime(st));
	}

	BOOL EnsureVisible(const SYSTEMTIME& From,const SYSTEMTIME& To)
	{
		COleDateTime date(From);
		m_nStartMonth = date.GetMonth();
		m_nStartYear = date.GetYear();
		m_dtSelect = From;
		m_dtAnchor = To;
		RedrawWindow();
		return TRUE;
	}

	BOOL ResetRange();

	void SetNotifyWindow(HWND hWnd)
	{
		hNotifyWindow = hWnd;
	}

	SYSTEMTIME GetSelectedDateTime() const
	{
		SYSTEMTIME st;
		m_dtSelect.GetAsSystemTime(st);
		return st;
	}

/*	void SetSelectedDateTime(const SYSTEMTIME& st)
	{
		m_dtSelect = st;
		Invalidate();
	}
*/
	DECLARE_WND_CLASS(_T("MiniCalendarCtrl"))

protected:

	BEGIN_MSG_MAP_EX(CMiniCalendarCtrl)
		MSG_WM_CREATE(OnCreate)

		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_CAPTURECHANGED(OnCaptureChanged)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_KILLFOCUS(OnKillFocus)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_STYLECHANGED(OnStyleChanged)
		MSG_WM_LBUTTONDBLCLK(OnDblClick)

		MESSAGE_HANDLER(WM_SYSCOLORCHANGE, OnSysSettingChanged)
		MESSAGE_HANDLER(WM_DEVMODECHANGE, OnSysSettingChanged)
		MESSAGE_HANDLER(WM_FONTCHANGE, OnSysSettingChanged)
		MESSAGE_HANDLER(WM_TIMECHANGE, OnSysSettingChanged)

		CHAIN_MSG_MAP(COffscreenDraw<CMiniCalendarCtrl>)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpcs);
	LRESULT OnStyleChanged(UINT nType, LPSTYLESTRUCT lpss);

	LRESULT OnLButtonDown(UINT nFlags, CPoint point);
	LRESULT OnLButtonUp(UINT nFlags, CPoint point);
	LRESULT OnMouseMove(UINT nFlags, CPoint point);
	LRESULT OnCaptureChanged(HWND hWnd);
	LRESULT OnTimer(UINT nID, TIMERPROC fnProc);

	LRESULT OnSysSettingChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		RedrawWindow();
		return FALSE;
	}

	void DoPaint(CDCHandle dc);

	LRESULT OnKillFocus(HWND hWnd)
	{		
		NMHDR nmh;
		nmh.code = MCXN_LOSEFOCUS;
		nmh.hwndFrom = m_hWnd;
		nmh.idFrom = GetDlgCtrlID();

		::SendMessage(hNotifyWindow,WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
		SetMsgHandled(FALSE);
		return 0;
	}

	LRESULT OnKeyDown(TCHAR c,UINT RepeatCount,UINT)
	{
		switch(c){
		case VK_ESCAPE:
		case VK_RETURN:
			{
				NMHDR nmh;
				nmh.code = MCXN_LOSEFOCUS;
				nmh.hwndFrom = m_hWnd;
				nmh.idFrom = GetDlgCtrlID();

				::SendMessage(hNotifyWindow,WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
			}
		}
		SetMsgHandled(FALSE);
		return 0;
	}

	LRESULT OnDblClick(UINT uiKeys,POINT p)
	{
		NMHDR nmh;
		nmh.code = MCXN_LOSEFOCUS;
		nmh.hwndFrom = m_hWnd;
		nmh.idFrom = GetDlgCtrlID();

		::SendMessage(hNotifyWindow,WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
		SetMsgHandled(FALSE);
		return 0;
	}
protected:

	enum
	{
		htHeader = 0x0100,
		htDate   = 0x0200,
		htButton = 0x0400,
		htBack   = 0x0001,
		htNext   = 0x0002,
		htToday  = 0x0004,
		htNone   = 0x0008
	};

	struct HitTestInfo
	{
		POINT        ptHit;
		UINT         nFlags;
		int          nCol;
		int          nRow;
		COleDateTime dtHit;
	};

	// Metrics
	void RecalcLayout();
	void GetMaxTextExtent(CDCHandle dc, SIZE& size);

	// Interaction
	BOOL HitTest(HitTestInfo& ht);
	void DoScroll(UINT nFlags, int nMonths = 1);
	void NotifySelChanged();
	void NotifyParent(UINT nCode, LPNMHDR lpnmh);

	// Rendering
	// These should only be called from DoPaint - they rely on SaveDC/RestoreDC
	// to restore GDI objects!
	void DrawBorder(CDCHandle dc, CRect& rectClient);
	int  DrawHeader(CDCHandle dc, CPoint pt, int nCol, int nRow);
	int  DrawDaysOfWeek(CDCHandle dc, CPoint pt, int nCol, int nRow);
	int  DrawDays(CDCHandle dc, CPoint pt, int nCol, int nRow);

	void CreateFont(int nFont);
	void SelectFont(CDCHandle dc, int nFont);

	// State
	void ApplyStyle(DWORD dwStyle);

	// Utilities
	COleDateTime GetFirstDayInCell(int nCol, int nRow) const;
	COleDateTime GetMonthFromCell(int nCol, int nRow) const;
	CString      GetDayOfWeekName(int nDayOfWeek) const;

private:

	enum
	{
		FontHeader,
		FontDayName,
		FontDayNumber,
		FontSpecialDayNumber
	};

	struct FontInfo
	{
		FontInfo();

		CString  strFaceName;
		COLORREF crColor;
		int      nFontSize;
		BOOL     bBold:1;
		BOOL     bItalic:1;
		BOOL     bUnderline:1;
		CFont	 font;
	};

	// GDI resources
	COLORREF m_crBack;
	FontInfo m_font[4];

	// Layout
	int  m_nCols;
	int  m_nRows;
	int  m_nFirstDayOfWeek;	// 1-based (1-7; Sunday=1)
	int  m_nStartMonth;		// 1-based (1-12)
	int  m_nStartYear;
	int  m_nMonthsToScroll;	// Used when scrolling with arrows

	// Styles
	BOOL m_bShow3dBorder:1;
	BOOL m_bShowNonMonthDays:1;
	BOOL m_bHighlightToday:1;
	BOOL m_bMultiSelEnabled:1;

	// UI metrics (calculated in RecalcLayout)
	SIZE m_cellSize;		// Dimensions of a calendar for a month (incl. header)
	SIZE m_daySpace;		// Spacing between dates
	SIZE m_dateSize;		// Size of an individual date
	UINT m_cyHeader;		// Header height
	UINT m_cyDayNames;		// Height of weekday names
	UINT m_xCol;			// Offset of first column from left of cell

	// Interaction tracking
	int          m_nActiveElement;
	COleDateTime m_dtAnchor;
	COleDateTime m_dtSelect;

	HWND hNotifyWindow;
};
// ----------------------------------------------------------------------------
