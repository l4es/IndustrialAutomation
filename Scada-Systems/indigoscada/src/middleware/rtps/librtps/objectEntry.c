    /*
 *  $Id: objectEntry.c,v 0.0.0.1   2003/09/10
 *
 *  DEBUG:  section 8                   Functions with database of objects
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
// Implementation of GAVL functions
GAVL_CUST_NODE_INT_IMP(ObjectEntryHID, ObjectEntry, ObjectEntryHID, HostId,
    objRoot, hidNode, hid, gavl_cmp_int)
GAVL_CUST_NODE_INT_IMP(ObjectEntryAID, ObjectEntryHID, ObjectEntryAID, AppId,
    aidRoot, aidNode, aid, gavl_cmp_int)
GAVL_CUST_NODE_INT_IMP(ObjectEntryOID, ObjectEntryAID, ObjectEntryOID, ObjectId,
    oidRoot, oidNode, oid, gavl_cmp_int)
  
/*****************************************************************************/
void
objectEntryRefreshApp(ORTEDomain *d,ObjectEntryOID *objectEntryOID) {
  if (!objectEntryOID) return;
  if ((objectEntryOID->oid!=OID_APP) ||
      (objectEntryOID->guid.aid==AID_UNKNOWN) ||
      (!gavl_cmp_guid(&objectEntryOID->guid,&d->guid))) return;
  eventDetach(d,
          objectEntryOID->objectEntryAID,
          &objectEntryOID->expirationPurgeTimer,
          0);
  eventAdd(d,
          objectEntryOID->objectEntryAID,
          &objectEntryOID->expirationPurgeTimer,
          0,
          "ExpirationTimer",
          objectEntryExpirationTimer,
          NULL,
          objectEntryOID,
          &((AppParams*)objectEntryOID->attributes)->expirationTime);
  debug(8,3) ("refreshed: 0x%x-0x%x\n",
              objectEntryOID->objectEntryHID->hid,
              objectEntryOID->objectEntryAID->aid);
}

/*****************************************************************************/
ObjectEntryOID *
objectEntryFind(ORTEDomain *d,GUID_RTPS *guid) {
  ObjectEntryHID *objectEntryHID;
  ObjectEntryAID *objectEntryAID;

  objectEntryHID=ObjectEntryHID_find(&d->objectEntry,&guid->hid);
  if (objectEntryHID==NULL) return NULL;
  objectEntryAID=ObjectEntryAID_find(objectEntryHID,&guid->aid);
  if (objectEntryAID==NULL) return NULL;
  return ObjectEntryOID_find(objectEntryAID,&guid->oid);
}

/*
 * objectEntryAdd - Add a object to structure objectEntry
 * @objectEntry: pointer to root structure 
 * @guid: pointer to guid of object (hid,aid,oid)
 *
 * Return pointer to objectEntryOID
 */
ObjectEntryOID *
objectEntryAdd(ORTEDomain *d,GUID_RTPS *guid,void *params) {
  ObjectEntryHID *objectEntryHID;
  ObjectEntryAID *objectEntryAID;
  ObjectEntryOID *objectEntryOID;

  debug(8,10) ("objectEntry: start\n");
  objectEntryHID=ObjectEntryHID_find(&d->objectEntry,&guid->hid);
  //not exists Host -> create 
  if (objectEntryHID==NULL) {
    objectEntryHID=(ObjectEntryHID*)MALLOC(sizeof(ObjectEntryHID));
    //initialization items of structure objectEntryHID
    ObjectEntryAID_init_root_field(objectEntryHID);
    objectEntryHID->hid=guid->hid;
    //insert
    ObjectEntryHID_insert(&d->objectEntry,objectEntryHID);
    debug(8,5) ("objectEntry: Host  : %#10.8x created\n",guid->hid);
  }
  objectEntryAID=ObjectEntryAID_find(objectEntryHID,&guid->aid);
  //not exists Application -> create 
  if (objectEntryAID==NULL) {
    objectEntryAID=(ObjectEntryAID*)MALLOC(sizeof(ObjectEntryAID));
    objectEntryAID->aobject=NULL;
    //init items of structure objectEntryAID
    objectEntryAID->aid=guid->aid;
    ObjectEntryOID_init_root_field(objectEntryAID);
    //init structure htimUnicast
    htimerUnicastCommon_init_queue(objectEntryAID);
    ul_htim_queue_init_detached(&objectEntryAID->htimUnicast.commonNode.htim);
    htimerUnicastSendMetatraffic_init_queue(objectEntryAID);
    ul_htim_queue_init_detached(&objectEntryAID->htimUnicast.sendMetatrafficNode.htim);
    htimerUnicastSendUserData_init_queue(objectEntryAID);
    ul_htim_queue_init_detached(&objectEntryAID->htimUnicast.sendUserDataNode.htim);
    //insert
    ObjectEntryAID_insert(objectEntryHID,objectEntryAID);
    debug(8,5) ("objectEntry: App   : %#10.8x created\n",guid->aid);
  }
  objectEntryOID=ObjectEntryOID_find(objectEntryAID,&guid->oid);
  //not exists Object -> create 
  if (objectEntryOID==NULL) {
    objectEntryOID=(ObjectEntryOID*)MALLOC(sizeof(ObjectEntryOID));
    //initialization items of structure objectEntryOID
    objectEntryOID->guid=*guid;
    objectEntryOID->oid=guid->oid;
    objectEntryOID->objectEntryAID=objectEntryAID;
    objectEntryOID->objectEntryHID=objectEntryHID;
    objectEntryOID->attributes=params;
    ul_htim_queue_init_detached(&objectEntryOID->expirationPurgeTimer.htim);
    ul_htim_queue_init_detached(&objectEntryOID->sendCallBackDelayTimer.htim);
    ObjectEntryMulticast_init_head(objectEntryOID);
    objectEntryOID->multicastPort=0;
    if (guid->oid==OID_APP) {
      objectEntryAID->aobject=objectEntryOID;
      debug(8,5) ("objectEntry: Object: %#10.8x connected to AID\n",guid->oid);
    }
    //insert
    ObjectEntryOID_insert(objectEntryAID,objectEntryOID);
    debug(8,5) ("objectEntry: Object: %#10.8x created\n",guid->oid);
  }
  debug(8,10) ("objectEntry: finished\n");
  return objectEntryOID;
}

/*
 * objectEntryDelete - Delete a object from structure objectEntry
 * @objectEntry: pointer to root structure 
 * @obejctEntryOID: pointer to the deleted objectEntryOID
 *
 * return 0-no obj. was deleted, 1-OID was deleted,2-OID,AID was deleted,
 *        3-OID,AID,HID was deleted
 */
int
objectEntryDelete(ORTEDomain *d,ObjectEntryOID *objectEntryOID,Boolean destroy) {
  ObjectEntryHID *objectEntryHID;
  ObjectEntryAID *objectEntryAID;
  int            result=0;

  debug(8,10) ("objectEntryDelete: start\n");
  if (!objectEntryOID) return result;
  objectEntryHID=objectEntryOID->objectEntryHID;
  objectEntryAID=objectEntryOID->objectEntryAID;
  //Destroy object on level OID
  eventDetach(d,
          objectEntryOID->objectEntryAID,
          &objectEntryOID->expirationPurgeTimer,
          0);
  eventDetach(d,
          objectEntryOID->objectEntryAID,
          &objectEntryOID->sendCallBackDelayTimer,
          0);
  FREE(objectEntryOID->attributes);
  if (objectEntryAID->aobject==objectEntryOID) {
    objectEntryAID->aobject=NULL;
    debug(8,5) ("objectEntry: Object: %#10.8x deleted from AID\n",objectEntryOID->oid);
  }
  if (destroy) {
    ObjectEntryOID_delete(objectEntryAID,objectEntryOID);
    FREE(objectEntryOID);
  }
  debug(8,5) ("objectEntry: Object: %#10.8x deleted\n",objectEntryOID->oid);
  result=1;
  //Destroy object on level AID
  if (ObjectEntryOID_is_empty(objectEntryAID)) {
    debug(8,5) ("objectEntry: App   : %#10.8x deleted\n",objectEntryAID->aid);
    if (destroy) {
      ObjectEntryAID_delete(objectEntryHID,objectEntryAID);
      FREE(objectEntryAID);
    }
    result=2;
  }
  //Destroy object on level HID
  if (ObjectEntryAID_is_empty(objectEntryHID)) {
    debug(8,5) ("objectEntry: Host  : %#10.8x deleted\n",objectEntryHID->hid);
    if (destroy) {
      ObjectEntryHID_delete(&d->objectEntry,objectEntryHID);
      FREE(objectEntryHID);
    }
    result=3;
  }
  debug(8,10) ("objectEntryDelete: finished\n");
  return result;
}

/*
 * objectEntryDeleteAll - Delete all structure objectEntry
 * @objectEntry: pointer to root structure
 *
 */
void
objectEntryDeleteAll(ORTEDomain *d,ObjectEntry *objectEntry) {
  ObjectEntryHID *objectEntryHID;
  ObjectEntryAID *objectEntryAID;
  ObjectEntryOID *objectEntryOID;

  while((objectEntryHID=ObjectEntryHID_cut_first(objectEntry))) {
    while((objectEntryAID=ObjectEntryAID_cut_first(objectEntryHID))) {
      while((objectEntryOID=ObjectEntryOID_cut_first(objectEntryAID))) {
        objectEntryDelete(d,objectEntryOID,ORTE_FALSE);
        FREE(objectEntryOID);
      }
      FREE(objectEntryAID);
    }
    FREE(objectEntryHID);
  }
}

/*
 * ObjectEntryDump - Dump a objectEntry structure on stdio
 * @objectEntry: pointer to root structure
 *
 */
void
objectEntryDump(ObjectEntry *objectEntry) {
  ObjectEntryHID *objectEntryHID;
  ObjectEntryAID *objectEntryAID;
  ObjectEntryOID *objectEntryOID;

  gavl_cust_for_each(ObjectEntryHID,objectEntry,objectEntryHID) {
    debug(8,5) ("hid:%x\n",objectEntryHID->hid);
    gavl_cust_for_each(ObjectEntryAID,objectEntryHID,objectEntryAID) {
      debug(8,5) ("  aid:%x\n",objectEntryAID->aid);
      gavl_cust_for_each(ObjectEntryOID,objectEntryAID,objectEntryOID) {
        debug(8,5) ("    oid:%x\n",objectEntryOID->oid,objectEntryOID);
      }
    }
  }
}

