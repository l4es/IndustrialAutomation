/*
 *  mms_client_read.c
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
#include <MmsPdu.h>
#include "mms_common.h"
#include "mms_client_connection.h"
#include "byte_buffer.h"
#include "stack_config.h"

#include "platform_endian.h"

#include "mms_client_internal.h"
#include "mms_common_internal.h"

MmsValue*
mmsClient_parseReadResponse(ByteBuffer* message, uint32_t* invokeId)
{
	int componentCount;
	int j;
	int elementCount;
	AccessResult_PR presentType;
	int i;
	MmsPdu_t* mmsPdu = 0; /* allow asn1c to allocate structure */
	MmsIndication retVal =  MMS_OK;

	MmsValue* valueList = NULL;
	MmsValue* value = NULL;

	asn_dec_rval_t rval;

	rval = ber_decode(NULL, &asn_DEF_MmsPdu,
			(void**) &mmsPdu, ByteBuffer_getBuffer(message), ByteBuffer_getSize(message));

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	if (mmsPdu->present == MmsPdu_PR_confirmedResponsePdu) {

		*invokeId = mmsClient_getInvokeId(&mmsPdu->choice.confirmedResponsePdu);

		if (mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.present == ConfirmedServiceResponse_PR_read) {
			ReadResponse_t* response = &(mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice.read);

			int elementCount = response->listOfAccessResult.list.count;

			if (elementCount > 1)
				valueList = MmsValue_createEmtpyArray(elementCount);

			i = 0;

			for (i = 0; i < elementCount; i++) {
				value = NULL;

				presentType = response->listOfAccessResult.list.array[i]->present;

				if (presentType == AccessResult_PR_failure) {
					if (DEBUG) printf("access error!\n");

					retVal = MMS_ERROR;
				}
				else if (presentType == AccessResult_PR_array) {
					value = calloc(1, sizeof(MmsValue));
					value->type = MMS_ARRAY;

					elementCount =
							response->listOfAccessResult.list.array[i]->choice.array.list.count;

					value->value.array.size = elementCount;
					value->value.array.components = calloc(elementCount, sizeof(MmsValue*));

					

					for (j = 0; j < elementCount; j++) {
						value->value.array.components[j] = mmsMsg_parseDataElement(
								response->listOfAccessResult.list.array[i]->choice.array.list.array[j]);
					}
				}
				else if (presentType == AccessResult_PR_structure) {

					int j;
					value = calloc(1, sizeof(MmsValue));
					value->type = MMS_STRUCTURE;

					componentCount =
							response->listOfAccessResult.list.array[i]->choice.structure.list.count;

					value->value.structure.componentCount = componentCount;
					value->value.structure.components = calloc(componentCount, sizeof(MmsValue*));

					
					for (j = 0; j < componentCount; j++) {
						value->value.structure.components[j] = mmsMsg_parseDataElement(
								response->listOfAccessResult.list.array[i]->choice.structure.list.array[j]);
					}
				}
				else if (presentType == AccessResult_PR_bitstring) {

					int size;
					value = calloc(1, sizeof(MmsValue));
					value->type = MMS_BIT_STRING;
					size = response->listOfAccessResult.list.array[i]->choice.bitstring.size;

					value->value.bitString.size = (size * 8)
					   - response->listOfAccessResult.list.array[i]->choice.bitstring.bits_unused;

					value->value.bitString.buf = malloc(size);
					memcpy(value->value.bitString.buf,
							response->listOfAccessResult.list.array[i]->choice.bitstring.buf, size);

				}
				else if (presentType == AccessResult_PR_integer) {
					long integerValue;
					asn_INTEGER2long(&response->listOfAccessResult.list.array[i]->choice.integer, &integerValue);
					value = MmsValue_newIntegerFromInt32((int32_t) integerValue);
				}
				else if (presentType == AccessResult_PR_floatingpoint) {
					int size = response->listOfAccessResult.list.array[i]->choice.floatingpoint.size;

					value = calloc(1, sizeof(MmsValue));
					value->type = MMS_FLOAT;

					if (size == 5) { /* FLOAT32 */
						uint8_t* floatBuf;
						value->value.floatingPoint.formatWidth = 32;
						value->value.floatingPoint.exponentWidth = response->listOfAccessResult.list.array[i]->choice.floatingpoint.buf[0];

						floatBuf = (response->listOfAccessResult.list.array[i]->choice.floatingpoint.buf + 1);

						value->value.floatingPoint.buf = malloc(4);

#ifdef ORDER_LITTLE_ENDIAN
							memcpyReverseByteOrder(value->value.floatingPoint.buf, floatBuf, 4);
#else
							memcpy(value->value.floatingPoint.buf, floatBuf, 4);
#endif

					}

					if (size == 9) { /* FLOAT64 */
						uint8_t* floatBuf;
						value->value.floatingPoint.formatWidth = 64;
						value->value.floatingPoint.exponentWidth = response->listOfAccessResult.list.array[i]->choice.floatingpoint.buf[0];

						floatBuf = (response->listOfAccessResult.list.array[i]->choice.floatingpoint.buf + 1);

						value->value.floatingPoint.buf = malloc(8);

#ifdef ORDER_LITTLE_ENDIAN
							memcpyReverseByteOrder(value->value.floatingPoint.buf, floatBuf, 8);
#else
							memcpy(value->value.floatingPoint.buf, floatBuf, 8);
#endif
					}

				}
				else if (presentType == AccessResult_PR_utctime) {

						value = calloc(1, sizeof(MmsValue)); //apa+++

						value->type = MMS_UTC_TIME;
						memcpy(value->value.utcTime,
								response->listOfAccessResult.list.array[i]->choice.utctime.buf, 8);
				}
				else {
					printf("unknown type\n");
					retVal = MMS_ERROR;
				}

				if (elementCount > 1)
					MmsValue_setArrayElement(valueList, i, value);
			}
		}
		else {
			retVal = MMS_ERROR;
		}
	}
	else {
		retVal = MMS_ERROR;
	}

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	if (valueList == NULL)
		valueList = value;

	return valueList;
}


static ReadRequest_t*
createReadRequest (MmsPdu_t* mmsPdu) {
	mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.present =
				ConfirmedServiceRequest_PR_read;

	return &(mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.choice.read);
}


int
mmsClient_createReadNamedVariableListRequest(uint32_t invokeId, char* domainId, char* itemId,
		ByteBuffer* writeBuffer, bool specWithResult)
{
	asn_enc_rval_t rval;
	ObjectName_t* objectName;
	MmsPdu_t* mmsPdu = mmsClient_createConfirmedRequestPdu(invokeId);

	ReadRequest_t* readRequest = createReadRequest(mmsPdu);

	if (specWithResult) {
		readRequest->specificationWithResult = calloc(1, sizeof(BOOLEAN_t));
		*(readRequest->specificationWithResult) = true;
	}
	else
		readRequest->specificationWithResult = NULL;

	readRequest->variableAccessSpecification.present = VariableAccessSpecification_PR_variableListName;

	objectName = &(readRequest->variableAccessSpecification.choice.variableListName);

	objectName->present = ObjectName_PR_domainspecific;

	objectName->choice.domainspecific.domainId.buf = copyString(domainId);
	objectName->choice.domainspecific.domainId.size = strlen(domainId);

	objectName->choice.domainspecific.itemId.buf = copyString(itemId);
	objectName->choice.domainspecific.itemId.size = strlen(itemId);

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
	            mmsClient_write_out, (void*) writeBuffer);

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	return rval.encoded;
}

int
mmsClient_createReadAssociationSpecificNamedVariableListRequest(
		uint32_t invokeId,
		char* itemId,
		ByteBuffer* writeBuffer,
		bool specWithResult)
{
	asn_enc_rval_t rval;
	ObjectName_t* objectName;
	MmsPdu_t* mmsPdu = mmsClient_createConfirmedRequestPdu(invokeId);

	ReadRequest_t* readRequest = createReadRequest(mmsPdu);

	if (specWithResult) {
		readRequest->specificationWithResult = calloc(1, sizeof(BOOLEAN_t));
		*(readRequest->specificationWithResult) = true;
	}
	else
		readRequest->specificationWithResult = NULL;

	readRequest->variableAccessSpecification.present = VariableAccessSpecification_PR_variableListName;

	objectName = &(readRequest->variableAccessSpecification.choice.variableListName);

	objectName->present = ObjectName_PR_aaspecific;

	objectName->choice.aaspecific.buf = copyString(itemId);
	objectName->choice.aaspecific.size = strlen(itemId);

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
	            mmsClient_write_out, (void*) writeBuffer);

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	return rval.encoded;
}

/**
 * Request a single value
 */
int
mmsClient_createReadRequest(char* domainId, char* itemId, ByteBuffer* writeBuffer)
{
	asn_enc_rval_t rval;
	ListOfVariableSeq_t* listOfVars;
	MmsPdu_t* mmsPdu = mmsClient_createConfirmedRequestPdu(1);

	ReadRequest_t* readRequest = createReadRequest(mmsPdu);

	readRequest->specificationWithResult = NULL;

	readRequest->variableAccessSpecification.present = VariableAccessSpecification_PR_listOfVariable;

	readRequest->variableAccessSpecification.choice.listOfVariable.list.array = calloc(1, sizeof(ListOfVariableSeq_t*));
	readRequest->variableAccessSpecification.choice.listOfVariable.list.count = 1;

	listOfVars = calloc(1, sizeof(ListOfVariableSeq_t));

	readRequest->variableAccessSpecification.choice.listOfVariable.list.array[0] = listOfVars;

	listOfVars->alternateAccess = NULL;
	listOfVars->variableSpecification.present = VariableSpecification_PR_name;
	listOfVars->variableSpecification.choice.name.present = ObjectName_PR_domainspecific;
	listOfVars->variableSpecification.choice.name.choice.domainspecific.domainId.buf = domainId;
	listOfVars->variableSpecification.choice.name.choice.domainspecific.domainId.size = strlen(domainId);
	listOfVars->variableSpecification.choice.name.choice.domainspecific.itemId.buf = itemId;
	listOfVars->variableSpecification.choice.name.choice.domainspecific.itemId.size = strlen(itemId);

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
	            mmsClient_write_out, (void*) writeBuffer);

	/* clean up data structures */
	free(listOfVars);
	free(readRequest->variableAccessSpecification.choice.listOfVariable.list.array);
	readRequest->variableAccessSpecification.choice.listOfVariable.list.array = NULL;
	readRequest->variableAccessSpecification.choice.listOfVariable.list.count = 0;
	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	return rval.encoded;
}

static AlternateAccess_t*
createAlternateAccess(uint32_t index, uint32_t elementCount)
{
	AlternateAccess_t* alternateAccess = calloc(1, sizeof(AlternateAccess_t));
	alternateAccess->list.count = 1;
	alternateAccess->list.array = calloc(1, sizeof(struct AlternateAccess__Member*));
	alternateAccess->list.array[0] = calloc(1, sizeof(struct AlternateAccess__Member));
	alternateAccess->list.array[0]->present = AlternateAccess__Member_PR_unnamed;

	alternateAccess->list.array[0]->choice.unnamed.present = AlternateAccessSelection_PR_selectAccess;

	if (elementCount > 0) {
		INTEGER_t* asnIndex;
		alternateAccess->list.array[0]->choice.unnamed.choice.selectAccess.present =
				AlternateAccessSelection__selectAccess_PR_indexRange;

		asnIndex =
			&(alternateAccess->list.array[0]->choice.unnamed.choice.selectAccess.choice.indexRange.lowIndex);

		asn_long2INTEGER(asnIndex, index);

		asnIndex =
			&(alternateAccess->list.array[0]->choice.unnamed.choice.selectAccess.choice.indexRange.numberOfElements);

		asn_long2INTEGER(asnIndex, elementCount);
	}
	else {
		INTEGER_t* asnIndex;
		alternateAccess->list.array[0]->choice.unnamed.choice.selectAccess.present =
				AlternateAccessSelection__selectAccess_PR_index;

		asnIndex =
			&(alternateAccess->list.array[0]->choice.unnamed.choice.selectAccess.choice.index);

		asn_long2INTEGER(asnIndex, index);
	}

	return alternateAccess;
}

static ListOfVariableSeq_t*
createVariableIdentifier(char* domainId, char* itemId)
{
	ListOfVariableSeq_t* variableIdentifier = calloc(1, sizeof(ListOfVariableSeq_t));

	variableIdentifier->variableSpecification.present = VariableSpecification_PR_name;
	variableIdentifier->variableSpecification.choice.name.present = ObjectName_PR_domainspecific;
	variableIdentifier->variableSpecification.choice.name.choice.domainspecific.domainId.buf = domainId;
	variableIdentifier->variableSpecification.choice.name.choice.domainspecific.domainId.size = strlen(domainId);
	variableIdentifier->variableSpecification.choice.name.choice.domainspecific.itemId.buf = itemId;
	variableIdentifier->variableSpecification.choice.name.choice.domainspecific.itemId.size = strlen(itemId);

	return variableIdentifier;
}

int
mmsClient_createReadRequestAlternateAccessIndex(char* domainId, char* itemId,
		uint32_t index, uint32_t elementCount, ByteBuffer* writeBuffer)
{
	asn_enc_rval_t rval;
	ListOfVariableSeq_t* variableIdentifier;
	MmsPdu_t* mmsPdu = mmsClient_createConfirmedRequestPdu(1);
	ReadRequest_t* readRequest = createReadRequest(mmsPdu);

	readRequest->specificationWithResult = NULL;

	readRequest->variableAccessSpecification.present = VariableAccessSpecification_PR_listOfVariable;

	readRequest->variableAccessSpecification.choice.listOfVariable.list.array = calloc(1, sizeof(ListOfVariableSeq_t*));
	readRequest->variableAccessSpecification.choice.listOfVariable.list.count = 1;

	variableIdentifier = createVariableIdentifier(domainId, itemId);

	readRequest->variableAccessSpecification.choice.listOfVariable.list.array[0] = variableIdentifier;

	variableIdentifier->alternateAccess = createAlternateAccess(index, elementCount);

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
	            mmsClient_write_out, (void*) writeBuffer);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	variableIdentifier->variableSpecification.choice.name.choice.domainspecific.domainId.buf = 0;
	variableIdentifier->variableSpecification.choice.name.choice.domainspecific.domainId.size = 0;
	variableIdentifier->variableSpecification.choice.name.choice.domainspecific.itemId.buf = 0;
	variableIdentifier->variableSpecification.choice.name.choice.domainspecific.itemId.size = 0;

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	return rval.encoded;
}

static ListOfVariableSeq_t**
createListOfVariables(ReadRequest_t* readRequest, int valuesCount) {
	readRequest->variableAccessSpecification.present = VariableAccessSpecification_PR_listOfVariable;

	readRequest->variableAccessSpecification.choice.listOfVariable.list.array =
				calloc(valuesCount, sizeof(ListOfVariableSeq_t*));
	readRequest->variableAccessSpecification.choice.listOfVariable.list.count = valuesCount;
	readRequest->variableAccessSpecification.choice.listOfVariable.list.size = valuesCount;

	return readRequest->variableAccessSpecification.choice.listOfVariable.list.array;
}

/**
 * Request multiple values of a single domain
 */
int
mmsClient_createReadRequestMultipleValues(char* domainId, LinkedList items,
		ByteBuffer* writeBuffer)
{
	asn_enc_rval_t rval;
	int i;
	char* itemId;
	LinkedList item;
	ListOfVariableSeq_t** listOfVars;
	int valuesCount;
	MmsPdu_t* mmsPdu = mmsClient_createConfirmedRequestPdu(1);

	ReadRequest_t* readRequest = createReadRequest(mmsPdu);

	readRequest->specificationWithResult = NULL;

	valuesCount = LinkedList_size(items);

	listOfVars = createListOfVariables(readRequest, valuesCount);

	item = items;
	
	i = 0;

	while ((item = LinkedList_getNext(item)) != NULL) {
		itemId = (char*) (item->data);
		listOfVars[i] = createVariableIdentifier(domainId, itemId);
		i++;
	}

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
	            mmsClient_write_out, (void*) writeBuffer);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	for (i = 0; i < valuesCount; i++) {
		free(listOfVars[i]);
	}
	free(listOfVars);

	readRequest->variableAccessSpecification.choice.listOfVariable.list.count = 0;
	readRequest->variableAccessSpecification.choice.listOfVariable.list.size = 0;
	readRequest->variableAccessSpecification.choice.listOfVariable.list.array = NULL;


	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	return rval.encoded;
}

