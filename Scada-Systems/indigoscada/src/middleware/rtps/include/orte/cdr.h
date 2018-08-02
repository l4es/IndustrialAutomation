/*
 *  $Id: cdr.h,v 0.0.0.1                2004/11/26 
 *
 *  -------------------------------------------------------------------  
 *                                ORTE                                 
 *                      Open Real-Time Ethernet                       
 *                                                                    
 *                      Copyright (C) 2001-2006                       
 *  Department of Control Engineering FEE CTU Prague, Czech Republic  
 *                      http://dce.felk.cvut.cz                       
 *                      http://www.ocera.org                          
 *                                                                    
 *  Author: 		 Petr Smolik	petr.smolik@wo.cz             
 *  Advisor: 		 Pavel Pisa                                   
 *  Project Responsible: Zdenek Hanzalek                              
 *  --------------------------------------------------------------------
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  Original of source was from ORBit: A CORBA v2.2 ORB
 */
#ifndef _ORTE_CDR_H_
#define _ORTE_CDR_H_

#include "basic_types.h"

/*
 * Alignment of CORBA types mapped to C.
 * These have *nothing* to do with CDR alignment.
 */
#define ORTE_ALIGNOF_CORBA_OCTET        1
#define ORTE_ALIGNOF_CORBA_BOOLEAN      1
#define ORTE_ALIGNOF_CORBA_CHAR         1
#define ORTE_ALIGNOF_CORBA_WCHAR        2
#define ORTE_ALIGNOF_CORBA_SHORT        2
#define ORTE_ALIGNOF_CORBA_LONG         4
#define ORTE_ALIGNOF_CORBA_LONG_LONG    8
#define ORTE_ALIGNOF_CORBA_FLOAT        4
#define ORTE_ALIGNOF_CORBA_DOUBLE       8
#define ORTE_ALIGNOF_CORBA_LONG_DOUBLE  16
#define ORTE_ALIGNOF_CORBA_STRUCT       1
#define ORTE_ALIGNOF_CORBA_POINTER      4

typedef enum {
	BigEndian=0,
	LittleEndian=1
} CDR_Endianness;

/**
 * struct CDR_Codec - used for serialization/deserialization
 * @host_endian: 
 * @data_endian: 
 * @buffer: buffer for data
 * @buf_len: buffer length
 * @wptr_max: maximal size of writing data 
 * @wptr: write pointer
 * @rptr: read pointer
 * @readonly: readonly attribute 
 * @release_buffer: use CORBA_TRUE if is necessary to free buffer memory after destruction of structure
 *
 * Struct @CDR_Codec is used by serialization and deserialization functions.
 */
typedef struct CDR_Codec {
	CDR_Endianness host_endian;
	CDR_Endianness data_endian;
	CORBA_octet *buffer;
	unsigned int buf_len;
	unsigned int wptr_max;
	unsigned int wptr;
	unsigned int rptr;
	CORBA_boolean readonly;
	CORBA_boolean release_buffer;
} CDR_Codec;

#define HEXDIGIT(c) (isdigit((guchar)(c))?(c)-'0':tolower((guchar)(c))-'a'+10)
#define HEXOCTET(a,b) ((HEXDIGIT((a)) << 4) | HEXDIGIT((b)))

extern CORBA_boolean CDR_buffer_init(CDR_Codec *codec, const unsigned int size);
extern CDR_Codec *CDR_codec_init(void);
extern CDR_Codec *CDR_codec_init_static(CDR_Codec *codec);
extern void CDR_codec_release_buffer(CDR_Codec *codec);
extern void CDR_codec_free(CDR_Codec *);

extern CORBA_boolean CDR_buffer_puts(CDR_Codec *codec, const void *data, const unsigned int len);
extern CORBA_boolean CDR_buffer_gets(CDR_Codec *codec, void *dest, const unsigned int len);

extern CORBA_boolean CDR_put_short(CDR_Codec *codec, CORBA_short s);
extern CORBA_boolean CDR_put_ushort(CDR_Codec *codec, CORBA_unsigned_short us);
extern CORBA_boolean CDR_put_long(CDR_Codec *codec, CORBA_long l);
extern CORBA_boolean CDR_put_ulong(CDR_Codec *codec, CORBA_unsigned_long ul);
#ifdef HAVE_CORBA_LONG_LONG
extern CORBA_boolean CDR_put_long_long(CDR_Codec *codec, CORBA_long_long ll);
extern CORBA_boolean CDR_put_ulong_long(CDR_Codec *codec, CORBA_unsigned_long_long ull);
extern CORBA_boolean CDR_get_ulong_long(CDR_Codec *codec, CORBA_unsigned_long_long *ul);
extern CORBA_boolean CDR_get_long_long(CDR_Codec *codec, CORBA_long_long *ul);
#endif
extern CORBA_boolean CDR_put_float(CDR_Codec *codec, CORBA_float f);
extern CORBA_boolean CDR_put_double(CDR_Codec *codec, CORBA_double d);
extern CORBA_boolean CDR_put_long_double(CDR_Codec *codec, CORBA_long_double ld);
extern CORBA_boolean CDR_put_octet(CDR_Codec *codec, CORBA_octet datum);
extern CORBA_boolean CDR_put_octets(CDR_Codec *codec, void *data, unsigned long len);
extern CORBA_boolean CDR_put_char(CDR_Codec *codec, CORBA_char c);
extern CORBA_boolean CDR_put_boolean(CDR_Codec *codec, CORBA_boolean datum);
extern CORBA_boolean CDR_put_string(CDR_Codec *codec, const char *str);
extern CORBA_boolean CDR_buffer_gets(CDR_Codec *codec, void *dest, const unsigned int len);
extern CORBA_boolean CDR_get_short(CDR_Codec *codec, CORBA_short *us);
extern CORBA_boolean CDR_get_ushort(CDR_Codec *codec, CORBA_unsigned_short *us);
extern CORBA_boolean CDR_get_long(CDR_Codec *codec, CORBA_long *l);
extern CORBA_boolean CDR_get_ulong(CDR_Codec *codec, CORBA_unsigned_long *ul);
extern CORBA_boolean CDR_get_octet(CDR_Codec *codec, CORBA_octet *datum);
extern CORBA_boolean CDR_get_boolean(CDR_Codec *codec, CORBA_boolean *b);
extern CORBA_boolean CDR_get_char(CDR_Codec *codec, CORBA_char *c);
extern CORBA_boolean CDR_get_string(CDR_Codec *codec, CORBA_char **str);
extern CORBA_boolean CDR_get_string_buff(CDR_Codec *codec, CORBA_char *str);
extern CORBA_boolean CDR_get_string_static(CDR_Codec *codec, CORBA_char **str);
extern CORBA_boolean CDR_get_seq_begin(CDR_Codec *codec, CORBA_unsigned_long *ul);
extern CORBA_boolean CDR_get_float(CDR_Codec *codec, CORBA_float *f);
extern CORBA_boolean CDR_get_double(CDR_Codec *codec, CORBA_double *d);


/* serialization functions */
#define CORBA_short_serialize(x,y)             CDR_put_short((x),*(y))
#define CORBA_long_serialize(x,y)              CDR_put_long((x),*(y))
#define CORBA_unsigned_short_serialize(x,y)    CDR_put_ushort((x),*(y))
#define CORBA_unsigned_long_serialize(x,y)     CDR_put_ulong((x),*(y))
#define CORBA_float_serialize(x,y)             CDR_put_float((x),*(y))
#define CORBA_double_serialize(x,y)            CDR_put_double((x),*(y))
#define CORBA_char_serialize(x,y)              CDR_put_char((x),*(y))
#define CORBA_boolean_serialize(x,y)           CDR_put_boolean((x),*(y))
#define CORBA_octet_serialize(x,y)             CDR_put_octet((x),*(y))
#define CORBA_long_double_serialize(x,y)       CDR_put_long_double((x),*(y))
#define CORBA_string_serialize(x,y)            CDR_put_string((x),*(y))

/* deserialization functions */
#define CORBA_short_deserialize(x,y)           CDR_get_short((x),(y))
#define CORBA_long_deserialize(x,y)            CDR_get_long((x),(y))
#define CORBA_unsigned_short_deserialize(x,y)  CDR_get_ushort((x),(y))
#define CORBA_unsigned_long_deserialize(x,y)   CDR_get_ulong((x),(y))
#define CORBA_float_deserialize(x,y)           CDR_get_float((x),(y))
#define CORBA_double_deserialize(x,y)          CDR_get_double((x),(y))
#define CORBA_char_deserialize(x,y)            CDR_get_char((x),(y))
#define CORBA_boolean_deserialize(x,y)         CDR_get_boolean((x),(y))
#define CORBA_octet_deserialize(x,y)           CDR_get_octet((x),(y))
#define CORBA_long_double_deserialize(x,y)     CDR_get_long_double((x),(y))
#define CORBA_string_deserialize(x,y)          CDR_get_string((x),(y))

/* get_max_size functions */
#define CORBA_short_get_max_size(x)            ((x)->csize=\
	(unsigned long)ALIGN_ADDRESS((x)->csize,ORTE_ALIGNOF_CORBA_SHORT)+ORTE_ALIGNOF_CORBA_SHORT)
#define CORBA_long_get_max_size(x)             ((x)->csize=\
	(unsigned long)ALIGN_ADDRESS((x)->csize,ORTE_ALIGNOF_CORBA_LONG)+ORTE_ALIGNOF_CORBA_LONG)
#define CORBA_unsigned_short_get_max_size(x)   ((x)->csize=\
	(unsigned long)ALIGN_ADDRESS((x)->csize,ORTE_ALIGNOF_CORBA_SHORT)+ORTE_ALIGNOF_CORBA_SHORT)
#define CORBA_unsigned_long_get_max_size(x)    ((x)->csize=\
	(unsigned long)ALIGN_ADDRESS((x)->csize,ORTE_ALIGNOF_CORBA_LONG)+ORTE_ALIGNOF_CORBA_LONG)
#define CORBA_float_get_max_size(x)            ((x)->csize=\
	(unsigned long)ALIGN_ADDRESS((x)->csize,ORTE_ALIGNOF_CORBA_FLOAT)+ORTE_ALIGNOF_CORBA_FLOAT)
#define CORBA_double_get_max_size(x)           ((x)->csize=\
	(unsigned long)ALIGN_ADDRESS((x)->csize,ORTE_ALIGNOF_CORBA_DOUBLE)+ORTE_ALIGNOF_CORBA_DOUBLE)
#define CORBA_char_get_max_size(x)             ((x)->csize=\
	(unsigned long)ALIGN_ADDRESS((x)->csize,ORTE_ALIGNOF_CORBA_CHAR)+ORTE_ALIGNOF_CORBA_CHAR)
#define CORBA_boolean_get_max_size(x)          ((x)->csize=\
	(unsigned long)ALIGN_ADDRESS((x)->csize,ORTE_ALIGNOF_CORBA_BOOLEAN)+ORTE_ALIGNOF_CORBA_BOOLEAN)
#define CORBA_octet_get_max_size(x)            ((x)->csize=\
	(unsigned long)ALIGN_ADDRESS((x)->csize,ORTE_ALIGNOF_CORBA_OCTET)+ORTE_ALIGNOF_CORBA_OCTET)
#define CORBA_long_double_get_max_size(x)      ((x)->csize=\
	(unsigned long)ALIGN_ADDRESS((x)->csize,ORTE_ALIGNOF_CORBA_LONG_DOUBLE)+ORTE_ALIGNOF_CORBA_DOUBLE)
#define CORBA_string_get_max_size(x,y)         \
	((x)->csize=(unsigned long)ALIGN_ADDRESS((x)->csize,ORTE_ALIGNOF_CORBA_LONG) + ORTE_ALIGNOF_CORBA_LONG + y + 1)

#endif /* !_ORTE_CDR_H_ */
