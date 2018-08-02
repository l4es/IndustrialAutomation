#include "StdAfx.h"
#include ".\logindlg.h"

LRESULT CLoginDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	DoDataExchange(DDX_LOAD);
	return TRUE;
}

LRESULT CLoginDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(wID == IDOK)
		DoDataExchange(DDX_SAVE);
	EndDialog(wID);
	return 0;
}

BOOL CLoginDlg::IsValid()
{
	HRESULT hr;
	COSERVERINFO	si;
	MULTI_QI		qi;
	ZeroMemory(&si,sizeof(COSERVERINFO));
	ZeroMemory(&qi,sizeof(MULTI_QI));

	si.pwszName = CComBSTR(m_Server);
	qi.pIID	= &__uuidof(IUnknown);
	hr = CoCreateInstanceEx( __uuidof(DBConnection), NULL, CLSCTX_ALL, &si, 1, &qi);

	if(FAILED(hr) || FAILED(qi.hr))
		return FALSE;
	IDBConnectionPtr Connection;
	hr = qi.pItf->QueryInterface(__uuidof(IDBConnection), (void**)&Connection);
	qi.pItf->Release();
	if(FAILED(hr))
		return FALSE;

	hr = Connection->raw_Open((BSTR)CComBSTR(m_User), (BSTR)CComBSTR(m_Password));
	if(hr == S_OK)
		return TRUE;
	Connection = NULL;
	return FALSE;
}

IDBConnectionPtr CLoginDlg::GetConnection()
{
	HRESULT hr;
	COSERVERINFO	si;
	MULTI_QI		qi;
	ZeroMemory(&si,sizeof(COSERVERINFO));
	ZeroMemory(&qi,sizeof(MULTI_QI));

	si.pwszName = CComBSTR(m_Server);
	qi.pIID	= &__uuidof(IUnknown);
	hr = CoCreateInstanceEx( __uuidof(DBConnection), NULL, CLSCTX_ALL, &si, 1, &qi);

	if(FAILED(hr) || FAILED(qi.hr))
		return NULL;

	IDBConnectionPtr Connection;
	hr = qi.pItf->QueryInterface(__uuidof(IDBConnection), (void**)&Connection);
	qi.pItf->Release();
	if(FAILED(hr))
		return NULL;

	hr = Connection->raw_Open((BSTR)CComBSTR(m_User), (BSTR)CComBSTR(m_Password));
	if(hr == S_OK)
		return Connection;

	Connection = NULL;
	return NULL;
}
LRESULT CLoginDlg::OnButtonBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CString server = BrowseComputer(m_hWnd);
	if(server != m_Server && server != "")
	{
		m_Server = server;
		DoDataExchange(DDX_LOAD, IDC_SERVER);
	}
	else
		DoDataExchange(DDX_SAVE, IDC_SERVER);
	return 0;
}
