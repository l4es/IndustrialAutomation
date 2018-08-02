#pragma once

#define DO_QUERY_RS(DataBase, RecordSet, Query)	\
		if(!RecordSet.Open(Query)){			\
		_dwLastError	= ERR_CUSTOM;		\
		_strLastError.Format(_T("Ошибка при выполнении запроса (%s)"), DataBase->GetLastErrorString());		\
		return E_FAIL;}

#define DO_QUERY_DB(DataBase, Query)	\
	if(!DataBase->Execute(Query)){	\
	_dwLastError	= ERR_CUSTOM;	\
	_strLastError.Format(_T("Ошибка при выполнении запроса (%s)"), DataBase->GetLastErrorString());		\
	return E_FAIL;}

#define DO_QUERY_RS_TRANS(DataBase, RecordSet, Query)	\
	if(!RecordSet.Open(Query)){			\
	DataBase->RollbackTransaction();	\
	_dwLastError	= ERR_CUSTOM;		\
	_strLastError.Format(_T("Ошибка при выполнении запроса (%s)"), DataBase->GetLastErrorString());		\
	return E_FAIL;}

#define DO_QUERY_DB_TRANS(DataBase, Query)	\
	if(!DataBase->Execute(Query)){	\
	DataBase->RollbackTransaction();	\
	_dwLastError	= ERR_CUSTOM;	\
	_strLastError.Format(_T("Ошибка при выполнении запроса (%s)"), DataBase->GetLastErrorString());		\
	return E_FAIL;}


enum enCryptoErrors
{
	CE_CANT_OPEN_CRYPT_PROV,
	CE_CANT_CREATE_HASH,
	CE_CANT_CREATE_KEY,
	CE_CANT_ENCODE,
	CE_CANT_DECODE
};

namespace Helpers
{
	class CHelpers
	{
	public:
		static CString LoadLongQuery(ULONG ID);
		static vector<CString> SplitQueryToShort(CString query);

		static CString EncodePass(CString Password);
		static CString DecodePass(CString Password);
	};

	CString DecodeError(DWORD Code);
	bool ConvertValToBool(CString value);
}