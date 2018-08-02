/*
 *  mms_value_cache.c
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

#include "mms_value_cache.h"
#include "string_utilities.h"
#include <stdlib.h>
#include <string.h>
#include "string_map.h"

struct sMmsValueCache {
	MmsDomain* domain;
	Map map;
};

typedef struct sMmsValueCacheEntry {
	MmsValue* value;
	MmsTypeSpecification* typeSpec;
} MmsValueCacheEntry;

MmsValueCache
MmsValueCache_create(MmsDomain* domain)
{
	MmsValueCache self = calloc(1, sizeof(struct sMmsValueCache));

	self->domain = domain;

	self->map = StringMap_create();

	return self;
}

void
MmsValueCache_insertValue(MmsValueCache self, char* itemId, MmsValue* value)
{
	MmsTypeSpecification* typeSpec = MmsDomain_getNamedVariable(self->domain, itemId);

	if (typeSpec != NULL) {
		MmsValueCacheEntry* cacheEntry = malloc(sizeof(MmsValueCacheEntry));

		cacheEntry->value = value;
		cacheEntry->typeSpec = typeSpec;

		Map_addEntry(self->map, copyString(itemId), cacheEntry);
	}
}

static char*
getParentSubString(char* itemId)
{
	int len = strlen(itemId);

	char* strPos = itemId + len;

	while (--strPos > itemId) {
		if (*strPos == '$') {
			*strPos = 0;
			return itemId;
		}
	}

	return NULL;
}

static char*
getChildSubString (char* itemId, char* parentId)
{
	return itemId + strlen(parentId) + 1;
}

static MmsValue*
searchCacheForValue(MmsValueCache self, char* itemId, char* parentId)
{
	MmsValueCacheEntry* cacheEntry;
	MmsValue* value = NULL;

	cacheEntry = (MmsValueCacheEntry*) Map_getEntry(self->map, parentId);

	if (cacheEntry == NULL) {
		char* parentItemId = getParentSubString(parentId);

		if (parentItemId != NULL) {
			value = searchCacheForValue(self, itemId, parentItemId);
		}
	}
	else {
		char* childId = getChildSubString(itemId, parentId);
		MmsTypeSpecification* typeSpec = MmsDomain_getNamedVariable(self->domain, parentId);
		value = MmsTypeSpecification_getChildValue(typeSpec, cacheEntry->value, childId);
	}

	return value;
}

MmsValue*
MmsValueCache_lookupValue(MmsValueCache self, char* itemId)
{
	// get value for first matching key substring!
	// Then iterate the value for the exact value.

	MmsValue* value = NULL;

	MmsValueCacheEntry* cacheEntry;

	cacheEntry = (MmsValueCacheEntry*) Map_getEntry(self->map, itemId);

	if (cacheEntry == NULL) {
		char* itemIdCopy = copyString(itemId);
		char* parentItemId = getParentSubString(itemIdCopy);

		if (parentItemId != NULL) {
			value = searchCacheForValue(self, itemId, parentItemId);
		}

		free(itemIdCopy);
	}

	if (cacheEntry != NULL)
		return cacheEntry->value;
	else
		return value;
}

static void
cacheEntryDelete(MmsValueCacheEntry* entry)
{
	if (entry != NULL) {
		MmsValue_delete(entry->value);
		free(entry);
	}
}

void
MmsValueCache_destroy(MmsValueCache self)
{
	Map_deleteDeep(self->map, true, cacheEntryDelete);
	free(self);
}
