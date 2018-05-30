#include <Arduino.h>
#include "w5100.h"

#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>

#include "xgeneral.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_device.h"
#include "fsm_slave.h"
#include "ec_sii.h"
#include "ec_regs.h"
#include "ec_net.h"
#include "ec_cmd.h"
#include "ec_com.h"


static byte mac[6] = { 0x54, 0x55, 0x58, 0x10, 0x00, 0x24 };
#define BUFFER_SIZE 600
uint8_t buf[BUFFER_SIZE];
SOCKET s;

extern "C" int ethernetSetup()
{
	W5100.init();
  	W5100.writeSnMR(s, SnMR::MACRAW); 
  	W5100.execCmdSn(s, Sock_OPEN);
	return 0;
}

void ecat_rcv(ecat_slave  *ecs)
{
	int16_t len;
	uint8_t *ecat;

	len = W5100.getRXReceivedSize(s);
	if (len <= 0){
		return;
	}
	W5100.recv_data_processing(s, buf, len);
	W5100.execCmdSn(s, Sock_RECV);
	ecat = (uint8_t *)&buf[2];

	if (eth_hdr(ecat)->ether_type !=  htons(ETH_P_ECAT)){
		return;
	}
	ec_process_datagrams(ecs, len - 2, ecat);
}

extern "C" void ec_tx_pkt(uint8_t *buf, int sz, struct ec_device *txdev)
{
 	W5100.send_data_processing(s, buf, sz);
 	W5100.execCmdSn(s, Sock_SEND_MAC);
}

