#include <Arduino.h>
#include "xgeneral.h"
#include "ethercattype.h"
#include "fsm_slave.h"
#include "ecs_slave.h"
#include "ec_sii.h"
#include "ec_regs.h"
#include "ec_net.h"
#include "ec_process_data.h"
#include "ec_com.h"
#include "ec_cmd.h"

struct fsm_slave fsm_slave;
ecat_slave ecs;

void ecat_rcv(ecat_slave  *ecs);

void setup()
{
	int ret;
        Serial.begin(9600);
        Serial.println("Arduino Ethercat slave"); 
 
	ecs.fsm = 0; /* act as flag */
	ret = ecs_net_init(0 , 0, &ecs);
	if (ret < 0) {
		Serial.print("Error init network");
		Serial.println(ret);
		return;
	}
	ret  = ec_init_regs(&ecs) ;
	if (ret < 0){
		Serial.println("Error init registers");
		Serial.println(ret);
		return;
	}

	init_sii(&ecs);

	if (init_process_data(&ecs) < 0) {
		Serial.println("illegal pdo configuration\n");
		return;
	}
	ecs.fsm = &fsm_slave;
	ecs.dgram_processed = 0;
	ecs.dgrams_cnt = 0;
}

void loop()
{
	if (ecs.fsm)
		ecat_rcv(&ecs);
}
