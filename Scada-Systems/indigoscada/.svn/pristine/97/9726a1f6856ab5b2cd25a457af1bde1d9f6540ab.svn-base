/*
 *  mms_client_get_var_access.c
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
#include <MmsPdu.h>
#include "stack_config.h"
#include "mms_common.h"
#include "mms_client_connection.h"
#include "byte_buffer.h"
#include "string_utilities.h"

#include "mms_client_internal.h"


/*
 	TypeSpecification_PR_NOTHING,
	TypeSpecification_PR_typeName,
	TypeSpecification_PR_array,
	TypeSpecification_PR_structure,
	TypeSpecification_PR_boolean,
	TypeSpecification_PR_bitstring,
	TypeSpecification_PR_integer,
	TypeSpecification_PR_unsigned,
	TypeSpecification_PR_floatingpoint,
	TypeSpecification_PR_octetstring,
	TypeSpecification_PR_visiblestring,
	TypeSpecification_PR_generalizedtime,
	TypeSpecification_PR_binarytime,
	TypeSpecification_PR_bcd,
	TypeSpecification_PR_objId,
	TypeSpecification_PR_mMSString,
	TypeSpecification_PR_utctime
 */

static MmsTypeSpecification*
createTypeSpecification(TypeSpecification_t* asnTypeSpec) {
	MmsTypeSpecification* typeSpec = calloc(1, sizeof(MmsTypeSpecification));

	switch (asnTypeSpec->present) {
	case TypeSpecification_PR_structure:
	{
			int elementCount;
			int i;
			typeSpec->type = MMS_STRUCTURE;

			elementCount = asnTypeSpec->choice.structure.components.list.count;
			typeSpec->typeSpec.structure.elementCount = elementCount;

			typeSpec->typeSpec.structure.elements =
					calloc(elementCount, sizeof(MmsTypeSpecification*));



			for (i = 0; i < elementCount; i++) {

				char* name = createStringFromBuffer(
						asnTypeSpec->choice.structure.components.list.array[i]->componentName->buf,
						asnTypeSpec->choice.structure.components.list.array[i]->componentName->size);

				typeSpec->typeSpec.structure.elements[i] =
						createTypeSpecification(asnTypeSpec->choice.structure.components.
								list.array[i]->componentType);

				typeSpec->typeSpec.structure.elements[i]->name = name;
			}
	}
	break;
	case TypeSpecification_PR_array:
		{
			long elementCount;
			typeSpec->type = MMS_ARRAY;
			
			asn_INTEGER2long(&asnTypeSpec->choice.array.numberOfElements, &elementCount);

			typeSpec->typeSpec.array.elementCount = elementCount;

			typeSpec->typeSpec.array.elementTypeSpec =
					createTypeSpecification(asnTypeSpec->choice.array.elementType);
		}
		break;
	case TypeSpecification_PR_boolean:
		typeSpec->type = MMS_BOOLEAN;
		break;
	case TypeSpecification_PR_bitstring:
		typeSpec->type = MMS_BIT_STRING;
		typeSpec->typeSpec.bitString = asnTypeSpec->choice.bitstring;
		break;
	case TypeSpecification_PR_integer:
		typeSpec->type = MMS_INTEGER;
		typeSpec->typeSpec.integer = asnTypeSpec->choice.integer;
		break;
	case TypeSpecification_PR_unsigned:
		typeSpec->type = MMS_UNSIGNED;
		typeSpec->typeSpec.unsignedInteger = asnTypeSpec->choice.Unsigned;
		break;
	case TypeSpecification_PR_floatingpoint:
		typeSpec->type = MMS_FLOAT;
		typeSpec->typeSpec.floatingpoint.exponentWidth =
				asnTypeSpec->choice.floatingpoint.exponentwidth;
		typeSpec->typeSpec.floatingpoint.formatWidth =
						asnTypeSpec->choice.floatingpoint.formatwidth;
		break;
	case TypeSpecification_PR_octetstring:
		typeSpec->type = MMS_OCTET_STRING;
		typeSpec->typeSpec.octetString = asnTypeSpec->choice.octetstring;
		break;
	case TypeSpecification_PR_visiblestring:
		typeSpec->type = MMS_VISIBLE_STRING;
		typeSpec->typeSpec.visibleString = asnTypeSpec->choice.visiblestring;
		break;
	case TypeSpecification_PR_mMSString:
		typeSpec->type = MMS_STRING;
		typeSpec->typeSpec.mmsString = asnTypeSpec->choice.mMSString;
		break;
	case TypeSpecification_PR_utctime:
		typeSpec->type = MMS_UTC_TIME;
		break;
	default:
		printf("ERROR: unknown type in type specification\n");
		break;
	}

	return typeSpec;
}

MmsTypeSpecification*
mmsClient_parseGetVariableAccessAttributesResponse(ByteBuffer* message, uint32_t* invokeId)
{
	MmsPdu_t* mmsPdu = 0; /* allow asn1c to allocate structure */
	MmsTypeSpecification* typeSpec = NULL;

	asn_dec_rval_t rval;

	rval = ber_decode(NULL, &asn_DEF_MmsPdu,
			(void**) &mmsPdu, ByteBuffer_getBuffer(message), ByteBuffer_getSize(message));

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	if (mmsPdu->present == MmsPdu_PR_confirmedResponsePdu) {

		*invokeId = mmsClient_getInvokeId(&mmsPdu->choice.confirmedResponsePdu);

		if (mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.present ==
				ConfirmedServiceResponse_PR_getVariableAccessAttributes)
		{
			TypeSpecification_t* asnTypeSpec;
			GetVariableAccessAttributesResponse_t* response;

			response = &(mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice.getVariableAccessAttributes);
			asnTypeSpec = &response->typeSpecification;

			typeSpec = createTypeSpecification(asnTypeSpec);
		}
	}

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	return typeSpec;
}

int
mmsClient_createGetVariableAccessAttributesRequest(
		char* domainId, char* itemId,
		ByteBuffer* writeBuffer)
{
	asn_enc_rval_t rval;
	GetVariableAccessAttributesRequest_t* request;
	MmsPdu_t* mmsPdu = mmsClient_createConfirmedRequestPdu(1);

	mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.present =
			ConfirmedServiceRequest_PR_getVariableAccessAttributes;

	request =
	  &(mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.choice.getVariableAccessAttributes);

	request->present = GetVariableAccessAttributesRequest_PR_name;

	request->choice.name.present = ObjectName_PR_domainspecific;

	request->choice.name.choice.domainspecific.domainId.buf = domainId;
	request->choice.name.choice.domainspecific.domainId.size = strlen(domainId);
	request->choice.name.choice.domainspecific.itemId.buf = itemId;
	request->choice.name.choice.domainspecific.itemId.size = strlen(itemId);

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
	            mmsClient_write_out, (void*) writeBuffer);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	request->choice.name.choice.domainspecific.domainId.buf = 0;
	request->choice.name.choice.domainspecific.domainId.size = 0;
	request->choice.name.choice.domainspecific.itemId.buf = 0;
	request->choice.name.choice.domainspecific.itemId.size = 0;

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	return rval.encoded;
}

