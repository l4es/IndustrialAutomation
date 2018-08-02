/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
                                                                          *
 LightOPCServer:: IOPCShutdown related routines
 **************************************************************************/
#include "privopc.h"
#include LO_OLE_ERRORS_h

#ifndef loShutdownConn_init
void loShutdownConn_init(loShutdownConn *sc)
{
 sc->request = 0; sc->reason = 0;
}
#endif

void loShutdownConn_clear(loShutdownConn *sc)
{
 if (sc->request) sc->request->Release();
 UL_DEBUG((LOGID, "loShutdownConn_clear(%p)", sc->request));
 if (sc->reason) freeX(sc->reason);
 loShutdownConn_init(sc);
}

void loShutdownConn_call(loShutdownConn *sc)
{
 if (sc->request)
   {
    UL_NOTICE((LOGID, "Going to ShutdownRequest(%ls)", loWnul(sc->reason)));
    sc->request->ShutdownRequest(sc->reason? sc->reason: L"");
    sc->request->Release();
    sc->request = 0;
   }
#if 1
   /* we need a duplicate of "reason" to perform the call;
      using reason[] is good idea */
 if (sc->reason) freeX(sc->reason), sc->reason = 0;
#endif
}

/*********************** LightOPCServer::IConnectionPoint stuff **********************/

HRESULT LightOPCServer::icp_AddRef(void)
{
 LO_CHECK_STATEz0("Server::IConnPoint::AddRef");
 return S_OK; }
}

void LightOPCServer::icp_Release(void)
{
 { LO_FINISH();
}

loObjTracker *LightOPCServer::icp_ObjTracker(void)
{
  return &otrk;
}

/*** IConnectionPoint methods *********************************************/

STDMETHODIMP LightOPCServer::GetConnectionInterface(IID *pIID)
{
 if (!pIID) return E_POINTER;
 *pIID = IID_IOPCShutdown;
 UL_NOTICE((LOG_SR("Server::GetConnIface() ")));
 return S_OK;
}

STDMETHODIMP LightOPCServer::Advise(IUnknown *pUnk, DWORD *pdwCookie)
{
 HRESULT hr = S_OK;

 LO_CHECK_STATEz1("icpAdvise", pdwCookie);
 if (!pUnk || !pdwCookie) { hr = E_INVALIDARG; goto Return; }

 hr = async_advise(loRQ_CONN_SHUTDOWN | loRQ_SYNC, &IID_IOPCShutdown, 0, 0, pUnk);
 if (E_NOINTERFACE == hr) hr = CONNECT_E_CANNOTCONNECT;
 else if (!FAILED(hr)) *pdwCookie = loRQ_CONN_SHUTDOWN;

Return:
 if (hr == S_OK)
   UL_NOTICE((LOGID, "Server::icpAdvise(IOPCShutdown) Ok"));
 else
   {
    UL_INFO((LOGID, "%!l Server::icpAdvise(IOPCShutdown) FAILED", hr));
   }

 LO_FINISH();
 return hr;
}

STDMETHODIMP LightOPCServer::Unadvise(DWORD dwCookie)
{
 HRESULT hr = S_OK;

 LO_CHECK_STATEz0("icpUnAdvise");

 if (loRQ_CONN_SHUTDOWN != dwCookie) hr = CONNECT_E_NOCONNECTION;
 else
   {
    hr = async_unadvise(loRQ_CONN_SHUTDOWN | loRQ_SYNC, &IID_IOPCShutdown, 0, 0);
    if (hr == OLE_E_NOCONNECTION) hr = CONNECT_E_NOCONNECTION;
   }

 if (S_OK == hr) UL_NOTICE((LOG_SR("icpUnAdvise(%u) %s"), dwCookie, loStrError(hr)));
 else UL_INFO((LOG_SR("icpUnAdvise(%u) %s"), dwCookie, loStrError(hr)));

 LO_FINISH();
 return hr;
}

/*************************************************************************/

/* end of clidown.cpp */
