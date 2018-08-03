/* Classic Ladder Project */
/* Copyright (C) 2001-2015 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* February 2011 */
/* ------------------------------------------------------ */
/* Monitor protocol - Sockets UDP for both slave/master   */
/* ------------------------------------------------------ */
/* This library is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU Lesser General Public */
/* License as published by the Free Software Foundation; either */
/* version 2.1 of the License, or (at your option) any later version. */

/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU */
/* Lesser General Public License for more details. */

/* You should have received a copy of the GNU Lesser General Public */
/* License along with this library; if not, write to the Free Software */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include <stdio.h>
#include <string.h>

#include "classicladder.h"
#include "monitor_sockets_udp.h"

#ifndef __WIN32__
#define SOCKET_ERROR -1
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#endif

//sockets usage: socket 0 = slave, socket 1 = master
StrMonitorSock MonitorSocket[ NBR_MONITOR_SOCKETS ];

void InitMonitorSocketsUdp( int SlaveListenPort, char InitOnlySlave )
{
	// WSAStartup for Windows already done for the historical modbus socket server...
	
	int ScanSocket;
	for( ScanSocket=0; ScanSocket<NBR_MONITOR_SOCKETS; ScanSocket++ )
	{
		StrMonitorSock * pMonSock = &MonitorSocket[ ScanSocket ];
		pMonSock->InitOk = FALSE;
		pMonSock->Socket_fd = SOCKET_ERROR;

		if ( !InitOnlySlave || ( ScanSocket==MONSOCKSLAVE && InitOnlySlave ) )
		{
			pMonSock->Socket_fd = socket( AF_INET, SOCK_DGRAM, 0 );
			if ( pMonSock->Socket_fd!=SOCKET_ERROR )
			{
				pMonSock->address_me.sin_family = AF_INET;
				pMonSock->address_me.sin_addr.s_addr = htonl( INADDR_ANY );
				pMonSock->address_me.sin_port = (ScanSocket==MONSOCKSLAVE)?htons(SlaveListenPort):0;

				if ( bind( pMonSock->Socket_fd, (struct sockaddr *)&pMonSock->address_me, sizeof( pMonSock->address_me ) )!=SOCKET_ERROR )
					pMonSock->InitOk = TRUE;
				else
					printf("monitor: socket init error for %s !!!\n", (ScanSocket==MONSOCKSLAVE)?"slave":"master" );
			}
		}
	}
}

//0 for TimeOutMs for infinite wait...(used in slave mode)
int ReceiveMonitorFrameSocketUdp( int SocketNum, char * BufferRecv, int BuffRecvSizeMax, int TimeOutMs )
{
	int NbrCharReceived = 0;
	if ( SocketNum>=0 && SocketNum<NBR_MONITOR_SOCKETS )
	{
		StrMonitorSock * pMonSock = &MonitorSocket[ SocketNum ];
		if ( pMonSock->InitOk && pMonSock->Socket_fd!=SOCKET_ERROR )
		{
			int DecripRecv = 999;
			if( TimeOutMs>0 )
			{
				fd_set myset;
				struct timeval tv;
				FD_ZERO( &myset);
				// adding descriptor to survey and set timeout wanted...
				FD_SET( pMonSock->Socket_fd, &myset );
				tv.tv_sec = 0; // secs
				tv.tv_usec = TimeOutMs*1000; // usecs
				DecripRecv = select( /*16*/pMonSock->Socket_fd+1, &myset, NULL, NULL, &tv );
			}
			if ( DecripRecv>0 )
			{
				unsigned int LongSrv = sizeof(pMonSock->address_other);
				NbrCharReceived = recvfrom( pMonSock->Socket_fd, BufferRecv, BuffRecvSizeMax, 0,
					(struct sockaddr *)&pMonSock->address_other, &LongSrv );
			}
		}
	}
	return NbrCharReceived;
}

void SendMonitorFrameSocketUdp( int SocketNum, char * BufferToSend, int SizeToSend, char * HostNameToSend , short int PortToSend )
{
	if ( SocketNum>=0 && SocketNum<NBR_MONITOR_SOCKETS )
	{
		StrMonitorSock * pMonSock = &MonitorSocket[ SocketNum ];
		if ( pMonSock->InitOk )
		{
			// if slave, respond directly to the one who asked the question...
			if ( SocketNum==MONSOCKMASTER )
			{
				pMonSock->address_other.sin_family = AF_INET;
				int IpAdrToSend = UdpConvAdrIpCharToInt( HostNameToSend );
				pMonSock->address_other.sin_addr.s_addr = IpAdrToSend;
				pMonSock->address_other.sin_port = htons(PortToSend);
			}
			sendto(pMonSock->Socket_fd, BufferToSend, SizeToSend, 0,
						 (struct sockaddr*)&pMonSock->address_other, sizeof(pMonSock->address_other));
		}
	}
}

void EndMonitorSocketsUdp( void )
{
	int ScanSocket;
	for( ScanSocket=0; ScanSocket<NBR_MONITOR_SOCKETS; ScanSocket++ )
	{
		StrMonitorSock * pMonSock = &MonitorSocket[ ScanSocket ];
		if ( pMonSock->Socket_fd!=SOCKET_ERROR )
#ifdef __WIN32__
			closesocket( pMonSock->Socket_fd );
#else
			close( pMonSock->Socket_fd );
#endif
		pMonSock->InitOk = FALSE;
	}
}

char IsMonitorSocketUdpInitOk( int SocketNum )
{
	return MonitorSocket[ SocketNum ].InitOk;
}

unsigned int UdpConvAdrIpCharToInt( char * ptcAdrIP )
{
	unsigned int adr;
	// search from hosts file before...
	struct hostent * host;
	host = gethostbyname( ptcAdrIP );
	if ( host )
		memcpy( &adr, host->h_addr, host->h_length );
	else
		adr = inet_addr( ptcAdrIP );
	return adr;
}

/*
char * UdpConvAdrIpIntToChar( unsigned int iAdrIP )
{
	struct sockaddr_in adrsock;
	adrsock.sin_addr.s_addr = iAdrIP;
	return inet_ntoa( adrsock.sin_addr );
}
*/
