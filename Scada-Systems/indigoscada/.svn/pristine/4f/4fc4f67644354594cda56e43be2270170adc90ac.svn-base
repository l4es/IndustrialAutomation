//#define RIPC_IMPLEMENTATION

#include "RIPCLockImpl.h"
#include "RIPCWaitObject.h"
#include "RIPCLockObject.h"
#include "RIPCServer.h"


bool RIPCLockImpl::detectDeadlock(RIPCWaitObject* wob, RIPCSessionImpl* session) 
{ 
    RIPCLockObject* writer = this->writer;
    if (writer != NULL) {
	if (writer->owner == session) { 
	    return true;
	} else if ((wob = writer->owner->waitFor) != NULL) { 
	    return wob->detectDeadlock(session);
	}
    } else { 
	RIPCWaitObject* rwob;
	for (RIPCLockObject* reader = readers; reader != NULL; reader = reader->nextOwner) { 
	    if (reader->owner == session) {
		return true;
	    } else if (reader->owner != wob->session && (rwob = reader->owner->waitFor) != NULL) { 
		if (rwob->detectDeadlock(session)) { 
		    return true;
		}
	    } 
	}
    }		    		    
    return false;
}


void RIPCLockImpl::priorityExclusiveLock(int rank)
{ 
    RIPCSessionImpl* session = server->getSession();
    RIPC_CS(cs);
    if (writer == NULL || writer->owner != session) { 
	if (writer == NULL 
	    && (readers == NULL || readers->owner == session && readers->nextOwner == NULL))
	{
	    if (readers != NULL) { 
		readers->flags = EXCLUSIVE_LOCK;
		writer = readers;
		readers = NULL;
	    } else { 
		writer = session->addLock(this, EXCLUSIVE_LOCK);
	    }
	} else {
	    server->lock(); 
	    RIPCWaitObject* wp;
	    if (writer != NULL) { 
		if ((wp = writer->owner->waitFor) != NULL) { 
		    if (wp->detectDeadlock(session)) {
			server->unlock(); 
			RIPC_THROW(RIPCDeadlockException);
		    }
		} 
	    } else {
		for (RIPCLockObject* lob = readers; lob != NULL; lob = lob->nextOwner) { 
		    if (lob->owner == session) { 
                        RIPCWaitObject* head = &wobList;
                        wp = head;
                        while ((wp = wp->next) != head) { 
			    if ((wp->flags & EXCLUSIVE_LOCK) != 0) { 
				server->unlock(); 
				RIPC_THROW(RIPCDeadlockException);
			    }
			}
		    } else if ((wp = lob->owner->waitFor) != NULL) { 
			if (wp->detectDeadlock(session)) {
			    server->unlock(); 
			    RIPC_THROW(RIPCDeadlockException);
			}
		    }
		}			
	    }
	    RIPCWaitObject wob(rank, RIPCServer::LOCKED_SERVER|EXCLUSIVE_LOCK);
	    server->waitNotification(wob, this);
	}
    }
}
    

bool RIPCLockImpl::priorityExclusiveLock(int rank, unsigned timeout)
{ 
    RIPCSessionImpl* session = server->getSession();
    bool result = true;
    RIPC_CS(cs);
    if (writer == NULL || writer->owner != session) { 	    
	if (writer == NULL 
	    && (readers == NULL || readers->owner == session && readers->nextOwner == NULL))
	{
	    if (readers != NULL) { 
		readers->flags = EXCLUSIVE_LOCK;
		writer = readers;
		readers = NULL;
	    } else { 
		writer = session->addLock(this, EXCLUSIVE_LOCK);
	    }
	} else { 
	    RIPCWaitObject wob(rank, EXCLUSIVE_LOCK);
	    server->waitNotificationWithTimeout(wob, this, timeout);
	    if (!wob.signaled) {
		result = false;
	    }
	}
    }
    return result;
}
    


void RIPCLockImpl::prioritySharedLock(int rank)
{ 
    RIPCSessionImpl* session = server->getSession();
    RIPC_CS(cs);
    if (writer == NULL || writer->owner != session) { 	    
	if (writer == NULL) {  	
	    RIPCLockObject* lob;
	    for (lob = readers; lob != NULL && lob->owner != session; lob = lob->nextOwner);
	    if (lob == NULL) { 
		lob = session->addLock(this, SHARED_LOCK);
		server->lock();
		lob->nextOwner = readers;
		readers = lob;
		server->unlock();
	    }
	} else { 
	    RIPCWaitObject* wp;
	    server->lock(); 
	    if ((wp = writer->owner->waitFor) != NULL) { 
		if (wp->detectDeadlock(session)) { 		    
		    server->unlock();
		    RIPC_THROW(RIPCDeadlockException);
		}
	    } 
	    RIPCWaitObject wob(rank, RIPCServer::LOCKED_SERVER|SHARED_LOCK);
	    server->waitNotification(wob, this);
	}
    }
}
    

bool RIPCLockImpl::prioritySharedLock(int rank, unsigned timeout) 
{ 
    RIPCSessionImpl* session = server->getSession();
    bool result = true;
    RIPC_CS(cs);
    if (writer == NULL || writer->owner != session) { 	    
	if (writer == NULL) {  	
	    RIPCLockObject* lob;
	    for (lob = readers; lob != NULL && lob->owner != session; lob = lob->nextOwner);
	    if (lob == NULL) { 
		lob = session->addLock(this, SHARED_LOCK);
		server->lock(); 
		lob->nextOwner = readers;
		readers = lob;
		server->unlock(); 
	    }
	} else { 
	    RIPCWaitObject wob(rank, SHARED_LOCK);
	    server->waitNotificationWithTimeout(wob, this, timeout);
	    if (!wob.signaled) {
		result = false;
	    }
	}
    }
    return result;
}
   

void RIPCLockImpl::unlock()
{
    unlock(server->getSession());
}

void RIPCLockImpl::unlock(RIPCLockObject* lob)
{ 
    unlock(lob->owner);
}

void RIPCLockImpl::retry()
{ 
    RIPCWaitObject* head = &wobList;
    RIPCWaitObject* wob = head;
    while ((wob = head->next) != head
           && ((wob->flags & EXCLUSIVE_LOCK) == 0 || readers == NULL 
               || (readers->owner == wob->session && readers->nextOwner == NULL)))
    {
	wob->sendNotification();
        wob->unlink();
	if ((wob->flags & EXCLUSIVE_LOCK) == 0) { 
	    RIPCLockObject* lob = wob->session->addLock(this, SHARED_LOCK);
	    server->lock();
	    lob->nextOwner = readers;
	    readers = lob;
	    server->unlock();
	} else { 
	    if (readers != NULL) {
		readers->flags = EXCLUSIVE_LOCK;
		writer = readers;
		readers = NULL;
	    } else {
		writer = wob->session->addLock(this, EXCLUSIVE_LOCK);
	    }
	    break;
	}
    }
}
	


void RIPCLockImpl::unlock(RIPCSessionImpl* session)
{
    RIPC_CS(cs);
    if (writer != NULL) {
	if (writer->owner != session) { 
	    RIPC_THROW(RIPCNotOwnerException);
	}
	session->removeLock(writer);
	writer = NULL;
    } else { 
	RIPCLockObject *lob, *prev;
	for (lob=readers, prev=NULL; lob != NULL && lob->owner != session; prev=lob, lob=lob->nextOwner);
	if (lob == NULL) { 
	    RIPC_THROW(RIPCNotOwnerException);
	}
	if (prev == NULL) { 
	    readers = lob->nextOwner;
	} else { 
	    prev->nextOwner = lob->nextOwner;
	}
	session->removeLock(lob);
    }
    retry();
}


void RIPCLockImpl::reset() 
{
    RIPC_CS(cs);
    if (writer != NULL) {
	writer->owner->removeLock(writer);
	writer = NULL;
    } else { 
	for (RIPCLockObject* lob = readers; lob != NULL; lob = lob->nextOwner) { 
	    lob->owner->removeLock(lob);
	}
	readers = NULL;
    }
    retry();
}

	
void RIPCLockImpl::priorityWait(int rank)
{
    priorityExclusiveLock(rank);
}

bool RIPCLockImpl::priorityWait(int rank, unsigned timeout)
{
    return priorityExclusiveLock(rank, timeout);
}

void RIPCLockImpl::deletePrimitive()
{
    server->deleteLock(this);
}

char* RIPCLockImpl::dump(char* buf)
{
    buf = RIPCPrimitiveImpl::dump(buf);
    RIPC_CS(cs);
    if (writer != NULL) { 
	buf += sprintf(buf, "  Exclusive Lock: %p\n", writer->owner);
    } else if (readers != NULL) { 
	buf += sprintf(buf, "  Shared Locks:\n");
	for (RIPCLockObject* lob = readers; lob != NULL; lob = lob->nextOwner) { 
	    buf += sprintf(buf, "    %p\n", lob->owner);
	}
    }
    return buf;
}
