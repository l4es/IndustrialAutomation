#ifndef __RIPC_OBJECT_H__
#define __RIPC_OBJECT_H__

#include <string.h>
#include <assert.h>

#include "RIPCdef.h"

class RIPCPrimitive;

class RIPC_DLL_ENTRY RIPCObject {    
  public:
    void*  data;
    size_t size;

    RIPCObject() 
    { 
	data = NULL;
	size = 0;
	deallocator = NULL;
    }

    RIPCObject(void* buf, size_t bufSize) 
    { 
	data = buf;
	size = bufSize;
	deallocator = NULL;
    }

    void assign(RIPCObject const& cp) 
    { 
	if (data == NULL) { 
	    data = cp.data;
	    size = cp.size;
	    deallocator = cp.deallocator;
	    container = cp.container;
	} else { 
	    assert(size >= cp.size);
	    size = cp.size;
	    memcpy(data, cp.data, size);
	}
    }

    void deallocate() const 
    { 
	if (deallocator != NULL) {
	    (*deallocator)(*this);
	}
    }

    void (*deallocator)(RIPCObject const& obj);
    RIPCPrimitive* container; // container to which object belongs
};

#endif
