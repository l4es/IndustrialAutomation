// Recordset.cpp : Implementation of CRecordset_

#include "stdafx.h"
#include "Dataset.h"
#include "Helpers.h"
#include ".\dataset.h"

using namespace DB;
using namespace Helpers;

// CRecordset
HRESULT CDataSet::FinalConstruct()
{
	return S_OK;
}

void CDataSet::FinalRelease() 
{
	if(m_Recordset.IsOpen())
		m_Recordset.Close();
}

STDMETHODIMP CDataSet::get_ErrorDescription(BSTR* pVal)
{
	

	if(_dwLastError == ERR_CUSTOM)
		return CComBSTR(_strLastError).CopyTo(pVal);

	CString s;
	s = Helpers::DecodeError(_dwLastError);

	return CComBSTR(s).CopyTo(pVal);
}

STDMETHODIMP CDataSet::get_ErrorCode(enErrorCodes* pVal)
{
	

	*pVal = static_cast<enErrorCodes>(_dwLastError);
	if(_dwLastError == ERR_CUSTOM)
		return S_FALSE;
	else
		return S_OK;
}
STDMETHODIMP CDataSet::GetFieldValue(BSTR FieldName, VARIANT* Value)
{
	CComVariant v;
	if(!m_Recordset.GetFieldValue(CString(FieldName), v))
	{
		_dwLastError	= ERR_CUSTOM;
		_strLastError.Format(_T("Ошибка при выполнении запроса (%s)"), m_Recordset.GetLastErrorString());
		
		return E_FAIL;
	}

	VariantInit(Value);
	VariantCopy(Value, &v);
	return S_OK;
}

STDMETHODIMP CDataSet::GetFieldValueByIndex(ULONG Index, VARIANT* Value)
{
	CComVariant v;
	if(!m_Recordset.GetFieldValue(Index, v))
	{
		_dwLastError	= ERR_CUSTOM;
		_strLastError.Format(_T("Ошибка при выполнении запроса (%s)"), m_Recordset.GetLastErrorString());
		
		return E_FAIL;
	}

	VariantInit(Value);
	VariantCopy(Value, &v);
	return S_OK;
}

STDMETHODIMP CDataSet::get_IsEOF(VARIANT_BOOL* pVal)
{
	*pVal = m_Recordset.IsEOF()?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CDataSet::MoveFirst(void)
{
	m_Recordset.MoveFirst();
	return S_OK;
}

STDMETHODIMP CDataSet::MoveNext(void)
{
	m_Recordset.MoveNext();
	return S_OK;
}

STDMETHODIMP CDataSet::GetRecordsCount(ULONG* Count)
{
	*Count = m_Recordset.GetRecordCount();
	return S_OK;
}
