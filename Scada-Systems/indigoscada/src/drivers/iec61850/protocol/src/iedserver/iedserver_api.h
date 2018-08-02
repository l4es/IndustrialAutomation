/*
 *  iedserver_api.h
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

#ifndef IEDSERVER_API_H_
#define IEDSERVER_API_H_

#include "model.h"
#include "ied_value.h"

typedef struct sSCSMapping* SCSMapping;

typedef struct sIedServer* IedServer;

typedef struct sIedServerCallbacks {
	void (*writeVariable) (char* objectReference, IedValue value);
	void (*readVariable) (char* objectReference, IedValue value);
} IedServerCallbacks;

IedServer
IedServer_create(IedModel* iedModel, SCSMapping scsm);

void
IedServer_startListening(IedServer self);

/**
 * Get the values for the given object.
 * If the values not exist. They will be created an populated with default values
 * If default values are set no callbacks will be called if a client reads the values!
 */
void
IedServer_getValue(IedServer self, char* objectReference, IedValue* value);

IsoServer
IedServer_getIsoServer(IedServer self);
/**
 * Set default values for the given object.
 * If default values are set no callbacks will be called if a client reads the values!
 */
void
IedServer_setDefaultValues2(IedServer self, char* objectReference);

void
IedServer_setDefaultValues(IedServer self, LogicalNode* node);

void
IedServer_updateValue(IedServer self, IedValue value);



ReportControlBlock IedServer_createReportControlBlock(IedServer self);
ReportControlBlock IedServer_getReportControlBlock(IedServer self, char* objectReference);

void IedServer_updateReportControlBlock(IedServer self, ReportControlBlock rcb);

IedValue IedServer_getValueReference(IedServer self, char* objectReference);

void
IedServer_stopListening(IedServer self);

void
IedServer_destroy(IedServer self);

#endif /* IEDSERVER_API_H_ */
