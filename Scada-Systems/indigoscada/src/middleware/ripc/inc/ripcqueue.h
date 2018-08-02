#ifndef __RIPC_SQUEUE_H__
#define __RIPC_SQUEUE_H__

#include "RIPCPrimitive.h"

/**
 * FIFO queue primitive. This primitive allows to pass data between 
 * consumer and producer. Elements are fetched by consumer in First-In-First-Out
 * order. It is possible to pass objects of any type in the queue.
 * Length of message queue is unlimited.<br>
 * If more than one consumer tries to get element from the queue, 
 * thie requests will be satisfied in FIFO order.<P>
 * Semantic of methods inherited from <code>RIPCPrimitive</code>:
 * <DL>
 * <DT><code>waitFor</code><DD>Wait until queue becomes not empty. If
 * queue contains some elements then method immediatly returns. This
 * method doesn't change the state of the queue.
 * <DT><code>reset</code><DD>Removes all elements from the queue
 * </DL>
 */
class RIPC_DLL_ENTRY RIPCQueue : public virtual RIPCPrimitive { 
  public:
    /**
     * Get element from the queue. If queue is empty this method
     * waits until somebody else put element in the queue.<br>
     * If <code>data</code> field of object descrtiptor is not null, 
     * then it is assumed that buffer was specified by programmer. In this case
     * <code>size</code> field of object descriptor specifies size of the buffer. 
     * If it is smaller than size of the fetched object, then RIPCInvalidParameterException
     * will bee thrown. Size of the fetched object is than assigned to <code>size</code> field
     * of the object descriptor.<BR>     
     * If <code>data</code> field was not specified (is null), then buffer is allocated by means of 
     * <code>allocate()</code> methodk. It is responsibility
     * of programmer to deallocate it (using <code>deallocate</code> method).
     * @param objDesc object descritor for retrieved element
     */
    void get(RIPCObject& objDesc) {
        priorityGet(DEFAULT_RANK, objDesc);
    }

    /**
     * Get element from the queue. If queue is empty this method
     * waits until somebody else put element in the queue.<br>
     * Element is created by means of <code>allocate()</code> method and is byte-to-byte
     * copy of element placed in the queue (no conversions are done). It is responsibility
     * of programmer to deallocate it.
     * @return received element
     */
    void* get() { 
	return priorityGet(DEFAULT_RANK);
    }


    /**
     * Get element from the queue with timeout.  If queue is empty this method
     * waits until somebody else put element in the queue.<br>
     * Element is created by means of <code>allocate()</code> method and is byte-to-byte
     * copy of element placed in the queue (no conversions are done). It is responsibility
     * of programmer to deallocate it.
     * @param objDesc object descriptor for retrieved element
     * @param timeout timeout in milliseconds
     * @return <code>true</code> if succeed, <code>false</code> if timeout expired
     */
    bool get(RIPCObject& objDesc, unsigned timeout) { 
        return priorityGet(DEFAULT_RANK, objDesc, timeout);
    }

    /**
     * Get element from the queue with timeout.  If queue is empty this method
     * waits until somebody else put element in the queue.<br>
     * Element is created by means of <code>allocate()</code> method and is byte-to-byte
     * copy of element placed in the queue (no conversions are done). It is responsibility
     * of programmer to deallocate it.
     * @param timeout timeout in milliseconds
     * @return received element or NULL if timeout is expired before
     * any element was placed in the queue.
     */
    void* get(unsigned timeout) {
        return priorityGet(DEFAULT_RANK, timeout);
    }
    
    /**
     * Get element from the queue. If queue is empty this method
     * waits until somebody else put element in the queue.<br>
     * If <code>data</code> field of object descrtiptor is not null, 
     * then it is assumed that buffer was specified by programmer. In this case
     * <code>size</code> field of object descriptor specifies size of the buffer. 
     * If it is smaller than size of the fetched object, then RIPCInvalidParameterException
     * will bee thrown. Size of the fetched object is than assigned to <code>size</code> field
     * of the object descriptor.<BR>     
     * If <code>data</code> field was not specified (is null), then buffer is allocated by means of 
     * <code>allocate()</code> methodk. It is responsibility
     * of programmer to deallocate it (using <code>deallocate</code> method).
     * @param rank processes will be placed in wait queue in the order of increasing 
     * rank value and in the same order will be taken from the queue.
     * @param objDesc object descritor for retrieved element
     */
    virtual void priorityGet(int rank, RIPCObject& objDesc) = 0;

    /**
     * Get element from the queue. If queue is empty this method
     * waits until somebody else put element in the queue.<br>
     * Element is created by means of <code>allocate()</code> method and is byte-to-byte
     * copy of element placed in the queue (no conversions are done). It is responsibility
     * of programmer to deallocate it.
     * @param rank processes will be placed in wait queue in the order of increasing 
     * rank value and in the same order will be taken from the queue.
     * @return received element
     */
    void* priorityGet(int rank) { 
	RIPCObject objDesc;
	priorityGet(rank, objDesc);
	return (void*)objDesc.data;
    }


    /**
     * Get element from the queue with timeout.  If queue is empty this method
     * waits until somebody else put element in the queue.<br>
     * Element is created by means of <code>allocate()</code> method and is byte-to-byte
     * copy of element placed in the queue (no conversions are done). It is responsibility
     * of programmer to deallocate it.
     * @param rank processes will be placed in wait queue in the order of increasing 
     * rank value and in the same order will be taken from the queue.
     * @param objDesc object descriptor for retrieved element
     * @param timeout timeout in milliseconds
     * @return <code>true</code> if succeed, <code>false</code> if timeout expired
     */
    virtual bool priorityGet(int rank, RIPCObject& objDesc, unsigned timeout) = 0;

    /**
     * Get element from the queue with timeout.  If queue is empty this method
     * waits until somebody else put element in the queue.<br>
     * Element is created by means of <code>allocate()</code> method and is byte-to-byte
     * copy of element placed in the queue (no conversions are done). It is responsibility
     * of programmer to deallocate it.
     * @param rank processes will be placed in wait queue in the order of increasing 
     * rank value and in the same order will be taken from the queue.
     * @param timeout timeout in milliseconds
     * @return received element or NULL if timeout is expired before
     * any element was placed in the queue.
     */
    void* priorityGet(int rank, unsigned timeout) {
	RIPCObject objDesc;
	return priorityGet(rank, objDesc, timeout) ? (void*)objDesc.data : NULL;
    }
    

    /**
     * Put element at the end of the queue.
     * @param objDesc object descriptor of enqueued object
     */
    virtual void put(RIPCObject const& objDesc) = 0;

    /**
     * Put element at the end of the queue.
     * @param obj any not-null object
     * @param objSize positive object size
     */
    virtual void put(void const* obj, size_t objSize) = 0;

    /**
     * Broadcast message to all sessions currently connected to the queue.
     * The message will no be deleted from the queue until all session read this 
     * @param objDesc object descriptor of broadcasted object
     */
    virtual void broadcast(RIPCObject const& objDesc) = 0;

    /**
     * Broadcast message to all sessions currently connected to the queue.
     * The message will no be deleted from the queue until all session read this 
     * @param obj any not-null serializable object
     * @param objSize positive object size
     */
    virtual void broadcast(void const* obj, size_t objSize) = 0;

    RIPCPrimitiveType getType() { 
	return RIPC_QUEUE;
    }
};


#endif
