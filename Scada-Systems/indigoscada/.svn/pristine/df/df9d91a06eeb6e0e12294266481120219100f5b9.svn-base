#ifndef __RIPC_PRIMITIVE_STUB_H__
#define __RIPC_PRIMITIVE_STUB_H__

#include "RIPCPrimitive.h"
#include "RIPCResponse.h"

class RIPCSessionStub;

class RIPCPrimitiveStub : public virtual RIPCPrimitive 
{ 
    friend class RIPCSessionStub;
  public:
    char const* getName();
    bool alreadyExists();
    void priorityWait(int rank);
    bool priorityWait(int rank, unsigned timeout);
    void reset();
    void close();

    RIPCPrimitiveStub(RIPCSessionStub* session, char const* name, RIPCResponse& resp);
    ~RIPCPrimitiveStub();


  protected:
    bool               exists;
    RIPCSessionStub*   session;
    char const*        name;
    unsigned           id;
    unsigned           sequenceNo;
    RIPCPrimitiveStub* next;
    RIPCPrimitiveStub* prev;
};

#endif    

