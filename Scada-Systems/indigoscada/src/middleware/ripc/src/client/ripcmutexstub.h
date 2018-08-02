#ifndef __RIPC_MUTEX_STUB_H__
#define __RIPC_MUTEX_STUB_H__

#include "RIPCMutex.h"
#include "RIPCPrimitiveStub.h"

class RIPCMutexStub : public RIPCPrimitiveStub, public RIPCMutex { 
  public:
    void priorityLock(int rank);    
    bool priorityLock(int rank, unsigned timeout);
    void unlock();

    RIPCMutexStub(RIPCSessionStub* session, char const* name, RIPCResponse& resp) 
      : RIPCPrimitiveStub(session, name, resp) {}    
    
};

#endif
