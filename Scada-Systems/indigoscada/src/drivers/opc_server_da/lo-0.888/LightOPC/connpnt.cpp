/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
                                                                          *
 Main part of IConnectionPoint implementation
 **************************************************************************/

#include "privopc.h"
#include "connpnt.h"
#include LO_OLE_ERRORS_h

#include "enum.h"

/******* ConnectionPoint itself *******************************************/
/******* IConnectionPointContainer methods ********************************/

STDMETHODIMP loConnPnt1::FindConnectionPoint(REFIID riid, IConnectionPoint **ppCP)
{
 HRESULT hr = S_OK;
 IID myiid;
 if (!ppCP) hr = E_POINTER;
 else if (S_OK == (hr = GetConnectionInterface(&myiid)))
   if (!IsEqualIID(riid, myiid)) hr = CONNECT_E_NOCONNECTION;
   else if (S_OK == (hr = icp_AddRef()))
     {
//      IConnectionPoint::AddRef();
      (*ppCP = this)->AddRef();
      icp_Release();
     }

 UL_NOTICE((LOGID, "FindConnPoints() %s", loStrError(hr)));
 return hr;
}

STDMETHODIMP loConnPnt1::EnumConnectionPoints(IEnumConnectionPoints **ppEnum)
{
 HRESULT hr = S_OK;

 if (!ppEnum) hr = E_POINTER;
 else
   {
    *ppEnum = 0;
    if (S_OK == (hr = icp_AddRef()))
      {
       loEnumConnPnt1 *en = new loEnumConnPnt1(this);
       if (en) 
         {
          if (S_OK == (hr = en->initiate(icp_ObjTracker())))
            {
             *ppEnum = en;
#if LO_USE_FREEMARSHALL
             if (!freemarsh) en->DisableFreemarsh();
#else
             en->DisableFreemarsh();
#endif
            }
          else delete en;
         }
       else hr = E_OUTOFMEMORY;
       icp_Release();
      }
   }
 UL_NOTICE((LOGID, "EnumConnPoints() %s", loStrError(hr)));
 return hr;
}

/*** IConnectionPoint methods ****************************/

STDMETHODIMP loConnPnt1::GetConnectionPointContainer(IConnectionPointContainer **ppCPC)
{
 HRESULT hr = S_OK;
 if (!ppCPC) hr = E_POINTER;
 else if (S_OK == (hr = icp_AddRef()))
   {
//    IConnectionPointContainer::AddRef();
    (*ppCPC = this)->AddRef();
    icp_Release();
   }
 UL_NOTICE((LOGID, "GetConnPointContainer() %s", loStrError(hr)));
 return hr;
}

STDMETHODIMP loConnPnt1::EnumConnections(IEnumConnections **ppEnum)
{ return E_NOTIMPL; }

/* end of connpnt.cpp */
