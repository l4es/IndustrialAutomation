/*
 *  mms_value.h
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

#ifndef MMS_VALUE_H_
#define MMS_VALUE_H_

/**
 * \defgroup common_api_group IEC 61850 API common parts
 */
/**@{*/

#include "mms_common.h"
#include "mms_types.h"
#include <stdbool.h>

/*************************************************************************************
 *  Array functions
 *************************************************************************************/

/**
 * Create an Array and initialize elements with default values.
 *
 * \param elementType type description for the elements the new array
 * \param size the size of the new array
 *
 * \return a newly created array instance
 */
MmsValue*
MmsValue_createArray(MmsTypeSpecification* elementType, int size);

/**
 * Get the size of an array.
 *
 * \param self MmsValue instance to operate on. Has to be of type MMS_ARRAY.
 *
 * \return the size of the array
 */
uint32_t
MmsValue_getArraySize(MmsValue* self);

/**
 * Get an element of an array.
 *
 * \param self MmsValue instance to operate on. Has to be of type MMS_ARRAY.
 * \param index array index of the requested array element
 *
 * \return the array element object
 */
MmsValue*
MmsValue_getElement(MmsValue* array, int index);

/**
 * Create an emtpy array.
 *
 * \param size the size of the new array
 *
 * \return a newly created empty array instance
 */
MmsValue*
MmsValue_createEmtpyArray(int size);

void
MmsValue_setArrayElement(MmsValue* array, int index, MmsValue* elementValue);

MmsValue*
MmsValue_getStructElementByIndex(MmsValue* structure, int index);

/*************************************************************************************
 * Basic type functions
 *************************************************************************************/

/**
 * Get the int64_t value of a MmsValue object.
 *
 * \param self MmsValue instance to operate on. Has to be of a type MMS_INTEGER, MMS_UNSIGNED
 * or MMS_FLOAT.
 *
 * \return signed 64 bit integer
 */
int64_t
MmsValue_toInt64(MmsValue* self);

/**
 * Get the double value of a MmsValue object.
 *
 * \param self MmsValue instance to operate on. Has to be of a type MMS_INTEGER, MMS_UNSIGNED
 * or MMS_FLOAT.
 *
 * \return 64 bit floating point value
 */
double
MmsValue_toDouble(MmsValue* self);

/**
 * Get the float value of a MmsValue object.
 *
 * \param self MmsValue instance to operate on. Has to be of a type MMS_INTEGER, MMS_UNSIGNED
 * or MMS_FLOAT.
 *
 * \return 32 bit floating point value
 */
float
MmsValue_toFloat(MmsValue* self);

/**
 * Get the unix timestamp of a MmsValue object of type MMS_UTCTIME.
 *
 * \param self MmsValue instance to operate on. Has to be of a type MMS_UTC_TIME.
 *
 * \return unix timestamp of the MMS_UTCTIME variable.
 */
uint32_t
MmsValue_toUnixTimestamp(MmsValue* self);

/**
 * Set the float value of a MmsValue object.
 *
 * \param self MmsValue instance to operate on. Has to be of a type MMS_FLOAT.
 */
void
MmsValue_setFloat(MmsValue* self, float newFloatValue);

/**
 * Set the double value of a MmsValue object.
 *
 * \param self MmsValue instance to operate on. Has to be of a type MMS_FLOAT.
 */
void
MmsValue_setDouble(MmsValue* self, double newFloatValue);

/**
 * Set the Int32 value of a MmsValue object.
 *
 * \param self MmsValue instance to operate on. Has to be of a type MMS_INTEGER.
 */
void
MmsValue_setInt32(MmsValue* self, int32_t integer);

char*
MmsValue_toString(MmsValue* self);

void
MmsValue_setVisibleString(MmsValue* self, char* string);

void
MmsValue_setBitStringBit(MmsValue* self, int bitPos, bool value);

bool
MmsValue_getBitStringBit(MmsValue* self, int bitPos);

MmsValue*
MmsValue_setUtcTime(MmsValue* self, uint32_t timeval);

bool
MmsValue_update(MmsValue* self, MmsValue*);

/*************************************************************************************
 * Constructors and destructors
 *************************************************************************************/

MmsValue*
MmsValue_newIntegerFromBerInteger(Asn1PrimitiveValue* berInteger);

MmsValue*
MmsValue_newUnsignedFromBerInteger(Asn1PrimitiveValue* berInteger);

MmsValue*
MmsValue_newInteger(int size);

MmsValue*
MmsValue_newUnsigned(int size);

MmsValue*
MmsValue_newBoolean(bool boolean);

MmsValue*
MmsValue_newBitString(int bitSize);

MmsValue*
MmsValue_newStructure(MmsTypeSpecification* typeSpec);

MmsValue*
MmsValue_newDefaultValue(MmsTypeSpecification* typeSpec);

MmsValue*
MmsValue_newIntegerFromInt16(int16_t integer);

MmsValue*
MmsValue_newIntegerFromInt32(int32_t integer);

MmsValue*
MmsValue_newIntegerFromInt64(int64_t integer);

MmsValue*
MmsValue_newFloat(float variable);

MmsValue*
MmsValue_newDouble(double variable);

MmsValue*
MmsValue_clone(MmsValue* value);

void
MmsValue_delete(MmsValue* value);

MmsValue*
MmsValue_newVisibleString(char* string);

MmsValue*
MmsValue_newVisibleStringFromByteArray(uint8_t* byteArray, int size);

MmsValue*
MmsValue_newUtcTime(uint32_t timeval);

void
MmsValue_setDeletable(MmsValue* value);

int
MmsValue_isDeletable(MmsValue* value);

MmsType
MmsValue_getType(MmsValue* value);

/**@}*/

#endif /* MMS_VALUE_H_ */
