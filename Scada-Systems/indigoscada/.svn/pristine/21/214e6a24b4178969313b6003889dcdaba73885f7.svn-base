/**********************  Thread synchronization  ********************
 **                         read/write locks                       **
 *                                                                  *
 *                    Copyright (c) 1996,2000 by Timofei Bondarenko *
 ********************************************************************/
#include <errno.h>
#include "rwlock.h"

#if 0
#include <unilog.h>
#define LOGID 0,0
#else
#define UL_ERROR(x)
#define UL_WARNING(x)
#endif

#define MUTEX_LOCK(mutex)    (EnterCriticalSection(&(mutex)))
#define MUTEX_UNLOCK(mutex)  (LeaveCriticalSection(&(mutex)))

#ifndef ETIME
#define ETIME EBUSY
#endif

#if 0
#define CK_LOCK(lk)  if (!(lk)) return EINVAL;
#else
#define CK_LOCK(lk)
#endif

int rwlock_init(rwlock_t *lk, int nul, void *null)
{
 if (!lk) return EINVAL;
#if 0
 lk->rw_write_stopped = 0;
 lk->rw_read_stopped = 0;
 lk->rw_write_active = 0;
 lk->rw_write_pending = 0;
 lk->rw_readers = 0;
 lk->rw_read_allowed = 0;
 lk->rw_write_allowed = 0;
#else
 memset(lk, 0, sizeof(*lk));
#endif
 InitializeCriticalSection(&lk->rw_mutex);
 if (!(lk->rw_write_allowed =
      CreateEvent(NULL,    /* security */
                  FALSE/*TRUE*/,    /* bManualReset */
                  TRUE,    /* bInitialState */
                  NULL)    /* name */
     ) ||
     !(lk->rw_read_allowed =
      CreateEvent(NULL,    /* security */
                  TRUE,    /* bManualReset */
                  TRUE,    /* bInitialState */
                  NULL)    /* name */
     ) )
   {
    rwlock_destroy(lk); return EMFILE;
   }
 return 0;
}

int rwlock_destroy(rwlock_t *lk)
{
 int rv = 0;
 if (!lk) return EINVAL;
 MUTEX_LOCK(lk->rw_mutex);
 if (lk->rw_read_allowed)
   {
    HANDLE th = lk->rw_read_allowed;
    lk->rw_read_allowed = NULL;
    if (FALSE == CloseHandle(th)) rv = EBADF;
   }
 if (lk->rw_write_allowed)
   {
    HANDLE th = lk->rw_write_allowed;
    lk->rw_write_allowed = NULL;
    if (FALSE == CloseHandle(th)) rv = EBADF;
   }
 MUTEX_UNLOCK(lk->rw_mutex);
 DeleteCriticalSection(&lk->rw_mutex);
#if 0
 lk->rw_write_active = 0;
 lk->rw_write_pending = 0;
 lk->rw_readers = 0;
 lk->rw_read_stopped = 0; /*???*/
 lk->rw_write_stopped = 0; /*???*/
#else
 //memset(lk, 0, sizeof(*lk));
#endif
 return rv;
}

int rw_timed_rdlock(rwlock_t *lk, DWORD timeout)
{
 int rv = 0;

 CK_LOCK(lk);

 UL_WARNING((LOGID, "RdLock...%p", lk));

 for(;;)
   {
    DWORD ws;
    MUTEX_LOCK(lk->rw_mutex);
// UL_WARNING((LOGID, "RdLock->%p", lk));

    if (0 == lk->rw_write_active && 0 == lk->rw_write_pending)
      {
      /* Let's writers stop itself. That is better than make
         one more syscall by reader */
       lk->rw_readers++;
       MUTEX_UNLOCK(lk->rw_mutex);
#if 0
       if (0 == rw_trywrlock(lk))
         UL_ERROR((LOGID, "RdLock FAILED"));
#endif
       break;
      }
#if 1
    if (0 != timeout &&
        0 == lk->rw_read_stopped)
      {
       lk->rw_read_stopped = 1;
       ResetEvent(lk->rw_read_allowed);
      }
#endif
    MUTEX_UNLOCK(lk->rw_mutex);
//    UL_ERROR((LOGID, "RdLock ..."));
    if (0 == timeout ||
        WAIT_TIMEOUT == (ws =
         WaitForSingleObject(lk->rw_read_allowed, timeout)))
      { rv = ETIME;  break; }
    if (WAIT_FAILED == ws)
      { rv = EINVAL; break; }
//    timeout = 0;
   }
 UL_WARNING((LOGID, "...RdLock %p = %d", lk, rv));

 return rv;
}

int rw_timed_wrlock(rwlock_t *lk, DWORD timeout)
{
 int rv = 0;

 CK_LOCK(lk);
 /* here is the place for optimization:
   1) incrementing of rw_write_pending grants priority to writers
   2) ResetEvent(read/write/allowed) can be performed in different places
  */
 UL_WARNING((LOGID, "WrLock...%p", lk));
 MUTEX_LOCK(lk->rw_mutex);
#if 1
 lk->rw_write_pending++;
#else
 if (0 == lk->rw_write_pending++ &&
     0 == lk->rw_read_stopped)
   {
    lk->rw_read_stopped = 1;
    ResetEvent(lk->rw_read_allowed);
   }
#endif

 for(;;)
   {
    DWORD ws;

// UL_WARNING((LOGID, "WrLock->%p", lk));
//    ResetEvent(lk->rw_write_allowed);
    if (0 == lk->rw_write_active && 0 == lk->rw_readers)
      {
//       ResetEvent(lk->rw_write_allowed);
       lk->rw_write_active = 1;
       lk->rw_write_pending--;
#if 0
 MUTEX_UNLOCK(lk->rw_mutex);
       if (0 == rw_trywrlock(lk) || 0 == rw_tryrdlock(lk))
         UL_ERROR((LOGID, "WrLock FAILED"));
       UL_WARNING((LOGID, "WrLock %p = 00", lk));
       return 0;
#endif
       goto Finish;
      }
    if (0 == timeout) { rv = ETIME;  break; }
    if (0 == lk->rw_write_stopped)
      {
       lk->rw_write_stopped = 1;
       ResetEvent(lk->rw_write_allowed);
      }
    MUTEX_UNLOCK(lk->rw_mutex);
// UL_ERROR((LOGID, "WrLock ..."));
    ws = WaitForSingleObject(lk->rw_write_allowed, timeout);
    MUTEX_LOCK(lk->rw_mutex);
    if (WAIT_TIMEOUT == ws) { rv = ETIME;  break; }
    if (WAIT_FAILED == ws)  { rv = EINVAL; break; }
   }
 if (0 == --lk->rw_write_pending &&
     0 ==   lk->rw_write_active &&
     0 !=   lk->rw_read_stopped)
   {
    lk->rw_read_stopped = 0;
    SetEvent(lk->rw_read_allowed);
   }
Finish:
 MUTEX_UNLOCK(lk->rw_mutex);
 UL_WARNING((LOGID, "WrLock %p = %d", lk, rv));
 return rv;
}

int rw_unlock(rwlock_t *lk)
{
 CK_LOCK(lk);
 UL_ERROR((LOGID, "UnLock..."));
 MUTEX_LOCK(lk->rw_mutex);
 if (lk->rw_write_active)
   {
    lk->rw_write_active = 0;
    if (0 == lk->rw_write_pending &&
        0 != lk->rw_read_stopped)
      {
       lk->rw_read_stopped = 0;
       SetEvent(lk->rw_read_allowed);
      }
    if (0 != lk->rw_write_stopped)
      {
       lk->rw_write_stopped = 0;
       SetEvent(lk->rw_write_allowed);
      }
   }
 else
   {
    if (0 == --lk->rw_readers &&
        0 != lk->rw_write_pending &&
        0 != lk->rw_write_stopped)
      {
       lk->rw_write_stopped = 0;
       SetEvent(lk->rw_write_allowed);
      }
   }
 MUTEX_UNLOCK(lk->rw_mutex);
 UL_WARNING((LOGID, "...UnLock %p = 0", lk));
 return 0;
}

/************ with local message loop ***************/
#if 1

static void local_message_loop(void)
{
 MSG msg;
 UL_WARNING((LOGID, "LmL start..."));
  while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
     DispatchMessage(&msg);
 UL_WARNING((LOGID, "LmL complete"));
}

/* 
QS_ALLEVENTS An input, WM_TIMER, WM_PAINT, WM_HOTKEY, or posted message is in the queue. 
QS_ALLINPUT Any message is in the queue. 
QS_ALLPOSTMESSAGE A posted message (other than those listed here) is in the queue.  
QS_HOTKEY A WM_HOTKEY message is in the queue. 
QS_INPUT An input message is in the queue. 
QS_KEY A WM_KEYUP, WM_KEYDOWN, WM_SYSKEYUP, or WM_SYSKEYDOWN message is in the queue. 
QS_MOUSE A WM_MOUSEMOVE message or mouse-button message (WM_LBUTTONUP, WM_RBUTTONDOWN, and so on). 
QS_MOUSEBUTTON A mouse-button message (WM_LBUTTONUP, WM_RBUTTONDOWN, and so on). 
QS_MOUSEMOVE A WM_MOUSEMOVE message is in the queue. 
QS_PAINT A WM_PAINT message is in the queue. 
QS_POSTMESSAGE A posted message (other than those just listed) is in the queue. 
QS_SENDMESSAGE A message sent by another thread or application is in the queue. 
QS_TIMER 
*/
#define lkQS_MODE QS_ALLINPUT

int rw_timed_rdlock_lml(rwlock_t *lk, DWORD timeout)
{
 int rv = 0;
 DWORD ws = WAIT_OBJECT_0;

 CK_LOCK(lk);

 UL_WARNING((LOGID, "RdLock lml...%p", lk));

 for(;;)
   {    
    MUTEX_LOCK(lk->rw_mutex);
// UL_WARNING((LOGID, "RdLock->%p", lk));
    
    if (0 == lk->rw_write_active && 0 == lk->rw_write_pending)
      {
      /* Let's writers stop itself. That is better than make
         one more syscall by reader */
       lk->rw_readers++;
       MUTEX_UNLOCK(lk->rw_mutex);
       break;
      }
    if (0 != timeout &&
        0 == lk->rw_read_stopped)
      {
       lk->rw_read_stopped = 1;
       ResetEvent(lk->rw_read_allowed);
      }
    MUTEX_UNLOCK(lk->rw_mutex);

    if (ws == WAIT_OBJECT_0 + 1) 
      {
       local_message_loop(); ws = WAIT_OBJECT_0; continue;
      }
    if (0 == timeout ||
        WAIT_TIMEOUT == (ws =
           MsgWaitForMultipleObjects(1, &lk->rw_read_allowed, 0, timeout, lkQS_MODE)))
      { rv = ETIME;  break; }
    if (WAIT_FAILED == ws)
      { rv = EINVAL; break; }
//    timeout = 0;
   }
 UL_WARNING((LOGID, "...RdLock lml %p = %d", lk, rv));

 return rv;
}

int rw_timed_wrlock_lml(rwlock_t *lk, DWORD timeout)
{
 int rv = 0;
 DWORD ws = WAIT_OBJECT_0;
 
 CK_LOCK(lk);
 /* here is the place for optimization:
   1) incrementing of rw_write_pending grants priority to writers
   2) ResetEvent(read/write/allowed) can be performed in different places
  */
 UL_WARNING((LOGID, "WrLock lml...%p", lk));

 MUTEX_LOCK(lk->rw_mutex);
 for(;;)
   {
    if (0 == lk->rw_write_active && 0 == lk->rw_readers)
      {
//       ResetEvent(lk->rw_write_allowed);
       lk->rw_write_active = 1;
       goto Finish;
      }
    if (0 == timeout) { rv = ETIME;  break; }
    if (0 == lk->rw_write_stopped)
      {
       lk->rw_write_stopped = 1;
       ResetEvent(lk->rw_write_allowed);
      }
    if (ws == WAIT_OBJECT_0 + 1) 
      {
       MUTEX_UNLOCK(lk->rw_mutex);
       local_message_loop(); ws = WAIT_OBJECT_0; 
       MUTEX_LOCK(lk->rw_mutex);
       continue;
      }

    lk->rw_write_pending++;
     MUTEX_UNLOCK(lk->rw_mutex);
      ws = MsgWaitForMultipleObjects(1, &lk->rw_write_allowed, 0, timeout, lkQS_MODE);
     MUTEX_LOCK(lk->rw_mutex);
    lk->rw_write_pending--;
    if (WAIT_TIMEOUT == ws) { rv = ETIME;  break; }
    if (WAIT_FAILED == ws)  { rv = EINVAL; break; }
   }
 if (0 == lk->rw_write_pending &&
     0 == lk->rw_write_active &&
     0 != lk->rw_read_stopped)
   {
    lk->rw_read_stopped = 0;
    SetEvent(lk->rw_read_allowed);
   }
Finish:
 MUTEX_UNLOCK(lk->rw_mutex);
 UL_WARNING((LOGID, "WrLock lml %p = %d", lk, rv));
 return rv;
}

#endif
/* end of rwlock.c */


