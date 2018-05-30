#include <linux/module.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/rcupdate.h>
#include <linux/uaccess.h>
#include <linux/net.h>
#include <linux/netdevice.h>
#include <linux/socket.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/skbuff.h>
#include <linux/ratelimit.h>
#include <net/net_namespace.h>
#include <net/sock.h>

#include "xgeneral.h"
#include "globals.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_offsched.h"
#include "ec_net.h"
#include "ec_device.h"
#include "ec_cmd.h"
#include "ecat_netproto.h"

#define ECAT_FORWARD_PKT	1
#define ECAT_PROCESS_PKT	0

ecat_slave *eslave = 0;

/*********************** packet type  start **************************/

static int ecat_pkt_direction(struct sk_buff *skb)
{
	if (skb->dev == eslave->intr[RX_INT_INDEX]->dev)
		return ECAT_PROCESS_PKT;
	if (skb->dev == eslave->intr[TX_INT_INDEX]->dev)
		return ECAT_FORWARD_PKT;
	return -1;
}

static void ecat_process_port_events(struct ec_device *device)
{
	struct ecat_event *ev;
	struct ecat_event *temp;
	unsigned long flags;
	
// better list_splice here. will do that later
	list_for_each_entry_safe(ev, temp, &device->events, list){
		ev->action(ev->__private);
		spin_lock_irqsave(&device->events_sync, flags);
		list_del_init(&ev->list);
		spin_unlock_irqrestore(&device->events_sync, flags);
	}
}

void ecat_process_pkt(struct sk_buff *skb)
{
	struct ec_device *rxintr = eslave->intr[RX_INT_INDEX];
	struct ec_device *txintr = eslave->intr[TX_INT_INDEX];
	int dir;

	dir = ecat_pkt_direction(skb);
	switch(dir)
	{
	case ECAT_FORWARD_PKT:
		ecat_process_port_events(txintr);
		ec_device_send(rxintr, skb);
		rxintr->processed_skb = NULL;
		break;
	
	case ECAT_PROCESS_PKT:
		ecat_process_port_events(rxintr);
		if (rxintr->processed_skb){
			printk("pkt is procssed \n");
			kfree_skb(skb);
			return;
		}
		rxintr->processed_skb = skb;
		ec_process_datagrams(eslave, skb->len, skb->data);
		break;
	default:
		printk("should not be here\n");
	}
}

static int ecat_rcv(struct sk_buff *skb, struct net_device *dev, 
                      struct packet_type *pt, struct net_device *orig_dev) 
{
	if (eth_hdr(skb)->h_proto != htons(ETH_P_ECAT))
		return   NET_RX_DROP;
	if (skb_is_nonlinear(skb)){
		printk("packet split not supported yet\n");
		return   NET_RX_DROP;
	}
	skb_push(skb, sizeof(struct ethhdr));
	ecat_process_pkt(skb);
	return NET_RX_SUCCESS;
}

static struct packet_type ecat_packet __read_mostly = {
        .type = cpu_to_be16(ETH_P_ECAT),
        .dev  = NULL,
        .func = ecat_rcv,
};

/*********************** packet type  here **************************/

int ecat_proto_init(ecat_slave * ecs)
{
	eslave = ecs;
	dev_add_pack(&ecat_packet);
//	ecat_netproto_init();
	return 0;
}

void ecat_proto_cleanup(void)
{
	dev_remove_pack(&ecat_packet);
//	ecat_netproto_exit();
}
