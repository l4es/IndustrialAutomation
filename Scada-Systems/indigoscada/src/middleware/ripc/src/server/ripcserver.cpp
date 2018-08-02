//#define RIPC_IMPLEMENTATION

#include <stdarg.h>
#include "RIPCPrimitiveImpl.h"
#include "RIPCServer.h"

RIPCServer RIPCServer::theServer;

char* RIPCServer::dump(char* buf) 
{
    RIPC_CS(cs);
    buf += sprintf(buf, "<<<-------- Sessions --------->>>\n");
    for (RIPCSessionImpl* s = sessions; s != NULL; s = s->next) { 
	buf = s->dump(buf);
    }
    buf += sprintf(buf, "<<<-------- Primitives --------->>>\n");
    for (RIPCPrimitiveImpl* prim = primitives; prim != NULL; prim = prim->next) { 
	buf = prim->dump(buf);
    }
    return buf;
}

inline void input(char const* prompt, char* buf, size_t buf_size)
{
    char* p;
    do {
	printf(prompt);
	*buf = '\0';
	fgets(buf, buf_size, stdin);
	p = buf + strlen(buf);
    } while (p <= buf+1);

    if (*(p-1) == '\n') {
	*--p = '\0';
    }
}

void RIPCServer::dialog() 
{ 
    char buf[32*1024];
    while (true) {	
	input("> ", buf, sizeof buf);
	if (strcmp(buf, "exit") == 0) { 
	    shutdown();	 
	    break;
	} else if (strcmp(buf, "info") == 0) { 
	    dump(buf);
	    fputs(buf, stdout);
	} else if (strcmp(buf, "help") == 0 || strcmp(buf, "?") == 0) { 
	    printf("Commands: exit, info, help\n");
	} else { 
	    printf("Invalid command %s\nCommands: exit, info, help\n", buf);
	}
    }
}

void thread_proc RIPCServer::acceptThread(void* arg)
{
    ((RIPCServer*)arg)->run();
}

void RIPCServer::start()
{
    running = true;
    thread.start(&acceptThread, this);
}

void RIPCServer::trace(char const* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void RIPCServer::run()
{
    while (running) { 
	RIPCSocket* s = socket->accept();
	if (!running) { 
            delete s;
	    break;
	}
	if (s == NULL) { 
	    if (debug >= DEBUG_EXCEPTIONS) { 
		char errbuf[1024];
		socket->get_error_text(errbuf, sizeof errbuf);
		trace("Accept socket error: %s\n", errbuf); 
	    }
	} else { 
	    startSession(s);
	}
    }
}

void RIPCServer::shutdown()
{
    running = false;
    RIPCSocket* s = RIPCSocket::connect("localhost", port);
    delete s;
}


RIPCPrimitiveImpl* RIPCServer::getPrimitive(unsigned oid) {
    RIPC_CS(cs);
    RIPCPrimitiveImpl* prim = NULL; 
    if (oid > primTableUsed || (prim = primTable[oid]) == NULL) { 
	RIPC_THROW(RIPCNotFoundException);
    }
    return prim;
}

RIPCEventImpl* RIPCServer::getEvent(unsigned oid)
{
    RIPCPrimitiveImpl* prim = getPrimitive(oid);
    if (prim->getType() != RIPCPrimitive::RIPC_EVENT) { 
	RIPC_THROW(RIPCNotFoundException);
    }
    return (RIPCEventImpl*)prim;
}

RIPCLockImpl* RIPCServer::getLock(unsigned oid)
{
    RIPCPrimitiveImpl* prim = getPrimitive(oid);
    if (prim->getType() != RIPCPrimitive::RIPC_LOCK) { 
	RIPC_THROW(RIPCNotFoundException);
    }
    return (RIPCLockImpl*)prim;
}

RIPCBarrierImpl* RIPCServer::getBarrier(unsigned oid)
{
    RIPCPrimitiveImpl* prim = getPrimitive(oid);
    if (prim->getType() != RIPCPrimitive::RIPC_BARRIER) { 
	RIPC_THROW(RIPCNotFoundException);
    }
    return (RIPCBarrierImpl*)prim;
}


RIPCSemaphoreImpl* RIPCServer::getSemaphore(unsigned oid)
{
    RIPCPrimitiveImpl* prim = getPrimitive(oid);
    if (prim->getType() != RIPCPrimitive::RIPC_SEMAPHORE) { 
	RIPC_THROW(RIPCNotFoundException);
    }
    return (RIPCSemaphoreImpl*)prim;
}


RIPCMutexImpl* RIPCServer::getMutex(unsigned oid)
{
    RIPCPrimitiveImpl* prim = getPrimitive(oid);
    if (prim->getType() != RIPCPrimitive::RIPC_MUTEX) { 
	RIPC_THROW(RIPCNotFoundException);
    }
    return (RIPCMutexImpl*)prim;
}


RIPCQueueImpl* RIPCServer::getQueue(unsigned oid)
{
    RIPCPrimitiveImpl* prim = getPrimitive(oid);
    if (prim->getType() != RIPCPrimitive::RIPC_QUEUE) { 
	RIPC_THROW(RIPCNotFoundException);
    }
    return (RIPCQueueImpl*)prim;
}


RIPCSharedMemoryImpl* RIPCServer::getSharedMemory(unsigned oid)
{
    RIPCPrimitiveImpl* prim = getPrimitive(oid);
    if (prim->getType() != RIPCPrimitive::RIPC_SHARED_MEMORY) { 
	RIPC_THROW(RIPCNotFoundException);
    }
    return (RIPCSharedMemoryImpl*)prim;
}


char* RIPCServer::strdup(char const* name) 
{
    char* copy = new char[strlen(name) + 1];
    strcpy(copy, name);
    return copy;
}


RIPCEventImpl* RIPCServer::createEvent(RIPCSessionImpl* session, char const* name, 
				       bool signaled, bool manualReset, bool local) 
{
    RIPC_CS(cs);
    RIPCEventImpl* event = (RIPCEventImpl*)events.get(name);
    if (event != NULL) { 
	session->exists = true;
    } else { 
	event = new RIPCEventImpl(this, local ? strdup(name) : name, signaled, manualReset);
	events.put(event);
	session->exists = false;
    }
    session->beginAccess(event);
    return event;
}    
	

RIPCMutexImpl* RIPCServer::createMutex(RIPCSessionImpl* session, char const* name, 
				       bool locked, bool local) 
{
    RIPC_CS(cs);
    RIPCMutexImpl* mutex = (RIPCMutexImpl*)mutexes.get(name);
    if (mutex != NULL) { 
	session->exists = true;
    } else { 
	mutex = new RIPCMutexImpl(this, local ? strdup(name) : name, locked);
	mutexes.put(mutex);
	session->exists = false;
    }
    session->beginAccess(mutex);
    return mutex;
}    
	

RIPCSemaphoreImpl* RIPCServer::createSemaphore(RIPCSessionImpl* session, char const* name, 
					       int initCount, bool local)
{
    RIPC_CS(cs);
    RIPCSemaphoreImpl* semaphore = (RIPCSemaphoreImpl*)semaphores.get(name);
    if (semaphore != NULL) { 
	session->exists = true;
    } else { 
	semaphore = new RIPCSemaphoreImpl(this, local ? strdup(name) : name, initCount);
	semaphores.put(semaphore);
	session->exists = false;
    }
    session->beginAccess(semaphore);
    return semaphore;
}    
	
RIPCSharedMemoryImpl* RIPCServer::createSharedMemory(RIPCSessionImpl* session, char const* name, 
						     RIPCObject const& objDesc, bool copy)
{
    RIPC_CS(cs);
    RIPCSharedMemoryImpl* shmem = (RIPCSharedMemoryImpl*)shmems.get(name);
    if (shmem != NULL) { 
	session->exists = true;
    } else { 
	shmem = new RIPCSharedMemoryImpl(this, strdup(name), objDesc, copy);
	shmems.put(shmem);
	session->exists = false;
    }
    session->beginAccess(shmem);
    return shmem;
}    
	
RIPCSharedMemoryImpl* RIPCServer::createSharedMemory(RIPCSessionImpl* session, char const* name, 
						     size_t objSize)
{
    RIPC_CS(cs);
    RIPCSharedMemoryImpl* shmem = (RIPCSharedMemoryImpl*)shmems.get(name);
    if (shmem != NULL) { 
	session->skipBody(objSize);
	session->exists = true;
    } else { 
	shmem = new RIPCSharedMemoryImpl(this, name, session, objSize);
	shmems.put(shmem);
	session->exists = false;
    }
    session->beginAccess(shmem);
    return shmem;
}    
	
RIPCLockImpl* RIPCServer::createLock(RIPCSessionImpl* session, char const* name, bool local)
{
    RIPC_CS(cs);
    RIPCLockImpl* lock = (RIPCLockImpl*)locks.get(name);
    if (lock != NULL) { 
	session->exists = true;
    } else { 
	lock = new RIPCLockImpl(this, local ? strdup(name) : name);
	locks.put(lock);
	session->exists = false;
    }
    session->beginAccess(lock);
    return lock;
}    

RIPCQueueImpl* RIPCServer::createQueue(RIPCSessionImpl* session, char const* name, bool local)
{
    RIPC_CS(cs);
    RIPCQueueImpl* queue = (RIPCQueueImpl*)queues.get(name);
    if (queue != NULL) { 
	session->exists = true;
    } else { 
	queue = new RIPCQueueImpl(this, local ? strdup(name) : name);
	queues.put(queue);
	session->exists = false;
    }
    session->beginAccess(queue);
    return queue;
}    

RIPCBarrierImpl* RIPCServer::createBarrier(RIPCSessionImpl* session, char const* name, int nSessions, bool local)
{
    RIPC_CS(cs);
    RIPCBarrierImpl* barrier = (RIPCBarrierImpl*)barriers.get(name);
    if (barrier != NULL) { 
	session->exists = true;
    } else { 
	barrier = new RIPCBarrierImpl(this, local ? strdup(name) : name, nSessions);
	barriers.put(barrier);
	session->exists = false;
    }
    session->beginAccess(barrier);
    return barrier;
}    


RIPCQueueImpl* RIPCServer::openQueue(RIPCSessionImpl* session, char const* name) 
{
    RIPC_CS(cs);
    RIPCQueueImpl* queue = (RIPCQueueImpl*)queues.get(name);
    if (queue != NULL) { 
	session->beginAccess(queue);
    }
    return queue;
}    

RIPCMutexImpl* RIPCServer::openMutex(RIPCSessionImpl* session, char const* name) 
{
    RIPC_CS(cs);
    RIPCMutexImpl* mutex = (RIPCMutexImpl*)mutexes.get(name);
    if (mutex != NULL) { 
	session->beginAccess(mutex);
    }
    return mutex;
}    

RIPCSharedMemoryImpl* RIPCServer::openSharedMemory(RIPCSessionImpl* session, char const* name) 
{
    RIPC_CS(cs);
    RIPCSharedMemoryImpl* shmem = (RIPCSharedMemoryImpl*)shmems.get(name);
    if (shmem != NULL) { 
	session->beginAccess(shmem);
    }
    return shmem;
}    

RIPCLockImpl* RIPCServer::openLock(RIPCSessionImpl* session, char const* name) 
{
    RIPC_CS(cs);
    RIPCLockImpl* lock = (RIPCLockImpl*)locks.get(name);
    if (lock != NULL) { 
	session->beginAccess(lock);
    }
    return lock;
}    

RIPCBarrierImpl* RIPCServer::openBarrier(RIPCSessionImpl* session, char const* name) 
{
    RIPC_CS(cs);
    RIPCBarrierImpl* barrier = (RIPCBarrierImpl*)barriers.get(name);
    if (barrier != NULL) { 
	session->beginAccess(barrier);
    }
    return barrier;
}    

RIPCSemaphoreImpl* RIPCServer::openSemaphore(RIPCSessionImpl* session, char const* name) 
{
    RIPC_CS(cs);
    RIPCSemaphoreImpl* semaphore = (RIPCSemaphoreImpl*)semaphores.get(name);
    if (semaphore != NULL) { 
	session->beginAccess(semaphore);
    }
    return semaphore;
}    

RIPCEventImpl* RIPCServer::openEvent(RIPCSessionImpl* session, char const* name) 
{
    RIPC_CS(cs);
    RIPCEventImpl* event = (RIPCEventImpl*)events.get(name);
    if (event != NULL) { 
	session->beginAccess(event);
    }
    return event;
}    



void RIPCServer::deleteEvent(RIPCEventImpl* event) 
{
    events.remove(event);
    deassignId(event);
}

void RIPCServer::deleteLock(RIPCLockImpl* lock) 
{
    locks.remove(lock);
    deassignId(lock);
}

void RIPCServer::deleteSharedMemory(RIPCSharedMemoryImpl* shmem) 
{
    shmems.remove(shmem);
    deassignId(shmem);
}

void RIPCServer::deleteBarrier(RIPCBarrierImpl* barrier) 
{
    barriers.remove(barrier);
    deassignId(barrier);
}

void RIPCServer::deleteQueue(RIPCQueueImpl* queue) 
{
    queues.remove(queue);
    deassignId(queue);
}

void RIPCServer::deleteSemaphore(RIPCSemaphoreImpl* semaphore) 
{
    semaphores.remove(semaphore);
    deassignId(semaphore);
}

void RIPCServer::deleteMutex(RIPCMutexImpl* mutex) 
{
    mutexes.remove(mutex);
    deassignId(mutex);
}

void RIPCServer::assignId(RIPCPrimitiveImpl* prim)
{
    if (freePrimId == NULL) {  
	prim->id = ++primTableUsed;
	if (primTableUsed >= primTableSize) { 
	    RIPCPrimitiveImpl** newPrimTable = new RIPCPrimitiveImpl*[primTableUsed*2];
	    memcpy(newPrimTable, primTable, primTableSize*sizeof(RIPCPrimitiveImpl*));
	    primTableSize = primTableUsed*2;
	    delete[] primTable;
	    primTable = newPrimTable;
	}
    } else { 
	prim->id = freePrimId - primTable;
	freePrimId = (RIPCPrimitiveImpl**)*freePrimId;
    }
    primTable[prim->id] = prim;
    if (primitives != NULL) { 
	primitives->prev = prim;
    } 
    prim->next = primitives;
    prim->prev = NULL;
    primitives = prim;
}


void RIPCServer::deassignId(RIPCPrimitiveImpl* prim) 
{ 
    primTable[prim->id] = (RIPCPrimitiveImpl*)freePrimId;
    freePrimId = &primTable[prim->id];
    if (prim->next != NULL) { 
	prim->next->prev = prim->prev;
    }
    if (prim->prev != NULL) { 
	prim->prev->next = prim->next;
    } else {
	primitives = prim->next;
    }	   
    delete prim;
}


static void charArrayDeallocator(RIPCObject const& objDesc) 
{ 
    delete[] (char*)objDesc.data;
}

bool RIPCServer::handleRequest(RIPCSessionImpl* session, RIPCRequest& req, RIPCResponse& resp)
{ 
    if (debug >= DEBUG_REQUESTS) { 
	RIPCPrimitiveImpl* prim = req.objectId != 0 ? getPrimitive(req.objectId) : NULL;
	trace("Session %p receive request %s:%s\n", 
	      session, req.getName(), (prim != NULL ? prim->name : req.name));
    }
    resp.statusCode = RIPCResponse::OK;

    switch (req.opCode) { 
      case RIPCRequest::WAIT:
	getPrimitive(req.objectId)->priorityWait(req.rank);
	break;
      case RIPCRequest::TIMED_WAIT:
	if (!getPrimitive(req.objectId)->priorityWait(req.rank, req.value)) { 
	    resp.statusCode = RIPCResponse::TIMEOUT_EXPIRED;
	}
	break;
      case RIPCRequest::RESET:
	getPrimitive(req.objectId)->reset();
	break;
      case RIPCRequest::SIGNAL_EVENT:
	getEvent(req.objectId)->signal();
	break;
      case RIPCRequest::SIGNAL_SEMAPHORE:
	getSemaphore(req.objectId)->signal(req.value);
	break;
      case RIPCRequest::PULSE:
	getEvent(req.objectId)->pulse();
	break;
      case RIPCRequest::LOCK_MUTEX:
	getMutex(req.objectId)->priorityLock(req.rank);
	break;
      case RIPCRequest::TIMED_LOCK_MUTEX:
	if (!getMutex(req.objectId)->priorityLock(req.rank, req.value)) { 
	    resp.statusCode = RIPCResponse::TIMEOUT_EXPIRED;
	}
	break;
      case RIPCRequest::UNLOCK_MUTEX:
	getMutex(req.objectId)->unlock();
	break;
      case RIPCRequest::SHARED_LOCK:
	getLock(req.objectId)->prioritySharedLock(req.rank);
	break;
      case RIPCRequest::TIMED_SHARED_LOCK:
	if (!getLock(req.objectId)->prioritySharedLock(req.rank, req.value)) { 
	    resp.statusCode = RIPCResponse::TIMEOUT_EXPIRED;
	}
	break;
      case RIPCRequest::EXCLUSIVE_LOCK:
	getLock(req.objectId)->priorityExclusiveLock(req.rank);
	break;
      case RIPCRequest::TIMED_EXCLUSIVE_LOCK:
	if (!getLock(req.objectId)->priorityExclusiveLock(req.rank, req.value)) { 
	    resp.statusCode = RIPCResponse::TIMEOUT_EXPIRED;
	}
	break;
      case RIPCRequest::UNLOCK:
	getLock(req.objectId)->unlock();
	break;
      case RIPCRequest::ENQUEUE:
	getQueue(req.objectId)->put(session, req.objDesc.size);
	break;
      case RIPCRequest::BROADCAST:
	getQueue(req.objectId)->broadcast(session, req.objDesc.size);
	break;
      case RIPCRequest::TIMED_DEQUEUE:
	getQueue(req.objectId)->priorityGet(req.rank, resp.objDesc, req.value);
	break;
      case RIPCRequest::DEQUEUE:
	getQueue(req.objectId)->priorityGet(req.rank, resp.objDesc);
	break;
      case RIPCRequest::GET_OBJECT:
	getSharedMemory(req.objectId)->get(resp.objDesc);
	break;
      case RIPCRequest::SET_OBJECT:
	getSharedMemory(req.objectId)->set(session, req.objDesc.size);
	break;
      case RIPCRequest::CREATE_SEMAPHORE:
	resp.objectId = createSemaphore(session, req.name, req.value, false)->id;
	if (session->exists) { 
	    resp.statusCode = RIPCResponse::ALREADY_EXISTS;
	}
	break;
      case RIPCRequest::CREATE_EVENT:
	resp.objectId = createEvent(session, req.name, req.value != 0, true, false)->id;
	if (session->exists) { 
	    resp.statusCode = RIPCResponse::ALREADY_EXISTS;
	}
	break;
      case RIPCRequest::CREATE_AUTO_EVENT:
	resp.objectId = createEvent(session, req.name, req.value != 0, false, false)->id;
	if (session->exists) { 
	    resp.statusCode = RIPCResponse::ALREADY_EXISTS;
	}
	break;
      case RIPCRequest::CREATE_MUTEX:
	resp.objectId = createMutex(session, req.name, req.value != 0, false)->id;
	if (session->exists) { 
	    resp.statusCode = RIPCResponse::ALREADY_EXISTS;
	}
	break;
      case RIPCRequest::CREATE_QUEUE:
	resp.objectId = createQueue(session, req.name, false)->id;
	if (session->exists) { 
	    resp.statusCode = RIPCResponse::ALREADY_EXISTS;
	}
	break;
      case RIPCRequest::CREATE_SHMEM:
	resp.objectId = createSharedMemory(session, req.name, req.objDesc.size)->id;
	if (session->exists) { 
	    resp.statusCode = RIPCResponse::ALREADY_EXISTS;
	}
	break;
      case RIPCRequest::CREATE_LOCK:
	resp.objectId = createLock(session, req.name, false)->id;
	if (session->exists) { 
	    resp.statusCode = RIPCResponse::ALREADY_EXISTS;
	}
	break;
      case RIPCRequest::CREATE_BARRIER:
	resp.objectId = createBarrier(session, req.name, req.value, false)->id;
	if (session->exists) { 
	    resp.statusCode = RIPCResponse::ALREADY_EXISTS;
	}
	break;
      case RIPCRequest::OPEN_SEMAPHORE:
	{ 
	    RIPCSemaphoreImpl* semaphore = openSemaphore(session, req.name);
	    if (semaphore == NULL) {
		resp.statusCode = RIPCResponse::NOT_FOUND;
	    } else { 
		resp.objectId = semaphore->id;
	    }
	}
	break;
      case RIPCRequest::OPEN_EVENT:
	{ 
	    RIPCEventImpl* event = openEvent(session, req.name);
	    if (event == NULL) {
		resp.statusCode = RIPCResponse::NOT_FOUND;
	    } else { 
		resp.objectId = event->id;
	    }
	}
	break;
      case RIPCRequest::OPEN_MUTEX:
	{ 
	    RIPCMutexImpl* mutex = openMutex(session, req.name);
	    if (mutex == NULL) {
		resp.statusCode = RIPCResponse::NOT_FOUND;
	    } else { 
		resp.objectId = mutex->id;
	    }
	}
	break;
      case RIPCRequest::OPEN_QUEUE:
	{ 
	    RIPCQueueImpl* queue = openQueue(session, req.name);
	    if (queue == NULL) {
		resp.statusCode = RIPCResponse::NOT_FOUND;
	    } else { 
		resp.objectId = queue->id;
	    }
	}
	break;
      case RIPCRequest::OPEN_SHMEM:
	{ 
	    RIPCSharedMemoryImpl* shmem = openSharedMemory(session, req.name);
	    if (shmem == NULL) {
		resp.statusCode = RIPCResponse::NOT_FOUND;
	    } else { 
		resp.objectId = shmem->id;
	    }
	}
	break;
      case RIPCRequest::OPEN_LOCK:
	{ 
	    RIPCLockImpl* lock = openLock(session, req.name);
	    if (lock == NULL) {
		resp.statusCode = RIPCResponse::NOT_FOUND;
	    } else { 
		resp.objectId = lock->id;
	    }
	}
	break;
      case RIPCRequest::OPEN_BARRIER:
	{ 
	    RIPCBarrierImpl* barrier = openBarrier(session, req.name);
	    if (barrier == NULL) {
		resp.statusCode = RIPCResponse::NOT_FOUND;
	    } else { 
		resp.objectId = barrier->id;
	    }
	}
	break;
      case RIPCRequest::CLOSE_PRIMITIVE:
	session->endAccess(getPrimitive(req.objectId)); 
	break;
      case RIPCRequest::CLOSE_SESSION:		    
	return false;
      case RIPCRequest::SHOW_INFO:
	{ 
	    char* buf = new char[RIPC_INFO_BUF_SIZE];
	    char* eof = dump(buf);
	    resp.objDesc.data = buf;
	    resp.objDesc.size = eof - buf + 1;
	    resp.objDesc.deallocator = &charArrayDeallocator;
	}
	break;
      case RIPCRequest::SHUTDOWN:
	shutdown();
	return false;
      default:
	resp.statusCode = RIPCResponse::INTERNAL_ERROR;		
    }
    if (debug >= DEBUG_REQUESTS) { 
	trace("Session %p comlete request %s\n", session, req.opCodeNames[req.opCode]);
    }
    return true;
}

RIPCSessionImpl* RIPCServer::getSession()
{
    RIPCSessionImpl* session = (RIPCSessionImpl*)RIPCCurrentThread::getThreadData();
    if (session == NULL) { 
	session = new RIPCSessionImpl(this);
	RIPCCurrentThread::setThreadData(session);
	cs.enter();
	session->next = sessions;
	if (sessions != NULL) { 
	    sessions->prev = session;
	}
	session->prev = NULL;
	sessions = session;	
	cs.leave();
    }
    return session;
}

void RIPCServer::startSession(RIPCSocket* s) 
{
    RIPCSessionImpl* session = new RIPCSessionImpl(this, s);
    if (debug >= DEBUG_SESSIONS) {
	trace("Start session %p\n", session);
    }
    cs.enter();
    session->next = sessions;
    if (sessions != NULL) { 
	sessions->prev = session;
    }
    session->prev = NULL;
    sessions = session;	
    cs.leave();
    session->start();
}

void RIPCServer::stopSession(RIPCSessionImpl* session) 
{
    if (debug >= DEBUG_SESSIONS) {
	trace("Stop session %p\n", session);
    }
    cs.enter();
    if (session->prev == NULL) { 
	sessions = session->next;
    } else { 
	session->prev->next = session->next;
    }
    if (session->next != NULL) { 
	session->next->prev = session->prev;
    }
    cs.leave();
}

void RIPCServer::waitNotification(RIPCWaitObject& wob, RIPCPrimitiveImpl* prim) 
{ 
    RIPCSessionImpl* session = getSession();
    wob.session = session;
    prim->addWaitObject(&wob);
    session->waitFor = &wob;
    if ((wob.flags & LOCKED_SERVER) != 0) { 
	cs.leave();
    }
    session->waitNotification(prim);
}

void RIPCServer::waitNotificationWithTimeout(RIPCWaitObject& wob, RIPCPrimitiveImpl* prim, unsigned timeout) 
{ 
    if (timeout != 0){ 
	RIPCSessionImpl* session = getSession();
	cs.enter();
	wob.session = session;
        prim->addWaitObject(&wob);
	session->waitFor = &wob;
	wob.flags |= TIMED_WAIT;
	cs.leave();
	session->waitNotificationWithTimeout(prim, timeout);
	if (!wob.signaled) { 
	    cs.enter();
	    wob.session->waitFor = NULL;
            wob.unlink();
	    cs.leave();
	}
    }
}

RIPCServer::~RIPCServer() 
{
    delete   socket;
    delete[] primTable;
}
