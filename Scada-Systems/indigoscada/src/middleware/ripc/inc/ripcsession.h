#ifndef __RIPC_SESSION_H__
#define __RIPC_SESSION_H__


#include "RIPCdef.h"
#include "RIPCException.h"
#include "RIPCLock.h"
#include "RIPCEvent.h"
#include "RIPCMutex.h"
#include "RIPCQueue.h"
#include "RIPCBarrier.h"
#include "RIPCSemaphore.h"
#include "RIPCSharedMemory.h"

/**
 * RIPC session. Session can be remote and local. Remote sessions
 * establish conntection with the server through TCP/IP stream socket. 
 * In this case primitives
 * at local computer servers as stubs and redirect requests to the server.
 * This mode is useful to provide sycnhronization and communication
 * between several Java processes at the same or different computers.
 * Local sessions are useful to provide synhcronization of threads within the same
 * process. The single local session can be shared by all threads.<br>
 * Session interface provides methods for creating sycnhronization objects.
 * Name of the object is unique with objects of the same type 
 * (events, semaphores,...). It is possible that, for example mutex and event
 * has the same name.
 */
class RIPC_DLL_ENTRY RIPCSession { 
   public:
    /**
     * Create or return existed event synchronization object.
     * @param name unique event name. 
     * @param signaled initial state of the event, ignored if mutex already exists
     * @param manualReset mode of the event, ignored if mutex already exists
     * @return created or existed event, use <code>alreadExists</code> method
     * to check if new object is created
     */
    virtual RIPCEvent* createEvent(char const* name, bool signaled, bool manualReset)=0;

    /**
     * Open existed event.
     * @param name event name
     * @return event or <code>null</code> if event with such name doesn't exists
     */
    virtual RIPCEvent* openEvent(char const* name)=0;

    /**
     * Create or return existed semaphore synchronization object.
     * @param name unique semaphore name. 
     * @param initCount non-negative initial value of semaphore counter, ignored if mutex already exists
     * @return created or existed semaphore, use <code>alreadExists</code> method
     * to check if new object is created
     */
    virtual RIPCSemaphore* createSemaphore(char const* name, int initCount)=0;

    /**
     * Open existed semaphore.
     * @param name semaphore name
     * @return semaphore or <code>null</code> if semaphore with such name doesn't exists
     */
    virtual RIPCSemaphore* openSemaphore(char const* name)=0;
   
    /**
     * Create or return existed mutex synchronization object.
     * @param name unique mutex name. 
     * @param locked initial state of the mutex, if <code>true</code>
     * then it is owned by session created this mutex, ignored if mutex already exists
     * @return created or existed mutex, use <code>alreadExists</code> method
     * to check if new object is created
     */
    virtual RIPCMutex* createMutex(char const* name, bool locked)=0;
   /**
     * Open existed mutex.
     * @param name mutex name
     * @return mutex or <code>null</code> if mutex with such name doesn't exists
     */
    virtual RIPCMutex* openMutex(char const* name)=0;
    
    /**
     * Create or return existed queue bject.
     * @param name unique queue name. 
     * @return created or existed queue, use <code>alreadExists</code> method
     * to check if new object is created
     */
    virtual RIPCQueue* createQueue(char const* name)=0;
   /**
     * Open existed queue.
     * @param name queue name
     * @return queue or <code>null</code> if queue with such name doesn't exists
     */
    virtual RIPCQueue* openQueue(char const* name)=0;

    /**
     * Create or return existed shared memory object.
     * @param name unique shared memory name. 
     * @param objDesc object descriptor of initial value of shared memory object
     * @return created or existed shared memory, use <code>alreadExists</code> method
     * to check if new object is created
     */
    virtual RIPCSharedMemory* createSharedMemory(char const* name, RIPCObject const& objDesc)=0;

    /**
     * Create or return existed shared memory object.
     * @param name unique shared memory name. 
     * @param obj non-null initial value of shared memory object
     * @param objSize size of object
     * @return created or existed shared memory, use <code>alreadExists</code> method
     * to check if new object is created
     */
    virtual RIPCSharedMemory* createSharedMemory(char const* name, void const* obj, size_t objSize) = 0;

    /**
     * Create or return existed shared memory object.
     * @param name unique shared memory name. 
     * @return created or existed shared memory, use <code>alreadExists</code> method
     * to check if new object is created
     */
     virtual RIPCSharedMemory* openSharedMemory(char const* name)=0;
    
    /**
     * Create or return existed lock synchronization object.
     * @param name unique lock name. 
     * @return created or existed lock, use <code>alreadExists</code> method
     * to check if new object is created
     */
    virtual RIPCLock* createLock(char const* name)=0;
    /**
     * Open existed lock.
     * @param name lock name
     * @return lock or <code>null</code> if lock with such name doesn't exists
     */
    virtual RIPCLock* openLock(char const* name)=0;

    /**
     * Create or return existed barrier synchronization object.
     * @param name unique barrier name. 
     * @param nSessions positive number of sessions to be synchronized on barrier
     * @return created or existed barrier, use <code>alreadExists</code> method
     * to check if new object is created
     */
    virtual RIPCBarrier* createBarrier(char const* name, int nSessions)=0;
    /**
     * Open existed barrier.
     * @param name barrier name
     * @return barrier or <code>null</code> if barrier with such name doesn't exists
     */
    virtual RIPCBarrier* openBarrier(char const* name)=0;

    /**
     * Close session. This method disconnects client from server, release
     * all locks hold by this session and close all primitives opened
     * by this session.<BR>
     * <b>Attention!</b>You should no call this method for local sessions
     * if it is shared by multiple threads
     */
    virtual void close()=0;

    /**
     * Terminate server. This method is used by <code>RIPCServerMonitor</code>
     * and should not be used by normal client unless it wants to monitor
     * server itself. This method do nothing for local sessions.
     * You should not execute any other session method after this this method.
     */
    virtual void shutdownServer()=0;

    /**
     * Get information about server state. This method is used by <code>RIPCServerMonitor</code>
     * and should not be used by normal client unless it wants to monitor
     * server itself.
     * @param buf buffer where to output information 
     * @param bufSize buffer size, if size of information is greater than size of the buffer, it will be trucated
     */
    virtual void showServerInfo(char* buf, size_t bufSize)=0;

    /**
     * Checks whether it is local session
     * @return <code>true</code> is session was created by means of 
     * <code>RIPCServerFactory</code> factory, <code>false</code> - if
     * session was created by  <code>RIPCClientFactory</code>.
     */
    virtual bool isLocal()=0;

    /**
     * Checks whether session is ok or not
     * @return <code>true</code> if connection is successfully established for remote session 
     * or it is local session, <code>false</code> - if conenction can not be established.
     * In last case error message can be obtained by getErrorText
     */
    virtual bool isOk()=0;
    
    /**
     * Get communication error message in case of conenction failure. 
     * This method should be used if <code>RIPCInternalException</code> exception was thrown 
     * or <code>isOk()</code> method returns false
     * @param buf buffer where to place error message
     * @param bufSize buffer size, if length of error message is greater than size of the buffer, it will be trucated
     */
    virtual void getErrorText(char* buf, size_t bufSize)=0;

    virtual ~RIPCSession() {}
};

#endif
