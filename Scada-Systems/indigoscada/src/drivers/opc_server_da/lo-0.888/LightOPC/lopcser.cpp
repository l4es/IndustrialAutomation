/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko, Kostya Volovich            *
                                                                          *
 Main part of LightOPCServer
 **************************************************************************/

#include <wchar.h> /* swprintf() */
#include <stdio.h> /* sprintf() */
#include <errno.h>
#include "privopc.h"
//#include "connpnt.h"
#if 0
#include <malloc.h>
#define LO_HEAPCHECK(pp) { int hc; if (_HEAPOK != (hc = _heapchk())) \
        UL_ERROR((LOGID, "heapchk() %d %p %s %d", hc, pp, __FILE__, __LINE__)); }
#else
#define LO_HEAPCHECK(pp)
#endif

static const struct
	{
	 char build[10];
	 char date[32];
	} identify =
#define sh(v,x) ('0' + (v >> x & 15))
#define stri(v) sh(v,28),sh(v,24),sh(v,20),sh(v,16),sh(v,12),sh(v,8),sh(v,4),sh(v,0)
	{ { '\r', '\n', stri(LIGHTOPC_H) }, " " __DATE__ " " __TIME__ " sign @\r\n"	};

/*********************************** LightOPCServer:: stuff **************************/
#define ifATTACHED    (1<< 0)
#define ifLKALL       (1<< 1)
#define ifLKBROWSE    (1<< 2)
#define ifLKASYNC     (1<< 3)
#define ifLKREMOVE    (1<< 4)
#define ifREQQUEUE    (1<< 5)
#define ifRETQUEUE    (1<< 7)
#define ifOBJTRACK    (1<< 8)
//#define ifLKVALUES  (1<<4)
#if ifATTACHED & loDf_BOTHMODEL
#error LightOPCGroup::initphase flags conflict
#endif

int LightOPCServer::lock_state(const char *msg, const IID *riid_nostrict)
{
 LO_HEAPCHECK(this);
 UL_DEBUG((LOGID, "Entering <%s>...", msg? msg: ""));
 if (loIS_VALID(this))
   {
    if (!otrk.ot_stopped || riid_nostrict && (!shuttingdown ||
#if LO_USE_FREEMARSHALL
                    IsEqualIID(*riid_nostrict, IID_IMarshal) ||
#endif
                    IsEqualIID(*riid_nostrict, IID_IUnknown) ) )
      {
       InterlockedIncrement(&RefCount);
       return 0;
      }
   }
 if (riid_nostrict)
   {
    char ifa[40];
    UL_NOTICE((LOG_SR("Server::<%s> interface %s closed: va=%d st=%d sd=%d rc=%d"),
      msg? msg: "", loTranslateIID(ifa, sizeof(ifa), riid_nostrict),
      loIS_VALID(this), otrk.ot_stopped, shuttingdown, RefCount));
   }
 else UL_WARNING((LOG_SR("Server::<%s> interface closed"), msg? msg: ""));
 return -1;
}

static lw_thrrettype run_client_scheduler(void *cli)
{
 UL_NOTICE((LOGID, "client_scheduler(%p) starting...", cli));
 if (cli /*loIS_VALID(cli)*/)
   ((LightOPCServer*)cli)->client_scheduler();
 /* when "self-stopping" is actual loIS_VALID() will return 0 */
 UL_TRACE((LOGID, "client_scheduler(%p) exiting...", cli));

 lw_RETURN;
}

LightOPCServer::LightOPCServer()
{
 iam = 0;
 initphase = 0;
 glGrowList_init(&grl);
 ldFlags = 0;
 access_mode = loAM_RDWR;
 qi_chain = 0;
 se = 0;
 release_handle = 0;
 release_handle_arg = 0;
 unique_GroupName = 0;
 unique_GroupHandle = 0;
 loStringBuf_init(&browsepos);
 serv_next = 0; serv_key = 0;
 shuttingdown = 0;
 otrk.ot_stopped = 0;
#if LO_USE_FREEMARSHALL
 freemarsh = 0;
#endif
#if LO_USE_BOTHMODEL
 ole_initialized = 0;
 lml_rd_lock = lw_rw_timedrdlock;
 lml_wr_lock = lw_rw_timedwrlock;
#endif
 /* IUnknown implementation */
 RefCount = 0;

 /* IOPCCommon implementation */
 client_name[1] = identify.build[8];
 memset(&ctxt, 0, sizeof(ctxt));
/* ctxt.cta.vc_lcid = 0;
   ctxt.cta.vc_tfl = 0; *//* VariantChangeType() flags:
 VARIANT_ALPHABOOL | VARIANT_LOCALBOOL |
 VARIANT_NOUSEROVERRIDE | VARIANT_NOVALUEPROP */
 memset(client_name, 0, sizeof(client_name));
// *client_name = '-';
 sprintf(client_name, "[%03X:%03X]", 0xffff & (unsigned)GetCurrentProcessId(),
                                     0xffff & (unsigned)this);

/* IConnectionPoint methods */
 loShutdownConn_init(&shutdown_conn);

 /* OPCSERVERSTATUS */
 memset(&ostatus, 0, sizeof(ostatus));
 ostatus.dwBandWidth = 0xffffffff;
 bandwidth_time = 0;

 iam = this; /****/
}

int LightOPCServer::initialize(loService *serv, int ld_flags,
                               const loVendorInfo *vi)
{
 int err = 0;

 if (!loIS_VALID(this) || se ||
     ostatus.szVendorInfo || initphase) return EINVAL;
 if (!loSERVICE_OK(serv)) { err = EINVAL; goto Fail; }
 se = serv;
 ctxt.cactx = se->cactx;
// ctxt.cactx.ca_cli = this;

 ldFlags = ld_flags | se->driver.ldFlags;
 if (ld_flags & loDf_FREEMODEL) ldFlags &= ~loDf_BOTHMODEL;
#if LO_USE_BOTHMODEL
 if (ldFlags & loDf_BOTHMODEL)
   {
    lml_rd_lock = lw_rw_timedrdlock_lml;
    lml_wr_lock = lw_rw_timedwrlock_lml;
   }
#endif

 if (vi)
   {
    ostatus.wMajorVersion = vi->lviMajor;
    ostatus.wMinorVersion = vi->lviMinor;
    ostatus.wBuildNumber = vi->lviBuild;
    ostatus.wReserved = vi->lviReserv;
    if (vi->lviInfo &&
        !(ostatus.szVendorInfo = loMWstrdup(vi->lviInfo)))
      { err = ENOMEM; goto Fail; }
   }

 se->driver.ldCurrentTime(&ctxt.cactx, &ostatus.ftStartTime);
 ostatus.dwServerState = OPC_STATUS_NOCONFIG;

 if (err = lw_rwlock_init(&lk_all, 0)) goto Fail;
 initphase |= ifLKALL;

 if (err = lw_rwlock_init(&lk_remove, 0)) goto Fail;
 initphase |= ifLKREMOVE;

 if (err = lw_mutex_init(&lk_browse, 0)) goto Fail;
 initphase |= ifLKBROWSE;

 if (err = loQueueBcast_init(&q_ret, ldFlags & loDf_BOTHMODEL)) goto Fail;
 initphase |= ifRETQUEUE;

 if (err = loQueueAsync_init(&q_req, &async, se->driver.ldQueueMax)) goto Fail;
 initphase |= ifREQQUEUE;

 if (err = loThrControl_init(&async)) goto Fail;
 initphase |= ifLKASYNC;

#if LO_USE_OBJTRACK
 if (err = lw_rmutex_init(&otrk.ot_lk, 0)) goto Fail;
 initphase |= ifOBJTRACK;
#endif

#if LO_USE_FREEMARSHALL
 if (/*se->driver.*/ldFlags & loDf_FREEMARSH)
   CoCreateFreeThreadedMarshaler((IOPCServer*)this, &freemarsh);
 UL_DEBUG((LOGID, "LightOPCServer( free marsh %p)",freemarsh));
 /* there are nothing bad if it fails */
#endif
 /* final step */
// iam = this;
Fail:
 UL_TRACE((LOGID, "%!e LightOPCServer(%ls)", err, loWnul(ostatus.szVendorInfo)));
 return err;
}

void LightOPCServer::selfdestroy(void)
{
#if 0
 void (*rh)(void *, loService *, loClient *) = release_handle; release_handle = 0;
 void  *rha = release_handle_arg; release_handle_arg = 0;
 loService *service = se;
 loClient *client = (loClient*)this;
 delete this;
 if (rh) rh(rha, service, client);  /* When we should call release_handle() ? */
#else
 delete this;
#endif
}

#define loDf_ALL (loDf_DWG | loDf_NOFORCE | loDf_EE_SFALSE |\
                  loDf_FREEMARSH|loDf_BOTHMODEL|loDf_FREEMODEL)

int loClientCreate_agg(loService *se, loClient **cli, 
                   IUnknown *outer, IUnknown **inner,
                   int ldFlags,
                   const loVendorInfo *vi,
                   void (*release_handle)(void *, loService *, loClient *),
                   void *release_handle_arg)
{
 int rv = 0;
 LightOPCServer *srv;

 if (cli) *cli = 0;
 if (inner) *inner = 0;
 if (!cli || (ldFlags & ~loDf_ALL)) return EINVAL;

 if (!loSERVICE_OK(se)) return EBADF;

/******************************************************************/
class LightOPCServer_aggregator: public LightOPCServer
{
public:
  class LightOPCServer_delegator: public IUnknown
  {
  public:
    LightOPCServer *inner;
    STDMETHOD_ (ULONG, AddRef)(void)  { return inner->LightOPCServer::AddRef(); }
    STDMETHOD_ (ULONG, Release)(void) { return inner->LightOPCServer::Release(); }
    STDMETHOD (QueryInterface)(REFIID riid, void **ppv)
         { 
           if (IsEqualIID(riid, IID_IUnknown)) 
             { *ppv = (void*)this; AddRef(); return S_OK; }
           return inner->LightOPCServer::QueryInterface(riid, ppv); 
         }
  } delegator;
  IUnknown *outer;

  STDMETHOD_ (ULONG, AddRef)(void)  { return outer->AddRef(); }
  STDMETHOD_ (ULONG, Release)(void) { return outer->Release(); }
  STDMETHOD (QueryInterface)(REFIID riid, void **ppv)
       { return outer->QueryInterface(riid, ppv); }     

  LightOPCServer_aggregator(IUnknown *Outer) 
    { delegator.inner = this; outer = Outer; }
} *agg = 0;
/******************************************************************/

 if (outer)
   {
    if (!inner) return EINVAL;
    agg = new LightOPCServer_aggregator(outer);
    if (srv = agg) agg->delegator.AddRef();
    else return ENOMEM;
   }
 else if (srv = new LightOPCServer) srv->AddRef();
 else return ENOMEM;
 
 if ((rv = srv->initialize(se, ldFlags, vi)))
   {
    UL_ERROR((LOGID, "%!e loClientCreate()::initialize(%p) FAILED", rv, srv));
   }
 else if ((rv = srv->attach())/* && loIS_VALID(srv)*/)
   {
    UL_ERROR((LOGID, "%!e loClientCreate()::attach(%p) FAILED", rv, srv));
   }
 else
   {
    srv->release_handle = release_handle;
    srv->release_handle_arg = release_handle_arg;
    srv->ctxt.cactx.ca_cli_arg = release_handle_arg;
    srv->ctxt.cactx.ca_cli = srv;
    *cli = srv;
    if (agg) *inner = &agg->delegator;
    return 0; /*** Success ***/
   }
 //srv->Release(); /* Release doesn't check for initialize() faults*/
 delete srv;
 return rv;
}

int loClientCreate(loService *se, loClient **cli, int ldFlags,
                   const loVendorInfo *vi,
                   void (*release_handle)(void *, loService *, loClient *),
                   void *release_handle_arg)
{
  return loClientCreate_agg(se, cli, NULL, NULL, 
                            ldFlags, vi, 
                            release_handle, release_handle_arg);
}

int loClientChain(loClient *cli,
                  HRESULT (*qi_chain)(void *rha, loService *, loClient *,
                                      const IID *, LPVOID *),
                  void (*release_handle)(void *rha, loService *, loClient *),
                  void *release_handle_arg)
{
 if (!loIS_VALID(cli)) return EBADF;

 cli->qi_chain = qi_chain;
 cli->ctxt.cactx.ca_cli_arg = release_handle_arg;
 cli->release_handle_arg = release_handle_arg;
 cli->release_handle = release_handle;
 return 0;
}

int loClientAccessMode(loService *se, loClient *cli, int accmode)
{
 int rv = EBADF;

 if (accmode != loAM_RDWR &&
    (accmode & ~(loAM_RDONLY_OP | loAM_RDONLY_ADD | loAM_RDONLY_BROWSE |
                 loAM_NOREAD_DEV | loAM_ERREAD_DEV))) return EINVAL;
 if (loSERVICE_OK(se))
   {
    lw_mutex_lock(&se->lkList);
    if (!se->shutdown)
      {
       LightOPCServer *los = (LightOPCServer*)se->servlist;
       for(rv = ENOENT; los; los = los->serv_next)
         if (!cli || los == cli)
           {
            los->access_mode = accmode;
            rv = 0;
           }
      }
    lw_mutex_unlock(&se->lkList);
   }
 return rv;
}

LightOPCServer::~LightOPCServer()
{
 iam = 0;

 if (!otrk.ot_stopped) otrk.ot_stopped = 1;
 UL_TRACE((LOG_SR("~LightOPCServer(%ls)"), loWnul(ostatus.szVendorInfo)));
 LO_HEAPCHECK(this);

 if (initphase & ifLKASYNC)
   {
    loThrControl_stop(&async);
    loQueueBcast_abort(&q_ret);
    loQueueAsync_clear(&q_req);
   }

 if (grl.gl_count)
   UL_WARNING((LOG_SR("Still unreleased %d groups"), grl.gl_count));

 if (shutdown_conn.request)
   UL_WARNING((LOG_SR("Unsafe Shutdown notification aborted")));
// loShutdownConn_call(&shutdown_conn);
 loShutdownConn_clear(&shutdown_conn);

 if (initphase & ifATTACHED)
   {
    loQueueBcast_abort(&q_ret);
    /* clear_all(); - performed in detach() */
    detach(); initphase &= ~ifATTACHED;
   }

 if ((initphase & ifOBJTRACK) && otrk.ot_disconnect_all(1))
   UL_WARNING((LOG_SR("Some Enumerators left unreleased")));

#if LO_USE_FREEMARSHALL
 if (freemarsh) { freemarsh->Release(); freemarsh = 0; }
#endif

 if (initphase & ifRETQUEUE)
   {
    loQueueBcast_destroy(&q_ret); initphase &= ~ifRETQUEUE;
   }
 if (initphase & ifREQQUEUE)
   {
    loQueueAsync_destroy(&q_req); initphase &= ~ifREQQUEUE;
   }

 if (initphase & ifLKASYNC)
   {
    loThrControl_destroy(&async); initphase &= ~ifLKASYNC;
   }

 if (initphase & ifLKALL)
   {
    lw_rwlock_destroy(&lk_all); initphase &= ~ifLKALL;
   }
 if (initphase & ifLKREMOVE)
   {
    lw_rwlock_destroy(&lk_remove); initphase &= ~ifLKREMOVE;
   }
 if (initphase & ifLKBROWSE)
   {
    lw_mutex_destroy(&lk_browse); initphase &= ~ifLKBROWSE;
   }

#if LO_USE_OBJTRACK
 if (initphase & ifOBJTRACK)
   {
    lw_rmutex_destroy(&otrk.ot_lk); initphase &= ~ifOBJTRACK;
   }
#endif

 del_all();

 loStringBuf_clear(&browsepos);

 if (ostatus.szVendorInfo) freeX(ostatus.szVendorInfo), ostatus.szVendorInfo = 0;
#if LO_USE_BOTHMODEL
 if (ole_initialized)
   UL_ERROR((LOGID, "OLE didn't uninitialized!"));
#endif
 if (initphase)
   UL_ERROR((LOGID, "Left unreleased: 0x%X", initphase));

 loShutdownConn_clear(&shutdown_conn);
 glGrowList_clear(&grl);

 if (release_handle) release_handle(release_handle_arg, se, (loClient*)this),
                     release_handle = 0;
 LO_HEAPCHECK(this);
}

void LightOPCServer::clear_all(void)
{
// if (initphase & ifLKBROWSE)
   {
    lw_mutex_lock(&lk_browse);
    loStringBuf_clear(&browsepos);
    lw_mutex_unlock(&lk_browse);
   }
// else if (br_position) freeX(br_position), br_position = 0, br_size = 0;
 loQueueAsync_clear(&q_req);
 loQueueBcast_abort(&q_ret);
// if (initphase & ifLKALL)
   {
    WR_lock(&lk_remove);
     lock_write();//lw_rw_wrlock(&lk_all);
      del_all(ldFlags & loDf_DWG);
     unlock();//lw_rw_unlock(&lk_all);
    lw_rw_unlock(&lk_remove);
   }
// else del_all(initphase & ifFORCEDELETE);
}

void LightOPCServer::set_state(int oper, int state, const loWchar *reason)
{
 int ost = 0xeee;

 if (loIS_VALID(this))
   {
    UL_TRACE((LOG_SR("Server::set_state(%X/%d) ..."), oper, state));

/* DISCONNECT is possible whenever async is `outof01` but
   in this case it could be done by sheduler.
   here we warrant all possible disconnects will be done before
   loSetStete() returned. */
       if (oper & loOP_DISCONNECT)
         {
          unsigned gcount;
          IUnknown *grp, **grpl;
          UL_TRACE((LOG_SR("Server::DISCONNECT...")));
          otrk.ot_disconnect_all(0);
          lock_read();
          grpl = (IUnknown**)grl.gl_list;
          for(gcount = grl.gl_count; gcount--;)
            if (grp = grpl[gcount]) LO_CO_DISCONNECT(grp, 0);
          LO_CO_DISCONNECT((IOPCServer*)this, 0);
          unlock();
          UL_TRACE((LOG_SR("Server::DISCONNECT Enums...")));
          otrk.ot_disconnect_all(0);
         }

    lw_mutex_lock(&async.lk);
    if (!loThrControl_outof01(&async))
      {
       ost = otrk.ot_stopped;

       if (oper & loOP_OPERATE)
         {
          if (otrk.ot_stopped && !(oper & loOP_STOP)) otrk.ot_stopped = 0;
          if (shuttingdown && !(oper & loOP_SHUTDOWN)) shuttingdown = 0;
         }

       if ((oper & loOP_SHUTDOWN) &&
          !shuttingdown) shuttingdown = 1;

       if ((oper & (loOP_STOP | loOP_DESTROY)) &&
          !otrk.ot_stopped) otrk.ot_stopped = 1;

       if (0 != state) ostatus.dwServerState = (OPCSERVERSTATE)state;
       if (0 != reason) loPWstrdup(&shutdown_conn.reason, reason);

       if (oper & loOP_DESTROY)
         {
          async.tstate = 2;
          lw_conds_signal(&async.cond);
         }
       else if (ost != otrk.ot_stopped || shuttingdown)
         {
          if (0 == async.tstate) async.tstate = 1;
          lw_conds_signal(&async.cond);
         }
      } /* if (!loThControl_outof01( */
    ost = otrk.ot_stopped << 8 | shuttingdown << 4 | async.tstate;
    lw_mutex_unlock(&async.lk);    
   }
 UL_TRACE((LOG_SR("Server::set_state(%X/%d) finished =%03x"), oper, state, ost));
}

int LightOPCServer::attach(void)
{
 int err = EBADF;
 UL_TRACE((LOG_SR("Server::attach() ...")));
 if (loIS_VALID(this) && loIS_VALID(se))
   {
    lw_mutex_lock(&se->lkList);
    if (!se->shutdown)
      {
       loClient *srv;
       unsigned serv_id = se->serv_key;
NewKey:
       if (0 == ++serv_id) ++serv_id;
       srv = se->servlist;
       while(srv)
         if (srv->serv_key != serv_id) srv = srv->serv_next;
         else goto NewKey;

       se->serv_key = serv_key = serv_id;
       serv_next = se->servlist;
       se->servlist = this;
       initphase |= ifATTACHED;
       err = 0;
      }
    else err = ENOENT;
    lw_mutex_unlock(&se->lkList);

    if (0 == err)
      err = loThrControl_start(&async, 1, run_client_scheduler, this);
   }

 UL_NOTICE((LOG_SR("Server::attach() %s"), err? "FAILED": "Ok"));
 return err;
}

int LightOPCServer::detach(void)
{
 int godown = 0;
 UL_TRACE((LOG_SR("Server::detach() ...")));
/*
 LO_CO_DISCONNECT((IOPCServer*)this, 0);
*/
 clear_all();
 UL_TRACE((LOG_SR("Server::detach() continued...")));

 if ((initphase & ifATTACHED) && loIS_VALID(se))
   {
    LightOPCServer **les;
    lw_mutex_lock(&se->lkList);
    les = &se->servlist;
    while(*les)
      if (*les != this) les = &(*les)->serv_next;
      else
        {
         *les = serv_next; serv_next = 0;
         godown = (se->shutdown && !se->servlist)? -1: 1;
//         break;
        }
    if (godown) initphase &= ~ifATTACHED;
    lw_mutex_unlock(&se->lkList);
    if (0 > godown)
      {
       UL_NOTICE((LOG_SR("going down")));
       loInternalServiceDestroy(se);
      }
   }

 clear_all();
 UL_NOTICE((LOG_SR("Server::detach() finished %d"), godown));
 return godown;
}


HRESULT LightOPCServer::mk_unique(loWchar buf[loUNIQUE_LEN], const void *ptr)
{
 int tc;

 if (!buf) return E_FAIL;
 for(tc = 0; tc < 1000; tc++)
   {
     /*InterlockedIncrement()*/++unique_GroupName;
    swprintf(buf, L"G-%03lu.%lx", unique_GroupName,
                                  0xfffff & (((unsigned long)ptr) >> 5));
/* the value may be not same as result of InterlockedIncrement(),
   but don't worried about */
    if (!by_name(buf)) return S_OK;
   }
 UL_WARNING((LOG_SR("Can not make unique name %lu"), unique_GroupName));
 return E_FAIL;
}

unsigned LightOPCServer::by_handle(OPCHANDLE sgh)
{
 LightOPCGroup *grp;
 unsigned uu = grl.gl_count;
 while(uu--)
   if ((grp = (LightOPCGroup*)grl.gl_list[uu]) && grp->ServerHandle == sgh)
     return uu + 1;
 return 0;
}

LightOPCGroup *LightOPCServer::by_handle_g(unsigned sgh)
{
 LightOPCGroup *grp;
 unsigned uu = grl.gl_count;
 while(uu--)
   if ((grp = (LightOPCGroup*)grl.gl_list[uu]) && grp->ServerHandle == sgh)
     return grp;
 return 0;
}

LightOPCGroup *LightOPCServer::by_name(const loWchar *name)
{
 if (name)
   {
    LightOPCGroup *grp, **groups = (LightOPCGroup **)grl.gl_list;
    unsigned uu = grl.gl_count;
    while(uu--)
      if ((grp = *groups++) && grp->name &&
          !wcscmp(name, grp->name)) return grp;
   }
 return 0;
}

unsigned LightOPCServer::add(LightOPCGroup *grp)
{
 unsigned slot;

 if (!grp) return 0;
 if (slot = gl_insert(&grl, grp, 0))
   {
    grp->AddRef();
    grp->attach(this);
   }
 return slot;
}

int LightOPCServer::del(unsigned index, int force)
{
 int rv = -1;
 LightOPCGroup *grp;
 if (grp = (LightOPCGroup*)gl_remove(&grl, index))
   {
    rv = (1 != grp->RefCount);
    grp->detach(this);
    if (force) 
      { 
       LO_CO_DISCONNECT((IOPCGroupStateMgt*)grp, 0);
       delete grp; 
       rv = 0; 
      }
    else 
      {
       rv /*|*/= (0 != grp->Release());
      }
   }
 return rv;
}

void LightOPCServer::del_all(int force)
{
 unsigned uu;
 for(uu = grl.gl_count; uu > 0; uu--) del(uu, force);
 glGrowList_clear(&grl);
}

/*********************************** LightOPCGroup:: stuff **************************/

int LightOPCGroup::iam_attached(void) // for logging only
{
 return loIS_VALID(this) && (initphase & ifATTACHED) && loIS_VALID(owner);
}


int LightOPCGroup::lock_state(const char *msg)
{
 LightOPCServer *own = owner;
 lw_rwlock *removelock = &own->lk_remove;
// UL_DEBUG((LOGID, "Group:%u(%p) <%s> Entering...", ServerHandle, this, msg? msg: ""));
 if (loIS_VALID(this) && (initphase & ifATTACHED) /*&&
     owner */ && !own->lock_state(msg, 0))
   {
    if (0 == own->RD_lock(removelock))
      {
       if (loIS_VALID(this) && (initphase & ifATTACHED))
         {
          return 0;
         }
       lw_rw_unlock(removelock);
      }
    own->unlock_state();
   }
 UL_WARNING((LOGID, "Group:%u(%p) <%s> interface closed",
            ServerHandle, this, msg? msg: ""));
 return -1;
}

#if 0
void LightOPCGroup::unlock_state(void)
{
 if (!(initphase & ifATTACHED) && (gl_count || advise_present))
   {
    lock_write();
    clear_all();
    unlock();
   }
 lw_rw_unlock(&owner->lk_remove);
 owner->unlock_state();
// UL_DEBUG((LOGID, "Group:%u(%p) Leave.Ok", ServerHandle, this));
}
#endif

LightOPCGroup::LightOPCGroup()
{
 /* IUnknown implementation */
 initphase = 0;
 glGrowList_init(&itl);
 RefCount = 0;
 owner = 0;
 name = 0;

 ClientHandle = ServerHandle = grLCID = TimeBias = 0;
 Active = 0;
 justactuated = 0;
 Deadband = 0.;
 UpdateRate = LastUpdate = 0;

 conn_databack = 0;
 conn_dataonly = conn_datatime = conn_writecompl = 0;
 advise_present = 0;
 advise_enabled = loRQ_CONN_DATA;
 active_count = 0;
 last_trid = 0;
#if LO_USE_FREEMARSHALL
 freemarsh = 0;
#endif
 iam = this;
}

LightOPCGroup::~LightOPCGroup()
{
 iam = 0;
 initphase &= ~ifATTACHED;
 UL_TRACE((LOGID, "~LightOPCGroup(%ls)", loWnul(name)));
 LO_HEAPCHECK(this);
/*
 LO_CO_DISCONNECT((IOPCGroupStateMgt*)this, 0);
*/
 if (name) { freeX(name); name = 0; }
 clear_all();
#if LO_USE_FREEMARSHALL
 if (freemarsh) { freemarsh->Release(); freemarsh = 0; }
#endif
 initphase = 0;
 glGrowList_clear(&itl);
 LO_HEAPCHECK(this);
}

void LightOPCGroup::clear_advise(void)
{
 UL_DEBUG((LOGID, "LightOPCGroup::clear_advise(%ls)", loWnul(name)));
 advise_present = 0;
 if (conn_databack)   loRELEASE_IFACE(conn_databack, IOPCDataCallback, initphase);
 if (conn_dataonly)   loRELEASE_IFACE(conn_dataonly, IAdviseSink, initphase);
 if (conn_datatime)   loRELEASE_IFACE(conn_datatime, IAdviseSink, initphase);
 if (conn_writecompl) loRELEASE_IFACE(conn_writecompl, IAdviseSink, initphase);
}

void LightOPCGroup::clear_all(void)
{
 del_all();
 active_count = 0;
 if (advise_present) clear_advise();

 /* delete anything else here */
}

void LightOPCGroup::actuate_async(int justActuated) /* touch owner->async & set just_activated */
{
 if (Active && (advise_present & advise_enabled))
   {
    if (justActuated) justactuated = 1, last_trid = 0;
    owner->touch_scheduler();
    UL_NOTICE((LOG_GRHO("group::actuate(%ls)"), loWnul(name)));
   }
}

void LightOPCGroup::attach(LightOPCServer *own)
{
 if (owner) UL_ERROR((LOG_GRHO("group::attach(%ls) BAD OWNER"), loWnul(name)));
 if (owner = own)
   {
    initphase = ifATTACHED | own->ldFlags & loDf_BOTHMODEL;
#if LO_USE_FREEMARSHALL
    if (own->/*se->driver.*/ldFlags & loDf_FREEMARSH)
      CoCreateFreeThreadedMarshaler((IOPCGroupStateMgt*)this, &freemarsh);
     /* there are nothing bad if it fails */
#endif
   }
}

void LightOPCGroup::detach(LightOPCServer *own)
{
 if (own == owner)
   UL_TRACE((LOG_GRHO("group::detach(%ls)"), loWnul(name)));
 else UL_ERROR((LOG_GRHO("group::detach(%ls) BAD OWNER"), loWnul(name)));
// LO_CO_DISCONNECT((IOPCGroupStateMgt*)this, 0);
 if (initphase & ifATTACHED)
   {
    set_active(0);
    initphase &= ~ifATTACHED;
   }
 clear_all();
}

int LightOPCGroup::del(unsigned indx)
{
 LightOPCItem *item;
 if (item = (LightOPCItem*)gl_remove(&itl, indx))
   { delete item; return 0; }
 return -1;
}

void LightOPCGroup::del_all(void)
{
 unsigned uu;
 for(uu = itl.gl_count; uu > 0; uu--) del(uu);
 glGrowList_clear(&itl);
}

/*********************** LightOPCGroup::IConnectionPoint stuff **********************/

HRESULT LightOPCGroup::icp_AddRef(void)
{
 if (!AddRef()) return LO_E_SHUTDOWN;
 LO_CHECK_STATEz0("Group::IConnPoint::AddRef");
 return S_OK; }
}

void LightOPCGroup::icp_Release(void)
{
 { LO_FINISH();
 Release();
}

loObjTracker *LightOPCGroup::icp_ObjTracker(void)
{
  return owner? &owner->otrk: 0;
}
/*********************************** LightOPCItem:: stuff **************************/

LightOPCItem::LightOPCItem()
{
 szItemID = 0;
 szAccessPath = 0;
 bActive = 0;
 tid = 0;
 convtype = loCONV_CHANGE;
/* hClient;*/
 vtRequestedDataType = VT_EMPTY;

 Quality = -1;
 AcPath = 0;
 last_changed = 0;
 numeric = 0;
 lastsent = 0;
}

LightOPCItem::~LightOPCItem()
{
 if (szItemID) { freeX(szItemID); szItemID = 0; }
 if (szAccessPath) { freeX(szAccessPath); szAccessPath = 0; }
 if (lastsent) { freeX(lastsent); lastsent = 0; }
}



/* end of lopcser.cpp */
