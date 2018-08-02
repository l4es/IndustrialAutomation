/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko                               *
                                                                          *
 thread procedures for LightOPCServer::async scheduler
 **************************************************************************/
#ifndef _WIN32_DCOM
#define _WIN32_DCOM /* for CoInitializeEx() */
#endif
#include "privopc.h"
#if 0
#include <malloc.h>
#define LO_HEAPCHECK(pp) { int hc; if (_HEAPOK != (hc = _heapchk())) \
        UL_ERROR((LOGID, "heapchk() %d %p %s %d", hc, pp, __FILE__, __LINE__)); }
#else
#define LO_HEAPCHECK(pp)
#endif

#define loHUGESLEEP  (((loMilliSec)~0)>>2)

#define loMAXSLEEP   (0xfffff) /* ~17min default refresh period */

#if 0
#define SATURATE_PERCENT (1) /* 0/1 */
#define USE_DOUBLE       (1) /* 0/1 */
#define USE_PAIR         (0) /* probably == !USE_DOUBLE */
#else
#define SATURATE_PERCENT (1) /* 0/1 */
#define USE_DOUBLE       (0) /* 0/1 */
#define USE_PAIR         (1) /* probably == !USE_DOUBLE */
#endif

#if 0
#define loSTATPERIOD    (1024) /* time for averaging bandwidth value */
unsigned lo_statperiod(unsigned stp) { return loSTATPERIOD; }
#else
static loMilliSec loSTATPERIOD = 1024/*1412*/;
unsigned lo_statperiod(unsigned stp)
{
 if ((stp >> 2) && stp <= (loHUGESLEEP >> 8)) loSTATPERIOD = stp;
 return loSTATPERIOD;
}
#endif

typedef struct loUpdTimes
  {
   loMilliSec    awoke, beginupd, finish;
   loMilliSec    syncpoint, latency, nextsleep;
   unsigned      datasent, overload, updated;
#if 0 != USE_DOUBLE
   double        bandwidth;
#  if 0 != USE_PAIR
   double        bandwidth0;
#  endif
#else
   unsigned long bandwidth;
#  if 0 != USE_PAIR
   unsigned long bandwidth0;
#  endif
#endif
  } loUpdTimes;

static void update_sheduler(LightOPCServer *cli, loService *se, loUpdTimes *times)
{
 unsigned gndx;
 unsigned advmask;
 LightOPCGroup *grp;
 loMilliSec curtime, syncpoint, latency,
            nextsleep = loHUGESLEEP;
 loUpdList upl;
 lo_upl_init(&upl, loUPL_variant | loUPL_timestamp | loUPL_quality | loUPL_errors |
                   loUPL_opchandle);

 curtime = times->beginupd;

 latency = se->driver.ldRefreshRate;
 syncpoint = curtime + (latency >> 1); /* or latency-1 ? We searching closest synchronization point. */
 latency = (syncpoint - times->syncpoint) % latency;
 times->syncpoint = (syncpoint -= latency);
 times->overload = 0;
 times->updated = 0;
 latency = 0;

 if (cli)
   {
    upl.rctx = cli->ctxt;
    cli->lock_read();
    for(gndx = 0; gndx < cli->grl.gl_count; gndx++)
      if ((grp = (LightOPCGroup*)cli->grl.gl_list[gndx]) && grp->Active &&
         (advmask = grp->advise_present & grp->advise_enabled))
        {
         int justactuated = grp->justactuated;
         loMilliSec updrate = grp->UpdateRate;
         loMilliSec elapsed = curtime - grp->LastUpdate;

         if (justactuated || elapsed >= updrate)
           {
            grp->justactuated = 0;
/******* data refreshing code **********/
            lw_rw_rdlock(&se->lkPrim);
            if (grp->last_trid != se->prim_trid) /* is cache changed? */
              {
               grp->last_trid = se->prim_trid;
               upl.rctx.cta.vc_lcid = grp->grLCID;
               lo_refresh_items(se, &upl, grp);
              }
            lw_rw_unlock(&se->lkPrim);
            if (upl.used)
              {
            /* upl.trqid = 0; */
               times->datasent +=
               cli->send_callback(grp, 0, &upl, advmask | loRQ_OP_REFRESH);
               loVariant_clear(upl.variant, upl.used); upl.used = 0;
               cli->lock_read(); /* relock me again */
              }
/******* end of data refreshing code ***/

            elapsed -= updrate;
            if (elapsed <= updrate && !justactuated)
              {
               if (elapsed > latency) latency = elapsed;
               grp->LastUpdate += updrate; /* Ok */
//         UL_WARNING((LOGID, "late %u/%u = %u%% >>%u ",
  //           elapsed, updrate, load, loadmax));
              }
            else /* we're late */
              {
//         UL_ERROR((LOGID, "late %u", elapsed));
               if (!justactuated)
                 {
                  times->overload++;
#if 0 == SATURATE_PERCENT
                  if (elapsed > latency) latency = elapsed;
#endif
                 }
               elapsed = curtime - (grp->LastUpdate = syncpoint);
              }
            times->updated++;
           } /* if (elapsed >= updrate) */
         updrate -= elapsed;
         if (updrate < nextsleep) nextsleep = updrate;
        } /* end of loop */
    cli->unlock();
   } /* enf of If(cli) */

 lo_upl_clear(&upl);

 times->latency = latency;
 times->nextsleep = nextsleep;
}

/* There are four case for timing:
   a) overload -> 100%
   b) Doesn't updated -> (finish - awoke) / statperiod * 100%
      where:
           statperiod = finish - times->finish
           if (nextsleep <= (finish - beginupd)) nextsleep = 0;
           else nextsleep <= (finish - beginupd);
      Updated:
   c) (finish - awoke) / (finish - awoke + nextsleep) * 100%
   d) (finish - beginupd + latency) /
       (finish - beginupd + latency + nextsleep) * 100%
 */
static void sched_timings(loUpdTimes *upt, loMilliSec finish)
{
 unsigned long percent;
 loMilliSec nextsleep = upt->nextsleep;
 loMilliSec statperiod, consumed2;

 consumed2 = finish - upt->beginupd;
 if (nextsleep > consumed2) nextsleep -= consumed2;
 else nextsleep = 0, upt->overload = 1;

 statperiod = upt->finish;
 upt->finish = finish;
 if (0 == (statperiod = finish - statperiod))
   {
#if 0
    statperiod = 1;
#else
    goto SkipBW;
#endif
   }

#if 0 != SATURATE_PERCENT
 if (upt->overload) percent = 100;
 else
#endif
   {
    unsigned long percent2;
    loMilliSec consumed1 = finish - upt->awoke;

    percent = consumed1 * 100;
    if (consumed1 += nextsleep)
      {
       if (consumed1 > statperiod) consumed1 = statperiod;
       percent = (percent + (consumed1 >> 1)) / consumed1;
      }

    percent2 = (consumed2 + upt->latency) * 100;
#if 0 != SATURATE_PERCENT
    if (consumed2 += nextsleep + upt->latency)
#else
    if (consumed2 = upt->nextsleep + upt->latency)
#endif
      percent2 = (percent2 + (consumed2 >> 1)) / consumed2;

    if (percent < percent2) percent = percent2;

#if 0 == SATURATE_PERCENT
    if (percent > 100)
      {
       statperiod = (statperiod * percent + 50) / 100;
    // UL_ERROR((LOGID, "percent=%u", percent));
       percent = 100;
      }
#else
//    if (percent > 100) UL_ERROR((LOGID, "percent=%u", percent));
#endif
   }

/*
 UL_DEBUG((LOGID, "period=%u/%u percent=%u+%u => %u",
   loSTATPERIOD, statperiod,
   (int)upt->bandwidth, percent, (int)(
   (upt->bandwidth * (loSTATPERIOD - statperiod) +
                   percent * statperiod) / loSTATPERIOD)));
// */
#if 0 == USE_PAIR
 upt->bandwidth = (upt->bandwidth * loSTATPERIOD +
                   percent * statperiod) / (statperiod + loSTATPERIOD);
#else
/*
 normalizing statperiod by statperiod /= shortslice
 to be converted from shortslice <= statperiod <= longslice
                   to  1  <= statperiod <= div_short * div_long
 shortslice = loSTATPERIOD / div_short
 longslice = loSTATPERIOD * div_long
 loSTATPERIOD ==> div_short

  bw0 (old bandwidth) = (bw0 + percent * statperiod) * div_short /
                           (statperiod + div_short)
  bw (current bandwidth) = bw0 / div_short

 to avoid overflows must be :
  (1 << 32) > 100 * div_short * (div_long + 1) * div_short

 thus: 7 bits for "100%"
      25 bits for least: 2 div_short + div_long
 for example div_short = div_long = 1 << 8
 or, for better resolution on short periods:
     div_short = 1 << 10, div_long = 1 << 5
 */
#if 0
{ unsigned long period;

 if (statperiod >= (loSTATPERIOD << 5)) period = 1 << 15;
 else if (0 == (period =
              ((statperiod << 10) + (loSTATPERIOD >> 1)) / loSTATPERIOD))
   period = 1;
 upt->bandwidth0 = (upt->bandwidth0 + percent * period) * (1 << 10);
#if 0
 upt->bandwidth0 /= (1 << 10) + period;
#else
 period += (1 << 10);
 upt->bandwidth0 = (upt->bandwidth0 + (period >> 1)) / period;
#endif
 upt->bandwidth = (upt->bandwidth0 + (1 << 9)) / (1 << 10);
}
#else
 if (statperiod >= (loSTATPERIOD << 1))
   upt->bandwidth0 = percent << 12;
 else
   {
    unsigned long period = ((statperiod << 11) +
                           (loSTATPERIOD >> 1)) / loSTATPERIOD;
    if (0 == period) period = 1;
    else if (period > (1 << 12)) period = 1 << 12;
#if 1
    upt->bandwidth0 -= (upt->bandwidth0 * period + (1 << 11)) / (1 << 12);
#else
    upt->bandwidth0 = (upt->bandwidth0 * ((1 << 12) - period) + (1 << 11)) / (1 << 12);
#endif
    upt->bandwidth0 += percent * period;
   }
 upt->bandwidth = (upt->bandwidth0 + (1 << 11)) / (1 << 12);
#endif
#endif

#if 0 == SATURATE_PERCENT
  if (upt->bandwidth > 100) upt->bandwidth = 100;
#endif

SkipBW:
 if (upt->bandwidth >= 2)
   {
    if (statperiod < loSTATPERIOD) statperiod = loSTATPERIOD;
    statperiod += statperiod >> 1;
    if (statperiod <= nextsleep) nextsleep = statperiod >> 1;
//       UL_WARNING((LOGID, "UPDATE %u%% -> %u", (int)upt->bandwidth, nextsleep));
   }
 if (loMAXSLEEP <  nextsleep &&
     loMAXSLEEP < (nextsleep >>= 1)) nextsleep = loMAXSLEEP;
/* if (nextsleep > 20000)
   {
    UL_WARNING((LOGID, "UPDATE %u", nextsleep));
   }*/
 upt->nextsleep = nextsleep;

 return;
}

DWORD lo_adjust_bandwidth(DWORD bandwidth, loMilliSec bandw_time, loMilliSec curtime)
{
 if (-1 != bandwidth)
   {
    curtime -= bandw_time;
    curtime -= loSTATPERIOD >> 4;
    if (curtime < ~curtime) /* no overflow */
      {
       curtime  +=  loSTATPERIOD;
       bandwidth = (loSTATPERIOD * bandwidth + (curtime >> 1)) / curtime;
      }
   }
 return bandwidth;
}

static void unadvise_all(LightOPCServer *cli);

/* Be careful looking on "#if LO_TIMEBACK": here are 4(!) 
   different handlig for this minor problem. 
In short words, under some circumance (e.g. runnning VDM)
the Win2k may produce delays that are shorter than requested.
For example, Sleep(20) may awake after 15ms. Then Sleep(5) 
returns immediately (i.e. there would be a few hundreds of Sleep(5) 
for real delay of 5ms). 

I don't know is it problem of Sleep() or WaitFor...() or GetTickCount().
Is it problem of waiting or rounding of system clocks...
Anyway we have a problem.

Basically we have to increase delay by the timer granularity 
on the next sleep. We have following choices:

LO_TIMEBACK == 0 : ignore problem. Call cond_wait(...,delay) till the 
time elapsed. It's a kind of empty loop with too much context switches.

Otherwise we can detect we have been awoked before requsted and no 
other events pending.

LO_TIMEBACK > 0 : run the _one_ empty update cycle and then increase
delay (round-up) for some (LO_TIMEBACK ms) time:
        a) rounding-up by nanosecond recalculation of timer granularity;
        b) simply add the granularity (millisecond accuracy) 
           to the NEXTSLEEP.

LO_TIMEBACK < 0 : assume we have awoken at the time; 
perform update cycle prematurely as if it just in time (round-down);
then increase NEXTSLEEP by the difference of the real and 
assumed time [TimeShift].

The last way minimizes average updaterate error.
*/

void LightOPCServer::client_scheduler(void)
{
#if LO_TIMEBACK > 0 /* round up */
 loMilliSec TimeBack = 0;
#endif
#if LO_TIMEBACK < 0 /* round down */
 loMilliSec TimeShift = 0;
#else
# define    TimeShift  (0)
#endif
 loUpdTimes upt;
 loDCL_ABSTIME(abst);
// if (!loIS_VALID(this)) return;
 UL_NOTICE((LOG_SR("thr_async(%p) started"), this));
 loThrControl_accept(&async);
 memset(&upt, 0, sizeof(upt));
 upt.nextsleep = loMAXSLEEP;
 lo_MILLISECABS(upt.finish, abst);

 lw_mutex_lock(&async.lk);

 if (async.tstate >= 0)
   {
#if LO_USE_BOTHMODEL
 if (0 == ole_initialized &&
     (/*se->driver.*/ldFlags & loDf_BOTHMODEL))
   {
    HRESULT oli = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (SUCCEEDED(oli)) ole_initialized = 1;
    else UL_WARNING((LOGID, "%!l client_scheduler(%p)::CoInitialize() FAILED",
                   oli, this));
   }
#endif

 for(;;)
   {
//if (async.state) UL_ERROR((LOG_SR("SHC  %d"), async.state));
#if 0
 UL_DEBUG((LOGID, "WA:%d %u + %ums =%u", 
   upt.datasent, bandwidth_time, upt.nextsleep, bandwidth_time + upt.nextsleep));
#endif
    if (0 == async.tstate && /* don't sleep when a request pending! */
        !(shuttingdown && shutdown_conn.request))
      {
#if LO_TIMEBACK > 0
       if (TimeBack)
         {
          if ((bandwidth_time - TimeBack) > LO_TIMEBACK) TimeBack = 0;
#if 1
          /* millisec round-up */
          lw_conds_timedwait(&async.cond, &async.lk, 
                             loABSREL(abst, upt.nextsleep + lo_timegran_ms));
          UL_DEBUG((LOGID, "WAIT AA: %u", lo_timegran_ms));
#else
          /* nanosec round-up */
          unsigned add_sleep;
          ULONGLONG upt_nextsleep = upt.nextsleep;
          upt_nextsleep *= 10000;
          if (add_sleep = (unsigned)(upt_nextsleep % lo_timegran_100ns))
            add_sleep = (lo_timegran_100ns - add_sleep + 9999) / 10000 & 0x0ff;
               /* add no more than 1/4 sec */
          lw_conds_timedwait(&async.cond, &async.lk, 
                             loABSREL(abst, upt.nextsleep add_sleep));
          UL_DEBUG((LOGID, "WAIT AA: %u", add_sleep));
#endif
         }
       else 
#endif /*LO_TIMEBACK > 0*/
       lw_conds_timedwait(&async.cond, &async.lk, 
                         loABSREL(abst, upt.nextsleep + TimeShift));
      }
    LO_HEAPCHECK(this);
    if (async.tstate < 0) break;
    lo_MILLISEC(upt.awoke);
#if 1
// if (bandwidth_time + upt.nextsleep >= upt.awoke)
   UL_DEBUG((LOGID, "WAIT WA:%d:%d %u + %ums =%u %+d", 
   upt.datasent, async.tstate, 
   bandwidth_time, upt.nextsleep, bandwidth_time + upt.nextsleep,   
   upt.awoke - (bandwidth_time + upt.nextsleep)));
#endif

    if (!async.tstate && !lo_NO_TIMEBACK &&
        upt.nextsleep > upt.awoke - bandwidth_time)
#if LO_TIMEBACK < 0
      {
        /* round down */
        UL_TRACE((LOGID, "WAITing problem was:%d now:%d", 
          TimeShift, upt.nextsleep - (upt.awoke - bandwidth_time)));
        TimeShift = upt.nextsleep - (upt.awoke - bandwidth_time);
        if (TimeShift > lo_timegran_ms) TimeShift = 0/*lo_timegran_ms*/;
        upt.awoke += TimeShift;
      }
    else TimeShift = 0;
#elif LO_TIMEBACK > 0
      {
        /* round up */
        TimeBack = upt.awoke;
        UL_TRACE((LOGID, "WAITing problem"));
      }
#else
    UL_WARNING((LOGID, "WAITing problem")); /* no solution */
#endif /*LO_TIMEBACK*/

    if (shuttingdown && shutdown_conn.request)
      {
       loShutdownConn sc = shutdown_conn;
       loShutdownConn_init(&shutdown_conn);
       lw_mutex_unlock(&async.lk);
       UL_NOTICE((LOG_SR("Going to IOPCShutdown")));
       loShutdownConn_call(&sc);
       lw_mutex_lock(&async.lk);
      }
    if (loThrControl_outof01(&async))
      {
       UL_DEBUG((LOG_SR("Going to shutdown (%p) ..."), this));
       loShutdownConn_clear(&shutdown_conn);
       lw_mutex_unlock(&async.lk); /* MUST be called outside of lock !!!*/
       UL_NOTICE((LOG_SR("Unadvising groups thr_async(%p) ..."), this));
       unadvise_all(this);
       otrk.ot_disconnect_all(0);
       clear_all();
       otrk.ot_disconnect_all(0);
       loQueueBcast_abort(&q_ret);
       UL_NOTICE((LOG_SR("Self terminating thr_async(%p) ..."), this));
       selfdestroy(); /* at this point <this> is no longer valid */
       UL_NOTICE((LOGID, "Self terminating thr_async(%p) Ok", this));
       return;
      }
/* on stopped we have to sleep
   on shutdown we have to sleep or to break */
    else
      {
       if (1 == async.tstate) /* no timeout occured */
         {
          async.tstate = 0; /* clear signal */
          if (q_req.req)
            {
             lo_rio_requests(this); /* Do requests first ==> low delay */
             if (loThrControl_outof01(&async)) continue;
            }
         }
       lw_mutex_unlock(&async.lk);

       upt.datasent = 0;
       upt.overload = 0;
       upt.updated = 0;
       upt.nextsleep = loHUGESLEEP;

       lo_MILLISEC(upt.beginupd);  upt.beginupd += TimeShift;
       update_sheduler(otrk.ot_stopped? 0: this, se, &upt);

       lw_mutex_lock(&async.lk);
#if 0
       if (q_req.req)
         {
          lo_rio_requests(this);  /* it does loThrControl_outof01(&async); */
         }
#endif
       if (upt.datasent) se->driver.ldCurrentTime(&ctxt.cactx, &ostatus.ftLastUpdateTime);

       lo_MILLISECABS(bandwidth_time, abst);  bandwidth_time += TimeShift;
       sched_timings(&upt, bandwidth_time);
       ostatus.dwBandWidth = /*stopped? (DWORD)-1: */(DWORD)upt.bandwidth;
      }
#if 0
    if (shuttingdown && upt.nextsleep > 4096) upt.nextsleep = 4096;
#endif
   } /* end of for(;;) */
 } /* if (async.tstate >= 0) */

{
#if LO_USE_BOTHMODEL
 int oleini = ole_initialized; ole_initialized = 0;
#endif

 loThrControl_finish(&async);
 lw_mutex_unlock(&async.lk);

 UL_NOTICE((LOGID, "thr_async(%p) finished", this));
#if LO_USE_BOTHMODEL
 if (oleini) CoUninitialize();
#endif
}
}

static void unadvise_all(LightOPCServer *cli)
{
 unsigned gndx;
 LightOPCGroup **grpl;

 cli->lock_read();
 gndx = cli->grl.gl_count;
 grpl = (LightOPCGroup**)cli->grl.gl_list;
 while(gndx)
   {
    LightOPCGroup *grp;
    if ((grp = grpl[--gndx]) && grp->advise_present)
      grp->clear_advise();
   }
 cli->unlock();
}
/* end of shedcl.cpp */
