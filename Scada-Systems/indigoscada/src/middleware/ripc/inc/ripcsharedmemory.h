#ifndef __RIPC_SHARED_MEMORY_H__
#define __RIPC_SHARED_MEMORY_H__

#include "RIPCPrimitive.h"

/**
 * Shared memory object. This class is used to organize
 * replication of data between all listeners of this object.
 * Listener should use wait method to detect moment when
 * object is updated by <code>set</code> method by some other session. 
 * Then it should call <code>get</code> method to get the most recent
 * version of the object. If client prefer to receive updates asynchronously,
 * it should lanunch separate thread which will preform wait in loop for
 * updates of this object, thne get the most recent version of the object
 * and somehow notify other threads that object is changed.<P>
 * Semantic of methods inherited from <code>RIPCPrimitive</code>:
 * <DL>
 * <DT><code>waitFor</code><DD>Wait until version of the object at client
 * becomes deteriorated. For local sessions, this metod always wait
 * until <code>set</code> method is invoked. For remote sessions, 
 * sequence number of the object is remembered by local stub and 
 * is sent to the server to be compared with sequence number of shared
 * memory object at the server.
 * <DT><code>reset</code><DD>Do nothing
 * </DL>
 */
class RIPC_DLL_ENTRY RIPCSharedMemory : public virtual RIPCPrimitive { 
  public:
    /**
     * Make new object visible to all other sessions using this shared memory
     * object
     * @param objDesc object descriptor of published object
     */
    virtual void set(RIPCObject const& objDesc) = 0;

    /**
     * Make new object visible to all other sessions using this shared memory
     * object
     * @param obj non NULL poiner to the object to be replicated
     * @param objSize positive object size
     */
    virtual void set(void const* obj, size_t objSize) = 0; 

    /**
     * Get the most recent version of shared memory object.
     * If <code>data</code> field of object descrtiptor is not null, 
     * then it is assumed that buffer was specified by programmer. In this case
     * <code>size</code> field of object descriptor specifies size of the buffer. 
     * If it is smaller than size of the fetched object, then RIPCInvalidParameterException
     * will bee thrown. Size of the fetched object is than assigned to <code>size</code> field
     * of the object descriptor.<BR>     
     * If <code>data</code> field was not specified (is null), then buffer is allocated by means of 
     * <code>allocate()</code> methodk. It is responsibility
     * of programmer to deallocate it (using <code>deallocate</code> method).
     * @param objDesc descriptor of retrieved object
     */
    virtual void get(RIPCObject& objDesc) = 0;

    /**
     * Get the most recent version of shared memory object.
     * Object is created by means of <code>allocate()</code> method and is byte-to-byte
     * copy of element placed in the queue (no conversions are done). It is responsibility
     * of programmer to deallocate it.
     * @return copy of shared memory object 
     */
    inline void* get() { 
	RIPCObject objDesc;
	get(objDesc);
	return (void*)objDesc.data;
    }

    RIPCPrimitiveType getType() { 
	return RIPC_SHARED_MEMORY;
    }
};

#endif
