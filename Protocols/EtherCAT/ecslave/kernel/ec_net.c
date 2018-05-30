#include "xgeneral.h"
#include "globals.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_device.h"
#include "fsm_slave.h"
#include "ec_sii.h"
#include "ec_regs.h"
#include "ec_net.h"
#include "ec_process_data.h"
#include "ec_com.h"
#include "ec_debug.h"
#include <linux/netdevice.h>

int ec_mac_equal(const uint8_t * mac1, /**< First MAC address. */
		 const uint8_t * mac2 /**< Second MAC address. */
    		)
{
	unsigned int i;

	for (i = 0; i < ETH_ALEN; i++)
		if (mac1[i] != mac2[i])
			return 0;

	return 1;
}

struct net_device *ec_find_dev_by_mac(const uint8_t *mac)
{
 	char *endp;
	int i = 0;
	int j = 0;
	int len = strlen(mac);
	struct net_device *netdev;
	uint8_t tempmac[ETH_ALEN];
	
	for ( ; j <  len; i++){
		if ( j + 2 >= len )
			 j = len-2;
		endp = (char *)&mac[j+2]; 
		tempmac[i] = simple_strtol( &mac[j], &endp, 16);
		j += 3;
	}

	for_each_netdev(&init_net, netdev) {
		if (ec_mac_equal(tempmac, netdev->dev_addr)) {
			return netdev;
		}
	}
	return NULL;
}

/*
 * scan network device list, find the interfaces by mac and 
 * grab them.
*/
int ec_net_init(ecat_slave * ecs, char *rxmac, char *txmac)
{
	struct net_device *netdev;
	struct ec_device *device;

	if (!rxmac) {
		EC_NODE_ERR(ecs, "Must provide receive interface MAC\n");
		return -1;
	}
	netdev = ec_find_dev_by_mac(rxmac);
	if (!netdev) {
		EC_NODE_ERR(ecs, "Receive mac %s is invalid\n", rxmac);
		return -1;
	}
	device = kmalloc(sizeof(struct ec_device), GFP_KERNEL);
	if (!device) {
		EC_NODE_ERR(ecs, "out of memory for %s\n", rxmac);
		return -1;
	}
	device->processed_skb = 0;
	ec_device_init(device, ecs);
	ec_device_attach(device, netdev);
	ecs->intr[RX_INT_INDEX] = device;
	EC_NODE_INFO(ecs, "Loaded RX listener on device %s\n",netdev->name);
	ecs->interfaces_nr = 1;
	/* closed loop */
	if (!txmac) {
		ecs->intr[TX_INT_INDEX] = ecs->intr[RX_INT_INDEX];
		EC_NODE_INFO(ecs, "Last slave . TX is %s",netdev->name);
		return 0;
	}
	netdev = ec_find_dev_by_mac(txmac);
	if (!netdev) {
		EC_NODE_ERR(ecs, "Transmit interface MAC %s is invalid\n",
			    txmac);
		return -1;
	}
	device = kmalloc(sizeof(struct ec_device), GFP_KERNEL);
	if (!device) {
		EC_NODE_ERR(ecs, "out of memory for %s\n", txmac);
		return -1;
	}
	device->processed_skb = 0;
	ec_device_init(device, ecs);
	ec_device_attach(device, netdev);
	ecs->intr[TX_INT_INDEX] = device;
	ecs->interfaces_nr = 2;
	EC_NODE_INFO(ecs, "Loaded Tx listener on device %s\n", netdev->name);
	return 0;
}

void ec_tx_pkt(	uint8_t *buf __attribute__ ((unused)),
		int size __attribute__ ((unused)), 
		struct ec_device *txdev)
{
	struct ec_device *rxdev = txdev->ecat_node->intr[RX_INT_INDEX];

	ec_device_send(txdev, rxdev->processed_skb);
	rxdev->processed_skb = 0;
}

int ec_is_nic_link_up(ecat_slave * ecs, struct ec_device *intr)
{
	return intr->dev->ethtool_ops->get_link(intr->dev);
}
