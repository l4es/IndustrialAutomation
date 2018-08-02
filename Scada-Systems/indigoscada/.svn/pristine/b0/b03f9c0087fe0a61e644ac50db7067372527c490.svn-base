//#define RIPC_IMPLEMENTATION

#include "RIPCSessionStub.h"

void RIPCMutexStub::priorityLock(int rank) 
{
    RIPCRequest req;
    RIPCResponse resp;
    req.objectId = id;
    req.opCode = RIPCRequest::LOCK_MUTEX;
    req.rank = rank;
    session->sendAndVerify(req, resp);	
}

bool RIPCMutexStub::priorityLock(int rank, unsigned timeout) 
{
    RIPCRequest req;
    RIPCResponse resp;
    req.objectId = id;
    req.opCode = RIPCRequest::TIMED_LOCK_MUTEX;
    req.rank = rank;
    req.value = timeout;
    return session->sendAndCheckForOk(req, resp);	
}

void RIPCMutexStub::unlock()
{
    RIPCRequest req;
    RIPCResponse resp;
    req.objectId = id;
    req.opCode = RIPCRequest::UNLOCK_MUTEX;
    session->sendAndVerify(req, resp);	
}	
