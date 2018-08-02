/*
 *  mms_mapping.h
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

#ifndef MMS_MAPPING_H_
#define MMS_MAPPING_H_

#include "model.h"
#include "mms_server_connection.h"
#include "mms_device_model.h"

typedef struct sMmsMapping {
	IedModel* model;
	MmsDevice* mmsDevice;
} MmsMapping;

void
MmsMapping_init(MmsMapping* mapping, IedModel* model);

MmsDevice*
MmsMapping_getMmsDeviceModel(MmsMapping* mapping);

void
MmsMapping_destroy(MmsMapping* mapping);

char*
MmsMapping_getMmsDomainFromObjectReference(char* objectReference);

char*
MmsMapping_createMmsVariableNameFromObjectReference(char* objectReference, FunctionalConstraint fc);

#endif /* MMS_MAPPING_H_ */
