/**************************************************************************
 *                                                                        *
 * Light-Weight Locks for Windows		                                  *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
                                                                          *
  Various Synchronization wrappers & procedures in spirit of SysV.
 
  Define LW_PTHREAD=1 to map lw_* wrappers to a PTHREADs implementation.
  Otherwise internal Win32 implementation being used.
 **************************************************************************/
#include "lwsynch.h"

#if 0 == LW_PTHREAD

#include <stdlib.h>
#include <process.h>
#include <errno.h>

#ifndef LW_malloc 
#define LW_malloc	malloc
#define LW_free		free
#endif

struct th_call_a
  {
   void *(*th_func)(void *);
   void *th_arg;
  };

static unsigned __stdcall th_call_f(void *arg)
{
 struct th_call_a tcalla = *((struct th_call_a*)arg);
 LW_free(arg);
 return (unsigned)tcalla.th_func(tcalla.th_arg);
}

int lw_thrcreate(lw_thread *thr, void *null,
                 lw_thrrettype (*thread)(void *), void *arg)
{
 struct th_call_a *tcalla;

 if (!thr) return EINVAL;
 if (!(tcalla = (struct th_call_a*)LW_malloc(sizeof(*tcalla))))
   return ENOMEM;
 tcalla->th_func = thread;
 tcalla->th_arg = arg;

 thr->lw_thrhandle = (HANDLE)_beginthreadex(0/*security*/, 0/*stacksize*/,
                                            th_call_f, tcalla, 0/*flags*/,
                                            &thr->lw_thrid);
 if (thr->lw_thrhandle) return 0; /*Ok*/
 LW_free(tcalla);
 return EAGAIN/*errno*/;
}

int lw_thrjoin_(lw_thread *thr, void **ret)
{
 int rv = 0;
 HANDLE th;

 if (!thr || lw_thrisself(*thr)) return EDEADLK;
 th = (HANDLE)InterlockedExchange((LONG*)&thr->lw_thrhandle, 0);

 if (WAIT_OBJECT_0 != WaitForSingleObject(th, INFINITE))
   rv = ESRCH;
 else if (ret)
   {
    DWORD rc = (DWORD)*ret;
    GetExitCodeThread(th, &rc);
    *ret = (void*)rc;
   }
 if (!CloseHandle(th)) rv = EINVAL;
 return rv;
}

int lw_thrdetach_(lw_thread *thr)
{
 HANDLE th;
 th = (HANDLE)InterlockedExchange((LONG*)&thr->lw_thrhandle, 0);
 return CloseHandle(th)? 0: EINVAL;
}

#undef UL_ERROR
#undef UL_WARNING
#include "rwlock.c"
#undef UL_ERROR
#undef UL_WARNING
#include "condsb.c"

#else /* LW_PTHREAD */

#ifndef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP

#include <errno.h>

int lw_rmutex_init(lw_rmutex *lk, void *p)
{
 int rv = EINVAL;
 pthread_mutexattr_t attr;
 if (lk)
   {
    rv = pthread_mutexattr_init(&attr); 
    if (!rv)
      {
       rv = pthread_mutexattr_setkind_np(&attr, PTHREAD_MUTEX_RECURSIVE_NP); 
       if (!rv) rv = pthread_mutex_init(&lk->rmux, &attr);
      }
    pthread_mutexattr_destroy(&attr); 
   }

 return rv;
}

#endif
#endif

/* end of lwsynch.c */
