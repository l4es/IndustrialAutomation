#include "StdAfx.h"
#include ".\modulearchive.h"
#include "ApplicationSettings.h"
#include "EnterUserDlg.h"
#include "ErrorReporter.h"

namespace ModuleArchive
{

	bool CModuleArchive::Detect()
	{
		return true;
	}

	HWND CModuleArchive::GetWndHandle()
	{
		m_hWindow = NULL;
		if(m_CurrentNode->ItemID == -1)
			m_hWindow = m_DBSettings;

		if(m_CurrentNode->ItemID == 1)
			m_hWindow = m_UsersSettings;

		if(m_CurrentNode->ItemID == 2)
			m_hWindow = m_Channels;

		if(m_CurrentNode->ItemID == 3)
			m_hWindow = m_ChannelSettings;

		if(m_CurrentNode->ItemID == 4)
			m_hWindow = m_Events;

		return m_hWindow;
	}


	bool CModuleArchive::Load(HWND hParent, HWND /*hMainWnd*/,  CRect /*wndRect*/)
	{

		m_hWindow = NULL;
		m_DBSettings.Create(hParent);
		m_DBSettings.m_Module = this;
		m_Channels.Create(hParent);
		m_Channels.m_Module = this;
		m_UsersSettings.Create(hParent);
		m_UsersSettings.m_Module = this;
		m_ChannelSettings.Create(hParent);
		m_ChannelSettings.m_Module = this;
		m_Events.Create(hParent);
		m_Events.m_Module = this;

		LoadNodes(NULL);

		if(IsWindow(m_hWindow))
			return true;

		return false;
	}

	bool CModuleArchive::LoadNodes(sNode* Parent)
	{

		m_Node = new sNode();
		m_Node->Module = this;
		m_Node->Type = T_BRANCH;
		m_Node->ParentNode = Parent;
		m_Node->IconOpen	= NI_ARCHIVER;
		m_Node->IconClose	= NI_ARCHIVER;
		m_Node->Module = this;
		m_Node->ItemID = static_cast<DWORD>(-1);
		m_Node->Name = L"Archiver";

		sNode* DBSettings = new sNode();
		DBSettings->Module = this;
		DBSettings->Type = T_ITEM;
		DBSettings->ParentNode = m_Node;
		DBSettings->IconInactive	= NI_DETAIL_OPTION;
		DBSettings->IconActive		= NI_DETAIL_OPTION;
		DBSettings->Module = this;
		DBSettings->ItemID = 1;
		DBSettings->Name = L"User configuration";
		
		m_Node->ChildNodes.push_back(DBSettings);

		sNode* AddChnnls = new sNode();
		AddChnnls->Module = this;
		AddChnnls->Type = T_ITEM;
		AddChnnls->ParentNode = m_Node;
		AddChnnls->IconInactive	= NI_DETAIL_OPTION;
		AddChnnls->IconActive	= NI_DETAIL_OPTION;
		AddChnnls->Module = this;
		AddChnnls->ItemID = 2;
		AddChnnls->Name = L"Add and remove channels";

		m_Node->ChildNodes.push_back(AddChnnls);

		sNode* SttgsChnnls = new sNode();
		SttgsChnnls->Module = this;
		SttgsChnnls->Type = T_ITEM;
		SttgsChnnls->ParentNode = m_Node;
		SttgsChnnls->IconInactive	= NI_DETAIL_OPTION;
		SttgsChnnls->IconActive	= NI_DETAIL_OPTION;
		SttgsChnnls->Module = this;
		SttgsChnnls->ItemID = 3;
		SttgsChnnls->Name = L"Channels configuration";

		m_Node->ChildNodes.push_back(SttgsChnnls);

		sNode* SttgsEvents = new sNode();
		SttgsEvents->Module = this;
		SttgsEvents->Type = T_ITEM;
		SttgsEvents->ParentNode = m_Node;
		SttgsEvents->IconInactive	= NI_DETAIL_OPTION;
		SttgsEvents->IconActive	= NI_DETAIL_OPTION;
		SttgsEvents->Module = this;
		SttgsEvents->ItemID = 4;
		SttgsEvents->Name = L"Events name";
		m_Node->ChildNodes.push_back(SttgsEvents);

		return true;
	}


	bool CModuleArchive::SetCurrent(sNode* Node)
	{
		m_CurrentNode = Node;
		if(m_CurrentNode->ItemID == 2)
		{
			SetChannelsView();
			return true;
		}

		if(m_CurrentNode->ItemID == 1)
		{
			SetUsersSettingsView();
		}

		if(m_CurrentNode->ItemID == 3)
		{
			SetChannelsSettingsView();
		}

		if(m_CurrentNode->ItemID == 4)
		{
			SetEventsView();
		}
		return false; 
	}

	bool CModuleArchive::SaveNode(sNode* /*Node*/)
	{
		return false;
	}


	bool CModuleArchive::ExpandNode(sNode* Node)
	{
		if(Node->ItemID != -1)
			return true;
		
		CEnterUserDlg dlg;
		dlg.m_User = "Administrator";
		if(dlg.DoModal() == IDOK)
		{
			IDBConnectionPtr Conn;
			HRESULT hr;
			try
			{
				hr = Conn.CreateInstance(__uuidof(DBConnection));
			}
			catch(_com_error err)
			{
				::Helpers::CErrorReporterPtr log;
				if(FAILED(hr))
					log->DecodeHRESULT(hr);
				else
					log->DecodeHRESULT(err.Error());
				return false;
			}
			if(FAILED(hr))
			{
				::Helpers::CErrorReporterPtr log;
				log->DecodeHRESULT(hr);
				return false;
			}
			m_User = dlg.m_User;
			m_Password = dlg.m_Password;
			_bstr_t Login = _bstr_t(m_User);
			_bstr_t Password = _bstr_t(m_Password);
			hr = Conn->raw_Open( Login, Password);

			if(hr == E_ACCESSDENIED)
			{
				MessageBox(NULL, L"Incorrect user name or password.", L"Input Error", MB_OK);
				return false;
			}
		
			if(FAILED(hr))
			{
				return false;
			}

			IUsersPtr Users;
			hr = Conn->raw_GetUsers(&Users);
			if(FAILED(hr))
			{
					return false;
			}

			try
			{
				BSTR First, Middle, Second;
				hr = Users->GetUserInfo(_bstr_t(m_User), &First, &Middle, &Second);
			}
			catch(_com_error err)
			{
				if(err.Error() == E_ACCESSDENIED)
				{
					MessageBox(NULL, L"You don't have rights to do that.",L"Access denied", MB_OK|MB_ICONSTOP);
					return false;
				} 
				::Helpers::CErrorReporterPtr log;
				log->ShowError(Users);
				return false;
			}

			if(FAILED(hr))
			{
				return false;
			}
			return true;
		}
		return false;
	}

	void CModuleArchive::SetChannelsView()
	{
		m_Channels.m_Computer = "localhost";
		m_Channels.DoDataExchange(DDX_LOAD);
		m_Channels.UpdateArchiveChannels();
	}

	void CModuleArchive::SetChannelsSettingsView()
	{
	
		m_ChannelSettings.UpdateChannels();
	}

	void CModuleArchive::SetUsersSettingsView()
	{
		m_UsersSettings.Reset();
		m_UsersSettings.FillUsers();
	}

	void CModuleArchive::SetEventsView()
	{
		m_Events.Update();
	}


	BOOL CModuleArchive::PreTranslateMessage(MSG* pMsg)
	{
		if(m_CurrentNode->ItemID == -1)
		{
			return m_DBSettings.PreTranslateMessage(pMsg);
		}
		if(m_CurrentNode->ItemID == 1)
		{
			return m_UsersSettings.PreTranslateMessage(pMsg);
		}
		if(m_CurrentNode->ItemID == 2)
		{
			return m_Channels.PreTranslateMessage(pMsg);
		}
		if(m_CurrentNode->ItemID == 4)
		{
			return m_Events.PreTranslateMessage(pMsg);
		}
		return FALSE;
	}
}
