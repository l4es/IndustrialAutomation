/*
 *  $Id: ORTETypeRegister.c,v 0.0.0.1   2003/08/21 
 *
 *  DEBUG:  section 26                  Type register
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

GAVL_CUST_NODE_INT_IMP(ORTEType, TypeEntry, TypeNode, const char *,
    types, node, typeRegister.typeName, gavl_cmp_str)
    

/*****************************************************************************/
Boolean
ORTETypeRegisterFind(ORTEDomain *d,const char *typeName) {
  Boolean            result=ORTE_FALSE;
  
  if (!d) 
    return ORTE_FALSE;  //bat handle

  pthread_rwlock_rdlock(&d->typeEntry.lock);    
  if (ORTEType_find(&d->typeEntry,&typeName)) 
    result=ORTE_TRUE;
  pthread_rwlock_unlock(&d->typeEntry.lock);    
  return result;
}

/*****************************************************************************/
int
ORTETypeRegisterAdd(ORTEDomain *d,const char *typeName,ORTETypeSerialize ts,
                    ORTETypeDeserialize ds,ORTETypeGetMaxSize gms,unsigned int ms) {
  TypeNode           *tn;
  
  if (!d) 
    return ORTE_BAD_HANDLE;       //bat handle

  if (gms) {
    if (d->domainProp.wireProp.userBytesPerPacket<ms) 
      return -2;
  }

  pthread_rwlock_wrlock(&d->typeEntry.lock);    
  tn=ORTEType_find(&d->typeEntry,&typeName);
  if (!tn) {
    tn=(TypeNode*)MALLOC(sizeof(TypeNode));
    tn->typeRegister.typeName=(char*)MALLOC(strlen(typeName)+1);
    if (tn->typeRegister.typeName) {
      memcpy((void*)tn->typeRegister.typeName, 
	     typeName, 
	     strlen(typeName) + 1);
    }    
    ORTEType_insert(&d->typeEntry,tn);
    debug(26,3) ("ORTETypeRegisterAdd: created\n");
  }
  tn->typeRegister.serialize=ts;
  tn->typeRegister.deserialize=ds;
  tn->typeRegister.getMaxSize=gms;
  tn->typeRegister.maxSize=ms;
  pthread_rwlock_unlock(&d->typeEntry.lock);    
  debug(26,3) ("ORTETypeRegisterAdd: registered type:%s\n",typeName);
  return ORTE_OK;
}

/*****************************************************************************/
int
ORTETypeRegisterDestroyAll(ORTEDomain *d) {
  TypeNode           *tn;
  
  if (!d) 
    return ORTE_BAD_HANDLE;  //bat handle

  pthread_rwlock_wrlock(&d->typeEntry.lock);    
  while((tn=ORTEType_cut_first(&d->typeEntry))) {
    FREE((char*)tn->typeRegister.typeName);
    FREE(tn);
  }
  pthread_rwlock_unlock(&d->typeEntry.lock);    
  return ORTE_OK;
}





