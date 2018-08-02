/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#ifndef IEC_ITEM__H
#define IEC_ITEM__H

#if defined( _MSC_VER)          /* Microsoft C */
    #pragma pack(1)             /* Byte Alignment   */
#endif

/* Information object */
struct iec_object {
	u_int		ioa;	/* information object address */
	union {
		 iec_type1	type1;
		 iec_type3	type3;
		 iec_type7	type7;
		 iec_type9	type9;
		 iec_type11	type11;
		 iec_type13	type13;
		 iec_type15	type15;
		 iec_type30	type30;
		 iec_type31	type31;
		 iec_type33	type33;
		 iec_type34	type34;
		 iec_type35	type35;
		 iec_type36	type36;
		 iec_type37	type37;
		 iec_type45	type45;
		 iec_type46	type46;
		 iec_type48	type48;
		 iec_type49	type49;
		 iec_type50	type50;
		 iec_type51	type51;
		 iec_type58	type58;
		 iec_type59	type59;
		 iec_type61	type61;
		 iec_type62	type62;
		 iec_type63	type63;
		 iec_type64	type64;
		 iec_type70	type70;
		 iec_type100	type100;
		 iec_type103	type103;
		 iec_type104	type104;
		 iec_type105	type105;
		 iec_type106	type106;
		 iec_type107	type107;
		 iec_type120	type120;
		 iec_type121	type121;
		 iec_type122	type122;
		 iec_type123	type123;
		 iec_type124	type124;
		 iec_type125	type125;
		 iec_type126	type126;
		 is_type150	type150;
		 is_type151	type151;
		 is_type152	type152;
		 is_type153	type153;
		 is_type154	type154;
		 is_type155	type155;
		 is_type156	type156;
	} o;	
};

//Record format of configuration database
struct iec_item {
    u_char iec_type;
	struct iec_object iec_obj;
	unsigned char cause; //spontaneous or general interrogation cause
	u_int   msg_id; //ID of the message
	unsigned int ioa_control_center; //unstructured
	u_short casdu; //IEC 104 CASDU where this record is allocated, it is unstructured
    u_char	is_neg; //positive == 0 or negative == 1
	u_char	checksum; //Checksum of the message, must be the last if filled with CRC 8
};

#if defined( _MSC_VER)          /* Microsoft C */
    #pragma pack()              /* Byte Alignment   */
#endif

#endif //IEC_ITEM__H