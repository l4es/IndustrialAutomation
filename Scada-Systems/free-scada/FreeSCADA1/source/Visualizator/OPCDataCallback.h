#pragma once

#include "../3rdParty/opc/opcda.h"

class COPCDataCallback : public IOPCDataCallback
{
public:

	class CDoc* m_Doc;

	COPCDataCallback() 
	{
		m_ulRefs = 1;
		m_Doc = NULL;
	}

	STDMETHODIMP QueryInterface(REFIID iid, LPVOID* ppInterface) 
	{
		if (ppInterface == NULL)
		{
			return E_INVALIDARG;
		}

		if (iid == IID_IUnknown)
		{
			*ppInterface = dynamic_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}

		if (iid == IID_IOPCDataCallback)
		{
			*ppInterface = dynamic_cast<IOPCDataCallback*>(this);
			AddRef();
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement((LONG*)&m_ulRefs); 
	}

	STDMETHODIMP_(ULONG) Release()
	{
		ULONG ulRefs = InterlockedDecrement((LONG*)&m_ulRefs); 

		if (ulRefs == 0) 
		{ 
			delete this; 
			return 0; 
		} 

		return ulRefs; 
	}

	STDMETHODIMP OnDataChange(
		DWORD       dwTransid, 
		OPCHANDLE   hGroup, 
		HRESULT     hrMasterquality,
		HRESULT     hrMastererror,
		DWORD       dwCount, 
		OPCHANDLE * phClientItems, 
		VARIANT   * pvValues, 
		WORD      * pwQualities,
		FILETIME  * pftTimeStamps,
		HRESULT   * pErrors
		);

	STDMETHODIMP OnReadComplete(
		DWORD       dwTransid, 
		OPCHANDLE   hGroup, 
		HRESULT     hrMasterquality,
		HRESULT     hrMastererror,
		DWORD       dwCount, 
		OPCHANDLE * phClientItems, 
		VARIANT   * pvValues, 
		WORD      * pwQualities,
		FILETIME  * pftTimeStamps,
		HRESULT   * pErrors
		);

	STDMETHODIMP OnWriteComplete(
		DWORD       dwTransid, 
		OPCHANDLE   hGroup, 
		HRESULT     hrMastererr, 
		DWORD       dwCount, 
		OPCHANDLE * pClienthandles, 
		HRESULT   * pErrors
		);

	// OnCancelComplete
	STDMETHODIMP OnCancelComplete(
		DWORD       dwTransid, 
		OPCHANDLE   hGroup
		);

private:
	ULONG m_ulRefs;
};
