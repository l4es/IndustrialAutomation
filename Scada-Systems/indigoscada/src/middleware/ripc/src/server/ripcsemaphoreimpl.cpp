//#define RIPC_IMPLEMENTATION

#include "RIPCSemaphoreImpl.h"
#include "RIPCWaitObject.h"
#include "RIPCServer.h"

void RIPCSemaphoreImpl::priorityWait(int rank)
{
    RIPC_CS(cs);
    if (count > 0) { 
	count -= 1;
    } else { 
	RIPCWaitObject wob(rank);
	server->waitNotification(wob, this);
    }	
}

bool RIPCSemaphoreImpl::priorityWait(int rank, unsigned timeout)
{
    RIPC_CS(cs);
    if (count > 0) { 
	count -= 1;
	return true;
    } else { 
	RIPCWaitObject wob(rank);
	server->waitNotificationWithTimeout(wob, this, timeout);
	return wob.signaled;
    }
}

void RIPCSemaphoreImpl::signal(int n)
{
    RIPC_CS(cs);
    if (n < 0) { 
	RIPC_THROW(RIPCInvalidParameterException);
    }
    while (n > 0 && !wobList.isEmpty()) { 	    
	wobList.next->sendNotification();
	wobList.next->unlink();
	n -= 1;
    }
    count += n;
}

void RIPCSemaphoreImpl::reset()
{
    RIPC_CS(cs);
    count = 0;
}
   
void RIPCSemaphoreImpl::deletePrimitive()
{
    server->deleteSemaphore(this);
}

char* RIPCSemaphoreImpl::dump(char* buf)
{
    buf = RIPCPrimitiveImpl::dump(buf);
    buf += sprintf(buf, "  Counter: %d\n", count);
    return buf;
}
