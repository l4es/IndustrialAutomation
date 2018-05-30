#include "xgeneral.h"
#include "globals.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_device.h"
#include "globals.h"
#include "fsm_slave.h"
#include "ec_sii.h"
#include "ec_regs.h"
#include "ec_net.h"
#include "ec_process_data.h"
#include "ec_com.h"
#include "ec_offsched.h"
#include "ecat_protocol.h"

extern void ecat_timer_exit(void);

static struct fsm_slave fsm_slave;
static ecat_slave ecs;
static char *txmac = 0;
static char *rxmac = 0;
static int debug_level = 0;

void ecs_module_cleanup(void)
{
	ecat_proto_cleanup();
	ecat_timer_exit();
}

int ecs_module_init(void)
{
	if (ec_net_init(&ecs, rxmac, txmac) < 0) {
		return -1;
	}
	ec_init_regs(&ecs);
	init_sii(&ecs);
	if (init_process_data(&ecs) < 0) {
		printk("Illegal pdo configuration\n");
		return -1;
	}
	ecs.fsm = &fsm_slave;
	ecs.dgram_processed = 0;
	ecs.dgrams_cnt = 0;
	ecat_proto_init(&ecs);	
	ecat_create_timer();
	return 0;
}

module_init(ecs_module_init);
module_exit(ecs_module_cleanup);

module_param(txmac, charp, 0);
MODULE_PARM_DESC(txmac, "Mac address of the transmssion interface");

module_param(rxmac, charp, 0);
MODULE_PARM_DESC(rxmac, "Mac address of the receive interface");

module_param(debug_level, uint, 0);
MODULE_PARM_DESC(debug_level, "Debug level");

MODULE_DESCRIPTION("ETHERCAT SLAVE");
MODULE_AUTHOR("Raz Ben Jehuda");
MODULE_LICENSE("GPL");
