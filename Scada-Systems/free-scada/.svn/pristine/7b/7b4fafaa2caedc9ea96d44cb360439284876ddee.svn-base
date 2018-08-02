#include "StdAfx.h"
#include ".\exporttoexceldlg.h"
#include "ExportThreadDlg.h"

LRESULT CExportToExcelDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DlgResize_Init(true, true, NULL);
	DoDataExchange(DDX_LOAD);

	CenterWindow(GetParent());

	DWORD Style;
	m_FromDate	= GetDlgItem(IDC_FROM_DATE);
	m_FromTime	= GetDlgItem(IDC_FROM_TIME);
	m_ToDate	= GetDlgItem(IDC_TO_DATE);
	m_ToTime	= GetDlgItem(IDC_TO_TIME);
	m_List		= GetDlgItem(IDC_CHANNEL_LIST);

	Style = m_FromTime.GetWindowLong(GWL_STYLE);
	m_FromTime.SetWindowLong(GWL_STYLE, Style|DTS_TIMEFORMAT);

	Style = m_ToTime.GetWindowLong(GWL_STYLE);
	m_ToTime.SetWindowLong(GWL_STYLE, Style|DTS_TIMEFORMAT);

	m_List.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | /*LVS_EX_GRIDLINES |*/ LVS_EX_CHECKBOXES);
	m_List.InsertColumn(0, L"Channel", LVCFMT_LEFT, -1, 0);
	
	GetDlgItem(IDOK).EnableWindow(FALSE);

	FillChannelList();

	CString s;
	s = COleDateTime::GetCurrentTime().Format(L"%Y-%m-%d");
	TCHAR MyDocs[MAX_PATH];
	if(SHGetSpecialFolderPath(m_hWnd, MyDocs, CSIDL_PERSONAL, FALSE))
	{
		s = CString(MyDocs)+CString('\\')+s;
	}
	else
	{
		GetCurrentDirectory(MAX_PATH, MyDocs);
		s = CString(MyDocs)+CString('\\')+s;
	}
	GetDlgItem(IDC_FILENAME).SetWindowText(s);
	return TRUE;
}

LRESULT CExportToExcelDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_SAVE);
	if(wID == IDOK)
	{
		if(IDYES != ::MessageBox(NULL,L"Data export could take a long time.\nContinue?",L"Export",MB_YESNO|MB_DEFBUTTON1|MB_ICONINFORMATION))
		{
			return 0;
		}
		//»щем выделенный канал
		ATLASSERT(GetCurSel() != -1);

		COleDateTime From,To;
		SYSTEMTIME st_date,st_time;
		m_FromDate.GetSystemTime(&st_date);
		m_FromTime.GetSystemTime(&st_time);
		From.SetDateTime(	st_date.wYear, st_date.wMonth, st_date.wDay,
							st_time.wHour, st_time.wMinute, st_time.wSecond);
		m_ToDate.GetSystemTime(&st_date);
		m_ToTime.GetSystemTime(&st_time);
		To.SetDateTime(	st_date.wYear, st_date.wMonth, st_date.wDay,
						st_time.wHour, st_time.wMinute, st_time.wSecond);

		TCHAR FileName[MAX_PATH];
		GetDlgItemText(IDC_FILENAME, FileName, MAX_PATH);
		_tcscat(FileName,_T(".xls"));
		DoExport(m_List.GetItemData(GetCurSel()), From, To, FileName);
	}
	EndDialog(wID);
	return 0;
}
void CExportToExcelDlg::FillChannelList(void)
{
	m_List.DeleteAllItems();

//	CTagDB::TVChannel vec = _db->GetChannelsList();
	CTagDB::TVChannel vec = _db->GetHumanReadableChannelsList();

	for(CTagDB::TVChannel::iterator i=vec.begin();i!=vec.end();i++)
	{
		int item = m_List.InsertItem(i-vec.begin(),i->Name);
		m_List.SetItemData(item, i->id);
	}

	m_List.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
}

LRESULT CExportToExcelDlg::OnLVSelChanging(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	BOOL AlreadySelected=(GetCurSel() != -1);

	//—мотрим стоит ли галочка
	LPNMLISTVIEW lpNLV = (LPNMLISTVIEW)pnmh;
	long ns, os;
	ns = (lpNLV->uNewState & LVIS_STATEIMAGEMASK) >> 12; // new state
	os = (lpNLV->uOldState & LVIS_STATEIMAGEMASK) >> 12; // old state
	if (os < ns)
		return AlreadySelected;
	else
		return FALSE;
}

LRESULT CExportToExcelDlg::OnLVSelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
	GetDlgItem(IDOK).EnableWindow(GetCurSel() != -1);
	return 0;
}

int CExportToExcelDlg::GetCurSel()
{
	int count = m_List.GetItemCount();
	for(int i=0;i<count;i++)
	{
		long state = m_List.GetItemState(i, LVIS_STATEIMAGEMASK) >> 12;
		if(state == 2)
		{
			return i;
		}
	}
	return -1;
}

void CExportToExcelDlg::DoExport(UINT TagID, COleDateTime From, COleDateTime To, wstring FileName)
{
	CWaitCursor wait;

	CTagDB::TVChannelData data = _db->GetData(TagID, From, To);
	
	int item = GetCurSel();
	CString Channel;
	ATLASSERT(item != -1);
	m_List.GetItemText(item,0,Channel);
	
	CExportTaskMenager* p = CExportTaskMenager::Instance();
	p->CreateNewTask(data, FileName, GetParent(),wstring("Channel: "+Channel));
	p->Release();
}