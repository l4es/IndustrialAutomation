#include "StdAfx.h"
#include ".\selectchanneldialog.h"
#include "resource.h"

LRESULT CSelectChannelDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DlgResize_Init(true, true, NULL);
	DoDataExchange(DDX_LOAD);
	CenterWindow(GetParent());
	m_Tree = (CTreeViewCtrl)GetDlgItem(IDC_TREE_CHANNEL);

	CImageList ImageList;
	ImageList.CreateFromImage(IDB_ICONS, 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
	m_Tree.SetImageList(ImageList, TVSIL_NORMAL);
	ImageList.Detach();


	GetDlgItem(IDOK).EnableWindow(!m_Channel.IsEmpty());

	HRESULT	hr;
	hr = PingRemoteServer(m_Computer, m_OPCServerCLSID, 2000 /*мс*/);
	if(hr != S_OK)
		return 0;

	CComPtr<IOPCBrowseServerAddressSpace> sas;
	COSERVERINFO		si;
	MULTI_QI			qi;
	ZeroMemory(&si, sizeof(COSERVERINFO));
	ZeroMemory(&qi, sizeof(MULTI_QI));
	si.pwszName = CComBSTR(m_Computer);
	qi.pIID		= &__uuidof(IOPCBrowseServerAddressSpace);
	hr = CoCreateInstanceEx(m_OPCServerCLSID, NULL, CLSCTX_ALL, &si, 1, &qi);
	if(SUCCEEDED(hr) && SUCCEEDED(qi.hr))
	{
		hr = qi.pItf->QueryInterface(__uuidof(IOPCBrowseServerAddressSpace), (void**)&sas);
		qi.pItf->Release();
	}
	else
		return FALSE;

	OPCNAMESPACETYPE ns_type = OPC_NS_HIERARCHIAL;
	hr = sas->QueryOrganization(&ns_type);
	if(ns_type == OPC_NS_HIERARCHIAL)
	{	
		while(SUCCEEDED(sas->ChangeBrowsePosition(OPC_BROWSE_UP, L""))){};
		FillTree(m_Tree.GetNextItem(TVI_ROOT, TVGN_CARET),	sas);
	}
	else if(ns_type == OPC_NS_FLAT)
	{
		FillTreeFlat(m_Tree.GetNextItem(TVI_ROOT, TVGN_CARET),	sas);
	}
	return TRUE;
}


void CSelectChannelDialog::FillTreeFlat(HTREEITEM root, CComPtr<IOPCBrowseServerAddressSpace> sas)
{
	CComPtr<IEnumString> es;
	HRESULT hr;
	hr = sas->BrowseOPCItemIDs(OPC_FLAT, L"", VT_EMPTY, 0, &es);
	if(FAILED(hr))
		return;

	DWORD fetched;
	do 
	{
		LPOLESTR tmp;
		hr = es->Next(1, &tmp, &fetched);
		if(fetched>0)
		{
			HTREEITEM item;
			item = m_Tree.InsertItem((CString)tmp,34,34,root,root);
			m_Tree.SetItemData(item, OPC_LEAF);
			CoTaskMemFree(tmp);
		}
	} while(fetched>0);
}

void CSelectChannelDialog::FillTree(HTREEITEM root, CComPtr<IOPCBrowseServerAddressSpace> sas)
{
	CComPtr<IEnumString> es;
	HRESULT hr;
	hr = sas->BrowseOPCItemIDs(OPC_BRANCH, L"", VT_EMPTY, 0/*OPC_READABLE|OPC_WRITEABLE*/, &es);
	if(FAILED(hr))
		return;

	DWORD fetched;
	do 
	{
		LPOLESTR tmp;
		hr = es->Next(1, &tmp, &fetched);
		if(fetched>0)
		{
			HTREEITEM item;
			item = m_Tree.InsertItem((CString)tmp,21,21,root,root);
			m_Tree.SetItemData(item, OPC_BRANCH);
			hr = sas->ChangeBrowsePosition(OPC_BROWSE_DOWN, tmp);
			if(FAILED(hr)) 
				return;
			FillTree(item, sas);
			hr = sas->ChangeBrowsePosition(OPC_BROWSE_UP, L"");
			if(FAILED(hr)) 
				return;
			CoTaskMemFree(tmp);
		}
	} while(fetched>0);

	es.Release();
	hr = sas->BrowseOPCItemIDs(OPC_LEAF, L"", VT_EMPTY, 0/*OPC_READABLE|OPC_WRITEABLE*/, &es);
	if(FAILED(hr))
		return;
	do 
	{
		LPOLESTR tmp;
		hr = es->Next(1, &tmp, &fetched);
		if(fetched>0)
		{
			HTREEITEM item;
			item = m_Tree.InsertItem((CString)tmp,34,34,root,root);
			m_Tree.SetItemData(item, OPC_LEAF);
			CoTaskMemFree(tmp);
		}
	} while(fetched>0);
}

LRESULT CSelectChannelDialog::OnTvnSelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
	HTREEITEM Item = m_Tree.GetSelectedItem();
	tagOPCBROWSETYPE Type = 
		(tagOPCBROWSETYPE)m_Tree.GetItemData(Item);
	if(Type == OPC_BRANCH)
	{
		GetDlgItem(IDOK).EnableWindow(FALSE);
		return TRUE;
	}
	
	m_Channel.Empty();
	HTREEITEM tmp = Item;
	while(tmp != NULL)
	{
		CString chnl;
		m_Tree.GetItemText(tmp, chnl);
		m_Channel = chnl + "." + m_Channel;
		tmp = m_Tree.GetParentItem(tmp);
	}
	m_Channel.TrimRight(L".");

	GetDlgItem(IDOK).EnableWindow(!m_Channel.IsEmpty());

	return TRUE;
}


LRESULT CSelectChannelDialog::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return TRUE;
}

LRESULT CSelectChannelDialog::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_SAVE);
	EndDialog(wID);
	return 0;
}


static DWORD __stdcall AtlPingRemoteServerProc(void* pv)
{
	sServerInfo* ServerInfo = (sServerInfo*)pv;
	CComBSTR Computer = ServerInfo->m_Computer;
	CLSID ServerCLSID = ServerInfo->m_ServerCLSID;
	HRESULT hr;
	// Initialize our COM apartment
//	hr = ::CoInitializeEx(0,COINIT_MULTITHREADED);
	hr = ::CoInitialize(NULL);
	if (FAILED(hr)) 
		return hr;

	COSERVERINFO		si;
	MULTI_QI			qi;
	ZeroMemory(&si, sizeof(COSERVERINFO));
	ZeroMemory(&qi, sizeof(MULTI_QI));
	si.pwszName = Computer;
	qi.pIID		= &__uuidof(IUnknown);
	hr = CoCreateInstanceEx(ServerCLSID, NULL, CLSCTX_ALL, &si, 1, &qi);

	if(hr == S_OK)
	{
		CComPtr<IOPCCommon> sas;
		hr = qi.pItf->QueryInterface(__uuidof(IOPCCommon), (void**)&sas);
		qi.pItf->Release();
	}
	
	// Tear down our apartment, and terminate this bkgrnd thread
	::CoUninitialize();
	return hr;
}

//Попытка создать объект на удаленном компьютере
HRESULT CSelectChannelDialog::PingRemoteServer(CString Computer, CLSID ServerOPOC, DWORD nTimeout)
{
	HRESULT hr;

	sServerInfo si;
	CComBSTR bstr(Computer);
	si.m_Computer = bstr;
	si.m_ServerCLSID = ServerOPOC;
	HANDLE hThread = ::CreateThread(0,0,AtlPingRemoteServerProc,(void*)(&si),0,0);

	if (::WaitForSingleObject(hThread,nTimeout) == WAIT_TIMEOUT)
	{
		TerminateThread(hThread,0);
		hr = S_FALSE;
	}
	else
		::GetExitCodeThread(hThread,(DWORD*)&hr);

	return hr;
}



