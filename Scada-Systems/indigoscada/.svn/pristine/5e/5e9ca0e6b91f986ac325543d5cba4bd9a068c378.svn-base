#ifndef __RIPC_SERVER_FACTORY_H__
#define __RIPC_SERVER_FACTORY_H__

#include "RIPCFactory.h"

class RIPCServer;

/**
 * Factory for creation local sessions
 */
class RIPC_DLL_ENTRY RIPCServerFactory : public RIPCFactory { 
  public:
    static RIPCServerFactory* getInstance() { 
	return &theFactory;
    }
    RIPCSession* create(char const* host, int port, int max_attempts=100);
    RIPCServerFactory(RIPCServer&);

  protected:
    RIPCServer& server;
    static RIPCServerFactory theFactory;
};

#endif
