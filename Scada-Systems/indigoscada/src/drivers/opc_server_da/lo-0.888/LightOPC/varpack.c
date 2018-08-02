/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko                               *
                                                                          *
 Variant packing for OPC v1
 **************************************************************************/

#include <windows.h>
#include <ole2.h>
#include <stddef.h> /* offsetof() */
#include "util.h"

unsigned loVariantSize(VARIANT *var)
{
 unsigned extra = 0;

 if (V_ISARRAY(var))
   {
    SAFEARRAY *sa;
    extra = offsetof(SAFEARRAY, rgsabound[1]);
    if (sa = V_ARRAY(var))
      {
       unsigned elems = sa->rgsabound[0].cElements;

       if ((VT_BSTR|VT_ARRAY) != V_VT(var))
         extra += sa->cbElements * elems;
       else
         {
          HRESULT hr;
          BSTR *bs;

          hr = loSA_AccessData(sa, (void **)&bs);
          if (FAILED(hr))
            {
             UL_ERROR((LOGID, "%!l loVariantSize(%x)::SafeArrayAccessData()",
                             hr, V_VT(var)));
             bs = (BSTR*)sa->pvData;
            }
          while(elems--)
            {
             extra += loBSTR_SIZE(*bs) + 2 + sizeof(DWORD);
             bs++;
            }
          if (!FAILED(hr)) SafeArrayUnaccessData(sa);
         }
      }
    else UL_ERROR((LOGID, "loVariantSize(%x)::SafeArray = null", V_VT(var)));
   }
 else if (VT_BSTR == V_VT(var))
   {
    extra = loBSTR_SIZE(V_BSTR(var)) + 2 + sizeof(DWORD);
   }
 return extra + sizeof(VARIANT);
}

unsigned loVariantPack(void **bufp, VARIANT *var)
{
 char *buf = (char*)*bufp;
 char *buf0 = buf;

#if defined(__cplusplus) && defined(_MSC_VER) /*&& _MSC_VER <= 1200*/
#define ADV_PTR(TYPE,ptr,val) (*((TYPE*)ptr) = val, ptr += sizeof(TYPE))
#else
#define ADV_PTR(TYPE,ptr,val) (*((TYPE*)ptr)++ = val)
#endif

 ADV_PTR(VARIANT, buf, *var);
 if (V_ISARRAY(var))
   {
    HRESULT hr;
    BSTR *bs;
    unsigned zz = offsetof(SAFEARRAY, rgsabound[1]);
    SAFEARRAY *sa = V_ARRAY(var);

    hr = loSA_AccessData(sa, (void **)&bs);
    if (FAILED(hr))
      {
       memset(buf, 0, zz); buf += zz;
       UL_ERROR((LOGID, "%!l loVariantPack(%x)::SafeArrayAccessData()",
                            hr, V_VT(var)));
      }
    else
      {
       unsigned elems = sa->rgsabound[0].cElements;

       memcpy(buf, sa, zz);
       ((SAFEARRAY*)buf)->pvData = 0;
       buf += zz;

       if ((VT_BSTR|VT_ARRAY) != V_VT(var))
         memcpy(buf, bs, zz = sa->cbElements * elems), buf += zz;
       else
         {
          unsigned xx = elems;
          while(xx--)
            {
             if (*bs)
               {
                zz = loBSTR_SIZE(*bs);
                ADV_PTR(DWORD, buf, zz);
                memcpy(buf, *bs, zz); buf += zz;
               }
             else ADV_PTR(DWORD, buf, 0);
             ADV_PTR(WCHAR, buf, 0);
             bs++;
            }
         }
       SafeArrayUnaccessData(sa);
      }
   }
 else if (VT_BSTR == V_VT(var))
   {
    BSTR bs;
    if (bs = V_BSTR(var))
      {
       unsigned zz = loBSTR_SIZE(bs);
       ADV_PTR(DWORD, buf, zz);
       memcpy(buf, bs, zz); buf += zz;
      }
    else ADV_PTR(DWORD, buf, 0);
    ADV_PTR(WCHAR, buf, 0);
   }
 *bufp = buf;
 return buf - buf0;
}

/* end of varpack.c */
