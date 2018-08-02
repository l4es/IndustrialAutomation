// Events.cpp : Implementation of CEvents

#include "stdafx.h"
#include "Events.h"
#include "DBConnection.h"
#include "Helpers.h"
#include <atlsafe.h>
#include "Dataset.h"
#include "ApplicationSettings.h"

using namespace DB;
using namespace Helpers;

// CEvents

STDMETHODIMP CEvents::get_ErrorDescription(BSTR* pVal)
{
	

	if(_dwLastError == ERR_CUSTOM)
		return CComBSTR(_strLastError).CopyTo(pVal);

	CString s;
	s = Helpers::DecodeError(_dwLastError);

	return CComBSTR(s).CopyTo(pVal);
}

STDMETHODIMP CEvents::get_ErrorCode(enErrorCodes* pVal)
{
	

	*pVal = static_cast<enErrorCodes>(_dwLastError);
	if(_dwLastError == ERR_CUSTOM)
		return S_FALSE;
	else
		return S_OK;
}

HRESULT CEvents::FinalConstruct()
{
	return S_OK;
}

void CEvents::FinalRelease() 
{
	if(_Owner)
	{
		_Owner->Release();
		_Owner = NULL;
	}
}

HRESULT CEvents::Initialize(CDBConnection *Owner)
{
	_Owner = Owner;
	if(_Owner)
		_Owner->AddRef();
	else
		return E_FAIL;
	return S_OK;
}
STDMETHODIMP CEvents::AddEvent(ULONG ChannelID, ULONG EventType, VARIANT Date)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanEditEvents())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	//Если дата не задана или задана неверно устанавливаем
	//текущую дату
	if(Date.vt != VT_DATE)
	{
		SYSTEMTIME st;
		GetSystemTime(&st);
		VariantInit(&Date);
		SystemTimeToVariantTime(&st, &Date.date);
		Date.vt = VT_DATE;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;

	query.Format(_T("SELECT ID FROM EventTypes WHERE ID=%u;"),EventType);
	DO_QUERY_RS(DB, rs, query);

	if(rs.GetRecordCount()<=0)
	{
		rs.Close();
		_dwLastError = ERR_NOT_FOUND;
		return E_INVALIDARG;
	}
	rs.Close();

	query.Format(_T("SELECT ID FROM Channels WHERE ID=%u;"),ChannelID);
	DO_QUERY_RS(DB, rs, query);

	if(rs.GetRecordCount()<=0)
	{
		rs.Close();
		_dwLastError = ERR_NOT_FOUND;
		return E_INVALIDARG;
	}
	rs.Close();

	CString sTime = COleDateTime(Date.date).Format(_T("%Y-%m-%d %H:%M:%S"));

	query.Format(_T("INSERT INTO Events (ChannelID, TypeID, EventTime) VALUES(%u, %u, '%s');"),
		ChannelID, EventType, sTime);
	DB->BeginTransaction();
	DO_QUERY_DB_TRANS(DB, query);
	DB->CommitTransaction();

	return S_OK;
}

STDMETHODIMP CEvents::DeleteEvent(ULONG ChannelID, ULONG TypeID, VARIANT FilterFromDate, VARIANT FilterToDate)
{
	CString Filter;

	//Добавляем фильтр по начальной дате
	if(FilterFromDate.vt == VT_DATE)
	{
		CString sTime = COleDateTime(FilterFromDate.date).Format(_T("%Y-%m-%d %H:%M:%S"));
		CString tmp;
		tmp.Format(_T("(EventTime >= '%s')"), sTime);

		if(!Filter.IsEmpty())
			Filter += _T(" AND ");
		Filter += tmp;
	}

	//Добавляем фильтр по конечной дате
	if(FilterToDate.vt == VT_DATE)
	{
		CString sTime = COleDateTime(FilterFromDate.date).Format(_T("%Y-%m-%d %H:%M:%S"));
		CString tmp;
		tmp.Format(_T("(EventTime <= '%s')"), sTime);
		if(!Filter.IsEmpty())
			Filter += " AND ";
		Filter += tmp;
	}

	CString query;
	if(Filter.IsEmpty())
		query.Format(_T("DELETE FROM Events WHERE ChannelID=%u AND TypeID=%u;"),ChannelID, TypeID);
	else
		query.Format(_T("DELETE FROM Events WHERE ChannelID=%u AND TypeID=%u AND (%s);"),ChannelID, TypeID, Filter);

	CADODatabase*	DB = _Owner->GetConnection();

	DB->BeginTransaction();
	DO_QUERY_DB_TRANS(DB,query);
	DB->CommitTransaction();

	return S_OK;
}


STDMETHODIMP CEvents::DeleteAllEvents(ULONG ChannelID, VARIANT FilterFromDate, VARIANT FilterToDate)
{
	CString Filter;

	//Добавляем фильтр по начальной дате
	if(FilterFromDate.vt == VT_DATE)
	{
		CString sTime = COleDateTime(FilterFromDate.date).Format(_T("%Y-%m-%d %H:%M:%S"));
		CString tmp;
		tmp.Format(_T("(EventTime >= '%s')"), sTime);

		if(!Filter.IsEmpty())
			Filter += _T(" AND ");
		Filter += tmp;
	}

	//Добавляем фильтр по конечной дате
	if(FilterToDate.vt == VT_DATE)
	{
		CString sTime = COleDateTime(FilterFromDate.date).Format(_T("%Y-%m-%d %H:%M:%S"));
		CString tmp;
		tmp.Format(_T("(EventTime <= '%s')"), sTime);
		if(!Filter.IsEmpty())
			Filter += " AND ";
		Filter += tmp;
	}

	CString query;
	if(Filter.IsEmpty())
		query.Format(_T("DELETE FROM Events WHERE ChannelID=%u;"),ChannelID);
	else
		query.Format(_T("DELETE FROM Events WHERE ChannelID=%u AND (%s);"),ChannelID, Filter);

	CADODatabase*	DB = _Owner->GetConnection();

	DB->BeginTransaction();
	DO_QUERY_DB_TRANS(DB,query);
	DB->CommitTransaction();

	return S_OK;
}

STDMETHODIMP CEvents::GetEventsList(ULONG ChannelID, VARIANT FilterTypeID, VARIANT FilterFromDate, VARIANT FilterToDate, IDataSet** pVal)
{
	CString Filter;
	Filter.Format(_T("(ChannelID = %u)"), ChannelID);

	//Добавляем фильтр по типу события
	if(FilterTypeID.vt != VT_EMPTY)
	{
		CString TypeIDFlt;
		if(FilterTypeID.vt == (VT_ARRAY|VT_UI4))
		{
			CComSafeArray<ULONG, VT_UI4> sa(FilterTypeID.parray);
			ULONG size = sa.GetCount();
			for(ULONG i=0;i<size;i++)
			{
				CString tmp;
				tmp.Format(_T("(TypeID=%u)"), sa.GetAt((LONG)i));
				if(!TypeIDFlt.IsEmpty())
					TypeIDFlt += _T(" OR ");
				TypeIDFlt += tmp;
			}
			if(!Filter.IsEmpty())
				Filter += _T(" AND ");
			Filter += _T("(") + TypeIDFlt + _T(") ");
		}
		else
		{
			CComVariant v(FilterTypeID);
			v.ChangeType(VT_UI4);
			CString tmp;
			tmp.Format(_T("TypeID=%u"), v.ulVal);
			if(!Filter.IsEmpty())
				Filter += _T(" AND ");
			Filter += _T("(") + tmp + _T(") ");
		}
	}

	
	//Добавляем фильтр по начальной дате
	if(FilterFromDate.vt == VT_DATE)
	{
		CString sTime = COleDateTime(FilterFromDate.date).Format(_T("%Y-%m-%d %H:%M:%S"));
		CString tmp;
		if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
			tmp.Format(_T("(EventTime >= #%s#)"), sTime);
		else
			tmp.Format(_T("(EventTime >= '%s')"), sTime);

		if(!Filter.IsEmpty())
			Filter += _T(" AND ");
		Filter += tmp;
	}

	//Добавляем фильтр по конечной дате
	if(FilterToDate.vt == VT_DATE)
	{
		CString sTime = COleDateTime(FilterToDate.date).Format(_T("%Y-%m-%d %H:%M:%S"));
		CString tmp;
		if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
			tmp.Format(_T("(EventTime <= #%s#)"), sTime);
		else
			tmp.Format(_T("(EventTime <= '%s')"), sTime);
		if(!Filter.IsEmpty())
			Filter += " AND ";
		Filter += tmp;
	}

	CString query;
	if(FilterTypeID.vt == VT_EMPTY)
	{
		if(Filter.IsEmpty())
			query = _T("SELECT TypeID,EventTime FROM Events;");
		else
			query.Format(_T("SELECT TypeID,EventTime FROM Events WHERE %s;"),Filter);
	}
	else
	{
		if(Filter.IsEmpty())
			query = _T("SELECT EventTime FROM Events;");
		else
			query.Format(_T("SELECT EventTime FROM Events WHERE %s;"),Filter);
	}

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
