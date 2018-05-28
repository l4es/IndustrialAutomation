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

#include <ecrt_support_simply.h>
#include <stdio.h>

struct ecat_simply {
  uint16_t work_index;
  uint16_t init_index;
  uint16_t do_init;
  uint16_t do_start;
  uint16_t do_end;
};

struct ecat_simply simply_data = {0,0,1,1,1};



const char* ecs_simply_send(void)
{
  const char* errvalue = NULL;

  if(simply_data.do_start == 1)
    {
      errvalue = ecs_start();
      if(errvalue == NULL)
	{
	  printf("ECS-Support Start successfully!\n");
	  simply_data.do_start = 0;
	}
      else
	{
	  simply_data.do_start = 1;
	  simply_data.do_init = 1;
	  return errvalue;
	}
    }

  if(simply_data.work_index == simply_data.init_index)
  {
    ecs_send(0);
    simply_data.work_index = 0;
  }

  return errvalue;
}

const char* ecs_simply_receive(void)
{
  const char* errvalue = NULL;

  if(simply_data.do_start == 1)
    {
      errvalue = ecs_start();
      if(errvalue == NULL)
	{
	  printf("ECS-Support Start successfully!\n");
	  simply_data.do_start = 0;
	}
      else
	{
	  simply_data.do_start = 1;
	  simply_data.do_init = 1;
	  return errvalue;
	}
    }

  if(simply_data.work_index == 0)
    {
      ecs_receive(0);
    }
  simply_data.work_index++;
  
  return errvalue;
}

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
        )
{
  const char* errvalue;


  if(simply_data.do_init == 1)
    {
      unsigned int ec_st[] = {
	100, //Dummy Value
	0 };

      simply_data.init_index = 0;
      simply_data.work_index = 0;
      simply_data.do_start = 1;
      simply_data.do_end = 1;
      errvalue = ecs_init(ec_st);
      if(errvalue == NULL)
	{
	  printf("ECS-Support Init successfully!\n");
	  simply_data.do_init = 0;
	}
      else
	{
	  simply_data.do_init = 1;
	  return errvalue;
	}
    }

  simply_data.init_index++;

  return ecs_reg_slave(0,       /**< Task id of the task this slave will 
                                     run in */
		       master_id, /**< Master id this slave is connected to */
		       domain_id, /**< Domain id this slave should be assigned
                                     to */
		       slave_alias,   /**< Alias number as the base from which 
                                     the position is determined */
		       slave_position,/**< EtherCAT location of the slave in the 
                                     bus */
		       vendor_id,     /**< vendor ID */
		       product_code,  /**< product code */
		       sdo_config_count, /**< The number of SDO configuration 
                                          objects passed in \a sdo_config */
		       sdo_config, /**< Slave configuration objects */
		       pdo_info, /**< PDO Configuration objects */
		       pdo_count, /**< Number of PDO mapping objects 
                                             passed in \a pdo */
		       pdo /**< PDO mapping objects */
		       );
}


void ecs_simply_end(void)
{
  if(simply_data.do_end == 1)
    {
      simply_data.work_index = 0;
      simply_data.do_end = 0;
    }
  simply_data.work_index++;

  if((simply_data.do_end == 0) && (simply_data.work_index == simply_data.init_index))
    {
      ecs_end();
      simply_data.work_index = 0;
      simply_data.init_index = 0;
      simply_data.do_init = 1;
      simply_data.do_start = 1;
      simply_data.do_end = 1;
    }
}
