//#define RIPC_IMPLEMENTATION

#include <stdio.h>
#include "RIPCPrimitiveImpl.h"
#include "RIPCWaitObject.h"
#include "RIPCServer.h"

RIPCPrimitiveImpl::RIPCPrimitiveImpl(RIPCServer* server, char const* name) 
{
    this->server = server;
    this->name = (char*)name;
    server->assignId(this);
    accessCount = 0;
}

RIPCPrimitiveImpl::~RIPCPrimitiveImpl() 
{
    delete[] name;
}

void RIPCPrimitiveImpl::beginAccess(RIPCSessionImpl* session)
{
    accessCount += 1;
}

void RIPCPrimitiveImpl::close()
{
    server->getSession()->endAccess(this);
}

void RIPCPrimitiveImpl::endAccess(RIPCSessionImpl* session)
{
    RIPC_CS(server->cs);
    if (--accessCount == 0) { 
	deletePrimitive();
    }
}
    
void RIPCPrimitiveImpl::unlock(RIPCLockObject*) 
{
}

char const* RIPCPrimitiveImpl::getName() 
{ 
    return name;
}
    
bool RIPCPrimitiveImpl::alreadyExists() 
{
    return server->getSession()->exists;
}


bool RIPCPrimitiveImpl::detectDeadlock(RIPCWaitObject*, RIPCSessionImpl*) 
{ 
    return false;
}

void RIPCPrimitiveImpl::addWaitObject(RIPCWaitObject* wob) 
{ 
    wob->prim = this;
    RIPCWaitObject* head = &wobList;        
    RIPCWaitObject* last = head;
    while ((last = last->prev) != head && last->rank > wob->rank);
    wob->linkAfter(last);
}


void RIPCPrimitiveImpl::wakeupAll() 
{ 
    RIPCWaitObject *head = &wobList, *wob = head; 
    while ((wob = wob->next) != head) { 
        wob->sendNotification();
    }
    head->prune();
}

void RIPCPrimitiveImpl::copyObject(RIPCObject& dst, RIPCObject& src)
{
    if (dst.data == NULL) { 
	dst.data = allocate(src.size);
	memcpy(dst.data, src.data, src.size);
	dst.size = src.size;
	dst.container = this;
	dst.deallocator = &deallocator;
    } else {
	assert(dst.size >= src.size);
	dst.size = src.size;
	memcpy(dst.data, src.data, src.size);
    }
}


char* RIPCPrimitiveImpl::dump(char* buf)
{
    buf += sprintf(buf, "%s:%s\n", getTypeName(), name);
    buf += sprintf(buf, "  Access Count: %u\n", accessCount);
    buf += sprintf(buf, "  Blocked Sessions:\n");
    cs.enter();
    RIPCWaitObject* head = &wobList, *wob = head; 
    while ((wob = wob->next) != head) { 
	buf += sprintf(buf, "    %p\n", wob->session);
    }
    cs.leave();
    return buf;
}



