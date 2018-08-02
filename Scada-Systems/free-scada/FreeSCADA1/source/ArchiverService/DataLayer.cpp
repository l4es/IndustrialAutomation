#include "StdAfx.h"
#include ".\datalayer.h"
#include "Messages.h"
#include "EventLogger.h"
#include "DataProcessors.h"

CAutoSingletonCreator<CDataLayer> _AutoCreateDataLayer;

CComAutoCriticalSection		CDataLayer::_cs;
TDataQueue					CDataLayer::_Queue;
BOOL						CDataLayer::_TerminateThread;
BOOL						CDataLayer::_WorkState;
TChannelsMap				CDataLayer::_Channels;

namespace Helpers
{
	bool ConvertValToBool(CComVariant value)
	{
		if(value.vt == VT_EMPTY)
			return false;
		if(value.vt == VT_BSTR)
		{
			CString tmp(value.bstrVal);
			tmp.MakeLower();
			if(	tmp == "yes" ||
				tmp == "1" ||
				tmp == "true" ||
				tmp == "y")
			{
				return true;
			}
			else
				return false;
		}
		value.ChangeType(VT_UI4);
		return value.ulVal != 0;
	}
}

CDataLayer::CDataLayer(void)
{
	_LockQueue = false;
}

CDataLayer::~CDataLayer(void)
{
	
}

BOOL CDataLayer::Initialize(void)
{
	_TerminateThread	= FALSE;
	_WorkState			= TRUE;

	_TerminateThread = FALSE;

	if(!LoadActiveChannels())
		return FALSE;
	
	_hThread = CreateThread( NULL, NULL, WorkThread, NULL, NULL, &_ThreadID);
	if(!_hThread)
	{
		CSingletonPtr<CEventLogger> log;
		log->GenMsg(ERR_CREATE_THREAD);
		return FALSE;
	}
	return TRUE;
}

void CDataLayer::DeInitialize()
{
	_cs.Lock();
	_TerminateThread = TRUE;
	SetThreadPriority(_hThread, THREAD_PRIORITY_HIGHEST);
	_cs.Unlock();

	if(WaitForSingleObject(_hThread, 1000) == WAIT_TIMEOUT)
	{
		TerminateThread(_hThread, 0);
		CSingletonPtr<CEventLogger> log;
		log->GenMsg(ERR_TERMINATE_THREAD);
	}
}

TDataQueue& CDataLayer::LockQueue(void)
{
/*	if(_LockQueue)
	{
		CEventLoggerPtr log;
		log->LogMessage(L"Попытка заблокировать очередь. Она уже заблокирована");
		throw L"Очередь уже заблокирована";
	}
*/
	
	_cs.Lock();
	_LockQueue = true;
	return _Queue;
}

void CDataLayer::ReleaseQueue(void)
{
	/*if(!_LockQueue)
		return;
*/
	
	
	_cs.Unlock();
	_LockQueue = false;
}

void CDataLayer::SetWorkState(BOOL Run)
{
	_cs.Lock();
	_WorkState = Run;
	_cs.Unlock();
}

DWORD WINAPI CDataLayer::WorkThread(LPVOID)
{
	CoInitialize(NULL);
	CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

	for(;;)
	{
		_cs.Lock();
		if(_TerminateThread)
		{
			_cs.Unlock();
			CoUninitialize();
			return 0;
		}

		if(!_Queue.empty() && _WorkState==TRUE)
		{
			ProcessQueue();
			
			_cs.Unlock();
		}
		else
		{
			_cs.Unlock();
			Sleep(10);
		}
	}
	CoUninitialize();
	return 0;
}

BOOL CDataLayer::LoadActiveChannels()
{
	CSingletonPtr<CEventLogger> log;
	_Channels.clear();
	try
	{
		IDBConnectionPtr Connection(__uuidof(DBConnection));
		CSingletonPtr<CApplicationSettings> app_set;
		HRESULT hr;
		hr = Connection->Open(CComBSTR(app_set->GetDBUserName()),CComBSTR(app_set->GetDBUserPass()));
		if(SUCCEEDED(hr))
		{
			IChannelsPtr Channels;
			hr = Connection->GetChannels(&Channels);
			if(SUCCEEDED(hr))
			{
				IVariantArrayPtr Array;
				hr = Channels->GetChannelsID(&Array);
				if(Array != NULL)
				{
					ULONG Count;
					Array->get_Count(&Count);
					for(ULONG i=0;i<Count;i++)
					{
						CComVariant v;
						Array->get_Item(i, &v);
						AddChannelToList(Connection, Channels, v.ulVal);
					}
					return TRUE;
				}
				else
				{
					log->GenMsg(ERR_DBACCESSOR);
					log->LogMessage(L"Ошибка в функции IChannels::GetChannelsID. Результат функции равен NULL.");
					if(hr != S_OK)
					{
						log->DecodeIError(Channels);
						log->DecodeHRESULT(hr);
					}
				}
			}
			else
				log->GenMsg(ERR_GET_CHANNELS);
		}
		else
			log->GenMsg(ERR_OPEN_DATABASE);
	}catch(_com_error err)
	{
		log->GenMsg(ERR_DBACCESSOR);
		if(err.Error()!=S_OK)
			log->LogMessage(err.ErrorMessage());
		return FALSE;
	}
	return FALSE;
}

void CDataLayer::AddChannelToList(IDBConnectionPtr Connection, IChannelsPtr Channels, ULONG ID)
{
	CComVariant val;
	HRESULT hr;

	//Если канал не активен, не добавляем его
	Channels->get_Attribute(ID, CComBSTR(cAttributeStateActive), &val);
	if(!ConvertValToBool(val))
		return;

	CComBSTR Server, Computer, Name;
	hr = Channels->GetChannelInfo(ID, &Server, &Computer, &Name);
	if(FAILED(hr))
		return;
	CLSID _tempCLSID;
	hr = CLSIDFromString(Server, &_tempCLSID);
	if(FAILED(hr))
		return;

	sChannel& chnl	= _Channels[ID];
	chnl.Computer	= Computer;
	chnl.Name		= Name;
	memcpy(&chnl.ServerCLSID, &_tempCLSID, sizeof(CLSID));

	Channels->get_Attribute(ID, CComBSTR(cAttributeUpdateInterval), &val);
	if(val.vt != VT_EMPTY)
	{
		val.ChangeType(VT_UI4);
		chnl.UpdateInterval = val.ulVal;
	}
	else
		chnl.UpdateInterval = 5000;

	//////////////////////////////////////////////////////////////////////////
	//Добавляем "процессоры" данных
	//Генерация событий на изменение канала
	BOOL UnconditionalArchive = TRUE;	//Включена ли безусловная архивация
	using namespace DataProcessors;

	Channels->get_Attribute(ID, CComBSTR(cAttributeGenEvntOnChange), &val);
	if(ConvertValToBool(val))
		chnl.DataProcessors.push_back(new fGenEvntOnChange());

	Channels->get_Attribute(ID, CComBSTR(cAttributeGenEvntOnCrossVal1), &val);
	if(ConvertValToBool(val))
	{
		Channels->get_Attribute(ID, CComBSTR(cAttributeGenEvntOnCrossVal1Set), &val);
		hr = val.ChangeType(VT_R8);
		if(SUCCEEDED(hr))
			chnl.DataProcessors.push_back(new fGenEvntOnCrossValue(val,ET_CROSS_VAL1_UP,ET_CROSS_VAL1_DOWN));
	}

	Channels->get_Attribute(ID, CComBSTR(cAttributeGenEvntOnCrossVal2), &val);
	if(ConvertValToBool(val))
	{
		Channels->get_Attribute(ID, CComBSTR(cAttributeGenEvntOnCrossVal2Set), &val);
		hr = val.ChangeType(VT_R8);
		if(SUCCEEDED(hr))
			chnl.DataProcessors.push_back(new fGenEvntOnCrossValue(val,ET_CROSS_VAL2_UP,ET_CROSS_VAL2_DOWN));
	}

	Channels->get_Attribute(ID, CComBSTR(cAttributeArchOnGreater), &val);
	if(ConvertValToBool(val))
	{
		UnconditionalArchive = FALSE;
		Channels->get_Attribute(ID, CComBSTR(cAttributeArchOnGreaterSet), &val);
		hr = val.ChangeType(VT_R8);
		if(SUCCEEDED(hr))
			chnl.DataProcessors.push_back(new fArchiveByOp< greater<CComVariant> >(val));
	}

	Channels->get_Attribute(ID, CComBSTR(cAttributeArchOnLess), &val);
	if(ConvertValToBool(val))
	{
		UnconditionalArchive = FALSE;
		Channels->get_Attribute(ID, CComBSTR(cAttributeArchOnLessSet), &val);
		hr = val.ChangeType(VT_R8);
		if(SUCCEEDED(hr))
			chnl.DataProcessors.push_back(new fArchiveByOp< less<CComVariant> >(val));
	}

	Channels->get_Attribute(ID, CComBSTR(cAttributeArchOnEqual), &val);
	if(ConvertValToBool(val))
	{
		UnconditionalArchive = FALSE;
		Channels->get_Attribute(ID, CComBSTR(cAttributeArchOnEqualSet), &val);
		hr = val.ChangeType(VT_R8);
		if(SUCCEEDED(hr))
			chnl.DataProcessors.push_back(new fArchiveByOp< equal_to<CComVariant> >(val));
	}

	if(UnconditionalArchive)
		chnl.DataProcessors.push_back(new fArchive());
}

void CDataLayer::CopyChannels(TChannelsMap& Dest)
{
	_cs.Lock();
	Dest.clear();
	for(TChannelsMap::iterator i=_Channels.begin();i!=_Channels.end();i++)
	{
		Dest[i->first] = i->second;
		Dest[i->first].DataProcessors.clear();
	}
	_cs.Unlock();
}

void CDataLayer::ProcessQueue(void)
{
	sData data = _Queue.front();
	_Queue.pop();
	if(_Channels.find(data.ItemID) == _Channels.end())
		return;

	sChannel &channel = _Channels[data.ItemID];
	for(vector<fDataProcessor*>::iterator i=channel.DataProcessors.begin();i!=channel.DataProcessors.end();i++)
	{
		try
		{
			if(*i != NULL && !IsBadCodePtr((FARPROC)*i))
				(*i)->Process(data);
		}
		catch(...)
		{
			CSingletonPtr<CEventLogger> log;
			log->GenMsg(ERR_DBACCESSOR);
		}
	}
}

BOOL CDataLayer::GetChannelByItemID(DWORD ItemID, sChannel& Channel)
{
	_cs.Lock();
	if(_Channels.find(ItemID) == _Channels.end())
	{
		_cs.Unlock();
		return FALSE;
	}

	sChannel &chnl = _Channels[ItemID];
	Channel = chnl;
	Channel.DataProcessors.clear();
	_cs.Unlock();
	return TRUE;
}