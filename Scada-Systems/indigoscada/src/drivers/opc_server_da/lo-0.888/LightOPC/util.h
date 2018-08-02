/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko, Kostya Volovich            *
 **************************************************************************/

#ifndef UTIL_H
#define UTIL_H

/**************************************************************************
 Miscelanous Internal utilites
 **************************************************************************/
#ifndef OPTIONS_H
#include "options.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SIZEOF_ARRAY
#define SIZEOF_ARRAY(array) (sizeof(array)/sizeof((array)[0]))
#endif

        /* malloc() wrappers: ***********************/

int  mallocX_trap(void); /* fo debugging only */
void *mallocX(unsigned size);
void freeX(void *buf);
void *reallocX(void *buf, unsigned size);
void *callocX(unsigned num, unsigned size);
void *preallocX(void **buf, unsigned size); /* changes *buf */

#if 0 != LO_USE_MALLOCX
extern long mallocX_count;
#else
#define mallocX  malloc
#define freeX    free
#define reallocX realloc
#define callocX  calloc
#endif

extern long lo_X_objcount;


#define loComAlloc   CoTaskMemAlloc
#define loComFree    CoTaskMemFree
#define loComRealloc CoTaskMemRealloc

#if defined(_WCHAR_T_DEFINED) || defined(WEOF) /*|| defined(WCHAR_MIN) */

#if !defined(LIGHTOPC_H) || !defined(__GNUC__)
typedef wchar_t loWchar;
#endif

int lo_wcshash(const loWchar *ws);
int lo_wcsihash(const loWchar *ws);

int lo_wcsnhash(const loWchar *ws, unsigned len);
int lo_wcsnihash(const loWchar *ws, unsigned len);

int lo_mbstowcs(loWchar *wcs, const char *mbs, int nn);
int lo_wcstombs(char *mbs, unsigned mb_len, const loWchar *wcs, unsigned wc_len);
loWchar *loWstrdup(const loWchar *str);
loWchar *loMWstrdup(const char *str);
loWchar *loWstrndup(const loWchar *str, unsigned len);

int loPWstrdup(loWchar **ps, const loWchar *str); /* returns 0/-1 */

loWchar *loComWstrdup(const loWchar *str);
loWchar *loComMWstrdup(const char *str);
loWchar *loComWstrndup(const loWchar *str, unsigned len);

int lo_name_level(loWchar brsep, const loWchar *name);
int lo_name_levlen(loWchar brsep, const loWchar *name, int level);

void *lo_string_to_struct(void *allocated, unsigned strusize, const loWchar *str);

/* Items' Properies helpers: */
const char *lo_prop_descr(unsigned propid);
loWchar *lo_ComPropDescr(unsigned propid, const char *pd);

const loWchar *loWnul(const loWchar *str);
#endif /* WEOF */
const char *loSnul(const char *str);


/**************************************************************************/
#ifdef WINAPI

char *loTranslateIID(char *buf, unsigned size, const GUID *iface);

#define loFT_GROUP(ft) ((ft).dwLowDateTime)
#define loFT_SUBST(ft,master) ((master)[loFT_GROUP(ft)])

#define IsFILETIME(ft) ((ft).dwHighDateTime /*| (ft).dwLowDateTime*/)
#define IsFILETIME_EQ(ft1,ft2) ((ft1).dwLowDateTime == (ft2).dwLowDateTime && \
                                (ft1).dwHighDateTime == (ft2).dwHighDateTime)

double lo_filetime_to_variant(const FILETIME *ft);
void   lo_variant_to_filetime(FILETIME *ft, double vd);

extern HRESULT loOPC_E_NOTFOUND,
               loOPC_E_INVALIDHANDLE,
               loOPC_S_CLAMP;

/* #define LO_E_0COUNT    (S_FALSE) 0 == dwCount is always E_INVALIDARG */
#define LO_E_NOTACTIVE (S_FALSE)
#define LO_E_SHUTDOWN  (E_UNEXPECTED/*E_FAIL*/)
#if 1
#define LO_E_BADTYPE(hr)  (hr)
#else
#define LO_E_BADTYPE(hr)  (OPC_E_BADTYPE)
#endif

#if 1
#define VARIANTCHANGE(dst,src,lcid,flags,type) \
         VariantChangeType((dst),(src),(flags),(type))
#else
#define VARIANTCHANGE(dst,src,lcid,flags,type) \
        VariantChangeTypeEx((dst),(src),(lcid),(flags),(type))
#endif
#define VARIANTCHANGEt(dst,src,vcfl,type) \
        VARIANTCHANGE((dst),(src),(vcfl).vc_lcid,(vcfl).vc_tfl,(type))


#define LO_E_BADTYPE_QUAL(hr,err,qual)  do {  \
    if (!FAILED((err))) (err) = (hr); \
    if (OPC_QUALITY_BAD != (OPC_QUALITY_MASK & (qual))) \
      (qual) = OPC_QUALITY_BAD;            } while(0)

#if 0
#define VAR_CHANGETYPE(to,from,reqt)  \
       (VT_EMPTY == (reqt) ||         \
        (reqt) == V_VT((from)) ||     \
          VT_EMPTY == V_VT((from))  ? \
        VariantCopy((to), (from)):    \
        VariantChangeType((to), (from), 0, (reqt)))
#else
#undef VAR_CHANGETYPE
#endif

#ifdef   VARIANT_NOVALUEPROP /* VARIANT defined */
unsigned loVariantPack(void **bufp, VARIANT *var);
unsigned loVariantSize(VARIANT *var);

#endif /* VARIANT defined */

#if 0
#define loBSTR_SIZE(s)  ((s)? SysStringByteLen(s): 0)
#else
#define loBSTR_SIZE(s)  ((s)? wcslen(s)*sizeof(loWchar): 0)
#endif

#if 0
#define loSA_AccessData(s,p)  ((s)? SafeArrayAccessData((s),(p)): E_POINTER)
#else
#define loSA_AccessData(s,p)       (SafeArrayAccessData((s),(p)))
#endif

/********************************************/

#if 0
const loWchar *loWStrError(HRESULT code); /* very private */
#else
const char *loStrError(HRESULT code); /* very private */
#endif

unsigned loGetErrorString(loWchar *buf, unsigned wcsize, HRESULT ecode, LCID locale);
/*  Translate the some error codes (mostly from opcerror.h) to strings.
    Unknown codes translated to numeric string.
    The wcsize is the size of buf in characters not bytes.
    In case of buf or wcsize are 0 the calculated size of the string returned.
    Return value: <number of characters in the translated string>
    not counting terminating NUL.
*/

#ifdef LIGHTOPC_H

HRESULT loComErrorString(loService *ls, const loCaller *cactx,
			 HRESULT dwError, LCID dwLocale, loWchar **ppString);
/*  Translate error codes using loService::loDriver::ldGetErrorString(), then (if failed)
    loGetErrorString(). Returned string allocated by CoTaskMemAlloc(). */

#endif /* LIGHTOPC_H */

void loComFree_LPWSTR(LPWSTR *ws, unsigned count);

#endif /* WINAPI **************************************************/

/********************************************/
#if 0 == LO_AUTO_ALLOC
#define loAA_DECLARE(size)
#define loAA_ALLOC(size)       mallocX(size)
#define loAA_FREE(ptr)         freeX(ptr)
#else
#define loAA_DECLARE(size)     char aa[(size) * LO_AUTO_ALLOC]
#define loAA_ALLOC(size)       ((size) <= sizeof(aa)? (void*)aa: mallocX(size))
#define loAA_FREE(ptr)         (aa == (char*)(ptr)? (void)0: freeX(ptr))
#endif

/**************************************************************************/

#ifdef __cplusplus
	}

#if LO_USE_IAM_CHECK
#define LO_IAM_DECLARE(type) type *iam
#define LO_IAM_INITIATE()         (iam = this)
#define LO_IAM_INVALIDATE()       (iam = 0)
#define LO_IAM_DISCONNECT()       (LO_IAM_INVALIDATE(),LO_CO_DISCONNECT(this,0))
#define LO_IAM_ISINVALID()        (iam != this)
#define LO_IAM_CHECK(ret) do { if (LO_IAM_ISINVALID()) return ret; } while(0)
#else
#define LO_IAM_DECLARE(type)
#define LO_IAM_INITIATE()
#define LO_IAM_INVALIDATE()
#define LO_IAM_DISCONNECT()
#define LO_IAM_ISINVALID()        (0)
#define LO_IAM_CHECK(ret)
#endif

#endif /*__cplusplus*/

#endif /*UTIL_H*/
