/*
 *  $Id: RTPSCSTWriterTimer.c,v 0.0.0.1 2003/10/19 
 *
 *  DEBUG:  section 52                  CSTWriter timer functions
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

/*****************************************************************************/
int 
CSTWriterRegistrationTimer(ORTEDomain *d,void *vcstWriter) {
  CSTWriter *cstWriter=(CSTWriter*)vcstWriter;
  CSTRemoteReader *cstRemoteReader;

  debug(52,10) ("CSTWriterRegistrationTimer: start\n");

  debug(52,5) ("CSTWriterRegistrationTimer: OID: 0xx%x - retries = %d\n",
		cstWriter->guid.oid,cstWriter->registrationCounter);
  eventDetach(d,
      cstWriter->objectEntryOID->objectEntryAID,
      &cstWriter->registrationTimer,
      0);   //common timer

  if (cstWriter->registrationCounter!=0) {
    cstWriter->registrationCounter--;
    gavl_cust_for_each(CSTRemoteReader,cstWriter,cstRemoteReader) {
      CSTWriterRefreshAllCSChanges(d,cstRemoteReader);
    }
    eventAdd(d,
        cstWriter->objectEntryOID->objectEntryAID,
        &cstWriter->registrationTimer,
        0,   //common timer
        "CSTWriterRegistrationTimer",
        CSTWriterRegistrationTimer,
        &cstWriter->lock,
        cstWriter,
        &cstWriter->params.registrationPeriod);               
  } else {
    if (d->domainEvents.onRegFail) {
	d->domainEvents.onRegFail(d->domainEvents.onRegFailParam);
    }
  }

  debug(52,10) ("CSTWriterRegistrationTimer: finished\n");
  return 0;
}


/*****************************************************************************/
int 
CSTWriterRefreshTimer(ORTEDomain *d,void *vcstWriter) {
  CSTWriter *cstWriter=(CSTWriter*)vcstWriter;
  CSTRemoteReader *cstRemoteReader;
  
  debug(52,10) ("CSTWriterRefreshTimer: start\n");
  
  gavl_cust_for_each(CSTRemoteReader,cstWriter,cstRemoteReader) {
    CSTWriterRefreshAllCSChanges(d,cstRemoteReader);
  }
  eventDetach(d,
      cstWriter->objectEntryOID->objectEntryAID,
      &cstWriter->refreshPeriodTimer,
      0);   //common timer
  eventAdd(d,
      cstWriter->objectEntryOID->objectEntryAID,
      &cstWriter->refreshPeriodTimer,
      0,   //common timer
      "CSTWriterRefreshTimer",
      CSTWriterRefreshTimer,
      &cstWriter->lock,
      cstWriter,
      &cstWriter->params.refreshPeriod);               
  debug(52,10) ("CSTWriterRefreshTimer: finished\n");
  return 0;
}

/*****************************************************************************/
int 
CSTWriterAnnounceTimer(ORTEDomain *d,void *vcstRemoteReader) {
  CSTRemoteReader *cstRemoteReader=(CSTRemoteReader*)vcstRemoteReader;

  debug(52,10) ("CSTWriterAnnounceTimer: start\n");
  if ((cstRemoteReader->commStateHB==MAYSENDHB) &&
      ((!cstRemoteReader->cstWriter->params.fullAcknowledge))) {// ||
//       (cstRemoteReader->unacknowledgedCounter))) {
    //create HB
    int len=RTPSHeartBeatCreate(
        &d->taskSend.mb.cdrCodec,
        &cstRemoteReader->cstWriter->firstSN,
        &cstRemoteReader->cstWriter->lastSN,
        OID_UNKNOWN,
        cstRemoteReader->cstWriter->guid.oid,
        ORTE_FALSE);
    if (len<0) {
      //not enought space in sending buffer
      d->taskSend.mb.needSend=ORTE_TRUE;
      return 1;
    }
    debug(52,3) ("sent: RTPS_HBF(0x%x) to 0x%x-0x%x\n",
                  cstRemoteReader->cstWriter->guid.oid,
                  cstRemoteReader->guid.hid,
                  cstRemoteReader->guid.aid);
  }
  eventDetach(d,
      cstRemoteReader->sobject->objectEntryAID,
      &cstRemoteReader->repeatAnnounceTimer,
      1);
  eventAdd(d,
      cstRemoteReader->sobject->objectEntryAID,
      &cstRemoteReader->repeatAnnounceTimer,
      1,   //metatraffic timer
      "CSTWriterAnnounceTimer",
      CSTWriterAnnounceTimer,
      &cstRemoteReader->cstWriter->lock,
      cstRemoteReader,
      &cstRemoteReader->cstWriter->params.repeatAnnounceTime);
  debug(52,10) ("CSTWriterAnnounceTimer: finished\n");
  return 0;
}

/*****************************************************************************/
int 
CSTWriterAnnounceIssueTimer(ORTEDomain *d,void *vcstRemoteReader) {
  CSTRemoteReader *cstRemoteReader=(CSTRemoteReader*)vcstRemoteReader;
  NtpTime         nextHB;
  ORTEPublProp    *pp;
  int             len;

  debug(52,10) ("CSTWriterAnnounceIssueTimer: start\n");
  pp=(ORTEPublProp*)cstRemoteReader->cstWriter->objectEntryOID->attributes;
  //create HB
  d->taskSend.mb.cdrCodecDirect=NULL;
  len=RTPSHeartBeatCreate(
      &d->taskSend.mb.cdrCodec,
      &cstRemoteReader->cstWriter->firstSN,
      &cstRemoteReader->cstWriter->lastSN,
      OID_UNKNOWN,
      cstRemoteReader->cstWriter->guid.oid,
      ORTE_FALSE);
  if (len<0) {
    //not enought space in sending buffer
    d->taskSend.mb.needSend=ORTE_TRUE;
    return 1;
  }
  debug(52,3) ("sent: RTPS_HBF(0x%x) to 0x%x-0x%x\n",
                cstRemoteReader->cstWriter->guid.oid,
                cstRemoteReader->guid.hid,
                cstRemoteReader->guid.aid);
  //next HB
  if (cstRemoteReader->cstWriter->csChangesCounter>=pp->criticalQueueLevel) {
    nextHB=pp->HBCQLRate;
  } else {
    nextHB=pp->HBNornalRate;
  }
  cstRemoteReader->HBRetriesCounter++;
  eventDetach(d,
      cstRemoteReader->sobject->objectEntryAID,
      &cstRemoteReader->repeatAnnounceTimer,
      2);
  if (cstRemoteReader->HBRetriesCounter<pp->HBMaxRetries) {              
    eventAdd(d,
        cstRemoteReader->sobject->objectEntryAID,
        &cstRemoteReader->repeatAnnounceTimer,
        2,   //metatraffic timer
        "CSTWriterAnnounceIssueTimer",
        CSTWriterAnnounceIssueTimer,
        &cstRemoteReader->cstWriter->lock,
        cstRemoteReader,
        &nextHB);
  } else {
    //destroy all csChangesForReader
    CSChangeForReader *csChangeForReader;
    while ((csChangeForReader=CSChangeForReader_first(cstRemoteReader))) {
      CSTWriterDestroyCSChangeForReader(
          csChangeForReader,ORTE_TRUE);
    }
    debug(52,3) ("CSTWriterAnnounceIssueTimer: HB RR(0x%x-0x%x) ritch MaxRetries\n",
                  cstRemoteReader->guid.hid,cstRemoteReader->guid.aid);
  }
  debug(52,10) ("CSTWriterAnnounceIssueTimer: finished\n");
  return 0;
}

/**********************************************************************************/
int
CSChangeForReaderUnderwayTimer(ORTEDomain *d,void *vcsChangeForReader) {
  CSChangeForReader *csChangeForReader=(CSChangeForReader*)vcsChangeForReader;
  csChangeForReader->commStateChFReader=UNACKNOWLEDGED;
  return 0;
}

/**********************************************************************************/
int
CSTWriterSendBestEffortTimer(ORTEDomain *d,void *vcstRemoteReader) {
  CSTRemoteReader   *cstRemoteReader=(CSTRemoteReader*)vcstRemoteReader;
  ORTESubsProp      *sp=(ORTESubsProp*)cstRemoteReader->pobject->attributes;
  CSChangeForReader *csChangeForReader=NULL;
        
  debug(52,10) ("CSTWriterSendBestEffortTimer: start\n");
  d->taskSend.mb.cdrCodecDirect=NULL;
  if (cstRemoteReader->commStateSend!=NOTHNIGTOSEND) {
    gavl_cust_for_each(CSChangeForReader,cstRemoteReader,csChangeForReader) {
      if (csChangeForReader->commStateChFReader==TOSEND) {
        CSChange *csChange=csChangeForReader->csChange;

        csChangeForReader->commStateChFReader=UNDERWAY;
        cstRemoteReader->commStateSend=MUSTSENDDATA;
        cstRemoteReader->lastSentIssueTime=getActualNtpTime();
        d->taskSend.mb.cdrCodecDirect=&csChange->cdrCodec;

        if (cstRemoteReader->sobject) {
          debug(52,3) ("sent: RTPS_ISSUE_BEST(0x%x) to 0x%x-0x%x-0x%x\n",
                        cstRemoteReader->cstWriter->guid.oid,
                        GUID_PRINTF(cstRemoteReader->sobject->guid));
        }

        ORTESendData(d,
            cstRemoteReader->sobject->objectEntryAID,
            ORTE_FALSE);

	//it's not nessecary to NewState, there is setuped only new state & after is deleted
        CSTWriterCSChangeForReaderNewState(csChangeForReader);

	/* mark multicast messages like processed */
        CSTWriterMulticast(csChangeForReader);

        CSTWriterDestroyCSChangeForReader(
            csChangeForReader,ORTE_TRUE);

        eventDetach(d,
            cstRemoteReader->sobject->objectEntryAID,
            &cstRemoteReader->delayResponceTimer,
            2);   

        //when is no csChange -> break processing 
        if (cstRemoteReader->cstWriter->csChangesCounter==0) 
          break;

        eventAdd(d,
            cstRemoteReader->sobject->objectEntryAID,
            &cstRemoteReader->delayResponceTimer,
            2,   
            "CSTWriterSendBestEffortTimer",
            CSTWriterSendBestEffortTimer,
            &cstRemoteReader->cstWriter->lock,
            cstRemoteReader,
            &sp->minimumSeparation);
        return 0;

      }
    }
  }
  debug(52,10) ("CSTWriterSendBestEffortTimer: finished\n");
  return 0;
}

/**********************************************************************************/
int
CSTWriterSendStrictTimer(ORTEDomain *d,void *vcstRemoteReader) {
  CSTRemoteReader   *cstRemoteReader=(CSTRemoteReader*)vcstRemoteReader;
  CSChangeForReader *csChangeForReader=NULL;
  int               len,data_offset,wptr_max;
  CSChange          *csChange;
  Boolean           firstTrace=ORTE_TRUE;
  
  debug(52,10) ("CSTWriterSendStrictTimer: start\n");
  if (cstRemoteReader->commStateSend!=NOTHNIGTOSEND) {
    gavl_cust_for_each(CSChangeForReader,cstRemoteReader,csChangeForReader) {
      csChange=csChangeForReader->csChange;
      if (csChangeForReader->commStateChFReader==TOSEND) {
        cstRemoteReader->commStateSend=MUSTSENDDATA;

        wptr_max=d->taskSend.mb.cdrCodec.wptr_max;
        d->taskSend.mb.cdrCodec.wptr_max=csChange->cdrCodec.wptr_max;
    	/* infoReply */
        if ((firstTrace) && (cstRemoteReader->cstWriter->params.fullAcknowledge) &&
            !d->taskSend.mb.containsInfoReply) {
          AppParams *ap=cstRemoteReader->cstWriter->objectEntryOID->attributes;
          firstTrace=ORTE_FALSE;
          len=RTPSInfoREPLYCreate(&d->taskSend.mb.cdrCodec,
              IPADDRESS_INVALID,
              ap->userdataUnicastPort);
          if (len<0) {
            d->taskSend.mb.needSend=ORTE_TRUE;
     	    d->taskSend.mb.cdrCodec.wptr_max=wptr_max;
            return 1;
          }
          d->taskSend.mb.containsInfoReply=ORTE_TRUE;  
          debug(52,3) ("sent: RTPS_InfoREPLY(0x%x) to 0x%x-0x%x\n",
                       cstRemoteReader->cstWriter->guid.oid,
                       cstRemoteReader->guid.hid,
                       cstRemoteReader->guid.aid);
        }

	data_offset=RTPS_HEADER_LENGTH+12;
        if (CDR_buffer_puts(&d->taskSend.mb.cdrCodec,
			    csChange->cdrCodec.buffer+data_offset, //src
			    csChange->cdrCodec.wptr-data_offset)==CORBA_FALSE) {
            d->taskSend.mb.needSend=ORTE_TRUE;
     	    d->taskSend.mb.cdrCodec.wptr_max=wptr_max;
            return 1;
        }
    
        d->taskSend.mb.cdrCodec.wptr_max=wptr_max;

	/* setup new state for csChangeForReader */
        CSTWriterCSChangeForReaderNewState(csChangeForReader);

	/* mark multicast messages like processed */
        CSTWriterMulticast(csChangeForReader);

        debug(52,3) ("sent: RTPS_ISSUE_STRICT(0x%x) to 0x%x-0x%x\n",
                    cstRemoteReader->cstWriter->guid.oid,
                    cstRemoteReader->guid.hid,
                    cstRemoteReader->guid.aid);
      }
    }
  }
  debug(52,10) ("CSTWriterSendStrictTimer: finished\n");
  //add HeardBeat  
  return CSTWriterAnnounceIssueTimer(d,cstRemoteReader);
}

/**********************************************************************************/
int
CSTWriterSendTimer(ORTEDomain *d,void *vcstRemoteReader) {
  CSTRemoteReader   *cstRemoteReader=(CSTRemoteReader*)vcstRemoteReader;
  CSChangeForReader *csChangeForReader=NULL;
  Boolean           firstTrace=ORTE_TRUE,f_bit=ORTE_TRUE;
  
  debug(52,10) ("CSTWriterSendTimer: start\n");

  /* setup f_bit of object */
  if (cstRemoteReader->cstWriter->params.fullAcknowledge)
    f_bit=ORTE_FALSE;

  if (cstRemoteReader->commStateSend!=NOTHNIGTOSEND) {

    gavl_cust_for_each(CSChangeForReader,cstRemoteReader,csChangeForReader) {

      if (csChangeForReader->commStateChFReader==TOSEND) {
        cstRemoteReader->commStateSend=MUSTSENDDATA;

        /* infoReply */
        if ((firstTrace) && (cstRemoteReader->cstWriter->params.fullAcknowledge) &&
            !d->taskSend.mb.containsInfoReply) {
          AppParams *ap=cstRemoteReader->cstWriter->objectEntryOID->attributes;
          firstTrace=ORTE_FALSE;
          if (RTPSInfoREPLYCreate(&d->taskSend.mb.cdrCodec,
            			  IPADDRESS_INVALID,
            			  ap->metatrafficUnicastPort) < 0) {
            d->taskSend.mb.needSend=ORTE_TRUE;
            return 1;
          }
          d->taskSend.mb.containsInfoReply=ORTE_TRUE;  
          debug(52,3) ("sent: RTPS_InfoREPLY from 0x%x-0x%x-0x%x to 0x%x-0x%x-0x%x\n",
                        GUID_PRINTF(cstRemoteReader->cstWriter->guid),
                        GUID_PRINTF(cstRemoteReader->guid));
        }

        /* VAR */
        if (SeqNumberCmp(csChangeForReader->csChange->gapSN,noneSN)==0) {
          debug(52,3) ("sent: RTPS_VAR from 0x%x-0x%x-0x%x to 0x%x-0x%x-0x%x\n",
                        GUID_PRINTF(cstRemoteReader->cstWriter->guid),
                        GUID_PRINTF(cstRemoteReader->guid));

          if (RTPSVarCreate(&d->taskSend.mb.cdrCodec,
			    OID_UNKNOWN,
                            cstRemoteReader->cstWriter->guid.oid,
			    csChangeForReader->csChange) < 0) {
            d->taskSend.mb.needSend=ORTE_TRUE;
            return 1;
          }

        } else {
        /* GAP */
          debug(52,3) ("sent: RTPS_GAP from 0x%x-0x%x-0x%x to 0x%x-0x%x-0x%x\n",
                        GUID_PRINTF(cstRemoteReader->cstWriter->guid),
                        GUID_PRINTF(cstRemoteReader->guid));

          if (RTPSGapCreate(&d->taskSend.mb.cdrCodec,
			    OID_UNKNOWN,
			    cstRemoteReader->cstWriter->guid.oid,
			    csChangeForReader->csChange) < 0) {
            d->taskSend.mb.needSend=ORTE_TRUE;
            return 1;
          }
        }

	/* setup new state for csChangeForReader */
	CSTWriterCSChangeForReaderNewState(csChangeForReader);

	/* mark multicast messages like processed */
        CSTWriterMulticast(csChangeForReader);

      }
    } /* gavl_cust_for_each */

    cstRemoteReader->commStateHB=MUSTSENDHB;

  }

  if (cstRemoteReader->commStateHB==MUSTSENDHB) {
    //add HeartBeat
    if (RTPSHeartBeatCreate(
        &d->taskSend.mb.cdrCodec,
        &cstRemoteReader->cstWriter->firstSN,
        &cstRemoteReader->cstWriter->lastSN,
        OID_UNKNOWN,
        cstRemoteReader->cstWriter->guid.oid,
        f_bit)<0) {
      d->taskSend.mb.needSend=ORTE_TRUE;
      return 1;
    } else {
      //schedule new time for Announce timer
      eventDetach(d,
           cstRemoteReader->sobject->objectEntryAID,
           &cstRemoteReader->repeatAnnounceTimer,
           1);
      eventAdd(d,
           cstRemoteReader->sobject->objectEntryAID,
           &cstRemoteReader->repeatAnnounceTimer,
           1,   //metatraffic timer
           "CSTWriterAnnounceTimer",
           CSTWriterAnnounceTimer,
           &cstRemoteReader->cstWriter->lock,
           cstRemoteReader,
           &cstRemoteReader->cstWriter->params.repeatAnnounceTime);
    }

    debug(52,3) ("sent: RTPS_HB from 0x%x-0x%x-0x%x to 0x%x-0x%x-0x%x\n",
                  GUID_PRINTF(cstRemoteReader->cstWriter->guid),
                  GUID_PRINTF(cstRemoteReader->guid));

    cstRemoteReader->commStateHB=MAYSENDHB;
  }
 
  debug(52,10) ("CSTWriterSendTimer: finished\n");
  return 0;
}
