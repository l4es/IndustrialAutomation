#ifndef __RIPC_SEMAPHORE_IMPL_H__
#define __RIPC_SEMAPHORE_IMPL_H__

#include "RIPCSemaphore.h"
#include "RIPCPrimitiveImpl.h"

class RIPC_DLL_ENTRY RIPCSemaphoreImpl : public RIPCPrimitiveImpl, public RIPCSemaphore { 
  public:
    void  priorityWait(int rank);
    bool  priorityWait(int rank, unsigned timeout);
    void  signal(int n);
    void  reset();
    char* dump(char* buf);

    RIPCSemaphoreImpl(RIPCServer* server, char const* name, int initValue)
    : RIPCPrimitiveImpl(server, name)
    {
	count = initValue;
    }
    void deletePrimitive();
    
  protected:
    int count;
};

#endif
