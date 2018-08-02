// EventTypes.h : Declaration of the CEventTypes

#pragma once
#include "resource.h"       // main symbols

#include "MPriborDBAccessor.h"


class CDBConnection;

// CEventTypes

class ATL_NO_VTABLE CEventTypes : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CEventTypes, &CLSID_EventTypes>,
	public IDispatchImpl<IEventTypes, &IID_IEventTypes, &LIBID_MPriborDBAccessorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispatchImpl<IError, &__uuidof(IError), &LIBID_MPriborDBAccessorLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
	CEventTypes()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_EVENTTYPES)


	BEGIN_COM_MAP(CEventTypes)
		COM_INTERFACE_ENTRY(IEventTypes)
		COM_INTERFACE_ENTRY2(IDispatch, IError)
		COM_INTERFACE_ENTRY(IError)
	END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();
	HRESULT Initialize(CDBConnection *Owner);

protected:
	CDBConnection*		_Owner;
	ULONG				_dwLastError;
	CString				_strLastError;

public:
	// IError Methods
	STDMETHOD(get_ErrorDescription)( BSTR *  pVal);
	STDMETHOD(get_ErrorCode)( enErrorCodes *  pVal);
	STDMETHOD(AddEventType)(ULONG ID, BSTR Name, BSTR Description);
	STDMETHOD(DeleteEventType)(ULONG ID);
	STDMETHOD(get_EventTypeName)(ULONG ID, BSTR* pVal);
	STDMETHOD(put_EventTypeName)(ULONG ID, BSTR newVal);
	STDMETHOD(get_EventTypeDescription)(ULONG ID, BSTR* pVal);
	STDMETHOD(put_EventTypeDescription)(ULONG ID, BSTR newVal);
};

OBJECT_ENTRY_AUTO(__uuidof(EventTypes), CEventTypes)
typedef CComObject<CEventTypes> CoEventTypes;