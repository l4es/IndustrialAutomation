/*
 *  mms_client_initiate.c
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

#include "stack_config.h"

#include "mms_client_internal.h"

/* servicesSupported = {GetNameList} - required by initiate request */
static uint8_t servicesSupported[] = {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


static InitiateRequestPdu_t
createInitiateRequestPdu()
{
	InitiateRequestPdu_t request;

	request.localDetailCalling = calloc(1, sizeof(Integer32_t));
	*(request.localDetailCalling) = 65000;

	request.proposedMaxServOutstandingCalled = DEFAULT_MAX_SERV_OUTSTANDING_CALLED;
	request.proposedMaxServOutstandingCalling = DEFAULT_MAX_SERV_OUTSTANDING_CALLING;

	request.proposedDataStructureNestingLevel = calloc(1, sizeof(Integer8_t));
	*(request.proposedDataStructureNestingLevel) = DEFAULT_DATA_STRUCTURE_NESTING_LEVEL;

	request.mmsInitRequestDetail.proposedParameterCBB.size = 2;
	request.mmsInitRequestDetail.proposedParameterCBB.bits_unused = 5;
	request.mmsInitRequestDetail.proposedParameterCBB.buf = calloc(2, sizeof(uint8_t));
	request.mmsInitRequestDetail.proposedParameterCBB.buf[0] = 0xf1;
	request.mmsInitRequestDetail.proposedParameterCBB.buf[1] = 0x00;

	request.mmsInitRequestDetail.proposedVersionNumber = 1;

	request.mmsInitRequestDetail.servicesSupportedCalling.size = 11;
	request.mmsInitRequestDetail.servicesSupportedCalling.bits_unused = 3;

	request.mmsInitRequestDetail.servicesSupportedCalling.buf = servicesSupported;

	return request;
}

static void
freeInitiateRequestPdu(InitiateRequestPdu_t request)
{
	free(request.localDetailCalling);
	free(request.proposedDataStructureNestingLevel);
	free(request.mmsInitRequestDetail.proposedParameterCBB.buf);
}

int
mmsClient_createInitiateRequest(ByteBuffer* writeBuffer)
{
	asn_enc_rval_t rval;

	MmsPdu_t* mmsPdu = calloc(1, sizeof(MmsPdu_t));

	mmsPdu->present = MmsPdu_PR_initiateRequestPdu;

	mmsPdu->choice.initiateRequestPdu = createInitiateRequestPdu();

	rval = der_encode(&asn_DEF_MmsPdu, mmsPdu,
	            mmsClient_write_out, (void*) writeBuffer);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_MmsPdu, mmsPdu);

	freeInitiateRequestPdu(mmsPdu->choice.initiateRequestPdu);
	free(mmsPdu);

	return rval.encoded;
}
