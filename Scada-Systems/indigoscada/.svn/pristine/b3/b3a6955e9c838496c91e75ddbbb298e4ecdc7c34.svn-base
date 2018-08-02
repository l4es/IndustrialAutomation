/*
 *  ied_server.c
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

#include "iec61850_simple_server_api.h"
#include "mms_mapping.h"

struct sSimpleIedServer {
	IedModel* model;
	MmsDevice* mmsDevice;
	MmsServer mmsServer;
	IsoServer isoServer;
	MmsMapping* mmsMapping;
};


IedServer
IedServer_create(IedModel* iedModel)
{
	IedServer self = calloc(1, sizeof(struct sSimpleIedServer));

	self->model = iedModel;

	self->mmsMapping = calloc(1, sizeof(MmsMapping));
	MmsMapping_init(self->mmsMapping, iedModel);

	self->mmsDevice = MmsMapping_getMmsDeviceModel(self->mmsMapping);

	self->isoServer = IsoServer_create();

	self->mmsServer = MmsServer_create(self->isoServer, self->mmsDevice);

	return self;
}

void /* Destructor */
IedServer_destroy(IedServer self)
{
	MmsServer_destroy(self->mmsServer);
	IsoServer_destroy(self->isoServer);
	MmsMapping_destroy(self->mmsMapping);
	free(self->mmsMapping);
	free(self);
}

IsoServer
IedServer_getIsoServer(IedServer self)
{
	return self->isoServer;
}

void
IedServer_start(IedServer self)
{
	MmsServer_startListening(self->mmsServer);
}

bool
IedServer_isRunning(IedServer self)
{
	if (IsoServer_getState(self->isoServer) == ISO_SVR_STATE_RUNNING)
		return true;
	else
		return false;
}

void
IedServer_stop(IedServer self)
{
	MmsServer_stopListening(self->mmsServer);
}

void
IedServer_lockDataModel(IedServer self)
{
	MmsServer_lockModel(self->mmsServer);
}

void
IedServer_unlockDataModel(IedServer self)
{
	MmsServer_unlockModel(self->mmsServer);
}

MmsDomain*
IedServer_getDomain(IedServer self, char* logicalDeviceName)
{
	return MmsDevice_getDomain(self->mmsDevice, logicalDeviceName);
}

MmsValue*
IedServer_getValue(IedServer self, MmsDomain* domain, char* mmsItemId)
{
	return MmsServer_getValueFromCache(self->mmsServer, domain, mmsItemId);
}

void
IedServer_setDefaultValue(IedServer self, MmsDomain* domain, char* mmsItemId)
{
	//TODO implement me
}

void
IedServer_setAllModelDefaultValues(IedServer self)
{

	int domain = 0;

	for (domain = 0; domain < self->mmsDevice->domainCount; domain++) {

		/* Install all top level MMS named variables (=Logical nodes) in the MMS server cache */
		MmsDomain* logicalDevice = self->mmsDevice->domains[domain];

		int i;

		for (i = 0; i < logicalDevice->namedVariablesCount; i++) {
			char* itemId = logicalDevice->namedVariables[i]->name;
			MmsValue* defaultValue = MmsValue_newDefaultValue(logicalDevice->namedVariables[i]);
			MmsServer_insertIntoCache(self->mmsServer, logicalDevice, itemId, defaultValue);
		}

	}


}

void
IedServer_installReadHandler(IedServer self, ReadVariableHandler readHandler, void* object)
{
	MmsServer_installReadHandler(self->mmsServer, readHandler, object);
}

void
IedServer_installWriteHandler(IedServer self, WriteVariableHandler writeHandler, void* object)
{
	MmsServer_installWriteHandler(self->mmsServer, writeHandler, object);
}
