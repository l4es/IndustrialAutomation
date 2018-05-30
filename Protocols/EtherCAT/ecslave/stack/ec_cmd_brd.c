#include "../include/xgeneral.h"
#include "ethercattype.h"
#include "fsm_slave.h"
#include "ec_regs.h"

void ec_cmd_brd(ecat_slave *ecs, uint8_t *dgram_ec)
{
	uint16_t ado;
	uint16_t adp;
	uint16_t datalen = __ec_dgram_dlength(dgram_ec);
	uint8_t *data    = __ec_dgram_data(dgram_ec);

	ado = __ec_dgram_ado(dgram_ec);
	ec_printf("%s data len=%d index=%x ado=0x%x adp=0x%x\n",
	       __FUNCTION__,
	       datalen, __ec_dgram_pkt_index(dgram_ec),ado,  __ec_dgram_adp(dgram_ec));
	/* should bitwise data */
	ec_get_ado(ecs, ado, data, datalen);
	
	__ec_inc_wkc__(dgram_ec);
	adp = __ec_dgram_adp(dgram_ec);
	adp++; /* every slave increment adp */
	__ec_dgram_set_adp(dgram_ec, adp);
	__set_fsm_state(ecs, ecs_process_next_dgram);
}

void ec_cmd_nop(ecat_slave * ecs,uint8_t* dgram_ec)
{
	__set_fsm_state(ecs, ecs_process_next_dgram);
}
