/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko, Kostya Volovich              *
                                                                          *
 IUnknown part of LightOPCServer/LightOPCGroup
 **************************************************************************/

#include "privopc.h"

#include <opcerror.h>
#if defined(GUID_DEFINED) || defined(_GUID_DEFINED) /* MinGW hack */
#ifndef __IID_DEFINED__
#define __IID_DEFINED__
#endif
#endif
#include <opcda_i.c>
#include <opccomn_i.c>
#if 0
#include <malloc.h>
#define LO_HEAPCHECK(pp) { int hc; if (_HEAPOK != (hc = _heapchk())) \
        UL_ERROR((LOGID, "heapchk() %d %p %s %d", hc, pp, __FILE__, __LINE__)); \
   else UL_TRACE((LOGID, "Release() %p %s %d", pp, __FILE__, __LINE__)); }
#else
#define LO_HEAPCHECK(pp)
#endif
/************************** LightOPCServer::IUnknown  stuff **************************/

STDMETHODIMP_ (ULONG)LightOPCServer::AddRef(void)
{
 UL_DEBUG((LOGID,"AddRef>>> Server %p / %d...", this, RefCount));
 if (loIS_VALID(this)) return (ULONG)InterlockedIncrement(&RefCount);
 UL_WARNING((LOGID, "Server::AddRef(%p) bad this %p", this, iam));
 return 0;
}

STDMETHODIMP_ (ULONG)LightOPCServer::Release(void)
{
 LO_HEAPCHECK(this);
 UL_DEBUG((LOGID,"Release>>> Server %p / %d...", this, RefCount));
 if (loIS_VALID(this))
   {
    ULONG rv;

    if (!(rv = InterlockedDecrement(&RefCount)))
      {
       iam = 0;
       LO_CO_DISCONNECT((IOPCServer*)this, 0);
//       UL_DEBUG((LOGID,"delete Server %p", this/*, mallocX_trap()*/));
       lw_mutex_lock(&async.lk);
       if (!otrk.ot_stopped) otrk.ot_stopped = 1;
       if (!loThrControl_outof01(&async))
         {
          async.tstate = 2;
          lw_conds_signal(&async.cond);
          lw_mutex_unlock(&async.lk);
          UL_NOTICE((LOGID, "Server::Release(%p) Pending", this));
         }
       else
         {
          int ts = async.tstate;
          lw_mutex_unlock(&async.lk);
          UL_ERROR((LOGID, "Server::Release(%p): FAILED: scheduler state is %d",
                    this, ts));
         }
      }
    return rv;
   }
 UL_WARNING((LOGID, "Server::Release(%p) bad this", this, mallocX_trap()));
 return 0;
}

STDMETHODIMP LightOPCServer::QueryInterface(REFIID riid, LPVOID* ppInterface)
{
 HRESULT hr = S_OK;
 void *iface;

// UL_DEBUG((LOG_SR("Server::QueryInterface...")));
 if (ppInterface == NULL) return E_INVALIDARG;
 *ppInterface = iface = NULL;

 if (lock_state("QueryInterface()", &riid)) return LO_E_SHUTDOWN;

 if (qi_chain)
   {
    hr = qi_chain(release_handle_arg, se, this, &riid, ppInterface);
    if (*ppInterface) goto Return;
    hr = S_OK;
   }

#define CASE_IID(iid) else if (IsEqualIID(riid, IID_##iid)) { iface = (iid*)this; \
                        UL_TRACE((LOG_SR("Server::QueryInterface(%s) Ok"), #iid)); }

 if (IsEqualIID(riid, IID_IUnknown))
   {
    iface = this;
    UL_TRACE((LOG_SR("Server::QueryInterface(IUnknown) Ok")));
   }
 CASE_IID(IOPCCommon)
 CASE_IID(IOPCServer)
 CASE_IID(IOPCBrowseServerAddressSpace)
 CASE_IID(IConnectionPoint)
 CASE_IID(IConnectionPointContainer)
 CASE_IID(IOPCItemProperties)
 else
   {
    char ifa[40];
#if LO_USE_FREEMARSHALL
    if (freemarsh)
      {
       hr = freemarsh->QueryInterface(riid, ppInterface);
       if (!FAILED(hr))
         {
          UL_TRACE((LOG_SR("Server::FreeMarsh:QI(%s) Ok"),
		   loTranslateIID(ifa, sizeof(ifa), &riid)));
          goto Return;
         }
      }
#endif
    UL_TRACE((LOG_SR("Server::QueryInterface(%s) Failed"),
		 loTranslateIID(ifa, sizeof(ifa), &riid)));
    hr = E_NOINTERFACE;
    iface = 0;
    goto Return;
   }
#undef CASE_IID

 if (*ppInterface = iface) ((IUnknown*)iface)->AddRef();

Return:

 unlock_state();
 return hr;
}

/************************** LightOPCServer::IOPCServer  stuff **************************/

STDMETHODIMP LightOPCServer::GetStatus(OPCSERVERSTATUS **ppServerStatus)
{
 HRESULT hr = S_OK;
 OPCSERVERSTATUS *sest = 0;
 loWchar *vendor = 0;

 if (!ppServerStatus) return E_INVALIDARG;
 *ppServerStatus = 0;
 if (lock_state("GetStatus()", &IID_IOPCServer/*&IID_IUnknown*/))
   return LO_E_SHUTDOWN;

 UL_DEBUG((LOG_SR("Server::GetStatus(%p)"), ppServerStatus));

 vendor = loComWstrdup(ostatus.szVendorInfo? ostatus.szVendorInfo: L"");
 sest = (OPCSERVERSTATUS*)loComAlloc(sizeof(OPCSERVERSTATUS));

 if (!sest || !vendor) hr = E_OUTOFMEMORY;
 else
   {
    loMilliSec bandw_time;
    *sest = ostatus;
    lock_read();
    //ostatus.ftCurrentTime = sest->ftCurrentTime;
    /* don't GetSystemTimeAsFileTime() inside lock! */
    sest->dwGroupCount = ostatus.dwGroupCount;
    unlock();
    lw_mutex_lock(&async.lk);
    sest->ftLastUpdateTime = ostatus.ftLastUpdateTime;
    sest->dwBandWidth = otrk.ot_stopped? (DWORD)-1: ostatus.dwBandWidth;
    sest->dwServerState = ostatus.dwServerState;
    bandw_time = bandwidth_time;
    lw_mutex_unlock(&async.lk);
    sest->dwBandWidth = lo_adjust_bandwidth(sest->dwBandWidth, bandw_time, lo_millisec());
   }

 if (S_OK != hr)
   {
    if (sest) loComFree(sest);
    if (vendor) loComFree(vendor);
   }
 else
   {
    sest->szVendorInfo = vendor;
    se->driver.ldCurrentTime(&ctxt.cactx, &sest->ftCurrentTime); /* for best accuracy */
    *ppServerStatus = sest;
   }
 UL_TRACE((LOG_SR("Server::GetStatus() = %#x, %d"), hr, ostatus.dwServerState));

 unlock_state();
 return hr;
}

STDMETHODIMP LightOPCServer::SetClientName(LPCWSTR szClientName)
{
 HRESULT hr = S_OK;

 if (!szClientName) return E_INVALIDARG;
 LO_CHECK_STATEz0("SetClientName");
 UL_TRACE((LOG_SR("Server::SetClientName(%ls)"), szClientName));

/* lock_write();
 hr = set_name(szClientName);
 unlock();
//if (hr == S_OK)*/
   lo_wcstombs(client_name, sizeof(client_name)-1, szClientName, sizeof(client_name)-1);
// UL_DEBUG((LOG_SR("Server::SetClientName(%s)"), client_name));
 LO_FINISH();
 return hr;
}

STDMETHODIMP LightOPCServer::GetErrorString(HRESULT dwError, LCID locale, LPWSTR *ppString)
{
 HRESULT hr;
 if (!ppString) return E_INVALIDARG;
 *ppString = 0;
 if (lock_state("GetErrorString()", &IID_IOPCServer/*&IID_IUnknown*/))
   return LO_E_SHUTDOWN;
 hr = loComErrorString(se, &ctxt.cactx, dwError, locale, ppString);
 unlock_state();
 return hr;
}

/***************************** LightOPCServer::IOPCCommon stuff **************************/

#if 1
#define GET_COMM_LCID(lcid)  (lw_mutex_lock(&lk_browse), \
   (lcid) = ctxt.cta.vc_lcid, lw_mutex_unlock(&lk_browse))
#define SET_COMM_LCID(lcid)  (lw_mutex_lock(&lk_browse), \
   ctxt.cta.vc_lcid = (lcid), lw_mutex_unlock(&lk_browse))
#else
#define GET_COMM_LCID(lcid)  (lock_read(), (lcid) = ctxt.cta.vc_lcid, unlock())
#define SET_COMM_LCID(lcid)  (lock_write(), ctxt.cta.vc_lcid = (lcid), unlock())
#endif

STDMETHODIMP LightOPCServer::GetErrorString(LONG dwError, LPWSTR *ppString)
{
 HRESULT hr;
 LCID locale;
 if (!ppString) return E_INVALIDARG;
 *ppString = 0;
 if (lock_state("GetErrorString()", &IID_IOPCCommon/*&IID_IUnknown*/))
   return LO_E_SHUTDOWN;
 GET_COMM_LCID(locale);
 hr = loComErrorString(se, &ctxt.cactx, dwError, locale, ppString);
 unlock_state();
 return hr;
}

STDMETHODIMP LightOPCServer::SetLocaleID(LCID dwLcid)
{
 HRESULT hr = S_OK;
 LO_CHECK_STATEz0("SetLocaleID");
 UL_TRACE((LOG_SR("Server::SetLocaleID(0x%08lX)"), dwLcid));
 if (se->driver.ldCheckLocale?
     se->driver.ldCheckLocale(&ctxt.cactx, dwLcid):
     (~0xfffff & dwLcid)/*!IsValidLocale(dwLcid, LCID_INSTALLED)*/) hr = E_INVALIDARG;
/* else if (se->driver.ldGetErrorString &&
          0 == se->driver.ldGetErrorString(se, this, OPC_E_BADTYPE, dwLcid, 0, 0) )
   hr = E_INVALIDARG;*/
 else SET_COMM_LCID(dwLcid);
 LO_FINISH();
 return hr;
}

STDMETHODIMP LightOPCServer::GetLocaleID(LCID *dwLcid)
{
 HRESULT hr = S_OK;
 LO_CHECK_STATEz0("GetLocaleID");
 UL_TRACE((LOG_SR("Server::GetLocaleID()")));
 if (!dwLcid) hr = E_INVALIDARG;
 else GET_COMM_LCID(*dwLcid);
 LO_FINISH();
 return hr;
}

STDMETHODIMP LightOPCServer::QueryAvailableLocaleIDs(DWORD* pdwCount, LCID** pdwLcid)
{
 HRESULT hr = S_OK;

 LO_CHECK_STATEz2("QueryAvailableLocaleIDs", pdwLcid, pdwCount);
 if (!pdwLcid || !pdwCount) hr = E_INVALIDARG;
 else if (se->driver.ldQueryAvailableLocaleIDs)
   hr = se->driver.ldQueryAvailableLocaleIDs(&ctxt.cactx, pdwCount, pdwLcid);
 else
   {
    LCID *buf = (LCID*)loComAlloc(sizeof(LCID)*2);
    if (!buf) hr = E_FAIL; /*E_OUTOFMEMORY ??*/
    else
      {
       buf[0] = MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT);
       buf[1] = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL), SORT_DEFAULT);
       *pdwLcid = buf; *pdwCount = 2;
      }
   }
 LO_FINISH();
 return hr;
}

/*************************** LightOPCGroup::IUnknown stuff **************************/

STDMETHODIMP_ (ULONG)LightOPCGroup::AddRef(void)
{
 if (loIS_VALID(this)) return (ULONG)InterlockedIncrement(&RefCount);
 UL_WARNING((LOGID, "Group::AddRef(%p) bad this", this));
 return 0;
}

STDMETHODIMP_ (ULONG)LightOPCGroup::Release(void)
{
 LO_HEAPCHECK(this);
 if (loIS_VALID(this))
   {
    ULONG rv;
    if (!(rv = InterlockedDecrement(&RefCount)))
      {
       iam = 0;
       LO_CO_DISCONNECT((IOPCGroupStateMgt*)this, 0);
       if (initphase & 1/*===ifATTACHED*/)
         {
          UL_ERROR((LOGID, "Releasing of an ATTACHED group %p %p", this, owner));
          return 0;
         }
//       UL_DEBUG((LOGID,"delete Group %p", this, mallocX_trap()));
       delete this;
      }
    return rv;
   }
 UL_WARNING((LOGID, "Group::Release(%p) bad this", this, mallocX_trap()));
 return 0;
}

STDMETHODIMP LightOPCGroup::QueryInterface(REFIID riid, LPVOID* ppInterface)
{
 HRESULT hr = S_OK;
 void *iface = 0;

// UL_DEBUG((LOG_GRHO("Group::QueryInterface...")));
 if (ppInterface == NULL) return E_INVALIDARG;

 *ppInterface = iface = NULL;
 if (!iam_valid()) return LO_E_SHUTDOWN;

#define CASE_IID(iid) else if (IsEqualIID(riid, IID_##iid)) { iface = (iid*)this; \
                        UL_TRACE((LOG_GRHO("Group::QueryInterface(%s) Ok"), #iid)); }

 if (IsEqualIID(riid, IID_IUnknown))
   {
    iface = this;
    UL_TRACE((LOG_GRHO("Group::QueryInterface(IUnknown) Ok")));
   }
 CASE_IID(IOPCGroupStateMgt)
 CASE_IID(IOPCPublicGroupStateMgt)
 CASE_IID(IOPCItemMgt)
 CASE_IID(IOPCSyncIO)
 CASE_IID(IOPCAsyncIO)
 CASE_IID(IOPCAsyncIO2)
 CASE_IID(IDataObject)
 CASE_IID(IConnectionPoint)
 CASE_IID(IConnectionPointContainer)
 else
   {
    char ifa[40];
#if LO_USE_FREEMARSHALL
    if (freemarsh)
      {
       hr = freemarsh->QueryInterface(riid, ppInterface);
       if (!FAILED(hr))
         {
          UL_TRACE((LOG_GRHO("Group::FreeMarsh:QI(%s) Ok"),
		   loTranslateIID(ifa, sizeof(ifa), &riid)));
          goto Return;
         }
      }
#endif
    UL_TRACE((LOG_GRHO("Group::QueryInterface(%s) Failed"),
               loTranslateIID(ifa, sizeof(ifa), &riid)));
    hr = E_NOINTERFACE;
    iface = 0;
    goto Return;
   }
#undef CASE_IID

 if (*ppInterface = iface)
   {
    ((IUnknown*)iface)->AddRef();
    if (!iam_valid())
      {
       ((IUnknown*)iface)->Release();
       hr = LO_E_SHUTDOWN;
       *ppInterface = 0;
      }
   }
Return:
 return hr;
}

/* IOPCPublicGroupStateMgt implementation */

STDMETHODIMP LightOPCGroup::GetState(BOOL *pPublic)
{
 HRESULT hr = S_OK;
 LO_CHECK_STATEz0("GetState");
 if (pPublic) *pPublic = 0;
 else hr = E_INVALIDARG;
 UL_TRACE((LOG_GRH("public-GetState() = private")));
 LO_FINISH();
 return hr;
}

STDMETHODIMP LightOPCGroup::MoveToPublic(void)
{
 HRESULT hr = S_OK;
 LO_CHECK_STATEz0("MoveToPublic");
 UL_TRACE((LOG_GRH("MoveToPublic() = ENOTIMPL")));
 hr = E_NOTIMPL;
 LO_FINISH();
 return hr;
}

/* end of servunk.cpp */
