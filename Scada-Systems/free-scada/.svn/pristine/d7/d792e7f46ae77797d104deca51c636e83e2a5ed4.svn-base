// DataSetEx.h : Declaration of the CDataSetEx

#pragma once
#include "resource.h"       // main symbols

#include "MPriborDBAccessor.h"


// CDataSetEx

class ATL_NO_VTABLE CDataSetEx : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CDataSetEx, &CLSID_DataSetEx>,
	public IDispatchImpl<IDataSet, &__uuidof(IDataSet), &LIBID_MPriborDBAccessorLib, /* wMajor = */ 1, /* wMinor = */ 0>,
	public IDispatchImpl<IError, &__uuidof(IError), &LIBID_MPriborDBAccessorLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
	CDataSetEx()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_DATASETEX)


	BEGIN_COM_MAP(CDataSetEx)
		COM_INTERFACE_ENTRY2(IDispatch, IDataSet)
		COM_INTERFACE_ENTRY(IDataSet)
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
	DB::CADODatabase	*m_DB;
	CString				m_TempTableName;

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

OBJECT_ENTRY_AUTO(__uuidof(DataSetEx), CDataSetEx)
typedef CComObject<CDataSetEx> CoDataSetEx;