/*
 *  cpatype_encode.c
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
#include <stdio.h>
#include <sys/types.h>

#include "iso_presentation.h"
#include "CPAPPDU.h"
#include "stack_config.h"


static int
write_out(const void *buffer, size_t size, void *app_key)
{
    ByteBuffer* writeBuffer = (ByteBuffer*) app_key;
    return ByteBuffer_append(writeBuffer, (uint8_t*) buffer, size);
}

static
void codeCPAMessage(ByteBuffer* writeBuffer, uint8_t* apdu, size_t apduSize)
{
	CPAPPDU_t* cpatype;
	Resultlist_t* pdvResultList;
	Transfersyntaxname_t* transfersyntax;
	Userdata_t* userdata;
	struct PDVlist* pdvList;
	uint8_t* apduCopy;
	asn_enc_rval_t rval;

	Respondingpresentationselector_t* respondingSelector;

    cpatype = calloc(1, sizeof(CPAPPDU_t));

    cpatype->modeselector.modevalue = modevalue_normalmode;

    cpatype->normalmodeparameters = calloc(1, sizeof(struct normalmodeparameters));

    respondingSelector =
    		calloc(1, sizeof(Respondingpresentationselector_t));

    respondingSelector->size = 4;
    respondingSelector->buf = calloc(1, 4);
    respondingSelector->buf[0] = 0;
    respondingSelector->buf[1] = 0;
    respondingSelector->buf[2] = 0;
    respondingSelector->buf[3] = 1;

    pdvResultList = calloc(1, sizeof(Resultlist_t));

    pdvResultList->list.count = 2;

    pdvResultList->list.array = calloc(2, sizeof(struct Member*));

    pdvResultList->list.array[0] = calloc(1, sizeof(struct Member));

    pdvResultList->list.array[0]->result = Result_acceptance;

    pdvResultList->list.array[0]->providerreason = 0;

    transfersyntax = calloc(1, sizeof(Transfersyntaxname_t));

    transfersyntax->size = 2;
    transfersyntax->buf = calloc(2, sizeof(uint8_t));
    transfersyntax->buf[0] = 0x51;
    transfersyntax->buf[1] = 0x01;

    pdvResultList->list.array[0]->transfersyntaxname = transfersyntax;

    pdvResultList->list.array[1] = calloc(1, sizeof(struct Member));

	pdvResultList->list.array[1]->result = Result_acceptance;

    pdvResultList->list.array[1]->providerreason = 0;

    transfersyntax = calloc(1, sizeof(Transfersyntaxname_t));

	transfersyntax->size = 2;
	transfersyntax->buf = calloc(2, sizeof(uint8_t));
	transfersyntax->buf[0] = 0x51;
	transfersyntax->buf[1] = 0x01;

	pdvResultList->list.array[1]->transfersyntaxname = transfersyntax;

    cpatype->normalmodeparameters->protocolversion = 0; //protocolversion;
	cpatype->normalmodeparameters->respondingpresentationselector = respondingSelector;
	cpatype->normalmodeparameters->presentationcontextdefinitionresultlist = pdvResultList;
	cpatype->normalmodeparameters->presentationrequirements = 0;
	cpatype->normalmodeparameters->usersessionrequirements = 0;

	userdata = calloc(1, sizeof(Userdata_t));

	userdata->present = Userdata_PR_fullyencodeddata;
	userdata->choice.fullyencodeddata.list.count=1;
	userdata->choice.fullyencodeddata.list.array = calloc(1, sizeof(struct PDVlist*));

	pdvList = calloc(1, sizeof(struct PDVlist));

	pdvList->transfersyntaxname = 0;
	pdvList->presentationcontextidentifier = 1; /* 1 - ACSE */
	pdvList->presentationdatavalues.present = presentationdatavalues_PR_singleASN1type;

	/* Copy apdu buffer - required for free_struct to work properly */
	apduCopy = calloc(apduSize, sizeof(uint8_t));
	memcpy(apduCopy, apdu, apduSize);

	pdvList->presentationdatavalues.choice.singleASN1type.size = apduSize;
	pdvList->presentationdatavalues.choice.singleASN1type.buf = apduCopy;

	userdata->choice.fullyencodeddata.list.array[0] = pdvList;

	cpatype->normalmodeparameters->userdata = userdata;

	 rval = der_encode(&asn_DEF_CPAPPDU, cpatype,
	            write_out, writeBuffer);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_CPAPPDU, cpatype);

	asn_DEF_CPAPPDU.free_struct(&asn_DEF_CPAPPDU, cpatype, 0);
}

void
IsoPresentation_createCpaMessage(IsoPresentation* presentation, ByteBuffer* writeBuffer, ByteBuffer* payload)
{
	codeCPAMessage(writeBuffer, payload->buffer, payload->size);
}

IsoPresentationIndication
IsoPresentation_parseAcceptMessage(IsoPresentation* self, ByteBuffer* message)
{
	CPAPPDU_t* cpatype = 0;
	asn_dec_rval_t rval;
	int decodedOctets;
	Resultlist_t* resultList;
	struct Member* resultListEntry;
	PDVlist_t* pdvlist;
	int userDataSize;

	rval = ber_decode(NULL, &asn_DEF_CPAPPDU,
			(void**) &cpatype, (void*) ByteBuffer_getBuffer(message), ByteBuffer_getSize(message));

	if (rval.code != RC_OK) {
		if (DEBUG) printf("iso_presentation.c: broken CPType encoding at byte %ld\n",
				(long) rval.consumed);
		goto cpatype_error;
	}

	decodedOctets = rval.consumed;

	if (cpatype->modeselector.modevalue != modevalue_normalmode)
		goto cpatype_error;

	if (cpatype->normalmodeparameters == NULL)
		goto cpatype_error;

	resultList = cpatype->normalmodeparameters->presentationcontextdefinitionresultlist;

	if (resultList == NULL)
		goto cpatype_error;

	if (resultList->list.count != 2)
		goto cpatype_error;

	
	resultListEntry = resultList->list.array[0];

	if (resultListEntry->result != Result_acceptance)
		goto cpatype_error;

	isoPres_setContextDefinition(self, 0, resultListEntry);

	resultListEntry = resultList->list.array[1];

	if (resultListEntry->result != Result_acceptance)
		goto cpatype_error;

	isoPres_setContextDefinition(self, 1, resultListEntry);

	if (cpatype->normalmodeparameters->userdata->present != Userdata_PR_fullyencodeddata)
		goto cpatype_error;

	if (cpatype->normalmodeparameters->userdata->choice.fullyencodeddata.list.count != 1)
		goto cpatype_error;

	pdvlist = cpatype->normalmodeparameters->userdata->choice.fullyencodeddata.list.array[0];

	if (pdvlist->presentationdatavalues.present != presentationdatavalues_PR_singleASN1type)
		goto cpatype_error;

	userDataSize = pdvlist->presentationdatavalues.choice.singleASN1type.size;

	ByteBuffer_wrap(&(self->nextPayload),
			ByteBuffer_getBuffer(message) + (decodedOctets - userDataSize),
			userDataSize, userDataSize);

	asn_DEF_CPAPPDU.free_struct(&asn_DEF_CPAPPDU, cpatype, 0);

	self->nextContextId = 3;

	return PRESENTATION_OK;

cpatype_error:
	asn_DEF_CPAPPDU.free_struct(&asn_DEF_CPAPPDU, cpatype, 0);
	return PRESENTATION_ERROR;
}
