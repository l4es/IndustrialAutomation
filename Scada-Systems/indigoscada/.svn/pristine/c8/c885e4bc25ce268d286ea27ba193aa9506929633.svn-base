//#define RIPC_IMPLEMENTATION

#include "RIPCQueueImpl.h"
#include "RIPCWaitObject.h"
#include "RIPCServer.h"

char* RIPCQueueImpl::QueueElement::dump(char* buf) 
{ 
    buf += sprintf(buf, "    Object=%p, size=%d\n", objDesc.data, objDesc.size);
    if (isBroadcast) { 
	if (receipts != NULL) { 
	    buf += sprintf(buf, "      Broadcasted by: %p\n", receipts->session);
	    if (receipts->next != NULL) { 
		buf += sprintf(buf, "      Already received by:");
		for (Receipt* rc = receipts->next; rc != NULL; rc = rc->next) { 
		    buf += sprintf(buf, "        %p\n", rc->session);
		}
	    }
	}
    }
    return buf;
}


bool RIPCQueueImpl::nextElementAvailable() 
{ 
    if (messageList == NULL) { 
	return false;
    }
    if (!messageList->isBroadcast) { 
	return true;
    }
    RIPCSessionImpl* session = server->getSession();
    for (Receipt* rc = messageList->receipts; rc != NULL; rc = rc->next) { 
	if (rc->session == session) { 
	    return false;
	}
    }
    return true;
}


bool RIPCQueueImpl::nextElementAvailable(QueueElement* element, RIPCWaitObject* wob) 
{ 
    if (!element->isBroadcast) { 
	return true;
    }
    for (Receipt* rc = element->receipts; rc != NULL; rc = rc->next) { 
	if (rc->session == wob->session) { 
	    return false;
	}
    }
    return true;
}


void RIPCQueueImpl::priorityWait(int rank)
{ 
    RIPC_CS(cs);
    if (!nextElementAvailable()) { 
	RIPCWaitObject wob(rank, CHECK_ONLY);
	server->waitNotification(wob, this);
    }	
}

bool RIPCQueueImpl::priorityWait(int rank, unsigned timeout)
{ 
    RIPC_CS(cs);
    if (!nextElementAvailable()) { 
	RIPCWaitObject wob(rank, CHECK_ONLY);
	server->waitNotificationWithTimeout(wob, this, timeout);
	return wob.signaled; 
    }	
    return true;
}


void RIPCQueueImpl::priorityGet(int rank, RIPCObject& objDesc)
{
    RIPC_CS(cs);
    if (nextElementAvailable()) { 
	if (messageList->isBroadcast) { 
	    if (++messageList->nReceipts == accessCount) {
		objDesc.assign(messageList->objDesc);
		Receipt *rc, *next;
		for (rc = messageList->receipts; rc != NULL; rc = next) { 
		    next = rc->next;
		    deallocateReceipt(rc);
		}
		QueueElement* elem = messageList;
		messageList = elem->next;
		deallocateElement(elem);

		while ((elem = messageList) != NULL && send(elem)) { 
		    messageList = elem->next;
		    deallocateElement(elem);
		}
	    } else { 
		copyObject(objDesc, messageList->objDesc);
		messageList->receipts = allocateReceipt(server->getSession(), messageList->receipts);
	    }
	} else { 
	    QueueElement* elem = messageList;
	    objDesc.assign(elem->objDesc);
	    messageList = elem->next;
	    deallocateElement(elem);
	}
    } else { 
	RIPCWaitObject wob(rank);
	server->waitNotification(wob, this);
	objDesc.assign(wob.objDesc);
    }	
}

bool RIPCQueueImpl::priorityGet(int rank, RIPCObject& objDesc, unsigned timeout)
{
    RIPC_CS(cs);
    if (nextElementAvailable()) { 
	if (messageList->isBroadcast) { 
	    if (++messageList->nReceipts == accessCount) { 
		objDesc.assign(messageList->objDesc);
		Receipt *rc, *next;
		for (rc = messageList->receipts; rc != NULL; rc = next) { 
		    next = rc->next;
		    deallocateReceipt(rc);
		}
		QueueElement* elem = messageList;
		messageList = elem->next;
		deallocateElement(elem);

		while ((elem = messageList) != NULL && send(elem)) { 
		    messageList = elem->next;
		    deallocateElement(elem);
		}
	    } else { 
		copyObject(objDesc, messageList->objDesc);
		messageList->receipts = allocateReceipt(server->getSession(), messageList->receipts);
	    }
	} else { 
	    QueueElement* elem = messageList;
	    objDesc.assign(elem->objDesc);
	    messageList = elem->next;
	    deallocateElement(elem);
	}
	return true;
    } else { 
	RIPCWaitObject wob(rank);
	server->waitNotificationWithTimeout(wob, this, timeout);
	if (wob.signaled) { 
	    objDesc.assign(wob.objDesc);
	    return true;
	}
    }
    return false;
}


bool RIPCQueueImpl::send(QueueElement* element) 
{
    RIPCWaitObject* head = &wobList;
    RIPCWaitObject* wob = head;
    while ((wob = wob->next) != head) { 
	if (nextElementAvailable(element, wob)) { 
	    wob->unlink();
	    if ((wob->flags & CHECK_ONLY) != 0) { 
		wob->sendNotification();
	    } else { 
		if (element->isBroadcast) { 
		    if (++element->nReceipts == accessCount) {
			Receipt *rp, *next;
			for (rp = element->receipts; rp != NULL; rp = next) { 
			    next = rp->next;
			    deallocateReceipt(rp);
			}
			wob->sendNotification(element->objDesc);
			return true;
		    } else { 
			RIPCObject objDesc;
			copyObject(objDesc, element->objDesc);
			wob->sendNotification(objDesc);
			element->receipts = allocateReceipt(wob->session, element->receipts);
		    }
		} else { 
		    wob->sendNotification(element->objDesc);
		    return true;
		}
	    }
	}
    }
    return false;
}


void RIPCQueueImpl::put(RIPCObject const& objDesc)
{
    RIPC_CS(cs);
    QueueElement* newElem = allocateElement(objDesc);
    newElem->receipts = NULL;
    newElem->isBroadcast = false;
    if (!send(newElem)) { 
	if (messageList == NULL) { 	    
	    messageList = newElem;
	} else { 
	    lastElement->next = newElem;
	}
	lastElement = newElem;
    } else { 
	deallocateElement(newElem);
    }
}

void RIPCQueueImpl::put(void const* obj, size_t objSize) 
{ 
    RIPCObject objDesc;
    objDesc.data = allocate(objSize);
    memcpy(objDesc.data, obj, objSize);
    objDesc.size = objSize;
    objDesc.container = this;
    objDesc.deallocator = &deallocator;
    put(objDesc);
}


void RIPCQueueImpl::broadcast(RIPCObject const& objDesc)
{
    RIPC_CS(cs);
    QueueElement* newElem = allocateElement(objDesc);
    newElem->nReceipts = 1;
    newElem->isBroadcast = true;
    newElem->receipts = allocateReceipt(server->getSession(), NULL);
    if (!send(newElem)) { 
	if (messageList == NULL) { 	    
	    messageList = newElem;
	} else { 
	    lastElement->next = newElem;
	}
	lastElement = newElem;
    } else { 
	deallocateElement(newElem);
    }
}

void RIPCQueueImpl::broadcast(void const* obj, size_t objSize)
{ 
    RIPCObject objDesc;
    objDesc.data = allocate(objSize);
    memcpy(objDesc.data, obj, objSize);
    objDesc.size = objSize;
    objDesc.container = this;
    objDesc.deallocator = &deallocator;
    broadcast(objDesc);
}

void RIPCQueueImpl::put(RIPCSessionImpl* session, size_t objSize)
{
    RIPCObject objDesc;
    objDesc.data = allocate(objSize);
    session->readBody(objDesc.data, objSize);
    objDesc.size = objSize;
    objDesc.container = this;
    objDesc.deallocator = &deallocator;
    put(objDesc);
}

void RIPCQueueImpl::broadcast(RIPCSessionImpl* session, size_t objSize)
{
    RIPCObject objDesc;
    objDesc.data = allocate(objSize);
    session->readBody(objDesc.data, objSize);
    objDesc.size = objSize;
    objDesc.container = this;
    objDesc.deallocator = &deallocator;
    broadcast(objDesc);
}


void RIPCQueueImpl::reset() 
{
    RIPC_CS(cs);
    clear();
}


void RIPCQueueImpl::clear() 
{
    QueueElement* elem, *nextElem;
    Receipt *receipt, *nextReceipt;
    for (elem = messageList; elem != NULL; elem = nextElem) { 
	nextElem = elem->next;
	elem->objDesc.deallocate();
	for (receipt = elem->receipts; receipt != NULL; receipt = nextReceipt) { 
	    nextReceipt = receipt->next;
	    deallocateReceipt(receipt);
	}
	deallocateElement(elem);
    }
    messageList = NULL;
}
    
void RIPCQueueImpl::endAccess(RIPCSessionImpl* session)
{ 
    { 
	RIPC_CS(cs);
	QueueElement *elem = messageList, *prev = NULL;
	bool newTopElem = false; 
	while (elem != NULL) { 
	    if (elem->isBroadcast) { 
		Receipt *rp, *next, **rpp; 
		for (rpp = &elem->receipts; (rp = *rpp) != NULL && rp->session != session; rpp = &rp->next);
		if (rp != NULL) { 
		    *rpp = rp->next;
		    deallocateReceipt(rp);
		    elem->nReceipts -= 1;
		} else if (elem->nReceipts == accessCount-1) { 
		    for (rp = elem->receipts; rp != NULL; rp = next) { 
			next = rp->next;
			deallocateReceipt(rp);
		    }
		    QueueElement* next = elem->next;
		    if (prev == NULL) { 
			newTopElem = true;
			messageList = next;
		    } else { 
			prev->next = next;
		    }
		    if (lastElement == elem) {
			lastElement = prev;
		    }		 
		    deallocateElement(elem);
		    elem = next;
		    continue;
		}
	    }
	    prev = elem;
	    elem = elem->next;
	}
	if (newTopElem) { 
	    while ((elem = messageList) != NULL && send(elem)) { 
		messageList = elem->next;
		deallocateElement(elem);
	    }
	}
    }
    RIPCPrimitiveImpl::endAccess(session);
}


void RIPCQueueImpl::deletePrimitive()
{
    clear();
    QueueElement *elem, *nextElem;
    for (elem = freeElements; elem != NULL; elem = nextElem) { 
	nextElem = elem->next;
	delete elem;
    }
    Receipt *receipt, *nextReceipt;
    for (receipt = freeReceipts; receipt != NULL; receipt = nextReceipt) { 
	nextReceipt = receipt->next;
	delete receipt;
    }
    server->deleteQueue(this);
}

char* RIPCQueueImpl::dump(char* buf)
{
    buf = RIPCPrimitiveImpl::dump(buf);
    RIPC_CS(cs);
    buf += sprintf(buf, "  Messages:\n");
    for (QueueElement* msg = messageList; msg != NULL; msg = msg->next) { 
	buf = msg->dump(buf);
    }
    return buf;
}


