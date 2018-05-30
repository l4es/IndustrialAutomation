/*
 * ec_regs.h
 *
 *  Created on: Oct 17, 2012
 *      Author: raz ben yehuda
 */

#ifndef EC_REGS_H_
#define EC_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

struct __ecat_slave__;

int  	 ec_init_regs(struct __ecat_slave__ *);
void 	 ec_set_ado(struct  __ecat_slave__ *,int reg, uint8_t * data, int datalen);
void 	 ec_get_ado(struct  __ecat_slave__*,int reg, uint8_t * data, int datalen);
uint32_t ecat_cyclic_interval_ns(void);
uint32_t ecat_cyclic_activation(struct  __ecat_slave__*);
uint32_t ecat_cylic_activation_sync0(void);
uint32_t ecat_systime_offset(void);
uint32_t ecat_system_time(void);
uint32_t ecat_cyclic_interval_ns(void);
uint32_t ecat_get_dcstart(int port,struct  __ecat_slave__*);
void 	 ecat_set_dcstart(int port, uint8_t* data, int datalen);
uint32_t ecat_recieve_time(int port);
uint16_t  ec_station_address(struct  __ecat_slave__*);
uint32_t ecat_propagation_delay(struct  __ecat_slave__*);

#ifdef __cplusplus
}
#endif

#endif /* EC_REGS_H_ */
