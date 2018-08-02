// Users.cpp : Implementation of CUsers

#include "stdafx.h"
#include "Users.h"
#include "DBConnection.h"
#include "Helpers.h"
#include "ApplicationSettings.h"
#include "DataSet.h"
#include <atlsafe.h>

using namespace DB;
using namespace Helpers;


// CUsers
STDMETHODIMP CUsers::get_ErrorDescription(BSTR* pVal)
{
	

	if(_dwLastError == ERR_CUSTOM)
		return CComBSTR(_strLastError).CopyTo(pVal);

	CString s;
	s = Helpers::DecodeError(_dwLastError);

	return CComBSTR(s).CopyTo(pVal);
}

STDMETHODIMP CUsers::get_ErrorCode(enErrorCodes* pVal)
{
	

	*pVal = static_cast<enErrorCodes>(_dwLastError);
	if(_dwLastError == ERR_CUSTOM)
		return S_FALSE;
	else
		return S_OK;
}


HRESULT CUsers::FinalConstruct()
{
	return S_OK;
}

void CUsers::FinalRelease() 
{
	if(_Owner)
	{
		_Owner->Release();
		_Owner = NULL;
	}
}

HRESULT CUsers::Initialize(CDBConnection *Owner)
{
	_Owner = Owner;
	if(_Owner)
		_Owner->AddRef();
	else
		return E_FAIL;
	return S_OK;
}
STDMETHODIMP CUsers::SetUserPermissions(BSTR Login, DWORD Permissions)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanEditUsers())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}
	CString tmp(Login);
	tmp.MakeLower();
	if(tmp == _T("administrator"))
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;
	sUser			user;

	query.Format(_T("SELECT ID FROM Users WHERE Login='%s';"),(LPCTSTR)(_bstr_t)Login);
	DO_QUERY_RS(DB, rs, query);

	if(rs.GetRecordCount() <= 0)
	{
		rs.Close();
		_dwLastError	= ERR_USER_NOT_FOUND;
		
		return E_INVALIDARG;
	}
	rs.MoveFirst();
	rs.GetFieldValue(_T("ID"), user.ID);
	rs.Close();

	user.SetPermissions(Permissions);
	
	HRESULT hr = user.WriteToDB(_strLastError, _dwLastError, DB);
	if(FAILED(hr))
		return E_FAIL;

	return S_OK;
}

STDMETHODIMP CUsers::AddUser(BSTR FirstName/*Имя*/, BSTR MiddleName/*Отчество*/, BSTR LastName/*Фамилия*/, BSTR Login, BSTR Password, ULONG Permissions)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanEditUsers())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}
	if(CComBSTR(Login).Length() <=0)
	{
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	CString			query;
	sUser			user;

	query.Format(_T("SELECT ID FROM Users WHERE Login='%s';"),(LPCTSTR)(_bstr_t)Login);
	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount() > 0)
	{
		_dwLastError = ERR_DUBLICATE;
		rs.Close();
		return S_FALSE;
	}
	rs.Close();

	if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
	{
		query.Format(_T("INSERT INTO Users(Login, [Password], FirstName,MiddleName,LastName) VALUES('%s','%s','%s','%s','%s');"),
					(LPCTSTR)(_bstr_t)Login,
					(LPCTSTR)(_bstr_t)Password,
					(LPCTSTR)(_bstr_t)FirstName,
					(LPCTSTR)(_bstr_t)MiddleName,
					(LPCTSTR)(_bstr_t)LastName);
	}
	else
	{
		query.Format(_T("INSERT INTO Users(Login, Password, FirstName,MiddleName,LastName) VALUES('%s','%s','%s','%s','%s');"),
			(LPCTSTR)(_bstr_t)Login,
			(LPCTSTR)(_bstr_t)Password,
			(LPCTSTR)(_bstr_t)FirstName,
			(LPCTSTR)(_bstr_t)MiddleName,
			(LPCTSTR)(_bstr_t)LastName);
	}

	DB->BeginTransaction();
	DO_QUERY_DB_TRANS(DB,query);
	DB->CommitTransaction();

	query.Format(_T("SELECT ID FROM Users WHERE Login='%s';"),(LPCTSTR)(_bstr_t)Login);
	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount() <= 0)
	{
		rs.Close();
		_dwLastError	= ERR_USER_NOT_FOUND;
		return E_FAIL;
	}
	rs.MoveFirst();
	rs.GetFieldValue(_T("ID"), user.ID);
	rs.Close();

	user.SetPermissions(Permissions);

	HRESULT hr = user.WriteToDB(_strLastError, _dwLastError, DB);
	if(FAILED(hr))
		return E_FAIL;

	return S_OK;
}
STDMETHODIMP CUsers::DeleteUser(BSTR Login)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanEditUsers())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CString query;
	query.Format(_T("SELECT ID FROM Users WHERE Login='%s';"), (LPCTSTR)(_bstr_t)Login);

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);
	DWORD			UserID;

	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount() <= 0)
	{
		rs.Close();
		_dwLastError	= ERR_USER_NOT_FOUND;
		return S_FALSE;
	}
	rs.MoveFirst();
	rs.GetFieldValue(_T("ID"), UserID);
	rs.Close();

	//Нельзя удалить самого себя
	if(_Owner->GetCurrentUser().ID == UserID)
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	DB->BeginTransaction();
	query.Format(_T("DELETE FROM UserPermissions WHERE UserID=%u;"), UserID);
	DO_QUERY_DB_TRANS(DB,query);
	query.Format(_T("DELETE FROM Users WHERE ID=%u;"), UserID);
	DO_QUERY_DB_TRANS(DB,query);
	DB->CommitTransaction();
	return S_OK;
}

STDMETHODIMP CUsers::GetUserInfo(BSTR Login, BSTR* FirstName, BSTR* MiddleName, BSTR* LastName)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanViewUsers())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CString query;
	query.Format(_T("SELECT * FROM Users WHERE Login='%s';"), (LPCTSTR)(_bstr_t)Login);

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);

	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount() <= 0)
	{
		rs.Close();
		_dwLastError	= ERR_USER_NOT_FOUND;
		return E_INVALIDARG;
	}
	std::wstring tmp;
	rs.MoveFirst();
	rs.GetFieldValue(_T("FirstName"), tmp);
	CComBSTR(tmp.c_str()).CopyTo(FirstName);
	rs.GetFieldValue(_T("MiddleName"), tmp);
	CComBSTR(tmp.c_str()).CopyTo(MiddleName);
	rs.GetFieldValue(_T("LastName"), tmp);
	CComBSTR(tmp.c_str()).CopyTo(LastName);
	rs.Close();

	return S_OK;
}

STDMETHODIMP CUsers::SetUserInfo(BSTR Login, BSTR FirstName, BSTR MiddleName, BSTR LastName)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanEditUsers())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}
	if(CComBSTR(FirstName).Length() <=0)
	{
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}

	CString query;

	query.Format(_T("UPDATE Users SET FirstName='%s', MiddleName='%s', \
				 LastName='%s' WHERE Login='%s';"), 
				 (LPCTSTR)(_bstr_t)FirstName, (LPCTSTR)(_bstr_t)MiddleName,
				 (LPCTSTR)(_bstr_t)LastName,(LPCTSTR)(_bstr_t)Login);

	CADODatabase*	DB = _Owner->GetConnection();

	DB->BeginTransaction();
	DO_QUERY_DB_TRANS(DB,query);
	DB->CommitTransaction();

	return S_OK;
}

STDMETHODIMP CUsers::GetUserPermissions(BSTR Login, ULONG* Permissions)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanViewUsers())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}

	CString query;
	query.Format(_T("SELECT ID FROM Users WHERE Login='%s';"), (LPCTSTR)(_bstr_t)Login);

	CADODatabase*	DB = _Owner->GetConnection();
	CADORecordset	rs(DB);

	DO_QUERY_RS(DB, rs, query);
	if(rs.GetRecordCount() <= 0)
	{
		rs.Close();
		_dwLastError	= ERR_USER_NOT_FOUND;
		return E_INVALIDARG;
	}
	CString tmp;
	sUser user;
	rs.MoveFirst();
	rs.GetFieldValue(_T("ID"), user.ID);
	rs.Close();

	HRESULT hr;
	hr = user.ReadFromDB(_strLastError, _dwLastError, DB);
	if(FAILED(hr))
		return E_FAIL;

	*Permissions = user.GetPermissions();
	return S_OK;
}

STDMETHODIMP CUsers::GetUsersList(IDataSet** pVal)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanViewUsers())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
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
	if(!rs.Open(_T("SELECT Login FROM Users;")))
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

STDMETHODIMP CUsers::SetUserPassword(BSTR Login, BSTR Password)
{
	
	_dwLastError = ERR_OK;

	if(!_Owner->GetCurrentUser().CanEditUsers())
	{
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}
	if(	CComBSTR(Login).Length() <=0)
	{
		_dwLastError = ERR_INVALIDARG;
		return E_INVALIDARG;
	}

	CADODatabase *DB = _Owner->GetConnection();
	CADORecordset rs(DB);
	CString query;
	CString pass = Password;
	if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
		query.Format(_T("UPDATE Users SET [Password]='%s' WHERE [Login]='%s';"),pass, CString(CComBSTR(Login)));
	else
		query.Format(_T("UPDATE Users SET Password='%s' WHERE Login='%s';"),pass, CString(CComBSTR(Login)));

	DB->BeginTransaction();
	DO_QUERY_DB_TRANS(DB,query);
	DB->CommitTransaction();

	return S_OK;
}
