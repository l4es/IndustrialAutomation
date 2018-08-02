/*
 *  mms_get_var_access_service.c
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
#include "mms_server_internal.h"

/**********************************************************************************************
 * MMS GetVariableAccessAttributes Service
 *********************************************************************************************/

static int
createTypeSpecification (
		MmsTypeSpecification* namedVariable,
		TypeSpecification_t* typeSpec)
{

	if (namedVariable->type == MMS_ARRAY) {
		typeSpec->present = TypeSpecification_PR_array;
		typeSpec->choice.array.numberOfElements;

		asn_long2INTEGER(&(typeSpec->choice.array.numberOfElements),
				(long) namedVariable->typeSpec.array.elementCount);

		typeSpec->choice.array.packed = NULL;
		typeSpec->choice.array.elementType = calloc(1, sizeof(TypeSpecification_t));

		createTypeSpecification(namedVariable->typeSpec.array.elementTypeSpec,
				typeSpec->choice.array.elementType);
	}
	else if (namedVariable->type == MMS_STRUCTURE) {
		int i;
		int componentCount;
		typeSpec->present = TypeSpecification_PR_structure;

		componentCount = namedVariable->typeSpec.structure.elementCount;

		typeSpec->choice.structure.components.list.count = componentCount;
		typeSpec->choice.structure.components.list.size = componentCount;

		typeSpec->choice.structure.components.list.array
			= calloc(componentCount, sizeof(StructComponent_t*));

		for (i = 0; i < componentCount; i++) {

			typeSpec->choice.structure.components.list.array[i] =
					calloc(1, sizeof(StructComponent_t));

			typeSpec->choice.structure.components.list.array[i]->componentName =
					calloc(1, sizeof(Identifier_t));

			typeSpec->choice.structure.components.list.array[i]->componentName->buf =
					namedVariable->typeSpec.structure.elements[i]->name;

			typeSpec->choice.structure.components.list.array[i]->componentName->size =
					strlen(namedVariable->typeSpec.structure.elements[i]->name);

			typeSpec->choice.structure.components.list.array[i]->componentType =
					calloc(1, sizeof(TypeSpecification_t));

			createTypeSpecification(namedVariable->typeSpec.structure.elements[i],
					typeSpec->choice.structure.components.list.array[i]->componentType);
		}
	}
	else {

//		typedef enum TypeSpecification_PR {
//			TypeSpecification_PR_NOTHING,	/* No components present */
//			TypeSpecification_PR_typeName,
//			TypeSpecification_PR_array,
//			TypeSpecification_PR_structure,
//			TypeSpecification_PR_boolean,
//			TypeSpecification_PR_bitstring,
//			TypeSpecification_PR_integer,
//			TypeSpecification_PR_unsigned,
//			TypeSpecification_PR_floatingpoint,
//			TypeSpecification_PR_octetstring,
//			TypeSpecification_PR_visiblestring,
//			TypeSpecification_PR_generalizedtime,
//			TypeSpecification_PR_binarytime,
//			TypeSpecification_PR_bcd,
//			TypeSpecification_PR_objId,
//			TypeSpecification_PR_mMSString,
//			TypeSpecification_PR_utctime
//		} TypeSpecification_PR;

		switch (namedVariable->type) {
		case MMS_BOOLEAN:
			typeSpec->present = TypeSpecification_PR_boolean;
			break;
		case MMS_BIT_STRING:
			typeSpec->present = TypeSpecification_PR_bitstring;
			typeSpec->choice.bitstring = namedVariable->typeSpec.bitString;
			break;
		case MMS_INTEGER:
			typeSpec->present = TypeSpecification_PR_integer;
			typeSpec->choice.integer = namedVariable->typeSpec.integer;
			break;
		case MMS_UNSIGNED:
			typeSpec->present = TypeSpecification_PR_unsigned;
			typeSpec->choice.Unsigned = namedVariable->typeSpec.unsignedInteger;
			break;
		case MMS_FLOAT:
			typeSpec->present = TypeSpecification_PR_floatingpoint;
			typeSpec->choice.floatingpoint.exponentwidth =
					namedVariable->typeSpec.floatingpoint.exponentWidth;
			typeSpec->choice.floatingpoint.formatwidth =
					namedVariable->typeSpec.floatingpoint.formatWidth;
			break;
		case MMS_OCTET_STRING:
			typeSpec->present = TypeSpecification_PR_octetstring;
			typeSpec->choice.octetstring = namedVariable->typeSpec.octetString;
			break;
		case MMS_VISIBLE_STRING:
			typeSpec->present = TypeSpecification_PR_visiblestring;
			typeSpec->choice.visiblestring = namedVariable->typeSpec.visibleString;
			break;
		case MMS_STRING:
			typeSpec->present = TypeSpecification_PR_mMSString;
			typeSpec->choice.mMSString = namedVariable->typeSpec.mmsString;
			break;
		case MMS_UTC_TIME:
			typeSpec->present = TypeSpecification_PR_utctime;
			break;
		default:
			if (DEBUG) printf("MMS-SERVER: Unsupported type %i!\n", namedVariable->type);
			return -1;
			break;
		}
	}

	return 1;
}

static void
freeTypeSpecRecursive(TypeSpecification_t* typeSpec) {
	if (typeSpec->present == TypeSpecification_PR_structure) {
		int elementCount =
				typeSpec->choice.structure.components.list.count;

		int i;

		for (i = 0; i < elementCount; i++) {
			free(typeSpec->choice.structure.components.list.array[i]->componentName);
			freeTypeSpecRecursive(typeSpec->choice.structure.components.list.array[i]->componentType);
			free(typeSpec->choice.structure.components.list.array[i]->componentType);
			free(typeSpec->choice.structure.components.list.array[i]);
		}

		free(typeSpec->choice.structure.components.list.array);
	}
	else if (typeSpec->present == TypeSpecification_PR_array) {
		free(typeSpec->choice.array.numberOfElements.buf);
		freeTypeSpecRecursive(typeSpec->choice.array.elementType);
		free(typeSpec->choice.array.elementType);
	}
}

static void
deleteVariableAccessAttributesResponse(
		GetVariableAccessAttributesResponse_t* getVarAccessAttr)
{
	if (getVarAccessAttr->typeSpecification.present	== TypeSpecification_PR_structure) {
		int size =	getVarAccessAttr->typeSpecification.choice.structure.components.list.size;
		int count =	getVarAccessAttr->typeSpecification.choice.structure.components.list.count;

		int i;
		TypeSpecification_t* typeSpec;
		for (i = 0; i < count; i++) {
			free(getVarAccessAttr->typeSpecification.choice.structure.components.list.array[i]->componentName);
			typeSpec =
					getVarAccessAttr->typeSpecification.choice.structure.components.list.array[i]->componentType;
			freeTypeSpecRecursive(typeSpec);
			free(typeSpec);
			free(getVarAccessAttr->typeSpecification.choice.structure.components.list.array[i]);
		}

		free(getVarAccessAttr->typeSpecification.choice.structure.components.list.array);

		getVarAccessAttr->typeSpecification.choice.structure.components.list.array = NULL;
		getVarAccessAttr->typeSpecification.choice.structure.components.list.count = 0;
		getVarAccessAttr->typeSpecification.choice.structure.components.list.size =	0;
	} else {
		if (DEBUG) printf("problem deleting ASN1 structure!\n");
	}
}

static int
createVariableAccessAttributesResponse(
		MmsServerConnection* connection,
		char* domainId,
		char* nameId,
		int invokeId,
		ByteBuffer* response)
{
	asn_enc_rval_t rval;
	GetVariableAccessAttributesResponse_t* getVarAccessAttr;
	MmsPdu_t* mmsPdu;
	MmsTypeSpecification* namedVariable;
	MmsDevice* device = MmsServer_getDevice(connection->server);

	MmsDomain* domain = MmsDevice_getDomain(device, domainId);
	
	if (domain == NULL) {
		if (DEBUG) printf("mms_server: domain %s not known\n", domainId);
		return -1;
	}

	namedVariable = MmsDomain_getNamedVariable(domain, nameId);

	if (namedVariable == NULL) {
		if (DEBUG) printf("mms_server: named variable %s not known\n", nameId);
		return -1;
	}

	mmsPdu = mmsServer_createConfirmedResponse(invokeId);

	mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.present =
			ConfirmedServiceResponse_PR_getVariableAccessAttributes;
	
	getVarAccessAttr = &(mmsPdu->choice.confirmedResponsePdu.
			confirmedServiceResponse.choice.getVariableAccessAttributes);

	getVarAccessAttr->mmsDeletable = 0;

	createTypeSpecification(namedVariable, &getVarAccessAttr->typeSpecification);

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
			mmsServer_write_out, (void*) response);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	deleteVariableAccessAttributesResponse(getVarAccessAttr);

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	return 0;
}

int
mmsServer_handleGetVariableAccessAttributesRequest(
		MmsServerConnection* connection,
		GetVariableAccessAttributesRequest_t* request,
		int invokeId,
		ByteBuffer* response)
{
	if (request->present == GetVariableAccessAttributesRequest_PR_name) {
		if (request->choice.name.present == ObjectName_PR_domainspecific) {
			Identifier_t domainId = request->choice.name.choice.domainspecific.domainId;
			Identifier_t nameId = request->choice.name.choice.domainspecific.itemId;

			char* domainIdStr = createStringFromBuffer(domainId.buf, domainId.size);
			char* nameIdStr = createStringFromBuffer(nameId.buf, nameId.size);
			if (DEBUG) printf("getVariableAccessAttributes domainId: %s nameId: %s\n", domainIdStr, nameIdStr);

			createVariableAccessAttributesResponse(connection, domainIdStr, nameIdStr, invokeId, response);

			free(domainIdStr);
			free(nameIdStr);
		}
		else {
			if (DEBUG) printf("GetVariableAccessAttributesRequest with name other than domainspecific is not supported!\n");
			return -1;
		}
	}
	else {
		if (DEBUG) printf("GetVariableAccessAttributesRequest with address not supported!\n");
		return -1;
	}

	return 0;
}

