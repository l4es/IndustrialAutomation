// Channelss.cpp : Implementation of CChannels

#include "stdafx.h"
#include "Channels.h"
#include "DBConnection.h"
#include "Helpers.h"
#include "ApplicationSettings.h"
#include "VariantEnumerator.h"
#include "DataSet.h"
#include "DataSetEx.h"
#include <atlsafe.h>
#include ".\channels.h"

using namespace DB;
using namespace Helpers;

// CChannels

STDMETHODIMP CChannels::get_ErrorDescription(BSTR* pVal)
{
	if(_dwLastError == ERR_CUSTOM)
		return CComBSTR(_strLastError).CopyTo(pVal);

	CString s;
	s = Helpers::DecodeError(_dwLastError);

	return CComBSTR(s).CopyTo(pVal);
}

STDMETHODIMP CChannels::get_ErrorCode(enErrorCodes* pVal)
{
	*pVal = static_cast<enErrorCodes>(_dwLastError);
	if(_dwLastError == ERR_CUSTOM)
		return S_FALSE;
	else
		return S_OK;
}

HRESULT CChannels::FinalConstruct()
{
	return S_OK;
}

void CChannels::FinalRelease() 
{
	if(_Owner)
	{
		_Owner->Release();
		_Owner = NULL;
	}
}

HRESULT CChannels::Initialize(CDBConnection *Owner)
{
	_Owner = Owner;
	if(_Owner)
		_Owner->AddRef();
	else
		return E_FAIL;
	return S_OK;
}
STDMETHODIMP CChannels::AddChannel(BSTR ServerCLSID, BSTR ComputerName, BSTR ChannelName, BSTR Description, ULONG* ID)
{
	_dwLastError = ERR_OK;
	
	if(wcslen(ChannelName) <= 0)
	{
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}
	if(!_Owner->GetCurrentUser().CanEditChannels() )
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CLSID ServerCLSID_;
	HRESULT hr;
	hr = CLSIDFromString(ServerCLSID, &ServerCLSID_);
	if(FAILED(hr))
	{
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}
	LPOLESTR strTmp;
	CComBSTR ServCLSID;
	StringFromCLSID(ServerCLSID_, &strTmp);
	ServCLSID = strTmp;
	LocalFree(strTmp);

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;
	ULONG			ServerID;

	DB->BeginTransaction();
	//Получить или создать новую запись сервера
	query.Format(_T("SELECT ID FROM Servers WHERE ServerGUID='%s' AND Computer='%s';"),
		(LPCTSTR)(_bstr_t)ServCLSID, (LPCTSTR)(_bstr_t)ComputerName);
	DO_QUERY_RS_TRANS(DB, rs, query);
	if(rs.GetRecordCount() > 0)
	{
		rs.MoveFirst();
		rs.GetFieldValue(_T("ID"), ServerID);
	}
	else
	{
		query.Format(_T("INSERT INTO Servers(ServerGUID,Computer) VALUES('%s','%s');"),
			(LPCTSTR)(_bstr_t)ServCLSID, (LPCTSTR)(_bstr_t)ComputerName);
		DO_QUERY_DB_TRANS(DB,query);
		CADORecordset srs(DB);
		query.Format(_T("SELECT ID FROM Servers WHERE ServerGUID='%s' AND Computer='%s';"),
			(LPCTSTR)(_bstr_t)ServCLSID, (LPCTSTR)(_bstr_t)ComputerName);
		if(!srs.Open(query))
		{
			rs.Close();
			DB->RollbackTransaction();
			_dwLastError	= ERR_CUSTOM;
			_strLastError.Format(_T("Ошибка при выполнении запроса (%s)"), DB->GetLastErrorString().c_str());
			
			return E_FAIL;
		}
		if(srs.GetRecordCount() > 0)
		{
			srs.MoveFirst();
			srs.GetFieldValue(_T("ID"), ServerID);
		}
		else
		{
			srs.Close();
			rs.Close();
			DB->RollbackTransaction();
			_dwLastError = ERR_CRITICAL;
			return E_FAIL;
		}
		srs.Close();

	}
	rs.Close();

	query.Format(_T("SELECT ID FROM Channels WHERE Name='%s' AND ServerID=%u"), (LPCTSTR)(_bstr_t)ChannelName, ServerID);
	DO_QUERY_RS_TRANS(DB, rs, query);
	if(rs.GetRecordCount() > 0)
	{
		DB->RollbackTransaction();
		rs.GetFieldValue(_T("ID"), *ID);
		rs.Close();
		_dwLastError = ERR_DUBLICATE;
		return S_FALSE;
	}
	rs.Close();

	query.Format(_T("INSERT INTO Channels(ServerID, Name, Description) VALUES(%u,'%s','%s');"),
				ServerID,(LPCTSTR)(_bstr_t)ChannelName,(LPCTSTR)(_bstr_t)Description);
	DO_QUERY_DB_TRANS(DB,query);
	DB->CommitTransaction();

	query.Format(_T("SELECT ID FROM Channels WHERE Name='%s' AND ServerID=%u"), (LPCTSTR)(_bstr_t)ChannelName, ServerID);
	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount() > 0)
		rs.GetFieldValue(_T("ID"), *ID);
	else
	{
		rs.Close();
		_dwLastError = ERR_CRITICAL;
		return E_FAIL;
	}
	rs.Close();
	return S_OK;
}

STDMETHODIMP CChannels::DeleteChannel(ULONG ID)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanEditChannels() )
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;

	//Если для сервера удалены все использующие его каналы, удаляем сервер
	
	//Определить ID сервера
	query.Format(_T("SELECT ServerID FROM Channels WHERE ID=%u;"), ID);
	DO_QUERY_RS(DB, rs, query);
	ULONG ServerID;
	if(rs.GetRecordCount()<=0)
	{
		rs.Close();
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}
	rs.MoveFirst();
	rs.GetFieldValue(_T("ServerID"), ServerID);
	rs.Close();

	//Удалить атрибуты канала
	DB->BeginTransaction();
	query.Format(_T("DELETE FROM ChannelAttributes WHERE ChannelID=%u;"), ID);
	DO_QUERY_DB_TRANS(DB,query);

	//Удалить события канала
	query.Format(_T("DELETE FROM Events WHERE ChannelID=%u;"), ID);
	DO_QUERY_DB_TRANS(DB,query);

	//Удалить данные канала
	query.Format(_T("DELETE FROM BooleanChannelData WHERE ChannelID=%u;"), ID);
	DO_QUERY_DB_TRANS(DB,query);

	//Удалить данные канала
	query.Format(_T("DELETE FROM DWordChannelData WHERE ChannelID=%u;"), ID);
	DO_QUERY_DB_TRANS(DB,query);

	//Удалить данные канала
	query.Format(_T("DELETE FROM FloatChannelData WHERE ChannelID=%u;"), ID);
	DO_QUERY_DB_TRANS(DB,query);

	//Удалить канал
	query.Format(_T("DELETE FROM Channels WHERE ID=%u;"), ID);
	DO_QUERY_DB_TRANS(DB,query);

	//Определить количество каналов на данном сервере
	query.Format(_T("SELECT ID FROM Channels WHERE ServerID=%u"), ServerID);
	DO_QUERY_RS_TRANS(DB, rs, query);
	const ULONG SomeServers = rs.GetRecordCount();
	rs.Close();

	//Нет каналов привязанных к серверу. Удаляем его
	if(SomeServers == 0)
	{
		query.Format(_T("DELETE FROM Servers WHERE ID=%u;"), ServerID);
		DO_QUERY_DB_TRANS(DB,query);
	}
	DB->CommitTransaction();

	return S_OK;
}

STDMETHODIMP CChannels::GetChannelsID(IVariantArray** pVal)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanViewData())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CoVariantEnumerator *en;
	HRESULT hr;
	CVariantEnumerator::TVariants data;
	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);

	DO_QUERY_RS(DB, rs, _T("SELECT ID FROM Channels;"));
	while(!rs.IsEOF())
	{
		CComVariant v;
		rs.GetFieldValue(_T("ID"), v);
		v.ChangeType(VT_UI4);
		data.push_back(v);
		rs.MoveNext();
	}
	rs.Close();

	hr = CoVariantEnumerator::CreateInstance(&en);
	if(FAILED(hr))
	{
		_dwLastError = ERR_CANT_CREATE_OBJECT;
		return E_FAIL;
	}
	en->AddRef();
	en->SetData(data);
	
	hr = en->QueryInterface(__uuidof(IVariantArray), (void**)pVal);
	en->Release();
	if(FAILED(hr))
	{
		_dwLastError = ERR_CANT_CREATE_OBJECT;
		return E_FAIL;
	}
	return S_OK;
}


STDMETHODIMP CChannels::get_Active(ULONG ChannelID, VARIANT_BOOL* pVal)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanViewData())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;

	query.Format(_T("SELECT ID FROM Channels WHERE ID=%u;"),ChannelID);
	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount()<=0)
	{
		rs.Close();
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}
	rs.Close();

	if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
		query.Format(_T("SELECT [Value] FROM ChannelAttributes WHERE ChannelID=%u AND Attribute='active';"), ChannelID);
	else
		query.Format(_T("SELECT Value FROM ChannelAttributes WHERE ChannelID=%u AND Attribute='active';"), ChannelID);

	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount()>0)
	{
		CComVariant v;
		rs.GetFieldValue(_T("Value"), v);
		v.ChangeType(VT_BSTR);
		*pVal = ConvertValToBool((LPCTSTR)(_bstr_t)v.bstrVal)?VARIANT_TRUE:VARIANT_FALSE;
	}
	else
		*pVal = VARIANT_FALSE;
	rs.Close();

	return S_OK;
}

STDMETHODIMP CChannels::put_Active(ULONG ChannelID, VARIANT_BOOL newVal)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanEditChannels())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;

	query.Format(_T("SELECT ID FROM Channels WHERE ID=%u;"),ChannelID);
	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount()<=0)
	{
		rs.Close();
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}
	rs.Close();

	if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
		query.Format(_T("SELECT [Value] FROM ChannelAttributes WHERE ChannelID=%u AND Attribute='active';"), ChannelID);
	else
		query.Format(_T("SELECT Value FROM ChannelAttributes WHERE ChannelID=%u AND Attribute='active';"), ChannelID);

	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount()>0)
	{
		if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
			query.Format(_T("UPDATE ChannelAttributes SET  [Value]='%s' WHERE ChannelID=%u AND Attribute='active';"),
				newVal?"Y":"N",ChannelID);
		else
			query.Format(_T("UPDATE ChannelAttributes SET  Value='%s' WHERE ChannelID=%u AND Attribute='active';"),
				newVal?"Y":"N",ChannelID);
	}
	else
	{
		if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
			query.Format(_T("INSERT INTO ChannelAttributes(ChannelID, Attribute, [Value]) VALUES(%u, 'active', '%s');"),
				ChannelID, newVal?"Y":"N");
		else
			query.Format(_T("INSERT INTO ChannelAttributes(ChannelID, Attribute, Value) VALUES(%u, 'active', '%s');"),
				ChannelID, newVal?"Y":"N");
	}
	rs.Close();

	DB->BeginTransaction();
	DO_QUERY_DB_TRANS(DB,query);
	DB->CommitTransaction();
	return S_OK;
}
STDMETHODIMP CChannels::get_UpdateInterval(ULONG ChannelID, ULONG* pVal)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanViewData())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;

	query.Format(_T("SELECT ID FROM Channels WHERE ID=%u;"),ChannelID);
	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount()<=0)
	{
		rs.Close();
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}
	rs.Close();

	if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
		query.Format(_T("SELECT [Value] FROM ChannelAttributes WHERE ChannelID=%u AND Attribute='updateinterval';"), ChannelID);
	else
		query.Format(_T("SELECT Value FROM ChannelAttributes WHERE ChannelID=%u AND Attribute='updateinterval';"), ChannelID);

	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount()>0)
	{
		CComVariant v;
		rs.GetFieldValue(_T("Value"), v);
		v.ChangeType(VT_UI4);
		*pVal = v.ulVal;
	}
	else
		*pVal = 0;
	rs.Close();

	return S_OK;
}

STDMETHODIMP CChannels::put_UpdateInterval(ULONG ChannelID, ULONG newVal)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanEditChannels())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;

	query.Format(_T("SELECT ID FROM Channels WHERE ID=%u;"),ChannelID);
	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount()<=0)
	{
		rs.Close();
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}
	rs.Close();

	if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
		query.Format(_T("SELECT [Value] FROM ChannelAttributes WHERE ChannelID=%u AND Attribute='updateinterval';"), ChannelID);
	else
		query.Format(_T("SELECT Value FROM ChannelAttributes WHERE ChannelID=%u AND Attribute='updateinterval';"), ChannelID);

	DO_QUERY_RS(DB, rs, query);
	CString tmp;
	tmp.Format(_T("%u"),newVal);
	if(rs.GetRecordCount()>0)
	{
		if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
			query.Format(_T("UPDATE ChannelAttributes SET  [Value]='%s' WHERE ChannelID=%u AND Attribute='updateinterval';"),
			ChannelID, tmp);
		else
			query.Format(_T("UPDATE ChannelAttributes SET  Value='%s' WHERE ChannelID=%u AND Attribute='updateinterval';"),
			ChannelID, tmp);
	}
	else
	{
		if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
			query.Format(_T("INSERT INTO ChannelAttributes(ChannelID, Attribute, [Value]) VALUES(%u, 'updateinterval', '%s');"),
			ChannelID, tmp);
		else
			query.Format(_T("INSERT INTO ChannelAttributes(ChannelID, Attribute, Value) VALUES(%u, 'updateinterval', '%s');"),
			ChannelID, tmp);
	}
	rs.Close();

	DB->BeginTransaction();
	DO_QUERY_DB_TRANS(DB,query);
	DB->CommitTransaction();
	return S_OK;
}

STDMETHODIMP CChannels::GetChannelsList(BSTR FilterServer, BSTR FilterComputer, BSTR FilterName, IDataSet** pRecordSet)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanViewData())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CString			query;
	CString			filter;

	query = "SELECT Channels.ID, Servers.ServerGUID, Servers.Computer, Channels.Name \
			FROM Servers INNER JOIN Channels ON Servers.ID = Channels.ServerID ";
	if(CComBSTR(FilterServer).Length()>0)
	{
		CLSID clsidTmp;
		if(SUCCEEDED(CLSIDFromString(FilterServer, &clsidTmp)))
		{
			LPOLESTR strTmp;
			if(SUCCEEDED(StringFromCLSID(clsidTmp, &strTmp)))
			{
				CString tmp;
				tmp.Format(_T("(Servers.ServerGUID='%s')"), CString(strTmp));
				LocalFree(strTmp);
				if(!filter.IsEmpty())
					filter += " AND ";
				filter += tmp;
			}
		}
	}

	if(CComBSTR(FilterComputer).Length()>0)
	{
		CString tmp;
		tmp.Format(_T("(Servers.Computer='%s')"), CString(FilterComputer));
		if(!filter.IsEmpty())
			filter += " AND ";
		filter += tmp;
	}
	if(CComBSTR(FilterName).Length()>0)
	{
		CString tmp;
		tmp.Format(_T("(Channels.Name='%s')"), CString(FilterName));
		if(!filter.IsEmpty())
			filter += " AND ";
		filter += tmp;
	}

	if(!filter.IsEmpty())
	{
		query += " WHERE ";
		query += filter;
	}
	query+="ORDER BY Channels.Name;";
	

	CoDataSet *ds;
	HRESULT hr;
	hr = CoDataSet::CreateInstance(&ds);
	if(FAILED(hr))
	{
		_dwLastError = ERR_CANT_CREATE_OBJECT;
		return E_FAIL;
	}
	ds->AddRef();
	CADODatabase *DB = _Owner->GetConnection();
	CADORecordset rs(DB);
	if(!rs.Open(query))
	{
		ds->Release();
		_dwLastError	= ERR_CUSTOM;
		_strLastError.Format(_T("Ошибка при выполнении запроса (%s)"), DB->GetLastErrorString().c_str());
		
		return E_FAIL;
	}
	if(!rs.Clone(ds->m_Recordset))
	{
		rs.Close();
		ds->Release();
		_dwLastError	= ERR_CUSTOM;
		_strLastError.Format(_T("Ошибка при выполнении запроса (%s)"), DB->GetLastErrorString().c_str());
		
		return E_FAIL;
	}

	hr = ds->QueryInterface(__uuidof(IDataSet), (void**)pRecordSet);
	ds->Release();
	if(FAILED(hr))
	{
		_dwLastError = ERR_CANT_CREATE_OBJECT;
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CChannels::get_Attribute(ULONG ChannelID, BSTR AttributeName, VARIANT* pVal)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanViewData())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;

	query.Format(_T("SELECT ID FROM Channels WHERE ID=%u;"),ChannelID);
	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount()<=0)
	{
		rs.Close();
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}
	rs.Close();

	CString tmp(AttributeName);
	tmp.MakeLower();
	if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
		query.Format(_T("SELECT [Value] FROM ChannelAttributes WHERE ChannelID=%u AND Attribute='%s';"), ChannelID,tmp);
	else
		query.Format(_T("SELECT Value FROM ChannelAttributes WHERE ChannelID=%u AND Attribute='%s';"), ChannelID,tmp);

	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount()>0)
	{
		CComVariant v;
		rs.GetFieldValue(_T("Value"), v);
		VariantInit(pVal);
		VariantCopy(pVal, &v);
	}
	else
		VariantInit(pVal);
	rs.Close();

	return S_OK;
}

STDMETHODIMP CChannels::put_Attribute(ULONG ChannelID, BSTR AttributeName, VARIANT newVal)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanEditChannels())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;

	query.Format(_T("SELECT ID FROM Channels WHERE ID=%u;"),ChannelID);
	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount()<=0)
	{
		rs.Close();
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}
	rs.Close();

	CString Attrib(AttributeName);
	Attrib.MakeLower();
	if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
		query.Format(_T("SELECT [Value] FROM ChannelAttributes WHERE ChannelID=%u AND Attribute='%s';"), ChannelID,Attrib);
	else
		query.Format(_T("SELECT Value FROM ChannelAttributes WHERE ChannelID=%u AND Attribute='%s';"), ChannelID,Attrib);

	DO_QUERY_RS(DB, rs, query);
	CComVariant v(newVal);
	v.ChangeType(VT_BSTR);
	CString tmp(v.bstrVal);
	if(rs.GetRecordCount()>0)
	{
		if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
			query.Format(_T("UPDATE ChannelAttributes SET  [Value]='%s' WHERE ChannelID=%u AND Attribute='%s';"),
				tmp, ChannelID, Attrib);
		else
			query.Format(_T("UPDATE ChannelAttributes SET  Value='%s' WHERE ChannelID=%u AND Attribute='%s';"),
				tmp, ChannelID, Attrib);
	}
	else
	{
		if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
			query.Format(_T("INSERT INTO ChannelAttributes(ChannelID, Attribute, [Value]) VALUES(%u, '%s', '%s');"),
			ChannelID, Attrib, tmp);
		else
			query.Format(_T("INSERT INTO ChannelAttributes(ChannelID, Attribute, Value) VALUES(%u, '%s', '%s');"),
			ChannelID, Attrib, tmp);
	}
	rs.Close();

	DB->BeginTransaction();
	DO_QUERY_DB_TRANS(DB,query);
	DB->CommitTransaction();
	return S_OK;
}

STDMETHODIMP CChannels::AddData(ULONG ChannelID, VARIANT DateTime, ULONG Quality, VARIANT Data)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanEditData())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	//Если дата не задана или задана неверно устанавливаем
	//текущую дату
	if(DateTime.vt != VT_DATE)
	{
		SYSTEMTIME st;
		GetSystemTime(&st);
		VariantInit(&DateTime);
		SystemTimeToVariantTime(&st, &DateTime.date);
		DateTime.vt = VT_DATE;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;

	query.Format(_T("SELECT ID FROM Channels WHERE ID=%u;"),ChannelID);
	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount()<=0)
	{
		rs.Close();
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}
	rs.Close();

	CComVariant v(Data);

	CString TableName;
	CString Values;
	CString sTime = COleDateTime(DateTime.date).Format(_T("%Y-%m-%d %H:%M:%S"));
	switch(v.vt)
	{
	case VT_I1:
	case VT_I2:
	case VT_I4:
	case VT_UI1:
	case VT_UI2:
	case VT_UI4:
		v.ChangeType(VT_UI4);
		TableName = _T("DWordChannelData");
		Values.Format(_T("VALUES(%u, '%s', %u, %d)"),ChannelID, sTime, v.ulVal, Quality);
		break;
	case VT_BOOL:
		TableName = _T("BooleanChannelData");
		Values.Format(_T("VALUES(%u, '%s', %u, %d)"),ChannelID, sTime, v.boolVal?TRUE:FALSE, Quality);
		break;
	case VT_R4:
	case VT_R8:
		v.ChangeType(VT_R8);
		TableName = _T("FloatChannelData");
		Values.Format(_T("VALUES(%u, '%s', %f, %d)"),ChannelID, sTime, v.dblVal, Quality);
		break;
	default:
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}

	if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
		query.Format(_T("INSERT INTO %s(ChannelID, EventTime, [Value], Quality) %s;"),TableName,Values);
	else
		query.Format(_T("INSERT INTO %s(ChannelID, EventTime, Value, Quality) %s;"),TableName,Values);

	DB->BeginTransaction();
	DO_QUERY_DB_TRANS(DB,query);
	DB->CommitTransaction();

	return S_OK;
}

STDMETHODIMP CChannels::GetChannelData(VARIANT FilterFromDate, VARIANT FilterToDate, VARIANT FilterQuality, enChannelDataTypes DataType, ULONG ChannelID, IDataSet** pVal)
{
	CString Filter;
	Filter.Format(_T("(ChannelID=%u)"), ChannelID);

	if(FilterFromDate.vt == VT_DATE)
	{
		CString sTime = COleDateTime(FilterFromDate.date).Format(_T("%Y-%m-%d %H:%M:%S"));
		CString tmp;
		if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
			tmp.Format(_T("EventTime >= #%s#"), sTime);
		else
			tmp.Format(_T("EventTime >= '%s'"), sTime);
		if(!Filter.IsEmpty())
			Filter += " AND ";
		Filter += tmp;
	}

	if(FilterToDate.vt == VT_DATE)
	{
		CString sTime = COleDateTime(FilterToDate.date).Format(_T("%Y-%m-%d %H:%M:%S"));
		CString tmp;
		if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
			tmp.Format(_T("EventTime <= #%s#"), sTime);
		else
			tmp.Format(_T("EventTime <= '%s'"), sTime);
		if(!Filter.IsEmpty())
			Filter += " AND ";
		Filter += tmp;
	}

	if(FilterQuality.vt == VT_UI2 || FilterQuality.vt == VT_UI4 || FilterQuality.vt == VT_I2 || FilterQuality.vt == VT_I4)
	{
		CComVariant v = FilterQuality;
		v.ChangeType(VT_UI4);

		CString tmp;
		tmp.Format(_T("Quality = %u"), v.ulVal);
		if(!Filter.IsEmpty())
			Filter += " AND ";
		Filter += tmp;
	}

    CString TableName;
	switch(DataType)
	{
	case CDT_BOOLEAN:	TableName = "BooleanChannelData";	break;
	case CDT_DWORD:		TableName = "DWordChannelData";		break;
	case CDT_FLOAT:		TableName = "FloatChannelData";		break;
	default:
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	};

	CString query;
	if(Filter.IsEmpty())
		query.Format(_T("SELECT * FROM %s;"), TableName);
	else
		query.Format(_T("SELECT * FROM %s WHERE %s;"),TableName, Filter);

	CoDataSet *ds;
	HRESULT hr;
	hr = CoDataSet::CreateInstance(&ds);
	if(FAILED(hr))
	{
		_dwLastError = ERR_CANT_CREATE_OBJECT;
		return E_FAIL;
	}
	ds->AddRef();
	CADODatabase *DB = _Owner->GetConnection();
	CADORecordset rs(DB);
	if(!rs.Open(query))
	{
		ds->Release();
		_dwLastError	= ERR_CUSTOM;
		_strLastError.Format(_T("Ошибка при выполнении запроса (%s)"), DB->GetLastErrorString().c_str());
		
		return E_FAIL;
	}
	if(!rs.Clone(ds->m_Recordset))
	{
		rs.Close();
		ds->Release();
		_dwLastError	= ERR_CUSTOM;
		_strLastError.Format(_T("Ошибка при выполнении запроса (%s)"), DB->GetLastErrorString().c_str());
		
		return E_FAIL;
	}

	hr = ds->QueryInterface(__uuidof(IDataSet), (void**)pVal);
	ds->Release();
	if(FAILED(hr))
	{
		_dwLastError = ERR_CANT_CREATE_OBJECT;
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CChannels::GetChannelInfo(ULONG ID, BSTR* ServerCLSID, BSTR* ComputerName, BSTR* ChannelName)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanViewData())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;

	query.Format(_T("SELECT Servers.ServerGUID, Servers.Computer, Channels.Name, Channels.ID \
				FROM Servers INNER JOIN Channels ON Servers.ID = Channels.ServerID \
				WHERE (((Channels.ID)=%u));"), ID);

	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount()<=0)
	{
		rs.Close();
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}

	CComVariant v;
	rs.GetFieldValue(_T("ServerGUID"), v);
	v.ChangeType(VT_BSTR);
	CComBSTR(v.bstrVal).CopyTo(ServerCLSID);

	rs.GetFieldValue(_T("Computer"), v);
	v.ChangeType(VT_BSTR);
	CComBSTR(v.bstrVal).CopyTo(ComputerName);
	
	rs.GetFieldValue(_T("Name"), v);
	v.ChangeType(VT_BSTR);
	CComBSTR(v.bstrVal).CopyTo(ChannelName);

	rs.Close();

	return S_OK;
}

STDMETHODIMP CChannels::GetChannelsDataAsString(VARIANT IDs, VARIANT FilterFromDate, VARIANT FilterToDate, IDataSet** pVal)
{
	if(!(IDs.vt & VT_ARRAY))
		return E_INVALIDARG;

	CString Filter;

	if(FilterFromDate.vt == VT_DATE)
	{
		CString sTime = COleDateTime(FilterFromDate.date).Format(_T("%Y-%m-%d %H:%M:%S"));
		CString tmp;
		if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
			tmp.Format(_T("EventTime >= #%s#"), sTime);
		else
			tmp.Format(_T("EventTime >= '%s'"), sTime);
		Filter += tmp;
	}

	if(FilterToDate.vt == VT_DATE)
	{
		CString sTime = COleDateTime(FilterToDate.date).Format(_T("%Y-%m-%d %H:%M:%S"));
		CString tmp;
		if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
			tmp.Format(_T("EventTime <= #%s#"), sTime);
		else
			tmp.Format(_T("EventTime <= '%s'"), sTime);
		if(!Filter.IsEmpty())
			Filter += " AND ";
		Filter += tmp;
	}

	//Условие выбора каналов по ID канала
	CString Tags;
	CComSafeArray<ULONG> sa(IDs.parray);
	for(LONG i=0 ; i<(LONG)sa.GetCount(); i++)
	{
		CString tmp;
		tmp.Format(_T("(ChannelID = %u)"), sa[i]);
		if(Tags.IsEmpty())
			Tags += tmp;
		else
			Tags += _T(" OR ") + tmp;
	}

	CString TableName;
	TableName.Format(_T("temp_table_%X"), GetTickCount());

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;

	//Создаем временную таблицу
	query.Format(_T("CREATE TABLE  %s ( Name VARCHAR(255), Val VARCHAR(255), EventTime DATETIME, Quality INTEGER);"), TableName);
	
	DO_QUERY_DB(DB, query);

	//Добавляем во временную таблицу данные битовых каналов
	query.Format(_T("INSERT INTO %s ( Name, Val, EventTime, Quality )	\
				 SELECT Channels.Name, BooleanChannelData.Value, BooleanChannelData.EventTime, BooleanChannelData.Quality \
				 FROM Channels, BooleanChannelData  WHERE (BooleanChannelData.ChannelID = Channels.ID) AND (%s)"),
				 TableName, Tags);
	if(!Filter.IsEmpty())
		query += _T(" AND (") + Filter + _T(");");
	else
		query += _T(";");
	DO_QUERY_DB(DB, query);

	//Добавляем во временную таблицу данные целых каналов
	query.Format(_T("INSERT INTO %s ( Name, Val, EventTime, Quality )	\
				 SELECT Channels.Name, DWordChannelData.Value, DWordChannelData.EventTime, DWordChannelData.Quality \
				 FROM Channels, DWordChannelData  WHERE (DWordChannelData.ChannelID = Channels.ID) AND (%s)"),
				 TableName, Tags);
	if(!Filter.IsEmpty())
		query += _T(" AND (") + Filter + _T(");");
	else
		query += _T(";");
	DO_QUERY_DB(DB, query);

	//Добавляем во временную таблицу данные вещественных каналов
	query.Format(_T("INSERT INTO %s ( Name, Val, EventTime, Quality )	\
					SELECT Channels.Name, FloatChannelData.Value, FloatChannelData.EventTime, FloatChannelData.Quality \
					FROM Channels, FloatChannelData  WHERE (FloatChannelData.ChannelID = Channels.ID) AND (%s)"),
					TableName,Tags);
	if(!Filter.IsEmpty())
		query += _T(" AND (") + Filter + _T(");");
	else
		query += _T(";");
	DO_QUERY_DB(DB, query);
	
	//Получить список каналов, отсортированный по дате
	query = _T("SELECT * FROM ") + TableName + _T(" ORDER BY EventTime;");

	CoDataSetEx *ds;
	HRESULT hr;
	hr = CoDataSetEx::CreateInstance(&ds);
	if(FAILED(hr))
	{
		_dwLastError = ERR_CANT_CREATE_OBJECT;
		return E_FAIL;
	}
	ds->AddRef();

	if(!rs.Open(query))
	{
		ds->Release();
		_dwLastError	= ERR_CUSTOM;
		_strLastError.Format(_T("Ошибка при выполнении запроса (%s)"), DB->GetLastErrorString().c_str());
		
		return E_FAIL;
	}
	ds->m_TempTableName = TableName;
	ds->m_DB = DB;
	if(!rs.Clone(ds->m_Recordset))
	{
		rs.Close();
		ds->Release();
		_dwLastError	= ERR_CUSTOM;
		_strLastError.Format(_T("Ошибка при выполнении запроса (%s)"), DB->GetLastErrorString().c_str());
		
		return E_FAIL;
	}

	hr = ds->QueryInterface(__uuidof(IDataSet), (void**)pVal);
	ds->Release();
	if(FAILED(hr))
	{
		_dwLastError = ERR_CANT_CREATE_OBJECT;
		return E_FAIL;
	}

	return S_OK;
}
