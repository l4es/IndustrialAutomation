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
#include <errno.h>
#include "condsb.h"

#if 0
#include <unilog.h>
#define LOGID 0,0
#else
#define UL_ERROR(x)
#define UL_WARNING(x)
#endif

#ifndef ETIME
#define ETIME EBUSY
#endif

#if 0
#define CK_LOCK(lk)     if (!(lk)) return EINVAL;
#define CK_LOCKm(lk,mt) if (!(lk) || !(mt)) return EINVAL;
#else
#define CK_LOCK(lk)
#define CK_LOCKm(lk,mt)
#endif

/****************** signaling condition *******************/
int (conds_timedwait)(conds_t *lk, mutex_t *mt, DWORD to)
{
 DWORD rv;
 CK_LOCKm(lk, mt);
 mutex_unlock(mt);
 rv = WaitForSingleObject(lk->cs_event, to);
 mutex_lock(mt);
 switch(rv)
   {
 case WAIT_OBJECT_0: return 0;
 case WAIT_TIMEOUT:  return ETIME;
   }
 return EBADF;
}

/****************** broadcasting condition *******************/

int condb_init(condb_t *lk, int nul, void *null)
{
 if (!lk) return EINVAL;
#if 0
 lk->cb_waiters = 0;
 lk->cb_event = 0;
 lk->cb_complete = 0;
#else
 memset(lk, 0, sizeof(*lk));
#endif
 if (!(lk->cb_event =
      CreateEvent(NULL,    /* security */
                  TRUE,    /* bManualReset */
                  FALSE,   /* bInitialState */
                  NULL)    /* name */
     ) ||
     !(lk->cb_complete =
      CreateEvent(NULL,    /* security */
                  FALSE,   /* bManualReset */
                  FALSE,   /* bInitialState */
                  NULL)    /* name */
     ) )
   {
    condb_destroy(lk); return EMFILE;
   }
 return 0;
}

int condb_destroy(condb_t *lk)
{
 int rv = 0;
 if (!lk) return EINVAL;
 if (lk->cb_event)
   {
    HANDLE th = lk->cb_event;
    lk->cb_event = NULL;
    if (FALSE == CloseHandle(th)) rv = EBADF;
   }
 if (lk->cb_complete)
   {
    HANDLE th = lk->cb_complete;
    lk->cb_complete = NULL;
    if (FALSE == CloseHandle(th)) rv = EBADF;
   }
#if 0
 lk->cb_waiters = 0; 
 lk->cb_event = 0;
 lk->cb_complete = 0;
#else
 //memset(lk, 0, sizeof(*lk));
#endif
 return rv;
}

int condb_timedwait(condb_t *lk, mutex_t *mutex, DWORD timeout)
{
 DWORD ws;
 int rv = 0;

 CK_LOCKm(lk, mutex);

 InterlockedIncrement(&lk->cb_waiters);
 mutex_unlock(mutex);
 ws = WaitForSingleObject(lk->cb_event, timeout);
 if (0 > InterlockedDecrement(&lk->cb_waiters))
   {
    SetEvent(lk->cb_complete);
   }
 else if (WAIT_TIMEOUT == ws) rv = ETIME;
 else if (WAIT_FAILED == ws) rv = EINVAL; 
 mutex_lock(mutex);

 return rv;
}

int condb_broadcast(condb_t *lk, mutex_t *mutex)
{
 int rv = 0;

 CK_LOCK(lk);

 if (mutex) mutex_lock(mutex);
 if (0 <= InterlockedDecrement(&lk->cb_waiters))
   {
    SetEvent(lk->cb_event);
    if (WAIT_FAILED == WaitForSingleObject(lk->cb_complete, INFINITE))
      rv = EINVAL;
    ResetEvent(lk->cb_event);
   }
 InterlockedIncrement(&lk->cb_waiters); /* == 0 */
 if (mutex) mutex_unlock(mutex);

 return rv;
}

/************ with local message loop ***************/
#if 1

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

int condb_timedwait_lml(condb_t *lk, mutex_t *mutex, DWORD timeout)
{
 DWORD ws;
 int rv = 0;

 CK_LOCKm(lk, mutex);
 UL_WARNING((LOGID, "Condb wait lml...%p", lk));

 InterlockedIncrement(&lk->cb_waiters);
 mutex_unlock(mutex);
 ws = MsgWaitForMultipleObjects(1, &lk->cb_event, 0, timeout, lkQS_MODE);
 if (0 > InterlockedDecrement(&lk->cb_waiters))
   {
    SetEvent(lk->cb_complete);
   }
 else if (WAIT_TIMEOUT == ws) rv = ETIME;
 else if (WAIT_FAILED == ws) rv = EINVAL; 
 else if (WAIT_OBJECT_0 + 1 == ws)
   {
    MSG msg;
 UL_ERROR((LOGID, "Condb wait DO lml...", lk));
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
       DispatchMessage(&msg);
   }
 mutex_lock(mutex);

 UL_WARNING((LOGID, "Condb wait lml...%p Ok", lk));
 return rv;
}

#endif
/* end of condsb.c */
