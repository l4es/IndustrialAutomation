/*
 *  socket_win32.c
 *
 *  Copyright 2013 Michael Zillgith
 *
 *	This file is part of libIEC61850.
 *
 *	libIEC61850 is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	libIEC61850 is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with libIEC61850.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	See COPYING file for the complete license text.
 */
// Modified by Enscada limited http://www.enscada.com
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>

#pragma comment (lib, "Ws2_32.lib")

#include "socket.h"
#include "stack_config.h"

struct tcp_keepalive {
    u_long  onoff;
    u_long  keepalivetime;
    u_long  keepaliveinterval;
};

#define SIO_KEEPALIVE_VALS    _WSAIOW(IOC_VENDOR,4)

struct sSocket {
	SOCKET fd;
};

struct sServerSocket {
	SOCKET fd;
	int backLog;
};

static void activateKeepAlive(SOCKET s)
{
	struct tcp_keepalive keepalive;
	DWORD retVal=0;

	keepalive.onoff = 1;
	keepalive.keepalivetime = CONFIG_TCP_KEEPALIVE_IDLE * 1000;
	keepalive.keepaliveinterval = CONFIG_TCP_KEEPALIVE_INTERVAL * 1000;

	 if (WSAIoctl(s, SIO_KEEPALIVE_VALS, &keepalive, sizeof(keepalive),
	            NULL, 0, &retVal, NULL, NULL) == SOCKET_ERROR)
	 {
	        printf("WSAIotcl(SIO_KEEPALIVE_VALS) failed; %d\n",
	            WSAGetLastError());
	 }
}

ServerSocket
TcpServerSocket_create(char* address, int port)
{
	ServerSocket serverSocket = NULL;
	int ec;
	WSADATA wsa;
	SOCKET listen_socket = INVALID_SOCKET;
	struct sockaddr_in server_addr;

	if (ec = WSAStartup(MAKEWORD(2,0), &wsa) != 0) {
		printf("winsock error: code %i\n");
		return NULL;
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

#if (CONFIG_ACTIVATE_TCP_KEEPALIVE == 1)
	activateKeepAlive(listen_socket);
#endif

	if (listen_socket == INVALID_SOCKET) {
		printf("socket failed with error: %i\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	ec = bind(listen_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

	if (ec == SOCKET_ERROR) {
		printf("bind failed with error:%i\n", WSAGetLastError());
		closesocket(listen_socket);
		WSACleanup();
		return 1;
	}



	serverSocket = malloc(sizeof(struct sServerSocket));

	serverSocket->fd = listen_socket;
	serverSocket->backLog = 10;

	return serverSocket;
}

void
ServerSocket_listen(ServerSocket socket)
{
	listen(socket->fd, socket->backLog);
}

Socket
ServerSocket_accept(ServerSocket socket)
{
	int fd;

	Socket conSocket = NULL;

	fd = accept(socket->fd, NULL, NULL);

	if (fd >= 0) {
		conSocket = TcpSocket_create();
		conSocket->fd = fd;
	}

	return conSocket;
}

void
ServerSocket_setBacklog(ServerSocket socket, int backlog)
{
	socket->backLog = backlog;
}

void
ServerSocket_destroy(ServerSocket socket)
{
	closesocket(socket->fd);
	WSACleanup();
	free(socket);
}

Socket
TcpSocket_create()
{
	Socket socket = malloc(sizeof(struct sSocket));

	socket->fd = -1;

	return socket;
}

int
Socket_connect(Socket self, char* address, int port)
{
	struct hostent *server;
	struct sockaddr_in serverAddress;
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2,0), &wsa) != 0) {
		printf("winsock error: code %i\n");
		return 0;
	}

	server = gethostbyname(address);

	memset((char *) &serverAddress, 0, sizeof(serverAddress));

	serverAddress.sin_family = AF_INET;

	memcpy((char *)&serverAddress.sin_addr.s_addr, (char *)server->h_addr, server->h_length);

	serverAddress.sin_port = htons(port);

	self->fd = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(self->fd, (struct sockaddr *) &serverAddress,sizeof(serverAddress)) < 0) {
		printf("Socket failed connecting!\n");
		return 0;
	}
	else
		return 1;
}

int
Socket_read(Socket socket, uint8_t* buf, int size)
{
	return recv(socket->fd, buf, size, 0);
}

int
Socket_write(Socket socket, uint8_t* buf, int size)
{
	return send(socket->fd, buf, size, 0);
}

void
Socket_destroy(Socket socket)
{
	if (socket->fd != -1) {
		closesocket(socket->fd);
	}

	free(socket);
}
