#ifndef __RIPC_LOCK_STUB_H__
#define __RIPC_LOCK_STUB_H__

#include "RIPCLock.h"
#include "RIPCPrimitiveStub.h"

class RIPCLockStub : public RIPCPrimitiveStub, public RIPCLock { 
  public:
    void priorityExclusiveLock(int rank);
    bool priorityExclusiveLock(int rank, unsigned timeout);
    void prioritySharedLock(int rank);
    bool prioritySharedLock(int rank, unsigned timeout);
    void unlock();

    RIPCLockStub(RIPCSessionStub* session, char const* name, RIPCResponse& resp) 
      : RIPCPrimitiveStub(session, name, resp) {}
};

#endif




