/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
                                                                          *
 IOPCGroup advise data connection
 **************************************************************************/

#include <opcerror.h>
#include "privopc.h"
#include LO_OLE_ERRORS_h /*CONNECT_E_ADVISELIMIT*/

#define loSearchIID(x)

/*** IConnectionPoint methods *********************************************/

STDMETHODIMP LightOPCGroup::GetConnectionInterface(IID *pIID)
{
 if (!pIID) return E_POINTER;
 *pIID = IID_IOPCDataCallback;
 UL_NOTICE((LOG_GRHO("Group::GetConnIface() ")));
 return S_OK;
}

STDMETHODIMP LightOPCGroup::Advise(IUnknown *pUnk, DWORD *pdwCookie)
{
 HRESULT hr = S_OK;

 LO_CHECK_STATEz1("icpAdvise", pdwCookie);
 if (!pUnk || !pdwCookie) { hr = E_INVALIDARG; goto Return; }

 loSearchIID(pUnk);

 hr = owner->async_advise(loRQ_CONN_DATABACK | loRQ_SYNC,
                          &IID_IOPCDataCallback, this, ServerHandle, pUnk);
 if (E_NOINTERFACE == hr) hr = CONNECT_E_CANNOTCONNECT;
 else if (!FAILED(hr)) *pdwCookie = loRQ_CONN_DATABACK;

Return:
 if (hr == S_OK)
   UL_NOTICE((LOGID, "Group::icpAdvise(IOPCDataCallback) Ok"));
 else
   UL_INFO((LOGID, "%!l Group::icpAdvise(IOPCDataCallback) FAILED", hr));

 LO_FINISH();
 return hr;
}

STDMETHODIMP LightOPCGroup::Unadvise(DWORD dwCookie)
{
 HRESULT hr = S_OK;

 LO_CHECK_STATEz0("icpUnAdvise");

 if (loRQ_CONN_DATABACK != dwCookie) hr = CONNECT_E_NOCONNECTION;
 else
   {
    hr = owner->async_unadvise(loRQ_CONN_DATABACK | loRQ_SYNC,
                               &IID_IOPCDataCallback, this, ServerHandle);
    if (hr == OLE_E_NOCONNECTION) hr = CONNECT_E_NOCONNECTION;
   }

 if (S_OK == hr) UL_NOTICE((LOG_GRH("icpUnAdvise(%x) %s"), dwCookie, loStrError(hr)));
 else UL_INFO((LOG_GRH("icpUnAdvise(%x) %s"), dwCookie, loStrError(hr)));

 LO_FINISH();
 return hr;
}

STDMETHODIMP LightOPCGroup::SetEnable(BOOL bEnable)
{
 HRESULT hr = S_OK;
 int oen;

 LO_CHECK_STATEz0("SetEnable");

 oen = bEnable? loRQ_CONN_DATA
              :(loRQ_CONN_DATA & ~loRQ_CONN_DATABACK);
 lock_write();
 if (!conn_databack) hr = CONNECT_E_NOCONNECTION;
 else if (oen != advise_enabled)
    {
     advise_enabled = oen;
     if (bEnable) actuate_async(1);
    }
 unlock();

 UL_NOTICE((LOG_GRH("SetEnable(%d) %s"), bEnable, loStrError(hr)));
 LO_FINISH();
 return S_OK;
}

STDMETHODIMP LightOPCGroup::GetEnable(BOOL *pbEnable)
{
 HRESULT hr = S_OK;
 if (pbEnable)
   {
    *pbEnable = advise_enabled & loRQ_CONN_DATABACK? TRUE: FALSE;
    if (!conn_databack) hr = CONNECT_E_NOCONNECTION;
   }
 else hr = E_INVALIDARG;
 UL_NOTICE((LOGID, "Group::GetEnable(%x) %s",
      advise_enabled & loRQ_CONN_DATABACK, loStrError(hr)));
 return hr;
}


/*** IDataObject methods ********************************************/

STDMETHODIMP LightOPCGroup::DAdvise(FORMATETC *pformatetc,
                                    DWORD advf,
                                    IAdviseSink *pAdvSink,
                                    DWORD *pdwConnection)
{
 HRESULT hr = S_OK;
 loService *se;
 int conn = 0;

 LO_CHECK_STATEz1("DAdvise", pdwConnection);

 if (!pdwConnection || !pAdvSink || !pformatetc) { hr = E_INVALIDARG; goto Return; }
 se = owner->se;

 if (pformatetc->cfFormat == se->cform_datatime)
    conn = loRQ_CONN_DATATIME;
 else if (pformatetc->cfFormat == se->cform_writecompl)
    conn = loRQ_CONN_WRITECOMPL;
 else if (pformatetc->cfFormat == se->cform_dataonly)
    conn = loRQ_CONN_DATAONLY;

 if (!conn ||
     /* pformatetc->dwAspect != DVASPECT_CONTENT ||
     pformatetc->ptd != NULL || pformatetc->lindex != -1 ||*/
     pformatetc->tymed != TYMED_HGLOBAL) { hr = DV_E_FORMATETC; goto Return; }

 loSearchIID(pAdvSink);

 hr = owner->async_advise(conn | loRQ_SYNC, &IID_IAdviseSink, this, ServerHandle, pAdvSink);
 if (CONNECT_E_CANNOTCONNECT == hr) hr = E_NOINTERFACE;
 else if (!FAILED(hr)) *pdwConnection = (DWORD)conn;

Return:

 if (S_OK == hr)
   UL_NOTICE((LOG_GRH("DAdvise(%x) Ok"), conn));
 else
   UL_INFO((LOG_GRH("Group:DAdvise(%x) FAILED %s"), conn, loStrError(hr)));

 LO_FINISH();
 return hr;
}


STDMETHODIMP LightOPCGroup::DUnadvise(DWORD dwConnection)
{
 HRESULT hr = S_OK;

 LO_CHECK_STATEz0("DUnadvise");

 switch(dwConnection)
   {
 case loRQ_CONN_DATAONLY:
 case loRQ_CONN_DATATIME:
 case loRQ_CONN_WRITECOMPL:
    break;
 default: hr = OLE_E_NOCONNECTION; goto Return;
   }

 hr = owner->async_unadvise(loRQ_SYNC | (int)dwConnection, &IID_IAdviseSink, this, ServerHandle);
 if (CONNECT_E_NOCONNECTION == hr) hr = OLE_E_NOCONNECTION;

Return:
 if (S_OK == hr)
    UL_NOTICE((LOG_GRH("DUnadvise(%x) Ok"), dwConnection));
 else
    UL_INFO((LOG_GRH("DUnadvise(%x) FAILED %s"), dwConnection, loStrError(hr)));

 LO_FINISH();
 return hr;
}

STDMETHODIMP LightOPCGroup::EnumFormatEtc(DWORD, IEnumFORMATETC **){ return E_NOTIMPL; }

STDMETHODIMP LightOPCGroup::EnumDAdvise(IEnumSTATDATA **) { return E_NOTIMPL; }

STDMETHODIMP LightOPCGroup::GetCanonicalFormatEtc(FORMATETC *, FORMATETC *)
{ return E_NOTIMPL; }

STDMETHODIMP LightOPCGroup::QueryGetData(FORMATETC *)             { return E_NOTIMPL; }

STDMETHODIMP LightOPCGroup::SetData(FORMATETC *, STGMEDIUM *, BOOL){ return E_NOTIMPL; }

STDMETHODIMP LightOPCGroup::GetData(FORMATETC *, STGMEDIUM *)     { return E_NOTIMPL; }

STDMETHODIMP LightOPCGroup::GetDataHere(FORMATETC *, STGMEDIUM *) { return E_NOTIMPL; }

/* end of advise_d.cpp */
