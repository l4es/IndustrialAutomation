//#define RIPC_IMPLEMENTATION

#include "RIPCSessionStub.h"

void RIPCEventStub::signal() 
{
    RIPCRequest req;
    RIPCResponse resp;
    req.objectId = id;
    req.opCode = RIPCRequest::SIGNAL_EVENT;
    session->sendAndVerify(req, resp);	
}

void RIPCEventStub::pulse()
{
    RIPCRequest req;
    RIPCResponse resp;
    req.objectId = id;
    req.opCode = RIPCRequest::PULSE;
    session->sendAndVerify(req, resp);	
}

