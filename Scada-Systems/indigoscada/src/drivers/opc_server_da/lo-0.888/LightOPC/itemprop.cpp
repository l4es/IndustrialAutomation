/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2001  Timofei Bondarenko                               *
                                                                          *
 ItemProperties support
 **************************************************************************/

#include "privopc.h"
#include "proplist.h"
#include <opcerror.h>

/***************************/

static HRESULT lo_basic_props(int *lockst, loClient *cli, loTagEntry *te, DWORD propid, VARIANT *var)
{
 HRESULT hr = S_OK;

 if (VT_EMPTY != V_VT(var)) VariantClear(var);
 if (VT_EMPTY == taVTYPE(&te->attr)) return OPC_E_INVALID_PID;

 switch(propid)
   {
 case 1: /* "Item Canonical DataType" */
    V_I2(var) = taVTYPE(&te->attr);
    V_VT(var) = VT_I2;
    break;

 case 5: /* "Item Access Rights" */
   V_I4(var) = te->attr.taRights &
              ((cli->access_mode & loAM_RDONLY_BROWSE)? (loOPC_RIGHTS & ~OPC_WRITEABLE)
                                                      :  loOPC_RIGHTS);
   V_VT(var) = VT_I4;
   break;

 case 6: /* "Server Scan Rate" */
   V_R4(var) = (float)cli->se->driver.ldRefreshRate_min;
   V_VT(var) = VT_R4;
   break;

 case 7: /* "Item EU Type" */
   V_I4(var) = te->attr.taDetail->tdEUtype;
   V_VT(var) = VT_I4;V_I4(var) = 2;
   break;

 case 8: /* "Item EU Info" */
   if ((te->attr.taFlags & loTF_CONVERT) &&
        te->attr.taDetail->tdEUtype == OPC_ENUMERATED)
      hr = lo_conv_enum(&cli->ctxt, var, &te->attr, NULL);
   else hr = VariantCopy(var, &te->attr.taDetail->tdEUinfo);
   break;

 case 2: /* "Item Value" */
 case 3: /* "Item Quality" */
 case 4: /* "Item Timestamp" */

   if (!lockst) lw_rw_rdlock(&cli->se->lkPrim);
   else if (!*lockst) *lockst = 1, lw_rw_rdlock(&cli->se->lkPrim);
 switch(propid)
   {
 case 2: /* "Item Value" */
    hr = (te->attr.taRights & OPC_READABLE)? VariantCopy(var, &te->primValue)
                                           : OPC_E_BADRIGHTS;
    break;

 case 3: /* "Item Quality" */
    V_I2(var) = (WORD)te->prim.tsQuality;
    V_VT(var) = VT_I2;
    break;

 case 4: /* "Item Timestamp" */
    V_DATE(var) = lo_filetime_to_variant(&te->prim.tsTime);
    V_VT(var) = VT_DATE;
    /* Anyway more accurate than SystemTimeToVariantTime(); */
    break;
   }
   if (!lockst) lw_rw_unlock(&cli->se->lkPrim);
   break;

 default:
    hr = OPC_E_INVALID_PID;
   }
 return hr;
}


/*************************************************************************/

STDMETHODIMP LightOPCServer::GetItemProperties(
            /* [in] */ LPWSTR szItemID,
            /* [in] */ DWORD dwCount,
            /* [size_is][in] */ DWORD *pdwPropertyIDs,
            /* [size_is][size_is][out] */ VARIANT **ppvData,
            /* [size_is][size_is][out] */ HRESULT **ppErrors)
{
 HRESULT hr = S_OK;
 HRESULT *errs = 0;
 VARIANT *vdata = 0;
 loTagPair tgp;

 LO_CHECK_STATEz2("GetItemProperties", ppvData, ppErrors);

 if (!szItemID || !dwCount || !pdwPropertyIDs || !ppvData || !ppErrors)
   { hr = E_INVALIDARG; goto Return; }

 if (!(errs = (HRESULT*)loComAlloc(sizeof(HRESULT) * dwCount)) ||
     !(vdata = (VARIANT*)loComAlloc(sizeof(VARIANT) * dwCount)))
   { hr = E_OUTOFMEMORY; goto Return; }
 loVariant_init(vdata, dwCount);
// memset(errs, 0, sizeof(HRESULT) * dwCount);
 memset(&tgp, 0, sizeof(tgp));

 lw_rw_rdlock(&se->lkMgmt);
 if (tgp.tpTi = loFindItemID(se, &ctxt.cactx, 1, szItemID, 0,
                       VT_EMPTY, loDAIG_IPROPGET, 0, &hr))
   {
    unsigned ii;
    int unlk = 0;
    loTagEntry *te = &se->tags[tgp.tpTi];
    loRelName rn;
    rn.se = se;
    rn.cactx = &ctxt.cactx;
    rn.locked = 1;
    loStringBuf_init(&rn.sb);
    loBaseName_init(&rn.bn, &te->attr, se->branch_sep);

//    tgp.tpTi = ti;
    tgp.tpRt = te->attr.taRt;

    for(ii = 0; ii < dwCount; ii++)
      {
       HRESULT lhr = OPC_E_INVALID_PID;
       unsigned pli;

       for(pli = 0; pli < loPROPLIST_MAX; pli++)
         {
          loProperty **prr;
          if (prr = lo_prop_find(&se->proplist[te->attr.taPropList[pli]],
                                 pdwPropertyIDs[ii]))
            {
             loProperty *pr = *prr;

             if (VT_EMPTY != V_VT(&pr->prValue))
               {
                lhr = VariantCopy(&vdata[ii], &pr->prValue);
               }
             else if (pr->prPath && *pr->prPath)
               {
                loTagId tti = lo_relative_tag(&rn, pr->prPath, loDAIG_IPROPRGET);
                lhr = rn.err;
                if (tti)
                  {
                   if (!unlk) { unlk = 1; lw_rw_rdlock(&se->lkPrim); }
#if 0
                   if (!(se->tags[tti].attr.taRights & OPC_READABLE)) lhr = OPC_E_BADRIGHTS;
                   else
#endif
                     lhr = VariantCopy(&vdata[ii], &se->tags[tti].primValue);
                  }
                else if (E_OUTOFMEMORY != lhr) lhr = E_FAIL;
               }
             else lhr = OPC_E_INVALID_PID;
             goto Cont;
            }
         }/* end of for(pli...) */
       lhr = lo_basic_props(&unlk, this, te, pdwPropertyIDs[ii], &vdata[ii]);
Cont:
       if (S_OK != (errs[ii] = lhr)) hr = S_FALSE;
      }  /* end of for(...dwCount) */
    if (unlk) lw_rw_unlock(&se->lkPrim);
    loStringBuf_clear(&rn.sb);
   }
 lw_rw_unlock(&se->lkMgmt);

 if (se->driver.ldGetItemProperties && ( SUCCEEDED(hr) 
     || OPC_E_UNKNOWNITEMID == hr
     || OPC_E_INVALIDITEMID == hr) )
   {
    LCID locale = 0;
    GetLocaleID(&locale);
    hr = se->driver.ldGetItemProperties(&ctxt.cactx, &tgp,
            szItemID, dwCount, pdwPropertyIDs, &vdata, &errs, locale);
   }

Return:
 if (FAILED(hr))
   {
    if (errs)
      { loComFree(errs); errs = 0; }
    if (vdata)
      {
       loVariant_clear(vdata, dwCount);
       loComFree(vdata); vdata = 0;
      }
    UL_INFO((LOG_SR("GetItemProperties(%ls, %u) = %s"),
               loWnul(szItemID), dwCount, loStrError(hr)));
   }
 else
   {
    *ppErrors = errs, errs = 0;
    *ppvData = vdata, vdata = 0;
    UL_NOTICE((LOG_SR("GetItemProperties(%ls, %u) ti:%u = Ok"),
              loWnul(szItemID), dwCount, tgp.tpTi));
   }

 LO_FINISH();
 return hr;
}

/***********************************************************************************/

STDMETHODIMP LightOPCServer::LookupItemIDs(
            /* [in] */ LPWSTR szItemID,
            /* [in] */ DWORD dwCount,
            /* [size_is][in] */ DWORD *pdwPropertyIDs,
            /* [size_is][size_is][string][out] */ LPWSTR **ppszNewItemIDs,
            /* [size_is][size_is][out] */ HRESULT **ppErrors)
{
 HRESULT hr = S_OK;
 HRESULT *errs = 0;
 LPWSTR *itemids = 0;
 loTagPair tgp;

 LO_CHECK_STATEz2("LookupItemIDs", ppszNewItemIDs, ppErrors);

 if (!szItemID || !dwCount || !pdwPropertyIDs || !ppszNewItemIDs || !ppErrors)
   { hr = E_INVALIDARG; goto Return; }

 if (!(errs = (HRESULT*)loComAlloc(sizeof(HRESULT) * dwCount)) ||
     !(itemids = (LPWSTR*)loComAlloc(sizeof(LPWSTR) * dwCount)))
   { hr = E_OUTOFMEMORY; goto Return; }
 memset(itemids, 0, sizeof(HRESULT) * dwCount);
// memset(errs, 0, sizeof(HRESULT) * dwCount);
 memset(&tgp, 0, sizeof(tgp));

 lw_rw_rdlock(&se->lkMgmt);
 if (tgp.tpTi = loFindItemID(se, &ctxt.cactx, 1, szItemID, 0,
                       VT_EMPTY, loDAIG_IPROPLOOKUP, 0, &hr))
   {
    unsigned ii;
    loTagAttrib *ta = &se->tags[tgp.tpTi].attr;
    loBaseName base;
    loStringBuf sb;
    loStringBuf_init(&sb);
    loBaseName_init(&base, ta, se->branch_sep);

    tgp.tpRt = ta->taRt;

    for(ii = 0; ii < dwCount; ii++)
      {
       const loWchar *itid = L""; /* no NULL pointers */
       HRESULT lhr = OPC_E_INVALID_PID;
       unsigned pli;

       for(pli = 0; pli < loPROPLIST_MAX; pli++)
         {
          loProperty **prr;
          if (prr = lo_prop_find(&se->proplist[ta->taPropList[pli]],
                                 pdwPropertyIDs[ii]))
            {
             loProperty *pr = *prr;
             if (!pr->prPath || !*pr->prPath)
               {
                hr = S_FALSE;
                lhr = VT_EMPTY == V_VT(&pr->prValue)? OPC_E_INVALID_PID: E_FAIL;
                                 /* A disabled property / No ItemID defined */
               }
             else
               {
                itid = lo_relative_name(&sb, &base, pr->prPath);
                lhr = S_OK;
               }
             goto Cont;
            }
         }
      hr = S_FALSE;
#if 0
      /* What's exactly required by the standard? */
      if (pdwPropertyIDs[ii] >= 1 && pdwPropertyIDs[ii] <= 6 &&
          VT_EMPTY != taVTYPE(&te->attr)) lhr = E_FAIL;
#endif

Cont:
      errs[ii] = lhr;
      if (!itid || !(itemids[ii] = loComWstrdup(itid)))
        {
         hr = E_OUTOFMEMORY;
         break;
        }
     } /* end of for(pli...) */
   loStringBuf_clear(&sb);
  }

 lw_rw_unlock(&se->lkMgmt);

 if (se->driver.ldLookupItemIDs && (SUCCEEDED(hr) 
     || OPC_E_UNKNOWNITEMID == hr
     || OPC_E_INVALIDITEMID == hr) )
   hr = se->driver.ldLookupItemIDs(&ctxt.cactx, &tgp,
            szItemID, dwCount, pdwPropertyIDs, &itemids, &errs);

Return:
 if (FAILED(hr))
   {
    if (errs) loComFree(errs), errs = 0;
    if (itemids) loComFree_LPWSTR(itemids, dwCount), itemids = 0;

    UL_INFO((LOG_SR("LookupItemIDs(%ls, %u) = %s"),
               loWnul(szItemID), dwCount, loStrError(hr)));
   }
 else
   {
    if (ppErrors) *ppErrors = errs, errs = 0;
    else loComFree(errs);
    *ppszNewItemIDs = itemids, itemids = 0;

    UL_NOTICE((LOG_SR("LookupItemIDs(%ls, %u) ti:%u = Ok"),
      loWnul(szItemID), dwCount, tgp.tpTi));
   }

 LO_FINISH();
 return hr;
}

/* end of itemprop.cpp */
