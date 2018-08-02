/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2012 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#ifndef RFC_1006_ITEM_H
#define RFC_1006_ITEM_H

typedef enum {s7_u8 = 1, s7_s8, s7_u16, s7_s16, s7_u32, s7_s32, s7_float, s7_string, s7_bit} plcs7type;

struct rfc1006Item
{
	//////////////////RTU part/////////////////////////////////////////
	char name[100];			//Item ID is an unique name identifing it
	char type[30];			//S7 PLC data types: 's7_u8', 's7_s8', 's7_u16', 's7_s16', 's7_u32', 's7_s32' (unsigned or signed integer types), 's7_float', 's7_string' and 's7_bit'
	int datablock;			//data block number
	int offset_addr;		//offset address into the data block
	int string_size;		//only for strings: the length of the string in bytes including the 2 length bytes
	int bit;			//only for bits: the bit number
	int writeable;			//1 if the value is writable
	//////////////control center part///////////////////////////////////
	unsigned int ioa_control_center; //unique inside CASDU
	unsigned int iec_type;   //IEC 104 type
};

#endif //RFC_1006_ITEM_H