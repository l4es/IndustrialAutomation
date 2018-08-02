#ifndef __RIPC_BARRIER_STUB_H__
#define __RIPC_BARRIER_STUB_H__

#include "RIPCBarrier.h"
#include "RIPCPrimitiveStub.h"

class RIPCBarrierStub : public RIPCPrimitiveStub, public RIPCBarrier { 
  public:
    RIPCBarrierStub(RIPCSessionStub* session, char const* name, RIPCResponse& resp) 
      : RIPCPrimitiveStub(session, name, resp) {}
};

#endif
