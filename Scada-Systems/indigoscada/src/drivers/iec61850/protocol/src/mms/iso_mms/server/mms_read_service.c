/*
 *  mms_read_service.c
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

#include "mms_server_internal.h"
#include "mms_common_internal.h"

#include "linked_list.h"

/**********************************************************************************************
 * MMS Read Service
 *********************************************************************************************/

static MmsPdu_t*
createReadResponse(int invokeId)
{
	ReadResponse_t* readResponse;
	MmsPdu_t* mmsPdu = mmsServer_createConfirmedResponse(invokeId);

	mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.present =
				ConfirmedServiceResponse_PR_read;

	readResponse =
			&(mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice.read);

	readResponse->variableAccessSpecification = NULL;

	return mmsPdu;
}



static Data_t*
addNamedVariableValue(MmsTypeSpecification* namedVariable, MmsServerConnection* connection,
		MmsDomain* domain, char* itemId, LinkedList typedValues)
{
	Data_t* dataElement = NULL;
	int componentCount;
	int i;

	if (namedVariable->type == MMS_STRUCTURE) {
		dataElement = calloc(1, sizeof(Data_t));

		componentCount = namedVariable->typeSpec.structure.elementCount;
		dataElement->present = Data_PR_structure;
		dataElement->choice.structure = calloc(1, sizeof(DataSequence_t));

		dataElement->choice.structure->list.size = componentCount;
		dataElement->choice.structure->list.count = componentCount;
		dataElement->choice.structure->list.array = calloc(componentCount, sizeof(Data_t*));

		for (i = 0; i < componentCount; i++) {
			char* newNameIdStr = createString(3, itemId, "$",
					namedVariable->typeSpec.structure.elements[i]->name);
			dataElement->choice.structure->list.array[i] =
					addNamedVariableValue(namedVariable->typeSpec.structure.elements[i],
							connection, domain,	newNameIdStr, typedValues);
			free(newNameIdStr);
		}
	}
	else {
		MmsValue* value = MmsServer_getValue(connection->server, domain, itemId);

		//TODO add checks???
		//TODO check if value != NULL

		LinkedList_add(typedValues, value);

		dataElement = mmsMsg_createBasicDataElement(value);
	}

	return dataElement;
}

static void
addComplexValueToResultList(AccessResult_t* accessResult, MmsTypeSpecification* namedVariable,
								LinkedList typedValues, MmsServerConnection* connection,
								MmsDomain* domain, char* nameIdStr)
{
	int i;
	int componentCount = namedVariable->typeSpec.structure.elementCount;

	accessResult->present = AccessResult_PR_structure;
	accessResult->choice.structure.list.count = componentCount;
	accessResult->choice.structure.list.size = componentCount;

	accessResult->choice.structure.list.array = calloc(componentCount, sizeof(Data_t*));

	for (i = 0; i < componentCount; i++) {
		char* newNameIdStr = createString(3, nameIdStr, "$",
				namedVariable->typeSpec.structure.elements[i]->name);
		accessResult->choice.structure.list.array[i] =
				addNamedVariableValue(namedVariable->typeSpec.structure.elements[i],
						connection, domain, newNameIdStr,typedValues);
		free(newNameIdStr);
	}
}

static void
deleteDataElement(Data_t* dataElement) {
	if (dataElement->present == Data_PR_structure) {
		int elementCount = dataElement->choice.structure->list.count;

		int i;
		for (i = 0; i < elementCount; i++) {
			deleteDataElement(dataElement->choice.structure->list.array[i]);
		}

		free(dataElement->choice.structure->list.array);
		free(dataElement->choice.structure);
	}
	else if (dataElement->present == Data_PR_array) {
		int elementCount = dataElement->choice.array->list.count;

		int i;
		for (i = 0; i < elementCount; i++) {
			deleteDataElement(dataElement->choice.array->list.array[i]);
		}

		free(dataElement->choice.array->list.array);
		free(dataElement->choice.array);
	}
	else if(dataElement->present == Data_PR_floatingpoint) {
		free(dataElement->choice.floatingpoint.buf);
	}
	else if(dataElement->present == Data_PR_utctime) {
		free(dataElement->choice.utctime.buf);
	}

	free(dataElement);
}

static void
deleteAccessResultList(AccessResult_t** accessResult, int variableCount)
{
	int i;

	for (i = 0; i < variableCount; i++) {

		if (accessResult[i]->present == AccessResult_PR_structure) {
			int elementCount = accessResult[i]->choice.structure.list.count;

			int j;

			for (j = 0; j < elementCount; j++) {
				deleteDataElement(accessResult[i]->choice.structure.list.array[j]);
			}

			free(accessResult[i]->choice.structure.list.array);
		}
		else if (accessResult[i]->present == AccessResult_PR_array) {
			int elementCount = accessResult[i]->choice.array.list.count;

			int j;

			for (j = 0; j < elementCount; j++) {
				deleteDataElement(accessResult[i]->choice.array.list.array[j]);
			}

			free(accessResult[i]->choice.array.list.array);
		}
		else if(accessResult[i]->present == AccessResult_PR_integer) {
			free(accessResult[i]->choice.integer.buf);
		}
		else if(accessResult[i]->present == AccessResult_PR_unsigned) {
			free(accessResult[i]->choice.Unsigned.buf);
		}
		else if(accessResult[i]->present == AccessResult_PR_floatingpoint) {
			free(accessResult[i]->choice.floatingpoint.buf);
		}
		else if(accessResult[i]->present == AccessResult_PR_utctime) {
			free(accessResult[i]->choice.utctime.buf);
		}
		else if(accessResult[i]->present == AccessResult_PR_failure) {
			free(accessResult[i]->choice.failure.buf);
		}

		free(accessResult[i]);
	}

	free(accessResult);
}

void
appendValueToResultList(MmsValue* value, LinkedList values,
		AccessResult_t* accessResultList)
{

	if (value != NULL )
		LinkedList_add(values, value);

	mmsMsg_addResultToResultList(accessResultList, value);
}

static void
deleteValueList(LinkedList values)
{
	LinkedList value = values;
	MmsValue* typedValue;
	while ((value = LinkedList_getNext(value)) != NULL ) {
		typedValue = (MmsValue*) (value->data);

		if (typedValue->deleteValue == 1) {
			MmsValue_delete(typedValue);
		}
	}
	LinkedList_destroyStatic(values);
}


void
alternateArrayAccess(MmsServerConnection* connection,
		AlternateAccess_t* alternateAccess, MmsDomain* domain,
		char* itemId, LinkedList values, AccessResult_t* accessResult)
{
	if (mmsServer_isIndexAccess(alternateAccess))
	{
		MmsValue* arrayValue;
		MmsValue* value = NULL;
		int index;
		int lowIndex = mmsServer_getLowIndex(alternateAccess);
		int numberOfElements = mmsServer_getNumberOfElements(alternateAccess);

		if (DEBUG) printf("Alternate access index: %i elements %i\n",
				lowIndex, numberOfElements);

		index = lowIndex;

		arrayValue = MmsServer_getValue(connection->server, domain, itemId);

		if (arrayValue != NULL) {

			if (numberOfElements == 0)
				value = MmsValue_getElement(arrayValue, index);
			else {
				int resultIndex = 0;
				value = MmsValue_createEmtpyArray(numberOfElements);

				MmsValue_setDeletable(value);
				
				while (index < lowIndex + numberOfElements) {
					MmsValue* elementValue = NULL;

					elementValue = MmsValue_getElement(arrayValue, index);

					if (!MmsValue_isDeletable(elementValue))
						elementValue = MmsValue_clone(elementValue);

					MmsValue_setArrayElement(value, resultIndex, elementValue);

					index++;
					resultIndex++;
				}
			}

			appendValueToResultList(value, values, accessResult);

			if (MmsValue_isDeletable(arrayValue))
				MmsValue_delete(arrayValue);

		}
		else
			mmsMsg_addResultToResultList(accessResult, NULL);



	}
	else { // invalid access
		if (DEBUG) printf("Invalid alternate access\n");
		mmsMsg_addResultToResultList(accessResult, NULL);
	}
}

static void //TODO remove me
singleValueAccess(LinkedList values, 	MmsServerConnection* connection,
		MmsDomain* domain, char* itemId,
		AccessResult_t* accessResult)
{
	MmsValue* value = MmsServer_getValue(connection->server, domain, itemId);
	appendValueToResultList(value, values, accessResult);
}

void
addNamedVariableToResultList(MmsTypeSpecification* namedVariable, MmsDomain* domain, char* nameIdStr, AccessResult_t* resultListEntry,
		LinkedList /*<MmsValue>*/ values, MmsServerConnection* connection, AlternateAccess_t* alternateAccess)
{
	if (namedVariable != NULL) {

		if (DEBUG) printf("MMS read: found named variable %s with search string %s\n",
				namedVariable->name, nameIdStr);

		if (namedVariable->type == MMS_STRUCTURE) {
			addComplexValueToResultList(resultListEntry, namedVariable,
					values, connection, domain, nameIdStr);
		}
		else if (namedVariable->type == MMS_ARRAY) {

			if (alternateAccess != NULL) {
				alternateArrayAccess(connection, alternateAccess, domain,
						nameIdStr, values, resultListEntry);
			}
			else { //getCompleteArray
				singleValueAccess(values, connection, domain, nameIdStr,
						resultListEntry);
			}
		}
		else {
			singleValueAccess(values, connection, domain, nameIdStr,
					resultListEntry);
		}

	}
	else {
		mmsMsg_addResultToResultList(resultListEntry, NULL);
	}
}

static void
freeMmsReadResponsePdu(MmsPdu_t* mmsPdu)
{
	int accessResultSize =
			mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice.read.listOfAccessResult.list.count;

	AccessResult_t** accessResult =
			mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice.read.listOfAccessResult.list.array;

	deleteAccessResultList(accessResult, accessResultSize);

	mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice.read.listOfAccessResult.list.array = 0;
	mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice.read.listOfAccessResult.list.count = 0;
	mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice.read.listOfAccessResult.list.size =	0;

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);
}

static bool
isSpecWithResult(ReadRequest_t* read)
{
	if (read->specificationWithResult != NULL)
		if (*(read->specificationWithResult) != false)
			return true;

	return false;
}

static void
handleReadListOfVariablesRequest(
		MmsServerConnection* connection,
		ReadRequest_t* read,
		int invokeId,
		ByteBuffer* response)
{
	int i;
	int variableCount = read->variableAccessSpecification.choice.listOfVariable.list.count;

	MmsPdu_t* mmsPdu = createReadResponse(invokeId);

	AccessResult_t** accessResultList =
				mmsMsg_createAccessResultsList(mmsPdu, variableCount);

	LinkedList /*<MmsValue>*/ values = LinkedList_create();

	if (isSpecWithResult(read)) { /* add specification to result */
		mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice.read.variableAccessSpecification
			= &(read->variableAccessSpecification);
	}

	MmsServer_lockModel(connection->server);

	for (i = 0; i < variableCount; i++) {
		AccessResult_t* resultListEntry = accessResultList[i];

		VariableSpecification_t varSpec =
			read->variableAccessSpecification.choice.listOfVariable.list.array[i]->variableSpecification;

		AlternateAccess_t* alternateAccess =
			read->variableAccessSpecification.choice.listOfVariable.list.array[i]->alternateAccess;

		if (varSpec.present == VariableSpecification_PR_name) {

			if (varSpec.choice.name.present == ObjectName_PR_domainspecific) {

				MmsTypeSpecification* namedVariable;
				char* domainIdStr = mmsMsg_createStringFromAsnIdentifier(
						varSpec.choice.name.choice.domainspecific.domainId);

				char* nameIdStr = mmsMsg_createStringFromAsnIdentifier(
						varSpec.choice.name.choice.domainspecific.itemId);

				MmsDomain* domain = MmsDevice_getDomain(MmsServer_getDevice(connection->server), domainIdStr);

				if (domain == NULL) {
					if (DEBUG) printf("MMS read: domain %s not found!\n", domainIdStr);
					mmsMsg_addResultToResultList(resultListEntry, NULL);
					break;
				}

				namedVariable = MmsDomain_getNamedVariable(domain, nameIdStr);

				addNamedVariableToResultList(namedVariable, domain, nameIdStr, resultListEntry,
						values, connection, alternateAccess);

				free(domainIdStr);
				free(nameIdStr);
			}
			else {
				mmsMsg_addResultToResultList(resultListEntry, NULL);
				if (DEBUG) printf("MMS read: object name type not supported!\n");
			}
		}
		else {
			//TODO should we send a ConfirmedErrorPdu here?
			mmsMsg_addResultToResultList(resultListEntry, NULL);
			if (DEBUG) printf("MMS read: varspec type not supported!\n");
		}
	}

	MmsServer_unlockModel(connection->server);

	der_encode(&asn_DEF_MmsPdu, mmsPdu, mmsServer_write_out, (void*) response);

	deleteValueList(values);

	mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice.read.variableAccessSpecification
		= NULL;

	freeMmsReadResponsePdu(mmsPdu);
}



#if MMS_DATA_SET_SERVICE == 1

static void
createNamedVariableListResponse(MmsServerConnection* connection, MmsNamedVariableList namedList,
		int invokeId, ByteBuffer* response, ReadRequest_t* read)
{
	LinkedList variable;
	int i;
	MmsPdu_t* mmsPdu = createReadResponse(invokeId);

	LinkedList /*<MmsValue>*/ values = LinkedList_create();
	LinkedList variables = MmsNamedVariableList_getVariableList(namedList);

	int variableCount = LinkedList_size(variables);

	AccessResult_t** accessResultList =
		mmsMsg_createAccessResultsList(mmsPdu, variableCount);

	if (isSpecWithResult(read)) { /* add specification to result */
		mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice.read.variableAccessSpecification
			= &(read->variableAccessSpecification);
	}

	MmsServer_lockModel(connection->server);

	variable = LinkedList_getNext(variables);

	for (i = 0; i < variableCount; i++) {
		AccessResult_t* resultListEntry = accessResultList[i];
		//char* variableName = (char*)

		MmsNamedVariableListEntry variableListEntry = (MmsNamedVariableListEntry) variable->data;

		MmsDomain* variableDomain = MmsNamedVariableListEntry_getDomain(variableListEntry);
		char* variableName = MmsNamedVariableListEntry_getVariableName(variableListEntry);

		MmsTypeSpecification* namedVariable = MmsDomain_getNamedVariable(variableDomain,
				variableName);

		addNamedVariableToResultList(namedVariable, variableDomain, variableName, resultListEntry,
								values, connection, NULL);

		variable = LinkedList_getNext(variable);
	}

	MmsServer_unlockModel(connection->server);

	der_encode(&asn_DEF_MmsPdu, mmsPdu, mmsServer_write_out, (void*) response);

	deleteValueList(values);

	mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice.read.variableAccessSpecification
			= NULL;

	freeMmsReadResponsePdu(mmsPdu);
}

static void
handleReadNamedVariableListRequest(
		MmsServerConnection* connection,
		ReadRequest_t* read,
		int invokeId,
		ByteBuffer* response)
{
	if (read->variableAccessSpecification.choice.variableListName.present ==
			ObjectName_PR_domainspecific)
	{
		char* domainId = mmsMsg_createStringFromAsnIdentifier(
				read->variableAccessSpecification.choice.variableListName.choice.domainspecific.domainId);

		char* listName = mmsMsg_createStringFromAsnIdentifier(
				read->variableAccessSpecification.choice.variableListName.choice.domainspecific.itemId);

		MmsDomain* domain = MmsDevice_getDomain(MmsServer_getDevice(connection->server), domainId);

		if (domain == NULL) {
			if (DEBUG) printf("MMS read: domain %s not found!\n", domainId);
			mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_NON_EXISTENT);
		}
		else {
			MmsNamedVariableList namedList = MmsDomain_getNamedVariableList(domain, listName);

			if (namedList != NULL) {
				createNamedVariableListResponse(connection, namedList, invokeId, response, read);
			}
			else {
				if (DEBUG) printf("MMS read: named variable list %s not found!\n", listName);
				mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_NON_EXISTENT);
			}
		}

		free(domainId);
		free(listName);
	}
	else if (read->variableAccessSpecification.choice.variableListName.present ==
				ObjectName_PR_aaspecific)
	{
		char* listName = mmsMsg_createStringFromAsnIdentifier(read->variableAccessSpecification.
				choice.variableListName.choice.aaspecific);

		MmsNamedVariableList namedList = MmsServerConnection_getNamedVariableList(connection, listName);

		if (namedList == NULL)
			mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_NON_EXISTENT);
		else {
			createNamedVariableListResponse(connection, namedList, invokeId, response, read);
		}

		free(listName);
	}
	else
		mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_ACCESS_UNSUPPORTED);
}

#endif /* MMS_DATA_SET_SERVICE == 1 */

void
mmsServer_handleReadRequest(
		MmsServerConnection* connection,
		ReadRequest_t* read,
		int invokeId,
		ByteBuffer* response)
{
	if (read->variableAccessSpecification.present == VariableAccessSpecification_PR_listOfVariable) {
		handleReadListOfVariablesRequest(connection, read, invokeId, response);
	}
#if MMS_DATA_SET_SERVICE == 1
	else if (read->variableAccessSpecification.present == VariableAccessSpecification_PR_variableListName) {
		handleReadNamedVariableListRequest(connection, read, invokeId, response);
	}
#endif
	else {
		mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_ACCESS_UNSUPPORTED);
	}
}

