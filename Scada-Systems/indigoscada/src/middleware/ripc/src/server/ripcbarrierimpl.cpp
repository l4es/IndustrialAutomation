//#define RIPC_IMPLEMENTATION

#include "RIPCBarrierImpl.h"
#include "RIPCWaitObject.h"
#include "RIPCServer.h"


void RIPCBarrierImpl::priorityWait(int rank)
{
    RIPC_CS(cs);
    if (++nBlocked < nSessions) { 
	RIPCWaitObject wob(rank);
	server->waitNotification(wob, this);
    } else { 
        wakeupAll();
	nBlocked = 0;
	sequenceNo += 1;
    }
}

bool RIPCBarrierImpl::priorityWait(int rank, unsigned timeout)
{
    RIPC_CS(cs);
    bool result = true;
    int sequenceNo = this->sequenceNo;
    if (++nBlocked < nSessions) { 
	RIPCWaitObject wob(rank);
	server->waitNotificationWithTimeout(wob, this, timeout);
	if (!wob.signaled) { 
	    if (sequenceNo == this->sequenceNo) { 
		nBlocked -= 1;
	    }
	    result = false;
	}
    } else { 
        wakeupAll();
	nBlocked = 0;
	sequenceNo += 1;
    }
    return result;
}


void RIPCBarrierImpl::reset()
{
    RIPC_CS(cs);
    wakeupAll();
    nBlocked = 0;
    sequenceNo += 1;
}
   
void RIPCBarrierImpl::deletePrimitive()
{
    server->deleteBarrier(this);
}

char* RIPCBarrierImpl::dump(char* buf)
{
    buf = RIPCPrimitiveImpl::dump(buf);
    buf += sprintf(buf, "  Expected number of sessions: %d\n  Blocked number of sessions: %d\n", nSessions, nBlocked);
    return buf;
}
