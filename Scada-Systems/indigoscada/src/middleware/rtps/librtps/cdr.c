/*
 *  $Id: cdr.c,v 0.0.0.1                2004/11/26
 *
 *  DEBUG:  section 5                   CDR codeing
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
 *  This module maintains a hash table of key/value pairs.
 *  Keys can be strings of any size, or numbers up to size
 *  unsigned long (HASHKEYTYPE).
 *  Values should be a pointer to some data you wish to store.
 *
 *  Original of source was from ORBit: A CORBA v2.2 ORB
 *
 */

#include "orte_all.h"

#define CDR_GROW_AMOUNT 128

CORBA_boolean 
CDR_buffer_init(CDR_Codec *codec, const unsigned int size)
{

	if(codec->release_buffer) {
		FREE(codec->buffer);
	}

	codec->buffer=(CORBA_octet *)MALLOC(size);
	codec->buf_len=size;
        codec->wptr_max=size;
	codec->rptr=codec->wptr=0;
        codec->release_buffer=CORBA_TRUE;

	return CORBA_TRUE;
}

/*
CORBA_boolean 
CDR_buffer_grow(CDR_Codec *codec, const unsigned int growth)
{
	unsigned int real_growth,quot;

	if(codec->release_buffer) {
                quot = growth / CDR_GROW_AMOUNT;
		real_growth=CDR_GROW_AMOUNT * (quot+1);
		
		codec->buffer=(CORBA_octet *)REALLOC(codec->buffer,
						     codec->buf_len
						     +real_growth);
	}

	return CORBA_TRUE;
}
*/
CORBA_boolean 
CDR_buffer_puts(CDR_Codec *codec, const void *data, const unsigned int len)
{
	if(codec->wptr+len > codec->wptr_max) {
		return CORBA_FALSE; 
	}

	memcpy(&codec->buffer[codec->wptr], data, len);
	codec->wptr+=len;

	return CORBA_TRUE;
}

CORBA_boolean 
CDR_buffer_gets(CDR_Codec *codec, void *dest, const unsigned int len)
{
	if(codec->rptr+len > codec->buf_len) {
		return CORBA_FALSE; 
	}

	memcpy(dest, &codec->buffer[codec->rptr], len);
	codec->rptr+=len;

	return CORBA_TRUE;
}

CORBA_boolean 
CDR_buffer_put(CDR_Codec *codec, void *datum)
{
	if(codec->wptr+1 > codec->wptr_max) {
		return(CORBA_FALSE);
	}

	codec->buffer[codec->wptr++]=*(unsigned char *)datum;
	return CORBA_TRUE;
}

CORBA_boolean 
CDR_buffer_get(CDR_Codec *codec, void *dest)
{
	if(codec->rptr+1 > codec->buf_len) {
		return(CORBA_FALSE);
	}

	*(CORBA_octet *)dest=codec->buffer[codec->rptr++];
	return CORBA_TRUE;
}

#define CDR_buffer_put2(codec, datum) CDR_buffer_putn(codec, datum, 2)
#define CDR_buffer_put4(codec, datum) CDR_buffer_putn(codec, datum, 4)
#define CDR_buffer_put8(codec, datum) CDR_buffer_putn(codec, datum, 8)
#define CDR_buffer_put16(codec, datum) CDR_buffer_putn(codec, datum, 16)
#define CDR_buffer_get2(codec, dest) CDR_buffer_getn(codec, dest, 2)
#define CDR_buffer_get4(codec, dest) CDR_buffer_getn(codec, dest, 4)
#define CDR_buffer_get8(codec, dest) CDR_buffer_getn(codec, dest, 8)
#define CDR_buffer_get16(codec, dest) CDR_buffer_getn(codec, dest, 16)

static CORBA_boolean
CDR_buffer_getn(CDR_Codec *codec, void *dest, int bsize)
{
	codec->rptr = (unsigned long)ALIGN_ADDRESS(codec->rptr, bsize);
	if(codec->host_endian==codec->data_endian)
		memcpy(dest, codec->buffer + codec->rptr, bsize);
	else
		rtps_byteswap(dest, codec->buffer + codec->rptr, bsize);
	codec->rptr += bsize;

	return CORBA_TRUE;
}

static CORBA_boolean
CDR_buffer_putn(CDR_Codec *codec, void *datum, int bsize)
{
        unsigned long forward,i;
	
	forward = (unsigned long)ALIGN_ADDRESS(codec->wptr, bsize);
	if (forward+bsize > codec->wptr_max) {
		return CORBA_FALSE;
	}	

	i = codec->wptr;
        while(forward > i)
	        codec->buffer[i++] = '\0';

	codec->wptr = forward;
	if(codec->host_endian==codec->data_endian)
		memcpy(codec->buffer + codec->wptr, datum, bsize);
	else
		rtps_byteswap(codec->buffer + codec->wptr, datum, bsize);
	codec->wptr += bsize;

	return CORBA_TRUE;
}

#define CDR_swap2(d,s) rtps_byteswap((d), (s), 2)
#define CDR_swap4(d,s) rtps_byteswap((d), (s), 4)
#define CDR_swap8(d,s) rtps_byteswap((d), (s), 8)
#define CDR_swap16(d,s) rtps_byteswap((d), (s), 16)

inline CORBA_boolean 
CDR_put_short(CDR_Codec *codec, CORBA_short s)
{
	return CDR_buffer_put2(codec, &s);
}

inline CORBA_boolean 
CDR_get_short(CDR_Codec *codec, CORBA_short *s)
{
	return CDR_buffer_get2(codec, s);
}

inline CORBA_boolean 
CDR_put_ushort(CDR_Codec *codec, CORBA_unsigned_short us)
{
	return CDR_buffer_put2(codec, &us);
}

inline CORBA_boolean 
CDR_get_ushort(CDR_Codec *codec, CORBA_unsigned_short *us)
{
	return CDR_buffer_get2(codec, us);
}

inline CORBA_boolean 
CDR_put_long(CDR_Codec *codec, CORBA_long l)
{
	return CDR_buffer_put4(codec, &l);
}

inline CORBA_boolean 
CDR_get_long(CDR_Codec *codec, CORBA_long *l)
{
	return CDR_buffer_get4(codec, l);
}

inline CORBA_boolean 
CDR_put_ulong(CDR_Codec *codec, CORBA_unsigned_long ul)
{
	return CDR_buffer_put4(codec, &ul);
}

inline CORBA_boolean 
CDR_get_ulong(CDR_Codec *codec, CORBA_unsigned_long *ul)
{
	return CDR_buffer_get4(codec, ul);
}

#ifdef HAVE_CORBA_LONG_LONG
inline CORBA_boolean 
CDR_get_long_long(CDR_Codec *codec, CORBA_long_long *ul)
{
	return CDR_buffer_get8(codec, ul);
}

inline CORBA_boolean 
CDR_put_long_long(CDR_Codec *codec, CORBA_long_long ll)
{
	return CDR_buffer_put8(codec, &ll);
}

inline CORBA_boolean 
CDR_put_ulong_long(CDR_Codec *codec, CORBA_unsigned_long_long ll)
{
	return CDR_buffer_put8(codec, &ll);
}

inline CORBA_boolean 
CDR_get_ulong_long(CDR_Codec *codec, CORBA_unsigned_long_long *ull)
{
	return CDR_buffer_get8(codec, ull);
}
#endif

inline CORBA_boolean 
CDR_put_float(CDR_Codec *codec, CORBA_float f)
{
	return CDR_buffer_put4(codec, &f);
}

inline CORBA_boolean 
CDR_get_float(CDR_Codec *codec, CORBA_float *f)
{
	return CDR_buffer_get4(codec, f);
}

inline CORBA_boolean 
CDR_put_double(CDR_Codec *codec, CORBA_double d)
{
	return CDR_buffer_put8(codec, &d);
}

inline CORBA_boolean 
CDR_get_double(CDR_Codec *codec, CORBA_double *d)
{
	return CDR_buffer_get8(codec, d);
}

inline CORBA_boolean 
CDR_put_long_double(CDR_Codec *codec, CORBA_long_double ld)
{
	return CDR_buffer_put16(codec, &ld);
}

inline CORBA_boolean 
CDR_put_octet(CDR_Codec *codec, CORBA_octet datum)
{
	return CDR_buffer_put(codec, &datum);
}

inline CORBA_boolean 
CDR_get_octet(CDR_Codec *codec, CORBA_octet *datum)
{
	return(CDR_buffer_get(codec, datum));
}

inline CORBA_boolean 
CDR_put_octets(CDR_Codec *codec, void *data, unsigned long len)
{
	return CDR_buffer_puts(codec, data, len);
}

inline CORBA_boolean 
CDR_put_char(CDR_Codec *codec, CORBA_char c)
{
	return CDR_buffer_put(codec, &c);
}

inline CORBA_boolean 
CDR_get_char(CDR_Codec *codec, CORBA_char *c)
{
	return CDR_buffer_get(codec, c);
}

inline CORBA_boolean 
CDR_put_boolean(CDR_Codec *codec, CORBA_boolean datum)
{
	datum = datum&&1;
	return CDR_buffer_put(codec, &datum);
}

inline CORBA_boolean 
CDR_get_boolean(CDR_Codec *codec, CORBA_boolean *b)
{
	return CDR_buffer_get(codec, b);
}

CORBA_boolean 
CDR_put_string(CDR_Codec *codec, const char *str)
{
	unsigned int len;

	len=strlen(str)+1;

	if (CDR_put_ulong(codec, len)==CORBA_FALSE) return -1;
	return CDR_buffer_puts(codec, str, len);
}

CORBA_boolean 
CDR_get_string_static(CDR_Codec *codec,CORBA_char **str)
{
	CORBA_unsigned_long len;

	if(CDR_get_ulong(codec, (CORBA_unsigned_long *)&len)==CORBA_FALSE)
		return CORBA_FALSE;

	if((codec->rptr + len) > codec->buf_len)
		return CORBA_FALSE;

	*str = ((CORBA_char *)codec->buffer) + codec->rptr;

	codec->rptr += len;

	return CORBA_TRUE;
}

CORBA_boolean 
CDR_get_string(CDR_Codec *codec, CORBA_char **str)
{
	CORBA_unsigned_long len;

	if(CDR_get_ulong(codec, (CORBA_unsigned_long *)&len)==CORBA_FALSE)
		return(CORBA_FALSE);

	if(len==0)
		return(CORBA_FALSE);

	*str=MALLOC(len);

	if(CDR_buffer_gets(codec, *str, len)==CORBA_FALSE) {
		FREE(*str);
		return(CORBA_FALSE);
	}

	if((*str)[len-1]!='\0') {
		(*str)[len-1]='\0';
	}

	return(CORBA_TRUE);
}

CORBA_boolean 
CDR_get_string_buff(CDR_Codec *codec, CORBA_char *str)
{
	CORBA_unsigned_long len;

	if(CDR_get_ulong(codec, (CORBA_unsigned_long *)&len)==CORBA_FALSE)
		return(CORBA_FALSE);

	if(len==0)
		return(CORBA_FALSE);

	if(CDR_buffer_gets(codec, str, len)==CORBA_FALSE) {
		return(CORBA_FALSE);
	}

	if(str[len-1]!='\0') {
		str[len-1]='\0';
	}

	return(CORBA_TRUE);
}

CORBA_boolean 
CDR_get_seq_begin(CDR_Codec *codec, CORBA_unsigned_long *ul)
{
	return(CDR_get_ulong(codec, (CORBA_unsigned_long *)ul));
}

CDR_Codec *
CDR_codec_init_static(CDR_Codec *codec)
{
	memset(codec, 0, sizeof(CDR_Codec));

	codec->host_endian = FLAG_ENDIANNESS;

	return codec;
}

CDR_Codec *
CDR_codec_init(void)
{
	CDR_Codec *c;

	c=MALLOC(sizeof(CDR_Codec));
	CDR_codec_init_static(c);

	return(c);
}

void 
CDR_codec_release_buffer(CDR_Codec *codec)
{
	if(codec->release_buffer)
		FREE(codec->buffer);
}


void 
CDR_codec_free(CDR_Codec *codec)
{
	CDR_codec_release_buffer(codec);
	FREE(codec);
}
