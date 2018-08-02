/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko                               *
                                                                          *
 List of real tags
 **************************************************************************/

#include <math.h> /* fabs() */
#include <errno.h>
#include <stddef.h>
#include <opcerror.h>
#include "loserv.h"
#include "util.h"

/**************************************************************************/

static loTagId lo_find_tag(loService *se, const loWchar *itemid)
{
 lo_hash hash = loSTRHASH(se, itemid);
 lo_hash *tn = se->name_hash,
         *stop = &tn[se->lastnamed];
// static maxx, zoo; int ii = 0, zo0 = zoo;

 while(tn != stop)
   if (*(++tn) == hash)
     {
      loTagEntry *te = &se->tags[tn - se->name_hash];
// if (++ii > maxx) {maxx = ii;  UL_WARNING((LOGID, "MAXX %u", maxx));}

      if (te->attr.taName && !se->wstrcmp(te->attr.taName, itemid))
        return /*VT_EMPTY == V_VT(&te->attr.value)? 0:*/ tn - se->name_hash;
// zoo++;
     }
// if (zo0 != zoo) UL_WARNING((LOGID, "EX %u", zoo));

 return 0;
}

loTagId loFindItemID(loService *se, loCaller *cactx,
                     int locked,
                     const loWchar *itemid,
                     const loWchar *accpath,
                     int vartype,
                     int ai_goal,
                     void   **acpath,
		             HRESULT *ecode)
{
 void *acpa = 0;
 HRESULT hr = OPC_E_UNKNOWNITEMID;
 loTagId ti = 0;

 if (!loSERVICE_OK(se)) { hr = E_INVALIDARG; goto Return; }
 if (!itemid || !*itemid) { hr = OPC_E_INVALIDITEMID; goto Return; }
 if (accpath && !*accpath) accpath = 0;

 if (!se->driver.ldAskItemID ||
    (!accpath || (se->driver.ldFlags & loDF_IGNACCPATH)) &&
     !(se->driver.ldFlags & loDF_CHECKITEM))
   {
    if (!locked) lw_rw_rdlock(&se->lkMgmt);
      ti = lo_find_tag(se, itemid);
    if (!locked) lw_rw_unlock(&se->lkMgmt);
    if (ti && !(se->tags[ti].attr.taFlags & loTF_CHECKITEM))
      goto Return;
   }
 if (se->driver.ldAskItemID)
   {
    if (locked) lw_rw_unlock(&se->lkMgmt);
    hr = se->driver.ldAskItemID(cactx? cactx: &se->cactx,
                                &ti, &acpa,
                                itemid, accpath, vartype, ai_goal);
    if (locked) lw_rw_rdlock(&se->lkMgmt);
    if (S_OK != hr) ti = 0;
    else if (0 == ti)
      {
       if (!locked) lw_rw_rdlock(&se->lkMgmt);
         ti = lo_find_tag(se, itemid);
       if (!locked) lw_rw_unlock(&se->lkMgmt);
      }
   }
Return:
 if (acpath) *acpath = ti? acpa: 0;
 if (ecode) *ecode = ti? S_OK: (S_OK == hr? E_FAIL: hr);
/* if (rdwr) *rdwr = ti? se->tags[ti].attr.taRights: 0;*/

 UL_TRACE((LOGID, "loFindItemID(%ls)(%ls) = %u",
         	   loWnul(itemid), loWnul(accpath), ti));
 return ti;
}

/**************************************************************************/

static double loDecodeRange(VARIANT *tEUinfo)
{
    double rmin, rmax;
    long imin, imax;
    SAFEARRAY *sa;
    HRESULT hr;

#if ll_TRACE > USE_LOG
    if (!tEUinfo || V_VT(tEUinfo) != (VT_ARRAY|VT_R8) ||
        !(sa = V_ARRAY(tEUinfo)) ||
        1 != SafeArrayGetDim(sa) ||
        sizeof(double) != SafeArrayGetElemsize(sa) ||
        S_OK != (hr = SafeArrayGetLBound(sa, 1, &imin)) ||
        S_OK != (hr = SafeArrayGetUBound(sa, 1, &imax)) ||
        imin + 1 != imax ||
        S_OK != (hr = SafeArrayGetElement(sa, &imin, &rmin)) ||
        S_OK != (hr = SafeArrayGetElement(sa, &imax, &rmax))
       ) return -1.;
#else
    if (!tEUinfo || V_VT(tEUinfo) != (VT_ARRAY|VT_R8) ||
        !(sa = V_ARRAY(tEUinfo)))
      {
       UL_TRACE((LOGID, "loAddRealTag->EUinfo.vt = %#x (%p/%p)",
		         tEUinfo? V_VT(tEUinfo): 0, tEUinfo, sa));
       return -1.;
      }
    if (1 != SafeArrayGetDim(sa) ||
        sizeof(double) != SafeArrayGetElemsize(sa))
      {
       UL_TRACE((LOGID, "loAddRealTag->EUinfo.dim = %d .size = %d != %d",
               SafeArrayGetDim(sa),
               SafeArrayGetElemsize(sa), sizeof(double)));
       return -1.;
      }
    if (S_OK != (hr = SafeArrayGetLBound(sa, 1, &imin)) ||
        S_OK != (hr = SafeArrayGetUBound(sa, 1, &imax)) ||
        imin + 1 != imax)
      {
       UL_TRACE((LOGID, "loAddRealTag->EUinfo.bound = [%ld, %ld] %s",
                 imin, imax, loStrError(hr)));
       return -1.;
      }
    if (S_OK != (hr = SafeArrayGetElement(sa, &imin, &rmin)) ||
        S_OK != (hr = SafeArrayGetElement(sa, &imax, &rmax)))
      {
       UL_TRACE((LOGID, "loAddRealTag->EUinfo.element = %s",
               loStrError(hr)));
       return -1.;
      }
 UL_DEBUG((LOGID, "loAddRealTag->EUinfo.range = [%g ... %g]", rmax, rmin));
#endif

 return fabs(rmax - rmin) / 100.;
}

/**************************************************************************/

static int intAddRealTag(loService     *se, /* actual service context */
                         loTagId       *ti, /* returned TagId */
                         loRealTag      rt, /* != 0 */
                         const char    *sName,
                         const loWchar *wName,
                         int            tFlag,
                         unsigned       tRight, /* OPC_READABLE|OPC_WRITEABLE */
                         VARIANT       *tValue,  /* Canonical VARTYPE & default valid value */
                         /*OPCEUTYPE*/int tEUtype,
                         VARIANT       *tEUinfo, /* optional, if tdEUtype is 0*/
                         loTagId        tBase)
{
 int rv = 0;
 double rangecent = 0.0;
 lo_hash ent_hash = 0;
 loTagEntry ent;
 loTagEntry_init(&ent, 1);
 ent.attr.taFlags = 0;
 ent.attr.taRights = 0;
#if 0 == LO_RANGECENT_DETAIL
 ent.attr.taRangecent = 0.;
#endif

// UL_DEBUG((LOGID, "loAddTag(%s)...", loSnul(tName)));
 if (ti) *ti = 0;
 if (!loSERVICE_OK(se)) { rv = EBADF; goto Exit; }
 if (/*!(tRight & (OPC_READABLE|OPC_WRITEABLE)) ||*/
     tEUtype && !tEUinfo)
   { rv = EINVAL; goto Exit; }

 if (tEUtype == OPC_ANALOG &&
     0 > (rangecent = loDecodeRange(tEUinfo)))
   {
    UL_INFO((LOGID, "loAddRealTag->EUinfo.range = %g", rangecent));
    rv = EINVAL; goto Exit;
   }

 if (wName && *wName &&
     !(ent.attr.taName = loWstrdup(wName))) { rv = ENOMEM; goto Exit; }
 if (!ent.attr.taName && sName && *sName &&
     !(ent.attr.taName = loMWstrdup(sName))) { rv = ENOMEM; goto Exit; }

 ent.attr.taName_addlevel = 0;
 if (ent.attr.taName)
   {
    unsigned len;
    len = wcslen(ent.attr.taName);
    while(len && ent.attr.taName[len - 1] == se->branch_sep)
      {
       ent.attr.taName_addlevel++,
       ent.attr.taName[--len] = 0;
      }
    if (len == 0)
      {
       freeX(ent.attr.taName),
       ent.attr.taName = 0,
       ent.attr.taName_addlevel = 0;
      }
    else if (ent.attr.taName_addlevel && !tValue && !tEUinfo)
      preallocX((void**)&ent.attr.taName,
                 (wcslen(ent.attr.taName) + 1) * sizeof(loWchar));
   }

 if (tEUinfo || tValue)
   {
    if (tBase)
      {
       UL_INFO((LOGID, "loAddRealTag-> tBase + tEUinfo + tValue"));
       rv = EINVAL; goto Exit;
      }

    ent.attr.taDetail = (loTagDetail*)
        lo_string_to_struct(ent.attr.taName,
                            offsetof(loTagDetail, tdName),
                            ent.attr.taName);

    if (!ent.attr.taDetail) { rv = ENOMEM; goto Exit; }
    ent.attr.taFlags = loTt_DETAILS;

    if (ent.attr.taName) ent.attr.taName = ent.attr.taDetail->tdName;
    loTagDetail_init(ent.attr.taDetail);
    ent.attr.taRangecent = rangecent;

    if (!tValue || VT_EMPTY == V_VT(tValue)) tFlag |= loTF_EMPTY;
    else if (S_OK != (V_ISBYREF(tValue)?
                   VariantCopyInd(&ent.attr.taValue, tValue)
                 : VariantCopy(&ent.attr.taValue, tValue)) ||
          S_OK != VariantCopy(&ent.primValue, &ent.attr.taValue))
      { rv = EFAULT; goto Exit; }

    if ((ent.attr.taEUtype = (OPCEUTYPE)tEUtype) != 0 &&
        S_OK != VariantCopy(&ent.attr.taEUinfo, tEUinfo))
      { rv = EFAULT; goto Exit; }
   }
 else
   {
    if (!tBase) tFlag |= loTF_EMPTY;
   }

 if (ent.attr.taName) tFlag ^= loTt_VISIBLE;
 else if (!se->driver.ldAskItemID || (tFlag & loTF_EMPTY))
   { rv = EINVAL; goto Exit; }
 else tFlag &= ~loTt_VISIBLE;

#if defined(OPC_QUALITY_BAD) /* OPC DA pre 3.0 */ && 0
 ent.prim.tsQuality = OPC_QUALITY_BAD;
#else
 ent.prim.tsQuality = OPC_QUALITY_WAITING_FOR_INITIAL_DATA;
#endif
 ent.prim.tsError = S_FALSE/*E_FAILED*/;
 ent.active = 0;

#define loTF_ALL (/*OPC_READABLE | OPC_WRITEABLE | */ \
                  loTF_NOCOMP | loTF_NOCONV | loTF_CANONAME | loTF_EMPTY | \
                  loTF_CONVERT | loTF_CHECKITEM | loTF_NOBROWSE)

 ent.attr.taFlags |= tFlag & loTF_ALL |
    se->driver.ldFlags & (loDF_NOCONV | loDF_NOCOMP | loDF_CANONAME);
 ent.attr.taRights |= tRight & loOPC_RIGHTS/*(OPC_READABLE | OPC_WRITEABLE)*/;

 ent.attr.taRt = rt;
 if (!se->driver.ldConvertTags) ent.attr.taFlags &= ~loTF_CONVERT;
 if (!se->driver.ldAskItemID) ent.attr.taFlags &= ~loTF_CHECKITEM;

 ent_hash = loSTRHASH(se, ent.attr.taName);
{
 unsigned ii;
 loTagEntry *te;
 lw_rw_wrlock(&se->lkMgmt);

 if (!ent.attr.taDetail/*tBase*/)
   {
    loTagEntry *te = &se->tags[tBase];
    if (tBase > se->lastused/*tag_count*/ ||
        tBase && !loTE_USED(te) ||
        !te->attr.taDetail)
      {
       rv = ENOENT; goto Unlock;
      }
    ent.attr.taDetail = te->attr.taDetail;
    ent.attr.taRangecent = te->attr.taRangecent;
    ent.attr.taFlags |= te->attr.taFlags & loTF_EMPTY;
    if (S_OK != VariantCopy(&ent.primValue, &ent.attr.taValue))
      { rv = EFAULT; goto Unlock; }
   }
 if (!ent.attr.taRangecent) ent.attr.taFlags |= loTt_ZERORANGE;

   if (ent.attr.taName && (ii = lo_find_tag(se, ent.attr.taName)))
     {
#if 1
      if (ti) *ti = ii;
#endif
      rv = EEXIST; goto Unlock;
     }

   for(ii = se->firstfree; ii < se->tag_count; ii++)
     if (!loTE_USED(&se->tags[ii]))
       {
        te = &se->tags[ii]; goto Found;
       }
   rv = EMLINK/*ENOMEM*/; goto Unlock;

Found:
   lw_mutex_lock(&se->update_pipe.lk);
     if (S_OK != VariantCopy(&se->secondary[ii].tvValue, &ent.attr.taValue))
       rv = EFAULT;
     else
       {
        se->secondary[ii].tvTi = 0;
        if (ii > se->lastused) se->lastused = ii;
        if (ent.attr.taName &&
            ii > se->lastnamed) se->lastnamed = ii;
        se->firstfree = ii/* + 1*/;
        /* we don't know will we use it or not so lets firstfree be one step behind */
        *te = ent;
        te->attr.taTi = ii;
        se->name_hash[ii] = ent_hash;
        ent.attr.taTi = ii;
        rv = 0;
       }
   lw_mutex_unlock(&se->update_pipe.lk);
#if 0
   if (rv) goto Unlock;
   *te = ent;
   te->attr.taTi = ii;
   ent.attr.taTi = ii;
#endif

Unlock:
 lw_rw_unlock(&se->lkMgmt);
}

Exit:
//UL_DEBUG((LOGID, "loAddTag(%s)...Exiting", loWnul(ent.attr.taName)));
 if (rv)
   {
    UL_INFO((LOGID, "%!e loAddTag(%s%ls) failed", rv,
      sName? sName: "", sName && !wName? L"": loWnul(ent.attr.taName)));
    loTagEntry_clear(&ent, 1);
   }
 else
   {
    if (ti) *ti = ent.attr.taTi;
    UL_NOTICE((LOGID, "loAddTag(%ls vt=%x fl=%x ha=%X) = %u",
       loWnul(ent.attr.taName), taVTYPE(&ent.attr),
       ent.attr.taFlags, ent_hash, ent.attr.taTi));
   }

 return rv;
}

/**************************************************************************/

static int intAddRealTag_a(loService  *se, /* actual service context */
                           loTagId    *ti, /* returned TagId */
                           loRealTag   rt, /* != 0 */
                           const char    *sName,
                           const loWchar *wName,
                           int            tFlag,
                           unsigned       tRight, /* OPC_READABLE|OPC_WRITEABLE */
                           VARIANT *tValue,  /* Canonical VARTYPE & default valid value */
                           double range_min,
                           double range_max)
{
 int rv = 0;
 SAFEARRAY *sa;
 SAFEARRAYBOUND sbound[1];
 sbound[0].lLbound = 0;
 sbound[0].cElements = 2;

 if (ti) *ti = 0;

 if (sa = SafeArrayCreate(VT_R8, 1, sbound))
   {
    HRESULT hr;
    VARIANT eui;
    long ix0 = 0, ix1 = 1;

    VARIANTINIT(&eui);
    V_ARRAY(&eui) = sa;
    V_VT(&eui) = VT_ARRAY|VT_R8;

/*    UL_DEBUG((LOGID, "loAddRealTag->Build EU-ANALOGEU = %#x (%p/%p)",
		         V_VT(&eui), &eui, sa));
*/
    if (S_OK != (hr = SafeArrayPutElement(sa, &ix0, &range_min)) ||
        S_OK != (hr = SafeArrayPutElement(sa, &ix1, &range_max)))
      {
       UL_INFO((LOGID, "loAddRealTag_a::SafeArrayPutElement() = %s", loStrError(hr)));
       rv = EFAULT;
      }
    else rv = intAddRealTag(se, ti, rt, sName, wName, tFlag, tRight, tValue, OPC_ANALOG, &eui, 0);
    VariantClear(&eui);
   }
 else
   {
    rv = ENOMEM;
    UL_INFO((LOGID, "loAddRealTag_a::SafeArrayCreate() failed"));
   }
 return rv;
}

/**************************************************************************/

int loAddRealTag(loService     *se, /* actual service context */
                 loTagId       *ti, /* returned TagId */
                 loRealTag      rt, /* != 0 */
                 const char    *tName,
                 int            tFlag,
                 unsigned       tRight, /* OPC_READABLE|OPC_WRITEABLE */
                 VARIANT       *tValue,  /* Canonical VARTYPE & default valid value */
                 /*OPCEUTYPE*/int tEUtype,
                 VARIANT       *tEUinfo) /* optional, if tdEUtype is 0*/
{
// UL_DEBUG((LOGID, "loAddTag(%s)...", loSnul(tName)));
 return intAddRealTag(se, ti, rt, tName, 0, tFlag, tRight, tValue, tEUtype, tEUinfo, 0);
}

int loAddRealTag_a(loService     *se, /* actual service context */
                 loTagId       *ti, /* returned TagId */
                 loRealTag      rt, /* != 0 */
                 const char    *tName,
                 int            tFlag,
                 unsigned       tRight, /* OPC_READABLE|OPC_WRITEABLE */
                 VARIANT       *tValue,  /* Canonical VARTYPE & default valid value */
                 double        range_min,
                 double        range_max) /* optional, if tdEUtype is 0*/
{
// UL_DEBUG((LOGID, "loAddTag_a(%s)...", loSnul(tName)));
 return intAddRealTag_a(se, ti, rt, tName, 0, tFlag, tRight, tValue, range_min, range_max);
}

int loAddRealTag_b(loService     *se, /* actual service context */
                 loTagId       *ti, /* returned TagId */
                 loRealTag      rt, /* != 0 */
                 const char    *tName,
                 int            tFlag,
                 unsigned       tRight, /* OPC_READABLE|OPC_WRITEABLE */
                 loTagId        tBase)
{
// UL_DEBUG((LOGID, "loAddTag_b(%s)...", loSnul(tName)));
 return intAddRealTag(se, ti, rt, tName, 0, tFlag, tRight, 0, 0, 0, tBase);
}

int loAddRealTagW(loService     *se, /* actual service context */
                 loTagId       *ti, /* returned TagId */
                 loRealTag      rt, /* != 0 */
                 const loWchar *tName,
                 int            tFlag,
                 unsigned       tRight, /* OPC_READABLE|OPC_WRITEABLE */
                 VARIANT       *tValue,  /* Canonical VARTYPE & default valid value */
                 /*OPCEUTYPE*/int tEUtype,
                 VARIANT       *tEUinfo) /* optional, if tdEUtype is 0*/
{
// UL_DEBUG((LOGID, "loAddTagU(%ls)...", loWnul(tName)));
 return intAddRealTag(se, ti, rt, 0, tName, tFlag, tRight, tValue, tEUtype, tEUinfo, 0);
}

int loAddRealTag_aW(loService     *se, /* actual service context */
                 loTagId       *ti, /* returned TagId */
                 loRealTag      rt, /* != 0 */
                 const loWchar *tName,
                 int            tFlag,
                 unsigned       tRight, /* OPC_READABLE|OPC_WRITEABLE */
                 VARIANT       *tValue,  /* Canonical VARTYPE & default valid value */
                 double        range_min,
                 double        range_max) /* optional, if tdEUtype is 0*/
{
// UL_DEBUG((LOGID, "loAddTag_aU(%ls)...", loWnul(tName)));
 return intAddRealTag_a(se, ti, rt, 0, tName, tFlag, tRight, tValue, range_min, range_max);
}

int loAddRealTag_bW(loService     *se, /* actual service context */
                 loTagId       *ti, /* returned TagId */
                 loRealTag      rt, /* != 0 */
                 const loWchar *tName,
                 int            tFlag,
                 unsigned       tRight, /* OPC_READABLE|OPC_WRITEABLE */
                 loTagId        tBase)
{
// UL_DEBUG((LOGID, "loAddTagU(%ls)...", loWnul(tName)));
 return intAddRealTag(se, ti, rt, 0, tName, tFlag, tRight, 0, 0, 0, tBase);
}

/**************************************************************************/
/* end of realtag.c */
