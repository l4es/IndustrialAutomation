#ifndef __RIPC_QUEUE_IMPL_H__
#define __RIPC_QUEUE_IMPL_H__

#include "RIPCQueue.h"
#include "RIPCPrimitiveImpl.h"

class RIPC_DLL_ENTRY RIPCQueueImpl : public RIPCPrimitiveImpl, public RIPCQueue { 
    friend class RIPCServer;
  public:
    class Receipt { 
      public:
	Receipt*         next;
	RIPCSessionImpl* session;
    };

    class QueueElement { 
      public:
	QueueElement* next; 
	RIPCObject    objDesc;
	Receipt*      receipts;
	unsigned      nReceipts;
	bool          isBroadcast;
	char* dump(char* buf); 
    };
    
    enum QUEUE_WOB_FLAG {
	CHECK_ONLY = 1
    };
    
    void  priorityWait(int rank);
    bool  priorityWait(int rank, unsigned timeout);
    void  priorityGet(int rank, RIPCObject& objDesc);
    bool  priorityGet(int rank, RIPCObject& objDesc, unsigned timeout);
    void  put(RIPCObject const& objDesc);
    void  put(void const* obj, size_t objSize);
    void  broadcast(RIPCObject const& objDesc);
    void  broadcast(void const* obj, size_t objSize);
    void  reset();
    char* dump(char* buf);

    RIPCQueueImpl(RIPCServer* server, char const* name)
    : RIPCPrimitiveImpl(server, name) 
    {
	messageList = NULL;
	lastElement = NULL;
	freeElements = NULL;
	freeReceipts = NULL;
    }
    void deletePrimitive();

  protected:
    void put(RIPCSessionImpl* session, size_t objSize);
    void broadcast(RIPCSessionImpl* session, size_t objSize);
    bool send(QueueElement* element);
    bool nextElementAvailable();
    bool nextElementAvailable(QueueElement* element, RIPCWaitObject* wob);
    void endAccess(RIPCSessionImpl* session);
    void clear();

    Receipt* allocateReceipt(RIPCSessionImpl* session, Receipt* chain) 
    { 
	Receipt* rc = freeReceipts;
	if (rc != NULL) {
	    freeReceipts = rc->next;
	} else {
	    rc = new Receipt;
	}
	rc->next = chain;
	rc->session = session;
	return rc;
    } 

    void deallocateReceipt(Receipt* receipt) 
    { 
	receipt->next = freeReceipts;
	freeReceipts = receipt;
    }

    QueueElement* allocateElement(RIPCObject const& objDesc)
    {
	QueueElement* elem = freeElements;
	if (elem != NULL) {
	    freeElements = elem->next;
	} else {
	    elem = new QueueElement();
	}
	elem->next = NULL;
	elem->objDesc = objDesc;
	return elem;
    }

    void deallocateElement(QueueElement* elem)
    {
	elem->next = freeElements;
	freeElements = elem;
    }

    QueueElement* messageList;
    QueueElement* lastElement;
    QueueElement* freeElements;
    Receipt*      freeReceipts;
};

#endif
