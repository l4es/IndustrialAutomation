/*
 *  acse.h
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

#include <stdint.h>
#include <sys/types.h>
#include "byte_buffer.h"

#ifndef ACSE_H_
#define ACSE_H_

typedef enum {
	AUTH_NONE,
	AUTH_PASSWORD
} AcseAuthenticationMechanism;

typedef struct sAcseAuthenticationParameter {
	AcseAuthenticationMechanism mechanism;
	union {
		struct {
			char* string;
		} password;
	} value;
}* AcseAuthenticationParameter;

typedef enum eAcseConnectionState {
	idle, requestIndicated, connected
} AcseConnectionState;

typedef enum eAcseIndication {
	ACSE_ERROR, ACSE_ASSOCIATE, ACSE_ASSOCIATE_FAILED, ACSE_OK
} AcseIndication;

typedef struct sAcseConnection {
	AcseConnectionState		state;
	long	            	nextReference;
	uint8_t* 				userDataBuffer;
	int 					userDataBufferSize;
	AcseAuthenticationParameter authentication;
} AcseConnection;

void
AcseConnection_init(AcseConnection* self);

void
AcseConnection_setAuthenticationParameter(AcseConnection* self, AcseAuthenticationParameter auth);

void
AcseConnection_destroy(AcseConnection* self);

AcseIndication
AcseConnection_parseMessage(AcseConnection* self, ByteBuffer* message);

void
AcseConnection_createAssociateFailedMessage(AcseConnection* connection, ByteBuffer* writeBuffer);

AcseIndication
AcseConnection_createAssociateResponseMessage(
		AcseConnection* self,
		ByteBuffer* writeBuffer,
		ByteBuffer* payload
);

void
AcseConnection_createAssociateRequestMessage(
		AcseConnection* self,
		ByteBuffer* writeBuffer,
		ByteBuffer* payload
);

#endif /* ACSE_H_ */
