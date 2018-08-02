//#define RIPC_IMPLEMENTATION

#include "RIPCServerFactory.h"
#include "RIPCServer.h"

RIPCServerFactory RIPCServerFactory::theFactory(RIPCServer::theServer); 

RIPCSession* RIPCServerFactory::create(char const* host, int port, int max_attempts) {
    return server.create(host, port);
}

RIPCServerFactory::RIPCServerFactory(RIPCServer& theServer) : server(theServer) {}
