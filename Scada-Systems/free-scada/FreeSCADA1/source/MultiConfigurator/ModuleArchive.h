#pragma once
#include "BaseModule.h"
#include "DBSettingsView.h"
#include "ChannelsView.h"
#include "UsersSettingsView.h"
#include "ChannelSettingsView.h"
#include "EventsView.h"

namespace ModuleArchive
{
	class CModuleArchive 
		:	public CBaseModule
	{
	public:
		CModuleArchive()
		{
			//_SupportedOptions=FO_LOAD;
		};
		
		CDBSettingsView			m_DBSettings;
		CChannelsView			m_Channels;
		CUsersSettingsView		m_UsersSettings;
		CChannelSettingsView	m_ChannelSettings;
		CEventsView				m_Events;

		CString m_User;
		CString m_Password;

		virtual bool Load(HWND hParent, HWND hMainWnd,  CRect wndRect);
		virtual bool LoadNodes(sNode* Parent);
		virtual bool Detect();
		virtual HWND GetWndHandle();
		virtual bool SetCurrent(sNode* Node);
		virtual bool ExpandNode(sNode* Node);
		virtual bool SaveNode(sNode* Node);
		virtual BOOL PreTranslateMessage(MSG* pMsg);

		void SetChannelsView();
		void SetChannelsSettingsView();
		void SetUsersSettingsView();
		void SetEventsView();

	};

}
