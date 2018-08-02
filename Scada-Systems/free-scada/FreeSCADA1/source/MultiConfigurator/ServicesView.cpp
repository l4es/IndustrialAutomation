#include "StdAfx.h"
#include ".\servicesview.h"
#include "ModuleServices.h"
#include "../Common/versions.h"


#define SERVICE_TIMEOUT		15

namespace ModuleServices
{
	CServicesView::CServicesView()
		: m_Module(NULL)
	{
		RescanServices();
	}

	//Запуск службы архиватора
	LRESULT CServicesView::OnStartArchive( WORD /*wNotifyCode*/,  WORD /*wID*/,  HWND /*hWndCtl*/,  BOOL& /*bHandled*/ )
	{
		if(Start(S_ARCHIVE))
			m_ArchiveRun = true;
		Update();
		return 0;
	}

	//Остановка службы архиватора
	LRESULT CServicesView::OnStopArchive( WORD /*wNotifyCode*/,  WORD /*wID*/,  HWND /*hWndCtl*/,  BOOL& /*bHandled*/ )
	{
		if(Stop(S_ARCHIVE))
			m_ArchiveRun = false;
		Update();
		return 0;
	}

	//Перезапуск службы архиватора
	LRESULT CServicesView::OnRestartArchive( WORD /*wNotifyCode*/,  WORD /*wID*/,  HWND /*hWndCtl*/,  BOOL& /*bHandled*/ )
	{
		if(Stop(S_ARCHIVE))
			m_ArchiveRun = false;
		Update();
		if(Start(S_ARCHIVE))
			m_ArchiveRun = true;
		Update();
		return 0;
	}
    
	//Активация и деактивация кнопок запука, остановки и перезапуска служб
	void CServicesView::Update()
	{
		((CWindow)GetDlgItem(IDC_START_ARCHIVE)).EnableWindow(!m_ArchiveRun);
		((CWindow)GetDlgItem(IDC_STOP_ARCHIVE)).EnableWindow(m_ArchiveRun);
		((CWindow)GetDlgItem(IDC_RESTART_ARCHIVE)).EnableWindow(m_ArchiveRun);
	}

	//Остановка службы
	bool CServicesView::Stop(enService ServType)
	{
		//Определяем название службы, 
		//которое отображается в диалоге управления службами (системном)
		wstring ServiceDisplayName;

		switch(ServType) 
		{
		case S_ARCHIVE:	ServiceDisplayName = ARCHIVER_NAME;			break;

		}

		if(ServiceDisplayName.empty())
			return false;
		
		//Получаем дескриптор менеджера служб
		SC_HANDLE hManager = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE,  SC_MANAGER_CONNECT );
		if(hManager == NULL)
			return false;

		wstring ServiceName;
		ServiceName.resize(0xff);
		DWORD size = 0xff;

		//Получаем имя службы по названию, которое
		//отображается в диалоге управления службами (системном)
		if(!GetServiceKeyName(hManager, ServiceDisplayName.c_str(), &ServiceName[0], &size))
			return false;

		//Открываем службу
		SC_HANDLE hService = OpenService(hManager, ServiceName.c_str(), 
			SERVICE_QUERY_STATUS | SERVICE_STOP);
	
		if(hService == NULL)
		{
			CloseServiceHandle(hManager);
			return false;
		}

		//Запрашиваем состояние службы
		SERVICE_STATUS ServiceStatus;
		if(!QueryServiceStatus(hService, &ServiceStatus))
		{
			CloseServiceHandle(hService);
			CloseServiceHandle(hManager);
			return false;
		}
		//Если служба остановлена - возвращаем true
		if(ServiceStatus.dwCurrentState == SERVICE_STOPPED)
			return true;

		//Пытаемся остановить службу
		if(!ControlService( hService, SERVICE_CONTROL_STOP, &ServiceStatus))
		{
			CloseServiceHandle(hService);
			CloseServiceHandle(hManager);
			return false;
		}


		CWaitCursor Wait;
		HelpersLib::CTimer timer;
		timer.Reset();
		while(timer < SERVICE_TIMEOUT)
		{
			//Запрашиваем состояние службы через каждые 0.1 с
			if(!QueryServiceStatus(hService, &ServiceStatus))
			{
				CloseServiceHandle(hService);
				CloseServiceHandle(hManager);
				return false;
			}
			//Если служба остановлена - выходим из цикла 
			if(ServiceStatus.dwCurrentState == SERVICE_STOPPED)
				break;
			Sleep(100);
		}

		//Закрываем службу и менеджер служб
		CloseServiceHandle(hService);
		CloseServiceHandle(hManager);

		//Если служба остановлена - возвращаем true
		if(ServiceStatus.dwCurrentState == SERVICE_STOPPED)
			return true;


		return false;
	}


	//Запуск службы
	bool CServicesView::Start(enService ServType)
	{
		wstring ServiceDisplayName;
		//Определяем название службы, 
		//которое отображается в диалоге управления службами (системном)
		switch(ServType) 
		{
		case S_ARCHIVE:	ServiceDisplayName = ARCHIVER_NAME;			break;
		}

		if(ServiceDisplayName.empty())
			return false;
		//Получаем дескриптор менеджера служб
		SC_HANDLE hManager = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE,  SC_MANAGER_CONNECT );
		if(hManager == NULL)
			return false;

		wstring ServiceName;
		ServiceName.resize(0xff);
		DWORD size = 0xff;

		//Получаем имя службы по названию, которое
		//отображается в диалоге управления службами (системном)
		if(!GetServiceKeyName(hManager, ServiceDisplayName.c_str(), &ServiceName[0], &size))
			return false;

		//Открываем службу
		SC_HANDLE hService = OpenService(hManager, ServiceName.c_str(), 
			SERVICE_QUERY_STATUS | SERVICE_START);

		if(hService == NULL)
		{
			CloseServiceHandle(hManager);
			return false;
		}
		//Запрашиваем состояние службы
		SERVICE_STATUS ServiceStatus;
		if(!QueryServiceStatus(hService, &ServiceStatus))
		{
			CloseServiceHandle(hService);
			CloseServiceHandle(hManager);
			return false;
		}
		//Если служба запущена - возвращаем true;
		if(ServiceStatus.dwCurrentState == SERVICE_RUNNING)
			return true;

		//Пытаемся запустить слузбу
		if (!StartService (hService, 0, NULL))
		{
			CloseServiceHandle(hManager);
			return false;
		}

		CWaitCursor Wait;
		
		HelpersLib::CTimer timer;
		timer.Reset();
		while(timer < SERVICE_TIMEOUT)
		{
			//Запрашиваем состояние службы каждые 0.1 с пока не запустися служба или
			//не истечет время
			if(!QueryServiceStatus(hService, &ServiceStatus))
			{
				CloseServiceHandle(hService);
				CloseServiceHandle(hManager);
				return false;
			}
			if(ServiceStatus.dwCurrentState == SERVICE_RUNNING)
				break;
			Sleep(100);
		}
		//Закрываем службу и менеджер служб
		CloseServiceHandle(hService);
		CloseServiceHandle(hManager);

		if(ServiceStatus.dwCurrentState == SERVICE_RUNNING)
			return true;



		return false;
	}

	DWORD CServicesView::GetServiceState(enService ServType)
	{
		wstring ServiceDisplayName;
		switch(ServType) 
		{
		case S_ARCHIVE:	ServiceDisplayName = ARCHIVER_NAME;			break;
		}

		if(ServiceDisplayName.empty())
			return NULL;

		//Получаем дескриптор менеджера служб
		SC_HANDLE hManager = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE,  SC_MANAGER_CONNECT );
		if(hManager == NULL)
			return NULL;

		wstring ServiceName;
		ServiceName.resize(0xff);
		DWORD size = 0xff;

		//Получаем имя службы по названию, которое
		//отображается в диалоге управления службами (системном)
		if(!GetServiceKeyName(hManager, ServiceDisplayName.c_str(), &ServiceName[0], &size))
			return false;

		//Открываем службу
		SC_HANDLE hService = OpenService(hManager, ServiceName.c_str(), 
			SERVICE_QUERY_STATUS | SERVICE_STOP);

		if(hService == NULL)
		{
			CloseServiceHandle(hManager);
			return false;
		}

		//Запрашиваем состояние службы
		SERVICE_STATUS ServiceStatus;
		if(!QueryServiceStatus(hService, &ServiceStatus))
		{
			CloseServiceHandle(hService);
			CloseServiceHandle(hManager);
			return NULL;
		}
		CloseServiceHandle(hService);
		CloseServiceHandle(hManager);

		return ServiceStatus.dwCurrentState;
	}

	void CServicesView::RescanServices()
	{
		m_ArchiveRun	= GetServiceState(S_ARCHIVE) == SERVICE_RUNNING?true:false;
	}
}
