/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 Timofei Bondarenko                                *
                                                                          *
 Sample server

 NOTE: the sample contains code for both in-proc and out-of-proc 
  servers. Therefore here is some overhead due this.

 NOTE: the exe version of sample can be built for CONSOLE or WINDOWS mode.
 see linker options for subsystem/entrypoint.
 In console mode it will print messages to console window, when invoked
 from command line, and can be terminated by Ctrl^C/Ctrl^Break.
 On another hand, it will display a console window for a short time when
 invoked from a windowed application.
 **************************************************************************/

#define _WIN32_DCOM
#include <windows.h>
#include "unilog.h"
#define LOGID log,0
#include <ole2.h>
#include <olectl.h>
#include <oleauto.h>
#include <process.h>
#include <stdio.h>
#include <errno.h>
#include <locale.h>
#include <opcda.h>
#include <opcerror.h>
#include "lightopc.h"

#if 0
static LONG server_process;
#define CoAddRefServerProcess()  (InterlockedIncrement(&server_process))
#define CoReleaseServerProcess() (InterlockedDecrement(&server_process))
#define CoQueryClientBlanket(a,b,c,d,e,userid,f) (*((const wchar_t**)userid)=L"Wine!")
#endif

unilog *log;                    /* logging entry */
int use_console = 0;
int test_mode = 0;

/********************* OPC vendor info ***************************************/
static const loVendorInfo vendor = {
  3 /*Major */ , 2 /*Minor */ , 1 /*Build */ , 0 /*Reserv */ ,
  "Sample OPC Server #9"
};

loService *my_service;
static int driver_init(int lflags);
static void driver_destroy(void);
static void simulate(unsigned pause);

/* OLE-specefic data: ***********************************************************/

// {C896CBD0-ABF5-11d4-BED0-00002120DB5C}   inproc-server
static const GUID CLSID_LightOPCServerDLL =
  { 0xc896cbd0, 0xabf5, 0x11d4, {0xbe, 0xd0, 0x0, 0x0, 0x21, 0x20, 0xdb,
                                 0x5c} };

// {4EA2713D-CA07-11d4-BEF5-00002120DB5C}   exe-server
static const GUID CLSID_LightOPCServerEXE =
  { 0x4ea2713d, 0xca07, 0x11d4, {0xbe, 0xf5, 0x0, 0x0, 0x21, 0x20, 0xdb,
                                 0x5c} };

/**** Server Counting stuff & OLE ICF implementation *****************************
  The IClassFactory is unavoidable evil. Feel free to go ahead.
  Basically we've to unload when the server_count being zero.
  But there are different techniques for in-/out-of- proc servers.
*/

class myClassFactory: public IClassFactory
{
public:
 int  is_out_of_proc, 
      server_inuse; /* go 0 when unloading initiated */
 LONG server_count;
 CRITICAL_SECTION lk_count;  /* protect server_count */

 myClassFactory(): is_out_of_proc(0), server_inuse(0), server_count(0)
   {
      InitializeCriticalSection(&lk_count);
   }
 ~myClassFactory()
   {
      DeleteCriticalSection(&lk_count);
   }

  void serverAdd(void);
  void serverRemove(void);

/* Do nothing: we're static, he-he */  
  STDMETHODIMP_(ULONG) AddRef(void) { return 1; }
  STDMETHODIMP_(ULONG) Release(void) { return 1; }

  STDMETHODIMP QueryInterface(REFIID iid, LPVOID *ppInterface)
    {
      if (ppInterface == NULL)
        return E_INVALIDARG;
      if (iid == IID_IUnknown || iid == IID_IClassFactory)
        {
          UL_DEBUG((LOGID, "myClassFactory::QueryInterface() Ok"));
          *ppInterface = this;
          AddRef();
          return S_OK;
        }
      UL_DEBUG((LOGID, "myClassFactory::QueryInterface() Failed"));
      *ppInterface = NULL;
      return E_NOINTERFACE;
    }

  STDMETHODIMP LockServer(BOOL fLock)
    {
      UL_DEBUG((LOGID, "myClassFactory::LockServer(%d)", fLock));
      if (fLock) serverAdd();
      else serverRemove();
      return S_OK;
    }

  STDMETHODIMP CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid,
                              LPVOID *ppvObject);

};

static myClassFactory my_CF;

void myClassFactory::serverAdd(void)
{
  EnterCriticalSection(&lk_count);
  if (is_out_of_proc) CoAddRefServerProcess();  
  ++server_count;
  LeaveCriticalSection(&lk_count);
}

void myClassFactory::serverRemove(void)
{
  EnterCriticalSection(&lk_count);
  if (is_out_of_proc)
    {
     if (0 == CoReleaseServerProcess())
       server_inuse = 0;
    }
  if (0 == --server_count && server_inuse) server_inuse = 0;
  LeaveCriticalSection(&lk_count);
}

static void a_server_finished(void *arg, loService *b, loClient *c)
{
  /* ARG is the same as we've passed to loClientCreate() */
  UL_DEBUG((LOGID, "a_server_finished(%lu)...", my_CF.server_count));
  my_CF.serverRemove();

/* OPTIONAL: */
  UL_INFO((LOGID, "a_server_finished(%lu) USERID=<%ls>",
    my_CF.server_count, arg? arg: L"<null>"));
  if (use_console)
    printf("-Detaching a client. USERID: <%ls>\n", arg? arg: L"<null>");
}

static void dll_simulator_wait(void); /* in-proc specefic */
static void dll_simulator_start(void); /* in-proc specefic */

STDMETHODIMP myClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid,
                                            LPVOID *ppvObject)
{
  IUnknown *server = 0;
  HRESULT hr = S_OK;

/* OPTIONAL: security stuff */
  OLECHAR *userid = 0;
  wchar_t *cuserid;
  LONG userno;
  static LONG usercount;

  userno = InterlockedIncrement(&usercount);

  CoQueryClientBlanket(0, 0, 0, 0, 0, (RPC_AUTHZ_HANDLE*)&userid, 0);
  if (!userid) userid = L"{unknown}";

  UL_WARNING((LOGID, "USER:#%ld <%ls>", userno, userid));
  if (use_console)
    printf("+Attaching a client #%ld. USERID: <%ls>\n", usercount, userid);

  if (cuserid = (wchar_t*)malloc((wcslen(userid) + 16) * sizeof(wchar_t)))
    swprintf(cuserid, L"#%ld %ls", userno, userid);
/* -- end of security stuff */

  if (pUnkOuter)
    {
#if 1 /* Do we support aggregation? */
     if (riid != IID_IUnknown) 
#endif
          return CLASS_E_NOAGGREGATION;
    }

  serverAdd();  /* the lock for a_server_finished() */

/***********************************************
 * check other conditions (i.e. security) here *
 ***********************************************/

  if (0 == server_inuse) /* we're stopped */
    {                    /* stopping initiated when there are no active instances */
      if (is_out_of_proc) 
        {                /* a stopped EXE should exit soon and can't work anymore */
          UL_MESSAGE((LOGID, "myClassFactory:: Server already finished"));
          serverRemove();
          return E_FAIL;
        }
      else /* in-proc specefic here: */
        {  /* restart the server if not already running */
          dll_simulator_wait();   /* wait for stopping complete */
          dll_simulator_start();  /* then restart */
          if (0 == server_inuse)  
            {
              UL_MESSAGE((LOGID, "myClassFactory:: Can't start server"));
              serverRemove();
              return E_FAIL;
            }
        }
    }
{
 IUnknown *inner = 0;
 if (loClientCreate_agg(my_service, (loClient**)&server, 
                       pUnkOuter, &inner,
                       0, &vendor, a_server_finished, cuserid/*this*/))
    {
      serverRemove();
      hr = E_OUTOFMEMORY;
      UL_MESSAGE((LOGID, "myClassFactory::loClientCreate_agg() failed"));
    }
  else if (pUnkOuter) *ppvObject = (void*)inner; /*aggregation requested*/
  else /* no aggregation */
    {
/* loClientCreate(my_service, (loClient**)&server, 
                  0, &vendor, a_server_finished, cuserid) - with no aggregation */
      /* Initally the created SERVER has RefCount=1 */
      hr = server->QueryInterface(riid, ppvObject); /* Then 2 (if success) */
      server->Release(); /* Then 1 (on behalf of client) or 0 (if QI failed) */
      if (FAILED(hr)) 
        UL_MESSAGE((LOGID, "myClassFactory::loClient QueryInterface() failed"));
      /* So we shouldn't carry about SERVER destruction at this point */
    }
}
  if (SUCCEEDED(hr))
    {
      loSetState(my_service, (loClient*)server,
             loOP_OPERATE, (int)OPC_STATUS_RUNNING, /* other states are possible */
             "Finished by client");
      UL_DEBUG((LOGID, "myClassFactory::server_count = %ld", server_count));
    }

  return hr;
}

/************************* The END of OLE-specific ***************************/

/****************** The Process Data to be exported via OPC ******************
 (driver's internal representation) */

static CRITICAL_SECTION lk_values;     /* protects ti[] from simultaneous 
                                    access by simulator() and WriteTags() */
/* Our data tags: */
/* zero is resierved for an invalid RealTag */
#define TI_zuzu      (1)
#define TI_lulu      (2)
#define TI_bandwidth (3)
#define TI_array     (4)
#define TI_enum      (5)
#define TI_quiet     (6)
#define TI_quality   (7)
#define TI_string    (8)
#define TI_MAX       (8)

static loTagId ti[TI_MAX + 1];          /* their IDs */
static const char *tn[TI_MAX + 1] =     /* their names */
{ "--not--used--", "zuzu", "lulu", "bandwidth", "array", "enum-localizable",
    "quiet", "quality", "string" };
static loTagValue tv[TI_MAX + 1];       /* their values */


/**********************************************************************
 sample server initiation & simulation
 **********************************************************************/

/* OPTIONAL: show client's interests */

void activation_monitor(const loCaller *ca, int count, loTagPair *til)
{
  int act = 1;
  if (0 > count)
    act = 0, count = -count;
  while(count--)
    {
      UL_DEBUG((LOGID, "MON: %u %s %s", til[count].tpTi,
                tn[(int) til[count].tpRt], act ? "On" : "Off"));
    }
}


/* OPTIONAL: write back to the device */

int WriteTags(const loCaller *ca,
              unsigned count, loTagPair taglist[],
              VARIANT values[], HRESULT error[], HRESULT *master, LCID lcid)
{
  unsigned ii;
  UL_TRACE((LOGID, "WriteTags(%x) invoked"));

  EnterCriticalSection(&lk_values);

  for(ii = 0; ii < count; ii++)
    {
      HRESULT hr = S_OK;
      loTagId clean = 0;

      switch((int)taglist[ii].tpRt)
        {
      case TI_zuzu:
        { double oldval = tv[TI_zuzu].tvValue.dblVal;
        hr = VariantChangeType(&tv[TI_zuzu].tvValue,
                               &values[ii], 0, V_VT(&tv[TI_zuzu].tvValue));
        UL_INFO((LOGID, "%p:%s:WriteTags(zuzu) = %.1f old=%.1f [%x]", 
            ca->ca_cli, loClientName(ca->ca_cli),
          tv[TI_zuzu].tvValue.dblVal, oldval, hr));        
        if (tv[TI_zuzu].tvValue.dblVal < 0)
          hr = DISP_E_TYPEMISMATCH; /* simulate a write error */
        }
        break;

      case TI_lulu:
        hr = VariantChangeType(&tv[TI_lulu].tvValue, &values[ii], 0, VT_I2);
        if (S_OK == hr)
          lo_statperiod(V_I2(&tv[TI_lulu].tvValue)); /* VERY OPTIONAL, really */
          /* The splining factor for bandwidth calculations/trending */
        break;

      case TI_quiet:
        hr = VariantChangeType(&tv[TI_quiet].tvValue, &values[ii], 0, 
                          V_VT(&tv[TI_quiet].tvValue));
        break;

      case TI_array:
        hr = lo_variant_changetype_array(&tv[TI_array].tvValue,
                                 &values[ii], lcid, 0, 
                                 V_VT(&tv[TI_array].tvValue));
        break;

      case 0:  /* ignore */
      default: /* preserve unhandled tags */
        clean = taglist[ii].tpTi;
        break;
        }
      if (S_OK != hr) 
        {
          *master = S_FALSE;
          error[ii] = hr;

          UL_TRACE((LOGID, "%!l WriteTags(Rt=%u Ti=%u %s)", 
              hr, taglist[ii].tpRt, taglist[ii].tpTi, 
              tn[(int)taglist[ii].tpRt <= TI_MAX? (int)taglist[ii].tpRt: 0]));
        }
      taglist[ii].tpTi = clean; /* clean if ok */
    }

  LeaveCriticalSection(&lk_values);

  return loDW_TOCACHE; /* put to the cache all tags unhandled here */
   // loDW_ALLDONE; 
}

/* OPTIONAL: example of non-trivial datatype conversion */

static void local_text(WCHAR buf[32], unsigned nn, LCID lcid);

void ConvertTags(const loCaller *ca,
                 unsigned count, const loTagPair taglist[],
                 VARIANT *values, WORD *qualities, HRESULT *errs,
                 HRESULT *master_err, HRESULT *master_qual,
                 const VARIANT src[], const VARTYPE vtypes[], LCID lcid)
{
  unsigned ii;

  for(ii = 0; ii < count; ii++)
    {
      HRESULT hr = S_OK;
      VARTYPE reqtype = vtypes[ii];
      if (reqtype == VT_EMPTY) reqtype = V_VT(&src[ii]);

      switch((int)taglist[ii].tpRt)
        {
      case 0: /* ignore */
        break;

      case TI_array:
        if (V_VT(&src[ii]) == reqtype)
          {
            hr = values == src? S_OK : VariantCopy(&values[ii], (VARIANT*)&src[ii]);
          }
        else 
          { 
            hr = lo_variant_changetype_array(&values[ii], 
                    (VARIANT*)&src[ii], lcid, 0, reqtype);
          }
        break;

      case TI_enum:
        if (V_VT(&src[ii]) == VT_I2 && reqtype == VT_BSTR)
          {
            WCHAR ww[32];
            int vv = V_I2(&src[ii]);
            VariantClear(&values[ii]);
            local_text(ww, vv, lcid);
            if (V_BSTR(&values[ii]) = SysAllocString(ww))
              V_VT(&values[ii]) = VT_BSTR;
            else hr = E_OUTOFMEMORY;
            break;
          }

      default:
        if (reqtype == V_VT(&src[ii]))
          {
            if (values == src) hr = S_OK;
            else hr = VariantCopy(values + ii, (VARIANT*)&src[ii]);
          }
        else
          hr = VariantChangeType(values + ii, (VARIANT*)&src[ii], 0, reqtype);

        UL_ERROR((LOGID, "ConvII NotRequested(%u %u)[%u/%u]", 
          taglist[ii].tpRt, taglist[ii].tpTi, ii, count));
        break;
        } /* end of switch( */

      if (S_OK != hr)
        {
          errs[ii] = hr;
          qualities[ii] = OPC_QUALITY_BAD;
          *master_err = *master_qual = S_FALSE;

          UL_WARNING((LOGID, "%!l ConvII Error(Rt=%u Ti=%u %s)[%u/%u]", 
              hr, taglist[ii].tpRt, taglist[ii].tpTi, 
              tn[(int)taglist[ii].tpRt <= TI_MAX? (int)taglist[ii].tpRt: 0],
              ii, count));
        }
    } /* end of for(...*/
}

/* OPTIONAL: example of DS_DEVICE read(). It enforces significant delay, 
  sufficient for view request's queueing and test the cancellation. */

loTrid ReadTags(const loCaller *ca,
                unsigned count, loTagPair taglist[],
                VARIANT *values, WORD *qualities,
                FILETIME *stamps, HRESULT *errs,
                HRESULT *master_err, HRESULT *master_qual,
                const VARTYPE vtypes[], LCID lcid)
{
  UL_TRACE((LOGID, "ReadTags( ) invoked"));
  Sleep(33333);                 /* just for Cancel() testing */
  return loDR_CACHED; /* perform actual reading from cache */
}

/* OPTIONAL: example of dynamic tag creation */

HRESULT AskItemID(const loCaller *ca, loTagId *ti, 
                  void **acpa, const loWchar *itemid, 
                  const loWchar *accpath, int vartype, int goal)  /* Dynamic tags */
{
  HRESULT hr = S_OK;
  VARIANT var;
  VariantInit(&var);  
  V_R4(&var) = 3;     /* God likes 3 */
  V_VT(&var) = VT_R4;

  UL_NOTICE((LOGID, "AskItemID %ls type = %u(0x%x)", itemid, vartype, vartype));

  if (VT_EMPTY != vartype) /* check conversion */
    hr = VariantChangeType(&var, &var, 0, vartype);

  if (S_OK == hr) /* we got a value of requested type */
    {
      int rv;
      rv =
        loAddRealTag_aW(ca->ca_se, ti, 0, itemid, 0,
                        OPC_READABLE | OPC_WRITEABLE, &var, 0, 100);
      if (rv)
        {
          if (rv == EEXIST) *ti = 0; /* Already there? - Use existing one! */
          else hr = E_OUTOFMEMORY;
        }
    }
  VariantClear(&var);
  return hr;
}

/******* ******* ******* ******* ******* ******* ******* *******/

int driver_init(int lflags)
{
  loDriver ld;
  VARIANT var;
  loTagId tti;
  int ecode;

  setlocale(LC_CTYPE, "");

  if (my_service) 
    {
      UL_ERROR((LOGID, "Driver already initialized!"));
      return 0;
    }

  memset(&ld, 0, sizeof(ld));   /* basic server parameters: */
//  ld.ldRefreshRate = 3;//10;
  ld.ldSubscribe = activation_monitor;
  ld.ldWriteTags = WriteTags;
//  ld.ldReadTags = ReadTags;
  ld.ldConvertTags = ConvertTags;
#if 0
  ld.ldAskItemID = AskItemID;
#endif
  ld.ldFlags = lflags | loDF_IGNCASE |  /*loDf_FREEMARSH | loDf_BOTHMODEL | */
    /*loDF_NOCOMP| */ loDf_NOFORCE & 0 /*| loDF_SUBSCRIBE_RAW*/;
    /*Fix the Bug in ProTool *//*|loDF_IGNCASE */ ;
  ld.ldBranchSep = '/'; /* Hierarchial branch separator */

  ecode = loServiceCreate(&my_service, &ld, 64 /* number of tags in the cache */);
                                               /* 500000 is ok too */ 
  UL_TRACE((LOGID, "%!e loCreate()=", ecode));
  if (ecode) return -1;

  InitializeCriticalSection(&lk_values);
  memset(tv, 0, sizeof(tv));    /* instead of VariantInit() for each of them */

  VariantInit(&var);

/* OPTIONAL: Tags creation. Do you need a few tags? */

  ecode = loAddRealTag_b(my_service,    /* actual service context */
                         &tti,  /* returned TagId */
                         0,     /* != 0  driver's key */
                         "Just the hint / [0...999]",   /* tag name (hint) */
                         0,     /* loTF_ Flags */
                         OPC_READABLE | OPC_WRITEABLE, 0);

  V_I1(&var) = -1; /* the good practice: set the value first, then type. */
  V_VT(&var) = VT_I1; /* Do you know why? */

  ecode = loAddRealTag_a(my_service,    
                  &ti[TI_bandwidth], /* returned TagId (lightopc's identifier)*/
                  (loRealTag)TI_bandwidth, /* != 0  (driver's internal identifier)*/
                  tn[TI_bandwidth],      /* tag name */
                  0,                     /* loTF_ Flags */
                  OPC_READABLE,          /* OPC access rights */
                  &var,                  /* type and value for conversion checks */
                  -1, 101); /*Analog EUtype: from -1 (unknown) to 101% (overload)*/
  /* Shows current bandwidth */

  /* We needn't to VariantClear() for simple datatypes like numbers */
  V_R8(&var) = 214.1; /* initial value. Will be used to check types conersions */
  V_VT(&var) = VT_R8;
  ecode = loAddRealTag_a(my_service,    /* actual service context */
                         &ti[TI_zuzu],  /* returned TagId */
                         (loRealTag)TI_zuzu, /* != 0 driver's key */
                         tn[TI_zuzu],   /* tag name */
                         0,     /* loTF_ Flags */
                         OPC_READABLE | OPC_WRITEABLE, &var, 12., 1200.);
  UL_TRACE((LOGID, "%!e loAddRealTag_a(zuzu) = %u ", ecode, ti[TI_zuzu]));

  V_I2(&var) = 1000;
  V_VT(&var) = VT_I2;
  ecode = loAddRealTag(my_service,      /* actual service context */
                       &ti[TI_lulu],    /* returned TagId */
                       (loRealTag) TI_lulu,     /* != 0 driver's key */
                       tn[TI_lulu],     /* tag name */
                       0,       /* loTF_ Flags */
                       OPC_READABLE | OPC_WRITEABLE, &var, 0, 0);
  UL_TRACE((LOGID, "%!e loAddRealTag(lulu) = %u ", ecode, ti[TI_lulu]));

  /* This 2nd 'zuzu' will fail with EEXISTS */
  ecode = loAddRealTag(my_service,     
                       &tti,    /* returned TagId */
                       (loRealTag) TI_zuzu,     /* != 0 driver's key */
                       "zuzu",  /* tag name */
                       0,       /* loTF_ Flags */
                       OPC_READABLE | OPC_WRITEABLE, &var, 0, 0);
  UL_TRACE((LOGID, "%!e loAddRealTag(zuzu) = %u (duplicated)", ecode, tti));

#if 1 /* OPTIONAL: generate several tags to test capabilites & performance */
  {
    char name[64];
    int ii, sep;
    DWORD sttime = GetTickCount();
    memset(name, 0, sizeof(name));
    strcpy(name, "A Huge list /");
    ii = strlen(name);
    memset(name + ii, 'Z', 32 - ii);

    for(ecode = 0, sep = '-'; *name <= 'B'; sep = '/', (*name)++)
      {
        for(ii = 0; ii < 4 /*0000 */ && !ecode; ii++)
          {
            loTagId tt;
            sprintf(name + 32, ".%010u%c@", ii, sep);
            ecode = loAddRealTag_b(my_service,
                                   &tt, 0, name, 0, OPC_READABLE,
                                   ti[TI_bandwidth]);
/* loAddRealTag_b() inherits attributes of specified tag - ti[TI_bandwidth] */
          }
        UL_WARNING((LOGID, "%u tags created for %d mS",
                    ii * (*name - 'A' + 1), GetTickCount() - sttime));
      }
  }
#endif

  loAddRealTag(my_service, &ti[TI_quiet], (loRealTag) TI_quiet, tn[TI_quiet],
               0, OPC_READABLE, &var, 0, 0);

  loAddRealTag(my_service, &tti, 0, "wr-only", 0, OPC_WRITEABLE, &var, 0, 0);

  /* OPTIONAL: Create some Hierachial structure: */
#define ADD_TAG(name) loAddRealTag_b(my_service, &tti, \
                      0, name, 0,\
                      OPC_READABLE|OPC_WRITEABLE, tti)
  ADD_TAG("Tree/qw/qq");
  ADD_TAG("Tree/qw/zz");
  ADD_TAG("Tree/qw/ii/hh");
  ADD_TAG("Tree/qw/ii/yy");
  ADD_TAG("Tree/qw/ii/yy/mm/dd");
  ADD_TAG("Tree/eec/ii/yy/mMm/dd");
  ADD_TAG("Tree/eec/ii/yy/mMm/ddd");
  ADD_TAG("Tree/ee/ii/yy/mm/dd");
  ADD_TAG("Tree/ee/ii/yyy/mm/dd");

/* OPTIONAL:  Advanced Item Properies: defining and assignement */
{                             
  loPLid plid;

  if (0 == (plid = loPropListCreate(my_service)))
    {
      UL_WARNING((LOGID, "loPropListCreate() FAILED"));
    }
  else
    {
      V_R8(&var) = 999.9;
      V_VT(&var) = VT_R8;
      if (loPropertyAdd(my_service, plid, 102, &var, 0, 0) ||
          loPropertyAdd(my_service, plid, 5001, 0, "/Two", 0) ||
          loPropertyAdd(my_service, plid, 5002, 0, "//Two", 0) ||
          loPropertyAdd(my_service, plid, 5003, 0, "Two",
                        "a Custom property Two"))
        {
          UL_WARNING((LOGID, "loPropertyAdd() FAILED"));
        }
      ADD_TAG("device/port/one/Two");
      loPropListAssign(my_service, plid, tti, 1);
      ADD_TAG("device/port/one");
      loPropListAssign(my_service, plid, tti, 1);
      ADD_TAG("device/port/Two");
      loPropListAssign(my_service, plid, tti, 1);
      ADD_TAG("device/port");
      loPropListAssign(my_service, plid, tti, 1);
      ADD_TAG("device/Two");
      loPropListAssign(my_service, plid, tti, 1);
      ADD_TAG("device");
      loPropListAssign(my_service, plid, tti, 1);
      ADD_TAG("Two");
      loPropListAssign(my_service, plid, tti, 1);
    }
}
#undef ADD_TAG
#if 1 /* OPTIONAL: Creation of complex datatypes */
{
  SAFEARRAY *sa;
  SAFEARRAYBOUND sbound[1];
  sbound[0].lLbound = 0;
  sbound[0].cElements = 2;

/*** VT_ARRAY type */
  VariantInit(&tv[TI_array].tvValue);

  if (sa = SafeArrayCreate(VT_I4, 1, sbound))
    {
      HRESULT hr;
      long ix0 = 0, ix1 = 1, val = 333;

      V_ARRAY(&tv[TI_array].tvValue) = sa;
      V_VT(&tv[TI_array].tvValue) = VT_ARRAY | VT_I4;

      if (S_OK != (hr = SafeArrayPutElement(sa, &ix0, &val)) ||
          S_OK != (hr = SafeArrayPutElement(sa, &ix1, &val)))
        {
          UL_NOTICE((LOGID, "%!l SafeArray", hr));
          VariantClear(&tv[TI_array].tvValue);
        }
      else
         if (!loAddRealTag(my_service, &ti[TI_array], 
                    (loRealTag)TI_array, tn[TI_array],
                    loTF_CONVERT, OPC_READABLE | OPC_WRITEABLE,
                    &tv[TI_array].tvValue, 0, 0))
        {
          tv[TI_array].tvTi = ti[TI_array];
          tv[TI_array].tvState.tsError = S_OK;
          tv[TI_array].tvState.tsQuality = OPC_QUALITY_GOOD;
        }
    }
  else
    {
      UL_ERROR((LOGID, "driver_init(): SafeArrayCreate() failed"));
    }

/*** An enumerated type (localized weekdays names) with EUinfo ***/
  sbound[0].lLbound = 0;
  sbound[0].cElements = 7;

  VariantInit(&tv[TI_enum].tvValue);
  V_I2(&tv[TI_enum].tvValue) = 6;
  V_VT(&tv[TI_enum].tvValue) = VT_I2;

  if (sa = SafeArrayCreate(VT_BSTR, 1, sbound))
    {
      VARIANT ed;
      VariantInit(&ed);
      long ix;

      V_VT(&ed) = VT_ARRAY | VT_BSTR;
      V_ARRAY(&ed) = sa;

      for(ix = 0; ix < 7; ix++)
        {
          HRESULT hr;
          BSTR bs;
          WCHAR ws[32];
          local_text(ws, (unsigned)ix, 0); /* generates localized name */

          bs = SysAllocString(ws);
          if (S_OK != (hr = SafeArrayPutElement(sa, &ix, bs)))
            {
              UL_NOTICE((LOGID, "%!l SafeArray PutString", hr));
            }
          SysFreeString(bs);
        }

      if (!loAddRealTag(my_service, &ti[TI_enum], (loRealTag) TI_enum,
                        tn[TI_enum], loTF_CONVERT/*localization request*/, 
                        OPC_READABLE, &tv[TI_enum].tvValue, OPC_ENUMERATED, &ed))
        {
          tv[TI_enum].tvTi = ti[TI_enum];
          tv[TI_enum].tvState.tsError = S_OK;
          tv[TI_enum].tvState.tsQuality = OPC_QUALITY_GOOD;
        }
      VariantClear(&ed);
    }
  else
    {
      UL_ERROR((LOGID, "driver_init(): SafeArrayCreate(enum) failed"));
    }
}
#endif

/*** Show differnt OPC-Qualities ***/
  V_BSTR(&tv[TI_quality].tvValue) = SysAllocString(L"");
  V_VT(&tv[TI_quality].tvValue) = VT_BSTR;
  ecode = loAddRealTag(my_service, &ti[TI_quality],
                       (loRealTag) TI_quality, tn[TI_quality],
                       0, OPC_READABLE | OPC_WRITEABLE, 
                       &tv[TI_quality].tvValue, 0, 0);

/*** STRING (VT_BSTR) datatype  ***/
  V_BSTR(&tv[TI_string].tvValue) = SysAllocString(L"");
  V_VT(&tv[TI_string].tvValue) = VT_BSTR;
  ecode = loAddRealTag(my_service, &ti[TI_string],  
                       (loRealTag) TI_string, tn[TI_string],     
                       0, OPC_READABLE | OPC_WRITEABLE, 
                       &tv[TI_string].tvValue, 0, 0);
//  VariantClear(&var); 
  /* don't required because we didn't assigned complex types to var */

  return 0;
}

void driver_destroy(void)
{
  if (my_service)
    {
      int ecode = loServiceDestroy(my_service);
      UL_INFO((LOGID, "%!e loDelete(%p) = ", ecode));

      for(ecode = 0; ecode < sizeof(tv) / sizeof(tv[0]); ecode++)
        VariantClear(&tv[ecode].tvValue);

      DeleteCriticalSection(&lk_values);

      my_service = 0;
    }
}

/********* Data simulator stuff ************************************************/

void simulate(unsigned pause)
{
  DWORD hitime = 0;
  unsigned starttime = GetTickCount();
  UL_WARNING((LOGID, "Simulator Started..."));

  EnterCriticalSection(&lk_values);

/* Set up initial values for the tags we didn't initialized in driver_init(): */

  tv[TI_zuzu].tvTi = ti[TI_zuzu];
  tv[TI_zuzu].tvState.tsError = S_OK;
  tv[TI_zuzu].tvState.tsQuality = OPC_QUALITY_GOOD;
  V_VT(&tv[TI_zuzu].tvValue) = VT_R8;
  V_R8(&tv[TI_zuzu].tvValue) = -100;

  tv[TI_lulu].tvTi = ti[TI_lulu];
  tv[TI_lulu].tvState.tsError = S_OK;
  tv[TI_lulu].tvState.tsQuality = OPC_QUALITY_GOOD;
  V_VT(&tv[TI_lulu].tvValue) = VT_I2;
  V_I2(&tv[TI_lulu].tvValue) = 78;

  tv[TI_enum].tvTi = ti[TI_enum];
  tv[TI_enum].tvState.tsError = S_OK;
  tv[TI_enum].tvState.tsQuality = OPC_QUALITY_GOOD;
  V_VT(&tv[TI_enum].tvValue) = VT_I2;
  V_I2(&tv[TI_enum].tvValue) = 1;

  tv[TI_bandwidth].tvTi = ti[TI_bandwidth];
  tv[TI_bandwidth].tvState.tsError = S_OK;
  tv[TI_bandwidth].tvState.tsQuality = OPC_QUALITY_GOOD;
  V_VT(&tv[TI_bandwidth].tvValue) = VT_I1;
  V_I1(&tv[TI_bandwidth].tvValue) = -1;

  tv[TI_quiet].tvTi = ti[TI_quiet];
  tv[TI_quiet].tvState.tsError = S_OK;
  tv[TI_quiet].tvState.tsQuality = OPC_QUALITY_LAST_KNOWN;
  V_VT(&tv[TI_quiet].tvValue) = VT_I2;
  V_I2(&tv[TI_quiet].tvValue) = 1412;

  tv[TI_quality].tvTi = ti[TI_quality];
  tv[TI_quality].tvState.tsError = S_OK;
//  tv[TI_quality].tvValue -- already assigned

  tv[TI_string].tvTi = ti[TI_string];
  tv[TI_string].tvState.tsQuality = OPC_QUALITY_GOOD;
  tv[TI_string].tvState.tsError = S_OK;
//  tv[TI_string].tvValue -- already assigned

//  tv[TI_enum], tv[TI_array], string, quality, -- already assigned 

  LeaveCriticalSection(&lk_values);

/**** Then do simulate ***********/

  while(0 != my_CF.server_inuse) /* still working? */
    {
      FILETIME ft;

      Sleep(10);   /* main delay */
      GetSystemTimeAsFileTime(&ft); /* awoke */

/* OPTIONAL: reload log's configuration fromtime to time */
      if (hitime != (ft.dwLowDateTime & 0xf8000000))    /* 13.5 sec */
        {                       /* 0xff000000 is about 1.67 sec */
          hitime = ft.dwLowDateTime & 0xf8000000;
#if 0
          unilog_Refresh(0);    /* all logs */
#else
          unilog_Refresh("LightOPC");
          unilog_Refresh("Lopc-Sample-exe");
          unilog_Refresh("Lopc-Sample-dll");
#endif
        }

/***** The main job: update the values ******/
      EnterCriticalSection(&lk_values);

double zuzu = 
      (V_R8(&tv[TI_zuzu].tvValue) += 1./3.); /* main simulation */
      V_VT(&tv[TI_zuzu].tvValue) = VT_R8;
      tv[TI_zuzu].tvState.tsTime = ft;

      V_I2(&tv[TI_lulu].tvValue) = (short)zuzu;
      V_VT(&tv[TI_lulu].tvValue) = VT_I2;
      tv[TI_lulu].tvState.tsTime = ft;

      V_I2(&tv[TI_enum].tvValue) = (short)((ft.dwLowDateTime >> 22) % 7);
      V_VT(&tv[TI_enum].tvValue) = VT_I2;
      tv[TI_enum].tvState.tsTime = ft;

      V_I1(&tv[TI_bandwidth].tvValue) = (char)loGetBandwidth(my_service, 0);
      V_VT(&tv[TI_bandwidth].tvValue) = VT_I1;
      tv[TI_bandwidth].tvState.tsTime = ft;
      
/** OPTIONAL: enumerate all possible qualities: */
      static const unsigned char legalq[] = {
    OPC_QUALITY_CONFIG_ERROR    ,    OPC_QUALITY_NOT_CONNECTED   ,
    OPC_QUALITY_DEVICE_FAILURE  ,    OPC_QUALITY_SENSOR_FAILURE  ,
    OPC_QUALITY_LAST_KNOWN      ,    OPC_QUALITY_COMM_FAILURE    ,
    OPC_QUALITY_OUT_OF_SERVICE  ,

// STATUS_MASK Values for Quality = UNCERTAIN
    OPC_QUALITY_LAST_USABLE     ,    OPC_QUALITY_SENSOR_CAL      ,
    OPC_QUALITY_EGU_EXCEEDED    ,    OPC_QUALITY_SUB_NORMAL      ,

// STATUS_MASK Values for Quality = GOOD
    OPC_QUALITY_LOCAL_OVERRIDE  };

      tv[TI_quality].tvState.tsTime = ft;
      tv[TI_quality].tvState.tsQuality = 
        legalq[((unsigned)ft.dwLowDateTime >> 24) % SIZEOF_ARRAY(legalq)];

/** OPTIONAL: change the STRING variable: */
      WCHAR wstr[32];
      local_text(wstr, (ft.dwLowDateTime >> 24) % 7, 0);
      VariantClear(&tv[TI_string].tvValue); /* don't miss it! 
                                    BSTR is not a simple type */
      V_BSTR(&tv[TI_string].tvValue) = SysAllocString(wstr);
      V_VT(&tv[TI_string].tvValue) = VT_BSTR;
      tv[TI_string].tvState.tsTime = ft;

/** OPTIONAL: change the ARRAY variable: */
      if (ti[TI_array]) /* has been successfuly initialized? */
        {
          long ix0 = 1, val = (long)zuzu;
          SafeArrayPutElement(V_ARRAY(&tv[TI_array].tvValue), &ix0, &val);
          tv[TI_array].tvTi = ti[TI_array];
          tv[TI_array].tvState.tsTime = ft;
        }

/** MANDATORY: send all the values into the cache: */
      loCacheUpdate(my_service, TI_MAX, tv + 1, 0);

      LeaveCriticalSection(&lk_values);

      if (pause) 
        {
/* IMPORTANT: force unloading of an out-of-proc 
   if not connected during PAUSE millisec */
          unsigned tnow = GetTickCount();
          if (tnow - starttime >= pause)
            {
              pause = 0;
              my_CF.serverRemove();
            }
        }

      if (test_mode)
        {
/* OPTIONAL: simulate client's connections to test loSetState
      and track down resource leakage */
          static int xx;
          static IUnknown *cli;
          unsigned tnow = GetTickCount();
          if (tnow - starttime >= 2500)
            {
              starttime = tnow;
/* To test for unreleased handles / threads
   run taskmgr and see what's happen...*/
              if (cli) 
                {
                  cli->Release(); cli = 0;
                  if (5 < ++xx) 
                    {
                      puts("SHUTDOWN initiated...");
                      loSetState(my_service, 0, loOP_SHUTDOWN,
                                 OPC_STATUS_SUSPENDED, "forced shutdown");
                      my_CF.serverRemove();
                      test_mode = 2;
                    }
                }
              else if (test_mode == 1)
                {
                  my_CF.CreateInstance(0, IID_IUnknown, (void **)&cli);
                }
              else
                {
                  puts("DISCONNECT initiated...");
                  loSetState(my_service, 0, loOP_STOP | loOP_DISCONNECT,
                             OPC_STATUS_SUSPENDED, "forced shutdown");
                }
            }
        } /* end of if (test_mode... */
    } /* end of loop */

  if (0 == my_CF.is_out_of_proc) /* For in-proc servers only! */
    {
      EnterCriticalSection(&my_CF.lk_count);
      driver_destroy();
      LeaveCriticalSection(&my_CF.lk_count);
    }
  
  UL_MESSAGE((LOGID, "All clean. exiting..."));
}


/***************************************************************************
 EXE-specific stuff
 ***************************************************************************/

const char eClsidName[] = "LightOPC Sample server (exe)";
const char eProgID[] = "OPC.LightOPC-exe";

HMODULE server_module = 0;
static int server_main(HINSTANCE hInstance, int argc, char *argv[]);

extern "C"
  int APIENTRY WinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  static char *argv[3] = { "fake.exe", NULL, NULL };
  argv[1] = lpCmdLine;
  return server_main(hInstance, 2, argv);
}

extern "C" int main(int argc, char *argv[])
{
  return server_main(GetModuleHandle(NULL), argc, argv);
}

int server_main(HINSTANCE hInstance, int argc, char *argv[])
{
  int main_rc = 0;
  DWORD objid;
  int daemon = 0, pause = 20;
  const char *exit_msg = "Exiting...";
  server_module = hInstance;

  log = unilog_Create("LOPC-exe", "|LOPC-exe", "%!T", -1,       /* Max filesize: -1 unlimited, -2 -don't change */
                      ll_MESSAGE);        /* level [ll_FATAL...ll_DEBUG] */
  unilog_Redirect("LOPC-exe", "LightOPC", 0);
  unilog_Delete(log);
  log = unilog_Create("Lopc-Sample-exe", "|Lopc-Sample-exe", "", -1,    /* Max filesize: -1 unlimited, -2 -don't change */
                      ll_TRACE);        /* level [ll_FATAL...ll_DEBUG] */
  UL_DEBUG((LOGID, "WinMain(%s) invoked...", argv[0]));

  if (argv[1])
    {
      printf("%s\nClass %s :  ", argv[0], eProgID);
      if (strstr(argv[1], "/r"))
        {
          char np[FILENAME_MAX + 32];
          printf("Registering");
          GetModuleFileName(NULL, np + 1, sizeof(np) - 8);
          np[0] = '"'; strcat(np, "\"");
          
          if (strstr(argv[1], "/r")[2] == 'c') strcat(np, " /c");

          if (loServerRegister(&CLSID_LightOPCServerEXE, 
                               eProgID, eClsidName, np, 0))
            {
              UL_ERROR((LOGID, "%!L Reg <%s> <%s> Failed", eProgID, argv[0]));
              puts(" FAILED");
              main_rc = 1;
            }
          else
            {
              UL_INFO((LOGID, "Reg <%s> <%s> Ok", eProgID, argv[0]));
              puts(" Succeeded");
            }
          goto Finish;
        }
      else if (strstr(argv[1], "/u"))
        {
          printf("UnRegistering");
          if (loServerUnregister(&CLSID_LightOPCServerEXE, eProgID))
            {
              UL_WARNING((LOGID, "%!L UnReg <%s> <%s> Failed", eProgID, argv[0]));
              puts(" FAILED");
              main_rc = 1;
            }
          else
            {
              UL_DEBUG((LOGID, "UnReg <%s> <%s> Ok", eProgID, argv[0]));
              puts(" Success");
            }
          goto Finish;
        }
      else if (strstr(argv[1], "/c") || strstr(argv[1], "/C"))
        {
          UL_MESSAGE((LOGID, "Creating new console..."));
          use_console = 1;
          AllocConsole();
          goto Cont;
        }
      else if (strstr(argv[1], "/daemon"))
        {
          UL_MESSAGE((LOGID, "Running as DAEMON"));
          puts("Running as DAEMON...");
          daemon = 1; use_console = 1;
          goto Cont;
        }
      else if (strstr(argv[1], "/detach"))
        {
          UL_MESSAGE((LOGID, "Running as Detached DAEMON"));
          puts("Running as Detached DAEMON...");
          daemon = 2;
          goto Cont;
        }
      else if (strstr(argv[1], "/test"))
        {
          UL_MESSAGE((LOGID, "Running in TEST mode"));
          puts("Running in TEST mode...");
          daemon = 1; use_console = 1; test_mode = 1;
          goto Cont;
        }
      else
        {
          UL_WARNING((LOGID, "unknown option <%s>", argv[1]));
          printf("Unknown option <%s>", argv[1]);
          puts("\nOne of following flags can be specified:"
               "\n\t/r       - register;"
               "\n\t/u       - unregister;"
               "\n\t/rc      - register to start with /c key;"
               "\n\t/c       - create console window;"
               "\n\t/daemon  - run in daemon mode;"
               "\n\t/test    - run in test mode for 25 seconds;"
               "\n\t/detach  - run in unbreakable daemon mode.");
          goto Cont;
        }
      goto Finish;
    }

Cont: /* Attempt to start the server */
  if (1 != daemon && 0 == use_console)
    FreeConsole();

  my_CF.is_out_of_proc = my_CF.server_inuse = 1;

  if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
    {
      exit_msg = "CoInitializeEx() failed. Exiting...";
      UL_ERROR((LOGID, exit_msg));
      goto Finish;
    }

  pause = 20; /* 20 sec can be too short for remote connections */
  if (use_console && !daemon)
    printf("Waiting %d seconds for connection...\n", pause);

  if (driver_init(0))
    {
      exit_msg = "driver_init() failed. Exiting...";
      UL_ERROR((LOGID, exit_msg));
    }
  else if (FAILED(CoRegisterClassObject(CLSID_LightOPCServerEXE, &my_CF,
                                        CLSCTX_LOCAL_SERVER |
                                        CLSCTX_REMOTE_SERVER |
                                        CLSCTX_INPROC_SERVER,
                                        REGCLS_MULTIPLEUSE, &objid)))
    {
      exit_msg = "CoRegisterClassObject() failed. Exiting...";
      UL_ERROR((LOGID, exit_msg));
    }
  else
    {
      if (daemon) pause = 0;          // infinite

      my_CF.serverAdd(); /* Oops. This will prewent our server from unloading
                till PAUSE elapsed and simulate() do my_CF.serverRemove() */
/* STARTing the simulator */
      simulate(pause * 1000); /* sec -> millisec; */ 
/* FINISHED */
      if (FAILED(CoRevokeClassObject(objid)))
        UL_WARNING((LOGID, "CoRevokeClassObject() failed..."));
    }
  driver_destroy();
  CoUninitialize();

Finish:
  if (use_console) puts(exit_msg);

  UL_DEBUG((LOGID, "WinMain(%s) finished", argv[0]));
  unilog_Delete(log);
  log = 0;
  return main_rc;
}

/***************************************************************************
 DLL-specefic stuff
 ***************************************************************************/

static void dll_simulator_wait(void) /* in-proc specefic */
{
  for(;;) 
    {
      int finished;
      EnterCriticalSection(&my_CF.lk_count); 
      finished = my_service == 0 || my_CF.server_inuse;
      LeaveCriticalSection(&my_CF.lk_count);
      if (finished) break;
      else Sleep(500);
    }
  /* Looking for conditional variables?
     See unilog/condsb.[hc] */
}

static void dll_simulator_start(void) /* in-proc specefic */
{
  int rv;
  EnterCriticalSection(&my_CF.lk_count);
  if (!my_service)
    {
      if (rv = driver_init( /* inproc only options! */
#if 1
               loDf_FREEMARSH | loDf_BOTHMODEL
#else
               0
#endif
        ))
        {
           UL_ERROR((LOGID, "!%e driver_init FAILED:", rv));
        }
      else if (-1 == (int)_beginthread((void(*)(void *))simulate, 0, (void *)0))
        {
           UL_ERROR((LOGID, "!%E _beginthread() FAILED:"));
           driver_destroy();
        }
      else my_CF.server_inuse = 1;
    }

  LeaveCriticalSection(&my_CF.lk_count);  
}

extern "C"
  BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  if (fdwReason == DLL_PROCESS_ATTACH)
    {
      server_module = hinstDLL;

      log = unilog_Create("LOPC-dll", "|LOPC-dll", "%!T", -1,   /* Max filesize: -1 unlimited, -2 -don't change */
                          ll_DEBUG);    /* level [ll_FATAL...ll_DEBUG] */
      unilog_Redirect("LOPC-dll", "LightOPC", 0);
      unilog_Delete(log);
      log = unilog_Create("Lopc-Sample-dll", "|Lopc-Sample-dll", "", -1,        /* Max filesize: -1 unlimited, -2 -don't change */
                          ll_DEBUG);    /* level [ll_FATAL...ll_DEBUG] */
      UL_DEBUG((LOGID, "DllMAin(process_attach)"));
      //driver_init();    /* not the best place */
    }
  else if (fdwReason == DLL_PROCESS_DETACH)
    {

      UL_DEBUG((LOGID, "DllMAin(process_detach)"));
      unilog_Delete(log);
      log = 0;
      //driver_destroy(); /* not the best place */
    }

  return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv)
{
  UL_DEBUG((LOGID, "DllGetClassObject() ..."));

  if (rclsid == CLSID_LightOPCServerDLL || rclsid == CLSID_LightOPCServerEXE)
    {
      HRESULT hr = my_CF.QueryInterface(riid, ppv);
      UL_DEBUG((LOGID, "%!l DllGetClassObject() >>%04X %s", hr, hr,
                FAILED(hr) ? "failed" : "Ok"));
      return hr;
    }
  UL_DEBUG((LOGID, "DllGetClassObject() >>CLASS_E_CLASSNOTAVAILABLE"));
  return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllCanUnloadNow(void)
{
  UL_DEBUG((LOGID, "DllCanUnloadNow() invoked servers=%d", my_CF.server_count));
  my_CF.serverAdd();
  my_CF.serverRemove(); /* Force stopping the simulator if not used */
/* the simulator can be restarted in ICF::CreateInstance() if requested */
  if (0 == my_CF.server_inuse) 
    {
      dll_simulator_wait();
      if (0 == my_CF.server_inuse) return S_OK;
    }
  return S_FALSE;
}

/* standard OLE registration stuff */

const char dClsidName[] = "LightOPC Sample server (dll)";
const char dProgID[] = "OPC.LightOPC-dll";

STDAPI DllRegisterServer(void)
{
  char sFile[FILENAME_MAX + 16];
  GetModuleFileName(server_module, sFile, sizeof(sFile) - 1);
  UL_DEBUG((LOGID, "DllRegister(%s)", sFile));
  return loServerRegister(&CLSID_LightOPCServerDLL, dProgID, dClsidName, sFile,
          "Both") ? SELFREG_E_CLASS: S_OK;
}

STDAPI DllUnregisterServer(void)
{
  return loServerUnregister(&CLSID_LightOPCServerDLL, dProgID)? SELFREG_E_CLASS: S_OK;
}

/* OPTIONAL: **************** special helpers ******************************/

static void local_text(WCHAR buf[32], unsigned nn, LCID lcid)
{
  char sbt[40];
  long lcp = CP_ACP;
  unsigned nx;

  nn %= 7;
  /* Nasty Win95 doesn't have proper unicode support.
     So we getting singlebyte strings and then converting them to unicode.
     But we've to know the right codepage first! 
     CP_ACP is our cp. An OPC client may choose other. */

  if (0 == GetLocaleInfoA(lcid, LOCALE_IDEFAULTANSICODEPAGE, sbt, sizeof(sbt)-1))
    goto Failure;
  lcp = strtoul(sbt, 0, 10);

  /* How does called "Monday"+nn in LCID country? */
  if (0 == GetLocaleInfoA(lcid, LOCALE_SDAYNAME1 + nn, sbt, sizeof(sbt) - 1))
    goto Failure;

  nx = strlen(sbt);
  if (sizeof(sbt) - nx > 12)
    {
      sbt[nx++] = ' '; /* Append language name [OPTIONAL] */
      if (0 == GetLocaleInfoA(lcid, LOCALE_SENGLANGUAGE, sbt + nx,
                         sizeof(sbt) - nx - 1) && 
          0 == GetLocaleInfoA(lcid, LOCALE_SENGCOUNTRY, sbt + nx,
                         sizeof(sbt) - nx - 1) )
        sbt[--nx] = 0; /* ... or the country name */
    }
  if (0 == MultiByteToWideChar(lcp, 0, sbt, -1, buf, 32))
    {
    Failure:
      swprintf(buf, L"%d [string]", nn);
    }
// UL_DEBUG((LOGID, "locals lcid:%x cp:%d [%s] [%ls]%d", 
   //lcid, lcp, sbt, buf, wcslen(buf)));
}

/******************* This is the end... ************************************/
