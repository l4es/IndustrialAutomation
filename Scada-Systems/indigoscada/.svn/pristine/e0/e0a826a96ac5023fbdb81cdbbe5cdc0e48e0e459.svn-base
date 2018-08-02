#ifndef __RIPC_BARRIER_IMPL_H__
#define __RIPC_BARRIER_IMPL_H__

#include "RIPCPrimitiveImpl.h"
#include "RIPCBarrier.h"

class RIPC_DLL_ENTRY RIPCBarrierImpl : public RIPCPrimitiveImpl, public RIPCBarrier { 
  public:
    void  priorityWait(int rank);
    bool  priorityWait(int rank, unsigned timeout);
    void  reset();
    char* dump(char* buf);

    RIPCBarrierImpl(RIPCServer* server, char const* name, int nSessions) 
    : RIPCPrimitiveImpl(server, name)
    {
	this->nSessions = nSessions;
	nBlocked = 0;
	sequenceNo = 0;
    }
    void deletePrimitive();

  protected:
    int sequenceNo;
    int nSessions;
    int nBlocked;
};

#endif
