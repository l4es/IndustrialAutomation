/*
 *  string_utilities.c
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

#include "string_utilities.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

char* copyString(char* string) {
	int newStringLength = strlen(string) + 1;

	char* newString = malloc(newStringLength);

	memcpy(newString, string, newStringLength);

	return newString;
}

char* createString(int count, ...) {
	va_list ap;
	char* newStr;
	char* currentPos;
	int newStringLength = 0;
	int i;

	/* Calculate new string length */
	va_start(ap, count);
	for (i = 0; i < count; i++) {
		char* str = va_arg(ap, char*);

		newStringLength += strlen(str);
	}
	va_end(ap);

	newStr = malloc(newStringLength + 1);
	currentPos = newStr;


	va_start(ap, count);
	for (i = 0; i < count; i++) {
		char* str = va_arg(ap, char*);
		strcpy(currentPos, str);
		currentPos += strlen(str);
	}
	va_end(ap);

	*currentPos = 0;

	return newStr;
}

char* createStringFromBuffer(uint8_t* buf, int size)
{
	char* newStr = malloc(size + 1);

	memcpy(newStr, buf, size);
	newStr[size] = 0;

	return newStr;
}

