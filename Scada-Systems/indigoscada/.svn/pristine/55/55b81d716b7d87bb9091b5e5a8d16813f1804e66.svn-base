/*
 *  $Id: RTPSHeader.c,v 0.0.0.1  2003/08/21 
 *
 *  DEBUG:  section 40          Operations with header of RTPS
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
 */ 

#include "orte_all.h"

/**********************************************************************************/
int
RTPSHeaderCreate(CDR_Codec *cdrCodec,HostId hid,AppId aid) {
  CDR_Endianness  data_endian;
  ProtocolVersion pv;
  VendorId vid;

  CDR_put_octet(cdrCodec,'R');
  CDR_put_octet(cdrCodec,'T');
  CDR_put_octet(cdrCodec,'P');
  CDR_put_octet(cdrCodec,'S');

  /* Protocol version */
  PROTOCOL_VERSION_1_0(pv);
  CDR_put_octet(cdrCodec,pv.major);
  CDR_put_octet(cdrCodec,pv.minor);

  //Vendor id
  VENDOR_ID_UNKNOWN(vid);
  CDR_put_octet(cdrCodec,vid.major);
  CDR_put_octet(cdrCodec,vid.minor);

  /* next data are sent in big endianing */
  data_endian=cdrCodec->data_endian;
  cdrCodec->data_endian=FLAG_BIG_ENDIAN;

  /* Host Id */
  CDR_put_ulong(cdrCodec,hid);

  /* App Id */
  CDR_put_ulong(cdrCodec,aid);

  cdrCodec->data_endian=data_endian;

  return 16;
} 
/**********************************************************************************/
int
RTPSHeaderCheck(CDR_Codec *cdrCodec,int32_t len,MessageInterpret *mi) {
  CDR_Endianness  data_endian;
  CORBA_octet c;

  if (len<16) return -1;                            /* message is too small */
  CDR_get_octet(cdrCodec, (CORBA_octet *)&c);
  if (c!='R') return -2;                            /* header is invalid */
  CDR_get_octet(cdrCodec, (CORBA_octet *)&c);
  if (c!='T') return -2;                            
  CDR_get_octet(cdrCodec, (CORBA_octet *)&c);
  if (c!='P') return -2;                            
  CDR_get_octet(cdrCodec, (CORBA_octet *)&c);
  if (c!='S') return -2;                            

  /* Protocol Version */
  CDR_get_octet(cdrCodec, (CORBA_octet *)&mi->sourceVersion.major);
  CDR_get_octet(cdrCodec, (CORBA_octet *)&mi->sourceVersion.minor);

   /* Vendor Id */
  CDR_get_octet(cdrCodec, (CORBA_octet *)&mi->sourceVendorId.major);
  CDR_get_octet(cdrCodec, (CORBA_octet *)&mi->sourceVendorId.minor);

  /* next data are sent in big endianing */
  data_endian=cdrCodec->data_endian;
  cdrCodec->data_endian=FLAG_BIG_ENDIAN;

  /* Host Id */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&mi->sourceHostId);

  /* App Id */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&mi->sourceAppId);

  cdrCodec->data_endian=data_endian;

  mi->haveTimestamp=ORTE_FALSE;                     /* false */
  return 0;
}


