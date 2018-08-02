/*
 *  mms_mapping.c
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
#include <string.h>
#include <stdio.h>
#include "mms_mapping.h"
#include "linked_list.h"
#include "array_list.h"
#include "stack_config.h"


static MmsTypeSpecification*
createNamedVariableFromDataAttribute(DataAttribute* attribute)
{
	MmsTypeSpecification* namedVariable;
	MmsTypeSpecification* origNamedVariable = calloc(1, sizeof(MmsTypeSpecification));
	origNamedVariable->name = attribute->name;

	namedVariable = origNamedVariable;

//	/*
//	TIMESTAMP,
//	CHECK,
//	// DBPOS,
//	// TCMD,
//	CONSTRUCTED
//	 */
	if (attribute->arraySize > 0) {
		namedVariable->type = MMS_ARRAY;
		namedVariable->typeSpec.array.elementCount = attribute->arraySize;
		namedVariable->typeSpec.array.elementTypeSpec =
				calloc(1, sizeof(MmsTypeSpecification));
		namedVariable = namedVariable->typeSpec.array.elementTypeSpec;
	}

	if (attribute->subDataAttributes != NULL) {
		int i;
		int componentCount;
		namedVariable->type = MMS_STRUCTURE;

		componentCount = ArrayList_listSize((void**) (attribute->subDataAttributes));

		namedVariable->typeSpec.structure.elements =
				calloc(componentCount, sizeof(MmsTypeSpecification*));

		i = 0;
		while (attribute->subDataAttributes[i] != NULL) {
			namedVariable->typeSpec.structure.elements[i] =
					createNamedVariableFromDataAttribute(attribute->subDataAttributes[i]);
			i++;
		}

		namedVariable->typeSpec.structure.elementCount = i;
	}
	else {
		switch (attribute->type) {
		case BOOLEAN:
			namedVariable->type = MMS_BOOLEAN;
			break;
		case INT8:
			namedVariable->typeSpec.integer = 8;
			namedVariable->type = MMS_INTEGER;
			break;
		case INT16:
			namedVariable->typeSpec.integer = 16;
			namedVariable->type = MMS_INTEGER;
			break;
		case INT32:
			namedVariable->typeSpec.integer = 32;
			namedVariable->type = MMS_INTEGER;
			break;
		case INT64:
			namedVariable->typeSpec.integer = 64;
			namedVariable->type = MMS_INTEGER;
			break;
		case INT8U:
			namedVariable->typeSpec.unsignedInteger = 8;
			namedVariable->type = MMS_UNSIGNED;
			break;
		case INT16U:
			namedVariable->typeSpec.unsignedInteger = 16;
			namedVariable->type = MMS_UNSIGNED;
			break;
		case INT24U:
			namedVariable->typeSpec.unsignedInteger = 24;
			namedVariable->type = MMS_UNSIGNED;
			break;
		case INT32U:
			namedVariable->typeSpec.unsignedInteger = 32;
			namedVariable->type = MMS_UNSIGNED;
			break;
		case FLOAT32:
			namedVariable->typeSpec.floatingpoint.formatWidth = 32;
			namedVariable->typeSpec.floatingpoint.exponentWidth = 8;
			namedVariable->type = MMS_FLOAT;
			break;
		case FLOAT64:
			namedVariable->typeSpec.floatingpoint.formatWidth = 64;
			namedVariable->typeSpec.floatingpoint.exponentWidth = 11;
			namedVariable->type = MMS_FLOAT;
			break;
		case ENUMERATED:
			namedVariable->typeSpec.integer = 32; // TODO fixme
			namedVariable->type = MMS_INTEGER;
			break;
		case CODEDENUM:
			namedVariable->typeSpec.bitString = 2;
			namedVariable->type = MMS_BIT_STRING;
			break;
		case OCTET_STRING_6:
			namedVariable->typeSpec.octetString = -6;
			namedVariable->type = MMS_OCTET_STRING;
			break;
		case OCTET_STRING_8:
			namedVariable->typeSpec.octetString = 8;
			namedVariable->type = MMS_OCTET_STRING;
			break;
		case OCTET_STRING_64:
			namedVariable->typeSpec.octetString = -64;
			namedVariable->type = MMS_OCTET_STRING;
			break;
		case VISIBLE_STRING_32:
			namedVariable->typeSpec.visibleString = -129;
			namedVariable->type = MMS_VISIBLE_STRING;
			break;
		case VISIBLE_STRING_64:
			namedVariable->typeSpec.visibleString = -129;
			namedVariable->type = MMS_VISIBLE_STRING;
			break;
		case VISIBLE_STRING_65:
			namedVariable->typeSpec.visibleString = -129;
			namedVariable->type = MMS_VISIBLE_STRING;
			break;
		case VISIBLE_STRING_129:
			namedVariable->typeSpec.visibleString = -129;
			namedVariable->type = MMS_VISIBLE_STRING;
			break;
		case VISIBLE_STRING_255:
			namedVariable->typeSpec.visibleString = -255;
			namedVariable->type = MMS_VISIBLE_STRING;
			break;
		case UNICODE_STRING_255:
			namedVariable->typeSpec.mmsString = -255;
			namedVariable->type = MMS_STRING;
			break;
		case GENERIC_BITSTRING:
			namedVariable->type = MMS_BIT_STRING;
			break;
		case TIMESTAMP:
			namedVariable->type = MMS_UTC_TIME;
			break;
		case QUALITY:
			namedVariable->typeSpec.bitString = -13;
			namedVariable->type = MMS_BIT_STRING;
			break;
		default:
			printf("MMS-MAPPING: type cannot be mapped %i\n", attribute->type);
			break;
		}
	}

	return origNamedVariable;
}

static MmsTypeSpecification*
createFCNamedVariableFromDataObject(DataObject* dataObject, FunctionalConstraint fc)
{
	/* Determine size of components TODO determine minimum count */
	int elementCount = 0;
	int i = 0;

	MmsTypeSpecification* namedVariable = calloc(1, sizeof(MmsTypeSpecification));
	namedVariable->name = dataObject->name;
	namedVariable->type = MMS_STRUCTURE;

	if (dataObject->dataAttributes != NULL) {
		elementCount += ArrayList_listSize((void**) (dataObject->dataAttributes));
	}

	if (dataObject->subDataObjects != NULL) {
		elementCount += ArrayList_listSize((void**) (dataObject->subDataObjects));
	}

	/* Allocate memory for components */
	namedVariable->typeSpec.structure.elements =
			calloc(elementCount, sizeof(MmsTypeSpecification*));

	elementCount = 0;

	/* Create components from data attributes */
	if (dataObject->dataAttributes != NULL) {
		i = 0;
		while (dataObject->dataAttributes[i] != NULL) {
			if (dataObject->dataAttributes[i]->fc == fc) {
				namedVariable->typeSpec.structure.elements[elementCount] =
						createNamedVariableFromDataAttribute(dataObject->dataAttributes[i]);
				elementCount++;
			}
			i++;
		}
	}

	/* Create components from sub data objects */
	if (dataObject->subDataObjects != NULL) {
		i = 0;
		while (dataObject->subDataObjects[i] != NULL) {
			if (DataObject_hasFCData(dataObject->subDataObjects[i], fc)) {
				namedVariable->typeSpec.structure.elements[elementCount]
				   = createFCNamedVariableFromDataObject(dataObject->subDataObjects[i], fc);
				elementCount++;
			}
			i++;
		}
	}

	namedVariable->typeSpec.structure.elementCount = elementCount;
	return namedVariable;
}

static MmsTypeSpecification*
createFCNamedVariable(LogicalNode* logicalNode, FunctionalConstraint fc)
{
	int dataObjectCount = 0;
	int i = 0;

	MmsTypeSpecification* namedVariable = calloc(1, sizeof(MmsTypeSpecification));
	namedVariable->name = FunctionalConstrained_toString(fc);
	namedVariable->type = MMS_STRUCTURE;

	while (logicalNode->dataObjects[i] != NULL) {
		if (DataObject_hasFCData(logicalNode->dataObjects[i], fc)) {
			dataObjectCount++;
		}
		i++;
	}

	namedVariable->typeSpec.structure.elementCount = dataObjectCount;
	namedVariable->typeSpec.structure.elements =
			calloc(dataObjectCount, sizeof(MmsTypeSpecification*));

	i = 0;
	dataObjectCount = 0;
	while (logicalNode->dataObjects[i] != NULL) {
		if (DataObject_hasFCData(logicalNode->dataObjects[i], fc)) {

			namedVariable->typeSpec.structure.elements[dataObjectCount] =
					createFCNamedVariableFromDataObject(logicalNode->dataObjects[i], fc);

			dataObjectCount++;
		}
		i++;
	}

	return namedVariable;
}


static MmsTypeSpecification*
createNamedVariableFromLogicalNode(LogicalNode* logicalNode)
{
	int componentCount = 0;
	MmsTypeSpecification* namedVariable = malloc(sizeof(MmsTypeSpecification));

	namedVariable->name = logicalNode->name;

	namedVariable->type = MMS_STRUCTURE;

	//TODO implement memory allocation with correct number of elements!
	namedVariable->typeSpec.structure.elements = calloc(14, sizeof(MmsTypeSpecification*));

	/* Create a named variable of type structure for each functional constrained */


	if (LogicalNode_hasFCData(logicalNode, ST)) {
		namedVariable->typeSpec.structure.elements[componentCount] =
				createFCNamedVariable(logicalNode, ST);
		componentCount++;
	}

	if (LogicalNode_hasFCData(logicalNode, MX)) {
		namedVariable->typeSpec.structure.elements[componentCount] =
				createFCNamedVariable(logicalNode, MX);
		componentCount++;
	}

	if (LogicalNode_hasFCData(logicalNode, SP)) {
		namedVariable->typeSpec.structure.elements[componentCount] =
				createFCNamedVariable(logicalNode, SP);
		componentCount++;
	}

	if (LogicalNode_hasFCData(logicalNode, SV)) {
		namedVariable->typeSpec.structure.elements[componentCount] =
				createFCNamedVariable(logicalNode, SV);
		componentCount++;
	}

	if (LogicalNode_hasFCData(logicalNode, CF)) {
		namedVariable->typeSpec.structure.elements[componentCount] =
				createFCNamedVariable(logicalNode, CF);
		componentCount++;
	}

	if (LogicalNode_hasFCData(logicalNode, DC)) {
		namedVariable->typeSpec.structure.elements[componentCount] =
				createFCNamedVariable(logicalNode, DC);
		componentCount++;
	}

	if (LogicalNode_hasFCData(logicalNode, SG)) {
		namedVariable->typeSpec.structure.elements[componentCount] =
				createFCNamedVariable(logicalNode, SG);
		componentCount++;
	}

	if (LogicalNode_hasFCData(logicalNode, SE)) {
		namedVariable->typeSpec.structure.elements[componentCount] =
				createFCNamedVariable(logicalNode, SE);
		componentCount++;
	}

	if (LogicalNode_hasFCData(logicalNode, SR)) {
		namedVariable->typeSpec.structure.elements[componentCount] =
				createFCNamedVariable(logicalNode, SR);
		componentCount++;
	}

	if (LogicalNode_hasFCData(logicalNode, OR)) {
		namedVariable->typeSpec.structure.elements[componentCount] =
				createFCNamedVariable(logicalNode, OR);
		componentCount++;
	}

	if (LogicalNode_hasFCData(logicalNode, BL)) {
		namedVariable->typeSpec.structure.elements[componentCount] =
				createFCNamedVariable(logicalNode, BL);
		componentCount++;
	}

	if (LogicalNode_hasFCData(logicalNode, EX)) {
		namedVariable->typeSpec.structure.elements[componentCount] =
				createFCNamedVariable(logicalNode, EX);
		componentCount++;
	}

	if (LogicalNode_hasFCData(logicalNode, CO)) {
		namedVariable->typeSpec.structure.elements[componentCount] =
				createFCNamedVariable(logicalNode, CO);
		componentCount++;
	}

	namedVariable->typeSpec.structure.elementCount = componentCount;

	return namedVariable;
}

static void
createDataSetsFromLogicalNode(MmsDevice* mmsDevice, LogicalNode* logicalNode, MmsDomain* domain)
{
	if (logicalNode->dataSets != NULL) {
		int dataSetCount = ArrayList_listSize((void**) (logicalNode->dataSets));

		int i;

		for (i = 0; i < dataSetCount; i++) {

			char* dataSetName = createString(3, logicalNode->name, "$", logicalNode->dataSets[i]->name);

			MmsNamedVariableList varList = MmsNamedVariableList_create(dataSetName, false);

			int fcdaCount = ArrayList_listSize((void**) (logicalNode->dataSets[i]->fcda));

			int j;

			MmsNamedVariableListEntry variableListEntry;

			for (j = 0; j < fcdaCount; j++) {
				char* variableName = MmsMapping_createMmsVariableNameFromObjectReference(
						logicalNode->dataSets[i]->fcda[j]->objectReference,
						logicalNode->dataSets[i]->fcda[j]->fc);

				char* domainName = MmsMapping_getMmsDomainFromObjectReference(
						logicalNode->dataSets[i]->fcda[j]->objectReference);

				MmsDomain* variableDomain;

				if (domainName != NULL)
					variableDomain = MmsDevice_getDomain(mmsDevice, domainName);
				else
					variableDomain = domain;

				variableListEntry =
						MmsNamedVariableListEntry_create(variableDomain, variableName);

				MmsNamedVariableList_addVariable(varList, variableListEntry);

				free(domainName);
				free(variableName);
			}

			MmsDomain_addNamedVariableList(domain, varList);

			free(dataSetName);
		}
	}
}

static MmsDomain*
createMmsDomainFromIedDevice(LogicalDevice* logicalDevice)
{
	MmsDomain* domain = MmsDomain_create(logicalDevice->name);

	int nodesCount = ArrayList_listSize((void**) (logicalDevice->logicalNodes));

	int i = 0;

	/* Logical nodes are first level named variables */
	domain->namedVariablesCount = nodesCount;
	domain->namedVariables = malloc(nodesCount * sizeof(MmsTypeSpecification*));


	while (logicalDevice->logicalNodes[i] != NULL) {
		domain->namedVariables[i] =
				createNamedVariableFromLogicalNode(logicalDevice->logicalNodes[i]);

		i++;
	}

	return domain;
}

static void
createMmsDataModel(int iedDeviceCount, MmsDevice* mmsDevice, IedModel* iedModel)
{
	int i = 0;
	mmsDevice->domains = malloc((iedDeviceCount) * sizeof(MmsDomain*));
	mmsDevice->domainCount = iedDeviceCount;

	while (iedModel->devices[i] != NULL ) {
		mmsDevice->domains[i] = createMmsDomainFromIedDevice(iedModel->devices[i]);
		i++;
	}
}


static void
createDomainDataSets(MmsDevice* mmsDevice, LogicalDevice* logicalDevice)
{
	MmsDomain* domain = MmsDevice_getDomain(mmsDevice, logicalDevice->name);

	int i = 0;

	while (logicalDevice->logicalNodes[i] != NULL) {

		createDataSetsFromLogicalNode(mmsDevice, logicalDevice->logicalNodes[i], domain);

		i++;
	}
}

static void
createDataSets(int iedDeviceCount, MmsDevice* mmsDevice, IedModel* iedModel)
{
	int i = 0;
	while (iedModel->devices[i] != NULL ) {
		createDomainDataSets(mmsDevice, iedModel->devices[i]);
		i++;
	}
}

static MmsDevice*
createMmsModelFromIedModel(IedModel* iedModel)
{
	MmsDevice* mmsDevice = NULL;

	if (iedModel->devices != NULL) {

		int iedDeviceCount;

		mmsDevice = MmsDevice_create(iedModel->name);

		iedDeviceCount = ArrayList_listSize((void**) (iedModel->devices));

		createMmsDataModel(iedDeviceCount, mmsDevice, iedModel);

		createDataSets(iedDeviceCount, mmsDevice, iedModel);
	}

	return mmsDevice;
}


void
MmsMapping_init(MmsMapping* mapping, IedModel* model)
{
	mapping->model = model;

	mapping->mmsDevice = createMmsModelFromIedModel(model);
}

MmsDevice*
MmsMapping_getMmsDeviceModel(MmsMapping* mapping) {
	return mapping->mmsDevice;
}

void
MmsMapping_destroy(MmsMapping* mapping)
{
	if (mapping->mmsDevice != NULL) {
		MmsDevice_destroy(mapping->mmsDevice);
		mapping->mmsDevice = NULL;
	}
}

char*
MmsMapping_getMmsDomainFromObjectReference(char* objectReference)
{
	int objRefLength = strlen(objectReference);
	char* domainName;
	int j;

	//check if LD name is present
	int i;
	for (i = 0; i < objRefLength; i++) {
		if (objectReference[i] == '/') {
			break;
		}
	}

	if (i == objRefLength)
		return NULL;

	domainName = malloc(i + 1);

	for (j = 0; j < i; j++) {
		domainName[j] = objectReference[j];
	}

	domainName[j] = 0;

	return domainName;
}

char*
MmsMapping_createMmsVariableNameFromObjectReference(char* objectReference, FunctionalConstraint fc)
{
	int destIndex;
	int sourceIndex;
	char* mmsVariableName;
	char* fcString;
	int objRefLength = strlen(objectReference);

	//check if LD name is present
	int i;
	for (i = 0; i < objRefLength; i++) {
		if (objectReference[i] == '/') {
			break;
		}
	}

	if (i == objRefLength)
		i = 0;
	else i++;

	fcString = FunctionalConstrained_toString(fc);

	if (fcString == NULL) return NULL;

	mmsVariableName = malloc(objRefLength - i + 4);

	sourceIndex = i;
	destIndex = 0;

	while (objectReference[sourceIndex] != '.')
		mmsVariableName[destIndex++] = objectReference[sourceIndex++];

	sourceIndex++;

	mmsVariableName[destIndex++] = '$';
	mmsVariableName[destIndex++] = fcString[0];
	mmsVariableName[destIndex++] = fcString[1];
	mmsVariableName[destIndex++] = '$';

	while (sourceIndex < objRefLength) {
		if (objectReference[sourceIndex] != '.')
			mmsVariableName[destIndex++] = objectReference[sourceIndex++];
		else {
			mmsVariableName[destIndex++] = '$';
			sourceIndex++;
		}
	}

	mmsVariableName[destIndex] = 0;

	return mmsVariableName;
}
