/*
 *  MmsValue.c
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
#include "mms_common.h"
#include "mms_value.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "string_utilities.h"
#include "platform_endian.h"
#include "mms_common_internal.h"

#define inline 

static inline int
bitStringByteSize(MmsValue* value)
{
	int bitSize = value->value.bitString.size;
	return (bitSize / 8) + ((bitSize % 8) > 0);
}

static void
updateStructuredComponent(MmsValue* self, MmsValue* update)
{
	int i;
	int componentCount;
	MmsValue** selfValues;
	MmsValue** updateValues;

	if (self->type == MMS_STRUCTURE) {
		componentCount = self->value.structure.componentCount;
		selfValues = self->value.structure.components;
		updateValues = update->value.structure.components;
	}
	else {
		componentCount = self->value.array.size;
		selfValues = self->value.array.components;
		updateValues = update->value.array.components;
	}

	
	for (i = 0; i < componentCount; i++) {
		MmsValue_update(selfValues[i], updateValues[i]);
	}
}

MmsValue*
MmsValue_newIntegerFromBerInteger(Asn1PrimitiveValue* berInteger)
{
	MmsValue* self = calloc(1, sizeof(MmsValue));
	self->type = MMS_INTEGER;

	self->value.integer = berInteger;

	return self;
}

MmsValue*
MmsValue_newUnsignedFromBerInteger(Asn1PrimitiveValue* berInteger)
{
	MmsValue* self = calloc(1, sizeof(MmsValue));
	self->type = MMS_UNSIGNED;

	self->value.unsignedInteger = berInteger;

	return self;
}

bool
MmsValue_update(MmsValue* self, MmsValue* update)
{
	if (self->type == update->type) {
		switch (self->type) {
		case MMS_STRUCTURE:
		case MMS_ARRAY:
			updateStructuredComponent(self, update);
			break;
		case MMS_BOOLEAN:
			self->value.boolean = update->value.boolean;
			break;
		case MMS_FLOAT:
			if (self->value.floatingPoint.formatWidth == update->value.floatingPoint.formatWidth) {
				self->value.floatingPoint.exponentWidth = update->value.floatingPoint.exponentWidth;
				memcpy(self->value.floatingPoint.buf, update->value.floatingPoint.buf,
						self->value.floatingPoint.formatWidth / 8);
			}
			else return false;
			break;
		case MMS_INTEGER:
			if (BerInteger_setFromBerInteger(self->value.integer, update->value.integer))
				return true;
			else
				return false;
			break;
		case MMS_UNSIGNED:
			if (BerInteger_setFromBerInteger(self->value.unsignedInteger,
					update->value.unsignedInteger))
				return true;
			else
				return false;
			break;
		case MMS_UTC_TIME:
			memcpy(self->value.utcTime, update->value.utcTime, 8);
			break;
		case MMS_BIT_STRING:
			if (self->value.bitString.size == update->value.bitString.size)
				memcpy(self->value.bitString.buf, update->value.bitString.buf, bitStringByteSize(self));
			else return false;
			break;
		case MMS_OCTET_STRING:
			if (self->value.octetString.size == update->value.octetString.size) {
				memcpy(self->value.octetString.buf, update->value.octetString.buf,
						self->value.octetString.size);
			}
			else return false;
			break;
		case MMS_VISIBLE_STRING:
			MmsValue_setVisibleString(self, update->value.visibleString);
			break;
		default:
			return false;
			break;
		}
		return true;
	}
	else
		return false;
}

MmsValue*
MmsValue_newBitString(int bitSize)
{
	MmsValue* self = malloc(sizeof(MmsValue));

	self->type = MMS_BIT_STRING;
	self->value.bitString.size = bitSize;
	self->value.bitString.buf = calloc(bitStringByteSize(self), 1);

	return self;
}

void
MmsValue_setBitStringBit(MmsValue* self, int bitPos, bool value)
{
	if (bitPos < self->value.bitString.size) {
		int bytePos = bitPos / 8;
		int bitPosInByte = bitPos % 8;
		int bitMask = (1 << bitPosInByte);

		if (value)
			self->value.bitString.buf[bytePos] |= bitMask;
		else
			self->value.bitString.buf[bytePos] &= (~bitMask);
	}
}

bool
MmsValue_getBitStringBit(MmsValue* self, int bitPos)
{
	if (bitPos < self->value.bitString.size) {
		int bytePos = bitPos / 8;
		int bitPosInByte = bitPos % 8;
		int bitMask = (1 << bitPosInByte);

		if ((self->value.bitString.buf[bytePos] & bitMask) > 0)
			return true;
		else
			return false;

	}
	else return false; /* out of range bits are always zero */
}



MmsValue*
MmsValue_newFloat(float variable)
{
	MmsValue* value = malloc(sizeof(MmsValue));

	value->type = MMS_FLOAT;
	value->value.floatingPoint.formatWidth = 32;
	value->value.floatingPoint.exponentWidth = 8;
	value->value.floatingPoint.buf = malloc(4);

	*((float*) value->value.floatingPoint.buf) = variable;

	return value;
}

void
MmsValue_setFloat(MmsValue* value, float newFloatValue)
{
	if (value->type == MMS_FLOAT) {
		if (value->value.floatingPoint.formatWidth == 32) {
			*((float*) value->value.floatingPoint.buf) = newFloatValue;
		}
		else if (value->value.floatingPoint.formatWidth == 64) {
			*((double*) value->value.floatingPoint.buf) = (double) newFloatValue;
		}
	}
}

void
MmsValue_setDouble(MmsValue* value, double newFloatValue)
{
	if (value->type == MMS_FLOAT) {
		if (value->value.floatingPoint.formatWidth == 32) {
			*((float*) value->value.floatingPoint.buf) = (float) newFloatValue;
		}
		else if (value->value.floatingPoint.formatWidth == 64) {
			*((double*) value->value.floatingPoint.buf) = newFloatValue;
		}
	}
}

MmsValue*
MmsValue_newDouble(double variable)
{
	MmsValue* value = malloc(sizeof(MmsValue));

	value->type = MMS_FLOAT;
	value->value.floatingPoint.formatWidth = 64;
	value->value.floatingPoint.exponentWidth = 11;
	value->value.floatingPoint.buf = malloc(8);

	*((double*) value->value.floatingPoint.buf) = variable;

	return value;
}

MmsValue*
MmsValue_newIntegerFromInt16(int16_t integer)
{
	MmsValue* value = malloc(sizeof(MmsValue));

	value->type = MMS_INTEGER;
	value->value.integer = BerInteger_createFromInt32((int32_t) integer);

	return value;
}

void
MmsValue_setInt32(MmsValue* value, int32_t integer)
{
	if (value->type == MMS_INTEGER) {
		if (Asn1PrimitiveValue_getMaxSize(value->value.integer) >= 4) {
			BerInteger_setInt32(value->value.integer, integer);
		}
		//TODO signal error
	}
}

MmsValue*
MmsValue_setUtcTime(MmsValue* value, uint32_t timeval)
{
	uint8_t* timeArray = (uint8_t*) &timeval;
	uint8_t* valueArray = value->value.utcTime;

	value->type = MMS_UTC_TIME; //apa+++

#ifdef ORDER_LITTLE_ENDIAN
		memcpyReverseByteOrder(valueArray, timeArray, 4);
#else
		memcpy(valueArray, timeArray, 4);
#endif

	return value;
}


MmsValue*
MmsValue_newIntegerFromInt32(int32_t integer)
{
	MmsValue* value = malloc(sizeof(MmsValue));

	value->type = MMS_INTEGER;
	value->value.integer = BerInteger_createFromInt32(integer);

	return value;
}

MmsValue*
MmsValue_newIntegerFromInt64(int64_t integer)
{
	MmsValue* value = malloc(sizeof(MmsValue));

	value->type = MMS_INTEGER;
	value->value.integer = BerInteger_createFromInt64(integer);

	return value;
}

/**
 * Convert signed integer to int32_t
 */
int32_t
MmsValue_toInt32(MmsValue* value)
{
	int32_t integerValue = 0;

	if (value->type == MMS_INTEGER)
		BerInteger_toInt32(value->value.integer, &integerValue);

	return integerValue;
}

//apa+++
/**
 * Convert signed integer to uint32_t
 */
uint32_t
MmsValue_toUInt32(MmsValue* value)
{
	uint32_t integerValue = 0;

	if (value->type == MMS_UNSIGNED)
		BerInteger_toUint32(value->value.integer, &integerValue);

	return integerValue;
}
//apa+++


/**
 * Convert signed integer to int64_t and do sign extension if required
 */
int64_t
MmsValue_toInt64(MmsValue* value)
{
	int64_t integerValue = 0;

	if (value->type == MMS_INTEGER)
			BerInteger_toInt64(value->value.integer, &integerValue);

	return integerValue;
}

float
MmsValue_toFloat(MmsValue* value)
{
	if (value->type == MMS_FLOAT) {
		if (value->value.floatingPoint.formatWidth == 32) {
			float val;

			val = *((float*) (value->value.floatingPoint.buf));
			return val;
		}
		else if (value->value.floatingPoint.formatWidth == 64) {
			float val;
			val = *((double*) (value->value.floatingPoint.buf));
			return val;
		}
	}
	else
		printf("MmsValue_toFloat: conversion error. Wrong type!\n");

	return 0.f;
}

double
MmsValue_toDouble(MmsValue* value)
{
	if (value->type == MMS_FLOAT) {
		double val;
		if (value->value.floatingPoint.formatWidth == 32) {
			val = (double) *((float*) (value->value.floatingPoint.buf));
			return val;
		}
		if (value->value.floatingPoint.formatWidth == 64) {
			val = *((double*) (value->value.floatingPoint.buf));
			return val;
		}
	}

	return 0.f;
}



uint32_t
MmsValue_toUnixTimestamp(MmsValue* value)
{
	uint32_t timestamp;
	uint8_t* timeArray = (uint8_t*) &timestamp;
	timeArray[0] = value->value.utcTime[3];
	timeArray[1] = value->value.utcTime[2];
	timeArray[2] = value->value.utcTime[1];
	timeArray[3] = value->value.utcTime[0];

	return timestamp;
}


// create a deep clone
MmsValue*
MmsValue_clone(MmsValue* value)
{
	int size;
	int i;

	MmsValue* newValue = calloc(1, sizeof(MmsValue));
	newValue->deleteValue = value->deleteValue;
	newValue->type = value->type;

	switch(value->type) {
	//TODO implement cases for other types: MMS_STRING, MMS_BINARY_TIME ...
	case MMS_ARRAY:
		{
			int componentCount = value->value.array.size;
			newValue->value.array.size = componentCount;
			newValue->value.array.components = calloc(componentCount, sizeof(MmsValue*));

			for (i = 0; i < componentCount; i++) {
				newValue->value.array.components[i] =
						MmsValue_clone(value->value.array.components[i]);
			}
		}
		break;
	case MMS_STRUCTURE:
		{
			int i;
			int componentCount = value->value.structure.componentCount;
			newValue->value.structure.componentCount = componentCount;
			newValue->value.structure.components = calloc(componentCount, sizeof(MmsValue*));

			for (i = 0; i < componentCount; i++) {
				newValue->value.structure.components[i] =
						MmsValue_clone(value->value.structure.components[i]);
			}
		}
		break;
	case MMS_INTEGER:
		newValue->value.integer = Asn1PrimitiveValue_clone(value->value.integer);
		break;
	case MMS_UNSIGNED:
		newValue->value.unsignedInteger = Asn1PrimitiveValue_clone(value->value.unsignedInteger);
		break;
	case MMS_FLOAT:
		newValue->value.floatingPoint.formatWidth = value->value.floatingPoint.formatWidth;
		newValue->value.floatingPoint.exponentWidth = value->value.floatingPoint.exponentWidth;
		size = value->value.floatingPoint.formatWidth / 8;
		newValue->value.floatingPoint.buf = malloc(size);
		memcpy(newValue->value.floatingPoint.buf, value->value.floatingPoint.buf, size);
		break;
	case MMS_BIT_STRING:
		newValue->value.bitString.size = value->value.bitString.size;
		size = bitStringByteSize(value);
		newValue->value.bitString.buf = malloc(size);
		memcpy(newValue->value.bitString.buf, value->value.bitString.buf, size);
		break;
	case MMS_BOOLEAN:
		newValue->value.boolean = value->value.boolean;
		break;
	case MMS_OCTET_STRING:
		size = value->value.octetString.size;
		newValue->value.octetString.size = size;
		newValue->value.octetString.buf = malloc(size);
		memcpy(newValue->value.octetString.buf, value->value.octetString.buf, size);
		break;
	case MMS_VISIBLE_STRING:
		size = strlen(value->value.visibleString) + 1;
		newValue->value.visibleString = malloc(size);
		strcpy(newValue->value.visibleString, value->value.visibleString);
		break;
	case MMS_UTC_TIME:
		memcpy(newValue->value.utcTime, value->value.utcTime, 8);
		break;
	}

	return newValue;
}

uint32_t inline
MmsValue_getArraySize(MmsValue* value)
{
	return value->value.array.size;
}

void
MmsValue_delete(MmsValue* value)
{
	//TODO implement case for MmsString */
	if (value->type == MMS_INTEGER)
		Asn1PrimitiveValue_destroy(value->value.integer);
	else if (value->type == MMS_UNSIGNED)
		Asn1PrimitiveValue_destroy(value->value.unsignedInteger);
	else if (value->type == MMS_FLOAT)
		free(value->value.floatingPoint.buf);
	else if (value->type == MMS_BIT_STRING) {
		free(value->value.bitString.buf);
	}
	else if (value->type == MMS_OCTET_STRING) {
		free(value->value.octetString.buf);
	}
	else if (value->type == MMS_VISIBLE_STRING) {
		if (value->value.visibleString != NULL)
			free(value->value.visibleString);
	}
	else if (value->type == MMS_STRUCTURE) {
		int componentCount = value->value.structure.componentCount;
		int i;

		for (i = 0; i < componentCount; i++) {
			MmsValue_delete(value->value.structure.components[i]);
		}

		free(value->value.structure.components);
	}
	else if (value->type == MMS_ARRAY) {
		int size = value->value.array.size;
		int i;
		for (i = 0; i < size; i++) {
			MmsValue_delete(value->value.array.components[i]);
		}
		free(value->value.array.components);
	}

	free(value);
}

MmsValue*
MmsValue_newInteger(int size /*integer size in bits*/)
{
	MmsValue* value = calloc(1, sizeof(MmsValue));
	value->type = MMS_INTEGER;

	if (size <= 32)
		value->value.integer = BerInteger_createInt32();
	else
		value->value.integer = BerInteger_createInt64();

	return value;
}

MmsValue*
MmsValue_newUnsigned(int size /*integer size in bits*/)
{
	MmsValue* value = calloc(1, sizeof(MmsValue));
	value->type = MMS_UNSIGNED;

	if (size <= 32)
		value->value.unsignedInteger = BerInteger_createInt32();
	else
		value->value.unsignedInteger = BerInteger_createInt64();

	return value;
}

MmsValue*
MmsValue_newBoolean(bool boolean)
{
	MmsValue* value = calloc(1, sizeof(MmsValue));
	value->type = MMS_BOOLEAN;
	if (boolean == true)
		value->value.boolean = 1;
	else
		value->value.boolean = 0;

	return value;
}

MmsValue*
MmsValue_newStructure(MmsTypeSpecification* typeSpec)
{
	int componentCount;
	int i;
	MmsValue* value = calloc(1, sizeof(MmsValue));

	value->type = MMS_STRUCTURE;
	componentCount = typeSpec->typeSpec.structure.elementCount;
	value->value.structure.componentCount = componentCount;
	value->value.structure.components = calloc(componentCount, sizeof(MmsValue*));
	
	for (i = 0; i < componentCount; i++) {
		value->value.structure.components[i] =
				MmsValue_newDefaultValue(typeSpec->typeSpec.structure.elements[i]);
	}

	return value;
}

MmsValue*
MmsValue_newDefaultValue(MmsTypeSpecification* typeSpec)
{
	MmsValue* value;

	switch (typeSpec->type) {
	case MMS_INTEGER:
		value = MmsValue_newInteger(typeSpec->typeSpec.integer);
		break;
	case MMS_UNSIGNED:
		value = MmsValue_newUnsigned(typeSpec->typeSpec.unsignedInteger);
		break;
	case MMS_FLOAT:
		value = calloc(1, sizeof(MmsValue));
		value->type = MMS_FLOAT;
		value->value.floatingPoint.exponentWidth = typeSpec->typeSpec.floatingpoint.exponentWidth;
		value->value.floatingPoint.formatWidth = typeSpec->typeSpec.floatingpoint.formatWidth;
		value->value.floatingPoint.buf = calloc(1, typeSpec->typeSpec.floatingpoint.formatWidth / 8);
		break;
	case MMS_BIT_STRING:
		value = calloc(1, sizeof(MmsValue));
		value->type = MMS_BIT_STRING;
		{
			int size;
			int bitSize = abs(typeSpec->typeSpec.bitString);
			value->value.bitString.size = bitSize;
			size = (bitSize / 8) + ((bitSize % 8) > 0);
			value->value.bitString.buf = calloc(1, size);
		}
		break;
	case MMS_OCTET_STRING:
		value = calloc(1, sizeof(MmsValue));
		value->type = MMS_OCTET_STRING;
		value->value.octetString.size = typeSpec->typeSpec.octetString;
		value->value.octetString.buf = calloc(1, typeSpec->typeSpec.octetString);
		break;
	case MMS_VISIBLE_STRING:
		value = MmsValue_newVisibleString(NULL);
		break;
	case MMS_BOOLEAN:
		value = MmsValue_newBoolean(false);
		break;
	case MMS_UTC_TIME:
		value = calloc(1, sizeof(MmsValue));
		value->type = MMS_UTC_TIME;
		break;
	case MMS_ARRAY:
		value = MmsValue_createArray(typeSpec->typeSpec.array.elementTypeSpec,
				typeSpec->typeSpec.array.elementCount);
		break;
	case MMS_STRUCTURE:
		value = MmsValue_newStructure(typeSpec);
		break;
	}

	return value;
}

static inline
setVisibleStringValue(MmsValue* value, char* string)
{
	if (string != NULL)
		value->value.visibleString = copyString(string);
	else
		value->value.visibleString = NULL;
}

MmsValue*
MmsValue_newVisibleString(char* string)
{
	MmsValue* value = calloc(1, sizeof(MmsValue));
	value->type = MMS_VISIBLE_STRING;

	setVisibleStringValue(value, string);

	return value;
}

MmsValue*
MmsValue_newVisibleStringFromByteArray(uint8_t* byteArray, int size)
{
	MmsValue* value = calloc(1, sizeof(MmsValue));
	value->type = MMS_VISIBLE_STRING;

	value->value.visibleString = createStringFromBuffer(byteArray, size);

	return value;
}

void
MmsValue_setVisibleString(MmsValue* value, char* string)
{
	if (value->type == MMS_VISIBLE_STRING) {
		if (value->value.visibleString != NULL)
			free(value->value.visibleString);

		setVisibleStringValue(value, string);
	}
}

char*
MmsValue_toString(MmsValue* value)
{
	if (value->type == MMS_VISIBLE_STRING) {
		return value->value.visibleString;
	}

	return NULL;
}

MmsValue*
MmsValue_newUtcTime(uint32_t timeval)
{
	uint8_t* timeArray;
	uint8_t* valueArray;

	MmsValue* value = calloc(1, sizeof(MmsValue));
	value->type = MMS_UTC_TIME;

	timeArray = (uint8_t*) &timeval;
	valueArray = value->value.utcTime;

#ifdef ORDER_LITTLE_ENDIAN
	valueArray[0] = timeArray[3];
	valueArray[1] = timeArray[2];
	valueArray[2] = timeArray[1];
	valueArray[3] = timeArray[0];
#else
	valueArray[0] = timeArray[0];
	valueArray[1] = timeArray[1];
	valueArray[2] = timeArray[2];
	valueArray[3] = timeArray[3];
#endif

	return value;
}


MmsValue*
MmsValue_createArray(MmsTypeSpecification* elementType, int size)
{
	int i;
	MmsValue* array = calloc(1, sizeof(MmsValue));

	array->type = MMS_ARRAY;
	array->value.array.size = size;
	array->value.array.components = calloc(size, sizeof(MmsValue*));

	for (i = 0; i < size; i++) {
		array->value.array.components[i] = MmsValue_newDefaultValue(elementType);
	}

	return array;
}

MmsValue*
MmsValue_createEmtpyArray(int size)
{
	int i;
	MmsValue* array = calloc(1, sizeof(MmsValue));

	array->type = MMS_ARRAY;
	array->value.array.size = size;
	array->value.array.components = calloc(size, sizeof(MmsValue*));

	for (i = 0; i < size; i++) {
		array->value.array.components[i] = NULL;
	}

	return array;
}

void
MmsValue_setArrayElement(MmsValue* array, int index, MmsValue* elementValue)
{
	if (array->type != MMS_ARRAY)
		return;

	if ((index < 0) || (index >= array->value.array.size))
		return;

	array->value.array.components[index] = elementValue;
}

MmsValue*
MmsValue_getElement(MmsValue* array, int index)
{
	if (array->type != MMS_ARRAY)
		return NULL;

	if ((index < 0) || (index >= array->value.array.size))
		return NULL;

	return array->value.array.components[index];
}

MmsValue*
MmsValue_getStructElementByIndex(MmsValue* structure, int index)
{
	if (structure->type != MMS_STRUCTURE)
		return NULL;

	if ((index < 0) || (index >= structure->value.structure.componentCount))
		return NULL;

	return structure->value.structure.components[index];
}


void inline
MmsValue_setDeletable(MmsValue* value)
{
	value->deleteValue = 1;
}

int
MmsValue_isDeletable(MmsValue* value)
{
	return value->deleteValue;
}

MmsType inline
MmsValue_getType(MmsValue* value)
{
	return value->type;
}
