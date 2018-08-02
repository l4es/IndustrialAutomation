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

#ifndef LWSYNCH_H
#define LWSYNCH_H

#if defined(LW_PTHREAD) && 0 != LW_PTHREAD
#include <pthread.h>
#else
#define LW_PTHREAD 0
#include <rwlock.h>
#include <condsb.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if     LW_PTHREAD

typedef pthread_mutex_t lw_mutex;
#define lw_mutex_init(lk,p)   pthread_mutex_init(lk,p)
#define lw_mutex_destroy      pthread_mutex_destroy
#define lw_mutex_lock         pthread_mutex_lock
#define lw_mutex_unlock       pthread_mutex_unlock

typedef struct { lw_mutex rmux; /* prevent from mixing [r]mutexes */
               } lw_rmutex; 

#ifndef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
int lw_rmutex_init(lw_rmutex *lk, void *p);
#else
#define lw_rmutex_init(lk,p)  (             \
  (lk)->rmux = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP, \
  lw_rmutex_lock(&(lk)->rmux), lw_rmutex_unlock(&(lk)->rmux) )
#endif
#define lw_rmutex_destroy(lk) lw_mutex_destroy(&(lk)->rmux)
#define lw_rmutex_lock(lk)    lw_mutex_lock(&(lk)->rmux)
#define lw_rmutex_unlock(lk)  lw_mutex_unlock(&(lk)->rmux)

typedef pthread_cond_t lw_conds;
#define lw_conds_init(cv,p)   pthread_cond_init(cv,p)
#define lw_conds_destroy      pthread_cond_destroy
#define lw_conds_wait         pthread_cond_wait
/* absolute timeout (cv,mt,abst)*/
#define lw_conds_timedwait    pthread_cond_timedwait
#define lw_conds_signal       pthread_cond_signal

typedef pthread_cond_t lw_condb;
#define lw_condb_init(cv,p)   pthread_cond_init(cv,p)
#define lw_condb_destroy      pthread_cond_destroy
#define lw_condb_wait         pthread_cond_wait
/* absolute timeout (cv,mt,abst)*/
#define lw_condb_timedwait    pthread_cond_timedwait
#define lw_condb_broadcast    pthread_cond_broadcast
#define lw_condb_broadcast_continious(cv,mt) ( (mt) \
       ?(lw_mutex_lock(mt), lw_condb_broadcast(cv), lw_mutex_unlock(mt)) \
       : lw_condb_broadcast(cv) )

typedef pthread_rwlock_t lw_rwlock;
#define lw_rwlock_init(rw,p)  pthread_rwlock_init(rw,p)
#define lw_rwlock_destroy     pthread_rwlock_destroy

#define lw_rw_rdlock          pthread_rwlock_rdlock
#define lw_rw_tryrdlock       pthread_rwlock_tryrdlock

#define lw_rw_wrlock          pthread_rwlock_wrlock
#define lw_rw_trywrlock       pthread_rwlock_trywrlock
#define lw_rw_unlock          pthread_rwlock_unlock
/* absolute timeout (cv,mt,abst)*/
#define lw_rw_timedrdlock     pthread_rwlock_timedrdlock
#define lw_rw_timedwrlock     pthread_rwlock_timedwrlock

typedef pthread_t lw_thread;

#define lw_thrcreate      pthread_create
#define lw_thrisself(thr) pthread_equal((thr),pthread_self())
#define lw_threxit        pthread_exit
#define lw_thrdetach      pthread_detach
#define lw_thrjoin        pthread_join

#else   /*LW_PTHREAD*/

typedef mutex_t lw_mutex;
#define lw_mutex_init(lk,p)   mutex_init(lk,p,0)
#define lw_mutex_destroy      mutex_destroy
#define lw_mutex_lock         mutex_lock
#define lw_mutex_unlock       mutex_unlock

typedef struct { mutex_t rmux; /* prevent from mixing [r]mutexes */
               } lw_rmutex; 
#define lw_rmutex_init(lk,p)  mutex_init(&(lk)->rmux,p,0)
#define lw_rmutex_destroy(lk) mutex_destroy(&(lk)->rmux)
#define lw_rmutex_lock(lk)    mutex_lock(&(lk)->rmux)
#define lw_rmutex_unlock(lk)  mutex_unlock(&(lk)->rmux)

typedef conds_t lw_conds;
#define lw_conds_init(cv,p)   conds_init(cv,p,0)
#define lw_conds_destroy      conds_destroy
#define lw_conds_wait         conds_wait
/* relative timeout (cv,mt,rel)*/
#define lw_conds_timedwait    conds_timedwait
#define lw_conds_signal(cv)   conds_signal((cv),0)

typedef condb_t lw_condb;
#define lw_condb_init(cv,p)   condb_init(cv,p,0)
#define lw_condb_destroy      condb_destroy
#define lw_condb_wait         condb_wait
/* relative timeout (cv,mt,rel)*/
#define lw_condb_timedwait    condb_timedwait
#define lw_condb_broadcast(cv) condb_broadcast((cv),0)
#define lw_condb_broadcast_continious(cv,mt) condb_broadcast_continious((cv),(mt))

#define lw_conds_timedwait_lml conds_timedwait_lml
#define lw_condb_timedwait_lml condb_timedwait_lml

typedef rwlock_t lw_rwlock;
#define lw_rwlock_init(rw,p)  rwlock_init(rw,p,0)
#define lw_rwlock_destroy     rwlock_destroy

#define lw_rw_rdlock          rw_rdlock
#define lw_rw_tryrdlock       rw_tryrdlock

#define lw_rw_wrlock          rw_wrlock
#define lw_rw_trywrlock       rw_trywrlock
#define lw_rw_unlock          rw_unlock
/* relative timeout */
#define lw_rw_timedrdlock     rw_timed_rdlock
#define lw_rw_timedwrlock     rw_timed_wrlock

#define lw_rw_timedrdlock_lml rw_timed_rdlock_lml
#define lw_rw_timedwrlock_lml rw_timed_wrlock_lml

/**********************************************************************************/

typedef struct lw_thread
    {
     unsigned lw_thrid;
     HANDLE   lw_thrhandle;
    } lw_thread;

int lw_thrcreate(lw_thread *thr, void *null, void *(*thread)(void *), void *arg);

int lw_thrisself(lw_thread thr);
#define lw_thrisself(thr)   ((thr).lw_thrid == GetCurrentThreadId())

int lw_threxit(void *ret);
#define lw_threxit(ret)     (_endthreadex((unsigned)(ret)))

int lw_thrdetach_(lw_thread *thr);
int lw_thrjoin_(lw_thread *thr, void **ret);
#define lw_thrdetach(thr)   lw_thrdetach_(&(thr))
#define lw_thrjoin(thr,ret) lw_thrjoin_(&(thr), (ret))

#endif /*LW_PTHREAD*/

typedef void *lw_thrrettype;
#define lw_RETURN return 0

#ifdef __cplusplus
	}
#endif

#endif /*LWSYNCH_H*/
