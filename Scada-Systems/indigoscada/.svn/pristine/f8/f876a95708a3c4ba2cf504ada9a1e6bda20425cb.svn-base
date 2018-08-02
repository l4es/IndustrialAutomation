#ifndef __RIPC_SESSION_STUB_H__
#define __RIPC_SESSION_STUB_H__

#include "RIPCSession.h"

#include "RIPCSocket.h"
#include "RIPCCriticalSection.h"
#include "RIPCCommBuffer.h"

#include "RIPCRequest.h"
#include "RIPCResponse.h"

#include "RIPCLockStub.h"
#include "RIPCEventStub.h"
#include "RIPCMutexStub.h"
#include "RIPCQueueStub.h"
#include "RIPCBarrierStub.h"
#include "RIPCSemaphoreStub.h"
#include "RIPCSharedMemoryStub.h"

#ifndef RIPC_NO_EXCEPTIONS
#define RIPC_THROW(x) throw x()
#else
#define RIPC_THROW(x) abort()
#endif


class RIPCSessionStub : public RIPCSession { 
    friend class RIPCPrimitiveStub;
    friend class RIPCClientFactory;
  public:
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
    void close();
    void shutdownServer();
    void showServerInfo(char* buf, size_t bufSize);
    bool isLocal();
    bool isOk();
    void getErrorText(char* buf, size_t bufSize);
    ~RIPCSessionStub();

    void send(RIPCRequest& req);
    void sendAndReceive(RIPCRequest& req, RIPCResponse& resp, RIPCPrimitive* prim = NULL);
    void sendAndVerify(RIPCRequest& req, RIPCResponse& resp, RIPCPrimitive* prim = NULL);
    bool sendAndCheckForOk(RIPCRequest& req, RIPCResponse& resp, RIPCPrimitive* prim = NULL);

  protected:
    RIPCSessionStub(char const* host, int port, int max_attempts=100);

    RIPCPrimitiveStub*  primitives; // L2 null-terminated list of primitives created by this session
    RIPCSocket*         socket;
    RIPCCriticalSection cs;
    RIPCCommBuffer      buf;
};

#endif





