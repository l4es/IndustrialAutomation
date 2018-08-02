#include "StdAfx.h"
#include ".\variabledialog.h"
#include "resource.h"
#include "SimpleVariableDialog.h"
#include "OPCVariableDialog.h"
#include "ImportOPCChannelsDlg.h"

//Функцтор для сканирования указанного OPC сервера.
//После сканирования член Channels содержит отсортированный список 
//всех каналов OPC сервера.
struct fOPCChannelsScaner
{
	fOPCChannelsScaner(CString Computer, CLSID &Clsid)
		:_Computer(Computer)
		,_Clsid(Clsid){};
	
	//Функтор для сравнения каналов при сортировке
	struct fSort
	{
		bool operator()(const CString& lhs, const CString& rhs)
		{
			return  StrCmpI(lhs,rhs) < 0;
		}
	};

	//Запуск сканирования
	bool Run()
	{
		Channels.clear();

		HRESULT	hr;

		CComPtr<IOPCBrowseServerAddressSpace> sas;

		//Use COM for local connections and DCOM for remote connection
		if(_Computer.MakeLower() == "localhost")
		{
			hr = sas.CoCreateInstance(_Clsid);
			if(FAILED(hr))
				return false; 
		}
		else
		{
			hr = AtlPingRemoteHost(_Computer, 2000 /*мс*/);
			if(hr != S_OK)
				return false;

			COSERVERINFO		si;
			MULTI_QI			qi;
			ZeroMemory(&si, sizeof(COSERVERINFO));
			ZeroMemory(&qi, sizeof(MULTI_QI));

			si.pwszName = CT2W(_Computer);
			qi.pIID		= &__uuidof(IOPCBrowseServerAddressSpace);
			hr = CoCreateInstanceEx(_Clsid, NULL, CLSCTX_ALL, &si, 1, &qi);
			if(SUCCEEDED(hr) && SUCCEEDED(qi.hr))
			{
				hr = qi.pItf->QueryInterface(__uuidof(IOPCBrowseServerAddressSpace), (void**)&sas);
				qi.pItf->Release();
			}
			else
				return false;
		}

		while(SUCCEEDED(sas->ChangeBrowsePosition(OPC_BROWSE_UP, L""))){};
		_FillTree(sas,"");
		
		sort(Channels.begin(), Channels.end(), fSort());
		return true;
	};
	vector<CString> Channels;

protected:
	CString _Computer;
	CLSID &_Clsid;
	void _FillTree(CComPtr<IOPCBrowseServerAddressSpace> sas,CString root)
	{
		CComPtr<IEnumString> es;
		HRESULT hr;
		DWORD fetched;
		OPCNAMESPACETYPE ns_type = OPC_NS_HIERARCHIAL;
		hr = sas->QueryOrganization(&ns_type);
		if(ns_type == OPC_NS_HIERARCHIAL)
		{	
			hr = sas->BrowseOPCItemIDs(OPC_BRANCH, L"", VT_EMPTY, 0/*OPC_READABLE|OPC_WRITEABLE*/, &es);
			if(FAILED(hr)) 
				return;
			do 
			{
				LPOLESTR tmp;
				hr = es->Next(1, &tmp, &fetched);
				if(fetched>0)
				{
					hr = sas->ChangeBrowsePosition(OPC_BROWSE_DOWN, tmp);
					if(FAILED(hr)) 
						return;
					CString newroot;
					if(!root.IsEmpty())
						newroot.Format(L"%s.%s", root,CString(tmp));
					else
						newroot = CString(tmp);
					CoTaskMemFree(tmp);
					_FillTree(sas,newroot);
					hr = sas->ChangeBrowsePosition(OPC_BROWSE_UP, L"");
					if(FAILED(hr)) 
						return;
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
					CString item;
					if(!root.IsEmpty())
						item.Format(L"%s.%s", root,CString(tmp));
					else
						item = CString(tmp);
					CoTaskMemFree(tmp);
					Channels.push_back(item);
				}
			} while(fetched>0);
		}
		else if(ns_type == OPC_NS_FLAT)
		{
			hr = sas->BrowseOPCItemIDs(OPC_FLAT, L"", VT_EMPTY, 0, &es);
			if(FAILED(hr)) 
				return;
			do 
			{
				LPOLESTR tmp;
				hr = es->Next(1, &tmp, &fetched);
				if(fetched>0)
				{
					CString item;
					if(!root.IsEmpty())
						item.Format(L"%s.%s", root,CString(tmp));
					else
						item = CString(tmp);
					CoTaskMemFree(tmp);
					Channels.push_back(item);
				}
			} while(fetched>0);
		}		
	};
};

LRESULT CVariableDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DlgResize_Init(true, true, NULL);
	CenterWindow(GetParent());
	m_List = (CListViewCtrl)GetDlgItem(IDC_VARIABLE_LIST);
	m_List.SetExtendedListViewStyle(LVS_REPORT | LVS_EX_FULLROWSELECT );

	m_List.InsertColumn(0, _T("Name"),	LVCFMT_LEFT, -1, 0);
	m_List.InsertColumn(1, _T("Value"),	LVCFMT_LEFT, -1, 0);

	CVariableMenagerPtr mng;
	TStrings Names = mng->GetVariableList();
	if(!Names.empty())
	{
		for(TStrings::iterator it = Names.begin(); it != Names.end(); it++)
		{
			int item = m_List.InsertItem(0, it->c_str());
			CVariableMenager::enVariableType Type = mng->GetVariableType(*it);
			wstring sValue;
			CComVariant vValue = mng->GetVariable(*it);

			if(Type == CVariableMenager::VT_SIMPLE)
				sValue = VariantToString(vValue);

			if(Type == CVariableMenager::VT_OPC)
			{
				CVariableMenager::sVariable::sSettings Settings = mng->GetVariableSetting(*it);
				sValue = DecodeOPCDesc(Settings);
			}
			m_List.SetItemText(item, 1, sValue.c_str());

			DWORD Data = 0;

			Data |= Type;
			Data <<= 0xf;
			Data |= vValue.vt;
			m_List.SetItemData(item, Data);
		}
	}

	for(int i=1;i>=0;i--)
		m_List.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
	OnItemchanged(NULL);

	return TRUE;
}

LRESULT CVariableDialog::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	EndDialog(IDOK);
	return TRUE;
}

LRESULT CVariableDialog::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	m_List = (CListViewCtrl)GetDlgItem(IDC_VARIABLE_LIST);
	for(int i=1;i>=0;i--)
		m_List.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);

	bHandled = FALSE;
	return TRUE;
}

LRESULT CVariableDialog::OnAddSimpleVariable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CSimpleVariableDialog dlg;
	
	if(dlg.DoModal() == IDOK)
	{
		wstring Name		= dlg.m_Name;
		wstring Value	= dlg.m_Value;

		VARTYPE vType = StringToVARTYPE((wstring)dlg.m_Type);
		if(vType == VT_EMPTY) return 0;

		CComVariant vValue = StringToVariant(Value, vType);

		CVariableMenagerPtr mng;
		mng->AddSimpleVariable(Name, vValue);

		int item = m_List.InsertItem(0, Name.c_str());
		m_List.SetItemText(item,1, VariantToString(vValue).c_str());

		DWORD Data;
		ZeroMemory(&Data, sizeof(DWORD));
		Data |= CVariableMenager::VT_SIMPLE;
		Data <<= 0xf;
		Data |= vValue.vt;
		m_List.SetItemData(item, Data);
	}
	return TRUE;
}

LRESULT CVariableDialog::OnAddOPCVariable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	COPCVariableDialog dlg;
	if(dlg.DoModal() == IDOK)
	{
		wstring Name = dlg.m_Name;
		CComVariant vValue;
		vValue.Clear();
		vValue.ChangeType(StringToVARTYPE((wstring)dlg.m_Type));
		wstring TagName = dlg.m_ChannelName;
		GUID guid = dlg.m_ServerGUID;
		wstring Computer = dlg.m_ComputerName;

		CVariableMenagerPtr mng;
		mng->AddOPCVariable(Name, vValue, TagName, guid, Computer);

		int item = m_List.InsertItem(0, Name.c_str());
		m_List.SetItemText(item,1, DecodeOPCDesc(mng->GetVariableSetting(Name)).c_str());

		DWORD Data;
		ZeroMemory(&Data, sizeof(DWORD));
		Data |= CVariableMenager::VT_OPC;
		Data <<= 0xf;
		Data |= vValue.vt;
		m_List.SetItemData(item, Data);
		 
	}
	return 0;
}

LRESULT CVariableDialog::OnAddVariable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CPoint pt;
	GetCursorPos(&pt);
	CMenu Menu;
	Menu.CreatePopupMenu();
	Menu.AppendMenu(MF_BYPOSITION, ID_ADD_SIMPLE_VARIABLE, L"Simple variable");
	Menu.AppendMenu(MF_BYPOSITION, ID_ADD_OPC_VARIABLE, L"ОРС variable");
	Menu.AppendMenu(MF_BYPOSITION, ID_IMPORT_OPC_CHANNELS, L"Import channels from a OPC server");
	Menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, pt.x, pt.y,m_hWnd);

	return TRUE;
}

LRESULT CVariableDialog::OnDelVariable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int last_selected=-1;
	vector<int> items;
	for(int i = 0; i < m_List.GetItemCount(); i++)
	{
		if(m_List.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			last_selected = i;
			CString Name;
			m_List.GetItemText(i, 0, Name);
			CVariableMenagerPtr mng;
			mng->RemoveVariable((LPCTSTR)Name);
			items.push_back(i);
		}
	}

	for(vector<int>::reverse_iterator it = items.rbegin(); it != items.rend(); it++)
		m_List.DeleteItem(*it);

	if(last_selected >= m_List.GetItemCount())
		last_selected = m_List.GetItemCount()-1;

	for(int i=0;i<m_List.GetItemCount();i++)
	{
		const DWORD flags = LVIS_SELECTED | LVIS_FOCUSED;
		if(i == last_selected)
		{
			const BOOL bRet = m_List.SetItemState(i, flags, LVIS_SELECTED | LVIS_FOCUSED);
			if(bRet)
				m_List.EnsureVisible(i, FALSE);
		}
		else
			m_List.SetItemState(i, ~flags, LVIS_SELECTED | LVIS_FOCUSED);
	}
	return TRUE;
}

LRESULT CVariableDialog::OnChangeVariable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int idx = -1;
	for(int i = 0; i < m_List.GetItemCount(); i++)
	{
		if(m_List.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			if(idx != -1)
				return 0; //Prevents multi-selection
			else
				idx = i;
		}
	}

	wchar_t bName[0xff], bValue[0xff];
	m_List.GetItemText(idx, 0, bName, 0xff);
	m_List.GetItemText(idx, 1, bValue, 0xff);

	DWORD Data = m_List.GetItemData(idx);
	VARTYPE vtType = Data & 0xf;
	Data >>= 0xf;
	CVariableMenager::enVariableType Type = (CVariableMenager::enVariableType)(Data & 0xf);
	wstring Name(bName), Value(bValue);
	
	if(Type == CVariableMenager::VT_SIMPLE)
	{
		CSimpleVariableDialog dlg;
		dlg.m_Name = Name.c_str();
		dlg.m_Value = Value.c_str();
		dlg.m_Type = VARTYPEToString(vtType).c_str();

		if(dlg.DoModal())
		{
			m_List.SetItemText(idx, 0, (LPCTSTR)dlg.m_Name);
			m_List.SetItemText(idx, 1, (LPCTSTR)dlg.m_Value);

			Data = CVariableMenager::VT_SIMPLE;
			Data <<= 0xf;
			Data |= StringToVARTYPE((wstring)dlg.m_Type);
			m_List.SetItemData(idx, Data);
			CVariableMenagerPtr mng;
			mng->SetVariable(Name, StringToVariant((LPCTSTR)dlg.m_Value, StringToVARTYPE((LPCTSTR)dlg.m_Type)), false);

			if(StrCmp(Name.c_str(), (LPCTSTR)dlg.m_Name) != 0)
			{
				mng->RemoveVariable(Name);
				Name = dlg.m_Name;
				Value = dlg.m_Value;
				VARTYPE vType = StringToVARTYPE((wstring)dlg.m_Type);
				mng->AddSimpleVariable(Name, StringToVariant(Value, vType));
			}
		}
	}

	if(Type == CVariableMenager::VT_OPC)
	{
		COPCVariableDialog dlg;
		dlg.m_Name = Name.c_str();
		dlg.m_Type = VARTYPEToString(vtType).c_str();
		
		CVariableMenagerPtr mng;
		CVariableMenager::sVariable::sSettings::sOPC OPC =  mng->GetVariableSetting(Name).OPC;
		dlg.m_ComputerName = OPC.Computer.c_str();
		dlg.m_ChannelName = OPC.TagName.c_str();
		dlg.m_ServerGUID = OPC.guid;

		if(dlg.DoModal() == IDOK)
		{
			wstring Name = dlg.m_Name;
			CComVariant vValue;
			vValue.vt = StringToVARTYPE((wstring)dlg.m_Type);
			wstring TagName = dlg.m_ChannelName;
			GUID guid = dlg.m_ServerGUID;
			wstring Computer = dlg.m_ComputerName;
			
			CVariableMenagerPtr mng;
			mng->AddOPCVariable(Name, vValue, TagName, guid, Computer);
			m_List.SetItemText(idx, 0, (LPCTSTR)dlg.m_Name);
			m_List.SetItemText(idx,1, DecodeOPCDesc(mng->GetVariableSetting(Name)).c_str());

			DWORD Data = 0;
			Data |= CVariableMenager::VT_OPC;
			Data <<= 2;
			Data |= StringToVARTYPE((wstring)dlg.m_Type);
			m_List.SetItemData(idx, Data);
		}

		

	}

	return 0;
}


CComVariant CVariableDialog::StringToVariant(wstring Value, VARTYPE Type)
{
	CComVariant var;
	var.vt = Type;

	switch(Type) 
	{
	case VT_BOOL:
		if((Value == L"0") || (Value == L"false"))
			var.boolVal = VARIANT_FALSE;
		else
			var.boolVal = VARIANT_TRUE;
		break;

	case VT_I4:
		var.lVal = _wtol(Value.c_str());
		break;
	case VT_R4:
		var.fltVal = _wtof(Value.c_str());
		break;
	}
	return var;
}

wstring CVariableDialog::VariantToString(CComVariant Var)
{
	wchar_t buffer[0xff];
	switch(Var.vt) {
	case VT_BOOL:
		if(Var.boolVal == VARIANT_FALSE)
			wsprintf(&buffer[0], L"%s", L"false");
		else
			wsprintf(&buffer[0], L"%s", L"true");
		break;
	case VT_I4:
		wsprintf(&buffer[0], L"%d", Var.lVal);
		break;
	case VT_R4:
		wsprintf(&buffer[0], L"%f", Var.fltVal);
		break;
	default:
		wsprintf(&buffer[0], L"");
	}
	wstring Result(buffer);
	return Result;
}

wstring CVariableDialog::VARTYPEToString(VARTYPE Type)
{
	wstring Name;
	switch(Type) {
	case VT_BOOL:
		Name = L"Boolean";
		break;
	case VT_I4:
		Name = L"Integer";
		break;
	case VT_R4:
		Name = L"Float";
		break;
	default:
		Name = L"";
	}
	return Name;
}


VARTYPE CVariableDialog::StringToVARTYPE(wstring Type)
{
	VARTYPE Result = VT_EMPTY;
	
	if(Type == L"Boolean")
		Result = VT_BOOL;
	if(Type == L"Integer")
		Result = VT_I4;
	if(Type == L"Float")
		Result = VT_R4;
	return Result;
}

wstring CVariableDialog::DecodeOPCDesc(TVariableSettings var)
{
	LPOLESTR guid;
	StringFromCLSID(var.OPC.guid,&guid);

	wstring res;
	res = str(wformat(L"{Tag=«%s», Computer=«%s», OPC Server=«%s»}")%var.OPC.TagName%var.OPC.Computer%wstring(CString(guid)));

	CoTaskMemFree(guid);
	return res;
}


LRESULT CVariableDialog::OnItemchanged(LPNMHDR pnmh)
{
	//LPNMLISTVIEW pnmv = (LPNMLISTVIEW)pnmh;
	
	int item = m_List.GetNextItem(-1, LVIS_SELECTED);

	CWindow(GetDlgItem(IDC_DEL_VAR)).EnableWindow(item >= 0?TRUE:FALSE);
	CWindow(GetDlgItem(IDC_CHANGE_VAR)).EnableWindow(item >= 0?TRUE:FALSE);

	return 0;
}

LRESULT CVariableDialog::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}
LRESULT CVariableDialog::OnImportOpcChannels(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	wchar_t illegal_chars[]=L" (){}[],.\\/-+";
	CImportOPCChannelsDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		//Workaround for "Matrikon OPC Server for Simulation and Testing"
		CComBSTR MatrikonCLSIDString = L"{F8582CF2-88FB-11D0-B850-00C0F0104305}";
		CLSID MatrikonCLSID;
		CLSIDFromString((BSTR)MatrikonCLSIDString, &MatrikonCLSID);
		BOOL isMatrikonServer = IsEqualCLSID(dlg.m_ServerGUID, MatrikonCLSID);
		//////////////////////////////////////////////////////////////////////////

		fOPCChannelsScaner scaner(dlg.m_ComputerName, dlg.m_ServerGUID);
		if(scaner.Run())
		{
			CVariableMenagerPtr mng;

			for(vector<CString>::iterator i=scaner.Channels.begin();i!=scaner.Channels.end();i++)
			{
				//Workaround for "Matrikon OPC Server for Simulation and Testing"
				if(isMatrikonServer)
					i->Replace(L"Simulation Items.",L"");
				//////////////////////////////////////////////////////////////////////////

				CString tmp;
				tmp.Format(L"%s%s",dlg.m_Prefix, *i);

				for(int j=0;j<tmp.GetLength();j++)
					if(StrChr(illegal_chars, tmp[j]))
						tmp.SetAt(j, '_');

				CComVariant vValue;
				vValue.ChangeType(VT_R4);
				mng->AddOPCVariable((LPCTSTR)tmp, vValue, (LPCTSTR)(*i), dlg.m_ServerGUID, (LPCTSTR)dlg.m_ComputerName);
				const int item = m_List.InsertItem(0, tmp);
				m_List.SetItemText(item,1, DecodeOPCDesc(mng->GetVariableSetting((LPCTSTR)tmp)).c_str());

				DWORD Data=NULL;
				Data |= CVariableMenager::VT_OPC;
				Data <<= 0xf;
				Data |= vValue.vt;
				m_List.SetItemData(item, Data);
			}
		}
	}
	for(int i=1;i>=0;i--)
		m_List.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
	return 0;
}
