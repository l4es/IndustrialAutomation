#ifndef __RIPC_SESSION_IMPL_H__
#define  __RIPC_SESSION_IMPL_H__


#ifndef RIPC_NO_EXCEPTIONS
#define RIPC_THROW(x) throw x()
#define RIPC_TRY      try
#define RIPC_CATCH(x) catch(x)
#else
//#define RIPC_THROW(x) abort() apa---
#define RIPC_THROW(x) exit(1) //apa+++
#define RIPC_TRY      if (true)
#define RIPC_CATCH(x) else if (false)
#endif

#include "RIPCSession.h"
#include "RIPCHashMap.h"
#include "RIPCSocket.h"
#include "RIPCThread.h"
#include "RIPCCriticalSection.h"

#include "RIPCLockImpl.h"
#include "RIPCEventImpl.h"
#include "RIPCMutexImpl.h"
#include "RIPCQueueImpl.h"
#include "RIPCBarrierImpl.h"
#include "RIPCSemaphoreImpl.h"
#include "RIPCSharedMemoryImpl.h"

#include "RIPCRequest.h"
#include "RIPCResponse.h"


class RIPC_DLL_ENTRY RIPCSessionImpl : public RIPCSession 
{ 
    friend class RIPCServer;
    friend class RIPCWaitObject;
  public:
    void run();
    void start(); 
    static void thread_proc runThread(void* arg);

    RIPCEvent* createEvent(char const* name, bool signaled, bool manualReset);
    RIPCEvent* openEvent(char const* name);
    RIPCSemaphore* createSemaphore(char const* name, int initCount);
    RIPCSemaphore* openSemaphore(char const* name);
    RIPCMutex* createMutex(char const* name, bool locked);
    RIPCMutex* openMutex(char const* name);
    RIPCQueue* createQueue(char const* name);
    RIPCQueue* openQueue(char const* name);
    RIPCSharedMemory* createSharedMemory(char const* name, RIPCObject const& objDesc);
    RIPCSharedMemory* createSharedMemory(char const* name, void const* obj, size_t objSize);
    RIPCSharedMemory* openSharedMemory(char const* name);
    RIPCLock* createLock(char const* name);
    RIPCLock* openLock(char const* name);
    RIPCBarrier* createBarrier(char const* name, int nSessions);
    RIPCBarrier* openBarrier(char const* name);

    void shutdownServer();
    void showServerInfo(char* buf, size_t bufSize);
    bool isLocal();
    bool isOk();
    void getErrorText(char* buf, size_t bufSize);

    void close();

    RIPCSessionImpl(RIPCServer* server, RIPCSocket* socket = NULL);
    ~RIPCSessionImpl();

    void notify();
    void waitNotification(RIPCPrimitiveImpl* prim);
    void waitNotificationWithTimeout(RIPCPrimitiveImpl* prim, unsigned timeout); 
    void createEvent();
    void deleteEvent();

    void beginAccess(RIPCPrimitiveImpl* prim);
    void endAccess(RIPCPrimitiveImpl* prim);

    RIPCLockObject* addLock(RIPCPrimitiveImpl* prim, int flags = 0);
    void            removeLock(RIPCLockObject* lock);

    void  readBody(void* buf, size_t size);
    void  skipBody(size_t size);

    char* dump(char* buf);

    RIPCSessionImpl*    next;
    RIPCSessionImpl*    prev;
    RIPCThread          thread;
    RIPCCriticalSection cs;
    RIPCLockObject*     lockList;     // L1 list of locks
    RIPCLockObject*     freeLOBList;  // L1 list of free lock objects
    RIPCWaitObject*     waitFor;
    bool                running;
    bool                exists;
    RIPCSocket*         socket;
    RIPCServer*         server;
    unsigned            sequenceNo;
    RIPCHashMap         primitives;

#ifdef _WIN32
    HANDLE e;
#else
    pthread_cond_t cond;
    int            signaled;
#endif

};

#endif
