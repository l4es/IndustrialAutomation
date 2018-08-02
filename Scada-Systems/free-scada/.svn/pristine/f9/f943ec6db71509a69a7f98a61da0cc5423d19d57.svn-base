#pragma once
#include <atlcrack.h>
#include "resource.h"


namespace ModuleArchive
{
	class CModuleArchive;

	class CEventsView
		: public CDialogImpl<CEventsView>
		, public CDialogResize<CEventsView>
		, public CWinDataExchange<CEventsView>
	{
	public:
		enum { IDD = IDD_EVENTVIEW };

		CModuleArchive* m_Module; //”казатель на модуль
		CString m_Statics[7];
		CString m_Events[7];

		CEventsView(void);
		~CEventsView(void);

		BEGIN_DDX_MAP(COPCView)
			DDX_TEXT(IDC_STATIC1, m_Statics[0])
			DDX_TEXT(IDC_STATIC2, m_Statics[1])
			DDX_TEXT(IDC_STATIC3, m_Statics[2])
			DDX_TEXT(IDC_STATIC4, m_Statics[3])
			DDX_TEXT(IDC_STATIC5, m_Statics[4])
			DDX_TEXT(IDC_STATIC6, m_Statics[5])
			DDX_TEXT(IDC_STATIC7, m_Statics[6])
			
			DDX_TEXT(IDC_EDIT1, m_Events[0])
			DDX_TEXT(IDC_EDIT2, m_Events[1])
			DDX_TEXT(IDC_EDIT3, m_Events[2])
			DDX_TEXT(IDC_EDIT4, m_Events[3])
			DDX_TEXT(IDC_EDIT5, m_Events[4])
			DDX_TEXT(IDC_EDIT6, m_Events[5])
			DDX_TEXT(IDC_EDIT7, m_Events[6])
		END_DDX_MAP()

		BEGIN_MSG_MAP(COPCView)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			MSG_WM_SHOWWINDOW(OnShowWindow)
			MSG_WM_DESTROY(OnDestroy)
		END_MSG_MAP()

		BEGIN_DLGRESIZE_MAP(COPCView)
		END_DLGRESIZE_MAP()

		BOOL PreTranslateMessage(MSG* pMsg);
		void LoadFromDB();
		void SaveToDB(BOOL Silent=FALSE);
		void Update();
		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		LRESULT OnShowWindow(BOOL fShow, UINT status);
		LRESULT OnDestroy(void);
	};
}