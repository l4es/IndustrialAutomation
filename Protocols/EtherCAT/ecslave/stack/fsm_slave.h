/*
 * fsm_slave.h
 *
 *  Created on: Oct 14, 2012
 *      Author: root
 */
#ifndef __FSM_SLAVE_H__
#define __FSM_SLAVE_H__

#include "ecs_slave.h"

#ifdef __cplusplus
extern "C" {
#endif

struct fsm_slave {
	void (*state) (ecat_slave *,uint8_t *);
};

static inline void __set_fsm_state(ecat_slave * slave,
	void (*state) (ecat_slave *,uint8_t *))
{
	slave->fsm->state = state;
}

void ecs_rx_packet(ecat_slave *,uint8_t *d);
int ec_capture(void);
void ec_tx_pkt(uint8_t *buf, int size, struct ec_device *);

#ifdef __cplusplus
}
#endif

#endif
