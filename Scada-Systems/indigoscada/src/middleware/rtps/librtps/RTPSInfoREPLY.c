/*
 *  $Id: RTPSInfoREPLY.c,v 0.0.0.1      2003/09/1
 *
 *  DEBUG:  section 43                  message INFO REPLY
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
RTPSInfoREPLYCreate(CDR_Codec *cdrCodec,IPAddress ipaddress,Port port)
{
  CORBA_octet flags;

  if (cdrCodec->buf_len<cdrCodec->wptr+12) return -1;

  /* submessage id */
  CDR_put_octet(cdrCodec,INFO_REPLY);

  /* flags */
  flags=cdrCodec->data_endian;
  CDR_put_octet(cdrCodec,flags);

  /* length */
  CDR_put_ushort(cdrCodec,8);

  /* ipaddress */
  CDR_put_ulong(cdrCodec,ipaddress);

  /* port */
  CDR_put_ulong(cdrCodec,port);
  return 12;
} 

/**********************************************************************************/
void 
RTPSInfoREPLY(CDR_Codec *cdrCodec,MessageInterpret *mi) 
{
  CORBA_octet        flags;  
  IPAddress          ipa;
  Port               port;

  /* restore flag possition in submessage */
  cdrCodec->rptr-=3;

  /* flags */
  CDR_get_octet(cdrCodec, (CORBA_octet *)&flags);

  /* move reading possition to begin of submessage */
  cdrCodec->rptr+=2;

  /* unicastReplyIPAddress */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&ipa);
  
  /* unicastReplyPort */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&port);
  
  debug(43,3) ("recv: RTPS InfoREPLY from 0x%x-0x%x\n",
                mi->sourceHostId,mi->sourceAppId);

  if (ipa!=IPADDRESS_INVALID) {
    mi->unicastReplyIPAddress=ipa;
  }
  mi->unicastReplyPort=port;

  if (flags & 0x02) {
    /* multicastReplyIPAddress */
    CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&ipa);

    /* multicastReplyPort */
    CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&port);

    mi->multicastReplyIPAddress=ipa;
    mi->multicastReplyPort=port;
  } else {
    mi->multicastReplyIPAddress=IPADDRESS_INVALID;
    mi->multicastReplyPort     =PORT_INVALID;
  }
}

