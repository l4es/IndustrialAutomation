#ifndef  __MBOX_HEADER__
#define  __MBOX_HEADER__

typedef struct __mbox_header__ {

	uint16_t len __attribute__packed__;
	uint16_t address __attribute__packed__;
	uint8_t channel:6, prio:2  __attribute__packed__;
	uint8_t type:4, cnt:3, reserved:1 __attribute__packed__;
} mbox_header;

typedef enum {

	COE_EMERGENCY = 0x01,
	COE_SDO_REQUEST = 0x02,
	COE_SDO_RESPONSE = 0x03,
	COE_TX_PDO = 0x04,
	COE_RX_PDO = 0x05,
	COE_TX_PDO_REMOTE = 0x06,
	COE_RX_PDO_REMOTE = 0x07,
	COE_SDO_INFO = 0x08
} coe_service;

typedef struct  __coe_header__ {
	uint16_t number:9, reserved:3, coe_service:4;
} coe_header ;

typedef struct __coe_sdo_info_header__ {
	uint8_t opcode:7,incomplete:1;
	uint8_t reserved;
	uint16_t frag_list __attribute__packed__ ;
	uint8_t sdo_info_service_data[1];
}coe_sdo_info_header ;

static inline coe_header*  __coe_header(uint8_t *data)
{
	return (coe_header *)&data[sizeof(mbox_header)];
}

static inline coe_sdo_info_header * __sdo_info_hdr(uint8_t *data)
{
	return (coe_sdo_info_header *)
		&data[sizeof(mbox_header) + sizeof(coe_header)];
}


static inline mbox_header *__mbox_hdr(uint8_t* data)
{
	return (mbox_header *)&data[0];
}


void mbox_set_state(ecat_slave*,
	void (*state)(ecat_slave*, uint8_t*,int ));

void ec_mbox_syncm(int reg, uint8_t* data, int datalen);
void ec_mbox(ecat_slave* ecs, int reg, uint8_t * data, int datalen);

#endif
