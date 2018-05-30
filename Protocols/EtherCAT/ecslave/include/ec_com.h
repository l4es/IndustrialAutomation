#ifndef __EC_COM_H__
#define __EC_COM_H__

struct __ecat_slave__ ;

void ecs_poll(struct __ecat_slave__  *,uint8_t *d);
void raw_ecs_tx_packet(struct __ecat_slave__ *);
int raw_ecs_rx_packet(struct __ecat_slave__  *);

#endif
