/*
 *  mms_msg_internal.h
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

#ifndef MMS_MSG_INTERNAL_H_
#define MMS_MSG_INTERNAL_H_

#include <stdlib.h>
#include "MmsPdu.h"
#include "linked_list.h"


/**
 * MMS Object class enumeration type
 */
typedef enum {
	MMS_NAMED_VARIABLE,
	MMS_NAMED_VARIABLE_LIST
} MmsObjectClass;


uint32_t
mmsClient_getInvokeId(ConfirmedResponsePdu_t* confirmedResponse);

int
mmsClient_write_out(void *buffer, size_t size, void *app_key);

int
mmsClient_createInitiateRequest(ByteBuffer* writeBuffer);

MmsPdu_t*
mmsClient_createConfirmedRequestPdu(long invokeId);

//TODO mmsMsg_parseInititateResponse;

int
mmsClient_createMmsGetNameListRequestVMDspecific(long invokeId, ByteBuffer* writeBuffer);

LinkedList
mmsClient_parseGetNameListResponse(ByteBuffer* message, uint32_t* invokeId);

int
mmsClient_createGetNameListRequestDomainSpecific(long invokeId, char* domainName,
		ByteBuffer* writeBuffer, MmsObjectClass objectClass);

MmsValue*
mmsClient_parseReadResponse(ByteBuffer* message, uint32_t* invokeId);

int
mmsClient_createReadRequest(char* domainId, char* itemId, ByteBuffer* writeBuffer);

int
mmsClient_createReadRequestAlternateAccessIndex(char* domainId, char* itemId,
		uint32_t index, uint32_t elementCount, ByteBuffer* writeBuffer);

int
mmsClient_createReadRequestMultipleValues(char* domainId, LinkedList /*<char*>*/ items,
		ByteBuffer* writeBuffer);

int
mmsClient_createReadNamedVariableListRequest(uint32_t invokeId, char* domainId, char* itemId,
		ByteBuffer* writeBuffer, bool specWithResult);

int
mmsClient_createReadAssociationSpecificNamedVariableListRequest(
		uint32_t invokeId,
		char* itemId,
		ByteBuffer* writeBuffer,
		bool specWithResult);

void
mmsClient_createGetNamedVariableListAttributesRequest(uint32_t invokeId, ByteBuffer* writeBuffer,
		char* domainId, char* listNameId);

LinkedList
mmsClient_parseGetNamedVariableListAttributesResponse(ByteBuffer* message, uint32_t* invokeId,
		bool* /*OUT*/ deletable);

int
mmsClient_createGetVariableAccessAttributesRequest(
		char* domainId, char* itemId,
		ByteBuffer* writeBuffer);

MmsTypeSpecification*
mmsClient_parseGetVariableAccessAttributesResponse(ByteBuffer* message, uint32_t* invokeId);

MmsIndication
mmsClient_parseWriteResponse(ByteBuffer* message);

int
mmsClient_createWriteRequest(long invokeId, char* domainId, char* itemId, MmsValue* value,
		ByteBuffer* writeBuffer);

void
mmsClient_createDefineNamedVariableListRequest(uint32_t invokeId, ByteBuffer* writeBuffer,
		char* domainId, char* listNameId, LinkedList /*<char*>*/ listOfVariables,
		bool associationSpecific);

MmsIndication
mmsClient_parseDefineNamedVariableResponse(ByteBuffer* message, uint32_t* invokeId);

void
mmsClient_createDeleteNamedVariableListRequest(long invokeId, ByteBuffer* writeBuffer,
		char* domainId, char* listNameId);

MmsIndication
mmsClient_parseDeleteNamedVariableListResponse(ByteBuffer* message, uint32_t* invokeId);

void
mmsClient_createDeleteAssociationSpecificNamedVariableListRequest(
		long invokeId,
		ByteBuffer* writeBuffer,
		char* listNameId);

#endif /* MMS_MSG_INTERNAL_H_ */
