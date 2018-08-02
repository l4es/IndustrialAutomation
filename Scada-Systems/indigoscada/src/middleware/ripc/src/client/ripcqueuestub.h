#ifndef __RIPC_SQUEUE_STUB_H__
#define __RIPC_SQUEUE_STUB_H__

#include "RIPCQueue.h"
#include "RIPCPrimitiveStub.h"

class RIPCQueueStub : public RIPCPrimitiveStub, public RIPCQueue 
{ 
  public:
    void priorityGet(int rank, RIPCObject& objDesc);
    bool priorityGet(int rank, RIPCObject& objDesc, unsigned timeout);
    void put(RIPCObject const& objDesc);
    void put(void const* obj, size_t objSize);
    void broadcast(RIPCObject const& objDesc);
    void broadcast(void const* obj, size_t objSize);

    RIPCQueueStub(RIPCSessionStub* session, char const* name, RIPCResponse& resp) 
      : RIPCPrimitiveStub(session, name, resp) {}
};


#endif
