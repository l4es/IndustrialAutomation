// ==========================================================
// Tag manipulation functions
//
// Design and implementation by
// - Hervé Drolon <drolon@infonie.fr>
//
// This file is part of FreeImage 3
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ==========================================================

#pragma warning (disable : 4786) // identifier was truncated to 'number' characters

#include "FreeImage.h"
#include "Utilities.h"
#include "FreeImageTag.h"

// --------------------------------------------------------------------------
// FITAG helper functions
// --------------------------------------------------------------------------

FITAG* 
FreeImage_CloneTag(const FITAG *tag) {
	if(!tag)
		return NULL;

	// allocate a new tag
	FITAG *clone = (FITAG*)malloc(sizeof(FITAG));
	if(!clone)
		return NULL;
	memset(clone, 0, sizeof(FITAG));

	// copy the tag

	// tag ID
	clone->id = tag->id;
	// tag key
	clone->key = (char*)malloc((strlen(tag->key) + 1) * sizeof(char));
	strcpy(clone->key, tag->key);
	// tag description
	if(tag->description) {
		clone->description = (char*)malloc((strlen(tag->description) + 1) * sizeof(char));
		strcpy(clone->description, tag->description);
	} else {
		clone->description = NULL;
	}
	// tag data type
	clone->type = tag->type;
	// tag count
	clone->count = tag->count;
	// tag length
	clone->length = tag->length;
	// tag value
	switch(clone->type) {
		case FIDT_ASCII:
			clone->value = (char*)malloc((strlen((char*)tag->value) + 1) * sizeof(char));
			strcpy((char*)clone->value, (char*)tag->value);
			break;
		default:
			clone->value = (BYTE*)malloc(tag->length * sizeof(BYTE));
			memcpy(clone->value, tag->value, tag->length);
			break;
	}

	return clone;
}

void 
FreeImage_DeleteTag(FITAG *tag) {
	if(tag) {
		tag->id = 0;
		free(tag->key); 
		tag->key = NULL;
		free(tag->description); 
		tag->description = NULL;
		tag->type = FIDT_NOTYPE;
		tag->count = 0;
		free(tag->value);
		tag->value = NULL;
		tag->length = 0;
	}
}

int 
FreeImage_TagDataWidth(WORD type) {
	static int format_bytes[] = { 0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8 };

	return (type < 13) ? format_bytes[type] : 0;
}



