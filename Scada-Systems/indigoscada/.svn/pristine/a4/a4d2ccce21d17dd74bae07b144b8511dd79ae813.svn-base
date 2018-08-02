/*
 *  mms_type_spec.c
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

#include <stdlib.h>
#include <string.h>
#include "mms_common.h"
#include "mms_type_spec.h"

void
MmsTypeSpecification_delete(MmsTypeSpecification* typeSpec)
{
	if (typeSpec->name != NULL)
		free(typeSpec->name);

	if (typeSpec->type == MMS_STRUCTURE) {
		int elementCount = typeSpec->typeSpec.structure.elementCount;
		int i;
		for (i = 0; i < elementCount; i++) {
			MmsTypeSpecification_delete(typeSpec->typeSpec.structure.elements[i]);
		}

		free(typeSpec->typeSpec.structure.elements);
	}
	else if (typeSpec->type == MMS_ARRAY) {
		MmsTypeSpecification_delete(typeSpec->typeSpec.array.elementTypeSpec);
	}

	free(typeSpec);
}

static int
directChildStrLen(char* childId)
{
	int i = 0;
	int childIdLen = strlen(childId);
	while (i < childIdLen) {
		if (*(childId + i) == '$')
			break;
		i++;
	}

	return i;
}

MmsValue*
MmsTypeSpecification_getChildValue(MmsTypeSpecification* typeSpec, MmsValue* value, char* childId)
{
	if (typeSpec->type == MMS_STRUCTURE) {
		int childLen = directChildStrLen(childId);
		int i;
		for (i = 0; i < typeSpec->typeSpec.structure.elementCount; i++) {
			if (strncmp(typeSpec->typeSpec.structure.elements[i]->name, childId, childLen) == 0) {
				if (childLen == strlen(childId)) {
					return value->value.structure.components[i];
				}
				else {
					return MmsTypeSpecification_getChildValue(typeSpec->typeSpec.structure.elements[i],
							value->value.structure.components[i], childId + childLen + 1);
				}
			}
		}
		return NULL;
	}
	else
		return NULL;
}
