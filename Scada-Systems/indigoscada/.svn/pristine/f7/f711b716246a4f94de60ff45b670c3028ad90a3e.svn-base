/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
                                                                          *
 Cache updating & related routines
 **************************************************************************/
#include <errno.h>
#include "loserv.h"
#include "util.h"

/**************************************************************************/

loTrid loTridLast(loService *se) /* get the last id of primary cache update. */
{
 loTrid trid;
 if (!loSERVICE_OK(se)) return 0;
 lw_rw_rdlock(&se->lkPrim);
 trid = se->update_pipe.tstate < 0? 0: se->prim_trid;
 lw_rw_unlock(&se->lkPrim);
 return trid;
}

int loTridWait(loService *se, loTrid trid) /* wait for data be moved to primary cache. */
{
 int rv = 0;
 if (!loSERVICE_OK(se)) return -1;
 if (!loIsTRID(trid)) return -1;
 lw_mutex_lock(&se->update_pipe.lk);
 for(;; lw_condb_wait(&se->lkTridWait, &se->update_pipe.lk))
   {
    unsigned trdif = se->prim_trid - trid;
#if 1
    if (trdif < ~trdif) { rv = 1; break; }
#elif 1
    if (trdif <= -trdif) { rv = 1; break; }
#else
    if ((int)trdif >= -(int)trdif) { rv = 1; break; }
#endif
    if (se->update_pipe.tstate < 0 || se->shutdown) break;
   }
 lw_mutex_unlock(&se->update_pipe.lk);
 return rv;
}

int loTridOrder(loTrid earlier, loTrid latter)
{
 unsigned trdif = latter - earlier;
 return (earlier && latter && trdif < ~trdif);
}

/**************************************************************************/

loTagValue *loCacheLock(loService *se)
{
 if (loSERVICE_OK(se))
   {
    lw_mutex_lock(&se->update_pipe.lk);
    if (se->update_pipe.tstate >= 0) return se->secondary;
    lw_mutex_unlock(&se->update_pipe.lk);
   }
 return 0;
}

loTrid loCacheUnlock(loService *se, FILETIME *timestamp)
{
 if (!loIS_VALID(se)) return 0;
 if (se->update_pipe.tstate >= 0) se->update_pipe.tstate = 1;
 if (se->sec_trid == se->prim_trid) ++se->sec_trid;
 if (loTRIDmin > se->sec_trid) se->sec_trid = loTRIDmin;
#if 0 != LO_EV_TIMESTAMP
 if (timestamp) se->ts_sec[se->ts_size] = *timestamp;
#endif
 lw_conds_signal(&se->update_pipe.cond);
 lw_mutex_unlock(&se->update_pipe.lk);
 return se->sec_trid;
}

int loCacheTimestamp(loService *se, unsigned count, const FILETIME ts[])
{
 int rv = 0;

 if (!loSERVICE_OK(se)) return EBADF; /*ERROR:EARG*/
#if 0 != LO_EV_TIMESTAMP
 if (!ts) return EINVAL;
 if (count > se->ts_size)
   {
    FILETIME *tsn;

    if (count > 0xffff) return EINVAL;    
    
    tsn = (FILETIME*)reallocX(se->ts_sec != se->ts_prim? 
                              se->ts_sec: 0, 
                              count * 2 * sizeof(FILETIME));
    if (tsn) se->ts_sec = tsn, se->ts_size = count;
    else rv = ENOMEM, count = se->ts_size;
   }
 memcpy(se->ts_sec + se->ts_size, ts, count * sizeof(FILETIME));
#endif
 return rv;
}

loTrid loCacheUpdate(loService *se, unsigned count, loTagValue taglist[], FILETIME *timestamp)
{
 unsigned ii;
 loTagValue *ts;
#if 0 != LO_FILL_TIMESTAMP
 FILETIME ft;
#endif
// UL_DEBUG((LOGID, "loCacheUpdate(%p %u %p)", se, count, taglist));
 if (!loSERVICE_OK(se)) return 0;//EBADF; /*ERROR:EARG*/
 if (!timestamp && (0 >= count || !taglist)) return 0;
#if 0 != LO_FILL_TIMESTAMP
#if 1 == LO_FILL_TIMESTAMP
 se->driver.ldCurrentTime(&se->cactx, &ft);
#else
 if (timestamp) ft = *timestamp;
 else memset(&ft, 0, sizeof(ft));
#endif
#endif

 if (!(ts = loCacheLock(se)))
   {
    UL_INFO((LOGID, "loCacheUpdate() lock failed"));
    return 0;//-1; /*ERROR:out-of-service*/
   }

 for(ii = 0; ii < count; ii++, taglist++)
   {
    loTagId ti;

    if (0 == (ti = taglist->tvTi)) continue;
    else if (ti < 1 || ti > se->lastused/*tag_count*/)
      {
       /*rv = -1;*//*ERROR:BAD ti*/
       UL_INFO((LOGID, "loCacheUpdate(BAD ti:%u)", ii));
       continue;
      }
    else
      {
       HRESULT hr;
       loTagValue *tv = &ts[ti];
       tv->tvState = taglist->tvState;
#if 0 != LO_FILL_TIMESTAMP
       if (!IsFILETIME(tv->tvState.tsTime)) tv->tvState.tsTime = ft;
#endif
/*     hr = (V_ISBYREF(&taglist->tvValue)?
                       VariantCopyInd(&tv->tvValue, &taglist->tvValue):
                       VariantCopy(&tv->tvValue, &taglist->tvValue)); */
       hr = VariantCopy(&tv->tvValue, &taglist->tvValue);
       if (S_OK != hr)
         {
          LO_E_BADTYPE_QUAL(hr, tv->tvState.tsError,
                            tv->tvState.tsQuality);
          /*rv = -1;*//*ERROR:BAD VALUE*/
          UL_WARNING((LOGID, "%!l loCacheUpdate(VariantCopy())", hr));
         }
       tv->tvTi = ti;
      }
   }
 return loCacheUnlock(se, timestamp);
/* return rv; */
}

/**************************************************************************/

lw_thrrettype loUpdatePipe(void *vse)
{
 loService *se = (loService *)vse;
 if (!loIS_VALID(se))
   {
    UL_ERROR((LOGID, "loUpdatePipe() prematurely terminated"));
    lw_RETURN;
   }
 UL_TRACE((LOGID, "loUpdatePipe() started..."));
 loThrControl_accept(&se->update_pipe);
 lw_mutex_lock(&se->update_pipe.lk);
 for(;; lw_conds_wait(&se->update_pipe.cond, &se->update_pipe.lk))
   if (0 != se->update_pipe.tstate)
     {
      if (0 > se->update_pipe.tstate) break; /* terminate thread */
      lw_mutex_unlock(&se->update_pipe.lk);
      lw_rw_wrlock(&se->lkPrim);
      lw_mutex_lock(&se->update_pipe.lk);
      if (0 > se->update_pipe.tstate)
        { lw_rw_unlock(&se->lkPrim); break; }/* terminate thread */
{       /* do actual update here */
 unsigned ii = se->lastused;
 loTagEntry *te = se->tags;
 loTagValue *ts = se->secondary;
 loTrid prim_changed = se->sec_trid;

 while(ii--)
   {
    te++;
    ts++;
    if (ts->tvTi)
      {
       if (ts->tvTi == loCH_TIMESTAMP)
         te->prim.tsTime = ts->tvState.tsTime;
       else
         {
          te->primChanged = prim_changed;
          te->prim = ts->tvState;
#if LO_KEEP_OLD_CACHE
          { HRESULT hr;
          if (S_OK != (hr = VariantCopy(&te->primValue, &ts->tvValue)))
            {
             LO_E_BADTYPE_QUAL(hr, te->prim.tsError, te->prim.tsQuality);
             UL_WARNING((LOGID, "%!l loUpdatePipe(VariantCopy())", hr));
          } }
#else
          VariantClear(&te->primValue);
          te->primValue = ts->tvValue;
          VARIANTINIT(&ts->tvValue);
#endif
         }
       ts->tvTi = 0;
      }
   }
}
      se->prim_trid = se->sec_trid;
#if 0 != LO_EV_TIMESTAMP
      if (se->ts_prim != se->ts_sec) 
        {
         freeX(se->ts_prim);
         se->ts_prim = se->ts_sec;
        }
      memcpy(se->ts_prim, 
             se->ts_prim + se->ts_size, 
             se->ts_size * sizeof(FILETIME));
#endif
      lw_rw_unlock(&se->lkPrim);
      se->update_pipe.tstate = 0;
      lw_condb_broadcast(&se->lkTridWait);
     }
 loThrControl_finish(&se->update_pipe);
 lw_mutex_unlock(&se->update_pipe.lk);
 UL_NOTICE((LOGID, "loUpdatePipe() finished"));

 lw_RETURN;
}

#if 2 == LO_EV_TIMESTAMP

void lo_check_ev_timestamp(unsigned count, FILETIME *timestamp, 
                           const FILETIME *master)
{
 while(count--)
   {
    if (!IsFILETIME(*timestamp)) 
      *timestamp = loFT_SUBST(*timestamp, master);
    timestamp++;
   }
}

#endif
/**************************************************************************/
/* cacheupd.c */
