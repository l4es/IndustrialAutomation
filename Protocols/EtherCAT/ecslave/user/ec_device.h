#ifndef __EC_DEVICE_H__
#define __EC_DEVICE_H__

#include "ecat_timer.h"

#define MAX_SLAVES	128

struct ec_device {
	pthread_mutex_t events_sync;
        LIST_HEAD(porteve, ecat_event) events;
 	struct ecat_event rx_time;
	int index;
	struct ether_header mac;
	char name[16];
	char ip[32];
	char macaddr[32];
	int sock;
	int subnet_mask;
	int link_up;
	void *ecslave;
};

int ecs_sock(struct ec_device * intr);
int ecs_get_intr_conf(struct ec_device * intr);
void ec_init_device(struct ec_device *ec);
void ecat_add_event_to_device(struct ec_device *ec, 
		struct ecat_event* ev,
		void (*action)(void *), 
		void *private);
#endif
