#include "StdAfx.h"
#include ".\opcconnection.h"
#include "Doc.h"

COPCConnection::COPCConnection(void)
{
	_Computer.empty();
	ZeroMemory(&_Clsid, sizeof(CLSID));
	
	_Server = NULL;
	_Group	= NULL;
	_hGroup	= NULL;
	_Advise	= NULL;
}

COPCConnection::~COPCConnection(void)
{
	ATLASSERT(_Server == NULL); //Надо корректно отключиться
	ATLASSERT(_Group == NULL);	//Надо корректно отключиться
}

BOOL COPCConnection::Connect(wstring Computer, CLSID &Clsid, CComPtr<IUnknown> Callback, CDoc* pDoc)
{
	ATLASSERT(Clsid != GUID_NULL);
	ATLASSERT(pDoc != NULL);

	//инициализируем локальные переменные
	_Computer = Computer;
	memcpy(&_Clsid, &Clsid, sizeof(CLSID));

	COSERVERINFO	si;
	MULTI_QI		qi;

	ZeroMemory(&si,sizeof(COSERVERINFO));
	ZeroMemory(&qi,sizeof(MULTI_QI));

	//Запрос интерфейса IOPCServer у сервера
	si.pwszName = /*CT2W*/ (LPWSTR)(Computer.c_str());
	qi.pIID	= &__uuidof(IOPCServer);
	HRESULT hr;
	hr = CoCreateInstanceEx(	Clsid, NULL, CLSCTX_ALL, &si, 1, &qi);
	if(TEST_HR(hr) && TEST_HR(qi.hr))
	{
		hr = qi.pItf->QueryInterface(__uuidof(IOPCServer), (void**)&_Server);
		qi.pItf->Release();
		if(TEST_HR(hr))
		{
			//Добавляем группу
			LONG TimeBias			= NULL;
			FLOAT PercentDeadBand	= NULL;
			DWORD UpdateRate		= NULL;
			GUID riid				= IID_IUnknown;
			CComPtr<IUnknown>		pUnk;
			
			sProjectSettings ps;
			pDoc->m_Archive.LoadProjectSettings(ps);
			hr=_Server->AddGroup(	L"",
									TRUE,
									ps.IntervalOPC,
									0,
									&TimeBias,
									&PercentDeadBand,
									LCID_RUSSIAN,
									&_hGroup,
									&UpdateRate,
									riid,
									&pUnk);
			if(hr==OPC_S_UNSUPPORTEDRATE)hr=S_OK;

			if(TEST_HR(hr))
			{
				hr = pUnk.QueryInterface(&_Group);
				if(TEST_HR(hr))
				{
					//Подключаем CallBack интерфейс
					hr =  AtlAdvise(	_Group, Callback, IID_IOPCDataCallback, &_Advise);
					return TEST_HR(hr);
				}
			}
		}
	}
	return FALSE;
}

void COPCConnection::Disconnect()
{
	if(_Server == NULL) return; //Уже отключен
	if(_Group == NULL) return;	//Уже отключен

	//Отключаем Callback
	AtlUnadvise(_Group, __uuidof(IOPCDataCallback), _Advise);
	//Удаляем группу
	_Server->RemoveGroup(_hGroup, FALSE);

	_Computer.empty();
	ZeroMemory(&_Clsid, sizeof(CLSID));

	//Освобождаем интерфейсы
	_Group	= NULL;
	_Server	= NULL;
	_hGroup	= NULL;
	_Advise	= NULL;
}

BOOL COPCConnection::AddTag(wstring Tag, OPCHANDLE hTag, OPCHANDLE* hServerTag)
{
	ATLASSERT(_Group != NULL);

	OPCITEMDEF		item;
	OPCITEMRESULT	*results;
	HRESULT			*Errors;

	//Добавляем канал на обновление
	ZeroMemory(&item, sizeof(OPCITEMDEF));
	item.bActive	= TRUE;
	item.hClient	= hTag;
	item.szItemID	= /*CT2W*/(LPWSTR)(Tag.c_str());
	item.vtRequestedDataType = VT_EMPTY;
	
	HRESULT hr;
	hr = _Group->AddItems(1, &item, &results, &Errors);
	TEST_HR(hr);

	*hServerTag = results[0].hServer;

	LocalFree(results); 
	LocalFree(Errors);

	return hr == S_OK;
}

void COPCConnection::RemoveTag(OPCHANDLE hServerTag)
{
	ATLASSERT(_Group != NULL);
	HRESULT *results;

	//Удалить канал
	_Group->RemoveItems(1, &hServerTag, &results);
	LocalFree(results);
}
bool COPCConnection::TestServer()
{
	OPCSERVERSTATUS *sStatus;
	HRESULT hr;
	hr=_Server->GetStatus(&sStatus);
	if(hr==S_OK)
		if(sStatus->dwServerState==OPC_STATUS_RUNNING) return 1;
	return 0;

}