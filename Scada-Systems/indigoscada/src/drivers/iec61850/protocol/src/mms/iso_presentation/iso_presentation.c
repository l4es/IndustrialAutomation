/*
 *  iso_presentation.c
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
#include <stdlib.h>

#include "iso_presentation.h"
#include "CPtype.h"
#include "stack_config.h"

static uint8_t presentationSelector[] = {0x00, 0x00, 0x00, 0x01};

static uint8_t asn_id_as_acse[] = {0x52, 0x01, 0x00, 0x01};

static uint8_t asn_id_mms[] = {0x28, 0xca, 0x22, 0x02, 0x01};

static uint8_t ber_id[] = {0x51, 0x01};

static int
write_out(const void *buffer, size_t size, void *app_key)
{
    ByteBuffer* writeBuffer = (ByteBuffer*) app_key;
    return ByteBuffer_append(writeBuffer, (uint8_t*) buffer, size);
}

IsoPresentationIndication
isoPres_setContextDefinition(IsoPresentation* self, int index, struct Member* member)
{
	if (member->abstractsyntaxname.size > 9) {
		if (DEBUG) printf("iso_presentation.c: ASN OID size to large.\n");
		return PRESENTATION_ERROR;
	}

	self->context[index].contextId =
			(unsigned char)member->presentationcontextidentifier;

	self->context[index].abstractSyntaxName[0] =
			 member->abstractsyntaxname.size;

	memcpy(self->context[index].abstractSyntaxName + 1,
			member->abstractsyntaxname.buf, member->abstractsyntaxname.size);

	return PRESENTATION_OK;
}


static void
createConnectPdu(IsoPresentation* self, ByteBuffer* buffer, ByteBuffer* payload)
{
	Contextlist_t* contextList;
	struct Member* member;
	Userdata_t* userData;
	asn_enc_rval_t rval;

	CPtype_t* cptype = 0;

	cptype = calloc(1, sizeof(CPtype_t));

	cptype->modeselector.modevalue = modevalue_normalmode;

	cptype->normalmodeparameters = calloc(1, sizeof(struct normalmodeparameters));

	cptype->normalmodeparameters->callingpresentationselector =
			calloc(1, sizeof(Callingpresentationselector_t));

	cptype->normalmodeparameters->callingpresentationselector->buf = malloc(4);
	memcpy(cptype->normalmodeparameters->callingpresentationselector->buf,
			presentationSelector, 4);
	cptype->normalmodeparameters->callingpresentationselector->size = 4;

	cptype->normalmodeparameters->calledpresentationselector =
			calloc(1, sizeof(Calledpresentationselector_t));

	cptype->normalmodeparameters->calledpresentationselector->buf = malloc(4);
	memcpy(cptype->normalmodeparameters->calledpresentationselector->buf,
			presentationSelector, 4);
	cptype->normalmodeparameters->calledpresentationselector->size = 4;

	contextList = calloc(1, sizeof(Contextlist_t));

	contextList->list.count = 2;
	contextList->list.size = 2;
	contextList->list.array = calloc(2, sizeof(struct Member*));

	

	/* Syntax definition for ACSE */
	member = calloc(1, sizeof(struct Member));
	member->presentationcontextidentifier = 1;
	member->abstractsyntaxname.size = 4;
	member->abstractsyntaxname.buf = malloc(4);
	memcpy(member->abstractsyntaxname.buf, asn_id_as_acse, 4);
	member->transfersyntaxnamelist.list.count = 1;
	member->transfersyntaxnamelist.list.array = calloc(1, sizeof(Transfersyntaxname_t*));
	member->transfersyntaxnamelist.list.array[0] = calloc(1, sizeof(Transfersyntaxname_t));
	member->transfersyntaxnamelist.list.array[0]->size = 2;
	member->transfersyntaxnamelist.list.array[0]->buf = malloc(2);
	memcpy(member->transfersyntaxnamelist.list.array[0]->buf, ber_id, 2);

	contextList->list.array[0] = member;

	/* Syntax definition for MMS */
	member = calloc(1, sizeof(struct Member));
	member->presentationcontextidentifier = 3;
	member->abstractsyntaxname.size = 5;
	member->abstractsyntaxname.buf = malloc(5);
	memcpy(member->abstractsyntaxname.buf, asn_id_mms, 5);
	member->transfersyntaxnamelist.list.count = 1;
	member->transfersyntaxnamelist.list.array = calloc(1, sizeof(Transfersyntaxname_t*));
	member->transfersyntaxnamelist.list.array[0] = calloc(1, sizeof(Transfersyntaxname_t));
	member->transfersyntaxnamelist.list.array[0]->size = 2;
	member->transfersyntaxnamelist.list.array[0]->buf = malloc(2);
	memcpy(member->transfersyntaxnamelist.list.array[0]->buf, ber_id, 2);

	contextList->list.array[1] = member;

	cptype->normalmodeparameters->presentationcontextdefinitionlist = contextList;

	userData = calloc(1, sizeof(Userdata_t));

	userData->present = Userdata_PR_fullyencodeddata;

	userData->choice.fullyencodeddata.list.count = 1;

	userData->choice.fullyencodeddata.list.array = calloc(1, sizeof(PDVlist_t*));
	userData->choice.fullyencodeddata.list.array[0] = calloc(1, sizeof(PDVlist_t));

	userData->choice.fullyencodeddata.list.array[0]->presentationcontextidentifier = 1;
	userData->choice.fullyencodeddata.list.array[0]->presentationdatavalues.present =
			presentationdatavalues_PR_singleASN1type;

	userData->choice.fullyencodeddata.list.array[0]
	     ->presentationdatavalues.choice.singleASN1type.size = payload->size;
	userData->choice.fullyencodeddata.list.array[0]
		     ->presentationdatavalues.choice.singleASN1type.buf = payload->buffer;


	cptype->normalmodeparameters->userdata = userData;

	rval = der_encode(&asn_DEF_CPtype, cptype,
				write_out, buffer);


	if (rval.encoded == -1)
		printf("ERROR ENCODING CONNECT PDU\n");

	if (DEBUG) xer_fprint(stdout, &asn_DEF_CPtype, cptype);

	free(userData->choice.fullyencodeddata.list.array[0]);
	free(userData->choice.fullyencodeddata.list.array);
	free(userData);

	cptype->normalmodeparameters->userdata = 0;

	asn_DEF_CPtype.free_struct(&asn_DEF_CPtype, cptype, 0);
}

static IsoPresentationIndication
parseConnectPdu
(
		IsoPresentation* self,
		ByteBuffer* buffer
)
{
	int i;
	int pdvListEntries;
	int decodedOctets;
	CPtype_t* cptype = 0;
	asn_dec_rval_t rval;  	/* Decoder return value  */

	rval = ber_decode(NULL, &asn_DEF_CPtype,
			(void**) &cptype, (void*) ByteBuffer_getBuffer(buffer), ByteBuffer_getSize(buffer));

	if (rval.code != RC_OK) {
		if (DEBUG) printf("iso_presentation.c: broken CPType encoding at byte %ld\n",
				(long) rval.consumed);
		goto cptype_error;
	}

	decodedOctets = rval.consumed;

	if (cptype->modeselector.modevalue != modevalue_normalmode) {
		if (DEBUG) printf("iso_presentation.c: only normal mode supported!");
		goto cptype_error;
	}

	pdvListEntries = cptype->normalmodeparameters->presentationcontextdefinitionlist->list.count;

	if (pdvListEntries != 2) {
		if (DEBUG) printf("iso_presentation: 2 pdv list items required found: %i\n", pdvListEntries);
		goto cptype_error;
	}


	for (i = 0; i < pdvListEntries; i++) {
		struct Member* member =
				cptype->normalmodeparameters->presentationcontextdefinitionlist->list.array[i];

		if (isoPres_setContextDefinition(self, i, member) == PRESENTATION_ERROR)
			goto cptype_error;
	}


	if (cptype->normalmodeparameters->userdata->present
			== Userdata_PR_fullyencodeddata)
	{
		int dataListEntries =
				cptype->normalmodeparameters->userdata->choice.fullyencodeddata.list.count;

		PDVlist_t* pdvListEntry =
				cptype->normalmodeparameters->userdata->choice.fullyencodeddata.list.array[0];

		int userDataSize =
				pdvListEntry->presentationdatavalues.choice.singleASN1type.size;

		uint8_t* userData =
				pdvListEntry->presentationdatavalues.choice.singleASN1type.buf;

		self->nextContextId =
				(unsigned char)pdvListEntry->presentationcontextidentifier;

		ByteBuffer_wrap(&(self->nextPayload),
				buffer->buffer + (decodedOctets - userDataSize),
				userDataSize, userDataSize);
	}
	else {
		if (DEBUG) printf("iso_presentation.c: Unsupported user data: simply encoded data\n");
		goto cptype_error;
	}

	if (DEBUG) xer_fprint(stdout, &asn_DEF_CPtype, cptype);
	asn_DEF_CPtype.free_struct(&asn_DEF_CPtype, cptype, 0);
	return PRESENTATION_OK;

cptype_error:
	asn_DEF_CPtype.free_struct(&asn_DEF_CPtype, cptype, 0);
	return PRESENTATION_ERROR;
}

void
IsoPresentation_init(IsoPresentation* session)
{

}

static int
calcLengthOfBERLengthField(int value)
{
	if (value < 128)
		return 1;
	if (value < 0x100)
		return 2;
	if (value < 0x10000) {
		return 3;
	}
	return -1;
}

static int
encodeBERLengthField(uint8_t* buffer, int pos, int value)
{
	if (value < 128) {
		buffer[pos++] = (uint8_t) value;
		return pos;
	}
	if (value < 0x100) {
		buffer[pos++] = (uint8_t) 0x81;
		buffer[pos++] = (uint8_t) value;
		return pos;
	}
	if (value < 0x10000) {
		buffer[pos++] = (uint8_t) 0x82;
		buffer[pos++] = (uint8_t) (value / 0x100);
		buffer[pos++] = (uint8_t) (value & 0xff);
		return pos;
	}
	return -1;
}

IsoPresentationIndication
IsoPresentation_createUserData(IsoPresentation* self, ByteBuffer* writeBuffer, ByteBuffer* payload)
{
	int pos =  ByteBuffer_getSize(writeBuffer);
	uint8_t* buf = ByteBuffer_getBuffer(writeBuffer);

	int payloadLength = ByteBuffer_getSize(payload);

	int userDataLengthFieldSize = calcLengthOfBERLengthField(payloadLength);
	int pdvListLength = payloadLength + (userDataLengthFieldSize + 4);

	int pdvListLengthFieldSize = calcLengthOfBERLengthField(pdvListLength);
	int presentationLength = pdvListLength + (pdvListLengthFieldSize + 1);

	// int presentationLengthFieldSize = calcLengthOfBERLengthField(totalLength);
	buf[pos++] = (uint8_t) 0x61;
	pos = encodeBERLengthField(buf, pos, presentationLength);
	buf[pos++] = (uint8_t) 0x30;
	pos = encodeBERLengthField(buf, pos, pdvListLength);
	buf[pos++] = (uint8_t) 0x02;
	buf[pos++] = (uint8_t) 0x01;
	buf[pos++] = (uint8_t) self->nextContextId;
	buf[pos++] = (uint8_t) 0xa0;
	pos = encodeBERLengthField(buf, pos, payloadLength);

	ByteBuffer_setSize(writeBuffer, pos);

	ByteBuffer_append(writeBuffer, ByteBuffer_getBuffer(payload), payloadLength);

	return PRESENTATION_OK;
}

static int
parseBERLengthField(uint8_t* buffer, int pos, int* length)
{
	int len;

	if (buffer[pos] & 0x80)
		len = buffer[pos] & 0x7f;
	else
		len = 0;

	if (length != NULL) {
		if (len == 0)
			*length = buffer[pos];
		else if (len == 1) {
			*length = buffer[pos + 1];
		}
		else {
			*length = (buffer[pos + 1] * 0x100) + buffer[pos + 2];
		}
	}

	return pos + 1 + len;
}

IsoPresentationIndication
IsoPresentation_parseUserData(IsoPresentation* self, ByteBuffer* buffer)
{
	int userDataLength;
	int pos;
	int length = buffer->size;
	uint8_t* buf = buffer->buffer;

	if (length < 9)
		return PRESENTATION_ERROR;

	if (buf[0] != 0x61)
		return PRESENTATION_ERROR;

	pos = 1;
	pos = parseBERLengthField(buf, pos, NULL);

	if (buf[pos++] != 0x30)
		return PRESENTATION_ERROR;

	pos = parseBERLengthField(buf, pos, NULL);

	if (buf[pos++] != 0x02)
		return PRESENTATION_ERROR;

	if (buf[pos++] != 0x01)
		return PRESENTATION_ERROR;

	self->nextContextId = buf[pos++];

	if (buf[pos++] != 0xa0)
		return PRESENTATION_ERROR;

	

	pos = parseBERLengthField(buf, pos, &userDataLength);

	ByteBuffer_wrap(&(self->nextPayload), buf + pos, userDataLength, userDataLength);

	return PRESENTATION_OK;
}

IsoPresentationIndication
IsoPresentation_parseConnect(IsoPresentation* self, ByteBuffer* buffer)
{
	return parseConnectPdu(self, buffer);
}

void
IsoPresentation_createConnectPdu(IsoPresentation* self, ByteBuffer* buffer, ByteBuffer* payload)
{
	createConnectPdu(self, buffer, payload);
}
