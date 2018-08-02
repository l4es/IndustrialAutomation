// DBConnection.cpp : Implementation of CDBConnection

#include "stdafx.h"
#include ".\dbconnection.h"
#include "ApplicationSettings.h"
#include "Helpers.h"
#include "Users.h"
#include "Channels.h"
#include "EventTypes.h"
#include "Events.h"
#include <atlsafe.h>

using namespace Helpers;
using namespace DB;

template<typename T> T* CreateObject(DWORD& ErrorCode, CDBConnection* Owner)
{
	ErrorCode = ERR_OK;

	HRESULT hr;
	T* obj;
	hr = T::CreateInstance(&obj);
	if(FAILED(hr))
	{
		ErrorCode = ERR_CANT_CREATE_OBJECT;
		
		return NULL;
	}

	hr = obj->Initialize(Owner);
	if(FAILED(hr))
	{
		ErrorCode = ERR_CANT_CREATE_OBJECT;
		
		return NULL;
	}

	return obj;
}

// CDBConnection
STDMETHODIMP CDBConnection::get_ErrorDescription(BSTR* pVal)
{
	

	if(_dwLastError == ERR_CUSTOM)
		return CComBSTR(_strLastError).CopyTo(pVal);

	CString s;
	s = Helpers::DecodeError(_dwLastError);

	return CComBSTR(s).CopyTo(pVal);
}

STDMETHODIMP CDBConnection::get_ErrorCode(enErrorCodes* pVal)
{
	

	*pVal = static_cast<enErrorCodes>(_dwLastError);
	if(_dwLastError == ERR_CUSTOM)
		return S_FALSE;
	else
		return S_OK;
}
STDMETHODIMP CDBConnection::Open(BSTR User, BSTR Password)
{
	
	_dwLastError = ERR_OK;

	if(_DB.IsOpen()) 
	{
		_dwLastError = ERR_ALREADY_OPEN;
		return S_FALSE;
	}

	if(!_DB.Open(g_ApplicationSettings.BuildConnectionString()))
	{
		_dwLastError	= ERR_CUSTOM;
		_strLastError.Format(_T("Ошибка при подключении к БД (%s)"), _DB.GetLastErrorString());
		
		return E_FAIL;
	}

	CADORecordset rs(&_DB);

	CString query;
	query.Format(_T("SELECT ID,Password FROM Users WHERE Login='%s'"),(LPCTSTR)(_bstr_t)User);

	if(rs.Open(query))
	{
		if(rs.GetRecordCount()<=0)
		{
			_dwLastError = ERR_ACCESSDENIED;
			rs.Close();
			_DB.Close();
			return E_ACCESSDENIED;
		}
		while(!rs.IsEOF())
		{
			CComVariant v;
			rs.GetFieldValue(_T("Password"), v);
			v.ChangeType(VT_BSTR);
			
			BOOL IsPassCorrect = FALSE;
			if(	wcscmp(Password, v.bstrVal) == NULL)
				IsPassCorrect = TRUE;
			if(IsPassCorrect)
			{
				sUser user;
				rs.GetFieldValue(_T("ID"), user.ID);
				user.ReadFromDB(_strLastError, _dwLastError, &_DB);
				_CurrentUser = user;
				return S_OK;
			}
			rs.MoveNext();
		}
		rs.Close();
		_DB.Close();
		_dwLastError = ERR_ACCESSDENIED;
		return E_ACCESSDENIED;
	}
	else
	{
		_DB.Close();
		_dwLastError	= ERR_CUSTOM;
		_strLastError.Format(_T("Ошибка при выполнении запроса (%s)"), _DB.GetLastErrorString());
		
		return E_FAIL;
	}

	if(_DB.IsOpen())
		_DB.Close();
	return E_FAIL;
}

STDMETHODIMP CDBConnection::GetUsers(IUsers** pVal)
{
	
	_dwLastError = ERR_OK;

	if(!_DB.IsOpen())
	{
		_dwLastError = ERR_NOT_OPEN;
		return S_FALSE;
	}

	HRESULT hr;
	CoUsers* users = CreateObject<CoUsers>(_dwLastError, this);
	if(users == NULL)
		return E_FAIL;

	hr = users->QueryInterface(__uuidof(IUsers), (void**)pVal);
	if(FAILED(hr))
	{
		_dwLastError = ERR_CANT_CREATE_OBJECT;
		
		return E_FAIL;
	}

	return S_OK;
}
STDMETHODIMP CDBConnection::GetChannels(IChannels** pVal)
{
	
	_dwLastError = ERR_OK;

	if(!_DB.IsOpen())
	{
		_dwLastError = ERR_NOT_OPEN;
		return S_FALSE;
	}

	HRESULT hr;
	CoChannels* chnls = CreateObject<CoChannels>(_dwLastError, this);
	if(chnls == NULL)
		return E_FAIL;

	hr = chnls->QueryInterface(__uuidof(IChannels), (void**)pVal);
	if(FAILED(hr))
	{
		_dwLastError = ERR_CANT_CREATE_OBJECT;
		
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CDBConnection::GetEventTypes(IEventTypes** pVal)
{
	
	_dwLastError = ERR_OK;

	if(!_DB.IsOpen())
	{
		_dwLastError = ERR_NOT_OPEN;
		return S_FALSE;
	}

	HRESULT hr;
	CoEventTypes* users = CreateObject<CoEventTypes>(_dwLastError, this);
	if(users == NULL)
		return E_FAIL;

	hr = users->QueryInterface(__uuidof(IEventTypes), (void**)pVal);
	if(FAILED(hr))
	{
		_dwLastError = ERR_CANT_CREATE_OBJECT;
		
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CDBConnection::GetEvents(IEvents** pVal)
{
	
	_dwLastError = ERR_OK;

	if(!_DB.IsOpen())
	{
		_dwLastError = ERR_NOT_OPEN;
		return S_FALSE;
	}

	HRESULT hr;
	CoEvents* users = CreateObject<CoEvents>(_dwLastError, this);
	if(users == NULL)
		return E_FAIL;

	hr = users->QueryInterface(__uuidof(IEvents), (void**)pVal);
	if(FAILED(hr))
	{
		_dwLastError = ERR_CANT_CREATE_OBJECT;
		
		return E_FAIL;
	}

	return S_OK;
}
