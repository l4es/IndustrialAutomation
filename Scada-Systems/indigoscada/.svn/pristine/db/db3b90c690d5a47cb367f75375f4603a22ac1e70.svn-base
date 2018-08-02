#ifndef __RIPC_PRIMITIVE_IMPL_H__
#define __RIPC_PRIMITIVE_IMPL_H__

#include "RIPCPrimitive.h"
#include "RIPCCriticalSection.h"
#include "RIPCWaitObject.h"

class RIPCLockObject;
class RIPCSessionImpl;
class RIPCWaitObject;
class RIPCLockObject;
class RIPCHashMap;
class RIPCServer;

class RIPC_DLL_ENTRY RIPCPrimitiveImpl : public virtual RIPCPrimitive { 
    friend class RIPCSessionImpl;
    friend class RIPCWaitObject;
    friend class RIPCLockObject;
    friend class RIPCHashMap;
    friend class RIPCServer;
  public:
    bool          alreadyExists();
    void          close();
    char const*   getName();
    virtual char* dump(char* buf);

  protected:
    virtual void  beginAccess(RIPCSessionImpl* session);
    virtual void  endAccess(RIPCSessionImpl* session);
    virtual bool  detectDeadlock(RIPCWaitObject* wob, RIPCSessionImpl* session);
    virtual void  unlock(RIPCLockObject* lob);
    virtual void  addWaitObject(RIPCWaitObject* wob);

    virtual void  deletePrimitive() = 0;

    virtual void  wakeupAll();

    void copyObject(RIPCObject& dst, RIPCObject& src);

    RIPCPrimitiveImpl(RIPCServer* server, char const* name);
    ~RIPCPrimitiveImpl();

    RIPCPrimitiveImpl*  next;
    RIPCPrimitiveImpl*  prev;
    RIPCServer*         server;
    char*               name;
    unsigned            accessCount;
    unsigned            id;
    RIPCWaitObject      wobList;
    RIPCCriticalSection cs;
};

#endif
