/*
 *  iec61850_simple_server_api.h
 *
 *  Simple server API for libiec61850.
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
 *
 *	Description:
 *
 *  The user of this API has to handle MMS values directly. It provides no additional
 *  abstraction layer on top of the MMS layer. Therefore it is not intended to implement
 *  a generic IEC 61850 server. But an IEC 61850 compliant MMS server can be implemented
 *  easily with this API.
 *  This API is intended to be used on embedded devices that should not be burdened with an
 *  additional data modeling and data handling layer.
 *
 *  This API provides two different usage patterns that can also be combined in a single
 *  application:
 *
 *  Option 1: The user can provide callbacks for MMS server read and write operations.
 *  Every time a client requests a value a callback will be invoked and the appropriate
 *  value has to be provided by the application.
 *
 *  Option 2: The MMS server can handle all read and write operations autonomously without
 *  invoking any application callbacks. The application can provide updated process values
 *  by pushing them periodically or on demand to the MMS server.
 *
 */

#ifndef IEC61850_SIMPLE_SERVER_API_H_
#define IEC61850_SIMPLE_SERVER_API_H_

/** \addtogroup server_api_group
 *  @{
 */

#include "mms_server.h"
#include "model.h"

typedef struct sSimpleIedServer* IedServer;

/**
 * Create a new IedServer instance
 *
 * \param iedModel reference to the IedModel data structure to be used as IEC 61850 model of the device
 *
 * \return the newly generated IedServer instance
 */
IedServer
IedServer_create(IedModel* iedModel);

/**
 * Destroy an IedServer instance and release all resources (memory, TCP sockets)
 *
 * \param self the instance of IedServer to operate on.
 */
void
IedServer_destroy(IedServer self);

/**
 * Start handling client connections
 *
 * \param self the instance of IedServer to operate on.
 */
void
IedServer_start(IedServer self);

/**
 * Stop handling client connections
 *
 * \param self the instance of IedServer to operate on.
 */
void
IedServer_stop(IedServer self);

/**
 * Check if IedServer instance is listening for client connections
 *
 * \param self the instance of IedServer to operate on.
 *
 * \return true if IedServer instance is listening for client connections
 */
bool
IedServer_isRunning(IedServer self);

/**
 * Get the MmsDomain object of an IEC 61850 Logical Device.
 *
 * \param self the instance of IedServer to operate on.
 */
MmsDomain*
IedServer_getDomain(IedServer self, char* logicalDeviceName);

/**
 * Get the MmsValue object of an MMS Named Variable that is part of the device model
 *
 * \param self the instance of IedServer to operate on.
 * \param domain the MmsDomain object containing the MMS Named Variable
 * \param mmsItemId the name of the MMS Named Variable
 *
 * \return MmsValue object of the MMS Named Variable or NULL if the value does not exist.
 */
MmsValue*
IedServer_getValue(IedServer self, MmsDomain* domain, char* mmsItemId);

void
IedServer_setDefaultValue(IedServer self, MmsDomain* domain, char* mmsItemId);

/**
 * Create default values for all Mms model nodes and install them in the MMS server cache
 *
 * \param self the instance of IedServer to operate on.
 */
void
IedServer_setAllModelDefaultValues(IedServer self);

void
IedServer_installReadHandler(IedServer self, ReadVariableHandler readHandler, void* object);

void
IedServer_installWriteHandler(IedServer self, WriteVariableHandler writeHandler, void* object);

/**
 * Lock the MMS server data model. Client requests will be postponed until the lock is removed
 *
 * \param self the instance of IedServer to operate on.
 */
void
IedServer_lockDataModel(IedServer self);

/**
 * Unlock the MMS server data model and process pending client requests.
 *
 * \param self the instance of IedServer to operate on.
 */
void
IedServer_unlockDataModel(IedServer self);

#ifdef STACK_CONFIG_DATASETS

/**
 * Create a data set (MMS Named Variable List)
 *
 * \param self the instance of IedServer to operate on.
 * \param dsName name of the data set
 * \param dsMembers list of members of the data set
 */
void
IedServer_createDataSet(IedServer self, char* dsName, LinkedList dsMembers);

/**
 * Delete a data set (MMS Named Variable List)
 *
 * \param self the instance of IedServer to operate on.
 * \param dsName name of the data set
 */
void
IedServer_deleteDataSet(IedServer self, char* dsName);
#endif

/**@}*/

#endif /* IEC61850_SIMPLE_SERVER_API_H_ */
