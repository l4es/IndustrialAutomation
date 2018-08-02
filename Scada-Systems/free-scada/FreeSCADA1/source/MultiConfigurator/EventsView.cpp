#include "StdAfx.h"
#include ".\eventsview.h"
#include "ModuleArchive.h"
#include "ErrorReporter.h"

namespace ModuleArchive
{
	CEventsView::CEventsView(void)
	{
		m_Module = 0;
		m_Statics[0]		= "Channel changing";
		m_Statics[1]		= "Channel value is increased";
		m_Statics[2]		= "Channel value is decreased";
		m_Statics[3]		= "Value crossed maximum constant";
		m_Statics[4]		= "Channel goes to normal (max)";
		m_Statics[5]		= "Value crossed minimum constant";
		m_Statics[6]		= "Channel goes to normal (min)";
		for(int i=0;i<7;i++)
		{
			m_Events[i] = m_Statics[i];
			m_Statics[i] += ":";
		}
	}

	CEventsView::~CEventsView(void)
	{
	}

	BOOL CEventsView::PreTranslateMessage(MSG* pMsg)
	{
		return IsDialogMessage(pMsg);
	}

	LRESULT CEventsView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init(false, true, NULL);		
		DoDataExchange(DDX_LOAD);
		return 0;
	}
	void CEventsView::LoadFromDB()
	{
		ATLASSERT(m_Module != NULL);

		using namespace ::Helpers;

		IDBConnectionPtr Conn(__uuidof(DBConnection));

		HRESULT hr = Conn->raw_Open(_bstr_t(m_Module->m_User), _bstr_t(m_Module->m_Password));
		if(FAILED(hr))
		{
			MessageBox(L"Error during DB connection.", L"Error", MB_OK|MB_ICONSTOP);
			return;
		}

		IEventTypesPtr events;
		try
		{
			events = Conn->GetEventTypes();
		}
		catch(...)
		{
			CErrorReporterPtr err;
			err->ShowError(Conn);
		}

		try
		{
			for(ULONG ID=0;ID<7;ID++)
				m_Events[ID] = (BSTR)events->GetEventTypeName(ID);
		}
		catch(...)
		{
			CErrorReporterPtr err;
			err->ShowError(Conn);
		}
		events = NULL;
		Conn = NULL;
	}

	void CEventsView::SaveToDB(BOOL Silent)
	{
		ATLASSERT(m_Module != NULL);

		using namespace ::Helpers;

		IDBConnectionPtr Conn;
		HRESULT hr;
		hr = Conn.CreateInstance(__uuidof(DBConnection));
		if(FAILED(hr))
		{
			if(!Silent)
				MessageBox(L"Error during DB connection.", L"Error", MB_OK|MB_ICONSTOP);
			return;
		}

		hr = Conn->raw_Open(_bstr_t(m_Module->m_User), _bstr_t(m_Module->m_Password));
		if(FAILED(hr))
		{
			if(!Silent)
				MessageBox(L"Error during DB connection.", L"Error", MB_OK|MB_ICONSTOP);
			return;
		}

		IEventTypesPtr events;
		try
		{
			events = Conn->GetEventTypes();
		}
		catch(...)
		{
			CErrorReporterPtr err;
			err->ShowError(Conn);
		}

		try
		{
			for(ULONG ID=0;ID<7;ID++)
				events->EventTypeName[ID] = (BSTR)CComBSTR(m_Events[ID]);
		}
		catch(...)
		{
			if(!Silent)
			{
				CErrorReporterPtr err;
                err->ShowError(Conn);
			}
		}
		events = NULL;
		Conn = NULL;
	}

	void CEventsView::Update()
	{
		LoadFromDB();
		DoDataExchange(DDX_LOAD);
	}

	LRESULT CEventsView::OnShowWindow(BOOL fShow, UINT /*status*/)
	{
		SetMsgHandled(FALSE);
		if(fShow == FALSE)
		{
			DoDataExchange(DDX_SAVE);
			SaveToDB();
		}
		return 0;
	}
	LRESULT CEventsView::OnDestroy(void)
	{
		SetMsgHandled(FALSE);
		DoDataExchange(DDX_SAVE);
		SaveToDB(TRUE);
		return 0;
	}
}