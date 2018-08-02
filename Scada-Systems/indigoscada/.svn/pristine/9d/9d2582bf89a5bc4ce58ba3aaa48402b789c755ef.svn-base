//-< SOCKIO.H >------------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:      7-Jan-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 23-Apr-97    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Socket abstraction
//-------------------------------------------------------------------*--------*

#ifndef __SOCKIO_H__
#define __SOCKIO_H__

#include "stdtp.h"

BEGIN_GIGABASE_NAMESPACE

#define DEFAULT_CONNECT_MAX_ATTEMPTS 100
#define DEFAULT_RECONNECT_TIMEOUT    1
#define DEFAULT_LISTEN_QUEUE_SIZE    5
#define LINGER_TIME                  10
#define WAIT_FOREVER                 ((time_t)-1)


//
// Abstract socket interface
//
class GIGABASE_DLL_ENTRY socket_t {
  public:
    bool              read(void* buf, size_t size) {
        return read(buf, size, size) == (int)size;
    }
    virtual int       read(void* buf, size_t min_size, size_t max_size,
                           time_t timeout = WAIT_FOREVER) = 0;
    virtual bool      write(void const* buf, size_t size) = 0;

    virtual bool      is_ok() = 0;
    virtual void      get_error_text(char_t* buf, size_t buf_size) = 0;

    //
    // This method is called by server to accept client connection
    //
    virtual socket_t* accept() = 0;

    //
    // Cancel accept operation and close socket
    //
    virtual bool      cancel_accept() = 0;

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
    enum socket_domain {
        sock_any_domain,   // domain is chosen automatically
        sock_local_domain, // local domain (i.e. Unix domain socket)
        sock_global_domain // global domain (i.e. INET sockets)
    };

    static socket_t*  connect(char const* address,
                              socket_domain domain = sock_global_domain,
                              int max_attempts = DEFAULT_CONNECT_MAX_ATTEMPTS,
                              time_t timeout = DEFAULT_RECONNECT_TIMEOUT);

    //
    // Create local domain socket
    //
    static socket_t*  create_local(char const* address,
                                   int listen_queue_size =
                                       DEFAULT_LISTEN_QUEUE_SIZE);

    //
    // Create global domain socket
    //
    static socket_t*  create_global(char const* address,
                                   int listen_queue_size =
                                       DEFAULT_LISTEN_QUEUE_SIZE);

    virtual ~socket_t() {}
    socket_t() { state = ss_close; }

    int   errcode;     // error code of last failed operation
    //char* address;     // host address 22-08-9
	char address[50];

  protected:
    enum { ss_open, ss_shutdown, ss_close } state;
};

END_GIGABASE_NAMESPACE

#endif



