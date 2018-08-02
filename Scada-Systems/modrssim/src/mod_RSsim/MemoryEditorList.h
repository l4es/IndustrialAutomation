#if !defined(AFX_MEMORYEDITORLIST_H__4B373BFC_B08A_4D3C_8400_6F3BFED68E95__INCLUDED_)
#define AFX_MEMORYEDITORLIST_H__4B373BFC_B08A_4D3C_8400_6F3BFED68E95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////
//
// FILE: MemoryEditorList.h : headder file
//
//
// See _README.CPP
//
/////////////////////////////////////////////////////////////////////////////

// List-Control columns are laid out as :
// Address | item0..9               (registers)
// Address | bit 0..15 | total      (digitals)
//
#define NUMBER_LISTCOLS    18    // max # columns including the left-index
#define WIDTH_LISTCOL1     86    // index col width
#define WIDTH_LISTCOLS     53    // other cols width
#define WIDTH_LISTCOLBIT   33    // if col is a bit, make it narrower
#define WIDTH_LISTCOLTOTAL 65    // bit "total" col width


/////////////////////////////////////////////////////////////////////////////
// CMemoryEditorList window

class CMemoryEditorList : public CListCtrl
{
// Construction
public:
	CMemoryEditorList();

   LRESULT GetListViewDISPINFO(LPARAM lParam);

   enum ListViewFormats_ { 
      VIEWFORMAT_DECIMAL =1,
      VIEWFORMAT_HEX,
      VIEWFORMAT_WORD,
      VIEWFORMAT_DWORD,
      VIEWFORMAT_LONG,
      VIEWFORMAT_FLOAT,    // 6 "%f"
      VIEWFORMAT_CHAR      // text-mode
   };

   void SetupColumns(LONG width, BOOL cloneAddressing);
   int GetColumnCount() {return(GetHeaderCtrl()->GetItemCount());};
   int GetDataColumnCount(); 

// helpers
   static void Format(DWORD memoryValue, ListViewFormats_ format, CString& text);
   static void Convert(DWORD memoryValue, ListViewFormats_ format, CString& text);
   static BOOL Is32BitFormat(ListViewFormats_ format);

// Attributes
   LONG GetAreaDisplayed() {return (m_memAreaDisplayed);};
   WORD GetDisplayedWidth() {return (m_displayableWidth);};
   void SetViewFormat(ListViewFormats_ fmt) {m_viewFormat = fmt;};
   ListViewFormats_ GetViewFormat();
   LPCTSTR GetViewFormatString();
   LPCTSTR GetFormatName();

   void SetAddressFormatHex(BOOL formatHEX = TRUE) {m_addressFormatHEX = formatHEX;};
   BOOL IsAddressFormatHex() {return m_addressFormatHEX;};
   LPCTSTR GetAddressFormat() { return (m_addressFormatHEX?"%04X":"%ld");};

// custom draw
   afx_msg void OnCustomdrawMyList ( NMHDR* pNMHDR, LRESULT* pResult );

// Operations
   LONG SetAreaDisplayed(LONG PLCArea);
   WORD SetDisplayedWidth(WORD width) {m_displayableWidth=width;return (m_displayableWidth);};
   ListViewFormats_ SetDataViewFormat(ListViewFormats_ fmt, BOOL invalidate = TRUE) {m_viewFormat=fmt;\
      if ((invalidate)&&(m_hWnd)) InvalidateRect(NULL); return (m_viewFormat);};

   LONG  m_selectedListPos[255/*MAX_MOD_MEMTYPES*/];
   COLORREF m_backSysColor;
   COLORREF m_backSysColorFade;

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMemoryEditorList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMemoryEditorList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMemoryEditorList)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

private:
   LONG m_memAreaDisplayed;      // index for current simulated type
   WORD  m_displayableWidth;     // # columns with data in them+ the index column
   ListViewFormats_ m_viewFormat;// data display format
   BOOL  m_addressFormatHEX;     // PLC addresses in Hex/Decimal?
   BOOL  m_cloneAddressing;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEMORYEDITORLIST_H__4B373BFC_B08A_4D3C_8400_6F3BFED68E95__INCLUDED_)
