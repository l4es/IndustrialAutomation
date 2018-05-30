#ifndef  __XGENERAL_H__ 
#define  __XGENERAL_H__ 

#define TIMESPEC2NS(T) ((uint64_t) (T).tv_sec * NSEC_PER_SEC + (T).tv_nsec)
#define RX_INT_INDEX	0	/* port in the master side */
#define TX_INT_INDEX	1	/* port in the next side  */

/*
#ifdef __x86_64__
	typedef long INT;
#else
	typedef int INT;
#endif
*/
#ifdef __KERNEL__

#include <linux/if_ether.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/time.h>


struct ether_header {
  u_int8_t  ether_dhost[ETH_ALEN];	/* destination eth addr	*/
  u_int8_t  ether_shost[ETH_ALEN];	/* source ether addr	*/
  u_int16_t ether_type;		        /* packet type ID field	*/
} __attribute__ ((__packed__));

#define xmalloc(size) 	kmalloc(size, GFP_KERNEL)
static inline void clock_gettime(int dummy __attribute__((unused)), struct timespec *sp)
{
	getnstimeofday(sp);
}

static inline int clock_settime(int dummy __attribute__((unused)), struct timespec *sp)
{
	return do_settimeofday(sp);
}

#define LIST_ENTRY(a) struct list_head 
#define __attribute__packed__  __attribute__ ((packed))

#endif

#ifdef ARDUINO

#include <Arduino.h>
#include "list.h"
#include "ec_util.h"

#define LIST_ENTRY(a) struct list_head 
#define ETH_ALEN  6
#define  __attribute__packed__ __attribute__ ((__packed__))
#define ec_printf(a,...)

struct semaphore{
};

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long int uint32_t ;

typedef signed char int8_t;
typedef int int16_t; /* integer in arduino is 16bit */
typedef long int int32_t ;

struct ether_header {
  uint8_t  ether_dhost[ETH_ALEN];      /* destination eth addr */
  uint8_t  ether_shost[ETH_ALEN];      /* source ether addr    */
  uint16_t ether_type;                 /* packet type ID field */
};

#endif

#if defined ( __linux__ ) && !defined(__KERNEL__)

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <asm/types.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <asm/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include <sys/time.h>
#include <pthread.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <string.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <sys/queue.h>

#define xmalloc(size)	malloc(size)
#define NSEC_PER_SEC (1000000000L)
#define __attribute__packed__  __attribute__ ((packed))

struct semaphore { 
	sem_t sem;
};

#endif

#endif 
