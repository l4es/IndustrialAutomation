/******************************************************************************
 *
 *  Copyright (C) 2008-2009  Andreas Stewering-Bone, Ingenieurgemeinschaft IgH
 *
 *  This file is part of the IgH EtherCOS Scicos Toolbox.
 *  
 *  The IgH EtherCOS Scicos Toolbox is free software; you can
 *  redistribute it and/or modify it under the terms of the GNU Lesser General
 *  Public License as published by the Free Software Foundation; version 2.1
 *  of the License.
 *
 *  The IgH EtherCOS Scicos Toolbox is distributed in the hope that
 *  it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the IgH EtherCOS Scicos Toolbox. If not, see
 *  <http://www.gnu.org/licenses/>.
 *  
 *  ---
 *  
 *  The license mentioned above concerns the source code only. Using the
 *  EtherCAT technology and brand is only permitted in compliance with the
 *  industrial property and similar rights of Beckhoff Automation GmbH.
 *
 *****************************************************************************/



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


#define DEBUG 1
#define DEBUG_MALLOC 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <asm/byteorder.h>
#include "list.h"
#include "ecrt_scicos_support.h"




/* The following message gets repeated quite frequently. */
char *no_mem_msg = "Could not allocate memory";
char errbuf[256];

/* Structure to temporarily store PDO objects prior to registration */
struct ecat_slave {
    List list;   /* Linked list of pdo's */
    /* Data required for ecrt_master_slave_config() */
    uint16_t alias;
    uint16_t position;
    uint32_t vendor_id; /**< vendor ID */
    uint32_t product_code; /**< product code */

    const ec_sync_info_t *sync_info;

    unsigned int sdo_config_count;
    const struct sdo_config *sdo_config;

    unsigned int pdo_map_count;
    struct mapped_pdo {
        struct ecat_domain *domain;
        const struct pdo_map *mapping;
        size_t base_offset;
    } mapped_pdo[];
};

struct endian_convert_t {
    enum si_datatype_t pdo_datatype;
    void *data_ptr;
};


/* Every domain has one of these structures. There can exist exactly one
 * domain for a sample time on an EtherCAT Master. */
struct ecat_domain {
    List list;                  /* Linked list of domains. This list
                                 * is only used prior to ecs_start(), where
                                 * it is reworked into an array for faster
                                 * access */

    unsigned int id;            /* RTW id number assigned to this domain */

    ec_domain_t *handle;        /* used when calling EtherCAT functions */
    ec_domain_state_t *state;   /* pointer to domain's state */

    struct ecat_master *master; /* The master this domain is registered
                                 * in. Every domain has exactly 1 master */

    unsigned int tid;           /* Id of the corresponding RealTime Workshop
                                 * task */

    unsigned int io_count;      /* The number of field io's this domain has */
    struct endian_convert_t       
        *endian_convert_list; /* List for data copy instructions. List is
                                  * terminated with list->from = NULL */

    size_t io_data_len;         /* Lendth of io_data image */
    void *io_data;              /* IO data is located here */
};

/* For every EtherCAT Master, one such structure exists */
struct ecat_master {
    List list;                  /* Linked list of masters. This list
                                 * is only used prior to ecs_start(), where
                                 * it is reworked into an array for faster
                                 * access */
    unsigned int fastest_tid;   /* Fastest RTW task id that uses this 
                                 * master */

    unsigned int id;            /* Id of the EtherCAT master */
    ec_master_t *handle;        /* Handle retured by EtherCAT code */
    ec_master_state_t *state;   /* Pointer for master's state */


    List slave_list;            /* List of all slaves. Only active during
                                  * initialisation */
    /* Temporary storage for domains registered to this master. Every
     * sample time has a read and a write list
     */
    struct {
        List input_domain_list;
        List output_domain_list;

    } st_domain[];
};

/* Data used by the EtherCAT support layer */
static struct ecat_data {
    unsigned int nst;           /* Number of unique RTW sample times */

    /* This list is used to store all masters during the registration
     * process. Thereafter this list is reworked in ecs_start(), moving 
     * the masters and domains into the st[] struct below */
    List master_list;
    struct st_data {
        unsigned int master_count;
        unsigned int input_domain_count;
        unsigned int output_domain_count;

        /* Array of masters and domains in this sample time. The data area
         * are slices out of st_data */
        struct ecat_master *master;
        struct ecat_domain *input_domain;
        struct ecat_domain *output_domain;

        /* The tick period in microseconds for this sample time */
        unsigned int period;
    } st[];
} *ecat_data;



#define pr_debug(fmt, arg...) printf(fmt, ##arg)


static void* my_kcalloc(size_t nmemb, size_t size, const char* who)
{
  void *p;
  p = calloc(nmemb, size);
  pr_debug("Allocated %u bytes for %s: %p\n", nmemb * size, who, p);
  return p;
}

static void
my_kfree(void *p, const char* who)
{
  pr_debug("Releasing memory for %s: %p\n", who, p);    
  free(p);
}






/* Do input processing for a RTW task. It does the following:
 *      - calls ecrt_master_receive() for every master whose fastest 
 *        domain is in this task. 
 *      - calls ecrt_domain_process() for every domain in this task
 */
void
ecs_receive(int tid)
{
    struct ecat_master *master = ecat_data->st[tid].master;
    struct ecat_master * const master_end = 
        master + ecat_data->st[tid].master_count;
    struct ecat_domain *domain = ecat_data->st[tid].input_domain;
    struct ecat_domain * const domain_end = 
        domain + ecat_data->st[tid].input_domain_count;
    struct endian_convert_t *conversion_list;
    //    static unsigned int count = 0;
        
    for ( ; master != master_end; master++) {
        ecrt_master_receive(master->handle);
        ecrt_master_state(master->handle, master->state);
    }

    for ( ; domain != domain_end; domain++) {
        ecrt_domain_process(domain->handle);
        ecrt_domain_state(domain->handle, domain->state);
        ecrt_domain_queue(domain->handle);
        for (conversion_list = domain->endian_convert_list;
                conversion_list->data_ptr;
                conversion_list++) {
            switch (conversion_list->pdo_datatype) {
                case si_uint16_T:
                case si_sint16_T:
                    __le16_to_cpus(conversion_list->data_ptr);
                    break;
                case si_uint32_T:
                case si_sint32_T:
                    __le32_to_cpus(conversion_list->data_ptr);
                    break;
                default:
                    /* Dont have singles and doubles in EtherCAT */
                    break;
            }
        }
//        if (!(count++ % 1000))
//            pr_debug("ecs_receive %u\n", *(char*)domain->io_data);
    }
}

/* Do EtherCAT output processing for a RTW task. It does the following:
 *      - calls ecrt_master_run() for every master whose fastest task
 *        domain is in this task
 *      - calls ecrt_master_send() for every domain in this task
 */
void
ecs_send(int tid) 
{
    struct ecat_master *master = ecat_data->st[tid].master;
    struct ecat_master * const master_end = 
        master + ecat_data->st[tid].master_count;
    struct ecat_domain *domain = ecat_data->st[tid].output_domain;
    struct ecat_domain * const domain_end = 
        domain + ecat_data->st[tid].output_domain_count;
    struct endian_convert_t *conversion_list;


    for ( ; domain != domain_end; domain++) {
        for (conversion_list = domain->endian_convert_list; 
                conversion_list->data_ptr;
                conversion_list++) {
            switch (conversion_list->pdo_datatype) {
                case si_uint16_T:
                case si_sint16_T:
                    __cpu_to_le16s(conversion_list->data_ptr);
                    break;
                case si_uint32_T:
                case si_sint32_T:
                    __cpu_to_le32s(conversion_list->data_ptr);
                    break;
                default:
                    break;
            }
        }
        ecrt_domain_process(domain->handle);
        ecrt_domain_state(domain->handle, domain->state);
        ecrt_domain_queue(domain->handle);
    }

    for ( ; master != master_end; master++) {
        ecrt_master_send(master->handle);
    }
}


unsigned int
get_addr_incr(enum si_datatype_t dtype)
{
    switch (dtype) {
        case si_uint32_T:
        case si_sint32_T:
            return 4;
        case si_uint16_T:
        case si_sint16_T:
            return 2;
        case si_uint8_T:
        case si_sint8_T:
            return 1;
        default:
            return 0;
    }
}

/* Initialise all slaves listed in master->slave_list. */
static const char* __init
init_slaves( struct ecat_master *master)
{
    struct ecat_slave *slave;
    ec_slave_config_t *slave_config;
    const struct sdo_config *sdo;
    const char* failed_method;
    struct mapped_pdo *mapped_pdo;
    ListElmt           *element;
    int i;
    element = list_head(&master->slave_list); 
    for(i=0;i< list_size(&master->slave_list);i++)
      {
	if(i>0)
	  {
	    element = list_next(element);
	  }
	slave = (struct ecat_slave *)list_data(element);

        pr_debug("Considering slave %p %u:%u on master %u(%p)\n", 
                slave,
                slave->alias, slave->position,
                master->id, master->handle);
        slave_config = ecrt_master_slave_config( master->handle, slave->alias,
                slave->position, slave->vendor_id, slave->product_code);
        if (!slave_config) {
            failed_method = "ecrt_master_slave_config";
            goto out_slave_failed;
        }

        /* Inform EtherCAT of how the slave configuration is expected */
        if (ecrt_slave_config_pdos(slave_config, 
                    EC_END, slave->sync_info)) {
            failed_method = "ecrt_slave_config_pdos";
            goto out_slave_failed;
        }

        /* Send SDO configuration to the slave */
        for (sdo = slave->sdo_config; 
                sdo != &slave->sdo_config[slave->sdo_config_count]; sdo++) {
            switch (sdo->datatype) {
                case si_uint8_T:
                    if (ecrt_slave_config_sdo8(slave_config, 
                                sdo->sdo_index, sdo->sdo_subindex,
                                (uint8_t)sdo->value)) {
                        failed_method = "ecrt_slave_config_sdo8";
                        goto out_slave_failed;
                    }
                    break;
                case si_uint16_T:
                    if (ecrt_slave_config_sdo16(slave_config, 
                                sdo->sdo_index, sdo->sdo_subindex,
                                (uint16_t)sdo->value)) {
                        failed_method = "ecrt_slave_config_sdo16";
                        goto out_slave_failed;
                    }
                    break;
                case si_uint32_T:
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

        /* Now map the required PDO's into the process image. The offset
         * inside the image is returned by the function call */
        for (mapped_pdo = slave->mapped_pdo;
                mapped_pdo != &slave->mapped_pdo[slave->pdo_map_count]; 
                mapped_pdo++) {
            const struct pdo_map *mapping = mapped_pdo->mapping;
            int offset = ecrt_slave_config_reg_pdo_entry(
                        slave_config,
                        mapping->pdo_entry_index,
                        mapping->pdo_entry_subindex,
                        mapped_pdo->domain->handle,
                        mapping->bitoffset);
            if (offset < 0) {
                failed_method = "ecrt_slave_config_reg_pdo_entry";
                goto out_slave_failed;
            }
            mapped_pdo->base_offset = offset;
            if (mapping->bitoffset) 
                pr_debug("Bit Offset %p %u: %u\n", 
                        mapping->bitoffset, slave->position, 
                        *mapping->bitoffset);
        }
    }

    return 0;

out_slave_failed:
    snprintf(errbuf, sizeof(errbuf), 
            "EtherCAT %s() failed "
            "for slave #%u:%u on master %u", failed_method,
            slave->alias, slave->position, master->id);
    return errbuf;

}


/* An internal function to return a master pointer.
 * If the master does not exist, one is created */
struct ecat_master * __init
get_master(
        unsigned int master_id,
        unsigned int tid,

        const char **errmsg
        )
{
    struct ecat_master *master;
    unsigned int i;
    ptrdiff_t len;
    ListElmt           *element;
    int j;
    /* Is the master registered? */
    element = list_head(&ecat_data->master_list); 
    for(j=0;j< list_size(&ecat_data->master_list);j++)
      {
	if(j>0)
	  {
	    element = list_next(element);
	  }
	master = (struct ecat_master *)list_data(element);
        if (master->id == master_id) {
	  /* An EtherCAT master device is assigned to the fastest task
	   * in the system. All other tasks just queue their
	   * packets, waiting to be sent by the fastest task.
	   *
	   * When these code lines are reached, a master structure has 
	   * already been created. Now just check whether the new new
	   * tast period isn't maybe faster than the one the master is
	   * currently assigned to, changing the management variables
	   * if this is the case */
	  if (ecat_data->st[master->fastest_tid].period > 
	      ecat_data->st[tid].period) {
	    
	    /* Tell the previous task that it has one less master */
	    ecat_data->st[master->fastest_tid].master_count--;
	    
	    /* Tell the new that task that it has a new master */
	    ecat_data->st[tid].master_count++;
	    master->fastest_tid = tid;
	  }
	  return master;
        }
      }
    
    /* Master with id master_id does not exist - create it */
    
    /* Although master is not initialised here, it is not dereferenced,
     * only address operators are being used. */
    len = (void*)&master->st_domain[ecat_data->nst] - (void*)master;
    if (!(master = my_kcalloc(1, len, "master"))) {
        *errmsg = no_mem_msg;
        return NULL;
    }
     list_ins_next(&ecat_data->master_list,list_tail(&ecat_data->master_list),(void *)master);

    /* Initialise variables */
    master->id = master_id;
    for (i = 0; i < ecat_data->nst; i++) {
      list_init(&master->st_domain[i].input_domain_list, free);
      list_init(&master->st_domain[i].output_domain_list, free);
    }
    list_init(&master->slave_list,free);
    master->fastest_tid = tid;
    ecat_data->st[tid].master_count++;

    master->state = my_kcalloc(1, sizeof(*master->state), "master state");
    if (!master->state) {
        *errmsg = no_mem_msg;
        return NULL;
    }

    pr_debug("Requesting master %i\n", master->id);
    master->handle = ecrt_request_master(master->id);
    pr_debug("\t%p = ecrt_request_master(%i)\n",
            master->handle, master->id);
    if (!master->handle) {
        snprintf(errbuf, sizeof(errbuf), 
                "EtherCAT Master %u initialise failed.",
                master->id);
        *errmsg = errbuf;
        return NULL;
    }

    return master;
}


/* An internal function to return a domain pointer of a master with sample
 * time tid.
 * If the domain does not exist, one is created */
struct ecat_domain * __init
get_domain(
        struct ecat_master *master,
        unsigned int domain_id,
        ec_direction_t direction,
        unsigned int tid,

        const char **errmsg
        )
{
    struct ecat_domain *domain;
    List *domain_list;
    unsigned int *domain_counter;
    ListElmt           *element;
    int j;

    /* Is the domain registered - if not, create it
     * The master maintains a separate list for the read and write 
     * directions for every sample time */
    switch (direction) {
        case EC_DIR_OUTPUT:
            domain_list = &master->st_domain[tid].output_domain_list;
            domain_counter = &ecat_data->st[tid].output_domain_count;
            break;
        case EC_DIR_INPUT:
            domain_list = &master->st_domain[tid].input_domain_list;
            domain_counter = &ecat_data->st[tid].input_domain_count;
            break;
        default:
            snprintf(errbuf, sizeof(errbuf), 
                    "Unknown ec_direction_t used for domain %u, tid %i.",
                    domain_id, tid);
            *errmsg = errbuf;
            return NULL;
    };

    /* Is the master registered? */
    element = list_head(domain_list); 
    for(j=0;j< list_size(domain_list);j++)
      {
	if(j>0)
	  {
	    element = list_next(element);
	  }
	domain = (struct ecat_domain *)list_data(element);
        if (domain->id == domain_id) {
            /* Yes, the domain has been registered once before */
            pr_debug("%s domain %u:%u for tid %u exists\n",
                    direction == EC_DIR_OUTPUT ? "Output" : "Input",
                    master->id, domain_id, tid);
            return domain;
        }
    }

    /* This is a new domain. Allocate memory */
    if (!(domain = my_kcalloc(1, sizeof(struct ecat_domain), "domain"))) {
        *errmsg = no_mem_msg;
        return NULL;
    }
    list_ins_next(domain_list,list_tail(domain_list),(void *)domain);

    /* Initialise variables */
    domain->tid = tid;
    domain->master = master;
    domain->id = domain_id;
    (*domain_counter)++;

    /* Memory for the domain's state */
    domain->state = my_kcalloc(1, sizeof(*domain->state), "domain state");
    if (!domain->state) {
        *errmsg = no_mem_msg;
        return NULL;
    }

    /* Let the EtherCAT Driver create a domain */
    domain->handle = ecrt_master_create_domain(master->handle);
    pr_debug("\t%p = ecrt_master_create_domain(%p)\n",
            domain->handle, master->handle);
    if (!domain->handle) {
        snprintf(errbuf, sizeof(errbuf), 
                "EtherCAT Domain initialise on Master %u failed.",
                master->id);
        *errmsg = errbuf;
        return NULL;
    }

    pr_debug("New %s domain %u:%u for tid %u created\n",
            direction == EC_DIR_OUTPUT ? "output" : "input",
            master->id, domain_id, tid);

    return domain;

}

/*
 * Register a slave.
 *
 * This function is used by the RTW Model to registers a single slave.
 * This is the first function that has to be called when dealing with complex
 * slaves - those that can be parameterised, etc.
 */
const char* __init
ecs_reg_slave(
        unsigned int tid,
        unsigned int master_id,
        unsigned int domain_id,

        uint16_t slave_alias,
        uint16_t slave_position,
        uint32_t vendor_id, /**< vendor ID */
        uint32_t product_code, /**< product code */

        unsigned int sdo_config_count,
        const struct sdo_config *sdo_config,

        const ec_sync_info_t *sync_info,

        unsigned int pdo_count,
        const struct pdo_map *pdo_map
        )
{
    const char *errmsg;
    struct ecat_master *master;
    struct ecat_domain *input_domain = NULL;
    struct ecat_domain *output_domain = NULL;
    struct ecat_slave  *slave = NULL;
    size_t len;
    int i;
    pr_debug( "ecs_reg_slave( tid = %u, master_id = %u, domain_id = %u, "
        "slave_alias = %hu, slave_position = %hu, vendor_id = 0x%x, "
        "product_code = 0x%x, sdo_config_count = %u, *sdo_config = %p, "
        "*sync_info = %p, pdo_count = %u, *pdo_map = %p )\n",
        tid, master_id, domain_id,
        slave_alias, slave_position, vendor_id,
        product_code, sdo_config_count, sdo_config,
        sync_info, pdo_count, pdo_map
        );

    /* Get a pointer to the master, creating a new one if it does not
     * exist already */
    if (!(master = get_master(master_id, tid, &errmsg)))
        return errmsg;

    /* Get memory for the slave structure */
    len = (void*)&slave->mapped_pdo[pdo_count] - (void*)slave;
    slave = my_kcalloc(1, len, "slave");
    if (!slave) {
        goto out_slave_alloc;
    }
    list_ins_next(&master->slave_list,list_tail(&master->slave_list),(void *)slave);

    /* Slave identification */
    slave->alias = slave_alias;
    slave->position = slave_position;
    slave->vendor_id = vendor_id;
    slave->product_code = product_code;

    /* Copy the SDO's locally */
    slave->sdo_config = sdo_config;
    slave->sdo_config_count = sdo_config_count;

    /* Copy the PDO structures locally */
    slave->sync_info = sync_info;

    /* The pdo lists the PDO's that should be mapped
     * into the process image and also contains pointers where the data
     * should be copied, as well as the data type. 
     * Here we go through the list assigning the mapped PDO to the correct
     * domain depending on whether it is an input or output */
    slave->pdo_map_count = pdo_count;
    for (i = 0; i < pdo_count; i++) {
        struct ecat_domain *domain;
        pr_debug("Doing pdo_mapping %i\n", i);
        slave->mapped_pdo[i].mapping = &pdo_map[i];
        switch (pdo_map[i].dir) {
            /* PDO Input as seen by the slave, i.e. block inputs. The
             * corresponding slaves in the field are outputs, 
             * eg. analog and digital outputs */
            case EC_DIR_INPUT: 
                if (!input_domain) {
                    input_domain = get_domain(master, domain_id, 
                            EC_DIR_INPUT, tid, &errmsg);
                    if (!input_domain) {
                        return errmsg;
                    }
                }
                domain = input_domain;
                pr_debug("Selecting Input Domain for PDO Entry #x%04X\n",
                        pdo_map[i].pdo_entry_index);
                break;

            /* PDO Output as seen by the slave, i.e. block outputs. The
             * corresponding slaves in the field are inputs, 
             * eg. analog and digital inputs */
            case EC_DIR_OUTPUT: 
                if (!output_domain) {
                    output_domain = get_domain(master, domain_id, 
                            EC_DIR_OUTPUT, tid, &errmsg);
                    if (!output_domain) {
                        return errmsg;
                    }
                }
                domain = output_domain;
                pr_debug("Selecting Output Domain for PDO Entry #x%04X\n",
                        pdo_map[i].pdo_entry_index);
                break;

            default:
                return "Unknown PDO Direction encountered.";
        }
        slave->mapped_pdo[i].domain = domain;
        domain->io_count +=
            get_addr_incr(pdo_map[i].pdo_datatype) > 1
            ? pdo_map[i].vector_len : 0;
    }

    return NULL;

out_slave_alloc:
    return no_mem_msg;
}

ec_master_t * __init 
ecs_get_master_ptr(unsigned int master_id, const char **errmsg)
{
    struct ecat_master *master;

    /* Get the master structure, making sure not to change the task it 
     * is currently assigned to */
    if (!(master = get_master(master_id, ecat_data->nst - 1, errmsg)))
        return NULL;

    return master->handle;
}

ec_domain_t * __init 
ecs_get_domain_ptr(unsigned int master_id, unsigned int domain_id, 
        ec_direction_t dir, unsigned int tid, const char **errmsg)
{
    struct ecat_master *master;
    struct ecat_domain *domain;

    /* Get the master structure, making sure not to change the task it 
     * is currently assigned to */
    if (!(master = get_master(master_id, ecat_data->nst - 1, errmsg)))
        return NULL;

    if (!(domain = get_domain(master, domain_id, dir, tid, errmsg)))
        return NULL;

    return domain->handle;
}

void __init
cleanup_mem(void)
{
    struct ecat_master *master, *m;
    struct ecat_domain *domain, *d;
    struct ecat_slave  *slave,  *s;
    int i;
    ListElmt           *element_master;
    int j;

    // Free all the memory allocated for master and domain structures
    // Note that kfree(NULL) is allowed.
    element_master = list_head(&ecat_data->master_list); 
    for(j=0;j< list_size(&ecat_data->master_list);j++)
      {
	if(j>0)
	  element_master = list_next(element_master);
	master = (struct ecat_master *)list_data(element_master);

        for (i = 0; i < ecat_data->nst; i++) {
	  list_destroy(&master->st_domain[i].input_domain_list);
	  list_destroy(&master->st_domain[i].output_domain_list);
        }
		list_destroy(&master->slave_list);
        // master->handle is NULL if the handle has already been transferred
        // to ecat_data->st[].master. It will be released there.
        if (master->handle)
            ecrt_release_master(master->handle);

        my_kfree(master, "master");
    }

    // Close the list so that it is not cleaned up again
    //list_destroy(&ecat_data->master_list);
}

const char * __init
init_domains(struct ecat_master *master, struct ecat_master *new_master) 
{
    unsigned int i;
    struct ecat_domain *domain;
    ListElmt           *element;
    int j;
    /* Loop through all of the masters sample time domains domains */
    for (i = 0; i < ecat_data->nst; i++) {
        unsigned int tid, domain_idx;
	
        /* Initialise all slaves in the input domain */
	
	element = list_head(&master->st_domain[i].input_domain_list); 
	for(j=0;j< list_size(&master->st_domain[i].input_domain_list);j++)
	  {
	    if(j>0)
	      element = list_next(element);
	    domain = (struct ecat_domain *)list_data(element);
            /* The list has to be zero terminated */
            domain->endian_convert_list = 
                my_kcalloc(domain->io_count + 1,
			   sizeof(*domain->endian_convert_list),
			   "input copy list");
            if (!domain->endian_convert_list)
	      return no_mem_msg;
	    
	    
	    domain->io_data = ecrt_domain_data(domain->handle);
            if (!domain->io_data)
	    return no_mem_msg;


            /* Move the domain to the sample time array */
            tid = domain->tid;
            domain_idx = ecat_data->st[tid].input_domain_count++;
            domain->master = new_master;
            ecat_data->st[tid].input_domain[domain_idx] = *domain;
        }

        /* Initialise all slaves in the output domain */
	element = list_head(&master->st_domain[i].output_domain_list); 
	for(j=0;j< list_size(&master->st_domain[i].output_domain_list);j++)
	  {
	    if(j>0)
	      element = list_next(element);
	    domain = (struct ecat_domain *)list_data(element);

            /* The list has to be zero terminated */
            domain->endian_convert_list = 
                my_kcalloc( domain->io_count + 1,
                        sizeof(*domain->endian_convert_list),
                        "output copy list");
            if (!domain->endian_convert_list)
                return no_mem_msg;


	    domain->io_data = ecrt_domain_data(domain->handle);
            if (!domain->io_data)
                return no_mem_msg;

            /* Move the domain to the sample time array */
            tid = domain->tid;
            domain_idx = ecat_data->st[tid].output_domain_count++;
            domain->master = new_master;
            ecat_data->st[tid].output_domain[domain_idx] = *domain;
        }
    }

    return NULL;
}

const char * __init
init_slave_iodata(struct ecat_slave *slave)
{
    struct mapped_pdo *mapped_pdo;
    unsigned int i;

    for (mapped_pdo = slave->mapped_pdo;
            mapped_pdo != &slave->mapped_pdo[slave->pdo_map_count];
            mapped_pdo++) {
        const struct pdo_map *pdo = mapped_pdo->mapping;
        char *data_ptr = 
            mapped_pdo->domain->io_data + mapped_pdo->base_offset;
        unsigned int bitposition = pdo->bitoffset ? *pdo->bitoffset : 0;
        struct endian_convert_t *endian_convert_list;
        unsigned int data_width = get_addr_incr(pdo->pdo_datatype);

        if (!data_width) {
            snprintf(errbuf, sizeof(errbuf),
                    "Error: do not know how to do endian "
                    "processing on datatype %u for "
                    "(slave #%u.%u)",
                    pdo->pdo_datatype,
                    slave->alias, slave->position);
            return  errbuf;
        }

        for (i = 0; i < pdo->vector_len; i++) {
	  printf("ECS %x\n",pdo->address);
            pdo->address[i] = data_ptr;
            pr_debug("Assigning io Address %u %p\n", i, pdo->address[i]);

            if (pdo->bitoffset) {
                pdo->bitoffset[i] = bitposition;

                bitposition += pdo->bitlen;
                data_ptr += bitposition / 8;
                bitposition %= 8;
            }
            else {
                data_ptr += data_width;
            }

            if (data_width > 1) {
                /* Note: mapped_pdo->domain is still a pointer to 
                 * the old domain structure (the one in the linked
                 * list), not the one in the sample time structure 
                 * (ecat_data->st[].input_domain). Since this will be 
                 * thrown away soon, we can misuse its 
                 * endian_convert_list pointer as a counter. */
                endian_convert_list = 
                    mapped_pdo->domain->endian_convert_list++;

                endian_convert_list->data_ptr = data_ptr;
                endian_convert_list->pdo_datatype = pdo->pdo_datatype;
            }
        }
    } 
    return NULL;
}

/*
 * Start EtherCAT driver.
 *
 * This function concludes the caching of all slave initialisation and 
 * registration items. Now the EtherCAT driver can be contacted to activate
 * the subsystem.
 */
const char * __init
ecs_start(void)
{
    struct ecat_master *master;
    unsigned int master_tid = 0;
    struct st_data *st;
    ListElmt           *element;
    int j;
    /* Allocate space for all the master and domain structures */
    for( st = ecat_data->st; st != &ecat_data->st[ecat_data->nst]; st++) {
        /* Although some of the *_count can be zero, kzalloc is able to 
         * deal with it. kzalloc returns ZERO_SIZE_PTR in this case.
         * kfree(ZERO_SIZE_PTR) is allowed! */

        st->master = 
            my_kcalloc(st->master_count, sizeof(*st->master), "master array");
        if (!st->master)
            return no_mem_msg;
        st->master_count = 0;

        st->input_domain = my_kcalloc(st->input_domain_count,
                sizeof(*st->input_domain), "input domain array");
        if (!st->input_domain)
            return no_mem_msg;
        st->input_domain_count = 0;

        st->output_domain = my_kcalloc(st->output_domain_count,
                sizeof(*st->output_domain), "output domain array");
        if (!st->output_domain)
            return no_mem_msg;
        st->output_domain_count = 0;

        /* Reset the counters. These are incremented later on again when
         * each master and domain structure in the arrays are initialised */
    }

    /* Go though the master list one by one and register the slaves */
    element = list_head(&ecat_data->master_list); 
    for(j=0;j< list_size(&ecat_data->master_list);j++)
      {
        unsigned int master_idx;
        struct ecat_master *new_master;
        struct ecat_slave *slave;
        const char *err;
	ListElmt           *element_slave;
	int k;

	if(j>0)
	  element = list_next(element);
	master = (struct ecat_master *)list_data(element);

        /* Find out to which tid this master will be allocated. The fastest
         * tid will get it */
        master_tid = master->fastest_tid;
        master_idx = ecat_data->st[master_tid].master_count++;
        new_master = &ecat_data->st[master_tid].master[master_idx];

        if ((err = init_slaves(master)))
            return err;

        /* Pass the responsibility from the master in the list structure
         * to the one in the array */
        *new_master = *master;
        pr_debug ("ecrt_master_activate(%p)\n", new_master->handle);
        if (ecrt_master_activate(new_master->handle)) {
            snprintf(errbuf, sizeof(errbuf),
                    "Master %i activate failed", new_master->id);
            return errbuf;
	}
        if ((err = init_domains(master, new_master)))
            return err;

        /* Now that the masters are activated, we can go through the
         * slave list again and setup the endian_convert_list in the domains
         * that the slave is registered in */
	element_slave = list_head(&master->slave_list); 
	for(k=0;k< list_size(&master->slave_list);k++)
	  {
	    if(k>0)
	      element_slave = list_next(element_slave);
	    slave = (struct ecat_slave *)list_data(element_slave);
            if ((err = init_slave_iodata(slave)))
	      return err;
	  }
        
	

        /* Pass the responsibility from the master in the list structure
         * to the one in the array */
        *new_master = *master;
        master->handle = NULL;

    }

    /* Release all temporary memory */
    cleanup_mem();

    return NULL;
}


void ecs_end(void)
{
    struct st_data *st;

    pr_debug("%s()\n", __func__);

    if (!ecat_data)
        return;

    if (!list_size(&ecat_data->master_list) > 0)
        cleanup_mem();

    for (st = ecat_data->st; st != &ecat_data->st[ecat_data->nst]; st++) {
        struct ecat_master *master;
        struct ecat_domain *domain;

        /* Free all allocated memory. Note that kfree(NULL) as well as
         * kfree(ZERO_SIZE_PTR), i.e. pointers returned by kmalloc(0), are
         * explicitly allowed */

        /* Free masters */
        for (master = st->master; 
                master && master != &st->master[st->master_count]; 
                master++) {
            if (master->handle) {
                ecrt_release_master(master->handle);
            }
            my_kfree(master->state, "master state");
        }
        my_kfree(st->master, "master array");

    }

    my_kfree(ecat_data, "ecat_data");
}

/* 
 * Initialise all data structures in this file.
 *
 * This function has to be called before any anything else in this file. Data
 * structures are initialised and the sample time domains are prepared.
 * Compatability with the EtherCAT interface is also checked.
 */
const char * __init
ecs_init( 
        unsigned int *st    /* a 0 terminated list of sample times */
        )
{
    unsigned int nst;       /* Number of sample times */
    size_t len;
    int i;

    /* Make sure that the correct header version is used */
#if ECRT_VERSION_MAGIC != ECRT_VERSION(1,5)
#error Incompatible EtherCAT header file found.
#error This source is only compatible with EtherCAT Version 1.5
#endif

    /* Make sure that the EtherCAT driver has the correct interface */
    if (ECRT_VERSION_MAGIC != ecrt_version_magic()) {
        snprintf(errbuf, sizeof(errbuf), 
                "EtherCAT driver version 0x%02x is not supported. "
                "Expecting version 0x%02x", 
                ecrt_version_magic(), ECRT_VERSION_MAGIC);
        return errbuf;
    }

    /* Count how many sample times there are. The list is zero terminated */
    for (nst = 0; st[nst]; nst++);
    pr_debug("Number of sample times: %u\n", nst);

    /* Get memory for the data */
    len = (void*)&ecat_data->st[nst] - (void*)ecat_data;
    ecat_data = my_kcalloc(1, len, "ecat_data");
    if (!ecat_data) {
        return no_mem_msg;
    }

    /* Set default values for all variables */
    ecat_data->nst = nst;
    list_init(&ecat_data->master_list, free);

    /* Set the period for all sample times */
    for ( i = 0; i < nst; i++ )
        ecat_data->st[i].period = st[i];

    return NULL;
}

unsigned int etl_strlen(const char *str) {
    return strlen(str);
}

extern char *etl_strrchr(const char *str, int c) {
    return strrchr(str, c);
}

extern char *etl_strncpy(char *target, const char *source, unsigned int n) {
    return strncpy(target, source, n);
}

