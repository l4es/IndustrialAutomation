/*
 *  iso_presentation.h
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
#ifndef ISO_PRESENTATION_H_
#define ISO_PRESENTATION_H_

#include "byte_buffer.h"

typedef struct {
	uint8_t contextId;
	uint8_t abstractSyntaxName[10]; /* contains OID */
} ContextDefinition;

typedef struct {
	uint32_t callingPresentationSelector;
	uint32_t calledPresentationSelector;
	ContextDefinition context[2];
	uint8_t nextContextId;
	ByteBuffer nextPayload;
} IsoPresentation;

typedef enum {
	PRESENTATION_OK,
	PRESENTATION_ERROR
} IsoPresentationIndication;

void
IsoPresentation_init(IsoPresentation* session);

IsoPresentationIndication
IsoPresentation_parseUserData(IsoPresentation* session, ByteBuffer* message);

IsoPresentationIndication
IsoPresentation_parseConnect(IsoPresentation* session, ByteBuffer* message);

void
IsoPresentation_createConnectPdu(
		IsoPresentation* self, ByteBuffer* writeBuffer, ByteBuffer* payload);

void
IsoPresentation_createCpaMessage(
		IsoPresentation* presentation,
		ByteBuffer* writeBuffer,
		ByteBuffer* payload
);


IsoPresentationIndication
isoPres_setContextDefinition(IsoPresentation* self, int index, struct Member* member);

IsoPresentationIndication
IsoPresentation_createUserData(IsoPresentation* self, ByteBuffer* writeBuffer, ByteBuffer* payload);

IsoPresentationIndication
IsoPresentation_parseAcceptMessage(IsoPresentation* self, ByteBuffer* message);

#endif /* ISO_PRESENTATION_H_ */
