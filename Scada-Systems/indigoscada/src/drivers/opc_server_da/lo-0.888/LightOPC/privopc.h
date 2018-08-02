/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko, Kostya Volovich              *
 **************************************************************************/

#ifndef PRIVOPC_H
#define PRIVOPC_H

/**************************************************************************
 TODO:
 It would be better to sort groups by UpdateRate.

 advise_enabled can be eliminated***
 GetEnable() SetEnable() can modify advise_present because
   they are not work if connection was not advised.
   but how we'll find connections for explicit Refresh/Read/Write ?

 make locking for Activate/Deactivate less extensive

 it's better to compute deadband using Requested datatypes

 include TagAlias as reduced RealTag

 CacheRead from PrimaryCache vs Per-Item-Cache? less locks/more updates.

 **************************************************************************
  IMPORTANT:
            Order of the locks. When more than 1 lock should be locked
            they must be locked in this order!
            Otherwise deadlocks can occure.

loService::lkList
loClient::async.lk
loClient::lk_remove
loClient::lk_all
loService::lkDr

loClient::lk_browse
loService::lkMgmt
loService::update_pipe.lk
loService::lkPrim

 **************************************************************************
 Internal declarations:
 **************************************************************************/

#include <opcda.h>	/* The OPC custom interface defintions */
#include <opccomn.h>

#ifndef OPTIONS_H
#include "options.h"
#endif

#ifndef LIGHTOPC_H
#include "lightopc.h"
#endif
#ifndef LOSERV_H
#include "loserv.h"
#endif
#ifndef REQQUEUE_H
#include "reqqueue.h"
#endif
#ifndef UTIL_H
#include "util.h"
#endif
#ifndef GROWLIST_H
#include "growlist.h"
#endif
#ifndef OBJTRACK_H
#include "objtrack.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
#ifdef __cplusplus
	}
//#endif see at the EOF
#include "connpnt.h"

class LightOPCServer;
class LightOPCGroup;
class LightOPCItem;

/*#define LightOPCServer loClient
 It's right: server will be created on each client connection.
   Thus, from driver point of view LightOPCServer is assotiated with a client */

#define CLINAME_FMT   "%.16s "
#define LOG_SR(fmt)   LOGID, CLINAME_FMT fmt, client_name
#define LOG_GR(fmt)   LOGID, CLINAME_FMT fmt, owner->client_name
#define LOG_GRO(fmt)  LOGID, CLINAME_FMT fmt, (iam_attached()? owner->client_name: "?")
#define LOG_GRH(fmt)  LOGID, CLINAME_FMT "%u:" fmt, owner->client_name, ServerHandle
#define LOG_GRHO(fmt) LOGID, CLINAME_FMT "%u:" fmt, (iam_attached()? owner->client_name: "?"), ServerHandle

#define LOG_SR1(fmt,first)   LOGID, CLINAME_FMT fmt, first, client_name
#define LOG_GR1(fmt,first)   LOGID, CLINAME_FMT fmt, first, owner->client_name

#if LO_USE_OBJXREF
struct loObjXref
  {
inline  loObjXref() { InterlockedIncrement(&lo_X_objcount); }
inline ~loObjXref() { InterlockedDecrement(&lo_X_objcount); }
  };
#define LO_OBJ_XREF  loObjXref xxx_ref
#else
#define LO_OBJ_XREF
#endif

#define loUNIQUE_LEN (32)

#define LO_CHECK_STATE_(string) if (lock_state(string)) return LO_E_SHUTDOWN; else {

#define LO_CHECK_STATEz0(string) LO_CHECK_STATE_(string)
#define LO_CHECK_STATEz1(string,a1)          if (a1) *(a1) = 0; LO_CHECK_STATEz0(string)
#define LO_CHECK_STATEz2(string,a1,a2)       if (a2) *(a2) = 0; LO_CHECK_STATEz1(string,a1)
#define LO_CHECK_STATEz3(string,a1,a2,a3)    if (a3) *(a3) = 0; LO_CHECK_STATEz2(string,a1,a2)
#define LO_CHECK_STATEz4(string,a1,a2,a3,a4) if (a4) *(a4) = 0; LO_CHECK_STATEz3(string,a1,a2,a3)

#define LO_FINISH()  unlock_state(); }

typedef struct
  {
   IOPCShutdown *request;
   loWchar      *reason;
  } loShutdownConn;

void loShutdownConn_init(loShutdownConn *);

void loShutdownConn_clear(loShutdownConn *);
void loShutdownConn_call(loShutdownConn *);

#define loShutdownConn_init(p) (memset((p), 0, sizeof(loShutdownConn)))

class LightOPCServer : public IOPCServer,
                       public IOPCCommon,
                       public IOPCBrowseServerAddressSpace,
                       public IOPCItemProperties,
                       public loConnPnt1 /*public IConnectionPointContainer,*/
{
     LO_OBJ_XREF;
public:
     unsigned long initphase;
     glGrowingList grl;
     loService *se;
     LightOPCServer *serv_next;
     loCallerx ctxt;

     unsigned serv_key;
     int ldFlags;
     short access_mode;
     HRESULT (*qi_chain)(void *rha, loService *, loClient *,
                         const IID *, LPVOID *);

     loThrControl async; /* On signaling the tasync.state should be = 1
                            2 forces client_scheduler() to call selfdestroy() */
inline void touch_scheduler(void)
            {
             if (0 == async.tstate)
               {
                lw_mutex_lock(&async.lk);
                if (0 == async.tstate) async.tstate = 1;
                lw_conds_signal(&async.cond);
                lw_mutex_unlock(&async.lk);
               }
            }
/* This additional flag is requered for proper hanling of late events
   which has occured in the middle of client_scheduler() loop.
   Such signals will cause zero-time sleep and will be handled on the
   next iteration. */
#if LO_USE_BOTHMODEL
     char ole_initialized;
#endif
     int  shuttingdown; /* locked by async.lk */
     loObjTracker otrk;   /* & otrk.ot_stopped - flags for lock_state() */
     loQueueAsync q_req;
     loQueueBcast q_ret;

     loShutdownConn shutdown_conn; /* locked by async.lk */

     HRESULT  sync_advise(int operation, void **arg, loRqid grp_key);
     HRESULT async_advise(int conn, const IID *iface,
                          LightOPCGroup *grp, unsigned grp_key,
                          IUnknown *pUnk);
inline HRESULT async_unadvise(int conn, const IID *iface,
                              LightOPCGroup *grp, unsigned grp_key)
        { return async_advise(conn, iface, grp, grp_key, 0); }

     /* [un]lock_state() must NOT be virtual ! */
     int lock_state(const char *msg, const IID *riid_nostrict = 0);
                 /* returns 0 if Ok or -1 if out of service */
inline ULONG unlock_state(void)
  { UL_DEBUG((LOG_SR("unlock_state() -> finished: va=%d st=%d sd=%d rc=%d"),      
      loIS_VALID(this), otrk.ot_stopped, shuttingdown, RefCount));
    return Release();}

     void set_state(int oper, int state, const loWchar *reason);
     void client_scheduler(void);

     lw_rwlock lk_remove;
     lw_rwlock lk_all;
#if LO_USE_BOTHMODEL
     int (*lml_rd_lock)(lw_rwlock *lk, DWORD timeout);
     int (*lml_wr_lock)(lw_rwlock *lk, DWORD timeout);
     inline void lock_read(void)   { lml_rd_lock(&lk_all, INFINITE); }
     inline void lock_write(void)  { lml_wr_lock(&lk_all, INFINITE); }
     inline int RD_lock(lw_rwlock *rw)  { return lml_rd_lock(rw, INFINITE); }
     inline int WR_lock(lw_rwlock *rw)  { return lml_wr_lock(rw, INFINITE); }
#else
     inline int RD_lock(lw_rwlock *rw)  { return lw_rw_rdlock(rw); }
     inline int WR_lock(lw_rwlock *rw)  { return lw_rw_wrlock(rw); }
     inline void lock_read(void)
         {
//          UL_DEBUG((LOGID, "RdLock..."));
          lw_rw_rdlock(&lk_all);
  //      UL_DEBUG((LOGID, "...RdLock"));
         }
     inline void lock_write(void)
         {
    //      UL_DEBUG((LOGID, "WrLock..."));
          lw_rw_wrlock(&lk_all);
      //  UL_DEBUG((LOGID, "...WrLock"));
         }
#endif
     inline void unlock(void)
         {
        //  UL_DEBUG((LOGID, "UnLock..."));
          lw_rw_unlock(&lk_all);
          //UL_DEBUG((LOGID, "...UnLock"));
         }

     loStringBuf browsepos;
     lw_mutex    lk_browse;
inline void lock_browse(void)   { lw_mutex_lock(&lk_browse); }
inline void unlock_browse(void) { lw_mutex_unlock(&lk_browse); }

HRESULT set_browsepos(const loWchar *pos, int len = -1);
    int cat_browsepos(const loWchar *cpos);

     void (*release_handle)(void *, loService *, loClient *);
     void  *release_handle_arg;

     OPCSERVERSTATUS ostatus;
     loMilliSec bandwidth_time;

     unsigned long unique_GroupName;
     OPCHANDLE unique_GroupHandle;
     HRESULT mk_unique(loWchar buf[loUNIQUE_LEN], const void *ptr);

     LightOPCServer();
virtual ~LightOPCServer();
     int initialize(loService *serv, int ld_flags, const loVendorInfo *vi);
     void selfdestroy(void);

     int attach(void); /* set the owner */
     int detach(void); /* should set ready = 0 and do some cleanup */

     void clear_all(void);
     LightOPCGroup *by_name(const loWchar *name);
     unsigned by_handle(OPCHANDLE sgh);
     LightOPCGroup *by_handle_g(unsigned sgh);
     void del_all(int force = 0);
	/* returns -1 on errors or ==0/!=0 depend on item's Release() */
     int del(unsigned index, int force = 0);
     unsigned add(LightOPCGroup *grp);

/* IUnknown implementation */
     LONG RefCount;
     STDMETHOD_ (ULONG, AddRef)(void);
     STDMETHOD_ (ULONG, Release)(void);
     STDMETHOD (QueryInterface) (REFIID riid, void **ppv);

/* IOPCCommon implementation */
     char client_name[64]; /* no pointers - no locks */

	 STDMETHOD (SetLocaleID) (LCID dwLcid);
	 STDMETHOD (GetLocaleID) (LCID *dwLcid);
	 STDMETHOD (QueryAvailableLocaleIDs) (DWORD* pdwCount, LCID** pdwLcid);
     STDMETHOD (GetErrorString)(LONG dwError, LPWSTR *ppString);
	 STDMETHOD (SetClientName) (LPCWSTR szClientName);

/* IConnectionPoint methods */
 STDMETHOD (GetConnectionInterface)(IID *pIID);
 STDMETHOD (Advise)(IUnknown *pUnk, DWORD *pdwCookie);
 STDMETHOD (Unadvise)(DWORD dwCookie);
/* loConnPnt1 methods: */
 HRESULT icp_AddRef(void);
 void    icp_Release(void);
 loObjTracker *icp_ObjTracker(void);

/* IOPCServer implementation */
     STDMETHOD (GetErrorString)(HRESULT hr, LCID locale, LPWSTR *ppString);
     STDMETHOD (AddGroup)(LPCWSTR szName, BOOL bActive, DWORD dwRequestedUpdateRate,
		              OPCHANDLE hClientGroup, LONG *pTimeBias, FLOAT *pPercentDeadband,
		              DWORD dwLCID, OPCHANDLE *phServerGroup, DWORD *pRevisedUpdateRate,
		              REFIID riid, LPUNKNOWN *ppUnk);
     STDMETHOD (GetGroupByName)(LPCWSTR szGroupName, REFIID riid, LPUNKNOWN *ppUnk);
     STDMETHOD (GetStatus)(OPCSERVERSTATUS** ppServerStatus);
     STDMETHOD (RemoveGroup)(OPCHANDLE groupHandleID, BOOL bForce);
     STDMETHOD (CreateGroupEnumerator)(OPCENUMSCOPE dwScope, REFIID riid, LPUNKNOWN *ppUnk);

/* IOPCBrowseServerAddressSpace implementation */
     STDMETHOD (QueryOrganization)(OPCNAMESPACETYPE *pNameSpaceType);
     STDMETHOD (BrowseAccessPaths)(LPCWSTR szItemID, LPENUMSTRING *ppIEnumString);
     STDMETHOD (ChangeBrowsePosition)(OPCBROWSEDIRECTION dwBrowseDirection,
                                      LPCWSTR szString);
     STDMETHOD (GetItemID)(LPWSTR szItemDataID, LPWSTR *szItemID);
     STDMETHOD (BrowseOPCItemIDs)(OPCBROWSETYPE dwBrowseFilterType,
            /* [string][in] */ LPCWSTR szFilterCriteria,
            /* [in] */ VARTYPE vtDataTypeFilter,
            /* [in] */ DWORD dwAccessRightsFilter,
            /* [out] */ LPENUMSTRING *ppIEnumString);

/* IOPCItemProperties implementation */
     STDMETHOD (QueryAvailableProperties) (LPWSTR szItemID, DWORD* pwdCount, DWORD** ppPropertyIDs,
                                           LPWSTR **ppDescriptions, VARTYPE **ppvtDataTypes);

     STDMETHOD (GetItemProperties) (LPWSTR szItemID, DWORD dwCount, DWORD* pdwPropertyIDs,
 	                            VARIANT **ppvData, HRESULT **ppErrors);
     STDMETHOD (LookupItemIDs) (LPWSTR szItemID, DWORD dwCount, DWORD* pdwPropertyIDs,
		                LPWSTR **ppsNewItemIDs, HRESULT **ppErrors);
/***************************/
      HRESULT internalAddGroup(LightOPCGroup *grp,
			       LPCWSTR szName, BOOL bActive, DWORD UpdateRate,
			       OPCHANDLE hClientGroup, LONG *pTimeBias,
			       FLOAT *pPercentDeadband, DWORD dwLCID,
			       OPCHANDLE *phServerGroup,
			       REFIID riid, LPUNKNOWN *ppUnk);
      HRESULT internalRemoveGroup(unsigned groupHandleID, int bForce);
     int send_callback(LightOPCGroup *grp, unsigned group_key, loUpdList *upl, int advmask);
        /* unlocks previous lock! */
     LightOPCServer *iam;
};

class LightOPCGroup : public IOPCGroupStateMgt,
		      public IOPCPublicGroupStateMgt,
		      public IOPCItemMgt,
		      public IOPCSyncIO,
                      public IDataObject,
		      public IOPCAsyncIO,
		      public IOPCAsyncIO2,
                      public loConnPnt1
{
    LO_OBJ_XREF;
private:
    int iam_attached(void);
public:
/* Group basic properties & methods */
    unsigned long initphase;
    glGrowingList itl;

    LightOPCServer *owner;
    loWchar *name;

    OPCHANDLE ClientHandle;
    OPCHANDLE ServerHandle;
    loMilliSec UpdateRate;
    FLOAT Deadband;
    LCID  grLCID;
    LONG  TimeBias;

    loMilliSec LastUpdate;

    void /*IOPCDataCallback*/ *conn_databack;
    void /*IAdviseSink*/ *conn_dataonly, *conn_datatime, *conn_writecompl;
    int /*char*/ advise_present, advise_enabled; /* loRQ_CONN_*** */
    int /*BOOL*/ Active;
/* GetEnable() SetEnable() can modify advise_present because
   they are not work if connection was not advised.
   Thus advise_enabled can be eliminated
   but how we'll find connections for explicit Refresh/Read/Write ? */
    int /*BOOL*/ justactuated;
    unsigned active_count;
    loTrid last_trid;

    void clear_advise(void);

/*      lw_mutex lkValues; since Read() must (or must not?) affects
        OnDataChange() behavior we will not change last_sent
        value except from async_pipe */

	LightOPCGroup();
	virtual ~LightOPCGroup();
    inline int iam_valid(void) { return loIS_VALID(this); }
    void attach(LightOPCServer *);
    void detach(LightOPCServer *);
    void clear_all(void);
        int lock_state(const char *msg);
 inline void unlock_state(void)
{ lw_rw_unlock(&owner->lk_remove);
  owner->unlock_state();
// UL_DEBUG((LOGID, "Group:%u(%p) Leave.Ok", ServerHandle, this));
}

	inline void lock_read(void)  { owner->lock_read(); }
	inline void lock_write(void) { owner->lock_write(); }
	inline void unlock(void) { owner->unlock(); }

    inline HRESULT set_name(const loWchar *nname)
          { return loPWstrdup(&name, nname)? E_OUTOFMEMORY: S_OK; }

    void actuate_async(int justActuated);
        /* touch owner->async & set just_activated (clear lasttrid) */
        /* should be called from set_active() & XX_Advise() */

inline LightOPCItem *by_index(unsigned index)
      { return /*index >= 1 &&*/ index-1 < itl.gl_count?
                              (LightOPCItem*)itl.gl_list[index-1]: 0; }
inline unsigned add(LightOPCItem *item) { return gl_insert(&itl, item, 0); }
      int del(unsigned indx);
      void del_all(void);

/* loConnPnt1 methods: */
 HRESULT icp_AddRef(void);
 void    icp_Release(void);
 loObjTracker *icp_ObjTracker(void);

/* IUnknown implementation */
     LONG RefCount;
     STDMETHOD_ (ULONG, AddRef)(void);
     STDMETHOD_ (ULONG, Release)(void);
     STDMETHOD (QueryInterface) (REFIID riid, void **ppv);

     void set_active(BOOL); /* unlocked */
//inline int is_handle(OPCHANDLE hd)
//{ return (unsigned)hd >= 1 && (unsigned)(hd) <= count? S_OK: loOPC_E_INVALIDHANDLE; }

/* IOPCGroupStateMgt implementation */
    STDMETHOD (GetState)(DWORD *pUpdateRate, BOOL *pActive, LPWSTR *ppName,
		                  LONG *pTimeBias, FLOAT *pPercentDeadband, DWORD *pLCID,
		                  OPCHANDLE * phClientGroup, OPCHANDLE * phServerGroup);
    STDMETHOD (SetState)(DWORD *pRequestedUpdateRate, DWORD *pRevisedUpdateRate,
		                  BOOL *pActive, LONG *pTimeBias, FLOAT *pPercentDeadband,
		                  DWORD *pLCID, OPCHANDLE *phClientGroup);
    STDMETHOD (SetName) (LPCWSTR szName);
    STDMETHOD (CloneGroup)(LPCWSTR szName,	REFIID riid, LPUNKNOWN * ppUnk);

/* IOPCPublicGroupStateMgt implementation */
    STDMETHOD (GetState)(BOOL *pPublic);
    STDMETHOD (MoveToPublic)(void);

/* IOPCSyncIO implementation */
    STDMETHOD (Read)(OPCDATASOURCE dwSource, DWORD dwNumItems, OPCHANDLE *phServer,
		         OPCITEMSTATE **ppItemValues, HRESULT **ppErrors);
    STDMETHOD (Write)(DWORD dwNumItems, OPCHANDLE  * phServer, VARIANT *pItemValues,
		          HRESULT **ppErrors);

/* IOPCAsyncIO implementation */
    STDMETHOD (Read)(/* [in] */ DWORD dwConnection,
    		     /* [in] */ OPCDATASOURCE dwSource,
		     /* [in] */ DWORD dwCount,
		    /* [size_is][in] */ OPCHANDLE *phServer,
		    /* [out] */ DWORD __RPC_FAR *pTransactionID,
		    /* [size_is][size_is][out] */ HRESULT **ppErrors);
    STDMETHOD (Write)(/* [in] */ DWORD dwConnection,
            /* [in] */ DWORD dwCount,
            /* [size_is][in] */ OPCHANDLE *phServer,
            /* [size_is][in] */ VARIANT *pItemValues,
            /* [out] */ DWORD __RPC_FAR *pTransactionID,
            /* [size_is][size_is][out] */ HRESULT **ppErrors);
    STDMETHOD (Refresh)(/* [in] */ DWORD dwConnection,
            /* [in] */ OPCDATASOURCE dwSource,
            /* [out] */ DWORD *pTransactionID);
    STDMETHOD (Cancel)(/* [in] */ DWORD dwTransactionID);

/* IOPCAsyncIO2 implementation */
	STDMETHOD (Read) (DWORD dwCount, OPCHANDLE *phServer, DWORD dwTransactionID,
		          DWORD *pdwCancelID, HRESULT **pErrors);
	STDMETHOD (Write)(DWORD dwCount, OPCHANDLE *phServer, VARIANT *pItemValues,
		          DWORD dwTransactionID, DWORD *pdwCancelID, HRESULT **pErrors);
	STDMETHOD (Refresh2)(OPCDATASOURCE dwSource, DWORD dwTransactionID, DWORD *pdwCancelID);
	STDMETHOD (Cancel2)(DWORD pdwCancelID);
	STDMETHOD (SetEnable)(BOOL bEnable);
	STDMETHOD (GetEnable)(BOOL *pbEnable);

/* IOPCItemMgt implementation */
	STDMETHOD (AddItems)(DWORD dwNumItems, OPCITEMDEF *pItemArray, OPCITEMRESULT **ppAddResults,
		             HRESULT **ppErrors);
	STDMETHOD (ValidateItems)(DWORD dwNumItems, OPCITEMDEF *pItemArray,	BOOL bBlobUpdate,
		                  OPCITEMRESULT **ppValidationResults, HRESULT **ppErrors);
	STDMETHOD (RemoveItems)(DWORD dwNumItems, OPCHANDLE  * phServer, HRESULT **ppErrors);
	STDMETHOD (SetActiveState)(DWORD dwNumItems, OPCHANDLE *phServer, BOOL bActive,
		                   HRESULT **ppErrors);
	STDMETHOD (SetClientHandles)(DWORD dwNumItems, OPCHANDLE *phServer, OPCHANDLE *phClient,
		                     HRESULT **ppErrors);
	STDMETHOD (SetDatatypes)(DWORD dwNumItems, OPCHANDLE *phServer, VARTYPE *pRequestedDatatypes,
		                 HRESULT **ppErrors);
 	STDMETHOD (CreateEnumerator)(REFIID riid, LPUNKNOWN *ppUnk);

/* IDataObject implementation */
        STDMETHOD (GetData)(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
        STDMETHOD (GetDataHere)(FORMATETC *pformatetc, STGMEDIUM *pmedium);
        STDMETHOD (QueryGetData)(FORMATETC *pformatetc);
        STDMETHOD (GetCanonicalFormatEtc)(FORMATETC *pformatectIn, FORMATETC *pformatetcOut);
        STDMETHOD (SetData)(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease);
        STDMETHOD (EnumFormatEtc)(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc);
        STDMETHOD (DAdvise)(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink,
                            DWORD *pdwConnection);
        STDMETHOD (DUnadvise)(DWORD dwConnection);
        STDMETHOD (EnumDAdvise)(IEnumSTATDATA **ppenumAdvise);

/* IConnectionPoint methods */
 STDMETHOD (GetConnectionInterface)(IID *pIID);
 STDMETHOD (Advise)(IUnknown *pUnk, DWORD *pdwCookie);
 STDMETHOD (Unadvise)(DWORD dwCookie);

/************************************************************************/

  HRESULT internalValidateItems(DWORD dwCount,
                                OPCITEMDEF *idefs,
                                loTagId *tid, void **acpath,
		                        OPCITEMRESULT **iresults,
                                HRESULT **ppErrors);

        LightOPCGroup *iam;
};


class LightOPCItem
{
     LO_OBJ_XREF;
public:
    loWchar *szItemID;
    loWchar *szAccessPath;
    BOOL bActive;
    OPCHANDLE hClient;
    VARTYPE vtRequestedDataType;
/* note: Changing from inactive to active should cause OnDataChange()
   thus: the quality or the value of last_sent must be updated together bActive */

    loTagId tid;
    loCONV  convtype;
    unsigned *lastsent;
    double   numeric;
    loTrid   last_changed;
 inline void TouchChanged(void) {
#if 0
                                 last_changed = 0;
#endif
                                }
    int      Quality; /* -1 is used as an unusable quality */
    void    *AcPath;

    LightOPCItem();
    virtual ~LightOPCItem ();

    void attach(void *);
    void detach(void *);
    inline HRESULT set_accpath(const loWchar *newpath)
          { return loPWstrdup(&szAccessPath, newpath)? E_OUTOFMEMORY: S_OK; };
    inline HRESULT set_itemid(const loWchar *itemid)
          { return loPWstrdup(&szItemID, itemid)? E_OUTOFMEMORY: S_OK; }
};

unsigned lo_refresh_items(loService *se, loUpdList *upl, LightOPCGroup *grp);
int lo_OnDataChange1(loUpdList *upl, OPCHANDLE hClient, unsigned cform,
                     int with_time, IAdviseSink *ias, HGLOBAL *gmem, unsigned *hint_dsize);
int lo_OnWriteComplete1(loUpdList *upl, OPCHANDLE hClient, unsigned cform,
                        IAdviseSink *ias, HGLOBAL *gmem);
int lo_rio_requests(LightOPCServer *cli);

#endif /*__cplusplus*/
/*********************  end of privopc.h *********************************/
#endif /* PRIVOPC_H */
