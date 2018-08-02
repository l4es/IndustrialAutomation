#pragma once
#include <atlmisc.h>
#include <atlcrack.h>
#include "ComboTreeButton.h"
#include "ComboTreeEdit.h"
#include "ComboTreeDropDown.h"

#define IDC_COMBOTREE_BUTTON		4317         
#define IDC_COMBOTREE_EDIT			4318 
#define IDC_COMBOTREE_DROPDOWN		4319         

class CComboTreeCtrl
	:public CWindowImpl<CComboTreeCtrl>
{
	typedef CComboTreeCtrl thisClass;
	typedef CWindowImpl<CComboTreeCtrl> baseClass;

public:
	DECLARE_WND_CLASS(NULL);

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_DRAWITEM(OnDrawItem)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SETFOCUS(OnSetFocus)
		MSG_WM_KILLFOCUS(OnKillFocus)
		MSG_WM_SIZE(OnSize)
		MSG_WM_MOVE(OnMove)
		MSG_WM_TIMER(OnTimer)
		COMMAND_HANDLER_EX(IDC_COMBOTREE_BUTTON, BN_CLICKED, OnCombotreeButtonBnClicked)
		MESSAGE_RANGE_HANDLER(WM_KEYFIRST, WM_KEYLAST, OnKey)
	END_MSG_MAP()
	
	void SubclassDlgItemID(UINT nID, HWND hParent);
	LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
	int m_ComboWidth;
	int m_ComboHeight;
	BOOL m_IsDropped;

	CComboTreeButton m_DropDownButton;
	CComboTreeEdit m_Edit;
	CComboTreeDropDown m_Tree;

	BOOL Create(RECT& rect, int DroppedWidth, int DroppedHeight, HWND hParent, UINT nID );
	BOOL CreateButton();
	BOOL CreateEdit();
	BOOL CreateTree();
	BOOL GetDroppedState(){return m_IsDropped;};
	void SetDroppedState(BOOL State){m_IsDropped = State;};
	BOOL OnKeyDropdownActive(UINT message, UINT nChar);
	BOOL OnKeyDropdownInactive(UINT message, UINT nChar);
public:
	LRESULT OnDrawItem(UINT ControlID, LPDRAWITEMSTRUCT lpDrawItem);
	LRESULT OnPaint(HDC dc);
	LRESULT OnDestroy(void);
	LRESULT OnKey(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void OnSelection();
	void OnCancel();
	void HideTree();
	void ShowTree();
	void DisplayTree();
	LRESULT SendParentComboMessage(UINT Action);
	void CalculateDroppedRect(LPRECT lpDroppedRect);
	LRESULT OnCombotreeButtonBnClicked(UINT wNotifyCode, int wID, HWND hWndCtl);
	LRESULT OnSetFocus(HWND hwndOldFocus);
	LRESULT OnKillFocus(HWND hwndNewFocus);
	void SetValue(CString Val);
	CString GetValue(void);
	void RefreshTreePos();

	inline CTreeViewCtrl& GetTree(){return m_Tree;};
	LRESULT OnSize(UINT state, CSize Size);
	LRESULT OnMove(CPoint Pt);
	LRESULT OnTimer(UINT id, TIMERPROC lpTimerProc);
};
