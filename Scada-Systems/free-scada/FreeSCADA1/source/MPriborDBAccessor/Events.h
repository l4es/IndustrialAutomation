// Events.h : Declaration of the CEvents

#pragma once
#include "resource.h"       // main symbols

#include "MPriborDBAccessor.h"


class CDBConnection;

// CEvents

class ATL_NO_VTABLE CEvents : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CEvents, &CLSID_Events>,
	public IDispatchImpl<IEvents, &IID_IEvents, &LIBID_MPriborDBAccessorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispatchImpl<IError, &__uuidof(IError), &LIBID_MPriborDBAccessorLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
	CEvents()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_EVENTS)


	BEGIN_COM_MAP(CEvents)
		COM_INTERFACE_ENTRY(IEvents)
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
	STDMETHOD(get_ErrorDescription)( BSTR* pVal);
	STDMETHOD(get_ErrorCode)( enErrorCodes* pVal);
	STDMETHOD(AddEvent)(ULONG ChannelID, ULONG EventType, VARIANT Date);
	STDMETHOD(DeleteEvent)(ULONG ChannelID, ULONG TypeID, VARIANT FilterFromDate, VARIANT FilterToDate);
	STDMETHOD(DeleteAllEvents)(ULONG ChannelID, VARIANT FilterFromDate, VARIANT FilterToDate);
	STDMETHOD(GetEventsList)(ULONG ChannelID, VARIANT FilterTypeID, VARIANT FilterFromDate, VARIANT FilterToDate, IDataSet** pVal);
};

OBJECT_ENTRY_AUTO(__uuidof(Events), CEvents)
typedef CComObject<CEvents> CoEvents;