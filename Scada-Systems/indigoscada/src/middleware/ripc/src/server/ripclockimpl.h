#ifndef __RIPC_LOCK_IMPL_H__
#define __RIPC_LOCK_IMPL_H__

#include "RIPCLock.h"
#include "RIPCPrimitiveImpl.h"

class RIPC_DLL_ENTRY RIPCLockImpl : public RIPCPrimitiveImpl, public RIPCLock { 
  public:
    enum LOCK_TYPES { 
	EXCLUSIVE_LOCK = 1, 
	SHARED_LOCK    = 2
    };


    void  priorityWait(int rank);
    bool  priorityWait(int rank, unsigned timeout);
    void  reset();
    char* dump(char* buf);
    void  priorityExclusiveLock(int rank);
    void  prioritySharedLock(int rank);
    bool  priorityExclusiveLock(int rank, unsigned timeout); 
    bool  prioritySharedLock(int rank, unsigned timeout); 
    void  unlock();

    RIPCLockImpl(RIPCServer* server, char const* name)  
    : RIPCPrimitiveImpl(server, name) 
    {
	writer = NULL;
	readers = NULL;
    }
    void deletePrimitive();

  protected: 
    bool detectDeadlock(RIPCWaitObject* wob, RIPCSessionImpl* session);
    void unlock(RIPCLockObject* lob);
    void unlock(RIPCSessionImpl* session);
    void retry();
    
    RIPCLockObject* writer;
    RIPCLockObject* readers;
};

#endif
