#if defined (_WIN32)
#  include <winsock.h>
typedef int socklen_t;
typedef char raw_type;
#elif defined (_MACOSX_) || defined (__linux__)
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netdb.h>
#  include <arpa/inet.h>
#  include <unistd.h>
#  include <netinet/in.h>
#  include <stdlib.h>
typedef void raw_type;
#endif

#include "FSocket.h"
#include "FException.h"
#include "liboapc.h"

#include <string.h>
#include <errno.h>
#include <iostream>

using namespace std;
using namespace Frewitt;

#if defined(_WIN32)
static bool initialized = false;
#endif

static void fillAddr(const string& address, const unsigned short port, sockaddr_in& addr) {
	memset(&addr, 0, sizeof(addr));  // Zero out address structure
	addr.sin_family = AF_INET;       // Internet address

	hostent* host;  // Resolve name
	if (NULL == (host = gethostbyname(address.c_str()))) {
		// strerror() will not work for gethostbyname() and hstrerror()
		// is supposedly obsolete
		throw FException("FSocket", "fillAddr", "Failed to resolve name", __FILE__, __LINE__);
	}
	addr.sin_addr.s_addr = *((unsigned long*) host->h_addr_list[0]);
	addr.sin_port = htons(port);     // Assign port in network byte order
} // fillAddr

Socket::Socket(int type, int protocol) {
#if defined(_WIN32)
   if (! initialized) {
		WORD wVersionRequested;
		WSADATA wsaData;

		wVersionRequested = MAKEWORD(2, 0);              // Request WinSock v2.0
		if (0 != WSAStartup(wVersionRequested, &wsaData)) {  // Load WinSock DLL
			cerr << "DEBUG: FSocket::Constructor: Unable to load WinSock DLL." << endl;
		}
		initialized = true;
   }
#endif

	// Make a new socket
	if ((sockDesc = (int)socket(PF_INET, type, protocol)) < 0) {
	   cerr << "DEBUG: Socket::Socket: error: "<< sockDesc << endl;
	} // if
}

Socket::Socket(const int sockDesc) {
	this->sockDesc = sockDesc;
}

Socket::~Socket() {
#if defined(_WIN32)
   ::closesocket(sockDesc);
#else
   ::close(sockDesc);
#endif
	sockDesc = -1;
}

string Socket::getLocalAddress() {
	sockaddr_in addr;
	unsigned int addr_len = sizeof(addr);

	if (getsockname(sockDesc, (sockaddr*) &addr, (socklen_t*) &addr_len) < 0) {
		throw FException("FSocket", "getLocalAddress", strerror(errno), __FILE__, __LINE__);
	}
	return inet_ntoa(addr.sin_addr);
} // getLocalAddress

unsigned short Socket::getLocalPort() {
	sockaddr_in addr;
	unsigned int addr_len = sizeof(addr);

	if (getsockname(sockDesc, (sockaddr*) &addr, (socklen_t*) &addr_len) < 0) {
		throw FException("FSocket", "getLocalPort", strerror(errno), __FILE__, __LINE__);
	} // if
	return ntohs(addr.sin_port);
} // getLocalPort

void Socket::setLocalPort(const unsigned short localPort) {
	// Bind the socket to its port
	sockaddr_in localAddr;
	memset(&localAddr, 0, sizeof(localAddr));
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons(localPort);

	if (bind(sockDesc, (sockaddr*) &localAddr, sizeof(sockaddr_in)) < 0) {
		throw FException("FSocket", "setLocalPort", strerror(errno), __FILE__, __LINE__);
	} // if
} // setLocalPort

void Socket::setLocalAddressAndPort(const string& localAddress, const unsigned short localPort) {
	// Get the address of the requested host
	sockaddr_in localAddr;
	fillAddr(localAddress, localPort, localAddr);

	if (bind(sockDesc, (sockaddr*) &localAddr, sizeof(sockaddr_in)) < 0) {
		throw FException("FSocket", "setLocalAddressPort", strerror(errno), __FILE__, __LINE__);
	} // if
} // setLocalAddressAndPort

void Socket::cleanup() {
#if defined(_WIN32)
   if (0 != WSACleanup()) {
		throw FException("FSocket", "cleanup", strerror(errno), __FILE__, __LINE__);
   } // if
#endif
} // cleanup

unsigned short Socket::resolveService(const string& service, const string& protocol) {
	struct servent* serv;        /* Structure containing service information */

	if (NULL == (serv = getservbyname(service.c_str(), protocol.c_str())))
		return (unsigned short)atoi(service.c_str());  /* Service is port number */
	else
		return ntohs(serv->s_port);    /* Found port (network byte order) by name */
} // resolveService



const unsigned int CommunicatingSocket::BUFFER_SIZE_MAX = 1024;

CommunicatingSocket::CommunicatingSocket(const int type, const int protocol) : Socket(type, protocol) {
    sendlock=oapc_thread_mutex_create();
    recvlock=oapc_thread_mutex_create();
}

CommunicatingSocket::CommunicatingSocket(const int newConnSD) : Socket(newConnSD) {
    sendlock=oapc_thread_mutex_create();
    recvlock=oapc_thread_mutex_create();
}

CommunicatingSocket::~CommunicatingSocket()
{
    oapc_thread_mutex_release(sendlock);
    oapc_thread_mutex_release(recvlock);
}

void CommunicatingSocket::connect(const string& foreignAddress, const unsigned short foreignPort) {
	// Get the address of the requested host
	sockaddr_in destAddr;
	fillAddr(foreignAddress, foreignPort, destAddr);

	// Try to connect to the given port
	if (::connect(sockDesc, (sockaddr*) &destAddr, sizeof(destAddr)) < 0) {
		throw FException("FSocket", "connect", strerror(errno), __FILE__, __LINE__);
	} // if
} // connect

void CommunicatingSocket::send(const void* buffer, const int bufferLen) {
   oapc_thread_mutex_lock(sendlock);
	if (::send(sockDesc, (raw_type*) buffer, bufferLen, 0) < 0) {
	   oapc_thread_mutex_unlock(sendlock);
		throw FException("FSocket", "send", strerror(errno), __FILE__, __LINE__);
	} // if
	oapc_thread_mutex_unlock(sendlock);
} // send

int CommunicatingSocket::receive(void* buffer, int bufferLen) {
   oapc_thread_mutex_lock(recvlock);
	int rtn = 0, packn = 0;
	while ( (rtn += (packn = ::recv(sockDesc, (raw_type*)(((char*)buffer)+rtn), bufferLen-rtn, 0))) < bufferLen) {
	    if( packn < 0 ) {
	    	oapc_thread_mutex_unlock(recvlock);
	    	throw FException("FSocket", "recv", strerror(errno), __FILE__, __LINE__);
	    }
	} // if
	oapc_thread_mutex_unlock(recvlock);
	return rtn;
} // recv

bool CommunicatingSocket::ready(bool dataReady, const long timeout_ms) {
   oapc_thread_mutex_lock(recvlock);
	int rc;
	fd_set fds;
	struct timeval tv;

	FD_ZERO(&fds);
	FD_SET(sockDesc,&fds);
	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec = (timeout_ms % 1000) * 1000;

	rc = select(sockDesc+1, &fds, NULL, NULL, &tv);
	oapc_thread_mutex_unlock(recvlock);
	if( dataReady ) return (rc > 0);
	else return (1 == FD_ISSET(sockDesc,&fds));
} // ready

void CommunicatingSocket::write(const string& buffer) {
	try {
		send(buffer.c_str(), (int)buffer.length());
	} catch (const exception& e) {
		throw FException("FSocket", "write", e.what(), __FILE__, __LINE__);
	}
} // write

void CommunicatingSocket::writeln(const string& buffer) {
	try {
		write_LF(buffer);
	} catch (const exception& e) {
		throw FException("FSocket", "writeln", e.what(), __FILE__, __LINE__);
	}
} // writeln

void CommunicatingSocket::writeln() {
	try {
		write_LF("");
	} catch (const exception& e) {
		throw FException("FSocket", "writeln", e.what(), __FILE__, __LINE__);
	}
} // writeln

void CommunicatingSocket::write_LF(const string& buffer) {
	try {
      string message;
		// add LF at the end of the message
		message.append(buffer);
		message.append(1, '\n');
		send(message.c_str(), (int)message.length());
	} catch (const exception& e) {
		throw FException("FSocket", "write_LF", e.what(), __FILE__, __LINE__);
	}
} // write_LF

void CommunicatingSocket::write_CR(const string& buffer) {
	try {
      string message;
		// add CR at the end of the message
		message.append(buffer);
		message.append(1, '\r');
		send(message.c_str(), (int)message.length());
	} catch (const exception& e) {
		throw FException("FSocket", "write_CR", e.what(), __FILE__, __LINE__);
	}
} // write_CR

string CommunicatingSocket::read() {
	try {
      char buffer[BUFFER_SIZE_MAX + 1];                     // Buffer for string + \0
      int bytesReceived = (receive(buffer, BUFFER_SIZE_MAX));  // Bytes read on each recv()
      if (0 == bytesReceived) {
         // empty message
         return "";
      } else if (0 > bytesReceived) {
         throw FException("MySocket", "read", "error, size don't match", __FILE__, __LINE__);
      } else {
         buffer[bytesReceived] = '\0'; // Terminate the string
         string message = buffer;
         return message;
      } // if
	} catch (const exception& e) {
		throw FException("FSocket", "read", e.what(), __FILE__, __LINE__);
	}
	return "";
} // read

// read until a LF is received
string CommunicatingSocket::readln() {
    string message = "";
	try {
      char buffer[1];
      int bytesReceived  = 0;

      for (;;) {
         bytesReceived = (receive(buffer, 1));   // read only 1 byte
         if (0 == bytesReceived) {
            // end of stream; return string
            return message;
         } else if (0 > bytesReceived) {
            throw FException("FSocket", "readln", "error, size don't match", __FILE__, __LINE__);
         } else {
            if ('\r' == buffer[0]) {
               // ignore CR
            } else if ('\n' == buffer[0]) {
               // LF is the terminator; return string
               return message;
            } else {
               // any other char will be added to the string
               message.append(1, buffer[0]);
            } // if
         } // if
      } // while
	} catch (const exception& e) {
		throw FException("FSocket", "readln", e.what(), __FILE__, __LINE__);
	}
	return "";
} // readln

CommunicatingSocket& CommunicatingSocket::operator << ( const char* s ) {
   write(string(s));
   return *this;
}

CommunicatingSocket& CommunicatingSocket::operator << ( const std::string& s ) {
   write(s);
   return *this;
}

CommunicatingSocket& CommunicatingSocket::operator >> ( std::string& s ) {
   s = read();
   return *this;
}

string CommunicatingSocket::getForeignAddress() {
	sockaddr_in addr;
	unsigned int addr_len = sizeof(addr);

	if (getpeername(sockDesc, (sockaddr*) &addr,(socklen_t*) &addr_len) < 0) {
		throw FException("FSocket", "getForeignAddress", "failed (getpeername())", __FILE__, __LINE__);
	} // if
	return inet_ntoa(addr.sin_addr);
} // getForeignAddress

unsigned short CommunicatingSocket::getForeignPort() {
	sockaddr_in addr;
	unsigned int addr_len = sizeof(addr);

	if (getpeername(sockDesc, (sockaddr*) &addr, (socklen_t*) &addr_len) < 0) {
		throw FException("FSocket", "getForeighPort", "failed (getpeername())", __FILE__, __LINE__);
	} // if
	return ntohs(addr.sin_port);
}





UDPSocket::UDPSocket(unsigned short localPort, unsigned short destPort) :
CommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP) {
   setLocalPort(localPort);
   connect("255.255.255.255", destPort);
   int broadcast = 1;
#ifdef ENV_WINDOWS
   if (setsockopt(sockDesc, SOL_SOCKET, SO_BROADCAST,(char*)&broadcast, sizeof(broadcast)) == -1) {
#else
   if (setsockopt(sockDesc, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) == -1) {
#endif
      throw FException("FSocket", "UDPSocket:setsockopt", strerror(errno), __FILE__, __LINE__);
    }
} // TCPSocket

UDPSocket::UDPSocket(unsigned short localPort, const string& foreignAddress, unsigned short foreignPort) :
CommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP) {
   setLocalPort(localPort);
   connect(foreignAddress, foreignPort);
} // TCPSocket

void UDPSocket::connect(const string& foreignAddress, const unsigned short foreignPort) {
   // Get the address of the requested host
   fillAddr(foreignAddress, foreignPort, destAddr);
}

void UDPSocket::send(const void* buffer, const int bufferLen) {
   oapc_thread_mutex_lock(sendlock);
   if( ::sendto(sockDesc, (raw_type*) buffer, bufferLen, 0,
                          (struct sockaddr *) &destAddr,
                          sizeof(destAddr)) < 0) {
      oapc_thread_mutex_unlock(sendlock);
      throw FException("FSocket", "send", strerror(errno), __FILE__, __LINE__);
   } // if
   oapc_thread_mutex_unlock(sendlock);
} // send

int UDPSocket::receive(void* buffer, int bufferLen) {
//   recvlock.Lock();
   int rtn;
   struct sockaddr_in fromaddr;
   socklen_t destlen = sizeof(fromaddr);
   if( (rtn = ::recvfrom(sockDesc, (raw_type*) buffer, bufferLen, 0,
                        (struct sockaddr *) &fromaddr,
                        &destlen)) < 0 ) {
      oapc_thread_mutex_unlock(recvlock); // -> unlock without a lock???
      throw FException("FSocket", "recv", strerror(errno), __FILE__, __LINE__);
   } // if
   oapc_thread_mutex_unlock(recvlock); // -> unlock without a lock?
   return rtn;
} // recv





TCPSocket::TCPSocket() :
CommunicatingSocket(SOCK_STREAM, IPPROTO_TCP) {
} // TCPSocket

TCPSocket::TCPSocket(const string& foreignAddress, unsigned short foreignPort) :
CommunicatingSocket(SOCK_STREAM, IPPROTO_TCP) {
   connect(foreignAddress, foreignPort);
} // TCPSocket

TCPSocket::TCPSocket(int newConnSD) : CommunicatingSocket(newConnSD) {
}

TCPServerSocket::TCPServerSocket(unsigned short localPort, int queueLen) : Socket(SOCK_STREAM, IPPROTO_TCP) {
	setLocalPort(localPort);
	setListen(queueLen);
} // TCPSocket





TCPServerSocket::TCPServerSocket(const string& localAddress, unsigned short localPort, int queueLen) :
Socket(SOCK_STREAM, IPPROTO_TCP) {
	setLocalAddressAndPort(localAddress, localPort);
	setListen(queueLen);
} // TCPServerSocket

TCPSocket* TCPServerSocket::accept() {
	int newConnSD;
	if ((newConnSD = (int)::accept(sockDesc, NULL, 0)) < 0) {
		throw FException("FSocket", "accept", strerror(errno), __FILE__, __LINE__);
	} // if
	return new TCPSocket(newConnSD);
} // accept

void TCPServerSocket::setListen(int queueLen) {
	if (listen(sockDesc, queueLen) < 0) {
		throw FException("FSocket", "setListen", strerror(errno), __FILE__, __LINE__);
	} // if
} // setListen

