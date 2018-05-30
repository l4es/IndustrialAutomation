#include "xgeneral.h"
#include "ethercattype.h"
#include "fsm_slave.h"
#include "ecs_slave.h"
#include "ec_sii.h"
#include "ec_regs.h"
#include "ec_net.h"
#include "ec_process_data.h"
#include "ec_com.h"
#include "ec_device.h"

extern int slaves_nr;
extern ecat_slave slaves[];

int init(int argc, char *argv[])
{
	int i = 0;
	struct ec_device *intr;
	ecat_slave * esv;
	
	slaves_nr = atoi(argv[2]);
	if (slaves_nr < 1 || slaves_nr > MAX_SLAVES){
		printf("ilegal nymber of slaves %d\n",slaves_nr);		
		return -1;
	}

	esv  =  &slaves[0];
	intr =  esv->intr[RX_INT_INDEX] = malloc(sizeof(struct ec_device));
	strncpy(intr->name, argv[1], sizeof(intr->name));
	esv->intr[TX_INT_INDEX] = esv->intr[RX_INT_INDEX];
	esv->interfaces_nr = 2;
	intr->ecslave = esv;

	if (ecs_sock(intr)) {
		return -1;
	}

	ec_init_regs(esv);
	init_sii(esv);
	if (ecs_get_intr_conf(intr)) {
		return -1;
	}

	printf("LINK %d %s  %s\n", i, intr->name,
			  ec_is_nic_link_up(esv, intr) ? "UP" : "DOWN");
	ec_init_device(intr);

	if (init_process_data(esv) < 0){
		printf ("illegal pdo configuration\n");
		return -1;
	}

	esv->fsm = malloc(sizeof(struct fsm_slave));
	esv->dgram_processed = &esv->pkt_head[0];
	esv->dgrams_cnt = 0;

	for (i = 1; i < slaves_nr - 1; i++) {
		esv = &slaves[i];
		esv->interfaces_nr = 2;
		esv->intr[0] =  malloc(sizeof(struct ec_device));
		memset(esv->intr[0], 0x00,sizeof(struct ec_device));
		ec_init_device(esv->intr[0]);
		esv->intr[0]->ecslave = esv;

		esv->intr[1] = malloc(sizeof(struct ec_device));
		memset(esv->intr[1], 0x00,sizeof(struct ec_device));
		ec_init_device(esv->intr[1]);
		esv->intr[1]->ecslave = esv;
		esv->index = i;
		ec_init_regs(esv);
		init_sii(esv);

		esv->fsm = malloc(sizeof(struct fsm_slave));
		esv->dgram_processed = &esv->pkt_head[0];
		esv->dgrams_cnt = 0;

		if (init_process_data(esv) < 0){
			printf ("%s illegal pdo configuration\n",argv[0]);
			return -1;
		}

	}
	// last slave
	esv = &slaves[i];
	esv->interfaces_nr = 1;
	esv->intr[0] = malloc(sizeof(struct ec_device));
	memset(esv->intr[0], 0x00,sizeof(struct ec_device));
	ec_init_device(esv->intr[0]);
	esv->intr[1] = esv->intr[0];
	esv->intr[0]->ecslave = esv;
	ec_init_regs(esv);
	init_sii(esv);
	esv->index = i;
	esv->fsm = malloc(sizeof(struct fsm_slave));
	esv->dgram_processed = &esv->pkt_head[0];
	esv->dgrams_cnt = 0;
	if (init_process_data(esv) < 0){
		printf ("%s illegal pdo configuration\n",argv[0]);
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		/*
		 * if you provide two different interfaces 
		 *  then it is considered an open loop,
 		  * else it is a closed loop ,ie, last slave.
		*/
		printf("%s <rx interface> <number of slaves>\n", argv[0]);
		return 0;
	}

  	if (init(argc, argv) < 0){
		return -1;
	}

	ecat_create_timer();
	ec_capture();
	return 0;
}

