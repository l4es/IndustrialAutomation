#include "../include/xgeneral.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_mbox.h"
#include "ec_regs.h"
#include "ec_coe.h"

void od_list_response(ecat_slave *ecs, uint8_t* data,int datalen)
{
	mbox_header *mbxhdr = __mbox_hdr(data);
	coe_header *coehdr = __coe_header(data);
	coe_sdo_info_header * sdoinfo = __sdo_info_hdr(data);
//	coe_sdo_service_data *srvdata = (coe_sdo_service_data *)(data + 6 + 6);
	coe_sdo_service_data *srvdata =
		(coe_sdo_service_data *)&sdoinfo->sdo_info_service_data[0];

	mbxhdr->type =  MBOX_COE_TYPE;
	/* see ec_fsm_coe_dict_response, data pointer is moved 6 bytes  */
	mbxhdr->len = 8 + NR_SDOS * 2; 
	coehdr->coe_service = COE_SDO_INFO;
	sdoinfo->opcode = OD_LIST_RESPONSE;
	srvdata->list_type = 0x1;
	/*
	 * Do the reponse
	*/
	sdoinfo->opcode   = 0x02; // table 43
	srvdata->index[0] = 0x1234;
	srvdata->index[1] = 0x5678;
	srvdata->index[2] = 0x0;;
}

// table 45
void obj_desc_response(ecat_slave* ecs, uint8_t *data, int datalen)
{
	typedef struct {
		uint16_t index __attribute__packed__ ;
		uint16_t data_type __attribute__packed__;
		uint8_t  max_subindex ;
		uint8_t  object_code;
		char     name[1];
	}sdo_info_service_data;

	coe_header *coehdr = __coe_header(data);
	mbox_header *mbxhdr = __mbox_hdr(data);
	coe_sdo_info_header * sdoinfo = __sdo_info_hdr(data);
	sdo_info_service_data *obj_desc =
		(sdo_info_service_data *)&sdoinfo->sdo_info_service_data[0];

	mbxhdr->type =  MBOX_COE_TYPE;
	coehdr->number = 0;
	coehdr->reserved = 0;
	coehdr->coe_service = COE_SDO_INFO;

	sdoinfo->reserved = 0;
	sdoinfo->frag_list = 0;
	sdoinfo->opcode = OBJ_DESC_RESPONSE;

	obj_desc->index = ecs->coe.obj_index;
	obj_desc->data_type = 0x05;
	obj_desc->max_subindex = 4;
	obj_desc->object_code = 7;

	sprintf(obj_desc->name,"LINUX DRIVE SDO 0x%X",obj_desc->index);
	mbxhdr->len = sizeof(*sdoinfo) +
		sizeof(*coehdr) +
		sizeof(*obj_desc) + strlen(obj_desc->name) - 1;
}

// table 44
void obj_desc_request(ecat_slave *ecs, uint8_t *data, int datalen)
{
	typedef struct {
		uint16_t  index  __attribute__packed__; 
	}sdo_info_service_data ;

	coe_sdo_info_header * sdoinfo = __sdo_info_hdr(data);
	sdo_info_service_data *obj_desc =
		(sdo_info_service_data *)&sdoinfo->sdo_info_service_data[0];
	ecs->coe.obj_index = obj_desc->index;
	mbox_set_state(ecs, obj_desc_response);
}

void entry_desc_response(ecat_slave* ecs, uint8_t *data, int datalen)
{	// table 47
	typedef struct {
		uint16_t index;	
		uint8_t  subindex;
		uint8_t  valueinfo;
		uint16_t datatype;
		uint16_t bit_len;
		uint16_t object_access;
		char     name[1];
	}sdo_entry_info_data;

	coe_header *coehdr = __coe_header(data);
	mbox_header *mbxhdr = __mbox_hdr(data);
	coe_sdo_info_header *sdoinfo = __sdo_info_hdr(data);
	sdo_entry_info_data *entry_desc = 
		(sdo_entry_info_data *)&sdoinfo->sdo_info_service_data[0];
	
	coehdr->number 	    = 0;
	coehdr->coe_service =  COE_SDO_INFO;

	sdoinfo->opcode = ENTRY_DESC_RESPONSE;
	sdoinfo->frag_list = 0;

	mbxhdr->type =  MBOX_COE_TYPE;

	entry_desc->valueinfo =  0b1000;
	entry_desc->datatype  = 0x5;
	entry_desc->bit_len   = 8;
	entry_desc->object_access = 0x0FFF;
	entry_desc->index	  = ecs->coe.obj_index;
	entry_desc->subindex 	  = ecs->coe.obj_subindex; 
	sprintf(entry_desc->name,"SDO ENTRY 0x%x:0x%X",
			ecs->coe.obj_index,
			ecs->coe.obj_subindex);

	mbxhdr->len = 	sizeof(*sdoinfo) + 
			sizeof(*coehdr) + 
			sizeof(*entry_desc) + strlen(entry_desc->name) -1;
}

// table 46
void entry_desc_request(ecat_slave* ecs,uint8_t *data, int datalen)
{
	typedef struct {
		uint16_t index  __attribute__packed__;	
		uint8_t  subindex;
		uint8_t  valueinfo;
	}sdo_entry_info_data;

	coe_sdo_info_header * sdoinfo = __sdo_info_hdr(data);
	sdo_entry_info_data *entry_desc = 
		(sdo_entry_info_data *)&sdoinfo->sdo_info_service_data[0];
	ecs->coe.obj_index = entry_desc->index;
	ecs->coe.obj_subindex = entry_desc->subindex;
	mbox_set_state(ecs, entry_desc_response);
}

void od_list_request(ecat_slave* ecs, uint8_t * data, int datalen)
{
	coe_sdo_info_header *sdoinfo = __sdo_info_hdr(data);
	coe_sdo_service_data *srvdata =
		(coe_sdo_service_data *)&sdoinfo->sdo_info_service_data[0];

	srvdata->list_type = 0x1;
	// do the reponse
	sdoinfo->opcode = 0x02; // table 43
	mbox_set_state(ecs, od_list_response);
}

void coe_sdo_info(ecat_slave* ecs, uint8_t * data, int datalen)
{
	coe_sdo_info_header * sdoinfo = __sdo_info_hdr(data);
	
	switch(sdoinfo->opcode)
	{
	case OD_LIST_REQUEST: /* the first thing elab asks for */
	//	puts("OD_LIST_REQUEST:");
		od_list_request(ecs, data, datalen);
		break;
	case OD_LIST_RESPONSE:
	//	puts("OD_LIST_RESPONSE:");
		od_list_response(ecs, data, datalen);
		break;
	case OBJ_DESC_REQUEST:
	//	puts("OBJ_DESC_RESPONSE:");
		obj_desc_request(ecs, data, datalen);
		break;
	case OBJ_DESC_RESPONSE:
	//	puts("OBJ_DESC_RESPONSE");
		break;
	case ENTRY_DESC_REQUEST:
	//	puts("ENTRY_DESC_RESPONSE:");
		entry_desc_request(ecs, data, datalen);
		break;
	case ENTRY_DESC_RESPONSE:
	//	printf("ENTRY_DESC_RESPONSE");
		break;
	case SDO_INFO_ERROR_REQUEST:
	//	printf("SDO_INFO_ERROR_REQUEST");
		break;
	}
}

void coe_parser(ecat_slave* ecs, int reg, uint8_t * data, int datalen)
{
	coe_header *hdr = __coe_header(data);

	if (reg > __sdo_high()){		
		return;
	}
	switch (hdr->coe_service) 
	{
	case COE_EMERGENCY:
		break;
	case COE_SDO_REQUEST:
		break;
	case COE_SDO_RESPONSE:
		break;
	case COE_TX_PDO:
		break;
	case COE_RX_PDO:
		break;
	case COE_TX_PDO_REMOTE:
		break;
	case COE_RX_PDO_REMOTE:
		break;
	case COE_SDO_INFO:
		coe_sdo_info(ecs, data, datalen);
		break;
	}
}
