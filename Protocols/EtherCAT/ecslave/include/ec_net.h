#ifndef __EC_NET_H__
#define __EC_NET_H__

struct ec_device;
struct __ecat_slave__;

int ec_is_nic_link_up(struct __ecat_slave__ *,struct ec_device *);
struct __ec_interface__ * ec_tx_interface(struct __ecat_slave__ *);
struct __ec_interface__ * ec_rx_interface(struct __ecat_slave__ *);
int ec_net_init(struct __ecat_slave__ *,char *txdev,char *rxdev);

/* is this last slave */
static inline int ec_is_nic_loop_closed(struct __ecat_slave__ * esv)
{
	return esv->intr[TX_INT_INDEX] == esv->intr[RX_INT_INDEX];
}

static inline int ec_is_nic_signal_detected(ecat_slave * esv, struct ec_device *intr)
{
	return ec_is_nic_link_up(esv, intr);
}

#endif
