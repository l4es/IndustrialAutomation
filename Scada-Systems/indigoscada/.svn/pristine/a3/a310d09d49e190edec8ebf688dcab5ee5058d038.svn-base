#ifndef __RIPC_SEMAPHORE_H__
#define __RIPC_SEMAPHORE_H__

#include "RIPCPrimitive.h"


/**
 * Classical sempahore with standard set of operations.
 * <P>
 * Semantic of methods inherited from <code>RIPCPrimitive</code>:
 * <DL>
 * <DT><code>waitFor</code><DD>Wait until sempahore counter becomes greater than
 * zero whereupon counter is decremented by 1
 * <DT><code>reset</code><DD>Reset counter to zero
 * </DL>
 */
class RIPC_DLL_ENTRY RIPCSemaphore : public virtual RIPCPrimitive { 
  public:
    /**
     * Add <code>count</code> to sempahore counter. If there are blocked session
     * one of them is awaken, decrements counter and proceed.
     * @param count positive value to be added to the semaphore counter
     */
    virtual void signal(int count=1) = 0;

    RIPCPrimitiveType getType() { 
	return RIPC_SEMAPHORE;
    }
};

#endif

