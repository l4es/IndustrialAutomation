//#define RIPC_IMPLEMENTATION

#include "RIPCEventImpl.h"
#include "RIPCServer.h"


void RIPCEventImpl::priorityWait(int rank)
{
    RIPC_CS(cs);
    if (signaled) { 
	if (!manualReset) { 
	    signaled = false;
	}	    
    } else { 
	RIPCWaitObject wob(rank);
	server->waitNotification(wob, this);
    }	    
}

bool RIPCEventImpl::priorityWait(int rank, unsigned timeout)
{
    RIPC_CS(cs);
    if (signaled) { 
	if (!manualReset) { 
	    signaled = false;
	}	
	return true;
    } else { 
	RIPCWaitObject wob(rank);
	server->waitNotificationWithTimeout(wob, this, timeout);
	return wob.signaled;
    }
}

void RIPCEventImpl::signal()
{
    RIPC_CS(cs);
    if (wobList.isEmpty()) { 
	signaled = true;
    } else {
	if (manualReset) { 
            wakeupAll();
	    signaled = true;
	} else { 
	    wobList.next->sendNotification();
	    wobList.next->unlink();
	}
    }    
}


void RIPCEventImpl::pulse()
{
    RIPC_CS(cs);
    if (manualReset) { 
        wakeupAll();
    } else if (!wobList.isEmpty()) { 
	wobList.next->sendNotification();
	wobList.next->unlink();
    }
}

void RIPCEventImpl::reset()
{
    RIPC_CS(cs);
    signaled = false;
}
   
void RIPCEventImpl::deletePrimitive()
{
    server->deleteEvent(this);
}

char* RIPCEventImpl::dump(char* buf)
{
    buf = RIPCPrimitiveImpl::dump(buf);
    buf += sprintf(buf, "  Signaled: %d\n  Manual Reset: %d\n", signaled, manualReset);
    return buf;
}
