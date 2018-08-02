// Maintainer.cpp : Implementation of CMaintainer

#include "stdafx.h"
#include "Helpers.h"
#include "ApplicationSettings.h"
#include <odbcinst.h>
#include ".\maintainer.h"

#import "c:\\Program Files\\Common Files\\System\\ado\\msadox.dll" 
#import "c:\\Program Files\\Common Files\\System\\ado\\MSJRO.DLL" no_namespace

#pragma comment(lib,"odbccp32.lib")

using namespace Helpers;
using namespace ADOX;
using namespace std;


// CMaintainer
STDMETHODIMP CMaintainer::get_ErrorDescription(BSTR* pVal)
{
	if(_dwLastError == ERR_CUSTOM)
		return CComBSTR(_strLastError).CopyTo(pVal);

	CString s = Helpers::DecodeError(_dwLastError);
	return CComBSTR(s).CopyTo(pVal);
}

STDMETHODIMP CMaintainer::get_ErrorCode(enErrorCodes* pVal)
{
	*pVal = static_cast<enErrorCodes>(_dwLastError);
	if(_dwLastError == ERR_CUSTOM)
		return S_FALSE;
	else
		return S_OK;
}


STDMETHODIMP CMaintainer::CreateDatabase(BSTR User, BSTR Password)
{
	

	_dwLastError = ERR_OK;

	switch(g_ApplicationSettings.GetDatabaseType())
	{
	case DBT_ACCESS: return CreateDB_Access();
	case DBT_MYSQL: return CreateDB_MySQL(User, Password);
	}
	return E_FAIL;
}

HRESULT CMaintainer::CreateDB_Access()
{
	
	_dwLastError = ERR_OK;

	HRESULT hr; 

	CString FileName;
	FileName = g_ApplicationSettings.GetDatabasePath() + _T("\\") + g_ApplicationSettings.GetDatabaseName() + _T(".mdb");

	CString strConn;
	strConn.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;"), FileName);

	//существует такой файл?
	if(!PathFileExists(FileName))
	{
		_CatalogPtr catalog;
		hr = catalog.CreateInstance( __uuidof(Catalog) );

		CComVariant va;
		va.Clear();
		hr = catalog->raw_Create(_bstr_t(strConn), &va );
		//отсоединение от базы данных
		if( va.vt == VT_DISPATCH)
			va.pdispVal->Release();

		DB::CADODatabase db;
		if(db.Open(strConn))
		{
			CString query = CHelpers::LoadLongQuery(IDR_SQL_CREATE_DB_ACCESS);
			vector<CString> queries = CHelpers::SplitQueryToShort(query);

			for(vector<CString>::iterator i=queries.begin();i!=queries.end();i++)
			{
				if(!db.Execute(*i))
				{
					_dwLastError	= ERR_CUSTOM;
					_strLastError.Format(_T("Ошибка при создании БД (%s)"), db.GetLastErrorString());
					
					return S_FALSE;
				}
			}
			db.Close();
		}
		else
		{
			_dwLastError	= ERR_CUSTOM;
			_strLastError.Format(_T("Ошибка при подключении к БД (%s)"), db.GetLastErrorString());
			
			return E_FAIL;
		}
	}
	else
	{
		
		_dwLastError = ERR_DB_ALLREADY_PRESENT;
		return S_FALSE;
	}	


	return S_OK;
}

HRESULT CMaintainer::CreateDB_MySQL(BSTR User, BSTR Password)
{
	
	_dwLastError = ERR_OK;

	SQLConfigDataSource(NULL, ODBC_REMOVE_SYS_DSN,
						_T("MySQL ODBC 3.51 Driver"),
						g_ApplicationSettings.BuildConnectionString((LPCTSTR)(_bstr_t)User,(LPCTSTR)(_bstr_t)Password));

	if(!SQLConfigDataSource(NULL, ODBC_ADD_SYS_DSN,
						_T("MySQL ODBC 3.51 Driver"),
						g_ApplicationSettings.BuildConnectionString((LPCTSTR)(_bstr_t)User,(LPCTSTR)(_bstr_t)Password)))
	{
		_dwLastError	= ERR_CUSTOM;
		_strLastError	= _T("Ошибка при создании записи в ODBC.\nУбедитесь что ODBC драйвер «MySQL ODBC 3.51 Driver» установлен.");
		
		return E_FAIL;
	}

	CString strConn = g_ApplicationSettings.BuildConnectionString((LPCTSTR)(_bstr_t)User,(LPCTSTR)(_bstr_t)Password);

	DB::CADODatabase db;
	if(!db.Open(_T("DSN=FreeSCADA Database;DATABASE=mysql;")))
	{
		_dwLastError	= ERR_CUSTOM;
		_strLastError.Format(_T("Ошибка при подключении к СУБД (%s)"), db.GetLastErrorString());
		
		return E_FAIL;
	}
	else
	{
		if(!db.Execute(CString(_T("CREATE DATABASE ")+g_ApplicationSettings.GetDatabaseName()+_T(";"))))
		{
			_dwLastError	= ERR_CUSTOM;
			_strLastError.Format(_T("Ошибка при создании БД (%s)"), db.GetLastErrorString());

			
			return S_FALSE;
		}
		db.Close();
	}

	if(!db.Open(strConn))
	{
		_dwLastError	= ERR_CUSTOM;
		_strLastError.Format(_T("Ошибка при подключении к БД (%s)"), db.GetLastErrorString());
		
		return E_FAIL;
	}

	CString query = CHelpers::LoadLongQuery(IDR_SQL_CREATE_DB_MYSQL);
	vector<CString> queries = CHelpers::SplitQueryToShort(query);
	
	for(vector<CString>::iterator i=queries.begin();i!=queries.end();i++)
	{
		if(!db.Execute(*i))
		{
			_dwLastError	= ERR_CUSTOM;
			_strLastError.Format(_T("Ошибка при создании БД:\n"), db.GetLastErrorString());
			
			return S_FALSE;
		}
	}
	db.Close();

	return S_OK;
}