/*
 *  $Id: RTPSIssue.c,v 0.0.0.1          2003/12/08
 *
 *  DEBUG:  section 56                  message ISSUE
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
RTPSIssueCreateHeader(CDR_Codec *cdrCodec,uint32_t length,
    ObjectId roid,ObjectId woid,SequenceNumber sn) {
  CDR_Endianness     data_endian;
  CORBA_octet        flags;
  
  if (cdrCodec->buf_len<cdrCodec->wptr+20) return -1;

  /* submessage id */
  CDR_put_octet(cdrCodec,ISSUE);

  /* flags */
  flags=cdrCodec->data_endian;
  CDR_put_octet(cdrCodec,flags);

  /* length */
  CDR_put_ushort(cdrCodec,(CORBA_unsigned_short)length);

  data_endian=cdrCodec->data_endian;
  cdrCodec->data_endian=FLAG_BIG_ENDIAN;

  /* readerObjectId */
  CDR_put_ulong(cdrCodec,roid);

  /* writerObjectId */
  CDR_put_ulong(cdrCodec,woid);

  cdrCodec->data_endian=data_endian;
  
  CDR_put_ulong(cdrCodec,sn.high);
  CDR_put_ulong(cdrCodec,sn.low);
  return 0;
}

/**********************************************************************************/
void 
RTPSIssue(ORTEDomain *d,CDR_Codec *cdrCodec,MessageInterpret *mi,IPAddress senderIPAddress) {
  GUID_RTPS          guid,writerGUID;
  ObjectId           roid,woid;
  SequenceNumber     sn,sn_tmp; 
  CORBA_octet        flags;  
  CORBA_unsigned_short submsg_len;
  CSTReader          *cstReader;
  CSTRemoteWriter    *cstRemoteWriter;
  CSChange           *csChange=NULL;
  CDR_Endianness     data_endian;

  /* restore flag possition in submessage */
  cdrCodec->rptr-=3;

  /* flags */
  CDR_get_octet(cdrCodec, (CORBA_octet *)&flags);

  /* submessage length */
  CDR_get_ushort(cdrCodec,&submsg_len);

  /* next data are sent in big endianing */
  data_endian=cdrCodec->data_endian;
  cdrCodec->data_endian=FLAG_BIG_ENDIAN;

  /* readerObjectId */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&roid);

  /* writerObjectId */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&woid);

  cdrCodec->data_endian=data_endian;

  /* sn */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&sn.high);
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&sn.low);

  /* at this moment is not supported p_bit */
  if (flags & 0x02) return;                     /* p_bit */

  writerGUID.hid=mi->sourceHostId;
  writerGUID.aid=mi->sourceAppId;
  writerGUID.oid=woid;
  
  debug(56,3) ("recv: RTPS_ISSUE(0x%x) from 0x%x-0x%x\n",
                woid,mi->sourceHostId,mi->sourceAppId);

  pthread_rwlock_rdlock(&d->subscriptions.lock);
  guid=d->guid;
  guid.oid=roid;

  gavl_cust_for_each(CSTReader,&d->subscriptions,cstReader) {
    if (roid!=OID_UNKNOWN)
      cstReader=CSTReader_find(&d->subscriptions,&guid);
    if (cstReader) {
      ORTESubsProp *sp;
      pthread_rwlock_wrlock(&cstReader->lock);
      sp=(ORTESubsProp*)cstReader->objectEntryOID->attributes;
      cstRemoteWriter=CSTRemoteWriter_find(cstReader,&writerGUID);
      if (cstRemoteWriter) {
        ORTEPublProp *pp,*pps;
        pp=(ORTEPublProp*)cstRemoteWriter->spobject->attributes;
        if (cstReader->cstRemoteWriterSubscribed!=NULL) {
          pps=(ORTEPublProp*)cstReader->cstRemoteWriterSubscribed->
                            spobject->attributes;
          if ((pp->strength>pps->strength) || (NtpTimeCmp(pps->persistence,zNtpTime)==0)) {
            cstReader->cstRemoteWriterSubscribed=cstRemoteWriter;
          }
        } else {
          cstReader->cstRemoteWriterSubscribed=cstRemoteWriter;
        }
        if (cstReader->cstRemoteWriterSubscribed==cstRemoteWriter) {
          eventDetach(d,
              cstReader->objectEntryOID->objectEntryAID,
              &cstReader->persistenceTimer,
              0);   //common timer
          eventAdd(d,
              cstReader->objectEntryOID->objectEntryAID,
              &cstReader->persistenceTimer,
              0,   //common timer
              "CSTReaderPersistenceTimer",
              CSTReaderPersistenceTimer,
              &cstReader->lock,
              cstReader,
              &pp->persistence);
        }

        if ((SeqNumberCmp(sn,cstRemoteWriter->sn)>0) &&   //have to be sn>writer_sn
            (CSChangeFromWriter_find(cstRemoteWriter,&sn)==NULL)) {

          csChange=(CSChange*)MALLOC(sizeof(CSChange));
          csChange->guid=writerGUID;
          csChange->sn=sn;
          SEQUENCE_NUMBER_NONE(csChange->gapSN);
          CSChangeAttributes_init_head(csChange);

          CDR_codec_init_static(&csChange->cdrCodec);
          CDR_buffer_init(&csChange->cdrCodec,
	  	          submsg_len-16);
	  csChange->cdrCodec.data_endian=cdrCodec->data_endian;

          memcpy(csChange->cdrCodec.buffer,
		 &cdrCodec->buffer[cdrCodec->rptr],submsg_len-16);

          if (SeqNumberCmp(sn,cstRemoteWriter->firstSN)>=0) { //sn>=firstSN
            if ((sp->reliabilityRequested & PID_VALUE_RELIABILITY_STRICT)!=0) {
              if (sp->recvQueueSize>cstRemoteWriter->csChangesCounter) {
                sn_tmp.high=0;
                sn_tmp.low=sp->recvQueueSize;
                SeqNumberAdd(sn_tmp,
                             cstRemoteWriter->sn,
                             sn_tmp);
                if (SeqNumberCmp(sn,sn_tmp)<=0) {         //sn<=(firstSN+QueueSize)
                  csChange->remoteTimePublished=mi->timestamp;
                  csChange->localTimeReceived=getActualNtpTime();
                  CSTReaderAddCSChange(cstRemoteWriter,csChange);
                  csChange=NULL;
                }
              }
            } else {
              if ((sp->reliabilityRequested & PID_VALUE_RELIABILITY_BEST_EFFORTS)!=0) {
                if ((sp->recvQueueSize<=cstRemoteWriter->csChangesCounter) ||
                    (cstReader->cstRemoteWriterSubscribed!=cstRemoteWriter)) {
                  CSChangeFromWriter *csChangeFromWriter;
                  csChangeFromWriter=CSChangeFromWriter_first(cstRemoteWriter);
                  CSTReaderDestroyCSChangeFromWriter(cstRemoteWriter,
                        csChangeFromWriter,
                        ORTE_FALSE);
                }
                if (sp->recvQueueSize>cstRemoteWriter->csChangesCounter) {
                  csChange->remoteTimePublished=mi->timestamp;
                  csChange->localTimeReceived=getActualNtpTime();
                  CSTReaderAddCSChange(cstRemoteWriter,csChange);
                  csChange=NULL;
                }
              }
            }
          } 
        }
        if (csChange) {
          FREE(csChange->cdrCodec.buffer);
          FREE(csChange);
        }
        CSTReaderProcCSChangesIssue(cstRemoteWriter,ORTE_FALSE);
      }
      pthread_rwlock_unlock(&cstReader->lock);  
    } else
      break;  //break traceing all cstReaders
  }
  pthread_rwlock_unlock(&d->subscriptions.lock);
}
