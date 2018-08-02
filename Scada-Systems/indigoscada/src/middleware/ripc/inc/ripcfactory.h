#ifndef __RIPC_FACTORY_H__
#define __RIPC_FACTORY_H__

#include "RIPCSession.h"

/**
 * RIPC session factory. This class is used to create session instances.
 * To get reference to the factory instance itself, use
 * <code>src/client/RIPCClientFactory::getInstance()</code> or
 * <code>src/server/RIPCServerFactory::getInstance()</code> methods.
 */
class RIPC_DLL_ENTRY RIPCFactory { 
  public:
    /**
     * Create session instance. For client session, connection with
     * server is established.
     * @param host server host address (ignored for local sessions)
     * @param port server port (ignored for local sessions)
     * @param max_attempts maximal connection attemps (performed with 1 second delay). 
     * This parameter makes sense opnly for client sessions.
     * @return session object
     */
    virtual RIPCSession* create(char const* host, int port, int max_attempts=100) = 0;
};

#endif
