#include "StdAfx.h"
#include ".\userssettingsview.h"
#include "ModuleArchive.h"
#include "EnterUserDlgEx.h"
#include "ErrorReporter.h"

namespace ModuleArchive
{
	BOOL CUsersSettingsView::PreTranslateMessage(MSG* pMsg)
	{
		return IsDialogMessage(pMsg);
	}


	LRESULT CUsersSettingsView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		
		DlgResize_Init(false, true, NULL);
		CListViewCtrl List = (CListViewCtrl)GetDlgItem(IDC_USERS);
		List.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		
		List.InsertColumn(0, L"Login", LVCFMT_LEFT, -1, -1);
		List.InsertColumn(1, L"User", LVCFMT_LEFT, -1, -1);

		for(int i = 0; i < 2; i++)
			List.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
		Reset();
		return 0;
	}

	LRESULT CUsersSettingsView::OnAddUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoDataExchange(DDX_SAVE);
		ReadPermission();

		CEnterUserDlgEx dlg;
		dlg.m_User = "";

		CString Login = "", Password = "";

		if(dlg.DoModal() == IDOK)
		{
			Login = dlg.m_User;
			Password = dlg.m_Pass;
		}
		else
		{
			return 0;
		}

		m_CurrentLogin = Login;

		IDBConnectionPtr Conn(__uuidof(DBConnection));
		HRESULT hr = Conn->raw_Open(_bstr_t(m_Module->m_User), _bstr_t(m_Module->m_Password));
		if(FAILED(hr))
			return 0;

		IUsersPtr Users;
		hr = Conn->raw_GetUsers(&Users);
		if(FAILED(hr))
			return 0;

		hr = Users->raw_AddUser(_bstr_t(m_FirstName), _bstr_t(m_MiddleName), _bstr_t(m_LastName),
						_bstr_t(Login), _bstr_t(Password), m_Permission);

		FillUsers();
		return 0;
	}

	LRESULT CUsersSettingsView::OnDelUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		IDBConnectionPtr Conn(__uuidof(DBConnection));
		
		HRESULT hr = Conn->raw_Open(_bstr_t(m_Module->m_User), _bstr_t(m_Module->m_Password));
		if(FAILED(hr))
			return 0;

		IUsersPtr Users;
		hr = Conn->raw_GetUsers(&Users);
		if(FAILED(hr))
			return 0;

		if(m_CurrentLogin.IsEmpty())
			return 0;

		try
		{
			Users->DeleteUser(_bstr_t(m_CurrentLogin));
		}
		catch(_com_error err)
		{
			if(err.Error() == E_ACCESSDENIED)
			{
				::MessageBox(NULL, L"You don't have rights to do that.",L"Access denied", MB_OK|MB_ICONSTOP);
				return 0;
			}
			::Helpers::CErrorReporterPtr log;
			log->ShowError(Users);
			return 0;
		}

		FillUsers();
		return 0;
	}

	LRESULT CUsersSettingsView::OnChangeUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		IDBConnectionPtr Conn(__uuidof(DBConnection));

		HRESULT hr = Conn->raw_Open(_bstr_t(m_Module->m_User), _bstr_t(m_Module->m_Password));
		if(FAILED(hr))
			return 0;

		IUsersPtr Users;
		hr = Conn->raw_GetUsers(&Users);
		if(FAILED(hr))
			return 0;

		if(m_CurrentLogin.IsEmpty())
			return 0;

		DoDataExchange(DDX_SAVE);
		hr = Users->raw_SetUserInfo(_bstr_t(m_CurrentLogin), _bstr_t(m_FirstName), _bstr_t(m_MiddleName), _bstr_t(m_LastName));

		ReadPermission();

		hr = Users->raw_SetUserPermissions(CComBSTR(m_CurrentLogin), m_Permission);
		if(FAILED(hr))
			return 0;

		FillUsers();
		return 0;
	}

	void CUsersSettingsView::FillUsers()
	{
		CListViewCtrl List = (CListViewCtrl)GetDlgItem(IDC_USERS);
		List.DeleteAllItems();
		IDBConnectionPtr Conn(__uuidof(DBConnection));

		HRESULT hr = Conn->raw_Open(_bstr_t(m_Module->m_User), _bstr_t(m_Module->m_Password));
		if(FAILED(hr))
			return;

		IUsersPtr Users;
		hr = Conn->raw_GetUsers(&Users);
		if(FAILED(hr))
			return;

		IDataSetPtr Set;
		hr = Users->raw_GetUsersList(&Set);
		if(FAILED(hr))
			return;

		Set->MoveFirst();
		for(ULONG i = 0; i < Set->GetRecordsCount(); i++)
		{
			CComVariant vLogin = Set->GetFieldValue(_bstr_t("Login"));
			BSTR bFirstName, bMiddleName, bLastName;
			Users->GetUserInfo(vLogin.bstrVal, &bFirstName, &bMiddleName, &bLastName);
			
			CString Login  =CString(vLogin);
			CString FirstName = CString(bFirstName);
			CString MiddleName = CString(bMiddleName);
			CString LastName = CString(bLastName);
			CString FullName = "";

			if(!LastName.IsEmpty())
			{
				FullName = LastName;
				if(!FirstName.IsEmpty())
				{
					FullName += '.';
					FullName += FirstName[0];
				}
				if(!MiddleName.IsEmpty())
				{
					FullName += '.';
					FullName += MiddleName[0];
				}
			}

			List.InsertItem(i, Login);
			List.SetItemText(i, 1, FullName);
			Set->MoveNext();
		}

		for(int i = 0; i < 2; i++)
			List.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER & LVSCW_AUTOSIZE);
	}

	LRESULT CUsersSettingsView::OnSetPassword(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(m_CurrentLogin.IsEmpty())
			return 0;

		CEnterUserDlgEx dlg;
		dlg.m_EnableUser = false;
	
		dlg.m_User = m_CurrentLogin;
		if(dlg.DoModal() == IDOK)
		{
			IDBConnectionPtr Conn(__uuidof(DBConnection));

			HRESULT hr = Conn->raw_Open(_bstr_t(m_Module->m_User), _bstr_t(m_Module->m_Password));
			if(FAILED(hr))
				return 0;

			IUsersPtr Users;
			hr = Conn->raw_GetUsers(&Users);
			if(FAILED(hr))
				return 0;
			
			try
			{
				Users->SetUserPassword((BSTR)CComBSTR(dlg.m_User), (BSTR)CComBSTR(dlg.m_Pass));
			}
			catch(_com_error err)
			{
				if(err.Error() == E_ACCESSDENIED)
				{
					::MessageBox(NULL, L"You don't have rights to do that.",L"Access denied", MB_OK|MB_ICONSTOP);
					return false;
				}
				::Helpers::CErrorReporterPtr log;
				log->ShowError(Users);
				return false;
			}
		}
		return 0;
	}

	void CUsersSettingsView::ReadPermission()
	{
		m_Permission =  0;
		if(m_CanEditUsers)
			m_Permission |= PERM_CANEDITUSERS;
		if(m_CanViewUsers)
			m_Permission |= PERM_CANVIEWUSERS;
		if(m_CanViewData)
			m_Permission |= PERM_CANVIEWDATA;
		if(m_CanEditData)
			m_Permission |= PERM_CANEDITDATA;
		if(m_CanViewEvents)
			m_Permission |= PERM_CANVIEWEVENTS;
		if(m_CanEditEvents)
			m_Permission |= PERM_CANEDITEVENTS;
		if(m_CanEditChannels)
			m_Permission |= PERM_CANEDITCHANNELS;
	}

	void CUsersSettingsView::Reset()
	{
		m_FirstName = "";
		m_MiddleName = "";
		m_LastName = "";
		m_CurrentLogin = "";

		m_Permission = 0;
		m_CanEditUsers = 0;
		m_CanViewUsers = 0;
		m_CanEditData = 0;
		m_CanViewData = 0;
		m_CanEditEvents = 0;
		m_CanViewEvents = 0;
		m_CanEditChannels = 0;
	}

	LRESULT CUsersSettingsView::OnItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	{
		CListViewCtrl List = (CListViewCtrl)GetDlgItem(IDC_USERS);
		if(List.GetSelectedCount() != 1)
			return 0;
	
		int idx = List.GetSelectedIndex();

		List.GetItemText(idx, 0, m_CurrentLogin);

		IDBConnectionPtr Conn(__uuidof(DBConnection));
		HRESULT hr = Conn->Open(_bstr_t(m_Module->m_User), _bstr_t(m_Module->m_Password));
		IUsersPtr Users;

		hr = Conn->raw_GetUsers(&Users);
		if(FAILED(hr))
			return 0;
		BSTR bFirstName, bMiddleName, bLastName;
		
		hr = Users->raw_GetUserInfo(CComBSTR(m_CurrentLogin), &bFirstName, &bMiddleName, &bLastName);
		if(FAILED(hr))
			return 0;
		
		ULONG Permission;
		hr = Users->raw_GetUserPermissions(CComBSTR(m_CurrentLogin), &Permission);
		if(FAILED(hr))
			return 0;

		m_FirstName = CString(bFirstName);
		m_MiddleName = CString(bMiddleName);
		m_LastName = CString(bLastName);

		if(Permission & PERM_CANEDITUSERS)
			m_CanEditUsers = 1;
		else
			m_CanEditUsers = 0;

		if(Permission & PERM_CANVIEWUSERS)
			m_CanViewUsers = 1;
		else
			m_CanViewUsers = 0;

		if(Permission & PERM_CANEDITDATA)
			m_CanEditData = 1;
		else
			m_CanEditData = 0;

		if(Permission & PERM_CANVIEWDATA)
			m_CanViewData = 1;
		else
			m_CanViewData = 0;

		if(Permission & PERM_CANEDITEVENTS)
			m_CanEditEvents = 1;
		else
			m_CanEditEvents = 0;

		if(Permission & PERM_CANVIEWEVENTS)
			m_CanViewEvents = 1;
		else
			m_CanViewEvents = 0;

		if(Permission & PERM_CANEDITCHANNELS)
			m_CanEditChannels = 1;
		else
			m_CanEditChannels = 0;

		DoDataExchange(DDX_LOAD);
		return 0;
	}

	LRESULT CUsersSettingsView::OnSetUserPermission(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoDataExchange(DDX_SAVE);
		m_CanEditUsers = 0;
		m_CanViewUsers = 0;
		m_CanEditData = 0;
		m_CanViewData = 1;
		m_CanEditEvents = 0;
		m_CanViewEvents = 1;
		m_CanEditChannels = 0;
		DoDataExchange(DDX_LOAD);
		return 0;
	}

    LRESULT CUsersSettingsView::OnSetAdminPermission(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoDataExchange(DDX_SAVE);
		m_CanEditUsers = 1;
		m_CanViewUsers = 1;
		m_CanEditData = 1;
		m_CanViewData = 1;
		m_CanEditEvents = 1;
		m_CanViewEvents = 1;
		m_CanEditChannels = 1;
		DoDataExchange(DDX_LOAD);
		return 0;
	}

}
