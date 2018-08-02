#pragma once

//CPropertyAlignListItem - item, реализующий выпадающий список с опциями выравнивания текста
//СPropertyColorItem - реализует item для выбора цвета в диалоге выбора цвета
//CPropertyCustomListItem - базовый класс для выпадающих списков
//CPropertyEditItemEx - реализует item в виде Edit
//CPropertyFontListItem - реализует item в виде выпадающего списка со стандартными шрифтами
//CPropertyImageListItem - реализует item в виде выпадающего списка с изображениями проекта 
//CPropertySoundListItem - реализует item в виде выпадающего списка со звуками проекта 
//CPropertyPolylineListItem - реализует item с выпадающим списком, содержащим полилинии проекта
//CPropertySideListItem - реализует item с выпадающим списком, содержащим направления по которым можно изменять размеры
//CPropertyVariableListItem - реализует item, содержащий выпадающий список переменных проекта


// Include property base class
#include "PropertyList/PropertyItem.h"
#include "PropertyList/PropertyItemEditors.h"
#include "PropertyList/PropertyItemImpl.h"
#include <atlmisc.h>
#include <vector>
#include <string>
#include <algorithm>
#include "../MainFrm.h"

#include "../Library/ObjectMenager.h"

#ifndef __PROPERTYITEM__H
#error Prop.h requires PropertyItem.h to be included first
#endif

#ifndef __PROPERTYITEMEDITORS__H
#error Prop.h requires PropertyItemEditors.h to be included first
#endif

#ifndef __ATLBASE_H__
#error Prop.h requires atlbase.h to be included first
#endif

#define PACT_UPDATE_LIST		0x0008

class CPropertyColorWindow : 
	public CPropertyDropWindowImpl<CPropertyColorWindow, CStatic>
{
	typedef CPropertyDropWindowImpl<CPropertyColorWindow, CStatic> baseClass;
public:
	DECLARE_WND_SUPERCLASS(_T("WTL_InplacePropertyColorWindow"), CStatic::GetWndClassName())

	CPropertyColorWindow():m_prop(NULL){};
	IProperty* m_prop; // BUG: Dangerous reference


	BEGIN_MSG_MAP(CPropertyColorWindow)
		COMMAND_CODE_HANDLER(BN_CLICKED, OnButtonClicked)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		CHAIN_MSG_MAP( baseClass )
		ALT_MSG_MAP(1) // Button
		CHAIN_MSG_MAP_ALT( baseClass, 1 )
	END_MSG_MAP()

	LRESULT OnButtonClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ATLASSERT(m_prop);
		// Call Property class' implementation of BROWSE action
		m_prop->Activate(PACT_BROWSE, 0);
		::PostMessage(GetParent(), WM_USER_PROP_UPDATEPROPERTY, 0, (LPARAM) m_hWnd);
		return 0;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		CPaintDC dc(*this);
		
		CRect rc;
		GetClientRect(&rc);
		
		dc.FillSolidRect(rc, GetSysColor(COLOR_WINDOW));

		COLORREF color;
		if(m_prop)
		{
			CComVariant v;
			m_prop->GetValue(&v);
			v.ChangeType(VT_UI4);
			color = v.ulVal;
		}
		else
			color = RGB(128,128,128);

		CRect btn_rc;
		m_wndButton.GetClientRect(&btn_rc);
		rc.right -= btn_rc.Width();
		rc.DeflateRect(5,2);
		
		CRect frame_rc = rc;
		rc.DeflateRect(1,1);
		CPen pen;
		COLORREF inv_color;
//		inv_color = RGB(0xff-GetRValue(color),0xff-GetGValue(color),0xff-GetBValue(color));
		inv_color = GetSysColor(COLOR_WINDOWFRAME);
		pen.CreatePen(PS_SOLID, 1, inv_color);
		CPenHandle OldPen = dc.SelectPen(pen);
		dc.Rectangle(frame_rc);
		dc.SelectPen(OldPen);

		dc.FillSolidRect(rc, color);

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
		if( m_wndButton != lpdis->hwndItem ) return 0;
		CDCHandle dc(lpdis->hDC);
		// Paint as ellipsis button
		dc.DrawFrameControl(&lpdis->rcItem, DFC_BUTTON, (lpdis->itemState & ODS_SELECTED) ? DFCS_BUTTONPUSH | DFCS_PUSHED : DFCS_BUTTONPUSH);
		dc.SetBkMode(TRANSPARENT);
		LPCTSTR pstrEllipsis = _T("...");
		dc.DrawText(pstrEllipsis, ::lstrlen(pstrEllipsis), &lpdis->rcItem, DT_CENTER | DT_EDITCONTROL | DT_SINGLELINE | DT_VCENTER);
		return 0;
	}
};

//CPropertyEditItemEx - реализует item в виде Edit
class CPropertyEditItemEx : public CPropertyItem
{
protected:
	HWND m_hwndEdit;

public:
	CPropertyEditItemEx(LPCTSTR pstrName, LPARAM lParam) : 
	  CPropertyItem(pstrName, lParam), 
		  m_hwndEdit(NULL)
	  {
	  }
	  BYTE GetKind() const 
	  { 
		  return PROPKIND_EDIT; 
	  }
	  HWND CreateInplaceControl(HWND hWnd, const RECT& rc) 
	  {
		  // Get default text
		  UINT cchMax = GetDisplayValueLength() + 1;
		  LPTSTR pszText = (LPTSTR) _alloca(cchMax * sizeof(TCHAR));
		  ATLASSERT(pszText);
		  if( !GetDisplayValue(pszText, cchMax) ) return NULL;
		  // Create EDIT control
		  CPropertyEditWindow* win = new CPropertyEditWindow();
		  ATLASSERT(win);
		  RECT rcWin = rc;
		  m_hwndEdit = win->Create(hWnd, rcWin, pszText, WS_VISIBLE | WS_CHILD | ES_LEFT | ES_AUTOHSCROLL);
		  ATLASSERT(::IsWindow(m_hwndEdit));
		  // Simple hack to validate numbers
		  switch( m_val.vt ) {
case VT_UI1:
case VT_UI2:
case VT_UI4:
	win->ModifyStyle(0, ES_NUMBER);
		  }
		  return m_hwndEdit;
	  }
	  BOOL SetValue(const VARIANT& value)
	  {
		  if( m_val.vt == VT_EMPTY ) m_val = value;
		  if(m_val.vt == VT_R4 || m_val.vt == VT_R8)
		  {
			  CComVariant v(value);
			  v.ChangeType(VT_BSTR);

			  CComVariant tmp = _wtof(CString(v.bstrVal));
			  return SUCCEEDED( m_val.ChangeType(m_val.vt, &tmp) );
		  }
		  else
			  return SUCCEEDED( m_val.ChangeType(m_val.vt, &value) );
	  }
	  BOOL SetValue(HWND hWnd) 
	  { 
		  ATLASSERT(::IsWindow(hWnd));
		  int len = ::GetWindowTextLength(hWnd) + 1;
		  LPTSTR pstr = (LPTSTR) _alloca(len * sizeof(TCHAR));
		  ATLASSERT(pstr);
		  if( ::GetWindowText(hWnd, pstr, len) == 0 ) {
			  // Bah, an empty wstring AND an error causes the same return code!
			  if( ::GetLastError() != 0 ) return FALSE;
		  }
		  CComVariant v = pstr;
		  return SetValue(v);
	  }
	  BOOL Activate(UINT action, LPARAM /*lParam*/)
	  {
		  switch( action ) {
case PACT_TAB:
case PACT_SPACE:
case PACT_DBLCLICK:
	if( ::IsWindow(m_hwndEdit) ) {
		::SetFocus(m_hwndEdit);
		::SendMessage(m_hwndEdit, EM_SETSEL, 0, -1);
	}
	break;
		  }
		  return TRUE;
	  }
};

/////////////////////////////////////////////////////////////////////////////
// Simple property (displays color)

//СPropertyColorItem - реализует item для выбора цвета в диалоге выбора цвета
class CPropertyColorItem : public CProperty
{
protected:
	COLORREF m_val;

public:
	CPropertyColorItem(LPCTSTR pstrName, LPARAM lParam) : CProperty(pstrName, lParam)
	{
	}
	BYTE GetKind() const 
	{ 
		return PROPKIND_CONTROL; 
	}

	HWND CreateInplaceControl(HWND hWnd, const RECT& rc) 
	{
		// Create control
		CPropertyColorWindow* win = new CPropertyColorWindow();
		ATLASSERT(win);
		RECT rcWin = rc;
		win->m_prop = this;
		win->Create(hWnd, rcWin);
		ATLASSERT(win->IsWindow());
		return *win;
	}

	void DrawValue(PROPERTYDRAWINFO& di) 
	{
		CDCHandle dc(di.hDC);

		CRect rc = di.rcItem;
		rc.right -= rc.Height()-2;

		dc.FillSolidRect(rc, GetSysColor(COLOR_WINDOW));

		rc.DeflateRect(5,3);

		CRect frame_rc = rc;
		rc.DeflateRect(1,1);

		CPen pen;
		pen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWFRAME));
		CPenHandle OldPen = dc.SelectPen(pen);
		dc.Rectangle(frame_rc);

		dc.SelectPen(OldPen);

		dc.FillSolidRect(rc, m_val);
	}

	BOOL SetValue(const VARIANT& value)
	{
		CComVariant v(value);
		v.ChangeType(VT_UI4);
		m_val = v.ulVal;
		return TRUE;
	}
	BOOL SetValue(HWND /*hWnd*/) 
	{
		return TRUE;
	}
	BOOL GetValue(VARIANT* pValue) const
	{
		CComVariant v = (UINT)m_val;
		v.Detach(pValue);
		return TRUE;
	}
	BOOL Activate(UINT action, LPARAM /*lParam*/)
	{
		if(action == PACT_DBLCLICK)
		{
			// Let control owner know
			NMPROPERTYITEM nmh = { m_hWndOwner, ::GetDlgCtrlID(m_hWndOwner), PIN_BROWSE, this };
			::SendMessage(::GetParent(m_hWndOwner), WM_NOTIFY, nmh.hdr.idFrom, (LPARAM) &nmh);
		}

		if(action == PACT_BROWSE)
		{
			CColorDialog dlg(m_val);
			if(dlg.DoModal() == IDOK)
			{
				m_val = dlg.GetColor();
			}
		}
		return TRUE;
	}
};

/////////////////////////////////////////////////////////////////////////////
// Editor with dropdown list

class CPropertyListWindowEx : 
	public CPropertyDropWindowImpl<CPropertyListWindowEx>
{
public:
	DECLARE_WND_SUPERCLASS(_T("WTL_InplacePropertyListEx"), CEdit::GetWndClassName())

	CContainedWindowT<CListBox> m_wndList;
	int m_cyList;      // Used to resize the listbox when first shown

	typedef CPropertyDropWindowImpl<CPropertyListWindowEx> baseClass;

	BEGIN_MSG_MAP(CPropertyListWindowEx)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		COMMAND_CODE_HANDLER(BN_CLICKED, OnButtonClicked)
		CHAIN_MSG_MAP(baseClass)
		ALT_MSG_MAP(1) // Button
		CHAIN_MSG_MAP_ALT(baseClass, 1)
		ALT_MSG_MAP(2) // List
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
	END_MSG_MAP()

	void AddItem(LPCTSTR pstrItem)
	{
		ATLASSERT(m_wndList.IsWindow());
		ATLASSERT(!::IsBadStringPtr(pstrItem,-1));
		m_wndList.AddString(pstrItem);
		m_cyList = 0;
	}
	void SelectItem(int idx)
	{
		ATLASSERT(m_wndList.IsWindow());      
		m_wndList.SetCurSel(idx);
	}
	void DeleteAllItems()
	{
		ATLASSERT(m_wndList.IsWindow());      
		m_wndList.ResetContent();
		m_cyList = 0;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// Create dropdown list (as hidden)
		RECT rc = CWindow::rcDefault;
		m_wndList.Create(this, 2, m_hWnd, &rc, NULL, WS_POPUP | WS_BORDER | WS_VSCROLL);
		ATLASSERT(m_wndList.IsWindow());
		m_wndList.SetFont( CWindow(GetParent()).GetFont() );
		// Go create the rest of the control...
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if( m_wndList.IsWindow() ) m_wndList.DestroyWindow();
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		// Let the dropdown-box handle the keypress...
		if( m_wndList.GetStyle() & WS_VISIBLE ) {
			m_wndList.PostMessage(uMsg, wParam, lParam);
		}
		else {
			TCHAR szStr[] = { (TCHAR) wParam, _T('\0') };
			int idx = m_wndList.FindString(-1, szStr);
			if( idx == LB_ERR ) return 0;
			m_wndList.SetCurSel(idx);
			BOOL bDummy;
			OnKeyDown(WM_KEYDOWN, VK_RETURN, 0, bDummy);      
		}
		return 0; // Don't allow any editing
	}
	LRESULT OnButtonClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CListBox parent = GetParent();
		if(parent.GetCurSel() >= 0)
		{
			HPROPERTY CurProp = (HPROPERTY)(parent).GetItemData(parent.GetCurSel());
			if(CurProp)
				CurProp->Activate(PACT_UPDATE_LIST, (LPARAM)this);
		}

		if( m_cyList == 0 ) {
			// Resize list to fit all items (but not more than 140 pixels)
			const int MAX_HEIGHT = 140;
			int cy = m_wndList.GetCount() * m_wndList.GetItemHeight(0);
			m_cyList = min( MAX_HEIGHT, cy + (::GetSystemMetrics(SM_CYBORDER)*2) );
		}
		// Move the dropdown under the item
		RECT rcWin;
		GetWindowRect(&rcWin);
		RECT rc = { rcWin.left, rcWin.bottom, rcWin.right, rcWin.bottom + m_cyList };
		m_wndList.SetWindowPos(HWND_TOPMOST, &rc, SWP_SHOWWINDOW);
		return 0;
	}

	// List message handlers

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		switch( wParam ) {
case VK_RETURN:
	{
		int idx = m_wndList.GetCurSel();
		if( idx >= 0 ) {
			// Copy text from list to item
			int len = m_wndList.GetTextLen(idx) + 1;
			LPTSTR pstr = (LPTSTR) _alloca(len * sizeof(TCHAR));
			m_wndList.GetText(idx, pstr);
			SetWindowText(pstr);
			// Announce the new value
			::SendMessage(GetParent(), WM_USER_PROP_UPDATEPROPERTY, 0, (LPARAM) m_hWnd);
		}
	}
	::SetFocus(GetParent());
	break;
case VK_ESCAPE:
	// Announce the cancellation
	::SendMessage(GetParent(), WM_USER_PROP_CANCELPROPERTY, 0, (LPARAM) m_hWnd);
	::SetFocus(GetParent());
	break;
		}
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		LRESULT lRes = m_wndList.DefWindowProc();
		// Selected an item? Fake RETURN key to copy new value...
		BOOL bDummy;
		OnKeyDown(WM_KEYDOWN, VK_RETURN, 0, bDummy);
		return lRes;
	}
	LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		LRESULT lRes = m_wndList.DefWindowProc();
		m_wndList.ShowWindow(SW_HIDE);
		return lRes;
	}

	// Ownerdrawn button message handler

	LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
		if( m_wndButton != lpdis->hwndItem ) return 0;
		CDCHandle dc(lpdis->hDC);
		// Paint as dropdown button
		dc.DrawFrameControl(&lpdis->rcItem, DFC_SCROLL, (lpdis->itemState & ODS_SELECTED) ? DFCS_SCROLLDOWN | DFCS_PUSHED : DFCS_SCROLLDOWN);
		return 0;
	}
};

/////////////////////////////////////////////////////////////////////////////
// DropDown List property

//CPropertyCustomListItem - базовый класс для выпадающих списков
class CPropertyCustomListItem : public CPropertyItem
{
protected:
	//typedef std::vector<std::wstring> TStrings;
	TStrings m_List;
	HWND m_hwndCombo;
	bool m_UpdateListOnDropDown;

public:
	CPropertyCustomListItem(LPCTSTR pstrName, LPARAM lParam) 
		: CPropertyItem(pstrName, lParam)
		, m_hwndCombo(NULL)
		, m_UpdateListOnDropDown(false)
	  {
		  m_val = -1L;
	  }

	  virtual void FillList() = 0;

	  BYTE GetKind() const 
	  { 
		  return PROPKIND_LIST; 
	  }
	  HWND CreateInplaceControl(HWND hWnd, const RECT& rc) 
	  {
		  // Get default text
		  UINT cchMax = GetDisplayValueLength() + 1;
		  LPTSTR pszText = (LPTSTR) _alloca(cchMax * sizeof(TCHAR));
		  ATLASSERT(pszText);
		  GetDisplayValue(pszText, cchMax);
		  // Create 'faked' DropDown control
		  CPropertyListWindowEx* win = new CPropertyListWindowEx();
		  ATLASSERT(win);
		  RECT rcWin = rc;
		  m_hwndCombo = win->Create(hWnd, rcWin, pszText);
		  ATLASSERT(win->IsWindow());
		  // Add list
		  if(m_UpdateListOnDropDown)
		  {
			  m_List.clear();
			  FillList();
		  }
		  for(TStrings::iterator i = m_List.begin(); i != m_List.end(); i++ ) 
			  win->AddItem(i->c_str());

		  win->SelectItem(m_val.lVal);
		  // Go...
		  return *win;
	  }

	  BOOL Activate(UINT action, LPARAM lParam)
	  {
		  switch( action ) 
		  {
		  case PACT_UPDATE_LIST:
			  if(m_UpdateListOnDropDown)
			  {
				  m_List.clear();
				  FillList();

				  CPropertyListWindowEx* list = (CPropertyListWindowEx*)lParam;
				  if(list != NULL && list->IsWindow())
				  {
					  list->DeleteAllItems();
					  for(TStrings::iterator i = m_List.begin(); i != m_List.end(); i++ ) 
						  list->AddItem(i->c_str());

					  list->SelectItem(m_val.lVal);
				  }
			  }
			  break;
		  case PACT_SPACE:
			  if( ::IsWindow(m_hwndCombo) ) 
			  {
				  // Fake button click...
				  ::SendMessage(m_hwndCombo, WM_COMMAND, MAKEWPARAM(0, BN_CLICKED), 0);
			  }
			  break;
		  case PACT_DBLCLICK:
			  // Simulate neat VB control effect. DblClick cycles items in list...
			  // Set value and recycle edit control
			  if( IsEnabled() ) 
			  {
				  CComVariant v = m_val.lVal + 1;
				  ::SendMessage(m_hWndOwner, WM_USER_PROP_CHANGEDPROPERTY, (WPARAM) (VARIANT*) &v, (LPARAM) this);
			  }
			  break;
		  }
		  return TRUE;
	  }
	  BOOL GetDisplayValue(LPTSTR pstr, UINT cchMax) const
	  {
		  ATLASSERT(m_val.vt==VT_I4);
		  ATLASSERT(!::IsBadStringPtr(pstr, cchMax));
		  *pstr = _T('\0');
		  if( m_val.lVal < 0 || m_val.lVal >= (LONG)m_List.size() ) return FALSE;
		  USES_CONVERSION;
		  ::lstrcpyn( pstr, m_List[m_val.lVal].c_str(), cchMax) ;
		  return TRUE;
	  }
	  UINT GetDisplayValueLength() const
	  {
		  ATLASSERT(m_val.vt==VT_I4);
		  if( m_val.lVal < 0 || m_val.lVal >= (LONG)m_List.size() ) return 0;
		  return (UINT)m_List[m_val.lVal].size();
	  };

	  BOOL SetValue(const VARIANT& value)
	  {
		  switch( value.vt ) 
		  {
		  case VT_BSTR:
		  {
			  m_val = 0;
			  TStrings::iterator it = std::find(m_List.begin(), m_List.end(), (std::wstring)CString(value.bstrVal));
			  if(it != m_List.end())
				  m_val = it - m_List.begin();
			  else
				  m_val = -1L;
			  return TRUE;
		  }
		  default:
			  // Treat as index into list
			  if( FAILED( m_val.ChangeType(VT_I4, &value) ) ) return FALSE;
			  if( m_val.lVal >= (LONG)m_List.size() ) m_val.lVal = -1L;
			  return TRUE;
		  }
		  return FALSE;
	  }
	  BOOL SetValue(HWND hWnd)
	  { 
		  ATLASSERT(::IsWindow(hWnd));
		  int len = ::GetWindowTextLength(hWnd) + 1;
		  LPTSTR pstr = (LPTSTR) _alloca(len * sizeof(TCHAR));
		  ATLASSERT(pstr);
		  if( !::GetWindowText(hWnd, pstr, len) ) {
			  if( ::GetLastError() != 0 ) return FALSE;
		  }
		  CComVariant v = pstr;
		  return SetValue(v);
	  }
};

//CPropertyFontListItem - реализует item в виде списка со стандартными шрифтами
class CPropertyFontListItem 
	: public CPropertyCustomListItem
{
public:
	CPropertyFontListItem(LPCTSTR pstrName, LPARAM lParam)
		: CPropertyCustomListItem(pstrName, lParam)
	{
		FillList();
	};

	static int CALLBACK EnumFontFamExProc(
		ENUMLOGFONTEX *lpelfe,    // logical-font data
		NEWTEXTMETRICEX *lpntme,  // physical-font data
		DWORD FontType,           // type of font
		LPARAM lParam             // application-defined data
		)
	{
		if(FontType == TRUETYPE_FONTTYPE)
		{
			TStrings *list = (TStrings*)lParam;
			TStrings::iterator i = 
				find(list->begin(),list->end(), wstring(lpelfe->elfFullName));
			if(i == list->end())
				list->push_back(lpelfe->elfFullName);
		}
		return TRUE;
	};

	void FillList()
	{
		CDCHandle dc = ::GetDC(GetDesktopWindow());

		LOGFONT lf;
		ZeroMemory(&lf, sizeof(LOGFONT));
		lf.lfCharSet	= DEFAULT_CHARSET;

		EnumFontFamiliesEx(dc, &lf, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)&m_List, NULL);

		::ReleaseDC(GetDesktopWindow(), dc);

		sort(m_List.begin(), m_List.end());
	}

	BOOL GetValue(VARIANT* pVal) const
	{
		wchar_t buffer[0xff];
		GetDisplayValue(buffer, 0xff);
		_bstr_t str(buffer);
		pVal->vt = VT_BSTR;
		pVal->bstrVal = str;
		return S_OK;//SUCCEEDED( CComVariant(m_val).Detach(pVal) );
	}
};


class CPropertyImageListItem 
	: public CPropertyCustomListItem
{
public:
	CPropertyImageListItem(LPCTSTR pstrName, LPARAM lParam)
		: CPropertyCustomListItem(pstrName, lParam)
	{
		m_UpdateListOnDropDown = true;
		FillList();
	};

	void FillList()
	{
		if(g_MainFrame == NULL)
			return;
		if(!g_MainFrame->m_Doc.GetImageNames(m_List))
			return;

	}

	BOOL GetValue(VARIANT* pVal) const
	{
		wchar_t buffer[0xff];
		GetDisplayValue(buffer, 0xff);
		_bstr_t str(buffer);
		pVal->vt = VT_BSTR;
		pVal->bstrVal = str;
		return S_OK;//SUCCEEDED( CComVariant(m_val).Detach(pVal) );
	}
};


class CPropertySoundListItem 
	: public CPropertyCustomListItem
{
public:
	CPropertySoundListItem(LPCTSTR pstrName, LPARAM lParam)
		: CPropertyCustomListItem(pstrName, lParam)
	{
		m_UpdateListOnDropDown = true;
		FillList();
	};

	void FillList()
	{
		if(g_MainFrame == NULL)
			return;
		if(!g_MainFrame->m_Doc.GetSoundNames(m_List))
			return;

	}

	BOOL GetValue(VARIANT* pVal) const
	{
		wchar_t buffer[0xff];
		GetDisplayValue(buffer, 0xff);
		_bstr_t str(buffer);
		pVal->vt = VT_BSTR;
		pVal->bstrVal = str;
		return S_OK;//SUCCEEDED( CComVariant(m_val).Detach(pVal) );
	}
};

//CPropertyPolylineListItem - реализует item с выпадающим списком, содержащим полилинии проекта
class CPropertyPolylineListItem 
	: public CPropertyCustomListItem
{
public:
	CPropertyPolylineListItem(LPCTSTR pstrName, LPARAM lParam)
		: CPropertyCustomListItem(pstrName, lParam)
	{
		m_UpdateListOnDropDown = true;
		FillList();
	};

	void FillList()
	{
		CObjectMenagerPtr mng;
		list<CObject*> Objects = mng->FindObjects(_POLYLINE);
		if(!Objects.empty())
		{
			for(list<CObject*>::iterator it = Objects.begin(); it != Objects.end(); it++)
				m_List.push_back((*it)->GetName());
		}
	}

	BOOL GetValue(VARIANT* pVal) const
	{
		wchar_t buffer[0xff];
		GetDisplayValue(buffer, 0xff);
		_bstr_t str(buffer);
		pVal->vt = VT_BSTR;
		pVal->bstrVal = str;
		return S_OK;//SUCCEEDED( CComVariant(m_val).Detach(pVal) );
	}
};

//CPropertySideListItem - реализует item с выпадающим списком, содержащим направления по которым можно изменять размеры
class CPropertySideListItem 
	: public CPropertyCustomListItem
{
public:
	CPropertySideListItem(LPCTSTR pstrName, LPARAM lParam)
		: CPropertyCustomListItem(pstrName, lParam)
	{
		FillList();
	};

	void FillList()
	{
		m_List.push_back(L"Left");
		m_List.push_back(L"Right");
		m_List.push_back(L"Up");
		m_List.push_back(L"Down");
		m_List.push_back(L"All sides");
	}

	BOOL GetValue(VARIANT* pVal) const
	{
		wchar_t buffer[0xff];
		GetDisplayValue(buffer, 0xff);
		_bstr_t str(buffer);
		pVal->vt = VT_BSTR;
		pVal->bstrVal = str;
		return S_OK;//SUCCEEDED( CComVariant(m_val).Detach(pVal) );
	}
};

//CPropertyVariableListItem - реализует item, содержащий выпадающий список переменных проекта
class CPropertyVariableListItem 
	: public CPropertyCustomListItem
{
public:
	CPropertyVariableListItem(LPCTSTR pstrName, LPARAM lParam)
		: CPropertyCustomListItem(pstrName, lParam)
	{
		m_UpdateListOnDropDown = true;
		FillList();
	};

	void FillList()
	{
		CVariableMenagerPtr vm;
		m_List = vm->GetVariableList();
	}

	BOOL GetValue(VARIANT* pVal) const
	{
		wchar_t buffer[0xff];
		GetDisplayValue(buffer, 0xff);
		_bstr_t str(buffer);
		pVal->vt = VT_BSTR;
		pVal->bstrVal = str;
		return S_OK;//SUCCEEDED( CComVariant(m_val).Detach(pVal) );
	}
};

//CPropertyAlignListItem - item, реализующий выпадающий список с опциями выравнивания текста

class CPropertyAlignListItem 
	: public CPropertyCustomListItem
{
public:
	CPropertyAlignListItem(LPCTSTR pstrName, LPARAM lParam)
		: CPropertyCustomListItem(pstrName, lParam)
	{
		FillList();
	};

	void FillList()
	{
		m_List.push_back(L"By left");
		m_List.push_back(L"By right");
		m_List.push_back(L"By center");
	}

	BOOL GetValue(VARIANT* pVal) const
	{
		wchar_t buffer[0xff];
		GetDisplayValue(buffer, 0xff);
		_bstr_t str(buffer);
		pVal->vt = VT_BSTR;
		pVal->bstrVal = str;
		return S_OK;//SUCCEEDED( CComVariant(m_val).Detach(pVal) );
	}
};

inline HPROPERTY PropCreateColor(LPCTSTR pstrName, COLORREF bValue, LPARAM lParam = 0)
{
	CComVariant vValue = (UINT) bValue;
	CPropertyColorItem* prop = NULL;
	ATLTRY( prop = new CPropertyColorItem(pstrName, lParam) );
	ATLASSERT(prop);
	if( prop ) prop->SetValue(vValue);
	return prop;
}

inline HPROPERTY PropCreateFontList(LPCTSTR pstrName, std::wstring Value, LPARAM lParam = 0)
{
	CComVariant vValue = Value.c_str();
	
	CPropertyFontListItem* prop = NULL;
	ATLTRY( prop = new CPropertyFontListItem(pstrName, lParam) );
	ATLASSERT(prop);
	if( prop ) prop->SetValue(vValue);
	return prop;
}

inline HPROPERTY PropCreateImageList(LPCTSTR pstrName, std::wstring Value, LPARAM lParam = 0)
{
	CComVariant vValue = Value.c_str();

	CPropertyImageListItem* prop = NULL;
	ATLTRY( prop = new CPropertyImageListItem(pstrName, lParam) );
	ATLASSERT(prop);
	if( prop ) prop->SetValue(vValue);
	return prop;
}

inline HPROPERTY PropCreateSoundList(LPCTSTR pstrName, std::wstring Value, LPARAM lParam = 0)
{
	CComVariant vValue = Value.c_str();

	CPropertySoundListItem* prop = NULL;
	ATLTRY( prop = new CPropertySoundListItem(pstrName, lParam) );
	ATLASSERT(prop);
	if( prop ) prop->SetValue(vValue);
	return prop;
}

inline HPROPERTY PropCreatePolylineList(LPCTSTR pstrName, std::wstring Value, LPARAM lParam = 0)
{
	CComVariant vValue = Value.c_str();

	CPropertyPolylineListItem* prop = NULL;
	ATLTRY( prop = new CPropertyPolylineListItem(pstrName, lParam) );
	ATLASSERT(prop);
	if( prop ) prop->SetValue(vValue);
	return prop;
}

inline HPROPERTY PropCreateSideList(LPCTSTR pstrName, std::wstring Value, LPARAM lParam = 0)
{
	CComVariant vValue = Value.c_str();

	CPropertySideListItem* prop = NULL;
	ATLTRY( prop = new CPropertySideListItem(pstrName, lParam) );
	ATLASSERT(prop);
	if( prop ) prop->SetValue(vValue);
	return prop;
}

inline HPROPERTY PropCreateAlignList(LPCTSTR pstrName, std::wstring Value, LPARAM lParam = 0)
{
	CComVariant vValue = Value.c_str();

	CPropertyAlignListItem* prop = NULL;
	ATLTRY( prop = new CPropertyAlignListItem(pstrName, lParam) );
	ATLASSERT(prop);
	if( prop ) prop->SetValue(vValue);
	return prop;
}

inline HPROPERTY PropCreateVariableList(LPCTSTR pstrName, std::wstring Value, LPARAM lParam = 0)
{
	CComVariant vValue = Value.c_str();

	CPropertyVariableListItem* prop = NULL;
	ATLTRY( prop = new CPropertyVariableListItem(pstrName, lParam) );
	ATLASSERT(prop);
	if( prop ) prop->SetValue(vValue);
	return prop;
}


inline HPROPERTY PropCreateVariantEx(LPCTSTR pstrName, const VARIANT& vValue, LPARAM lParam = 0)
{
	CPropertyEditItemEx* prop = NULL;
	ATLTRY( prop = new CPropertyEditItemEx(pstrName, lParam) );
	ATLASSERT(prop);
	if( prop ) prop->SetValue(vValue);
	return prop;
}