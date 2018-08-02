/*
 *  $Id: RTPSGap.c,v 0.0.0.1            2003/10/07 
 *
 *  DEBUG:  section 49                  RTPS message GAP
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
RTPSGapCreate(CDR_Codec *cdrCodec,ObjectId roid,ObjectId woid,CSChange *csChange) 
{
  CDR_Endianness     data_endian;
  SequenceNumber     bsn;
  CORBA_octet        flags;

  if (cdrCodec->buf_len<cdrCodec->wptr+32) return -1;

  /* submessage id */
  CDR_put_octet(cdrCodec,GAP);

  /* flags */
  flags=cdrCodec->data_endian;
  CDR_put_octet(cdrCodec,flags);

  /* length */
  CDR_put_ushort(cdrCodec,28);

  /* next data are sent in big endianing */
  data_endian=cdrCodec->data_endian;
  cdrCodec->data_endian=FLAG_BIG_ENDIAN;

  /* readerObjectId */
  CDR_put_ulong(cdrCodec,roid);
  
  /* writerObjectId */
  CDR_put_ulong(cdrCodec,woid);

  cdrCodec->data_endian=data_endian;

  /* firstSeqNumber */
  CDR_put_ulong(cdrCodec,csChange->sn.high);
  CDR_put_ulong(cdrCodec,csChange->sn.low);

  /* bitmap sn */
  SeqNumberAdd(bsn,  
               csChange->sn,
               csChange->gapSN);
  CDR_put_ulong(cdrCodec,bsn.high);
  CDR_put_ulong(cdrCodec,bsn.low);

  /* numbits */
  CDR_put_ulong(cdrCodec,0);

  return 32;
}

/*****************************************************************************/
void 
RTPSGapAdd(CSTRemoteWriter *cstRemoteWriter,GUID_RTPS *guid,SequenceNumber *fsn,
    SequenceNumber *sn,uint32_t numbits,CDR_Codec *cdrCodec) {
  SequenceNumber      lsn,ssn;
  uint32_t            i;
  int8_t              bit,bit_last=0;
  CSChange            *csChange;
  uint32_t            bitmap;
  
  if (SeqNumberCmp(*sn,cstRemoteWriter->sn)<0) return;//have to be sn>=writer_sn ! 
  if (SeqNumberCmp(*fsn,*sn)==1) return;              //cannot be fsn>sn ! 
  if (numbits>256) return;

  //first case of GAP sn
  if (SeqNumberCmp(*fsn,*sn)<0) {                        //if fsn<sn  
    if (CSChangeFromWriter_find(cstRemoteWriter,fsn)) {
      CSTReaderDestroyCSChange(cstRemoteWriter, 
                               fsn,ORTE_FALSE);
    }
    csChange=(CSChange*)MALLOC(sizeof(CSChange));
    csChange->cdrCodec.buffer=NULL;
    csChange->sn=*fsn;
    csChange->guid=*guid;
    csChange->alive=ORTE_TRUE;
    SeqNumberSub(csChange->gapSN,*sn,*fsn);            //setup flag GAP
    CSChangeAttributes_init_head(csChange);
    CSTReaderAddCSChange(cstRemoteWriter,csChange);
  }

  //second case of GAP sn
  lsn=ssn=*sn;bit=0;
  for(i=0;i<numbits;i++) {
    if ((i%32)==0) 
      CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&bitmap);

    bit=(bitmap & (1<<(31-i%32))) ? 1:0;
    if (i>0) {
      if (bit_last && !bit) {                           //end of GAP     1->0
        if (!CSChangeFromWriter_find(cstRemoteWriter,&ssn)) {
          if (SeqNumberCmp(ssn,cstRemoteWriter->sn)>0) {  
            csChange=(CSChange*)MALLOC(sizeof(CSChange));
            csChange->cdrCodec.buffer=NULL;
            csChange->sn=ssn;
            csChange->guid=*guid;
            csChange->alive=ORTE_TRUE;
            SeqNumberSub(csChange->gapSN,lsn,ssn);         //setup flag GAP
            CSChangeAttributes_init_head(csChange);
            CSTReaderAddCSChange(cstRemoteWriter,csChange);
          }
        }
      } else {
        if (!bit_last && bit) {                         //begin GAP      0->1
          ssn=lsn;                                      //start pointer
        }
      }
    }  
    SeqNumberInc(lsn,lsn);
    bit_last=bit;
  }

  if (bit) {
    if (!CSChangeFromWriter_find(cstRemoteWriter,&ssn)) {
      if (SeqNumberCmp(ssn,cstRemoteWriter->sn)>0) {  
        csChange=(CSChange*)MALLOC(sizeof(CSChange));
        csChange->cdrCodec.buffer=NULL;
        csChange->sn=ssn;
        csChange->guid=*guid;
        csChange->alive=ORTE_TRUE;
        SeqNumberSub(csChange->gapSN,lsn,ssn);         //setup flag GAP
        CSChangeAttributes_init_head(csChange);
        CSTReaderAddCSChange(cstRemoteWriter,csChange);
      }
    }
  }
}

/**********************************************************************************/
void 
RTPSGap(ORTEDomain *d,CDR_Codec *cdrCodec,MessageInterpret *mi,IPAddress senderIPAddress) {
  CSTReader          *cstReader=NULL;
  CSTRemoteWriter    *cstRemoteWriter;
  GUID_RTPS          writerGUID;
  ObjectId	     roid,woid;
  SequenceNumber     sn,fsn;
  uint32_t           numbits;
  CDR_Endianness     data_endian;

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

  /* Bitmap - SN  */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&sn.high);
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&sn.low);

  /* numbits  */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&numbits);

  writerGUID.hid=mi->sourceHostId;
  writerGUID.aid=mi->sourceAppId;
  writerGUID.oid=woid;

  debug(49,3) ("recv: RTPS_GAP(0x%x) from 0x%x-0x%x fSN:%d, bSN:%d, numbits:%d\n",
                woid,mi->sourceHostId,mi->sourceAppId,fsn.low,sn.low,numbits);
  
  /* Manager */
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

  /* Application */
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
  }  

  if (!cstReader) return;
  cstRemoteWriter=CSTRemoteWriter_find(cstReader,&writerGUID);
  if (!cstRemoteWriter) {
    pthread_rwlock_unlock(&cstReader->lock);
    return;
  }

  RTPSGapAdd(cstRemoteWriter,&writerGUID,&fsn,&sn,numbits,
             cdrCodec);

  CSTReaderProcCSChanges(d,cstRemoteWriter);
  pthread_rwlock_unlock(&cstReader->lock);
} 


