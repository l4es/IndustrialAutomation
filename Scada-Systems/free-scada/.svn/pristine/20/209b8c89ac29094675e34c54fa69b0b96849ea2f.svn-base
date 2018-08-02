#include <PropertyList/PropertyList.h>

//////////////////////////////////////////////////////////////////////////
//Цветовой ComboBox
//Inplace Control для цветового ComboBox-a

const COLORREF C_RED = RGB(255,0,0);
const COLORREF C_GREEN = RGB(0,255,0);
const COLORREF C_BLUE = RGB(0,0,255);
const COLORREF C_YELLOW = RGB(255,255,0);


inline bool IsStdColor(COLORREF Color)
{
	return ((Color == C_RED) || (Color == C_GREEN) ||
		(Color == C_BLUE) || (Color == C_YELLOW));
}

inline string ColorAsText(COLORREF Color)
{
	string Text;
	switch(Color) {
	case C_RED: Text = "Красный"; break;
	case C_GREEN: Text = "Зеленый"; break;
	case C_BLUE: Text = "Синий"; break;
	case C_YELLOW: Text = "Желтый"; break;
	default:
		Text.resize(11);
		sprintf(&Text[0], "%03d %03d %03d", 
			GetRValue(Color), GetGValue(Color),GetBValue(Color));
	}
	return Text;
}


class CPropertyColorComboWindow : 
	public CPropertyComboWindow
{
public:
	DECLARE_WND_SUPERCLASS(_T("WTL_InplacePropertycolorCombo"), CStatic::GetWndClassName())

	typedef CPropertyComboWindow baseClass;

	BEGIN_MSG_MAP(CPropertyColorComboWindow)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		CHAIN_MSG_MAP( baseClass )	
		ALT_MSG_MAP(1) // Button
		CHAIN_MSG_MAP_ALT( baseClass, 1 )
		ALT_MSG_MAP(2) 
		CHAIN_MSG_MAP_ALT( baseClass, 2 )
	END_MSG_MAP()


	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc( m_hWnd );      
		RECT rcButton;
		m_wndButton.GetWindowRect(&rcButton);
		RECT rcClient;
		GetClientRect(&rcClient);
		rcClient.right -= rcButton.right - rcButton.left;
		DRAWITEMSTRUCT dis = { 0 };
		dis.hDC = dc;
		dis.hwndItem = m_wndList;
		dis.CtlID = 0;//m_wndList.GetDlgCtrlID();
		dis.CtlType = ODT_LISTBOX;
		dis.rcItem = rcClient;
		dis.itemState = ODS_DEFAULT | ODS_COMBOBOXEDIT;
		dis.itemID = m_wndList.GetCurSel();
		dis.itemData = (int) m_wndList.GetItemData(dis.itemID);
		//////////////////////////////////////////////////////////////////////////

		COLORREF color = (COLORREF)dis.itemData;
		RECT rcItem = dis.rcItem;

		string pText = ColorAsText(color);


		CBrush WhiteBrush;
		WhiteBrush.CreateSolidBrush(RGB(255,255,255));
		dc.FillRect(&rcItem, WhiteBrush);

		CBrush br;
		br.CreateSolidBrush(color);

		RECT rcText, rcColor;

		rcColor.left = rcItem.left + 3;
		rcColor.right = rcColor.left + 20;
		rcColor.top = rcItem.top + 3;
		rcColor.bottom = rcItem.bottom - 3;
		rcText.left = rcColor.right + 3;
		rcText.right = rcItem.right - 3;
		rcText.top = rcItem.top ;
		rcText.bottom = rcItem.bottom ;

		dc.SelectFont(m_wndList.GetFont());
		dc.DrawText(pText.c_str(), -1, 
			&rcText, 
			DT_LEFT | DT_SINGLELINE | DT_EDITCONTROL | DT_NOPREFIX | DT_END_ELLIPSIS | DT_VCENTER);

		dc.FillRect(&rcColor, br);

		//////////////////////////////////////////////////////////////////////////

		m_wndList.SendMessage(OCM_DRAWITEM, dis.CtlID, (LPARAM) &dis);
		return 0;
	}

};

//ListBox, который появляется при нажатии на кнопку цветового ComboBox-a
class  CColorListBox 
	:public CListBox
	,public COwnerDraw<CColorListBox>
{
	BEGIN_MSG_MAP(CColorListBox)
		CHAIN_MSG_MAP( COwnerDraw<CColorListBox>)
	END_MSG_MAP()

	BOOL m_bHandledOD;

	BOOL IsMsgHandled() const
	{
		return m_bHandledOD;
	}
	void SetMsgHandled(BOOL bHandled)
	{
		m_bHandledOD = bHandled;
	}

	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{

		bHandled = FALSE;
		return 0;
	}


	void DrawItem(LPDRAWITEMSTRUCT lpDIS)
	{
		if( lpDIS->itemID == -1 ) return; // If there are no list box items, skip this message. 
		CBrush br;
		CDCHandle dc(lpDIS->hDC);
		RECT rc = lpDIS->rcItem;

		COLORREF color = (COLORREF)lpDIS->itemData;
		RECT rcItem = lpDIS->rcItem;

		string pText = ColorAsText(color);
		if(!IsStdColor(color))
			pText = "...";

		br.CreateSolidBrush(color);
		RECT rcText, rcColor;
		rcColor.left = rcItem.left + 3;
		rcColor.right = rcColor.left + 20;
		rcColor.top = rcItem.top + 3;
		rcColor.bottom = rcItem.bottom - 3;
		rcText.left = rcColor.right + 3;
		rcText.right = rcItem.right - 3;
		rcText.top = rcItem.top ;
		rcText.bottom = rcItem.bottom ;

		dc.DrawText(pText.c_str(), -1, 
			&rcText, 
			DT_LEFT | DT_SINGLELINE | DT_EDITCONTROL | DT_NOPREFIX | DT_END_ELLIPSIS | DT_VCENTER);
		if(!IsStdColor(color))
			dc.Rectangle(&rcColor);
		else
			dc.FillRect(&rcColor, br);
	}
};

//ComboBox с цветными прямоугольниками//////////////////////////////////////////////////////////////////////////

class CPropertyColorComboItem : public CPropertyItem
{
public:
	CListBox m_ctrl;
	COLORREF m_value;

	CPropertyColorComboItem(LPCTSTR pstrName, LPARAM lParam) : 
	CPropertyItem(pstrName, lParam)
	{
	}
	HWND CreateInplaceControl(HWND hWnd, const RECT& rc) 
	{
		ATLASSERT(::IsWindow(m_ctrl));
		// Create window
		CPropertyColorComboWindow* win = new CPropertyColorComboWindow();
		ATLASSERT(win);
		RECT rcWin = rc;
		win->m_hWndCombo = m_ctrl;
		win->Create(hWnd, rcWin);
		ATLASSERT(::IsWindow(*win));

		::SetFocus(hWnd);
		return *win;
	}
	BYTE GetKind() const 
	{ 
		return PROPKIND_CONTROL; 
	}
	void DrawValue(PROPERTYDRAWINFO& di) 
	{ 
		RECT rc = di.rcItem;
		::InflateRect(&rc, 0, -1);
		
		CDCHandle dc(di.hDC);

		VARIANT val;
		VariantInit(&val);
		GetValue(&val);
		COLORREF color = (COLORREF)val.intVal;  //dis.itemData;
		RECT rcItem = rc;
		
		string pText = ColorAsText(color);

		CBrush br;
		br.CreateSolidBrush(color);
		RECT rcText, rcColor;
		rcColor.left = rcItem.left + 3;
		rcColor.right = rcColor.left + 20;
		rcColor.top = rcItem.top + 3;
		rcColor.bottom = rcItem.bottom - 3;

		rcText.left = rcColor.right + 3;
		rcText.right = rcItem.right - 3;
		rcText.top = rcItem.top ;
		rcText.bottom = rcItem.bottom ;

		dc.DrawText(pText.c_str(), -1, 
			&rcText, 
			DT_LEFT | DT_SINGLELINE | DT_EDITCONTROL | DT_NOPREFIX | DT_END_ELLIPSIS | DT_VCENTER);
		CBrush oldBrush = dc.SelectBrush(br);

		dc.FillRect(&rcColor, br);
	//	dc.Rectangle(&rcColor);
		dc.SelectBrush(oldBrush);
	}

	BOOL GetValue(VARIANT* pValue) const 
	{ 
		CComVariant v = (int) m_value;//m_ctrl.GetItemData(m_ctrl.GetCurSel());
		return SUCCEEDED( v.Detach(pValue) );
	}
	BOOL SetValue(HWND /*hWnd*/) 
	{      
		int iSel = m_ctrl.GetCurSel();
		CComVariant v = (int) m_ctrl.GetItemData(iSel);
		return SetValue(v); 
	}
	BOOL SetValue(const VARIANT& value)
	{
		ATLASSERT(value.vt==VT_I4);
		m_value = (COLORREF)value.lVal;
		for( int i = 0; i < m_ctrl.GetCount(); i++ ) {
			if( m_ctrl.GetItemData(i) == (DWORD_PTR) value.lVal ) 
			{
				m_ctrl.SetCurSel(i);

				return TRUE;
			}
		}
		return FALSE;
	}
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//Текстовый ComboBox

//Inplace Control для текстового ComboBox-a
class CPropertyTextComboWindow : 
	public CPropertyComboWindow
{
public:
	DECLARE_WND_SUPERCLASS(_T("WTL_InplacePropertyTextCombo"), CStatic::GetWndClassName())

	typedef CPropertyComboWindow baseClass;

	BEGIN_MSG_MAP(CPropertyTextComboWindow)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		CHAIN_MSG_MAP( baseClass )	
		ALT_MSG_MAP(1) // Button
		CHAIN_MSG_MAP_ALT( baseClass, 1 )
		ALT_MSG_MAP(2) 
		CHAIN_MSG_MAP_ALT( baseClass, 2 )
	END_MSG_MAP()


	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc( m_hWnd );      
		RECT rcButton;
		m_wndButton.GetWindowRect(&rcButton);
		RECT rcClient;
		GetClientRect(&rcClient);
		rcClient.right -= rcButton.right - rcButton.left;
		DRAWITEMSTRUCT dis = { 0 };
		dis.hDC = dc;
		dis.hwndItem = m_wndList;
		dis.CtlID = 0;//m_wndList.GetDlgCtrlID();
		dis.CtlType = ODT_LISTBOX;
		dis.rcItem = rcClient;
		dis.itemState = ODS_DEFAULT | ODS_COMBOBOXEDIT;
		dis.itemID = m_wndList.GetCurSel();
		dis.itemData = (int) m_wndList.GetItemData(dis.itemID);
		//////////////////////////////////////////////////////////////////////////
		CBrush Brush;
		Brush.CreateSolidBrush(RGB(255,255,255));
		dc.FillRect(&rcClient, Brush);

		char buffer[0xff];
		ZeroMemory(buffer, 0xff);
		m_wndList.GetText(dis.itemID, buffer);
		//  CDCHandle dc(di.hDC);
		dc.SelectFont(m_wndList.GetFont());
		dc.DrawText(buffer, -1,  &rcClient, 
			DT_LEFT | DT_SINGLELINE | DT_EDITCONTROL | DT_NOPREFIX | DT_END_ELLIPSIS | DT_VCENTER);
		//////////////////////////////////////////////////////////////////////////

		m_wndList.SendMessage(OCM_DRAWITEM, dis.CtlID, (LPARAM) &dis);
		return 0;
	}

};

//Свойство с текстовым ComboBox-ом

class CPropertyTextComboItem
	: public CPropertyComboItem
{
public:
	CPropertyTextComboItem(LPCTSTR pstrName, LPARAM lParam) : 
	  CPropertyComboItem(pstrName, lParam)
	  {

	  }

	  HWND CreateInplaceControl(HWND hWnd, const RECT& rc) 
	  {
		  ATLASSERT(::IsWindow(m_ctrl));
		  // Create window
		  CPropertyTextComboWindow* win = new CPropertyTextComboWindow();
		  ATLASSERT(win);
		  RECT rcWin = rc;
		  win->m_hWndCombo = m_ctrl;
		  win->Create(hWnd, rcWin);
		  ATLASSERT(::IsWindow(*win));
		  return *win;
	  }


	  void DrawValue(PROPERTYDRAWINFO& di) 
	  { 
		  RECT rc = di.rcItem;
		  ::InflateRect(&rc, 0, -1);
		  DRAWITEMSTRUCT dis = { 0 };
		  dis.hDC = di.hDC;
		  dis.hwndItem = m_ctrl;
		  dis.CtlID = m_ctrl.GetDlgCtrlID();
		  dis.CtlType = ODT_LISTBOX;
		  dis.rcItem = rc;
		  dis.itemState = ODS_DEFAULT | ODS_COMBOBOXEDIT;
		  dis.itemID = m_ctrl.GetCurSel();
		  dis.itemData = (int) m_ctrl.GetItemData(dis.itemID);

		  char buffer[0xff];
		  ZeroMemory(buffer, 0xff);
		  m_ctrl.GetText(dis.itemID, buffer);
		  CDCHandle dc(di.hDC);

		  dc.DrawText(buffer, -1, 
			  &rc, 
			  DT_LEFT | DT_SINGLELINE | DT_EDITCONTROL | DT_NOPREFIX | DT_END_ELLIPSIS | DT_VCENTER);

		  ::SendMessage(m_ctrl, OCM_DRAWITEM, dis.CtlID, (LPARAM) &dis);
	  }
};

inline HPROPERTY PropCreateTextComboControl(LPCTSTR pstrName, HWND hWnd, int iValue, LPARAM lParam = 0)
{
	ATLASSERT(::IsWindow(hWnd));
	CPropertyTextComboItem* prop = NULL;
	ATLTRY( prop = new CPropertyTextComboItem(pstrName, lParam) );
	ATLASSERT(prop);
	if( prop ) {
		prop->m_ctrl = hWnd;
		CComVariant vValue = iValue;
		prop->SetValue(vValue);
	}
	return prop;
}




inline HPROPERTY PropCreateColorComboControl(LPCTSTR pstrName, HWND hWnd, int iValue, LPARAM lParam = 0)
{
	ATLASSERT(::IsWindow(hWnd));
	CPropertyColorComboItem* prop = NULL;
	ATLTRY( prop = new CPropertyColorComboItem(pstrName, lParam) );
	ATLASSERT(prop);
	if( prop ) {
		prop->m_ctrl = hWnd;
		CComVariant vValue = iValue;
		prop->SetValue(vValue);
	}
	return prop;
}

