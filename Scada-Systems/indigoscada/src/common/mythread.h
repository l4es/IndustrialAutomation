///////////////////////////begin MT mutex///////////////////////////////////////////////////////
#ifndef MYTHREAD_H
#define MYTHREAD_H

#ifdef __cplusplus
extern "C" {
#endif
	
	#if defined(WIN32)
	#define USE_WIN32_THREADS
	#elif (defined(ENABLE_THREADS) && defined(HAVE_PTHREAD_H) && \
		   defined(HAVE_PTHREAD_CREATE))
	#define USE_PTHREADS
	#else
	
	#endif

	#include "scada_dll.h"

	/* Because we use threads instead of processes on most platforms (Windows,
	 * Linux, etc), we need locking for them.  On platforms with poor thread
	 * support or broken gethostbyname_r, these functions are no-ops. */

	/** A generic lock structure for multithreaded builds. */
	typedef struct ins_mutex_t {
	#if defined(USE_WIN32_THREADS)
	  CRITICAL_SECTION mutex;
	#elif defined(USE_PTHREADS)
	  pthread_mutex_t mutex;
	#else
	  int _unused;
	#endif
	} ins_mutex_t;

QSEXPORT extern ins_mutex_t *ins_mutex_new(void);
QSEXPORT extern void ins_mutex_init(ins_mutex_t *m);
QSEXPORT extern void ins_mutex_acquire(ins_mutex_t *m);
QSEXPORT extern void ins_mutex_release(ins_mutex_t *m);
QSEXPORT extern void ins_mutex_free(ins_mutex_t *m);
QSEXPORT extern void ins_mutex_uninit(ins_mutex_t *m);
QSEXPORT extern unsigned long ins_get_thread_id(void);
QSEXPORT extern void ins_threads_init(void);

#ifdef __cplusplus
}
#endif

#endif //MYTHREAD_H

///////////////////////////end MT mutex///////////////////////////////////////////////////////