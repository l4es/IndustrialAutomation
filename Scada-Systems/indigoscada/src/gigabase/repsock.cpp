//-< REPSOCK.CPP >---------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:      3-May-2003  K.A. Knizhnik  * / [] \ *
//                          Last update:  6-May-2003  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Replication socket implementation
//-------------------------------------------------------------------*--------*

#define INSIDE_GIGABASE

#include "repsock.h"
#include "sync.h"

BEGIN_GIGABASE_NAMESPACE

void replication_socket_t::handleError(int socket, const char_t* operation, const char_t* error)
{
    FPRINTF(stderr, _T("Operation %s failed for socket %d: %s\n"), operation, socket, error);
}

int replication_socket_t::read(void* buf, size_t min_size, size_t max_size, time_t timeout)
{
    dbSmallBuffer<byte> receiveBuf(min_size*n_sockets);
    dbSmallBuffer<int>  matchBuf(n_sockets);
    byte* rcv = receiveBuf.base();
    int*  matches = matchBuf.base();
    int i, j, n = n_sockets;

    for (i = 0; i < n_sockets; i++) { 
        matches[i] = -1;
        if (sockets[i] != NULL) { 
            size_t received = 0;
            while (received < min_size) { 
                int rc = sockets[i]->read(rcv + i*min_size + received, min_size - received, min_size - received, timeout);
                if (rc <= 0) { 
                    char_t msg[64];
                    sockets[i]->get_error_text(msg, sizeof(msg));
                    handleError(i, _T("read"), msg);
                    delete sockets[i];
                    sockets[i] = NULL;
                    break;
                }
                received += rc;
            }
            if (received == min_size) { 
                matches[i] = 0;
                for (j = 0; j < i; j++) { 
                    if (matches[j] == 0) {
                        if (memcmp(rcv + j*min_size, rcv + i*min_size, min_size) == 0) { 
                            matches[j] = i;
                            break;
                        }
                    }
                }
            }
        }
    }
    int maxVotes = 0;
    int correctResponse = -1;
    for (i = 0; i < n; i++) { 
        if (matches[i] >= 0) {
            int nVotes = 0;
            j = i;
            do {
                int next = matches[j];
                nVotes += 1;
                matches[j] = -1;
                j = next;
            } while (j != 0);

            if (nVotes > maxVotes) { 
                maxVotes = nVotes;
                correctResponse = i;
            } else if (nVotes == maxVotes) { 
                correctResponse = -1;
            }
        }
    }
    if (correctResponse >= 0) { 
        succeed = true;
        memcpy(buf, rcv + correctResponse*min_size, min_size);
        return min_size;
    } else { 
        handleError(-1, _T("read"), _T("failed to choose correct response"));
        succeed = false;
        return -1;
    } 
}

bool replication_socket_t::write(void const* buf, size_t size)
{
    succeed = false;
    for (int i = n_sockets; --i >= 0;) { 
        if (sockets[i] != NULL) { 
            if (sockets[i]->write(buf, size)) { 
                succeed = true;
            } else { 
                char_t msg[64];
                sockets[i]->get_error_text(msg, sizeof(msg));
                handleError(i, _T("write"), msg);
                delete sockets[i];
                sockets[i] = NULL;
            }
        }
    }
    return succeed;
}

bool replication_socket_t::is_ok()
{
    return succeed;
}

void replication_socket_t::get_error_text(char_t* buf, size_t buf_size)
{
    STRNCPY(buf, succeed ? _T("ok") : _T("failed to select valid server"), buf_size);
}

bool replication_socket_t::shutdown()
{
    succeed = false;
    for (int i = n_sockets; --i >= 0;) { 
        if (sockets[i] != NULL) { 
            if (sockets[i]->shutdown()) { 
                succeed = true;
            } else { 
                char_t msg[64];
                sockets[i]->get_error_text(msg, sizeof(msg));
                handleError(i, _T("shutdown"), msg);
                delete sockets[i];
                sockets[i] = NULL;
            }
        }
    }
    return succeed;
}    

bool replication_socket_t::close()
{
    succeed = false;
    for (int i = n_sockets; --i >= 0;) { 
        if (sockets[i] != NULL) { 
            if (sockets[i]->close()) { 
                succeed = true;
            } else { 
                char_t msg[64];
                sockets[i]->get_error_text(msg, sizeof(msg));
                handleError(i, _T("close"), msg);
                delete sockets[i];
                sockets[i] = NULL;
            }
        }
    }
    return succeed;
}    

replication_socket_t*  replication_socket_t::connect(char const* addresses[],
                                                     int n_addresses, 
                                                     int max_attempts, 
                                                     time_t timeout)
{
    return new replication_socket_t(addresses, n_addresses, max_attempts, timeout);
}

replication_socket_t::replication_socket_t(char const* addresses[], int n_addresses, int max_attempts, time_t timeout)
{
    n_sockets = n_addresses;
    sockets = new socket_t*[n_addresses];
    for (int i = n_addresses; --i >= 0;) { 
        socket_t* s = socket_t::connect(addresses[i], socket_t::sock_global_domain, max_attempts, timeout);
        if (s != NULL) { 
            if (s->is_ok()) {
                succeed = true;
            } else { 
                char_t msg[64];
                s->get_error_text(msg, sizeof(msg));
                handleError(i, _T("connect"), msg);
                delete s;
                s = NULL;
            }
        } else { 
            handleError(i, _T("connect"), _T("failed to create socket"));
        }
        sockets[i] = s;
    }
}

socket_t* replication_socket_t::accept()
{
    return NULL;
}

bool replication_socket_t::cancel_accept()
{
    return false;
}

char* replication_socket_t::get_peer_name()
{
    return NULL;
}

replication_socket_t::~replication_socket_t()
{
    if (sockets != NULL) { 
        for (int i = n_sockets; --i >= 0;) { 
            delete sockets[i];
        }
    }
    delete[] sockets;
}

END_GIGABASE_NAMESPACE

