/*
 *  $Id: ORTEThreadUtils.c,v 0.0.0.1    2003/08/21 
 *
 *  DEBUG:  section 25                   Thread utility
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
void
ORTEDomainWakeUpReceivingThread(ORTEDomain *d,sock_t *sock,uint16_t port) {
  struct sockaddr_in    des;
  char                  sIPAddress[MAX_STRING_IPADDRESS_LENGTH];
  int                   i;

  des.sin_family = AF_INET;
  des.sin_port = htons(port);             //to receiving port
  if (d->domainProp.IFCount) {
    for(i=0;i<d->domainProp.IFCount;i++) {
      des.sin_addr.s_addr=htonl(d->domainProp.IFProp[i].ipAddress);
      sock_sendto(sock,&i,1,&des,sizeof(des));
      debug(25,2) ("Sent wake up signal to: %s.%d\n",
                    IPAddressToString(ntohl(des.sin_addr.s_addr),sIPAddress),
      port);
    }
  } else {
    des.sin_addr.s_addr = inet_addr("127.0.0.1"); //local IPAddress
    sock_sendto(sock,&i,1,&des,sizeof(des));
    debug(25,2) ("Sent wake up signal to: %s.%d\n",
                  IPAddressToString(ntohl(des.sin_addr.s_addr),sIPAddress),
                  port);
  }
}

/*****************************************************************************/
void
ORTEDomainWakeUpSendingThread(ObjectEntry *objectEntry) {
  debug(25,10) ("WakeUpSendingThread : start\n");
  if (objectEntry->htimNeedWakeUp) {
    pthread_mutex_lock(&objectEntry->htimSendMutex);
    if (objectEntry->htimSendCondValue==0) {
      debug(25,8) ("WakeUpSendingThread : send wakeup signal\n");
      pthread_cond_signal(&objectEntry->htimSendCond);
      objectEntry->htimSendCondValue=1;
    }
    pthread_mutex_unlock(&objectEntry->htimSendMutex);
  }
}



