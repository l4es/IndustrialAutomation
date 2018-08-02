#pragma once

// Include property base class
#include "PropertyList/PropertyItem.h"
#include "PropertyList/PropertyItemEditors.h"
#include "PropertyList/PropertyItemImpl.h"

#include <vector>
#include <wstring>
#include <algorithm>


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
	typedef std::vector<std::wstring> TStrings;
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
		  return m_List[m_val.lVal].size();
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






//CPropertySideListItem - реализует item с выпадающим списком, содержащим направления по которым можно изменять размеры
class CPropertyPortSpeedListItem 
	: public CPropertyCustomListItem
{
public:
	CPropertyPortSpeedListItem(LPCTSTR pstrName, LPARAM lParam)
		: CPropertyCustomListItem(pstrName, lParam)
	{
		FillList();
	};

	void FillList()
	{
		m_List.push_back("9600");
		m_List.push_back("14400");
		m_List.push_back("19200");
		m_List.push_back("38400");
		m_List.push_back("56000");
		m_List.push_back("57600");
		m_List.push_back("115200");

	}

	BOOL GetValue(VARIANT* pVal) const
	{
		char buffer[0xff];
		GetDisplayValue(buffer, 0xff);
		_bstr_t str(buffer);
		pVal->vt = VT_BSTR;
		pVal->bstrVal = str;
		return S_OK;//SUCCEEDED( CComVariant(m_val).Detach(pVal) );
	}
};


inline HPROPERTY PropCreatePortSpeedList(LPCTSTR pstrName, std::wstring Value, LPARAM lParam = 0)
{
	CComVariant vValue = Value.c_str();

	CPropertyPortSpeedListItem* prop = NULL;
	ATLTRY( prop = new CPropertyPortSpeedListItem(pstrName, lParam) );
	ATLASSERT(prop);
	if( prop ) prop->SetValue(vValue);
	return prop;
}






