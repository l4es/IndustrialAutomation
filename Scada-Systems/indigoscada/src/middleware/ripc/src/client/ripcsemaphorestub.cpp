//#define RIPC_IMPLEMENTATION

#include "RIPCSessionStub.h"

void RIPCSemaphoreStub::signal(int count)
{ 
    if (count <= 0) {
	RIPC_THROW(RIPCInvalidParameterException);
    }
    RIPCRequest  req;
    RIPCResponse resp;
    req.objectId = id;
    req.opCode = RIPCRequest::SIGNAL_SEMAPHORE;
    req.value = count;
    session->sendAndVerify(req, resp);	
}
