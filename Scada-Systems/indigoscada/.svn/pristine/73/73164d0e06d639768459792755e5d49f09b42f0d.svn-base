//#define RIPC_IMPLEMENTATION

#include "RIPCWaitObject.h"
#include "RIPCSessionImpl.h"
#include "RIPCServer.h"

void RIPCWaitObject::sendNotification()
{
    signaled = true;
    session->notify();
}

void RIPCWaitObject::sendNotification(RIPCObject const& objDesc) 
{ 
    this->objDesc = objDesc;
    sendNotification();
}

bool RIPCWaitObject::detectDeadlock(RIPCSessionImpl* session) { 
    return ((flags & RIPCServer::TIMED_WAIT) == 0) 
        ? prim->detectDeadlock(this, session) : false;
}
