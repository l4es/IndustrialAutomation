/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2001  Timofei Bondarenko                               *
                                                                          *
 IO request handlers
 **************************************************************************/

#include <opcerror.h>
#include "privopc.h"

static void rio_cache_read(loService *se, loUpdList *upl)
{
 HRESULT mastererr  = upl->master_err;
 HRESULT masterqual = upl->master_qual;
 HRESULT  *errors = upl->errors;
 WORD    *quality = upl->quality;
 VARIANT *variant = upl->variant;
 VARTYPE *vartype = upl->vartype;
 FILETIME *timestamp = upl->timestamp;
 loTagPair *tpl = upl->tagpair;
 loTagEntry *tags = se->tags;
 unsigned ii;
 unsigned localize_max = 0;
 unsigned localize_min = 0;

 lw_rw_rdlock(&se->lkPrim);

 for(ii = upl->used; ii--; errors++, quality++, variant++,
                           vartype++, timestamp++, tpl++)
   {
    if (tpl->tpTi)
      {
       HRESULT hr;
       loTagEntry *te = &tags[tpl->tpTi];

#if 1 == LO_EV_TIMESTAMP
       if (!IsFILETIME(te->prim.tsTime))
         *timestamp = loFT_SUBST(te->prim.tsTime, se->ts_prim);
       else
#endif
       *timestamp = te->prim.tsTime;
       *errors = te->prim.tsError;
#ifdef VAR_CHANGETYPE
       *quality = te->prim.tsQuality;
       hr = VAR_CHANGETYPE(variant, &te->primValue, *vartype);
#else
       loCONV convtype = (loCONV)*quality;
       *quality = te->prim.tsQuality;

       switch(convtype/*lo_check_conversion(&te->attr, *vartype)*/)
         {
       case loCONV_CHANGE:
          hr = VARIANTCHANGEt(variant, &te->primValue, 
                              upl->rctx.cta, *vartype);
          break;
       case loCONV_COPY:
          hr = VariantCopy(variant, &te->primValue);
          break;
       case loCONV_ASSIGN:
          *variant = te->primValue; 
#if 1
          loTagPair_init(tpl, 1);
          goto Cont;
#else
          hr = S_OK; 
          break;
#endif
       case loCONV_CONVERT:
          if (S_OK == (hr = VariantCopy(variant, &te->primValue)))
            {
             if (!localize_max)
               localize_min = quality - upl->quality;
             localize_max = quality - upl->quality + 1;
             goto PreserveTP;
            }
          break;
         }
       loTagPair_init(tpl, 1);
PreserveTP:
#endif
       if (S_OK != hr)
         {
          LO_E_BADTYPE_QUAL(hr, *errors, *quality);
          masterqual = S_FALSE;
          mastererr = S_FALSE;
          continue;
         }
Cont:;
      }

    if (OPC_QUALITY_GOOD != (OPC_QUALITY_MASK & *quality)) masterqual = S_FALSE;
    if (S_OK != *errors) mastererr = S_FALSE;
   }

#if 2 == LO_EV_TIMESTAMP
  if (IsFILETIME(*se->ts_prim))
    lo_check_ev_timestamp(upl->used, upl->timestamp, se->ts_prim);
#endif

 lw_rw_unlock(&se->lkPrim);

 if (localize_max)
   se->driver.ldConvertTags(&upl->rctx.cactx, 
                            localize_max - localize_min,
                            upl->tagpair + localize_min,
                            upl->variant + localize_min,
                            upl->quality + localize_min,
                            upl->errors  + localize_min,
                            &mastererr, &masterqual,
                            upl->variant + localize_min,
                            upl->vartype + localize_min, 
                            upl->rctx.cta.vc_lcid);

 upl->master_qual = masterqual;
 /*if (upl->master_err == S_OK)*/
 upl->master_err = mastererr;
}

static void rio_cache_write(loService *se, loUpdList *upl)
{
 HRESULT  *errors = upl->errors;
 VARIANT *variant = upl->variant;
 loTagPair *tpl = upl->tagpair;
 loTagEntry *tags = se->tags;
 unsigned ii, items_fail = 0, items_clamp = 0;
 FILETIME ft;
 VARIANT var; VARIANTINIT(&var);
 loTagValue *ts;
 se->driver.ldCurrentTime(&se->cactx, &ft);

// UL_INFO((LOGID, "rio_cache_write() in %d", upl->master_err));

 if (!(ts = loCacheLock(se))) return;

 for(ii = upl->used; ii--; tpl++, errors++, variant++)
   if (tpl->tpTi)
     {
      loTagValue *tv = &ts[tpl->tpTi];
      VARTYPE cvt = taVTYPE(&tags[tpl->tpTi].attr);
      HRESULT hr;

      if (V_VT(variant) != VT_EMPTY &&
          V_VT(variant) != cvt &&
          S_OK != (hr =
#if 0
              VARIANTCHANGEt(variant, variant, upl->rctx.cta, cvt)
#else
              VariantChangeType(variant, variant, 0, cvt)
#endif
                  ))
        {
         *errors = LO_E_BADTYPE(hr); 
         items_fail = 1;
        }
      else if (tv->tvTi) 
        {
         *errors = OPC_S_CLAMP;
         items_fail = 1;
        }
      else
        {
         VARIANT tvmp;
         tv->tvTi = tpl->tpTi;
         tvmp = tv->tvValue;
         tv->tvValue = *variant;
         *variant = tvmp;
         tv->tvState.tsTime = ft;
         tv->tvState.tsError = *errors = loOPC_S_CLAMP;
         tv->tvState.tsQuality = OPC_QUALITY_LOCAL_OVERRIDE;
         items_clamp = 1;
        }
     }
 loCacheUnlock(se, 0);

 if (items_fail || items_clamp && loOPC_S_CLAMP != S_OK)
   upl->master_err = S_FALSE;
}

static void rio_refresh(LightOPCServer *cli, loRequest *rq)
{
 LightOPCGroup *grp;
 unsigned group_key = rq->group_key;
 int operation = rq->operation;
 loRqid trqid = rq->upl.trqid;
 DWORD transid = rq->upl.transaction_id;
 loUpdList upl;
 lo_upl_init(&upl, loUPL_variant | loUPL_timestamp | loUPL_quality | loUPL_errors |
                   loUPL_opchandle);
 upl.trqid = trqid;
 upl.transaction_id = transid;
 lo_req_free(rq);

 cli->lock_read();
 if ((grp = cli->by_handle_g(group_key)) &&
     (grp->advise_present & operation & loRQ_CONN_DATA))
   {
    lo_refresh_items(cli->se, &upl, grp);
    cli->send_callback(grp, 0, &upl, operation);
   }
 else
   {
    cli->unlock();
    UL_INFO((LOGID, "Refresh(%x) Aborted: No such group or connection", trqid));
   }
 lo_upl_clear(&upl);
}

int lo_rio_requests(LightOPCServer *cli)
{
 loRequest *rq;
 loService *se = cli->se;

 while(rq = cli->q_req.req)
   {
    int operation = rq->operation;
    loTrid trid;

    if (loThrControl_outof01(&cli->async)) return -1;
    cli->q_req.req = rq->rq_next;
    if (operation & loRQ_OPER_IO)
      cli->q_req.metric_overload = 0;
    lw_mutex_unlock(&cli->async.lk);

    UL_TRACE((LOGID, "REQUEST Retreived %x / %x", operation, rq->upl.trqid));

    switch(operation & (loRQ_OPER_MASK | loRQ_DEVICE))
      {
#if LO_USE_BOTHMODEL
    case loRQ_OP_ADVISE:
      {
       void *aconn = 0;
       if (rq->conn_stream)
         {
          rq->upl.master_err = CoGetInterfaceAndReleaseStream(rq->conn_stream,
                                                             *rq->conn_iface,
                                                              &aconn);
          rq->conn_stream = 0;
         }
       else rq->upl.master_err = S_OK;
       if (SUCCEEDED(rq->upl.master_err))
         rq->upl.master_err = cli->sync_advise(operation, &aconn, rq->group_key);
       if (aconn) ((IUnknown*)aconn)->Release();
       if (operation & loRQ_SYNC)
         lo_req_put_bcast(&cli->q_ret, rq);
       else lo_req_free(rq);
      }
       break;

    case loRQ_OP_REMOVE_GROUP:
       rq->upl.master_err = cli->internalRemoveGroup(rq->group_key,
                                  operation & loRQ_OF_REMOVE_FORCE);
       if (operation & loRQ_SYNC)
         lo_req_put_bcast(&cli->q_ret, rq);
       else lo_req_free(rq);
       break;
#endif
/********************************************/

    case loRQ_OP_WRITE|loRQ_DEVICE:
       if (se->driver.ldWriteTags)
         {
          int rv;
          loLOCK_DRIVER(se);
          rv = se->driver.ldWriteTags(&rq->upl.rctx.cactx, rq->upl.used,
                                      rq->upl.tagpair, rq->upl.variant,
                                      rq->upl.errors, &rq->upl.master_err,
                                      rq->upl.rctx.cta.vc_lcid);
          loUNLOCK_DRIVER(se);
          if (rv == loDW_ALLDONE)
            {
             if (operation & loRQ_SYNC)
               lo_req_put_bcast(&cli->q_ret, rq);
             else
               {
                cli->send_callback(0, rq->group_key, &rq->upl, operation);
                lo_req_free(rq);
               }
             break;
            }
         }
// UL_ERROR((LOGID, "DEV WRITE failed"));
    case loRQ_OP_WRITE:
       rio_cache_write(se, &rq->upl);
       if (operation & loRQ_SYNC)
         lo_req_put_bcast(&cli->q_ret, rq);
       else
         {
          cli->send_callback(0, rq->group_key, &rq->upl, operation);
          lo_req_free(rq);
         }
       break;
/********************************************/

    case loRQ_OP_READ|loRQ_DEVICE:
       if (se->driver.ldReadTags /*&&
           !(cli->access_mode & (loAM_NOREAD_DEV |
                                 loAM_ERREAD_DEV))*/)
         {
          HRESULT m_err = rq->upl.master_err,
                  m_qual = rq->upl.master_qual;
          loLOCK_DRIVER(se);
          trid = se->driver.ldReadTags(&rq->upl.rctx.cactx, rq->upl.used,
                                       rq->upl.tagpair, rq->upl.variant, rq->upl.quality,
                                       rq->upl.timestamp, rq->upl.errors, &m_err, &m_qual,
                                       rq->upl.vartype, rq->upl.rctx.cta.vc_lcid);
          loUNLOCK_DRIVER(se);
          if (loDR_STORED == trid)
            {
             if (S_OK != m_err) rq->upl.master_err = S_FALSE;
             if (S_OK != m_qual) rq->upl.master_qual = S_FALSE;
             if (operation & loRQ_SYNC)
               lo_req_put_bcast(&cli->q_ret, rq);
             else
               {
                cli->send_callback(0, rq->group_key, &rq->upl, operation);
                lo_req_free(rq);
               }
             break;
            }
          else if (loDR_CACHED != trid) loTridWait(se, trid);
         }
    case loRQ_OP_READ:
       rio_cache_read(se, &rq->upl);
       if (operation & loRQ_SYNC)
         lo_req_put_bcast(&cli->q_ret, rq);
       else
         {
          cli->send_callback(0, rq->group_key, &rq->upl, operation);
          lo_req_free(rq);
         }
       break;
/********************************************/

    case loRQ_OP_REFRESH|loRQ_DEVICE:
       if (se->driver.ldReadTags /*&&
          !(cli->access_mode & (loAM_NOREAD_DEV |
                                loAM_ERREAD_DEV))*/)
         {
          loLOCK_DRIVER(se);
          trid = se->driver.ldReadTags(&rq->upl.rctx.cactx, 
                                       rq->upl.used, rq->upl.tagpair,
                                       0, 0, 0, 0, 0, 0, 0, 0
                                     /*rq->upl.variant, rq->upl.quality,
                                       rq->upl.timestamp, rq->upl.errors,
                                       &master_err, &master_qual,
                                       rq->upl.vartype, rq->upl.gr_lcid*/);
          loUNLOCK_DRIVER(se);
          if (loDR_CACHED != trid) loTridWait(se, trid);
         }
    case loRQ_OP_REFRESH:
       if (operation & loRQ_SYNC)
         UL_ERROR((LOGID, "SYNC REFRESH is invalid %x / %x", rq->operation, rq->upl.trqid));
       rio_refresh(cli, rq);
       break;
/********************************************/

    case loRQ_OP_CANCEL:
       cli->send_callback(0, rq->group_key, &rq->upl, operation);
       lo_req_free(rq);
       break;
/********************************************/

    default:
       UL_ERROR((LOGID, "BAD REQUEST %x / %x", rq->operation, rq->upl.trqid));
       lo_req_free(rq);
       break;
      }
#if 0
    if (rq) /*??*/
      {
       if (operation & loRQ_SYNC)
         lo_req_put_bcast(&cli->q_ret, rq);
       else
         {
          if (operation & loRQ_OPER_IO)
            cli->send_callback(0, rq->group_key, &rq->upl, operation);
          lo_req_free(rq);
         }
      }
#endif
    lw_mutex_lock(&cli->async.lk);
   }

 return 0;
}


/**************************************************************************/
/* end of ioreal.cpp */
