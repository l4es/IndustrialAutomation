//#define RIPC_IMPLEMENTATION

#include "RIPCSessionStub.h"


RIPCSessionStub::RIPCSessionStub(char const* host, int port, int max_attempts)
{ 
    socket = RIPCSocket::connect(host, port, max_attempts);
    primitives = NULL;
}


void RIPCSessionStub::send(RIPCRequest& req) 
{
    RIPC_CS(cs);
    buf.packRequest(req);
    socket->write(buf.getAddr(), buf.getSize());	
}

void RIPCSessionStub::sendAndReceive(RIPCRequest& req, RIPCResponse& resp, RIPCPrimitive* prim) 
{
    RIPC_CS(cs);
    unsigned bufSize = resp.objDesc.size;
    buf.packRequest(req);
    socket->write(buf.getAddr(), buf.getSize());	
    socket->read(buf.getAddr(), sizeof(RIPCResponseHeader));
    buf.unpackResponse(resp);	
    if (resp.statusCode == RIPCResponse::OK && resp.objDesc.size != 0) { 
	if (resp.objDesc.data != NULL) { 
	    assert(bufSize >= resp.objDesc.size);
	} else { 
	    resp.objDesc.data = prim->allocate(resp.objDesc.size);	
	    resp.objDesc.deallocator = &RIPCPrimitive::deallocator;
	    resp.objDesc.container = prim;
	}
	socket->read(resp.objDesc.data, resp.objDesc.size);
    }
}

void RIPCSessionStub::sendAndVerify(RIPCRequest& req, RIPCResponse& resp, RIPCPrimitive* prim) 
{
    sendAndReceive(req, resp, prim);
    switch (resp.statusCode) { 
      case RIPCResponse::OK:
      case RIPCResponse::ALREADY_EXISTS:
      case RIPCResponse::TIMEOUT_EXPIRED:
	return;
      case RIPCResponse::NOT_FOUND:
	RIPC_THROW(RIPCNotFoundException);  
      case RIPCResponse::NOT_OWNER:
	RIPC_THROW(RIPCNotOwnerException);  
      case RIPCResponse::INVALID_PARAMETER:
	RIPC_THROW(RIPCInvalidParameterException);
      case RIPCResponse::INTERRUPTED:
	RIPC_THROW(RIPCInterruptedException);
      case RIPCResponse::DEADLOCK:
	RIPC_THROW(RIPCDeadlockException);
      default:
	  /////////////////apa added//////////////////
	  #ifdef WIN32
	  ExitProcess(1);
	  #else
	  exit(1);
	  #endif
	  //////////////////////////////////////
	//RIPC_THROW(RIPCInternalException); //Exception here if server is no more available apa---
    }
}
   
bool RIPCSessionStub::sendAndCheckForOk(RIPCRequest& req, RIPCResponse& resp, RIPCPrimitive* prim)
{
    sendAndVerify(req, resp, prim);
    return resp.statusCode == RIPCResponse::OK;
}

RIPCEvent* RIPCSessionStub::createEvent(char const* name, bool signaled, bool manualReset) 
{
    RIPCRequest req;
    RIPCResponse resp;
    req.name = (char*)name;
    req.objectId = 0;
    req.opCode = manualReset ? RIPCRequest::CREATE_EVENT : RIPCRequest::CREATE_AUTO_EVENT;
    req.value = signaled ? 1 : 0;
    sendAndReceive(req, resp);
    return new RIPCEventStub(this, name, resp);
}

RIPCEvent* RIPCSessionStub::openEvent(char const* name) 
{ 
    RIPCRequest req;
    RIPCResponse resp;
    req.name = (char*)name;
    req.objectId = 0;
    req.opCode = RIPCRequest::OPEN_EVENT;
    sendAndVerify(req, resp);
    return resp.statusCode == RIPCResponse::OK ? new RIPCEventStub(this, name, resp) : NULL;
}
	

RIPCSemaphore* RIPCSessionStub::createSemaphore(char const* name, int initCount) 
{ 
    RIPCRequest req;
    RIPCResponse resp;
    req.name = (char*)name;
    req.objectId = 0;
    req.opCode = RIPCRequest::CREATE_SEMAPHORE;
    req.value = initCount;
    sendAndVerify(req, resp);
    return new RIPCSemaphoreStub(this, name, resp);
}
	
RIPCSemaphore* RIPCSessionStub::openSemaphore(char const* name) 
{
    RIPCRequest req;
    RIPCResponse resp;
    req.name = (char*)name;
    req.objectId = 0;
    req.opCode = RIPCRequest::OPEN_SEMAPHORE;
    sendAndVerify(req, resp);
    return resp.statusCode == RIPCResponse::OK ? new RIPCSemaphoreStub(this, name, resp) : NULL;
}
   

RIPCMutex* RIPCSessionStub::createMutex(char const* name, bool locked) 
{ 
    RIPCRequest req;
    RIPCResponse resp;
    req.opCode = RIPCRequest::CREATE_MUTEX;
    req.name = (char*)name;
    req.objectId = 0;
    req.value = locked ? 1 : 0;
    sendAndVerify(req, resp);
    return new RIPCMutexStub(this, name, resp);
}
	
	
RIPCMutex* RIPCSessionStub::openMutex(char const* name) 
{
    RIPCRequest req;
    RIPCResponse resp;
    req.opCode = RIPCRequest::OPEN_MUTEX;
    req.name = (char*)name;
    req.objectId = 0;
    sendAndVerify(req, resp);
    return resp.statusCode == RIPCResponse::OK ? new RIPCMutexStub(this, name, resp) : NULL;
}
   	
    
RIPCQueue* RIPCSessionStub::createQueue(char const* name) 
{ 
    RIPCRequest req;
    RIPCResponse resp;
    req.opCode = RIPCRequest::CREATE_QUEUE;
    req.name = (char*)name;
    req.objectId = 0;
    sendAndVerify(req, resp);
    return new RIPCQueueStub(this, name, resp);
}
	
RIPCQueue* RIPCSessionStub::openQueue(char const* name) 
{ 
    RIPCRequest req;
    RIPCResponse resp;
    req.opCode = RIPCRequest::OPEN_QUEUE;
    req.name = (char*)name;
    req.objectId = 0;
    sendAndVerify(req, resp);
    return resp.statusCode == RIPCResponse::OK ? new RIPCQueueStub(this, name, resp) : NULL;
}

RIPCSharedMemory* RIPCSessionStub::createSharedMemory(char const* name, RIPCObject const& objDesc)
{ 
    RIPCRequest req;
    RIPCResponse resp;
    if (objDesc.data == NULL || objDesc.size <= 0) { 
	RIPC_THROW(RIPCInvalidParameterException);
    }
    req.opCode = RIPCRequest::CREATE_SHMEM;
    req.name = (char*)name;
    req.objectId = 0;
    req.objDesc = objDesc;
    sendAndVerify(req, resp);
    return new RIPCSharedMemoryStub(this, name, resp);
}
	
RIPCSharedMemory* RIPCSessionStub::createSharedMemory(char const* name, void const* obj, size_t objSize)
{ 
    RIPCObject objDesc;
    objDesc.data = (void*)obj;
    objDesc.size = objSize;
    return createSharedMemory(name, objDesc);
}
	

RIPCSharedMemory* RIPCSessionStub::openSharedMemory(char const* name) 
{
    RIPCRequest req;
    RIPCResponse resp;
    req.opCode = RIPCRequest::OPEN_SHMEM;
    req.name = (char*)name;
    req.objectId = 0;
    sendAndVerify(req, resp);
    return resp.statusCode == RIPCResponse::OK ? new RIPCSharedMemoryStub(this, name, resp) : NULL;
}

RIPCLock* RIPCSessionStub::createLock(char const* name) 
{ 
    RIPCRequest req;
    RIPCResponse resp;
    req.name = (char*)name;
    req.objectId = 0;
    req.opCode = RIPCRequest::CREATE_LOCK;
    sendAndVerify(req, resp);
    return new RIPCLockStub(this, name, resp);
}
	
RIPCLock* RIPCSessionStub::openLock(char const* name) 
{
    RIPCRequest req;
    RIPCResponse resp;
    req.name = (char*)name;
    req.objectId = 0;
    req.opCode = RIPCRequest::OPEN_LOCK;
    sendAndVerify(req, resp);
    return resp.statusCode == RIPCResponse::OK ? new RIPCLockStub(this, name, resp) : NULL;
}

RIPCBarrier* RIPCSessionStub::createBarrier(char const* name, int nSessions)
{
    if (nSessions <= 0) { 
	RIPC_THROW(RIPCInvalidParameterException);
    }
    RIPCRequest req;
    RIPCResponse resp;
    req.name = (char*)name;
    req.objectId = 0;
    req.opCode = RIPCRequest::CREATE_BARRIER;
    req.value = nSessions;
    sendAndVerify(req, resp);
    return new RIPCBarrierStub(this, name, resp);
}
	
RIPCBarrier* RIPCSessionStub::openBarrier(char const* name)
{
    RIPCRequest req;
    RIPCResponse resp;
    req.name = (char*)name;
    req.objectId = 0;
    req.opCode = RIPCRequest::OPEN_BARRIER;
    sendAndVerify(req, resp);
    return resp.statusCode == RIPCResponse::OK ? new RIPCBarrierStub(this, name, resp) : NULL;
}


void RIPCSessionStub::close() 
{
    if (socket != NULL) { 
	RIPCRequest req;
	req.opCode = RIPCRequest::CLOSE_SESSION;
        req.objectId = 0;
	send(req);        
	delete socket;
	socket = NULL;
    }
    RIPCPrimitiveStub *prim, *next; 
    for (prim = primitives; prim != NULL; prim = next) { 
	next = prim->next;
	delete prim;
    }
    primitives = NULL;
}
   	
void RIPCSessionStub::shutdownServer() 
{
    RIPCRequest req;
    req.opCode = RIPCRequest::SHUTDOWN;
    req.objectId = 0;
    send(req);	    	
    delete socket;
    socket = NULL;
}

void RIPCSessionStub::showServerInfo(char* buf, size_t bufSize) 
{
    if (buf == NULL || bufSize <= 0) { 
	RIPC_THROW(RIPCInvalidParameterException);
    }
    RIPCRequest req;
    RIPCResponse resp;
    req.opCode = RIPCRequest::SHOW_INFO;
    req.objectId = 0;
    resp.objDesc.data = buf;
    resp.objDesc.size = bufSize;
    sendAndVerify(req, resp);
}

bool RIPCSessionStub::isLocal() { 
    return false;
}

bool RIPCSessionStub::isOk()
{
    return socket == NULL ? false : socket->is_ok();
}

void RIPCSessionStub::getErrorText(char* buf, size_t bufSize)
{
    if (socket == NULL) { 
        strncpy(buf, "socket is not opened", bufSize);
    } else { 
        socket->get_error_text(buf, bufSize);
    }
}


RIPCSessionStub::~RIPCSessionStub()
{
    close();
}
