//#define RIPC_IMPLEMENTATION

#include "RIPCMutexImpl.h"
#include "RIPCWaitObject.h"
#include "RIPCLockObject.h"
#include "RIPCServer.h"

bool RIPCMutexImpl::detectDeadlock(RIPCWaitObject* wob, RIPCSessionImpl* session) 
{ 
    RIPCSessionImpl* owner = this->owner;
    if (owner != NULL) { 
	if (owner == session) { 
	    return true;
	} else if ((wob = owner->waitFor) != NULL) { 
	    return wob->detectDeadlock(session);
	}
    }
    return false;
}


void RIPCMutexImpl::priorityLock(int rank) 
{ 
    RIPCSessionImpl* session = server->getSession();
    RIPC_CS(cs);
    if (owner == NULL || owner == session) {
	if (owner == NULL) { 
	    owner = session;
	    counter = 1;
	    lck = session->addLock(this, 0);
	} else { 
	    counter += 1;
	}
    } else { 
	RIPCWaitObject wob(rank, RIPCServer::LOCKED_SERVER);
	RIPCWaitObject *wp;
	server->lock();
	if ((wp = owner->waitFor) != NULL) { 
	    if (wp->detectDeadlock(session)) { 
		server->unlock();
		RIPC_THROW(RIPCDeadlockException);
	    }
	}
	server->waitNotification(wob, this);
    }
}

bool RIPCMutexImpl::priorityLock(int rank, unsigned timeout) 
{ 
    RIPCSessionImpl* session = server->getSession();
    RIPC_CS(cs);
    if (owner == NULL || owner == session) {
	if (owner == NULL) { 
	    owner = session;
	    counter = 1;
	    lck = session->addLock(this, 0);
	} else { 
	    counter += 1;
	}
	return true;
    } else { 
	RIPCWaitObject wob(rank);
	server->waitNotificationWithTimeout(wob, this, timeout);
	return wob.signaled;
    }
}    

void RIPCMutexImpl::unlock() 
{
    RIPCSessionImpl* session = server->getSession();
    if (owner != session) { 
	RIPC_THROW(RIPCNotOwnerException);
    }
    RIPC_CS(cs);
    if (--counter == 0) { 	
	owner = NULL;
	session->removeLock(lck);	
	lck = NULL;
	if (!wobList.isEmpty()) { 
            RIPCWaitObject* wob = wobList.next;
	    wob->sendNotification();
            wob->unlink();
	    owner = wob->session;
	    counter = 1;
	    lck = owner->addLock(this, 0);
	}
    }
}

void RIPCMutexImpl::unlock(RIPCLockObject*)
{
    RIPC_CS(cs);
    if (lck != NULL) { 
	owner = NULL;
	counter = 0;
	lck->owner->removeLock(lck);	
	lck = NULL;
	if (!wobList.isEmpty()) { 
            RIPCWaitObject* wob = wobList.next;
	    wob->sendNotification();
            wob->unlink();
	    owner = wob->session;
	    counter = 1;
	    lck = owner->addLock(this, 0);
	}
    }
}

	
void RIPCMutexImpl::priorityWait(int rank)
{
    priorityLock(rank);
}

bool RIPCMutexImpl::priorityWait(int rank, unsigned timeout)
{
    return priorityLock(rank, timeout);
}

void RIPCMutexImpl::reset()
{
    unlock(lck);
}
   
RIPCMutexImpl::RIPCMutexImpl(RIPCServer* server, char const* name, bool locked)  
: RIPCPrimitiveImpl(server, name)
{
    if (locked) { 
	owner = server->getSession();
	counter = 1;
	lck = owner->addLock(this);
    } else { 
	owner = NULL;
	counter = 0;
	lck = NULL;
    }
}


void RIPCMutexImpl::deletePrimitive()
{
    server->deleteMutex(this);
}

char* RIPCMutexImpl::dump(char* buf)
{
    buf = RIPCPrimitiveImpl::dump(buf);
    buf += sprintf(buf, "  Owner: %p\n  Nesting: %d\n", owner, counter);
    return buf;
}
