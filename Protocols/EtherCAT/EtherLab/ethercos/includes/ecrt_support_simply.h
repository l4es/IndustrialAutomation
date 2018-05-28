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

#ifndef __ECRT_SUPPORT_SIMPLY_H__
#define __ECRT_SUPPORT_SIMPLY_H__

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <inttypes.h>
#endif

#include <ecrt.h>
#include <etl_scicos_data_info.h>
#include <ecrt_scicos_support.h>



const char* ecs_simply_send(void);
const char* ecs_simply_receive(void);

const char* ecs_simply_reg_slave(
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



void ecs_simply_end(void);


#endif //__ECRT_SUPPORT_SIMPLY_H__
