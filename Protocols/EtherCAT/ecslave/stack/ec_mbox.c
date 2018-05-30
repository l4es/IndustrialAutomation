#include "../include/xgeneral.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_coe.h"
#include "ec_mbox.h"

void mbox_set_state(ecat_slave* ecs,
	void (*state)(ecat_slave*, uint8_t* data,int datalen))
{
	ecs->mbox.state = state;
}

void ec_mbox(ecat_slave* ecs, int reg, uint8_t * data, int datalen)
{
	mbox_header *mbxhdr = __mbox_hdr(data);
	
	if (mbxhdr->type ==  MBOX_COE_TYPE){
		coe_parser(ecs, reg, data, datalen);
		mbxhdr->cnt++;
		return;
	}
	if (ecs->mbox.state) {
		ecs->mbox.state(ecs, data, datalen);
	}

}
