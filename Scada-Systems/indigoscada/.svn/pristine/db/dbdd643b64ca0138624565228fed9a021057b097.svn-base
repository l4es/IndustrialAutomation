#ifndef __OPCHDACLASSES_H
#define __OPCHDACLASSES_H


/*
typedef struct tagOPCHDA_ITEM
    {
    OPCHANDLE hClient;
    DWORD haAggregate;
    DWORD dwCount;
    // [size_is] 
	FILETIME *pftTimeStamps;
    // [size_is] 
	DWORD *pdwQualities;
    // [size_is] 
	VARIANT *pvDataValues;
    } 	OPCHDA_ITEM;
*/

//////////////////////////class COPCHistoricDASink//////////////////////////////////////

class ATL_NO_VTABLE COPCHistoricDASink : 
	public CComObjectRoot,
	public IOPCHDA_DataCallback
{
public:
	COPCHistoricDASink(){}

BEGIN_COM_MAP(COPCHistoricDASink)
	//COM_INTERFACE_ENTRY_IID( __uuidof(IOPCHDA_DataCallback), IOPCHDA_DataCallback)
	COM_INTERFACE_ENTRY(IOPCHDA_DataCallback)
END_COM_MAP()

	HRESULT STDMETHODCALLTYPE OnDataChange( 
            /* [in] */ DWORD dwTransactionID,
            /* [in] */ HRESULT hrStatus,
            /* [in] */ DWORD dwNumItems,
            /* [size_is][in] */ OPCHDA_ITEM *pItemValues,
            /* [size_is][in] */ HRESULT *phrErrors) 
	{
		IT_IT("COPCHistoricDASink::OnDataChange");

		//IMPORTANT NOTE: no blocking function may be called here
	
		return S_OK;
	};

	HRESULT STDMETHODCALLTYPE OnReadComplete( 
            /* [in] */ DWORD dwTransactionID,
            /* [in] */ HRESULT hrStatus,
            /* [in] */ DWORD dwNumItems,
            /* [size_is][in] */ OPCHDA_ITEM *pItemValues,
            /* [size_is][in] */ HRESULT *phrErrors){ return S_OK;};
        
    HRESULT STDMETHODCALLTYPE OnReadModifiedComplete( 
            /* [in] */ DWORD dwTransactionID,
            /* [in] */ HRESULT hrStatus,
            /* [in] */ DWORD dwNumItems,
            /* [size_is][in] */ OPCHDA_MODIFIEDITEM *pItemValues,
            /* [size_is][in] */ HRESULT *phrErrors){return S_OK;};
        
    HRESULT STDMETHODCALLTYPE OnReadAttributeComplete( 
            /* [in] */ DWORD dwTransactionID,
            /* [in] */ HRESULT hrStatus,
            /* [in] */ OPCHANDLE hClient,
            /* [in] */ DWORD dwNumItems,
            /* [size_is][in] */ OPCHDA_ATTRIBUTE *pAttributeValues,
            /* [size_is][in] */ HRESULT *phrErrors){return S_OK;};
        
    HRESULT STDMETHODCALLTYPE OnReadAnnotations( 
            /* [in] */ DWORD dwTransactionID,
            /* [in] */ HRESULT hrStatus,
            /* [in] */ DWORD dwNumItems,
            /* [size_is][in] */ OPCHDA_ANNOTATION *pAnnotationValues,
            /* [size_is][in] */ HRESULT *phrErrors){return S_OK;};
        
    HRESULT STDMETHODCALLTYPE OnInsertAnnotations( 
            /* [in] */ DWORD dwTransactionID,
            /* [in] */ HRESULT hrStatus,
            /* [in] */ DWORD dwCount,
            /* [size_is][in] */ OPCHANDLE *phClients,
            /* [size_is][in] */ HRESULT *phrErrors){return S_OK;};
        
    HRESULT STDMETHODCALLTYPE OnPlayback( 
            /* [in] */ DWORD dwTransactionID,
            /* [in] */ HRESULT hrStatus,
            /* [in] */ DWORD dwNumItems,
            /* [size_is][in] */ OPCHDA_ITEM **ppItemValues,
            /* [size_is][in] */ HRESULT *phrErrors){return S_OK;};
        
        HRESULT STDMETHODCALLTYPE OnUpdateComplete( 
            /* [in] */ DWORD dwTransactionID,
            /* [in] */ HRESULT hrStatus,
            /* [in] */ DWORD dwCount,
            /* [size_is][in] */ OPCHANDLE *phClients,
            /* [size_is][in] */ HRESULT *phrErrors){return S_OK;};
        
        HRESULT STDMETHODCALLTYPE OnCancelComplete( 
		/* [in] */ DWORD dwCancelID){return S_OK;};
};

typedef CComObject<COPCHistoricDASink> CComCOPCHistoricDASink;

//////////////////////////class COPCShutdownRequest//////////////////////////////////////

class ATL_NO_VTABLE COPCShutdownRequest : 
	//public CComObjectRootEx<CComSingleThreadModel>,
	public CComObjectRoot,
	public IOPCShutdown
{
public:
	COPCShutdownRequest(){}

BEGIN_COM_MAP(COPCShutdownRequest)
	//COM_INTERFACE_ENTRY_IID( __uuidof(IOPCShutdown), IOPCShutdown)
	COM_INTERFACE_ENTRY(IOPCShutdown)
END_COM_MAP()

	HRESULT STDMETHODCALLTYPE ShutdownRequest( 
            /* [string][in] */ LPCWSTR szReason)
	{
		char show_msg[150];
		sprintf(show_msg, "Remote HDA server sends shutdown request");

		Opc_client_hda_imp::LogMessage(S_OK, "", show_msg);
		return S_OK;
	};
};

typedef CComObject<COPCShutdownRequest> CComCOPCShutdownRequest;


#endif