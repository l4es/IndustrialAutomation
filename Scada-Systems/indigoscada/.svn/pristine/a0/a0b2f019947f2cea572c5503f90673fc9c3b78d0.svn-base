/*
 *  mms_server_internal.h
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

#ifndef MMS_SERVER_INTERNAL_H_
#define MMS_SERVER_INTERNAL_H_

#include <stdint.h>
//#include <stdlib.h>
#include <MmsPdu.h>
#include "mms_common.h"
#include "mms_server_connection.h"
#include "mms_device_model.h"
#include "stack_config.h"

#include "byte_buffer.h"
#include "string_utilities.h"

typedef enum {
	MMS_ERROR_TYPE_OBJECT_NON_EXISTENT,
	MMS_ERROR_TYPE_OBJECT_ACCESS_UNSUPPORTED,
	MMS_ERROR_TYPE_ACCESS_DENIED
} MmsConfirmedErrorType;

/* write_out function required for ASN.1 encoding */
int
mmsServer_write_out(const void *buffer, size_t size, void *app_key);

void
mmsServer_handleDefineNamedVariableListRequest(
		MmsServerConnection* connection,
		DefineNamedVariableListRequest_t* request,
		int invokeId,
		ByteBuffer* response);

void
mmsServer_handleDeleteNamedVariableListRequest(MmsServerConnection* connection,
		DeleteNamedVariableListRequest_t* request,
		int invokeId,
		ByteBuffer* response);

void
mmsServer_handleGetNamedVariableListAttributesRequest(
		MmsServerConnection* connection,
		GetNamedVariableListAttributesRequest_t* request,
		int invokeId,
		ByteBuffer* response);

void
mmsServer_handleReadRequest(
		MmsServerConnection* connection,
		ReadRequest_t* read,
		int invokeId,
		ByteBuffer* response);

MmsPdu_t*
mmsServer_createConfirmedResponse(int invokeId);

void
mmsServer_createConfirmedErrorPdu(int invokeId, ByteBuffer* response, MmsConfirmedErrorType errorType);

void
mmsServer_writeConcludeResponsePdu(ByteBuffer* response);

int
mmsServer_handleGetVariableAccessAttributesRequest(
		MmsServerConnection* connection,
		GetVariableAccessAttributesRequest_t* request,
		int invokeId,
		ByteBuffer* response);

void
mmsServer_handleGetNameListRequest(
		MmsServerConnection* connection,
		GetNameListRequest_t* getNameList,
		int invokeId,
		ByteBuffer* response);

int /* MmsServiceError */
mmsServer_handleWriteRequest(
		MmsServerConnection* connection,
		WriteRequest_t* writeRequest,
		int invokeId,
		ByteBuffer* response);

int
mmsServer_isIndexAccess(AlternateAccess_t* alternateAccess);

int
mmsServer_getLowIndex(AlternateAccess_t* alternateAccess);

int
mmsServer_getNumberOfElements(AlternateAccess_t* alternateAccess);

void
mmsServer_deleteVariableList(LinkedList namedVariableLists, char* variableListName);

#endif /* MMS_SERVER_INTERNAL_H_ */
