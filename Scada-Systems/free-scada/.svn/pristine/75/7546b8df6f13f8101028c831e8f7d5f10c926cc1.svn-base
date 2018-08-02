#include "StdAfx.h"
#include ".\arrayofopcconnection.h"

CArrayOfOPCConnection::CArrayOfOPCConnection()
{
	//Создать инстанцию Callback объекта
	_Callback = new COPCDataCallback();
}

CArrayOfOPCConnection::~CArrayOfOPCConnection(void)
{
	//Отключить все каналы
	DisconnectAll();

	//Освободить объект Callback
	_Callback->Release();
	_Callback = NULL;
}

BOOL CArrayOfOPCConnection::Connect(wstring Computer, CLSID Clsid, wstring Tag, OPCHANDLE hTag, CDoc* pDoc, OPCHANDLE *hServerTag)
{
	ATLASSERT(!Computer.empty());
	ATLASSERT(!IsEqualCLSID(Clsid, GUID_NULL));
	ATLASSERT(!Tag.empty());
	ATLASSERT(hTag != NULL);
	ATLASSERT(pDoc != NULL);

	ATLASSERT(_Callback->m_Doc == pDoc || _Callback->m_Doc == NULL);
	_Callback->m_Doc = pDoc;

	typedef TVConnections::iterator it;
	OPCHANDLE tmp;

	//Ищем. Неустановлено ли уже подобное соединение с сервером?
	it item = find_if(_Connections.begin(), _Connections.end(), sServerID::fCompareServerID(Computer, Clsid));
	if(item == _Connections.end())
	{
		ATLTRACE("Устанавливается новое соединение\n");
		//Соединение не установлено. Создаем его.
		sServerID* conn = new sServerID();
		BOOL res = conn->Connection.Connect(Computer, Clsid, _Callback, pDoc);
		if(!res)
		{
			delete conn;
			return FALSE;
		}

		//Добавить канал
		if(conn->Connection.AddTag(Tag, hTag, &tmp) != TRUE)
		{
			conn->Connection.Disconnect();
			delete conn;
			return FALSE;
		}
		conn->ServerTagHandles.push_back(tmp);
		if(hServerTag != NULL)
			*hServerTag = tmp;

		_Connections.push_back(conn);
		_ClientTagHandles.push_back(hTag);
	}
	else
	{
		ATLTRACE("Используется старое соединение\n");
		//Соединение установлено.
		sServerID* conn = *item;

		//Добавить канал
		if(conn->Connection.AddTag(Tag, hTag, &tmp) != TRUE)
			return FALSE;
		conn->ServerTagHandles.push_back(tmp);

		if(hServerTag != NULL)
			*hServerTag = tmp;

		_ClientTagHandles.push_back(hTag);
	}
	return TRUE;
}

CComPtr<IOPCGroupStateMgt> CArrayOfOPCConnection::GetServerGroup(wstring Computer, CLSID Clsid)
{
	ATLASSERT(!Computer.empty());
	ATLASSERT(!IsEqualCLSID(Clsid, GUID_NULL));

	typedef TVConnections::iterator it;
	CComPtr<IOPCGroupStateMgt> group;

	//Ищем. Неустановлено ли уже подобное соединение с сервером?
	it item = find_if(_Connections.begin(), _Connections.end(), sServerID::fCompareServerID(Computer, Clsid));
	if(item != _Connections.end())
	{
		//Соединение установлено.
		sServerID* conn = *item;
		group = conn->Connection.GetGroupIUnknown();
	}
    return group;
}

void CArrayOfOPCConnection::DisconnectAll()
{
	ATLTRACE("Отключаемся от всех серверов\n");
	typedef TVConnections::iterator it;
	typedef vector<OPCHANDLE>::iterator hit;
	
	for(it i = _Connections.begin(); i!= _Connections.end(); i++)
	{
		sServerID* conn = *i;
		for(hit j=conn->ServerTagHandles.begin(); j!=conn->ServerTagHandles.end(); j++)
		{
			conn->Connection.RemoveTag(*j);
		}
		conn->ServerTagHandles.clear();
		AtlUnadvise(	conn->Connection.GetGroupIUnknown(),
								__uuidof(IOPCDataCallback), 
								conn->Advise);
		conn->Connection.Disconnect();
		delete conn;
	}
	_Connections.clear();
}

OPCHANDLE CArrayOfOPCConnection::GenerateUniqueHandle()
{
	typedef vector<OPCHANDLE>::iterator it;
	OPCHANDLE res = 1;
	BOOL IsPresent;

	do
	{
		IsPresent = FALSE;
		it item = find(_ClientTagHandles.begin(), _ClientTagHandles.end(), res);
		if(item != _ClientTagHandles.end())
		{
			IsPresent = TRUE;
			res++;
		}
	}while(IsPresent);
	return res;
}

bool CArrayOfOPCConnection::TestServers()
{
	ATLTRACE("Testing connections to servers\n");
	typedef TVConnections::iterator it;
	typedef vector<OPCHANDLE>::iterator hit;
	
	for(it i = _Connections.begin(); i!= _Connections.end(); i++)
	{
		sServerID* conn = *i;
		if(!conn->Connection.TestServer())return  false;
	
	}
	return true;
}
void CArrayOfOPCConnection::RefreshChannels()
{
	for(TVConnections::iterator i=_Connections.begin();i!=_Connections.end();i++)
	{
		CComPtr<IUnknown> group = (*i)->Connection.GetGroupIUnknown();
		CComQIPtr<IOPCAsyncIO2> async(group);
		if(async)
		{
			DWORD tmp;
			async->Refresh2(OPC_DS_CACHE,0,&tmp);
		}
	}
}