#ifndef __RIPC_RESPONSE_H__
#define __RIPC_RESPONSE_H__

#include "RIPCObject.h"

class RIPCResponseHeader { 
  public:
    unsigned    size;
    unsigned    objectId;
    unsigned    statusCode;
    unsigned    sequenceNo;
};

class RIPC_DLL_ENTRY RIPCResponse : public RIPCResponseHeader { 
  public:
    RIPCObject objDesc;

    enum STATUC_CODES { 
	OK,
	ALREADY_EXISTS,
	TIMEOUT_EXPIRED,
	NOT_FOUND,
	NOT_OWNER,
	DEADLOCK,
	INTERRUPTED,
	INVALID_PARAMETER,
	INTERNAL_ERROR
    };
};

#endif










