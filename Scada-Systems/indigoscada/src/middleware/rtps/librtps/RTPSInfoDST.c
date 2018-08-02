/*
 *  $Id: RTPSInfoDST.c,v 0.0.0.1        2003/08/21
 *
 *  DEBUG:  section 42                  message INFO DST
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
void RTPSInfoDST(CDR_Codec *cdrCodec,MessageInterpret *mi) {
  CDR_Endianness     data_endian;
  HostId             hid;
  AppId              aid;

  data_endian=cdrCodec->data_endian;
  cdrCodec->data_endian=FLAG_BIG_ENDIAN;

  /* Host Id */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&hid);

  /* App Id */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&aid);

  cdrCodec->data_endian=data_endian;

  debug(42,3) ("  RTPSInfoDST:\n");
  debug(42,4) ("    hid:0x%x, aid:0x%x\n",hid,aid);

  if (hid!=HID_UNKNOWN) {
    mi->destHostId=hid;
  }
  if (aid!=AID_UNKNOWN) {
    mi->destAppId=aid;
  }
}


