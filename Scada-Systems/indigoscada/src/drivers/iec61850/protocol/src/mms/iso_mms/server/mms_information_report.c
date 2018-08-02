/*
 * mms_information_report.c
 *
 */
// Modified by Enscada limited http://www.enscada.com
#include <stdlib.h>

#include "mms_server_internal.h"
#include "mms_common_internal.h"


void
MmsServer_sendInformationReportVarList(MmsServer self, char* domainId, char* itemId, LinkedList values)
{
	int i;
	LinkedList value;
	AccessResult_t** accessResultList;
	int variableCount;
	InformationReport_t* report;
	MmsPdu_t* mmsPdu = calloc(1, sizeof(MmsPdu_t));

	mmsPdu->present = MmsPdu_PR_unconfirmedPDU;
	mmsPdu->choice.unconfirmedPDU.unconfirmedService.present = UnconfirmedService_PR_informationReport;

	report =  &(mmsPdu->choice.unconfirmedPDU.unconfirmedService.choice.informationReport);
	report->variableAccessSpecification.present = VariableAccessSpecification_PR_variableListName;

	if (domainId != NULL) {
		report->variableAccessSpecification.choice.variableListName.present = ObjectName_PR_domainspecific;
		report->variableAccessSpecification.choice.variableListName.choice.domainspecific.domainId.size = strlen(domainId);
		report->variableAccessSpecification.choice.variableListName.choice.domainspecific.domainId.buf = domainId;
		report->variableAccessSpecification.choice.variableListName.choice.domainspecific.itemId.size = strlen(itemId);
		report->variableAccessSpecification.choice.variableListName.choice.domainspecific.itemId.buf = itemId;
	}
	else {
		report->variableAccessSpecification.choice.variableListName.present = ObjectName_PR_vmdspecific;
		report->variableAccessSpecification.choice.variableListName.choice.vmdspecific.size = strlen(itemId);
		report->variableAccessSpecification.choice.variableListName.choice.vmdspecific.buf = itemId;
	}

	variableCount = LinkedList_size(values);

	accessResultList =
		mmsMsg_createAccessResultsList(mmsPdu, variableCount);

	/* iterate values list and add values to the accessResultList */
	value = LinkedList_getNext(values);

	for (i = 0; i < variableCount; i++) {
		AccessResult_t* accessResult = accessResultList[i];

		mmsMsg_addResultToResultList(accessResult, (MmsValue*) value->data);

		value = LinkedList_getNext(value);
	}

	report->listOfAccessResult.list.array = accessResultList;
	report->listOfAccessResult.list.size = variableCount;
	report->listOfAccessResult.list.count = variableCount;
}
