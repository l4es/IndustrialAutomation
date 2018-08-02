/*
 *  linked_list.c
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
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"

static LinkedList
getLastElement(LinkedList list)
{
	while (list->next != NULL) {
		list = list->next;
	}
	return list;
}

LinkedList
LinkedList_create()
{
	LinkedList newList;

	newList = malloc(sizeof(struct sLinkedList));
	newList->data = NULL;
	newList->next = NULL;

	return newList;
}

/**
 * Destroy list (free). Also frees element data with helper function.
 */

void
LinkedList_destroyDeep(LinkedList list, void (*valueDeleteFunction) (void*))
{
	LinkedList nextElement = list;
	LinkedList currentElement;

	do {
		currentElement = nextElement;
		nextElement = currentElement->next;
		if (currentElement->data != NULL)
			valueDeleteFunction(currentElement->data);
		free(currentElement);
	} while (nextElement != NULL);
}

void
LinkedList_destroy(LinkedList list)
{
	LinkedList_destroyDeep(list, free);
}

/**
 * Destroy list (free) without freeing the element data
 */
void
LinkedList_destroyStatic(LinkedList list)
{
	LinkedList nextElement = list;
	LinkedList currentElement;

	do {
		currentElement = nextElement;
		nextElement = currentElement->next;
		free(currentElement);
	} while (nextElement != NULL);
}

int
LinkedList_size(LinkedList list)
{
	LinkedList nextElement = list;
	int size = 0;

	while (nextElement->next != NULL) {
		nextElement = nextElement->next;
		size++;
	}

	return size;
}

void
LinkedList_add(LinkedList list, void* data)
{
	LinkedList listEnd;
	LinkedList newElement = LinkedList_create();

	newElement->data = data;

	listEnd = getLastElement(list);

	listEnd->next = newElement;
}

#define inline

LinkedList inline
LinkedList_getNext(LinkedList list)
{
	return list->next;
}

void
LinkedList_printStringList(LinkedList list)
{
	LinkedList element = list;

	int elementCount = 0;
	char* str;

	while ((element = LinkedList_getNext(element)) != NULL) {
		str = (char*) (element->data);
		printf("%s\n", str);
		elementCount++;
	}
}



