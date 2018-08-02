//#define RIPC_IMPLEMENTATION

#include "RIPCWinSocket.h"
#include <stdio.h>

class RIPCWinSocketLibrary {
  public:
    SYSTEM_INFO sinfo;

    RIPCWinSocketLibrary() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
	    fprintf(stderr,"Failed to initialize windows sockets: %d\n",
		    WSAGetLastError());
	}
    }
    ~RIPCWinSocketLibrary() {
	//	WSACleanup();
    }
};

static RIPCWinSocketLibrary ws32_lib;


bool RIPCWinSocket::open(char const* hostname, int port, int listen_queue_size)
{
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
	errcode = WSAGetLastError();
	return false;
    }
    struct sockaddr_in insock;
    insock.sin_family = AF_INET;
    if (hostname != NULL && *hostname != '\0' && strcmp(hostname, "localhost") != 0) {
	struct hostent* hp;  // entry in hosts table
	if ((hp = gethostbyname(hostname)) == NULL
	    || hp->h_addrtype != AF_INET)
	{
	    errcode = bad_address;
	    return false;
	}
	memcpy(&insock.sin_addr, hp->h_addr, sizeof insock.sin_addr);
    } else {
	insock.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    insock.sin_port = htons(port);

    int on = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof on);

    if (bind(s, (sockaddr*)&insock, sizeof(insock)) != 0) {
	errcode = WSAGetLastError();
	closesocket(s);
	return false;
    }
    if (listen(s, listen_queue_size) != 0) {
	errcode = WSAGetLastError();
	closesocket(s);
	return false;
    }
    errcode = ok;
    state = ss_open;
    return true;
}

bool RIPCWinSocket::is_ok()
{
    return errcode == ok;
}

void RIPCWinSocket::get_error_text(char* buf, size_t buf_size)
{
    int   len;
    char* msg;
    char  msgbuf[64];

    switch(errcode) {
      case ok:
        msg = "ok";
	break;
      case not_opened:
	msg = "socket not opened";
        break;
      case bad_address:
	msg = "bad address";
	break;
      case connection_failed:
	msg = "exceed limit of attempts of connection to server";
	break;
      case broken_pipe:
	msg = "connection is broken";
	break;
      case invalid_access_mode:
        msg = "invalid access mode";
	break;
      default:
	len = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
			    NULL,
			    errcode,
			    0,
			    buf,
			    buf_size,
			    NULL);
	if (len == 0) {
            sprintf(msgbuf, "unknown error code %u", errcode);
	    msg = msgbuf;
	} else {
	    return;
	}
    }
    strncpy(buf, msg, buf_size);
}

RIPCSocket* RIPCWinSocket::accept()
{
    if (state != ss_open) {
	errcode = not_opened;
	return NULL;
    }

    SOCKET new_sock = ::accept(s, NULL, NULL );

    if (new_sock == INVALID_SOCKET) {
	errcode = WSAGetLastError();
	return NULL;
    } else {
	static struct linger l = {1, LINGER_TIME};
	if (setsockopt(new_sock, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof l) != 0) {
	    errcode = WSAGetLastError();
	    closesocket(new_sock);
	    return NULL;
	}
	int enabled = 1;
	if (setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&enabled,
		       sizeof enabled) != 0)
	{
	    errcode = WSAGetLastError();
	    closesocket(new_sock);
	    return NULL;
	}
	errcode = ok;
	return new RIPCWinSocket(new_sock);
    }
}


bool RIPCWinSocket::connect(char const* hostname, int port, int max_attempts, time_t timeout)
{
    struct sockaddr_in insock;  // inet socket address
    struct hostent*    hp;      // entry in hosts table

    unsigned int addr = inet_addr(hostname);

    if (addr == INADDR_NONE) {
        if ((hp = gethostbyname(hostname)) == NULL || hp->h_addrtype != AF_INET) {
	    errcode = bad_address;
    	    return false;
        }
    }
    insock.sin_family = AF_INET;
    insock.sin_port = htons(port);

    while (true) {
	for (int i = 0; addr != INADDR_NONE || hp->h_addr_list[i] != NULL; i++) {
            if (addr == INADDR_NONE) { 
                memcpy(&insock.sin_addr, hp->h_addr_list[i],
                       sizeof insock.sin_addr);
            } else {
                memcpy(&insock.sin_addr, &addr, sizeof insock.sin_addr);
            }
	    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		errcode = WSAGetLastError();
		return false;
	    }
	    if (::connect(s, (sockaddr*)&insock, sizeof insock) != 0) {
		errcode = WSAGetLastError();
		closesocket(s);
		if (errcode != WSAECONNREFUSED) {
		    return false;
		}
	    } else {
		int enabled = 1;
                static struct linger l = {1, LINGER_TIME};
                if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&enabled,
			       sizeof enabled) != 0)
		{
		    errcode = WSAGetLastError();
		    closesocket(s);
		    return false;
		}
                if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof l) != 0) {
                    errcode = WSAGetLastError();
                    closesocket(s);
                    return false;
                }
		errcode = ok;
		state = ss_open;
		return true;
	    }
	}
	if (--max_attempts > 0) {
	    Sleep(timeout*MILLISECOND);
	} else {
	    errcode = connection_failed;
	    return false;
	}
    }
}

#include <assert.h>

int RIPCWinSocket::read(void* buf, size_t min_size, size_t max_size)
{
    size_t size = 0;
    if (state != ss_open) {
	errcode = not_opened;
	return -1;
    }
    do {
	int rc = recv(s, (char*)buf + size, max_size - size, 0);
	if (rc < 0) {
	    errcode = WSAGetLastError();
	    return -1;
	} else if (rc == 0) {
	    errcode = broken_pipe;
	    return -1;
	} else {
	    size += rc;
	}
    } while (size < min_size);

    return (int)size;
}

bool RIPCWinSocket::write(void const* buf, size_t size)
{
    if (state != ss_open) {
	errcode = not_opened;
	return false;
    }

    do {
	int rc = send(s, (char*)buf, size, 0);
	if (rc < 0) {
	    errcode = WSAGetLastError();
	    return false;
	} else if (rc == 0) {
	    errcode = broken_pipe;
	    return false;
	} else {
	    buf = (char*)buf + rc;
	    size -= rc;
	}
    } while (size != 0);

    return true;
}

bool RIPCWinSocket::shutdown()
{
    if (state == ss_open) {
	state = ss_shutdown;
        int rc = ::shutdown(s, 2);
        if (rc != 0) {
	    errcode = WSAGetLastError();
	    return false;
	}
    }
    errcode = ok;
    return true;
}


bool RIPCWinSocket::close()
{
    if (state != ss_close) {
	state = ss_close;
	if (closesocket(s) == 0) {
	    errcode = ok;
	    return true;
	} else {
	    errcode = WSAGetLastError();
	    return false;
	}
    }
    return true;
}

char* RIPCWinSocket::get_peer_name()
{
    if (state != ss_open) {
	errcode = not_opened;
	return NULL;
    }
    struct sockaddr_in insock;
    int len = sizeof(insock);
    if (getpeername(s, (struct sockaddr*)&insock, &len) != 0) {
	errcode = WSAGetLastError();
	return NULL;
    }
    char* addr = inet_ntoa(insock.sin_addr);
    if (addr == NULL) {
	errcode = WSAGetLastError();
	return NULL;
    }
    char* addr_copy = new char[strlen(addr)+1];
    strcpy(addr_copy, addr);
    errcode = ok;
    return addr_copy;
}

RIPCWinSocket::~RIPCWinSocket()
{
    close();
}

RIPCWinSocket::RIPCWinSocket(SOCKET new_sock)
{
    s = new_sock;
    state = ss_open;
    errcode = ok;
}

RIPCSocket* RIPCSocket::create(char const* hostname, int port, int listen_queue_size)
{
    RIPCWinSocket* sock = new RIPCWinSocket();
    sock->open(hostname, port, listen_queue_size);
    return sock;
}

RIPCSocket* RIPCSocket::connect(char const* hostname, int port,
				int max_attempts,
				time_t timeout)
{
    RIPCWinSocket* s = new RIPCWinSocket();
    s->connect(hostname, port, max_attempts, timeout);
    return s;
}

