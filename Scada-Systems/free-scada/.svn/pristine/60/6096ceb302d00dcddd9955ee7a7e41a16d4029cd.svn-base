#include "StdAfx.h"
#include ".\selectserverdialog.h"
#include "opc/OpcEnum.h"

CSelectServerDialog::CSelectServerDialog(void)
{
}

LRESULT CSelectServerDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DlgResize_Init(true, true, NULL);
	DoDataExchange(DDX_LOAD);
	CenterWindow(GetParent());

	m_ListBox = (CListBox)GetDlgItem(IDC_LIST_SERVER);
	FillListServer();

	return TRUE;
}


LRESULT CSelectServerDialog::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return TRUE;
}

LRESULT CSelectServerDialog::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_SAVE);
	EndDialog(wID);
	return 0;
}

void CSelectServerDialog::FillListServer()
{
	CComPtr<IOPCServerList>	sl;
	COSERVERINFO		si;
	MULTI_QI			qi;
	HRESULT				hr;
	ZeroMemory(&si, sizeof(COSERVERINFO));
	ZeroMemory(&qi, sizeof(MULTI_QI));
	si.pwszName = CComBSTR(m_Computer);
	qi.pIID		= &__uuidof(IOPCServerList);
	hr = CoCreateInstanceEx(CLSID_OpcServerList, NULL, CLSCTX_ALL, &si, 1, &qi);
	if(SUCCEEDED(hr) && SUCCEEDED(qi.hr))
	{
		hr = qi.pItf->QueryInterface(__uuidof(IOPCServerList), (void**)&sl);
		qi.pItf->Release();
	}
	else
		return;
//	wait.Restore();

	CATID req = CATID_OPCDAServer20;
	CComPtr<IEnumGUID> eguids;
	hr = sl->EnumClassesOfCategories(1, &req, 0, NULL, &eguids);
	DWORD fetched;
	do 
	{
		//wait.Restore();
		GUID id;
		hr = eguids->Next(1, &id, &fetched);
		if(SUCCEEDED(hr) && fetched>0)
		{
			LPOLESTR ProgID;
			LPOLESTR Name;
			hr = sl->GetClassDetails(id, &ProgID, &Name);

			CLSID *pid = new CLSID;
			memcpy(pid, &id, sizeof(CLSID));
			int item = m_ListBox.AddString((CString)Name);
			m_ListBox.SetItemDataPtr(item, pid);
			CoTaskMemFree(ProgID);
			CoTaskMemFree(Name);
		}
	} while(fetched>0);
//	wait.Restore();
	if(m_ListBox.GetCount()>0)
	{
		m_ListBox.SetCurSel(0);
//		OnLbnSelchangeServerList();
	}
}