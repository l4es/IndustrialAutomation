#pragma once
#include <atlprint.h>
#include "resource.h"







typedef CWinTraitsOR<LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS> CTableTraits;

class CTableView 
	: public DVF::CBaseView
	, public CWindowImpl<CTableView, CListViewCtrl, CTableTraits>
	
	
{
public:	
	struct sData
	{
		double x,y;
		struct fLessX
			: public binary_function<const sData&, const sData&, bool>
		{
			bool operator()(const sData& _Left, const sData& _Right) const
			{
				return (_Left.x < _Right.x);
			}
		};
	};

	DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

	BEGIN_MSG_MAP(CTableView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	//	COMMAND_ID_HANDLER(ID_PRINT_PREVIEW, OnPrintPreview)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);
	void OnUpdate();
	
	//void PrintPreview();

	CPrintPreviewWindow m_wndPreview;
	CPrinter			m_Printer;
	
};
