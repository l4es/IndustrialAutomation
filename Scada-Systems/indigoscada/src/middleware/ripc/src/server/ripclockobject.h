#ifndef __RIPC_LOCK_OBJECT_H__
#define __RIPC_LOCK_OBJECT_H__

#include "RIPCPrimitiveImpl.h"

class RIPC_DLL_ENTRY RIPCLockObject { 
  public:
    RIPCLockObject(RIPCSessionImpl* session) 
    { 
	owner = session;
    }

    void unlock() 
    { 
	prim->unlock(this);
    }

    RIPCSessionImpl*   owner;
    RIPCPrimitiveImpl* prim;
    RIPCLockObject*    nextLock;
    RIPCLockObject*    nextOwner;
    int                flags;
};

#endif
