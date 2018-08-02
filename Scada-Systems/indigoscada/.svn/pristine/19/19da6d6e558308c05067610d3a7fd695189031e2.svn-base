#ifndef __RIPC_THREAD_H__
#define __RIPC_THREAD_H__

#include "RIPCdef.h"

#ifdef _WIN32

#include <windows.h>

#define thread_proc WINAPI

class RIPC_DLL_ENTRY RIPCThread {
    HANDLE h;
  public:
    enum ThreadPriority { 
	THR_PRI_LOW, 
	THR_PRI_HIGH
    };

    void setPriority(ThreadPriority pri) { 
	SetThreadPriority(h, pri == THR_PRI_LOW ? THREAD_PRIORITY_IDLE : THREAD_PRIORITY_HIGHEST);
    }

    typedef void (thread_proc* thread_proc_t)(void*);

    void start(thread_proc_t f, void* arg) {
	DWORD threadid;
	h = CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(f), arg,
			 0, &threadid);
    }
    void join() {
	WaitForSingleObject(h, INFINITE);
	CloseHandle(h);
	h = NULL;
    }
    void detach() {
	if (h != NULL) {
	    CloseHandle(h);
	    h = NULL;
	}
    }

    RIPCThread() {
	h = NULL;
    }

    ~RIPCThread() {
	if (h != NULL) {
	    CloseHandle(h);
	}
    }
};

class RIPCCurrentThread { 
  public:
    static void  setThreadData(void* obj) {
	TlsSetValue(tlsIndex, obj);
    }
    static void* getThreadData() { 
        return TlsGetValue(tlsIndex);
    }
    RIPCCurrentThread() { 
	tlsIndex = TlsAlloc();
    }

    ~RIPCCurrentThread() { 
	TlsFree(tlsIndex);
    }    
  private:
    static int tlsIndex;    
};

#else

#define thread_proc

#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

const size_t RIPCThreadStackSize = 1024*1024;

class RIPCThread { 
    pthread_t thread;
  public:
    typedef void (thread_proc* thread_proc_t)(void*);

    void start(thread_proc_t f, void* arg) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
#if !defined(__linux__)
	pthread_attr_setstacksize(&attr, RIPCThreadStackSize);
#endif
#if defined(_AIX41)
	// At AIX 4.1, 4.2 threads are by default created detached
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_UNDETACHED);
#endif
	pthread_create(&thread, &attr, (void*(*)(void*))f, arg);
	pthread_attr_destroy(&attr);
    }

    enum ThreadPriority { 
	THR_PRI_LOW, 
	THR_PRI_HIGH
    };
    void setPriority(ThreadPriority pri) { 
#if defined(PRI_OTHER_MIN) && defined(PRI_OTHER_MAX)
	struct sched_param sp;
	sp.sched_priority = pri == THR_PRI_LOW ? IPRI_OTHER_MIN : PRI_OTHER_MAX;
	pthread_setschedparam(thread, SCHED_OTHER, &sp); 
#endif
    }

    void join() {
	void* result;
	pthread_join(thread, &result);
    }

    void detach() {
	pthread_detach(thread);
    }
};


class RIPCCurrentThread { 
  public:
    static void  setThreadData(void* obj) {
	pthread_setspecific(key, obj);
    }
    static void* getThreadData() { 
        return pthread_getspecific(key);    
    }
    RIPCCurrentThread() { 
	pthread_key_create(&key, NULL);
    }

    ~RIPCCurrentThread() { 
	pthread_key_delete(key);
    }    
  private:
    static pthread_key_t key;
};

#endif

#endif















