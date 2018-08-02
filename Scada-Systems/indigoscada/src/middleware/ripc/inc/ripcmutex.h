#ifndef __RIPC_MUTEX_H__
#define __RIPC_MUTEX_H__

#include "RIPCPrimitive.h"


/**
 * Synchronization object providing mutual exclusion.
 * Mutex object supports nested locks, it means that if 
 * some session locks mutex <code>N</code> times, it should
 * unlock it <code>N</code> time to release the mutex.
 * Server is able to detect deadlock for mutexes.<P>
 * Semantic of methods inherited from <code>RIPCPrimitive</code>:
 * <DL>
 * <DT><code>waitFor</code><DD>lock the mutex
 * <DT><code>reset</code><DD>release the mutex (remove all locks).
 * It is not required that session, invoking <code>reset</code> method be owner
 * of the mutex
 * </DL>
 */
class RIPC_DLL_ENTRY RIPCMutex : public virtual RIPCPrimitive { 
  public:
    /**
     * Lock mutex. Only one session can lock mutex each moment of time.
     */
    void lock() { 
        priorityLock(DEFAULT_RANK);
    }
    
    /**
     * Try to lock the mutex with specified timeout. If lock can not be granted
     * within specified time, request is failed.
     * @param timeout timeout in milliseconds
     * @return <code>true</code> if lock is granted, <code>false</code> of timeout
     * is expired
     */
    bool lock(unsigned timeout) {
        return priorityLock(DEFAULT_RANK, timeout);
    }
       
    /**
     * Lock mutex. Only one session can lock mutex each moment of time.
     * @param rank processes will be placed in wait queue in the order of increasing 
     * rank value and in the same order will be taken from the queue.
     */
    virtual void priorityLock(int rank) = 0;
    
    /**
     * Try to lock the mutex with specified timeout. If lock can not be granted
     * within specified time, request is failed.
     * @param rank processes will be placed in wait queue in the order of increasing 
     * rank value and in the same order will be taken from the queue.
     * @param timeout timeout in milliseconds
     * @return <code>true</code> if lock is granted, <code>false</code> of timeout
     * is expired
     */
    virtual bool priorityLock(int rank, unsigned timeout) = 0;

    /**
     * Unlock mutex. This method release the mutex if number of unlocks
     * is equal to number of locks
     */
    virtual void unlock() = 0;

    RIPCPrimitiveType getType() { 
	return RIPC_MUTEX;
    }
};

#endif
