#include "../include/xgeneral.h"
#include "ethercattype.h"
#include "fsm_slave.h"
#include "ecs_slave.h"
#include "ec_regs.h"
#include "ec_process_data.h"

/** Logical Read Write */
void ec_cmd_lrw(ecat_slave * ecs,uint8_t *dgram_ec)
{
	uint16_t datalen = __ec_dgram_dlength(dgram_ec);
	uint8_t *data	 = __ec_dgram_data(dgram_ec);
	uint32_t offset  = __ec_dgram_laddr(dgram_ec);
	uint8_t temp[datalen];

	normalize_sizes(ecs, &offset, &datalen);
	if (!get_process_data(ecs, temp, offset, datalen)) {
		if (!set_process_data(ecs, data + offset, offset, datalen) )
			memcpy(data + offset, temp, datalen);
	}
       // If LRW is used, output FMMUs increment the working counter by 2,
        // while input FMMUs increment it by 1.
	__ec_inc_wkc__(dgram_ec);
	__ec_inc_wkc__(dgram_ec);
	__ec_inc_wkc__(dgram_ec);
     	__set_fsm_state(ecs, ecs_process_next_dgram);
}
