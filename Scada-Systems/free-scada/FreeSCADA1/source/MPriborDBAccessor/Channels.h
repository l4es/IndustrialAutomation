// Channelss.h : Declaration of the CChannels

#pragma once
#include "resource.h"       // main symbols

#include "MPriborDBAccessor.h"


class CDBConnection;

// CChannels
class ATL_NO_VTABLE CChannels : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CChannels, &CLSID_Channels>,
	public IDispatchImpl<IChannels, &IID_IChannels, &LIBID_MPriborDBAccessorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispatchImpl<IError, &__uuidof(IError), &LIBID_MPriborDBAccessorLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
	CChannels()
		:_Owner(NULL)
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_CHANNELS)


	BEGIN_COM_MAP(CChannels)
		COM_INTERFACE_ENTRY(IChannels)
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

	STDMETHOD(AddChannel)(BSTR ServerCLSID, BSTR ComputerName, BSTR ChannelName, BSTR Description, ULONG* ID);
	STDMETHOD(DeleteChannel)(ULONG ID);
	STDMETHOD(get_Active)(ULONG ChannelID, VARIANT_BOOL* pVal);
	STDMETHOD(put_Active)(ULONG ChannelID, VARIANT_BOOL newVal);
	STDMETHOD(GetChannelsID)(IVariantArray** pVal);
	STDMETHOD(get_UpdateInterval)(ULONG ChannelID, ULONG* pVal);
	STDMETHOD(put_UpdateInterval)(ULONG ChannelID, ULONG newVal);
	STDMETHOD(GetChannelsList)(BSTR FilterServer, BSTR FilterComputer, BSTR FilterName, IDataSet** pRecordSet);
	STDMETHOD(get_Attribute)(ULONG ChannelID, BSTR AttributeName, VARIANT* pVal);
	STDMETHOD(put_Attribute)(ULONG ChannelID, BSTR AttributeName, VARIANT newVal);
	STDMETHOD(AddData)(ULONG ChannelID, VARIANT DateTime, ULONG Quality, VARIANT Data);
	STDMETHOD(GetChannelData)(VARIANT FilterFromDate, VARIANT FilterToDate, VARIANT FilterQuality, enChannelDataTypes DataType, ULONG ChannelID, IDataSet** pVal);
	STDMETHOD(GetChannelInfo)(ULONG ID, BSTR* ServerCLSID, BSTR* ComputerName, BSTR* ChannelName);
	STDMETHOD(GetChannelsDataAsString)(VARIANT IDs, VARIANT FilterFromDate, VARIANT FilterToDate, IDataSet** pVal);
};

OBJECT_ENTRY_AUTO(__uuidof(Channels), CChannels)
typedef CComObject<CChannels> CoChannels;