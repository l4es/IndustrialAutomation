/*
 *  mms_common.h
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

#ifndef MMS_COMMON_H_
#define MMS_COMMON_H_

#define DEFAULT_MAX_SERV_OUTSTANDING_CALLING 5
#define DEFAULT_MAX_SERV_OUTSTANDING_CALLED 5
#define DEFAULT_DATA_STRUCTURE_NESTING_LEVEL 10
#define DEFAULT_MAX_PDU_SIZE 65000

typedef enum {
	MMS_ERROR, MMS_INITIATE, MMS_CONFIRMED_REQUEST, MMS_OK, MMS_CONCLUDE
} MmsIndication;

/*
	TypeSpecification_PR_array,
	TypeSpecification_PR_structure,
	TypeSpecification_PR_boolean,
	TypeSpecification_PR_bitstring,
	TypeSpecification_PR_integer,
	TypeSpecification_PR_unsigned,
	TypeSpecification_PR_floatingpoint,
	TypeSpecification_PR_octetstring,
	TypeSpecification_PR_visiblestring,
	TypeSpecification_PR_generalizedtime,
	TypeSpecification_PR_binarytime,
	TypeSpecification_PR_bcd,
	TypeSpecification_PR_objId,
	TypeSpecification_PR_mMSString,
	TypeSpecification_PR_utctime
*/
typedef enum {
	MMS_ARRAY,
	MMS_STRUCTURE,
	MMS_BOOLEAN,
	MMS_BIT_STRING,
	MMS_INTEGER,
	MMS_UNSIGNED,
	MMS_FLOAT,
	MMS_OCTET_STRING,
	MMS_VISIBLE_STRING,
	MMS_GENERALIZED_TIME,
	MMS_BINARY_TIME,
	MMS_BCD,
	MMS_OBJ_ID,
	MMS_STRING,
	MMS_UTC_TIME
} MmsType;

typedef struct sMmsDomain MmsDomain;
typedef struct sMmsNamedVariableList* MmsNamedVariableList;
typedef struct sMmsNamedVariableListEntry* MmsNamedVariableListEntry;

#endif /* MMS_COMMON_H_ */
