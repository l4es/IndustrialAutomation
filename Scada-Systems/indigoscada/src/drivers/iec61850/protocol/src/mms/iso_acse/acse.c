/*
 *  acse.c
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
#include "acse.h"
#include "ACSEapdu.h"
#include "stack_config.h"
#include <stdbool.h>
#include <stdlib.h>

static uint8_t appContextNameMms[] = {0x28, 0xca, 0x22, 0x02, 0x03};

static uint8_t apTitle_1_1_1_999_1[] = {0x29, 0x01, 0x87, 0x67, 0x01};

static uint8_t berOid[2] = {0x51, 0x01};

static uint8_t auth_mech_password_oid[] = {0x52, 0x03, 0x01};

static uint8_t requirements_authentication[] = {0x80};

static int
write_out(const void *buffer, size_t size, void *app_key) {
    ByteBuffer* writeBuffer = (ByteBuffer*) app_key;
    return ByteBuffer_append(writeBuffer, (uint8_t*) buffer, size);
}


static bool
checkAuthMechanismName(AcseConnection* self, AARQapdu_t* aarq)
{
	if (aarq->mechanismname != NULL) {
		if (self->authentication->mechanism == AUTH_PASSWORD) {
			if (aarq->mechanismname->size != 3)
				return false;

			if (memcmp(auth_mech_password_oid, aarq->mechanismname->buf, 3) == 0)
				return true;
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

static bool
checkAuthenticationValue(AcseConnection* self, AARQapdu_t* aarq)
{
	if (aarq->callingauthenticationvalue == NULL)
		return false;

	if (self->authentication->mechanism == AUTH_PASSWORD) {
		if (aarq->callingauthenticationvalue->present == Authenticationvalue_PR_charstring) {
			if (aarq->callingauthenticationvalue->choice.charstring.size !=
					strlen(self->authentication->value.password.string))
				return false;

			if (strncmp(aarq->callingauthenticationvalue->choice.charstring.buf,
					self->authentication->value.password.string,
					strlen(self->authentication->value.password.string)) != 0)
				return false;

			return true;
		}
	}

	return false;
}

static bool
checkAuthentication(AcseConnection* self, AARQapdu_t* aarq)
{
	if (self->authentication != NULL) {
		if (!checkAuthMechanismName(self, aarq))
			return false;

		return checkAuthenticationValue(self, aarq);
	}
	else
		return true;
}

static AcseIndication
parseAarePdu(AcseConnection* self, AAREapdu_t* aare)
{
	Myexternal_t* external;
	uint8_t* userDataBuffer;
	int userDataBufferSize;

	if (aare->result == Associateresult_accepted) {
		if (aare->userinformation->list.count != 1)
			return ACSE_ERROR;

		external = aare->userinformation->list.array[0];

		if (external->encoding.present != encoding_PR_singleASN1type)
			return ACSE_ERROR;

		userDataBuffer = external->encoding.choice.singleASN1type.buf;
		userDataBufferSize = external->encoding.choice.singleASN1type.size;

		self->userDataBufferSize = userDataBufferSize;

		if (self->userDataBuffer != NULL)
			free(self->userDataBuffer);

		self->userDataBuffer = malloc(userDataBufferSize);
		memcpy(self->userDataBuffer, userDataBuffer, userDataBufferSize);

		return ACSE_ASSOCIATE;
	}

	return ACSE_ASSOCIATE_FAILED;
}

static AcseIndication
parseAarqPdu(AcseConnection* self, AARQapdu_t* aarq)
{
	if (checkAuthentication(self, aarq) == false)
		return ACSE_ASSOCIATE_FAILED;

	if (aarq->userinformation->list.count == 1) {
		Myexternal_t* external = aarq->userinformation->list.array[0];

		if (external->indirectreference == 0)
			return ACSE_ERROR;
		else {
			if (external->encoding.present != encoding_PR_singleASN1type)
				return ACSE_ERROR;
			else {
				uint8_t* userDataBuffer = external->encoding.choice.singleASN1type.buf;
				int userDataBufferSize = external->encoding.choice.singleASN1type.size;

				self->userDataBufferSize = userDataBufferSize;

				if (self->userDataBuffer != NULL)
					free(self->userDataBuffer);

				self->userDataBuffer = malloc(userDataBufferSize);
				memcpy(self->userDataBuffer, userDataBuffer, userDataBufferSize);
			}

			self->nextReference = *(external->indirectreference);
		}

		return ACSE_ASSOCIATE;
	}
	else
		return ACSE_ERROR;
}

static AcseIndication
parseAcsePdu(AcseConnection* self, ByteBuffer* message)
{
	ACSEapdu_t* acseApdu = 0;
	AcseIndication indication;
	asn_dec_rval_t rval;

	/* Decoder return value  */
	rval = ber_decode(NULL, &asn_DEF_ACSEapdu,
			(void**) &acseApdu, (void*) message->buffer, message->size);

	/* print message XER encoded (XML) */
	if (DEBUG) xer_fprint(stdout, &asn_DEF_ACSEapdu, acseApdu);

	if (acseApdu->present == ACSEapdu_PR_aarq) {
		indication = parseAarqPdu(self, &(acseApdu->choice.aarq));
	}
	else if (acseApdu->present == ACSEapdu_PR_aare) {
		indication = parseAarePdu(self, &(acseApdu->choice.aare));
	}
	else
		indication = ACSE_ERROR;

	asn_DEF_ACSEapdu.free_struct(&asn_DEF_ACSEapdu, acseApdu, 0);

	return indication;
}

void
AcseConnection_init(AcseConnection* self)
{
	self->state = idle;
	self->nextReference = 0;
	self->userDataBuffer = NULL;
	self->userDataBufferSize = 0;
	self->authentication = NULL;
}

void
AcseConnection_setAuthenticationParameter(AcseConnection* self, AcseAuthenticationParameter auth)
{
	self->authentication = auth;
}

void
AcseConnection_destroy(AcseConnection* connection) {
	if (connection->userDataBuffer != NULL)
		free(connection->userDataBuffer);
}

AcseIndication
AcseConnection_parseMessage(AcseConnection* connection, ByteBuffer* message)
{
	return parseAcsePdu(connection, message);
}

void
AcseConnection_createAssociateFailedMessage(AcseConnection* connection, ByteBuffer* writeBuffer)
{
	ACSEapdu_t* pdu = calloc(1, sizeof(ACSEapdu_t));

	pdu->present = ACSEapdu_PR_aare;

	pdu->choice.aare.applicationcontextname.buf = appContextNameMms;
	pdu->choice.aare.applicationcontextname.size = 5;

	pdu->choice.aare.result = Associateresult_rejectedpermanent;

	pdu->choice.aare.resultsourcediagnostic.present =
			Associatesourcediagnostic_PR_acseserviceuser;

	pdu->choice.aare.resultsourcediagnostic.choice.acseserviceuser =
			acseserviceuser_null;

	der_encode(&asn_DEF_ACSEapdu, pdu,
				write_out, (void*) writeBuffer);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_ACSEapdu, pdu);

	/* free data structure */
	free(pdu);
}

AcseIndication
AcseConnection_createAssociateResponseMessage(AcseConnection* connection,
			ByteBuffer* writeBuffer,
			ByteBuffer* payload
			)
{

	asn_enc_rval_t rval;
	Associationinformation_t* assocInfo;
	ACSEapdu_t* pdu = calloc(1, sizeof(ACSEapdu_t));

	pdu->present = ACSEapdu_PR_aare;

	pdu->choice.aare.applicationcontextname.buf = appContextNameMms;
	pdu->choice.aare.applicationcontextname.size = 5;

	pdu->choice.aare.result = Associateresult_accepted;

	pdu->choice.aare.resultsourcediagnostic.present =
			Associatesourcediagnostic_PR_acseserviceuser;

	pdu->choice.aare.resultsourcediagnostic.choice.acseserviceuser =
			acseserviceuser_null;

	assocInfo =
			calloc(1, sizeof(Associationinformation_t));

	assocInfo->list.count = 1;
	assocInfo->list.array = calloc(1, sizeof(struct MyExternal*));
	assocInfo->list.array[0] = calloc(1, sizeof(struct Myexternal));

	assocInfo->list.array[0]->directreference = calloc(1, sizeof(OBJECT_IDENTIFIER_t));
	assocInfo->list.array[0]->directreference->size = 2;
	assocInfo->list.array[0]->directreference->buf = berOid;

	assocInfo->list.array[0]->indirectreference = &(connection->nextReference);

	assocInfo->list.array[0]->encoding.present = encoding_PR_singleASN1type;
	assocInfo->list.array[0]->encoding.choice.singleASN1type.size =
			ByteBuffer_getSize(payload);
	assocInfo->list.array[0]->encoding.choice.singleASN1type.buf =
			ByteBuffer_getBuffer(payload);

	pdu->choice.aare.userinformation = assocInfo;



	rval = der_encode(&asn_DEF_ACSEapdu, pdu,
		            write_out, (void*) writeBuffer);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_ACSEapdu, pdu);

	/* free data structure */
	free(assocInfo->list.array[0]->directreference);
	free(assocInfo->list.array[0]);
	free(assocInfo->list.array);
	free(assocInfo);
	free(pdu);

	return rval.encoded;
}


static void
addAuthenticationParametersToRequest(AcseConnection* self, AARQapdu_t* aarq)
{
	if (self->authentication != NULL) {
		if (self->authentication->mechanism == AUTH_PASSWORD) {
			aarq->mechanismname = calloc(1, sizeof(Mechanismname_t));
			aarq->mechanismname->size = 3;
			aarq->mechanismname->buf = auth_mech_password_oid;

			aarq->senderacserequirements = calloc(1, sizeof(ACSErequirements_t));
			aarq->senderacserequirements->size = 1;
			aarq->senderacserequirements->bits_unused = 6;
			aarq->senderacserequirements->buf = requirements_authentication;

			aarq->callingauthenticationvalue = calloc(1, sizeof(Authenticationvalue_t));
			aarq->callingauthenticationvalue->present = Authenticationvalue_PR_charstring;
			aarq->callingauthenticationvalue->choice.charstring.size =
					strlen(self->authentication->value.password.string);
			aarq->callingauthenticationvalue->choice.charstring.buf =
					self->authentication->value.password.string;
		}
	}
}

static void
freeAuthenticationParametersInRequest(AcseConnection* self, AARQapdu_t* aarq)
{
	if (self->authentication != NULL) {
		if (self->authentication->mechanism == AUTH_PASSWORD) {
			free(aarq->mechanismname);
			free(aarq->senderacserequirements);
			free(aarq->callingauthenticationvalue);
		}
	}
}

void
AcseConnection_createAssociateRequestMessage(AcseConnection* self,
			ByteBuffer* writeBuffer,
			ByteBuffer* payload)
{
	AARQapdu_t* aarq;
	long indirectReference;
	asn_enc_rval_t rval;
	ACSEapdu_t* pdu = calloc(1, sizeof(ACSEapdu_t));

	pdu->present = ACSEapdu_PR_aarq;

	aarq = &(pdu->choice.aarq);

	aarq->applicationcontextname.buf = appContextNameMms;
	aarq->applicationcontextname.size = 5;

	aarq->calledAPtitle = calloc(1, sizeof(APtitle_t));
	aarq->calledAPtitle->present = APtitle_PR_aptitleform2;
	aarq->calledAPtitle->choice.aptitleform2.buf = apTitle_1_1_1_999_1;
	aarq->calledAPtitle->choice.aptitleform2.size = 5;

	aarq->calledAEqualifier = calloc(1, sizeof(AEqualifier_t));
	aarq->calledAEqualifier->present = AEqualifier_PR_aequalifierform2;
	aarq->calledAEqualifier->choice.aequalifierform2 = 12;

	aarq->callingAPtitle = calloc(1, sizeof(APtitle_t));
	aarq->callingAPtitle->present = APtitle_PR_aptitleform2;
	aarq->callingAPtitle->choice.aptitleform2.buf = apTitle_1_1_1_999_1;
	aarq->callingAPtitle->choice.aptitleform2.size = 4; /* only 4 bytes of the title ! */

	aarq->callingAEqualifier = calloc(1, sizeof(AEqualifier_t));
	aarq->callingAEqualifier->present = AEqualifier_PR_aequalifierform2;
	aarq->callingAEqualifier->choice.aequalifierform2 = 12;

	aarq->userinformation = calloc(1, sizeof(Associationinformation_t));
	aarq->userinformation->list.count = 1;
	aarq->userinformation->list.array = calloc(1, sizeof(Myexternal_t*));

	aarq->userinformation->list.array[0] = calloc(1, sizeof(Myexternal_t));
	aarq->userinformation->list.array[0]->directreference =
			calloc(1, sizeof(OBJECT_IDENTIFIER_t));
	aarq->userinformation->list.array[0]->directreference->buf = berOid;
	aarq->userinformation->list.array[0]->directreference->size = 2;


	indirectReference = 3;
	aarq->userinformation->list.array[0]->indirectreference = &indirectReference;

	aarq->userinformation->list.array[0]->encoding.present = encoding_PR_singleASN1type;
	aarq->userinformation->list.array[0]->encoding.choice.singleASN1type.size
		= ByteBuffer_getSize(payload);
	aarq->userinformation->list.array[0]->encoding.choice.singleASN1type.buf
		= ByteBuffer_getBuffer(payload);

	addAuthenticationParametersToRequest(self, aarq);

	rval = der_encode(&asn_DEF_ACSEapdu, pdu,
					write_out, (void*) writeBuffer);

	if (DEBUG) xer_fprint(stdout, &asn_DEF_ACSEapdu, pdu);

	/* free data structure */
	freeAuthenticationParametersInRequest(self, aarq);
	free(aarq->userinformation->list.array[0]->directreference);
	free(aarq->userinformation->list.array[0]);
	free(aarq->userinformation->list.array);
	free(aarq->userinformation);
	free(aarq->callingAEqualifier);
	free(aarq->callingAPtitle);
	free(aarq->calledAEqualifier);
	free(aarq->calledAPtitle);
	free(pdu);
}

