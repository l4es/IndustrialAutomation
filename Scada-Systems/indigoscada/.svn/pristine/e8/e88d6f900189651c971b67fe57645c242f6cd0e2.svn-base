//#define RIPC_IMPLEMENTATION

#include "RIPCSessionStub.h"

RIPCPrimitiveStub::RIPCPrimitiveStub(RIPCSessionStub* session, char const* name, RIPCResponse& resp)
{
    exists = resp.statusCode == RIPCResponse::ALREADY_EXISTS;
    this->name = name;
    this->session = session;
    this->id = resp.objectId;
    next = session->primitives;
    if (next != NULL) { 
	next->prev = this;
    }
    prev = NULL;
    session->primitives = this;
}
	
RIPCPrimitiveStub::~RIPCPrimitiveStub()
{
    close();
}


bool RIPCPrimitiveStub::alreadyExists() 
{ 
    return exists;
}
    
void RIPCPrimitiveStub::priorityWait(int rank) 
{ 
    RIPCRequest  req;
    RIPCResponse resp;
    req.objectId = id;
    req.opCode = RIPCRequest::WAIT;
    req.rank = rank;
    req.sequenceNo = sequenceNo;
    session->sendAndVerify(req, resp);	
}

bool RIPCPrimitiveStub::priorityWait(int rank, unsigned timeout)
{
    RIPCRequest req;
    RIPCResponse resp;
    req.objectId = id;
    req.opCode = RIPCRequest::TIMED_WAIT;
    req.rank = rank;
    req.value = timeout;
    req.sequenceNo = sequenceNo;
    session->sendAndVerify(req, resp);
    return resp.statusCode == RIPCResponse::OK;
}


void RIPCPrimitiveStub::reset()
{ 
    RIPCRequest req;
    RIPCResponse resp;
    req.objectId = id;
    req.opCode = RIPCRequest::RESET;
    session->sendAndVerify(req, resp);	
}

void RIPCPrimitiveStub::close() 
{
    if (session != NULL && session->socket != NULL) { 
	RIPCRequest req;
	RIPCResponse resp;
	req.objectId = id;
	req.opCode = RIPCRequest::CLOSE_PRIMITIVE;
	session->cs.leave(); //apa+++ 06-08-2014
	//session->sendAndVerify(req, resp); //apa--- 06-08-2014	
	session->send(req);//apa+++ 06-08-2014

	if (next != NULL) { 
	    next->prev = prev;
	}
	if (prev != NULL) { 
	    prev->next = next;
	} else { 
	    session->primitives = next;
	}
	session = NULL;
    }
}

char const* RIPCPrimitiveStub::getName() 
{
    return name;
}


