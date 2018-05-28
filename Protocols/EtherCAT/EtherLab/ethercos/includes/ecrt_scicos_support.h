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

#include <inttypes.h>
#define __init

#include <ecrt.h>
#include <etl_scicos_data_info.h>


#if ECRT_VERSION_MAGIC < ECRT_VERSION(1, 5)
# error "Incompatible EtherCAT header!"
#endif

struct pdo_map {
    uint16_t pdo_entry_index;
    uint8_t pdo_entry_subindex;
    unsigned int vector_len;
    ec_direction_t dir;

    enum si_datatype_t pdo_datatype;
    unsigned int bitlen;
    void **address;
    unsigned int *bitoffset;
};

/* Structure to temporarily store SDO objects prior to registration */
struct sdo_config {
    /* The data type of the sdo: 
     * only si_uin8_t, si_uint16_t, si_uint32_t are allowed */
    enum si_datatype_t datatype;

    /* SDO values. Used by EtherCAT functions */
    uint16_t sdo_index;
    uint8_t sdo_subindex;
    uint32_t value;
};

void ecs_send(int tid);
void ecs_receive(int tid);
const char *ecs_start(void);

const char*
__init ecs_reg_slave(
        unsigned int tid,       /**< Task id of the task this slave will 
                                     run in */
        unsigned int master_id, /**< Master id this slave is connected to */
        unsigned int domain_id, /**< Domain id this slave should be assigned
                                     to */

        uint16_t slave_alias,   /**< Alias number as the base from which 
                                     the position is determined */
        uint16_t slave_position,/**< EtherCAT location of the slave in the 
                                     bus */
        uint32_t vendor_id,     /**< vendor ID */
        uint32_t product_code,  /**< product code */

        unsigned int sdo_config_count, /**< The number of SDO configuration 
                                          objects passed in \a sdo_config */
        const struct sdo_config *sdo_config, /**< Slave configuration objects */

        const ec_sync_info_t *pdo_info, /**< PDO Configuration objects */

        unsigned int pdo_count, /**< Number of PDO mapping objects 
                                             passed in \a pdo */
        const struct pdo_map *pdo /**< PDO mapping objects */
        );

ec_master_t *ecs_get_master_ptr(
        unsigned int master_id, 
        const char **errmsg);

ec_domain_t *ecs_get_domain_ptr(
        unsigned int master_id, 
        unsigned int domain_id, 
        ec_direction_t dir,
        unsigned int tid,
        const char **errmsg);

void ecs_end(void);

const char *ecs_init( 
        unsigned int *st       /* Zero terminated list of sample times in 
                                 * microseconds */
        );
