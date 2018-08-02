/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2001  Timofei Bondarenko                               *
                                                                          *
 Helpers for ItemProperties support
 **************************************************************************/

#include "privopc.h"  /*??*/
#include <opcerror.h> /*??*/
#include "loserv.h"
#include "proplist.h"
#include "util.h"


/**************************************************************************/

static int lo_prop_insertable(const DWORD *props, unsigned count, DWORD propid)
{
 int low = 0;
 int high = (int)count - 1;

 while(low <= high)
   {
    int indx = ((unsigned)(low + high)) >> 1;

    if (props[indx] == propid) return -1; /* found */
    if (props[indx] > propid)
      high = indx - 1;
    else low = indx + 1;
   }

 return (int)low;
}

unsigned lo_list_insert(void **list, unsigned *total,
                        unsigned count, unsigned itemsize,
                        int index)
{
 if (-1 == (int)index)
   {
    if (0 == count)
      {
       if (*list) loComFree(*list), *list = 0;
       *total = 0;
      }
    else if (count != *total)
      {
       void *xp;
       if (xp = loComRealloc(*list, count * itemsize))
         {
          *list = xp;
          *total = count;
         }
       else count = 0;
      }
   }
 else
   {
    char *xp;
    unsigned newtotal = *total;
    count++;
    if ((unsigned)index >= count) return 0;
    if (count > newtotal)
      {
       newtotal = count + (count >> 1);

       if (newtotal < 6) newtotal = 6;
       if (xp = (char*)loComRealloc(*list, newtotal * itemsize))
         {
          *list = (void*)xp;
          *total = newtotal;
         }
       else return 0;
      }
    else xp = (char*)*list;

    memmove(&xp[itemsize * (index + 1)],
            &xp[itemsize *  index],
                itemsize * (count - index - 1));
    memset( &xp[itemsize *  index], 0, itemsize);
   }
 return count;
}

typedef struct loQueryProp
  {
   unsigned black_count, black_size;
   DWORD *black;

   unsigned props_count,
            props_size, descr_size, vtype_size;
   DWORD   *props;
   LPWSTR  *descr;
   VARTYPE *vtype;

   loRelName  rn;
  } loQueryProp;

static HRESULT lo_prop_insert(loQueryProp *qp,
                              DWORD propid, VARTYPE vt, const loWchar *path,
                              const char *descr, const loWchar *descrW)
{
 unsigned ncount = 0;
 int insw, insb;
 if (-1 != (insb = lo_prop_insertable(qp->black, qp->black_count, propid)) &&
     -1 != (insw = lo_prop_insertable(qp->props, qp->props_count, propid)) )
   {

    if (VT_EMPTY == vt && path && *path)
      {
       loTagId tti = lo_relative_tag(&qp->rn, path, loDAIG_IPROPRQUERY);

       if (tti)
         {
          vt = taVTYPE(&qp->rn.se->tags[tti].attr);
         }
       else
         {
          if (qp->rn.err == E_OUTOFMEMORY) return E_OUTOFMEMORY;
         }
      }
    if (VT_EMPTY == vt)
      {
       ncount = lo_list_insert((void**)&qp->black, &qp->black_size,
                               qp->black_count, sizeof(*qp->black), insb);
       if (ncount)
         {
          qp->black[insb] = propid;
          qp->black_count = ncount;
         }
      }
    else
      {
       loWchar *dsc = 0;

       if (!lo_list_insert((void**)&qp->props, &qp->props_size,
                           qp->props_count, sizeof(*qp->props), insw) ||
           !lo_list_insert((void**)&qp->vtype, &qp->vtype_size,
                           qp->props_count, sizeof(*qp->vtype), insw) ||
           !(dsc = descrW? loComWstrdup(descrW): lo_ComPropDescr(propid, descr)) ||
           !lo_list_insert((void**)&qp->descr, &qp->descr_size,
                           qp->props_count, sizeof(*qp->descr), insw) )
         {
          if (dsc) loComFree(dsc);
          ncount = 0;
         }
       else
         {
          qp->props[insw] = propid;
          qp->vtype[insw] = vt;
          qp->descr[insw] = dsc;
          ncount = ++qp->props_count;
         }
      }
    if (!ncount) return E_OUTOFMEMORY;
   }

 return S_OK;
}

static HRESULT lo_enum_props_basic(loQueryProp *qp, loTagAttrib *ta)
{
    static const struct
      { DWORD      propid;
        VARTYPE     vtype;
        const char *descr;
      } props[] =
      {
    1, VT_I2,    "Item Canonical DataType",
    2, VT_EMPTY, "Item Value",
    3, VT_I2,    "Item Quality",
    4, VT_DATE,  "Item Timestamp",
    5, VT_I4,    "Item Access Rights",
    6, VT_R4,    "Server Scan Rate",
    7, VT_I4,    "Item EU Type",
    8, VT_EMPTY, "Item EU Info"
      };

 unsigned trx;

 for(trx = 0; trx < SIZEOF_ARRAY(props); trx++)
   {
    HRESULT hr;
    VARTYPE vt = props[trx].vtype;
    if (2 == props[trx].propid) vt = taVTYPE(ta);
    else if (8 == props[trx].propid)
      {
        vt = V_VT(&ta->taDetail->tdEUinfo);
        if (vt == VT_EMPTY) continue;
      }

    hr = lo_prop_insert(qp, props[trx].propid, vt, 0,
                            props[trx].descr, 0);
    if (S_OK != hr) return hr;
   }

 return S_OK;
}


STDMETHODIMP LightOPCServer::QueryAvailableProperties(
            /* [in] */ LPWSTR szItemID,
            /* [out] */ DWORD *pdwCount,
            /* [size_is][size_is][out] */ DWORD **ppPropertyIDs,
            /* [size_is][size_is][out] */ LPWSTR **ppDescriptions,
            /* [size_is][size_is][out] */ VARTYPE **ppvtDataTypes)
{
 HRESULT hr = S_OK;
 loTagPair tgp;
 loQueryProp qp;
 memset(&qp, 0, sizeof(qp));

 LO_CHECK_STATEz3("QueryAvailableProperties",
                  ppPropertyIDs, ppDescriptions, ppvtDataTypes);
 if (pdwCount) *pdwCount = 0;
 if (!szItemID || !pdwCount ||
     !ppPropertyIDs || !ppDescriptions || !ppvtDataTypes)
   { hr = E_INVALIDARG; goto Return; }
 memset(&tgp, 0, sizeof(tgp));

 lw_rw_rdlock(&se->lkMgmt);
 if (tgp.tpTi = loFindItemID(se, &ctxt.cactx, 1, szItemID, 0,
                       VT_EMPTY, loDAIG_IPROPQUERY, 0, &hr))
   {
    unsigned pli;
    loTagAttrib *ta = &se->tags[tgp.tpTi].attr;
    loStringBuf_init(&qp.rn.sb);
    loBaseName_init(&qp.rn.bn, ta, se->branch_sep);
    qp.rn.se = se;
    qp.rn.cactx = &ctxt.cactx;
    qp.rn.locked = 1;

    tgp.tpRt = ta->taRt;
    hr = S_OK;

    for(pli = 0; pli < loPROPLIST_MAX; pli++)
      {
       loProperty **prr = lo_proplist(se, ta->taPropList[pli]);
       if (prr)
         {
          loProperty *pr;
          for(pr = *prr; pr; pr = pr->prNext)
            {
             if (S_OK != (hr = lo_prop_insert(&qp, pr->prPropID,
                                              V_VT(&pr->prValue), pr->prPath,
                                              pr->prDescription, pr->prDescriptionW)))
               break;
            }
         }
      } /* end of for(pli...) */

    if (VT_EMPTY != taVTYPE(ta) && S_OK == hr)
      hr = lo_enum_props_basic(&qp, ta);

    loStringBuf_clear(&qp.rn.sb);
   }
 lw_rw_unlock(&se->lkMgmt);

 lo_list_insert((void**)&qp.black, &qp.black_size, 0, sizeof(*qp.black), -1);

 if (S_OK != hr)
   {
    loComFree_LPWSTR(qp.descr, qp.props_count);
    qp.props_count = 0;
   }
 *pdwCount = qp.props_count;
 lo_list_insert((void**)&qp.props, &qp.props_size, qp.props_count, sizeof(*qp.props), -1);
 *ppPropertyIDs = qp.props;
 lo_list_insert((void**)&qp.vtype, &qp.vtype_size, qp.props_count, sizeof(*qp.vtype), -1);
 *ppvtDataTypes = qp.vtype;
 lo_list_insert((void**)&qp.descr, &qp.descr_size, qp.props_count, sizeof(*qp.descr), -1);
 *ppDescriptions = qp.descr;

 if (se->driver.ldQueryAvailableProperties && (S_OK == hr 
     || OPC_E_UNKNOWNITEMID == hr
     || OPC_E_INVALIDITEMID == hr) )
   hr = se->driver.ldQueryAvailableProperties(&ctxt.cactx, &tgp, szItemID, pdwCount,
                               ppPropertyIDs, ppDescriptions, ppvtDataTypes);
 if (FAILED(hr))
   {
    if (*ppPropertyIDs)
      { loComFree(*ppPropertyIDs); *ppPropertyIDs = 0; }
    if (*ppvtDataTypes)
      { loComFree(*ppvtDataTypes); *ppvtDataTypes = 0; }
    if (*ppDescriptions)
      { loComFree_LPWSTR(*ppDescriptions, *pdwCount); *ppDescriptions = 0; }
    *pdwCount = 0;
   }

Return:
 if (FAILED(hr))
   {
    UL_INFO((LOG_SR("QueryAvailableProperties(%ls) = %s"),
               loWnul(szItemID), loStrError(hr)));
   }
 else
   {
    UL_NOTICE((LOG_SR("QueryAvailableProperties(%ls) ti:%u = %u"),
      loWnul(szItemID), tgp.tpTi, *pdwCount));
   }
 LO_FINISH();
 return hr;
}


/* end of propenum.c */
