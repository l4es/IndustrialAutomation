/*
 *  mms_client_connection.c
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

#include "mms_client_connection.h"
#include "iso_client_connection.h"
#include "mms_client_internal.h"
#include "stack_config.h"

#include <MmsPdu.h>

#include "byte_buffer.h"

#define MMS_BUFFER_SIZE 10000

typedef enum {
	MMS_STATE_CLOSED,
	MMS_STATE_CONNECTING,
	MMS_STATE_CONNECTED
} AssociationState;

typedef enum {
	MMS_CON_IDLE,
	MMS_CON_WAITING,
	MMS_CON_ASSOCIATION_FAILED,
	MMS_CON_ASSOCIATED,
	MMS_CON_RESPONSE_PENDING
} ConnectionState;

typedef enum {
	MMS_REQ_NONE,
	MMS_REQ_INITIATE,
	MMS_REQ_GET_NAME_LIST,
	MMS_REQ_READ,
	MMS_REQ_WRITE,
	MMS_REQ_GET_VAR_ACCESS_ATTRIBUTES,
	MMS_REQ_DEFINE_NAMED_VARIABLE_LIST,
	MMS_REQ_DELETE_NAMED_VARIABLE_LIST,
	MMS_REQ_GET_NAMED_VARIABLE_LIST_ATTRIBUTES
} RequestType;

/* private instance variables */
struct sMmsConnection {
	uint32_t lastInvokeId;
	RequestType lastRequestType;
	ByteBuffer* lastResponse;
	MmsClientError lastError;
	IsoClientConnection isoClient;
	AssociationState associationState;
	ConnectionState connectionState;
	uint8_t* buffer;
	MmsConnectionParameters parameters;
	IsoConnectionParameters* isoParameters;
	int isoConnectionParametersSelfAllocated;
};

static void mmsIsoCallback(IsoIndication indication, void* parameter, ByteBuffer* payload)
{
	MmsConnection self = (MmsConnection) parameter;

	if (DEBUG) printf("mmsIsoCallback called with indication %i\n", indication);

	if (self->lastRequestType == MMS_REQ_INITIATE) {
		if (indication == ISO_IND_ASSOCIATION_SUCCESS) {
			self->connectionState = MMS_CON_ASSOCIATED;
		}
		else {
			self->connectionState = MMS_CON_ASSOCIATION_FAILED;
		}

		IsoClientConnection_releasePayloadBuffer(self->isoClient, payload);
	}
	else if (self->lastRequestType == MMS_REQ_GET_NAME_LIST) {
		self->lastResponse = payload;
		self->connectionState = MMS_CON_RESPONSE_PENDING;
	}
	else if (self->lastRequestType == MMS_REQ_READ) {
		self->lastResponse = payload;
		self->connectionState = MMS_CON_RESPONSE_PENDING;
	}
	else if (self->lastRequestType == MMS_REQ_WRITE) {
		self->lastResponse = payload;
		self->connectionState = MMS_CON_RESPONSE_PENDING;
	}
	else if (self->lastRequestType == MMS_REQ_GET_VAR_ACCESS_ATTRIBUTES) {
		self->lastResponse = payload;
		self->connectionState = MMS_CON_RESPONSE_PENDING;
	}
	else if (self->lastRequestType == MMS_REQ_DEFINE_NAMED_VARIABLE_LIST) {
		self->lastResponse = payload;
		self->connectionState = MMS_CON_RESPONSE_PENDING;
	}
	else if (self->lastRequestType == MMS_REQ_DELETE_NAMED_VARIABLE_LIST) {
		self->lastResponse = payload;
		self->connectionState = MMS_CON_RESPONSE_PENDING;
	}
	else if (self->lastRequestType == MMS_REQ_GET_NAMED_VARIABLE_LIST_ATTRIBUTES) {
		self->lastResponse = payload;
		self->connectionState = MMS_CON_RESPONSE_PENDING;
	}
	else {
		printf("Unexpected message from server");
		IsoClientConnection_releasePayloadBuffer(self->isoClient, payload);
	}

	//TODO check invoke ID
}

MmsConnection
MmsConnection_create()
{
	MmsConnection self = calloc(1, sizeof(struct sMmsConnection));

	self->buffer = malloc(MMS_BUFFER_SIZE);

	return self;
}

void
MmsConnection_destroy(MmsConnection self)
{
	if (self->isoClient != NULL)
		IsoClientConnection_destroy(self->isoClient);

	if (self->isoConnectionParametersSelfAllocated)
		free(self->isoParameters);

	free(self->buffer);
	free(self);
}

void
MmsConnection_setIsoConnectionParameters(MmsConnection self, IsoConnectionParameters* params)
{
	self->isoParameters = params;
}


MmsClientError
MmsConnection_getError(MmsConnection self) {
	return self->lastError;
}

MmsIndication
MmsConnection_connect(MmsConnection self, char* serverName, int serverPort)
{
	ByteBuffer payload;
	self->isoClient = IsoClientConnection_create(mmsIsoCallback, (void*) self);

	if (self->isoParameters == NULL) {
		self->isoConnectionParametersSelfAllocated = 1;
		self->isoParameters = calloc(1, sizeof(IsoConnectionParameters));
	}

	self->isoParameters->hostname = serverName;
	self->isoParameters->tcpPort = serverPort;

	

	ByteBuffer_wrap(&payload, self->buffer, 0, MMS_BUFFER_SIZE);

	mmsClient_createInitiateRequest(&payload);

	self->connectionState = MMS_CON_WAITING;
	self->lastRequestType = MMS_REQ_INITIATE;

	IsoClientConnection_associate(self->isoClient, self->isoParameters, &payload);

	/* poll callback handler TODO poll with timeout */
	while (self->connectionState == MMS_CON_WAITING) {
		Thread_sleep(1);
	}

	if (self->connectionState == MMS_CON_ASSOCIATED) {
		//TODO parse connection response message.
		self->associationState = MMS_STATE_CONNECTED;
	}
	else {
		self->associationState = MMS_STATE_CLOSED;
	}

	self->connectionState = MMS_CON_IDLE;

	if (self->associationState == MMS_STATE_CONNECTED)
		return MMS_OK;
	else
		return MMS_ERROR;
}


static LinkedList /* <char*> */
mmsClient_getNameList(MmsConnection self, char* domainId, MmsObjectClass objectClass, bool associationSpecific)
{
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, MMS_BUFFER_SIZE);

	self->lastInvokeId++;

	if (associationSpecific)
		mmsClient_createMmsGetNameListRequestAssociationSpecific(self->lastInvokeId, &payload);
	else {

		if (domainId == NULL)
			mmsClient_createMmsGetNameListRequestVMDspecific(self->lastInvokeId, &payload);
		else
			mmsClient_createGetNameListRequestDomainSpecific(self->lastInvokeId, domainId, &payload,
				objectClass);
	}

	self->lastRequestType = MMS_REQ_GET_NAME_LIST;
	self->connectionState = MMS_CON_WAITING;

	IsoClientConnection_sendMessage(self->isoClient, &payload);

	/* poll callback handler TODO poll with timeout */
	while (self->connectionState == MMS_CON_WAITING)
		Thread_sleep(1);

	if (self->connectionState == MMS_CON_RESPONSE_PENDING) {
		uint32_t invokeId;
		LinkedList namedVariables = mmsClient_parseGetNameListResponse(self->lastResponse,
																	&invokeId);

		if (invokeId != self->lastInvokeId) {
			printf("Invalid invoke ID\n");

			self->lastError = MMS_CE_INVOKE_ID_MISMATCH;

			if (namedVariables != NULL) {
				LinkedList_destroy(namedVariables);
			}
			return NULL;
		}

		IsoClientConnection_releasePayloadBuffer(self->isoClient, self->lastResponse);
		self->connectionState = MMS_CON_IDLE;
		return namedVariables;
	}

	self->connectionState = MMS_CON_IDLE;
	return NULL;
}

LinkedList /* <char*> */
MmsConnection_getDomainNames(MmsConnection self) {
	return mmsClient_getNameList(self, NULL, MMS_NAMED_VARIABLE, false);
}

LinkedList /* <char*> */
MmsConnection_getDomainVariableNames(MmsConnection self, char* domainId) {
	return mmsClient_getNameList(self, domainId, MMS_NAMED_VARIABLE, false);
}

LinkedList /* <char*> */
MmsConnection_getDomainVariableListNames(MmsConnection self, char* domainId) {
	return mmsClient_getNameList(self, domainId, MMS_NAMED_VARIABLE_LIST, false);
}

LinkedList /* <char*> */
MmsConnection_getVariableListNamesAssociationSpecific(MmsConnection self)
{
	return mmsClient_getNameList(self, NULL, MMS_NAMED_VARIABLE_LIST, true);
}

MmsValue*
MmsConnection_readVariable(MmsConnection self, char* domainId, char* itemId)
{
	ByteBuffer payload;
	MmsValue* value = NULL;
	ByteBuffer_wrap(&payload, self->buffer, 0, MMS_BUFFER_SIZE);

	self->lastInvokeId++;

	mmsClient_createReadRequest(domainId, itemId, &payload);

	self->lastRequestType = MMS_REQ_READ;
	self->connectionState = MMS_CON_WAITING;

	IsoClientConnection_sendMessage(self->isoClient, &payload);

	/* poll callback handler TODO poll with timeout */
	while (self->connectionState == MMS_CON_WAITING)
		Thread_sleep(1);;

	if (self->connectionState == MMS_CON_RESPONSE_PENDING) {
		uint32_t invokeId;

		value = mmsClient_parseReadResponse(self->lastResponse, &invokeId);
		IsoClientConnection_releasePayloadBuffer(self->isoClient, self->lastResponse);
	}

	self->connectionState = MMS_CON_IDLE;
	return value;
}

MmsValue*
MmsConnection_readArrayElements(MmsConnection self, char* domainId, char* itemId,
		uint32_t startIndex, uint32_t numberOfElements)
{
	ByteBuffer payload;
	MmsValue* value = NULL;
	ByteBuffer_wrap(&payload, self->buffer, 0, MMS_BUFFER_SIZE);

	self->lastInvokeId++;

	mmsClient_createReadRequestAlternateAccessIndex(domainId, itemId, startIndex, numberOfElements,
			&payload);

	self->lastRequestType = MMS_REQ_READ;
	self->connectionState = MMS_CON_WAITING;

	IsoClientConnection_sendMessage(self->isoClient, &payload);

	/* poll callback handler TODO poll with timeout */
	while (self->connectionState == MMS_CON_WAITING)
		Thread_sleep(1);


	if (self->connectionState == MMS_CON_RESPONSE_PENDING) {
		uint32_t invokeId;

		value = mmsClient_parseReadResponse(self->lastResponse, &invokeId);
		IsoClientConnection_releasePayloadBuffer(self->isoClient, self->lastResponse);
	}

	self->connectionState = MMS_CON_IDLE;
	return value;
}

MmsValue*
MmsConnection_readMultipleVariables(MmsConnection self, char* domainId,
		LinkedList /*<char*>*/ items)
{
	ByteBuffer payload;
	MmsValue* value = NULL;
	ByteBuffer_wrap(&payload, self->buffer, 0, MMS_BUFFER_SIZE);

	self->lastInvokeId++;

	//TODO handle invokeID
	mmsClient_createReadRequestMultipleValues(domainId, items, &payload);

	self->lastRequestType = MMS_REQ_READ;
	self->connectionState = MMS_CON_WAITING;

	IsoClientConnection_sendMessage(self->isoClient, &payload);

	/* poll callback handler TODO poll with timeout */
	while (self->connectionState == MMS_CON_WAITING)
		Thread_sleep(1);

	if (self->connectionState == MMS_CON_RESPONSE_PENDING) {
		uint32_t invokeId;

		value = mmsClient_parseReadResponse(self->lastResponse, &invokeId);
		IsoClientConnection_releasePayloadBuffer(self->isoClient, self->lastResponse);
	}

	self->connectionState = MMS_CON_IDLE;
	return value;
}

MmsValue*
MmsConnection_readNamedVariableListValues(MmsConnection self, char* domainId, char* listName,
		bool specWithResult)
{
	MmsValue* value = NULL;
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, MMS_BUFFER_SIZE);

	self->lastInvokeId++;

	mmsClient_createReadNamedVariableListRequest(self->lastInvokeId, domainId, listName,
			&payload, specWithResult);

	self->lastRequestType = MMS_REQ_READ;
	self->connectionState = MMS_CON_WAITING;

	IsoClientConnection_sendMessage(self->isoClient, &payload);

	/* poll callback handler TODO poll with timeout */
	while (self->connectionState == MMS_CON_WAITING)
		Thread_sleep(1);

	if (self->connectionState == MMS_CON_RESPONSE_PENDING) {
		uint32_t invokeId;

		value = mmsClient_parseReadResponse(self->lastResponse, &invokeId);
		IsoClientConnection_releasePayloadBuffer(self->isoClient, self->lastResponse);
	}

	self->connectionState = MMS_CON_IDLE;
	return value;
}

MmsValue*
MmsConnection_readNamedVariableListValuesAssociationSpecific(
		MmsConnection self,
		char* listName,
		bool specWithResult)
{
	MmsValue* value = NULL;
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, MMS_BUFFER_SIZE);

	self->lastInvokeId++;

	mmsClient_createReadAssociationSpecificNamedVariableListRequest(self->lastInvokeId, listName,
			&payload, specWithResult);

	self->lastRequestType = MMS_REQ_READ;
	self->connectionState = MMS_CON_WAITING;

	IsoClientConnection_sendMessage(self->isoClient, &payload);

	/* poll callback handler TODO poll with timeout */
	while (self->connectionState == MMS_CON_WAITING)
		Thread_sleep(1);

	if (self->connectionState == MMS_CON_RESPONSE_PENDING) {
		uint32_t invokeId;

		value = mmsClient_parseReadResponse(self->lastResponse, &invokeId);
		IsoClientConnection_releasePayloadBuffer(self->isoClient, self->lastResponse);
	}

	self->connectionState = MMS_CON_IDLE;
	return value;
}

LinkedList /* <char*> */
MmsConnection_readNamedVariableListDirectory(MmsConnection self, char* domainId, char* listName,
		bool* deletable)
{
	LinkedList attributes = NULL;
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, MMS_BUFFER_SIZE);

	self->lastInvokeId++;

	mmsClient_createGetNamedVariableListAttributesRequest(self->lastInvokeId, &payload, domainId,
			listName);

	self->lastRequestType = MMS_REQ_GET_NAMED_VARIABLE_LIST_ATTRIBUTES;
	self->connectionState = MMS_CON_WAITING;

	IsoClientConnection_sendMessage(self->isoClient, &payload);

	/* poll callback handler TODO poll with timeout */
	while (self->connectionState == MMS_CON_WAITING)
		Thread_sleep(1);


	if (self->connectionState == MMS_CON_RESPONSE_PENDING) {
		uint32_t invokeId;

		attributes = mmsClient_parseGetNamedVariableListAttributesResponse(self->lastResponse, &invokeId,
				deletable);

		IsoClientConnection_releasePayloadBuffer(self->isoClient, self->lastResponse);
	}

	self->connectionState = MMS_CON_IDLE;

	return attributes;
}

MmsIndication
MmsConnection_defineNamedVariableList(MmsConnection self, char* domainId, char* listName,
		LinkedList variableSpecs)
{
	MmsIndication indication = MMS_ERROR;
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, MMS_BUFFER_SIZE);

	self->lastInvokeId++;

	mmsClient_createDefineNamedVariableListRequest(self->lastInvokeId, &payload, domainId,
			listName, variableSpecs, false);

	self->lastRequestType = MMS_REQ_DEFINE_NAMED_VARIABLE_LIST;
	self->connectionState = MMS_CON_WAITING;

	IsoClientConnection_sendMessage(self->isoClient, &payload);

	/* poll callback handler TODO poll with timeout */
	while (self->connectionState == MMS_CON_WAITING)
		Thread_sleep(1);
	

	if (self->connectionState == MMS_CON_RESPONSE_PENDING) {
		uint32_t invokeId;

		indication = mmsClient_parseDefineNamedVariableResponse(self->lastResponse, &invokeId);

		IsoClientConnection_releasePayloadBuffer(self->isoClient, self->lastResponse);
	}

	self->connectionState = MMS_CON_IDLE;

	return indication;
}

MmsIndication
MmsConnection_defineNamedVariableListAssociationSpecific(MmsConnection self, char* listName,
		LinkedList variableSpecs)
{
	MmsIndication indication = MMS_ERROR;
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, MMS_BUFFER_SIZE);

	self->lastInvokeId++;

	mmsClient_createDefineNamedVariableListRequest(self->lastInvokeId, &payload, NULL,
			listName, variableSpecs, true);

	self->lastRequestType = MMS_REQ_DEFINE_NAMED_VARIABLE_LIST;
	self->connectionState = MMS_CON_WAITING;

	IsoClientConnection_sendMessage(self->isoClient, &payload);

	/* poll callback handler TODO poll with timeout */
	while (self->connectionState == MMS_CON_WAITING)
		Thread_sleep(1);


	if (self->connectionState == MMS_CON_RESPONSE_PENDING) {
		uint32_t invokeId;

		indication = mmsClient_parseDefineNamedVariableResponse(self->lastResponse, &invokeId);

		IsoClientConnection_releasePayloadBuffer(self->isoClient, self->lastResponse);
	}

	self->connectionState = MMS_CON_IDLE;

	return indication;
}

MmsIndication
MmsConnection_deleteNamedVariableList(MmsConnection self, char* domainId, char* listName)
{
	MmsIndication indication = MMS_ERROR;
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, MMS_BUFFER_SIZE);

	self->lastInvokeId++;

	mmsClient_createDeleteNamedVariableListRequest(self->lastInvokeId, &payload, domainId, listName);

	self->lastRequestType = MMS_REQ_DELETE_NAMED_VARIABLE_LIST;
	self->connectionState = MMS_CON_WAITING;

	IsoClientConnection_sendMessage(self->isoClient, &payload);

	/* poll callback handler TODO poll with timeout */
	while (self->connectionState == MMS_CON_WAITING)
		Thread_sleep(1);

	if (self->connectionState == MMS_CON_RESPONSE_PENDING) {
		uint32_t invokeId;

		indication = mmsClient_parseDeleteNamedVariableListResponse(self->lastResponse, &invokeId);

		IsoClientConnection_releasePayloadBuffer(self->isoClient, self->lastResponse);
	}

	self->connectionState = MMS_CON_IDLE;

	return indication;
}

MmsIndication
MmsConnection_deleteAssociationSpecificNamedVariableList(MmsConnection self, char* listName)
{
	MmsIndication indication = MMS_ERROR;
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, MMS_BUFFER_SIZE);

	self->lastInvokeId++;

	mmsClient_createDeleteAssociationSpecificNamedVariableListRequest(
			self->lastInvokeId, &payload, listName);

	self->lastRequestType = MMS_REQ_DELETE_NAMED_VARIABLE_LIST;
	self->connectionState = MMS_CON_WAITING;

	IsoClientConnection_sendMessage(self->isoClient, &payload);

	/* poll callback handler TODO poll with timeout */
	while (self->connectionState == MMS_CON_WAITING)
		Thread_sleep(1);

	if (self->connectionState == MMS_CON_RESPONSE_PENDING) {
		uint32_t invokeId;

		indication = mmsClient_parseDeleteNamedVariableListResponse(self->lastResponse, &invokeId);

		IsoClientConnection_releasePayloadBuffer(self->isoClient, self->lastResponse);
	}

	self->connectionState = MMS_CON_IDLE;

	return indication;
}

MmsTypeSpecification*
MmsConnection_getVariableAccessAttributes(MmsConnection self,
		char* domainId, char* itemId)
{
	ByteBuffer payload;
	MmsTypeSpecification* typeSpec = NULL;
	ByteBuffer_wrap(&payload, self->buffer, 0, MMS_BUFFER_SIZE);
	
	self->lastInvokeId++;

	mmsClient_createGetVariableAccessAttributesRequest(domainId, itemId, &payload);

	self->lastRequestType = MMS_REQ_GET_VAR_ACCESS_ATTRIBUTES;
	self->connectionState = MMS_CON_WAITING;

	IsoClientConnection_sendMessage(self->isoClient, &payload);

	/* poll callback handler TODO poll with timeout */
	while (self->connectionState == MMS_CON_WAITING)
		Thread_sleep(1);

	if (self->connectionState == MMS_CON_RESPONSE_PENDING) {

		uint32_t invokeId;

		typeSpec = mmsClient_parseGetVariableAccessAttributesResponse(self->lastResponse,
				&invokeId);

		IsoClientConnection_releasePayloadBuffer(self->isoClient, self->lastResponse);
	}

	self->connectionState = MMS_CON_IDLE;
	return typeSpec;
}

MmsIndication
MmsConnection_writeVariable(MmsConnection self,
		char* domainId, char* itemId,
		MmsValue* value)
{
	MmsIndication indication = MMS_ERROR;
	ByteBuffer payload;
	ByteBuffer_wrap(&payload, self->buffer, 0, MMS_BUFFER_SIZE);

	self->lastInvokeId++;

	mmsClient_createWriteRequest(self->lastInvokeId, domainId, itemId, value, &payload);

	self->lastRequestType = MMS_REQ_WRITE;
	self->connectionState = MMS_CON_WAITING;

	IsoClientConnection_sendMessage(self->isoClient, &payload);

	/* poll callback handler TODO poll with timeout */
	while (self->connectionState == MMS_CON_WAITING)
		Thread_sleep(1);

	if (self->connectionState == MMS_CON_RESPONSE_PENDING) {
		//uint32_t invokeId;

		indication = mmsClient_parseWriteResponse(self->lastResponse);
		IsoClientConnection_releasePayloadBuffer(self->isoClient, self->lastResponse);
	}

	self->connectionState = MMS_CON_IDLE;
	return indication;
}

MmsVariableSpecification*
MmsVariableSpecification_create(char* domainId, char* itemId)
{
	MmsVariableSpecification* varSpec = malloc(sizeof(MmsVariableSpecification));

	varSpec->domainId = domainId;
	varSpec->itemId = itemId;

	return varSpec;
}
