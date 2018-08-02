#include "StdAfx.h"
#include ".\channelsettingsview.h"
#include "ModuleArchive.h"


namespace ModuleArchive
{
	LRESULT CChannelSettingsView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init(false, true, NULL);
		CListViewCtrl List = (CListViewCtrl)GetDlgItem(IDC_CHANNELS);
		List.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES );
	
		List.InsertColumn(0, L"Channel",LVCFMT_CENTER, -1, -1);
		List.InsertColumn(1, L"Host",LVCFMT_CENTER, -1, -1);
		List.InsertColumn(2, L"OPC Server",LVCFMT_CENTER, -1, -1);

		for(int i = 0; i < 3; i++)
			List.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	
		InitializeAtributes();
		m_Name = m_Atributes[IDC_NAME].Value;
		DoDataExchange(DDX_LOAD);

		for(UINT i = IDC_UPDATE_INTERVAL; i <= IDC_ARCH_ON_GREATER_SET; i++)
				GetDlgItem(i).EnableWindow(FALSE);
		GetDlgItem(IDC_NAME).EnableWindow(FALSE);
		GetDlgItem(IDC_ARCH_ON_EQUAL).EnableWindow(FALSE);
		GetDlgItem(IDC_ARCH_ON_EQUAL_SET).EnableWindow(FALSE);

		m_ItemIndex = -1;
		m_State = -1;
		m_CurrentID = 0;
		m_LockUpdate = FALSE;
		UpdateEnable();
		return 0;
	}

	void CChannelSettingsView::InitializeAtributes()
	{
		sAtribute UpdateInterval;
		UpdateInterval.Name = "UpdateInterval";
		UpdateInterval.Value.ChangeType(VT_R4);
		UpdateInterval.Value.fltVal = 0; 
		m_Atributes[IDC_UPDATE_INTERVAL] = UpdateInterval;

		sAtribute GenerateEventsChange;
		GenerateEventsChange.Name = "GenerateEventsChange";
		GenerateEventsChange.Value.ChangeType(VT_I4);
		GenerateEventsChange.Value.boolVal = 0; 
		m_Atributes[IDC_EVENTS_CHANGE] = GenerateEventsChange;

		sAtribute GenerateEventsOnCrossVal1;
		GenerateEventsOnCrossVal1.Name = "GenerateEventsOnCrossVal1";
		GenerateEventsOnCrossVal1.Value.ChangeType(VT_I4);
		GenerateEventsOnCrossVal1.Value.intVal = 0;
		m_Atributes[IDC_EVENTS_CROSS_VAL1] = GenerateEventsOnCrossVal1;

		sAtribute GenerateEventsOnCrossVal1_Set;
		GenerateEventsOnCrossVal1_Set.Name = "GenerateEventsOnCrossVal1_Set";
		GenerateEventsOnCrossVal1_Set.Value.ChangeType(VT_R4);
		GenerateEventsOnCrossVal1_Set.Value.fltVal = 0.0;
		m_Atributes[IDC_EVENTS_CROSS_VAL1_SET] = GenerateEventsOnCrossVal1_Set;

		sAtribute GenerateEventsOnCrossVal2;
		GenerateEventsOnCrossVal2.Name = "GenerateEventsOnCrossVal2";
		GenerateEventsOnCrossVal2.Value.ChangeType(VT_I4);
		GenerateEventsOnCrossVal2.Value.intVal = 0;
		m_Atributes[IDC_EVENTS_CROSS_VAL2] = GenerateEventsOnCrossVal2;

		sAtribute GenerateEventsOnCrossVal2_Set;
		GenerateEventsOnCrossVal2_Set.Name = "GenerateEventsOnCrossVal2_Set";
		GenerateEventsOnCrossVal2_Set.Value.ChangeType(VT_R4);
		GenerateEventsOnCrossVal2_Set.Value.fltVal = 0.0;
		m_Atributes[IDC_EVENTS_CROSS_VAL2_SET] = GenerateEventsOnCrossVal2_Set;

		sAtribute ArchOnLess;
		ArchOnLess.Name = "ArchOnLess";
		ArchOnLess.Value.ChangeType(VT_I4);
		ArchOnLess.Value.intVal = 0;
		m_Atributes[IDC_ARCH_ON_LESS] = ArchOnLess;

		sAtribute ArchOnGreater;
		ArchOnGreater.Name = "ArchOnGreater";
		ArchOnGreater.Value.ChangeType(VT_I4);
		ArchOnGreater.Value.intVal = 0;
		m_Atributes[IDC_ARCH_ON_GREATER] = ArchOnGreater;

		sAtribute ArchOnEqual;
		ArchOnEqual.Name = "ArchOnEqual";
		ArchOnEqual.Value.ChangeType(VT_I4);
		ArchOnEqual.Value.intVal = 0;
		m_Atributes[IDC_ARCH_ON_EQUAL] = ArchOnEqual;

		sAtribute ArchOnLess_Set;
		ArchOnLess_Set.Name = "ArchOnLess_Set";
		ArchOnLess_Set.Value.ChangeType(VT_R4);
		ArchOnLess_Set.Value.fltVal = 0.0;
		m_Atributes[IDC_ARCH_ON_LESS_SET] = ArchOnLess_Set;

		sAtribute ArchOnGreater_Set;
		ArchOnGreater_Set.Name = "ArchOnGreater_Set";
		ArchOnGreater_Set.Value.ChangeType(VT_R4);
		ArchOnGreater_Set.Value.fltVal = 0.0;
		m_Atributes[IDC_ARCH_ON_GREATER_SET] = ArchOnGreater_Set;

		sAtribute ArchOnEqual_Set;
		ArchOnEqual_Set.Name = "ArchOnEqual_Set";
		ArchOnEqual_Set.Value.ChangeType(VT_R4);
		ArchOnEqual_Set.Value.fltVal = 0.0;
		m_Atributes[IDC_ARCH_ON_EQUAL_SET] = ArchOnEqual_Set;

		sAtribute HumanReadableName;
		HumanReadableName.Name = "HumanReadableName";
		HumanReadableName.Value.ChangeType(VT_BSTR);
		HumanReadableName.Value.bstrVal = CComBSTR("");
		m_Atributes[IDC_NAME] = HumanReadableName;
	}

	LRESULT CChannelSettingsView::OnClickCheck(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		UpdateEnable();

		TAtribMap::iterator it = m_Atributes.find(wID);
		if(it ==  m_Atributes.end())
			return 0;

		sAtribute Atribute = it->second;

		CListViewCtrl List = (CListViewCtrl)GetDlgItem(IDC_CHANNELS);
		int idx = List.GetSelectedIndex();
		if(idx == -1)
			return 0;

		ULONG id = List.GetItemData(idx);

		IDBConnectionPtr Conn(__uuidof(DBConnection));
		HRESULT hr = Conn->raw_Open(_bstr_t(m_Module->m_User), _bstr_t(m_Module->m_Password));
		if(FAILED(hr))
			return 0;

		IChannelsPtr Channels;
		hr = Conn->raw_GetChannels(&Channels);
		if(FAILED(hr))
			return 0;

		CComVariant Value = Atribute.Value;
		Value.ChangeType(VT_BSTR);

		hr = Channels->put_Attribute(id, CComBSTR(Atribute.Name), Value);

		return 0;
	}

	LRESULT CChannelSettingsView::OnItemChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
	{
		CWaitCursor wait;
		LPNMLISTVIEW pLV = (LPNMLISTVIEW)pnmh;

		CListViewCtrl List = (CListViewCtrl)GetDlgItem(IDC_CHANNELS);
		int idx = pLV->iItem;
		if(m_CurrentID != 0)
		{
			DoDataExchange(DDX_SAVE);
			m_Atributes[IDC_NAME].Value = m_Name;
			SaveChannel(m_CurrentID);
		}
		if(idx == -1)
			m_CurrentID = 0;
		else
			m_CurrentID = List.GetItemData(idx);


		if(!m_LockUpdate)
		{
			IDBConnectionPtr Conn(__uuidof(DBConnection));
			HRESULT hr = Conn->raw_Open(_bstr_t(m_Module->m_User), _bstr_t(m_Module->m_Password));
			if(SUCCEEDED(hr))
			{
				IChannelsPtr Channels;
				hr = Conn->raw_GetChannels(&Channels);
				if(SUCCEEDED(hr))
					Channels->put_Active(m_CurrentID, List.GetCheckState(idx)?VARIANT_TRUE:VARIANT_FALSE);
			}
		}
		LoadChannel(m_CurrentID);
		UpdateEnable();
		return 0;
	}

	void CChannelSettingsView::UpdateChannels()
	{
		ULONG tmpID = m_CurrentID;
		CWaitCursor wait;
		m_LockUpdate = TRUE;
		CListViewCtrl List = (CListViewCtrl)GetDlgItem(IDC_CHANNELS);
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
			for(ULONG i = 0; i < DataSet->GetRecordsCount(); i++)
			{
				CComVariant Computer = DataSet->GetFieldValue("Computer");
				CComVariant Channel = DataSet->GetFieldValue("Name");
				CComVariant Server = DataSet->GetFieldValue("ServerGUID");
				CComVariant ID = DataSet->GetFieldValue("ID");
				
				CString ServerName = GetNameFromGUID(Computer.bstrVal, Server.bstrVal);

				List.InsertItem(i, _bstr_t(Channel.bstrVal));
				List.SetItemText(i, 1, _bstr_t(Computer.bstrVal));
				List.SetItemText(i, 2, ServerName);
	
				VARIANT_BOOL Active;
				Channels->get_Active(ID.intVal, &Active); 
		
			//SetCheckState вызывать перед SetItemData
			//Иначе будет перезапись CheckState в OnItemChanged
			
				if(Active == VARIANT_TRUE)
					List.SetCheckState(i, TRUE);
				else
					List.SetCheckState(i, FALSE);

				
				List.SetItemData(i, ID.intVal);
				if(ID.intVal == tmpID)
					List.SelectItem(i);
				DataSet->MoveNext();
			}
		}
		for(int i = 0; i < 3; i++)
		{
			List.SetColumnWidth(i, LVSCW_AUTOSIZE & LVSCW_AUTOSIZE_USEHEADER);
		}
		m_CurrentID = tmpID;
		m_LockUpdate = FALSE;
	}

	CString CChannelSettingsView::GetNameFromGUID(BSTR Computer, BSTR clsid)
	{
		CString Result = CString(clsid);
		CLSID ServerCLSID;

		CLSIDFromString(clsid, &ServerCLSID);
		
		HRESULT hresult = AtlPingRemoteHost(CString(Computer), 3000 /*мс*/);
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
		si.pwszName = Computer;//CComBSTR(m_Computer);
		qi.pIID		= &__uuidof(IOPCServerList);

		CLSID ClsidServer;
		CLSIDFromProgID(L"OPC.ServerList", &ClsidServer);

		hr = CoCreateInstanceEx(ClsidServer, NULL, CLSCTX_ALL, &si, 1, &qi);

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

	void CChannelSettingsView::UpdateEnable()
	{
		for(UINT i = IDC_UPDATE_INTERVAL; i <= IDC_ARCH_ON_GREATER; i++)
		{
			if(!GetDlgItem(i).IsWindowEnabled())
			{
				GetDlgItem(i).EnableWindow(TRUE);
			}
		}
		if(!GetDlgItem(IDC_ARCH_ON_EQUAL).IsWindowEnabled())
		{
			GetDlgItem(IDC_ARCH_ON_EQUAL).EnableWindow(TRUE);
		}

		GetDlgItem(IDC_NAME).EnableWindow(TRUE);

		DoDataExchange(DDX_SAVE);
		m_Atributes[IDC_NAME].Value = m_Name;
		GetDlgItem(IDC_EVENTS_CROSS_VAL1_SET).EnableWindow(m_Atributes[IDC_EVENTS_CROSS_VAL1].Value.intVal);
		GetDlgItem(IDC_EVENTS_CROSS_VAL2_SET).EnableWindow(m_Atributes[IDC_EVENTS_CROSS_VAL2].Value.intVal);
		GetDlgItem(IDC_ARCH_ON_LESS_SET).EnableWindow(m_Atributes[IDC_ARCH_ON_LESS].Value.intVal);
		GetDlgItem(IDC_ARCH_ON_GREATER_SET).EnableWindow(m_Atributes[IDC_ARCH_ON_GREATER].Value.intVal);
		GetDlgItem(IDC_ARCH_ON_EQUAL_SET).EnableWindow(m_Atributes[IDC_ARCH_ON_EQUAL].Value.intVal);
	}

	LRESULT CChannelSettingsView::OnShowWindow(BOOL fShow, UINT /*status*/)
	{
		SetMsgHandled(FALSE);
		if(fShow == FALSE)
		{
			if(m_CurrentID != 0)
				SaveChannel(m_CurrentID);
		}
		return 0;
	}

	LRESULT CChannelSettingsView::OnDestroy(void)
	{
		SetMsgHandled(FALSE);
		DoDataExchange(DDX_SAVE);
		m_Atributes[IDC_NAME].Value = m_Name;
		if(m_CurrentID != 0)
			SaveChannel(m_CurrentID);
		return 0;
	}

	void CChannelSettingsView::SaveChannel(ULONG ID)
	{
		if(m_LockUpdate)
			return;

		CWaitCursor wait;
		IDBConnectionPtr Conn(__uuidof(DBConnection));
		HRESULT hr = Conn->raw_Open(_bstr_t(m_Module->m_User), _bstr_t(m_Module->m_Password));
		if(FAILED(hr))
			return;

		IChannelsPtr Channels;
		hr = Conn->raw_GetChannels(&Channels);
		if(FAILED(hr))
			return;

		for(TAtribMap::iterator i = m_Atributes.begin(); i != m_Atributes.end(); i++)
		{
			sAtribute Atribute = i->second;

			CComVariant Value = Atribute.Value;
			if(Atribute.Name == "UpdateInterval")
			{
				Value.fltVal *= 1000;
				Value.ChangeType(VT_UI4);
			}

			Value.ChangeType(VT_BSTR);
			hr = Channels->put_Attribute(ID, CComBSTR(Atribute.Name), Value);
		}
	}

	void CChannelSettingsView::LoadChannel(ULONG ID)
	{
		if(m_LockUpdate)
			return;

		CWaitCursor wait;
		IDBConnectionPtr Conn(__uuidof(DBConnection));
		HRESULT hr = Conn->raw_Open(_bstr_t(m_Module->m_User), _bstr_t(m_Module->m_Password));
		if(FAILED(hr))
			return;

		IChannelsPtr Channels;
		hr = Conn->raw_GetChannels(&Channels);
		if(FAILED(hr))
			return;

		for(TAtribMap::iterator i = m_Atributes.begin(); i != m_Atributes.end(); i++)
		{
			sAtribute Atribute = i->second;

			CComVariant Value;

			hr = Channels->get_Attribute(ID, CComBSTR(Atribute.Name), &Value);
			if(SUCCEEDED(hr))
			{
				hr = Value.ChangeType(Atribute.Value.vt);
				if(SUCCEEDED(hr))
				{
					if(Atribute.Name == "UpdateInterval")
						Value.fltVal /= 1000;
					i->second.Value = Value;
				}
			}
		}
		m_Name = m_Atributes[IDC_NAME].Value;
		DoDataExchange(DDX_LOAD);
	}
}
