/******************************************************************************
 *
 *  Copyright (C) 2008-2009  Andreas Stewering-Bone, Ingenieurgemeinschaft IgH
 *
 *  This file is part of the IgH EtherLAB Scicos Toolbox.
 *  
 *  The IgH EtherLAB Scicos Toolbox is free software; you can
 *  redistribute it and/or modify it under the terms of the GNU Lesser General
 *  Public License as published by the Free Software Foundation; version 2.1
 *  of the License.
 *
 *  The IgH EtherLAB Scicos Toolbox is distributed in the hope that
 *  it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the IgH EtherLAB Scicos Toolbox. If not, see
 *  <http://www.gnu.org/licenses/>.
 *  
 *  ---
 *  
 *  The license mentioned above concerns the source code only. Using the
 *  EtherCAT technology and brand is only permitted in compliance with the
 *  industrial property and similar rights of Beckhoff Automation GmbH.
 *
 *****************************************************************************/


#include <stdio.h>
#include <machine.h>
#include <math.h>
#include <scicos_block4.h>
#include <ecrt_support_simply.h>

#define BLOCKTYPE "ELxxxx"


#define GETVALUE(BLOCKNO,LOOP,OFFSET) ((int32_t*)Getint32OparPtrs(block,BLOCKNO))[LOOP*GetOparSize(block,BLOCKNO,1)+OFFSET]
#define GETVALUE_DOUBLE(BLOCKNO,LOOP,OFFSET) ((double *)GetRealOparPtrs(block,BLOCKNO))[LOOP*GetOparSize(block,BLOCKNO,1)+OFFSET]

//OPAR configuration
#define OPAR_SYNC 1
#define OPAR_PDO 2
#define OPAR_ENTRY 3
#define OPAR_SDO 4
#define OPAR_MAP 5
#define OPAR_GENERIC 6
#define OPAR_MAP_SCALE 7
//#define OPAR_ADDITIONAL 8

//OPAR Check Fix Sizes
#define OPAR_SYNC_SIZE 3
#define OPAR_PDO_SIZE 2
#define OPAR_ENTRY_SIZE 4
#define OPAR_SDO_SIZE 4
#define OPAR_MAP_SIZE 9
#define OPAR_GENERIC_SIZE 2
#define OPAR_MAP_SCALE_SIZE 2
//#define OPAR_ADDITIONAL_SIZE 2


//IPAR INDEX
#define DO_DEBUG 0
#define MASTER_ID 1
#define DOMAIN_ID 2
#define SLAVE_ALIAS 3
#define SLAVE_POSITION 4


struct channel_spec{
  void *address;
  unsigned int bitoffset;
  int valuetype;
  int channelno;
  int vectorlen;
  int direction;
  int typecode;
  double fullrange;
  double scale;
  double offset;
};

struct ADCOMDev{
  int channel_count;
  struct channel_spec *channel;
  ec_sync_info_t *sync_config;
  ec_pdo_info_t *pdo_config;
  ec_pdo_entry_info_t *pdo_entry; 
  struct sdo_config * sdo_config_values;
  struct pdo_map *pdo_map;
  int failure_flag;
  int debug_info;
};




enum si_datatype_t Scicos_to_CType(int datatyp)
{
  enum si_datatype_t tt[9] = {si_double_T ,si_single_T, si_uint8_T, si_sint8_T, si_uint16_T, si_sint16_T, si_uint32_T, si_sint32_T, si_boolean_T };
  
  return tt[datatyp-1];
}





static int init(scicos_block *block)
{
  struct ADCOMDev * comdev = (struct ADCOMDev *) scicos_malloc(sizeof(struct ADCOMDev));
  const char* errvalue = NULL;
  int i;
  ec_sync_info_t *sync_config;
  ec_pdo_info_t *pdo_config;
  int  pdo_config_counter = 0;
  ec_pdo_entry_info_t *pdo_entry; 
  int pdo_entry_counter = 0;
  struct sdo_config * sdo_config_values;
  struct pdo_map *pdo_map;
  struct channel_spec *channel_spec_values;



  //Set Working Vector
  *block->work=(void *)comdev;
  //Set Failure Flag
  comdev->failure_flag = 0; //Status Ok
  comdev->debug_info = (int)GetIparPtrs(block)[DO_DEBUG]; //Set high for Channel Debug information after full ethercat bus initialisation

  if(GetNopar(block)!=7)
    {
      printf("%s:OPAR Interface Error\n",BLOCKTYPE);
      goto out_failure;
    }

  

  if((GetOparSize(block,OPAR_SYNC,1)!=OPAR_SYNC_SIZE)||(GetOparSize(block,OPAR_PDO,1)!=OPAR_PDO_SIZE)||(GetOparSize(block,OPAR_ENTRY,1)!=OPAR_ENTRY_SIZE)||(GetOparSize(block,OPAR_MAP,1)!=OPAR_MAP_SIZE)||(GetOparSize(block,OPAR_GENERIC,1)!=OPAR_GENERIC_SIZE)||(GetOparSize(block,OPAR_MAP_SCALE,1)!=OPAR_MAP_SCALE_SIZE))
    {
      printf("%s:OPAR Interface Size Error\n",BLOCKTYPE);
      goto out_failure;
    }

  if(GetIparPtrs(block)[DO_DEBUG]==1)//DebugMode
    {
      printf("%s:------------------------------------------------------------------------\n",BLOCKTYPE);
      printf("%s:------------------------------------------------------------------------\n",BLOCKTYPE);
      printf("%s:------------------------------------------------------------------------\n",BLOCKTYPE);
      printf("%s:Init EL Scicos Block\n",BLOCKTYPE);
      //printf("%s:Number of OPAR %i \n",GetNopar(block));
      printf("%s:Sync Manager Configuration Count % i\n",BLOCKTYPE,GetOparSize(block,OPAR_SYNC,2));
      //printf("%s:Sync Manager Configuration Fix Entries % i\n",BLOCKTYPE,GetOparSize(block,OPAR_SYNC,1));
      printf("%s:Pdo Configuration Count % i\n",BLOCKTYPE,GetOparSize(block,OPAR_PDO,2));
      //printf("%s:Pdo Configuration Fix Entries % i\n",BLOCKTYPE,GetOparSize(block,OPAR_PDO,1));
      printf("%s:Pdo Entry Count % i\n",BLOCKTYPE,GetOparSize(block,OPAR_ENTRY,2));
      //printf("%s:Pdo Entry Fix Entries % i\n",BLOCKTYPE,GetOparSize(block,OPAR_ENTRY,1));
      printf("%s:Sdo Config Count % i\n",BLOCKTYPE,GetOparSize(block,OPAR_SDO,2));
      //printf("%s:Sdo Config Fix Entries % i\n",BLOCKTYPE,GetOparSize(block,OPAR_SDO,1));
      printf("%s:Pdo Mapping Count % i\n",BLOCKTYPE,GetOparSize(block,OPAR_MAP,2));
      //printf("%s:Pdo Mapping Fix Entries % i\n",BLOCKTYPE,GetOparSize(block,OPAR_MAP,1));
      //printf("%s:Generic Fix Entries % i\n",BLOCKTYPE,GetOparSize(block,OPAR_GENERIC,1));
      printf("%s:Pdo Mapping Scale Count % i\n",BLOCKTYPE,GetOparSize(block,OPAR_MAP_SCALE,2));
    }

  //Malloc configuration arrays
  sync_config = scicos_malloc((GetOparSize(block,OPAR_SYNC,2)+1)*sizeof(ec_sync_info_t));
  pdo_config = scicos_malloc(GetOparSize(block,OPAR_PDO,2)*sizeof(ec_pdo_info_t));
  pdo_entry = scicos_malloc(GetOparSize(block,OPAR_ENTRY,2)*sizeof(ec_pdo_entry_info_t));
  sdo_config_values = scicos_malloc(GetOparSize(block,OPAR_SDO,2)*sizeof(struct sdo_config));
  pdo_map = scicos_malloc(GetOparSize(block,OPAR_MAP,2)*sizeof(struct pdo_map));
  channel_spec_values = scicos_malloc(GetOparSize(block,OPAR_MAP,2)*sizeof(struct channel_spec));


  //fill comdev struct
  comdev->channel = channel_spec_values;
  comdev->channel_count = GetOparSize(block,OPAR_MAP,2);
  comdev->sync_config = sync_config;
  comdev->pdo_config = pdo_config;
  comdev->pdo_entry = pdo_entry;
  comdev->sdo_config_values = sdo_config_values;
  comdev->pdo_map = pdo_map;

  //Check Malloc Return
  if((sync_config == NULL)||(pdo_config ==NULL)||(pdo_entry == NULL)||(pdo_map == NULL)||(channel_spec_values == NULL))
    {
      printf("%s:Generic Malloc Failure\n",BLOCKTYPE);
      goto out_failure;
    }


  //Handle Syncmanager Configuration
  if(GetOparSize(block,OPAR_SYNC,2)>0)
    {
      for(i=0;i<GetOparSize(block,OPAR_SYNC,2);i++)
	{
	  if(GetIparPtrs(block)[DO_DEBUG]==1)//DebugMode
	    {  
	      printf("%s:Sync Manager PDO Counter %i Index %X Direction %i \n",BLOCKTYPE,GETVALUE(OPAR_SYNC,i,0),GETVALUE(OPAR_SYNC,i,1)-1,GETVALUE(OPAR_SYNC,i,2));
	    }
	  //Get Index
	  if((uint8_t)GETVALUE(OPAR_SYNC,i,1)> 0)
	    {
	      //Valid Entry
	      sync_config[i].index = (uint8_t)GETVALUE(OPAR_SYNC,i,1)-1;
	    }
	  else
	    {
	      //Dummy Entry
	      sync_config[i].index = 0;
	    }
	  
	  //Get Direction
	  switch(GETVALUE(OPAR_SYNC,i,2)) 
	    {
	    case 2:
	      //Master send to Slave
	      sync_config[i].dir = EC_DIR_OUTPUT;
	      break;
	    case 1:
	      //Slave send to Master
	      sync_config[i].dir = EC_DIR_INPUT;
	      break;
	    }
	  //Get PDO Number
	  sync_config[i].n_pdos = (unsigned int)GETVALUE(OPAR_SYNC,i,0);
	  //Set PDO Pointer
	  sync_config[i].pdos = &pdo_config[pdo_config_counter];
	  pdo_config_counter += sync_config[i].n_pdos;

	  if(GetIparPtrs(block)[DO_DEBUG]==1)//DebugMode
	    {
	      printf("%s:PDO COnfig Counter: %i\n",BLOCKTYPE,pdo_config_counter);
	    }
	  
	}
      sync_config[GetOparSize(block,OPAR_SYNC,2)].index = (uint8_t)EC_END;
    }
  else
    {
      printf("%s:Missing Sync manager Configuration\n",BLOCKTYPE);
      goto out_failure;
    }

  //Handle Pdo Configuration
  if(GetOparSize(block,OPAR_PDO,2)>0)
    {
      for(i=0;i<GetOparSize(block,OPAR_PDO,2);i++)
	{
	  if(GetIparPtrs(block)[DO_DEBUG]==1)//DebugMode
	    {
	      printf("%s:PDO Configuration Index %X Entry Count %i \n",BLOCKTYPE,GETVALUE(OPAR_PDO,i,0),GETVALUE(OPAR_PDO,i,1));
	    }
	  pdo_config[i].index = (uint16_t)GETVALUE(OPAR_PDO,i,0);
	  pdo_config[i].n_entries = GETVALUE(OPAR_PDO,i,1);
	  pdo_config[i].entries = &pdo_entry[pdo_entry_counter];
	  pdo_entry_counter += pdo_config[i].n_entries; 
	  if(GetIparPtrs(block)[DO_DEBUG]==1)//DebugMode
	    {
	      printf("%s:PDO Entry Counter: %i\n",BLOCKTYPE,pdo_entry_counter);
	    }
	}
    }
  else
    {
      printf("%s:Missing PDO Configuration\n",BLOCKTYPE);
      goto out_failure;
    }

  //Handle Pdo Entry Configuration
  if(GetOparSize(block,OPAR_ENTRY,2)>0)
    {
      for(i=0;i<GetOparSize(block,OPAR_ENTRY,2);i++)
	{
	  if(GetIparPtrs(block)[DO_DEBUG]==1)//DebugMode
	    {
	      printf("%s:PDO Entry Configuration Index %X Entry Subindex %X  Bitlen %i \n",BLOCKTYPE,GETVALUE(OPAR_ENTRY,i,0),GETVALUE(OPAR_ENTRY,i,1),GETVALUE(OPAR_ENTRY,i,2));
	    }
	  pdo_entry[i].index = (uint16_t)GETVALUE(OPAR_ENTRY,i,0);
	  pdo_entry[i].subindex = (uint8_t)GETVALUE(OPAR_ENTRY,i,1);
	  pdo_entry[i].bit_length = (uint8_t)GETVALUE(OPAR_ENTRY,i,2);
	  
	}
    }
  else
    {
      printf("%s:Missing PDO Entry Configuration \n",BLOCKTYPE);
      goto out_failure;
    }

  //Handle SDO Config
  if(GetOparSize(block,OPAR_SDO,2)>0)
    {
      if(sdo_config_values == NULL)
	{
	  printf("%s:SDO Malloc Failure\n",BLOCKTYPE);
	  goto out_failure;
	}

      if((GetOparSize(block,OPAR_SDO,1)!=OPAR_SDO_SIZE))
	{
	  printf("%s:SDO Configuration Failure\n",BLOCKTYPE);
	  goto out_failure;
	}

      for(i=0;i<GetOparSize(block,OPAR_SDO,2);i++)
	{
	  if(GetIparPtrs(block)[DO_DEBUG]==1)//DebugMode
	    {
	      //SDO Config
	      //Index Subindex Datatype Value
	      //Datatype 1:si_uin8_t, 2:si_uint16_t, 3:si_uint32_t
	      printf("%s:SDO Configuration Type % i Index %X Entry Subindex %X  Value %i \n",BLOCKTYPE,GETVALUE(OPAR_SDO,i,2),GETVALUE(OPAR_SDO,i,0),GETVALUE(OPAR_SDO,i,1),GETVALUE(OPAR_SDO,i,3));
	    }
	  sdo_config_values[i].datatype = Scicos_to_CType(GETVALUE(OPAR_SDO,i,2));
	  sdo_config_values[i].sdo_index = (uint16_t)GETVALUE(OPAR_SDO,i,0);
	  sdo_config_values[i].sdo_subindex = (uint8_t)GETVALUE(OPAR_SDO,i,1);
	  sdo_config_values[i].value = (uint32_t)GETVALUE(OPAR_SDO,i,3);
	}
    }

  //Handle Pdo Mapping
  if(GetOparSize(block,OPAR_MAP,2)>0)
    {
      for(i=0;i<GetOparSize(block,OPAR_MAP,2);i++)
	{
	  if(GetIparPtrs(block)[DO_DEBUG]==1)//DebugMode
	    {
	      //PDO Mapping
	      //Index subindex vectorlength valuetype bitlength Channelno Direction TypeCode Fullrange Scale Offset
	      printf("%s:PDO Mapping Index %X Subindex %X Vectorsize %i Valuetype %i Bitlen %i ChannelNo %i Direction %i Typecode %i Fullrange %i \n",BLOCKTYPE,GETVALUE(OPAR_MAP,i,0),GETVALUE(OPAR_MAP,i,1),GETVALUE(OPAR_MAP,i,2),GETVALUE(OPAR_MAP,i,3),GETVALUE(OPAR_MAP,i,4),GETVALUE(OPAR_MAP,i,5),GETVALUE(OPAR_MAP,i,6),GETVALUE(OPAR_MAP,i,7),GETVALUE(OPAR_MAP,i,8));
	    }
	  pdo_map[i].pdo_entry_index = (uint16_t)GETVALUE(OPAR_MAP,i,0);
	  pdo_map[i].pdo_entry_subindex = (uint8_t)GETVALUE(OPAR_MAP,i,1);
	  pdo_map[i].vector_len = (unsigned int)GETVALUE(OPAR_MAP,i,2);
	  pdo_map[i].pdo_datatype =Scicos_to_CType(GETVALUE(OPAR_MAP,i,3));
	  channel_spec_values[i].valuetype = pdo_map[i].pdo_datatype;
	  pdo_map[i].bitlen = (unsigned int)GETVALUE(OPAR_MAP,i,4);
	  channel_spec_values[i].channelno = GETVALUE(OPAR_MAP,i,5); //Index start at zero
	  channel_spec_values[i].direction = GETVALUE(OPAR_MAP,i,6);
	  switch(channel_spec_values[i].direction)
	    {
	    case 2:
	      //Master send to Slave
	      pdo_map[i].dir = EC_DIR_OUTPUT;
	      break;
	    case 1:
	      //Slave send to Master
	      pdo_map[i].dir = EC_DIR_INPUT;
	      break;
	    }
	  channel_spec_values[i].typecode = GETVALUE(OPAR_MAP,i,7);
	  pdo_map[i].address = &channel_spec_values[i].address;
	  if(channel_spec_values[i].typecode == 2) //Channel is digital
	    {
	      pdo_map[i].bitoffset = &channel_spec_values[i].bitoffset;
	    }
	  else
	    {
	      pdo_map[i].bitoffset = NULL;
	    }
	  
	  if(channel_spec_values[i].typecode != 2) //Channel is not digital
	    {
	      channel_spec_values[i].fullrange = pow(2.0,(double)(unsigned int)GETVALUE(OPAR_MAP,i,8));
	    }
	  
	}
    }
  else
    {
      printf("%s:Missing PDO Mapping Configuration\n",BLOCKTYPE);
      goto out_failure;
    }

  //Handle Pdo Mapping Scale
  if((GetOparSize(block,OPAR_MAP_SCALE,2)>0)&& (GetOparSize(block,OPAR_MAP_SCALE,2) == GetOparSize(block,OPAR_MAP,2)))
    {
      for(i=0;i<GetOparSize(block,OPAR_MAP_SCALE,2);i++)
	{
  	  if(GetIparPtrs(block)[DO_DEBUG]==1)//DebugMode
  	    {
	      //PDO Mapping Sclae
	      //Slace Offset
	      printf("%s:PDO Mapping Scale Channelnumber %i Scale %f Offset %f\n",BLOCKTYPE,channel_spec_values[i].channelno,GETVALUE_DOUBLE(OPAR_MAP_SCALE,i,0),GETVALUE_DOUBLE(OPAR_MAP_SCALE,i,1));
  	    }
	  if((channel_spec_values[i].typecode == 0)||(channel_spec_values[i].typecode == 1)) //Channel is analog or rawscale
	    {
	      channel_spec_values[i].scale = (double)GETVALUE_DOUBLE(OPAR_MAP_SCALE,i,0);
	      channel_spec_values[i].offset = (double)GETVALUE_DOUBLE(OPAR_MAP_SCALE,i,1);
	    }
	  else
	    {
	      channel_spec_values[i].scale = 0.0;
	      channel_spec_values[i].offset = 0.0;
	    }
	}
    }
  else
    {
      printf("%s:Missing PDO Mapping Scale Configuration\n",BLOCKTYPE);
      goto out_failure;
    }
  


  //fill comdev struct
  comdev->channel = channel_spec_values;
  comdev->channel_count = GetOparSize(block,OPAR_MAP,2);
  comdev->sync_config = sync_config;
  comdev->pdo_config = pdo_config;
  comdev->pdo_entry = pdo_entry;
  comdev->sdo_config_values = sdo_config_values;
  comdev->pdo_map = pdo_map;
  

  //Check Channel Configuration
  for(i=0;i<comdev->channel_count;i++)
    {
      if(GetIparPtrs(block)[DO_DEBUG]==1)//DebugMode
  	{
	  printf("%s: Channel Number %i Direction %i\n",BLOCKTYPE,comdev->channel[i].channelno,comdev->channel[i].direction);
  	}
      switch(comdev->channel[i].direction)
	{
	case 2: //Output, Master send to Slave
	  if(comdev->channel[i].channelno > (GetNin(block)-1))
	    {
	      printf("%s: PDO Mapping Failure, Output Channel %i Index Size Configuration %i\n",BLOCKTYPE,comdev->channel[i].channelno,i);
	      goto out_failure;
	    }
	  /*	  if(comdev->channel[i].vectorlen > GetInPortRows(block,comdev->channel[i].channelno+1))
	    {
	      printf("%s: PDO Mapping Failure, Output Channel %i Vector Size Configuration %i\n",BLOCKTYPE,comdev->channel[i].channelno,i);
	      goto out_failure;
	      }*/
	  break;
	case 1: //Input, Slave send to Slave
	  if(comdev->channel[i].channelno > (GetNout(block)-1))
	    {
	      printf("%s: PDO Mapping Failure, Input Channel %i Index Size Configuration %i\n",BLOCKTYPE,comdev->channel[i].channelno,i);
	      goto out_failure;
	    }
	  /*	  if(comdev->channel[i].vectorlen > GetOutPortRows(block,comdev->channel[i].channelno+1))
	    {
	      printf("%s: PDO Mapping Failure, Input Channel %i Vector Size Configuration %i\n",BLOCKTYPE,comdev->channel[i].channelno,i);
	      goto out_failure;
	      }*/
	  break;
	}	      
  }
  
  errvalue = NULL;

  //Check Slave Configuration
  if(GetIparPtrs(block)[DO_DEBUG]==1)//DebugMode
    {
      printf("%s: Vendor ID %X, Product Code %X\n",BLOCKTYPE,GETVALUE(OPAR_GENERIC,0,0),GETVALUE(OPAR_GENERIC,0,1));
    }


  errvalue = ecs_simply_reg_slave(
				  (unsigned int)GetIparPtrs(block)[MASTER_ID],
				  (unsigned int)GetIparPtrs(block)[DOMAIN_ID],
				  (uint16_t)GetIparPtrs(block)[SLAVE_ALIAS],
				  (uint16_t)GetIparPtrs(block)[SLAVE_POSITION],
				  (uint32_t)GETVALUE(OPAR_GENERIC,0,0),//Vendor ID
				  (uint32_t)GETVALUE(OPAR_GENERIC,0,1),//Product Code 
				  (unsigned int)GetOparSize(block,OPAR_SDO,2),
				  sdo_config_values,
				  sync_config,
				  (unsigned int)GetOparSize(block,OPAR_MAP,2),
				  pdo_map);

  if(errvalue != NULL)
    {
      printf("%s:Slave Registration failed\n",BLOCKTYPE);
      goto out_failure;
    }

  return 0;

 out_failure:
  printf("%s:Error Blockno. %i: %s\n",BLOCKTYPE,get_block_number(),errvalue); 
  comdev->failure_flag =1;
  set_block_error(-3); //internal_error
  //end_scicos_sim();
  return 0;
}

static int inout(scicos_block *block)
{

  struct ADCOMDev * comdev = (struct ADCOMDev *) (*block->work);
  int i,j;
  unsigned int value_digital;
  double *y;
  double value;
  const char* errvalue;
  if(comdev->failure_flag ==0)
   {
      errvalue = ecs_simply_receive();
      if(errvalue != NULL)
	{
	  printf("%s:Slave Activation failed\n",BLOCKTYPE);
	  comdev->failure_flag = 1;
	  return 0;
	}
      //Debug Channel Configuration
      if((GetIparPtrs(block)[DO_DEBUG]==1)&&(comdev->debug_info == 1)&&(errvalue==NULL))//DebugMode
	{
	  comdev->debug_info = 0;
	  printf("%s: Channel Pointer:\n",BLOCKTYPE);
	  for(i=0;i<comdev->channel_count;i++)
	    {
	      if(comdev->channel[i].typecode == 2)
		{
		  printf("%s: Channel Number %i Direction %i Type %i Address %X Bitoffset %X\n",BLOCKTYPE,comdev->channel[i].channelno,comdev->channel[i].direction,comdev->channel[i].typecode,(unsigned int)comdev->channel[i].address,(unsigned int)comdev->channel[i].bitoffset);
		}
	      else
		{
		  printf("%s: Channel Number %i Direction %i Type %i Address %X Vectorlen %i\n",BLOCKTYPE,comdev->channel[i].channelno,comdev->channel[i].direction,comdev->channel[i].typecode,(unsigned int)comdev->channel[i].address,comdev->channel[i].vectorlen);
		}
	    }
	}

      
      if(errvalue == NULL)
	{
	  for(i=0;i<comdev->channel_count;i++)
	  {
	    switch(comdev->channel[i].direction)
	      {
	      case 1: //Input
		//Slave send to Master
		switch(comdev->channel[i].typecode)
		  {
		  case 0: //Analog
		    y =GetRealOutPortPtrs(block,comdev->channel[i].channelno+1);
		    //    si_double_T = 1, si_single_T =2, si_uint8_T = 3, si_sint8_T = 4, 
		    //    si_uint16_T = 5, si_sint16_T = 6,si_uint32_T = 7, si_sint32_T = 8,
		    //    si_boolean_T =9, 
		    switch(comdev->channel[i].valuetype)
		      {
		      case 1: //si_double_T, not supported by EtherCAT
			y[0] = 0.0;
			break;
		      case 2: //si_single_T, not supported by EtherCAT
			y[0] = 0.0;
			break;
		      case 3: //si_uint8_T
			y[0] =comdev->channel[i].scale * EC_READ_U8(comdev->channel[i].address) / comdev->channel[i].fullrange + comdev->channel[i].offset ;
			break;
		      case 4: //si_sint8_T
			y[0] =comdev->channel[i].scale * EC_READ_S8(comdev->channel[i].address) / comdev->channel[i].fullrange + comdev->channel[i].offset ;
			break;
		      case 5: //si_uint16_T
			y[0] =comdev->channel[i].scale * EC_READ_U16(comdev->channel[i].address) / comdev->channel[i].fullrange + comdev->channel[i].offset ;
			break;
		      case 6: //si_sint16_T
			y[0] =comdev->channel[i].scale * EC_READ_S16(comdev->channel[i].address) / comdev->channel[i].fullrange + comdev->channel[i].offset ;
			break;
		      case 7: //si_uint32_T
			y[0] =comdev->channel[i].scale * EC_READ_U32(comdev->channel[i].address) / comdev->channel[i].fullrange + comdev->channel[i].offset ;
			break;
		      case 8: //si_sint32_T
			y[0] =comdev->channel[i].scale * EC_READ_S32(comdev->channel[i].address) / comdev->channel[i].fullrange + comdev->channel[i].offset ;
			break;
		      case 9: //si_boolean_T, not supported for this Signal Type
			y[0] = 0.0;
			break;
			}
		    break;
		  case 1: //Rawscaled
		    y =GetRealOutPortPtrs(block,comdev->channel[i].channelno+1);
		    switch(comdev->channel[i].valuetype)
		      {
		      case 1: //si_double_T, not supported by EtherCAT
			y[0] = 0.0;
			break;
		      case 2: //si_single_T, not supported by EtherCAT
			y[0] = 0.0;
			break;
		      case 3: //si_uint8_T
			y[0] =comdev->channel[i].scale * EC_READ_U8(comdev->channel[i].address) + comdev->channel[i].offset ;
			break;
		      case 4: //si_sint8_T
			y[0] =comdev->channel[i].scale * EC_READ_S8(comdev->channel[i].address) + comdev->channel[i].offset ;
			break;
		      case 5: //si_uint16_T
			y[0] =comdev->channel[i].scale * EC_READ_U16(comdev->channel[i].address) + comdev->channel[i].offset ;
			break;
		      case 6: //si_sint16_T
			y[0] =comdev->channel[i].scale * EC_READ_S16(comdev->channel[i].address) + comdev->channel[i].offset ;
			break;
		      case 7: //si_uint32_T
			y[0] =comdev->channel[i].scale * EC_READ_U32(comdev->channel[i].address) + comdev->channel[i].offset ;
			break;
		      case 8: //si_sint32_T
			y[0] =comdev->channel[i].scale * EC_READ_S32(comdev->channel[i].address) + comdev->channel[i].offset ;
			break;
		      case 9: //si_boolean_T, not supported for this Signal Type
			y[0] = 0.0;
			break;
			}
		    break;
		  case 2: //Digital
		    y =GetRealOutPortPtrs(block,comdev->channel[i].channelno+1);
		    y[0] = (double)EC_READ_BIT(comdev->channel[i].address,comdev->channel[i].bitoffset);

		    if(GetIparPtrs(block)[DO_DEBUG]==1)//DebugMode
		      {
			//			printf("%s: Channel Number %i Direction %i Value %i\n",BLOCKTYPE,comdev->channel[i].channelno,comdev->channel[i].direction,value_digital);
		      }		    

		    break;
		  }

		break;
	      case 2: //Output
		//Master send to Slave
		y =GetRealInPortPtrs(block,comdev->channel[i].channelno+1);
		switch(comdev->channel[i].typecode)
		  {
		  case 0: //Analog
		    //Value = Values between (0.0-1.0)*Fullscale
		    value = comdev->channel[i].fullrange * ((y[0] + comdev->channel[i].offset) / comdev->channel[i].scale );
		    value = value >= comdev->channel[i].fullrange ? (comdev->channel[i].fullrange -1): (value <= -comdev->channel[i].fullrange ? (-comdev->channel[i].fullrange + 1) : value);

		    switch(comdev->channel[i].valuetype)
		      {
		      case 1: //si_double_T, not supported by EtherCAT
			break;
		      case 2: //si_single_T, not supported by EtherCAT
			break;
		      case 3: //si_uint8_T
			EC_WRITE_U8(comdev->channel[i].address, value);
			break;
		      case 4: //si_sint8_T
			EC_WRITE_S8(comdev->channel[i].address, value);
			break;
		      case 5: //si_uint16_T
			EC_WRITE_U16(comdev->channel[i].address, value);
			break;
		      case 6: //si_sint16_T
			EC_WRITE_S16(comdev->channel[i].address, value);
			break;
		      case 7: //si_uint32_T
			EC_WRITE_U32(comdev->channel[i].address, value);
			break;
		      case 8: //si_sint32_T
			EC_WRITE_S32(comdev->channel[i].address, value);
			break;
		      case 9: //si_boolean_T, not supported for this Signal Type
			break;
		      }

		    break;
		  case 1: //Rawscaled
		    break;
		  case 2: //Digital

		    if(y[0]>0.5)
		      {
			value_digital = 1;
		      }
		    else
		      {
			value_digital = 0;
		      }

		    if(GetIparPtrs(block)[DO_DEBUG]==1)//DebugMode
		      {
			//			printf("%s: Channel Number %i Direction %i Value %i\n",BLOCKTYPE,comdev->channel[i].channelno,comdev->channel[i].direction,value_digital);
		      }		    
		    EC_WRITE_BIT(comdev->channel[i].address,comdev->channel[i].bitoffset,value_digital);
		    break;
		  }
		break;
	      }
	      /*  void *address;
		  unsigned int bitoffset;
		  int channelno;
		  int vectorlen;
		  int direction;
		  int typecode;
		  unsigned int fullrange;
		  int scale;
		  int offset;*/

	      
	  }
	  /* for(i=0;i<GetNin(block);i++)
	     {
	     y =GetRealInPortPtrs(block,i+1) ;

	     EC_WRITE_BIT(comdev->address[i],comdev->bitoffset[1],value);
	     
	     }*/
  	  errvalue = ecs_simply_send();
	}

 
      if(errvalue!=NULL)
	{
	  goto run_failure;
	}
    }
  return 0;

 run_failure:
  printf("%s:Error Blockno.  %i: %s\n",BLOCKTYPE,get_block_number(),errvalue);
  set_block_error(-3); //internal_error
  return 0;
}

static int end(scicos_block *block)
{
  if(GetIparPtrs(block)[DO_DEBUG]==1)//DebugMode
    {
      printf("%s:Stop EL Scicos Block\n",BLOCKTYPE);
    }
  
  struct ADCOMDev * comdev = (struct ADCOMDev *) (*block->work);
  int i;

  if(comdev->failure_flag ==0)
    {  
      
      /*for(i=0;i<GetNin(block);i++)
	{
	//EC_WRITE_BIT(comdev->address[i],comdev->bitoffset[1],0);
	}*/
      ecs_simply_end();
    }

  
  if(GetIparPtrs(block)[DO_DEBUG]==1)//DebugMode
    {
      printf("%s:Release Structures.\n",BLOCKTYPE);
    }
  if(comdev->sync_config !=NULL) scicos_free(comdev->sync_config);
  if(comdev->pdo_config !=NULL) scicos_free(comdev->pdo_config);
  if(comdev->pdo_entry !=NULL) scicos_free(comdev->pdo_entry);
  if(comdev->sdo_config_values !=NULL) scicos_free(comdev->sdo_config_values);
  if(comdev->pdo_map !=NULL) scicos_free(comdev->pdo_map);
  if(comdev->channel !=NULL) scicos_free(comdev->channel);
  if(GetIparPtrs(block)[DO_DEBUG]==1)//DebugMode
    {
      printf("%s:Release Work structure\n",BLOCKTYPE);
    }
  if(comdev !=NULL) scicos_free(comdev);
  return 0;
}

void etl_scicos(scicos_block *block,int flag)
{
  if (flag==1){          /* set output */
    inout(block);
  }
  else if (flag==5){     /* termination */ 
    end(block);
  }
  else if (flag ==4){    /* initialisation */
    init(block);
  }
}


