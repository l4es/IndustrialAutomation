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

#ifndef MODBUS_DB_H
#define MODBUS_DB_H

//We express the MODBUS types like the OPC types

enum MODBUS_TYPE {	
	VT_I2	= 2,
	VT_I4	= 3,
	VT_R4	= 4,
	VT_R8	= 5,
	VT_ERROR = 10,
	VT_BOOL	= 11,
	VT_I1	= 16,
	VT_UI1	= 17,
	VT_UI2	= 18,
	VT_UI4	= 19,
	VT_I8	= 20,
	VT_UI8	= 21,
	VT_R4SWAP = 22
};

struct modbusDbRecord
{
	//////////////////MODBUS RTU part/////////////////////////////////////////
	int slave_id;				//Slave ID of RTU
	int modbus_function_read;	//modbus funtion to read
	int modbus_function_write;	//modbus funtion to write
	int modbus_address;	//address of the memory to fetch
	int modbus_type; //modbus type expressed like an OPC type, eg. VT_BOOL
	//////////////control center part/////////////////////////////////////////
	unsigned int ioa_control_center; //unique inside CASDU
	float deadband; //Deadband for analog values
	union {
		unsigned int ua;
		int a;
		float f;
    } last_value;
};

#endif //MODBUS_DB_H