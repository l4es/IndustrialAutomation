#pragma once
#include <queue>
#include <map>
#include "EventLogger.h"

using namespace Helpers;

//Изменение канала
const TCHAR cAttributeGenEvntOnChange[]			= _T("GenerateEventsChange");

//Превышение уставки
const TCHAR cAttributeGenEvntOnCrossVal1[]		= _T("GenerateEventsOnCrossVal1");
const TCHAR cAttributeGenEvntOnCrossVal1Set[]	= _T("GenerateEventsOnCrossVal1_Set");
//Падение ниже уставки
const TCHAR cAttributeGenEvntOnCrossVal2[]		= _T("GenerateEventsOnCrossVal2");
const TCHAR cAttributeGenEvntOnCrossVal2Set[]	= _T("GenerateEventsOnCrossVal2_Set");

//Падение ниже уставки
const TCHAR cAttributeArchOnLess[]				= _T("ArchOnLess");
const TCHAR cAttributeArchOnLessSet[]			= _T("ArchOnLess_Set");
//Превышение уставки
const TCHAR cAttributeArchOnGreater[]			= _T("ArchOnGreater");
const TCHAR cAttributeArchOnGreaterSet[]		= _T("ArchOnGreater_Set");
//Равно уставке
const TCHAR cAttributeArchOnEqual[]				= _T("ArchOnEqual");
const TCHAR cAttributeArchOnEqualSet[]			= _T("ArchOnEqual_Set");

const TCHAR cAttributeUpdateInterval[]	= _T("UpdateInterval");
const TCHAR cAttributeStateActive[]		= _T("Active");


struct sData
{
	ULONG		ItemID;		//ID канала
	CComVariant	Data;		//Данные
	ULONG		Quality;	//"Качество" данных
	FILETIME	Time;		//"Time" данных
};

struct fDataProcessor
{
	fDataProcessor()
	{
		_Connection = NULL;
	};
	virtual ~fDataProcessor()
	{
		_Connection = NULL;
	};

	virtual void Process(sData&) = 0;

protected:
	IDBConnectionPtr _Connection;

	inline bool IsConnected(){return (_Connection != NULL);};
	bool Connect()
	{
		CSingletonPtr<CApplicationSettings> app_set;
		HRESULT hr;
		hr = _Connection.CreateInstance(__uuidof(DBConnection));
		if(SUCCEEDED(hr))
		{
			hr = _Connection->Open(CComBSTR(app_set->GetDBUserName()),CComBSTR(app_set->GetDBUserPass()));
			if(SUCCEEDED(hr))
				return true;
			else
			{
				CEventLoggerPtr log;
				log->DecodeIError(_Connection);
			}
		}
		_Connection = NULL;
		return false;
	}
};

struct sChannel
{
	CString Name;
	DWORD UpdateInterval;
	CLSID ServerCLSID;
	CString Computer;

	vector<fDataProcessor*> DataProcessors;

	~sChannel()
	{
		for(vector<fDataProcessor*>::iterator i=DataProcessors.begin();i!=DataProcessors.end();i++)
			delete *i;
	};
};

typedef std::queue<sData> TDataQueue;
typedef std::map<ULONG,sChannel> TChannelsMap;

class CDataLayer 
	: public CSingleton<CDataLayer>
{
public:
	BOOL Initialize(void);
	void DeInitialize();

	TDataQueue& LockQueue(void);
	void ReleaseQueue(void);

	void SetWorkState(BOOL Run);

	void CopyChannels(TChannelsMap& Dest);

	BOOL GetChannelByItemID(DWORD ItemID, sChannel& Channel);

protected:
	static CComAutoCriticalSection _cs;
	static TDataQueue _Queue;
	static BOOL _TerminateThread;
	static BOOL _WorkState;				//Архивировать или нет данные (PAUSE/RUN)
	bool _LockQueue;
	HANDLE _hThread;
	DWORD _ThreadID;
	static TChannelsMap _Channels;

protected:
	CDataLayer(void);
	~CDataLayer(void);
	DEF_SINGLETON(CDataLayer);

	static void ProcessQueue(void);
	static DWORD WINAPI WorkThread(LPVOID);
	BOOL LoadActiveChannels();
	void AddChannelToList(IDBConnectionPtr Connection, IChannelsPtr Channels, ULONG ID);
};
