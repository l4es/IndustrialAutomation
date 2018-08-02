/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko, Kostya Volovich              *
                                                                          *
 OPC Group management
 **************************************************************************/

#include <opcerror.h>
#include "privopc.h"
#include "nan.h"

/*****************************************************************************/

STDMETHODIMP LightOPCGroup::CloneGroup(LPCWSTR szName,	REFIID riid, LPUNKNOWN *ppUnk)
{
 HRESULT hr = S_OK;
 LightOPCGroup *grp = 0;
 OPCHANDLE serverhandle;

 LO_CHECK_STATEz1("CloneGroup", ppUnk);
 UL_TRACE((LOG_GRH("CloneGroup(-> %ls)"), loWnul(szName)));
 if (!ppUnk) { hr = E_INVALIDARG; goto Exit; }

 if (!(grp = new LightOPCGroup) || !grp->iam_valid())
   { hr = E_OUTOFMEMORY; goto Exit; }

 lock_read();

    if (szName && owner->by_name(szName)) hr = OPC_E_DUPLICATENAME;
    else
      {
       unsigned ii;
       LightOPCItem *proto;
/* do cloning of items here */
       for(ii = 0; ii < itl.gl_count; ii++)
         if (proto = (LightOPCItem*)itl.gl_list[ii])
           {
            LightOPCItem *newby = new LightOPCItem;
            if (newby)
              {
               newby->convtype = proto->convtype;
               newby->tid = proto->tid;
               newby->AcPath = proto->AcPath;
               newby->bActive = proto->bActive;
               newby->hClient = proto->hClient;
               newby->vtRequestedDataType = proto->vtRequestedDataType;
               if (S_OK != (hr = newby->set_accpath(proto->szAccessPath)) ||
                   S_OK != (hr = newby->set_itemid(proto->szItemID)))
                 { delete newby; break; }
               if (!grp->add(newby))
                 { delete newby; hr = E_OUTOFMEMORY; break; }
               newby->Quality = -1;
               if (newby->bActive) grp->active_count++;
               else newby->Quality = OPC_QUALITY_OUT_OF_SERVICE;
              }
            else { hr = E_OUTOFMEMORY; break; }
           }
      }
 unlock();

 if (S_OK == hr)
   hr = owner->internalAddGroup(grp, szName, 0/*bActive*/,
                  UpdateRate,
                  ClientHandle,
                  &TimeBias,
                  &Deadband,
                  grLCID,
                  &serverhandle,
                  riid,
                  ppUnk);
Exit:
 if (S_OK == hr)
   {
    UL_NOTICE((LOG_GRH("CloneGroup(-> %ls) =%u /%p"),
	              loWnul(szName), serverhandle, *ppUnk));
   }
 else
   {
    UL_INFO((LOG_GRH("CloneGroup(-> %ls) =%s"),
                      loWnul(szName), loStrError(hr)));
    if (grp) delete grp;
   }

 LO_FINISH();
 return hr;
}

/*****************************************************************************/

STDMETHODIMP LightOPCServer::AddGroup(
		LPCWSTR szName,
		BOOL bActive,
		DWORD dwRequestedUpdateRate,
		OPCHANDLE hClientGroup,
		LONG *pTimeBias,
		FLOAT *pPercentDeadband,
		DWORD dwLCID,
		OPCHANDLE *phServerGroup,
		DWORD *pRevisedUpdateRate,
		REFIID riid,
		LPUNKNOWN *ppUnk
		)
{
 HRESULT hr = S_OK;
 LightOPCGroup *grp = 0;
 DWORD RevisedRate;
 OPCHANDLE ServerGroup;

 LO_CHECK_STATEz1("AddGroup", ppUnk);

 UL_TRACE((LOG_SR("AddGroup(%ls)"), loWnul(szName)));
 if (!ppUnk) { hr = E_INVALIDARG; goto Exit; }

// if (!phServerGroup || !pRevisedUpdateRate) { hr = E_INVALIDARG; goto Exit; }
 if (!phServerGroup) phServerGroup = &ServerGroup;
 if (!(grp = new LightOPCGroup) || !grp->iam_valid())
   { hr = E_OUTOFMEMORY; goto Exit; }
/*  need assign all group atributes */

 RevisedRate = loReviseUpdateRate(se, dwRequestedUpdateRate);

 hr = internalAddGroup(grp, szName, bActive,
			RevisedRate,
			hClientGroup,
			pTimeBias,
			pPercentDeadband,
			dwLCID,
			phServerGroup,
			riid,
			ppUnk);

Exit:
// if (!szName && grp) szName = grp->name;
 if (S_OK == hr)
   {
    if (pRevisedUpdateRate) *pRevisedUpdateRate = RevisedRate;
    if (RevisedRate != dwRequestedUpdateRate) hr = OPC_S_UNSUPPORTEDRATE;
    UL_NOTICE((LOG_SR("AddGroup(%ls) =%u (%umS (%umS) %g%%) %p"),
      loWnul(szName), *phServerGroup,
      (unsigned)RevisedRate, (unsigned)dwRequestedUpdateRate,
      pPercentDeadband? *pPercentDeadband: 0.0, *ppUnk));
   }
 else
   {
    UL_INFO((LOG_SR("AddGroup(%ls) =%s"), loWnul(szName), loStrError(hr)));
    if (grp) delete grp;
   }
 LO_FINISH();
 return hr;
}

/*****************************************************************************/

HRESULT LightOPCServer::internalAddGroup(
        LightOPCGroup *grp,
        LPCWSTR szName,
        BOOL bActive,
        DWORD UpdateRate,
        OPCHANDLE hClientGroup,
        LONG *pTimeBias,
        FLOAT *pPercentDeadband,
        DWORD dwLCID,
        OPCHANDLE *phServerGroup,
        REFIID riid,
        LPUNKNOWN *ppUnk
        )
{
 HRESULT hr = S_OK;
 unsigned servindex = 0;
 OPCHANDLE servhandle = 0;
 loWchar tname[loUNIQUE_LEN];

 if (phServerGroup) *phServerGroup = 0;
 if (!ppUnk) { hr = E_INVALIDARG; goto Exit; }
 *ppUnk = 0;

/*    if (!grp && !(grp = new LightOPCGroup()) ||
     grp->sstate != loST_READY) { hr = E_OUTOFMEMORY; goto Exit; }*/
/*  need assign all group atributes */
 grp->ClientHandle = hClientGroup;
 grp->UpdateRate = UpdateRate;
 grp->Active = bActive != 0;
 grp->Deadband = .0f;
 if (pPercentDeadband)
   {
    if (lo_IsNANf(*pPercentDeadband) ||
        *pPercentDeadband < 0. ||
        *pPercentDeadband > 100.) { hr = E_INVALIDARG; goto Exit; }
    grp->Deadband = *pPercentDeadband;
   }
 if (se->driver.ldCheckLocale && 
     se->driver.ldCheckLocale(&ctxt.cactx, dwLCID))
   GetLocaleID(&dwLCID);
 grp->grLCID = dwLCID;

 if (pTimeBias) grp->TimeBias = *pTimeBias;
 else
   {
    TIME_ZONE_INFORMATION tzi;
    tzi.Bias = 0;
    GetTimeZoneInformation(&tzi);
    grp->TimeBias = tzi.Bias;
   }

 if (szName)
   if (!*szName) szName = 0;
   else if (S_OK != grp->set_name(szName)) { hr = E_OUTOFMEMORY; goto Exit; }

// grp->sstate = loST_READY;
 hr = grp->QueryInterface(riid, (void**)ppUnk);
// grp->sstate = loST_DOWN;
 if (S_OK != hr) goto Exit;

 lock_write();
    if (!szName)
      {
       if (S_OK == (hr = mk_unique(tname, grp)))
         hr = grp->set_name(szName = tname);
      }
    else if (by_name(szName)) hr = OPC_E_DUPLICATENAME;

    if (S_OK == hr)
      if (servindex = add(grp))
        {
         do {
             if ((servhandle = ++unique_GroupHandle) == 0)
               servhandle = ++unique_GroupHandle;
            }
         while(by_handle(servhandle));

         grp->ServerHandle = servhandle;
         ostatus.dwGroupCount++;
        }
      else { hr = E_OUTOFMEMORY; /* delete grp; will be deleted by caller */ }
 unlock();

 if (phServerGroup) *phServerGroup = servhandle;

Exit:
 if (S_OK == hr)
   {
    UL_TRACE((LOG_SR("intAddGroup(%ls) i:%u h:%u /%p"),
              szName? szName: tname, servindex, servhandle, *ppUnk));
   }
 else
   {
     UL_TRACE((LOG_SR("intAddGroup(%ls) =%s"), loWnul(szName), loStrError(hr)));
//	 if (grp) delete grp;
    *ppUnk = 0;
   }

 return hr;
}

/*****************************************************************************/

HRESULT LightOPCServer::internalRemoveGroup(unsigned groupHandleID, int bForce)
{
#if 1
 HRESULT hr = loOPC_E_INVALIDHANDLE;
#else
 HRESULT hr = loOPC_E_NOTFOUND;
#endif
 unsigned ii;

 UL_DEBUG((LOG_SR("internalRemoveGroup(%u %s)"), groupHandleID, bForce? "FORCE": ""));

    WR_lock(&lk_remove);
    lock_write();
        if (ii = by_handle(groupHandleID))
          {
           hr = del(ii, bForce && !(/*se->driver.*/ldFlags & loDf_NOFORCE))
                   ? OPC_S_INUSE: S_OK;
           ostatus.dwGroupCount--;
          }
    unlock();
    lw_rw_unlock(&lk_remove);


 UL_DEBUG((LOGID, "%!l internalRemoveGroup(%u %s) =",
                   hr, groupHandleID, bForce? "FORCE": ""));
 return hr;
}

STDMETHODIMP LightOPCServer::RemoveGroup(OPCHANDLE groupHandleID, BOOL bForce)
{
 HRESULT hr = loOPC_E_INVALIDHANDLE;

 LO_CHECK_STATEz0("RemoveGroup");
 UL_TRACE((LOG_SR("RemoveGroup(%u %s)"), groupHandleID, bForce? "FORCE": ""));

 if (groupHandleID)
   {
#if LO_USE_BOTHMODEL
    if (ldFlags & loDf_BOTHMODEL)
      {
       loRequest *rq = lo_req_alloc(0, 0);
       UL_DEBUG((LOG_SR("RemoveGroup() going to async")));
       if (!rq) hr = E_OUTOFMEMORY;
       else
         {
          rq->operation = loRQ_SYNC | loRQ_OP_REMOVE_GROUP |
                             (bForce? loRQ_OF_REMOVE_FORCE: 0);
          rq->group_key = groupHandleID;
          rq->serv_key = serv_key;

          if (rq = lo_req_put_sync(&q_req, &q_ret, rq))
            {
             hr = rq->upl.master_err;
             lo_req_free(rq);
            }
          else hr = E_FAIL;
         }
      }
    else
#endif
        hr = internalRemoveGroup(groupHandleID, bForce);
   }

 if (S_OK != hr) UL_INFO((LOG_SR("RemoveGroup(%u %s) =%s"),
                              groupHandleID, bForce? "FORCE": "", loStrError(hr)));
 else UL_NOTICE((LOG_SR("RemoveGroup(%u %s) =Ok"), groupHandleID, bForce? "FORCE": ""));

 LO_FINISH();
 return hr;
}

/*****************************************************************************/

STDMETHODIMP LightOPCServer::GetGroupByName(LPCWSTR szGroupName, REFIID riid, LPUNKNOWN *ppUnk)
{
 HRESULT hr = S_OK;
 LightOPCGroup *grp;

 LO_CHECK_STATEz1("GetGroupByName", ppUnk);
 UL_TRACE((LOG_SR("GetGroupByName(%ls)"), loWnul(szGroupName)));
 if (!ppUnk) hr = E_INVALIDARG;
 else
   {
    if (!szGroupName) hr = E_INVALIDARG;
    else
      {
       lock_read();
          hr = loOPC_E_NOTFOUND;
	  if (grp = by_name(szGroupName))
	    hr = grp->QueryInterface(riid, (LPVOID*)ppUnk);
       unlock();
      }
   }

 if (S_OK != hr) UL_INFO((LOG_SR("GetGroupByName(%ls) =%s"),
               loWnul(szGroupName), loStrError(hr)));
 else UL_NOTICE((LOG_SR("GetGroupByName(%ls) =%u /%p"),
			szGroupName, ((LightOPCGroup*)grp)->ServerHandle, *ppUnk));
 LO_FINISH();
 return hr;
}

/*********************************** LightOPCGroup::IOPCGroupStateMgt  stuff **************************/

STDMETHODIMP LightOPCGroup::GetState (DWORD *pUpdateRate, BOOL *pActive, LPWSTR *ppName,
		                  LONG *pTimeBias, FLOAT *pPercentDeadband, DWORD *pLCID,
		                  OPCHANDLE * phClientGroup, OPCHANDLE * phServerGroup)
{
 HRESULT hr = S_OK;

 LO_CHECK_STATEz1("GetState", ppName);

 if (!ppName) { hr = E_INVALIDARG; goto Return; }

 UL_TRACE((LOG_GRH("LightOPCGroup::GetState()")));

 lock_read();
     if (phClientGroup) *phClientGroup = ClientHandle;
     if (phServerGroup) *phServerGroup = ServerHandle;
     if (pUpdateRate) *pUpdateRate = UpdateRate;
     if (pActive) *pActive = Active;
     if (pPercentDeadband) *pPercentDeadband = Deadband;
     if (pLCID) *pLCID = grLCID;
     if (pTimeBias) *pTimeBias = TimeBias;
     if (ppName && !(*ppName = loComWstrdup(name))) hr = E_OUTOFMEMORY;
 unlock();

Return:
 if (S_OK != hr) UL_INFO((LOG_GRH("GetGroupState() =%s"), loStrError(hr)));
 else UL_NOTICE((LOG_GRH("GetGroupState(%ls) = Ok"), loWnul(ppName? *ppName: 0)));

 LO_FINISH();
 return hr;
}

STDMETHODIMP LightOPCGroup::SetState(DWORD *pRequestedUpdateRate, DWORD *pRevisedUpdateRate,
		                  BOOL *pActive, LONG *pTimeBias, FLOAT *pPercentDeadband,
		                  DWORD *pLCID, OPCHANDLE *phClientGroup)
{
 HRESULT hr = S_OK;

 LO_CHECK_STATEz0("SetState");
 UL_TRACE((LOG_GRH("LightOPCGroup::SetState()")));

 if (pLCID && owner->se->driver.ldCheckLocale &&
              owner->se->driver.ldCheckLocale(&owner->ctxt.cactx, *pLCID))
   pLCID = 0; /* ignore on errors */
 lock_write();
    if (pRequestedUpdateRate)
      {
//	  if (!pRevisedUpdateRate) hr = E_INVALIDARG;
       DWORD revised_rate = loReviseUpdateRate(owner->se, *pRequestedUpdateRate);
       if (UpdateRate > revised_rate && (!pActive || *pActive)
         /* && Active && 0 != (advise_present & advise_enabled)*/)
         UpdateRate = revised_rate, actuate_async(0);
       else UpdateRate = revised_rate;
       if (UpdateRate != *pRequestedUpdateRate) hr = OPC_S_UNSUPPORTEDRATE;
      }
    if (pRevisedUpdateRate) *pRevisedUpdateRate = UpdateRate;
    if (pActive) set_active(*pActive);
    if (pTimeBias) TimeBias = *pTimeBias;
    if (pPercentDeadband)
      if (lo_IsNANf(*pPercentDeadband) ||
          *pPercentDeadband < 0. ||
          *pPercentDeadband > 100.) hr = E_INVALIDARG;
      else Deadband = *pPercentDeadband;
    if (pLCID) grLCID = *pLCID;
    if (phClientGroup) ClientHandle = *phClientGroup;
 unlock();

 if (!FAILED(hr)) UL_NOTICE((LOG_GR("SetGroupState(%u) =(%umS (%umS) %g%%)Ok"),
                  ServerHandle, (unsigned)UpdateRate,
                  (unsigned)(pRequestedUpdateRate? *pRequestedUpdateRate: 0),
                  Deadband));
 else UL_INFO((LOG_GR("SetGroupState(%u) =%s"), ServerHandle, loStrError(hr)));

 LO_FINISH();
 return hr;
}

STDMETHODIMP LightOPCGroup::SetName(LPCWSTR szName)
{
 HRESULT hr = S_OK;

 LO_CHECK_STATEz0("SetName");

 UL_TRACE((LOG_GRH("LightOPCGroup::SetName()")));
 if (!szName) hr = E_INVALIDARG;
 else
   {
    LightOPCGroup *grp;
    lock_write();
       grp = owner->by_name(szName);
       if (!grp) hr = set_name(szName);
       else if (grp != this) hr = OPC_E_DUPLICATENAME;
    unlock();
   }
 if (S_OK != hr) UL_INFO((LOG_GRH("SetGroupName(%ls) =%s"),
                              loWnul(szName), loStrError(hr)));
 else UL_NOTICE((LOG_GRH("SetGroupName(%ls)"), szName));

 LO_FINISH();
 return hr;
}

void LightOPCGroup::set_active(BOOL act)
{
 unsigned ii;
 LightOPCItem *oi;
 int aqual = OPC_QUALITY_OUT_OF_SERVICE;
// UL_DEBUG((LOG_GRHO("set_active(%u, %ls)"), act, loWnul(name)));

 if (act = act != 0) aqual = -1;
 if (act == Active) return;

 if (owner->se->driver.ldSubscribe)
   {
    for(ii = itl.gl_count; ii;)
      {
       unsigned ch;
       loTagId chact[32];

       for(ch = 0; ch < SIZEOF_ARRAY(chact) && ii; )
         if ((oi = (LightOPCItem*)itl.gl_list[--ii]) && oi->bActive)
           chact[ch++] = oi->tid,
           oi->Quality = aqual,
           oi->TouchChanged();

       if (ch) loChangeActivity(&owner->ctxt.cactx, act, ch, chact);
      }
   }
 else if (act)
   for(ii = itl.gl_count; ii;)
     if (oi = (LightOPCItem*)itl.gl_list[--ii])
       oi->Quality = -1, oi->TouchChanged();

 Active = act;
 if (Active /*&&
     0 != (advise_present & advise_enabled)*/)
    actuate_async(1);

// UL_DEBUG((LOG_GRHO("set_active(%u, %ls), finished"), act, loWnul(name)));
}

/* end of groupman.cpp */
