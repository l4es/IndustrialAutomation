/*
 *  model.h
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

#ifndef MODEL_H_
#define MODEL_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum eFunctionalConstraint {
	// FCs according to IEC 61850-7-2:
	ST, /** Status information */
	MX, /** Measurands - analogue values */
	SP, /** Setpoint */
	SV, /** Substitution */
	CF, /** Configuration */
	DC, /** Description */
	SG, /** Setting group */
	SE, /** Setting group editable */
	SR, /** Service response / Service tracking */
	OR, /** Operate received */
	BL, /** Blocking */
	EX, /** Extended definition */
	CO  /** Control */
} FunctionalConstraint;

typedef char* ObjectReference;

typedef union uDataAttributeValue {
	float float32;
} DataAttributeValue;

typedef enum eDataAttributeType {
	BOOLEAN,/* int */
	INT8,   /* int8_t */
	INT16,  /* int16_t */
	INT32,  /* int32_t */
	INT64,  /* int64_t */
	INT8U,  /* uint8_t */
	INT16U, /* uint16_t */
	INT24U, /* uint32_t */
	INT32U, /* uint32_t */
	FLOAT32, /* float */
	FLOAT64, /* double */
	ENUMERATED,
	OCTET_STRING_64,
	OCTET_STRING_6,
	OCTET_STRING_8,
	VISIBLE_STRING_32,
	VISIBLE_STRING_64,
	VISIBLE_STRING_65,
	VISIBLE_STRING_129,
	VISIBLE_STRING_255,
	UNICODE_STRING_255,
	TIMESTAMP,
	QUALITY,
	CHECK,
	CODEDENUM,
	GENERIC_BITSTRING,
	CONSTRUCTED
} IedDataType;

typedef struct sDataAttribute {
	ObjectReference name;
	FunctionalConstraint fc;
	IedDataType type;
	int arraySize; /* is set to zero if attribute is not an array */

	/* A Basic data attribute is modeled by setting subDataAttributes to NULL */
	struct sDataAttribute** subDataAttributes;

	DataAttributeValue* value; /* can probably be used for the default value */
} DataAttribute;

typedef struct sDataObject {
	ObjectReference name;
	struct sDataObject** subDataObjects;
	DataAttribute** dataAttributes;
} DataObject;


typedef struct {
	char* objectReference;
	FunctionalConstraint fc;
} FunctionalConstrainedData;

typedef struct {
	char* name;
	FunctionalConstrainedData** fcda;
} DataSet;

typedef struct {
	char* name;
	char* rptId;
	bool enabled;
	DataSet* dataSet;    /* Reference to the data set to monitor */
	uint32_t confRef;    /* ConfRef - configuration revision */
	uint8_t trgOps;      /* TrgOps - trigger conditions */
	uint8_t options;     /* OptFlds */
	uint32_t bufferTime; /* BufTm - time to buffer events until a report is generated */
	uint16_t seqNum;     /* SqNum - sequence number */
	uint32_t intPeriod;  /* IntPrd - integrity period */

	/* EntryId */
	/* TimeOfEntry */
	char* owner;

} ReportControlBlock;

typedef struct {
	ObjectReference	name;
	DataObject** dataObjects;
	DataSet** dataSets;
} LogicalNode;


typedef struct {
	ObjectReference name;
	LogicalNode** logicalNodes;
} LogicalDevice;

typedef struct {
	ObjectReference name;    /* Name of IED */
	LogicalDevice** devices; /* Array/list of logical devices */
} IedModel;

char*
FunctionalConstrained_toString(FunctionalConstraint fc);

LogicalDevice*
IedModel_getDevice(IedModel* model, char* deviceName);

LogicalNode*
LogicalDevice_getLogicalNode(LogicalDevice* device, char* nodeName);

int
LogicalNode_hasFCData(LogicalNode* node, FunctionalConstraint fc);

int
DataObject_hasFCData(DataObject* dataObject, FunctionalConstraint fc);

#endif /* MODEL_H_ */
