/********************************************************************************************

This program and source file is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

*********************************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef ENV_WINDOWS
 #undef _UNICODE
 #include <windows.h>
 #define MSG_NOSIGNAL 0
#else
 #include <sys/types.h>
 #include <arpa/inet.h>
 #include <unistd.h>
 #include <pthread.h>
#endif

#ifdef ENV_WINDOWS
 #ifndef ENV_WINDOWSCE
  #include <errno.h>
 #else
  #include <Winsock.h>
 #endif
#endif

#include "liboapc.h"


#ifndef ENV_WINDOWS
struct pthreadCondHandle
{
   pthread_mutex_t condMutex;
   pthread_cond_t  cond;
};
#endif


/**
 * Creates all resources required for wait and signal mechanism.
 * @return a signaling handler or NULL in case the required resources could
 *         not be allocated; this handler needs to be released by calling
 *         oapc_thread_signal_release()
 */
OAPC_EXT_API void *oapc_thread_signal_create(void)
{
#ifndef ENV_WINDOWS
   struct pthreadCondHandle *cHandle;

   cHandle=(struct pthreadCondHandle*)malloc(sizeof(struct pthreadCondHandle));
   if (!cHandle) return NULL;
   pthread_mutex_init(&cHandle->condMutex,NULL); // -> check return value here?
   pthread_cond_init(&cHandle->cond,NULL); // -> check return value here?
#else
   HANDLE cHandle;
 
 #ifdef ENV_WINDOWSCE
   cHandle=CreateEvent(NULL,TRUE,FALSE,_T(""));
 #else
   cHandle=CreateEvent(NULL,TRUE,FALSE,"");
 #endif //ENV_WINDOWSCE
#endif //!ENV_WINDOWS
   return cHandle;
}


/**
 * Sends a signal that can be received by a waiting call to oapc_thread_signal_wait()
 * @param[in] handle the signal handler that is used by the wait function too
 * @return OAPC_OK in case the signal could be sent successfully or an error code
 *         otherwise
 */
OAPC_EXT_API int oapc_thread_signal_send(void *handle)
{
   if (!handle) return OAPC_ERROR_RESOURCE;
#ifndef ENV_WINDOWS
   struct pthreadCondHandle *cHandle;

   cHandle=(struct pthreadCondHandle*)handle;
   pthread_mutex_lock(&cHandle->condMutex);
   pthread_cond_signal(&cHandle->cond);
   pthread_mutex_unlock(&cHandle->condMutex);
#else
   if (SetEvent(handle)) return OAPC_OK;
#endif
   return OAPC_ERROR;
}


/**
 * Waits for a signal sent by oapc_thread_signal_send(); this function returns
 * when the signal was received or when an optional timeout elapsed.
 * @param[in] handle the signal handler that is used by the send function too
 * @param[in] msecs timeout in unit milliseconds; when a value smaller than 0 is
 *            given here the function returns only when a signal is received, for
 *            values greater than 0 it returns after the given time when no signal
 *            was received
 * @return OAPC_OK in case a signal was received/the timeout elapsed or an error code
 *         otherwise
 */
OAPC_EXT_API int oapc_thread_signal_wait(void *handle,int msecs)
{
   if (!handle) return OAPC_ERROR_RESOURCE;
#ifndef ENV_WINDOWS
   struct pthreadCondHandle *cHandle;

   cHandle=(struct pthreadCondHandle*)handle;
   pthread_mutex_lock(&cHandle->condMutex);
   if (msecs<0) pthread_cond_wait(&cHandle->cond,&cHandle->condMutex);
   else
   {
      struct timespec abstime;
      int             secs;

      clock_gettime(CLOCK_REALTIME, &abstime);
      secs=msecs/1000;
      abstime.tv_sec+=secs;
      msecs=msecs-secs*1000;
      abstime.tv_nsec+=msecs*1000000;
      if (abstime.tv_nsec>=1000000000)
      {
          abstime.tv_nsec-=1000000000;
          abstime.tv_sec++;
      }
      pthread_cond_timedwait(&cHandle->cond,&cHandle->condMutex,&abstime);
   }
   pthread_mutex_unlock(&cHandle->condMutex);
   return OAPC_OK;
#else
   DWORD res;

   if (msecs>=0)
   {
      res=WaitForSingleObject(handle,msecs);
      ResetEvent(handle);
      if ((res==WAIT_OBJECT_0) || (res==WAIT_TIMEOUT)) return OAPC_OK;
   }
   else
   {
      res=WaitForSingleObject(handle,INFINITE);
      ResetEvent(handle);
      if (res==WAIT_OBJECT_0) return OAPC_OK;
   }
#endif
   return OAPC_ERROR;
}


OAPC_EXT_API void  oapc_thread_signal_release(void *handle)
{
   if (!handle) return; 
#ifndef ENV_WINDOWS
   struct pthreadCondHandle *cHandle;

   cHandle=(struct pthreadCondHandle*)handle;
   pthread_mutex_destroy(&cHandle->condMutex);
   pthread_cond_destroy(&cHandle->cond);
   free(handle);
#else
   CloseHandle(handle);
#endif
}


/**
 * Releases the resources of a terminated thread
 * @param[in] handle the handle of the thread to be released
 */
OAPC_EXT_API void oapc_thread_release(void *handle)
{
#ifndef ENV_WINDOWS
   free(handle);
#else
   CloseHandle(handle);
#endif
}



/**
 * Creates a new thread and starts it immediately
 * @param[in] start_routine pointer to the function that has to be executed as separate thread
 * @param[in] arg parameters that have to be handed over as argument to that new thread
 * @return handle of the new thread or NULL in case of an error
 */
OAPC_EXT_API void *oapc_thread_create(void *(*start_routine)(void*), void *arg)
{
#ifdef ENV_WINDOWS
   DWORD   id=42;
   HANDLE  tHandle;

   tHandle=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)start_routine,arg,0,&id);
   return tHandle;
#else
   pthread_t     *thread;
   pthread_attr_t attr;
   int            ret;

   thread=(pthread_t*)malloc(sizeof(pthread_t));
   if (!thread) return NULL;

   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
   ret=pthread_create(thread,&attr,start_routine,arg);
   if (ret==0) return thread;
   if (ret==EAGAIN) // try once again
   {
      oapc_util_thread_sleep(0);
      oapc_util_thread_sleep(0);
      ret=pthread_create(thread,NULL,start_routine,arg);
      pthread_attr_destroy(&attr);
      if (ret==0) return thread;
   }
   pthread_attr_destroy(&attr);
   printf("Error creating thread: %d\n",ret);
   free(thread);
   return NULL;
#endif
}



/**
 * Deprecated, please use oapc_thread_create() instead
 */
OAPC_EXT_API bool oapc_util_create_thread(void *(*start_routine)(void*), void *arg)
{
   void *handle;

   handle=oapc_thread_create(start_routine,arg);
#ifndef ENV_WINDOWS
   free(handle);
#endif
   return (handle!=NULL);
}



/**
 * Set the execution priority of own thread
 * @param[in] handle the handle of the thread to set the new priority for
 * @param[in] new priority of thread in range -2 .. 2 where 0 is the default priority,
 *            2 is the absolute maximum (which may block a system completely) and -2
 *            is the minimum where a thread may run very seldom
 * @return true in  case the priority could be set successfully
 */
OAPC_EXT_API bool oapc_thread_set_prio(const void *handle,const char prio)
{
#ifndef ENV_WINDOWS
   sched_param param;
   int         policy,range;
   pthread_t  *thread;

   thread=(pthread_t*)handle;
   if (pthread_getschedparam(*thread, &policy, &param)!=0) return false;
   range=(sched_get_priority_max(policy)-sched_get_priority_min(policy))-2; // get the range but never use the absolute max/min values
   param.sched_priority=(range/5)*(prio+2)+1;
   return (pthread_setschedparam(*thread, policy, &param)==0);
#else
   return (SetThreadPriority((HANDLE)handle,prio)!=0);
#endif
}



/**
 * Deprecated, please use oapc_thread_set_prio() instead
 */
OAPC_EXT_API bool oapc_util_thread_set_prio(const unsigned char prio)
{
#ifndef ENV_WINDOWS
   pthread_t thread;

   thread=pthread_self();
#else
   HANDLE thread;

   thread=GetCurrentThread();
#endif
   return oapc_thread_set_prio((void*)&thread,prio);
}


/**
Sleep for a given amount of time
@param[in] msecs the number of milliseconds to sleep
*/
OAPC_EXT_API int oapc_thread_sleep(int msecs)
{
#ifdef ENV_WINDOWSCE
   Sleep(msecs);
#else
#ifdef ENV_WINDOWS
    Sleep(msecs);
#else
    usleep(msecs*1000);
#endif
#endif
   return msecs;
}



/**
 * Deprecated, please use oapc_thread_sleep() instead
 */
OAPC_EXT_API int oapc_util_thread_sleep(int msecs)
{
   return oapc_thread_sleep(msecs);
}



/**
 * Creates a mutual exclusion object
 * @return the handle to the mutex object or NULL in case an error occurred during creation
 */
OAPC_EXT_API void *oapc_thread_mutex_create(void)
{
#ifndef ENV_WINDOWS
   pthread_mutex_t *mutex;

   mutex=(pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
   if (!mutex) return NULL;
   pthread_mutex_init(mutex,NULL);
#else
   HANDLE mutex;

   mutex=CreateSemaphore(NULL,1,1,NULL);
   if (!mutex) return NULL;
#endif
   return (void*)mutex;
}



/**
 * Tries to enter a mutual exclusion section; in case the section defined by the parameter
 * handle is already in use it blocks until all preceding calls to that section are finished
 * by calling oapc_thread_mutex_lock()
 * @param[in] handle the mutex handle created with oapc_thread_mutex_create()
 */
OAPC_EXT_API void oapc_thread_mutex_lock(void *handle)
{
   if (!handle) 
   {
      assert(0); 
      return;
   }
#ifndef ENV_WINDOWS
   pthread_mutex_lock((pthread_mutex_t*)handle);
#else
   WaitForSingleObject((HANDLE)handle,INFINITE);
#endif
}



/**
 * Leaves a mutual exclusion section and releases the lock to it so that other threads can enter
 * @param[in] handle the mutex handle used for a preceding call of oapc_thread_mutex_lock()
 */
OAPC_EXT_API void oapc_thread_mutex_unlock(void *handle)
{
   if (!handle) return;
#ifndef ENV_WINDOWS
   pthread_mutex_unlock((pthread_mutex_t*)handle);
#else
   ReleaseSemaphore((HANDLE)handle,1,NULL);
#endif
}



/**
 * Destroys a mutex object and releases all related resources
 * @param[in] handle the mutex handle created with oapc_thread_mutex_create()
 */
OAPC_EXT_API void oapc_thread_mutex_release(void *handle)
{
   if (!handle) return;
#ifndef ENV_WINDOWS
   pthread_mutex_destroy((pthread_mutex_t*)handle);
   free(handle);
#else
   CloseHandle((HANDLE)handle);
#endif
}



struct timer_data
{
   void (*start_routine)(void*,int);
   void *data,*threadHandle;
   int   timerID,time;
};



static void *timer_thread(void *arg)
{
   struct timer_data *timerData;

   timerData=(struct timer_data*)arg;
   oapc_thread_sleep(timerData->time);
   timerData->start_routine(timerData->data,timerData->timerID);
   oapc_thread_release(timerData->threadHandle);
   free(timerData);
   return NULL;
}



/**
 * Start a timer
 * @param[in] start_routine callback function to be called after the given time has elapsed
 * @param[in] time time in milliseconds
 * @param[in] data custom data to be handed over to the callback function
 * @param[in] timerID identifier of the timer to be handed over to the callback function
 * @return true in case the timer could be started successfully, false otherwise
 */
OAPC_EXT_API bool oapc_thread_timer_start(void (*start_routine)(void*,int),int time,void *data,int timerID)
{
   struct timer_data *timerData;

   timerData=(struct timer_data*)malloc(sizeof(struct timer_data));
   if (!timerData) return false;
   memset(timerData,0,sizeof(struct timer_data));
   timerData->data=data;
   timerData->timerID=timerID;
   timerData->time=time;
   timerData->start_routine=start_routine;
   timerData->threadHandle=oapc_thread_create(timer_thread,timerData);
   return true;
}


