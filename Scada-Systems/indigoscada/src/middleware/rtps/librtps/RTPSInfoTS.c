/*
 *  $Id: RTPSInfoTS.c,v 0.0.0.1         2003/09/10
 *
 *  DEBUG:  section 45                  message INFO TS
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
RTPSInfoTSCreate(CDR_Codec *cdrCodec,NtpTime time) {
  CORBA_octet flags;

  if (cdrCodec->buf_len<cdrCodec->wptr+12) return -1;

  /* submessage id */
  CDR_put_octet(cdrCodec,INFO_TS);

  /* flags - I-bit=0 */
  flags=cdrCodec->data_endian;
  CDR_put_octet(cdrCodec,flags);

  /* length */
  CDR_put_ushort(cdrCodec,8);

  /* time in seconds */
  CDR_put_long(cdrCodec,time.seconds);

  /* time in seconds / 2^32 */
  CDR_put_ulong(cdrCodec,time.fraction);

  return 12;
}

/**********************************************************************************/
void 
RTPSInfoTS(CDR_Codec *cdrCodec,MessageInterpret *mi) {
  char 		     buff[MAX_STRING_NTPTIME_LENGTH];
  CORBA_octet        flags;  

  /* restore flag possition in submessage */
  cdrCodec->rptr-=3;

  /* flags */
  CDR_get_octet(cdrCodec, (CORBA_octet *)&flags);

  /* move reading possition to begin of submessage */
  cdrCodec->rptr+=2;


  if ((flags & 0x02)==0) {              /* I = bit */
    mi->haveTimestamp=ORTE_TRUE;

    /* time in seconds */
    CDR_get_long(cdrCodec,&mi->timestamp.seconds);

    /* time in seconds / 2^32 */
    CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&mi->timestamp.fraction);
  } else {
    mi->haveTimestamp=ORTE_FALSE;
    NTPTIME_ZERO(mi->timestamp);
  }
  if (mi->haveTimestamp)
    debug(45,3) ("recv: RTPSInfoTS, timestamp %s\n",
                  NtpTimeToStringUs(mi->timestamp, buff));
  else
    debug(45,3) ("recv: RTPSInfoTS\n");
}
