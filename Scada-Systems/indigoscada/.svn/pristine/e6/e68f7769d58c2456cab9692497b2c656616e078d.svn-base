/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko                               *
                                                                          *
 LightOPCGroup::IOPC[A]SyncIO::Write() methods
 **************************************************************************/

#include <opcerror.h>
#include "privopc.h"
#include LO_OLE_ERRORS_h /*CONNECT_E_NOCONNECTION*/

/* IOPCSyncIO:: */

STDMETHODIMP LightOPCGroup::Write(DWORD dwCount,
                                  OPCHANDLE  *phServer,
                                  VARIANT *pItemValues,
                                  HRESULT **ppErrors)
{
 HRESULT hr = S_OK;
 HRESULT *err = 0;
 unsigned items_ok = 0;
 loRequest *rq;

 LO_CHECK_STATEz1("SyncIO:Write", ppErrors);
 UL_TRACE((LOG_GRH("SyncIO:Write(%u)..."), dwCount));

 if (!phServer || !pItemValues || !dwCount)
   { hr = E_INVALIDARG; goto Return; }

 err = (HRESULT*)loComAlloc(dwCount * sizeof(HRESULT));
 rq = lo_req_alloc(dwCount, loUPL_variant /*| loUPL_errors*/ | loUPL_tagpair);
 if (!rq || !err) { hr = E_OUTOFMEMORY; goto Return; }
 rq->operation = loRQ_OP_WRITE | loRQ_SYNC | loRQ_DEVICE;
 rq->group_key = ServerHandle;
 rq->serv_key = owner->serv_key;
 rq->com_allocated = rq->upl.errors = err;
 loTagPair_init(rq->upl.tagpair, dwCount);
 rq->upl.rctx = owner->ctxt;

 lock_read();
 rq->upl.rctx.cta.vc_lcid = grLCID;

 if (owner->access_mode & loAM_RDONLY_OP)
   {
    unsigned ii;
    for(ii = 0; ii < dwCount; ii++) err[ii] = OPC_E_BADRIGHTS;
   }
 else
   {
    loService *se = owner->se;
    loTagEntry *tags = se->tags;
    loTagPair *tpl = rq->upl.tagpair;
//    HRESULT *errors = rq->upl.errors;
    VARIANT *variant = rq->upl.variant;
    LightOPCItem *it;
    unsigned ii;

    for(ii = 0; ii < dwCount; ii++, tpl++, pItemValues++)
      if (!(it = by_index(phServer[ii]))) err[ii] = OPC_E_INVALIDHANDLE;
      else
        {
         loTagEntry *te = &tags[it->tid];

#if 0 != LO_CHECK_RIGHTS
         if (!(OPC_WRITEABLE & te->attr.taRights)) err[ii] = OPC_E_BADRIGHTS;
         else
#endif
              if (VT_EMPTY == V_VT(pItemValues) ||/*) *errors = OPC_E_BADTYPE;
         else if (*/ S_OK == (err[ii] =
                       VariantCopy(&variant[ii], pItemValues)))
           {
            err[ii] = S_OK;
            tpl->tpTi = te->attr.taTi;
            tpl->tpRt = te->attr.taRt;
            tpl->tpAP = it->AcPath;
            items_ok++;
           }
         else UL_TRACE((LOG_GRH("SyncIO:Write(%u) [ %x / %x ] %s"),
               ii, V_VT(pItemValues), taVTYPE(&te->attr), loStrError(err[ii])));
       }
    rq->upl.used = dwCount;
   }
 unlock();
// rq->upl.used = dwCount;

 if (0 == items_ok)
   {
//    memcpy(err, rq->upl.errors, dwCount * sizeof(*err));
    rq->com_allocated = 0;
    lo_req_free(rq);
    hr = S_FALSE;
   }
 else
   {
    rq->upl.master_err = items_ok == dwCount? S_OK: S_FALSE;
    err = 0;
    if (rq = lo_req_put_sync(&owner->q_req, &owner->q_ret, rq))
      {
       hr = rq->upl.master_err;
//       memcpy(err, rq->upl.errors, dwCount * sizeof(*err));
       err = rq->upl.errors;
       rq->com_allocated = 0;
       lo_req_free(rq);
      }
    else hr = LO_E_SHUTDOWN;
   }

Return:
  if (SUCCEEDED(hr))
   {
    if (ppErrors) *ppErrors = err, err = 0;
//    if (items_ok != dwCount && hr == S_OK) hr = S_FALSE;
    UL_NOTICE((LOG_GRH("SyncIO:Write(%u) = %u Ok"), dwCount, items_ok));
   }
 else
   {
    UL_INFO((LOG_GRH("SyncIO:Write(%u) = %u /%s"),
               dwCount, items_ok, loStrError(hr)));
   }
 if (err) loComFree(err);
 LO_FINISH();
 return hr;
}


#define loUPL_AsyncWrite  (loUPL_variant | loUPL_tagpair | loUPL_opchandle | loUPL_errors)

/********** AsyncIO1:: */

STDMETHODIMP LightOPCGroup::Write(/* [in] */ DWORD dwConnection,
                                  /* [in] */ DWORD dwCount,
                                  /* [size_is][in] */ OPCHANDLE *phServer,
                                  /* [size_is][in] */ VARIANT *pItemValues,
                                  /* [out] */ DWORD           *pTransactionID,
                                  /* [size_is][size_is][out] */ HRESULT **ppErrors)
{
 HRESULT hr = S_FALSE;
 HRESULT *errs = 0;
 loRequest *rq = 0;
 unsigned item_ok = 0, items_ok = 0;

 LO_CHECK_STATEz1("AsyncIO:Write", ppErrors);

 UL_TRACE((LOG_GRH("AsyncIO:Write(%u)..."), dwCount));

 if (!phServer || !dwCount || !pTransactionID || !pItemValues)
   { hr = E_INVALIDARG; goto Return; }
 *pTransactionID = 0;
 if (owner->q_req.metric_overload) { hr = CONNECT_E_ADVISELIMIT; goto Return; }

 errs = (HRESULT*)loComAlloc(dwCount * sizeof(HRESULT));
 rq = lo_req_alloc(dwCount, loUPL_AsyncWrite);
 if (!rq || !errs) { hr = E_OUTOFMEMORY; goto Return; }
 loTagPair_init(rq->upl.tagpair, dwCount);

 rq->operation = loRQ_OP_WRITE | loRQ_CONN_WRITECOMPL | loRQ_DEVICE;
 rq->group_key = ServerHandle;
 rq->serv_key = owner->serv_key;
 rq->upl.rctx = owner->ctxt;

 lock_read();
 rq->upl.rctx.cta.vc_lcid = grLCID;

 if (owner->access_mode & loAM_RDONLY_OP)
   {
    unsigned ii;
    for(ii = 0; ii < dwCount; ii++) errs[ii] = OPC_E_BADRIGHTS;
    goto Unlock;
   }

 if (dwConnection != loRQ_CONN_WRITECOMPL ||
     0 == (advise_present & loRQ_CONN_WRITECOMPL)/*conn_writecompl*/)
   {
    hr = CONNECT_E_NOCONNECTION;
   }
 else
   {
    unsigned ii;
    loTagPair *tpl = rq->upl.tagpair;
    HRESULT *errors = rq->upl.errors;
    LightOPCItem *it;
    loTagEntry *tags = owner->se->tags;

    for(ii = 0; ii < dwCount; ii++, tpl++, errors++, pItemValues++)
      if (!(it = by_index(phServer[ii]))) errs[ii] = OPC_E_INVALIDHANDLE;
      else
        {
         loTagEntry *te = &tags[it->tid];

#if 0 != LO_CHECK_RIGHTS
         if (!(OPC_WRITEABLE & te->attr.taRights))
           { //errs[ii] = OPC_E_BADRIGHTS; - in callback
            *errors = OPC_E_BADRIGHTS;
           }
         else
#endif
              if (VT_EMPTY == V_VT(pItemValues) || /*) *errors = OPC_E_BADTYPE;
         else if (*/S_OK == (*errors =
                       VariantCopy(&rq->upl.variant[ii], pItemValues)))
           {
            tpl->tpTi = te->attr.taTi;
            tpl->tpRt = te->attr.taRt;
            tpl->tpAP = it->AcPath;
            items_ok++;
           }
         rq->upl.opchandle[ii] = it->hClient;
         errs[ii] = S_OK;
         item_ok++;
        }/* end of for */
   }
Unlock:
 unlock();

 if (dwCount == item_ok)
   {
    rq->upl.used = item_ok;
    rq->upl.master_err = items_ok == dwCount? S_OK: S_FALSE;
    if (0 == (*pTransactionID = lo_req_put_async(&owner->q_req, rq)))
      hr = CONNECT_E_ADVISELIMIT;
    else hr = S_OK;
    rq = 0;
   }

Return:
 if (rq) lo_req_free(rq);

 if (SUCCEEDED(hr))
   {
    if (ppErrors) *ppErrors = errs, errs = 0;
    UL_NOTICE((LOG_GRH("AsyncIO:Write(%u) = %u -> %x Ok"),
                       dwCount, item_ok, *pTransactionID));
   }
 else
   {
    UL_INFO((LOG_GRH("AsyncIO:Write[conn=%x advs=%x](%u) = %u /%s"),
               dwConnection, advise_present,
               dwCount, item_ok, loStrError(hr)));
   }
 if (errs) loComFree(errs);

 LO_FINISH();
 return hr;
}

/********** AsyncIO2:: */

STDMETHODIMP LightOPCGroup::Write(DWORD dwCount, OPCHANDLE *phServer, VARIANT *pItemValues,
		                  DWORD dwTransactionID, DWORD *pdwCancelID, HRESULT **ppErrors)
{
 HRESULT hr = S_FALSE;
 HRESULT *errs = 0;
 loRequest *rq = 0;
 unsigned items_ok = 0;

 LO_CHECK_STATEz1("AsyncIO2:Write", ppErrors);

 UL_TRACE((LOG_GRH("AsyncIO2:Write(%u)..."), dwCount));

 if (!phServer || !dwCount || !pdwCancelID || !pItemValues)
   { hr = E_INVALIDARG; goto Return; }
 *pdwCancelID = 0;
 if (owner->q_req.metric_overload) { hr = CONNECT_E_ADVISELIMIT; goto Return; }

 errs = (HRESULT*)loComAlloc(dwCount * sizeof(HRESULT));
 rq = lo_req_alloc(dwCount, loUPL_AsyncWrite);
 if (!rq || !errs) { hr = E_OUTOFMEMORY; goto Return; }
 loTagPair_init(rq->upl.tagpair, dwCount);
 rq->operation = loRQ_OP_WRITE | loRQ_CONN_DATABACK | loRQ_DEVICE;
 rq->group_key = ServerHandle;
 rq->serv_key = owner->serv_key;
 rq->upl.transaction_id = dwTransactionID;
 rq->upl.rctx = owner->ctxt;

 lock_read();
 rq->upl.rctx.cta.vc_lcid = grLCID;

 if (owner->access_mode & loAM_RDONLY_OP)
   {
    unsigned ii;
    for(ii = 0; ii < dwCount; ii++) errs[ii] = OPC_E_BADRIGHTS;
    goto Unlock;
   }

 if (0 == (advise_present & loRQ_CONN_DATABACK)/*conn_databack*/)
   hr = CONNECT_E_NOCONNECTION;
 else
   {
    unsigned ii;
    loTagPair *tpl = rq->upl.tagpair;
//    HRESULT *errors = rq->upl.errors;
    LightOPCItem *it;
    loTagEntry *tags = owner->se->tags;

    for(ii = 0; ii < dwCount; ii++, pItemValues++)
      if (!(it = by_index(phServer[ii]))) errs[ii] = OPC_E_INVALIDHANDLE;
      else
        {
         loTagEntry *te = &tags[it->tid];

#if 0 != LO_CHECK_RIGHTS
         if (!(OPC_WRITEABLE & te->attr.taRights)) errs[ii] = OPC_E_BADRIGHTS;
         else
#endif
              if (VT_EMPTY == V_VT(pItemValues) || /*) *errors = OPC_E_BADTYPE;
         else if (*/S_OK == (errs[ii] =
                       VariantCopy(&rq->upl.variant[ii], pItemValues)))
           {
            tpl->tpTi = te->attr.taTi;
            tpl->tpRt = te->attr.taRt;
            tpl->tpAP = it->AcPath;
            tpl++;
            rq->upl.opchandle[items_ok++] = it->hClient;
            errs[ii] = S_OK;
           }
        }/* end of for */
   }
Unlock:
 unlock();

 if (items_ok)
   {
    rq->upl.used = items_ok;
    rq->upl.master_err = hr = items_ok == dwCount? S_OK: S_FALSE;
    if (0 == (*pdwCancelID = lo_req_put_async(&owner->q_req, rq)))
      hr = CONNECT_E_ADVISELIMIT;
    rq = 0;
   }

Return:
 if (rq) lo_req_free(rq);

 if (SUCCEEDED(hr))
   {
    if (ppErrors) *ppErrors = errs, errs = 0;
    UL_NOTICE((LOG_GRH("AsyncIO2:Write(%u) = %u -> %x Ok"),
                       dwCount, items_ok, *pdwCancelID));
   }
 else
   {
    UL_INFO((LOG_GRH("AsyncIO2:Write(%u) = %u /%s"),
               dwCount, items_ok, loStrError(hr)));
   }
 if (errs) loComFree(errs);

 LO_FINISH();
 return hr;
}


/* end of iowrite.cpp */
