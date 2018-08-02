#ifndef __RIPC_CRITICAL_SECTION_H__
#define __RIPC_CRITICAL_SECTION_H__

#include "RIPCdef.h"

#ifdef _WIN32

#include <windows.h>

class RIPC_DLL_ENTRY RIPCCriticalSection {
    CRITICAL_SECTION cs;
  public:
    RIPCCriticalSection() {
	InitializeCriticalSection(&cs);
    }
    ~RIPCCriticalSection() {
	DeleteCriticalSection(&cs);
    }
    void enter() {
	EnterCriticalSection(&cs);
    }
    void leave() {
	LeaveCriticalSection(&cs);
    }
};

#else 

#include <pthread.h>

class RIPC_DLL_ENTRY RIPCCriticalSection {
    friend class RIPCSessionImpl;
  public:
    RIPCCriticalSection() {
	pthread_mutex_init(&mutex, NULL);
    }
    ~RIPCCriticalSection() {
	pthread_mutex_destroy(&mutex);
    }
    void enter() {
	pthread_mutex_lock(&mutex); 
    }
    void leave() {
	pthread_mutex_unlock(&mutex);
    }
  protected:
    pthread_mutex_t mutex;
};

#endif

class RIPC_DLL_ENTRY RIPCCriticalSectionTrigger { 
    RIPCCriticalSection& cs;
  public:
    RIPCCriticalSectionTrigger(RIPCCriticalSection& aCS) 
      : cs(aCS)
    {
	cs.enter();
    }
    ~RIPCCriticalSectionTrigger() { 
	cs.leave();
    }
};

#define RIPC_CS(cs) RIPCCriticalSectionTrigger __RIPCCS(cs)


#endif
