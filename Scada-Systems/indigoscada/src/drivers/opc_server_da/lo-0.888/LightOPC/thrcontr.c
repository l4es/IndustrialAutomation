/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
                                                                          *
 thread start/stop/signal
 **************************************************************************/

#include <errno.h>
#include <process.h>
#include "loserv.h"

int loThrControl_init(loThrControl *tc)
{
 int rv;

 if (!tc) return EINVAL;
 tc->tstate = 0;
 tc->thr_run = 0;
 tc->tf = 0;
 if (rv = lw_mutex_init(&tc->lk, 0))
   {
    UL_NOTICE((LOGID, "loThrInit() lw_mutex FAILED"));
   }
 else if (rv = lw_conds_init(&tc->cond, 0))
   {
    UL_NOTICE((LOGID, "loThrInit() condition FAILED"));
    lw_mutex_destroy(&tc->lk);
   }

 return rv;
}

int loThrControl_destroy(loThrControl *tc)
{
 if (!tc) return EINVAL;
 loThrControl_stop(tc);
 lw_mutex_destroy(&tc->lk);
 lw_conds_destroy(&tc->cond);
 return 0;
}

#define loThrID(thr) (*((void**)&(thr)))

int loThrControl_start(loThrControl *tc, int uselock,
                       lw_thrrettype (*tf)(void*), void *targ)
{
 int rv = 0;

 if (!tc || !tf) return EINVAL;
 if (tc->thr_run != 0) return EEXIST;
 if (uselock) lw_mutex_lock(&tc->lk);
 if (tc->thr_run != 0) rv = EEXIST;
 else
   {
    tc->tf = tf;
    tc->ta = targ;
    /*if (0 > tc->state)*/ tc->tstate = 0;
    tc->thr_run = 1;
    rv = lw_thrcreate(&tc->thr, 0, tf, targ);
    if (rv)
      {
       tc->thr_run = 0;
       tc->tf = 0;
      }
   }
 if (uselock) lw_mutex_unlock(&tc->lk);
 if (rv)
   {
    UL_NOTICE((LOGID, "%!e loThrStart() _beginthread(%p) FAILED", 
                rv, tf));
   }
 else
   {
    UL_TRACE((LOGID, "loThrStart(%p) started %p", 
              loThrID(tc->thr), tf));
   }

 return rv;
}

int loThrControl_stop(loThrControl *tc)
{
 lw_thrrettype (*tf)(void*);

 if (!tc || !tc->tf) return EINVAL;

 lw_mutex_lock(&tc->lk);
 if (!(tf = tc->tf))
   {
    lw_mutex_unlock(&tc->lk);
    UL_WARNING((LOGID, "loThrStop(%p) possible deadlock", loThrID(tc->thr)));
    return EDEADLK;
   }
 tc->tf = 0;

 if (0 != tc->thr_run)
   {
    int rv;
    tc->tstate = -1;
    if (lw_thrisself(tc->thr))
      {
       lw_mutex_unlock(&tc->lk);
       UL_NOTICE((LOGID, "loThrStop(%p) trying to terminate itself (r%d)", 
                  loThrID(tc->thr), tc->thr_run));
       tf(tc->ta);
       UL_TRACE((LOGID, "loThrStop(%p) selfterminate Ok (r%d)", 
                  loThrID(tc->thr), tc->thr_run));
       rv = lw_thrdetach(tc->thr);
       if (rv)
         UL_MESSAGE((LOGID, "%!e loThrStop(%p) thrDetach() failed", 
                     rv, loThrID(tc->thr)));
       else UL_TRACE((LOGID, "loThrStop(%p) Detached Ok", 
                    loThrID(tc->thr)));
      }
    else
      {
       lw_conds_signal(&tc->cond);
       lw_mutex_unlock(&tc->lk);
       UL_DEBUG((LOGID, "loThrStop(%p) Wait() r%d...", 
                loThrID(tc->thr), tc->thr_run));
       rv = lw_thrjoin(tc->thr, 0);
       if (rv)
         {
          UL_MESSAGE((LOGID, "%!e loThrStop(%p) thrJoin() failed r%d ", 
                      rv, loThrID(tc->thr), tc->thr_run));
         }
       else
         {
          UL_TRACE((LOGID, "loThrStop(%p) Wait() Finished r%d", 
                    loThrID(tc->thr), tc->thr_run));
         }
      }
     tc->thr_run = 0;
    }
  else
    {
     lw_mutex_unlock(&tc->lk);
     UL_TRACE((LOGID, "loThrStop(%p) already stopped r%d", 
            loThrID(tc->thr), tc->thr_run));
    }
 tc->tf = tf;
 return 0;
}

/* end of thrcontr.c */
