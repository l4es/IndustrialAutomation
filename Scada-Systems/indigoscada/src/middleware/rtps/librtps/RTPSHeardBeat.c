/*
 *  $Id: RTPSHeardBeat.c,v 0.0.0.1      2003/10/07 
 *
 *  DEBUG:  section 48                  RTPS message HeardBeat
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
RTPSHeartBeatCreate(CDR_Codec *cdrCodec,
    SequenceNumber *fsn,SequenceNumber *lsn,
    ObjectId roid,ObjectId woid,Boolean f_bit) 
{
  CDR_Endianness     data_endian;
  CORBA_octet	     flags;

  if (cdrCodec->buf_len<cdrCodec->wptr+28) return -1;

  /* submessage id */
  CDR_put_octet(cdrCodec,HEARTBEAT);

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

  /* firstSeqNumber */
  CDR_put_ulong(cdrCodec,fsn->high);
  CDR_put_ulong(cdrCodec,fsn->low);

  /* lastSeqNumber */
  CDR_put_ulong(cdrCodec,lsn->high);
  CDR_put_ulong(cdrCodec,lsn->low);

  return 28;
}

/**********************************************************************************/
void 
HeartBeatProc(CSTReader *cstReader,GUID_RTPS *writerGUID,
    SequenceNumber *fsn,SequenceNumber *lsn,char f_bit) {
  CSTRemoteWriter    *cstRemoteWriter;
  
  if (!cstReader) return;
  cstRemoteWriter=CSTRemoteWriter_find(cstReader,writerGUID);
  if (!cstRemoteWriter) return;

  cstRemoteWriter->firstSN=*fsn;
  cstRemoteWriter->lastSN=*lsn;
  cstRemoteWriter->ACKRetriesCounter=0;

  if (SeqNumberCmp(cstRemoteWriter->sn,*lsn)>0)
    cstRemoteWriter->sn=*lsn;
  if (SeqNumberCmp(cstRemoteWriter->sn,*fsn)<0) {
    if (SeqNumberCmp(*fsn,noneSN)!=0) {
      SeqNumberDec(cstRemoteWriter->sn,*fsn);
    }
  }

  if ((writerGUID->oid & 0x07) == OID_PUBLICATION) {
    CSTReaderProcCSChangesIssue(cstRemoteWriter,ORTE_FALSE);
  } else {
    CSTReaderProcCSChanges(cstReader->domain,cstRemoteWriter);
  }

  if ((!f_bit) && (cstRemoteWriter->commStateACK==WAITING)) {
    char queue=1;
    cstRemoteWriter->commStateACK=ACKPENDING;
    if ((cstRemoteWriter->guid.oid & 0x07) == OID_PUBLICATION) 
      queue=2;
    eventDetach(cstReader->domain,
        cstRemoteWriter->spobject->objectEntryAID,
        &cstRemoteWriter->repeatActiveQueryTimer,
        queue); 
    eventDetach(cstReader->domain,
        cstRemoteWriter->spobject->objectEntryAID,
        &cstRemoteWriter->delayResponceTimer,
        queue);   //metatraffic timer
    eventAdd(cstReader->domain,
        cstRemoteWriter->spobject->objectEntryAID,
        &cstRemoteWriter->delayResponceTimer,
        queue,    //metatraffic timer
        "CSTReaderResponceTimer",
        CSTReaderResponceTimer,
        &cstRemoteWriter->cstReader->lock,
        cstRemoteWriter,
        &cstRemoteWriter->cstReader->params.delayResponceTimeMin);
  }
}

/**********************************************************************************/
void 
RTPSHeartBeat(ORTEDomain *d,CDR_Codec *cdrCodec,MessageInterpret *mi) {
  GUID_RTPS          writerGUID;
  ObjectId	     roid,woid;
  SequenceNumber     fsn,lsn;
  CSTReader          *cstReader=NULL;
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

  /* firstSeqNumber */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&fsn.high);
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&fsn.low);

  /* lastSeqNumber */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&lsn.high);
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&lsn.low);

  if (SeqNumberCmp(fsn,lsn)==1) return;	        // lsn<fsn -> break
  writerGUID.hid=mi->sourceHostId;
  writerGUID.aid=mi->sourceAppId;
  writerGUID.oid=woid;

  debug(48,3) ("recv: RTPS HB%c(0x%x) from 0x%x-0x%x\n",
                f_bit ? 'F':'f',woid,mi->sourceHostId,mi->sourceAppId);

  if ((d->guid.aid & 0x03)==MANAGER) {
    if ((writerGUID.oid==OID_WRITE_APPSELF) && 
        ((writerGUID.aid & 0x03)==MANAGER)) {
      pthread_rwlock_wrlock(&d->readerManagers.lock);
      cstReader=&d->readerManagers;
    }
    if (((writerGUID.oid==OID_WRITE_APPSELF) &&
         ((writerGUID.aid & 0x03)==MANAGEDAPPLICATION)) ||
        ((writerGUID.oid==OID_WRITE_APP) &&
         ((writerGUID.aid & 0x03)==MANAGER))) {
      pthread_rwlock_wrlock(&d->readerApplications.lock);
      cstReader=&d->readerApplications;
    }
  }

  if ((d->guid.aid & 3)==MANAGEDAPPLICATION) {
    switch (writerGUID.oid) {
      case OID_WRITE_MGR:
        pthread_rwlock_wrlock(&d->readerManagers.lock);
        cstReader=&d->readerManagers;
        break;
      case OID_WRITE_APP:
        pthread_rwlock_wrlock(&d->readerApplications.lock);
        cstReader=&d->readerApplications;
        break;
      case OID_WRITE_PUBL:
        pthread_rwlock_wrlock(&d->readerPublications.lock);
        cstReader=&d->readerPublications;
        break;
      case OID_WRITE_SUBS:
        pthread_rwlock_wrlock(&d->readerSubscriptions.lock);
        cstReader=&d->readerSubscriptions;
        break;
    }

    if ((writerGUID.oid & 0x07) == OID_PUBLICATION) {
      pthread_rwlock_rdlock(&d->subscriptions.lock);
      gavl_cust_for_each(CSTReader,&d->subscriptions,cstReader) {
        pthread_rwlock_wrlock(&cstReader->lock);
        HeartBeatProc(cstReader,&writerGUID,&fsn,&lsn,f_bit);
        pthread_rwlock_unlock(&cstReader->lock);    
      }
      pthread_rwlock_unlock(&d->subscriptions.lock);
      cstReader=NULL;
    }
  }  

  HeartBeatProc(cstReader,&writerGUID,&fsn,&lsn,f_bit);

  if (cstReader)
    pthread_rwlock_unlock(&cstReader->lock);
} 
