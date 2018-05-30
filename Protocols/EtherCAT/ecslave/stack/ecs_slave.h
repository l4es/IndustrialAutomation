#ifndef __ECS_SLAVE_H__
#define __ECS_SLAVE_H__

#include "ecat_timer.h"
#include "ec_categories.h"

#ifdef __cplusplus
extern "C" {
#endif

struct __ecat_slave__;
struct fsm_slave;

typedef struct {
	void (*state)(struct __ecat_slave__ *,uint8_t *,int);
} fsm_mbox;

typedef struct {
	uint16_t obj_index __attribute__packed__;
	uint8_t obj_subindex ;
} fsm_coe; 

#define EC_MAX_PORTS 		4
#define TOT_PDOS		4 	/* change when change pdo entries number */
#define HTYPE_ETHER     	0x1	/* Ethernet  */
#define ETHERCAT_TYPE 		0x88a4

struct ec_device;

typedef struct __dlstatus__ {
	uint16_t pdi_operational:1,
		 dlsuserwatchdog_status:1,
		 extended_link_detection:1,
		 reserved:1,
		 link_status_port0:1,
		 link_status_port1:1,
		 link_status_port2:1,
		 link_status_port3:1,
		 loop_status_port0:1,
		 signal_detection_port0:1,
		 loop_status_port1:1,
		 signal_detection_port1:1,
		 loop_status_port2:1,
		 signal_detection_port2:1,
		 loop_status_port3:1,
		 signal_detection_port3:1;
} dlstatus;

struct ecat_regs {
	uint8_t base;
	uint8_t revision;
	uint8_t portdes;
	uint16_t alstat;
	uint16_t dlstat;
	uint8_t station_address;
	uint8_t alias;
	uint8_t dlctl;
	uint8_t dlport;
	uint8_t dlalias;
	uint8_t alctl;
	uint8_t alstacode;
	uint8_t pdictl;
	uint8_t itqmask;
	uint8_t rxerr;
	uint8_t eepcfg;
	uint8_t eepctl;
	uint8_t eepaddr;
	uint8_t eepdat;
	uint8_t cycle_unit_ctrl;
	uint8_t assign_active;
	uint32_t propagation_delay;
	uint32_t offset_from_systemtime;
	uint32_t drift;
	uint32_t sync0_start;
	uint32_t sync1_start;
	uint32_t cycle_ns;
	uint32_t dcoffset;
	uint32_t rxtime_port[4];
};

typedef struct {
	uint8_t* data;
	int size;
}process_data;

struct __sii_categories__;

typedef struct __ecat_slave__ {
	uint8_t *pkt_head;
	uint8_t *dgram_processed; /* current ethercat dgram processed */
	uint8_t dgrams_cnt;
	int pkt_size;
	int debug_level;
	int interfaces_nr;
	process_data pd;
	struct ec_device* intr[EC_MAX_PORTS];
	int pdoe_sizes[TOT_PDOS]; /* description array of pdos sizes */
	struct fsm_slave *fsm;	/* finite state machine */
	fsm_coe coe;
	fsm_mbox mbox;
	uint8_t index;			/* used by etherlab debug api */
	struct semaphore device_sem;	/* used by etherlab */
	struct ecat_regs registers;
	struct __sii_categories__ categories;
	int16_t last_word_offset;
	void (*sii_command)(struct __ecat_slave__*, int16_t offset, int16_t datalen, uint8_t * data);
} ecat_slave;

typedef struct __ecat_slave__ ecat_node_t;

int  ecs_net_init(int ,char *argv[], ecat_slave *);
int  ecs_init(ecat_slave *);
void ecs_run(ecat_slave *);

void ec_cmd_apwr(ecat_slave * slave, uint8_t *ecdgram);
void ec_cmd_armw(ecat_slave * slave, uint8_t *ecdgram);
void ec_cmd_aprw(ecat_slave * slave, uint8_t *ecdgram);
void ec_cmd_aprd(ecat_slave * slave, uint8_t *ecdgram);
void ec_cmd_fprd(ecat_slave * slave, uint8_t *ecdgram);
void ec_cmd_frmw(ecat_slave * slave, uint8_t *ecdgram);
void ec_cmd_fpwr(ecat_slave * slave, uint8_t *ecdgram);
void ec_cmd_fprw(ecat_slave * slave, uint8_t *ecdgram);
void ec_cmd_brw(ecat_slave * slave,  uint8_t *ecdgram);
void ec_cmd_bwr(ecat_slave * slave,  uint8_t *ecdgram);
void ec_cmd_brd(ecat_slave * slave,  uint8_t *ecdgram);
void ec_cmd_nop(ecat_slave * slave,  uint8_t *ecdgram);
void ec_cmd_lrd(ecat_slave * slave,  uint8_t *ecdgram);
void ec_cmd_lrw(ecat_slave * slave,  uint8_t *ecdgram);
void ec_cmd_lwr(ecat_slave * slave,  uint8_t *ecdgram);

void ecs_process_next_dgram(ecat_slave * slave,  uint8_t *ecdgram);
int  ec_nr_dgrams(uint8_t *raw_pkt);

/* d points at start of datagram.  */
static inline uint8_t *__ec_dgram_data(uint8_t *d)
{
	return (uint8_t *)&d[sizeof(ec_dgram)];
}

/* d points at start of datagram.  */
static inline int __ec_dgram_command(uint8_t * d)
{
	ec_dgram *datagram = (ec_dgram *)d;
	return datagram->command;
}

static inline int __ec_is_last_dgram(uint8_t *d)
{
	ec_dgram *datagram = (ec_dgram *)d;
	return datagram->dlength & 0b1000000000000000;
}
/* d points at start of datagram.  */
static inline uint16_t __ec_dgram_dlength(uint8_t *d)
{
	ec_dgram *datagram = (ec_dgram *)d;
	uint16_t datalen = (datagram->dlength & 0x07FF);
	return datalen;
}

/* d points at start of datagram.  */
static inline uint16_t __ec_dgram_adp(uint8_t * d)
{
	ec_dgram *datagram = (ec_dgram *)d;
	return (datagram->adp);
}

/* d points at start of datagram.  */
static inline void __ec_dgram_set_adp(uint8_t *d,uint16_t adp)
{
	ec_dgram *datagram = (ec_dgram *)d;
	datagram->adp = adp;
}

/* d points at start of datagram.  */
static inline uint32_t __ec_dgram_laddr(uint8_t * d)
{
	ec_dgram *datagram = (ec_dgram *)d;
	return ((uint32_t) datagram->adp | (uint32_t) (((uint32_t)datagram->ado) << 16));
}

/* d points at start of datagram.  */
/* address offset */
static inline uint16_t __ec_dgram_ado(uint8_t * d)
{
	ec_dgram *datagram = (ec_dgram *)d;
	return datagram->ado;
}

/* d points at start of datagram.  */
static inline uint8_t __ec_dgram_pkt_index(uint8_t * d)
{
	ec_dgram *datagram = (ec_dgram *)d;
	return datagram->index;
}

static inline uint8_t *__ec_get_shost(uint8_t* h)
{
	struct ether_header *eh = (struct ether_header *)h;
	return (uint8_t *)&(eh->ether_shost[0]);
}

static inline int __ec_is_ethercat(uint8_t * h)
{
	struct ether_header *eh = (struct ether_header *)h;
	return htons(eh->ether_type) == ETHERCAT_TYPE;
}

/* d points at start of datagram.  */
static inline void __ec_inc_wkc__(uint8_t *d)
{
	uint16_t wkc1;
	uint16_t size =  __ec_dgram_dlength(d);
	uint16_t *wkc = (uint16_t *)&d[size + sizeof(ec_dgram)];

	wkc1 = *wkc;
	wkc1++;
	*wkc = wkc1;
}

static inline uint16_t __ec_wkc(uint8_t *d)
{
	uint16_t size =  __ec_dgram_dlength(d);
	uint16_t *wkc = (uint16_t *)&d[size + sizeof(ec_dgram)];
	return *wkc;
}

/* h points to mac address */
static inline uint8_t *__ecat_frameheader(uint8_t *h)
{
	return (uint8_t *) & h[sizeof(struct ether_header)];
}

/* d is on mac address */
static inline uint16_t __ec_frame_size(uint8_t * d)
{
	ec_frame_header *ec_frame = (ec_frame_header *) __ecat_frameheader(d);
	return (ec_frame->elength & 0x07FF); /* old compilers do not like binary numbers*/
}

#ifdef __cplusplus
}
#endif

#endif
