// Recordset.h : Declaration of the CRecordset_

#pragma once
#include "resource.h"       // main symbols

#include "MPriborDBAccessor.h"


// CRecordset_

class ATL_NO_VTABLE CDataSet : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CDataSet, &CLSID_DataSet>,
	public IDispatchImpl<IDataSet, &IID_IDataSet, &LIBID_MPriborDBAccessorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispatchImpl<IError, &__uuidof(IError), &LIBID_MPriborDBAccessorLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
	CDataSet()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_DATASET)


	BEGIN_COM_MAP(CDataSet)
		COM_INTERFACE_ENTRY(IDataSet)
		COM_INTERFACE_ENTRY2(IDispatch, IError)
		COM_INTERFACE_ENTRY(IError)
	END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

protected:
	ULONG				_dwLastError;
	CString				_strLastError;
public:
	DB::CADORecordset	m_Recordset;

	// IError Methods
public:
	STDMETHOD(get_ErrorDescription)( BSTR *  pVal);
	STDMETHOD(get_ErrorCode)( enErrorCodes *  pVal);
	STDMETHOD(GetFieldValue)(BSTR FieldName, VARIANT* Value);
	STDMETHOD(GetFieldValueByIndex)(ULONG Index, VARIANT* Value);
	STDMETHOD(get_IsEOF)(VARIANT_BOOL* pVal);
	STDMETHOD(MoveFirst)(void);
	STDMETHOD(MoveNext)(void);
	STDMETHOD(GetRecordsCount)(ULONG* Count);
};

OBJECT_ENTRY_AUTO(__uuidof(DataSet), CDataSet)
typedef CComObject<CDataSet> CoDataSet;