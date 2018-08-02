/*
 *  mms_client_write.c
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
#include "mms_common.h"
#include "mms_client_connection.h"
#include "byte_buffer.h"

#include "mms_client_internal.h"
#include "mms_common_internal.h"

#include "stack_config.h"

MmsIndication
mmsClient_parseWriteResponse(ByteBuffer* message)
{
	MmsPdu_t* mmsPdu = 0;
	MmsIndication retVal =  MMS_OK;

	asn_dec_rval_t rval;

	rval = ber_decode(NULL, &asn_DEF_MmsPdu,
			(void**) &mmsPdu, ByteBuffer_getBuffer(message), ByteBuffer_getSize(message));

	if (rval.code != RC_OK) {
		retVal = MMS_ERROR;
		goto cleanUp;
	}

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	if (mmsPdu->present == MmsPdu_PR_confirmedResponsePdu) {
		//*invokeId = mmsClient_getInvokeId(&mmsPdu->choice.confirmedResponsePdu);

		if (mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.present == ConfirmedServiceResponse_PR_write)
		{
			WriteResponse_t* response =
					&(mmsPdu->choice.confirmedResponsePdu.confirmedServiceResponse.choice.write);

			if (response->list.count > 0) {
				if (response->list.array[0]->present == WriteResponse__Member_PR_success)
					retVal = MMS_OK;
				else
					retVal = MMS_ERROR;
			}
			else
				retVal = MMS_ERROR;

		}
		else {
			retVal = MMS_ERROR;
		}
	}
	else {
		retVal = MMS_ERROR;
	}

cleanUp:
	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	return retVal;
}


static VariableSpecification_t*
createNewDomainVariableSpecification(char* domainId, char* itemId)
{
	VariableSpecification_t* varSpec = calloc(1, sizeof(ListOfVariableSeq_t));
	varSpec->present = VariableSpecification_PR_name;
	varSpec->choice.name.present = ObjectName_PR_domainspecific;
	varSpec->choice.name.choice.domainspecific.domainId.buf = domainId;
	varSpec->choice.name.choice.domainspecific.domainId.size = strlen(domainId);
	varSpec->choice.name.choice.domainspecific.itemId.buf = itemId;
	varSpec->choice.name.choice.domainspecific.itemId.size = strlen(itemId);

	return varSpec;
}

int
mmsClient_createWriteRequest(long invokeId, char* domainId, char* itemId, MmsValue* value,
		ByteBuffer* writeBuffer)
{
	asn_enc_rval_t rval;
	WriteRequest_t* request;
	MmsPdu_t* mmsPdu = mmsClient_createConfirmedRequestPdu(invokeId);

	mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.present =
			ConfirmedServiceRequest_PR_write;
	request =
			&(mmsPdu->choice.confirmedRequestPdu.confirmedServiceRequest.choice.write);

	/* Create list of variable specifications */
	request->variableAccessSpecification.present = VariableAccessSpecification_PR_listOfVariable;
	request->variableAccessSpecification.choice.listOfVariable.list.count = 1;
	request->variableAccessSpecification.choice.listOfVariable.list.size = 1;
	request->variableAccessSpecification.choice.listOfVariable.list.array =
			calloc(1, sizeof(ListOfVariableSeq_t*));
	request->variableAccessSpecification.choice.listOfVariable.list.array[0] =
			createNewDomainVariableSpecification(domainId, itemId);

	/* Create list of typed data values */
	request->listOfData.list.count = 1;
	request->listOfData.list.size = 1;
	request->listOfData.list.array = calloc(1, sizeof(struct Data*));
	request->listOfData.list.array[0] = mmsMsg_createBasicDataElement(value);

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
				mmsClient_write_out, (void*) writeBuffer);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	/* Free ASN structure */
	request->variableAccessSpecification.choice.listOfVariable.list.count = 0;

	free(request->variableAccessSpecification.choice.listOfVariable.list.array[0]);
	free(request->variableAccessSpecification.choice.listOfVariable.list.array);
	request->variableAccessSpecification.choice.listOfVariable.list.array = 0;

	request->listOfData.list.count = 0;
	if (request->listOfData.list.array[0]->present == Data_PR_floatingpoint) {
		free(request->listOfData.list.array[0]->choice.floatingpoint.buf);
	}

	free(request->listOfData.list.array[0]);
	free(request->listOfData.list.array);
	request->listOfData.list.array = 0;

	asn_DEF_MmsPdu.free_struct(&asn_DEF_MmsPdu, mmsPdu, 0);

	return rval.encoded;
}
