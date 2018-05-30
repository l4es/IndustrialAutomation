#include "../include/xgeneral.h"
#include "ethercattype.h"
#include "fsm_slave.h"
#include "ecs_slave.h"
#include "ec_regs.h"

/* Broadcat Read Write */
void ec_cmd_brw(ecat_slave * ecs,uint8_t *dgram_ec)
{
	uint16_t ado;
	uint8_t *data = __ec_dgram_data(dgram_ec);
	uint16_t datalen = __ec_dgram_dlength(dgram_ec);
	uint8_t val[datalen];

	( (ec_dgram *) dgram_ec)->adp++;	/* each slave ++ in APWR */

	ado = __ec_dgram_ado(dgram_ec);
	ec_get_ado(ecs, ado, val, datalen);
	ec_set_ado(ecs, ado, data, datalen);
	memcpy(data, val, datalen);

	__ec_inc_wkc__(dgram_ec);
	__ec_inc_wkc__(dgram_ec);
	__ec_inc_wkc__(dgram_ec);
	__set_fsm_state(ecs, ecs_process_next_dgram);
}
