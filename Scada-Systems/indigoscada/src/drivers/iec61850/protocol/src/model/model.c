/*
 *  model.c
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
#include "model.h"
#include <stdlib.h>

char*
FunctionalConstrained_toString(FunctionalConstraint fc) {
	switch (fc) {
	case ST:
		return "ST";
	case MX:
		return "MX";
	case SP:
		return "SP";
	case SV:
		return "SV";
	case CF:
		return "CF";
	case DC:
		return "DC";
	case SG:
		return "SG";
	case SE:
		return "SE";
	case SR:
		return "SR";
	case OR:
		return "OR";
	case BL:
		return "BL";
	case EX:
		return "EX";
	case CO:
		return "CO";
	default:
		return NULL;
	}
}


LogicalDevice*
IedModel_getDevice(IedModel* model, char* deviceName)
{
	LogicalDevice** devices = model->devices;
	int i = 0;

	while (devices[i] != NULL) {
		if (strcmp(devices[i]->name, deviceName) == 0)
			return devices[i];
		i++;
	}

	return NULL;
}

int
DataObject_hasFCData(DataObject* dataObject, FunctionalConstraint fc)
{
	DataObject** subDataObjects;
	DataAttribute** attributes = dataObject->dataAttributes;

	if (attributes != NULL) {
		int i = 0;

		while (attributes[i] != NULL) {
			if (attributes[i]->fc == fc)
				return 1;
			i++;
		}
	}

	subDataObjects = dataObject->subDataObjects;

	if (subDataObjects != NULL) {
		int i = 0;

		while (subDataObjects[i] != NULL) {
			if (DataObject_hasFCData(subDataObjects[i], fc))
				return 1;
			i++;
		}

	}

	return 0;
}

int
LogicalNode_hasFCData(LogicalNode* node, FunctionalConstraint fc)
{
	DataObject** objects = node->dataObjects;

	int i = 0;

	while (objects[i] != NULL) {
		if (DataObject_hasFCData(objects[i], fc))
			return 1;
		i++;
	}

	return 0;
}

