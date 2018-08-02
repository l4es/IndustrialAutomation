/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2002 by Timofei Bondarenko                             *

 Object tracker for Enumerators.
 It used to perform CoDisconnect on all returned objects.
 **************************************************************************/

#ifndef OBJTRACK_H
#define OBJTRACK_H

#ifndef OPTIONS_H
#include "options.h"
#endif

#ifdef __cplusplus

class loObjTrack;

class loObjTracker
{
public:
        int         ot_stopped;
#if LO_USE_OBJTRACK
        loObjTrack *ot_first;
        lw_rmutex   ot_lk; /* !! Actually SHOULD be rmutex_t due to
                              ot_disconnect_all() implementation */
 inline void     ot_lock(void)   { lw_rmutex_lock(&ot_lk); }
 inline void     ot_unlock(void) { lw_rmutex_unlock(&ot_lk); }
        unsigned ot_disconnect_all(int remove);
 inline  loObjTracker(): ot_stopped(0), ot_first(0) {};
#else
 inline  loObjTracker(): ot_stopped(0) {};
 inline unsigned ot_disconnect_all(int remove) { return 0; }
#endif
};

class loObjTrack
{
public:
#if LO_USE_OBJTRACK
  loObjTracker  *ot_tr;
  loObjTrack    *ot_next;
  loObjTrack   **ot_iam;
  IUnknown      *ot_unk;

inline  loObjTrack(): ot_tr(0), ot_next(0), ot_iam(0), ot_unk(0) {};
inline ~loObjTrack() { ot_remove(); }

    void  ot_remove(void);
 HRESULT  ot_connect(IUnknown *pUnk, loObjTracker *list);

inline  loObjTracker *ot_tracker(void) { return ot_tr; };

private:
  friend class loObjTracker;
inline void unlocked_remove(void);
inline void unlocked_connect(loObjTracker *list);

#else /*!LO_USE_OBJTRACK*/
inline  HRESULT  ot_connect(IUnknown *pUnk, loObjTracker *list) { return S_OK; }
inline  loObjTracker *ot_tracker(void) { return 0; };
#endif
};

#endif /*__cplusplus*/
#endif /*OBJTRACK*/
