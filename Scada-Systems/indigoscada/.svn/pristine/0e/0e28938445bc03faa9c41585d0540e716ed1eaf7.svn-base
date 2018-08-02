//#define RIPC_IMPLEMENTATION

#include "RIPCPrimitive.h"

static char const* const RIPCPrimitiveTypeName[] = { 
    "MUTEX", 
    "EVENT", 
    "SEMAPHORE", 
    "BARRIER", 
    "LOCK", 
    "SHARED_MEMORY", 
    "QUEUE"
};

char const* RIPCPrimitive::getTypeName()
{
    return RIPCPrimitiveTypeName[getType()];
}

void* RIPCPrimitive::allocate(size_t size) 
{ 
    return new char[size];
}

void RIPCPrimitive::deallocate(void* obj, size_t) { 
    delete[] (char*)obj;
}

void RIPCPrimitive::deallocator(RIPCObject const& objDesc) 
{
    objDesc.container->deallocate((void*)objDesc.data, objDesc.size);
}

