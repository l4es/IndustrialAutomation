#pragma once

#include <vector>
#include <list>

using namespace std;

class CTagDB
{
public:
	CTagDB(void);
	~CTagDB(void);

	//Типы
	enum enTagType
	{
		TT_BIT = 0,
		TT_WORD,
		TT_FLOAT,
	};
	struct sChannelData
	{
		COleDateTime DateTime;
		double val;
	};

	struct sChannel
	{
		CString Name;
		UINT	id;
	};

	struct sChannelValue
	{
		CString Name;
		COleDateTime DateTime;
		CComVariant Value;
	};


	typedef vector<sChannelData> TVChannelData;
	typedef vector<sChannel> TVChannel;
	typedef vector<sChannelValue> TVChannelValues;

	//Запросы
	TVChannel GetChannelsList();
	TVChannel GetHumanReadableChannelsList();
	TVChannelData GetData(DWORD ChannelID, COleDateTime From, COleDateTime To);
	TVChannelValues GetValues(vector<DWORD> ChannelIDs, COleDateTime From, COleDateTime To);
	
	//получить список всех событий
	TVChannelValues GetChannelEvents(vector<DWORD> ChannelIDs, COleDateTime From, COleDateTime To);
	//получить список событий по фильтру
	TVChannelValues GetChannelEvents(vector<DWORD> ChannelIDs, COleDateTime From, COleDateTime To, DWORD EventID);
	//получить список типов событий для указанных каналов за указанное время
	TVChannel GetEventTypes(vector<DWORD> ChannelIDs, COleDateTime From, COleDateTime To);
	

private:
	DB::CADODatabase	_DB;
};
