/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2002 by Timofei Bondarenko                             *

 Object tracker for Enumerators.
 It used to perform CoDisconnect on all returned objects.
 **************************************************************************/

#include "privopc.h"
#include "objtrack.h"

#if LO_USE_OBJTRACK

inline void loObjTrack::unlocked_remove(void)
{
    if (ot_iam) 
      {
       if (*ot_iam = ot_next)
         ot_next->ot_iam = ot_iam;
       ot_iam = 0;
      }
    ot_next = 0;
//    ot_tr = 0;
}

inline void loObjTrack::unlocked_connect(loObjTracker *list)
{
// ot_tr = list;
 ot_iam = &list->ot_first;
 if (ot_next = *ot_iam)
   ot_next->ot_iam = &ot_next;
 *ot_iam = this;
}

unsigned loObjTracker::ot_disconnect_all(int remove)
{
 unsigned count = 0;
 loObjTrack *tail;

    ot_lock();

    UL_DEBUG((LOGID, "ot_disconnect(%u) %p started", remove, ot_first));
    
    if (tail = ot_first)
      {
       ot_first = 0;
       tail->ot_iam = &tail;
       while(tail)
         {
          loObjTrack *cur = tail;
          cur->unlocked_remove();
          if (!remove)
            cur->unlocked_connect(this);
          LO_CO_DISCONNECT(cur->ot_unk, 0);
          count++;
         }
      }
    UL_DEBUG((LOGID, "ot_disconnect(%u) finished", count));

    ot_unlock();
 
 return count;
}

HRESULT loObjTrack::ot_connect(IUnknown *pUnk, loObjTracker *list)
{
 if (!ot_tr && pUnk && list && !list->ot_stopped)
   {
    list->ot_lock();
    unlocked_connect(list);
    ot_tr = list;
    ot_unk = pUnk;
    list->ot_unlock();
    if (!list->ot_stopped) return S_OK;
   }
 return LO_E_SHUTDOWN;
}

void loObjTrack::ot_remove(void)
{
 if (ot_iam && ot_tr)
   {
    loObjTracker *list = ot_tr;
    UL_DEBUG((LOGID, "ot_remove(%p->%p)", ot_iam, ot_unk));
    list->ot_lock();
    unlocked_remove();
    list->ot_unlock();
   }
}

#endif /*LO_USE_OBJTRACK*/
/* end of objtrack.cpp */
