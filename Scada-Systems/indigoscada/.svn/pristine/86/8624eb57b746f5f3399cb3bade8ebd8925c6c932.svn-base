/*
 *  $Id: RTPSAck.c,v 0.0.0.1            2003/10/07 
 *
 *  DEBUG:  section 47                  RTPS message ACK
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
RTPSAckCreate(CDR_Codec *cdrCodec,
    SequenceNumber *seqNumber,
    ObjectId roid,ObjectId woid,Boolean f_bit) 
{
  SequenceNumber     sn_tmp;                    
  CDR_Endianness     data_endian;
  CORBA_octet	     flags;

  if (cdrCodec->buf_len<cdrCodec->wptr+28) return -1;

  /* submessage id */
  CDR_put_octet(cdrCodec,ACK);

  /* flags */
  flags=cdrCodec->data_endian;
  if (f_bit) flags|=2;
  CDR_put_octet(cdrCodec,flags);

  /* length */
  CDR_put_ushort(cdrCodec,24);

  /* next data are sent in big endianing */
  data_endian=cdrCodec->data_endian;
  cdrCodec->data_endian=FLAG_BIG_ENDIAN;

  /* readerObjectId */
  CDR_put_ulong(cdrCodec,roid);
  
  /* writerObjectId */
  CDR_put_ulong(cdrCodec,woid);

  cdrCodec->data_endian=data_endian;

  SeqNumberInc(sn_tmp,*seqNumber);

  /* SeqNumber */
  CDR_put_ulong(cdrCodec,sn_tmp.high);
  CDR_put_ulong(cdrCodec,sn_tmp.low);

  /* bitmap - bits */
  CDR_put_ulong(cdrCodec,32);
  CDR_put_ulong(cdrCodec,0);

  return 28;
} 

/**********************************************************************************/
void 
RTPSAck(ORTEDomain *d,CDR_Codec *cdrCodec,MessageInterpret *mi,IPAddress senderIPAddress) {
  GUID_RTPS          readerGUID;
  CSTWriter          *cstWriter=NULL;
  CSTRemoteReader    *cstRemoteReader;
  CSChangeForReader  *csChangeForReader;
  StateMachineSend   stateMachineSendNew;
  ObjectId	     roid,woid;
  SequenceNumber     sn,isn;   
  char               queue=1;
  CDR_Endianness     data_endian;
  CORBA_octet	     flags;
  char	             f_bit;

  /* restore flag possition in submessage */
  cdrCodec->rptr-=3;

  /* flags */
  CDR_get_octet(cdrCodec, (CORBA_octet *)&flags);
  f_bit=flags & 2;

  /* move reading possition to begin of submessage */
  cdrCodec->rptr+=2;

  /* next data are sent in big endianing */
  data_endian=cdrCodec->data_endian;
  cdrCodec->data_endian=FLAG_BIG_ENDIAN;

  /* readerObjectId */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&roid);
  
  /* writerObjectId */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&woid);

  cdrCodec->data_endian=data_endian;

  /* SeqNumber */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&sn.high);
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&sn.low);

  readerGUID.hid=mi->sourceHostId;
  readerGUID.aid=mi->sourceAppId;
  readerGUID.oid=roid;

  debug(47,3) ("recv: RTPS ACK%c(0x%x) from 0x%x-0x%x\n",
                f_bit ? 'F':'f',
                woid,mi->sourceHostId,mi->sourceAppId);
  
  /* Manager */
  if ((d->guid.aid & 0x03)==MANAGER) {
    switch (woid) {
      case OID_WRITE_APPSELF:
        pthread_rwlock_wrlock(&d->writerApplicationSelf.lock);
        cstWriter=&d->writerApplicationSelf;
        readerGUID.hid=senderIPAddress;
        readerGUID.aid=AID_UNKNOWN;
        readerGUID.oid=roid;
        eventDetach(d,
    	    cstWriter->objectEntryOID->objectEntryAID,
    	    &cstWriter->registrationTimer,
    	    0);   //common timer
        break;
      case OID_WRITE_MGR:
        pthread_rwlock_wrlock(&d->writerManagers.lock);
        cstWriter=&d->writerManagers;
        break;
      case OID_WRITE_APP:
        pthread_rwlock_wrlock(&d->writerApplications.lock);
        cstWriter=&d->writerApplications;
        break;
    }
  }

  /* Application */
  if ((d->guid.aid & 0x03)==MANAGEDAPPLICATION) {
    switch (roid) {
      case OID_READ_APP:
      case OID_READ_APPSELF:
        pthread_rwlock_wrlock(&d->writerApplicationSelf.lock);
        cstWriter=&d->writerApplicationSelf;
        eventDetach(d,
    	    cstWriter->objectEntryOID->objectEntryAID,
    	    &cstWriter->registrationTimer,
    	    0);   //common timer
        break;
      case OID_READ_PUBL:
        pthread_rwlock_wrlock(&d->writerPublications.lock);
        cstWriter=&d->writerPublications;
        break;
      case OID_READ_SUBS:
        pthread_rwlock_wrlock(&d->writerSubscriptions.lock);
        cstWriter=&d->writerSubscriptions;
        break;
      default:
        if ((woid & 0x07) == OID_PUBLICATION) {
          GUID_RTPS  guid=d->guid;
          guid.oid=woid;
          pthread_rwlock_rdlock(&d->publications.lock);
          cstWriter=CSTWriter_find(&d->publications,&guid);
          pthread_rwlock_wrlock(&cstWriter->lock);
          queue=2;
        }
        break;
    }
  }

  if (!cstWriter) {
    if ((woid & 0x07) == OID_PUBLICATION) 
      pthread_rwlock_unlock(&d->publications.lock);
    return;
  }
  cstRemoteReader=CSTRemoteReader_find(cstWriter,&readerGUID);
  if (!cstRemoteReader) {
    pthread_rwlock_unlock(&cstWriter->lock);
    if ((woid & 0x07) == OID_PUBLICATION) 
      pthread_rwlock_unlock(&d->publications.lock);
    return;
  }

  stateMachineSendNew=NOTHNIGTOSEND;
  csChangeForReader=CSChangeForReader_first(cstRemoteReader);
  while(csChangeForReader) {
    isn=csChangeForReader->csChange->sn;
    if (SeqNumberCmp(csChangeForReader->csChange->gapSN,noneSN)>0) {
      SeqNumberAdd(isn,csChangeForReader->csChange->sn,csChangeForReader->csChange->gapSN);
      SeqNumberDec(isn,isn);
    }
    if (SeqNumberCmp(isn,sn)<0)   {   //ACK
      if (csChangeForReader->commStateChFReader!=ACKNOWLEDGED) {
        CSChangeForReader *csChangeForReaderDestroyed;
        csChangeForReaderDestroyed=csChangeForReader;
        csChangeForReader->commStateChFReader=ACKNOWLEDGED;
        csChangeForReader=
          CSChangeForReader_next(cstRemoteReader,csChangeForReader);
        if ((woid & 0x07) == OID_PUBLICATION) {
          CSTWriterDestroyCSChangeForReader(
            csChangeForReaderDestroyed,ORTE_TRUE);
        }
      } else {
        csChangeForReader=
          CSChangeForReader_next(cstRemoteReader,csChangeForReader);
      }
    } else {                                                      //NACK
      if (csChangeForReader->commStateChFReader!=TOSEND) {
        csChangeForReader->commStateChFReader=TOSEND;
        cstRemoteReader->commStateToSentCounter++;
      }
      stateMachineSendNew=MUSTSENDDATA;
      csChangeForReader=
        CSChangeForReader_next(cstRemoteReader,csChangeForReader);
    }
  }

  if ((cstRemoteReader->commStateSend==NOTHNIGTOSEND) && 
      (stateMachineSendNew==MUSTSENDDATA)) {
    cstRemoteReader->commStateSend=stateMachineSendNew;
    eventDetach(d,
        cstRemoteReader->sobject->objectEntryAID,
        &cstRemoteReader->delayResponceTimer,
        queue);
    if (queue==1) {
      eventAdd(d,
          cstRemoteReader->sobject->objectEntryAID,
          &cstRemoteReader->delayResponceTimer,
          queue,   //metatraffic timer
          "CSTWriterSendTimer",
          CSTWriterSendTimer,
          &cstRemoteReader->cstWriter->lock,
          cstRemoteReader,
          &cstRemoteReader->cstWriter->params.delayResponceTime);               
    } else {
      eventAdd(d,
          cstRemoteReader->sobject->objectEntryAID,
          &cstRemoteReader->delayResponceTimer,
          queue,   //userdata timer
          "CSTWriterSendStrictTimer",
          CSTWriterSendStrictTimer,
          &cstRemoteReader->cstWriter->lock,
          cstRemoteReader,
          &cstRemoteReader->cstWriter->params.delayResponceTime);               
    }
  } 

  if (stateMachineSendNew==NOTHNIGTOSEND) {
    cstRemoteReader->commStateSend=NOTHNIGTOSEND;
    if (queue==1) {
      eventDetach(d,
          cstRemoteReader->sobject->objectEntryAID,
          &cstRemoteReader->delayResponceTimer,
          queue);
    } else {
      eventDetach(d,
          cstRemoteReader->sobject->objectEntryAID,
          &cstRemoteReader->repeatAnnounceTimer,
          queue);
    }
  }  

  if ((!f_bit) && (cstRemoteReader->commStateSend==NOTHNIGTOSEND)) {
    cstRemoteReader->commStateHB=MUSTSENDHB;
    eventDetach(d,
        cstRemoteReader->sobject->objectEntryAID,
        &cstRemoteReader->delayResponceTimer,
        queue);
    if (queue==1) {
      eventAdd(d,
          cstRemoteReader->sobject->objectEntryAID,
          &cstRemoteReader->delayResponceTimer,
          queue,   //metatraffic timer
          "CSTWriterSendTimer",
          CSTWriterSendTimer,
          &cstRemoteReader->cstWriter->lock,
          cstRemoteReader,
          &cstRemoteReader->cstWriter->params.delayResponceTime);               
    } else {
      eventAdd(d,
          cstRemoteReader->sobject->objectEntryAID,
          &cstRemoteReader->delayResponceTimer,
          queue,   //userdata timer
          "CSTWriterSendStrictTimer",
          CSTWriterSendStrictTimer,
          &cstRemoteReader->cstWriter->lock,
          cstRemoteReader,
          &cstRemoteReader->cstWriter->params.delayResponceTime);               
    }
  } 

  pthread_rwlock_unlock(&cstWriter->lock);
  if ((woid & 0x07) == OID_PUBLICATION) 
    pthread_rwlock_unlock(&d->publications.lock);
} 


