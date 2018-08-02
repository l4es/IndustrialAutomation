#include "stdafx.h"
#include "DBConnection.h"
#include "ApplicationSettings.h"
#include "Helpers.h"

using namespace Helpers;

/*
	значение в БД	- описание
---------------------------------------------------------------------
	caneditusers	- можно ли добавлять новый пользователей
	canviewusers	- можно ли просматривать список пользователей

*/

HRESULT sUser::WriteToDB(CString& ErrorString, DWORD& ErrorCode, DB::CADODatabase* DB)
{
	DB->BeginTransaction();

	CString query;
	query.Format(_T("DELETE FROM UserPermissions WHERE UserID=%u;"), ID);
	if(!DB->Execute(query))
	{
		DB->RollbackTransaction();
		ErrorCode	= ERR_CUSTOM;
		ErrorString	= _T("Ошибка при выполнении запроса:\n");
		ErrorString += DB->GetLastErrorString().c_str();
		return E_FAIL;
	}

	for(it i=Permissions.begin();i!=Permissions.end();i++)
	{
		if(g_ApplicationSettings.GetDatabaseType() == DBT_ACCESS)
            query.Format(_T("INSERT INTO UserPermissions(UserID, Permission, [Value]) VALUES (%u,'%s','%s');"),ID,i->first,i->second);
		else
			query.Format(_T("INSERT INTO UserPermissions(UserID, Permission, Value) VALUES (%u,'%s','%s');"),ID,i->first,i->second);

		if(!DB->Execute(query))
		{
			DB->RollbackTransaction();
			ErrorCode	= ERR_CUSTOM;
			ErrorString	= _T("Ошибка при выполнении запроса:\n");
			ErrorString += DB->GetLastErrorString().c_str();
			return E_FAIL;
		}
	}
	DB->CommitTransaction();
	return S_OK;
}

HRESULT sUser::ReadFromDB(CString& /*ErrorString*/, DWORD& /*ErrorCode*/, DB::CADODatabase* DB)
{
	Permissions.clear();
	DB::CADORecordset perm(DB);
	CString tmp;
	tmp.Format(_T("SELECT Permission,Value FROM UserPermissions WHERE UserID=%u;"),ID);
	if(perm.Open(tmp))
	{
		while(!perm.IsEOF())
		{
			CComVariant v;
			CString val1,val2;
			perm.GetFieldValue(_T("Permission"), v);
			v.ChangeType(VT_BSTR);
			val1 = (LPTSTR)(_bstr_t)v.bstrVal;
			perm.GetFieldValue(_T("Value"), v);
			v.ChangeType(VT_BSTR);
			val2 = (LPTSTR)(_bstr_t)v.bstrVal;

			val1.MakeLower();
			val2.MakeLower();
			Permissions[val1] = val2;

			perm.MoveNext();
		}
		perm.Close();
	}
	return S_OK;
}

bool sUser::CanEditUsers()
{
	it item = Permissions.find(_T("caneditusers"));
	if(item == Permissions.end())
		return false;
	return ConvertValToBool(item->second);
}

bool sUser::CanViewUsers()
{
	it item = Permissions.find(_T("canviewusers"));
	if(item == Permissions.end())
		return false;
	return ConvertValToBool(item->second);
}

bool sUser::CanEditEvents()
{
	it item = Permissions.find(_T("caneditevents"));
	if(item == Permissions.end())
		return false;
	return ConvertValToBool(item->second);
}


bool sUser::CanViewData()
{
	it item = Permissions.find(_T("canviewdata"));
	if(item == Permissions.end())
		return false;
	return ConvertValToBool(item->second);
}

bool sUser::CanViewEvents()
{
	it item = Permissions.find(_T("canviewevents"));
	if(item == Permissions.end())
		return false;
	return ConvertValToBool(item->second);
}

bool sUser::CanEditData()
{
	it item = Permissions.find(_T("caneditdata"));
	if(item == Permissions.end())
		return false;
	return ConvertValToBool(item->second);
}

bool sUser::CanEditChannels()
{
	it item = Permissions.find(_T("caneditchannels"));
	if(item == Permissions.end())
		return false;
	return ConvertValToBool(item->second);
}

void sUser::SetEditUsers(bool flag)
{
	Permissions[_T("caneditusers")] = flag?_T("yes"):_T("no");
}

void sUser::SetEditEvents(bool flag)
{
	Permissions[_T("caneditevents")] = flag?_T("yes"):_T("no");
}

void sUser::SetViewUsers(bool flag)
{
	Permissions[_T("canviewusers")] = flag?_T("yes"):_T("no");
}

void sUser::SetViewData(bool flag)
{
	Permissions[_T("canviewdata")] = flag?_T("yes"):_T("no");
}

void sUser::SetViewEvents(bool flag)
{
	Permissions[_T("canviewevents")] = flag?_T("yes"):_T("no");
}

void sUser::SetEditData(bool flag)
{
	Permissions[_T("caneditdata")] = flag?_T("yes"):_T("no");
}

void sUser::SetEditChannels(bool flag)
{
	Permissions[_T("caneditchannels")] = flag?_T("yes"):_T("no");
}

void sUser::SetPermissions(DWORD Permissions)
{
	SetEditUsers(Permissions & PERM_CANEDITUSERS?true:false);
	SetViewUsers(Permissions & PERM_CANVIEWUSERS?true:false);
	SetViewData(Permissions & PERM_CANVIEWDATA?true:false);
	SetEditData(Permissions & PERM_CANEDITDATA?true:false);
	SetEditChannels(Permissions & PERM_CANEDITCHANNELS?true:false);
	SetEditEvents(Permissions & PERM_CANEDITEVENTS?true:false);
	SetViewEvents(Permissions & PERM_CANVIEWEVENTS?true:false);
}

DWORD sUser::GetPermissions()
{
	DWORD res = NULL;
	if(CanEditUsers())		res |= PERM_CANEDITUSERS;
	if(CanViewUsers())		res |= PERM_CANVIEWUSERS;
	if(CanViewData())		res |= PERM_CANVIEWDATA;
	if(CanEditData())		res |= PERM_CANEDITDATA;
	if(CanEditChannels())	res |= PERM_CANEDITCHANNELS;
	if(CanEditEvents())		res |= PERM_CANEDITEVENTS;
	if(CanViewEvents())		res |= PERM_CANVIEWEVENTS;
	return res;
}