/*
 * SOES Simple Open EtherCAT Slave
 *
 * File    : objectlist.h
 * Version : 0.9.2
 * Date    : 22-02-2010
 * Copyright (C) 2007-2010 Arthur Ketels
 *
 * SOES is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 as published by the Free
 * Software Foundation.
 *
 * SOES is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 *
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 *
 * The EtherCAT Technology, the trade name and logo "EtherCAT" are the intellectual
 * property of, and protected by Beckhoff Automation GmbH.
 */
#ifndef __objectlist__
#define __objectlist__

/**
For info about this file, please check http://bitbucket.org/utwente_bss/soes_arm/wiki/objectlist_h
*/
typedef const struct
  {
    uint16        subindex;
    uint16        datatype;
    uint16        bitlength;
    uint16        access;
    char   const *name;
    uint32        value;
    void          *data;
  } _objd;

typedef const struct
  {
    uint16        index;
    uint16        objtype;
    uint8         maxsub;
    uint8         pad1;
    char   const *name;
    _objd   const *objdesc;
  } _objectlist;

#define OBJH_READ               0
#define OBJH_WRITE              1

#define _ac const char
#define nil 0

//Described in Table 63 & Table 64 of ETG1000.6
#define OTYPE_DOMAIN            0x0002
#define OTYPE_DEFTYPE           0x0005
#define OTYPE_DEFSTRUCT         0x0006
#define OTYPE_VAR               0x0007
#define OTYPE_ARRAY             0x0008
#define OTYPE_RECORD            0x0009

//Described in Table 70 of ETG1000.6
#define DTYPE_BOOLEAN           0x0001
#define DTYPE_INTEGER8          0x0002
#define DTYPE_INTEGER16         0x0003
#define DTYPE_INTEGER32         0x0004
#define DTYPE_UNSIGNED8         0x0005
#define DTYPE_UNSIGNED16        0x0006
#define DTYPE_UNSIGNED32        0x0007
#define DTYPE_REAL32            0x0008
#define DTYPE_VISIBLE_STRING    0x0009
#define DTYPE_OCTET_STRING      0x000A
#define DTYPE_UNICODE_STRING    0x000B
#define DTYPE_INTEGER24         0x0010
#define DTYPE_UNSIGNED24        0x0016
#define DTYPE_INTEGER64         0x0015
#define DTYPE_UNSIGNED64        0x001B
#define DTYPE_REAL64            0x0011
#define DTYPE_PDO_MAPPING       0x0021
#define DTYPE_IDENTITY          0x0023
#define DTYPE_BIT1              0x0030
#define DTYPE_BIT2              0x0031
#define DTYPE_BIT3              0x0032
#define DTYPE_BIT4              0x0033
#define DTYPE_BIT5              0x0034
#define DTYPE_BIT6              0x0035
#define DTYPE_BIT7              0x0036
#define DTYPE_BIT8              0x0037

//See 'Object Access' in Table 47 of ETG1000.6
#define ATYPE_R                 0x07
#define ATYPE_RW                0x3F
#define ATYPE_RWpre             0x0F
#define ATYPE_RXPDO             0x40
#define ATYPE_TXPDO             0x80

/** Mapping of parameters is according to ETG.5000 document, and ETG.1000.6
*/
_ac acName1000[]="Device Type";
_ac acName1008[]="Manufacturer Device Name";
_ac acName1009[]="Manufacturer Hardware Version";
_ac acName100A[]="Manufacturer Software Version";
_ac acName1018[]="Identity Object";
/*Subindexes for Identity Object*/
_ac acName1018_01[]="Vendor ID";
_ac acName1018_02[]="Product Code";
_ac acName1018_03[]="Revision Number";
_ac acName1018_04[]="Serial Number";
/*End of subindexes for Identity Object*/
_ac acNameMO[]="Mapped object";
_ac acName1600[]="Receive PDO mapping";
_ac acName1A00[]="Transmit PDO mapping - Digital";
_ac acName1A10[]="Transmit PDO mapping - Analog";
_ac acName1C00[]="Sync Manager Communication type";
/*Subindexes for Sync Manager Communication type*/
_ac acName1C00_01[]="Communications type SM0";
_ac acName1C00_02[]="Communications type SM1";
_ac acName1C00_03[]="Communications type SM2";
_ac acName1C00_04[]="Communications type SM3";
/*End of subindexes for Sync Manager Communication type*/
_ac acName1C10[]="Sync Manager 0 PDO Assignment";
_ac acName1C11[]="Sync Manager 1 PDO Assignment";
_ac acName1C12[]="Sync Manager 2 PDO Assignment";
_ac acName1C13[]="Sync Manager 3 PDO Assignment";
_ac acNameNOE[]="Number of entries";
_ac acName6000[]="Digital Inputs";
/*Subindexes for Digital Inputs*/
_ac acName6000_01[]="Status";
_ac acName6000_02[]="Counter";
_ac acName6000_03[]="Digital inputs";
/*End of subindexes for Digital Inputs*/
_ac acName6001[]="Analog Inputs";
/*Subindexes for Analog inputs*/
_ac acName6001_01[]="Analog 1";
_ac acName6001_02[]="Analog 2";
_ac acName6001_03[]="Analog 3";
_ac acName6001_04[]="Analog 4";
_ac acName6001_05[]="Analog 5";
_ac acName6001_06[]="Analog 6";
_ac acName6001_07[]="Analog 7";
_ac acName6001_08[]="Analog 8";
_ac acName6001_09[]="Timestamp";
/*End of subindexes for Analog inputs*/
_ac acName7000[]="Outputs";
/*Subindexes of Outputs*/
_ac acName7000_01[]="Control";
_ac acName7000_02[]="digoutput";
/*End of subindexes for Outputs*/
_ac acName8000[]="Configuration";
/*Subindexes of configuration*/
_ac acName8000_01[]="Settings 8";
_ac acName8000_02[]="Settings 16";
/*End of subindexes of configuration*/

/////////////////////////////////////////////////////////////////////////////////////
/** Definiton of Device Name */
char ac1008_00[]="SOES Simple Open EtherCAT Slave";
/** Definition of Hardware version*/
char ac1009_00[]="0.0.1";
/** Definition of Software version*/
char ac100A_00[]="0.0.1";
/** Service Data Object 1000: Device Type */
const _objd SDO1000[]=
{{0x00,DTYPE_UNSIGNED32,32,ATYPE_R,&acName1000[0],0x00000000}};
/** Service Data Object 1008: Device Name */
const _objd SDO1008[]=
{{0x00,DTYPE_VISIBLE_STRING,sizeof(ac1008_00)<<3,ATYPE_R,&acName1008[0],0,&ac1008_00[0]}};
/** Service Data Object 1009: Hardware Version */
const _objd SDO1009[]=
{{0x00,DTYPE_VISIBLE_STRING,sizeof(ac1009_00)<<3,ATYPE_R,&acName1009[0],0,&ac1009_00[0]}};
/** Service Data Object 100A: Software Version */
const _objd SDO100A[]=
{{0x00,DTYPE_VISIBLE_STRING,sizeof(ac100A_00)<<3,ATYPE_R,&acName100A[0],0,&ac100A_00[0]}};
const _objd SDO1018[]=                                              //See ETG.1000.6 'Identity Object'
 {{0x00,DTYPE_UNSIGNED8,8,ATYPE_R,&acNameNOE[0],0x04},               //Number of Entries
  {0x01,DTYPE_UNSIGNED32,32,ATYPE_R,&acName1018_01[0],0x500},  //Vendor ID
  {0x02,DTYPE_UNSIGNED32,32,ATYPE_R,&acName1018_02[0],0x00908001 },  //Product Code
  {0x03,DTYPE_UNSIGNED32,32,ATYPE_R,&acName1018_03[0],0x002},  //Revision Number
  {0x04,DTYPE_UNSIGNED32,32,ATYPE_R,&acName1018_04[0],0x001}   //Serial Number};
};
/////////////////////////////////////////////////////////////////////////////////////////////
const _objd SDO1600[]=                                              //RxPDO mapping (objects from master to slave)
{{0x00,DTYPE_UNSIGNED8,8,ATYPE_R,&acNameNOE[0],0x02},               //Number of RxPDOs
  {0x01,DTYPE_UNSIGNED32,32,ATYPE_R,&acNameMO[0],0x70000108},       //First Object, pointing to object 7001:01
  {0x02,DTYPE_UNSIGNED32,32,ATYPE_R,&acNameMO[0],0x70000208}       //Second Object, pointing to object 7001:02
};
const _objd SDO1A00[]=                                              //TxPDO mapping (objects from slave to master)
{{0x00,DTYPE_UNSIGNED8,8,ATYPE_R,&acNameNOE[0],0x03},               //Number of TxPDOs
  {0x01,DTYPE_UNSIGNED32,32,ATYPE_R,&acNameMO[0],0x60000108},       //First Object, pointing to object  6000:01
  {0x02,DTYPE_UNSIGNED32,32,ATYPE_R,&acNameMO[0],0x60000208},       //Second Object, pointing to object 6000:02
  {0x03,DTYPE_UNSIGNED32,32,ATYPE_R,&acNameMO[0],0x60000308}       //..
};
const _objd SDO1A10[]=                                              //Second TxPDO module
{{0x00,DTYPE_UNSIGNED8,8,ATYPE_R,&acNameNOE[0],0x09},
  {0x01,DTYPE_UNSIGNED32,32,ATYPE_R,&acNameMO[0],0x60010110},
  {0x02,DTYPE_UNSIGNED32,32,ATYPE_R,&acNameMO[0],0x60010210},
  {0x03,DTYPE_UNSIGNED32,32,ATYPE_R,&acNameMO[0],0x60010310},
  {0x04,DTYPE_UNSIGNED32,32,ATYPE_R,&acNameMO[0],0x60010410},
  {0x05,DTYPE_UNSIGNED32,32,ATYPE_R,&acNameMO[0],0x60010510},
  {0x06,DTYPE_UNSIGNED32,32,ATYPE_R,&acNameMO[0],0x60010610},
  {0x07,DTYPE_UNSIGNED32,32,ATYPE_R,&acNameMO[0],0x60010710},
  {0x08,DTYPE_UNSIGNED32,32,ATYPE_R,&acNameMO[0],0x60010810},
  {0x09,DTYPE_UNSIGNED32,32,ATYPE_R,&acNameMO[0],0x60010920}
};
const _objd SDO1C00[]=                                              //Sync Manager
{{0x00,DTYPE_UNSIGNED8,8,ATYPE_R,&acNameNOE[0],0x04},               //Number of used Sync Manager channels
  {0x01,DTYPE_UNSIGNED8,8,ATYPE_R,&acName1C00_01[0],0x01},          //Sync Manager 0: Mailbox Receive (master->slave)
  {0x02,DTYPE_UNSIGNED8,8,ATYPE_R,&acName1C00_02[0],0x02},          //Sync Manager 1: Mailbos send    (slave->master)
  {0x03,DTYPE_UNSIGNED8,8,ATYPE_R,&acName1C00_03[0],0x03},          //Process Data Output (or Inputs if no Outputs are used)
  {0x04,DTYPE_UNSIGNED8,8,ATYPE_R,&acName1C00_04[0],0x04}           //Process Data Input
};
const _objd SDO1C10[]=                                              //Sync Manager 0 PDO Assignment
{{0x00,DTYPE_UNSIGNED8,8,ATYPE_R,&acName1C10[0],0x00}};
const _objd SDO1C11[]=                                              //Sync Manager 1 PDO Assignment
{{0x00,DTYPE_UNSIGNED8,8,ATYPE_R,&acName1C11[0],0x00}};
const _objd SDO1C12[]=                                              //RxPDO Assign objects
{{0x00,DTYPE_UNSIGNED8,8,ATYPE_R,&acNameNOE[0],0x01},               //Number of Entries
  {0x01,DTYPE_UNSIGNED16,16,ATYPE_R,&acNameMO[0],0x1600}           //Send objects in index 0x1600
};
const _objd SDO1C13[]=                                              //TxPDO Assign objects ; CHANGEABLE, thus 'RWpre' mode
{{0x00,DTYPE_UNSIGNED8,8,ATYPE_RWpre,&acNameNOE[0],0x02},               //Number of Entries
  {0x01,DTYPE_UNSIGNED16,16,ATYPE_RWpre,&acNameMO[0],0x1A00},            //Send objects in index 0x1A00
  {0x02,DTYPE_UNSIGNED16,16,ATYPE_RWpre,&acNameMO[0],0x1A10}             //Send objects in index 0x1A10
};
const _objd SDO6000[]=                                                      //TxPDO module
{{0x00,DTYPE_UNSIGNED8  ,8,ATYPE_R,&acNameNOE[0],0x03},                     //Number of elements
  {0x01,DTYPE_UNSIGNED8  ,8,ATYPE_R,&acName6000_01[0],0,&(Rb.status)},      //6000:01 is status
  {0x02,DTYPE_UNSIGNED8  ,8,ATYPE_R,&acName6000_02[0],0,&(Rb.counter)},     //6000:02 is counter
  {0x03,DTYPE_UNSIGNED8  ,8,ATYPE_R,&acName6000_03[0],0,&(Rb.diginput)}     //6000:03 is digital inputs
};
const _objd SDO6001[]=                                                      //TxPDO module
{{0x00,DTYPE_UNSIGNED8  ,8,ATYPE_R,&acNameNOE[0],0x09},                     //Number of elements
  {0x01,DTYPE_INTEGER16  ,16,ATYPE_R,&acName6001_01[0],0,&(Rb.analog[0])},  //6001:1 is analog 0
  {0x02,DTYPE_INTEGER16  ,16,ATYPE_R,&acName6001_02[0],0,&(Rb.analog[1])},  //6001:2 is analog 1
  {0x03,DTYPE_INTEGER16  ,16,ATYPE_R,&acName6001_03[0],0,&(Rb.analog[2])},  //6001:3 is analog 2
  {0x04,DTYPE_INTEGER16  ,16,ATYPE_R,&acName6001_04[0],0,&(Rb.analog[3])},  //6001:4 is analog 3
  {0x05,DTYPE_INTEGER16  ,16,ATYPE_R,&acName6001_05[0],0,&(Rb.analog[4])},  //6001:5 is analog 4
  {0x06,DTYPE_INTEGER16  ,16,ATYPE_R,&acName6001_06[0],0,&(Rb.analog[5])},  //6001:6 is analog 5
  {0x07,DTYPE_INTEGER16  ,16,ATYPE_R,&acName6001_07[0],0,&(Rb.analog[6])},  //6001:7 is analog 6
  {0x08,DTYPE_INTEGER16  ,16,ATYPE_R,&acName6001_08[0],0,&(Rb.analog[7])},  //6001:8 is analog 7
  {0x09,DTYPE_UNSIGNED32 ,32,ATYPE_R,&acName6001_09[0],0,&(Rb.timestamp)}   //6001:9 is timestamp
};
const _objd SDO7000[]=                                                      //RxPDO module
{{0x00,DTYPE_UNSIGNED8  ,8,ATYPE_R,&acNameNOE[0],0x02},                     //Number of elements
  {0x01,DTYPE_UNSIGNED8  ,8,ATYPE_RW,&acName7000_01[0],0,&(Wb.control)},    //7001:01 is control
  {0x02,DTYPE_UNSIGNED8  ,8,ATYPE_RW,&acName7000_02[0],0,&(Wb.digoutput)}   //7001:02 is digoutput
};
const _objd SDO8000[]=                                                      //Configuration mailbox
{{0x00,DTYPE_UNSIGNED8  ,8,ATYPE_R,&acNameNOE[0],0x02},                     //number of elements
  {0x01,DTYPE_UNSIGNED8  ,8,ATYPE_RW,&acName8000_01[0],0,&(Eb.setting8)},   //8000:01 is setting8
  {0x02,DTYPE_UNSIGNED16 ,16,ATYPE_RW,&acName8000_02[0],0,&(Eb.setting16)}  //8000:02 is setting16
};
/** Object list; all objects used in EtherCAT slave */
const _objectlist SDOobjects[]=
{{0x1000,OTYPE_VAR     , 0,0,&acName1000[0],&SDO1000[0]},       //Device Type
  {0x1008,OTYPE_VAR     , 0,0,&acName1008[0],&SDO1008[0]},      //Device Name
  {0x1009,OTYPE_VAR     , 0,0,&acName1009[0],&SDO1009[0]},      //Hardware Version
  {0x100A,OTYPE_VAR     , 0,0,&acName100A[0],&SDO100A[0]},      //Software Version
  {0x1018,OTYPE_RECORD  , 4,0,&acName1018[0],&SDO1018[0]},      //Identity
  {0x1600,OTYPE_RECORD  , 0x02,0,&acName1600[0],&SDO1600[0]},   //RxPDO mapping
  {0x1A00,OTYPE_RECORD  , 0x03,0,&acName1A00[0],&SDO1A00[0]},   //TxPDO mapping
  {0x1A10,OTYPE_RECORD  , 0x09,0,&acName1A10[0],&SDO1A10[0]},   //TxPDO mapping
  {0x1C00,OTYPE_ARRAY   , 4,0,&acName1C00[0],&SDO1C00[0]},      //Sync Manager configuration
  {0x1C10,OTYPE_ARRAY   , 0,0,&acName1C10[0],&SDO1C10[0]},      //Sync Manager 0 PDO assignment
  {0x1C11,OTYPE_ARRAY   , 0,0,&acName1C11[0],&SDO1C11[0]},      //Sync Manager 1 PDO assignment
  {0x1C12,OTYPE_ARRAY   , 1,0,&acName1C12[0],&SDO1C12[0]},      //RxPDO objects
  {0x1C13,OTYPE_ARRAY   , 2,0,&acName1C13[0],&SDO1C13[0]},      //TxPDO objects
  {0x6000,OTYPE_ARRAY   , 0x03,0,&acName6000[0],&SDO6000[0]},   //TxPDO module
  {0x6001,OTYPE_ARRAY   , 0x09,0,&acName6001[0],&SDO6001[0]},   //TxPDO module
  {0x7000,OTYPE_ARRAY   , 0x02,0,&acName7000[0],&SDO7000[0]},   //RxPDO module
  {0x8000,OTYPE_ARRAY   , 0x02,0,&acName8000[0],&SDO8000[0]},   //RxPDO module
  {0xffff,0xff,0xff,0xff,nil,nil}
};

#endif
