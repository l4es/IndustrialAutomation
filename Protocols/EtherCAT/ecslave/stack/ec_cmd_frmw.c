#include "../include/xgeneral.h"
#include "ethercattype.h"
#include "fsm_slave.h"
#include "ecs_slave.h"
#include "ec_regs.h"
#include "ec_sii.h"

/** Configured Address Read mutltiple write */
void ec_cmd_frmw(ecat_slave *ecs,uint8_t *dgram_ec)
{
	uint16_t ado, adp;
	uint16_t datalen = __ec_dgram_dlength(dgram_ec);
	uint8_t *data = __ec_dgram_data(dgram_ec);

	ado = __ec_dgram_ado(dgram_ec);
	adp = __ec_dgram_adp(dgram_ec);

	ec_printf("%s Index=%x not me adp=%x,%x \n",
		__FUNCTION__,
		__ec_dgram_pkt_index(dgram_ec),
		adp,
		ec_station_address(ecs));

	if (adp != ec_station_address(ecs)) {
		if (ado == ECT_REG_EEPSTAT)
			ec_sii_rw(ecs, data, datalen);
		else
			ec_set_ado(ecs, ado, data, datalen);
	} else{
		ec_get_ado(ecs, ado, data, datalen);
	}
	__ec_inc_wkc__(dgram_ec);
        __set_fsm_state(ecs, ecs_process_next_dgram);
}
