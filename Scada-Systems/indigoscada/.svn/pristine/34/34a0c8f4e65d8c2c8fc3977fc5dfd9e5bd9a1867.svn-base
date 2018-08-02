/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2001 by Timofei Bondarenko                             *
                                                                          *
 ItemProperties support: Property Lists
 **************************************************************************/

#include <errno.h>
#include <stddef.h> /* offsetof() */
#include "loserv.h"
#include "proplist.h"
#include "util.h"

int loPropListAssign(loService *se, loPLid plid, loTagId ti, int prio)
{
 int rv = 0;
 loTagEntry *te;

 if (prio > loPROPLIST_MAX || prio < 1 && (prio != -1 || plid))
   {
    rv = EINVAL; goto Return;
   }
 if (!loSERVICE_OK(se))
   {
    rv = EBADF; goto Return;
   }

 lw_rw_wrlock(&se->lkMgmt);

 te = &se->tags[ti];
 if (ti <= se->lastused && loTE_USED(te) &&
     plid < se->proplist_count)
   {
    if (prio > 0) te->attr.taPropList[prio - 1] = plid;
    else
      {
       for(prio = 0; prio < loPROPLIST_MAX; prio++)
         te->attr.taPropList[prio] = plid;
      }
   }
 else rv = ENOENT;

 lw_rw_unlock(&se->lkMgmt);

Return:
 if (rv)
   {
    UL_INFO((LOGID, "%!e loPropListAssign(Tag:%u Pr:%d PLid:%u) failed",
             rv, ti, prio, plid));
   }
 else
   {
    UL_NOTICE((LOGID, "loPropListAssign(Tag:%u Pr:%d PLid:%u) Ok", ti, prio, plid));
   }
 return rv;
}

static void loProperty_clear(loProperty *pr);

#define loProperty_clear(pr) (VariantClear(&(pr)->prValue), freeX(pr))

static void loProperty_clear_all(loProperty **prr)
{
 loProperty *pr;
 while(pr = *prr)
   {
    *prr = pr->prNext;
    loProperty_clear(pr);
   }
}

loPLid loPropListCreate(loService *se)
{
 loPLid plid = 0;
 unsigned pl_count, ad_count;

 if (!loSERVICE_OK(se)) return 0;

 lw_rw_wrlock(&se->lkMgmt);

 pl_count = se->proplist_count;
 ad_count =  (pl_count == 0)? 2: 1;

 if (preallocX((void**)&se->proplist, sizeof(loProperty*) * (pl_count + ad_count)))
   {
    memset(se->proplist + pl_count, 0, sizeof(loProperty*) * ad_count);
    plid = pl_count + ad_count - 1;
    se->proplist_count = pl_count + ad_count;
   }

 lw_rw_unlock(&se->lkMgmt);

 if (plid)
   {
    UL_NOTICE((LOGID, "loPropListCreate(%u) Ok", plid));
   }
 else
   {
    UL_INFO((LOGID, "loPropListCreate() failed - Out of memery"));
   }

 return plid;
}


static int intPropertyAdd(loService *se, loPLid plid, unsigned propid, VARIANT *val,
                          const char *path, const char *description,
                          const loWchar *pathW, const loWchar *descriptionW)
{
 int rv = 0;
 loProperty *prop, **prlist;

 if (!loSERVICE_OK(se)) return EBADF;

 if (pathW)
   {
    prop = (loProperty*)lo_string_to_struct(0, offsetof(loProperty, prPath), pathW);
   }
 else if (path)
   {
    if ((pathW = loMWstrdup(path)) &&
        !(prop = (loProperty*)
          lo_string_to_struct((void*)pathW, offsetof(loProperty, prPath), pathW)))
      freeX((void*)pathW);
   }
 else
   {
    prop = (loProperty*)lo_string_to_struct(0, offsetof(loProperty, prPath), L"");
   }
 if (!prop) { rv = ENOMEM; goto Return; }

 prop->prNext = 0;
 prop->prPropID = propid;
 VARIANTINIT(&prop->prValue);
 prop->prDescriptionW = descriptionW;
 prop->prDescription = description? description: lo_prop_descr(propid);

 if (val && S_OK != VariantCopy(&prop->prValue, val))
   {
    rv = ENOMEM; goto Return;
   }

 lw_rw_wrlock(&se->lkMgmt);
 if (prlist = lo_proplist(se, plid))
   {
    prlist = lo_prop_locate(prlist, propid);
    if (*prlist && (*prlist)->prPropID == propid)
      rv = EEXIST;
    else
      {
       prop->prNext = *prlist;
       *prlist = prop;
       prop = 0;
      }
   }
 else rv = EINVAL;

 lw_rw_unlock(&se->lkMgmt);

Return:
 if (prop) loProperty_clear_all(&prop);

 if (rv)
   {
    UL_INFO((LOGID, "%!e loPropertyAdd(%u) failed", rv, plid));
   }
 else
   {
    UL_NOTICE((LOGID, "loPropertyAdd(%u) %u/%x Ok", plid, propid, propid));
   }
 return rv;
}

int loPropertyAdd(loService *se, loPLid plid, unsigned propid, VARIANT *val,
                  const char *path, const char *description)
{
 return intPropertyAdd(se, plid, propid, val, path, description, 0, 0);
}

int loPropertyAddW(loService *se, loPLid plid, unsigned propid, VARIANT *val,
                   const loWchar *path, const loWchar *description)
{
 return intPropertyAdd(se, plid, propid, val, 0, 0, path, description);
}

int loPropertyRemove(loService *se, loPLid plid, unsigned propid)
{
 int rv = 0;
 loProperty **prl;

 if (!loSERVICE_OK(se)) return EBADF;
 lw_rw_wrlock(&se->lkMgmt);

 if (prl = lo_proplist(se, plid))
   {
    if ((prl = lo_prop_find(prl, propid)) && *prl)
      {
       loProperty *pr = *prl;
       *prl = pr->prNext;
       pr->prNext = 0;
       loProperty_clear_all(&pr);
      }
    else rv = ENOENT;
   }
 else rv = EINVAL;

 lw_rw_unlock(&se->lkMgmt);

 if (rv)
   {
    UL_INFO((LOGID, "%!e loPropertyRemove(%u :: %u/%x) failed", rv, plid, propid, propid));
   }
 else
   {
    UL_NOTICE((LOGID, "loPropertyRemove(%u :: %u/%x) Ok", plid, propid, propid));
   }

 return rv;
}

int loPropertyChange(loService *se, loPLid plid, unsigned propid, VARIANT *val)
{
 int rv = 0;
 loProperty **prl;

 if (!loSERVICE_OK(se)) return EBADF;
 lw_rw_wrlock(&se->lkMgmt);

 if (prl = lo_proplist(se, plid))
   {
    if ((prl = lo_prop_find(prl, propid)) && *prl)
      if (val)
        {
         HRESULT hr;
         hr = VariantCopy(&(*prl)->prValue, val);
         if (S_OK != hr)
           rv = E_OUTOFMEMORY == hr? ENOMEM: EFAULT;
        }
      else VariantClear(&(*prl)->prValue);
    else rv = ENOENT;
   }
 else rv = EINVAL;

 lw_rw_unlock(&se->lkMgmt);

 if (rv)
   {
    UL_INFO((LOGID, "%!e loPropertyChange(%u :: %u/%x) failed", rv, plid, propid, propid));
   }
 else
   {
    UL_TRACE((LOGID, "loPropertyChange(%u :: %u/%x) Ok", plid, propid, propid));
   }
 return rv;
}


loProperty **lo_prop_locate(loProperty **prl, unsigned propid)
{
 if (prl)
   {
    while(*prl && (*prl)->prPropID > propid) prl = &(*prl)->prNext;
   }
 return prl;
}

loProperty **lo_prop_find(loProperty **prl, unsigned propid)
{
 return ((prl = lo_prop_locate(prl, propid)) &&
        *prl && (*prl)->prPropID == propid)? prl: 0;
}

int lo_proplist_init(loService *se)
{
 se->proplist_count = 0;
 if (se->proplist = (loProperty**)mallocX(sizeof(loProperty*)))
   {
    se->proplist[0] = 0;
    se->proplist_count = 1;
    return 0;
   }
 return ENOMEM;
}

void lo_proplist_clear(loService *se)
{
 if (se->proplist)
   {
    unsigned xx = se->proplist_count;
    loProperty **proplist = se->proplist;
    se->proplist_count = 0;
    se->proplist = 0;

    while(xx--) loProperty_clear_all(proplist + xx);
    freeX(proplist);
   }
 se->proplist_count = 0;
}

/* end of proplist.c */
