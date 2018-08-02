/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2002  Timofei Bondarenko                               *
                                                                          *
 Changing type of a VT_ARRAY
 **************************************************************************/
#include <windows.h>
#ifdef __MINGW32__
#include <ole2.h>
#endif
#include <string.h>

#include "lightopc.h"

HRESULT lo_variant_changetype_array(VARIANT *dst, VARIANT *src, 
                                    LCID lcid, unsigned short flags, 
                                    VARTYPE vt)
{
 unsigned alen, ii;
 SAFEARRAY *sad, *sas;
 VARTYPE vts, vtd;

 if (vt == V_VT(src)) 
   return dst == src? S_OK: VariantCopy(dst, (VARIANT*)src);

 if (0 == (VT_ARRAY &  vt & V_VT(src)) || 
     0 != (VT_BYREF & (vt | V_VT(src))) )
   return lcid? VariantChangeTypeEx(dst, src, lcid, flags, vt)
              : VariantChangeType(dst, src, flags, vt);

 sas = V_ARRAY(src);
 if (0 == sas) return E_INVALIDARG;

 alen = 1;
 for(ii = 0; ii < sas->cDims; ii++)
   alen *= sas->rgsabound[ii].cElements;

 if (0 == alen) return E_INVALIDARG;

 vts = V_VT(src) & ~VT_ARRAY;
 vtd = vt & ~VT_ARRAY;

 sad = SafeArrayCreate(vtd, sas->cDims, sas->rgsabound);
 if (!sad) return E_OUTOFMEMORY;
/* Now we've something to destruct, so no immediatre returns ... */
{
 size_t selem = sas->cbElements, 
        delem = sad->cbElements;
 char HUGEP *avs;
 char HUGEP *avd;
 HRESULT hr, hrs, hrd;
 VARIANT tvs, tvd;
 void *pvs = &V_UI1(&tvs);
 void *pvd = &V_UI1(&tvd);
 VARTYPE *fvts = &vts;
 avs = avd = 0;
 hr = hrs = hrd = E_FAIL;

 memset(&tvs, 0, sizeof(tvs));
 memset(&tvd, 0, sizeof(tvd)); //V_VT(&tvd) = VT_EMPTY;

 if (!selem || !delem) { hr = E_INVALIDARG; goto Cleanup; } 

 hr = hrd = SafeArrayAccessData(sad, (void HUGEP**)&avd);
 if (FAILED(hr)) goto Cleanup;
 if (!avd) { hr = E_INVALIDARG; goto Cleanup; } 
 memset(avd, 0, alen * delem);

#define VARSIZE (sizeof(VARIANT) - ((unsigned)(char*)&V_UI1((VARIANT*)0)))

 if (delem > VARSIZE)
   if (delem == sizeof(tvd)) pvd = &tvd; /* ensure correct VT_DECIMAL handling */
   else { hr = DISP_E_TYPEMISMATCH; goto Cleanup; } 

 hr = hrs = SafeArrayAccessData(sas, (void HUGEP**)&avs);
 if (FAILED(hr)) goto Cleanup;
 if (!avs) { hr = E_INVALIDARG; goto Cleanup; } 

 if (selem > VARSIZE)
   if (selem == sizeof(tvs))
     {
      pvs = &tvs;
      if (vts == VT_VARIANT) fvts = &V_VT(&tvs); 
      else if (vts != VT_DECIMAL) 
        { hr = DISP_E_TYPEMISMATCH; goto Cleanup; } 
     }
   else { hr = DISP_E_TYPEMISMATCH; goto Cleanup; } 
 
 if (lcid)
   {
    do {
        memcpy(pvs, avs, selem); avs += selem;
        V_VT(&tvs) = *fvts;
        hr = VariantChangeTypeEx(&tvd, &tvs, lcid, flags, vtd);
        if (FAILED(hr)) break;
        memcpy(avd, pvd, delem); avd += delem;
        V_VT(&tvd) = VT_EMPTY;
       }
    while(--alen);
   }
 else
   {
    do {
        memcpy(pvs, avs, selem); avs += selem;
        V_VT(&tvs) = *fvts;
        hr = VariantChangeType(&tvd, &tvs, flags, vtd);
        if (FAILED(hr)) break;
        memcpy(avd, pvd, delem); avd += delem;     
        V_VT(&tvd) = VT_EMPTY;
       }
    while(--alen);
   }

Cleanup:
 VariantClear(&tvd);
 if (!FAILED(hrd)) SafeArrayUnaccessData(sad);
 if (!FAILED(hrs)) SafeArrayUnaccessData(sas);
 if (!FAILED(hr))
   {
    VariantClear(dst);
    V_ARRAY(dst) = sad;
    V_VT(dst) = vt;
   }
 else SafeArrayDestroyData(sad);

 return hr;
 }
}

/* end of charray.c */
