#ifndef __RIPC_WAIT_OBJECT__
#define __RIPC_WAIT_OBJECT__

#include <stdio.h>
#include "RIPCObject.h"

class RIPCSessionImpl;
class RIPCPrimitiveImpl;

class RIPC_DLL_ENTRY RIPCWaitObject 
{ 
  public:
    RIPCWaitObject*      next;
    RIPCWaitObject*      prev;
    RIPCPrimitiveImpl*   prim;
    RIPCSessionImpl*     session;
    int                  flags;
    int                  rank;
    bool                 signaled;
    RIPCObject           objDesc;

    void sendNotification();
    void sendNotification(RIPCObject const& objDesc); 
    bool detectDeadlock(RIPCSessionImpl* session);

    RIPCWaitObject(int rank, int flags = 0) { 
        this->rank = rank;
	this->flags = flags;	
	signaled = false;
    }

    void linkAfter(RIPCWaitObject* after) { 
        prev = after;
        next = after->next;
        next->prev = this;
        after->next = this;
    }
    
    void unlink() { 
        next->prev = prev;
        prev->next = next;
    }
    
    void prune() { 
        next = prev = this;
    }

    bool isEmpty() { 
        return next == this;
    }

    RIPCWaitObject() { 
        prune();
    }

    char* dump(char* buf) { 
	return buf + sprintf(buf, "    %p flags=%d rank=%d\n", session, flags, rank);
    }
};

#endif


