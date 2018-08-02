//#define RIPC_IMPLEMENTATION

#include "RIPCSharedMemoryImpl.h"
#include "RIPCWaitObject.h"
#include "RIPCServer.h"

RIPCSharedMemoryImpl::RIPCSharedMemoryImpl(RIPCServer* server, char const* name, 
					   RIPCObject const& initObjDesc, bool copy)
: RIPCPrimitiveImpl(server, name) 
{
    sequenceNo = 0;
    if (copy) { 
	objDesc.data = allocate(initObjDesc.size);
	memcpy(objDesc.data, initObjDesc.data, initObjDesc.size);
	objDesc.size = initObjDesc.size;
	objDesc.container = this;
	objDesc.deallocator = &deallocator;
    } else { 
	objDesc = initObjDesc;
    }
}


RIPCSharedMemoryImpl::RIPCSharedMemoryImpl(RIPCServer* server, char const* name, 
					   RIPCSessionImpl* session, size_t objSize)
: RIPCPrimitiveImpl(server, name) 
{
    sequenceNo = 0;
    objDesc.data = allocate(objSize);
    session->readBody(objDesc.data, objSize);
    objDesc.size = objSize;
    objDesc.container = this;
    objDesc.deallocator = &deallocator;
}


void RIPCSharedMemoryImpl::priorityWait(int rank)
{ 
    RIPC_CS(cs);
    RIPCSessionImpl* session = server->getSession();
    if (session->isLocal() || session->sequenceNo == sequenceNo) { 
	RIPCWaitObject wob(rank);
	server->waitNotification(wob, this);
    }	
}

bool RIPCSharedMemoryImpl::priorityWait(int rank, unsigned timeout)
{ 
    RIPC_CS(cs);
    RIPCSessionImpl* session = server->getSession();
    if (session->isLocal() || session->sequenceNo == sequenceNo) { 
	RIPCWaitObject wob(rank);
	server->waitNotificationWithTimeout(wob, this, timeout);
	return wob.signaled;
    }	
    return true;
}


void RIPCSharedMemoryImpl::get(RIPCObject& objDesc)
{
    RIPC_CS(cs);
    RIPCSessionImpl* session = server->getSession();
    session->sequenceNo = sequenceNo;
    copyObject(objDesc, this->objDesc);
}
	
void RIPCSharedMemoryImpl::set(RIPCObject const& objDesc)
{
    RIPC_CS(cs);
    RIPCSessionImpl* session = server->getSession();
    wakeupAll();
    session->sequenceNo = ++sequenceNo;
    this->objDesc.deallocate();
    this->objDesc = objDesc;
}

void RIPCSharedMemoryImpl::set(void const* obj, size_t objSize) 
{ 
    RIPCObject objDesc;
    objDesc.data = allocate(objSize);
    memcpy(objDesc.data, obj, objSize);
    objDesc.size = objSize;
    objDesc.container = this;
    objDesc.deallocator = &deallocator;
    set(objDesc);
}

void RIPCSharedMemoryImpl::set(RIPCSessionImpl* session, size_t size)
{
    RIPCObject objDesc;
    objDesc.data = allocate(size);
    session->readBody(objDesc.data, size);
    objDesc.size = size;
    objDesc.container = this;
    objDesc.deallocator = &deallocator;
    set(objDesc);
}


void RIPCSharedMemoryImpl::reset() {}

void RIPCSharedMemoryImpl::deletePrimitive()
{
    objDesc.deallocate();
    server->deleteSharedMemory(this);
}

char* RIPCSharedMemoryImpl::dump(char* buf)
{
    buf = RIPCPrimitiveImpl::dump(buf);
    RIPC_CS(cs);
    buf += sprintf(buf, "  Object: %p\n  Sequence Number: %d\n", objDesc.data, sequenceNo);
    return buf;
}


