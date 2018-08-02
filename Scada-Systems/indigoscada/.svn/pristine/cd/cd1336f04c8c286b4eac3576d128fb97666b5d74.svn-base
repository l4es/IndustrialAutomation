/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
                                                                          *
 IOPCServer advise data & shutdown connections
 **************************************************************************/
#include <opcerror.h>
#include "privopc.h"
#include LO_OLE_ERRORS_h /*CONNECT_E_ADVISELIMIT*/

static HRESULT group_advise(LightOPCGroup *grp, int advconn, void **arg)
{
 HRESULT hr = S_OK;
 void **ias = 0;

 if (!arg) hr = E_INVALIDARG;
 else
   switch(advconn &= loRQ_CONN_MASK)
     {
   case loRQ_CONN_DATABACK:   ias = &grp->conn_databack;   break;
   case loRQ_CONN_DATATIME:   ias = &grp->conn_datatime;   break;
   case loRQ_CONN_WRITECOMPL: ias = &grp->conn_writecompl; break;
   case loRQ_CONN_DATAONLY:   ias = &grp->conn_dataonly;   break;
   case loRQ_CONN_DATA_W:
      if (0 == *arg) grp->clear_advise();
      else hr = E_INVALIDARG;
      break;
   default: hr = E_INVALIDARG; break;
     }

 if (ias)
   {
    if (*arg)
      {
       if (*ias) hr = CONNECT_E_ADVISELIMIT;
       else
         {
          int oldsink = grp->advise_present;
          *ias = *arg; *arg = 0;
          grp->advise_present |= advconn;
#if 0  /* Do we preserve SetEnable() state across Unadvise()/Advise()? */
          grp->advise_enabled |= advconn & loRQ_CONN_DATA;
#endif
         if (grp->Active &&
             !(oldsink & loRQ_CONN_DATA) &&
              (advconn & loRQ_CONN_DATA & grp->advise_enabled) )
           grp->actuate_async(1);

         if (((oldsink |= advconn) & loRQ_CONN_DATA_1) &&
             (oldsink & loRQ_CONN_DATABACK))
           UL_WARNING((LOGID, "icpAdvise(%ls/%x) IAdviseSink & IOPCDataCallback both are advised",
                       grp->name? grp->name: L"", grp->ServerHandle));
         }
      }
    else /* arg == 0 ==> unadvise */
      {
       if ((*arg = *ias)) *ias = 0, grp->advise_present &= ~advconn;
       else  hr = CONNECT_E_NOCONNECTION;
      }
   }
 UL_DEBUG((LOGID, "group_advize(%x) old:%p->new:%p",
            advconn, arg? *arg: (void*)-1, ias? *ias: (void*)-1));
 return hr;
}


HRESULT LightOPCServer::sync_advise(int operation, void **arg, loRqid grp_key)
{
 int advconn;
 void *oarg;
 HRESULT hr = S_OK;

 advconn = operation & loRQ_CONN_MASK;
 operation &= loRQ_OPER_MASK;

 if (!arg ||
#ifdef loRQ_OP_UNADVISE
      *arg && operation != loRQ_OP_ADVISE ||
     !*arg && operation != loRQ_OP_UNADVISE)
#else
      operation != loRQ_OP_ADVISE)
#endif
   {
    hr = E_INVALIDARG; goto Return;
   }
 oarg = *arg;

 if (advconn == loRQ_CONN_SHUTDOWN)
   {
    if (grp_key) { hr = E_INVALIDARG; goto Return; }
    lw_mutex_lock(&async.lk);
    if (*arg)
      {
       if (shutdown_conn.request) hr = CONNECT_E_ADVISELIMIT;
       else
         {
          shutdown_conn.request = (IOPCShutdown*)*arg;
          *arg = 0;
         }
      }
    else
      {
       if (!(*arg = shutdown_conn.request)) hr = CONNECT_E_NOCONNECTION;
       else shutdown_conn.request = 0;
      }
    lw_mutex_unlock(&async.lk);
   }
 else
   {
    LightOPCGroup *grp;
    if (!grp_key) { hr = E_INVALIDARG; goto Return; }
    lock_write();
    if ((grp = by_handle_g(grp_key)))
      {
       hr = group_advise(grp, advconn, arg);
      }
    else hr = LO_E_SHUTDOWN;
    unlock();
   }

Return:
 if (hr == E_INVALIDARG)
   UL_WARNING((LOG_SR("sync_advise(oper:%x arg:%p group:%x) BAD OPERATION"),
               operation, *arg, grp_key));
 UL_DEBUG((LOGID, "sync_advize(%x) old:%p->new:%p",
                operation|advconn, arg? *arg: (void*)-1, oarg));
 return hr;
}

//#include "fiface.cpp"

HRESULT LightOPCServer::async_advise(int advconn, const IID *iface,
                                     LightOPCGroup *grp, unsigned grp_key,
                                     IUnknown *pUnk)
{
 HRESULT hr = S_OK;
 void *arg = 0;
 int oper;

 oper = advconn & loRQ_OPER_MASK;
 if (loRQ_OP_ADVISE != oper)
   if (0 == oper) oper = loRQ_OP_ADVISE;
   else { hr = E_INVALIDARG; goto Return; }
 advconn |= oper;

 UL_TRACE((LOGID, "async_advise(%x) entered", advconn));

 if (pUnk)
   {
#if 0
    if (IsEqualIID(*iface, IID_IOPCShutdown))
      {
       pUnk = (IUnknown*) new loOPCShutdown();
      }
#endif
#if 0
    IEnumString *es = (IEnumString*)new loEnumString(0);
    IEnumString *esu = 0;
    if (es)
      {
       extern IUnknown *lo_remarshal_iface(IStream **strm, const IID *iface);
       IStream *str = 0;
       es->QueryInterface(IID_IEnumString, (void**)&esu);
       hr = CoMarshalInterThreadInterfaceInStream(IID_IEnumString, esu, &str);
       UL_DEBUG((LOGID, "%!l Marshal %p->%p->%p", hr, es, esu, str));
       UL_DEBUG((LOGID, "Release()=%u", esu->Release()));
//Sleep(13 * 60 * 1000);
       esu = (IEnumString*)lo_remarshal_iface(&str, &IID_IEnumString);
       UL_DEBUG((LOGID, "rem-> %p", esu));
       UL_DEBUG((LOGID, "Release()=%u", esu->Release()));
       lo_release_stream(&str, &IID_IEnumString);
       UL_DEBUG((LOGID, "DONE:"));
      }
#endif
//if (grp) { pUnk = (IEnumString*)(new loEnumString(0)); iface = &IID_IEnumString; }
//advconn &= ~loRQ_SYNC;
    hr = pUnk->QueryInterface(*iface, &arg);
    if (S_OK != hr) goto Return;
    if (0 == arg) { hr = E_NOINTERFACE; goto Return; }
   }
 UL_DEBUG((LOGID, "async_advize::QI %p->%p", pUnk, arg));

#if LO_USE_BOTHMODEL
 if (ldFlags & loDf_BOTHMODEL)
   {
    loRequest *rq = lo_req_alloc(0, 0);
    if (!rq) { hr = E_OUTOFMEMORY; goto Return; }
    rq->operation = advconn;
    rq->group_key = grp? grp->ServerHandle: grp_key;
    rq->serv_key = serv_key;
    rq->conn_iface = iface;
    rq->conn_stream = 0;

    if (arg)
      {
       hr = CoMarshalInterThreadInterfaceInStream(*iface, (LPUNKNOWN)arg, &rq->conn_stream);
       UL_DEBUG((LOGID, "async_advize::IStrm i:%p->s:%p", arg, rq->conn_stream));
      }

    if (hr == S_OK)
      {
       if (advconn & loRQ_SYNC)
         {
          hr = (rq = lo_req_put_sync(&q_req, &q_ret, rq))?
                rq->upl.master_err: E_FAIL;
         }
       else
         {
          hr = lo_req_put_async(&q_req, rq)? S_FALSE: E_FAIL;
          rq = 0;
         }
      }
    if (rq) lo_req_free(rq);
   }
 else
#endif /* LO_USE_BOTHMODEL */
   {
    if (grp)
      {
       lock_write();
       hr = group_advise(grp, advconn, &arg);
       unlock();
      }
    else
      {
       hr = sync_advise(advconn, &arg, grp_key);
      }
   }

Return:
 if (hr == E_INVALIDARG)
   UL_WARNING((LOG_SR("async_advise(oper:%x arg:%p group:%p) INTERNAL ERROR"),
               advconn, arg, grp));
 if (arg) ((IUnknown*)arg)->Release();
 UL_DEBUG((LOGID, "async_advize::Release(i:%p)", arg));

 return hr;
}

#if LO_USE_BOTHMODEL
#ifdef __cplusplus
#define IFACE *iface
#else
#define IFACE  iface
#endif

void lo_release_stream(IStream **strm, const IID *iface)
{
 if (*strm)
   {
#if 0 /* BUG Fix */
      /* Releasing the stream doesn't release an contained
      object integrated with free-threaded marshaler */
# ifdef __cplusplus
      (*strm)->Release();
# else
      (*strm)->lpVtbl->Release(*strm);
# endif
#else
      IUnknown *iu = 0;
      HRESULT hr = CoGetInterfaceAndReleaseStream(*strm, IFACE, (void**)&iu);
      if (SUCCEEDED(hr) && iu)
# ifdef __cplusplus
          iu->Release();
# else
          iu->lpVtbl->Release(iu);
# endif
      else UL_WARNING((LOGID, "%!l lo_release_stream(): UnMarshal FAILED", hr));
#endif
    UL_DEBUG((LOGID, "lo_release_stream(s:%p->i:%p)", *strm, iu));
    *strm = 0;
   }
}
#undef IFACE
#endif /*LO_USE_BOTHMODEL*/

/* end of advise_q.cpp */
