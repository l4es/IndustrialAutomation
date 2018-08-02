#include "StdAfx.h"
#include ".\dbsettingsview.h"
#include "ModuleArchive.h"

#include "ApplicationSettings.h"
#include "EnterUserDlg.h"
#include "EnterUserDlgEx.h"
#include "ErrorReporter.h"

namespace ModuleArchive
{
	
	BOOL CDBSettingsView::PreTranslateMessage(MSG* pMsg)
	{
		return IsDialogMessage(pMsg);
	}

	using namespace Helpers;

	LRESULT CDBSettingsView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CApplicationSettings* Settings = g_ApplicationSettings.Instance();

		if(Settings->GetDatabaseType() == DBT_ACCESS)
		{
			m_Access = 1;
			m_MySQL = 0;
			m_NameAccess = Settings->GetDatabaseName();
			m_PathAccess = Settings->GetDatabasePath();
			m_NameMySQL = "";
			m_ServerMySQL = "";
		}


		if(Settings->GetDatabaseType() == DBT_MYSQL)
		{
			m_Access = 0;
			m_MySQL = 1;
			m_NameMySQL = Settings->GetDatabaseName();
			m_ServerMySQL = Settings->GetDatabaseServer();
			m_NameAccess = "";
			m_PathAccess = "";
		}

		Settings->Release();

		DoDataExchange(FALSE);
		UpdateEnable();
		return 0;
	}


	LRESULT CDBSettingsView::OnSelRadio(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		UpdateEnable();
		return 0;
	}


	LRESULT CDBSettingsView::OnCreateAccess(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoDataExchange(TRUE);
		
		if(!m_Access || m_NameAccess.IsEmpty() || m_PathAccess.IsEmpty())
			return 0;

		CApplicationSettings* Settings = g_ApplicationSettings.Instance();
	
		Settings->SetDatabaseType(DBT_ACCESS);
		Settings->SetDatabaseName(m_NameAccess);
		Settings->SetDatabasePath(m_PathAccess);

		Settings->Release();

		IMaintainerPtr Maintainer;
		HRESULT hr = Maintainer.CreateInstance(__uuidof(::Maintainer));

		using namespace ::Helpers;
		if(FAILED(hr))
		{
			CErrorReporterPtr err;
			err->DecodeHRESULT(hr);
			return 0;
		}

		try
		{
			Maintainer->CreateDatabase("","");
		}catch (...)
		{
			CErrorReporterPtr err;
			err->ShowError(Maintainer);
		}
		return 0;
	}

	LRESULT CDBSettingsView::OnCreateMySQL(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CEnterUserDlg dlg;

		if(dlg.DoModal() == IDOK)
		{
			DoDataExchange(TRUE);
			_bstr_t User = _bstr_t(dlg.m_User);
			_bstr_t Pass = _bstr_t(dlg.m_Password);

			if(!m_MySQL || m_NameMySQL.IsEmpty() || m_ServerMySQL.IsEmpty())
				return 0;

			using namespace Helpers;
			CApplicationSettings* Settings = g_ApplicationSettings.Instance();

			Settings->SetDatabaseType(DBT_MYSQL);
			Settings->SetDatabaseName(m_NameMySQL);
			Settings->SetDatabaseServer(m_ServerMySQL);

			Settings->Release();

			IMaintainerPtr Maintainer;
			HRESULT hr = Maintainer.CreateInstance(__uuidof(::Maintainer));

			using namespace ::Helpers;
			if(FAILED(hr))
			{
				CErrorReporterPtr err;
				err->DecodeHRESULT(hr);
				return 0;
			}

			try
			{
				Maintainer->CreateDatabase(User,Pass);
			}catch(...)
			{
				CErrorReporterPtr err;
				err->ShowError(Maintainer);
				return 0;
			}
		}
		return 0;
	}

	
	void CDBSettingsView::UpdateEnable()
	{
		DoDataExchange(DDX_SAVE);

		GetDlgItem(IDC_SERVER_MYSQL).EnableWindow(m_MySQL);
		GetDlgItem(IDC_NAME_MYSQL).EnableWindow(m_MySQL);
		GetDlgItem(IDC_CREATE_MYSQL).EnableWindow(m_MySQL);
		
		GetDlgItem(IDC_PATH_ACCESS).EnableWindow(m_Access);
		GetDlgItem(IDC_NAME_ACCESS).EnableWindow(m_Access);
		GetDlgItem(IDC_CREATE_ACCESS).EnableWindow(m_Access);
	}

	LRESULT CDBSettingsView::OnSetUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CEnterUserDlgEx dlg;
		if(dlg.DoModal())
		{
			using namespace Helpers;
			CApplicationSettings* Settings = g_ApplicationSettings.Instance();
			Settings->SetDBUser(dlg.m_User, dlg.m_Pass);
			Settings->Release();
		}
		return 0;
	}
	

}
