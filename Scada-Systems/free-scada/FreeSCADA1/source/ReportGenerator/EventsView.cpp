#include "StdAfx.h"
#include ".\eventsview.h"
#include "Document.h"

void CEventsView::OnInitialUpdate()
{
	m_Filter = 0;

	DlgResize_Init(false, true/*, NULL*/);
	DoDataExchange(DDX_LOAD);

	UpdateControls();

	CListViewCtrl lv = GetDlgItem(IDC_EVENTS);
	lv.InsertColumn(0, _T("Time"),		LVCFMT_LEFT, -1, 0);
	lv.InsertColumn(1, _T("Channel"),		LVCFMT_LEFT, -1, 0);
	lv.InsertColumn(2, _T("Event"),	LVCFMT_LEFT, -1, 0);
	
	lv.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

BOOL CEventsView::PreTranslateMessage(MSG* pMsg)
{
	return IsDialogMessage(pMsg);
}

void CEventsView::OnUpdate()
{
	CDocument* pDoc = (CDocument*)GetDocument();

	if(pDoc->m_ActiveView != CDocument::AV_EVENTS)
		return;

	CWaitCursor wait;

	DoDataExchange(DDX_SAVE);
	UpdateControls();

	vector<DWORD> IDs;
	for(CTagDB::TVChannel::iterator j=pDoc->m_GraphChannels.begin();j!=pDoc->m_GraphChannels.end();j++)
		IDs.push_back(j->id);

	CTagDB::TVChannelValues data;
	CComboBox combo = GetDlgItem(IDC_EVENT_TYPES);
	if(m_Filter == 1)
	{
		int item = combo.GetCurSel();
		if(item>=0)
		{
			const DWORD Filter = combo.GetItemData(item);
			data = pDoc->m_DB.GetChannelEvents(IDs, pDoc->m_FromDateTime, pDoc->m_ToDateTime, Filter);
		}
	}
	else
		data = pDoc->m_DB.GetChannelEvents(IDs, pDoc->m_FromDateTime, pDoc->m_ToDateTime);

	CListViewCtrl lv = GetDlgItem(IDC_EVENTS);

	lv.DeleteAllItems();
	typedef CTagDB::TVChannelValues::iterator it;
	for(it i=data.begin();i!=data.end();i++)
	{
		CString tmp;
		tmp = i->DateTime.Format(L"%d.%m.%Y %H:%M:%S");

		int item = lv.InsertItem((int)(i-data.begin()), tmp);
		
		lv.SetItemText(item, 1, i->Name);

		tmp = i->Value;
		lv.SetItemText(item, 2, tmp);
	}

	for(int i=0;i<3;i++)
		lv.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
}

void CEventsView::UpdateControls()
{
	if(m_Filter == 1)
		LoadCombo();
	CWindow(GetDlgItem(IDC_EVENT_TYPES)).EnableWindow(m_Filter == 1);
}

LRESULT CEventsView::OnRadioClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_SAVE);
	UpdateControls();
	OnUpdate();
	return 0;
}

void CEventsView::LoadCombo()
{
	CComboBox combo = GetDlgItem(IDC_EVENT_TYPES);
	bool HasID = false;
	DWORD LastID=0;

	if(combo.GetCurSel()>=0)
	{
		HasID = true;
		LastID = combo.GetItemData(combo.GetCurSel());
	}
	combo.ResetContent();

	CDocument* pDoc = (CDocument*)GetDocument();
	vector<DWORD> IDs;
	for(CTagDB::TVChannel::iterator j=pDoc->m_GraphChannels.begin();j!=pDoc->m_GraphChannels.end();j++)
		IDs.push_back(j->id);

	CTagDB::TVChannel types = pDoc->m_DB.GetEventTypes(IDs, pDoc->m_FromDateTime, pDoc->m_ToDateTime);

	typedef CTagDB::TVChannel::iterator it;
	for(it i=types.begin();i!=types.end();i++)
	{
		int item = combo.AddString(i->Name);
		combo.SetItemData(item, i->id);

		if(HasID && LastID == i->id)
		{
			combo.SetCurSel(item);
		}
	}
	
	if(combo.GetCount() > 0 && HasID != true)
		combo.SetCurSel(0);
}

LRESULT CEventsView::OnComboChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	OnUpdate();
	return 0;
}
LRESULT CEventsView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;

	CListViewCtrl lv = GetDlgItem(IDC_EVENTS);
	for(int i=0;i<3;i++)
		lv.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);

	return 0;
}


CTagDB::TVChannelValues CEventsView::GetEvents()
{
	CWaitCursor wait;
	DoDataExchange(DDX_SAVE);

	CDocument* pDoc = (CDocument*)GetDocument();

	vector<DWORD> IDs;
	for(CTagDB::TVChannel::iterator j=pDoc->m_GraphChannels.begin();j!=pDoc->m_GraphChannels.end();j++)
		IDs.push_back(j->id);

	CComboBox combo = GetDlgItem(IDC_EVENT_TYPES);
	if(m_Filter == 1)
	{
		int item = combo.GetCurSel();
		if(item>=0)
		{
			const DWORD Filter = combo.GetItemData(item);
			return pDoc->m_DB.GetChannelEvents(IDs, pDoc->m_FromDateTime, pDoc->m_ToDateTime, Filter);
		}
	}
	else
		return pDoc->m_DB.GetChannelEvents(IDs, pDoc->m_FromDateTime, pDoc->m_ToDateTime);

	return CTagDB::TVChannelValues(); //Возвращаем пустой список
}