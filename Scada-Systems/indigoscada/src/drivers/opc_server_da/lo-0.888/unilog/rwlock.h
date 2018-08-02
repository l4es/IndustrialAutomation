/**********************  Thread synchronization  ********************
 ***                        read/write locks                      ***
 *                                                                  *
 **                 Copyright (c) 1996,2000 by Timofei Bondarenko  **
 *                                                                  *
 * Requests are NOT serialized properly                             *
 * but write request has a chance even in unfair schedulig          *
 ********************************************************************/
#ifndef  RWLOCK_H
#define  RWLOCK_H
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct rwlock_t
       {
        CRITICAL_SECTION rw_mutex;
        HANDLE   rw_write_allowed;
        HANDLE   rw_read_allowed;
        unsigned rw_readers;
        unsigned rw_write_pending;
        char     rw_write_active;
        char     rw_read_stopped,
                 rw_write_stopped;
       } rwlock_t;

int rwlock_init(rwlock_t *rwlock, int nul, void *null);
int rwlock_destroy(rwlock_t *rwlock);

int rw_rdlock(rwlock_t *lk);
int rw_tryrdlock(rwlock_t *lk);
int rw_wrlock(rwlock_t *lk);
int rw_trywrlock(rwlock_t *lk);
int rw_unlock(rwlock_t *lk);

int rw_timed_rdlock(rwlock_t *lk, DWORD timeout); /* internal */
int rw_timed_wrlock(rwlock_t *lk, DWORD timeout); /* internal */

#define rw_rdlock(lk)       rw_timed_rdlock((lk), INFINITE)
#define rw_tryrdlock(lk)    rw_timed_rdlock((lk), 0)
#define rw_wrlock(lk)       rw_timed_wrlock((lk), INFINITE)
#define rw_trywrlock(lk)    rw_timed_wrlock((lk), 0)

#ifndef ETIME
#define ETIME EBUSY
#endif

   /* with local message loop for singlethreaded ole: */
int rw_timed_rdlock_lml(rwlock_t *lk, DWORD timeout);
int rw_timed_wrlock_lml(rwlock_t *lk, DWORD timeout);
#define rw_rdlock_lml(lk)       rw_timed_rdlock_lml((lk), INFINITE)
#define rw_wrlock_lml(lk)       rw_timed_wrlock_lml((lk), INFINITE)

#ifdef __cplusplus
           }
#endif
#endif /*RWLOCK_H*/
