// Maintainer.h : Declaration of the CMaintainer

#pragma once
#include "resource.h"       // main symbols

#include "MPriborDBAccessor.h"

// CMaintainer

class ATL_NO_VTABLE CMaintainer : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMaintainer, &CLSID_Maintainer>,
	public IDispatchImpl<IMaintainer, &IID_IMaintainer, &LIBID_MPriborDBAccessorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispatchImpl<IError, &__uuidof(IError), &LIBID_MPriborDBAccessorLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
	CMaintainer()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_MAINTAINER)


	BEGIN_COM_MAP(CMaintainer)
		COM_INTERFACE_ENTRY(IMaintainer)
		COM_INTERFACE_ENTRY2(IDispatch, IError)
		COM_INTERFACE_ENTRY(IError)
	END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct(){return S_OK;}
	void FinalRelease(){}

public:

	STDMETHOD(CreateDatabase)(BSTR User, BSTR Password);

	// IError Methods
	STDMETHOD(get_ErrorDescription)(BSTR* pVal);
	STDMETHOD(get_ErrorCode)(enErrorCodes* pVal);

protected:
	ULONG _dwLastError;
	CString _strLastError;
	HRESULT CreateDB_Access();
	HRESULT CreateDB_MySQL(BSTR User, BSTR Password);
};

OBJECT_ENTRY_AUTO(__uuidof(Maintainer), CMaintainer)
