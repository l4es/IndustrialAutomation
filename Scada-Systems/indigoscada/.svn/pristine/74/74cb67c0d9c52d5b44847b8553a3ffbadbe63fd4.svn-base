#ifndef __RIPC_SHARED_MEMORY_STUB_H__
#define __RIPC_SHARED_MEMORY_STUB_H__

#include "RIPCSharedMemory.h"
#include "RIPCPrimitiveStub.h"

class RIPCSharedMemoryStub : public RIPCPrimitiveStub, public RIPCSharedMemory { 
  public:
    void set(RIPCObject const& obj);
    void set(void const* obj, size_t objSize);
    void get(RIPCObject& objDesc);

    RIPCSharedMemoryStub(RIPCSessionStub* session, char const* name, RIPCResponse& resp) 
      : RIPCPrimitiveStub(session, name, resp) {}
};

#endif
