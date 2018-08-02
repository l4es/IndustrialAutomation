/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
                                                                          *
 item updating routines
 **************************************************************************/
#include "privopc.h"
#include "nan.h"
#include <math.h>

/*********************************************************************************/

#if 0
#define R8_CLEAR(v) VariantClear(v)
#else
#define R8_CLEAR(v)
#endif

static int lo_compare_lastsent(LightOPCItem *item, loTagEntry *prim, double *deadband)
{
 VARIANT *pv = &prim->primValue;
 VARIANT var;

 if (loTF_NOCOMP & prim->attr.taFlags) return 1;

 if (VT_BSTR == V_VT(pv))
   {
    unsigned lastsent_len = item->lastsent? item->lastsent[0]: 0;
    unsigned len = loBSTR_SIZE(V_BSTR(pv));

    if (deadband &&
        len == lastsent_len &&
        !memcmp(V_BSTR(pv), &item->lastsent[1], len)) return 0;

    if (len <= lastsent_len && lastsent_len ||
        preallocX((void**)&item->lastsent, len + sizeof(item->lastsent[0])))
      {
       memcpy(&item->lastsent[1], V_BSTR(pv), item->lastsent[0] = len);
      }
    return 1;
   }
 VARIANTINIT(&var);

 if (V_ISARRAY(pv)) /* Under Wine the VarChangeType() does not detect arrays. */
   {
    void *ptr;
    HRESULT hr;
    SAFEARRAY *sa = V_ARRAY(pv);

    hr = loSA_AccessData(sa, &ptr);
    if (FAILED(hr))
      {
       UL_ERROR((LOGID, "%!l SafeArrayAccessData() FAILED", hr));
      }
    else
      {
       unsigned len = sa->cbElements * sa->rgsabound[0].cElements;
       unsigned lastsent_len = item->lastsent? item->lastsent[0]: 0;

       if (deadband &&
           len == lastsent_len &&
           !memcmp(ptr, item->lastsent + 1, len))
         {
          SafeArrayUnaccessData(sa); return 0;
         }
       if (len <= lastsent_len && lastsent_len ||
           preallocX((void**)&item->lastsent, len + sizeof(item->lastsent[0])))
         {
          memcpy(item->lastsent + 1, ptr, item->lastsent[0] = len);
         }
       SafeArrayUnaccessData(sa);
      }
   }
 else if (/*VT_EMPTY == (vt = V_VT(pv)) ||*/     
          S_OK == VariantChangeType(&var, pv, 0, VT_R8))
   {
    if (deadband)
      {
       if (!memcmp(&item->numeric, &V_R8(&var), sizeof(item->numeric)))
         {
          R8_CLEAR(&var); return 0;
         }
       if (
#if loTt_ZERORANGE
           !(prim->attr.taFlags & loTt_ZERORANGE) && *deadband &&
#else
           *deadband &&/***/ prim->attr.taRangecent /*OPC_ANALOG*/ &&
#endif
           !lo_IsNANd(V_R8(&var)) && !lo_IsNANd(item->numeric) &&
           fabs(V_R8(&var) - item->numeric) <= *deadband * prim->attr.taRangecent)
         {
          R8_CLEAR(&var); return 0;
         }
/*else UL_ERROR((LOGID, "Dead=%g range/100=%g abs=%g - %g = %g result:%g",
              *deadband,
              prim->attr.rangecent,
              V_R8(&var), item->numeric, fabs(V_R8(&var) - item->numeric),
              fabs(V_R8(&var) - item->numeric)/prim->attr.rangecent));*/
      }
    memcpy(&item->numeric, &V_R8(&var), sizeof(item->numeric));
    R8_CLEAR(&var);
   }
 else UL_INFO((LOGID, "lo_compare_lastent() Unknown VARTYPE 0x%X", V_VT(pv)));

 return 1;
}

static int lo_compare_dummy(LightOPCItem *item, loTagEntry *prim, double *deadband)
{
 return 1; /* Always different */
}

unsigned lo_refresh_items(loService *se, loUpdList *upl, LightOPCGroup *grp)
{
 int (*compare_item)(LightOPCItem *, loTagEntry *, double *);
 unsigned ii, count = grp->active_count;
 LightOPCItem **items = (LightOPCItem **)grp->itl.gl_list;
 double deadband = grp->Deadband;
 loTagEntry *tags = se->tags;

// UL_DEBUG((LOGID, "refresh_items() {..."));

 if (count > grp->itl.gl_count)
   {
    UL_ERROR((LOGID, "Group::active %u(%d) count %u mismatch",
              count, count, grp->itl.gl_count));
    count = grp->itl.gl_count;
   }

 compare_item = (se->driver.ldFlags & loDF_NOCOMP)
           ? lo_compare_dummy
           : lo_compare_lastsent;

 upl->used = 0;
 upl->master_err = S_OK;
 upl->master_qual = S_OK;
 if (lo_upl_grow(upl, count))
   {
    UL_ERROR((LOGID, "%ls refresh/grow(%u) FAILED",
                      loWnul(grp->name), grp->itl.gl_count));
    upl->master_err = E_OUTOFMEMORY;
    upl->master_qual = S_FALSE;
   }
 else
   {
    unsigned used = 0;
    loUpdList lup = *upl;
// lup.deadband = grp->Deadband;
// lup.tags = se->tags;

    count = grp->itl.gl_count;
    for(ii = 0; ii < count; ii++)
      {
       LightOPCItem *item;
       if ((item = items[ii]) && item->bActive)
         {
          HRESULT hr;
          WORD quality;
          loTagEntry *prim = &tags[item->tid];

#if 0 != LO_CHECK_RIGHTS
          if (!(OPC_READABLE & prim->attr.taRights)) continue;
#endif

          if (item->Quality == (quality = prim->prim.tsQuality))
            {
             if (item->last_changed == prim->primChanged)
               {
                if (0 == lup.trqid) continue;
               }
             else if (!compare_item(item, prim, &deadband))
               {
                item->last_changed = prim->primChanged;
                if (0 == lup.trqid) continue;
               }
            }
          else compare_item(item, prim, 0);
          item->last_changed = prim->primChanged;

          if (OPC_QUALITY_GOOD != (OPC_QUALITY_MASK & quality))
            lup.master_qual = S_FALSE;

#if 1 == LO_EV_TIMESTAMP
          if (!IsFILETIME(prim->prim.tsTime))
            lup.timestamp[used] = loFT_SUBST(prim->prim.tsTime, se->ts_prim);
          else
#endif
          lup.timestamp[used] = prim->prim.tsTime;
          lup.opchandle[used] = item->hClient;

          if (S_OK != (lup.errors[used] = prim->prim.tsError))
            lup.master_err = S_FALSE;
#ifdef VAR_CHANGETYPE
          hr = VAR_CHANGETYPE(&lup.variant[used], &prim->primValue,
                              item->vtRequestedDataType);
#else
          switch(item->convtype)
            {
          case loCONV_CHANGE:
             hr = VARIANTCHANGEt(&lup.variant[used], &prim->primValue,
                                 lup.rctx.cta, item->vtRequestedDataType);
             break;
          case loCONV_COPY:
             hr = VariantCopy(&lup.variant[used],
                              &prim->primValue);
             break;
          case loCONV_ASSIGN:
             lup.variant[used] = prim->primValue;
             goto SkipHR;
             hr = S_OK;
             break;
          case loCONV_CONVERT:
               {
                loTagPair tp;
                tp.tpTi = item->tid;
                tp.tpAP = item->AcPath;
                tp.tpRt = prim->attr.taRt;

                se->driver.ldConvertTags(&lup.rctx.cactx, 1, &tp,
                      &lup.variant[used], &quality, &lup.errors[used],
                      &lup.master_err, &lup.master_qual, &prim->primValue,
                      &item->vtRequestedDataType, grp->grLCID);
                goto SkipHR;
                hr = S_OK;
               }
             break;
            }
#endif
          if (S_OK != hr)
            {
             LO_E_BADTYPE_QUAL(hr, lup.errors[used], quality);
             lup.master_err = S_FALSE;
             lup.master_qual = S_FALSE;
//             item->Quality = lup.quality[used];
             //item->Quality = -1; /* try again next time?? */
//UL_WARNING((LOGID, "%!l upd_item(%ls)::CHNGETYPE",loWnul(prim->attr.taName), hr));
            }
SkipHR:   lup.quality[used] = quality;
          item->Quality = quality;
          used++;
 // if (V_VT(&upl->vars[used]) == VT_EMPTY) UL_ERROR((LOGID, "update_item"));
         }
      } /* end of for(;;) */

    if (used > grp->active_count)
      {
       UL_ERROR((LOGID, "Group::active %u used %u mismatch",
              grp->active_count, lup.used));
      }
//    if (S_OK == lup.master_err && !lup.used) lup.master_err = S_FALSE;
    upl->master_err = lup.master_err;
    upl->master_qual = lup.master_qual;
    upl->used = used;
#if 2 == LO_EV_TIMESTAMP
    if (IsFILETIME(*se->ts_prim))
      lo_check_ev_timestamp(used, upl->timestamp, se->ts_prim);
#endif
   }
// UL_DEBUG((LOGID, "refresh_items()...}"));
 return upl->used;
}

/* end of upd_item.cpp */
