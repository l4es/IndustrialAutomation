//#define RIPC_IMPLEMENTATION

#include "RIPCClientFactory.h"
#include "RIPCSessionStub.h"

RIPCClientFactory RIPCClientFactory::theFactory;

RIPCSession* RIPCClientFactory::create(char const* host, int port, int max_attempts) 
{
    return new RIPCSessionStub(host, port, max_attempts);
}
