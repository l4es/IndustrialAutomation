#ifndef __RIPC_MUTEX_IMPL_H__
#define __RIPC_MUTEX_IMPL_H__

#include "RIPCMutex.h"
#include "RIPCPrimitiveImpl.h"

class RIPC_DLL_ENTRY RIPCMutexImpl : public RIPCPrimitiveImpl, public RIPCMutex { 
  public:
    void  priorityWait(int rank);
    bool  priorityWait(int rank, unsigned timeout);
    void  reset();
    char* dump(char* buf);
    void  priorityLock(int rank);
    bool  priorityLock(int rank, unsigned timeout); 
    void  unlock();

    RIPCMutexImpl(RIPCServer* server, char const* name, bool locked);  
    void deletePrimitive();

  protected: 
    bool detectDeadlock(RIPCWaitObject* wob, RIPCSessionImpl* session);
    void unlock(RIPCLockObject* lob);
    
    RIPCSessionImpl* owner;
    RIPCLockObject*  lck;
    int              counter;
};

#endif
