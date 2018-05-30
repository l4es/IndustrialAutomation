#include "../include/xgeneral.h"
#include "ethercattype.h"
#include "fsm_slave.h"
#include "ecs_slave.h"
#include "ec_regs.h"
#include "ec_sii.h"

/** Configured Address Read */
void ec_cmd_aprd(ecat_slave *esv,uint8_t *dgram_ec)
{
	uint16_t ado, adp;
	uint16_t datalen = __ec_dgram_dlength(dgram_ec);
	uint8_t *data = __ec_dgram_data(dgram_ec);

	ado = __ec_dgram_ado(dgram_ec);
	adp = __ec_dgram_adp(dgram_ec);
	if (adp != ec_station_address(esv)) {
		goto FPRD_OUT;
	}
	if (datalen == 0) {
		goto FPRD_OUT;
	}
	__ec_inc_wkc__(dgram_ec);
	ec_get_ado(esv, ado, data, datalen);
	if (ado == ECT_REG_EEPSTAT){
		ec_sii_rw(esv, data, datalen);
	}
FPRD_OUT:
	adp++;
	 __ec_dgram_set_adp(dgram_ec, adp);
        __set_fsm_state(esv, ecs_process_next_dgram);
}
