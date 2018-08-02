#ifndef __RIPC_SEMAPHORE_STUB_H__
#define __RIPC_SEMAPHORE_STUB_H__

#include "RIPCSemaphore.h"
#include "RIPCPrimitiveStub.h"

class RIPCSemaphoreStub : public RIPCPrimitiveStub, public RIPCSemaphore { 
  public:
    void signal(int count) ;

    RIPCSemaphoreStub(RIPCSessionStub* session, char const* name, RIPCResponse& resp) 
      : RIPCPrimitiveStub(session, name, resp) {}    
};

#endif
