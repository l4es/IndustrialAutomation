//#define RIPC_IMPLEMENTATION

#include "RIPCSessionStub.h"

void RIPCSharedMemoryStub::set(RIPCObject const& objDesc)
{
    if (objDesc.data == NULL || objDesc.size <= 0) { 
	RIPC_THROW(RIPCInvalidParameterException);
    }
    RIPCRequest  req;
    RIPCResponse resp;
    req.opCode = RIPCRequest::SET_OBJECT;
    req.objectId = id;
    req.objDesc = objDesc;
    session->sendAndVerify(req, resp);
    sequenceNo = resp.sequenceNo;
}
	
void RIPCSharedMemoryStub::set(void const* obj, size_t objSize) 
{ 
    RIPCObject objDesc;
    objDesc.data = (void*)obj;
    objDesc.size = objSize;
    set(objDesc);
}

void RIPCSharedMemoryStub::get(RIPCObject& objDesc) 
{
    RIPCRequest req;
    RIPCResponse resp;
    req.opCode = RIPCRequest::GET_OBJECT;
    req.objectId = id; 
    resp.objDesc = objDesc;
    session->sendAndVerify(req, resp, this);
    sequenceNo = resp.sequenceNo;
    objDesc = resp.objDesc;    
}
