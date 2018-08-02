/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko                               *
                                                                          *
 loService main functions
 **************************************************************************/

#include <errno.h>
#include <stddef.h>
#include "privopc.h"
#include "proplist.h"

unilog *lolog;

#define ifLKMGMT  (1<<0)
#define ifLKPRIM  (1<<1)
#define ifLKSEC   (1<<2)
#define ifLKDR    (1<<3)
#define ifLKLIST  (1<<4)
#define ifTRWAIT  (1<<5)

int loServiceCreate(loService **result, const loDriver *drv, unsigned tagcount)
{
 int rv = 0;
 size_t size;
 loService *se;

 if (!result) return EINVAL;
 *result = 0;
 if (!drv || 0 >= tagcount || 0 >= ++tagcount) return EINVAL;

 UL_DEBUG((LOGID, "structs: tE=%u tA=%u tD=%u tV=%u tS=%u tP=%u",
   sizeof(loTagEntry), sizeof(loTagAttrib), sizeof(loTagDetail),
   sizeof(loTagValue), sizeof(loTagState), sizeof(loTagPair)));

 size = sizeof(loService) +
       (sizeof(loTagEntry) + sizeof(loTagValue) + sizeof(lo_hash)) * tagcount;
 UL_TRACE((LOGID, "loCreate(%u) requested size = %u", tagcount - 1, size));

 se = (loService*)mallocX(size);
 if (!se)
   {
    UL_ERROR((LOGID, "loCreate(%u) requested size = %ud FAILED", tagcount, size));
    return ENOMEM;
   }
 se->iam = se;
 se->servlist = 0;
 se->serv_key = 0;
 se->shutdown = 0;

 se->initphase = 0;
 se->tag_count = 0;
 se->firstfree = 1; /* first entry is not used */
 se->lastused  = 0;
 se->lastnamed = 0;

 se->proplist = 0;
 se->proplist_count = 0;

 se->driver = *drv;

 se->cactx.ca_se = se;
 se->cactx.ca_se_arg = se->driver.ldDriverArg;
 se->cactx.ca_cli = 0;
 se->cactx.ca_cli_arg = 0;

 se->ts_prim = se->ts_sec = 0;
 se->ts_size = 0;

 se->log = 0;
#if 0 <= USE_LOG
 if (!lolog)
   {
    lolog = (unilog*)(se->log = INIT_LOG());
    UL_INFO((LOGID, "UNILOG initialization missed..."));
    UL_TRACE((LOGID, "loCreate(%u) requested size = %u", tagcount - 1, size));
   }
#endif

 se->ts_prim = se->ts_sec = (FILETIME*)mallocX(2 * sizeof(FILETIME));
 if (!se->ts_prim)
   {
    rv = ENOMEM;
    UL_ERROR((LOGID, "ts_prim init() FAILED"));
    goto Fail;
   }
 memset(se->ts_prim, 0, 2 * sizeof(FILETIME));
 se->ts_size = 1;

 if (rv = lo_proplist_init(se))
   {
    UL_ERROR((LOGID, "lo_proplist_init() FAILED"));
    goto Fail;
   }

 lo_setup_clock();
 if (se->driver.ldRefreshRate < 1)
   {
    se->driver.ldRefreshRate = lo_default_timegran(
         se->driver.ldRefreshRate_min < 1 ?
        &se->driver.ldRefreshRate_min : 0);
    if (se->driver.ldRefreshRate/*_min*/ < 1)
      se->driver.ldRefreshRate/*_min*/ = 16;
   }

 if (se->driver.ldRefreshRate_min < se->driver.ldRefreshRate)
   se->driver.ldRefreshRate_min = se->driver.ldRefreshRate;

 if (se->driver.ldQueueMax < 1) se->driver.ldQueueMax = 4; /* DEFAULT */

 if ((se->driver.ldFlags & loDf_EE_SFALSE) == loDf_EE_SFALSE)
   se->driver.ldFlags &= ~loDf_EE_SFALSE;

 se->wstrcmp = (se->driver.ldFlags & loDF_IGNCASE)? _wcsicmp: wcscmp;
 se->wstrncmp = (se->driver.ldFlags & loDF_IGNCASE)? _wcsnicmp: wcsncmp;
 se->wstrhash = (se->driver.ldFlags & loDF_IGNCASE)? lo_wcsihash: lo_wcshash;
#if 0
 se->wstrnhash = (se->driver.ldFlags & loDF_IGNCASE)? lo_wcsnihash: lo_wcsnhash;
#endif

 if (!se->driver.ldCurrentTime) se->driver.ldCurrentTime = ld_current_time;

 if (!se->driver.ldBranchSep ||
     0 >= mbtowc(&se->branch_sep, &se->driver.ldBranchSep, 1)) se->branch_sep = 0;
 UL_TRACE((LOGID, "Branch Separator = \\x%02X <%lc>\\x%02X",
           se->driver.ldBranchSep, se->branch_sep, se->branch_sep));

 se->tags = (loTagEntry*)(&se[1]);
 loTagEntry_init(se->tags, tagcount);
 se->secondary = (loTagValue*)(&se->tags[tagcount]);
 loTagValue_init(se->secondary, tagcount);
 se->tag_count = tagcount;
 se->name_hash = (lo_hash*)(&se->secondary[tagcount]);
 memset(se->name_hash, 0, sizeof(lo_hash) * tagcount);
/* we assume VT_EMPTY === 0 */
 se->sec_trid = se->prim_trid = 0;
/* se->prim_changed = 0; */

 se->tags[0].attr.taDetail = (loTagDetail*)mallocX(sizeof(loTagDetail));
 if (!se->tags[0].attr.taDetail)
   {
    UL_ERROR((LOGID, "loCreate(%u) taDetail FAILED", tagcount));
    rv = ENOMEM;
    goto Fail;
   }
 loTagDetail_init(se->tags[0].attr.taDetail);
 se->tags[0].attr.taFlags = loTt_DETAILS | loTF_EMPTY;
 se->tags[0].attr.taDetail->tdName[0] = 0;
 se->tags[0].attr.taRangecent = 0.0;

 if (rv = lw_rwlock_init(&se->lkMgmt, 0))
   {
    UL_ERROR((LOGID, "loCreate()::lkMgmt FAILED"));
    goto Fail;
   }
 se->initphase |= ifLKMGMT;

 if (rv = lw_rwlock_init(&se->lkPrim, 0))
   {
    UL_ERROR((LOGID, "loCreate()::lkPrim FAILED"));
    goto Fail;
   }
 se->initphase |= ifLKPRIM;

 if (rv = lw_mutex_init(&se->lkList, 0))
   {
    UL_ERROR((LOGID, "loCreate()::lkList FAILED"));
    goto Fail;
   }
 se->initphase |= ifLKLIST;

 if (rv = lw_mutex_init(&se->lkDr, 0))
   {
    UL_ERROR((LOGID, "loCreate()::lkDr FAILED"));
    goto Fail;
   }
 se->initphase |= ifLKDR;

 if (rv = lw_condb_init(&se->lkTridWait, 0))
   {
    UL_ERROR((LOGID, "loCreate()::lkTridWait FAILED"));
    goto Fail;
   }
 se->initphase |= ifTRWAIT;

 if (rv = loThrControl_init(&se->update_pipe))
   {
    UL_ERROR((LOGID, "loCreate()::loThr_init FAILED"));
    goto Fail;
   }
 se->initphase |= ifLKSEC;

 if (rv = loThrControl_start(&se->update_pipe, 0, loUpdatePipe, se))
   {
    UL_ERROR((LOGID, "loCreate()::loThr_start FAILED"));
    goto Fail;
   }

 se->cform_dataonly = RegisterClipboardFormat("OPCSTMFORMATDATA");
 se->cform_datatime = RegisterClipboardFormat("OPCSTMFORMATDATATIME");
 se->cform_writecompl = RegisterClipboardFormat("OPCSTMFORMATWRITECOMPLETE");

 *result = se;
 return 0;
Fail:
 UL_WARNING((LOGID, "%!e loCreate(%u) failed", rv, tagcount));
 loServiceDestroy(se);
 return rv;
}

int loServiceDestroy(loService *se)
{
 UL_TRACE((LOGID, "loServiceDestroy(%p)...", se));

 if (!loIS_VALID(se)) return EBADF;
 if ((se->initphase & (ifLKLIST|ifLKSEC)) == (ifLKLIST|ifLKSEC))
   {
    lw_mutex_lock(&se->lkList);
    if (se->shutdown)
      {
       lw_mutex_unlock(&se->lkList);
       UL_ERROR((LOGID, "loServiceDestroy(%p) shutdown started again", se));
       return EBADF;
      }
    if (se->servlist)
      {
       LightOPCServer *los = (LightOPCServer*)se->servlist;
       se->shutdown = 1;
       while(los)
         {
          los->set_state(loOP_SHUTDOWN|loOP_STOP|loOP_DISCONNECT, 0, 0);
          los = los->serv_next;
         }
       lw_mutex_unlock(&se->lkList);
       UL_NOTICE((LOGID, "loServiceDestroy(%p) shutdown started", se));
       return EBUSY;
      }
    se->shutdown = -1;
    lw_mutex_unlock(&se->lkList);
#if 0
    loThrControl_stop(&se->update_pipe);
#endif
    lw_condb_broadcast_continious(&se->lkTridWait, &se->update_pipe.lk);
   }
 return loInternalServiceDestroy(se);
}

int loInternalServiceDestroy(loService *se)
{
 int rv0, rv;
 unilog *log;

 UL_TRACE((LOGID, "loInternalServiceDestroy(%p)...", se));

 if (!loIS_VALID(se))
   {
    UL_ERROR((LOGID, "loInternalServiceDestroy(%p) illegal pointer", se));
    return EBADF;
   }
 se->iam = 0;
 log = (unilog*)se->log; se->log = 0;
 rv = 0;

 if ((se->initphase & ifLKSEC) &&
     (rv0 = loThrControl_destroy(&se->update_pipe)) && !rv) rv = rv0;

 if ((se->initphase & ifLKMGMT) &&
     (rv0 = lw_rwlock_destroy(&se->lkMgmt)) && !rv) rv = rv0;

 if ((se->initphase & ifLKPRIM) &&
     (rv0 = lw_rwlock_destroy(&se->lkPrim)) && !rv) rv = rv0;

 if ((se->initphase & ifLKDR) &&
     (rv0 = lw_mutex_destroy(&se->lkDr)) && !rv) rv = rv0;

 if ((se->initphase & ifLKLIST) &&
     (rv0 = lw_mutex_destroy(&se->lkList)) && !rv) rv = rv0;

 if ((se->initphase & ifTRWAIT) &&
     (rv0 = lw_condb_destroy(&se->lkTridWait)) && !rv) rv = rv0;

 se->initphase = 0;

 /* cleanup the CACHE & ATTRIB */
 if (se->secondary) loTagValue_clear(se->secondary, se->tag_count);
 if (se->tags)      loTagEntry_clear(se->tags, se->tag_count); 
 se->tag_count = 0;
 lo_proplist_clear(se);

 se->ts_size = 0;
 if (se->ts_prim != se->ts_sec && se->ts_sec) freeX(se->ts_sec);
 se->ts_sec = 0;
 if (se->ts_prim) freeX(se->ts_prim);
 se->ts_prim = 0;

 freeX(se);

 if (rv) UL_WARNING((LOGID, "%!e loDelete() finished", rv));
 else UL_TRACE((LOGID, "loDelete() finished = %u", rv));
   {
#if 0 != LO_USE_MALLOCX
//    extern long mallocX_count;
    UL_WARNING((LOGID, "loDelete()::mallocX = %ld xobjref = %ld",
                   mallocX_count, lo_X_objcount));
#endif
    mallocX_trap();
   }

 if (log) unilog_Delete(log);

 return rv;
}

/**************************************************************************/

const char *loClientName(loClient *cli)
{
 if (cli)
   {
    const char *rv = cli->client_name;
    if (loIS_VALID(cli)) return rv;
   }
 return 0;
}

void *loClientArg(loClient *cli) /* Returns release_handle_arg of service */
{
 //if (cli)
   {
    void *rv = cli->release_handle_arg;
    if (loIS_VALID(cli)) return rv;
   }
 return 0;
}

void *(loDriverArg)(loService *se) /* Returns ldFirstArg of service */
{
 return loIS_VALID(se)? se->driver.ldDriverArg: 0;
}

/**************************************************************************/

int loSetState(loService *se, loClient *cli, int oper, int state, const char *reason)
{
 int rv;
 loWchar *ureason = 0;
 if (reason) ureason = loMWstrdup(reason);
 rv = loSetStateW(se, cli, oper, state, ureason);
 if (ureason) freeX(ureason);
 return rv;
}

int loSetStateW(loService *se, loClient *cli, int oper, int state, const loWchar *reason)
{
 int rv = EBADF;

 if (oper & ~(loOP_OPERATE | loOP_STOP | loOP_DISCONNECT | loOP_SHUTDOWN | loOP_DESTROY))
   return EINVAL;
 if (loSERVICE_OK(se))
   {
    lw_mutex_lock(&se->lkList);
    if (!se->shutdown)
      {
       LightOPCServer *los;
       rv = ENOENT;
       for(los = (LightOPCServer*)se->servlist; los; los = los->serv_next)
         if (!cli || los == cli)
           {
            los->set_state(oper, state, reason);
            rv = 0;
           }
      }
    lw_mutex_unlock(&se->lkList);
   }

 return rv;
}

/**************************************************************************/

loMilliSec loReviseUpdateRate(loService *se, loMilliSec req)
{
 loMilliSec rq;
 if (!se) return 0;
 if (req <= se->driver.ldRefreshRate_min) return se->driver.ldRefreshRate_min;
 if (req > 0x3fffffff) req = 0x3fffffff;
 rq = (req + se->driver.ldRefreshRate - 1) / se->driver.ldRefreshRate;
 if (0 == rq) rq = 1;
#if 1
 else if (rq > 20) return req;
#endif
 return rq * se->driver.ldRefreshRate;
}

int loGetBandwidth(loService *se, loClient *cli)
{
 int rv = -1;
 LightOPCServer *ls;
 loMilliSec bandw_time, curtime;

 if (!loSERVICE_OK(se)) return -1;
 curtime = lo_millisec();
 lw_mutex_lock(&se->lkList);
 ls = se->servlist;
 if (cli)
   {
    while(ls && ls != cli) ls = ls->serv_next;
    if (ls)
      {
       lw_mutex_lock(&ls->async.lk);
       rv = (int)ls->ostatus.dwBandWidth;
       bandw_time = ls->bandwidth_time;
       lw_mutex_unlock(&ls->async.lk);
       rv = (int)lo_adjust_bandwidth(rv, bandw_time, curtime);
      }
   }
 else
   while(ls)
     {
      int tv;
      lw_mutex_lock(&ls->async.lk);
      tv = (int)ls->ostatus.dwBandWidth;
      bandw_time = ls->bandwidth_time;
      lw_mutex_unlock(&ls->async.lk);
      tv = (int)lo_adjust_bandwidth(tv, bandw_time, curtime);
      if (tv > rv) rv = tv;
      ls = ls->serv_next;
     }
 lw_mutex_unlock(&se->lkList);
 return rv < 0? 0: rv;
}

/* end of loservice.cpp */
