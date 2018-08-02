//#define RIPC_IMPLEMENTATION

#include "RIPCSessionStub.h"

void RIPCLockStub::priorityExclusiveLock(int rank)
{ 
    RIPCRequest  req;
    RIPCResponse resp;
    req.opCode = RIPCRequest::EXCLUSIVE_LOCK;
    req.objectId = id;
    req.rank = rank;
    session->sendAndVerify(req, resp);	
}
	
bool RIPCLockStub::priorityExclusiveLock(int rank, unsigned timeout) 
{
    RIPCRequest  req;
    RIPCResponse resp;
    req.opCode = RIPCRequest::TIMED_EXCLUSIVE_LOCK;
    req.objectId = id;
    req.rank = rank;
    req.value = timeout;
    return session->sendAndCheckForOk(req, resp);	
}

void RIPCLockStub::prioritySharedLock(int rank)
{ 
    RIPCRequest  req;
    RIPCResponse resp;
    req.opCode = RIPCRequest::SHARED_LOCK;
    req.rank = rank;
    req.objectId = id;
    session->sendAndVerify(req, resp);	
}
	
bool RIPCLockStub::prioritySharedLock(int rank, unsigned timeout)
{
    RIPCRequest  req;
    RIPCResponse resp;
    req.opCode = RIPCRequest::TIMED_SHARED_LOCK;
    req.objectId = id;
    req.rank = rank;
    req.value = timeout;
    return session->sendAndCheckForOk(req, resp);	
}
	
void RIPCLockStub::unlock()
{ 
    RIPCRequest  req;
    RIPCResponse resp;
    req.opCode = RIPCRequest::UNLOCK;
    req.objectId = id;
    session->sendAndVerify(req, resp);	
}	
