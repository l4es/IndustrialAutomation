/*
 *  mms_named_variable_list_service.c
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
#include "mms_server_internal.h"
#include "mms_named_variable_list.h"

#include "string_utilities.h"

#if MMS_DATA_SET_SERVICE == 1

static void
createDeleteNamedVariableListResponse(uint32_t invokeId, ByteBuffer* response,
		long numberMatched, long numberDeleted)
{
	asn_enc_rval_t rval;
	DeleteNamedVariableListResponse_t* _delete;
	MmsPdu_t* mmsPdu = mmsServer_createConfirmedResponse(invokeId);

	mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.present =
			ConfirmedServiceResponse_PR_deleteNamedVariableList;

	_delete =
		&(mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice.deleteNamedVariableList);

	asn_long2INTEGER(&(_delete->numberDeleted), numberDeleted);
	asn_long2INTEGER(&(_delete->numberMatched), numberMatched);

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
			mmsServer_write_out, (void*) response);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);
}

void
mmsServer_handleDeleteNamedVariableListRequest(MmsServerConnection* connection,
		DeleteNamedVariableListRequest_t* request,
		int invokeId,
		ByteBuffer* response)
{
	//TODO implement for association specific named variable lists

	MmsDevice* device;
	long scopeOfDelete;
	request->scopeOfDelete;

	asn_INTEGER2long(request->scopeOfDelete, &scopeOfDelete);

	device = MmsServer_getDevice(connection->server);

	if (scopeOfDelete == DeleteNamedVariableListRequest__scopeOfDelete_specific) {
		int numberMatched = 0;
		int numberDeleted = 0;

		int numberItems = request->listOfVariableListName->list.count;

		int i;

		for (i = 0; i < numberItems; i++) {
			if (request->listOfVariableListName->list.array[i]->present == ObjectName_PR_domainspecific) {
				char* domainId = mmsMsg_createStringFromAsnIdentifier(
						request->listOfVariableListName->list.array[i]->choice.domainspecific.domainId);

				MmsDomain* domain = MmsDevice_getDomain(device, domainId);

				char* itemId = mmsMsg_createStringFromAsnIdentifier(
						request->listOfVariableListName->list.array[i]->choice.domainspecific.itemId);

				MmsNamedVariableList variableList = MmsDomain_getNamedVariableList(domain, itemId);

				if (variableList != NULL) {
					numberMatched++;

					if (MmsNamedVariableList_isDeletable(variableList)) {
						MmsDomain_deleteNamedVariableList(domain, itemId);
						numberDeleted++;
					}
				}

				free(domainId);
				free(itemId);
			}
			else if (request->listOfVariableListName->list.array[i]->present == ObjectName_PR_aaspecific) {
				char* itemId = mmsMsg_createStringFromAsnIdentifier(
						request->listOfVariableListName->list.array[i]->choice.aaspecific);

				MmsNamedVariableList variableList = MmsServerConnection_getNamedVariableList(connection, itemId);

				if (variableList != NULL) {
					numberMatched++;
					numberDeleted++;

					MmsServerConnection_deleteNamedVariableList(connection, itemId);
				}

				free(itemId);
			}
			//TODO else send error???
		}

		createDeleteNamedVariableListResponse(invokeId, response, numberMatched, numberDeleted);
	}
	else {
		mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_ACCESS_UNSUPPORTED);
	}
}

static void
createDefineNamedVariableListResponse(int invokeId, ByteBuffer* response)
{
	asn_enc_rval_t rval;
	MmsPdu_t* mmsPdu = mmsServer_createConfirmedResponse(invokeId);

	mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.present =
			ConfirmedServiceResponse_PR_defineNamedVariableList;

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
			mmsServer_write_out, (void*) response);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);
}

static MmsNamedVariableList
createNamedVariableList(MmsDevice* device,
		DefineNamedVariableListRequest_t* request,
		char* variableListName)
{
	MmsNamedVariableList namedVariableList = MmsNamedVariableList_create(variableListName, true);

	int variableCount = request->listOfVariable.list.count;

	int i;
	for (i = 0; i < variableCount; i++) {
		VariableSpecification_t* varSpec =
				&request->listOfVariable.list.array[i]->variableSpecification;

		if (varSpec->present == VariableSpecification_PR_name) {
			char* variableName = createStringFromBuffer(
					varSpec->choice.name.choice.domainspecific.itemId.buf,
					varSpec->choice.name.choice.domainspecific.itemId.size);

			char* domainId = createStringFromBuffer(
					varSpec->choice.name.choice.domainspecific.domainId.buf,
					varSpec->choice.name.choice.domainspecific.domainId.size);

			MmsDomain* domain = MmsDevice_getDomain(device, domainId);

			MmsNamedVariableListEntry variable =
					MmsNamedVariableListEntry_create(domain, variableName);

			MmsNamedVariableList_addVariable(namedVariableList, variable);

			free(domainId);
			free(variableName);
		}
		else {
			MmsNamedVariableList_destroy(namedVariableList);
			namedVariableList = NULL;
			break;
		}
	}

	return namedVariableList;
}

void
mmsServer_handleDefineNamedVariableListRequest(
		MmsServerConnection* connection,
		DefineNamedVariableListRequest_t* request,
		int invokeId,
		ByteBuffer* response)
{
	MmsDevice* device = MmsServer_getDevice(connection->server);

	if (request->variableListName.present == ObjectName_PR_domainspecific) {

		MmsNamedVariableList namedVariableList;
		char* variableListName;
		char* domainName = createStringFromBuffer(
				request->variableListName.choice.domainspecific.domainId.buf,
				request->variableListName.choice.domainspecific.domainId.size);

		MmsDomain* domain = MmsDevice_getDomain(device, domainName);

		free(domainName);

		if (domain == NULL) {
			mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_NON_EXISTENT);
			return;
		}

		variableListName = createStringFromBuffer(
				request->variableListName.choice.domainspecific.itemId.buf,
				request->variableListName.choice.domainspecific.itemId.size);

		namedVariableList = createNamedVariableList(device,
				request, variableListName);

		free(variableListName);

		if (namedVariableList != NULL) {
			MmsDomain_addNamedVariableList(domain, namedVariableList);
			createDefineNamedVariableListResponse(invokeId, response);
		}
		else
			mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_ACCESS_UNSUPPORTED);

	}
	else if (request->variableListName.present == ObjectName_PR_aaspecific) {

		char* variableListName = createStringFromBuffer(
				request->variableListName.choice.aaspecific.buf,
				request->variableListName.choice.aaspecific.size);

		MmsNamedVariableList namedVariableList = createNamedVariableList(device,
				request, variableListName);

		if (namedVariableList != NULL) {
			MmsServerConnection_addNamedVariableList(connection, namedVariableList);
			createDefineNamedVariableListResponse(invokeId, response);
		}
		else
			mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_ACCESS_UNSUPPORTED);

		free(variableListName);
	}
	else
		mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_ACCESS_UNSUPPORTED);
}

static void
createGetNamedVariableListAttributesResponse(int invokeId, ByteBuffer* response,
		MmsNamedVariableList variableList, char* domainName)
{
	char* variableDomainName;
	asn_enc_rval_t rval;
	int i;
	int variableCount;
	LinkedList variable;
	LinkedList variables;
	GetNamedVariableListAttributesResponse_t* varListResponse;
	MmsPdu_t* mmsPdu = mmsServer_createConfirmedResponse(invokeId);

	BOOLEAN_t moreFollows = 0;

	mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.present =
			ConfirmedServiceResponse_PR_getNamedVariableListAttributes;

	varListResponse =
		&(mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.
				choice.getNamedVariableListAttributes);

	varListResponse->mmsDeletable = MmsNamedVariableList_isDeletable(variableList);

	variables = MmsNamedVariableList_getVariableList(variableList);

	variableCount = LinkedList_size(variables);

	varListResponse->listOfVariable.list.count = variableCount;
	varListResponse->listOfVariable.list.size = variableCount;

	varListResponse->listOfVariable.list.array = calloc(variableCount, sizeof(void*));

	variable = LinkedList_getNext(variables);

	for (i = 0; i < variableCount; i++) {
		MmsNamedVariableListEntry variableEntry = (MmsNamedVariableListEntry) variable->data;

		varListResponse->listOfVariable.list.array[i] =  calloc(1,
				sizeof(struct GetNamedVariableListAttributesResponse__listOfVariable__Member));

		varListResponse->listOfVariable.list.array[i]->variableSpecification.present =
				VariableSpecification_PR_name;

		varListResponse->listOfVariable.list.array[i]->variableSpecification.choice.name.present =
				ObjectName_PR_domainspecific;

		variableDomainName = MmsDomain_getName(variableEntry->domain);

		varListResponse->listOfVariable.list.array[i]->variableSpecification.choice.name.choice.
			domainspecific.domainId.buf = copyString(variableDomainName);

		varListResponse->listOfVariable.list.array[i]->variableSpecification.choice.name.choice.
			domainspecific.domainId.size = strlen(variableDomainName);

		varListResponse->listOfVariable.list.array[i]->variableSpecification.choice.name.choice.
			domainspecific.itemId.buf = copyString(variableEntry->variableName);

		varListResponse->listOfVariable.list.array[i]->variableSpecification.choice.name.choice.
			domainspecific.itemId.size = strlen(variableEntry->variableName);

		variable = LinkedList_getNext(variable);
	}

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
			mmsServer_write_out, (void*) response);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);


	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);
}

void
mmsServer_handleGetNamedVariableListAttributesRequest(
		MmsServerConnection* connection,
		GetNamedVariableListAttributesRequest_t* request,
		int invokeId,
		ByteBuffer* response)
{
	if (request->present == ObjectName_PR_domainspecific) {

		char* domainName = createStringFromBuffer(
				request->choice.domainspecific.domainId.buf,
				request->choice.domainspecific.domainId.size);

		char* itemName = createStringFromBuffer(
				request->choice.domainspecific.itemId.buf,
				request->choice.domainspecific.itemId.size);

		MmsDevice* mmsDevice = MmsServer_getDevice(connection->server);

		MmsDomain* domain = MmsDevice_getDomain(mmsDevice, domainName);

		if (domain != NULL) {
			MmsNamedVariableList variableList =
					MmsDomain_getNamedVariableList(domain, itemName);

			if (variableList != NULL)
				createGetNamedVariableListAttributesResponse(invokeId, response, variableList, domainName);
			else
				mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_NON_EXISTENT);
		}
		else
			mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_NON_EXISTENT);


		free(domainName);
		free(itemName);
	}
	else {
		mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_ACCESS_UNSUPPORTED);
	}
}

#endif
