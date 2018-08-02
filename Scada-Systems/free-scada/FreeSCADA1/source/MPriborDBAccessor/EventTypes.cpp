// EventTypes.cpp : Implementation of CEventTypes

#include "stdafx.h"
#include "EventTypes.h"
#include "DBConnection.h"
#include "Helpers.h"

using namespace DB;
using namespace Helpers;


// CEventTypes

STDMETHODIMP CEventTypes::get_ErrorDescription(BSTR* pVal)
{
	

	if(_dwLastError == ERR_CUSTOM)
		return CComBSTR(_strLastError).CopyTo(pVal);

	CString s;
	s = Helpers::DecodeError(_dwLastError);

	return CComBSTR(s).CopyTo(pVal);
}

STDMETHODIMP CEventTypes::get_ErrorCode(enErrorCodes* pVal)
{
	

	*pVal = static_cast<enErrorCodes>(_dwLastError);
	if(_dwLastError == ERR_CUSTOM)
		return S_FALSE;
	else
		return S_OK;
}

HRESULT CEventTypes::FinalConstruct()
{
	return S_OK;
}

void CEventTypes::FinalRelease() 
{
	if(_Owner)
	{
		_Owner->Release();
		_Owner = NULL;
	}
}

HRESULT CEventTypes::Initialize(CDBConnection *Owner)
{
	_Owner = Owner;
	if(_Owner)
		_Owner->AddRef();
	else
		return E_FAIL;
	return S_OK;
}

STDMETHODIMP CEventTypes::AddEventType(ULONG ID, BSTR Name, BSTR Description)
{
	
	_dwLastError = ERR_OK;

	if(CComBSTR(Name).Length() <= 0)
	{
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}
	if(!_Owner->GetCurrentUser().CanEditEvents())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;

	query.Format(_T("SELECT ID FROM EventTypes WHERE ID=%u"),ID);
	DO_QUERY_RS(DB, rs, query);

	if(rs.GetRecordCount()>0)
	{
		rs.Close();
		_dwLastError = ERR_DUBLICATE;
		return E_INVALIDARG;
	}
	
	query.Format(_T("INSERT INTO EventTypes (ID, Name, Description) VALUES(%u, '%s', '%s');"),
		ID, (LPCTSTR)(_bstr_t)Name, (LPCTSTR)(_bstr_t)Description);
	DB->BeginTransaction();
	DO_QUERY_DB_TRANS(DB, query);
	DB->CommitTransaction();

	return S_OK;
}

STDMETHODIMP CEventTypes::DeleteEventType(ULONG ID)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanEditEvents())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CString			query;

	DB->BeginTransaction();
	query.Format(_T("DELETE FROM Events WHERE TypeID=%u"),ID);
	DO_QUERY_DB_TRANS(DB, query);
	query.Format(_T("DELETE FROM EventTypes WHERE ID=%u"),ID);
	DO_QUERY_DB_TRANS(DB, query);
	DB->RollbackTransaction();

	return S_OK;
}

STDMETHODIMP CEventTypes::get_EventTypeName(ULONG ID, BSTR* pVal)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanViewEvents())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;

	query.Format(_T("SELECT Name FROM EventTypes WHERE ID=%u"),ID);
	DO_QUERY_RS(DB, rs, query);

	if(rs.GetRecordCount()<=0)
	{
		rs.Close();
		_dwLastError = ERR_NOT_FOUND;
		return S_FALSE;
	}

	rs.MoveFirst();
	CComVariant v;
	rs.GetFieldValue(_T("Name"), v);
	rs.Close();

	v.ChangeType(VT_BSTR);
	if(FAILED(CComBSTR(v.bstrVal).CopyTo(pVal)))
	{
		_dwLastError = ERR_CRITICAL;
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CEventTypes::put_EventTypeName(ULONG ID, BSTR newVal)
{
	
	_dwLastError = ERR_OK;

	if(CComBSTR(newVal).Length() <= 0)
	{
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}
	if(!_Owner->GetCurrentUser().CanEditEvents())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;

	query.Format(_T("SELECT ID FROM EventTypes WHERE ID=%u"),ID);
	DO_QUERY_RS(DB, rs, query);

	if(rs.GetRecordCount()<=0)
	{
		rs.Close();
		_dwLastError = ERR_NOT_FOUND;
		return E_INVALIDARG;
	}
	rs.Close();

	query.Format(_T("UPDATE EventTypes SET Name='%s' WHERE ID=%u;"), (LPCTSTR)(_bstr_t)newVal, ID);
	DB->BeginTransaction();
	DO_QUERY_DB_TRANS(DB, query);
	DB->CommitTransaction();

	return S_OK;
}

STDMETHODIMP CEventTypes::get_EventTypeDescription(ULONG ID, BSTR* pVal)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanViewEvents())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;

	query.Format(_T("SELECT Description FROM EventTypes WHERE ID=%u"),ID);
	DO_QUERY_RS(DB, rs, query);

	if(rs.GetRecordCount()<=0)
	{
		rs.Close();
		_dwLastError = ERR_NOT_FOUND;
		return S_FALSE;
	}

	rs.MoveFirst();
	CComVariant v;
	rs.GetFieldValue(_T("Description"), v);
	rs.Close();

	v.ChangeType(VT_BSTR);
	if(FAILED(CComBSTR(v.bstrVal).CopyTo(pVal)))
	{
		_dwLastError = ERR_CRITICAL;
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CEventTypes::put_EventTypeDescription(ULONG ID, BSTR newVal)
{
	
	_dwLastError = ERR_OK;

	if(CComBSTR(newVal).Length() <= 0)
	{
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}
	if(!_Owner->GetCurrentUser().CanEditEvents())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;

	query.Format(_T("SELECT ID FROM EventTypes WHERE ID=%u"),ID);
	DO_QUERY_RS(DB, rs, query);

	if(rs.GetRecordCount()<=0)
	{
		rs.Close();
		_dwLastError = ERR_NOT_FOUND;
		return E_INVALIDARG;
	}
	rs.Close();

	query.Format(_T("UPDATE EventTypes SET Description='%s' WHERE ID=%u;"), (LPCTSTR)(_bstr_t)newVal, ID);
	DB->BeginTransaction();
	DO_QUERY_DB_TRANS(DB, query);
	DB->CommitTransaction();

	return S_OK;
}
