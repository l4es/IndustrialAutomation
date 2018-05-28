/* This is the EtherCAT support layer for EtherLab. It is linked to every 
 * model and deals with the complexity of registering EtherCAT terminals 
 * on different EtherCAT masters for various sample times.
 * 
 * The code is used as follows:
 *      - Initialise structures using ecs_init()
 *      - Register EtherCAT slaves using ecs_reg_pdo() or ecs_reg_pdo_range()
 *      - If a slave requires SDO configuration objects, register them 
 *        using ecs_reg_sdo()
 *      - Before going into the cyclic real time mode, call ecs_start().
 *      - In the cyclic mode, call ecs_process() to input new values
 *        and ecs_send() to write new values to the EtherCAT terminals.
 *      - When finished, call ecs_end()
 *
 * See the individual functions for more details.
 * */

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
//#error "Don't have config.h"
#endif

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include "ecrt_support.h"

#if MT
#include <semaphore.h>
#endif

#ifndef EC_TIMESPEC2NANO
#define EC_TIMESPEC2NANO(TV) \
    (((TV).tv_sec - 946684800ULL) * 1000000000ULL + (TV).tv_nsec)
#endif

/* The following message gets repeated quite frequently. */
char *no_mem_msg = "Could not allocate memory";
char errbuf[256];

#if 0
#  define pr_debug(fmt, args...) printf(fmt, args)
#else
#  define pr_debug(fmt, args...)
#endif

/*#######################################################################
 * List definition, ideas taken from linux kernel
  #######################################################################*/
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */
struct list_head {
    struct list_head *next, *prev;
};

static void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each(pos, head, member)				\
	for (pos = container_of((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 	                                \
	     pos = container_of(pos->member.next, typeof(*pos), member))

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static void list_add_tail(struct list_head *new, struct list_head *head)
{
    new->next = head;
    new->prev = head->prev;
    head->prev->next = new;
    head->prev = new;
}

/*#######################################################################*/

struct ec_slave_sdo {
    struct list_head list;
    unsigned int master;
    unsigned int alias;
    unsigned int position;
    unsigned int len;
    void **addr;
};
struct list_head ec_slave_sdo_head = {&ec_slave_sdo_head, &ec_slave_sdo_head};


struct endian_convert_t {
    void (*copy)(const struct endian_convert_t*);
    void *dst;
    const uint8_t *src;
    size_t index;
};

/** EtherCAT domain.
 *
 * Every domain has one of these structures. There can exist exactly one
 * domain for a sample time on an EtherCAT Master.
 */
struct ecat_domain {
    struct list_head list;      /* Linked list of domains. This list
                                 * is only used prior to ecs_start(), where
                                 * it is reworked into an array for faster
                                 * access */

    char input;                 /* Input domain (TxPdo's) */
    char output;                /* Output domain (RxPdo's) */

    unsigned int id;            /* RTW id number assigned to this domain */

    ec_domain_t *handle;        /* used when calling EtherCAT functions */
    ec_domain_state_t state;    /* pointer to domain's state */

    struct ecat_master *master; /* The master this domain is registered
                                 * in. Every domain has exactly 1 master */

    unsigned int tid;           /* Id of the corresponding RealTime Workshop
                                 * task */

    size_t  input_count;
    size_t output_count;

    struct endian_convert_t *input_convert_list;
    struct endian_convert_t *output_convert_list;

    void *io_data;              /* IO data is located here */
};

/** EtherCAT master.
 *
 * For every EtherCAT Master, one such structure exists.
 */
struct ecat_master {
    struct list_head list;      /* Linked list of masters. This list
                                 * is only used prior to ecs_start(), where
                                 * it is reworked into an array for faster
                                 * access */

    unsigned int fastest_tid;   /* Fastest RTW task id that uses this 
                                 * master */

    struct task_stats *task_stats; /* Index into global task_stats */

    unsigned int id;            /* Id of the EtherCAT master */
    ec_master_t *handle;        /* Handle retured by EtherCAT code */
    ec_master_state_t state;    /* Pointer for master's state */

    unsigned int refclk_trigger_init; /* Decimation for reference clock
                                         == 0 => do not use dc */
    unsigned int refclk_trigger; /* When == 1, trigger a time syncronisation */

#if MT
    sem_t lock;
#endif

    struct list_head domain_list;
};

/** EtherCAT support layer.
 *
 * Data used by the EtherCAT support layer.
 */
static struct ecat_data {
    unsigned int nst;           /* Number of unique RTW sample times */
    unsigned int single_tasking;

    /* This list is used to store all masters during the registration
     * process. Thereafter this list is reworked in ecs_start(), moving 
     * the masters and domains into the st[] struct below */
    struct list_head master_list;

} ecat_data = {
    .master_list = {&ecat_data.master_list, &ecat_data.master_list},
};

/////////////////////////////////////////////////

/** Read non-aligned data types.
 *
 * The following functions are used to read non aligned data types.
 *
 * The return value is a generic data type that can represent the
 * source, left shifted so that the highest bits are occupied.
 *
 * The data is interpreted as little endian, which is the format
 * used by EtherCAT */

/*****************************************************************/

static void
ecs_copy_uint8(const struct endian_convert_t *c)
{
    *(uint8_t*)c->dst = *(uint8_t*)c->src;
}

/*****************************************************************/

static void
ecs_write_uint1(const struct endian_convert_t *c)
{
    uint8_t mask = 1U << c->index;
    uint8_t *val = c->dst;

    *val = (*val & ~mask) | ((*c->src & 1U) << c->index);
}

/*****************************************************************/

static void
ecs_write_uint2(const struct endian_convert_t *c)
{
    uint8_t mask = 3U << c->index;
    uint8_t *val = c->dst;

    *val = (*val & ~mask) | ((*c->src & 3U) << c->index);
}

/*****************************************************************/

static void
ecs_write_uint3(const struct endian_convert_t *c)
{
    uint8_t mask = 7U << c->index;
    uint8_t *val = c->dst;

    *val = (*val & ~mask) | ((*c->src & 7U) << c->index);
}

/*****************************************************************/

static void
ecs_write_uint4(const struct endian_convert_t *c)
{
    uint8_t mask = 15U << c->index;
    uint8_t *val = c->dst;

    *val = (*val & ~mask) | ((*c->src & 15U) << c->index);
}

/*****************************************************************/

static void
ecs_write_uint5(const struct endian_convert_t *c)
{
    uint8_t mask = 31U << c->index;
    uint8_t *val = c->dst;

    *val = (*val & ~mask) | ((*c->src & 31U) << c->index);
}

/*****************************************************************/

static void
ecs_write_uint6(const struct endian_convert_t *c)
{
    uint8_t mask = 63U << c->index;
    uint8_t *val = c->dst;

    *val = (*val & ~mask) | ((*c->src & 63U) << c->index);
}

/*****************************************************************/

static void
ecs_write_uint7(const struct endian_convert_t *c)
{
    uint8_t mask = 127U << c->index;
    uint8_t *val = c->dst;

    *val = (*val & ~mask) | ((*c->src & 127U) << c->index);
}

/*****************************************************************/

static void
ecs_write_le_uint16(const struct endian_convert_t *c)
{
    *(uint16_t*)c->dst = htole16(*(uint16_t*)c->src);
}

/*****************************************************************/

static void
ecs_write_le_uint24(const struct endian_convert_t *c)
{
    uint32_t value = htole32(*(uint32_t*)c->src);

    *(uint16_t*)c->dst = value >> 8;
    ((uint8_t*)c->dst)[2] = value;
}

/*****************************************************************/

static void
ecs_write_le_uint32(const struct endian_convert_t *c)
{
    *(uint32_t*)c->dst = htole32(*(uint32_t*)c->src);
}

/*****************************************************************/

static void
ecs_write_le_uint40(const struct endian_convert_t *c)
{
    uint64_t value = htole64(*(uint64_t*)c->src);

    *(uint32_t*)c->dst = value >> 8;
    ((uint8_t*)c->dst)[4] = value;
}

/*****************************************************************/

static void
ecs_write_le_uint48(const struct endian_convert_t *c)
{
    uint64_t value = htole64(*(uint64_t*)c->src);

    *(uint32_t*)c->dst = value >> 16;
    ((uint16_t*)c->dst)[2] = value;
}

/*****************************************************************/

static void
ecs_write_le_uint56(const struct endian_convert_t *c)
{
    uint64_t value = htole64(*(uint64_t*)c->src);

    *(uint32_t*)c->dst = value >> 24;
    ((uint16_t*)c->dst)[2] = value >> 8;
    ((uint8_t*)c->dst)[7] = value;
}

/*****************************************************************/

static void
ecs_write_le_uint64(const struct endian_convert_t *c)
{
    *(uint64_t*)c->dst = htole64(*(uint64_t*)c->src);
}

/*****************************************************************/

static void
ecs_write_le_single(const struct endian_convert_t *c)
{
    *(uint32_t*)c->dst = htole32(*(uint32_t*)c->src);
}

/*****************************************************************/

static void
ecs_write_le_double(const struct endian_convert_t *c)
{
    *(uint64_t*)c->dst = htole64(*(uint64_t*)c->src);
}

/*****************************************************************/

static void
ecs_write_be_uint16(const struct endian_convert_t *c)
{
    *(uint16_t*)c->dst = htobe16(*(uint16_t*)c->src);
}

/*****************************************************************/

static void
ecs_write_be_uint24(const struct endian_convert_t *c)
{
    uint32_t value = htobe32(*(uint32_t*)c->src);

    *(uint16_t*)c->dst = value >> 8;
    ((uint8_t*)c->dst)[2] = value;
}

/*****************************************************************/

static void
ecs_write_be_uint32(const struct endian_convert_t *c)
{
    *(uint32_t*)c->dst = htobe32(*(uint32_t*)c->src);
}

/*****************************************************************/

static void
ecs_write_be_uint40(const struct endian_convert_t *c)
{
    uint64_t value = htobe64(*(uint64_t*)c->src);

    *(uint32_t*)c->dst = value >> 8;
    ((uint8_t*)c->dst)[4] = value;
}

/*****************************************************************/

static void
ecs_write_be_uint48(const struct endian_convert_t *c)
{
    uint64_t value = htobe64(*(uint64_t*)c->src);

    *(uint32_t*)c->dst = value >> 16;
    ((uint16_t*)c->dst)[2] = value;
}

/*****************************************************************/

static void
ecs_write_be_uint56(const struct endian_convert_t *c)
{
    uint64_t value = htobe64(*(uint64_t*)c->src);

    *(uint32_t*)c->dst = value >> 24;
    ((uint16_t*)c->dst)[2] = value >> 8;
    ((uint8_t*)c->dst)[7] = value;
}

/*****************************************************************/

static void
ecs_write_be_uint64(const struct endian_convert_t *c)
{
    *(uint64_t*)c->dst = htobe64(*(uint64_t*)c->src);
}

/*****************************************************************/

static void
ecs_write_be_single(const struct endian_convert_t *c)
{
    *(uint32_t*)c->dst = htobe32(*(uint32_t*)c->src);
}

/*****************************************************************/

static void
ecs_write_be_double(const struct endian_convert_t *c)
{
    *(uint64_t*)c->dst = htobe64(*(uint64_t*)c->src);
}

/*****************************************************************/
/*****************************************************************/

static void
ecs_read_uint1(const struct endian_convert_t *c)
{
    *(uint8_t*)c->dst = (*c->src >> c->index) & 0x01;
}

/*****************************************************************/

static void
ecs_read_uint2(const struct endian_convert_t *c)
{
    *(uint8_t*)c->dst = (*c->src >> c->index) & 0x03;
}

/*****************************************************************/

static void
ecs_read_uint3(const struct endian_convert_t *c)
{
    *(uint8_t*)c->dst = (*c->src >> c->index) & 0x07;
}

/*****************************************************************/

static void
ecs_read_uint4(const struct endian_convert_t *c)
{
    *(uint8_t*)c->dst = (*c->src >> c->index) & 0x0F;
}

/*****************************************************************/

static void
ecs_read_uint5(const struct endian_convert_t *c)
{
    *(uint8_t*)c->dst = (*c->src >> c->index) & 0x1F;
}

/*****************************************************************/

static void
ecs_read_uint6(const struct endian_convert_t *c)
{
    *(uint8_t*)c->dst = (*c->src >> c->index) & 0x3F;
}

/*****************************************************************/

static void
ecs_read_uint7(const struct endian_convert_t *c)
{
    *(uint8_t*)c->dst = (*c->src >> c->index) & 0x7F;
}

/*****************************************************************/

static void
ecs_read_le_uint16(const struct endian_convert_t *c)
{
    *(uint16_t*)c->dst = le16toh(*(uint16_t*)c->src);
}

/*****************************************************************/

static void
ecs_read_le_uint24(const struct endian_convert_t *c)
{
    *(uint32_t*)c->dst =
        ((uint32_t) le16toh(*(uint16_t*)(c->src + 1)) << 8)
        + *c->src;
}

/*****************************************************************/

static void
ecs_read_le_uint32(const struct endian_convert_t *c)
{
    *(uint32_t*)c->dst = le32toh(*(uint32_t*)c->src);
}

/*****************************************************************/

static void
ecs_read_le_uint40(const struct endian_convert_t *c)
{
    *(uint64_t*)c->dst =
        ((uint64_t) le32toh(*(uint32_t*)(c->src + 1)) << 8)
        + *c->src;
}

/*****************************************************************/

static void
ecs_read_le_uint48(const struct endian_convert_t *c)
{
    *(uint64_t*)c->dst =
        ((uint64_t)le32toh(*(uint32_t*)(c->src+2)) << 16)
        + le16toh(*(uint16_t*)c->src);
}

/*****************************************************************/

static void
ecs_read_le_uint56(const struct endian_convert_t *c)
{
    *(uint64_t*)c->dst =
        ((uint64_t)le32toh(*(uint32_t*)(c->src+3)) << 24)
        + ((uint64_t)le16toh(*(uint16_t*)(c->src+1)) <<  8)
        + *c->src;
}

/*****************************************************************/

static void
ecs_read_le_uint64(const struct endian_convert_t *c)
{
    *(uint64_t*)c->dst = le64toh(*(uint64_t*)c->src);
}

/*****************************************************************/

static void
ecs_read_le_single(const struct endian_convert_t *c)
{
    *(uint32_t*)c->dst = le32toh(*(uint32_t*)c->src);
}

/*****************************************************************/

static void
ecs_read_le_double(const struct endian_convert_t *c)
{
    *(uint64_t*)c->dst = le64toh(*(uint64_t*)c->src);
}

/*****************************************************************/

static void
ecs_read_be_uint16(const struct endian_convert_t *c)
{
    *(uint16_t*)c->dst = be16toh(*(uint16_t*)c->src);
}

/*****************************************************************/

static void
ecs_read_be_uint24(const struct endian_convert_t *c)
{
    *(uint32_t*)c->dst = ((uint32_t)*c->src << 16)
        + be16toh(*(uint16_t*)(c->src + 1));
}

/*****************************************************************/

static void
ecs_read_be_uint32(const struct endian_convert_t *c)
{
    *(uint32_t*)c->dst = be32toh(*(uint32_t*)c->src);
}

/*****************************************************************/

static void
ecs_read_be_uint40(const struct endian_convert_t *c)
{
    *(uint64_t*)c->dst = ((uint64_t)*c->src << 32)
        + be32toh(*(uint32_t*)(c->src+1));
}

/*****************************************************************/

static void
ecs_read_be_uint48(const struct endian_convert_t *c)
{
    *(uint64_t*)c->dst =
        ((uint64_t)be16toh(*(uint16_t*)c->src) << 32)
        + be32toh(*(uint32_t*)(c->src+2));
}

/*****************************************************************/

static void
ecs_read_be_uint56(const struct endian_convert_t *c)
{
    *(uint64_t*)c->dst =
        ((uint64_t)*c->src << 48)
        + ((uint64_t)be16toh(*(uint16_t*)(c->src+1)) << 32)
        + be32toh(*(uint32_t*)(c->src+3));
}

/*****************************************************************/

static void
ecs_read_be_uint64(const struct endian_convert_t *c)
{
    *(uint64_t*)c->dst = be64toh(*(uint64_t*)c->src);
}

/*****************************************************************/

static void
ecs_read_be_single(const struct endian_convert_t *c)
{
    *(uint32_t*)c->dst = be32toh(*(uint32_t*)c->src);
}


/*****************************************************************/

static void
ecs_read_be_double(const struct endian_convert_t *c)
{
    *(uint64_t*)c->dst = be64toh(*(uint64_t*)c->src);
}

/*****************************************************************/

/* Do input processing for a RTW task.
 *
 * It does the following:
 *  - calls ecrt_master_receive() for every master whose fastest domain is in
 *    this task. 
 *  - calls ecrt_domain_process() for every domain in this task
 */
void
ecs_receive(size_t tid)
{
    struct ecat_master *master;
    struct ecat_domain *domain;
    struct endian_convert_t *conversion_list;
        
    list_for_each(master, &ecat_data.master_list, list) {

#if MT
        sem_wait(&master->lock);
#endif

        if (master->fastest_tid == tid) {
            ecrt_master_receive(master->handle);
            ecrt_master_state(master->handle, &master->state);
        }
        
        list_for_each(domain, &master->domain_list, list) {

            if (domain->tid != tid)
                continue;

            ecrt_domain_process(domain->handle);
            ecrt_domain_state(domain->handle, &domain->state);

            if (!domain->input)
                continue;

            for (conversion_list = domain->input_convert_list;
                    conversion_list->copy; conversion_list++) {
                conversion_list->copy(conversion_list);
            }
        }
#if MT
        sem_post(&master->lock);
#endif
    }
}

/* Do EtherCAT output processing for a RTW task.
 *
 * It does the following:
 * - calls ecrt_master_run() for every master whose fastest task domain is in
 *   this task
 * - calls ecrt_master_send() for every domain in this task
 */
void
ecs_send(size_t tid)
{
    struct ecat_master *master;
    struct ecat_domain *domain;
    struct endian_convert_t *conversion_list;

    list_for_each(master, &ecat_data.master_list, list) {
#if MT
        sem_wait(&master->lock);
#endif
        list_for_each(domain, &master->domain_list, list) {

            if (domain->tid != tid)
                continue;

            if (domain->output) {
                for (conversion_list = domain->output_convert_list;
                        conversion_list->copy; conversion_list++) {
                    conversion_list->copy(conversion_list);
                }
            }

            ecrt_domain_queue(domain->handle);
        }

        if (master->fastest_tid == tid) {
            struct timespec tp;

            clock_gettime(CLOCK_MONOTONIC, &tp);
            ecrt_master_application_time(master->handle,
                    EC_TIMESPEC2NANO(tp));

            if (master->refclk_trigger_init && !--master->refclk_trigger) {
                ecrt_master_sync_reference_clock(master->handle);
                master->refclk_trigger = master->refclk_trigger_init;
            }

            ecrt_master_sync_slave_clocks(master->handle);
            ecrt_master_send(master->handle);
        }
#if MT
        sem_post(&master->lock);
#endif
    }
}

/***************************************************************************/

static struct ecat_master *
get_master(
        unsigned int master_id, unsigned int tid, const char **errmsg)
{
    struct ecat_master *master;

    list_for_each(master, &ecat_data.master_list, list) {

        if (master->id == master_id) {

            if (master->fastest_tid > tid)
                master->fastest_tid = tid;

            return master;
        }
    }

    master = calloc(1, sizeof(struct ecat_master));
    master->id = master_id;
    master->fastest_tid = tid;
#if MT
    sem_init(&master->lock, 0, 1);
#endif
    INIT_LIST_HEAD(&master->domain_list);
    list_add_tail(&master->list, &ecat_data.master_list);

    master->handle = ecrt_request_master(master_id);

    if (!master->handle) {
        snprintf(errbuf, sizeof(errbuf),
                "ecrt_request_master(%u) failed", master_id);
        *errmsg = errbuf;
        return NULL;
    }

    return master;
}

/***************************************************************************/

static struct ecat_domain *
get_domain( struct ecat_master *master, unsigned int domain_id,
        char input, char output, unsigned int tid, const char **errmsg)
{
    struct ecat_domain *domain;

    pr_debug("get_domain(master=%u, domain=%u, input=%u, output=%u, tid=%u)\n",
            master->id, domain_id, input, output, tid);

    /* Go through every master's domain list to see whether the
     * required domain exists */
    list_for_each(domain, &master->domain_list, list) {
        if (domain->id == domain_id
                && ((domain->output && output) || (domain->input && input))
                && domain->tid == tid) {

            /* Set input and output flags. The flags are cumulative */
            domain->input  |=  input != 0;
            domain->output |= output != 0;

            return domain;
        }
    }

    /* No domain found, create new one */
    domain = calloc(1, sizeof(struct ecat_domain));
    domain->id = domain_id;
    domain->tid = tid;
    domain->master = master;
    domain->input  =  input != 0;
    domain->output = output != 0;
    list_add_tail(&domain->list, &master->domain_list);

    domain->handle = ecrt_master_create_domain(master->handle);
    if (!domain->handle) {
        snprintf(errbuf, sizeof(errbuf),
                "ecrt_master_create_domain(master=%u) failed", master->id);
        *errmsg = errbuf;
        return NULL;
    }

    pr_debug("New domain(%u) = %p IP=%u, OP=%u, tid=%u\n",
            domain_id, domain->handle, input, output, tid);

    return domain;
}

/***************************************************************************/

static const char *
register_pdos( ec_slave_config_t *slave_config, struct ecat_domain *domain,
        char dir, struct pdo_map *pdo_map, size_t count)
{
    const struct pdo_map *pdo_map_end = pdo_map + count;
    const char *failed_method;

    for (; pdo_map != pdo_map_end; pdo_map++) {
        unsigned int bitlen = pdo_map->datatype % 1000;

        pdo_map->offset = ecrt_slave_config_reg_pdo_entry(
                slave_config,
                pdo_map->pdo_entry_index,
                pdo_map->pdo_entry_subindex,
                domain->handle,
                &pdo_map->bit_pos);

        pr_debug("offset=%i %i\n", pdo_map->offset, pdo_map_end - pdo_map);

        pdo_map->domain = domain;
        if (dir)
            domain->input_count++;
        else
            domain->output_count++;

        if (pdo_map->offset < 0) {
            failed_method = "ecrt_slave_config_reg_pdo_entry";
            goto out_slave_failed;
        }

        if (bitlen < 8) {
            pdo_map->bit_pos += bitlen * pdo_map->idx;
            pdo_map->offset += pdo_map->bit_pos / 8;
            pdo_map->bit_pos = pdo_map->bit_pos % 8;
        }
        else {
            if (pdo_map->bit_pos) {
                snprintf(errbuf, sizeof(errbuf), 
                        "Pdo Entry #x%04X.%u is not byte aligned",
                        pdo_map->pdo_entry_index,
                        pdo_map->pdo_entry_subindex);
                return errbuf;
            }
            pdo_map->offset += bitlen*pdo_map->idx / 8;
        }
    }

    return NULL;

out_slave_failed:
    snprintf(errbuf, sizeof(errbuf), 
            "%s() failed ", failed_method);
    return errbuf;
}

/***************************************************************************/

static const char *
init_slave(size_t nst, const struct ec_slave *slave)
{
    struct ecat_master *master;
    struct ecat_domain *domain;
    ec_slave_config_t *slave_config;
    const struct sdo_config *sdo;
    const struct ec_slave_sdo *sdo_req;
    const struct soe_config *soe;
    const char *failed_method;

    pr_debug("Initializing slave %u\n", slave->position);
    if (!(master = get_master(slave->master, slave->tid, &failed_method)))
        return failed_method;

    slave_config = ecrt_master_slave_config(master->handle, slave->alias,
            slave->position, slave->vendor, slave->product);
    if (!slave_config) {
        failed_method = "ecrt_master_slave_config";
        goto out_slave_failed;
    }

    /* Inform EtherCAT of how the slave configuration is expected */
    if (ecrt_slave_config_pdos(slave_config, EC_END, slave->ec_sync_info)) {
        failed_method = "ecrt_slave_config_pdos";
        goto out_slave_failed;
    }

    /* Send SDO configuration to the slave */
    for (sdo = slave->sdo_config;
            sdo != &slave->sdo_config[slave->sdo_config_count]; sdo++) {
        if (sdo->byte_array) {
            if (sdo->sdo_subindex >= 0) {
                if (ecrt_slave_config_sdo(slave_config, 
                            sdo->sdo_index, sdo->sdo_subindex,
                            (const uint8_t*)sdo->byte_array,
                            sdo->value)) {
                    failed_method = "ecrt_slave_config_sdo";
                    goto out_slave_failed;
                }
            }
            else {
                if (ecrt_slave_config_complete_sdo(slave_config,
                            sdo->sdo_index,
                            (const uint8_t*)sdo->byte_array,
                            sdo->value)) {
                    failed_method = "ecrt_slave_config_complete_sdo";
                    goto out_slave_failed;
                }
            }
        }
        else {
            switch (sdo->datatype) {
                case 1008U:
                    if (ecrt_slave_config_sdo8(slave_config,
                                sdo->sdo_index, sdo->sdo_subindex,
                                (uint8_t)sdo->value)) {
                        failed_method = "ecrt_slave_config_sdo8";
                        goto out_slave_failed;
                    }
                    break;

                case 1016U:
                    if (ecrt_slave_config_sdo16(slave_config,
                                sdo->sdo_index, sdo->sdo_subindex,
                                (uint16_t)sdo->value)) {
                        failed_method = "ecrt_slave_config_sdo16";
                        goto out_slave_failed;
                    }
                    break;

                case 1032U:
                    if (ecrt_slave_config_sdo32(slave_config,
                                sdo->sdo_index, sdo->sdo_subindex,
                                sdo->value)) {
                        failed_method = "ecrt_slave_config_sdo32";
                        goto out_slave_failed;
                    }
                    break;

                default:
                    failed_method = "ecrt_slave_config_sdo_unknown";
                    goto out_slave_failed;
                    break;
            }
        }
    }

    /* Send SoE configuration to the slave */
    for (soe = slave->soe_config; 
            soe != &slave->soe_config[slave->soe_config_count]; soe++) {
        if (ecrt_slave_config_idn(slave_config,
                    0, /* drive_no */
                    soe->idn,
                    EC_AL_STATE_PREOP,
                    (const uint8_t*)soe->data,
                    soe->data_len)) {
            failed_method = "ecrt_slave_config_idn";
            goto out_slave_failed;
        }
    }

    /* Configure distributed clocks for slave if assign_activate is set */
    if (slave->dc_config.assign_activate) {
        ecrt_slave_config_dc(slave_config,
                slave->dc_config.assign_activate,
                slave->dc_config.cycle_time0,
                slave->dc_config.shift_time,
                slave->dc_config.cycle_time1,
                0);
    }

    /* Register RxPdo's (output domain) */
    if (slave->rxpdo_count) {
        const char *err;

        domain = get_domain(master, slave->domain, 0, 1, slave->tid, &err);
        if (!domain)
            return err;

        if ((err = register_pdos(slave_config, domain, 0,
                        slave->pdo_map, slave->rxpdo_count)))
            return err;
    }

    /* Register TxPdo's (input domain) */
    if (slave->txpdo_count) {
        const char *err;

        domain = get_domain(master, slave->domain, 1, 0, slave->tid, &err);
        if (!domain)
            return err;

        if ((err = register_pdos(slave_config, domain, 1,
                        slave->pdo_map + slave->rxpdo_count,
                        slave->txpdo_count)))
            return err;
    }

    list_for_each(sdo_req, &ec_slave_sdo_head, list) {
        if (sdo_req->master != slave->master
                || sdo_req->alias != slave->alias
                || sdo_req->position != slave->position)
            continue;

        *sdo_req->addr =
            ecrt_slave_config_create_sdo_request(slave_config, 0, 0, sdo_req->len);
    }

    return NULL;

out_slave_failed:
    snprintf(errbuf, sizeof(errbuf), 
            "%s() failed while configuring slave %u:%u",
            failed_method, slave->alias, slave->position);
    return errbuf;
}

/***************************************************************************/
int ecs_sdo_handler(
        unsigned int master_id, 
        unsigned int alias, 
        unsigned int position, 
        unsigned int len, 
        void **addr)
{
    struct ec_slave_sdo *s = malloc(sizeof(struct ec_slave_sdo));

    if (!s)
        return 1;

    s->master = master_id;
    s->alias = alias;
    s->position = position;
    s->len = len;
    s->addr = addr;

    list_add_tail(&s->list, &ec_slave_sdo_head);
    return 0;
}

/***************************************************************************/

const char * ecs_start( 
        const struct ec_slave *slave_head,
        unsigned int *st,       /* List of sample times in nanoseconds */
        size_t nst,
        unsigned int single_tasking     /* Set if the model is single tasking,
                                         * even though there are more than one
                                         * sample time */
        )
{
    const char *err;
    const struct ec_slave *slave;
    struct ecat_master *master;

    ecat_data.nst = nst;
    ecat_data.single_tasking = single_tasking;

    for (slave = slave_head; slave; slave = slave->next) {
    pr_debug("init: %i\n", __LINE__);
        if ((err = init_slave(nst, slave)))
            goto out;
    }
    pr_debug("init: %i\n", __LINE__);

    list_for_each(master, &ecat_data.master_list, list) {
        struct ecat_domain *domain;

        pr_debug("init: %i\n", __LINE__);
        if (ecrt_master_activate(master->handle)) {
            snprintf(errbuf, sizeof(errbuf),
                    "Master %i activate failed", master->id);
            return errbuf;
        }

        list_for_each(domain, &master->domain_list, list) {
            domain->io_data = ecrt_domain_data(domain->handle);
            pr_debug("domain %p master=%u domain=%u, IP=%u, OP=%u, tid=%u\n",
                    domain->io_data, domain->master->id, domain->id,
                    domain->input, domain->output, domain->tid);

            domain->input_convert_list =
                calloc(domain->input_count + domain->output_count + 2,
                        sizeof(struct endian_convert_t));
            domain->output_convert_list =
                domain->input_convert_list + domain->input_count + 1;

            domain->input_count = 0;
            domain->output_count = 0;
        }
    }

    for (slave = slave_head; slave; slave = slave->next) {
        struct pdo_map *pdo_map = slave->pdo_map;
        struct pdo_map *pdo_map_end = slave->pdo_map + slave->rxpdo_count;

        for (; pdo_map != pdo_map_end; pdo_map++) {
            struct endian_convert_t *convert =
                pdo_map->domain->output_convert_list
                + pdo_map->domain->output_count++;
            size_t bitlen = pdo_map->datatype % 1000;
            size_t bytes = bitlen / 8;

            convert->src = pdo_map->address;
            convert->dst = pdo_map->domain->io_data + pdo_map->offset;

            if (pdo_map->datatype < 1008) {
                /* bit operations */
                static void (* const convert_funcs[])(
				const struct endian_convert_t *) = {
                    ecs_write_uint1, ecs_write_uint2,
                    ecs_write_uint3, ecs_write_uint4,
                    ecs_write_uint5, ecs_write_uint6,
                    ecs_write_uint7,
                };
                convert->copy = convert_funcs[bitlen-1];
                convert->dst = pdo_map->domain->io_data + pdo_map->offset;
                convert->index = pdo_map->bit_pos;
            }
            else if (pdo_map->datatype > 3000) {
                /* floating points */
                static void (* const convert_funcs[][2])(
				const struct endian_convert_t *) = {
                    {ecs_write_le_single, ecs_write_be_single},
                    {ecs_write_le_double, ecs_write_be_double},
                };
                convert->copy =
                    convert_funcs[bytes / 4 - 1][pdo_map->bigendian];
            }
            else {
                /* integers */
                static void (* const convert_funcs[][2])(
				const struct endian_convert_t *) = {
                    {ecs_copy_uint8,      ecs_copy_uint8     },
                    {ecs_write_le_uint16, ecs_write_be_uint16},
                    {ecs_write_le_uint24, ecs_write_be_uint24},
                    {ecs_write_le_uint32, ecs_write_be_uint32},
                    {ecs_write_le_uint40, ecs_write_be_uint40},
                    {ecs_write_le_uint48, ecs_write_be_uint48},
                    {ecs_write_le_uint56, ecs_write_be_uint56},
                    {ecs_write_le_uint64, ecs_write_be_uint64},
                };
                convert->copy =
                    convert_funcs[bytes - 1][pdo_map->bigendian];
            }
        }

        /* At this point pdo_map points to the start of TxPdo's
         * Only have to update pdo_map_end */
        pdo_map_end += slave->txpdo_count;
        for (; pdo_map != pdo_map_end; pdo_map++) {
            struct endian_convert_t *convert =
                pdo_map->domain->input_convert_list
                + pdo_map->domain->input_count++;
            size_t bitlen = pdo_map->datatype % 1000;
            size_t bytes = bitlen / 8;

            convert->dst = pdo_map->address;
            convert->src = pdo_map->domain->io_data + pdo_map->offset;

            if (pdo_map->datatype < 1008) {
                static void (* const convert_funcs[])(
				const struct endian_convert_t *) = {
                    ecs_read_uint1, ecs_read_uint2,
                    ecs_read_uint3, ecs_read_uint4,
                    ecs_read_uint5, ecs_read_uint6,
                    ecs_read_uint7,
                };
                convert->copy = convert_funcs[bitlen-1];
                convert->src = pdo_map->domain->io_data + pdo_map->offset;
                convert->index = pdo_map->bit_pos;
            }
            else if (pdo_map->datatype > 3000) {
                static void (* const convert_funcs[][2])(
				const struct endian_convert_t *) = {
                    {ecs_read_le_single, ecs_read_be_single},
                    {ecs_read_le_double, ecs_read_be_double},
                };
                convert->copy =
                    convert_funcs[bytes / 4 - 1][pdo_map->bigendian];
            }
            else {
                static void (* const convert_funcs[][2])(
				const struct endian_convert_t *) = {
                    {ecs_copy_uint8,     ecs_copy_uint8    },
                    {ecs_read_le_uint16, ecs_read_be_uint16},
                    {ecs_read_le_uint24, ecs_read_be_uint24},
                    {ecs_read_le_uint32, ecs_read_be_uint32},
                    {ecs_read_le_uint40, ecs_read_be_uint40},
                    {ecs_read_le_uint48, ecs_read_be_uint48},
                    {ecs_read_le_uint56, ecs_read_be_uint56},
                    {ecs_read_le_uint64, ecs_read_be_uint64},
                };
                convert->copy =
                    convert_funcs[bytes - 1][pdo_map->bigendian];
            }
        }
    }

    return NULL;

out:
    pr_debug("initout : %s\n", err);
    return err;
}

/***************************************************************************/

void ecs_end(size_t nst)
{
}

/***************************************************************************/

const char *
ecs_setup_master( unsigned int master_id,
        unsigned int refclk_sync_dec, void **master_p)
{
    const char *errmsg;
    /* Get the master structure, making sure not to change the task it 
     * is currently assigned to */
    struct ecat_master *master = get_master(master_id, ~0U, &errmsg);

    if (!master)
        return errmsg;

    if (master_p)
        *master_p = master->handle;

    master->refclk_trigger_init = refclk_sync_dec;
    master->refclk_trigger = 1;

    return NULL;
}

/***************************************************************************/

ec_domain_t *
ecs_get_domain_ptr(unsigned int master_id, unsigned int domain_id, 
        char input, char output, unsigned int tid, const char **errmsg)
{
    struct ecat_master *master;
    struct ecat_domain *domain;

    if (!(master = get_master(master_id, tid, errmsg)))
        return NULL;

    domain = get_domain(master, domain_id, input, output, tid, errmsg);
    return domain ? domain->handle : NULL;
}

/***************************************************************************/
