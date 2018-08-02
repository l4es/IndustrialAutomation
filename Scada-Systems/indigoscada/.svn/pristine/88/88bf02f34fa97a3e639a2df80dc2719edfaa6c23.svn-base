/*
 *  mms_type_spec.h
 *
 *  MmsTypeSpecfication objects are used to describe simple and complex MMS types.
 *  Complex types are arrays or structures of simple and complex types.
 *  They also represent MMS NamedVariables.
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

#ifndef MMS_TYPE_SPEC_H_
#define MMS_TYPE_SPEC_H_

#include "mms_common.h"
#include "mms_types.h"

#include <stdint.h>


/**
 * Delete MmsTypeSpecification object (recursive).
 */
void
MmsTypeSpecification_delete(MmsTypeSpecification* typeSpec);

MmsValue*
MmsTypeSpecification_getChildValue(MmsTypeSpecification* typeSpec, MmsValue* value, char* childId);

#endif /* MMS_TYPE_SPEC_H_ */
