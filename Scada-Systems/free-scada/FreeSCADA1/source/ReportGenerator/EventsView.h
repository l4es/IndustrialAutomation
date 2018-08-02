#pragma once

#include "TagDB.h"

class CEventsView
	: public CDialogImpl<CEventsView>
	, public CDialogResize<CEventsView>
	, public CWinDataExchange<CEventsView>
	, public DVF::CBaseView
{
	typedef CEventsView thisClass;

public:
	enum { IDD = IDD_EVENTS_VIEW };

	int m_Filter;

	BEGIN_DDX_MAP(thisClass)
		DDX_RADIO(IDC_ALL_EVENTS, m_Filter)
	END_DDX_MAP()

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		COMMAND_HANDLER(IDC_ALL_EVENTS, BN_CLICKED, OnRadioClicked)
		COMMAND_HANDLER(IDC_FILTER_EVENTS, BN_CLICKED, OnRadioClicked)
		COMMAND_HANDLER(IDC_EVENT_TYPES, CBN_SELCHANGE, OnComboChange)

		CHAIN_MSG_MAP(CDialogResize<thisClass>)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_EVENT_TYPES, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_EVENTS, DLSZ_SIZE_X|DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BOOL PreTranslateMessage(MSG* pMsg);
	void OnInitialUpdate();
	void OnUpdate();

	void UpdateControls();
	void LoadCombo();
	CTagDB::TVChannelValues GetEvents();

	LRESULT OnRadioClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnComboChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
