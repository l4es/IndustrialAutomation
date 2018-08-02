#ifndef __RIPC_EVENT_STUB_H__
#define __RIPC_EVENT_STUB_H__

#include "RIPCEvent.h"
#include "RIPCPrimitiveStub.h"

class RIPCEventStub : public RIPCPrimitiveStub, public RIPCEvent { 
  public:
    void signal();
    void pulse();

    RIPCEventStub(RIPCSessionStub* session, char const* name, RIPCResponse& resp) 
      : RIPCPrimitiveStub(session, name, resp) {}
    
};

#endif

