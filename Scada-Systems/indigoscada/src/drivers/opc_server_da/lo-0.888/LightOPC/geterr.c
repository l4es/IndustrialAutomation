/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko, Kostya Volovich              *
                                                                          *
 Error descriptions
 **************************************************************************/

#include <windows.h>
#include <winerror.h>

#include <wchar.h>
#include <opcerror.h>
#include "loserv.h"
#include "util.h"
#include LO_OLE_ERRORS_h
#include <olectl.h>

#if 1
#define LO_ERR(err,derr) HRESULT lo##err = err
#else
#define LO_ERR(err,derr) HRESULT lo##err = derr
#endif
LO_ERR(OPC_E_NOTFOUND, E_FAIL);
LO_ERR(OPC_E_INVALIDHANDLE, E_FAIL);
LO_ERR(OPC_S_CLAMP, S_OK);
#undef LO_ERR

typedef struct err_descr
{
	int hresult;
	unsigned size;
	const char *text;
} err_descr;

#define ERR_TEXT(text) SIZEOF_ARRAY(text), text
#define ERR_DESCR(name,text) { name, ERR_TEXT(text " /" #name) }

static const err_descr descr[] = {
/**************** OPC codes from opcerror.h *********************************/
ERR_DESCR(OPC_E_INVALIDHANDLE, "The value of the handle is invalid."),
ERR_DESCR(OPC_E_BADTYPE, "The server cannot convert the data between the "
			 "requested data type and the canonical data type."),
ERR_DESCR(OPC_E_PUBLIC, "The requested operation cannot be done on a public group."),
ERR_DESCR(OPC_E_BADRIGHTS, "The Items AccessRights do not allow the operation."),
ERR_DESCR(OPC_E_UNKNOWNITEMID, "The item is no longer available in the server address space"),
ERR_DESCR(OPC_E_INVALIDITEMID, "The item definition doesn't conform to the server's syntax."),
ERR_DESCR(OPC_E_INVALIDFILTER, "The filter string was not valid"),
ERR_DESCR(OPC_E_UNKNOWNPATH, "The item's access path is not known to the server."),
ERR_DESCR(OPC_E_RANGE, "The value was out of range."),
ERR_DESCR(OPC_E_DUPLICATENAME, "Duplicate name not allowed."),
ERR_DESCR(OPC_S_UNSUPPORTEDRATE, "The server does not support the requested data rate "
		                 "but will use the closest available rate."),
ERR_DESCR(OPC_S_CLAMP, "A value passed to WRITE was accepted but the output was clamped."),

#ifdef OPC_E_INVALID_PID /* OPC-DA v2 */
ERR_DESCR(OPC_S_INUSE, "The operation cannot be completed because the "
		       "object still has references that exist."),
ERR_DESCR(OPC_E_INVALIDCONFIGFILE, "The server's configuration file is an invalid format."),
ERR_DESCR(OPC_E_NOTFOUND, "The server could not locate the requested object."),
ERR_DESCR(OPC_E_INVALID_PID, "The server does not recognise the passed property ID."),
#endif

#ifdef OPC_E_NOTSUPPORTED /* OPC-DA v3 */
ERR_DESCR(OPC_E_DEADBANDNOTSET, "The item deadband has not been set for this item."),
ERR_DESCR(OPC_E_DEADBANDNOTSUPPORTED, "The item does not support deadband."),
ERR_DESCR(OPC_E_NOBUFFERING, "The server does not support buffering of data items "
          "that are collected at a faster rate than the group update rate."),
ERR_DESCR(OPC_E_INVALIDCONTINUATIONPOINT, "The continuation point is not valid."),
ERR_DESCR(OPC_S_DATAQUEUEOVERFLOW, "Not every detected change has been returned since "
          "the server's buffer reached its limit and had to purge out the oldest data."),
ERR_DESCR(OPC_E_RATENOTSET, "There is no sampling rate set for the specified item."),
ERR_DESCR(OPC_E_NOTSUPPORTED, "The server does not support writing of quality "
          "and/or timestamp."),
#endif

/**************** common codes ***************************************/
ERR_DESCR(E_FAIL, "The operation failed."),
ERR_DESCR(E_UNEXPECTED, "Catastrophic failure."),
ERR_DESCR(E_OUTOFMEMORY, "Not enough memory."),
ERR_DESCR(E_INVALIDARG, "An argument to the function was invalid."),
ERR_DESCR(E_NOINTERFACE, "The interface (riid) asked for is not supported by server."),
ERR_DESCR(E_NOTIMPL, "The requested method is not implemented."),
ERR_DESCR(E_POINTER, "Invalid pointer."),
ERR_DESCR(DISP_E_BADINDEX, "The specified index is invalid."),
ERR_DESCR(DISP_E_BADVARTYPE, "The input parameter is not a valid type of variant."),
ERR_DESCR(DISP_E_OVERFLOW, "The data pointed to by the output parameter "
                           "does not fit in the destination type."),
ERR_DESCR(DISP_E_TYPEMISMATCH, "The argument could not be coerced to the specified type."),
/*<olectl.h>*/
ERR_DESCR(CONNECT_E_NOCONNECTION, "The specified CONNECTION is not a valid connection."),
ERR_DESCR(CONNECT_E_ADVISELIMIT, "Advise limit exceeded for this object."),
ERR_DESCR(CONNECT_E_CANNOTCONNECT, "The SINK does not support the interface required "
                                   "by this connection point."),
ERR_DESCR(OLE_E_NOCONNECTION, "Cannot Unadvise - there is no such connection."),
/*<winerror.h>*/
ERR_DESCR(DV_E_FORMATETC, "Invalid or unregistered Format specified in FORMATEC."),

ERR_DESCR(S_FALSE, "The operation partially successed."),
ERR_DESCR(S_OK, "The operation successed."),
{ -1, ERR_TEXT("<UNKNOWN ERROR>") } /* MANDATORY TERMINATOR */
};


const char *loStrError(HRESULT ecode)
{
 const err_descr *ed = descr;
 while(ecode != (HRESULT)ed->hresult && ed->hresult != -1) ed++;
 return ed->text;
}

unsigned loGetErrorString(loWchar *buf, unsigned wcsize,
                          HRESULT ecode, LCID locale)
{
 const err_descr *ed; 
 char *smsg = 0;
 loWchar *wmsg = 0;
 unsigned mlen = 0;

 for(ed = descr; ed->hresult != -1; ed++)
   if (ecode == (HRESULT)ed->hresult) break;

 mlen = ed->size - 1;
 if (ed->hresult != -1) goto Finish;

#define FMT_MSG_ARG(ecode,locale,str) FORMAT_MESSAGE_FROM_SYSTEM|\
                                      FORMAT_MESSAGE_IGNORE_INSERTS| \
                                      FORMAT_MESSAGE_ALLOCATE_BUFFER, \
                                      NULL, ecode, locale, str, 0, NULL

 mlen = FormatMessageW(FMT_MSG_ARG(ecode, locale, (LPWSTR)&wmsg));
 if ((!mlen || !wmsg) && locale)
   {
    if (wmsg) LocalFree(wmsg);
    wmsg = 0;
    mlen = FormatMessageW(FMT_MSG_ARG(ecode, 0/*safe locale*/, (LPWSTR)&wmsg));
   }

 if (wmsg)
   {           /* are we sure the wchar_t is unsigned? */
    while(mlen && wmsg[mlen-1] <= L' ' && 
                  wmsg[mlen-1] >= 0) mlen--;
    if (mlen) goto Finish;
    LocalFree(wmsg);
   }
 wmsg = 0; mlen = 0; /* Failed to get a wide string */
/* 2nd try: On w95/98 we haven't FormatMessageW()... */
/* for narrow strings a locale may be useless */
 mlen = FormatMessageA(FMT_MSG_ARG(ecode, locale, (LPSTR)&smsg));
 if ((!mlen || !smsg) && locale)
   {
    if (smsg) LocalFree(smsg);
    smsg = 0;
    mlen = FormatMessageA(FMT_MSG_ARG(ecode, 0/*safe locale*/, (LPSTR)&smsg));
   }

 if (smsg)
   {
    while(mlen && ' ' >= (unsigned char)smsg[mlen-1]) mlen--;
    if (mlen) goto Finish;
    LocalFree(smsg); 
   }
 smsg = 0; mlen = 0;

Finish:

 if (buf && wcsize)
   {
    unsigned tlen = mlen < wcsize? mlen: wcsize - 1;

    if (wmsg) memcpy(buf, wmsg, tlen * sizeof(loWchar));  
    else 
      {
/* This is for w95 again */
        if (smsg)
          {
            unsigned slen = tlen;
            do tlen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, 
                                       smsg, slen, buf, wcsize-1);
            while(!tlen && --slen);
            if (mlen < tlen) mlen = tlen;
          }
        else tlen = 0;
        if (!tlen)
          {
            mlen = ed->size - 1;
            tlen = mlen < wcsize? mlen: wcsize - 1;
            tlen = lo_mbstowcs(buf, ed->text, tlen);
            if (0 > (int)tlen) tlen = 0;
          }
      }

    wcsize -= tlen;  /* clean line-breaks produced by FormatMessage() */
    for(; tlen--; buf++) 
      if (0 <= *buf && *buf <= L' ') *buf = L' ';
    *buf = 0;

    if (wcsize)
      {
//     ecode &= 0xffffffff;
       _snwprintf(buf, wcsize, L" =0x%lX(%ld)", (long)ecode, (long)ecode);
       buf[wcsize-1] = 0;
      }
   }

 if (wmsg) LocalFree(wmsg);
 if (smsg) LocalFree(smsg);

 return mlen + 
        sizeof(long) * 5 + /* 2 haxadec + 3 dec digits per byte */
        sizeof(" =0x()");  /* separators */
}

HRESULT loComErrorString(loService *se, const loCaller *cactx,
                         HRESULT dwError, LCID dwLocale, loWchar **ppString)
{
 loWchar buf[256];
 loWchar *abuf = 0;
 unsigned size;
 unsigned asize = 0;

 if (!ppString) return E_INVALIDARG;
 *ppString = 0;
 if (!loSERVICE_OK(se)) return LO_E_SHUTDOWN;
 if (!cactx) cactx = &se->cactx;
 
 buf[SIZEOF_ARRAY(buf)-1] = 0;
 if (!se->driver.ldGetErrorString ||
     0 == (size = se->driver.ldGetErrorString(cactx, dwError, dwLocale,
                                            buf, SIZEOF_ARRAY(buf)-1)) )
   size = loGetErrorString(buf, SIZEOF_ARRAY(buf), dwError, dwLocale);

 if (size >= SIZEOF_ARRAY(buf))
   if (abuf = (loWchar*)loComAlloc((size + 2) * sizeof(loWchar)))
     {
      if (se->driver.ldGetErrorString &&
          (asize = se->driver.ldGetErrorString(cactx, dwError, dwLocale,
                                               abuf, size + 1)) ||
          (asize = loGetErrorString(abuf, size + 1, dwError, dwLocale)))
        {
         goto Finish;
        }
     }
   else size = SIZEOF_ARRAY(buf) - 1; 
 else if (!size) size = SIZEOF_ARRAY(buf) - 1;

 if (!abuf &&
    !(abuf = (loWchar*)loComAlloc((size + 1) * sizeof(loWchar))))
   return E_OUTOFMEMORY;
 memcpy(abuf, buf, size * sizeof(loWchar));

Finish:
 abuf[size] = 0;
 *ppString = abuf;

 return S_OK;
}

/* end of geterr.cpp */
