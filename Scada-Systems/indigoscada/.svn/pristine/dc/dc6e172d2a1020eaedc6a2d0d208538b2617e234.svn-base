/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko                               *
                                                                          *
 LightOPCGroup::IOPC[A]SyncIO::Read() methods
 **************************************************************************/

#include <opcerror.h>
#include "privopc.h"
#include LO_OLE_ERRORS_h /*CONNECT_E_NOCONNECTION*/

/* OPC-DA v2.04:
   4.3 p.22:
   It is expected that most clients will use either Reads or Subscriptions for
   a particular group but not both. If both are used then there is some interaction
   between Reads and Subscriptions in that anything sent to the client as a result
   of a 'read' is also considered to be the 'last value sent'.

   4.5.1.8 p.72
   There are basically three ways to get data into a client
   (ignoring the 'old' IDataObject/IAdviseSink).
   IOPCSyncIO::Read (from cache or device)
   IOPCAsyncIO2::Read (from device)
   IOPCCallback::OnDataChange() (exception based)
                                which can also be triggered by IOPCAsyncIO2::Refresh.
   In general the three methods operate independently without 'side effects' on each other.
*****************/

/*********************************** LightOPCGroup::IOPCSyncIO stuff **************************/

#if 0
static unsigned extract_itemstate_from_req(unsigned count,
                                           HRESULT *err,
                                           OPCITEMSTATE *ist,
                                           VARIANT *vars,
                                           VARTYPE *vtype,
                                           FILETIME *timestamp,
                                           WORD     *quality)
{
 unsigned items_ok;

 for(items_ok = 0; count--; err++, ist++, vars++, vtype++)
   {
    HRESULT hrr;
    ist->wQuality = *quality++;
    ist->ftTimeStamp = *timestamp++;
    hrr = *err;
    if (SUCCEEDED(hrr))
      {
       VARTYPE vt;

       if ((vt = *vtype) != VT_EMPTY &&
            vt != V_VT(vars) &&
            S_OK != (hrr = VariantChangeType(vars, vars, 0, vt)))
         {
            LO_E_BADTYPE_QUAL(hrr, *err, ist->wQuality);
         }
       else items_ok++;
       ist->vDataValue = *vars;
       VARIANTINIT(vars);
      }
   }
 return items_ok;
}

#endif

static HRESULT extract_itemstate_from_prim(unsigned count,
                                           HRESULT *err,
                                           OPCITEMSTATE *ist,
                                           OPCHANDLE *phServer,
                                           LightOPCGroup *grp,
                                           const loCallerx *cctx)
{
 loService *se = cctx->cactx.ca_se;//grp->owner->se;
 loTagEntry *tags = se->tags;
 int andActive = grp->Active? ~0: 0;
 HRESULT master_err = S_OK;
 LightOPCItem *it;

 if (andActive/* | from_device*/) lw_rw_rdlock(&se->lkPrim);

 for(; count--; err++, ist++, phServer++)
   if (!(it = grp->by_index(*phServer)))
     { *err = OPC_E_INVALIDHANDLE; master_err = S_FALSE; continue; }
   else
     {
      loTagEntry *te = &tags[it->tid];
        //    irs->wReserved = 0;
      ist->hClient = it->hClient;

#if 0 != LO_CHECK_RIGHTS
      if (!(OPC_READABLE & te->attr.taRights))
        { *err = OPC_E_BADRIGHTS; master_err = S_FALSE; continue; }
      else
#endif
           if (!(andActive & it->bActive /*| from_device*/))
        {
         ist->wQuality = OPC_QUALITY_OUT_OF_SERVICE;
         *err = LO_E_NOTACTIVE;
         master_err = S_FALSE;
         continue;
        }
      else
        {
         HRESULT hrr;
         ist->ftTimeStamp = te->prim.tsTime;
         ist->wQuality = te->prim.tsQuality;
         *err = hrr = te->prim.tsError;
         if (S_OK != hrr) master_err = S_FALSE;
#ifdef VAR_CHANGETYPE
         hrr = VAR_CHANGETYPE(&ist->vDataValue, &te->primValue,
                              it->vtRequestedDataType);
#else
          switch(it->convtype)
            {
          case loCONV_CHANGE:
             hrr = VARIANTCHANGE(&ist->vDataValue,
                                 &te->primValue,
                                 grp->grLCID, cctx->cta.vc_tfl,
                                 it->vtRequestedDataType);
             break;
          case loCONV_COPY:
             hrr = VariantCopy(&ist->vDataValue,
                               &te->primValue);
             break;
          case loCONV_ASSIGN:
             ist->vDataValue = te->primValue;
             continue;
             hrr = S_OK;
             break;
          case loCONV_CONVERT:
               {
                HRESULT thr;
                loTagPair tp;
                tp.tpTi = it->tid;
                tp.tpAP = it->AcPath;
                tp.tpRt = te->attr.taRt;

                se->driver.ldConvertTags(&cctx->cactx, 1, &tp,
                      &ist->vDataValue, &ist->wQuality, err,
                      &master_err, &thr, &te->primValue,
                      &it->vtRequestedDataType, grp->grLCID);
                continue;
                hrr = S_OK;
               }
             break;
            }
#endif
         if (hrr != S_OK)
           {
            LO_E_BADTYPE_QUAL(hrr, *err, ist->wQuality);
            master_err = S_FALSE;
           }
        } /* active */
     } /* handle */
   /* end of for() */
 if (andActive /*| from_device*/) lw_rw_unlock(&se->lkPrim);
 return master_err;
}

/************************************************************************/

STDMETHODIMP LightOPCGroup::Read(OPCDATASOURCE dwSource,
                                 DWORD dwCount,
                                 OPCHANDLE *phServer,
                                 OPCITEMSTATE **ppItemValues,
                                 HRESULT **ppErrors)
{
 HRESULT hr = S_OK, master_err = S_OK;
 OPCITEMSTATE *ist = 0;
 HRESULT *err = 0;
 unsigned items_ok = 0;

 LO_CHECK_STATEz2("SyncIO:Read", ppItemValues, ppErrors);

 UL_TRACE((LOG_GRH("SyncIO:Read(%u/%u)..."), dwSource, dwCount));

 if (!phServer || !ppItemValues || !dwCount ||
     dwSource != OPC_DS_DEVICE && dwSource != OPC_DS_CACHE)
   { hr = E_INVALIDARG; goto Return; }

 err = (HRESULT*)loComAlloc(dwCount * sizeof(HRESULT));
 if (ist = (OPCITEMSTATE*)loComAlloc(dwCount * sizeof(OPCITEMSTATE)))
   {
    memset(ist, 0, dwCount * sizeof(OPCITEMSTATE));
#if 0 != LO_INIT_VARIANT
    for(ii = dwCount; ii--;) VARIANTINIT(&ist[ii].vDataValue);
#endif
   }

 if (!err || !ist) { hr = E_OUTOFMEMORY; goto Return; }

 if (dwSource == OPC_DS_DEVICE) /* real device reading */
   {
    unsigned ii;
    loService *se = owner->se;
    loTagEntry *tags = se->tags;
    loRequest *rq;
    loTagPair *tpl;
//    HRESULT *errors;

    rq = lo_req_alloc(dwCount, loUPL_variant | loUPL_timestamp | loUPL_quality |
                             /*loUPL_errors |*/ loUPL_tagpair | loUPL_vartype);

    if (!rq) { hr = E_OUTOFMEMORY; goto Return; }
    rq->com_allocated = rq->upl.errors = err;
    rq->operation = loRQ_OP_READ | loRQ_SYNC | loRQ_DEVICE;
    rq->group_key = ServerHandle;
    rq->serv_key = owner->serv_key;
//    memset(rq->upl.timestamp, 0, sizeof(FILETIME) * dwCount);
    tpl = rq->upl.tagpair;
    loTagPair_init(tpl, dwCount);
//    errors = rq->upl.errors;
    rq->upl.rctx = owner->ctxt;

    lock_read();
    rq->upl.rctx.cta.vc_lcid = grLCID;

    if (owner->access_mode & (loAM_NOREAD_DEV|loAM_ERREAD_DEV))
      {
       if (owner->access_mode & loAM_NOREAD_DEV) rq->operation &= ~loRQ_DEVICE;
       if (owner->access_mode & loAM_ERREAD_DEV)
         {
          unsigned ii;
          for(ii = 0; ii < dwCount; ii++) err[ii] = OPC_E_BADRIGHTS;
          goto Unlock;
         }
      }

      for(ii = 0; ii < dwCount; ii++, tpl++)
        {
         LightOPCItem *it;
         if (!(it = by_index(phServer[ii]))) err[ii] = OPC_E_INVALIDHANDLE;
         else
           {
            loTagEntry *te = &tags[it->tid];
            ist[ii].hClient = it->hClient;

#if 0 != LO_CHECK_RIGHTS
            if (!(OPC_READABLE & te->attr.taRights)) err[ii] = OPC_E_BADRIGHTS;
            else
#endif
              {
               err[ii] = S_OK;
               tpl->tpTi = te->attr.taTi;
               tpl->tpRt = te->attr.taRt;
               tpl->tpAP = it->AcPath;
               rq->upl.vartype[ii] = it->vtRequestedDataType;
               rq->upl.quality[ii] = (WORD)it->convtype;
               items_ok++;
              }
           }
        }
Unlock:
    unlock();
    err = 0;
    rq->upl.used = dwCount;
    if (items_ok != dwCount) rq->upl.master_err = S_FALSE;

    if (0 == items_ok)
      {
//       memcpy(err, rq->upl.errors, dwCount * sizeof(*err));
       err = rq->upl.errors;
       rq->com_allocated = 0;
       lo_req_free(rq);
      }
    else if (rq = lo_req_put_sync(&owner->q_req, &owner->q_ret, rq))
      {
#if 0
       items_ok = extract_itemstate_from_req(dwCount, ist,
                  rq->upl.variant, rq->upl.timestamp, rq->upl.quality);
#else
       VARIANT  *vars = rq->upl.variant;
       FILETIME *timestamp = rq->upl.timestamp;
       WORD     *quality = rq->upl.quality;
       unsigned ii = dwCount;
       while(ii--)
         {
          ist[ii].wQuality = quality[ii];
          ist[ii].ftTimeStamp = timestamp[ii];
          ist[ii].vDataValue = vars[ii];
          VARIANTINIT(&vars[ii]);
         }
       master_err = rq->upl.master_err;
#endif
//       memcpy(err, rq->upl.errors, dwCount * sizeof(*err));
       err = rq->upl.errors;
       rq->com_allocated = 0;
       lo_req_free(rq);
      }
    else hr = LO_E_SHUTDOWN;
   }
 else    /* real cache reading */
   {
    lock_read();
    master_err = extract_itemstate_from_prim(dwCount, err, ist, phServer, 
                                             this, &owner->ctxt);
    unlock();
   }

Return:
  if (S_OK == hr)
   {
    if (ppErrors) *ppErrors = err, err = 0;
    *ppItemValues = ist, ist = 0;
    if (S_OK != master_err) hr = S_FALSE;
    UL_NOTICE((LOG_GRH("SyncIO:Read(%u) = %x Ok"), dwCount, master_err));
   }
 else
   {
    UL_INFO((LOG_GRH("SyncIO:Read(%u) = %x /%s"),
               dwCount, master_err, loStrError(hr)));
   }
    /* it is not neccessary to perform VariantClear() on ist
     because we don't make serious errors after ist.tvDataValue changed */
 if (ist) loComFree(ist);
 if (err) loComFree(err);

 LO_FINISH();
 return hr;
}


/*********************************** LightOPCGroup::IOPCAsyncIO stuff **************************/

#define loUPL_AsyncRead   (loUPL_variant | loUPL_timestamp | loUPL_quality | loUPL_errors |\
                           loUPL_tagpair | loUPL_opchandle | loUPL_vartype )

/********** AsyncIO1:: */

STDMETHODIMP LightOPCGroup::Read(/* [in] */ DWORD dwConnection,
			         /* [in] */ OPCDATASOURCE dwSource,
				 /* [in] */ DWORD dwCount,
				 /* [size_is][in] */ OPCHANDLE *phServer,
				 /* [out] */ DWORD   *pTransactionID,
				 /* [size_is][size_is][out] */ HRESULT **ppErrors)
{
 HRESULT hr = S_FALSE;
 HRESULT *errs = 0;
 loRequest *rq = 0;
 unsigned item_ok = 0;
 int am_mask = OPC_READABLE;

 LO_CHECK_STATEz1("AsyncIO:Read", ppErrors);

 UL_TRACE((LOG_GRH("AsyncIO:Read(%x/%u/%u)..."), dwConnection, dwSource, dwCount));

 if (!phServer || !dwCount || !pTransactionID ||
     dwSource != OPC_DS_DEVICE &&
     dwSource != OPC_DS_CACHE)  { hr = E_INVALIDARG; goto Return; }
 *pTransactionID = 0;
 if (owner->q_req.metric_overload) { hr = CONNECT_E_ADVISELIMIT; goto Return; }

 errs = (HRESULT*)loComAlloc(dwCount * sizeof(HRESULT));
 rq = lo_req_alloc(dwCount, loUPL_AsyncRead);
 if (!rq || !errs) { hr = E_OUTOFMEMORY; goto Return; }
/* if some enties are not OK we will not use the rest fields at all
   thuse wo don't have to initialize them */
// memset(rq->upl.timestamp, 0, sizeof(FILETIME) * dwCount);
 loTagPair_init(rq->upl.tagpair, dwCount);
// memset(rq->upl.errors, 0, sizeof(HRESULT) * dwCount);

 rq->operation = dwConnection | loRQ_OP_READ |
                (dwSource == OPC_DS_DEVICE ? loRQ_DEVICE: 0);
 rq->group_key = ServerHandle;
 rq->serv_key = owner->serv_key;
 rq->upl.rctx = owner->ctxt;

 lock_read();

 rq->upl.rctx.cta.vc_lcid = grLCID;

 if (dwSource == OPC_DS_DEVICE &&
     owner->access_mode & (loAM_NOREAD_DEV|loAM_ERREAD_DEV))
   {
    if (owner->access_mode & loAM_NOREAD_DEV) rq->operation &= ~loRQ_DEVICE;
    if (owner->access_mode & loAM_ERREAD_DEV)
      am_mask = 0, rq->operation &= ~loRQ_DEVICE;
   }

 if (dwConnection != loRQ_CONN_DATATIME &&
     dwConnection != loRQ_CONN_DATAONLY ||
    (dwConnection & advise_present) == 0) 
   {
    hr = CONNECT_E_NOCONNECTION;
   }
 else
   {
    unsigned ii;
    loTagPair *tpl = rq->upl.tagpair;
    LightOPCItem *it;
    loTagEntry *tags = owner->se->tags;
    int a_active = Active? ~0: 0;

    for(ii = 0; ii < dwCount; ii++, tpl++)
      if (!(it = by_index(phServer[ii]))) errs[ii] = OPC_E_INVALIDHANDLE;
      else
        {
         loTagEntry *te = &tags[it->tid];

#if 0 != LO_CHECK_RIGHTS
         if (!(am_mask/*OPC_READABLE*/ & te->attr.taRights))
           { //errs[ii] = OPC_E_BADRIGHTS; - in callback
            rq->upl.quality[ii] = OPC_QUALITY_BAD;
            rq->upl.errors[ii] = OPC_E_BADRIGHTS;
            rq->upl.master_err = S_FALSE;
           }
         else
#endif
              if (dwSource == OPC_DS_CACHE && !(a_active & it->bActive))
           {
            rq->upl.quality[ii] = OPC_QUALITY_OUT_OF_SERVICE;
            rq->upl.errors[ii] = LO_E_NOTACTIVE;
            rq->upl.master_err = S_FALSE;
           }
         else
           {
            rq->upl.quality[ii] = (WORD)it->convtype;
            tpl->tpTi = te->attr.taTi;
            tpl->tpRt = te->attr.taRt;
            tpl->tpAP = it->AcPath;
           }
         rq->upl.opchandle[ii] = it->hClient;
         rq->upl.vartype[ii] = it->vtRequestedDataType;
         errs[ii] = S_OK;
         item_ok++;
        }/* end of for */
   }
 unlock();

 if (dwCount == item_ok)
   {
    rq->upl.used = item_ok;
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
    UL_NOTICE((LOG_GRH("AsyncIO:Read(%u) = %u -> %x Ok"),
                       dwCount, item_ok, *pTransactionID));
   }
 else
   {
    UL_INFO((LOG_GRH("AsyncIO:Read[conn=%x advs=%x](%u) = %u /%s"),
               dwConnection, advise_present,
               dwCount, item_ok, loStrError(hr)));
   }
 if (errs) loComFree(errs);

 LO_FINISH();
 return hr;
}

/********** AsyncIO2:: */

STDMETHODIMP LightOPCGroup::Read(DWORD dwCount, OPCHANDLE *phServer, DWORD dwTransactionID,
		                 DWORD *pdwCancelID, HRESULT **ppErrors)
{
 HRESULT hr = S_FALSE;
 HRESULT *errs = 0;
 loRequest *rq = 0;
 unsigned items_ok = 0;

 LO_CHECK_STATEz1("AsyncIO2:Read", ppErrors);

 UL_TRACE((LOG_GRH("AsyncIO2:Read(%x/%u)..."), dwTransactionID, dwCount));

 if (!phServer || !dwCount || !pdwCancelID)  { hr = E_INVALIDARG; goto Return; }
 *pdwCancelID = 0;
 if (owner->q_req.metric_overload) { hr = CONNECT_E_ADVISELIMIT; goto Return; }

 errs = (HRESULT*)loComAlloc(dwCount * sizeof(HRESULT));
 rq = lo_req_alloc(dwCount, loUPL_AsyncRead);
 if (!rq || !errs) { hr = E_OUTOFMEMORY; goto Return; }
//??  loTagPair_init(rq->upl.tagpair, dwCount);

 rq->operation = loRQ_OP_READ | loRQ_CONN_DATABACK | loRQ_DEVICE;
 rq->group_key = ServerHandle;
 rq->serv_key = owner->serv_key;
 rq->upl.transaction_id = dwTransactionID;
 rq->upl.rctx = owner->ctxt;

 lock_read();
 rq->upl.rctx.cta.vc_lcid = grLCID;

 if (owner->access_mode & (loAM_NOREAD_DEV|loAM_ERREAD_DEV))
   {
    if (owner->access_mode & loAM_NOREAD_DEV) rq->operation &= ~loRQ_DEVICE;
    if (owner->access_mode & loAM_ERREAD_DEV)
      {
       unsigned ii;
       for(ii = 0; ii < dwCount; ii++) errs[ii] = OPC_E_BADRIGHTS;
       goto Unlock;
      }
   }

 if (0 == (advise_present & loRQ_CONN_DATABACK)/*conn_databack*/)
   hr = CONNECT_E_NOCONNECTION;
 else
   {
    unsigned ii;
    loTagPair *tpl = rq->upl.tagpair;
    LightOPCItem *it;
    loTagEntry *tags = owner->se->tags;

    for(ii = 0; ii < dwCount; ii++)
      if (!(it = by_index(phServer[ii]))) errs[ii] = OPC_E_INVALIDHANDLE;
      else
        {
         loTagEntry *te = &tags[it->tid];

#if 0 != LO_CHECK_RIGHTS
         if (!(OPC_READABLE & te->attr.taRights)) errs[ii] = OPC_E_BADRIGHTS;
         else
#endif
           {
            tpl->tpTi = te->attr.taTi;
            tpl->tpRt = te->attr.taRt;
            tpl->tpAP = it->AcPath;
            tpl++;
            rq->upl.opchandle[items_ok] = it->hClient;
            rq->upl.quality[items_ok] = (WORD)it->convtype;
            rq->upl.vartype[items_ok++] = it->vtRequestedDataType;
            errs[ii] = S_OK;
           }
        }/* end of for */
   }
Unlock:
 unlock();

 if (items_ok)
   {
    rq->upl.master_err = rq->upl.master_qual =
    hr = items_ok == dwCount? S_OK: S_FALSE;
    rq->upl.used = items_ok;
    if (0 == (*pdwCancelID = lo_req_put_async(&owner->q_req, rq)))
      hr = CONNECT_E_ADVISELIMIT;
    rq = 0;
   }

Return:
 if (rq) lo_req_free(rq);

 if (SUCCEEDED(hr))
   {
    if (ppErrors) *ppErrors = errs, errs = 0;
    UL_NOTICE((LOG_GRH("AsyncIO2:Read(%u) = %u -> %x Ok"),
                       dwCount, items_ok, *pdwCancelID));
   }
 else
   {
    UL_INFO((LOG_GRH("AsyncIO2:Read(%u) = %u /%s"),
               dwCount, items_ok, loStrError(hr)));
   }
 if (errs) loComFree(errs);

 LO_FINISH();
 return hr;
}


/* end of ioread.cpp */
