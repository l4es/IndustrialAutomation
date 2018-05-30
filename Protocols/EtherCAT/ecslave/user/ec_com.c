#include "xgeneral.h"
#include "ec_device.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "fsm_slave.h"
#include "ec_sii.h"
#include "ec_regs.h"
#include "ec_net.h"
#include "ec_process_data.h"
#include "ec_com.h"
#include "ec_cmd.h"
#include <sys/prctl.h>
#include <pcap/pcap.h>

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>


static void ec_pkt_filter(u_char *user, const struct pcap_pkthdr *h,
                                   const u_char *bytes);
ecat_slave slaves[MAX_SLAVES];
int slaves_nr = 0;
static pcap_t *rx_handle = 0;

int dbg_index(ecat_slave *ecs)
{
	uint8_t *p =   __ecat_frameheader(ecs->pkt_head) + sizeof(ec_frame_header);
	return __ec_dgram_pkt_index(p);
}

uint16_t ec_dbg_wkc(ecat_slave *ecs)
{
	uint8_t *p =   __ecat_frameheader(ecs->pkt_head) + sizeof(ec_frame_header);
	return __ec_wkc(p);
}

void ec_tx_pkt(uint8_t* buf, int size, struct ec_device *intr)
{
	int i;
	int bytes;
	struct ether_header *eh = (struct ether_header *)buf;
	struct sockaddr_ll socket_address = { 0 };
	// collect the current slave
	ecat_slave* ecs = intr->ecslave;

	intr = slaves[0].intr[TX_INT_INDEX];

	for (i = 0; i < ETH_ALEN ; i++) {
		intr->mac.ether_dhost[i] = 0xFF;
	}
	intr->mac.ether_type = htons(ETHERCAT_TYPE);
	socket_address.sll_family = PF_PACKET;
	socket_address.sll_protocol = ETHERCAT_TYPE;
	socket_address.sll_ifindex = intr->index;
	socket_address.sll_hatype = htons(ETHERCAT_TYPE);
	socket_address.sll_pkttype = PACKET_BROADCAST;
	socket_address.sll_halen = ETH_ALEN;

	memcpy(socket_address.sll_addr, intr->mac.ether_dhost, ETH_ALEN);
	memcpy(eh->ether_shost,
	       &intr->mac.ether_shost,
		sizeof(intr->mac.ether_shost));
	eh->ether_type = htons(ETHERCAT_TYPE);
	if (ecs->index == (slaves_nr - 1)) {
		ecs = &slaves[0];
		intr = ecs->intr[TX_INT_INDEX];
		// 	transmit back the packet without 
		//	pushing it back 
		bytes = sendto(intr->sock,
			       buf, size, 0,
		       		(struct sockaddr *)&socket_address,
		       		(socklen_t) sizeof(socket_address));
		if (bytes < 0) {
			perror("tx packet: ");
		}
		return;
	}

	{
		struct pcap_pkthdr h;
		ecs = &slaves[ecs->index + 1];
		h.len = size;
		ec_pkt_filter((u_char *)ecs, &h, buf);
	}
}

/* we may catch out own transmitted packet */
static int is_outgoing_pkt(ecat_slave *ecs, uint8_t *d)
{
	if (!memcmp(__ec_get_shost(d),
		ecs->intr[RX_INT_INDEX]->mac.ether_shost,
		ETH_ALEN)) {
       		return 1;
	}
	if (!memcmp(__ec_get_shost(d),
		ecs->intr[TX_INT_INDEX]->mac.ether_shost,
		ETH_ALEN)) {
       		return 1;
	}
	return 0;
}

static void ec_pkt_filter(u_char *user, const struct pcap_pkthdr *h,
                                   const u_char *bytes)
{
	ecat_slave *ecs = (ecat_slave *)user;
	uint8_t *d = (uint8_t *)bytes;
	struct ec_device *intr = ecs->intr[TX_INT_INDEX];
	struct ecat_event *ev;

	if (!__ec_is_ethercat(d)){
		return;
	}

	if (is_outgoing_pkt(ecs, d)){
		return;
	}

	pthread_mutex_lock(&intr->events_sync);
	while (LIST_FIRST(&intr->events) != NULL) {
	      	ev = LIST_FIRST(&intr->events);
		ev->action(ev->__private);
		LIST_REMOVE(ev, list);
		ev->action = 0x00;
	}
	pthread_mutex_unlock(&intr->events_sync);
	ec_process_datagrams(ecs, h->len, d);
}

int ec_capture(void)
{
	struct sched_param sp ={0};
	ecat_slave *ecs;
 	char errbuf[PCAP_ERRBUF_SIZE];          /* error buffer */
	int snap_len = 1492;	
	int promisc = 1;
	int timeout_ms = 0;

	ecs = &slaves[0];
	rx_handle = pcap_open_live(ecs->intr[RX_INT_INDEX]->name,
			snap_len, promisc, timeout_ms, errbuf);

	if(!rx_handle){
		puts(errbuf);
		return -1;
	}

	prctl(PR_SET_NAME,"ecslave",0,0,0);
	sp.sched_priority = 90;
	pthread_setschedparam(pthread_self(), SCHED_FIFO, &sp);

	while (1) {
		// works only with the master, recv and transmit
		int num_pkt = 0;
		pcap_loop(rx_handle, num_pkt, ec_pkt_filter ,(u_char *)ecs);
	}
}
