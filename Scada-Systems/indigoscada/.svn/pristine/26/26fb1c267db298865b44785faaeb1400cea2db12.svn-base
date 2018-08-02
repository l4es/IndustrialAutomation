#ifndef __RIPC_CLIENT_FACTORY_H__
#define __RIPC_CLIENT_FACTORY_H__

#include "RIPCFactory.h"

/**
 * Factory for creation client remote sessions
 */
class RIPC_DLL_ENTRY RIPCClientFactory : public RIPCFactory { 
  public:
    static RIPCClientFactory* getInstance() { 
	return &theFactory;
    }
    RIPCSession* create(char const* host, int port, int max_attempts);
    
  protected:
    static RIPCClientFactory theFactory;
};

#endif
