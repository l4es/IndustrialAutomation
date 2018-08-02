#include "StdAfx.h"
#include "../3rdParty/opc/opcquality.h"
#include ".\tagdb.h"
#include "ErrorReporter.h"
#include "Helpers.h"
#include <atlsafe.h>
#include <map>

#define WARN_RECORDS_COUNT	1000

using namespace DB;
using namespace Helpers;

bool CompareChnlDataByTime(const CTagDB::sChannelData& lhs, const CTagDB::sChannelData& rhs)
{
	return lhs.DateTime < rhs.DateTime;
}

struct fFindChnlByID
{
	ULONG ID;
	fFindChnlByID(ULONG ID_):ID(ID_){};

	bool operator()(const CTagDB::sChannel& chnl)
	{
		return chnl.id == ID;
	}
};

BOOL CheckRecordsCount(IDataSetPtr data)
{
	if(data == NULL)
		return FALSE;

	if(data->GetRecordsCount() > WARN_RECORDS_COUNT)
	{
		CString msg;
		msg.Format(L"Query result has %u records.\nIts showing could takes a long time. Continue?", data->GetRecordsCount());
		if(MessageBox(NULL, msg, L"Warning", MB_YESNO|MB_ICONWARNING) == IDYES)
			return TRUE;
		else
			return FALSE;
	}
	return TRUE;
}

CTagDB::CTagDB(void)
{
}

CTagDB::~CTagDB(void)
{
}

CTagDB::TVChannel CTagDB::GetChannelsList()
{
	IChannelsPtr Channels;
	TVChannel vec;

	try
	{
		Channels = g_Connection->GetChannels();
	}
	catch(...)
	{
		CErrorReporterPtr rep;
		rep->ShowError(g_Connection);
		return vec;
	}

	try
	{
		IDataSetPtr data = Channels->GetChannelsList("", "", "");
		if(!CheckRecordsCount(data))
			return vec;
		vec.reserve(data->GetRecordsCount());
		while(!data->IsEOF)
		{
			sChannel chnl;
			CComVariant v;
			
			v = data->GetFieldValue("Name");
			chnl.Name = v.bstrVal;
			
			v = data->GetFieldValue("ID");
			v.ChangeType(VT_UI4);
			chnl.id = v.ulVal;
			vec.push_back(chnl);
			data->MoveNext();
		}
		data = NULL;
		Channels = NULL;
	}	
	catch(...)
	{
		CErrorReporterPtr rep;
		rep->ShowError(Channels);
	}
	return vec;
}

CTagDB::TVChannel CTagDB::GetHumanReadableChannelsList()
{
	IChannelsPtr Channels;
	TVChannel vec;

	try
	{
		Channels = g_Connection->GetChannels();
	}
	catch(...)
	{
		CErrorReporterPtr rep;
		rep->ShowError(g_Connection);
		return vec;
	}

	try
	{
		IDataSetPtr data = Channels->GetChannelsList("", "", "");
		if(!CheckRecordsCount(data))
			return vec;
		vec.reserve(data->GetRecordsCount());
		while(!data->IsEOF)
		{
			sChannel chnl;
			CComVariant v;

			v = data->GetFieldValue("ID");
			v.ChangeType(VT_UI4);
			chnl.id = v.ulVal;

			v = Channels->Attribute[chnl.id]["HumanReadableName"];
			v.ChangeType(VT_BSTR);
			chnl.Name = v.bstrVal;

			if(chnl.Name.IsEmpty())
			{
				v = data->GetFieldValue("Name");
				v.ChangeType(VT_BSTR);
				chnl.Name = v.bstrVal;
			}

			vec.push_back(chnl);
			data->MoveNext();
		}
		data = NULL;
		Channels = NULL;
	}	
	catch(...)
	{
		CErrorReporterPtr rep;
		rep->ShowError(Channels);
	}
	return vec;
}

CTagDB::TVChannelData CTagDB::GetData(DWORD ChannelID, COleDateTime From, COleDateTime To)
{
	TVChannelData vec;
	IChannelsPtr Channels;

	try
	{
		Channels = g_Connection->GetChannels();
	}
	catch(...)
	{
		CErrorReporterPtr rep;
		rep->ShowError(g_Connection);
		return vec;
	}

	try
	{
		CComVariant FromDateTime;
		CComVariant ToDateTime;
		FromDateTime.date = From;
		ToDateTime.date = To;
		FromDateTime.vt = ToDateTime.vt = VT_DATE;
		FromDateTime = VariantLocalTimeToSystemTime(FromDateTime);
		ToDateTime = VariantLocalTimeToSystemTime(ToDateTime);
		
		IDataSetPtr dataBool	= Channels->GetChannelData(FromDateTime, ToDateTime, CComVariant(Q_GOOD), CDT_BOOLEAN, ChannelID);
		IDataSetPtr dataDWord	= Channels->GetChannelData(FromDateTime, ToDateTime, CComVariant(Q_GOOD), CDT_DWORD, ChannelID);
		IDataSetPtr dataFloat	= Channels->GetChannelData(FromDateTime, ToDateTime, CComVariant(Q_GOOD), CDT_FLOAT, ChannelID);
		if(!CheckRecordsCount(dataBool))
			return vec;
		if(!CheckRecordsCount(dataDWord))
			return vec;
		if(!CheckRecordsCount(dataFloat))
			return vec;
		vec.reserve(dataBool->GetRecordsCount() + dataDWord->GetRecordsCount() + dataFloat->GetRecordsCount());

		while(!dataBool->IsEOF)
		{
			sChannelData chnl;
			CComVariant v;
			v = dataBool->GetFieldValue("EventTime");
			chnl.DateTime = VariantSystemTimeToLocalTime(v);
	

			v = dataBool->GetFieldValue("Value");
			v.ChangeType(VT_R8);
			chnl.val = v.dblVal;
			vec.push_back(chnl);
			dataBool->MoveNext();
		}
		while(!dataDWord->IsEOF)
		{
			sChannelData chnl;
			CComVariant v;
			v = dataDWord->GetFieldValue("EventTime");
			chnl.DateTime = VariantSystemTimeToLocalTime(v);

			v = dataDWord->GetFieldValue("Value");
			v.ChangeType(VT_R8);
			chnl.val = v.dblVal;
			vec.push_back(chnl);
			dataDWord->MoveNext();
		}
		while(!dataFloat->IsEOF)
		{
			sChannelData chnl;
			CComVariant v;
			v = dataFloat->GetFieldValue("EventTime");
			chnl.DateTime = VariantSystemTimeToLocalTime(v);

			v = dataFloat->GetFieldValue("Value");
			v.ChangeType(VT_R8);
			chnl.val = v.dblVal;
			vec.push_back(chnl);
			dataFloat->MoveNext();
		}
		sort(vec.begin(),vec.end(), CompareChnlDataByTime);
	}
	catch(...)
	{
		CErrorReporterPtr rep;
		rep->ShowError(Channels);
	}
	return vec;
}

CTagDB::TVChannelValues CTagDB::GetValues(vector<DWORD> ChannelIDs, COleDateTime From, COleDateTime To)
{
	CWaitCursor wait;
	TVChannelValues vec;
	IChannelsPtr Channels;
	if(ChannelIDs.empty())
		return vec;

	try
	{
		Channels = g_Connection->GetChannels();
	}
	catch(...)
	{
		CErrorReporterPtr rep;
		rep->ShowError(g_Connection);
		return vec;
	}

	try
	{
		CComVariant FromDateTime;
		CComVariant ToDateTime;
		FromDateTime.date = From;
		ToDateTime.date = To;
		FromDateTime.vt = ToDateTime.vt = VT_DATE;
		FromDateTime = VariantLocalTimeToSystemTime(FromDateTime);
		ToDateTime = VariantLocalTimeToSystemTime(ToDateTime);

		CComSafeArray<ULONG> sa;
		sa.Create(ChannelIDs.size());
		for(vector<DWORD>::iterator i=ChannelIDs.begin();i!=ChannelIDs.end();i++)
			sa[static_cast<LONG>(i-ChannelIDs.begin())] = *i;

		IDataSetPtr data = Channels->GetChannelsDataAsString(CComVariant(sa), FromDateTime, ToDateTime);
		if(!CheckRecordsCount(data))
			return vec;
		try
		{
			vec.reserve(data->GetRecordsCount());

			while(!data->IsEOF)
			{
				CComVariant v;
				v = data->GetFieldValue("Quality");
				v.ChangeType(VT_UI4);
				if(v.ulVal == Q_GOOD)
				{
					sChannelValue ChnlVal;
					ChnlVal.Name	= data->GetFieldValue("Name");
					ChnlVal.DateTime= VariantSystemTimeToLocalTime(data->GetFieldValue("EventTime"));
					ChnlVal.Value	= data->GetFieldValue("Val");
					vec.push_back(ChnlVal);
				}
				data->MoveNext();
			}
		}catch(...)
		{
			CErrorReporterPtr rep;
			rep->ShowError(data);
		};
	}catch(...)
	{
		CErrorReporterPtr rep;
		rep->ShowError(Channels);
	};

	return vec;
}

CTagDB::TVChannelValues CTagDB::GetChannelEvents(vector<DWORD> ChannelIDs, COleDateTime From, COleDateTime To)
{
	TVChannelValues vec;
	if(ChannelIDs.empty())
		return vec;
	IEventsPtr Events;
	IEventTypesPtr EventTypes;
	IChannelsPtr Channels;
	try
	{
		Events = g_Connection->GetEvents();
		EventTypes = g_Connection->GetEventTypes();
		Channels = g_Connection->GetChannels();
	}
	catch(...)
	{
		CErrorReporterPtr rep;
		rep->ShowError(g_Connection);
		return vec;
	}
	
	try
	{
		CComVariant FromDateTime;
		CComVariant ToDateTime;
		FromDateTime.date = From;
		ToDateTime.date = To;
		FromDateTime.vt = ToDateTime.vt = VT_DATE;
		FromDateTime = VariantLocalTimeToSystemTime(FromDateTime);
		ToDateTime = VariantLocalTimeToSystemTime(ToDateTime);

		map<ULONG, CString> TypeMap;
		map<ULONG, CString> ChnlMap;
		typedef map<ULONG, CString>::iterator it;
		for(vector<DWORD>::iterator ChannelID=ChannelIDs.begin();ChannelID!=ChannelIDs.end();ChannelID++)
		{
			IDataSetPtr data;
			data = Events->GetEventsList(*ChannelID, CComVariant(), FromDateTime, ToDateTime);
			if(!CheckRecordsCount(data))
				return vec;
			vec.reserve(vec.size() + data->GetRecordsCount());
			
			//Пробуем узнать название канала из кэша
			it ChannelItem = ChnlMap.find(*ChannelID);
			if(ChannelItem == ChnlMap.end())
			{
				CComBSTR ServerCLSID, ComputerName, ChannelName;
				CComVariant v;
				v = Channels->GetAttribute(*ChannelID, "HumanReadableName");
				v.ChangeType(VT_BSTR);
				ChannelName = v.bstrVal;
				if(ChannelName.Length() <= 0)
					Channels->GetChannelInfo(*ChannelID, &ServerCLSID, &ComputerName, &ChannelName);
				ChnlMap[*ChannelID] = CString(ChannelName);
				ChannelItem = ChnlMap.find(*ChannelID);
			}

			while(!data->IsEOF)
			{
				sChannelValue tmp;
				CComVariant TypeID;
				CComVariant EventTime;
				TypeID = data->GetFieldValueByIndex(0);
				EventTime = data->GetFieldValueByIndex(1);

				
				tmp.Name = ChannelItem->second;

				//Пробуем узнать название события кэша
				it item = TypeMap.find(TypeID.ulVal);
				if(item == TypeMap.end())
				{
					CComBSTR TypeName;
					TypeName = (BSTR)EventTypes->GetEventTypeName(TypeID.ulVal);
					TypeMap[TypeID.ulVal] = CString(TypeName);
					item = TypeMap.find(TypeID.ulVal);
				}
				tmp.Value = item->second;
				
				tmp.DateTime = VariantSystemTimeToLocalTime(EventTime);
				vec.push_back(tmp);
				data->MoveNext();
			}
		}
	}
	catch(...)
	{
		IErrorPtr err;
		CErrorReporterPtr rep;
		err = Events;
		if(err->GetErrorCode() != ERR_OK)
			rep->ShowError(err);
		err = EventTypes;
		if(err->GetErrorCode() != ERR_OK)
			rep->ShowError(err);
		err = Channels;
		if(err->GetErrorCode() != ERR_OK)
			rep->ShowError(err);
	}
	return vec;
}

CTagDB::TVChannelValues CTagDB::GetChannelEvents(vector<DWORD> ChannelIDs, COleDateTime From, COleDateTime To, DWORD EventID)
{
	TVChannelValues vec;
	if(ChannelIDs.empty())
		return vec;
	IEventsPtr Events;
	IEventTypesPtr EventTypes;
	IChannelsPtr Channels;
	try
	{
		Events = g_Connection->GetEvents();
		EventTypes = g_Connection->GetEventTypes();
		Channels = g_Connection->GetChannels();
	}
	catch(...)
	{
		CErrorReporterPtr rep;
		rep->ShowError(g_Connection);
		return vec;
	}

	try
	{
		CComVariant FromDateTime;
		CComVariant ToDateTime;
		FromDateTime.date = From;
		ToDateTime.date = To;
		FromDateTime.vt = ToDateTime.vt = VT_DATE;
		FromDateTime = VariantLocalTimeToSystemTime(FromDateTime);
		ToDateTime = VariantLocalTimeToSystemTime(ToDateTime);

		map<ULONG, CString> TypeMap;
		map<ULONG, CString> ChnlMap;
		typedef map<ULONG, CString>::iterator it;
		for(vector<DWORD>::iterator ChannelID=ChannelIDs.begin();ChannelID!=ChannelIDs.end();ChannelID++)
		{
			IDataSetPtr data;
			data = Events->GetEventsList(*ChannelID, CComVariant(EventID), FromDateTime, ToDateTime);
			if(!CheckRecordsCount(data))
				return vec;
			vec.reserve(vec.size()+data->GetRecordsCount());

			//Пробуем узнать название канала из кэша
			it ChannelItem = ChnlMap.find(*ChannelID);
			if(ChannelItem == ChnlMap.end())
			{
				CComBSTR ServerCLSID, ComputerName, ChannelName;
				CComVariant v;
				v = Channels->GetAttribute(*ChannelID, "HumanReadableName");
				v.ChangeType(VT_BSTR);
				ChannelName = v.bstrVal;
				if(ChannelName.Length() <= 0)
					Channels->GetChannelInfo(*ChannelID, &ServerCLSID, &ComputerName, &ChannelName);
				ChnlMap[*ChannelID] = CString(ChannelName);
				ChannelItem = ChnlMap.find(*ChannelID);
			}

			//Пробуем узнать название события кэша
			it TypeItem = TypeMap.find(EventID);
			if(TypeItem == TypeMap.end())
			{
				CComBSTR TypeName;
				TypeName = (BSTR)EventTypes->GetEventTypeName(EventID);
				TypeMap[EventID] = CString(TypeName);
				TypeItem = TypeMap.find(EventID);
			}

			while(!data->IsEOF)
			{
				sChannelValue tmp;
				CComVariant EventTime;
				EventTime = data->GetFieldValueByIndex(0);

				tmp.Name = ChannelItem->second;
				tmp.Value = TypeItem->second;

				tmp.DateTime = VariantSystemTimeToLocalTime(EventTime);
				vec.push_back(tmp);
				data->MoveNext();
			}
		}
	}
	catch(...)
	{
		IErrorPtr err;
		CErrorReporterPtr rep;
		err = Events;
		if(err->GetErrorCode() != ERR_OK)
			rep->ShowError(err);
		err = EventTypes;
		if(err->GetErrorCode() != ERR_OK)
			rep->ShowError(err);
		err = Channels;
		if(err->GetErrorCode() != ERR_OK)
			rep->ShowError(err);
	}
	return vec;
}

CTagDB::TVChannel CTagDB::GetEventTypes(vector<DWORD> ChannelIDs, COleDateTime From, COleDateTime To)
{
	TVChannel vec;
	if(ChannelIDs.empty())
		return vec;
	IEventsPtr Events;
	IEventTypesPtr EventTypes;
	try
	{
		Events = g_Connection->GetEvents();
		EventTypes = g_Connection->GetEventTypes();
	}
	catch(...)
	{
		CErrorReporterPtr rep;
		rep->ShowError(g_Connection);
		return vec;
	}

	try
	{
		CComVariant FromDateTime;
		CComVariant ToDateTime;
		FromDateTime.date = From;
		ToDateTime.date = To;
		FromDateTime.vt = ToDateTime.vt = VT_DATE;
		FromDateTime = VariantLocalTimeToSystemTime(FromDateTime);
		ToDateTime = VariantLocalTimeToSystemTime(ToDateTime);

		map<ULONG, CString> TypeMap;
		map<ULONG, CString> ChnlMap;
		typedef map<ULONG, CString>::iterator it;
		for(vector<DWORD>::iterator i=ChannelIDs.begin();i!=ChannelIDs.end();i++)
		{
			IDataSetPtr data;
			data = Events->GetEventsList(*i, CComVariant(), FromDateTime, ToDateTime);
			if(!CheckRecordsCount(data))
				return vec;
			while(!data->IsEOF)
			{
				sChannel tmp;
				CComVariant TypeID;
				TypeID = data->GetFieldValue("TypeID");
				TypeID.ChangeType(VT_UI4);
				TVChannel::iterator k = find_if(vec.begin(),vec.end(), fFindChnlByID(TypeID.ulVal));
				if(k == vec.end())
				{
					//Пробуем узнать название события кэша
					it item = TypeMap.find(TypeID.ulVal);
					if(item == TypeMap.end())
					{
						CComBSTR TypeName;
						TypeName = (BSTR)EventTypes->GetEventTypeName(TypeID.ulVal);
						TypeMap[TypeID.ulVal] = CString(TypeName);
						item = TypeMap.find(TypeID.ulVal);
					}
					tmp.Name = item->second;
					tmp.id = TypeID.ulVal;
					vec.push_back(tmp);
				}
				data->MoveNext();
			}
		}
	}
	catch(...)
	{
		IErrorPtr err;
		CErrorReporterPtr rep;
		err = Events;
		if(err->GetErrorCode() != ERR_OK)
			rep->ShowError(err);
		err = EventTypes;
		if(err->GetErrorCode() != ERR_OK)
			rep->ShowError(err);
	}
	return vec;
}