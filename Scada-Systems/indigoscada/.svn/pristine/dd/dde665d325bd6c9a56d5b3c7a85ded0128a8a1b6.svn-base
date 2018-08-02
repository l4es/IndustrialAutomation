/*
 *  $Id: ORTEAppRecvThread.c,v 0.0.0.1  2005/01/3 
 *
 *  DEBUG:  section 22                  Receiving thread
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

/* ThreadKind to String */
char* TK2S(TaskProp *tp) 
{
  ORTEDomain		*d=tp->d;

  if (tp==&d->taskRecvUnicastMetatraffic) 
    return "UM";

  if (tp==&d->taskRecvMulticastMetatraffic) 
    return "MM";

  if (tp==&d->taskRecvUnicastUserdata) 
    return "UU";

  if (tp==&d->taskRecvMulticastUserdata) 
    return "MU";

  return "";
}

/*****************************************************************************/
void ORTEAppRecvThread(TaskProp *tp) {
  struct sockaddr_in    des;
  uint32_t		RTPS_Codec_len;
  MessageInterpret      mi; 
  CDR_Codec		*cdrCodec=&tp->mb.cdrCodec;
  ORTEDomain		*d=tp->d;
  unsigned int		rptr;

  debug(22,10) ("ORTEAppRecvThread %s: start\n",TK2S(tp));

  while (!tp->terminate) {
    debug(22,7) ("ORTEAppRecvThread %s: receiving\n",TK2S(tp));
    
    //prepare cdrCodec
    cdrCodec->wptr=cdrCodec->rptr=0;
    RTPS_Codec_len = sock_recvfrom(
                  &tp->sock,  		         //socked handle
                  cdrCodec->buffer,             //buffer
                  cdrCodec->buf_len,            //max length of message
                  &des,sizeof(des));             //info from sending host

    debug(22,7) ("ORTEAppRecvThread %s: fired, msg_len: 0x%x\n",TK2S(tp),RTPS_Codec_len);

    //is it header of valid RTPS packet?
    if (RTPSHeaderCheck(cdrCodec,RTPS_Codec_len,&mi)<0) {
      debug(22,2) ("ORTEAppRecvThread: not valid RTPS header!\n");
      continue;
    } 

    debug(22,7) ("ORTEAppRecvThread: RTPS Heard OK\n");
    debug(22,7) ("  PV: %d.%d VID:%d.%d HID:0x%x AID:0x%x\n",
                    mi.sourceVersion.major,mi.sourceVersion.minor,
    		    mi.sourceVendorId.major,mi.sourceVendorId.minor,
    		    mi.sourceHostId,mi.sourceAppId);
    do {
      // check if length of submessage header is OK
      if ((cdrCodec->rptr+3)<=RTPS_Codec_len) {
        CORBA_char flags,sub_id;
	CORBA_unsigned_short sub_len;

	CDR_get_octet(cdrCodec, (CORBA_octet *)&sub_id);
	CDR_get_octet(cdrCodec, (CORBA_octet *)&flags);
	if (flags & 0x01) 	  
           cdrCodec->data_endian=FLAG_LITTLE_ENDIAN;
        else
           cdrCodec->data_endian=FLAG_BIG_ENDIAN;
	CDR_get_ushort(cdrCodec,&sub_len);

        debug(22,7) ("ORTEAppRecvThread %s: sub_id: 0x%x, sub_len 0x%x: %u\n",
                      TK2S(tp),sub_id,sub_len);

	// check if length of submessage OK
	if ((sub_len+cdrCodec->rptr)<=RTPS_Codec_len) {
          pthread_rwlock_wrlock(&d->objectEntry.objRootLock);    
          pthread_rwlock_wrlock(&d->objectEntry.htimRootLock);
	  rptr=cdrCodec->rptr;
          switch ((SubmessageId)sub_id) {
            case VAR:
              RTPSVar(d,cdrCodec,&mi,ntohl(des.sin_addr.s_addr));
            break;
            case ACK:
              RTPSAck(d,cdrCodec,&mi,ntohl(des.sin_addr.s_addr));
            break;
            case HEARTBEAT:
              RTPSHeartBeat(d,cdrCodec,&mi);
            break;
            case GAP:
              RTPSGap(d,cdrCodec,&mi,ntohl(des.sin_addr.s_addr));
            break;
            case INFO_TS:
              RTPSInfoTS(cdrCodec,&mi);
            break;
            case INFO_SRC:
              RTPSInfoSRC(cdrCodec,&mi);
            break;
            case INFO_REPLY:
              RTPSInfoREPLY(cdrCodec,&mi);
            break;
            case INFO_DST:
              RTPSInfoDST(cdrCodec,&mi);
            break;
            case PAD:
              RTPSPad(cdrCodec,&mi);
            break;
            case ISSUE:
              RTPSIssue(d,cdrCodec,&mi,ntohl(des.sin_addr.s_addr));
            break;
            default:
              debug(22,5) ("ORTEAppRecvThread: unknown message :0x%x\n",sub_id);
            break;
          }  
	  cdrCodec->rptr=rptr+sub_len;
          pthread_rwlock_unlock(&d->objectEntry.htimRootLock);
          pthread_rwlock_unlock(&d->objectEntry.objRootLock);    
        } else break;          /* submessage is too big */
      } else break;            /* submessage is too big */
    } while (cdrCodec->rptr<RTPS_Codec_len);
    debug(22,7) ("ORTEAppRecvThread: processing of message(s) finnished\n");
  } 

  debug(22,10) ("ORTEAppRecvThread %s: finished\n",TK2S(tp));
  pthread_exit(NULL);
}
