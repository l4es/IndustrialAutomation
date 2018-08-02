/*
 *  mms_get_namelist_service.c
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

/**********************************************************************************************
 * MMS GetNameList Service
 *********************************************************************************************/

static LinkedList
getNameListVMDSpecific(MmsServerConnection* connection) {
	MmsDevice* device = MmsServer_getDevice(connection->server);

	LinkedList list = LinkedList_create();

	int i;

	for (i = 0; i < device->domainCount; i++) {
		LinkedList_add(list, device->domains[i]->domainName);
	}

	return list;
}


static void
addSubNamedVaribleNamesToList(LinkedList nameList, char* prefix, MmsTypeSpecification* variable)
{
	if (variable->type == MMS_STRUCTURE) {

		int i;

		MmsTypeSpecification** variables = variable->typeSpec.structure.elements;

		for (i = 0; i < variable->typeSpec.structure.elementCount; i++) {
			char* variableName = createString(3, prefix, "$", variables[i]->name);

			LinkedList_add(nameList, variableName);

			addSubNamedVaribleNamesToList(nameList, variableName, variables[i]);
		}
	}
	else if (variable->type == MMS_ARRAY) {
		MmsTypeSpecification* element = variable->typeSpec.array.elementTypeSpec;
		addSubNamedVaribleNamesToList(nameList, prefix, element);
	}
}

static LinkedList
getNameListDomainSpecific(MmsServerConnection* connection, char* domainName) {
	MmsDevice* device = MmsServer_getDevice(connection->server);

	LinkedList nameList = NULL;

	MmsDomain* domain = MmsDevice_getDomain(device, domainName);

	if (domain != NULL) {
		MmsTypeSpecification** variables;
		int i;
		char* prefix;
		nameList = LinkedList_create();
		variables = domain->namedVariables;

		for (i = 0; i < domain->namedVariablesCount; i++) {
			LinkedList_add(nameList, copyString(variables[i]->name));
			prefix = variables[i]->name;

			addSubNamedVaribleNamesToList(nameList, prefix, variables[i]);
		}
	}

	return nameList;
}

#ifdef MMS_DATA_SET_SERVICE 1

static LinkedList
createStringsFromNamedVariableList(LinkedList nameList, LinkedList variableLists)
{
	LinkedList variableListsElement;
	nameList = LinkedList_create();
	variableListsElement = LinkedList_getNext(variableLists);
	while (variableListsElement != NULL ) {
		MmsNamedVariableList variableList =
				(MmsNamedVariableList) variableListsElement->data;

		LinkedList_add(nameList,
				copyString(MmsNamedVariableList_getName(variableList)));

		variableListsElement = LinkedList_getNext(variableListsElement);
	}
	return nameList;
}

static LinkedList
getNamedVariableListDomainSpecific(MmsServerConnection* connection, char* domainName)
{
	MmsDevice* device = MmsServer_getDevice(connection->server);

	LinkedList nameList = NULL;

	MmsDomain* domain = MmsDevice_getDomain(device, domainName);

	if (domain != NULL) {
		LinkedList variableLists = MmsDomain_getNamedVariableLists(domain);

		nameList = createStringsFromNamedVariableList(nameList, variableLists);
	}

	return nameList;
}

static LinkedList
getNamedVariableListAssociationSpecific(MmsServerConnection* connection)
{
	LinkedList nameList = NULL;

	LinkedList variableLists = MmsServerConnection_getNamedVariableLists(connection);

	nameList = createStringsFromNamedVariableList(nameList, variableLists);

	return nameList;
}
#endif

static void
createNameListResponse(
		int invokeId,
		LinkedList nameList,
		ByteBuffer* response)
{
	asn_enc_rval_t rval;
	LinkedList element;
	int i;
	int nameCount;
	GetNameListResponse_t* getNameList;
	MmsPdu_t* mmsPdu = mmsServer_createConfirmedResponse(invokeId);

	BOOLEAN_t moreFollows = 0;

	mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.present =
			ConfirmedServiceResponse_PR_getNameList;


	getNameList = &(mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice
			.getNameList);

	getNameList->moreFollows = &moreFollows;

	nameCount = LinkedList_size(nameList);

	getNameList->listOfIdentifier.list.count = nameCount;
	getNameList->listOfIdentifier.list.size = nameCount;
	getNameList->listOfIdentifier.list.array =
			calloc(nameCount, sizeof(Identifier_t*));

	element = nameList;
	i = 0;

	while ((element = LinkedList_getNext(element)) != NULL) {
		Identifier_t* identifier = calloc(1, sizeof(Identifier_t));
		identifier->buf = (char*) (element->data);
		identifier->size = strlen((char*) (element->data));
		getNameList->listOfIdentifier.list.array[i] = identifier;
		i++;
	}

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
			mmsServer_write_out, (void*) response);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	/* free allocated memory */
	for (i = nameCount - 1; i >= 0; i--) {
		free(getNameList->listOfIdentifier.list.array[i]);
	}
	free(getNameList->listOfIdentifier.list.array);
	getNameList->listOfIdentifier.list.size = 0;
	getNameList->listOfIdentifier.list.count = 0;
	getNameList->listOfIdentifier.list.array = NULL;

	getNameList->moreFollows = 0;

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);
}

void
mmsServer_handleGetNameListRequest(
		MmsServerConnection* connection,
		GetNameListRequest_t* getNameList,
		int invokeId,
		ByteBuffer* response)
{
	long objectClass;
	long objectScope;
	char* domainSpecificName = NULL;

	asn_INTEGER2long(&getNameList->objectClass.choice.basicObjectClass, &objectClass);

	switch (getNameList->objectScope.present) {
	case GetNameListRequest__objectScope_PR_NOTHING:
		objectScope = 0;
		break;
	case GetNameListRequest__objectScope_PR_vmdSpecific:
		objectScope = 1;
		break;
	case GetNameListRequest__objectScope_PR_domainSpecific:
		objectScope = 2;
		break;
	case GetNameListRequest__objectScope_PR_aaSpecific:
		objectScope = 3;
		break;
	default:
		objectScope = -1;
		break;
	}

	if (objectScope == 2) {

		domainSpecificName = createStringFromBuffer(
				getNameList->objectScope.choice.domainSpecific.buf,
				getNameList->objectScope.choice.domainSpecific.size);

		if (objectClass == ObjectClass__basicObjectClass_namedVariable) {
			LinkedList nameList = getNameListDomainSpecific(connection, domainSpecificName);

			if (nameList == NULL)
				mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_NON_EXISTENT);
			else {
				createNameListResponse(invokeId, nameList, response);
				LinkedList_destroy(nameList);
			}
		}
#ifdef MMS_DATA_SET_SERVICE 1
		else if (objectClass == ObjectClass__basicObjectClass_namedVariableList) {
			LinkedList nameList = getNamedVariableListDomainSpecific(connection, domainSpecificName);

			createNameListResponse(invokeId, nameList, response);

			LinkedList_destroy(nameList);
		}
#endif
		else {
			if (DEBUG) printf("mms_server: getNameList objectClass %li not supported!\n", objectClass);

			mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_ACCESS_UNSUPPORTED);
		}

		free(domainSpecificName);
	}

	else if (objectScope == 1) {
		//TODO implement getNamedVariableList for VMD scope

		LinkedList nameList = getNameListVMDSpecific(connection);

		createNameListResponse(invokeId, nameList, response);

		LinkedList_destroyStatic(nameList);
	}
#ifdef MMS_DATA_SET_SERVICE 1
	else if (objectScope == 3) {

		if (objectClass == ObjectClass__basicObjectClass_namedVariableList) {
			LinkedList nameList = getNamedVariableListAssociationSpecific(connection);

			createNameListResponse(invokeId, nameList, response);

			//LinkedList_destroyStatic(nameList);
			LinkedList_destroy(nameList);
		}
		else
			mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_ACCESS_UNSUPPORTED);
	}
#endif
	else {
		if (DEBUG) printf("mms_server: getNameList(%li) not supported!\n", objectScope);
		mmsServer_createConfirmedErrorPdu(invokeId, response, MMS_ERROR_TYPE_OBJECT_ACCESS_UNSUPPORTED);
	}
}
