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

/*************************************************************/
//This file will be removed in future releases, do not use it
/*************************************************************/

#ifndef ECRT_SLAVES_H
#define ECRT_SLAVES_H
#include <ecrt.h> // EtherCAT realtime interface


//Rangedefines
#define U8MAX 256
#define U16MAX 65536
#define U32MAX 4294967296

//Vendor Id
#define Beckhoff_VendorId 0x00000002

//Product Code
#define Beckhoff_EL1004_ProductCode 0x03EC3052
#define Beckhoff_EL1012_ProductCode 0x03f43052
#define Beckhoff_EL1014_ProductCode 0x03F63052
#define Beckhoff_EL1034_ProductCode 0x040a3052
#define Beckhoff_EL1008_ProductCode 0x03f03052
#define Beckhoff_EL1018_ProductCode 0x03fa3052
#define Beckhoff_EL2004_ProductCode 0x07D43052
#define Beckhoff_EL2032_ProductCode 0x07F03052
#define Beckhoff_EL2622_ProductCode 0x0a3e3052
#define Beckhoff_EL3102_ProductCode 0x0C1E3052
#define Beckhoff_EL3152_ProductCode 0x0C503052
#define Beckhoff_EL3162_ProductCode 0x0C5A3052

//DEFINE ETHERCAT SLAVES PRODUCT/VENDORID

#define Beckhoff_EL1004 Beckhoff_VendorId, Beckhoff_EL1004_ProductCode
#define Beckhoff_EL1012 Beckhoff_VendorId, Beckhoff_EL1012_ProductCode
#define Beckhoff_EL1014 Beckhoff_VendorId, Beckhoff_EL1014_ProductCode
#define Beckhoff_EL1034 Beckhoff_VendorId, Beckhoff_EL1034_ProductCode
#define Beckhoff_EL1008 Beckhoff_VendorId, Beckhoff_EL1008_ProductCode
#define Beckhoff_EL1018 Beckhoff_VendorId, Beckhoff_EL1018_ProductCode
#define Beckhoff_EL2004 Beckhoff_VendorId, Beckhoff_EL2004_ProductCode
#define Beckhoff_EL2032 Beckhoff_VendorId, Beckhoff_EL2032_ProductCode
#define Beckhoff_EL2622 Beckhoff_VendorId, Beckhoff_EL2622_ProductCode
#define Beckhoff_EL3102 Beckhoff_VendorId, Beckhoff_EL3102_ProductCode
#define Beckhoff_EL3152 Beckhoff_VendorId, Beckhoff_EL3152_ProductCode
#define Beckhoff_EL3162 Beckhoff_VendorId, Beckhoff_EL3162_ProductCode


//DEFINE PDO CONFIGURATION
#define Beckhoff_EL1004_PDO_Input1 0x6000, 1
#define Beckhoff_EL1004_PDO_Input2 0x6010, 1
#define Beckhoff_EL1004_PDO_Input3 0x6020, 1
#define Beckhoff_EL1004_PDO_Input4 0x6030, 1
#define Beckhoff_EL1004_Input1 Beckhoff_EL1004, Beckhoff_EL1004_PDO_Input1
#define Beckhoff_EL1004_Input2 Beckhoff_EL1004, Beckhoff_EL1004_PDO_Input2
#define Beckhoff_EL1004_Input3 Beckhoff_EL1004, Beckhoff_EL1004_PDO_Input3
#define Beckhoff_EL1004_Input4 Beckhoff_EL1004, Beckhoff_EL1004_PDO_Input4


#define Beckhoff_EL1012_PDO_Input1 0x6000, 1
#define Beckhoff_EL1012_PDO_Input2 0x6010, 1
#define Beckhoff_EL1012_Input1 Beckhoff_EL1012, Beckhoff_EL1012_PDO_Input1
#define Beckhoff_EL1012_Input2 Beckhoff_EL1012, Beckhoff_EL1012_PDO_Input2


#define Beckhoff_EL1014_PDO_Input1 0x6000, 1
#define Beckhoff_EL1014_PDO_Input2 0x6010, 1
#define Beckhoff_EL1014_PDO_Input3 0x6020, 1
#define Beckhoff_EL1014_PDO_Input4 0x6030, 1
#define Beckhoff_EL1014_Input1 Beckhoff_EL1014, Beckhoff_EL1014_PDO_Input1
#define Beckhoff_EL1014_Input2 Beckhoff_EL1014, Beckhoff_EL1014_PDO_Input2
#define Beckhoff_EL1014_Input3 Beckhoff_EL1014, Beckhoff_EL1014_PDO_Input3
#define Beckhoff_EL1014_Input4 Beckhoff_EL1014, Beckhoff_EL1014_PDO_Input4

#define Beckhoff_EL1034_PDO_Input1 0x6000, 1
#define Beckhoff_EL1034_PDO_Input2 0x6010, 1
#define Beckhoff_EL1034_PDO_Input3 0x6020, 1
#define Beckhoff_EL1034_PDO_Input4 0x6030, 1
#define Beckhoff_EL1034_Input1 Beckhoff_EL1034, Beckhoff_EL1034_PDO_Input1
#define Beckhoff_EL1034_Input2 Beckhoff_EL1034, Beckhoff_EL1034_PDO_Input2
#define Beckhoff_EL1034_Input3 Beckhoff_EL1034, Beckhoff_EL1034_PDO_Input3
#define Beckhoff_EL1034_Input4 Beckhoff_EL1034, Beckhoff_EL1034_PDO_Input4

#define Beckhoff_EL1008_PDO_Input1 0x6000, 1
#define Beckhoff_EL1008_PDO_Input2 0x6010, 1
#define Beckhoff_EL1008_PDO_Input3 0x6020, 1
#define Beckhoff_EL1008_PDO_Input4 0x6030, 1
#define Beckhoff_EL1008_PDO_Input5 0x6040, 1
#define Beckhoff_EL1008_PDO_Input6 0x6050, 1
#define Beckhoff_EL1008_PDO_Input7 0x6060, 1
#define Beckhoff_EL1008_PDO_Input8 0x6070, 1
#define Beckhoff_EL1008_Input1 Beckhoff_EL1008, Beckhoff_EL1008_PDO_Input1
#define Beckhoff_EL1008_Input2 Beckhoff_EL1008, Beckhoff_EL1008_PDO_Input2
#define Beckhoff_EL1008_Input3 Beckhoff_EL1008, Beckhoff_EL1008_PDO_Input3
#define Beckhoff_EL1008_Input4 Beckhoff_EL1008, Beckhoff_EL1008_PDO_Input4
#define Beckhoff_EL1008_Input5 Beckhoff_EL1008, Beckhoff_EL1008_PDO_Input5
#define Beckhoff_EL1008_Input6 Beckhoff_EL1008, Beckhoff_EL1008_PDO_Input6
#define Beckhoff_EL1008_Input7 Beckhoff_EL1008, Beckhoff_EL1008_PDO_Input7
#define Beckhoff_EL1008_Input8 Beckhoff_EL1008, Beckhoff_EL1008_PDO_Input8


#define Beckhoff_EL1018_PDO_Input1 0x6000, 1
#define Beckhoff_EL1018_PDO_Input2 0x6010, 1
#define Beckhoff_EL1018_PDO_Input3 0x6020, 1
#define Beckhoff_EL1018_PDO_Input4 0x6030, 1
#define Beckhoff_EL1018_PDO_Input5 0x6040, 1
#define Beckhoff_EL1018_PDO_Input6 0x6050, 1
#define Beckhoff_EL1018_PDO_Input7 0x6060, 1
#define Beckhoff_EL1018_PDO_Input8 0x6070, 1
#define Beckhoff_EL1018_Input1 Beckhoff_EL1018, Beckhoff_EL1018_PDO_Input1
#define Beckhoff_EL1018_Input2 Beckhoff_EL1018, Beckhoff_EL1018_PDO_Input2
#define Beckhoff_EL1018_Input3 Beckhoff_EL1018, Beckhoff_EL1018_PDO_Input3
#define Beckhoff_EL1018_Input4 Beckhoff_EL1018, Beckhoff_EL1018_PDO_Input4
#define Beckhoff_EL1018_Input5 Beckhoff_EL1018, Beckhoff_EL1018_PDO_Input5
#define Beckhoff_EL1018_Input6 Beckhoff_EL1018, Beckhoff_EL1018_PDO_Input6
#define Beckhoff_EL1018_Input7 Beckhoff_EL1018, Beckhoff_EL1018_PDO_Input7
#define Beckhoff_EL1018_Input8 Beckhoff_EL1018, Beckhoff_EL1018_PDO_Input8

#define Beckhoff_EL2004_PDO_Output1 0x7000, 1
#define Beckhoff_EL2004_PDO_Output2 0x7010, 1
#define Beckhoff_EL2004_PDO_Output3 0x7020, 1
#define Beckhoff_EL2004_PDO_Output4 0x7030, 1
#define Beckhoff_EL2004_Output1 Beckhoff_EL2004, Beckhoff_EL2004_PDO_Output1
#define Beckhoff_EL2004_Output2 Beckhoff_EL2004, Beckhoff_EL2004_PDO_Output2
#define Beckhoff_EL2004_Output3 Beckhoff_EL2004, Beckhoff_EL2004_PDO_Output3
#define Beckhoff_EL2004_Output4 Beckhoff_EL2004, Beckhoff_EL2004_PDO_Output4

#define Beckhoff_EL2032_PDO_Output1 0x7000, 1
#define Beckhoff_EL2032_PDO_Output2 0x7010, 1
#define Beckhoff_EL2032_PDO_Diag1 0x6000, 1
#define Beckhoff_EL2032_PDO_Diag2 0x6010, 1
#define Beckhoff_EL2032_Output1 Beckhoff_EL2032, Beckhoff_EL2032_PDO_Output1
#define Beckhoff_EL2032_Output2 Beckhoff_EL2032, Beckhoff_EL2032_PDO_Output2
#define Beckhoff_EL2032_Diag1 Beckhoff_EL2032, Beckhoff_EL2032_PDO_Diag1
#define Beckhoff_EL2032_Diag2 Beckhoff_EL2032, Beckhoff_EL2032_PDO_Diag2

#define Beckhoff_EL2622_PDO_Output1 0x7000, 1
#define Beckhoff_EL2622_PDO_Output2 0x7010, 1
#define Beckhoff_EL2622_Output1 Beckhoff_EL2622, Beckhoff_EL2622_PDO_Output1
#define Beckhoff_EL2622_Output2 Beckhoff_EL2622, Beckhoff_EL2622_PDO_Output2

#define Beckhoff_EL3102_PDO_Status1 0x3101, 1
#define Beckhoff_EL3102_PDO_Input1  0x3101, 2
#define Beckhoff_EL3102_PDO_Status2 0x3102, 1
#define Beckhoff_EL3102_PDO_Input2  0x3102, 2
#define Beckhoff_EL3102_Status1 Beckhoff_EL3102, Beckhoff_EL3102_PDO_Status1
#define Beckhoff_EL3102_Input1  Beckhoff_EL3102, Beckhoff_EL3102_PDO_Input1 
#define Beckhoff_EL3102_Status2 Beckhoff_EL3102, Beckhoff_EL3102_PDO_Status2
#define Beckhoff_EL3102_Input2  Beckhoff_EL3102, Beckhoff_EL3102_PDO_Input2

#define Beckhoff_EL3152_PDO_Status1 0x3101, 1
#define Beckhoff_EL3152_PDO_Input1  0x3101, 2
#define Beckhoff_EL3152_PDO_Status2 0x3102, 1
#define Beckhoff_EL3152_PDO_Input2  0x3102, 2
#define Beckhoff_EL3152_Status1 Beckhoff_EL3152, Beckhoff_EL3152_PDO_Status1
#define Beckhoff_EL3152_Input1  Beckhoff_EL3152, Beckhoff_EL3152_PDO_Input1
#define Beckhoff_EL3152_Status2 Beckhoff_EL3152, Beckhoff_EL3152_PDO_Status2
#define Beckhoff_EL3152_Input2  Beckhoff_EL3152, Beckhoff_EL3152_PDO_Input2


#define Beckhoff_EL3162_PDO_Status1 0x3101, 1
#define Beckhoff_EL3162_PDO_Input1  0x3101, 2
#define Beckhoff_EL3162_PDO_Status2 0x3102, 1
#define Beckhoff_EL3162_PDO_Input2  0x3102, 2
#define Beckhoff_EL3162_Status1 Beckhoff_EL3162, Beckhoff_EL3162_PDO_Status1
#define Beckhoff_EL3162_Input1  Beckhoff_EL3162, Beckhoff_EL3162_PDO_Input1
#define Beckhoff_EL3162_Status2 Beckhoff_EL3162, Beckhoff_EL3162_PDO_Status2
#define Beckhoff_EL3162_Input2  Beckhoff_EL3162, Beckhoff_EL3162_PDO_Input2





/*****************************************************************************/

/*****************************************************************************/


//EL3152
static ec_pdo_entry_info_t el3152_pdo_entries[] = {
    {Beckhoff_EL3152_PDO_Status1,  8}, // channel 1 status
    {Beckhoff_EL3152_PDO_Input1, 16}, // channel 1 value
    {Beckhoff_EL3152_PDO_Status2,  8}, // channel 2 status
    {Beckhoff_EL3152_PDO_Input2, 16} // channel 2 value
};

static ec_pdo_info_t el3152_pdos[] = {
    {0x1A00, 2, el3152_pdo_entries},
    {0x1A01, 2, el3152_pdo_entries + 2}
};

static ec_sync_info_t el3152_syncs[] = {
    {2, EC_DIR_OUTPUT},
    {3, EC_DIR_INPUT, 2, el3152_pdos},
    {0xff}
};


//EL3162
static ec_pdo_entry_info_t el3162_pdo_entries[] = {
    {Beckhoff_EL3162_PDO_Status1,  8}, // channel 1 status
    {Beckhoff_EL3162_PDO_Input1, 16}, // channel 1 value
    {Beckhoff_EL3162_PDO_Status2,  8}, // channel 2 status
    {Beckhoff_EL3162_PDO_Input2, 16} // channel 2 value
};

static ec_pdo_info_t el3162_pdos[] = {
    {0x1A00, 2, el3162_pdo_entries},
    {0x1A01, 2, el3162_pdo_entries + 2}
};

static ec_sync_info_t el3162_syncs[] = {
    {2, EC_DIR_OUTPUT},
    {3, EC_DIR_INPUT, 2, el3162_pdos},
    {0xff}
};

//EL2004
static ec_pdo_entry_info_t el2004_pdo_entries[] = {
    {Beckhoff_EL2004_PDO_Output1, 1}, // channel 1 value
    {Beckhoff_EL2004_PDO_Output2, 1}, // channel 2 value
    {Beckhoff_EL2004_PDO_Output3, 1}, // channel 3 value
    {Beckhoff_EL2004_PDO_Output4, 1} // channel 4 value
};

static ec_pdo_info_t el2004_pdos[] = {
    {0x1600, 1, el2004_pdo_entries},
    {0x1601, 1, el2004_pdo_entries + 1},
    {0x1602, 1, el2004_pdo_entries + 2},
    {0x1603, 1, el2004_pdo_entries + 3}
};

static ec_sync_info_t el2004_syncs[] = {
    {2, EC_DIR_OUTPUT, 4, el2004_pdos},
    {3, EC_DIR_INPUT},
    {0xff}
};

//EL2032
static ec_pdo_entry_info_t el2032_pdo_entries[] = {
    {Beckhoff_EL2032_PDO_Output1, 1}, // channel 1 value
    {Beckhoff_EL2032_PDO_Output2, 1}, // channel 2 value
    {Beckhoff_EL2032_PDO_Diag1, 1}, // Diagnose 1 value
    {Beckhoff_EL2032_PDO_Diag2, 1} // Diagnose 2 value
};

static ec_pdo_info_t el2032_pdos_output[] = {
    {0x1600, 1, el2032_pdo_entries},
    {0x1601, 1, el2032_pdo_entries + 1}
};

static ec_pdo_info_t el2032_pdos_diag[] = {
    {0x1A00, 1, el2032_pdo_entries + 2},
    {0x1A01, 1, el2032_pdo_entries + 3}
};

static ec_sync_info_t el2032_syncs[] = {
    {2, EC_DIR_OUTPUT, 2, el2032_pdos_output},
    {3, EC_DIR_INPUT, 2, el2032_pdos_diag},
    {0xff}
};


//EL2622
static ec_pdo_entry_info_t el2622_pdo_entries[] = {
    {Beckhoff_EL2622_PDO_Output1, 1}, // channel 1 value
    {Beckhoff_EL2622_PDO_Output2, 1} // channel 2 value
};

static ec_pdo_info_t el2622_pdos[] = {
    {0x1600, 1, el2622_pdo_entries},
    {0x1601, 1, el2622_pdo_entries + 1}
};

static ec_sync_info_t el2622_syncs[] = {
    {2, EC_DIR_OUTPUT, 2, el2622_pdos},
    {3, EC_DIR_INPUT},
    {0xff}
};



//EL1034
static ec_pdo_entry_info_t el1034_pdo_entries[] = {
    {Beckhoff_EL1034_PDO_Input1, 1}, // channel 1 value
    {Beckhoff_EL1034_PDO_Input2, 1}, // channel 2 value
    {Beckhoff_EL1034_PDO_Input3, 1}, // channel 3 value
    {Beckhoff_EL1034_PDO_Input4, 1} // channel 4 value
};

static ec_pdo_info_t el1034_pdos[] = {
    {0x1A00, 1, el1034_pdo_entries},
    {0x1A01, 1, el1034_pdo_entries + 1},
    {0x1A02, 1, el1034_pdo_entries + 2},
    {0x1A03, 1, el1034_pdo_entries + 3}
};

static ec_sync_info_t el1034_syncs[] = {
    {2, EC_DIR_OUTPUT},
    {3, EC_DIR_INPUT, 4, el1034_pdos},
    {0xff}
};

//EL1004
static ec_pdo_entry_info_t el1004_pdo_entries[] = {
    {Beckhoff_EL1004_PDO_Input1, 1}, // channel 1 value
    {Beckhoff_EL1004_PDO_Input2, 1}, // channel 2 value
    {Beckhoff_EL1004_PDO_Input3, 1}, // channel 3 value
    {Beckhoff_EL1004_PDO_Input4, 1} // channel 4 value
};

static ec_pdo_info_t el1004_pdos[] = {
    {0x1A00, 1, el1004_pdo_entries},
    {0x1A01, 1, el1004_pdo_entries + 1},
    {0x1A02, 1, el1004_pdo_entries + 2},
    {0x1A03, 1, el1004_pdo_entries + 3}
};

static ec_sync_info_t el1004_syncs[] = {
    {2, EC_DIR_OUTPUT},
    {3, EC_DIR_INPUT, 4, el1004_pdos},
    {0xff}
};

//EL1012
static ec_pdo_entry_info_t el1012_pdo_entries[] = {
    {Beckhoff_EL1012_PDO_Input1, 1}, // channel 1 value
    {Beckhoff_EL1012_PDO_Input2, 1} // channel 2 value
};

static ec_pdo_info_t el1012_pdos[] = {
    {0x1A00, 1, el1012_pdo_entries},
    {0x1A01, 1, el1012_pdo_entries + 1}
};

static ec_sync_info_t el1012_syncs[] = {
    {2, EC_DIR_OUTPUT},
    {3, EC_DIR_INPUT, 2, el1012_pdos},
    {0xff}
};

//EL1014
static ec_pdo_entry_info_t el1014_pdo_entries[] = {
    {Beckhoff_EL1014_PDO_Input1, 1}, // channel 1 value
    {Beckhoff_EL1014_PDO_Input2, 1}, // channel 2 value
    {Beckhoff_EL1014_PDO_Input3, 1}, // channel 3 value
    {Beckhoff_EL1014_PDO_Input4, 1} // channel 4 value
};

static ec_pdo_info_t el1014_pdos[] = {
    {0x1A00, 1, el1014_pdo_entries},
    {0x1A01, 1, el1014_pdo_entries + 1},
    {0x1A02, 1, el1014_pdo_entries + 2},
    {0x1A03, 1, el1014_pdo_entries + 3}
};

static ec_sync_info_t el1014_syncs[] = {
    {2, EC_DIR_OUTPUT},
    {3, EC_DIR_INPUT, 4, el1014_pdos},
    {0xff}
};


//EL1008
static ec_pdo_entry_info_t el1008_pdo_entries[] = {
    {Beckhoff_EL1008_PDO_Input1, 1}, // channel 1 value
    {Beckhoff_EL1008_PDO_Input2, 1}, // channel 2 value
    {Beckhoff_EL1008_PDO_Input3, 1}, // channel 3 value
    {Beckhoff_EL1008_PDO_Input4, 1}, // channel 4 value
    {Beckhoff_EL1008_PDO_Input5, 1}, // channel 5 value
    {Beckhoff_EL1008_PDO_Input6, 1}, // channel 6 value
    {Beckhoff_EL1008_PDO_Input7, 1}, // channel 7 value
    {Beckhoff_EL1008_PDO_Input8, 1} // channel 8 value
};

static ec_pdo_info_t el1008_pdos[] = {
    {0x1A00, 1, el1008_pdo_entries},
    {0x1A01, 1, el1008_pdo_entries + 1},
    {0x1A02, 1, el1008_pdo_entries + 2},
    {0x1A03, 1, el1008_pdo_entries + 3},
    {0x1A04, 1, el1008_pdo_entries + 4},
    {0x1A05, 1, el1008_pdo_entries + 5},
    {0x1A06, 1, el1008_pdo_entries + 6},
    {0x1A07, 1, el1008_pdo_entries + 7}
};

static ec_sync_info_t el1008_syncs[] = {
    {2, EC_DIR_OUTPUT},
    {3, EC_DIR_INPUT, 8, el1008_pdos},
    {0xff}
};

//EL1018
static ec_pdo_entry_info_t el1018_pdo_entries[] = {
    {Beckhoff_EL1018_PDO_Input1, 1}, // channel 1 value
    {Beckhoff_EL1018_PDO_Input2, 1}, // channel 2 value
    {Beckhoff_EL1018_PDO_Input3, 1}, // channel 3 value
    {Beckhoff_EL1018_PDO_Input4, 1}, // channel 4 value
    {Beckhoff_EL1018_PDO_Input5, 1}, // channel 5 value
    {Beckhoff_EL1018_PDO_Input6, 1}, // channel 6 value
    {Beckhoff_EL1018_PDO_Input7, 1}, // channel 7 value
    {Beckhoff_EL1018_PDO_Input8, 1} // channel 8 value
};

static ec_pdo_info_t el1018_pdos[] = {
    {0x1A00, 1, el1018_pdo_entries},
    {0x1A01, 1, el1018_pdo_entries + 1},
    {0x1A02, 1, el1018_pdo_entries + 2},
    {0x1A03, 1, el1018_pdo_entries + 3},
    {0x1A04, 1, el1018_pdo_entries + 4},
    {0x1A05, 1, el1018_pdo_entries + 5},
    {0x1A06, 1, el1018_pdo_entries + 6},
    {0x1A07, 1, el1018_pdo_entries + 7}
};

static ec_sync_info_t el1018_syncs[] = {
    {2, EC_DIR_OUTPUT},
    {3, EC_DIR_INPUT, 8, el1018_pdos},
    {0xff}
};


#endif //ECRT_SLAVES_H
