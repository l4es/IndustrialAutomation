#include "StdAfx.h"
#include ".\connections.h"
#include "DataLayer.h"
#include "EventLogger.h"
#include <vector>

#define LCID_RUSSIAN	MAKELCID(MAKELANGID(LANG_RUSSIAN, SUBLANG_NEUTRAL),SORT_DEFAULT)

//"функтор" для сравнения соединений
struct fCompareConnection
{
	CLSID	_ServerCLSID;
	CString	_Computer;
	ULONG	_UpdateInterval;

	fCompareConnection(CLSID ServerCLSID, CString Computer, ULONG UpdateInterval)
		: _ServerCLSID(ServerCLSID)
		, _Computer(Computer)
		, _UpdateInterval(UpdateInterval){};

	bool operator()(const CConnections::sConnection& conn)
	{
		return (	IsEqualCLSID(_ServerCLSID, conn.ServerCLSID) &&
			StrCmpI(_Computer, conn.Computer) == 0 &&
			_UpdateInterval == conn.UpdateInterval);
	}
};


CConnections::CConnections(void)
{
}

CConnections::~CConnections(void)
{
}

BOOL CConnections::Start(void)
{
	//Копируем список каналов в локальный поток (обработчики данных не копируются)
	CSingletonPtr<CDataLayer> DataLayer;
	TChannelsMap Channels;
	DataLayer->CopyChannels(Channels);

	//Создать объект для приема дынных с OPC
	_Callback = new CDataCallback(this);

	//Просматриваем все каналы
	for(TChannelsMap::iterator i=Channels.begin();i!=Channels.end();i++)
	{
		{
			CSingletonPtr<CEventLogger> log;
			CString msg;
			msg.Format(L"Подключение канала '%s'",i->second.Name);
			log->LogMessage(msg);
		}
		//Получить указатель на соединение
		sConnection* Connection = GetConnection(i->second.ServerCLSID, i->second.Computer, i->second.UpdateInterval);
		if(Connection)
		{
			ATLASSERT(Connection->OPCGroup != NULL);

			OPCITEMDEF		item;
			OPCITEMRESULT	*results	= NULL;
			HRESULT			*Errors		= NULL;
			HRESULT			hr;

			ZeroMemory(&item, sizeof(OPCITEMDEF));
			item.bActive	= TRUE;
			item.hClient	= i->first;
			item.szItemID	= CComBSTR(i->second.Name);

			//Поставить канал на обновление
			hr = Connection->OPCGroup->AddItems(1, &item, &results, &Errors);
			if(hr != S_OK)
			{
				//Обработка ошибки
				CSingletonPtr<CEventLogger> log;
				switch(hr) //Описание ошибки "в общем"
				{
				case E_FAIL:		log->GenMsg(ERR_OPCCONNECT_FAIL);break;
				case E_OUTOFMEMORY:	log->GenMsg(ERR_OPCCONNECT_OUTOFMEMORY);break;
				case E_INVALIDARG:	log->GenMsg(ERR_OPCCONNECT_INVALIDARG);break;
				default:
					if(FAILED(hr))
						log->GenMsg(ERR_OPCCONNECT_FAIL_UNKN);
				}
				if(Errors) //Более подробное описание
				{
					switch(Errors[0])
					{
					case OPC_E_INVALIDITEMID:	log->GenMsg(ERR_OPCCONNECT_INVALIDITEMID);break;
					case OPC_E_UNKNOWNITEMID:	log->GenMsg(ERR_OPCCONNECT_UNKNOWNITEMID);break;
					}
				}
			}
			else
			{
				//Добавляем канал в карту подключеных каналов и сбрасываем таймер
				Connection->Items[i->first] = results[0].hServer;
				Connection->ResetTimer(i->first);
			}
			//Освобождаем память
			LocalFree(results);
			LocalFree(Errors);
		}
	}
	return TRUE; //Все соединения успешно установленны
}

void CConnections::Stop(void)
{
	//Просматриваем все соединения
	for(TConnections::iterator connection=_Connections.begin();connection!=_Connections.end();connection++)
	{
		ATLASSERT(connection->OPCGroup != NULL);
	
		//составляем список hServerItems (для отключения)
		vector<OPCHANDLE> hServerItems;
		typedef map<DWORD,OPCHANDLE>::const_iterator it;
		for(it tag=connection->Items.begin();tag!=connection->Items.end();tag++)
		{	
			//Если такого handle нет в списке - добавляем
			if(find(hServerItems.begin(), hServerItems.end(), tag->second) == hServerItems.end())
				hServerItems.push_back(tag->second);
		}

		//Отключаем все каналы
		HRESULT *results;
		TEST_HR(connection->OPCGroup->RemoveItems(static_cast<DWORD>(hServerItems.size()), &hServerItems[0], &results));
		LocalFree(results);

		//"отписываемся" от рассылки данных
		TEST_HR(AtlUnadvise(connection->OPCGroup, __uuidof(IOPCDataCallback), connection->Advise));
		//удаляем OPC группу
		connection->OPCServer->RemoveGroup(connection->hGroup, FALSE);

		//Освобождаем интерфейсы
		connection->OPCGroup	= NULL;
		connection->OPCServer	= NULL;
	}
	//Очищаем массив соединений
	_Connections.clear();

	//Удаляем объект OPCDataCallback 
	_Callback->Release();
	_Callback = NULL;
}

CConnections::sConnection* CConnections::GetConnection(CLSID ServerCLSID, CString Computer, ULONG UpdateInterval)
{
	//Ищем запрошеное подключение среди уже открытых
	TConnections::iterator it = find_if(_Connections.begin(),
										_Connections.end(),
										fCompareConnection(ServerCLSID,Computer,UpdateInterval));
	if(it != _Connections.end())
		return &*it;	//Возвращаем подключение

	CEventLoggerPtr log;
	CString msg;
	msg.Format(L"Устанавливается соединение с OPC сервером: Computer='%s', UpdateInterval=%uмс",Computer,UpdateInterval);
	log->LogMessage(msg);

	//Такого соединения нет. Создаем новое
	sConnection Connection;
	memcpy(&Connection.ServerCLSID, &ServerCLSID, sizeof(CLSID));
	Connection.Computer			= Computer;
	Connection.UpdateInterval	= UpdateInterval;

	//Запрашиваем интерфейс IOPCServer у указанного сервера
	COSERVERINFO	si;
	MULTI_QI		qi;
	ZeroMemory(&si,sizeof(COSERVERINFO));
	ZeroMemory(&qi,sizeof(MULTI_QI));
	si.pwszName = CComBSTR(Connection.Computer);
	qi.pIID	= &__uuidof(IOPCServer);
	HRESULT hr;
	hr = CoCreateInstanceEx(Connection.ServerCLSID, NULL, CLSCTX_ALL, &si, 1, &qi);
	if(!TEST_HR(hr) || !TEST_HR(qi.hr))
	{
		log->GenMsg(ERR_OPCCONNECT_CANT_CREATE_INSTANCE);
		return NULL;
	}
	hr = qi.pItf->QueryInterface(__uuidof(IOPCServer), (void**)&Connection.OPCServer);
	qi.pItf->Release();
	if(!TEST_HR(hr))
	{
		log->GenMsg(ERR_OPCCONNECT_CANT_QUERY_IOPCSERVER);
		return NULL;
	}

	//Добавляем группу на сервере и запрашиваем ее интерфейс
	LONG TimeBias			= NULL;
	FLOAT PercentDeadBand	= NULL;
	GUID riid				= IID_IUnknown;
	CComPtr<IUnknown>		pUnk;
	
	hr = Connection.OPCServer->AddGroup(	CComBSTR(""), TRUE, UpdateInterval,
							0, &TimeBias, &PercentDeadBand, LCID_RUSSIAN,
							&Connection.hGroup, &Connection.UpdateInterval,
							riid, &pUnk);
	if(!TEST_HR(hr))
	{
		log->GenMsg(ERR_OPCCONNECT_CANT_ADD_GROUP);
		return NULL;
	}
	if(!TEST_HR(pUnk.QueryInterface(&Connection.OPCGroup)))
	{
		log->GenMsg(ERR_OPCCONNECT_CANT_QUERY_IOPCITEMMGT);
		return NULL;
	}
	if(Connection.UpdateInterval != UpdateInterval)
	{
		log->GenMsg(ERR_OPCCONNECT_TOO_FAST);
		UpdateInterval = Connection.UpdateInterval;
	}

	//"Подписываемся" на рассылку данных
	hr = AtlAdvise(	Connection.OPCGroup, _Callback, IID_IOPCDataCallback, &Connection.Advise);
	if(!TEST_HR(hr))
	{
		CSingletonPtr<CEventLogger> log;
		if(hr == E_ACCESSDENIED)
			log->GenMsg(ERR_OPCCONNECT_ACCESSDENIED);
		else
			log->GenMsg(ERR_OPCCONNECT_CANT_SUBSCRIBE_CALLBACK);
		return NULL;
	}

	//Добавляем вновь установленное соединение в массив соединений
	_Connections.push_back(Connection);

	it = find_if(_Connections.begin(),
				_Connections.end(),
				fCompareConnection(ServerCLSID,Computer,UpdateInterval));
	ATLASSERT(it != _Connections.end());
	return &*it;	//Возвращаем подключение
}

void CConnections::UpdateItemTimer(DWORD ItemID)
{
	//сбрасываем таймер у указанного канала
	for(TConnections::iterator i=_Connections.begin();i!=_Connections.end();i++)
	{
		if(i->Items.find(ItemID) != i->Items.end())
			i->ResetTimer(ItemID);
	}
}

void CConnections::CheckItemTimers()
{
	//Просматриваем все соединения
	typedef map<DWORD,OPCHANDLE>::iterator it;
	for(TConnections::iterator connection=_Connections.begin();connection!=_Connections.end();connection++)
	{
		//просматриваем все подключенные каналы
		for(it tag=connection->Items.begin();tag!=connection->Items.end();tag++)
		{
			//Не просрочен ли таймер?
			if(connection->IsTimeout(tag->first))
			{	//Таймер просрочен. Переподключемся.
				ATLASSERT(connection->OPCGroup != NULL);

				CSingletonPtr<CEventLogger> log;
				log->GenMsg(ERR_OPCCONNECT_LOST_CONNECTION);

				//Получить полное описание текущего канала
				CSingletonPtr<CDataLayer> DataLayer;
				sChannel Channel;
				if(DataLayer->GetChannelByItemID(tag->first, Channel) == FALSE)
				{
					
					log->GenMsg(ERR_OPCCONNECT_FAIL);
					continue;
				}

				//Переподключаем канал
				OPCITEMDEF		item;
				OPCITEMRESULT	*results	= NULL;
				HRESULT			*Errors		= NULL;

				ZeroMemory(&item, sizeof(OPCITEMDEF));
				item.bActive	= TRUE;
				item.hClient	= tag->first;
				item.szItemID	= CComBSTR(Channel.Name);

				HRESULT hr;
				hr = connection->OPCGroup->AddItems(1, &item, &results, &Errors);
				if(hr != S_OK)
				{
					if(hr == 0x800706ba)
					{
						//Сервер RPC недоступен (нужно устанавливать новое соединение)
						ReconnectAllItemsOnConnection(*connection);
						return;
					}

					if(hr != S_OK)
					{
						CSingletonPtr<CEventLogger> log;
						switch(hr)
						{
						case E_FAIL:		log->GenMsg(ERR_OPCCONNECT_FAIL);break;
						case E_OUTOFMEMORY:	log->GenMsg(ERR_OPCCONNECT_OUTOFMEMORY);break;
						case E_INVALIDARG:	log->GenMsg(ERR_OPCCONNECT_INVALIDARG);break;
						default:
							if(FAILED(hr))
								log->GenMsg(ERR_OPCCONNECT_FAIL_UNKN);
						}
						if(Errors)
						{
							switch(Errors[0])
							{
							case OPC_E_INVALIDITEMID:	log->GenMsg(ERR_OPCCONNECT_INVALIDITEMID);break;
							case OPC_E_UNKNOWNITEMID:	log->GenMsg(ERR_OPCCONNECT_UNKNOWNITEMID);break;
							}
						}
					}
				}
				else
				{
					//Обновляем handle канала в карте подключений
					connection->Items[tag->first] = results[0].hServer;
					//Сбрасываем таймер
					connection->ResetTimer(tag->first);
				}
				LocalFree(results);
				LocalFree(Errors);
			}
		}
	}
}

void CConnections::ReconnectAllItemsOnConnection(sConnection OldConnection)
{
	//Строим список подключенных каналов (их все нужно подключить заново)
	vector<DWORD> ItemIDs;
	for(map<DWORD,OPCHANDLE>::const_iterator i=OldConnection.Items.begin();i!=OldConnection.Items.end();i++)
		ItemIDs.push_back(i->first);

	//Ищем указанное соединение в списке соединений и удаляем его
	TConnections::iterator it = find_if(_Connections.begin(),
										_Connections.end(), 
										fCompareConnection(OldConnection.ServerCLSID,OldConnection.Computer,OldConnection.UpdateInterval));
	ATLASSERT(it != _Connections.end());
	//"отписываемся" от рассылки данных
	TEST_HR(AtlUnadvise(it->OPCGroup, __uuidof(IOPCDataCallback), it->Advise));
	//удаляем OPC группу
	it->OPCServer->RemoveGroup(it->hGroup, FALSE);
	//Освобождаем интерфейсы
	it->OPCGroup	= NULL;
	it->OPCServer	= NULL;
	_Connections.erase(it);

	//просматриваем все каналы
	for(vector<DWORD>::iterator ItemID=ItemIDs.begin();ItemID!=ItemIDs.end();ItemID++)
	{
		//Получить подробную информацию о канале
		CSingletonPtr<CDataLayer> DataLayer;
		sChannel Channel;
		DataLayer->GetChannelByItemID(*ItemID, Channel);

		//Получить/установить новое соединение
		sConnection* Connection = GetConnection(OldConnection.ServerCLSID, OldConnection.Computer, OldConnection.UpdateInterval);
		if(Connection)
		{
			ATLASSERT(Connection->OPCGroup != NULL);

			//Ставим канал на обновление
			OPCITEMDEF		item;
			OPCITEMRESULT	*results	= NULL;
			HRESULT			*Errors		= NULL;
			
			ZeroMemory(&item, sizeof(OPCITEMDEF));
			item.bActive	= TRUE;
			item.hClient	= *ItemID;
			item.szItemID	= CComBSTR(Channel.Name);

			HRESULT hr;
			hr = Connection->OPCGroup->AddItems(1, &item, &results, &Errors);
			if(hr != S_OK)
			{
				//Обработка ошибок
				CSingletonPtr<CEventLogger> log;
				switch(hr)
				{
				case E_FAIL:		log->GenMsg(ERR_OPCCONNECT_FAIL);break;
				case E_OUTOFMEMORY:	log->GenMsg(ERR_OPCCONNECT_OUTOFMEMORY);break;
				case E_INVALIDARG:	log->GenMsg(ERR_OPCCONNECT_INVALIDARG);break;
				default:
					if(FAILED(hr))
						log->GenMsg(ERR_OPCCONNECT_FAIL_UNKN);
				}
				if(Errors)
				{
					switch(Errors[0])
					{
					case OPC_E_INVALIDITEMID:	log->GenMsg(ERR_OPCCONNECT_INVALIDITEMID);break;
					case OPC_E_UNKNOWNITEMID:	log->GenMsg(ERR_OPCCONNECT_UNKNOWNITEMID);break;
					}
				}
			}
			else
			{
				//Добавляем канал в карту подключеных каналов и сбрасываем таймер
				Connection->Items[*ItemID] = results[0].hServer;
				Connection->ResetTimer(*ItemID);
			}
			LocalFree(results);
			LocalFree(Errors);
		}
	}
}