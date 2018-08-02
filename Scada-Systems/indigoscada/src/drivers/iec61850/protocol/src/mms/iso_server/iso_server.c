/*
 *  iso_server.c
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stack_config.h"
#include "platform_types.h"
#include "mms_server_connection.h"

#include "thread.h"

#include "iso_server.h"

#define BUF_SIZE 1024
#define TCP_PORT 102
#define BACKLOG 10

//apa+++
#define inline

struct sIsoServer {
	IsoServerState state;
	ConnectionIndicationHandler connectionHandler;
	void* connectionHandlerParameter;
	AcseAuthenticationParameter authParameter;
	Thread serverThread;
	Socket serverSocket;
};


static void
isoServerThread(void* isoServerParam)
{
	IsoServer self = (IsoServer) isoServerParam;

	Socket connectionSocket;

	self->serverSocket = TcpServerSocket_create(NULL, TCP_PORT);

	if (self->serverSocket == NULL) {
		self->state = ISO_SVR_STATE_ERROR;
		goto cleanUp;
	}

	ServerSocket_setBacklog(self->serverSocket, BACKLOG);

	ServerSocket_listen(self->serverSocket);

	self->state = ISO_SVR_STATE_RUNNING;

    while (self->state == ISO_SVR_STATE_RUNNING)
    {

    	if ((connectionSocket = ServerSocket_accept(self->serverSocket)) == NULL) {
			break;;
		}
		else {
			IsoConnection isoConnection = IsoConnection_create(connectionSocket, self);

			self->connectionHandler(ISO_CONNECTION_OPENED, self->connectionHandlerParameter,
					isoConnection);

		}

	}

    self->state = ISO_SVR_STATE_STOPPED;

cleanUp:
 	self->serverSocket = NULL;
}

IsoServer
IsoServer_create()
{
	IsoServer self = calloc(1, sizeof(struct sIsoServer));

	self->state = ISO_SVR_STATE_IDLE;

	return self;
}

IsoServerState
IsoServer_getState(IsoServer self)
{
	return self->state;
}

inline void
IsoServer_setAuthenticationParameter(IsoServer self, AcseAuthenticationParameter authParameter)
{
	self->authParameter = authParameter;
}

inline AcseAuthenticationParameter
IsoServer_getAuthenticationParameter(IsoServer self)
{
	return self->authParameter;
}

void
IsoServer_startListening(IsoServer self)
{
	self->serverThread = Thread_create(isoServerThread, self, false);

	Thread_start(self->serverThread );

	while (self->state == ISO_SVR_STATE_IDLE)
		Thread_sleep(1);

	if (DEBUG) printf("new iso server thread started\n");
}

void
IsoServer_stopListening(IsoServer self)
{
	self->state = ISO_SVR_STATE_STOPPED;

	if (self->serverSocket != NULL) {
	    	ServerSocket_destroy(self->serverSocket);
	    	self->serverSocket = NULL;
	}

	if (self->serverThread != NULL)
		Thread_destroy(self->serverThread);
}

void
IsoServer_closeConnection(IsoServer self, IsoConnection isoConnection)
{
	self->connectionHandler(ISO_CONNECTION_CLOSED, self->connectionHandlerParameter,
						isoConnection);

	IsoConnection_close(isoConnection);
	IsoConnection_destroy(isoConnection);
}

void
IsoServer_setConnectionHandler(IsoServer self, ConnectionIndicationHandler handler,
		void* parameter)
{
	self->connectionHandler = handler;
	self->connectionHandlerParameter = parameter;
}

void
IsoServer_destroy(IsoServer self)
{
	if (self->state == ISO_SVR_STATE_RUNNING)
		IsoServer_stopListening(self);

	free(self);
}

