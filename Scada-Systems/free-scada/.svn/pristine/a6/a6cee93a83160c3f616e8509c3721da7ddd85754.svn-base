#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   13:51
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Visualizator\ArrayOfOPCConnection.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Visualizator
	Имя файла:	    ArrayOfOPCConnection
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CArrayOfOPCConnection
	
Описание:	Класс для управления соединениями с OPC серверами.
Основная идея, это использование одного подключения (группы) для множества
каналов. Для разных серверов создаются разные группы.
*********************************************************************/

#include "OPCConnection.h"
#include "OPCDataCallback.h"


class CArrayOfOPCConnection
{
protected:
	struct sServerID
	{
		COPCConnection		Connection;
		DWORD				Advise;
		vector<OPCHANDLE>	ServerTagHandles;

		//Сравнение серверов по имени компьютера и CLSID
		struct fCompareServerID
		{
			CLSID clsid;
			wstring computer;
			fCompareServerID(wstring _computer, CLSID _clsid)
			{
				computer = _computer;
				memcpy(&clsid, &_clsid, sizeof(CLSID));
			}
			bool operator()(sServerID* val)
			{
				return ((IsEqualCLSID(val->Connection.GetCLSID(), clsid)) &&
						(computer == val->Connection.GetComputer()));
			}
		};
	};

	typedef vector<sServerID*> TVConnections;
	TVConnections		_Connections;
	COPCDataCallback*	_Callback;
	vector<OPCHANDLE>	_ClientTagHandles;

public:
	CArrayOfOPCConnection();
	~CArrayOfOPCConnection(void);

	//Подключить канал на обновление
	BOOL Connect(wstring Computer, CLSID Clsid, wstring Tag, OPCHANDLE hTag, CDoc* pDoc, OPCHANDLE *hServerTag = NULL);
	//Отключить канал.
	void DisconnectAll();

	void RefreshChannels();

	//Получить интерфейс группы указанного сервера
	CComPtr<IOPCGroupStateMgt> GetServerGroup(wstring Computer, CLSID Clsid);

	//Генерировать уникальный Handle для канала (клиентский)
	OPCHANDLE GenerateUniqueHandle();
	bool TestServers();
	//Получить указатель на Callback объект
	inline COPCDataCallback* GetCallbackPtr(){return _Callback;};
};
