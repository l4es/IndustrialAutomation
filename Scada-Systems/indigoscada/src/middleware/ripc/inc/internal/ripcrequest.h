#ifndef __RIPC_REQUEST_H__
#define __RIPC_REQUEST_H__

#include "RIPCObject.h"

class RIPCRequestHeader { 
  public:
    unsigned     size;
    unsigned     objectId;
    unsigned     opCode;
    unsigned     rank;
    unsigned     value;
    unsigned     sequenceNo;
    unsigned     nameLen;
};


class RIPC_DLL_ENTRY RIPCRequest : public RIPCRequestHeader { 
  public:
    char*        name;
    RIPCObject   objDesc;

    RIPCRequest() { 
	name = NULL;
    }

    enum OPCODES { 
	WAIT,
	TIMED_WAIT,
	RESET,
	SIGNAL_EVENT,
	SIGNAL_SEMAPHORE,
	PULSE,
	LOCK_MUTEX,
	TIMED_LOCK_MUTEX,
	UNLOCK_MUTEX,
	ENQUEUE,
	BROADCAST,
	DEQUEUE,
	TIMED_DEQUEUE,
	SET_OBJECT,
	GET_OBJECT,
	EXCLUSIVE_LOCK,
	SHARED_LOCK,
	TIMED_EXCLUSIVE_LOCK,
	TIMED_SHARED_LOCK,
	UNLOCK,
	CREATE_SEMAPHORE,
	CREATE_EVENT,
	CREATE_AUTO_EVENT,
	CREATE_MUTEX,
	CREATE_QUEUE,
	CREATE_SHMEM,
	CREATE_LOCK,
	CREATE_BARRIER,
	OPEN_SEMAPHORE,
	OPEN_EVENT,
	OPEN_MUTEX,
	OPEN_QUEUE,
	OPEN_SHMEM,
	OPEN_LOCK,
	OPEN_BARRIER,
	CLOSE_PRIMITIVE,
	CLOSE_SESSION,
	SHOW_INFO,
	SHUTDOWN
    };

    static const char* const opCodeNames[];

    char const* getName() { 
	return opCodeNames[opCode];
    }
};

#endif




