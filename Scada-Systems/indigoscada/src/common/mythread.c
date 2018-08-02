
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOSERVICE
#undef INC_OLE1
#undef INC_OLE2
#include <windows.h>
#include <malloc.h>
#include <assert.h>
#include "mythread.h"

#if defined(USE_WIN32_THREADS)
void
ins_mutex_init(ins_mutex_t *m)
{
  InitializeCriticalSection(&m->mutex);
}
void
ins_mutex_uninit(ins_mutex_t *m)
{
  DeleteCriticalSection(&m->mutex);
}
void
ins_mutex_acquire(ins_mutex_t *m)
{
  assert(m);
  EnterCriticalSection(&m->mutex);
}
void
ins_mutex_release(ins_mutex_t *m)
{
  LeaveCriticalSection(&m->mutex);
}
unsigned long
ins_get_thread_id(void)
{
  return (unsigned long)GetCurrentThreadId();
}
#elif defined(USE_PTHREADS)
/** A mutex attribute that we're going to use to tell pthreads that we want
 * "reentrant" mutexes (i.e., once we can re-lock if we're already holding
 * them.) */
static pthread_mutexattr_t attr_reentrant;
/** True iff we've called ins_threads_init() */
static int threads_initialized = 0;
/** Initialize <b>mutex</b> so it can be locked.  Every mutex must be set
 * up with ins_mutex_init() or ins_mutex_new(); not both. */
void
ins_mutex_init(ins_mutex_t *mutex)
{
  int err;
  if (PREDICT_UNLIKELY(!threads_initialized))
	ins_threads_init();
  err = pthread_mutex_init(&mutex->mutex, &attr_reentrant);
  if (PREDICT_UNLIKELY(err)) {
	log_err(LD_GENERAL, "Error %d creating a mutex.", err);
	ins_fragile_assert();
  }
}
/** Wait until <b>m</b> is free, then acquire it. */
void
ins_mutex_acquire(ins_mutex_t *m)
{
  int err;
  assert(m);
  err = pthread_mutex_lock(&m->mutex);
  if (PREDICT_UNLIKELY(err)) {
	log_err(LD_GENERAL, "Error %d locking a mutex.", err);
	ins_fragile_assert();
  }
}
/** Release the lock <b>m</b> so another thread can have it. */
void
ins_mutex_release(ins_mutex_t *m)
{
  int err;
  assert(m);
  err = pthread_mutex_unlock(&m->mutex);
  if (PREDICT_UNLIKELY(err)) {
	log_err(LD_GENERAL, "Error %d unlocking a mutex.", err);
	ins_fragile_assert();
  }
}
/** Clean up the mutex <b>m</b> so that it no longer uses any system
 * resources.  Does not free <b>m</b>.  This function must only be called on
 * mutexes from ins_mutex_init(). */
void
ins_mutex_uninit(ins_mutex_t *m)
{
  int err;
  assert(m);
  err = pthread_mutex_destroy(&m->mutex);
  if (PREDICT_UNLIKELY(err)) {
	log_err(LD_GENERAL, "Error %d destroying a mutex.", err);
	ins_fragile_assert();
  }
}
/** Return an integer representing this thread. */
unsigned long
ins_get_thread_id(void)
{
  union {
	pthread_t thr;
	unsigned long id;
  } r;
  r.thr = pthread_self();
  return r.id;
}
#endif

/** Return a newly allocated, ready-for-use mutex. */
ins_mutex_t *
ins_mutex_new(void)
{
  ins_mutex_t *m = (struct ins_mutex_t *)calloc(1,sizeof(ins_mutex_t));
  ins_mutex_init(m);
  return m;
}
/** Release all storage and system resources held by <b>m</b>. */
void
ins_mutex_free(ins_mutex_t *m)
{
  ins_mutex_uninit(m);
  free(m);
}


//globals
/** mutex for thread to stop the threads hitting data at the same time. */
QSEXPORT ins_mutex_t * mut = NULL;

