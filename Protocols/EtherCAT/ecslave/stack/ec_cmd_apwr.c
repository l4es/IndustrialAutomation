/* * ec_cmd_apwr.c
 *
 *  Created on: Oct 18, 2012
 *      Author: raz ben yehuda <raziebe@gmail.com>
 */
#include "../include/xgeneral.h"
#include "ethercattype.h"
#include "fsm_slave.h"
#include "ecs_slave.h"
#include "ec_regs.h"

 /** Auto Increment Write. by ring position */
void ec_cmd_apwr(ecat_slave *ecs, uint8_t *dgram_ec)
{
	uint16_t adp;
	uint16_t ado;
	uint8_t *data = __ec_dgram_data(dgram_ec);

	adp = __ec_dgram_adp(dgram_ec);
	ado = __ec_dgram_ado(dgram_ec);
	if(adp != 0)
		goto APWR_OUT; 
	{
		uint16_t datalen = __ec_dgram_dlength(dgram_ec);
		uint8_t val[datalen];

		ec_get_ado(ecs, ado, &val[0], datalen);
		ec_set_ado(ecs, ado, data, datalen);
		memcpy(data, &val, datalen);
	}
	__ec_inc_wkc__(dgram_ec);
APWR_OUT:
	( (ec_dgram *) dgram_ec)->adp++;	/* each slave ++ in APWR */
	__set_fsm_state(ecs, ecs_process_next_dgram);
}
