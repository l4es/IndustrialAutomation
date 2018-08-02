#ifndef __RIPC_SERVER_H__
#define __RIPC_SERVER_H__

#include "RIPCFactory.h"
#include "RIPCHashMap.h"
#include "RIPCSessionImpl.h"

#define RIPC_INFO_BUF_SIZE (32*1024)

/**
 * Factory for local session. These session can be used to provide
 * synchornization and communitcation between threads in the same process.
 * It is possible to share tghe single session between all threads.
 * Also this class is used as server for remove sessions.
 * Server should be launched by
 * "<code>RIPCServer [-d] [-debug=DEBUG-LEVEL] PORT</code>" command.<BR>
 * Supported debug levels:
 * <UL>
 * <LI><code>0</code> no debugging
 * <LI><code>1</code> trace all exceptions
 * <LI><code>2</code> trace session open/close requests
 * <LI><code>3</code> trace all requests
 * </UL>
 * </DL> 
 */
class RIPC_DLL_ENTRY RIPCServer {
    friend class RIPCSessionImpl;
    friend class RIPCPrimitiveImpl;
  public:
    /**
     * Get instance of the server 
     */
    static RIPCServer theServer;

    virtual RIPCSession* create(char const*, int) { 
	return getSession();
    }
    
    /**
     * Dump information about state of the server:
     * all active sessions and all primitives opened by these sessions
     * @param out output stream
     */
    virtual char* dump(char* buf);

    /**
     * Server interactive dialog
     * The following commands are supported:
     * <DL>
     * <DT><code>info</code><DD>Dump information about all sessions and
     * all primitives
     * <DT><code>exit</code><DD>Shutdown server
     * <DT><code>help</code><DD>Print list of accepted commands
     * </DL><P>
     * When server is started in daemon mode, commands to the server can be sent
     * using <code>RIPCServerMonitor</code> utility
     */
    virtual void dialog();

    
    void initialize() { 
	primTableSize = primTableUsed = 0;
	primTable = NULL;
	primitives = NULL;
	sessions = NULL;
	freePrimId = NULL;
    }

    /**
     * Constructor server for remote connections. 
     * This constructor creates server socket at which client connections
     * will be accepted
     * @param port port number at which client connections will be accepted
     */
    RIPCServer(int port, int debugLevel) { 
	initialize();
	socket = RIPCSocket::create(NULL, port);
	this->port = port;
	debug = debugLevel;
        running = true;
    }

    RIPCServer() {
	initialize();
    }


    virtual ~RIPCServer();
    
    virtual void trace(char const* fmt, ...);

    /**
     * This method is executed by thread accepting client connections 
     */
    static void thread_proc acceptThread(void*);

    virtual void run(); 
    
    virtual void start(); 
    
    /**
     * Shutdown the server
     */
    void shutdown();

    enum WOB_GENERIC_FLAGS { 
	LOCKED_SERVER = 0x1000,
	TIMED_WAIT    = 0x2000
    };

    virtual void deleteEvent(RIPCEventImpl* event);
    virtual void deleteSemaphore(RIPCSemaphoreImpl* event);
    virtual void deleteMutex(RIPCMutexImpl* event);
    virtual void deleteQueue(RIPCQueueImpl* event);
    virtual void deleteSharedMemory(RIPCSharedMemoryImpl* event);
    virtual void deleteBarrier(RIPCBarrierImpl* event);
    virtual void deleteLock(RIPCLockImpl* event);


    virtual void assignId(RIPCPrimitiveImpl* prim);
    virtual void deassignId(RIPCPrimitiveImpl* prim); 

    virtual void waitNotification(RIPCWaitObject& wob, RIPCPrimitiveImpl* prim);

    virtual void waitNotificationWithTimeout(RIPCWaitObject& wob, RIPCPrimitiveImpl* prim, unsigned timeout);

    virtual RIPCSessionImpl* getSession();

    /**
     * Debug levels
     */
    enum DEBUG_LEVELS { 
	DEBUG_NONE, 
	DEBUG_EXCEPTIONS, 
	DEBUG_SESSIONS, 
	DEBUG_REQUESTS, 
	
	DEFAULT_DEBUG_LEVEL = DEBUG_EXCEPTIONS

    };
    
    inline void lock() { 
	cs.enter();
    }
    
    inline void unlock() { 
	cs.leave();
    }
    
  protected:
    virtual RIPCPrimitiveImpl*    getPrimitive(unsigned oid);

    virtual RIPCEventImpl*        getEvent(unsigned oid);
    virtual RIPCMutexImpl*        getMutex(unsigned oid);
    virtual RIPCQueueImpl*        getQueue(unsigned oid);
    virtual RIPCSemaphoreImpl*    getSemaphore(unsigned oid);
    virtual RIPCLockImpl*         getLock(unsigned oid);
    virtual RIPCSharedMemoryImpl* getSharedMemory(unsigned oid);
    virtual RIPCBarrierImpl*      getBarrier(unsigned oid);

    virtual RIPCEventImpl*        createEvent(RIPCSessionImpl* session, char const* name, 
					      bool signaled, bool manualReset, bool local);
    virtual RIPCMutexImpl*        createMutex(RIPCSessionImpl* session, char const* name, 
					      bool locked, bool local);
    virtual RIPCSemaphoreImpl*    createSemaphore(RIPCSessionImpl* session, char const* name, 
					       int initCount, bool local);
    virtual RIPCSharedMemoryImpl* createSharedMemory(RIPCSessionImpl* session, char const* name, 
						     RIPCObject const& objDesc, bool copy);
    virtual RIPCSharedMemoryImpl* createSharedMemory(RIPCSessionImpl* session, char const* name, 
						     size_t objSize);
    virtual RIPCQueueImpl*        createQueue(RIPCSessionImpl* session, char const* name, bool local);
    virtual RIPCBarrierImpl*      createBarrier(RIPCSessionImpl* session, char const* name, 
						int nSessions, bool local);
    virtual RIPCLockImpl*         createLock(RIPCSessionImpl* session, char const* name, bool local);

    virtual RIPCEventImpl*        openEvent(RIPCSessionImpl* session, char const* name);
    virtual RIPCMutexImpl*        openMutex(RIPCSessionImpl* session, char const* name);
    virtual RIPCSemaphoreImpl*    openSemaphore(RIPCSessionImpl* session, char const* name);
    virtual RIPCLockImpl*         openLock(RIPCSessionImpl* session, char const* name);
    virtual RIPCQueueImpl*        openQueue(RIPCSessionImpl* session, char const* name);
    virtual RIPCBarrierImpl*      openBarrier(RIPCSessionImpl* session, char const* name);
    virtual RIPCSharedMemoryImpl* openSharedMemory(RIPCSessionImpl* session, char const* name);


    virtual bool handleRequest(RIPCSessionImpl* session, RIPCRequest& req, RIPCResponse& resp);

    virtual void startSession(RIPCSocket* s);
    
    virtual void stopSession(RIPCSessionImpl* session);

    virtual char* strdup(char const* name);

    bool                running;
    int                 port;
    int                 debug;
    RIPCSocket*         socket;
    RIPCThread          thread;

    RIPCHashMap         semaphores;
    RIPCHashMap         barriers;
    RIPCHashMap         mutexes;
    RIPCHashMap         events;
    RIPCHashMap         queues;
    RIPCHashMap         shmems;
    RIPCHashMap         locks;


    RIPCSessionImpl*    sessions;   // L2 null terminated list of sessions
    RIPCPrimitiveImpl*  primitives; // L2 null terminated list of sessions
    RIPCPrimitiveImpl** primTable;  // array of pointer to primitives with objectId used as index
    RIPCPrimitiveImpl** freePrimId; // pointer to L1 list of free elements of primTable
    unsigned            primTableSize;
    unsigned            primTableUsed;
    RIPCCriticalSection cs;
};

#endif

