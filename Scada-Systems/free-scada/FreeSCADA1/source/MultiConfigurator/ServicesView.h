#pragma once
#include <atlcrack.h>

namespace ModuleServices
{

	enum enService {S_ARCHIVE};
	class CModuleServices;
	
	//Вид для запуска, остановки и перезапуска служб
	class CServicesView
		: public CDialogImpl<CServicesView>
		, public CDialogResize<CServicesView>
		, public CWinDataExchange<CServicesView>
	{
	public:
		enum { IDD = IDD_SERVICES };

		bool m_ArchiveRun; //Запущена ли служба архиватора


		CModuleServices* m_Module; //Указатель на модуль

		CServicesView();

		BEGIN_DDX_MAP(CServicesView)

		END_DDX_MAP()

		BEGIN_MSG_MAP(CServicesView)
			COMMAND_ID_HANDLER( IDC_START_ARCHIVE, OnStartArchive)
			COMMAND_ID_HANDLER( IDC_RESTART_ARCHIVE, OnRestartArchive)
			COMMAND_ID_HANDLER( IDC_STOP_ARCHIVE, OnStopArchive)			

			CHAIN_MSG_MAP(CDialogResize<CServicesView>)
		END_MSG_MAP()


		BEGIN_DLGRESIZE_MAP(CServicesView)
		END_DLGRESIZE_MAP()

		//Запуск, остановка и перезапуск службы архиватора
		LRESULT OnStartArchive( WORD wNotifyCode,  WORD wID,  HWND hWndCtl,  BOOL& bHandled );
		LRESULT OnStopArchive( WORD wNotifyCode,  WORD wID,  HWND hWndCtl,  BOOL& bHandled );
		LRESULT OnRestartArchive( WORD wNotifyCode,  WORD wID,  HWND hWndCtl,  BOOL& bHandled );

		//Активация / деактивация кнопок управления служб
		void Update();
		//Запуск службы
		bool Start(enService ServType);
		//Остановка службы
		bool Stop(enService ServType);
		//получить состояние службы
		DWORD GetServiceState(enService ServType);

		void RescanServices();
	};

}
