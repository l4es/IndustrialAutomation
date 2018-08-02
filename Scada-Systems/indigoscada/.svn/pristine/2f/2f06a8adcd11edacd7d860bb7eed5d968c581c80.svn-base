#ifndef __RIPC_SOCKET_H__
#define __RIPC_SOCKET_H__

#include "RIPCdef.h"

#include <time.h>

#define DEFAULT_CONNECT_MAX_ATTEMPTS 100
#define DEFAULT_RECONNECT_TIMEOUT    1
#define DEFAULT_LISTEN_QUEUE_SIZE    5
#define LINGER_TIME                  10


//
// Abstract socket interface
//
class RIPC_DLL_ENTRY RIPCSocket {
  public:
    bool              read(void* buf, size_t size) {
	return read(buf, size, size) == (int)size;
    }
    virtual int       read(void* buf, size_t min_size, size_t max_size) = 0;
    virtual bool      write(void const* buf, size_t size) = 0;

    virtual bool      is_ok() = 0;
    virtual void      get_error_text(char* buf, size_t buf_size) = 0;

    //
    // This method is called by server to accept client connection
    //
    virtual RIPCSocket* accept() = 0;

    //
    // Shutdown socket: prohibite write and read operations on socket
    //
    virtual bool      shutdown() = 0;

    //
    // Close socket
    //
    virtual bool      close() = 0;

    //
    // Get socket peer name.
    // name is created using new char[]. If perr name can not be obtained NULL
    // is returned and errcode is set.
    //
    virtual char*     get_peer_name() = 0;

    //
    // Create client socket connected to local or global server socket
    //
    static RIPCSocket*  connect(char const* host, int port,
				int max_attempts = DEFAULT_CONNECT_MAX_ATTEMPTS,
				time_t timeout = DEFAULT_RECONNECT_TIMEOUT);

    static RIPCSocket*  create(char const* host, int port,
			       int listen_queue_size = DEFAULT_LISTEN_QUEUE_SIZE);
    
    virtual ~RIPCSocket() {}
    RIPCSocket() { state = ss_close; }

    int   errcode;     // error code of last failed operation

  protected:
    enum { ss_open, ss_shutdown, ss_close } state;
};

#endif



