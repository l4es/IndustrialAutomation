#ifndef __RIPC_WIN_SOCKET_H__
#define __RIPC_WIN_SOCKET_H__

#include "RIPCSocket.h"
#include <windows.h>

#define MILLISECOND 1000

class RIPCWinSocket : public RIPCSocket {
  protected:
    SOCKET        s;

    enum error_codes {
	ok = 0,
	not_opened = -1,
	bad_address = -2,
	connection_failed = -3,
	broken_pipe = -4,
	invalid_access_mode = -5
    };

  public:
    bool      open(char const* hostname, int port, int listen_queue_size);
    bool      connect(char const* hostname, int port, int max_attempts, time_t timeout);

    int       read(void* buf, size_t min_size, size_t max_size);
    bool      write(void const* buf, size_t size);

    bool      is_ok();
    bool      close();
    char*     get_peer_name();
    bool      shutdown();
    void      get_error_text(char* buf, size_t buf_size);

    RIPCSocket* accept();

    RIPCWinSocket() {}
    RIPCWinSocket(SOCKET new_sock);

    ~RIPCWinSocket();
};

#endif
