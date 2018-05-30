#include "xgeneral.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_device.h"
#include "fsm_slave.h"
#include "ec_sii.h"
#include "ec_regs.h"
#include "ec_net.h"
#include "ec_process_data.h"
#include "ec_com.h"

void ethernetSetup();
static struct ec_device etherDev[2];

/*
 * scan network device list, find the interfaces by mac and 
 * grab them.
*/
int  ecs_net_init(int __attribute__ ((unused)) dummy,char __attribute__ ((unused)) *argv[], ecat_slave *ecs)
{
	struct ec_device *device;

	device = &etherDev[0];
	ecs->intr[RX_INT_INDEX] = device;
	ecs->interfaces_nr = 1;
	ecs->intr[TX_INT_INDEX] = ecs->intr[RX_INT_INDEX];
        ethernetSetup();
	return 0;
}

int ec_is_nic_link_up(struct __ecat_slave__* __attribute__ ((unused)) dummy,struct ec_device __attribute__ ((unused)) *d)
{ 
	return 1;
}
