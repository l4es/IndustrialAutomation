/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 * Sample NT service based on ntse.                                       *
 *                                                                        *
 *   Copyright (c) 2000-2003 Timofei Bondarenko                           *
                                                                          *
 Simple OPC server / NT service

 You require the LightOPC toolkit (binary) in order to compile this sample.

 Remove this file from project and 
 reference to svcRun() from SAMP_svc.c::svcMain() 
 if you looking for NTSE only and don't wont OPC.

 The LightOPC stuff has been simplified.
 For more complex OPC sample see the LightOPC toolkit.

WARNING: This _simple_ server exposes only one tag 
  named "CurrentTime" of type VT_DATE.
  Not all clients capable to display this datatype.
  In this case You may request datatype VT_BSTR on the client side.

 **************************************************************************/

#define _WIN32_DCOM
#include <windows.h>
#include <winsvc.h>

#include <unilog.h>
#define LOGID log,0
#include <ole2.h>
#include <oleauto.h>
#include <opcda.h>

#include "samp_svc.h"
#include <lightopc.h>

#if 0
#define CoQueryClientBlanket(a,b,c,d,e,userid,f) (*((const wchar_t**)userid)=L"Wine!")
#endif

#ifndef TEST_SVC

char svcName[] = "LOPC-Sample-SVC";
char svcDescr[] = "ntse+LightOPC simple NT service";

unsigned svcAcceptCtl = (SERVICE_ACCEPT_PAUSE_CONTINUE | \
    SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
unsigned svcWaitHint = (5000) /* 5 sec */;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,     // command line
                   int nCmdShow) // show state
{
    char *argv[3] = { "samp_svc", lpCmdLine, NULL };
    return svcMain(2, argv);
}

int main(int argc, char *argv[])
{
  return svcMain(argc, argv);
}
#endif /*TEST_SVC*/

unilog *log;                    /* logging entry */

/********************* OPC vendor info ***************************************/
static const loVendorInfo vendor = {
  3 /*Major */ , 2 /*Minor */ , 1 /*Build */ , 0 /*Reserv */ ,
  "Sample OPC Server #9"
};

loService *my_service;

/* OLE-specefic data: ***********************************************************/

// {5855E9DD-FB7C-405b-B43A-00F31E71BA60}  NT-service
static const GUID CLSID_LightOPCService = 
{ 0x5855e9dd, 0xfb7c, 0x405b, { 0xb4, 0x3a, 0x0, 0xf3, 0x1e, 0x71, 0xba, 0x60 } };

/**** Server Counting stuff & OLE CF implementation ******************************
  The IClassFactory is unavoidable evil.
  Basically we've to unload when the server_count being zero.
  But there are different tecniques for in/out proc servers.
*/

class TinyClassFactory: public IClassFactory
{
public:
 LONG server_count;
 CRITICAL_SECTION lk_count;  /* protect server_count */
 HANDLE cond;

 TinyClassFactory(): server_count(0)
   {
      InitializeCriticalSection(&lk_count);
      cond = CreateEvent(NULL, FALSE, FALSE, NULL);
   }
 ~TinyClassFactory()
   {
      DeleteCriticalSection(&lk_count);
      CloseHandle(cond);
   }

  void serverAdd(void)
    {
      EnterCriticalSection(&lk_count);
      ++server_count;
      LeaveCriticalSection(&lk_count);
    }

  void serverRemove(void)
    {
      EnterCriticalSection(&lk_count);
      if (0 == --server_count) SetEvent(cond);
      LeaveCriticalSection(&lk_count);
    }
  LONG waitZeroCount(unsigned millisec)
    {
      DWORD time = GetTickCount();
      EnterCriticalSection(&lk_count);
      while(0 != server_count)
        {
          DWORD now = GetTickCount();
          if (millisec != INFINITE)
            { 
              if (millisec < now - time) break;
              millisec -= now - time;
              time = now;
            }
          LeaveCriticalSection(&lk_count);
          WaitForSingleObject(cond, millisec);
          EnterCriticalSection(&lk_count);
        }
      LONG rcount = server_count;
      LeaveCriticalSection(&lk_count);
      return rcount;
    }

/* Do nothing: we're static, he-he */  
  STDMETHODIMP_(ULONG) AddRef(void) { return 1; }
  STDMETHODIMP_(ULONG) Release(void) { return 1; }

/* Do nothing: service should be unloaded explicitly */
  STDMETHODIMP LockServer(BOOL fLock) { return S_OK; } 

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

  STDMETHODIMP CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid,
                              LPVOID *ppvObject);

};

static TinyClassFactory my_CF;

static void a_server_finished(void *a, loService *b, loClient *c)
{
  UL_DEBUG((LOGID, "a_server_finished(%lu)...", my_CF.server_count));
  my_CF.serverRemove();
}

STDMETHODIMP TinyClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid,
                                            LPVOID *ppvObject)
{
  IUnknown *server = 0;
  OLECHAR *userid = 0;
  HRESULT hr = S_OK;

  CoQueryClientBlanket(0, 0, 0, 0, 0, (RPC_AUTHZ_HANDLE*)&userid, 0);

  UL_WARNING((LOGID, "USER:<%ls>", userid ? userid : L"{unknown}"));

  if (pUnkOuter)
    {
#if 1 /* Do we support aggregation? */
     if (riid != IID_IUnknown) 
#endif
          return CLASS_E_NOAGGREGATION;
    }

  serverAdd();                  /* for a_server_finished() */
/* check other conditions (i.e. security) here */
{
 IUnknown *inner = 0;
 if (loClientCreate_agg(my_service, (loClient**)&server, 
                       pUnkOuter, &inner,
                       0, &vendor, a_server_finished, this))
    {
      serverRemove();
      hr = E_OUTOFMEMORY;
      UL_DEBUG((LOGID, "myClassFactory::loClientCreate_agg() failed"));
    }
  else if (pUnkOuter) /*aggregation requested*/
    *ppvObject = (void*)inner;
  else /* no aggregation */
    {
/* loClientCreate(my_service, (loClient**)&server, 
                  0, &vendor, a_server_finished, this) - with no aggregation */
      hr = server->QueryInterface(riid, ppvObject);
      server->Release();
      if (FAILED(hr)) 
        UL_DEBUG((LOGID, "myClassFactory::loClient QueryInterface() failed"));
    }
}
  if (SUCCEEDED(hr))
    {
      loSetState(my_service, (loClient*)server,
             loOP_OPERATE, (int) OPC_STATUS_RUNNING,
             "Finished by client");
      UL_DEBUG((LOGID, "myClassFactory::server_count = %ld", server_count));
    }
  return hr;
}

/**********************************************************************
 sample server initiation & simulation
 **********************************************************************/

int svcRun(svcHandle *ctx, const char *svc_name)
{
  int state = SERVICE_RUNNING;
  int ctl;
  loTagId ti;
  loTagValue tv;
  int ecode;
  DWORD objid;
  int oleini = 0, classobj = 0;
  HRESULT hr = E_FAIL;
  loDriver ld;

  UL_TRACE((LOGID, "svcRun() invoked..."));

/*************************** Log initialization ************/  

  log = unilog_Create("LOPC-svc", "|LOPC-svc", "%!T", -1,       /* Max filesize: -1 unlimited, -2 -don't change */
                      ll_MESSAGE);        /* level [ll_FATAL...ll_DEBUG] */
  unilog_Redirect("LOPC-svc", "LightOPC", 0);
  unilog_Delete(log);
  log = unilog_Create("Lopc-Sample-svc", "|Lopc-Sample-svc", "", -1,    /* Max filesize: -1 unlimited, -2 -don't change */
                      ll_TRACE);        /* level [ll_FATAL...ll_DEBUG] */

/*************************** LigtOPC initialization ************/  
  memset(&ld, 0, sizeof(ld));
  ld.ldFlags = loDF_IGNCASE;
  ld.ldBranchSep = '/';

  VariantInit(&tv.tvValue);
  GetSystemTimeAsFileTime(&tv.tvState.tsTime);
  V_DATE(&tv.tvValue) = lo_filetime_to_variant(&tv.tvState.tsTime);
  V_VT(&tv.tvValue) = VT_DATE;
  tv.tvState.tsError = S_OK;
  tv.tvState.tsQuality = OPC_QUALITY_GOOD;
  ecode = loServiceCreate(&my_service, &ld, 64 /*000 *//*16 */ );
  if (ecode)
    {
      UL_ERROR((LOGID, "%!e loCreate()=", ecode));
      goto Return;
    }
  UL_TRACE((LOGID, "loCreate()= Ok"));

  ecode = loAddRealTag(my_service,    /* actual service context */
                       &ti,      /* returned TagId */
                         0,     /* != 0  driver's key */
                         "CurrentTime",   /* tag name */
                         0,     /* loTF_ Flags */
                         OPC_READABLE, 
                         &tv.tvValue, 0, 0);
  if (ecode)
    {
      UL_ERROR((LOGID, "%!e loAddRealTag()=", ecode));
      goto Return;
    }
  UL_TRACE((LOGID, "loAddRealTag()= Ok"));

/*************************** OLE initialization ************/  
/* ATL initialization can go here as well. */

  hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
  if (FAILED(hr)) 
    {
      UL_ERROR((LOGID, "%!l CoInitialize()=", hr));
      goto Return;
    }
  oleini = 1;

  hr = CoInitializeSecurity(NULL /*sd */, -1, NULL, NULL,
                            RPC_C_AUTHN_LEVEL_PKT,
                            RPC_C_IMP_LEVEL_IMPERSONATE, NULL,
                            EOAC_NONE, NULL);
  if (FAILED(hr))
	{
      UL_WARNING((LOGID, "%!l CoInitializeSecurity() failed[Win95?]", hr));
	}
  else UL_TRACE((LOGID, "CoInitializeSecurity()= Ok"));

  hr = CoRegisterClassObject(CLSID_LightOPCService, &my_CF,
                             CLSCTX_LOCAL_SERVER |
                             CLSCTX_REMOTE_SERVER |
                             CLSCTX_INPROC_SERVER,
                             REGCLS_MULTIPLEUSE, &objid);
  if (FAILED(hr)) 
    {
      UL_ERROR((LOGID, "%!l CoRegisterClassObject()=", hr));
      goto Return;
    }
  classobj = 1;
  UL_TRACE((LOGID, "CoRegisterClassObject()= Ok"));

/*************************** Let's work ************/  

  UL_WARNING((LOGID, "Simulator Started..."));

  /* There are several ways to implement this loop 
     for various kinds of services. */
  for(ctl = svcControlAndSleep(ctx, 0, SERVICE_RUNNING, 0, 0);
      ctl != SERVICE_STOP;
      ctl = svcControlAndSleep(ctx,
                              state == SERVICE_RUNNING? 10: 32728, /* timeout (ms)*/
                               ctl, 0, 0))
    {
      switch(ctl)
        {
      case SERVICE_CONTROL_PAUSE:    
        ctl = state = SERVICE_PAUSED;  
        loSetState(my_service, NULL,
             loOP_STOP, (int)OPC_STATUS_SUSPENDED, 0);
        /* it's possible to call CoRevokeClassObject() here or lock my_CF */
        break;

      case SERVICE_CONTROL_CONTINUE: 
        ctl = state = SERVICE_RUNNING; 
        loSetState(my_service, NULL,
             loOP_OPERATE, (int)OPC_STATUS_RUNNING, 0);
        break;

      case SERVICE_CONTROL_STOP:
      case SERVICE_CONTROL_SHUTDOWN: goto BREAK;
      default: ctl = 0; 
            break;
        }
      if (state == SERVICE_RUNNING) /* do simulation */
        {
          static DWORD hitime;

          tv.tvTi = ti;
          GetSystemTimeAsFileTime(&tv.tvState.tsTime);
          VariantClear(&tv.tvValue);
          V_DATE(&tv.tvValue) = lo_filetime_to_variant(&tv.tvState.tsTime);
          V_VT(&tv.tvValue) = VT_DATE;

          loCacheUpdate(my_service, 1, &tv, 0);

          if (hitime != (tv.tvState.tsTime.dwLowDateTime & 0xf8000000)) /* 13.5 sec */
            {                       /* 0xff000000 is about 1.67 sec */
             hitime = tv.tvState.tsTime.dwLowDateTime & 0xf8000000;
#if 0
              unilog_Refresh(0);    /* all logs */
#else
              unilog_Refresh("LightOPC");
              unilog_Refresh("Lopc-Sample-SVC");
#endif
            }
        }
    } /* end of for(... */
 
BREAK:
  UL_TRACE((LOGID, "Stopping the service..."));

/****************** uninitialization *****************/

  hr = CoRevokeClassObject(objid);
  if (FAILED(hr))
     UL_WARNING((LOGID, "%!l CoRevokeClassObject() failed...", hr));
  else hr = 0, classobj = 0;

  loSetState(my_service, NULL,
             loOP_SHUTDOWN, (int)OPC_STATUS_FAILED, "Administratively down");
  my_CF.waitZeroCount(10000); /* let them finish correctly */

Return:

  if (classobj)
    CoRevokeClassObject(objid);

  if (my_service) 
    {
      ecode = loServiceDestroy(my_service);
      my_service = 0;
      if (ecode)
        UL_ERROR((LOGID, "%!e loServiceDestroy(%p) = ", ecode));
      my_CF.waitZeroCount(10000); /* let them finish incorrectly */
    }
  VariantClear(&tv.tvValue);

  if (classobj) CoRevokeClassObject(objid);

  if (oleini) CoUninitialize();

  UL_TRACE((LOGID, "Service Finished."));
  unilog_Delete(log);
  log = 0;

  return hr;
}

static const char svcProgID[] = "OPC.LightOPC-svc";

int svcRegister(const char *svc_name)
{
  return loServerRegister(&CLSID_LightOPCService, 
                          svcProgID,
                          "LightOPC Sample server (svc)", 0, 0);
}

int svcUnregister(const char *svc_name)
{
  return loServerUnregister(&CLSID_LightOPCService, svcProgID);
}

/***************************************************************************/
/* end of samp_opc.cpp */
