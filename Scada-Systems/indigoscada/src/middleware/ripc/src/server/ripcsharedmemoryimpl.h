#ifndef __RIPC_SHARED_MEMORY_IMPL_H__
#define __RIPC_SHARED_MEMORY_IMPL_H__

#include "RIPCSharedMemory.h"
#include "RIPCPrimitiveImpl.h"

class RIPC_DLL_ENTRY RIPCSharedMemoryImpl : public RIPCPrimitiveImpl, public RIPCSharedMemory { 
    friend class RIPCServer;
  public:
    void  priorityWait(int rank);
    bool  priorityWait(int rank, unsigned timeout);
    void  get(RIPCObject& objDesc);
    void  set(RIPCObject const& objDesc);
    void  set(void const* obj, size_t objSize);
    void  reset();
    char* dump(char* buf);

    RIPCSharedMemoryImpl(RIPCServer* server, char const* name, RIPCObject const& objDesc, bool copy);
    RIPCSharedMemoryImpl(RIPCServer* server, char const* name, RIPCSessionImpl* session, size_t objSize);
    void deletePrimitive();

  protected:
    void  set(RIPCSessionImpl* session, size_t size);

    unsigned   sequenceNo;
    RIPCObject objDesc;
};

#endif
