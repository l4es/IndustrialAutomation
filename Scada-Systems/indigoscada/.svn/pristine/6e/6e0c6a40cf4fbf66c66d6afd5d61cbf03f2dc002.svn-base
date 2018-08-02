//#define RIPC_IMPLEMENTATION

#include "RIPCSessionImpl.h"
#include "RIPCServer.h"
#include "RIPCCommBuffer.h"
#include "RIPCIoException.h"
#include "RIPCLockObject.h"
#include "RIPCWaitObject.h"

#ifdef _WIN32
void RIPCSessionImpl::notify() 
{
    waitFor = NULL;
    SetEvent(e);
}
    
void RIPCSessionImpl::waitNotification(RIPCPrimitiveImpl* prim) 
{ 
    ResetEvent(e);
    prim->cs.leave();
    if (WaitForSingleObject(e, INFINITE) != WAIT_OBJECT_0) { 
	RIPC_THROW(RIPCInterruptedException);
    }
    prim->cs.enter();
}

void RIPCSessionImpl::waitNotificationWithTimeout(RIPCPrimitiveImpl* prim, unsigned timeout) 
{ 
    ResetEvent(e);
    prim->cs.leave();    
    int rc = WaitForSingleObject(e, timeout);
    if (rc != WAIT_OBJECT_0 && rc != WAIT_TIMEOUT) { 
	RIPC_THROW(RIPCInterruptedException);
    }
    prim->cs.enter();
}

void RIPCSessionImpl::createEvent() 
{ 
    e = CreateEvent(NULL, true, false, NULL);
}

void RIPCSessionImpl::deleteEvent() 
{
    CloseHandle(e);
}

#else //  Unix

#include "errno.h"

void RIPCSessionImpl::notify() 
{ 
    signaled = true;
    waitFor = NULL;
    pthread_cond_signal(&cond);
}

void RIPCSessionImpl::waitNotification(RIPCPrimitiveImpl* prim) 
{ 
    signaled = false;
    do { 
	int rc = pthread_cond_wait(&cond, &prim->cs.mutex);
	if (rc != 0 && rc != EINTR) { 
	    RIPC_THROW(RIPCInterruptedException);
	}
    } while (!signaled); 
}

void RIPCSessionImpl::waitNotificationWithTimeout(RIPCPrimitiveImpl* prim, unsigned timeout) 
{ 
    struct timeval cur_tv;
    gettimeofday(&cur_tv, NULL);
    struct timespec abs_ts; 
    unsigned usec = (cur_tv.tv_usec + timeout%1000*1000);
    
    signaled = false;
    do { 
	abs_ts.tv_nsec = usec % 1000000 * 1000;
	abs_ts.tv_sec = cur_tv.tv_sec + timeout/1000 + usec / 1000000; 
	int rc = pthread_cond_timedwait(&cond, &prim->cs.mutex, &abs_ts);
	if (rc == ETIMEDOUT) { 
	    break;
	}
	if (rc != 0 && rc != EINTR) { 
	    RIPC_THROW(RIPCInterruptedException);
	}		
    } while (!signaled); 
}

void RIPCSessionImpl::createEvent() 
{ 
    pthread_cond_init(&cond, NULL);
    signaled = false;
}

void RIPCSessionImpl::deleteEvent() 
{ 
    pthread_cond_destroy(&cond);
}


#endif

RIPCSessionImpl::RIPCSessionImpl(RIPCServer* server, RIPCSocket* socket)
{
    this->server = server;
    this->socket = socket;
    sequenceNo = 0;
    running = true;
    waitFor = NULL;
    freeLOBList = NULL;
    lockList = NULL;
    exists = false;
    createEvent();
}

RIPCSessionImpl::~RIPCSessionImpl()
{
    close();
}

void thread_proc RIPCSessionImpl::runThread(void* arg) 
{
    ((RIPCSessionImpl*)arg)->run();
}


void RIPCSessionImpl::start()
{
    running = true;
    thread.start(&runThread, this);
}





void RIPCSessionImpl::run() 
{ 
    char errmsg[256];
    RIPCCommBuffer buf;
    RIPCCurrentThread::setThreadData(this);
    while (running) {
	RIPCRequest  req;
	RIPCResponse resp;
	RIPC_TRY { 
	    if (socket->read(buf.getAddr(), sizeof(RIPCRequestHeader))) { 
		buf.unpackRequest(req);
		if (req.nameLen != 0) { 
		    req.name = new char[req.nameLen];
		    if (!socket->read(req.name, req.nameLen)) { 
                        delete[] req.name;
			socket->get_error_text(errmsg, sizeof(errmsg));
			server->trace("Failed to read primitive name: %s\n", this, errmsg);	    
			break;
		    }
		}
		sequenceNo = req.sequenceNo;
		if (!server->handleRequest(this, req, resp)) { 
		    break;
		}
	    } else { 
		socket->get_error_text(errmsg, sizeof(errmsg));
		server->trace("Failed to read request for session %p: %s\n", this, errmsg);	    
		break;
	    }	
	} RIPC_CATCH(RIPCIoException const&) { 
	    socket->get_error_text(errmsg, sizeof(errmsg));
	    server->trace("Socket IO error: %s\n", this, errmsg);	    
	    break;
	} RIPC_CATCH(RIPCDeadlockException const&) { 
	    resp.statusCode = RIPCResponse::DEADLOCK;
	} RIPC_CATCH(RIPCInternalException const&) { 
	    resp.statusCode = RIPCResponse::INTERNAL_ERROR;
	} RIPC_CATCH(RIPCInvalidParameterException const&) { 
	    resp.statusCode = RIPCResponse::INVALID_PARAMETER;
	} RIPC_CATCH(RIPCNotFoundException const&) { 
	    resp.statusCode = RIPCResponse::NOT_FOUND;
	} RIPC_CATCH(RIPCNotOwnerException const&) { 
	    resp.statusCode = RIPCResponse::NOT_OWNER;
	} RIPC_CATCH(RIPCInterruptedException const&) { 
	    resp.statusCode = RIPCResponse::INTERRUPTED;
	}
	resp.sequenceNo = sequenceNo;
	buf.packResponse(resp);
	if (!socket->write(buf.getAddr(), buf.getSize())) { 			
	    socket->get_error_text(errmsg, sizeof(errmsg));
	    server->trace("Failed to send response for session %p: %s\n", this, errmsg);	    
	    break;
	}
    }
    delete this;
}


void RIPCSessionImpl::close()
{
    RIPC_CS(cs);
    deleteEvent();
    RIPCLockObject *nextLOB, *lob;
    RIPCPrimitiveImpl *prim;
    for (lob = lockList; lob != NULL; lob = nextLOB) { 
	nextLOB = lob->nextLock;
	lob->unlock();
    }    
    lockList = NULL;
    for (lob = freeLOBList; lob != NULL; lob = nextLOB) { 
	nextLOB = lob->nextLock;
	delete lob;
    }
    freeLOBList = NULL;
    RIPCHashMap::Iterator iterator(primitives);    
    while ((prim = iterator.next()) != NULL) { 
	prim->endAccess(this);
    }
    running = false;
    delete socket;
    if (server != NULL) { 
	server->stopSession(this);
	server = NULL;
    }
}


void RIPCSessionImpl::beginAccess(RIPCPrimitiveImpl* prim)
{
    RIPC_CS(cs);
    prim->beginAccess(this);
    primitives.put(prim);
}
    
void RIPCSessionImpl::endAccess(RIPCPrimitiveImpl* prim)
{
    RIPC_CS(cs);
    primitives.remove(prim);
    prim->endAccess(this);
}

RIPCLockObject* RIPCSessionImpl::addLock(RIPCPrimitiveImpl* prim, int flags) 
{
    RIPC_CS(cs);
    RIPCLockObject* lob = freeLOBList;
    if (lob != NULL) { 
	freeLOBList = lob->nextLock;
    } else { 
	lob = new RIPCLockObject(this);
    }
    lob->prim = prim;
    lob->flags = flags;
    lob->nextLock = lockList;
    lockList = lob;
    return lob;
}

void RIPCSessionImpl::removeLock(RIPCLockObject* lock) 
{ 
    RIPC_CS(cs);
    RIPCLockObject **lpp;
    for (lpp = &lockList; *lpp != lock; lpp = &(*lpp)->nextLock);
    *lpp = lock->nextLock;
    lock->nextLock = freeLOBList;
    freeLOBList = lock;
}

char* RIPCSessionImpl::dump(char* buf)
{
    RIPC_CS(cs);
    buf += sprintf(buf, "Session %p\n", this);
    if (waitFor != NULL) { 
	 buf += sprintf(buf, "  Waiting for: %s:%s\n", waitFor->prim->getTypeName(), waitFor->prim->name);
    }
    if (lockList != NULL) { 
	buf += sprintf(buf, "  Locking:\n");
	for (RIPCLockObject* lob = lockList; lob != NULL; lob = lob->nextLock) { 
	    buf += sprintf(buf, "    %s:%s\n", lob->prim->getTypeName(), lob->prim->name);
	}
    }
    buf += sprintf(buf, "  Opened Primitives:\n");
    RIPCHashMap::Iterator iterator(primitives);    
    RIPCPrimitiveImpl* prim;
    while ((prim = iterator.next()) != NULL) { 
	buf += sprintf(buf, "    %s:%s\n", prim->getTypeName(), prim->name);
    }
    return buf;
}


RIPCEvent* RIPCSessionImpl::createEvent(char const* name, bool signaled, bool manualReset)
{
    return server->createEvent(this, name, signaled, manualReset, true);
}

RIPCEvent* RIPCSessionImpl::openEvent(char const* name)
{
    return server->openEvent(this, name);
}

RIPCSemaphore* RIPCSessionImpl::createSemaphore(char const* name, int initCount)
{
    return server->createSemaphore(this, name, initCount, true);
}

RIPCSemaphore* RIPCSessionImpl::openSemaphore(char const* name)
{
    return server->openSemaphore(this, name);
}

RIPCMutex* RIPCSessionImpl::createMutex(char const* name, bool locked)
{
    return server->createMutex(this, name, locked, true);
}

RIPCMutex* RIPCSessionImpl::openMutex(char const* name)
{
    return server->openMutex(this, name);
}

RIPCQueue* RIPCSessionImpl::createQueue(char const* name)
{
    return server->createQueue(this, name, true);
}

RIPCQueue* RIPCSessionImpl::openQueue(char const* name)
{
    return server->openQueue(this, name);
}

RIPCSharedMemory* RIPCSessionImpl::createSharedMemory(char const* name, RIPCObject const& objDesc)
{
    return server->createSharedMemory(this, name, objDesc, false);    
}

RIPCSharedMemory* RIPCSessionImpl::createSharedMemory(char const* name, void const* obj, size_t objSize)
{
    RIPCObject objDesc;
    objDesc.data = (void*)obj;
    objDesc.size = objSize;
    return server->createSharedMemory(this, name, objDesc, true);    
}

RIPCSharedMemory* RIPCSessionImpl::openSharedMemory(char const* name)
{
    return server->openSharedMemory(this, name);    
}

RIPCLock* RIPCSessionImpl::createLock(char const* name)
{
    return server->createLock(this, name, true);
}

RIPCLock* RIPCSessionImpl::openLock(char const* name)
{
    return server->openLock(this, name);
}

RIPCBarrier* RIPCSessionImpl::createBarrier(char const* name, int nSessions)
{
    return server->createBarrier(this, name, nSessions, true);
}

RIPCBarrier* RIPCSessionImpl::openBarrier(char const* name)
{
    return server->openBarrier(this, name);
}


bool RIPCSessionImpl::isLocal()
{
    return socket == NULL;
}

bool RIPCSessionImpl::isOk()
{
    return socket == NULL ? true : socket->is_ok();
}

void RIPCSessionImpl::getErrorText(char* buf, size_t bufSize)
{
    if (socket == NULL) { 
        strncpy(buf, "ok", bufSize);
    } else { 
        socket->get_error_text(buf, bufSize);
    }
}

void RIPCSessionImpl::shutdownServer() {}
   
void RIPCSessionImpl::showServerInfo(char* buf, size_t) 
{ 
    dump(buf);
}

void RIPCSessionImpl::readBody(void* buf, size_t size)
{
    if (!socket->read(buf, size)) { 
	RIPC_THROW(RIPCIoException);
    }
}

void RIPCSessionImpl::skipBody(size_t size)
{
    char buf[4096];
    while (size > 0) { 
	size_t toRead = size < sizeof(buf) ? size : sizeof(buf);
	if (!socket->read(buf, toRead)) { 
	    RIPC_THROW(RIPCIoException);
	}
	size -= toRead;
    }
}
