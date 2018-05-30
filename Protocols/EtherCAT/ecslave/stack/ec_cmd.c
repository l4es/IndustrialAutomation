#include "../include/xgeneral.h"
#include "ethercattype.h"
#include "fsm_slave.h"
#include "ecs_slave.h"
#include "ec_sii.h"
#include "ec_regs.h"
#include "ec_process_data.h"
#include "ec_com.h"
#include "ec_net.h"
#include "ec_cmd.h"


#define  WORKING_CNT_SIZE 2

void ecs_process_next_dgram(ecat_slave * ecs,uint8_t *d)
{
	if (--ecs->dgrams_cnt) {
		/* move to next packet */
		ecs->dgram_processed += 
			WORKING_CNT_SIZE + 
			sizeof(ec_dgram) + __ec_dgram_dlength(d);
		return __set_fsm_state(ecs, ecs_process_cmd);
	}
	/* pass packet back to next slave */
	ec_tx_pkt(ecs->pkt_head, ecs->pkt_size, ecs->intr[TX_INT_INDEX]);
	 __set_fsm_state(ecs, NULL); /* move to next interface in ec_poll */
}

int  ec_nr_dgrams(uint8_t *raw_pkt)
{
	int i = 0;
	int frame_size = __ec_frame_size(raw_pkt);
	uint8_t* dgram  = __ecat_frameheader(raw_pkt) +  sizeof(ec_frame_header);
		
	for (;frame_size > 0;i++){
		frame_size -= 
			(sizeof(ec_dgram) + 
			__ec_dgram_dlength(dgram)
			 + WORKING_CNT_SIZE) ;
		dgram += sizeof(ec_dgram) + WORKING_CNT_SIZE + __ec_dgram_dlength(dgram); 		
	}
	if (frame_size < 0){
		return 0;
	}
	return i;
}

void ecs_process_cmd(ecat_slave * ecs, uint8_t *dgram_ec)
{
	__set_fsm_state(ecs, ec_cmd_nop);
	
	switch (__ec_dgram_command(dgram_ec)) {
	case EC_CMD_APRD:
		__set_fsm_state(ecs,ec_cmd_aprd);
		break;

	case EC_CMD_APWR:
		__set_fsm_state(ecs, ec_cmd_apwr);
		break;

	case EC_CMD_APRW:
		__set_fsm_state(ecs, ec_cmd_aprw);
		break;

	case EC_CMD_FPRD:
		__set_fsm_state(ecs, ec_cmd_fprd);
		break;

	case EC_CMD_FPWR:
		__set_fsm_state(ecs, ec_cmd_fpwr);
		break;

	case EC_CMD_FPRW:
		__set_fsm_state(ecs, ec_cmd_fprw);
		break;

	case EC_CMD_BRD:
		__set_fsm_state(ecs, ec_cmd_brd);
		break;

	case EC_CMD_BWR:
		__set_fsm_state(ecs, ec_cmd_bwr);
		break;

	case EC_CMD_BRW:
		__set_fsm_state(ecs, ec_cmd_brw);
		break;

	case EC_CMD_LRD:
		__set_fsm_state(ecs, ec_cmd_lrd);
		break;

	case EC_CMD_LWR:
		__set_fsm_state(ecs, ec_cmd_lwr);
		break;

	case EC_CMD_LRW:
		__set_fsm_state(ecs, ec_cmd_lrw);
		break;

	case EC_CMD_ARMW:
		__set_fsm_state(ecs, ec_cmd_armw);
		break;

	case EC_CMD_FRMW:
		__set_fsm_state(ecs, ec_cmd_frmw);
		break;
	//default:
		//ec_printf("unknown command %d\n",__ec_dgram_command(dgram_ec));
	}
	ecs->fsm->state(ecs, dgram_ec);
}

void ec_process_datagrams(ecat_slave *ecs,int len, uint8_t *dgram_ec)
{
	ecs->pkt_head = dgram_ec;
	ecs->pkt_size = len;
	// grab first ecat dgram
	ecs->dgram_processed =  __ecat_frameheader(ecs->pkt_head) + sizeof(ec_frame_header);
	ecs->dgrams_cnt = ec_nr_dgrams(ecs->pkt_head);
	__set_fsm_state(ecs, ecs_process_cmd);
	while (ecs->fsm->state) {
		ecs->fsm->state(ecs, ecs->dgram_processed);
	}
}
