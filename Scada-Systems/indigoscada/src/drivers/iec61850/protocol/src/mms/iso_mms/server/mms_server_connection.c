/*
 *  mms_server_connection.c
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
 *
 *
 *  MMS client connection handling code for libiec61850.
 *
 *  Handles a MMS client connection.
 */
// Modified by Enscada limited http://www.enscada.com
#include "mms_server_internal.h"
#include "iso_server.h"

#ifdef DEBUG
#include <stdio.h>
#endif

#define REJECT_UNRECOGNIZED_SERVICE 1
#define REJECT_UNKNOWN_PDU_TYPE 2

#define MMS_SERVICE_STATUS 0x80
#define MMS_SERVICE_GET_NAME_LIST 0x40
#define MMS_SERVICE_IDENTIFY 0x20
#define MMS_SERVICE_RENAME 0x10
#define MMS_SERVICE_READ 0x08
#define MMS_SERVICE_WRITE 0x04
#define MMS_SERVICE_GET_VARIABLE_ACCESS_ATTRIBUTES 0x02
#define MMS_SERVICE_DEFINE_NAMED_VARIABLE 0x01

#define MMS_SERVICE_DEFINE_SCATTERED_ACCESS 0x80
#define MMS_SERVICE_GET_SCATTERED_ACCESS_ATTRIBUTES 0x40
#define MMS_SERVICE_DELETE_VARIABLE_ACCESS 0x20
#define MMS_SERVICE_DEFINE_NAMED_VARIABLE_LIST 0x10
#define MMS_SERVICE_GET_NAMED_VARIABLE_LIST_ATTRIBUTES 0x08
#define MMS_SERVICE_DELETE_NAMED_VARIABLE_LIST 0x04
#define MMS_SERVICE_DEFINE_NAMED_TYPE 0x02
#define MMS_SERVICE_GET_NAMED_TYPE_ATTRIBUTES 0x01


/* servicesSupported MMS bitstring */
static uint8_t servicesSupported[] =
{
		0x00
		| MMS_SERVICE_GET_NAME_LIST
		| MMS_SERVICE_READ
		| MMS_SERVICE_WRITE
		| MMS_SERVICE_GET_VARIABLE_ACCESS_ATTRIBUTES
		,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00
};


/**********************************************************************************************
 * MMS Common support functions
 *********************************************************************************************/

static void
writeMmsRejectPdu(int* invokeId, int reason, ByteBuffer* response) {

	asn_enc_rval_t rval;
	MmsPdu_t* mmsPdu = calloc(1, sizeof(MmsPdu_t));

	mmsPdu->present = MmsPdu_PR_rejectPDU;

	if (invokeId != NULL) {
		mmsPdu->choice.rejectPDU.originalInvokeID = calloc(1, sizeof(Unsigned32_t));
		asn_long2INTEGER(mmsPdu->choice.rejectPDU.originalInvokeID, *invokeId);
	}

	if (reason == REJECT_UNRECOGNIZED_SERVICE) {
		mmsPdu->choice.rejectPDU.rejectReason.present = RejectPDU__rejectReason_PR_confirmedRequestPDU;
		mmsPdu->choice.rejectPDU.rejectReason.choice.confirmedResponsePDU =
			RejectPDU__rejectReason__confirmedRequestPDU_unrecognizedService;
	}
	else if(reason == REJECT_UNKNOWN_PDU_TYPE) {
		mmsPdu->choice.rejectPDU.rejectReason.present = RejectPDU__rejectReason_PR_pduError;
		asn_long2INTEGER(&mmsPdu->choice.rejectPDU.rejectReason.choice.pduError,
				RejectPDU__rejectReason__pduError_unknownPduType);
	}
	else {
		mmsPdu->choice.rejectPDU.rejectReason.present = RejectPDU__rejectReason_PR_confirmedRequestPDU;
		mmsPdu->choice.rejectPDU.rejectReason.choice.confirmedResponsePDU =
			RejectPDU__rejectReason__confirmedRequestPDU_other;
	}

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
			mmsServer_write_out, (void*) response);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);
}

/**********************************************************************************************
 * MMS Initiate Service
 *********************************************************************************************/

static void
freeInitiateResponsePdu(InitiateResponsePdu_t response)
{
	free(response.localDetailCalled);
	free(response.negotiatedDataStructureNestingLevel);
	free(response.mmsInitResponseDetail.negotiatedParameterCBB.buf);
	free(response.mmsInitResponseDetail.servicesSupportedCalled.buf);
}

static InitiateResponsePdu_t
createInitiateResponsePdu(MmsServerConnection* self)
{
	InitiateResponsePdu_t response;

	response.localDetailCalled = calloc(1, sizeof(Integer32_t));
	*(response.localDetailCalled) = self->maxPduSize;

	response.negotiatedMaxServOutstandingCalled =
			self->maxServOutstandingCalled;

	response.negotiatedMaxServOutstandingCalling =
			self->maxServOutstandingCalling;

	response.negotiatedDataStructureNestingLevel = calloc(1, sizeof(Integer8_t));
	*(response.negotiatedDataStructureNestingLevel) =
			self->dataStructureNestingLevel;

	response.mmsInitResponseDetail.negotiatedVersionNumber = 1;

	response.mmsInitResponseDetail.negotiatedParameterCBB.size = 2;
	response.mmsInitResponseDetail.negotiatedParameterCBB.bits_unused = 5;
	response.mmsInitResponseDetail.negotiatedParameterCBB.buf = calloc(2, 1);
	//TODO add CBB value

	response.mmsInitResponseDetail.servicesSupportedCalled.size = 11;
	response.mmsInitResponseDetail.servicesSupportedCalled.bits_unused = 3;
	response.mmsInitResponseDetail.servicesSupportedCalled.buf = calloc(11, sizeof(uint8_t));
	memcpy(response.mmsInitResponseDetail.servicesSupportedCalled.buf, servicesSupported, 11);

	return response;
}

static int
createInitiateResponse(MmsServerConnection* self, ByteBuffer* writeBuffer)
{
	asn_enc_rval_t rval;
	MmsPdu_t* mmsPdu = calloc(1, sizeof(MmsPdu_t));

	mmsPdu->present = MmsPdu_PR_initiateResponsePdu;

	mmsPdu->choice.initiateResponsePdu = createInitiateResponsePdu(self);

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
	            mmsServer_write_out, (void*) writeBuffer);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	//TODO solve deallocation problem
	freeInitiateResponsePdu(mmsPdu->choice.initiateResponsePdu);
	free(mmsPdu);

	//asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	return rval.encoded;
}

void
parseInitiateRequestPdu(MmsServerConnection* self, const InitiateRequestPdu_t* request)
{
	int cbbLen;
	int servicesSize;

	self->maxServOutstandingCalled =
			request->proposedMaxServOutstandingCalled;
	self->maxServOutstandingCalling =
			request->proposedMaxServOutstandingCalling;
	if (request->localDetailCalling == 0)
		self->maxPduSize = DEFAULT_MAX_PDU_SIZE;
	else
		self->maxPduSize = *(request->localDetailCalling);

	if (request->proposedDataStructureNestingLevel == 0)
		self->dataStructureNestingLevel =
				DEFAULT_DATA_STRUCTURE_NESTING_LEVEL;
	else
		self->dataStructureNestingLevel =
				*(request->proposedDataStructureNestingLevel);

	//TODO handle proposedParameterCBB and servicesSupported
	/* CBB */
	cbbLen = request->mmsInitRequestDetail.proposedParameterCBB.size;
	/* Requested services */
	servicesSize =
			request->mmsInitRequestDetail.servicesSupportedCalling.size;
}

static int
handleInitiateRequestPdu (
		MmsServerConnection* self,
		InitiateRequestPdu_t* request,
		ByteBuffer* response)
{
	parseInitiateRequestPdu(self, request);

	//TODO here we should indicate if something is wrong!
	createInitiateResponse(self, response);

	return 0;
}


/**********************************************************************************************
 * MMS General service handling functions
 *********************************************************************************************/

static int
handleConfirmedRequestPdu(
		MmsServerConnection* self,
		ConfirmedRequestPdu_t* request,
		ByteBuffer* response)
{
	long invokeIdLong;
	int32_t invokeId;

	asn_INTEGER2long((INTEGER_t*) &(request->invokeID), &invokeIdLong);

	invokeId = (int32_t) invokeIdLong;

	if (DEBUG) printf("invokeId: %i\n", invokeId);

	switch(request->confirmedServiceRequest.present) {
	case ConfirmedServiceRequest_PR_getNameList:
		mmsServer_handleGetNameListRequest(self, &(request->confirmedServiceRequest.choice.getNameList),
				invokeId, response);
		break;
	case ConfirmedServiceRequest_PR_read:
		mmsServer_handleReadRequest(self, &(request->confirmedServiceRequest.choice.read),
				invokeId, response);
		break;
	case ConfirmedServiceRequest_PR_write:
		mmsServer_handleWriteRequest(self, &(request->confirmedServiceRequest.choice.write),
						invokeId, response);
		break;
	case ConfirmedServiceRequest_PR_getVariableAccessAttributes:
		mmsServer_handleGetVariableAccessAttributesRequest(self,
				&(request->confirmedServiceRequest.choice.getVariableAccessAttributes),
				invokeId, response);
		break;
	case ConfirmedServiceRequest_PR_defineNamedVariableList:
		mmsServer_handleDefineNamedVariableListRequest(self,
				&(request->confirmedServiceRequest.choice.defineNamedVariableList),
				invokeId, response);
		break;
	case ConfirmedServiceRequest_PR_getNamedVariableListAttributes:
		mmsServer_handleGetNamedVariableListAttributesRequest(self,
				&(request->confirmedServiceRequest.choice.getNamedVariableListAttributes),
				invokeId, response);
		break;
	case ConfirmedServiceRequest_PR_deleteNamedVariableList:
		mmsServer_handleDeleteNamedVariableListRequest(self,
				&(request->confirmedServiceRequest.choice.deleteNamedVariableList),
				invokeId, response);
		break;
	default:
		writeMmsRejectPdu(&invokeId, REJECT_UNRECOGNIZED_SERVICE, response);
		break;
	}
}


static inline MmsIndication
parseMmsPdu(MmsServerConnection* self, ByteBuffer* message, ByteBuffer* response)
{
	MmsPdu_t* mmsPdu = 0; /* allow asn1c to allocate structure */
	MmsIndication retVal;

	asn_dec_rval_t rval; /* Decoder return value  */

	rval = ber_decode(NULL, &asn_DEF_MmsPdu,
			(void**) &mmsPdu, (void*) message->buffer, message->size);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	if (rval.code != RC_OK) {
		retVal = MMS_ERROR;
		goto parseMmsPdu_exit;
	}

	switch (mmsPdu->present) {
	case MmsPdu_PR_initiateRequestPdu:
		handleInitiateRequestPdu(self, &(mmsPdu->choice.initiateRequestPdu), response);
		retVal = MMS_INITIATE;
		break;
	case MmsPdu_PR_confirmedRequestPdu:
		handleConfirmedRequestPdu(self, &(mmsPdu->choice.confirmedRequestPdu), response);
		retVal = MMS_CONFIRMED_REQUEST;
		break;
	case MmsPdu_PR_concludeRequestPDU:
		mmsServer_writeConcludeResponsePdu(response);
		IsoConnection_close(self->isoConnection);
		retVal = MMS_CONCLUDE;
		break;
	default:
		writeMmsRejectPdu(NULL, REJECT_UNKNOWN_PDU_TYPE, response);
		retVal = MMS_ERROR;
		break;
	}

parseMmsPdu_exit:
	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);
	return retVal;
}

static void /* will be called by IsoConnection */
messageReceived(void* parameter, ByteBuffer* message, ByteBuffer* response)
{
	MmsServerConnection* self = (MmsServerConnection*) parameter;

	MmsServerConnection_parseMessage(self,  message, response);
}

/**********************************************************************************************
 * MMS server connection public API functions
 *********************************************************************************************/

MmsServerConnection*
MmsServerConnection_init(MmsServerConnection* connection, MmsServer server, IsoConnection isoCon)
{
	MmsServerConnection* self;

	if (connection == NULL)
		self = calloc(1, sizeof(MmsServerConnection));
	else
		self = connection;

	self->maxServOutstandingCalled = 0;
	self->maxServOutstandingCalling = 0;
	self->maxPduSize = 0;
	self->dataStructureNestingLevel = 0;
	self->server = server;
	self->isoConnection = isoCon;
	self->namedVariableLists = LinkedList_create();

	IsoConnection_installListener(isoCon, messageReceived, (void*) self);

	return self;
}

void
MmsServerConnection_destroy(MmsServerConnection* self)
{
	LinkedList_destroyDeep(self->namedVariableLists, MmsNamedVariableList_destroy);
	free(self);
}

bool
MmsServerConnection_addNamedVariableList(MmsServerConnection* self, MmsNamedVariableList variableList)
{
	//TODO check if operation is allowed!

	LinkedList_add(self->namedVariableLists, variableList);

	return true;
}

void
MmsServerConnection_deleteNamedVariableList(MmsServerConnection* self, char* listName)
{
	mmsServer_deleteVariableList(self->namedVariableLists, listName);
}

MmsNamedVariableList
MmsServerConnection_getNamedVariableList(MmsServerConnection* self, char* variableListName)
{
	//TODO remove code duplication - similar to MmsDomain_getNamedVariableList !
	MmsNamedVariableList variableList = NULL;

	LinkedList element = LinkedList_getNext(self->namedVariableLists);

	while (element != NULL) {
		MmsNamedVariableList varList = (MmsNamedVariableList) element->data;

		if (strcmp(MmsNamedVariableList_getName(varList), variableListName) == 0) {
			variableList = varList;
			break;
		}

		element = LinkedList_getNext(element);
	}

	return variableList;
}

LinkedList
MmsServerConnection_getNamedVariableLists(MmsServerConnection* self)
{
	return self->namedVariableLists;
}

MmsIndication
MmsServerConnection_parseMessage
(MmsServerConnection* self, ByteBuffer* message, ByteBuffer* response)
{
	return parseMmsPdu(self, message, response);
}


