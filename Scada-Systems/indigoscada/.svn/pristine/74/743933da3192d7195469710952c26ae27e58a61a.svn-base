/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2014 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#ifndef PRODUCER_H

#define PRODUCER_H

#include <opcda.h>
#include <opcerror.h>

#define DEV_ID_MAX 15
//#define DEV_DATALEN_MAX 6

#define DEV_DATALEN_MAX 15

typedef struct DeviceRecord DevRecord;

struct DeviceRecord {
  //char *name;
	char name[150];
//  unsigned int writeable;
//  unsigned int readable;
  VARTYPE dtype;
//  int tEUtype;
//  double range_min;
//  double range_max;
//  int enumMax;
  //BOOL scan_it;
  unsigned int ioa_control_center; //unstructured
  double current_value;
  FILETIME ft;
  int      Quality;
//  int updated_in_this_load;
};

//#define DEVICE_DEMO

#ifdef DEVICE_DEMO

DevRecord DeviceDatabase[] =
  {{"POINT NAME01",	0x0000, 0x1213, VT_I2, 0, 0.0, 0.0, 0, true},
   {"POINT NAME02",	0x0000, 0x1212, VT_I2, 0, 0.0, 0.0, 0, true},
   {"POINT NAME03",	0x0100, 0x1100, VT_I2, 0, 0.0, 0.0, 0, true},
   {"POINT NAME04",	0x0101, 0x1101, VT_I2, 0, 0.0, 0.0, 0, true},
   {"POINT NAME05",	0x0102, 0x1102, VT_I2, 0, 0.0, 0.0, 0, true},
   {"POINT NAME06", 0x0103, 0x1103, VT_I2, OPC_ENUMERATED, 0.0, 0.0, 4, true},
   {"POINT NAME07", 0x0104, 0x1104, VT_I2, OPC_ENUMERATED, 0.0, 0.0, 1, true},
   {"POINT NAME08", 0x0105, 0x1105, VT_I2, 0, 0.0, 0.0, 0, true},
   {"POINT NAME09", 0x0106, 0x1106, VT_R4, 0, 0.0, 0.0, 0, true},
   {"POINT NAME010", 0x0107, 0x1107, VT_R4, 0, 0.0, 0.0, 0, true},
   {"POINT NAME011", 0x0108, 0x1108, VT_R4, 0, 0.0, 0.0, 0, true},
   {"POINT NAME012", 0x0109, 0x1109, VT_R4, 0, 0.0, 0.0, 0, true},
   {"POINT NAME013", 0x010A, 0x110A, VT_I2, 0, 0.0, 0.0, 0, true},
   {"POINT NAME014", 0x010B, 0x110B, VT_R4, 0, 0.0, 0.0, 0, true},
   {"POINT NAME015", 0x010C, 0x110C, VT_R4, 0, 0.0, 0.0, 0, true},
   {"POINT NAME016", 0x0200, 0x1200, VT_I2, OPC_ENUMERATED, 0.0, 0.0, 13, true},
   {"POINT NAME017", 0x0201, 0x1201, VT_I2, OPC_ENUMERATED, 0.0, 0.0, 1, true},
   {"POINT NAME018", 0x0202, 0x1202, VT_I2, OPC_ENUMERATED, 0.0, 0.0, 3, true},
   {"POINT NAME019", 0x0203, 0x1203, VT_I2, 0, 0.0, 0.0, 0, true},
   {"POINT NAME020", 0x0204, 0x1204, VT_I2, 0, 0.0, 0.0, 0, true},
   {"POINT NAME021", 0x0205, 0x1205, VT_I2, OPC_ENUMERATED, 0.0, 0.0, 3, true},
   {"POINT NAME022", 0x0206, 0x1206, VT_I2, 0, 0.0, 0.0, 0, true},
   {"POINT NAME023", 0x0207, 0x1207, VT_I2, 0, 0.0, 0.0, 0, true},
   {"POINT NAME024", 0x0208, 0x1208, VT_I2, 0, 0.0, 0.0, 0, true},
   {"POINT NAME025", 0x0209, 0x1209, VT_I2, OPC_ENUMERATED, 0.0, 0.0, 2, true},
   {"POINT NAME026", 0x020A, 0x120A, VT_R4, 0, 0.0, 0.0, 0, true},
   {"POINT NAME027", 0x020B, 0x120B, VT_R4, 0, 0.0, 0.0, 0, true},
   {"POINT NAME028", 0x020C, 0x120C, VT_I2, OPC_ENUMERATED, 0.0, 0.0, 1, true},
   {"POINT NAME029", 0x020D, 0x120D, VT_I2, OPC_ENUMERATED, 0.0, 0.0, 2, true},
   {"POINT NAME030", 0x020E, 0x120E, VT_I2, 0, 0.0, 0.0, 0, true},
   {"POINT NAME031", 0x020F, 0x120F, VT_R4, 0, 0.0, 0.0, 0, true},
   {"POINT NAME032", 0x0210, 0x1210, VT_I2, OPC_ENUMERATED, 0.0, 0.0, 1, true},
   {"POINT NAME033", 0x0211, 0x1211, VT_R4, OPC_ENUMERATED, 0.0, 0.0, 1, true},
   {"POINT NAME034", 0x0214, 0x1214, VT_I2, 0, 0.0, 0.0, 0, true},
   {"POINT NAME035", 0x0000, 0x1431, VT_BSTR, 0, 0.0, 0.0, 0, true},
   {"POINT NAME036", 0x0000, 0x1432, VT_BSTR, 0, 0.0, 0.0, 0, true},
   {"POINT NAME037", 0x0000, 0x1433, VT_BSTR, 0, 0.0, 0.0, 0, true},
   {"POINT NAME038", 0x0000, 0x1434, VT_R4, 0, 0.0, 0.0, 0, true},
   {"POINT NAME039", 0x0000, 0x1435, VT_R4, 0, 0.0, 0.0, 0, true},
   {"POINT NAME040", 0x0400, 0x0000, VT_I2, OPC_ENUMERATED, 0.0, 0.0, 1, true},
   {"POINT NAME041", 0x0401, 0x0000, VT_R4, 0, 0.0, 0.0, 0, true},
   {"POINT NAME042", 0x0402, 0x0000, VT_I2, OPC_ENUMERATED, 0.0, 0.0, 1, true},
   {"POINT NAME043", 0x0403, 0x0000, VT_I2, 0, 0.0, 0.0, -1, true},
   NULL
  };

#else

extern DevRecord *DeviceDatabase;

#endif

typedef struct Device DEV;

struct Device {

  time_t mtime; // measurement time
  int cv_size;
  LPSTR *cv;
  int *cv_id;
  BOOL *cv_status;
  int ids[DEV_ID_MAX+1];
  int idnum;

  Device(): mtime(0), idnum(0)
  {
    int i;
    
    for (i = 0, cv_size = 0; DeviceDatabase[i].name[0] != NULL; i++)
	{
      //if (DeviceDatabase[i].scan_it)
	  //{
		cv_size += 1;
	  //}
	}

    cv = new LPSTR[cv_size];
    cv_status = new BOOL[cv_size];
    cv_id = new int[cv_size];

    int cv_i;
    for (i = 0, cv_i = 0; DeviceDatabase[i].name[0] != NULL; i++)
	{
      //if (DeviceDatabase[i].scan_it) 
	  //{
		cv[cv_i] = new char[DEV_DATALEN_MAX+1];
		cv_status[cv_i] = FALSE;
		cv_id[cv_i] = i;
		cv_i++;
      //}
    }
  }

  ~Device()
  {
    int i;
    for (i =0; i < cv_size; i++)
      delete[] cv[i];

    delete[] cv_status;
    delete[] cv_id;
    delete[] cv;
  }
};

#endif // PRODUCER_H
