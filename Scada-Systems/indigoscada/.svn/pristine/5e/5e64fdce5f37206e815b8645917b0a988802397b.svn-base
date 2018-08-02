/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Kostya Volovich, Timofei Bondarenko              *
                                                                          *
 COM registration
 **************************************************************************/

#include <stdio.h>
#include "loserv.h"
#include "util.h"

#ifdef OPC_QUALITY_MASK /* is pre OPC-DA v3 headers? */
#define __GUID_DEFINED__
#include <opcda_cats.c>
#endif

static HRESULT manageOPCDAcat(const GUID *service, int remove);

static void IIDtoStr (char str[40], const GUID* iid)
{
	sprintf (str, "{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		     iid->Data1, iid->Data2, iid->Data3, iid->Data4[0], iid->Data4[1],
			 iid->Data4[2], iid->Data4[3], iid->Data4[4],
             iid->Data4[5], iid->Data4[6], iid->Data4[7]);
}

int loServerRegister(const GUID *CLSID_Svr, const char *ProgID,
                     const char *Name, const char *exPath,
                     const char *Model /* 0=exe, ""=STA dll, "Both", "Free" ...*/)
{
 HKEY hKey;
 HKEY hKeyAux;
 char sStr[260];
 char sIID [40];
 const char *srvtype = "InprocServer32";
 LONG ec = ERROR_INVALID_PARAMETER;

 if (!ProgID /*|| !exPath*/) goto SetErr;
 if (exPath && !*exPath) exPath = NULL;
 UL_TRACE((LOGID, "loServerRegister(%s)", ProgID));

 if (0 == Model) srvtype = "LocalServer32";
/*   {
    char *ext;
    if (0 > extype ||
        (ext = strrchr(exPath, '.')) &&
        !stricmp(ext, ".exe")) srvtype = "LocalServer32";
   }*/

 IIDtoStr (sIID, CLSID_Svr);

 if (ERROR_SUCCESS != (ec = RegCreateKey(HKEY_CLASSES_ROOT, ProgID, &hKey)))
   goto SetErr;
   
  if (Name) RegSetValue(hKey, NULL, REG_SZ, Name, strlen(Name)+1);
  if (ERROR_SUCCESS != (ec = RegCreateKey(hKey, "CLSID", &hKeyAux)))
    goto Fail;
  ec = RegSetValue(hKeyAux, NULL, REG_SZ, sIID, strlen(sIID)+1);
  RegCloseKey(hKeyAux);
  if (ERROR_SUCCESS != ec) goto Fail;

  if (ERROR_SUCCESS != (ec = RegCreateKey(hKey, "OPC", &hKeyAux)))
    goto Fail;
{
  HKEY vendor;
/*        RegSetValue (hKeyAux, NULL, REG_SZ, sIID, strlen(sIID)+1); */
/* Should "vendor" be a subkey or a named value? */
  if (ERROR_SUCCESS == RegCreateKey(hKeyAux, "Vendor", &vendor))
    {
     RegSetValue(vendor, NULL, REG_SZ, "Enscada", sizeof("Enscada"));
     RegCloseKey(vendor);
    }
  RegCloseKey(hKeyAux);
}
  RegCloseKey(hKey);

 sprintf (sStr, "AppID\\%s", sIID);
 if (ERROR_SUCCESS != (ec = RegCreateKey(HKEY_CLASSES_ROOT, sStr, &hKey)))
   goto UnReg;
 if (Name) RegSetValue(hKey, NULL, REG_SZ, Name, strlen(Name)+1);
/* DllSurrogate=""
   RunAs="Interactive User"
   RemoteServerName= */
 RegCloseKey(hKey);

 sprintf (sStr, "CLSID\\%s", sIID);
 if (ERROR_SUCCESS != (ec = RegCreateKey(HKEY_CLASSES_ROOT, sStr, &hKey)))
   goto UnReg;
 if (Name) RegSetValue(hKey, NULL, REG_SZ, Name, strlen(Name)+1);
 if (exPath)
   {
    if (ERROR_SUCCESS != (ec = RegCreateKey(hKey, srvtype, &hKeyAux)))
      goto Fail;
    ec = RegSetValue(hKeyAux, NULL, REG_SZ, exPath, strlen(exPath)+1);
    if (Model && *Model) /* dll */
      RegSetValueEx(hKeyAux, "ThreadingModel", 0, REG_SZ, 
                    (const BYTE*)Model, strlen(Model)+1);
    RegCloseKey(hKeyAux);
    if (ERROR_SUCCESS != ec) goto Fail;
   }
  // The NAIL for DCOMcnfg !!!
  //       RegSetValueEx(hKeyAux, NULL, 0, REG_EXPAND_SZ, exPath, strlen(exPath)+1);
 RegSetValueEx(hKey, "AppID", 0, REG_SZ, (const BYTE*)sIID, strlen(sIID)+1);
  //    RegSetValue(hKey, "AppID", REG_SZ, sIID, strlen(sIID)+1);

 if (ERROR_SUCCESS != (ec = RegCreateKey(hKey, "ProgID", &hKeyAux)))
   goto Fail;
 ec = RegSetValue(hKeyAux, NULL, REG_SZ, ProgID, strlen(ProgID)+1);
 RegCloseKey(hKeyAux);
 if (ERROR_SUCCESS != ec) goto Fail;

 if (FAILED(manageOPCDAcat(CLSID_Svr, 0)))
   {
    IIDtoStr(sIID, &CATID_OPCDAServer10);
    sprintf (sStr, "Implemented Categories\\%s", sIID);
    if (ERROR_SUCCESS != (ec = RegCreateKey(hKey, sStr, &hKeyAux)))
      goto Fail;
    RegCloseKey(hKeyAux);
    IIDtoStr (sIID, &CATID_OPCDAServer20);
    sprintf (sStr, "Implemented Categories\\%s", sIID);
    if (ERROR_SUCCESS != (ec = RegCreateKey(hKey, sStr, &hKeyAux)))
      goto Fail;
    RegCloseKey(hKeyAux);
   }
 RegCloseKey(hKey);

 return 0;

Fail:
 RegCloseKey(hKey);
UnReg:
 loServerUnregister(CLSID_Svr, Name);
SetErr:
 SetLastError(ec);
 UL_ERROR((LOGID, "%!l loServerRegister(%s) FAILED", 
          ec, ProgID? ProgID: "<null>"));
 return -1;
}

int loServerUnregister(const GUID *CLSID_Svr, const char *ProgID)
{
	char sStr[256];
	char sIID [40];
    int offs;
    LONG errs = ERROR_INVALID_PARAMETER, ec = 0;
	IIDtoStr (sIID, CLSID_Svr);

    if (!ProgID || !*ProgID) goto SetErr;

    UL_TRACE((LOGID, "loUnRegisterServer(%s)", ProgID));

    manageOPCDAcat(CLSID_Svr, -1);

	offs = sprintf(sStr, "%s\\", ProgID);
    strcpy(sStr + offs, "CurrentVersion");
	RegDeleteKey (HKEY_CLASSES_ROOT, sStr);
	strcpy(sStr + offs, "OPC\\Vendor");
	RegDeleteKey (HKEY_CLASSES_ROOT, sStr);

	strcpy(sStr + offs, "OPC");
	RegDeleteKey (HKEY_CLASSES_ROOT, sStr);
	strcpy(sStr + offs, "CLSID");
	errs = RegDeleteKey (HKEY_CLASSES_ROOT, sStr);
	if (ERROR_SUCCESS != (ec = RegDeleteKey (HKEY_CLASSES_ROOT, ProgID)))
      {
       if (ERROR_SUCCESS == errs) errs = ec;
       UL_NOTICE((LOGID, "loUnRegisterServer(%s) can't remove HKCR:%s", ProgID, ProgID));
      }

	offs = sprintf(sStr, "AppID\\%s\\", sIID);
	strcpy(sStr + offs, "LocalService");
	RegDeleteKey (HKEY_CLASSES_ROOT, sStr);
	strcpy(sStr + offs, "ServiceParameters");
	RegDeleteKey (HKEY_CLASSES_ROOT, sStr);
    sStr[offs - 1] = 0;
    if (ERROR_SUCCESS != (ec = RegDeleteKey (HKEY_CLASSES_ROOT, sStr)))
      {
       if (ERROR_SUCCESS == errs) errs = ec;
       UL_NOTICE((LOGID, "loUnRegisterServer(%s) can't remove %s", ProgID, sStr));
      }

	offs = sprintf(sStr, "CLSID\\%s\\", sIID);

    IIDtoStr(sIID, &CATID_OPCDAServer10);
	sprintf(sStr + offs, "Implemented Categories\\%s", sIID);
	RegDeleteKey (HKEY_CLASSES_ROOT, sStr);
    IIDtoStr(sIID, &CATID_OPCDAServer20);
	sprintf(sStr + offs, "Implemented Categories\\%s", sIID);
	RegDeleteKey (HKEY_CLASSES_ROOT, sStr);
	strcpy(sStr + offs, "Implemented Categories");
	RegDeleteKey (HKEY_CLASSES_ROOT, sStr);

	strcpy(sStr + offs, "InprocServer32\\ThreadingModel");
	RegDeleteKey (HKEY_CLASSES_ROOT, sStr);
	strcpy(sStr + offs, "InprocServer32");
	RegDeleteKey (HKEY_CLASSES_ROOT, sStr);
	strcpy(sStr + offs, "LocalServer32");
	RegDeleteKey (HKEY_CLASSES_ROOT, sStr);
	strcpy(sStr + offs, "InprocHandler32");
	RegDeleteKey (HKEY_CLASSES_ROOT, sStr);
	strcpy(sStr + offs, "ProgID");
	RegDeleteKey (HKEY_CLASSES_ROOT, sStr);
	strcpy(sStr + offs, "AppID");
	RegDeleteKey (HKEY_CLASSES_ROOT, sStr);
    sStr[offs - 1] = 0;
    if (ERROR_SUCCESS != (ec = RegDeleteKey (HKEY_CLASSES_ROOT, sStr)))
      {
       if (ERROR_SUCCESS == errs) errs = ec;
       UL_NOTICE((LOGID, "loUnRegisterServer(%s) can't remove %s", ProgID, sStr));
      }

SetErr:
    if (ERROR_SUCCESS != errs) 
      {
        SetLastError(errs);
        UL_ERROR((LOGID, "%!l loServerUnRegister(%s) FAILED", 
          errs, ProgID? ProgID: "<null>"));
        return -1;
      }
    return 0;
}

char *loTranslateIID(char *buf, unsigned size, const GUID *iface)
{
 LONG cb, rv;
 char name[260];
 char *val;
#define KPA "Interface\\"

 if (!iface || !buf) return "<?>";
 memcpy(name, KPA, sizeof(KPA));
 IIDtoStr(name + sizeof(KPA)-1, iface);
 if (size > sizeof(name)) { val = buf; cb = size; }
 else { val = name; cb = sizeof(name); }

 if (ERROR_SUCCESS != (rv = RegQueryValue(HKEY_CLASSES_ROOT, name, val, &cb)))
   {
//	 return rv == ERROR_MORE_DATA? 1: -1;
    IIDtoStr(val = name, iface);
    cb = strlen(val) + 1;
   }
 if (val != buf)
   {
    if (((unsigned)cb > size) && (cb = size))
      buf[cb - 1] = 0;
    memcpy(buf, val, cb);
   }
 return buf;
}

#if 0
//#include <stdio.h>

static void StrToIID(GUID* iid, const char* str)
{
 int data[8];
 sscanf(str, "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}",
	      &iid->Data1, &iid->Data2, &iid->Data3,
              &data[0], &data[1],
              &data[2], &data[3], &data[4], &data[5], &data[6], &data[7]);
 iid->Data4[0] = data[0];
 iid->Data4[1] = data[1];
 iid->Data4[2] = data[2];
 iid->Data4[3] = data[3];
 iid->Data4[4] = data[4];
 iid->Data4[5] = data[5];
 iid->Data4[6] = data[6];
 iid->Data4[7] = data[7];

 char b2[80];
 IIDtoStr(b2, iid);
 if (stricmp(b2, str)) UL_ERROR((LOGID, "IID_CONV: %s -> %s", str, b2));
}

void loSearchIID(IUnknown *punk)
{
 DWORD indx;
 HKEY key;
 char buf[256], *tbuf;

 UL_TRACE((LOGID, "Iface: %p", punk));

 if (ERROR_SUCCESS != RegOpenKey(HKEY_CLASSES_ROOT, "Interface", &key))
   {
    UL_ERROR((LOGID, "%!L RegOpenKey(Interface)"));
    return;
   }
 strcpy(buf, "Interface\\");
 tbuf = buf + strlen(buf);
 for(indx = 0; ERROR_SUCCESS == RegEnumKey(
               key, indx, tbuf, buf + sizeof(buf) - tbuf); indx++)
   {
    GUID iid;
    HRESULT hr;
    IUnknown *qi;
    LONG cb;
    char name[256]; name[0] = 0;
    cb = sizeof(name);
    RegQueryValue(key, tbuf, name, &cb);
    StrToIID(&iid, tbuf);
    if (S_OK == (hr = punk->QueryInterface(iid, (LPVOID*)&qi)))
      {
       UL_TRACE((LOGID, "Ok %s %p", name, qi));
       qi->Release();
      }
/*    else if (hr != E_NOINTERFACE)
      {
       UL_DEBUG((LOGID, "%!l %s -> %s", hr, name, tbuf));
      }    */
   }
 RegCloseKey(key);
}
#endif

#ifndef LO_FAKE_COMCAT
#include <comcat.h>
#endif
/* lack of  comcat.h in mingw */
#ifdef  LO_FAKE_COMCAT

HRESULT manageOPCDAcat(const GUID *service, int remove)
{
  return E_NOTIMPL;
}

#else
/*******************************************************************************************
Note: At this time the Component Categories Manager stores its information in the registry,
however this will change in the near future.  Please use the Component Categories Manager API
to access this information rather than using the registry directly.

6.1. Server Registration
To Register with the Component Categories Manager, a server should first register the
OPC defined Category ID (CATID) and the OPC defined Category Description by calling
ICatRegister:: RegisterCategories(),
and then register its own CLSID as an implementation of the CATID with a call to
ICatRegister:: RegisterClassImplCategories().
To get an interface pointer to ICatRegister, call CoCreateInstance() as in this example:
*******************************************************************************************/

HRESULT manageOPCDAcat(const GUID *service, int remove)
{
 HRESULT hr, hri;
 ICatRegister *pcr;

 hri = CoInitialize(0);
 hr = CoCreateInstance(
#ifndef __cplusplus
                       &
#endif
                       CLSID_StdComponentCategoriesMgr, 
                       NULL, CLSCTX_INPROC_SERVER, 
#ifndef __cplusplus
                       &
#endif
                       IID_ICatRegister, (void **)&pcr );

 if (FAILED(hr))
   {
    UL_INFO((LOGID, "%!l FAILED to get ICatRegister", hr));
   }
 else
   {
    CATID catid[2];

    if (0 == remove)
      {
       CATEGORYINFO catinf[2];
       catinf[0].catid = CATID_OPCDAServer10;
       wcscpy(catinf[0].szDescription, L"OPC Data Access Servers Version 1.0");
       catinf[0].lcid = 0x0409;
       catinf[1].catid = CATID_OPCDAServer20;
       wcscpy(catinf[1].szDescription, L"OPC Data Access Servers Version 2.0");
       catinf[1].lcid = 0x0409;

#ifdef __cplusplus
       hr = pcr->RegisterCategories(2, catinf);
#else
       hr = pcr->lpVtbl->RegisterCategories(pcr, 2, catinf);
#endif
       if (FAILED(hr))
         UL_INFO((LOGID, "%!l FAILED ICatRegister::RegisterCategories()", hr));
      }

    catid[0] = CATID_OPCDAServer10;
    catid[1] = CATID_OPCDAServer20;

#ifdef __cplusplus
    hr = remove? pcr->UnRegisterClassImplCategories(*service, 2, catid):
                 pcr->RegisterClassImplCategories(*service, 2, catid);
    pcr->Release();
#else
    hr = remove? pcr->lpVtbl->UnRegisterClassImplCategories(pcr, service, 2, catid):
                 pcr->lpVtbl->RegisterClassImplCategories(pcr, service, 2, catid);
    pcr->lpVtbl->Release(pcr);
#endif
    if (FAILED(hr))
      UL_INFO((LOGID, "%!l FAILED ICatRegister::%sRegisterClassImplCategories()",
              hr, remove? "Un": ""));
   }
 if (SUCCEEDED(hri)) CoUninitialize();
 UL_DEBUG((LOGID, "%!l ICatRegister::%sRegister", hr, remove? "Un": ""));
 return hr;
}

#endif
/* end of reg_com.c */
