#include "StdAfx.h"
#include ".\applicationsettings.h"

namespace Helpers
{
	CApplicationSettings g_ApplicationSettings;

	CApplicationSettings::CApplicationSettings(void)
	{
		_reg.Open(_T("FreeSCADA\\Database"), false, false);
	}

	CApplicationSettings::~CApplicationSettings(void)
	{
	}

	enDatabaseType CApplicationSettings::GetDatabaseType()
	{
		return (enDatabaseType)_reg.ReadDWORD(_T("Type"), DBT_ACCESS);
	}

	void CApplicationSettings::SetDatabaseType(enDatabaseType NewType)
	{
		_reg.WriteDWORD(_T("Type"), NewType);
	}

	CString CApplicationSettings::GetDatabaseName()
	{
		return _reg.ReadString(_T("DBName")).c_str();
	}

	void CApplicationSettings::SetDatabaseName(CString NewName)
	{
		_reg.WriteString(_T("DBName"), (LPCTSTR)NewName);
	}

	CString CApplicationSettings::GetDatabasePath()
	{
		return _reg.ReadString(_T("DBPath")).c_str();
	}

	void CApplicationSettings::SetDatabasePath(CString NewPath)
	{
		_reg.WriteString(_T("DBPath"), (LPCTSTR)NewPath);
	}

	CString CApplicationSettings::GetDatabaseServer()
	{
		return _reg.ReadString(_T("DBServer")).c_str();
	}

	void CApplicationSettings::SetDatabaseServer(CString NewServer)
	{
		_reg.WriteString(_T("DBServer"), (LPCTSTR)NewServer);
	}

	CString CApplicationSettings::BuildConnectionString(CString User, CString Password)
	{
		CString res;

		if(GetDatabaseType() == DBT_MYSQL)
		{
			res = _T("DSN=FreeSCADA Database;DESC=FreeSCADA channel database;");
			res += _T("DATABASE=") + GetDatabaseName()+ _T(";");
			res += _T("SERVER=") + GetDatabaseServer()+ _T(";");
			res += _T("UID=") + User + _T(";PASSWORD=")+Password+_T(";PORT=3306;OPTION=3;STMT=;");
			return res;
		}
		if(GetDatabaseType() == DBT_ACCESS)
		{
			CString FileName;
			FileName = GetDatabasePath() + _T("\\") + GetDatabaseName() + _T(".mdb");

			res.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;"),FileName);
			return res;
		}
		return _T("");
	}
	CString CApplicationSettings::BuildConnectionString()
	{
		CString res;

		if(GetDatabaseType() == DBT_MYSQL)
		{
			res = _T("DSN=MPribor Database;DESC=MPribor channel database;");
			res += _T("DATABASE=") + GetDatabaseName()+ _T(";");
			res += _T("SERVER=") + GetDatabaseServer()+ _T(";");
			return res;
		}
		if(GetDatabaseType() == DBT_ACCESS)
		{
			CString FileName;
			FileName = GetDatabasePath() + _T("\\") + GetDatabaseName() + _T(".mdb");

			res.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;"),FileName);
			return res;
		}
		return _T("");
	}
}