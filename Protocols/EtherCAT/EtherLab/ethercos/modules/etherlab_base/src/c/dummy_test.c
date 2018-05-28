/**-----------------------------------------------------------------------------------
 *  Copyright (C) 2008  IgH Essen, Andreas Stewering-Bone
 *
 *  This file is part of the Etherlab Project.
 *
 *  Etherlab is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Etherlab is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Etherlab; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *--------------------------------------------------------------------------------- */


//#include <machine.h>
//#include <scicos_block4.h>
#include <ecrt_slaves.h>
#include <ecrt_support_simply.h>
#include <stdio.h>
#include <unistd.h>


#define ANZ_OUT 2
#define MAX_SIGNAL 10.0

struct ADCOMDev{
  unsigned int master_id;
  unsigned int domain_id;
  uint16_t slave_alias;
  uint16_t slave_position;
  uint32_t vendor_id; /**< vendor ID */
  uint32_t product_code; /**< product code */
  unsigned int sdo_config_count;
  const struct sdo_config *sdo_config;
  const ec_sync_info_t *sync_info;
  unsigned int pdo_count;
  struct pdo_map *pdo_map;
  void * address[ANZ_OUT];
};


  



static int init(struct ADCOMDev ** cd)
{
  struct ADCOMDev * comdev = (struct ADCOMDev *) malloc(sizeof(struct ADCOMDev));

  const char* errvalue;







  comdev->master_id = 0; //Master Id
  comdev->domain_id = 0; // Domain Id
  comdev->slave_alias = 0; // Slave Alias
  comdev->slave_position = 1; // Slave Position
  comdev->vendor_id = Beckhoff_VendorId;
  comdev->product_code = Beckhoff_EL3162_ProductCode;
  comdev->sdo_config_count = 0;
  comdev->sdo_config = NULL;
  comdev->sync_info = el3162_syncs;
  comdev->pdo_count = 2;
  comdev->pdo_map = (struct pdo_map *) malloc(comdev->pdo_count*sizeof(const struct pdo_map));
  
  
  comdev->pdo_map[0].pdo_entry_index =  0x3101;
  comdev->pdo_map[0].pdo_entry_subindex = 2;
  comdev->pdo_map[0].vector_len = 1;
  comdev->pdo_map[0].dir = EC_DIR_INPUT;
  comdev->pdo_map[0].pdo_datatype = si_sint16_T;
  comdev->pdo_map[0].bitlen = 16;
  comdev->pdo_map[0].address = &comdev->address[0];
  comdev->pdo_map[0].bitoffset = NULL;
  comdev->pdo_map[1].pdo_entry_index =  0x3102;
  comdev->pdo_map[1].pdo_entry_subindex = 2;
  comdev->pdo_map[1].vector_len = 1;
  comdev->pdo_map[1].dir = EC_DIR_INPUT;
  comdev->pdo_map[1].pdo_datatype = si_sint16_T;
  comdev->pdo_map[1].bitlen = 16;
  comdev->pdo_map[1].address = &comdev->address[1];
  printf(" %x\n",comdev->pdo_map[0].address);
  comdev->pdo_map[1].bitoffset = NULL;

  errvalue = ecs_simply_reg_slave(
		comdev->master_id,
		comdev->domain_id,
		comdev->slave_alias,
		comdev->slave_position,
		comdev->vendor_id, /**< vendor ID */
		comdev->product_code, /**< product code */
		comdev->sdo_config_count,
		comdev->sdo_config,
		comdev->sync_info,
		comdev->pdo_count,
		comdev->pdo_map);
  

  if(errvalue != NULL)
    {
      printf("Error Block EL3162, Blockno. 1: %s\n",errvalue);
      //set_block_error(-3); //internal_error
      //end_scicos_sim();
    }
  *cd = comdev;
  return 0;
}

static int inout(struct ADCOMDev * comdev)
{

  int i;
  double y;
  const char* errvalue;
  errvalue = ecs_simply_receive();

    if(errvalue == NULL)
    {
      y = MAX_SIGNAL*(double)(EC_READ_U16(comdev->address[0])/U16MAX);
      printf("Value = %10.4f V\n",y);
      errvalue = ecs_simply_send();
    }

  

  if(errvalue!=NULL)
    {
      printf("Error Block EL3162, Blockno.  1: %s\n",errvalue);
      //set_block_error(-3); //internal_error
      //end_scicos_sim();
    }
  
  return 0;
}

static int end(  struct ADCOMDev * comdev)
{
  int i;
  double *y;

  ecs_simply_end();
  free(comdev->pdo_map);
  free(comdev);
  return 0;
}

int main(void)
{
  struct ADCOMDev * comdev;
  int i;
  init(&comdev);
  //for(i=0;i<100;i++)
  while(1)
    {
      inout(comdev);
      usleep(10000);
    }
  end(comdev);

  return 0;
}


