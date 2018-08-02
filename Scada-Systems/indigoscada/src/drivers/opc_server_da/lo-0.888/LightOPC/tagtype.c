/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko                               *
                                                                          *
 tag types' checking
 **************************************************************************/

/*#include <math.h>  fabs() */
//#include <errno.h>
#include <opcerror.h>
#include "loserv.h"
#include "util.h"

/**************************************************************************/

#define loDO_CONVERT(attr,type) ((loTF_CONVERT & (attr)->taFlags) && \
                                 ( VT_BSTR == (type) ||              \
                                   VT_DATE == (type) ||              \
                                  (VT_ARRAY & (type)) ) )

HRESULT lo_checktype(loCallerx *cctx, loTagAttrib *attr, VARTYPE type)
{
 HRESULT hr = S_OK;
 int pconv = 0;
 HRESULT meq = S_OK;
 WORD qua = 0;
 loTagPair tp;
 VARIANT tmp; VARIANTINIT(&tmp);

#if 0
 if (loTF_EMPTY & attr->taFlag) return OPC_E_UNKNOWNITEMID;
#endif
 if (VT_EMPTY == taVTYPE(attr)) return OPC_E_INVALIDITEMID;//UNKNOWNITEMID;

 if (VT_EMPTY == type) type = taVTYPE(attr);

 if (loDO_CONVERT(attr, type))
   {
    if ((loTF_NOCONV & attr->taFlags) &&
        (type != VT_EMPTY || type != taVTYPE(attr)))
      return OPC_E_BADTYPE;
    tp.tpAP = 0; /* unknown */
    tp.tpTi = attr->taTi;
    tp.tpRt = attr->taRt;
    pconv = 1;
   }
 else
   {
    if (type == VT_EMPTY) return S_OK;
    if (type == taVTYPE(attr)) return S_OK;
    if (loTF_NOCONV & attr->taFlags) return OPC_E_BADTYPE;
   }

 if (pconv)
   cctx->cactx.ca_se->driver.ldConvertTags(&cctx->cactx, 1, &tp,
                  &tmp, &qua, &hr, &meq, &meq, &attr->taValue, &type, 
                  cctx->cta.vc_lcid);
 else hr = VARIANTCHANGEt(&tmp, &attr->taValue, cctx->cta, type);

 if (S_OK == hr && OPC_ANALOG == attr->taEUtype)
   {
    double *minmax;
         
    if (type & VT_ARRAY)
      {  
       /* Let's check 2 array members at once */
       if (pconv)
         cctx->cactx.ca_se->driver.ldConvertTags(&cctx->cactx, 1, &tp,
                       &tmp, &qua, &hr, &meq, &meq, 
                       &attr->taEUinfo, &type, cctx->cta.vc_lcid);
       else hr = VARIANTCHANGEt(&tmp, &attr->taEUinfo, cctx->cta, type);
      } 
    else /* already checked by AddRealTag/decode_range */
      if (/*V_VT(&attr->eu_info) == (VT_ARRAY|VT_R8) && V_ARRAY(&attr->eu_info) &&*/
        S_OK == (hr = loSA_AccessData(V_ARRAY(&attr->taEUinfo), (void**)&minmax)))
      {
       VARIANT mm;
       V_R8(&mm) = minmax[0];
       V_VT(&mm) = VT_R8;
       if (pconv) cctx->cactx.ca_se->driver.ldConvertTags(&cctx->cactx, 1, &tp,
                       &tmp, &qua, &hr, &meq, &meq, &mm, &type, 
                       cctx->cta.vc_lcid);
       else hr = VARIANTCHANGEt(&tmp, &mm, cctx->cta, type);
       if (S_OK == hr)
         {
          V_R8(&mm) = minmax[1];
          if (pconv) cctx->cactx.ca_se->driver.ldConvertTags(&cctx->cactx, 1, &tp,
                          &tmp, &qua, &hr, &meq, &meq, &mm, &type, 
                          cctx->cta.vc_lcid);
          else hr = VARIANTCHANGEt(&tmp, &mm, cctx->cta, type);
         }
       SafeArrayUnaccessData(V_ARRAY(&attr->taEUinfo));
      }
    else
      {
       UL_ERROR((LOGID, "lo_checktype(%d>%ls) bad EU/%s",
                         type, loWnul(attr->taName), loStrError(hr)));
       hr = S_OK;
      }
   }
 VariantClear(&tmp);
#if 0 /* LO_E_BADTYPE( */
 return S_OK == hr? S_OK: OPC_E_BADTYPE;
#else
 return hr;
#endif
}

loCONV lo_check_conversion(loTagAttrib *attr, VARTYPE requested)
{
 if (VT_EMPTY == requested) requested = taVTYPE(attr);

 if (loDO_CONVERT(attr, requested)) return loCONV_CONVERT;

 if (taVTYPE(attr) == requested ||
     VT_EMPTY == requested)
   {
    if (!(VT_ARRAY & taVTYPE(attr)) &&
          VT_BSTR != taVTYPE(attr)) return loCONV_ASSIGN;
    return loCONV_COPY;
   }
 return loCONV_CHANGE;
}

/**************************************************************************/

void loChangeActivity(loCaller *ca, int act, unsigned count, loTagId *til)
{
  int cc;
  loTagId ti;
  loTagPair ch_rt[32];
  loService *se;

 UL_TRACE((LOGID, "loChangeActivity(%u %u)", act, count));
 
 if (!til || !ca) return;
 se = ca->ca_se;
 if (!loSERVICE_OK(se) || !se->driver.ldSubscribe) return;

 if (se->driver.ldFlags & loDF_SUBSCRIBE_RAW)
   {
     while(count)
       {
        for(cc = 0; cc < SIZEOF_ARRAY(ch_rt) && count; til++, count--)
          if (0 < (ti = *til) && ti < se->tag_count/*ti <= se->lastused*/)
            {                     /* se->lastused is not locked now */
             loTagEntry *te = &se->tags[ti];
             ch_rt[cc].tpTi = te->attr.taTi;
             ch_rt[cc].tpRt = te->attr.taRt;
             ch_rt[cc++].tpAP = 0;
             UL_TRACE((LOGID, "lo%sActivate(%u %ls)",
                          act? "": "De", ti, loWnul(te->attr.taName)));
            }
        if (cc) se->driver.ldSubscribe(ca, act? cc: -cc, ch_rt);
       }
   }
 else
   {
    while(count)
     {
    //    lw_rw_rdlock(&se->lkMgmt);
      lw_mutex_lock(&se->lkDr);
      for(cc = 0; cc < SIZEOF_ARRAY(ch_rt) && count; til++, count--)
        if (0 < (ti = *til) && ti < se->tag_count/*ti <= se->lastused*/)
          {                     /* se->lastused is not locked now */
           loTagEntry *te = &se->tags[ti];
           if (loTE_USED(te) && 0 == (act? te->active++: --te->active))
             {
              ch_rt[cc].tpTi = te->attr.taTi;
              ch_rt[cc].tpRt = te->attr.taRt;
              ch_rt[cc++].tpAP = 0;
              UL_TRACE((LOGID, "lo%sActivate(%u %ls)",
                        act? "": "De", ti, loWnul(te->attr.taName)));
             }
          }
      if (cc) se->driver.ldSubscribe(&se->cactx, act? cc: -cc, ch_rt);
      lw_mutex_unlock(&se->lkDr);
    //    lw_rw_unlock(&se->lkMgmt);
        // UL_DEBUG((LOGID, "ldSubscribe... finished"));
     }
   }
// UL_DEBUG((LOGID, "loChangeActivity(%u)... finished", act));
}

/* end of tagtype.c */
