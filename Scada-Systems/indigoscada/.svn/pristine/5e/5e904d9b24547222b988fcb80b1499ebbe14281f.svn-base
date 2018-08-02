/*
 *  iso_connection.c
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
#include "byte_stream.h"
#include "cotp.h"
#include "iso_session.h"
#include "iso_presentation.h"
#include "acse.h"
#include "iso_server.h"
#include "socket.h"
#include "thread.h"

#define RECEIVE_BUF_SIZE 10000
#define SEND_BUF_SIZE 10000

#define ISO_CON_STATE_RUNNING 1
#define ISO_CON_STATE_STOPPED 0

struct sIsoConnection {
	uint8_t* receive_buf;
	uint8_t* send_buf_1;
	uint8_t* send_buf_2;
	MessageReceivedHandler msgRcvdHandler;
	IsoServer isoServer;
	void* msgRcvdHandlerParameter;
	Socket socket;
	int state;
};


static void
handleTcpConnection(IsoConnection self) {

	ByteBuffer* cotpPayload;
	ByteBuffer* sessionUserData;
	ByteBuffer* acseBuffer;
	ByteBuffer mmsRequest;
	ByteBuffer* pmmsRequest;

	CotpIndication cotpIndication;
	CotpConnection cotpConnection;

	IsoSessionIndication sIndication;
	IsoSession session;

	IsoPresentation presentation;
	IsoPresentationIndication pIndication;

	AcseIndication aIndication;
	AcseConnection acseConnection;

	ByteBuffer receiveBuffer;

	ByteBuffer responseBuffer1;
	ByteBuffer responseBuffer2;

	CotpConnection_init(&cotpConnection, self->socket, &receiveBuffer);
	IsoSession_init(&session);
	IsoPresentation_init(&presentation);
	AcseConnection_init(&acseConnection);
	AcseConnection_setAuthenticationParameter(&acseConnection,
			IsoServer_getAuthenticationParameter(self->isoServer));

	while (self->msgRcvdHandlerParameter == NULL)
		Thread_sleep(1);

	printf("IsoConnection: state = RUNNING. Start to handle connection\n");

	while (self->state == ISO_CON_STATE_RUNNING) {
		ByteBuffer_wrap(&receiveBuffer, self->receive_buf, 0, RECEIVE_BUF_SIZE);
		ByteBuffer_wrap(&responseBuffer1, self->send_buf_1, 0, SEND_BUF_SIZE);
		ByteBuffer_wrap(&responseBuffer2, self->send_buf_2, 0, SEND_BUF_SIZE);

		cotpIndication = CotpConnection_parseIncomingMessage(&cotpConnection);

		switch (cotpIndication) {
		case CONNECT_INDICATION:
			if (DEBUG) printf("COTP connection indication\n");
			CotpConnection_sendConnectionResponseMessage(&cotpConnection);
			break;
		case DATA_INDICATION:
			if (DEBUG) printf("COTP data indication\n");

			cotpPayload = CotpConnection_getPayload(&cotpConnection);

			sIndication = IsoSession_parseMessage(&session, cotpPayload);

			sessionUserData = IsoSession_getUserData(&session);

			switch (sIndication) {
			case SESSION_CONNECT:
				if (DEBUG) printf("cotp_server: session connect indication\n");

				pIndication = IsoPresentation_parseConnect(&presentation, sessionUserData);

				if (pIndication == PRESENTATION_OK) {
					if (DEBUG) printf("cotp_server: presentation ok\n");

					acseBuffer = &(presentation.nextPayload);

					aIndication = AcseConnection_parseMessage(&acseConnection, acseBuffer);

					if (aIndication == ACSE_ASSOCIATE) {
						if (DEBUG) printf("cotp_server: acse associate\n");

						
						ByteBuffer_wrap(&mmsRequest, acseConnection.userDataBuffer,
								acseConnection.userDataBufferSize, acseConnection.userDataBufferSize);

						self->msgRcvdHandler(self->msgRcvdHandlerParameter,
													&mmsRequest, &responseBuffer1);

						if (responseBuffer1.size > 0) {
							if (DEBUG) printf("cotp_server: application payload size: %i\n",
									responseBuffer1.size);

							AcseConnection_createAssociateResponseMessage(&acseConnection, &responseBuffer2,
									&responseBuffer1);

							responseBuffer1.size = 0;

							IsoPresentation_createCpaMessage(&presentation, &responseBuffer1,
									&responseBuffer2);

							responseBuffer2.size = 0;

							IsoSession_createAcceptSpdu(&session, &responseBuffer2,
									responseBuffer1.size);

							ByteBuffer_append(&responseBuffer2, responseBuffer1.buffer,
									responseBuffer1.size);

							CotpConnection_sendDataMessage(&cotpConnection, &responseBuffer2);

							break;
						}
						else {
							if (DEBUG) printf("cotp_server: association error. No response from application!\n");
						}
					}
					else {
						if (DEBUG) printf("cotp_server: acse association failed\n");
						self->state = ISO_CON_STATE_STOPPED;
					}

				}
				break;
			case SESSION_DATA:
				if (DEBUG) printf("cotp_server: session data indication\n");

				pIndication = IsoPresentation_parseUserData(&presentation, sessionUserData);

				if (pIndication == PRESENTATION_ERROR) {
					if (DEBUG) printf("cotp_server: presentation error\n");
					self->state = ISO_CON_STATE_STOPPED;
					break;
				}

				if (presentation.nextContextId == 3) {
					if (DEBUG) printf("cotp_server: mms message\n");

					pmmsRequest = &(presentation.nextPayload);

					self->msgRcvdHandler(self->msgRcvdHandlerParameter,
													pmmsRequest, &responseBuffer1);

					IsoPresentation_createUserData(&presentation,
							&responseBuffer2, &responseBuffer1);

					responseBuffer1.size = 0;

					IsoSession_createDataSpdu(&session, &responseBuffer1);

					ByteBuffer_append(&responseBuffer1, responseBuffer2.buffer,
							responseBuffer2.size);

					CotpConnection_sendDataMessage(&cotpConnection, &responseBuffer1);
				}

				break;
			case SESSION_ERROR:
				self->state = ISO_CON_STATE_STOPPED;
				break;
			}
			break;
		case ERROR:
			if (DEBUG) printf("COTP protocol error\n");
			self->state = ISO_CON_STATE_STOPPED;
			break;
		default:
			if (DEBUG) printf("COTP Unknown Indication: %i\n", cotpIndication);
			self->state = ISO_CON_STATE_STOPPED;
			break;
		}
	}

	Socket_destroy(self->socket);

	//if (DEBUG)
		printf("IsoConnection: connection closed!\n");

	AcseConnection_destroy(&acseConnection);

	IsoServer_closeConnection(self->isoServer, self);

	CotpConnection_destroy(&cotpConnection);
}

IsoConnection
IsoConnection_create(Socket socket, IsoServer isoServer)
{
	Thread thread;

	IsoConnection self = calloc(1, sizeof(struct sIsoConnection));
	self->socket = socket;
	self->receive_buf = malloc(RECEIVE_BUF_SIZE);
	self->send_buf_1 = malloc(SEND_BUF_SIZE);
	self->send_buf_2 = malloc(SEND_BUF_SIZE);
	self->msgRcvdHandler = NULL;
	self->msgRcvdHandlerParameter = NULL;
	self->isoServer = isoServer;
	self->state = ISO_CON_STATE_RUNNING;

	thread = Thread_create(handleTcpConnection, self, true);

	Thread_start(thread);

	if (DEBUG) printf("new iso connection thread started\n");

	return self;
}

void
IsoConnection_close(IsoConnection self)
{
	self->state = ISO_CON_STATE_STOPPED;
}

void
IsoConnection_installListener(IsoConnection self, MessageReceivedHandler handler,
		void* parameter)
{
	self->msgRcvdHandler = handler;
	self->msgRcvdHandlerParameter = parameter;
}

void
IsoConnection_destroy(IsoConnection self)
{
	free(self->receive_buf);
	free(self->send_buf_1);
	free(self->send_buf_2);
	free(self);
}

