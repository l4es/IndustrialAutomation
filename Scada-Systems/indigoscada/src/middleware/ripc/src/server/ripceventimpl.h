#ifndef __RIPC_EVENT_IMPL_H__
#define __RIPC_EVENT_IMPL_H__

#include "RIPCEvent.h"
#include "RIPCPrimitiveImpl.h"

class RIPC_DLL_ENTRY RIPCEventImpl : public RIPCPrimitiveImpl, public RIPCEvent { 
  public:
    void  priorityWait(int rank);
    bool  priorityWait(int rank, unsigned timeout);
    void  signal();
    void  pulse();
    void  reset();
    char* dump(char* buf);

    RIPCEventImpl(RIPCServer* server, char const* name, bool signaled, bool manualReset) 
    : RIPCPrimitiveImpl(server, name)
    {
	this->signaled = signaled;
	this->manualReset = manualReset;
    }
    void deletePrimitive();

  protected:
    bool signaled;
    bool manualReset;
};

#endif
