/*
 *  $Id: ORTEDomainApp.c,v 0.0.0.1      2003/08/21 
 *
 *  DEBUG:  section 21                  Domain application
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
ORTEDomain * 
ORTEDomainAppCreate(int domain, ORTEDomainProp *prop,
                    ORTEDomainAppEvents *events,Boolean suspended) {
  ORTEDomain 		*d;

  debug(21,10) ("ORTEDomainAppCreate: start\n");

  d=ORTEDomainCreate(domain,prop,events,ORTE_FALSE);
  if (!d)
    return NULL;
  
  //Start threads
  if (!suspended) {
    ORTEDomainStart(d,					/* domain */
		    ORTE_TRUE,				/* recvUnicastMetarafficThread */
		    d->domainProp.multicast.enabled,    /* recvMulticastMetarafficThread */
		    ORTE_TRUE,				/* recvUnicastUserdataThread */
		    d->domainProp.multicast.enabled,	/* recvMulticastUserdataThread */
		    ORTE_TRUE);				/* sendThread */
  }
  debug(21,10) ("ORTEDomainAppCreate: finished\n");
  return d;
}

/*****************************************************************************/
Boolean
ORTEDomainAppDestroy(ORTEDomain *d) {
  Boolean ret;

  debug(21,10) ("ORTEDomainAppDestroy: start\n");

  ret=ORTEDomainDestroy(d,ORTE_FALSE);

  debug(21,10) ("ORTEDomainAppDestroy: finished\n");
  return ret;
}

/*****************************************************************************/
Boolean 
ORTEDomainAppSubscriptionPatternAdd(ORTEDomain *d,const char *topic,
    const char *type,ORTESubscriptionPatternCallBack subscriptionCallBack, 
    void *param) {
  PatternNode *pnode;
  
  if (!d) return ORTE_FALSE;
  pnode=(PatternNode*)MALLOC(sizeof(PatternNode));
  strcpy((char *)pnode->topic,topic);
  strcpy((char *)pnode->type,type);
  pnode->subscriptionCallBack=subscriptionCallBack;
  pnode->param=param;
  pthread_rwlock_wrlock(&d->patternEntry.lock);
  Pattern_insert(&d->patternEntry,pnode);
  pthread_rwlock_unlock(&d->patternEntry.lock);
  return ORTE_TRUE;
}

/*****************************************************************************/
Boolean 
ORTEDomainAppSubscriptionPatternRemove(ORTEDomain *d,const char *topic,
    const char *type) {
  PatternNode *pnode;
  
  if (!d) return ORTE_FALSE;
  pthread_rwlock_wrlock(&d->patternEntry.lock);
  ul_list_for_each(Pattern,&d->patternEntry,pnode) {
    if ((strcmp((const char *)pnode->topic, (const char*)topic)==0) &&
        (strcmp((const char *)pnode->type, (const char*)type)==0)) {
      Pattern_delete(&d->patternEntry,pnode);
      FREE(pnode);
      return ORTE_TRUE;
    }
  }
  pthread_rwlock_unlock(&d->patternEntry.lock);
  return ORTE_FALSE;
}

/*****************************************************************************/
Boolean 
ORTEDomainAppSubscriptionPatternDestroy(ORTEDomain *d) {
  PatternNode *pnode;
  
  if (!d) return ORTE_FALSE;
  pthread_rwlock_wrlock(&d->patternEntry.lock);
  while((pnode=Pattern_cut_first(&d->patternEntry))) {
    FREE(pnode);
  }
  pthread_rwlock_unlock(&d->patternEntry.lock);
  return ORTE_TRUE;
}
