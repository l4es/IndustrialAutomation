#include "StdAfx.h"
#include "TagDB.h"
#include "Document.h"
#include ".\tableview.h"

LRESULT CTableView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_Printer.OpenDefaultPrinter();
	
	bHandled = FALSE;
	return 0;
};

void CTableView::OnUpdate()
{
	CWaitCursor wait;

	CDocument* pDoc = (CDocument*)GetDocument();

	if(pDoc->m_ActiveView != CDocument::AV_TABLE)
		return;

	DeleteAllItems();
	while(DeleteColumn(0)){};
	InsertColumn(0, _T("Time"),		LVCFMT_LEFT, -1, 0);
	InsertColumn(1, _T("Channel"),		LVCFMT_LEFT, -1, 0);
	InsertColumn(2, _T("Data"),	LVCFMT_LEFT, -1, 0);

	vector<DWORD> IDs;
	for(CTagDB::TVChannel::iterator j=pDoc->m_GraphChannels.begin();j!=pDoc->m_GraphChannels.end();j++)
		IDs.push_back(j->id);

	CTagDB::TVChannelValues values = pDoc->m_DB.GetValues(IDs, pDoc->m_FromDateTime, pDoc->m_ToDateTime);
	typedef CTagDB::TVChannelValues::iterator it;

	for(it i=values.begin();i!=values.end();i++)
	{
		const int item = InsertItem(i-values.begin(), L"");
		
		CString str;
		str = i->DateTime.Format(_T("%d.%m.%Y %H:%M:%S"));
		SetItemText(item, 0, str);

		SetItemText(item, 1, i->Name);

		i->Value.ChangeType(VT_BSTR);
		str = i->Value.bstrVal;
		SetItemText(item, 2, str);
	}

	for(ULONG i=0;i<3;i++)
		SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
}








