//
//  MODULE:   Ado.h
//
//	AUTHOR: Carlos Antollini 
//
//  mailto: cantollini@hotmail.com
//
//	Date: 11/05/2001
//
//	Version 1.37
// 

#ifndef _ADO_H_
#define _ADO_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "../Global.h"
#include <math.h>
#include <string>
#include <tchar.h>
//#include <ATLComTime.h>

#include <atlbase.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifndef _DONT_IMPORT_ADO
#pragma warning (disable: 4146)
// CG : In order to use this code against a different version of ADO, the appropriate
// ADO library needs to be used in the #import statement
#import "C:\Program Files\Common Files\System\ADO\msado15.dll" rename_namespace("ADOCG") rename("EOF", "EndOfFile")
#pragma warning (default: 4146)
#endif //_DONT_IMPORT_ADO

using namespace ADOCG;

#include "icrsint.h"

//#ifndef _CSTRING_NS
//	#ifdef __ATLSTR_H__
//		#define _CSTRING_NS ATL
//	#else
//		#define _CSTRING_NS
//	#endif
//#endif

namespace DB
{
	struct HELPERS_LIB_API CADOFieldInfo
	{
		TCHAR m_strName[30]; 
		short m_nType;
		long m_lSize; 
		long m_lDefinedSize;
		long m_lAttributes;
		short m_nOrdinalPosition;
		BOOL m_bRequired;   
		BOOL m_bAllowZeroLength; 
		long m_lCollatingOrder;  
	};

	HELPERS_LIB_API std::wstring IntToStr(int nVal);

	HELPERS_LIB_API std::wstring LongToStr(long lVal);

	HELPERS_LIB_API std::wstring DblToStr(double dblVal);

	HELPERS_LIB_API std::wstring DblToStr(float fltVal);


	class HELPERS_LIB_API CADODatabase
	{
	public:
		bool Execute(LPCTSTR lpstrExec);
		CADODatabase()
		{
			::CoInitialize(NULL);

			m_pConnection = NULL;
			m_strConnection = _T("");
			m_strLastError = _T("");
			m_dwLastError = 0;
			m_pConnection.CreateInstance(__uuidof(Connection));
			m_fShowErrorMsgBox = FALSE;
		}

		CADODatabase(const CADODatabase& src)
		{
			::CoInitialize(NULL);

			//src.m_pConnection->QueryInterface(m_pConnection.GetIID(),(void**)&m_pConnection);
			m_pConnection = src.m_pConnection;
			m_strConnection = src.m_strConnection;
			m_strLastError = src.m_strLastError;
			m_dwLastError = src.m_dwLastError;
			m_fShowErrorMsgBox = src.m_fShowErrorMsgBox;
		}

		~CADODatabase()
		{
			Close();
			m_pConnection.Release();
			m_pConnection = NULL;
			m_strConnection = _T("");
			m_strLastError = _T("");
			m_dwLastError = 0;
			::CoUninitialize();
		}

		bool Open(LPCTSTR lpstrConnection = _T(""));
		_ConnectionPtr GetActiveConnection() {return m_pConnection;};
		DWORD GetRecordCount(_RecordsetPtr m_pRs);
		long BeginTransaction() 
		{return m_pConnection->BeginTrans();};
		long CommitTransaction() 
		{return m_pConnection->CommitTrans();};
		long RollbackTransaction() 
		{return m_pConnection->RollbackTrans();};
		bool IsOpen();
		void Close();
		void SetConnectionString(LPCTSTR lpstrConnection)
		{m_strConnection = lpstrConnection;};
		std::wstring GetConnectionString()
		{return m_strConnection;};
		std::wstring GetLastErrorString() 
		{return m_strLastError;};
		DWORD GetLastError()
		{return m_dwLastError;};
		void ShowErrorMsgBox(BOOL Show){m_fShowErrorMsgBox = Show;};
	protected:
		void dump_com_error(_com_error &e);

	public:
		_ConnectionPtr m_pConnection;

	protected:
		std::wstring	m_strConnection;
		std::wstring	m_strLastError;
		DWORD					m_dwLastError;
		BOOL					m_fShowErrorMsgBox;
	};

	class HELPERS_LIB_API CADORecordset
	{
	public:
		bool Clone(CADORecordset& pRs);
		enum cadoOpenEnum
		{
			openUnknown = 0,
			openQuery = 1,
			openTable = 2,
			openStoredProc = 3
		};

		enum cadoEditEnum
		{
			dbEditNone = 0,
			dbEditNew = 1,
			dbEdit = 2
		};

		enum cadoPositionEnum
		{

			positionUnknown = -1,
			positionBOF = -2,
			positionEOF = -3
		};

		enum cadoSearchEnum
		{	
			searchForward = 1,
			searchBackward = -1
		};

		enum cadoDataType
		{
			typeEmpty = adEmpty,
			typeTinyInt = adTinyInt,
			typeSmallInt = adSmallInt,
			typeInteger = adInteger,
			typeBigInt = adBigInt,
			typeUnsignedTinyInt = adUnsignedTinyInt,
			typeUnsignedSmallInt = adUnsignedSmallInt,
			typeUnsignedInt = adUnsignedInt,
			typeUnsignedBigInt = adUnsignedBigInt,
			typeSingle = adSingle,
			typeDouble = adDouble,
			typeCurrency = adCurrency,
			typeDecimal = adDecimal,
			typeNumeric = adNumeric,
			typeBoolean = adBoolean,
			typeError = adError,
			typeUserDefined = adUserDefined,
			typeVariant = adVariant,
			typeIDispatch = adIDispatch,
			typeIUnknown = adIUnknown,
			typeGUID = adGUID,
			typeDate = adDate,
			typeDBDate = adDBDate,
			typeDBTime = adDBTime,
			typeDBTimeStamp = adDBTimeStamp,
			typeBSTR = adBSTR,
			typeChar = adChar,
			typeVarChar = adVarChar,
			typeLongVarChar = adLongVarChar,
			typeWChar = adWChar,
			typeVarWChar = adVarWChar,
			typeLongVarWChar = adLongVarWChar,
			typeBinary = adBinary,
			typeVarBinary = adVarBinary,
			typeLongVarBinary = adLongVarBinary,
			typeChapter = adChapter,
			typeFileTime = adFileTime,
			typePropVariant = adPropVariant,
			typeVarNumeric = adVarNumeric,
			typeArray = adVariant
		};

		bool SetFieldValue(int nIndex, ATL::CComVariant Value);
		bool SetFieldValue(LPCTSTR lpFieldName, ATL::CComVariant Value);
		bool SetFieldValue(int nIndex, std::wstring strValue);
		bool SetFieldValue(LPCTSTR lpFieldName, std::wstring strValue);
		bool SetFieldValue(int nIndex, int nValue);
		bool SetFieldValue(LPCTSTR lpFieldName, int nValue);
		bool SetFieldValue(int nIndex, long lValue);
		bool SetFieldValue(LPCTSTR lpFieldName, long lValue);
		bool SetFieldValue(int nIndex, double dblValue);
		bool SetFieldValue(LPCTSTR lpFieldName, double dblValue);

		bool SetFieldValueAsDate(int nIndex, const DATE& time);
		bool SetFieldValueAsDate(LPCTSTR lpFieldName, const DATE& time);

		//	bool SetFieldValue(int nIndex, COleDateTime time);
		//	bool SetFieldValue(LPCTSTR lpFieldName, COleDateTime time);

		void CancelUpdate();
		bool Update();
		void Edit();
		bool AddNew();
		bool AddNew(CADORecordBinding &pAdoRecordBinding);

		bool Find(LPCTSTR lpFind, int nSearchDirection = CADORecordset::searchForward);
		bool FindFirst(LPCTSTR lpFind);
		bool FindNext();

		void ShowErrorMsgBox(BOOL Show){m_fShowErrorMsgBox = Show;};

		CADORecordset()
		{
			m_pRecordset = NULL;
			m_pCmd = NULL;
			m_strQuery = _T("");
			m_strLastError = _T("");
			m_dwLastError = 0;
			m_pRecBinding = NULL;
			m_pRecordset.CreateInstance(__uuidof(Recordset));
			m_pCmd.CreateInstance(__uuidof(Command));
			m_nEditStatus = CADORecordset::dbEditNone;
			m_nSearchDirection = CADORecordset::searchForward;
		}

		CADORecordset(CADODatabase* pAdoDatabase);

		~CADORecordset()
		{
			Close();
			if(m_pRecordset)
				m_pRecordset.Release();
			if(m_pCmd)
				m_pCmd.Release();
			m_pRecordset = NULL;
			m_pCmd = NULL;
			m_pRecBinding = NULL;
			m_strQuery = _T("");
			m_strLastError = _T("");
			m_dwLastError = 0;
			m_nEditStatus = dbEditNone;
			m_fShowErrorMsgBox = FALSE;
		}

		std::wstring GetQuery() 
		{return m_strQuery;};
		void SetQuery(LPCWSTR strQuery) 
		{m_strQuery = strQuery;};
		bool RecordBinding(CADORecordBinding &pAdoRecordBinding);
		DWORD GetRecordCount();
		bool IsOpen();
		void Close();
		bool Open(_ConnectionPtr mpdb, LPCTSTR lpstrExec = _T(""), int nOption = CADORecordset::openUnknown);
		bool Open(LPCTSTR lpstrExec = _T(""), int nOption = CADORecordset::openUnknown);
		long GetFieldCount()
		{return m_pRecordset->Fields->GetCount();};
		bool GetFieldValue(LPCTSTR lpFieldName, double& dbValue);
		bool GetFieldValue(int nIndex, double& dbValue);
		bool GetFieldValue(LPCTSTR lpFieldName, long& lValue);
		bool GetFieldValue(int nIndex, long& lValue);
		bool GetFieldValue(LPCTSTR lpFieldName, int& nValue);
		bool GetFieldValue(int nIndex, int& nValue);
		bool GetFieldValue(LPCTSTR lpFieldName, std::wstring& strValue, std::wstring strDateFormat = _T(""));
		bool GetFieldValue(int nIndex, std::wstring& strValue, std::wstring strDateFormat = _T(""));
		bool GetFieldValueAsDate(LPCTSTR lpFieldName, DATE& time);
		bool GetFieldValueAsDate(int nIndex, DATE& time);
		//	bool GetFieldValue(LPCTSTR lpFieldName, COleDateTime& time);
		//	bool GetFieldValue(int nIndex, COleDateTime& time);
		bool GetFieldValue(LPCTSTR lpFieldName, ULONG& ulVal);
		bool GetFieldValue(int nIndex, ULONG& ulVal);
		bool GetFieldValue(LPCTSTR lpFieldName, ATL::CComVariant& Val);
		bool GetFieldValue(int nIndex, ATL::CComVariant& Val);

		bool IsFieldNull(LPCTSTR lpFieldName);
		bool IsFieldNull(int nIndex);
		bool IsFieldEmpty(LPCTSTR lpFieldName);
		bool IsFieldEmpty(int nIndex);	
		bool IsEof()
		{return m_pRecordset->EndOfFile == VARIANT_TRUE;};
		bool IsEOF()
		{return m_pRecordset->EndOfFile == VARIANT_TRUE;};
		bool IsBof()
		{return m_pRecordset->BOF == VARIANT_TRUE;};
		bool IsBOF()
		{return m_pRecordset->BOF == VARIANT_TRUE;};
		void MoveFirst() 
		{m_pRecordset->MoveFirst();};
		void MoveNext() 
		{m_pRecordset->MoveNext();};
		void MovePrevious() 
		{m_pRecordset->MovePrevious();};
		void MoveLast() 
		{m_pRecordset->MoveLast();};
		long GetAbsolutePage()
		{return m_pRecordset->GetAbsolutePage();};
		void SetAbsolutePage(int nPage)
		{m_pRecordset->PutAbsolutePage((enum PositionEnum)nPage);};
		long GetPageCount()
		{return m_pRecordset->GetPageCount();};
		long GetPageSize()
		{return m_pRecordset->GetPageSize();};
		void SetPageSize(int nSize)
		{m_pRecordset->PutPageSize(nSize);};
		long GetAbsolutePosition()
		{return m_pRecordset->GetAbsolutePosition();};
		void SetAbsolutePosition(int nPosition)
		{m_pRecordset->PutAbsolutePosition((enum PositionEnum)nPosition);};
		bool GetFieldInfo(LPCTSTR lpFieldName, CADOFieldInfo* fldInfo);
		bool GetFieldInfo(int nIndex, CADOFieldInfo* fldInfo);
		bool GetChunk(LPCTSTR lpFieldName, std::wstring& strValue);
		std::wstring GetString(LPCTSTR lpCols, LPCTSTR lpRows, LPCTSTR lpNull, long numRows = 0);
		std::wstring GetLastErrorString() 
		{return m_strLastError;};
		DWORD GetLastError()
		{return m_dwLastError;};
		void GetBookmark()
		{m_varBookmark = m_pRecordset->Bookmark;};
		bool SetBookmark();
		bool Delete();
		bool IsConnectionOpen()
		{return m_pConnection->GetState() != adStateClosed;};
		_RecordsetPtr GetRecordset()
		{return m_pRecordset;};
		bool SetFilter(LPCTSTR strFilter);

	public:
		_RecordsetPtr m_pRecordset;
		_CommandPtr m_pCmd;

	protected:
		_ConnectionPtr m_pConnection;
		int m_nSearchDirection;
		std::wstring m_strFind;
		_variant_t m_varBookFind;
		_variant_t m_varBookmark;
		int m_nEditStatus;
		std::wstring m_strLastError;
		DWORD m_dwLastError;
		void dump_com_error(_com_error &e);
		IADORecordBinding *m_pRecBinding;
		std::wstring m_strQuery;
		BOOL					m_fShowErrorMsgBox;

	protected:
		bool PutFieldValue(LPCTSTR lpFieldName, _variant_t vtFld);
		bool PutFieldValue(_variant_t vtIndex, _variant_t vtFld);
		bool GetFieldInfo(FieldPtr pField, CADOFieldInfo* fldInfo);

	};
}

#endif