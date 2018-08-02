//
//  MODULE:   Ado.cpp
//
//	AUTHOR: Carlos Antollini 
//
//  mailto: cantollini@hotmail.com
//
//	Date: 11/05/2001
//
//	Version 1.37
// 
#include "stdafx.h"
#include "ado.h"

#include <sstream>

#include <boost/format.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
using namespace boost::algorithm;

namespace DB
{
///////////////////////////////////////////////////////
//
// CADODatabase Class
//

DWORD CADODatabase::GetRecordCount(_RecordsetPtr m_pRs)
{
	DWORD numRows = 0;
	
	numRows = m_pRs->GetRecordCount();

	if(numRows == -1)
	{
		if(m_pRs->EndOfFile != VARIANT_TRUE)
			m_pRs->MoveFirst();

		while(m_pRs->EndOfFile != VARIANT_TRUE)
		{
			numRows++;
			m_pRs->MoveNext();
		}
		if(numRows > 0)
			m_pRs->MoveFirst();
	}
	return numRows;
}

bool CADODatabase::Open(LPCTSTR lpstrConnection)
{
	HRESULT hr = S_OK;

	if(IsOpen())
		Close();

	if(_tcscmp(lpstrConnection, _T("")) != 0)
		m_strConnection = lpstrConnection;

	ATLASSERT(!m_strConnection.empty());

	try
	{
		hr = m_pConnection->Open(_bstr_t(m_strConnection.c_str()), "", "", NULL);
		return hr == S_OK;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
	
}

void CADODatabase::dump_com_error(_com_error &e)
{
	std::wstring ErrorStr;
	
	
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());
	ErrorStr = (boost::wformat(L"CADODataBase Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n") 
		% e.Error()
		% std::wstring((LPCWSTR)e.ErrorMessage())
		% std::wstring((LPCWSTR)bstrSource)
		% std::wstring((LPCWSTR)bstrDescription)).str();

	//ErrorStr.Format( _T("CADODataBase Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n"),
	//	e.Error(), e.ErrorMessage(), (LPCTSTR)bstrSource, (LPCTSTR)bstrDescription );
	m_strLastError = L"Connection String = ";
	m_strLastError += GetConnectionString() + L'\n' + ErrorStr;
	m_dwLastError = e.Error(); 
	#ifdef _DEBUG
	if(m_fShowErrorMsgBox)
		MessageBox( NULL, ErrorStr.c_str(), _T(""), MB_OK | MB_ICONERROR );
	#endif	
}

bool CADODatabase::IsOpen()
{
	if(m_pConnection )
		return m_pConnection->GetState() != adStateClosed;
	return false;
}

void CADODatabase::Close()
{
	if(IsOpen())
		m_pConnection->Close();
}


CADORecordset::CADORecordset(CADODatabase* pAdoDatabase)
{
	m_pRecordset = NULL;
	m_pCmd = NULL;
	m_strQuery = _T("");
	m_pRecordset.CreateInstance(__uuidof(Recordset));
	m_pCmd.CreateInstance(__uuidof(Command));
	m_nEditStatus = CADORecordset::dbEditNone;
	m_nSearchDirection = CADORecordset::searchForward;

	m_pConnection = pAdoDatabase->GetActiveConnection();
}

bool CADORecordset::Open(_ConnectionPtr mpdb, LPCTSTR lpstrExec, int nOption)
{	
	Close();
	
	if(_tcscmp(lpstrExec, _T("")) != 0)
		m_strQuery = lpstrExec;

	ATLASSERT(!m_strQuery.empty());
	
	trim_left_if(m_strQuery,is_any_of(L" \r\n\t"));

	BOOL bIsSelect = istarts_with(m_strQuery, L"select");

	try
	{
		m_pRecordset->CursorType = adOpenStatic;
		m_pRecordset->CursorLocation = adUseClient;
		if(bIsSelect || nOption == openQuery)
			m_pRecordset->Open(m_strQuery.c_str(), _variant_t((IDispatch*)mpdb, true), 
							adOpenStatic, adLockOptimistic, adCmdUnknown);
		else if(nOption == openTable)
			m_pRecordset->Open(m_strQuery.c_str(), _variant_t((IDispatch*)mpdb, true), 
							adOpenKeyset, adLockOptimistic, adCmdTable);
		else if(nOption == openStoredProc)
		{
			m_pCmd->ActiveConnection = mpdb;
			m_pCmd->CommandText = _bstr_t(m_strQuery.c_str());
			m_pCmd->CommandType = adCmdStoredProc;
			
			m_pRecordset = m_pCmd->Execute(NULL, NULL, adCmdText);
		}
		else
		{
			ATLTRACE( "Unknown parameter. %d", nOption);
			return false;
		}
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}

	return m_pRecordset != NULL;
}

bool CADORecordset::Open(LPCTSTR lpstrExec, int nOption)
{
	ATLASSERT(m_pConnection != NULL);
	ATLASSERT(m_pConnection->GetState() != adStateClosed);
	return Open(m_pConnection, lpstrExec, nOption);
}

bool CADORecordset::GetFieldValue(LPCTSTR lpFieldName, double& dbValue)
{	
	double val = (double)NULL;
	_variant_t vtFld;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt)
		{
		case VT_R4:
			val = vtFld.fltVal;
			break;
		case VT_R8:
			val = vtFld.dblVal;
			break;
		case VT_DECIMAL:
			val = vtFld.lVal;
			val /= pow(10.0f, vtFld.wReserved1); 
			break;
		case VT_UI1:
			val = vtFld.iVal;
			break;
		case VT_I2:
		case VT_I4:
			val = vtFld.lVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			val = 0;
			break;
		default:
			val = vtFld.dblVal;
		}
		dbValue = val;
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}


bool CADORecordset::GetFieldValue(int nIndex, double& dbValue)
{	
	double val = (double)NULL;
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		switch(vtFld.vt)
		{
		case VT_R4:
			val = vtFld.fltVal;
			break;
		case VT_R8:
			val = vtFld.dblVal;
			break;
		case VT_DECIMAL:
			val = vtFld.lVal;
			val /= pow(10.0f, vtFld.wReserved1); 
			break;
		case VT_UI1:
			val = vtFld.iVal;
			break;
		case VT_I2:
		case VT_I4:
			val = vtFld.lVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			val = 0;
			break;
		default:
			val = vtFld.dblVal;
		}
		dbValue = val;
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}


bool CADORecordset::GetFieldValue(LPCTSTR lpFieldName, long& lValue)
{
	long val = (long)NULL;
	_variant_t vtFld;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		if(vtFld.vt != VT_NULL && vtFld.vt != VT_EMPTY)
			val = vtFld.lVal;
		lValue = val;
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}

bool CADORecordset::GetFieldValue(int nIndex, long& lValue)
{
	long val = (long)NULL;
	_variant_t vtFld;
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		if(vtFld.vt != VT_NULL && vtFld.vt != VT_EMPTY)
			val = vtFld.lVal;
		lValue = val;
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}


bool CADORecordset::GetFieldValue(LPCTSTR lpFieldName, int& nValue)
{
	int val = NULL;
	_variant_t vtFld;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt)
		{
		case VT_BOOL:
			val = vtFld.boolVal;
			break;
		case VT_I2:
		case VT_UI1:
			val = vtFld.iVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			break;
		default:
			nValue = 0;
			return false;
		}	
		nValue = val;
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}

bool CADORecordset::GetFieldValue(int nIndex, int& nValue)
{
	int val = (int)NULL;
	_variant_t vtFld;
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		switch(vtFld.vt)
		{
		case VT_BOOL:
			val = vtFld.boolVal;
			break;
		case VT_I2:
		case VT_UI1:
			val = vtFld.iVal;
			break;
		case VT_NULL:
		case VT_EMPTY:
			val = 0;
			break;
		default:
			return false;
		}	
		nValue = val;
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}

bool CADORecordset::GetFieldValue(LPCTSTR lpFieldName, std::wstring& strValue, std::wstring strDateFormat)
{
	std::wstring str = _T("");
	_variant_t vtFld;

	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt) 
		{
		case VT_R4:
			str = DblToStr(vtFld.fltVal);
			break;
		case VT_R8:
			str = DblToStr(vtFld.dblVal);
			break;
		case VT_BSTR:
			str = vtFld.bstrVal;
			break;
		case VT_I2:
		case VT_UI1:
			str = IntToStr(vtFld.iVal);
			break;
		case VT_I4:
			str = LongToStr(vtFld.lVal);
			break;
		case VT_DATE:
			{
				VARIANT tmp_var = vtFld;
				VariantChangeType(&tmp_var, &tmp_var, VARIANT_NOVALUEPROP, VT_BSTR);
				str = tmp_var.bstrVal;
				VariantClear(&tmp_var);
				/*COleDateTime dt(vtFld);

				if(strDateFormat.IsEmpty())
					strDateFormat = _T("%Y-%m-%d %H:%M:%S");
				str = dt.Format(strDateFormat);*/
			}
			break;
		case VT_EMPTY:
		case VT_NULL:
			break;
		default:
			strValue.clear();
			return false;
		}
		strValue = str;
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}

bool CADORecordset::GetFieldValue(int nIndex, std::wstring& strValue, std::wstring strDateFormat)
{
	std::wstring str = _T("");
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		switch(vtFld.vt) 
		{
		case VT_R4:
			str = DblToStr(vtFld.fltVal);
			break;
		case VT_R8:
			str = DblToStr(vtFld.dblVal);
			break;
		case VT_BSTR:
			str = vtFld.bstrVal;
			break;
		case VT_I2:
		case VT_UI1:
			str = IntToStr(vtFld.iVal);
			break;
		case VT_I4:
			str = LongToStr(vtFld.lVal);
			break;
		case VT_DATE:
			{
				VARIANT tmp_var = vtFld;
				VariantChangeType(&tmp_var, &tmp_var, VARIANT_NOVALUEPROP, VT_BSTR);
				str = tmp_var.bstrVal;
				VariantClear(&tmp_var);
				/*COleDateTime dt(vtFld);
				
				if(strDateFormat.IsEmpty())
					strDateFormat = _T("%Y-%m-%d %H:%M:%S");
				str = dt.Format(strDateFormat);*/
			}
			break;
		case VT_EMPTY:
		case VT_NULL:
			break;
		default:
			strValue.clear();
			return false;
		}
		strValue = str;
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}

bool CADORecordset::GetFieldValueAsDate(LPCTSTR lpFieldName, DATE& time)
{
	_variant_t vtFld;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt) 
		{
		case VT_DATE:
			{
				//COleDateTime dt(vtFld);
				//time = dt;
				time = vtFld.date;
			}
			break;
		case VT_EMPTY:
		case VT_NULL:
			break;
		default:
			return false;
		}
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}

bool CADORecordset::GetFieldValueAsDate(int nIndex, DATE& time)
{
	_variant_t vtFld;
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		switch(vtFld.vt) 
		{
		case VT_DATE:
			{
				time = vtFld.date;
				/*COleDateTime dt(vtFld);
				time = dt;*/
			}
			break;
		case VT_EMPTY:
		case VT_NULL:
			break;
		default:
			return false;
		}
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}

bool CADORecordset::GetFieldValue(LPCTSTR lpFieldName, ULONG& ulVal)
{
	_variant_t vtFld;

	try
	{
		CComVariant v;
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt)
		{
		case VT_BOOL:
			ulVal = vtFld.boolVal;
			break;
		case VT_I1:
		case VT_UI1:
		case VT_I2:
		case VT_UI2:
		case VT_I4:
		case VT_UI4:
			v = vtFld;
			if(SUCCEEDED(v.ChangeType(VT_UI4)))
				ulVal = v.ulVal;
			else
				ulVal = 0;
			break;
		case VT_NULL:
		case VT_EMPTY:
			ulVal = 0;
			break;
		default:
			return false;
		}	
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}

bool CADORecordset::GetFieldValue(int nIndex, ULONG& ulVal)
{
	ULONG val = NULL;
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		CComVariant v;
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		switch(vtFld.vt)
		{
		case VT_BOOL:
			val = vtFld.boolVal;
			break;
		case VT_I1:
		case VT_UI1:
		case VT_I2:
		case VT_UI2:
		case VT_I4:
		case VT_UI4:
			v = vtFld;
			if(SUCCEEDED(v.ChangeType(VT_UI4)))
                val = v.ulVal;
			else
				val = 0;
			break;
		case VT_NULL:
		case VT_EMPTY:
			val = 0;
			break;
		default:
			return false;
		}	
		ulVal = val;
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}

bool CADORecordset::GetFieldValue(LPCTSTR lpFieldName, CComVariant& Val)
{
	try
	{
		Val = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}

bool CADORecordset::GetFieldValue(int nIndex, CComVariant& Val)
{
	ULONG val = NULL;
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	try
	{
		Val = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}


bool CADORecordset::IsFieldNull(LPCTSTR lpFieldName)
{
	_variant_t vtFld;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		return vtFld.vt == VT_NULL;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}

bool CADORecordset::IsFieldNull(int nIndex)
{
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		return vtFld.vt == VT_NULL;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}

bool CADORecordset::IsFieldEmpty(LPCTSTR lpFieldName)
{
	_variant_t vtFld;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		return vtFld.vt == VT_EMPTY || vtFld.vt == VT_NULL;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}

bool CADORecordset::IsFieldEmpty(int nIndex)
{
	_variant_t vtFld;
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		return vtFld.vt == VT_EMPTY || vtFld.vt == VT_NULL;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}

DWORD CADORecordset::GetRecordCount()
{
	DWORD nRows = 0;
	
	nRows = m_pRecordset->GetRecordCount();

	if(nRows == -1)
	{
		nRows = 0;
		if(m_pRecordset->EndOfFile != VARIANT_TRUE)
			m_pRecordset->MoveFirst();
		
		while(m_pRecordset->EndOfFile != VARIANT_TRUE)
		{
			nRows++;
			m_pRecordset->MoveNext();
		}
		if(nRows > 0)
			m_pRecordset->MoveFirst();
	}
	
	return nRows;
}

bool CADORecordset::IsOpen()
{
	if(m_pRecordset)
		return m_pRecordset->GetState() != adStateClosed;
	return false;
}

void CADORecordset::Close()
{
	if(IsOpen())
		m_pRecordset->Close();	
}


bool CADODatabase::Execute(LPCTSTR lpstrExec)
{
	ATLASSERT(m_pConnection != NULL);
	ATLASSERT(_tcscmp(lpstrExec, _T("")) != 0);

	try
	{
		m_pConnection->Execute(_bstr_t(lpstrExec), NULL, adExecuteNoRecords);
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;	
	}
}

bool CADORecordset::RecordBinding(CADORecordBinding &pAdoRecordBinding)
{
	HRESULT hr;
	m_pRecBinding = NULL;

	//Open the binding interface.
	if(FAILED(hr = m_pRecordset->QueryInterface(__uuidof(IADORecordBinding), (LPVOID*)&m_pRecBinding )))
	{
		_com_issue_error(hr);
		return false;
	}
	
	//Bind the recordset to class
	if(FAILED(hr = m_pRecBinding->BindToRecordset(&pAdoRecordBinding)))
	{
		_com_issue_error(hr);
		return false;
	}
	return true;
}

void CADORecordset::dump_com_error(_com_error &e)
{
	std::wstring ErrorStr;
	
	
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());
	ErrorStr = (boost::wformat(L"CADORecordset Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n")
		% e.Error()
		% e.ErrorMessage()
		% (LPCWSTR)bstrSource
		% (LPCWSTR)bstrDescription).str();
	//ErrorStr.Format( _T("CADORecordset Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n"),
	//	e.Error(), e.ErrorMessage(), (LPCTSTR)bstrSource, (LPCTSTR)bstrDescription );
	m_strLastError = L"Query = " + GetQuery() + L'\n' + ErrorStr;
	m_dwLastError = e.Error();
	#ifdef _DEBUG
	if(m_fShowErrorMsgBox)
		MessageBox( NULL,ErrorStr.c_str(), _T(""), MB_OK | MB_ICONERROR );
	#endif	
}

bool CADORecordset::GetFieldInfo(LPCTSTR lpFieldName, CADOFieldInfo* fldInfo)
{
	FieldPtr pField = m_pRecordset->Fields->GetItem(lpFieldName);
	
	return GetFieldInfo(pField, fldInfo);
}

bool CADORecordset::GetFieldInfo(int nIndex, CADOFieldInfo* fldInfo)
{
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	FieldPtr pField = m_pRecordset->Fields->GetItem(vtIndex);

	return GetFieldInfo(pField, fldInfo);
}


bool CADORecordset::GetFieldInfo(FieldPtr pField, CADOFieldInfo* fldInfo)
{
	memset(fldInfo, 0, sizeof(CADOFieldInfo));

	_tcscpy(fldInfo->m_strName, (LPCTSTR)pField->GetName());
	fldInfo->m_lDefinedSize = pField->GetDefinedSize();
	fldInfo->m_nType = pField->GetType();
	fldInfo->m_lAttributes = pField->GetAttributes();
	if(!IsEof())
		fldInfo->m_lSize = pField->GetActualSize();
	return true;
}

bool CADORecordset::GetChunk(LPCTSTR lpFieldName, std::wstring& strValue)
{
	std::wstring str = _T("");
	long lngSize, lngOffSet = 0;
	_variant_t varChunk;    
	int ChunkSize = 100;

	lngSize = m_pRecordset->Fields->GetItem(lpFieldName)->ActualSize;
	
	str.clear();
	while(lngOffSet < lngSize)
	{
		varChunk = m_pRecordset->Fields->GetItem(lpFieldName)->GetChunk(ChunkSize);
		str += varChunk.bstrVal;
		lngOffSet += ChunkSize;
	}

	lngOffSet = 0;
	strValue = str;
	return TRUE;
}

std::wstring CADORecordset::GetString(LPCTSTR lpCols, LPCTSTR lpRows, LPCTSTR lpNull, long numRows)
{
	_bstr_t varOutput;
	_bstr_t varNull("");
	_bstr_t varCols("\t");
	_bstr_t varRows("\r");

	if(_tcslen(lpCols) != 0)
		varCols = _bstr_t(lpCols);

	if(_tcslen(lpRows) != 0)
		varRows = _bstr_t(lpRows);
	
	if(numRows == 0)
		numRows =(long)GetRecordCount();			
			
	varOutput = m_pRecordset->GetString(adClipString, numRows, varCols, varRows, varNull);

	return (LPCTSTR)varOutput;
}

std::wstring IntToStr(int nVal)
{
	std::wostringstream fmt;
	fmt<<nVal;
	return fmt.str();
}

std::wstring LongToStr(long lVal)
{
	std::wostringstream fmt;
	fmt<<lVal;
	return fmt.str();
}

std::wstring DblToStr(double dblVal)
{
	std::wostringstream fmt;
	fmt<<dblVal;
	return fmt.str();
}

std::wstring DblToStr(float fltVal)
{
	std::wostringstream fmt;
	fmt<<fltVal;
	return fmt.str();
}

void CADORecordset::Edit()
{
	m_nEditStatus = dbEdit;
}

bool CADORecordset::AddNew()
{
	m_nEditStatus = dbEditNone;
	if(m_pRecordset->AddNew() != S_OK)
		return false;

	m_nEditStatus = dbEditNew;
	return true;
}

bool CADORecordset::AddNew(CADORecordBinding &pAdoRecordBinding)
{
	try
	{
		if(m_pRecBinding->AddNew(&pAdoRecordBinding) != S_OK)
		{
			return false;
		}
		else
		{
			m_pRecBinding->Update(&pAdoRecordBinding);
			return true;
		}
			
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}	
}

bool CADORecordset::Update()
{
	bool bret = true;

	if(m_nEditStatus != dbEditNone)
	{

		try
		{
			if(m_pRecordset->Update() != S_OK)
				bret = false;
		}
		catch(_com_error &e)
		{
			dump_com_error(e);
			bret = false;
		}
	}

	m_nEditStatus = dbEditNone;
	return bret;
}

void CADORecordset::CancelUpdate()
{
	m_pRecordset->CancelUpdate();
	m_nEditStatus = dbEditNone;
}
bool CADORecordset::SetFieldValue(int nIndex, CComVariant Value)
{
	_variant_t vtIndex;	

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	return PutFieldValue(vtIndex, Value);
}

bool CADORecordset::SetFieldValue(LPCTSTR lpFieldName, CComVariant Value)
{
	return PutFieldValue(lpFieldName, Value);
}

bool CADORecordset::SetFieldValue(int nIndex, std::wstring strValue)
{
	_variant_t vtFld;
	_variant_t vtIndex;	
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	if(!strValue.empty())
		vtFld.vt = VT_BSTR;
	else
		vtFld.vt = VT_NULL;

	vtFld.bstrVal = ::SysAllocString(strValue.c_str());
	bool res = PutFieldValue(vtIndex, vtFld);
	::SysFreeString( vtFld.bstrVal );

	return res;
}

bool CADORecordset::SetFieldValue(LPCTSTR lpFieldName, std::wstring strValue)
{
	_variant_t vtFld;

	if(!strValue.empty())
		vtFld.vt = VT_BSTR;
	else
		vtFld.vt = VT_NULL;

	vtFld.bstrVal = ::SysAllocString(strValue.c_str());
	bool res = PutFieldValue(lpFieldName, vtFld);
	::SysFreeString( vtFld.bstrVal );

	return res;
}

bool CADORecordset::SetFieldValue(int nIndex, int nValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_I2;
	vtFld.iVal = nValue;
	
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	return PutFieldValue(vtIndex, vtFld);
}

bool CADORecordset::SetFieldValue(LPCTSTR lpFieldName, int nValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_I2;
	vtFld.iVal = nValue;
	
	return PutFieldValue(lpFieldName, vtFld);
}

bool CADORecordset::SetFieldValue(int nIndex, long lValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_I4;
	vtFld.lVal = lValue;
	
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	return PutFieldValue(vtIndex, vtFld);
	
}

bool CADORecordset::SetFieldValue(LPCTSTR lpFieldName, long lValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_I4;
	vtFld.lVal = lValue;
	
	return PutFieldValue(lpFieldName, vtFld);
}

bool CADORecordset::SetFieldValue(int nIndex, double dblValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_R8;
	vtFld.dblVal = dblValue;

	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	return PutFieldValue(vtIndex, vtFld);
}

bool CADORecordset::SetFieldValue(LPCTSTR lpFieldName, double dblValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_R8;
	vtFld.dblVal = dblValue;
		
	return PutFieldValue(lpFieldName, vtFld);
}

bool CADORecordset::SetFieldValueAsDate(int nIndex, const DATE& time)
{
	_variant_t vtFld;
	vtFld.vt = VT_DATE;
	vtFld.date = time;
	
	_variant_t vtIndex;
	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	
	return PutFieldValue(vtIndex, vtFld);
}

bool CADORecordset::SetFieldValueAsDate(LPCTSTR lpFieldName, const DATE& time)
{
	_variant_t vtFld;
	vtFld.vt = VT_DATE;
	vtFld.date = time;
	
	return PutFieldValue(lpFieldName, vtFld);
}

bool CADORecordset::SetBookmark()
{
	if(m_varBookmark.vt != VT_EMPTY)
	{
		m_pRecordset->Bookmark = m_varBookmark;
		return true;
	}
	return false;
}

bool CADORecordset::Delete()
{
	if(m_pRecordset->Delete(adAffectCurrent) != S_OK)
		return false;

	if(m_pRecordset->Update() != S_OK)
		return false;

	return true;
}

bool CADORecordset::Find(LPCTSTR lpFind, int nSearchDirection)
{

	m_strFind = lpFind;
	m_nSearchDirection = nSearchDirection;

	ATLASSERT(!m_strFind.empty());

	if(m_nSearchDirection == searchForward)
	{
		m_pRecordset->Find(_bstr_t(m_strFind.c_str()), 0, adSearchForward, "");
		if(!IsEof())
		{
			m_varBookFind = m_pRecordset->Bookmark;
			return true;
		}
	}
	else if(m_nSearchDirection == searchBackward)
	{
		m_pRecordset->Find(_bstr_t(m_strFind.c_str()), 0, adSearchBackward, "");
		if(!IsBof())
		{
			m_varBookFind = m_pRecordset->Bookmark;
			return true;
		}
	}
	else
	{
		ATLTRACE("Unknown parameter. %d", nSearchDirection);
		m_nSearchDirection = searchForward;
	}
	return false;
}

bool CADORecordset::FindFirst(LPCTSTR lpFind)
{
	m_pRecordset->MoveFirst();
	return Find(lpFind);
}

bool CADORecordset::FindNext()
{
	if(m_nSearchDirection == searchForward)
	{
		m_pRecordset->Find(_bstr_t(m_strFind.c_str()), 1, adSearchForward, m_varBookFind);
		if(!IsEof())
		{
			m_varBookFind = m_pRecordset->Bookmark;
			return true;
		}
	}
	else
	{
		m_pRecordset->Find(_bstr_t(m_strFind.c_str()), 1, adSearchBackward, m_varBookFind);
		if(!IsBof())
		{
			m_varBookFind = m_pRecordset->Bookmark;
			return true;
		}
	}
	return false;
}

bool CADORecordset::PutFieldValue(LPCTSTR lpFieldName, _variant_t vtFld)
{
	if(m_nEditStatus == dbEditNone)
		return false;
	
	try
	{
		m_pRecordset->Fields->GetItem(lpFieldName)->Value = vtFld; 
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;	
	}
}


bool CADORecordset::PutFieldValue(_variant_t vtIndex, _variant_t vtFld)
{
	if(m_nEditStatus == dbEditNone)
		return false;

	try
	{
		m_pRecordset->Fields->GetItem(vtIndex)->Value = vtFld;
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}

bool CADORecordset::Clone(CADORecordset &pRs)
{
	try
	{
		pRs.m_pRecordset = m_pRecordset->Clone(adLockUnspecified);
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}

bool CADORecordset::SetFilter(LPCTSTR strFilter)
{
	ATLASSERT(IsOpen());
	
	try
	{
		m_pRecordset->PutFilter(strFilter);
		return true;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return false;
	}
}
};