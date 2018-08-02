#ifndef __RIPC_PRIMITIVE_H__
#define __RIPC_PRIMITIVE_H__

#include "RIPCObject.h"

class RIPCSession;

/**
 * Base class for all synchronizatin primitives.
 * Semantic of <code>waitFor</code> and <code>reset</code> methods 
 * for particular primitives explained in the descripotion of these proimitives
 */
class RIPC_DLL_ENTRY RIPCPrimitive { 
  public:
    /**
     * Primitive types
     */
    enum RIPCPrimitiveType { 
	RIPC_MUTEX, 
	RIPC_EVENT, 
	RIPC_SEMAPHORE, 
	RIPC_BARRIER, 
	RIPC_LOCK, 
	RIPC_SHARED_MEMORY, 
	RIPC_QUEUE 
    };

    enum RIPCRanks { 
        DEFAULT_RANK = 0
    };

    /**
     * Get RIPC primitive type
     * @return one of the constants from  RIPCPrimitiveType enum
     */
    virtual RIPCPrimitiveType getType() = 0;

    /**
     * Get RIPC primitive type name
     * @return name of primitive type
     */
    char const* getTypeName(); 

    /**
     * Get primitive name. Name of the primitive is unique with primitives
     * of the same type (events, semaphores,...). It is possible
     * that, for example, event and mutex has the same name.
     * @return primitive name
     */
    virtual char const* getName() = 0;
    
    /**
     * Primitive returned by <code>createXXX</code> method already exists
     * This method should be call immediatly after <code>createXXX</code>
     * to check if new primitive was created or existed one was returned.
     * @return <code>true</code> if <code>createXXX</code> method doesn't
     * create new primitive
     */
    virtual bool alreadyExists() = 0;
    
    /**
     * Wait until state of primitive is switched. Semantic of this method
     * depends on particular primitive type and is explained in specification
     * of each primitive.
     */
    void waitFor() { 
        priorityWait(DEFAULT_RANK);
    }

    /**
     * Wait until state of primitive is switched with timeout. 
     * Semantic of this method depends on particular primitive type and is 
     * explained in specification of each primitive.
     * @return <code>false</code> if timeout is expired before primitive
     * state is changed
     */
    bool waitFor(unsigned timeout) { 
        return priorityWait(DEFAULT_RANK, timeout);
    }

    /**
     * Priority wait until state of primitive is switched. 
     * Requests with the lowest <code>rank</code> value will be satisfied first.
     * Semantic of this method depends on particular primitive type and is explained 
     * in specification of each primitive.
     * @param rank processes will be placed in wait queue in the order of increasing 
     * rank value and in the same order will be taken from the queue.
     */
    virtual void priorityWait(int rank) = 0;

    /**
     * Priority wait until state of primitive is switched with timeout. 
     * Requests with the lowest <code>rank</code> value will be satisfied first.
     * Semantic of this method depends on particular primitive type and is 
     * explained in specification of each primitive.
     * @param rank processes will be placed in wait queue in the order of increasing 
     * rank value and in the same order will be taken from the queue.
     * @param timeout operation timeout in millisoconds
     * @return <code>false</code> if timeout is expired before primitive
     * state is changed
     */
    virtual bool priorityWait(int rank, unsigned timeout) = 0;

    /**
     * Reset primitive.  Semantic of this method
     * depends on particular primitive type and is explained in specification
     * of each primitive.
     */
    virtual void reset() = 0;

    /**
     * Close primitive. This method decrease access counter of the primitive
     * and once it becomes zero, primitive is destroyed.
     */
    virtual void close() = 0;

    /**
     * Method for allocation memory for container elements (currently queue and shared memory).
     * This method can be overriden by derived class* to provide custom allocation policy.
     */
    virtual void* allocate(size_t size); 

    /**
     * Method for deallocation memory  for container elements (currently queue and shared memory). 
     * This method can be overriden by derived class to provide custom allocation policy.
     */
    virtual void deallocate(void* obj, size_t size=0);

    static void deallocator(RIPCObject const& objDesc);

    virtual ~RIPCPrimitive() {}
};

#endif    

