#include "../include/xgeneral.h"
#include "ethercattype.h"
#include "fsm_slave.h"
#include "ecs_slave.h"
#include "ec_regs.h"

void ec_cmd_bwr(ecat_slave * ecs,uint8_t *dgram_ec)
{
	uint16_t ado;
	uint8_t *data = __ec_dgram_data(dgram_ec);
	uint16_t datalen = __ec_dgram_dlength(dgram_ec);

	( (ec_dgram *) dgram_ec)->adp++;	/* each slave ++ in APWR */
	__ec_inc_wkc__(dgram_ec);

	ado = __ec_dgram_ado(dgram_ec);
	ec_set_ado(ecs, ado, data, datalen);
	__set_fsm_state(ecs, ecs_process_next_dgram);
}
