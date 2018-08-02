/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko, Kostya Volovich              *
                                                                          *
 Main part of Item-related service
 **************************************************************************/

#include <opcerror.h>
#include "privopc.h"

/********************** LightOPCGroup::IOPCItemMgt ************************/

STDMETHODIMP LightOPCGroup::AddItems(DWORD dwCount,
                                     OPCITEMDEF *pItemArray,
                                     OPCITEMRESULT **ppAddResults,
                                     HRESULT **ppErrors)
{
 HRESULT hr;
 HRESULT *err = 0;
 OPCITEMRESULT *ir = 0;
 loTagId *tid = 0;
 void **acpath;
 LightOPCItem **oi = 0;
 DWORD ii;
 loTagEntry *tags;
 unsigned act_count;
 loAA_DECLARE(sizeof(LightOPCItem*) + sizeof(loTagId) + sizeof(void*));

 LO_CHECK_STATEz2("AddItems", ppAddResults, ppErrors);
 if (!ppAddResults) { hr = E_INVALIDARG; goto Return; }

 ii = dwCount * (sizeof(*oi) + sizeof(*tid) + sizeof(*acpath));
 if (0 == ii) { hr = E_INVALIDARG; goto Return; }
 if (!(oi = (LightOPCItem**)loAA_ALLOC(ii))) { hr = E_OUTOFMEMORY; goto Return; }
 memset(oi, 0, ii);
 tid = (loTagId*)&oi[dwCount];
 acpath = (void**)&tid[dwCount];

 hr = internalValidateItems(dwCount, pItemArray, tid, acpath, &ir, &err);
 if (FAILED(hr)) goto Return;

 tags = owner->se->tags;

 for(ii = 0; ii < dwCount; ii++)
   if (S_OK == err[ii])
     {
      LightOPCItem *newby;
      hr = E_OUTOFMEMORY;

      if (oi[ii] = newby = new LightOPCItem)
        {
         OPCITEMDEF *idef = &pItemArray[ii];
         loTagAttrib *ta = &tags[tid[ii]].attr;
         newby->tid = tid[ii];
         newby->AcPath = acpath[ii];
         newby->hClient = idef->hClient;

         newby->vtRequestedDataType = idef->vtRequestedDataType;
         newby->convtype = lo_check_conversion(ta, newby->vtRequestedDataType);

         hr = S_OK;
         if ((NULL == newby->AcPath ||
             S_OK == (hr = newby->set_accpath(idef->szAccessPath))) &&
             (!ta->taName || !(ta->taFlags & loTF_CANONAME) &&
#if 0
              wcscmp
#else
         owner->se->wstrcmp
#endif
                    (ta->taName, idef->szItemID)))
           hr = newby->set_itemid(idef->szItemID);
         newby->Quality = -1;
         if (0 == (newby->bActive = 0 != idef->bActive))
           tid[ii] = 0, newby->Quality = OPC_QUALITY_OUT_OF_SERVICE;
//         else if (!(OPC_READABLE & tags[tid[ii]].attr.flag))
  //         newby->bActive = 0;
        }
      err[ii] = hr;
      if (hr != S_OK)
        UL_NOTICE((LOG_GR("AddItem(%ls)(%ls)%x/%x = %s"),
	                loWnul(pItemArray[ii].szItemID),
	   		        loWnul(pItemArray[ii].szAccessPath),
                           pItemArray[ii].vtRequestedDataType,
                           ir[ii].vtCanonicalDataType,
			        loStrError(err[ii])));
     }

 act_count = 0;
 lock_write();
    for(ii = 0; ii < dwCount; ii++)
      if (S_OK == err[ii])
        {
         unsigned idx;
         if (idx = add(oi[ii]))
           {
            act_count += oi[ii]->bActive /* != 0*/;
            oi[ii] = 0;
            ir[ii].hServer = (OPCHANDLE)idx;
           }
	     else
           {
            err[ii] = E_OUTOFMEMORY;
            tid[ii] = 0;
            hr = S_FALSE;
            UL_NOTICE((LOG_GR("AddItem:add(%ls)(%ls)%x/%x = %s"),
	                loWnul(pItemArray[ii].szItemID),
	   		        loWnul(pItemArray[ii].szAccessPath),
                           pItemArray[ii].vtRequestedDataType,
                           ir[ii].vtCanonicalDataType,
			        loStrError(err[ii])));
           }
        }
      else
        {
         tid[ii] = 0;
         hr = S_FALSE;
        } /* end of for(;;) */
    if (act_count)
      {
       active_count += act_count;
       if (Active)
         {
          loChangeActivity(&owner->ctxt.cactx, 1, (unsigned)dwCount, tid);
          last_trid = 0; /* force callback */
         }
      }
 unlock();

Return:

 if (oi)
   {
    for(ii = 0; ii < dwCount; ii++)
      if (oi[ii]) delete oi[ii];
    loAA_FREE(oi);
   }

 if (FAILED(hr))
   {
    UL_INFO((LOG_GRH("AddItems(%lu) = %s"), dwCount, loStrError(hr)));
   }
 else
   {
    *ppAddResults = ir, ir = 0;
    if (ppErrors) *ppErrors = err, err = 0;
    UL_NOTICE((LOG_GRH("AddItems(%lu) = %s"), dwCount, loStrError(hr)));
   }
 if (ir)  loComFree(ir);
 if (err) loComFree(err);

 LO_FINISH();
 return hr;
}

/**************************************************************************/

STDMETHODIMP LightOPCGroup::ValidateItems(DWORD dwCount,
                                          OPCITEMDEF *pItemArray,
                                          BOOL bBlobUpdate,
		                          OPCITEMRESULT **ppValidationResults,
                                          HRESULT **ppErrors)
{
 HRESULT hr;

 LO_CHECK_STATEz2("ValidateItems", ppValidationResults, ppErrors);
 hr = internalValidateItems(dwCount, pItemArray, 0, 0, ppValidationResults, ppErrors);

 if (FAILED(hr))
   {
    UL_INFO((LOG_GRH("ValidateItems(%lu) FAILED/ %s"), dwCount, loStrError(hr)));
   }
 else
   {
    UL_NOTICE((LOG_GRH("ValidateItems(%lu) = %s"), dwCount, loStrError(hr)));
   }
 LO_FINISH();
 return hr;
}

/**************************************************************************/

HRESULT LightOPCGroup::internalValidateItems(DWORD dwCount,
                                             OPCITEMDEF *pItemDefs,
                                             loTagId *tid, void **acpath,
		                                     OPCITEMRESULT **ppResults,
                                             HRESULT **ppErrors)
{
 HRESULT hr = S_OK;
 HRESULT *err = 0;
 OPCITEMRESULT *ir = 0;
 DWORD ii;
 loService *se;
 loTagEntry *tags;
 int free_tid = 0,
     inc_ap = 0,
     ai_goal = loDAIG_ADDITEM,
     am_mask;
 loCallerx cctx;
 loAA_DECLARE(sizeof(loTagId));

 if (!tid)
   {
    free_tid = 1;
    ai_goal = loDAIG_VALIDATE;
    tid = (loTagId*)loAA_ALLOC(dwCount * sizeof(loTagId));
   }
 if (acpath) inc_ap = 1;

 UL_TRACE((LOG_GRH("loGroup::intValidateItems(%lu, %p %p %p)"),
           dwCount, pItemDefs, ppResults, ppErrors));

 if (ppResults) *ppResults = 0;
 if (ppErrors) *ppErrors = 0;

 if (!dwCount || !pItemDefs || !ppResults)
   { hr = E_INVALIDARG; goto Return; }

 cctx = owner->ctxt;
 se = owner->se; tags = se->tags;

 err = (HRESULT*)loComAlloc(dwCount * sizeof(HRESULT));
 ir = (OPCITEMRESULT*)loComAlloc(dwCount * sizeof(OPCITEMRESULT));
 if (!err || !ir || !tid) { hr = E_OUTOFMEMORY; goto Return; }
 memset(ir, 0, dwCount * sizeof(OPCITEMRESULT));

 am_mask = owner->access_mode & loAM_RDONLY_ADD?
                (loOPC_RIGHTS & ~OPC_WRITEABLE): loOPC_RIGHTS;

 lw_rw_rdlock(&se->lkMgmt);
 for(ii = 0; ii < dwCount; ii++, acpath += inc_ap)
   if (tid[ii] = loFindItemID(se, &owner->ctxt.cactx, 1,
 		              pItemDefs[ii].szItemID,
		              pItemDefs[ii].szAccessPath,
                      pItemDefs[ii].vtRequestedDataType,
                      ai_goal,
                      acpath, &err[ii]))
     {
      loTagAttrib *ta = &tags[tid[ii]].attr;
      ir[ii].dwAccessRights = am_mask/*loOPC_RIGHTS*/ & ta->taRights;
      ir[ii].vtCanonicalDataType = taVTYPE(ta);
     }
   else hr = S_FALSE;
 lw_rw_unlock(&se->lkMgmt);

 lock_read();
   cctx.cta.vc_lcid = grLCID;
 unlock();

 for(ii = 0; ii < dwCount; ii++)
   if (S_OK == err[ii])
     {
      loTagAttrib *ta = &tags[tid[ii]].attr;
      if (loTF_EMPTY & ta->taFlags)
        { 
          err[ii] = OPC_E_INVALIDITEMID;
          tid[ii] = 0; hr = S_FALSE;
          goto LogErr;
        }
      else if (
        /*VT_EMPTY != pItemDefs[ii].vtRequestedDataType &&*/
         S_OK != (err[ii] = lo_checktype(&cctx, ta,
                                pItemDefs[ii].vtRequestedDataType)))
        { 
          /*err[ii] = OPC_E_BADTYPE */; 
         tid[ii] = 0; hr = S_FALSE; 
         goto LogErr;
        }
      else
         UL_TRACE((LOG_GR("intValidateItem(%ls)(%ls) = %u"),
                          loWnul(pItemDefs[ii].szItemID),
 		                  loWnul(pItemDefs[ii].szAccessPath),
			              tid[ii]));
     }
   else 
     {
LogErr: UL_NOTICE((LOG_GR("intValidateItem(%ls)(%ls) = %s"),
	                  loWnul(pItemDefs[ii].szItemID),
	   		          loWnul(pItemDefs[ii].szAccessPath),
			          loStrError(err[ii])));
     }

Return:

 if (free_tid)
   {
    loAA_FREE(tid);
   }

 if (FAILED(hr))
   {
    UL_TRACE((LOG_GRH("intValidateItems(%lu) = %s"), dwCount, loStrError(hr)));
   }
 else
   {
    *ppResults = ir, ir = 0;
    if (ppErrors) *ppErrors = err, err = 0;

    UL_TRACE((LOG_GRH("intValidateItems(%lu) = %s"), dwCount, loStrError(hr)));
   }
 if (ir)  loComFree(ir);
 if (err) loComFree(err);
 return hr;
}

/**************************************************************************/

STDMETHODIMP LightOPCGroup::RemoveItems(DWORD dwCount,
                                        OPCHANDLE *phServer,
                                        HRESULT **ppErrors)
{
 HRESULT hr = S_OK;
 HRESULT *err = 0;

 LO_CHECK_STATEz1("RemoveItems", ppErrors);

 if (!dwCount || !phServer) { hr = E_INVALIDARG; goto Return; }

 err = (HRESULT*)loComAlloc(dwCount * sizeof(HRESULT));
 if (!err) { hr = E_OUTOFMEMORY; goto Return; }

 lock_write();
   {
    DWORD ii;
    LightOPCItem *item;

    for(ii = 0; ii < dwCount;)
      {
       unsigned tt;
       loTagId ti[32];

       for(tt = 0; tt < SIZEOF_ARRAY(ti) && ii < dwCount; ii++)
         if (item = by_index((unsigned)phServer[ii]))
           {
            if (item->bActive)
              {
               active_count--;
               ti[tt++] = item->tid;
              }
            del((unsigned)phServer[ii]);
            err[ii] = S_OK;
           }
         else
           {
            err[ii] = OPC_E_INVALIDHANDLE;
            hr = S_FALSE;
           }
       if (tt && Active) loChangeActivity(&owner->ctxt.cactx, 0, tt, ti);
      }

   }
 unlock();

Return:
 if (FAILED(hr))
   {
    UL_INFO((LOG_GRH("RemoveItems(%lu) = %s"), dwCount, loStrError(hr)));
   }
 else
   {
    if (ppErrors) *ppErrors = err, err = 0;
    UL_NOTICE((LOG_GRH("RemoveItems(%lu) = %s"), dwCount, loStrError(hr)));
   }
 if (err) loComFree(err);
 LO_FINISH();
 return hr;
}

STDMETHODIMP LightOPCGroup::SetActiveState(DWORD dwCount,
                                           OPCHANDLE *phServer,
                                           BOOL bActive,
                                           HRESULT **ppErrors)
{
 HRESULT hr = S_OK;
 HRESULT *err = 0;
 int aqual = OPC_QUALITY_OUT_OF_SERVICE;
 unsigned aact = 0;

 LO_CHECK_STATEz1("SetActiveState", ppErrors);

 if (!dwCount || !phServer /*|| !ppErrors*/)
   { hr = E_INVALIDARG; goto Return; }

 err = (HRESULT*)loComAlloc(dwCount * sizeof(HRESULT));
 if (!err) { hr = E_OUTOFMEMORY; goto Return; }
 if (bActive = 0 != bActive) aqual = -1;

 lock_write();
   {
    DWORD ii;
    LightOPCItem *item;

    for(ii = 0; ii < dwCount;)
      {
       unsigned tt;
       loTagId ti[32];

       for(tt = 0; tt < SIZEOF_ARRAY(ti) && ii < dwCount; ii++)
         if (item = by_index((unsigned)phServer[ii]))
           {
            if (item->bActive != bActive /*&&
               (OPC_READABLE & tags[item->tid].attr.flag)*/)
              {
               aact++;
               item->bActive = bActive;
               ti[tt++] = item->tid;
               item->Quality = aqual;
               item->TouchChanged();
              }
            err[ii] = S_OK;
           }
         else
           {
            err[ii] = OPC_E_INVALIDHANDLE;
            hr = S_FALSE;
           }

       if (tt && Active) loChangeActivity(&owner->ctxt.cactx, bActive, tt, ti);
      }
    if (bActive)
      active_count += aact, last_trid = 0;
    else active_count -= aact;
   }
 unlock();

Return:

 if (FAILED(hr))
   {
    UL_INFO((LOG_GRH("SetActiveState(%lu, %u) = %s"),
                       dwCount, bActive, loStrError(hr)));
   }
 else
   {
    if (ppErrors) *ppErrors = err, err = 0;
    UL_NOTICE((LOG_GRH("SetActiveState(%lu, %u) = %s"),
                      dwCount, bActive, loStrError(hr)));
   }
 if (err) loComFree(err);

 LO_FINISH();
 return hr;
}

STDMETHODIMP LightOPCGroup::SetClientHandles(DWORD dwCount,
                                             OPCHANDLE *phServer,
                                             OPCHANDLE *phClient,
		                             HRESULT **ppErrors)
{
 HRESULT hr = S_OK;
 HRESULT *err = 0;

 LO_CHECK_STATEz1("SetClientHandles", ppErrors);

 if (!dwCount || !phServer || !phClient)
   { hr = E_INVALIDARG; goto Return; }

 err = (HRESULT*)loComAlloc(dwCount * sizeof(HRESULT));
 if (!err) { hr = E_OUTOFMEMORY; goto Return; }

 lock_write();
   {
    DWORD ii;
    LightOPCItem *item;

    for(ii = 0; ii < dwCount; ii++)
      if (item = by_index((unsigned)phServer[ii]))
	{
         item->hClient = phClient[ii];
         err[ii] = S_OK;
        }
      else
        {
         err[ii] = OPC_E_INVALIDHANDLE;
         hr = S_FALSE;
        }
   }
 unlock();

Return:

 if (FAILED(hr))
   {
    UL_INFO((LOG_GRH("SetClientHandles(%lu) = %s"), dwCount, loStrError(hr)));
   }
 else
   {
    if (ppErrors) *ppErrors = err, err = 0;
    UL_NOTICE((LOG_GRH("SetClientHandles(%lu) = %s"), dwCount, loStrError(hr)));
   }
 if (err) loComFree(err);

 LO_FINISH();
 return hr;
}

STDMETHODIMP LightOPCGroup::SetDatatypes(DWORD dwCount,
                                         OPCHANDLE *phServer,
                                         VARTYPE *pRequestedDatatypes,
		                         HRESULT **ppErrors)
{
 HRESULT hr = S_OK;
 HRESULT *err = 0;
 loTagEntry *te;
 loCallerx cctx;

 LO_CHECK_STATEz1("SetDatatypes", ppErrors);

 if (!dwCount || !phServer || !pRequestedDatatypes)
   { hr = E_INVALIDARG; goto Return; }

 err = (HRESULT*)loComAlloc(dwCount * sizeof(HRESULT));
 if (!err) { hr = E_OUTOFMEMORY; goto Return; }

 te = owner->se->tags;
 cctx = owner->ctxt;

 lock_write();
   {
    DWORD ii;
    LightOPCItem *item;
    cctx.cta.vc_lcid = grLCID;

    for(ii = 0; ii < dwCount; ii++)
      if (item = by_index((unsigned)phServer[ii]))
        {
         VARTYPE vt;
         loTagAttrib *ta;
        /* if (VT_EMPTY == (vt = pRequestedDatatypes[ii]))
           item->vtRequestedDataType = vt, err[ii] = S_OK;
         else */
         if (S_OK == (err[ii] = lo_checktype(&cctx,
                                  ta = &te[item->tid].attr,
				                  vt = pRequestedDatatypes[ii])))
           {
            item->vtRequestedDataType = vt;
            item->convtype = lo_check_conversion(ta, vt);
           }
         else hr = S_FALSE;
        }
      else
        {
         err[ii] = OPC_E_INVALIDHANDLE;
         hr = S_FALSE;
        }
   }
 unlock();

Return:

 if (FAILED(hr))
   {
    UL_INFO((LOG_GRH("SetDatatypes(%lu) = %s"), dwCount, loStrError(hr)));
   }
 else
   {
    if (ppErrors) *ppErrors = err, err = 0;
    UL_NOTICE((LOG_GRH("SetDatatypes(%lu) = %s"), dwCount, loStrError(hr)));
   }
 if (err) loComFree(err);
 LO_FINISH();
 return hr;
}

/* end of itemman.cpp */
