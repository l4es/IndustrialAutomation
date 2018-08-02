#pragma once
#include <map>
#include "DataCallback.h"

using namespace std;

//Множитель для "следящего" таймера
#define WATCHDOG_TIMEOUT_MUL	30

class CConnections
{
public:
	CConnections(void);
	~CConnections(void);
	BOOL Start(void);		//Установить все соединения
	void Stop(void);		//Остановить все соединения
	void CheckItemTimers();	//Проверить таймеры у каналов. Если таймер просрочен выполняется переподключение

protected:
	friend struct fCompareConnection;
	friend class CDataCallback;

	//Структура описывающая подключение
	struct sConnection
	{
		CLSID	ServerCLSID;		//CLSID сервера
		CString	Computer;			//Компьютер
		ULONG	UpdateInterval;		//Интервал обновления группы

		CComPtr<IOPCItemMgt>	OPCGroup;	//Группа на OPC сервере
		CComPtr<IOPCServer>		OPCServer;	//OPC сервер
		OPCHANDLE				hGroup;		//Handle группы
		DWORD					Advise;		//Код для Connection Point

		map<DWORD /*ItemID*/,OPCHANDLE /*hServerItem*/>	Items;		//Карта связей ItemID-hServerItem
		map<DWORD /*ItemID*/,DWORD /*Ticks*/>			ItemTimers;	//WatchDog таймеры для элментов
		
		//Сбросить таймер (если таймер будет "прострочен" произойдет переподключение к OPC)
		inline void ResetTimer(DWORD ItemID){ItemTimers[ItemID] = GetTickCount();}

		//Проверить не просрочен ли таймер?
		inline bool IsTimeout(DWORD ItemID)
		{
			if(ItemTimers.find(ItemID) == ItemTimers.end())
				return false;

			if((GetTickCount() - ItemTimers[ItemID]) > UpdateInterval*WATCHDOG_TIMEOUT_MUL)
			{
				//ATLTRACE("timeout: tagid=%d time=%u\n",ItemID,GetTickCount() - ItemTimers[ItemID]);
				return true;
			}
			else
				return false;
		}
	};

	CDataCallback* _Callback;	//Callback интерфейс для приема данных
	
	typedef vector<sConnection> TConnections;
	TConnections _Connections;				//Все соединения с OPC серверами

	//Получить указатель на указанное соединение. Если такого соединения нет, оно устанавливается.
	sConnection* GetConnection(CLSID ServerCLSID, CString Computer, ULONG UpdateInterval);

	//Сбросить таймер у указанного ItemID во всех соединениях
	void UpdateItemTimer(DWORD ItemID);

	//Переподключить все каналы у указанного соединения (изменяет _Connections !!!)
	void ReconnectAllItemsOnConnection(sConnection Connection);
};
