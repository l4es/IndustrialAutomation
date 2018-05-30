#include "xgeneral.h"
#include "ec_device.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_net.h"

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>

#include <linux/sockios.h>
#include <linux/ethtool.h>

extern int slaves_nr;
extern ecat_slave slaves[];

static inline int ec_is_local_slave(ecat_slave *esv) {
       return (esv->intr[0]->sock == 0);
}


/* use the ethtool way to determine whether link is up*/
int ec_is_nic_link_up(ecat_slave *esv,struct ec_device *intr)
{
    int sock;
    struct ifreq ifr;
    struct ethtool_value edata;
    int rc;
	
    if (ec_is_local_slave(esv)) {
	return 1;
    }

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
    	return 0;
    }
    strncpy(ifr.ifr_name, intr->name, sizeof(ifr.ifr_name));
    ifr.ifr_data = (void *)&edata;

    edata.cmd = ETHTOOL_GLINK;

    rc = ioctl(sock, SIOCETHTOOL, &ifr);
    close(sock);
    if (rc < 0) {
        return 0;
    }

    return  edata.data  ? 1 : 0;
}

int ecs_get_intr_conf(struct ec_device * intr)
{
	struct ifreq ifr;
	int ret;
	char temp_str[20];
	struct sockaddr_in sin_ip;
	struct sockaddr_in sin_mask;
	struct sockaddr *sa;

	/* Get host's ip */
	strcpy(ifr.ifr_name, intr->name);
	ret = ioctl(intr->sock, SIOCGIFADDR, &ifr);
	if (ret < 0) {
		/* no IP. put all zeros */
		memset(&sin_ip, 0, sizeof(struct sockaddr));
	} else {
		memcpy(&sin_ip, &ifr.ifr_addr, sizeof(struct sockaddr));
	}

	inet_ntop(AF_INET, &sin_ip.sin_addr, intr->ip, sizeof(intr->ip));

	ec_printf("%s:\nLOCAL IP %s\n", ifr.ifr_name, intr->ip);
	/*
	 * get host's subnet mask
	 */
	strcpy(ifr.ifr_name, intr->name);
	ret = ioctl(intr->sock, SIOCGIFNETMASK, &ifr);
	if (ret < 0) {
		/* no mask. put all zeros */
		memset(&sin_mask, 0, sizeof(struct sockaddr));
		ec_printf("LOCAL SUBNET MASK 0.0.0.0\n");
	} else{
		memcpy(&sin_mask, &ifr.ifr_netmask,
		       sizeof(struct sockaddr));
		ec_printf("LOCAL SUBNET MASK %s\n",
			  inet_ntop(AF_INET, &sin_mask.sin_addr, temp_str,
				    sizeof(temp_str)));
	}
	intr->subnet_mask = sin_mask.sin_addr.s_addr;
	/* get mac address */
	strcpy(ifr.ifr_name, intr->name);
	ret = ioctl(intr->sock, SIOCGIFHWADDR, &ifr);
	if (ret < 0) {
		perror("failed to get interface address\n");
		return -1;
	}
	sa = &ifr.ifr_hwaddr;
	if (sa->sa_family != ARPHRD_ETHER) {
		perror("interface without ARPHRD_ETHER");
		return -1;
	}
	/* first byte is type */
	intr->mac.ether_type = HTYPE_ETHER;
	/* other six bytes the actual addresses */
	memcpy(&intr->mac.ether_shost, sa->sa_data, 6);

	sprintf(intr->macaddr,
		  "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
		  intr->mac.ether_shost[0],
		  intr->mac.ether_shost[1],
		  intr->mac.ether_shost[2],
		  intr->mac.ether_shost[3],
		  intr->mac.ether_shost[4], intr->mac.ether_shost[5]);
	if (ioctl(intr->sock, SIOCGIFINDEX, &ifr) == -1) {
		return (-1);
	}
	intr->index = ifr.ifr_ifindex;
	ec_printf("LOCAL MAC %s\n", intr->macaddr);
	return 0;
}

int ecs_sock(struct ec_device * intr)
{
	struct ifreq ifr;

	intr->sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (intr->sock < 0) {
		perror("socket failed:");
		return -1;
	}
	memset(&ifr,0,sizeof(ifr));
	strcpy(ifr.ifr_name, intr->name);
	if (setsockopt(intr->sock, SOL_SOCKET, 
			SO_BINDTODEVICE, &ifr, sizeof(ifr)) < 0){
		fprintf(stderr, "failed to bind socket to interface %s (%s)\n",
				ifr.ifr_name,strerror(errno));		
		return -1;
	}
	return 0;
}


