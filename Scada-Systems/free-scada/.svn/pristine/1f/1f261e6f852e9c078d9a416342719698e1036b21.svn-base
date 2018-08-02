#include "StdAfx.h"
#include ".\opcvariabledialog.h"
#include "../3rdParty/opc/opcda.h"
#include "SelectChannelDialog.h"



LRESULT COPCVariableDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DlgResize_Init(true, true, NULL);
	DoDataExchange(DDX_LOAD);
	CenterWindow(GetParent());

	CComboBox Type = (CComboBox)GetDlgItem(IDC_TYPE_VARIABLE);
	GetDlgItem(IDC_NAME_SERVER).EnableWindow(!m_ComputerName.IsEmpty());
	GetDlgItem(IDC_NAME_CHANNEL).EnableWindow(!m_ComputerName.IsEmpty() && !m_ServerName.IsEmpty());
	GetDlgItem(IDC_BROWSE_CHANNEL).EnableWindow(!m_ComputerName.IsEmpty() && !m_ServerName.IsEmpty());

	Type.AddString(L"Boolean");
	Type.AddString(L"Integer");
	Type.AddString(L"Float");

	if(m_Type.IsEmpty())
		m_Type = L"Float";
	Type.SelectString(-1, m_Type);
	GetDlgItem(IDOK).EnableWindow(IsSetAll());

	//Получаем список серверов для компьютера на который ссылается переменная
	FillListServer();
	
	CComboBox Servers = (CComboBox)GetDlgItem(IDC_NAME_SERVER);
	if(Servers)
	{
		for(int i = 0; i < Servers.GetCount(); i++)
		{
			GUID* pin = (GUID*)Servers.GetItemDataPtr(i);
			if(*pin == m_ServerGUID)
			{
				Servers.SetCurSel(i);
				wchar_t buffer[0xff];
				Servers.GetLBText(i, buffer);
				m_ServerName = buffer;
				break;
			}
		}
	}


	BOOL tmp = FALSE;
	//Делаем выбранным сервер на который ссылается переменная
	OnServerSelchange(NULL,NULL,NULL,tmp);
	return TRUE;
}


LRESULT COPCVariableDialog::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return TRUE;
}

LRESULT COPCVariableDialog::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_SAVE);
	EndDialog(wID);
	return 0;
}

//Разворачивание ComboBox-а со списком серверов
LRESULT COPCVariableDialog::OnCbnDropDown(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	FillListServer();
	return 0;
}

//Заполнение ComboBox-а списком серверов
void COPCVariableDialog::FillListServer()
{
	CWaitCursor wait;
	CComboBox Servers = (CComboBox)GetDlgItem(IDC_NAME_SERVER);
	if(!Servers) return;
	Servers.ResetContent();

	HRESULT hresult = AtlPingRemoteHost(m_ComputerName, 3000 /*мс*/);
	if(FAILED(hresult))
	{
		return;
	}

	CComPtr<IOPCServerList>	sl;
	COSERVERINFO		si;
	MULTI_QI			qi;
	HRESULT				hr;
	ZeroMemory(&si, sizeof(COSERVERINFO));
	ZeroMemory(&qi, sizeof(MULTI_QI));
	si.pwszName = CComBSTR(m_ComputerName);
	qi.pIID		= &__uuidof(IOPCServerList);
	hr = CoCreateInstanceEx(CLSID_OpcServerList, NULL, CLSCTX_ALL, &si, 1, &qi);
	if(SUCCEEDED(hr) && SUCCEEDED(qi.hr))
	{
		hr = qi.pItf->QueryInterface(__uuidof(IOPCServerList), (void**)&sl);
		qi.pItf->Release();
	}
	else
		return;

	CATID req = CATID_OPCDAServer20;
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
	{
		Servers.SetCurSel(0);
	}
}

//Выбор сервера
LRESULT COPCVariableDialog::OnServerSelchange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComboBox Server = (CComboBox)GetDlgItem(IDC_NAME_SERVER);
	int i = Server.GetCurSel();
	if(i == -1) return 0;
	wchar_t buffer[0xff];
	Server.GetLBText(i, buffer);
	m_ServerName = buffer;
	GUID* guid  = (GUID*)Server.GetItemDataPtr(i);
	m_ServerGUID = *guid;


	GetDlgItem(IDC_NAME_SERVER).EnableWindow(!m_ComputerName.IsEmpty());
	GetDlgItem(IDC_NAME_CHANNEL).EnableWindow(!m_ComputerName.IsEmpty() && !m_ServerName.IsEmpty());
	GetDlgItem(IDC_BROWSE_CHANNEL).EnableWindow(!m_ComputerName.IsEmpty() && !m_ServerName.IsEmpty());


	GetDlgItem(IDOK).EnableWindow(IsSetAll());
	return 0;
}

//Выбор типа переменной
LRESULT COPCVariableDialog::OnTypeSelchange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComboBox Combo = (CComboBox)GetDlgItem(IDC_TYPE_VARIABLE);
	int i = Combo.GetCurSel();
	if(i == -1) return 0;
	wchar_t buffer[0xff];
	Combo.GetLBText(i, buffer);
	wstring Type(buffer);
	m_Type = Type.c_str();
	return 0;
}

//Получение имени компьютера при вписывании его "вручную"
LRESULT COPCVariableDialog::OnEnChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	wchar_t buffer[0xff];
	GetDlgItemText(IDC_NAME_COMPUTER, buffer, 0xff);
	if(wcscmp((LPCTSTR)m_ComputerName, buffer) != 0)
	{
		m_ServerName = "";
		m_ComputerName = "";
		m_ServerGUID = GUID_NULL;
		UpdateCtrls();
	}
	m_ComputerName = buffer;

	GetDlgItem(IDC_NAME_SERVER).EnableWindow(!m_ComputerName.IsEmpty());
	GetDlgItem(IDC_NAME_CHANNEL).EnableWindow(!m_ComputerName.IsEmpty() && !m_ServerName.IsEmpty());
	GetDlgItem(IDC_BROWSE_CHANNEL).EnableWindow(!m_ComputerName.IsEmpty() && !m_ServerName.IsEmpty());

	GetDlgItem(IDOK).EnableWindow(IsSetAll());

	return 0;
}

LRESULT COPCVariableDialog::OnNameEnChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	GetDlgItem(IDOK).EnableWindow(IsSetAll());
	return 0;
}

//Проверка заполнены ли все поля для ОРС переменной
bool COPCVariableDialog::IsSetAll()
{
	wchar_t bName[0xff], bComputer[0xff], bChannel[0xff];
	GetDlgItemText(IDC_NAME_OPC_VARIABLE, bName, 0xff);
	GetDlgItemText(IDC_NAME_COMPUTER, bComputer, 0xff);
	GetDlgItemText(IDC_NAME_CHANNEL, bChannel, 0xff);
	int iType = ((CComboBox)GetDlgItem(IDC_TYPE_VARIABLE)).GetCurSel();
	int iServer = ((CComboBox)GetDlgItem(IDC_NAME_SERVER)).GetCurSel();
	wstring sName(bName), sComputer(bComputer), sChannel(bChannel);

	return ((iType != -1) && (iServer != -1) && (!sComputer.empty()) 
		&& (!sName.empty()) && (!sChannel.empty()));
}

//Получение имени компьютера, занесение имени в Edit
LRESULT COPCVariableDialog::OnBrowseComputer(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString buffer = BrowseComputer();
	if(wcscmp((LPCTSTR)m_ComputerName, buffer) != 0)
	{
		m_ServerName = L"";
		m_ComputerName = L"";
		m_ServerGUID = GUID_NULL;
		UpdateCtrls();
	}

	m_ComputerName = buffer;

	if(!m_ComputerName.IsEmpty())
	{
		SetDlgItemText(IDC_NAME_COMPUTER, (LPCTSTR)m_ComputerName);
		((CComboBox)GetDlgItem(IDC_NAME_SERVER)).EnableWindow(TRUE);
	}
	GetDlgItem(IDOK).EnableWindow(IsSetAll());
	return 0;
}

//Вызов диалога выбора компьютера
CString COPCVariableDialog::BrowseComputer()
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

//Вызов диалога выбора канала
LRESULT COPCVariableDialog::OnBrowseChannel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CSelectChannelDialog dlg;
	dlg.m_Computer = m_ComputerName;
	dlg.m_OPCServerCLSID = m_ServerGUID;
	if(dlg.DoModal() == IDOK)
	{
		m_ChannelName = dlg.m_Channel;
		SetDlgItemText(IDC_NAME_CHANNEL, (LPCTSTR)m_ChannelName);
	}
	GetDlgItem(IDOK).EnableWindow(IsSetAll());
	return 0;
}

LRESULT COPCVariableDialog::OnGetMinMaxInfo(LPMINMAXINFO lpMinMaxInfo)
{
	lpMinMaxInfo->ptMaxTrackSize.y = m_ptMinTrackSize.y;
	SetMsgHandled(FALSE);
	return 0;
}

//Обновление контролов отображающих название сервера и тэга
void COPCVariableDialog::UpdateCtrls()
{
	CComboBox Servers = (CComboBox)GetDlgItem(IDC_NAME_SERVER);
	if(m_ServerName == "")
		Servers.SetCurSel(-1);
	else
		Servers.SelectString(-1,m_ServerName);

	SetDlgItemText(IDC_NAME_CHANNEL, (LPCTSTR)m_ChannelName);
	

}
