#include <linux/module.h>

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <linux/in.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/if_ether.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/route.h>
#include <linux/inet.h>
#include <linux/etherdevice.h>
#include <linux/if_arp.h>
#include <linux/wireless.h>
#include <linux/skbuff.h>
#include <linux/udp.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <net/sock.h>
#include <net/inet_common.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/if_ec.h>
#include <net/ip.h>
#include <linux/spinlock.h>
#include <linux/rcupdate.h>
#include <linux/bitops.h>
#include <linux/mutex.h>

#include <linux/uaccess.h>
#include <asm/system.h>

#include "xgeneral.h"
#include "globals.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_offsched.h"
#include "ec_net.h"
#include "ec_device.h"
#include "ec_cmd.h"
#include "ecat_netproto.h"


/*********************** net proto  start **************************/
#define PF_ETHERCAT PF_ECONET

struct ecat_sock {
	struct ec_slave*  ecslave;
	struct ecat_event timer_event; /* registers and event on the timer */
	struct ecat_event port_event;
	struct semaphore  recv_sem;
	struct sk_buff*   skb;
	char  *userbuf;
};

static void sock_port_action(long *private)
{
	struct sk_buff* skb;
	struct ecat_sock *ecatsock =(struct ecat_sock *)private;
	struct ec_slave *ecslave = (ecat_slave *)ecatsock->ecslave;
	struct ec_device *device = ecslave->intr[RX_INT_INDEX];

	skb = device->process_skb;
	if (skb) {
		skb_get(skb);
		/* we save last skb. */
		ecatsock->skb = skb;
		up(&ecatsock->recv_sem);
	}
}

/*
 * this is a call back from the timer.
*/
static void sock_timer_action(long *private)
{
	struct ecat_sock *ecatsock = (struct ecat_sock *)private;
}

static inline struct ecat_sock *ecat_sk(const struct sock *sk)
{
        return (struct ecat_sock *)sk;
}

static const struct proto_ops ecat_ops;

static struct proto ecat_proto = {
	.name	  = "ETHERCAT",
	.owner	  = THIS_MODULE,
	.obj_size = sizeof(struct ecat_sock),
};

static int ecat_create(struct net *net, struct socket *sock, int protocol,
			 int kern);

static struct net_proto_family ecat_family_ops = {
	.family =	PF_ECONET,
	.create =	ecat_create,
	.owner	=	THIS_MODULE,
};

static struct hlist_head ecat_sklist;
static DEFINE_SPINLOCK(ecat_lock);
static DEFINE_MUTEX(ecat_mutex);

/*********************** net proto  start **************************/

static void ecat_remove_socket(struct hlist_head *list, struct sock *sk)
{
	spin_lock_bh(&ecat_lock);
	sk_del_node_init(sk);
	spin_unlock_bh(&ecat_lock);
}

static void ecat_insert_socket(struct hlist_head *list, struct sock *sk)
{
	spin_lock_bh(&ecat_lock);
	sk_add_node(sk, list);
	spin_unlock_bh(&ecat_lock);
}

/*
 *	Create an ethercat socket
 */
static int ecat_create(struct net *net, struct socket *sock, int protocol,
			 int kern)
{
	struct sock *sk;
	struct ecat_sock *ecatsock;
	int err;

	if (!net_eq(net, &init_net))
		return -EAFNOSUPPORT;

	/* ethercat only provides datagram services. */
	if (sock->type != SOCK_DGRAM)
		return -ESOCKTNOSUPPORT;

	sock->state = SS_UNCONNECTED;

	err = -ENOBUFS;
	sk = sk_alloc(net,  PF_ETHERCAT, GFP_KERNEL, &ecat_proto);
	if (sk == NULL)
		goto out;

	sk->sk_reuse = 1;
	sock->ops = &ecat_ops;
	sock_init_data(sock, sk);

	ecatsock = ecat_sk(sk);
	sock_reset_flag(sk, SOCK_ZAPPED);
	sk->sk_family = PF_ETHERCAT;
	ecatsock->num = protocol;
	sem_init(&ecatsock->recv_sem, 0);
	ecat_insert_socket(&ecat_sklist, sk);
	return 0;
out:
	return err;
}

/*
 *	Close an ecat socket.
 */
static int ecat_release(struct socket *sock)
{
	struct sock *sk;

	mutex_lock(&ecat_mutex);

	sk = sock->sk;
	if (!sk)
		goto out_unlock;

	ecat_remove_socket(&ecat_sklist, sk);
	/*
	 *	Now the socket is dead. No more input will appear.
	 */
	sk->sk_state_change(sk);	/* It is useless. Just for sanity. */

	sock_orphan(sk);

	/* Purge queues */

	skb_queue_purge(&sk->sk_receive_queue);
	sk_free(sk);

out_unlock:
	mutex_unlock(&ecat_mutex);
	return 0;
}

static int ecat_ioctl(struct socket *sock, unsigned int cmd,
			unsigned long arg)
{
	switch (cmd)
	{
	case	GET_PROCDATA_SIZE:
		break;
	
	}
	return -1;
}

static int ecat_sendmsg(struct kiocb *iocb, struct socket *sock,
			  struct msghdr *msg, size_t len)
{
	int err;
	int copied;
	struct ecat_sock *ecatsock = ecat_sk(sk);

	if (ecatsock->userdata == NULL)
		return -EINVAL;
	copied = (len < process_data_size) ? len : process_data_size;
	err = memcpy_fromiovec(ecatsock->userbuf, msg->msg_iov, copied);
	if (err) {
		return -EINVAL;
	}
	return copied;
}

static int ecat_recvmsg(struct kiocb *iocb, struct socket *sock,
			  struct msghdr *msg, size_t len, int flags)
{
	struct sock *sk = sock->sk;
	struct sk_buff *skb = NULL;
	size_t copied;
	struct ecat_sock *ecatsock = ecat_sk(sk);
	int err;

	down_interruptible(&ecatsock->recv_sem);
	skb = ecatsock->skb;
	if (!skb) {
		return -EFAULT;
	}
	ecatsock->skb = NULL;
	copied = skb->len - sizeof(struct ethercat_);
	if (copied > len) {
		copied = len;
		msg->msg_flags |= MSG_TRUNC;
	}
	pd = process_data(skb->data);
	err = memcpy_toiovec(msg->msg_iov, skb->data, copied);
	skb_put(skb);
	if (err) {
		return -ENOMEM;
	}
	sk->sk_stamp = skb->tstamp;
	return copied;
}

static const struct proto_ops ecat_ops = {
	.family =	PF_ETHERCAT,
	.owner =	THIS_MODULE,
	.release =	ecat_release,
	.bind =		sock_no_bind,
	.connect =	sock_no_connect,
	.socketpair =	sock_no_socketpair,
	.accept =	sock_no_accept,
	.getname =	sock_no_getname,
	.poll =		sock_no_poll,
	.ioctl =	ecat_ioctl,
	.listen =	sock_no_listen,
	.shutdown =	sock_no_shutdown,
	.setsockopt =	sock_no_setsockopt,
	.getsockopt =	sock_no_getsockopt,
	.sendmsg =	ecat_sendmsg,
	.recvmsg =	ecat_recvmsg,
	.mmap =		sock_no_mmap,
	.sendpage =	sock_no_sendpage,
};

void ecat_netproto_exit(void)
{
//	unregister_netdevice_notifier(&econet_netdev_notifier);
	sock_unregister(ecat_family_ops.family);
	proto_unregister(&ecat_proto);
}

int ecat_netproto_init(void)
{
	int err = proto_register(&ecat_proto, 0);

	if (err != 0)
		goto out;
	sock_register(&ecat_family_ops);
//	register_netdevice_notifier(&ecat_netdev_notifier);
out:
	return err;
}
