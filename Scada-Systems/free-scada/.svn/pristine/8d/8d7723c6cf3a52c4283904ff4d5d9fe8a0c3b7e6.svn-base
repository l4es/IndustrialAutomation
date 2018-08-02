#include "StdAfx.h"
#include "cfgreg.h"

namespace HelpersLib
{
	CCfgReg::CCfgReg(void)
	{
	}

	CCfgReg::~CCfgReg(void)
	{
		_reg.Close();
	}

	void CCfgReg::Open(tstring Product, bool ReadOnly, bool UsePersonalSetting)
	{
		ATLASSERT(!Product.empty());

		const HKEY parent		= UsePersonalSetting?HKEY_CURRENT_USER:HKEY_LOCAL_MACHINE;
		const DWORD OpenRights	= ReadOnly?KEY_READ:KEY_READ|KEY_WRITE;

		tstring key = _T("Software\\");
		key += Product;

		if(ReadOnly)
			_reg.Open(parent, key.c_str(), OpenRights);
		else
			_reg.Create(parent, key.c_str());
	}

	tstring CCfgReg::ReadString(tstring ValueName, tstring Default)
	{
		if(_reg == NULL)
			return Default;

		TCHAR val[MAX_PATH+1];
		DWORD size = MAX_PATH;
		if(_reg.QueryValue(val, ValueName.c_str(), &size) == ERROR_SUCCESS)
			return val;
		else
			return Default;
	}

	void CCfgReg::WriteString(tstring ValueName, tstring Value)
	{
		ATLASSERT(_reg != NULL);
		_reg.SetValue(Value.c_str(), ValueName.c_str());
	}

	DWORD CCfgReg::ReadDWORD(tstring ValueName, DWORD Default)
	{
		if(_reg == NULL)
			return Default;

		DWORD val = 0;
		if(_reg.QueryValue(val, ValueName.c_str()) == ERROR_SUCCESS)
			return val;
		else
			return Default;
	}

	void CCfgReg::WriteDWORD(tstring ValueName, DWORD Value)
	{
		ATLASSERT(_reg != NULL);
		_reg.SetValue(Value, ValueName.c_str());
	}

	float CCfgReg::ReadFloat(tstring ValueName, float Default)
	{
		if(_reg == NULL)
			return Default;

		TCHAR val[MAX_PATH+1];
		DWORD size = MAX_PATH;
		if(_reg.QueryValue(val, ValueName.c_str(), &size) == ERROR_SUCCESS)
			return (float)_tstof(val);
		else
			return Default;
	}

	void CCfgReg::WriteFloat(tstring ValueName, float Value)
	{
		ATLASSERT(_reg != NULL);

		tostringstream fmt;
		fmt<<Value;
		_reg.SetValue(fmt.str().c_str(), ValueName.c_str());
	}

	BOOL CCfgReg::ReadBinary(tstring ValueName, vector<BYTE>& Data)
	{
		Data.clear();
		if(_reg == NULL)
			return FALSE;

		ULONG size = 0;
		DWORD type;
		::RegQueryValueEx(_reg.m_hKey, ValueName.c_str(), NULL, &type, NULL, &size);
		if(type != REG_BINARY)
			return FALSE;
		Data.resize(size);
		if(::RegQueryValueEx(_reg.m_hKey, ValueName.c_str(), NULL, &type, (LPBYTE)&Data[0], &size) == ERROR_SUCCESS)
			return TRUE;
		else
			return FALSE;
	}

	void CCfgReg::WriteBinary(tstring ValueName, vector<BYTE>& Data)
	{
		ATLASSERT(_reg != NULL);
		ULONG size = static_cast<ULONG>(Data.size());
		DWORD type = REG_BINARY;
		::RegSetValueEx(_reg.m_hKey, ValueName.c_str(), NULL, type, (LPBYTE)&Data[0], size);
	}
}