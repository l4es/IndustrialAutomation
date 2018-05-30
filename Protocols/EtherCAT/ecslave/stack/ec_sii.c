#include "../include/xgeneral.h"
#include "ethercattype.h"
#include "fsm_slave.h"
#include "ecs_slave.h"
#include "ec_regs.h"
#include "ec_sii.h"

void write_category_hdr(ecat_slave *esv, int16_t off,int16_t datalen, uint8_t *data)
{
	ec_printf("%s off%d \n",__FUNCTION__,off);
}

void read_category_hdr(ecat_slave* esv,int16_t off,int16_t datalen, uint8_t *data)
{
	int offset = off * 2;
	uint8_t* cat_off = (uint8_t *)&esv->categories;

	ec_printf("%s off %d offset %d data len=%d\n",
			__FUNCTION__,off, offset ,datalen);

	if (offset + datalen > sizeof(esv->categories)){
		ec_printf("%s insane offset offset=%d "
					"datalen=%d sizeof cat %d\n",
				__FUNCTION__,
				offset ,datalen,
				sizeof(esv->categories));
		return;
	}
	memcpy(data, &(cat_off[offset]), datalen);
}

void init_general(ecat_slave *esv, category_general * general,category_header * hdr)
{
	hdr->size = sizeof(*general)/2;

	if (sizeof(*general) %2){
		ec_printf("%s illegal size\n",__FUNCTION__);
		return;
	}

	hdr->type = CAT_TYPE_GENERAL;

	memset(general, 0x00, sizeof(*general));
	esv->last_word_offset = -1;
	general->groupd_idx = GROUP_IDX+1;
	general->img_idx = IMAGE_IDX+1;
	general->order_idx = ORDER_IDX+1;
	general->name_idx = NAME_IDX+1;

	general->coe_details = SDO_ENABLED | SDO_INFO | PDO_ASSIGN
	    | PDO_CONF | STARTUP_UPLOAD | SDO_COMPLETE_ACCESS;

	general->foe_detials = 0;
	general->eoe_detials = 0;
	general->soe_detials = 0;
	general->ds402channels = 0;
	general->sysman_class = 0;
	general->flags = 0;
	general->current_on_bus = 100;
	general->pad_byte1 = 0;
	general->physical_port = (EC_PORT_MII << PORT0_SHIFT);
}
// table 23
void init_syncm(ecat_slave *esv, category_syncm *syncm,int index,category_header * hdr)
{
	hdr->size = ( sizeof(category_syncm)) / 2;
	if (sizeof(*syncm) %2){
		printf("ilegal size\n");
		return;
	}

	hdr->type = CAT_TYPE_SYNCM;
	syncm->length = SYNCM_SIZE;
	syncm->ctrl_reg = 0b00110010;
	syncm->status_reg = 0b00001000; /*b1000 - 1-buf written,b0000 1-buf read */
	syncm->enable_syncm = 0b01;
	if (index % 2){
		syncm->syncm_type = 0x04;
		syncm->ctrl_reg  |= 0x04;
		syncm->phys_start_address = (uint16_t)esv->categories.sii.std_rx_mailbox_offset; 
	}else{
		syncm->phys_start_address = (uint16_t)esv->categories.sii.std_tx_mailbox_offset; 
		syncm->syncm_type = 0x03; /* 0x03 = out*/
	}
}

void toggle_rw_bit(category_syncm *syncm)
{
	if (syncm->status_reg & 0b1000) {
		syncm->status_reg = 0b0000;
	} else{
		syncm->status_reg = 0b1000;
	}
}

void ec_sii_syncm(ecat_slave* esv,int reg, uint8_t* data, int datalen)
{
	category_syncm *syncm;
	int i = 0;

	while (datalen) { 
		switch(reg)
		{
		case ECT_REG_SM0:
			syncm = &esv->categories.syncm0;
			break;
		case ECT_REG_SM1:
			syncm = &esv->categories.syncm1;
			break;
		default:
			printf("no sync managet %d exists\n",reg);
			return;
		}
		toggle_rw_bit(syncm);
		memcpy(data + i, syncm, sizeof(*syncm));
		reg += sizeof(*syncm);
		i += sizeof(*syncm);
		datalen -= sizeof(*syncm);
	}
}

void init_fmmu(category_fmmu *fmmu,category_header *hdr)
{
	hdr->size = sizeof(category_fmmu) / 2;
	if (sizeof(*fmmu) %2){
		ec_printf("ilegal size\n");
		return;
	}

	hdr->type = CAT_TYPE_FMMU;
	fmmu->fmmu0 = 0x1; // outputs
	fmmu->fmmu1 = 0x2; // inputs
}

void init_end_hdr(category_header * hdr)
{
	hdr->size = 0;
	hdr->type = 0x7FFF;
	hdr->vendor_specific = 0x1; // etherlab does not care for vendor
}

void init_strings(category_strings * str, category_header * hdr)
{
	hdr->size = sizeof(*str) / 2;

	if (sizeof(*str) % 2){
		ec_printf("%s ilegal size %zd %zd\n",
			__FUNCTION__,
			sizeof(*str),
			STRINGS_SIZE);
		return;
	}

	str->nr_strings = NR_STRINGS;
	hdr->type = CAT_TYPE_STRINGS;

	str->str0_len = __SIZEOF__(STRING0);
	strcpy(str->str0, STRING0);

	str->str1_len = __SIZEOF__(STRING1);
	strcpy(str->str1, STRING1);

	str->str2_len =  __SIZEOF__(STRING2);
	strcpy(str->str2, STRING2);

	str->str3_len =  __SIZEOF__(STRING3);
	strcpy(str->str3, STRING3);

	str->str4_len =  __SIZEOF__(STRING4);
	strcpy(str->str4, STRING4);

	str->str5_len = __SIZEOF__(STRING5);
	strcpy(str->str5, STRING5);

	str->str6_len =__SIZEOF__(STRING6);
	strcpy(str->str6, STRING6);

	str->str7_len = __SIZEOF__(STRING7);
	strcpy(str->str7, STRING7);

	str->str8_len = __SIZEOF__(STRING8);
	strcpy(str->str8, STRING8);

	str->str9_len =__SIZEOF__(STRING9);
	strcpy(str->str9, STRING9);
}

void init_pdo(pdo_entry * pdo,
	      uint16_t index,
	      uint8_t subindex,
	      uint8_t name_idx,
	      uint8_t data_type, uint8_t bit_len, uint16_t flags)
{
	pdo->bit_len = bit_len;
	pdo->data_type = data_type;
	pdo->flags = flags;
	pdo->index = index;
	pdo->name_idx = name_idx;
	pdo->subindex = subindex;
}

void init_si_info(ec_sii_t *sii)
{
	memset(sii,0x00,sizeof(*sii));

	sii->alias = 0x00;
	sii->vendor_id = 0x017;
	sii->product_code = 0x04;
	sii->revision_number = 0x12;
	sii->serial_number = 0x45;

	sii->boot_rx_mailbox_offset = __mbox_start() ; 
	sii->boot_rx_mailbox_size = MBOX_SIZE ; 
	sii->boot_tx_mailbox_offset = sii->boot_rx_mailbox_offset + MBOX_SIZE;
	sii->boot_tx_mailbox_size  = MBOX_SIZE; 
	sii->std_rx_mailbox_offset = sii->boot_tx_mailbox_offset + MBOX_SIZE;
	sii->std_rx_mailbox_size = MBOX_SIZE;
	sii->std_tx_mailbox_offset = sii->std_rx_mailbox_offset + MBOX_SIZE;
	sii->std_tx_mailbox_size =  MBOX_SIZE;
	sii->mailbox_protocols = EC_MBOX_COE;
}

/* slave information interface */
void init_sii(ecat_slave *esv)
{
	int pdoe_idx = 0;
	
	memset(&esv->categories,0,sizeof(struct __sii_categories__));
	init_si_info(&esv->categories.sii);
	init_strings(&esv->categories.strings, &esv->categories.strings_hdr);
	init_fmmu(&esv->categories.fmmu, &esv->categories.fmmu_hdr);
	init_syncm(esv, &esv->categories.syncm0, 0 ,&esv->categories.syncm_hdr0);
	init_syncm(esv, &esv->categories.syncm1, 1 ,&esv->categories.syncm_hdr1);
	init_general(esv, &esv->categories.general, &esv->categories.general_hdr);
	init_end_hdr(&esv->categories.endhdr);
	
	// pdos
	esv->categories.rxpdo_hdr.type = CAT_TYPE_RXPDO;
	esv->categories.rxpdo_hdr.size = sizeof(esv->categories.rxpdo)/2;
	esv->categories.rxpdo.entries = 2;
	esv->categories.rxpdo.flags = 0;
	esv->categories.rxpdo.name_idx = RXPDO_CAT_NAME_IDX + 1;
	esv->categories.rxpdo.synchronization = 0;
	esv->categories.rxpdo.syncm = 0;
	esv->categories.rxpdo.pdo_index = 0x1600;
	
	init_pdo(&esv->categories.rxpdo.pdo[0], 0x1600, 0X02, RX_PDO1_NAME_IDX + 1, 0, 8, 0);
	esv->pdoe_sizes[pdoe_idx++] = 8;
	init_pdo(&esv->categories.rxpdo.pdo[1], 0x1600, 0X01, RX_PDO2_NAME_IDX + 1, 0, 32, 0);
	esv->pdoe_sizes[pdoe_idx++]  = 32;

	esv->categories.txpdo_hdr.type = CAT_TYPE_TXPDO;
	esv->categories.txpdo.entries = 2;
	esv->categories.txpdo.flags = 0;
	esv->categories.txpdo.name_idx = TXPDO_CAT_NAME_IDX  +1;
	esv->categories.txpdo.synchronization = 0;
	esv->categories.txpdo.syncm = 1;
	esv->categories.txpdo.pdo_index = 0x1a00;
	esv->categories.txpdo_hdr.size = sizeof(esv->categories.txpdo)/2;

	init_pdo(&esv->categories.txpdo.pdo[0], 0x1a00, 0X02, TX_PDO1_NAME_IDX + 1, 0, 32, 0);
	esv->pdoe_sizes[pdoe_idx++]  = 32;
	init_pdo(&esv->categories.txpdo.pdo[1], 0x1a00, 0X01, TX_PDO2_NAME_IDX + 1, 0, 16, 0);
	esv->pdoe_sizes[pdoe_idx++]  = 16;
}


void ec_sii_rw(ecat_slave* esv, uint8_t * data, int datalen)
{
	if (esv->sii_command){
		ec_printf("%s datalen =%d\n",__FUNCTION__,datalen);
		esv->sii_command(esv, esv->last_word_offset, datalen - 6, (uint8_t *)&data[6]);
	} else{
		ec_printf("%s no command\n",__FUNCTION__);
	}
	esv->sii_command = 0;
	esv->last_word_offset = -1;
}

int ec_sii_start_read(ecat_slave* esv,uint8_t * data, int datalen)
{
	int16_t word_offset;

	ec_printf("%s received data len %d\n",
			__FUNCTION__, datalen);

	if (data[0] != 0x80 && data[0] != 0x81) {
		printf("%s no two addressed octets %x %x\n",
		       __FUNCTION__	, data[0], data[1]);
		return 1;
	}
	word_offset = *(int16_t *) & data[2];
	ec_printf("%s request %d operation on offset %d\n",
			__FUNCTION__, data[1], word_offset);

	switch(data[1])
	{
	case 0x01: // read
		esv->last_word_offset = word_offset;
		esv->sii_command = read_category_hdr;
		break;

	case 0x02: // write
		esv->last_word_offset = word_offset;
		esv->sii_command = write_category_hdr;
		break;

	default: // unknown
		printf("%s default\n",__FUNCTION__);
		break;
	}

	return 0;
}

int ec_sii_pdoes_sizes(ecat_slave *ecs)
{
	int i = 0;
	int size_bits = 0;

	for (; i < TOT_PDOS; i++){
		size_bits += ecs->pdoe_sizes[i];
	}
	return size_bits>>3;
}

