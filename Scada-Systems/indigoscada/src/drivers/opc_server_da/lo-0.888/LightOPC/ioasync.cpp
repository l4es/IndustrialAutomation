/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko, Kostya Volovich            *
                                                                          *
 Asynchronous operations Refresh & Cancel
 **************************************************************************/

#include "privopc.h"
#include <opcerror.h>
#include LO_OLE_ERRORS_h /*CONNECT_E_NOCONNECTION*/


static HRESULT internal_force_refresh(LightOPCGroup *grp,
                                      DWORD dwTransactionID,
                                      DWORD dwConnection,
                                      OPCDATASOURCE dwSource,
                                      DWORD *pCancelID)
{
 HRESULT hr = S_OK;
 loRequest *rq = 0;

 UL_TRACE((LOGID, "AsyncIO:Refresh(%x/%u)...", dwConnection, dwSource));

 if (!pCancelID ||
     dwSource != OPC_DS_DEVICE &&
     dwSource != OPC_DS_CACHE)  { hr = E_INVALIDARG; goto Return; }
 *pCancelID = 0;
 if (grp->owner->q_req.metric_overload) { hr = CONNECT_E_ADVISELIMIT; goto Return; }

 grp->lock_read();

 if (dwSource == OPC_DS_DEVICE &&
     grp->owner->access_mode & (loAM_NOREAD_DEV | loAM_ERREAD_DEV))
   dwSource = OPC_DS_CACHE;

 if (!grp->Active || !grp->active_count) hr = E_FAIL;
 else if ((dwConnection & grp->advise_present) == 0)
   hr = CONNECT_E_NOCONNECTION;
 else
   {
    unsigned ii = (dwSource == OPC_DS_DEVICE &&
             grp->owner->se->driver.ldReadTags)? grp->active_count: 0;
    rq = lo_req_alloc(ii, loUPL_tagpair);
    if (!rq) hr = E_OUTOFMEMORY;
    else
      {
       rq->group_key = grp->ServerHandle;
       rq->serv_key = grp->owner->serv_key;
       rq->operation = dwConnection | loRQ_OP_REFRESH |
                      (dwSource == OPC_DS_DEVICE ? loRQ_DEVICE: 0);
       rq->upl.transaction_id = dwTransactionID;
       rq->upl.rctx = grp->owner->ctxt;
       rq->upl.rctx.cta.vc_lcid = grp->grLCID;

       if (ii)
         {
          LightOPCItem *it;
          loTagPair *tpl = rq->upl.tagpair;
          loTagEntry *tags = grp->owner->se->tags;

          loTagPair_init(tpl, ii);

          for(ii = 0; ii < grp->itl.gl_count; ii++)
            if ((it = (LightOPCItem*)grp->itl.gl_list[ii]) && it->bActive)
              {
               loTagEntry *te = &tags[it->tid];

#if 0 != LO_CHECK_RIGHTS
               if (OPC_READABLE & te->attr.taRights)
#endif
                 {
                  tpl->tpTi = te->attr.taTi;
                  tpl->tpRt = te->attr.taRt;
                  tpl->tpAP = it->AcPath;
                  tpl++;
                 }
              }/* end of for */
          if (0 == (rq->upl.used = tpl - rq->upl.tagpair)) hr = E_FAIL;
          else if (rq->upl.used > grp->active_count)
            UL_ERROR((LOGID, "AsyncIO:Refresh(): active count mismatch"));
         }
      }
   }
 grp->unlock();

 if (S_OK == hr)
   {
    if (0 == (*pCancelID = lo_req_put_async(&grp->owner->q_req, rq)))
      hr = CONNECT_E_ADVISELIMIT;
    rq = 0;
   }

Return:
 if (rq) lo_req_free(rq);

 return hr;
}

/********** AsyncIO1:: */

STDMETHODIMP LightOPCGroup::Refresh(/* [in] */ DWORD dwConnection,
                                    /* [in] */ OPCDATASOURCE dwSource,
                                    /* [out] */ DWORD *pCancelID)
{
 HRESULT hr;

 LO_CHECK_STATEz0("AsyncIO:Refresh");

 if (dwConnection != loRQ_CONN_DATATIME &&
     dwConnection != loRQ_CONN_DATAONLY)
   {
    hr = CONNECT_E_NOCONNECTION;
    dwConnection = 0;
   }
 else hr = internal_force_refresh(this, 0, dwConnection, dwSource, pCancelID);

 if (SUCCEEDED(hr))
   {
    UL_NOTICE((LOG_GRH("AsyncIO:Refresh() -> %x Ok"), *pCancelID));
   }
 else
   {
    UL_INFO((LOG_GRH("AsyncIO:Refresh() = %s"), loStrError(hr)));
   }

 LO_FINISH();
 return hr;
}

/********** AsyncIO2:: */

STDMETHODIMP LightOPCGroup::Refresh2(OPCDATASOURCE dwSource, DWORD dwTransactionID, DWORD *pdwCancelID)
{
 HRESULT hr;

 LO_CHECK_STATEz0("AsyncIO2:Refresh2");

 hr = internal_force_refresh(this, dwTransactionID, loRQ_CONN_DATABACK, dwSource, pdwCancelID);

 if (SUCCEEDED(hr))
   {
    UL_NOTICE((LOG_GRH("AsyncIO2:Refresh2() -> %x Ok"), *pdwCancelID));
   }
 else
   {
    UL_INFO((LOG_GRH("AsyncIO2:Refresh2() = %s"), loStrError(hr)));
   }

 LO_FINISH();
 return hr;
}

/********** AsyncIO1:: */

STDMETHODIMP LightOPCGroup::Cancel(DWORD dwTransactionID)
{
 HRESULT hr = E_FAIL;
 int oper = 0;
 loRequest *rq;

 LO_CHECK_STATEz0("AsyncIO:Cancel");

 rq = lo_req_replace(&owner->q_req, 0, dwTransactionID,
                    owner->serv_key, ServerHandle,
                    loRQ_CONN_DATA_1 | loRQ_CONN_WRITECOMPL);
 if (rq)
   {
    oper = rq->operation;
    lo_req_free(rq);
    hr = S_OK;
   }

 UL_NOTICE((LOG_GRH("AsyncIO:Cancel(%x) = %x"), dwTransactionID, oper));

 LO_FINISH();
 return hr;
}

/********** AsyncIO2:: */

STDMETHODIMP LightOPCGroup::Cancel2(DWORD dwCancelID)
{
 HRESULT hr = E_FAIL;
 int oper = 0;
 loRequest *rq, *rrq;

 LO_CHECK_STATEz0("AsyncIO2:Cancel2");

 if (!(rrq = lo_req_alloc(0, 0))) hr = E_OUTOFMEMORY;
 else
   {
    rrq->serv_key = owner->serv_key;
    rrq->group_key = ServerHandle;
    rrq->operation = loRQ_OP_CANCEL | loRQ_CONN_DATABACK;
    rrq->upl.trqid = dwCancelID;
    rq = lo_req_replace(&owner->q_req, rrq, dwCancelID,
                        owner->serv_key, ServerHandle,
                        loRQ_CONN_DATABACK);
    if (rq)
      {
       oper = rq->operation;
       hr = S_OK;
       rrq = rq;
      }
    lo_req_free(rrq);
   }

 UL_NOTICE((LOG_GRH("AsyncIO2:Cancel2(%x) = %x"), dwCancelID, oper));

 LO_FINISH();
 return hr;
}


/* end of file ioasync.cpp */
