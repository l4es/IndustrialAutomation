//#define RIPC_IMPLEMENTATION

#include "RIPCSessionStub.h"


void RIPCQueueStub::priorityGet(int rank, RIPCObject& objDesc) 
{
    RIPCRequest  req;
    RIPCResponse resp;
    req.objectId = id;
    req.opCode = RIPCRequest::DEQUEUE;
    req.rank = rank;
    resp.objDesc = objDesc;
    session->sendAndVerify(req, resp, this);
    objDesc = resp.objDesc;
}

bool RIPCQueueStub::priorityGet(int rank, RIPCObject& objDesc, unsigned timeout) 
{
    RIPCRequest  req;
    RIPCResponse resp;
    req.objectId = id;
    req.opCode = RIPCRequest::TIMED_DEQUEUE;
    req.rank = rank;
    req.value = timeout;
    resp.objDesc = objDesc;
    session->sendAndVerify(req, resp, this);
    if (resp.objDesc.data != NULL && resp.objDesc.size != 0) { 
	objDesc = resp.objDesc;
	return true;
    }
    return false;
}

void RIPCQueueStub::put(RIPCObject const& objDesc) 
{
    if (objDesc.data == NULL || objDesc.size == 0) { 
	RIPC_THROW(RIPCInvalidParameterException);
    }
    RIPCRequest  req;
    RIPCResponse resp;
    req.objectId = id;
    req.objDesc = objDesc;
    req.opCode = RIPCRequest::ENQUEUE;
    session->sendAndVerify(req, resp);
}

void RIPCQueueStub::put(void const* obj, size_t objSize) 
{ 
    RIPCObject objDesc;
    objDesc.data = (void*)obj;
    objDesc.size = objSize;
    put(objDesc);
}

void RIPCQueueStub::broadcast(RIPCObject const& objDesc)
{
    if (objDesc.data == NULL || objDesc.size == 0) { 
	RIPC_THROW(RIPCInvalidParameterException);
    }
    RIPCRequest  req;
    RIPCResponse resp;
    req.objectId = id;
    req.objDesc = objDesc;
    req.opCode = RIPCRequest::BROADCAST;
    session->sendAndVerify(req, resp);
}

    
void RIPCQueueStub::broadcast(void const* obj, size_t objSize) 
{ 
    RIPCObject objDesc;
    objDesc.data = (void*)obj;
    objDesc.size = objSize;
    broadcast(objDesc);
}



