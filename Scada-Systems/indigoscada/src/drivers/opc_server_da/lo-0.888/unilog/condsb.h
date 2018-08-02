/*********************  Thread  synchronization  ********************
 ***                     conditional variables                    ***
 *                                                                  *
 **                      Copyright (c) 2001 by Timofei Bondarenko  **
 *                                                                  *
 * NOTES:                                                           *
 *   a) The SAME mutex must be used in all calls to a condition;    *
 *   b) Signaling/Bcasting must be called under the same mutex too. *
 *                                                                  *
 ********************************************************************/
#ifndef CONDSB_H
#define CONDSB_H
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef CRITICAL_SECTION mutex_t;

int mutex_init(mutex_t *lk, int nul, void *null);
int mutex_destroy(mutex_t *lk);

int mutex_lock(mutex_t *lk);
int mutex_unlock(mutex_t *lk);

#define mutex_init(lk,i,p)  (InitializeCriticalSection(lk),0)
#define mutex_destroy(lk)   (DeleteCriticalSection(lk),0)
#define mutex_lock(lk)      (EnterCriticalSection(lk),0)
#define mutex_unlock(lk)    (LeaveCriticalSection(lk),0)

#ifndef ETIME
#define ETIME EBUSY
#endif

/****************** signaling condition *******************/

typedef struct conds_t
       {
        HANDLE           cs_event;
//        LONG             cs_waiters;
       } conds_t;

int conds_init(conds_t *, int nul, void *null);
int conds_destroy(conds_t *);
int conds_wait(conds_t *, mutex_t *mutex);
int conds_timedwait(conds_t *, mutex_t *mutex, DWORD timeout);
int conds_signal(conds_t *, mutex_t *mutex);
/* For conds_signal() the mutex argument is optional:
   if it != 0 this function will lock & unlock it;
   if it == 0 this function assume it already locked. */

#define conds_init(cv,nul,null)    (((cv)->cs_event = \
        CreateEvent(0, 0/*auto=signal*/, 0/*non-signaled*/, 0))? 0: -1)

#define conds_destroy(cv) (CloseHandle((cv)->cs_event)? 0: -1)

#define conds_timedwait(cv,mt,to) ((void)(/*ResetEvent((cv)->evs),prevents from self-signal*/ \
                                   mutex_unlock(mt), \
                                   WaitForSingleObject((cv)->cs_event,(to)), \
                                   mutex_lock(mt)))
/* The macro doesn't return proper value */

#define conds_wait(cv,mt)    conds_timedwait(cv,mt,INFINITE)

#define conds_signal(cv,mt)  (SetEvent((cv)->cs_event)? 0: -1)


/****************** broadcasting condition *******************/

typedef struct condb_t
       {
//        CRITICAL_SECTION cb_mutex;
        HANDLE           cb_event;
        HANDLE           cb_complete;
        LONG             cb_waiters;
       } condb_t; /* signal allowed */

int condb_init(condb_t *, int nul, void *null);
int condb_destroy(condb_t *);
int condb_wait(condb_t *, mutex_t *mutex);
int condb_timedwait(condb_t *, mutex_t *mutex, DWORD timeout);
int condb_signal(condb_t *, mutex_t *mutex);
int condb_broadcast(condb_t *, mutex_t *mutex);
#define condb_wait(cv,mt)   condb_timedwait((cv),(mt),INFINITE)
/* For condb_broadcast() the mutex argument is optional:
   if it != 0 this function will lock & unlock it;
   if it == 0 this function assume it already locked. */
int condb_broadcast_continious(condb_t *, mutex_t *mutex);

int condb_timedwait_lml(condb_t *lk, mutex_t *mutex, DWORD timeout);
   /* with local message loop for singlethreaded ole */

#define condb_broadcast_continious(cv,mt) (SetEvent((cv)->cb_event)? 0: -1)

#ifdef __cplusplus
	}
#endif

#endif /*CONDSB_H*/
