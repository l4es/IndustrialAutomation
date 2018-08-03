

#ifndef monitor_sockets_udpH
#define monitor_sockets_udpH

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#ifdef __WIN32__
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#define NBR_MONITOR_SOCKETS 2

#define MONSOCKSLAVE 0
#define MONSOCKMASTER 1

typedef struct StrMonitorSock
{
	char InitOk;
#ifdef __WIN32__
	SOCKET Socket_fd;
#else
	int Socket_fd;
#endif
	struct sockaddr_in address_me;
	struct sockaddr_in address_other;
}StrMonitorSock;

void InitMonitorSocketsUdp( int SlaveListenPort, char InitOnlySlave );
int ReceiveMonitorFrameSocketUdp( int SocketNum, char * BufferRecv, int BuffRecvSizeMax, int TimeOutMs );
void SendMonitorFrameSocketUdp( int SocketNum, char * BufferToSend, int SizeToSend, char * HostNameToSend, short int PortToSend );
void EndMonitorSocketsUdp( void );
char IsMonitorSocketUdpInitOk( int SocketNum );

unsigned int UdpConvAdrIpCharToInt( char * ptcAdrIP );
char * UdpConvAdrIpIntToChar( unsigned int iAdrIP );

#ifdef __cplusplus
}
#endif

#endif

