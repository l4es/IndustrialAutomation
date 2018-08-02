#include "StdAfx.h"
#include ".\channelsview.h"
#include "ModuleArchive.h"
#include "ApplicationSettings.h"
#include "ErrorReporter.h"
#include "..\library\library.h"


using namespace Helpers;

#define LCID_RUSSIAN	MAKELCID(MAKELANGID(LANG_RUSSIAN, SUBLANG_NEUTRAL),SORT_DEFAULT)

extern int GetDigitalPos(wstring &str);
extern int GetDigit(int start, wstring &str);

//Сравнение пунктов (для сортировки)
int CALLBACK CompareChannelItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CTreeViewCtrl Tree((HWND)lParamSort);
	int Data1 = (int)Tree.GetItemData((HTREEITEM)lParam1);
	int Data2 = (int)Tree.GetItemData((HTREEITEM)lParam2);


	if((Data1 == OPC_BRANCH) && (Data2 == OPC_LEAF))
		return -1;

	if((Data1 == OPC_LEAF) && (Data2 == OPC_BRANCH)) 
		return 1;

	//Если пункты содержат цифры, сортируем с учетом их
	CString Name1,Name2;
	Tree.GetItemText((HTREEITEM)lParam1, Name1);
	Tree.GetItemText((HTREEITEM)lParam2, Name2);

	const int	pos1 = GetDigitalPos(wstring(Name1)),
		pos2 = GetDigitalPos(wstring(Name2));
	if(pos1>=0 && pos2 >=0 && pos1 == pos2)
	{
		const int	digit1 = GetDigit(pos1, wstring(Name1)),
			digit2 = GetDigit(pos2, wstring(Name2));
		if(digit1 > digit2)
			return 1;
		else
		{
			if(digit1 == digit2)
				return 0;
			else
				return -1;
		}
	}
	else
		return  lstrcmpi(Name1, Name2);
}

//Сравнение пунктов (для сортировки)
int CALLBACK CompareChannelItemsLV(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListViewCtrl List((HWND)lParamSort);

	//Если пункты содержат цифры, сортируем с учетом их
	CString Name1,Name2;
	List.GetItemText(lParam1, 0, Name1);
	List.GetItemText(lParam2, 0, Name2);

	const int	pos1 = GetDigitalPos(wstring(Name1)),
		pos2 = GetDigitalPos(wstring(Name2));
	if(pos1>=0 && pos2 >=0 && pos1 == pos2)
	{
		if(Name1.Mid(0,pos1) != Name2.Mid(0,pos2))
			return lstrcmpi(Name1, Name2);

		const int	digit1 = GetDigit(pos1, wstring(Name1)),
			digit2 = GetDigit(pos2, wstring(Name2));
		if(digit1 > digit2)
			return 1;
		else
		{
			if(digit1 == digit2)
				return 0;
			else
				return -1;
		}
	}
	else
		return  lstrcmpi(Name1, Name2);
}

namespace ModuleArchive
{
	BOOL CChannelsView::PreTranslateMessage(MSG* pMsg)
	{
		return IsDialogMessage(pMsg);
	}

	LRESULT CChannelsView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init(false, true, NULL);
		CListViewCtrl List = (CListViewCtrl)GetDlgItem(IDC_ARCHIVE);

		List.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

		List.InsertColumn(0, L"Channel",LVCFMT_CENTER, -1, -1);
		List.InsertColumn(1, L"Host",LVCFMT_CENTER, -1, -1);
		List.InsertColumn(2, L"OPC Server",LVCFMT_CENTER, -1, -1);

		for(int i = 0; i < 3; i++)
			List.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);

		m_Channels.SubclassDlgItemID(IDC_CHANNEL, m_hWnd);
		return 0;
	}

	LRESULT CChannelsView::OnBrowseComputer( WORD /*wNotifyCode*/,  WORD /*wID*/,  HWND /*hWndCtl*/,  BOOL& /*bHandled */)
	{
		CString Computer = BrowseComputer();

		if(!Computer.IsEmpty())
		{
			SetDlgItemText(IDC_ARCH_COMPUTER, (LPCTSTR)Computer);
			((CComboBox)GetDlgItem(IDC_ARCH_SERVER)).EnableWindow(TRUE);
		}

		return 0;
	}

	CString CChannelsView::BrowseComputer()
	{
		CString result;
		BROWSEINFO bi;
		LPITEMIDLIST	proot,pil;
		proot = SHCloneSpecialIDList(m_hWnd, CSIDL_NETWORK, FALSE);
		ZeroMemory(&bi, sizeof(BROWSEINFO));
		bi.hwndOwner	= m_hWnd;
		bi.ulFlags		= BIF_BROWSEFORCOMPUTER;
		bi.pidlRoot		= proot;
		pil = SHBrowseForFolder(&bi);
		if(pil)
		{
			IShellFolder *isf;
			SHGetDesktopFolder(&isf);
			STRRET cn;
			ZeroMemory(&cn, sizeof(STRRET));
			isf->GetDisplayNameOf(pil,SHGDN_FORPARSING, &cn);
			switch(cn.uType)
			{
			case STRRET_CSTR:
				result = cn.cStr;
				break;
			case STRRET_WSTR:
				result = cn.pOleStr;
				break;
			case STRRET_OFFSET:
				result = CString((byte*)pil + cn.uOffset);
				break;
			}
			isf->Release();
			ILFree(pil);
		}
		ILFree(proot);
		return result;
	}


	LRESULT CChannelsView::OnSelChangeServer( WORD /*wNotifyCode*/,  WORD /*wID*/,  HWND /*hWndCtl*/,  BOOL& /*bHandled */)
	{
		HRESULT hr = FillChannels();
		if(FAILED(hr))
		{
			CErrorReporterPtr err;
			err->DecodeHRESULT(hr);
		}
		return 0;
	}

	LRESULT CChannelsView::OnDropDownServer(WORD /*wNotifyCode*/,  WORD /*wID*/,  HWND /*hWndCtl*/,  BOOL& /*bHandled */)
	{
		FillServers();
		return 0;
	}

	void CChannelsView::FillServers()
	{
		m_Channels.GetTree().DeleteAllItems();
		CWaitCursor wait;

		CString Computer;
		GetDlgItemText(IDC_ARCH_COMPUTER, Computer);

		if(Computer.IsEmpty())
			return;

		CComboBox Servers = (CComboBox)GetDlgItem(IDC_ARCH_SERVER);
		Servers.ResetContent();

		HRESULT hresult = AtlPingRemoteHost(Computer, 3000 /*мс*/);
		if(FAILED(hresult))
			return;

		CComPtr<IOPCServerList>	sl;
		COSERVERINFO		si;
		MULTI_QI			qi;
		HRESULT				hr;
		ZeroMemory(&si, sizeof(COSERVERINFO));
		ZeroMemory(&qi, sizeof(MULTI_QI));
		si.pwszName = CComBSTR(Computer);
		qi.pIID		= &__uuidof(IOPCServerList);


		hr = CoCreateInstanceEx(CLSID_OpcServerList, NULL, CLSCTX_ALL, &si, 1, &qi);

		if(SUCCEEDED(hr) && SUCCEEDED(qi.hr))
		{
			hr = qi.pItf->QueryInterface(__uuidof(IOPCServerList), (void**)&sl);
			qi.pItf->Release();
		}
		else
			return;

		UUID req = CATID_OPCDAServer20;
		CComPtr<IEnumGUID> eguids;
		hr = sl->EnumClassesOfCategories(1, &req, 0, NULL, &eguids);
		DWORD fetched;
		if(!TEST_HR(hr))
			return;
		do 
		{
			GUID id;
			hr = eguids->Next(1, &id, &fetched);
			if(SUCCEEDED(hr) && fetched>0)
			{
				LPOLESTR ProgID;
				LPOLESTR Name;
				hr = sl->GetClassDetails(id, &ProgID, &Name);

				CLSID *pid = new CLSID;
				memcpy(pid, &id, sizeof(CLSID));
				int item = Servers.AddString((CString)Name);
				Servers.SetItemDataPtr(item, pid);
				CoTaskMemFree(ProgID);
				CoTaskMemFree(Name);
			}
		} while(fetched>0);
		if(Servers.GetCount()>0)
			Servers.SetCurSel(0);
	}

	HRESULT CChannelsView::FillChannels()
	{
		HRESULT	hr = S_FALSE;
		DoDataExchange(DDX_SAVE);

		CString Computer;
		GetDlgItemText(IDC_ARCH_COMPUTER, Computer);

		if(Computer.IsEmpty())
			return hr;

		CComboBox Server = (CComboBox)GetDlgItem(IDC_ARCH_SERVER);
		int i = Server.GetCurSel();

		if(i == -1) 
			return hr;

		GUID* ServerCLSID  = (GUID*)Server.GetItemDataPtr(i);
		if(*ServerCLSID == CLSID_NULL)
			return hr;

		CTreeViewCtrl Tree = m_Channels.GetTree();
		Tree.DeleteAllItems();

		CImageList ImageList;
		ImageList.CreateFromImage(IDB_ICONS, 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
		Tree.SetImageList(ImageList, TVSIL_NORMAL);
		ImageList.Detach();

		CComPtr<IOPCBrowseServerAddressSpace> sas;
		COSERVERINFO		si;
		MULTI_QI			qi;
		ZeroMemory(&si, sizeof(COSERVERINFO));
		ZeroMemory(&qi, sizeof(MULTI_QI));
		si.pwszName = CComBSTR(Computer);
		qi.pIID		= &__uuidof(IOPCBrowseServerAddressSpace);

		hr = CoCreateInstanceEx(*ServerCLSID, NULL, CLSCTX_ALL, &si, 1, &qi);
		if(SUCCEEDED(hr) && SUCCEEDED(qi.hr))
		{
			hr = qi.pItf->QueryInterface(__uuidof(IOPCBrowseServerAddressSpace), (void**)&sas);
			qi.pItf->Release();
		}
		else
			return hr;

		OPCNAMESPACETYPE ns_type = OPC_NS_HIERARCHIAL;
		hr = sas->QueryOrganization(&ns_type);
		if(ns_type == OPC_NS_HIERARCHIAL)
		{	
			while(SUCCEEDED(sas->ChangeBrowsePosition(OPC_BROWSE_UP, L""))){};
			FillTree(Tree.GetNextItem(TVI_ROOT, TVGN_CARET), sas);
		}
		else if(ns_type == OPC_NS_FLAT)
		{
			FillTreeFlat(Tree.GetNextItem(TVI_ROOT, TVGN_CARET), sas);
		}

		return hr;
	}

	void CChannelsView::FillTree(HTREEITEM root, CComPtr<IOPCBrowseServerAddressSpace> sas)
	{
		CTreeViewCtrl Tree = m_Channels.GetTree();
		CComPtr<IEnumString> es;
		HRESULT hr;
		hr = sas->BrowseOPCItemIDs(OPC_BRANCH, L"", VT_EMPTY, 0/*OPC_READABLE|OPC_WRITEABLE*/, &es);
		DWORD fetched;
		do 
		{
			LPOLESTR tmp;
			hr = es->Next(1, &tmp, &fetched);
			if(fetched>0)
			{
				HTREEITEM item;
				item = Tree.InsertItem((CString)tmp,1,1,root,root);
				Tree.SetItemData(item, OPC_BRANCH);
				sas->ChangeBrowsePosition(OPC_BROWSE_DOWN, tmp);
				FillTree(item, sas);
				Tree.Expand(item);
				sas->ChangeBrowsePosition(OPC_BROWSE_UP, L"");
				CoTaskMemFree(tmp);
			}
		} while(fetched>0);

		es.Release();
		hr = sas->BrowseOPCItemIDs(OPC_LEAF, L"", VT_EMPTY,0 , &es);/*OPC_READABLE*/
		do 
		{
			LPOLESTR tmp;
			hr = es->Next(1, &tmp, &fetched);
			if(fetched>0)
			{
				HTREEITEM item;
				item = Tree.InsertItem((CString)tmp,2,2,root,root);
				Tree.SetItemData(item, OPC_LEAF);
				CoTaskMemFree(tmp);
			}
		} while(fetched>0);

		TVSORTCB SortCB;
		SortCB.hParent = root;
		SortCB.lParam = (LPARAM)Tree.m_hWnd;
		SortCB.lpfnCompare = &CompareChannelItems;
		Tree.SortChildrenCB(&SortCB, 0);
	}

	void CChannelsView::FillTreeFlat(HTREEITEM root, CComPtr<IOPCBrowseServerAddressSpace> sas)
	{
		CTreeViewCtrl Tree = m_Channels.GetTree();
		CComPtr<IEnumString> es;
		HRESULT hr;
		hr = sas->BrowseOPCItemIDs(OPC_FLAT, L"", VT_EMPTY, 0, &es);
		DWORD fetched;
		do 
		{
			LPOLESTR tmp;
			hr = es->Next(1, &tmp, &fetched);
			if(fetched>0)
			{
				HTREEITEM item;
				item = Tree.InsertItem((CString)tmp,2,2,root,root);
				Tree.SetItemData(item, OPC_LEAF);
				CoTaskMemFree(tmp);
			}
		} while(fetched>0);

		TVSORTCB SortCB;
		SortCB.hParent = root;
		SortCB.lParam = (LPARAM)Tree.m_hWnd;
		SortCB.lpfnCompare = &CompareChannelItems;
		Tree.SortChildrenCB(&SortCB, 0);
	}

	LRESULT CChannelsView::OnAddChannel(WORD /*wNotifyCode*/,  WORD /*wID*/,  HWND /*hWndCtl*/,  BOOL& /*bHandled */)
	{
		DoDataExchange(TRUE);

		CString Computer;
		GetDlgItemText(IDC_ARCH_COMPUTER, Computer);

		if(Computer.IsEmpty())
			return 0;

		Computer.Trim(L"\\/");
		_bstr_t BSTRComputer = _bstr_t(Computer);


		CComboBox Servers = (CComboBox)GetDlgItem(IDC_ARCH_SERVER);
		int idx = Servers.GetCurSel();
		if(idx == -1)
			return 0;

		CLSID* ServerCLSID = (CLSID*)Servers.GetItemDataPtr(idx);

		if(*ServerCLSID == CLSID_NULL)
			return 0;

		BSTR BSTRServer;
		StringFromCLSID(*ServerCLSID, &BSTRServer);

		CTreeViewCtrl Tree = m_Channels.GetTree();

		HTREEITEM Item = Tree.GetSelectedItem();
		if(Item)
		{
			tagOPCBROWSETYPE Type = (tagOPCBROWSETYPE)Tree.GetItemData(Item);
			if(Type == OPC_BRANCH)
				return 0;
		}

		CString Channel = m_Channels.GetValue();
		if(Channel.IsEmpty())
			return 0;
		if(!ValidateChannel(Computer, ServerCLSID, Channel))
		{
			MessageBeep(-1);
			return 0;
		}

		_bstr_t BSTRChannel = _bstr_t(Channel);

		IDBConnectionPtr Conn(__uuidof(DBConnection));

		HRESULT hr = Conn->raw_Open(_bstr_t(m_Module->m_User), _bstr_t(m_Module->m_Password));
		if(FAILED(hr))
		{
			MessageBox(L"Ошибка при подключении к БД.", L"Ошибка", MB_OK|MB_ICONSTOP);
			return 0;
		}

		IChannelsPtr Channels;
		try
		{
			Channels = Conn->GetChannels();
		}
		catch(...)
		{
			CErrorReporterPtr err;
			err->ShowError(Conn);
		}

		try
		{
			const ULONG ChannelID = Channels->AddChannel(BSTRServer, BSTRComputer, BSTRChannel, BSTRChannel);
			Channels->Attribute[ChannelID]["Active"] = "Y";
			Channels->Attribute[ChannelID]["UpdateInterval"] = 1000;
		}
		catch (...)
		{
			CErrorReporterPtr err;
			err->ShowError(Channels);
		}

		UpdateArchiveChannels();

		return 0;
	}
	LRESULT  CChannelsView::OnImportChannels( WORD wNotifyCode,  WORD wID,  HWND hWndCtl,  BOOL& bHandled )
	{
		const wchar_t cFilterProject[]	= L"Project files (*.mn)\0*.mn\0";
		CFileDialog dlg(TRUE, L".mp", NULL, OFN_HIDEREADONLY, cFilterProject);
		if(dlg.DoModal() == IDOK)
		{
			CArchiveManager Archive;
			Archive.SetArchiveFile(dlg.m_szFileName);
			if(!Archive.LoadVariables())
			{
				Archive.SetArchiveFile(L"");
				return false;
			}
			CVariableMenagerPtr mng;
			TStrings list=mng->GetVariableList();
			TStrings::iterator it;
			for(it=list.begin();it!=list.end();it++)
			{
				
				
				


				CVariableMenager::sVariable::sSettings vs=mng->GetVariableSetting(*it);
				CString Channel=vs.OPC.TagName.c_str();
				_bstr_t BSTRComputer = _bstr_t(vs.OPC.Computer.c_str());
				_bstr_t BSTRChannel = _bstr_t(Channel);
				BSTR BSTRServer;
				StringFromCLSID(vs.OPC.guid, &BSTRServer);
				if(!ValidateChannel(vs.OPC.Computer.c_str(), &vs.OPC.guid, Channel))
				{
					MessageBeep(-1);
					return 0;
				}

				

				IDBConnectionPtr Conn(__uuidof(DBConnection));

				HRESULT hr = Conn->raw_Open(_bstr_t(m_Module->m_User), _bstr_t(m_Module->m_Password));
				if(FAILED(hr))
				{
					MessageBox(L"Ошибка при подключении к БД.", L"Ошибка", MB_OK|MB_ICONSTOP);
					return 0;
				}

				IChannelsPtr Channels;
				try
				{
					Channels = Conn->GetChannels();
				}
				catch(...)
				{
					CErrorReporterPtr err;
					err->ShowError(Conn);
				}

				try
				{

					const ULONG ChannelID = Channels->AddChannel(BSTRServer, BSTRComputer, BSTRChannel, BSTRChannel);
					Channels->Attribute[ChannelID]["Active"] = "Y";
					Channels->Attribute[ChannelID]["UpdateInterval"] = 1000;
				}
				catch (...)
				{
					CErrorReporterPtr err;
					err->ShowError(Channels);
				}

				UpdateArchiveChannels();
				

			}
		}




		return 0;
	}


	void CChannelsView::UpdateArchiveChannels()
	{
		FillChannels();
		CListViewCtrl List = (CListViewCtrl)GetDlgItem(IDC_ARCHIVE);
		List.DeleteAllItems();

		IDBConnectionPtr Conn(__uuidof(DBConnection));

		HRESULT hr = Conn->raw_Open(_bstr_t(m_Module->m_User), _bstr_t(m_Module->m_Password));

		IChannelsPtr Channels;
		hr = Conn->raw_GetChannels(&Channels);
		IDataSetPtr DataSet; 
		hr = Channels->raw_GetChannelsList(CComBSTR(""),CComBSTR(""),CComBSTR(""), &DataSet);
		ULONG count;
		hr = DataSet->raw_GetRecordsCount(&count);
		if(DataSet->GetRecordsCount() > 0)
		{
			DataSet->MoveFirst();
			while(!DataSet->IsEOF)
			{
				CComVariant Computer = DataSet->GetFieldValue("Computer");
				CComVariant Channel = DataSet->GetFieldValue("Name");
				CComVariant Server = DataSet->GetFieldValue("ServerGUID");
				CComVariant ID = DataSet->GetFieldValue("ID");

				CString ServerName = GetNameFromGUID(Server.bstrVal);

				int item = List.InsertItem(0, _bstr_t(Channel.bstrVal));
				List.SetItemText(item, 1, _bstr_t(Computer.bstrVal));
				List.SetItemText(item, 2, ServerName);
				List.SetItemData(item, ID.intVal);
				DataSet->MoveNext();
			}
			List.SortItemsEx(CompareChannelItemsLV, (LPARAM)List.m_hWnd);
		}

		for(int i = 0; i < 3; i++)
		{
			List.SetColumnWidth(i, LVSCW_AUTOSIZE & LVSCW_AUTOSIZE_USEHEADER);
		}

	}

	CString CChannelsView::GetNameFromGUID(BSTR clsid)
	{
		CString Result = CString(clsid);
		CLSID ServerCLSID;

		CString Computer;
		CLSIDFromString(clsid, &ServerCLSID);

		HRESULT hresult = AtlPingRemoteHost(Computer, 3000 /*мс*/);
		if(FAILED(hresult))
		{
			return Result;
		}

		CComPtr<IOPCServerList>	sl;
		COSERVERINFO		si;
		MULTI_QI			qi;
		HRESULT				hr;
		ZeroMemory(&si, sizeof(COSERVERINFO));
		ZeroMemory(&qi, sizeof(MULTI_QI));
		si.pwszName = CComBSTR(Computer);
		qi.pIID		= &__uuidof(IOPCServerList);

		hr = CoCreateInstanceEx(CLSID_OpcServerList, NULL, CLSCTX_ALL, &si, 1, &qi);

		if(SUCCEEDED(hr) && SUCCEEDED(qi.hr))
		{
			hr = qi.pItf->QueryInterface(__uuidof(IOPCServerList), (void**)&sl);
			qi.pItf->Release();
		}
		else
			return Result;
		UUID req = CATID_OPCDAServer20;

		CComPtr<IEnumGUID> eguids;
		hr = sl->EnumClassesOfCategories(1, &req, 0, NULL, &eguids);

		if(!TEST_HR(hr))
			return Result;

		LPOLESTR Name;
		LPOLESTR ProgID;
		hr = sl->GetClassDetails(ServerCLSID, &ProgID, &Name);

		Result = CString(Name);
		CoTaskMemFree(ProgID);
		CoTaskMemFree(Name);

		return Result;
	}

	LRESULT CChannelsView::OnDelChannel(WORD /*wNotifyCode*/,  WORD /*wID*/,  HWND /*hWndCtl*/,  BOOL& /*bHandled */)
	{
		CListViewCtrl List = (CListViewCtrl)GetDlgItem(IDC_ARCHIVE);

		if(List.GetSelectedCount() == 0)
			return 0;

		int idx = List.GetSelectedIndex();

		ULONG id = (ULONG)List.GetItemData(idx);

		IDBConnectionPtr Conn(__uuidof(DBConnection));

		HRESULT hr = Conn->raw_Open(_bstr_t(m_Module->m_User), _bstr_t(m_Module->m_Password));
		if(FAILED(hr))
			return 0;

		IChannelsPtr Channels;
		hr = Conn->raw_GetChannels(&Channels);

		if(FAILED(hr))
			return 0;


		hr = Channels->raw_DeleteChannel(id);
		IErrorPtr Error;
		Error = Channels;
		BSTR Str;
		Error->get_ErrorDescription(&Str);

		UpdateArchiveChannels();

		return 0;
	}

	BOOL CChannelsView::ValidateChannel(CString Computer, CLSID* ServerCLSID, CString Channel)
	{
		COSERVERINFO	si;
		MULTI_QI		qi;

		ZeroMemory(&si,sizeof(COSERVERINFO));
		ZeroMemory(&qi,sizeof(MULTI_QI));

		//Запрос интерфейса IOPCServer у сервера
		si.pwszName = CComBSTR(Computer);
		qi.pIID	= &__uuidof(IUnknown);
		HRESULT hr;
		hr = CoCreateInstanceEx(	*ServerCLSID, NULL, CLSCTX_ALL, &si, 1, &qi);
		if(!TEST_HR(hr) || !TEST_HR(qi.hr))
			return FALSE;

		CComPtr<IOPCServer> pServer;
		hr = qi.pItf->QueryInterface(__uuidof(IOPCServer), (void**)&pServer);
		qi.pItf->Release();
		if(!TEST_HR(hr))
			return FALSE;

		//Добавляем группу
		LONG TimeBias			= NULL;
		FLOAT PercentDeadBand	= NULL;
		DWORD UpdateRate		= NULL;
		GUID riid				= __uuidof(IUnknown);
		CComPtr<IUnknown>		pUnk;
		CComPtr<IOPCItemMgt>	pGroup;
		OPCHANDLE				hGroup;

		hr=pServer->AddGroup(	L"",
			TRUE,
			1000,
			0,
			&TimeBias,
			&PercentDeadBand,
			LCID_RUSSIAN,
			&hGroup,
			&UpdateRate,
			riid,
			&pUnk);
		if(!TEST_HR(hr))
			return FALSE;
		if(!TEST_HR(pUnk.QueryInterface(&pGroup)))
			return FALSE;

		OPCITEMDEF		item;
		OPCITEMRESULT	*results;
		HRESULT			*Errors;

		//Добавляем канал на обновление
		ZeroMemory(&item, sizeof(OPCITEMDEF));
		item.bActive	= TRUE;
		item.hClient	= 0;
		item.szItemID	= CComBSTR(Channel);

		hr = pGroup->ValidateItems(1, &item, 0, &results, &Errors);
		if(hr != S_OK)
		{
			pGroup = NULL;
			pServer->RemoveGroup(hGroup, FALSE);
			return FALSE;
		}

		BOOL res = FALSE;

		if(hr == S_OK && Errors[0] == S_OK)
			res = TRUE;
		LocalFree(results);
		LocalFree(Errors);

		pGroup = NULL;
		pServer->RemoveGroup(hGroup, FALSE);
		pServer = NULL;

		return res;
	}
}
