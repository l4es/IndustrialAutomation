/* Copyright (c) 1996-2017, OPC Foundation. All rights reserved.

   The source code in this file is covered under a dual-license scenario:
     - RCL: for OPC Foundation members in good-standing
     - GPL V2: everybody else

   RCL license terms accompanied with this source code. See http://opcfoundation.org/License/RCL/1.00/

   GNU General Public License as published by the Free Software Foundation;
   version 2 of the License are accompanied with this source code. See http://opcfoundation.org/License/GPLv2

   This source code is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <opcua.h>
#include <opcua_memory.h>

#include <opcua_string.h>

#define OPCUA_STRING_PARANOID_MEMORY 1

/*============================================================================
* Allows to separate a OpcUa_String from a C string.
*===========================================================================*/
#define OpcUa_uiMagic       0

/*============================================================================
* The internal string representation. Take care of sizeof(OpcUa_String)!
*===========================================================================*/
typedef struct _OpcUa_StringInternal
{
    OpcUa_UInt          uMagic          : 8;    /* ==0x00 -> Ua String, != 0x00 -> C-String, an empty string result in a null pointer! */
    OpcUa_UInt          bFreeSecondMem  : 1;    /* strContent is a Pointer to the string */
    OpcUa_UInt          uReserved       : 7;
    OpcUa_UInt32        uLength;                /* Length without terminating '\0' */
    OpcUa_CharA*        strContent;             /* Pointer or start of the string or 4 first unsignend chars (mind bFreeSecondMem) */
} OpcUa_StringInternal, *OpcUa_pStringInternal;


/*============================================================================
* Get a pointer to the first character of the content.
*===========================================================================*/
#define _OpcUa_String_GetRawString(x)   ( (((OpcUa_StringA)x)[0]=='\0')?(OpcUa_StringA)(((OpcUa_pStringInternal)x)->strContent):((OpcUa_StringA)x))

/*============================================================================
* Cast a C string into a OpcUa_String.
*===========================================================================*/
#if !OPCUA_PREFERINLINE
    OpcUa_String* OpcUa_String_FromCString( OpcUa_StringA   a_strCString )
    {
        if(a_strCString == OpcUa_Null)
        {
            return OpcUa_Null;
        }
        if(a_strCString[0]==0x00)
        {
            return OpcUa_Null;
        }
        return (OpcUa_String*) a_strCString;
    }
#endif /* OPCUA_PREFERINLINE */


/*============================================================================
* Check if argument is OpcUa_String instance.
*===========================================================================*/
#if OPCUA_PREFERINLINE
    #define _OpcUa_IsUaString(x)    (x==OpcUa_Null?OpcUa_False:((char*)x)[0]=='\0'?OpcUa_True:OpcUa_False)
#else /* OPCUA_PREFERINLINE */
    OpcUa_Boolean _OpcUa_IsUaString(const OpcUa_Void* a_strCString)
    {
        if(a_strCString == OpcUa_Null)
        {
            return OpcUa_False;
        }

        if(((OpcUa_StringA)a_strCString)[0]==0x00)
        {
            return OpcUa_True;
        }

        return OpcUa_False;
    }
#endif /* OPCUA_PREFERINLINE */


/*============================================================================
* Enlarge the buffer of the given string.
*===========================================================================*/
static OpcUa_StatusCode _OpcUa_ExpandString(  /* bi */ OpcUa_String*    a_pString,
                                              /* in */ OpcUa_UInt32     a_uNewLen,
                                              /* bi */ OpcUa_UInt32*    a_puLimitLen,
                                              /* in */ OpcUa_Boolean    a_bAutoExpand)
{
    OpcUa_pStringInternal   pStringInt  = (OpcUa_pStringInternal) a_pString;
    OpcUa_UInt32            uiSrcLen    = a_uNewLen;
    OpcUa_UInt32            uiDestLen   = 0;
    OpcUa_UInt32            uiLimitLen  = 0;
    OpcUa_StringA           strRaw      = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_String);

    uiDestLen = OpcUa_String_StrSize(a_pString); /* we do not get the real buffer size for comparison, only the content length! */

    /* get the needed memory space from the maximum of source and destination length */
    uiLimitLen = (uiSrcLen > uiDestLen)         ? uiSrcLen   : uiDestLen;
    /* get the final memory space from the minimum of needed length and given upper limit */
    uiLimitLen = (uiLimitLen < (*a_puLimitLen)) ? uiLimitLen : (*a_puLimitLen);
    /* update out parameter */
    (*a_puLimitLen) = uiLimitLen;

    if(a_bAutoExpand == OpcUa_False)
    {
        /* return after calculation */
        return OpcUa_Good;
    }

    if(uiLimitLen > uiDestLen)
    {
        /* we have to expand the stream */

        /* allocate the new memory and copy the content */
        strRaw = (OpcUa_StringA)OpcUa_Alloc(uiLimitLen+1);
        OpcUa_ReturnErrorIfAllocFailed(strRaw);
        OpcUa_MemCpy(strRaw, uiDestLen, _OpcUa_String_GetRawString(a_pString), uiDestLen);
        strRaw[uiDestLen]='\0';

        /* free the old content */
        if(pStringInt->bFreeSecondMem != OpcUa_False)
        {
            if(pStringInt->strContent != OpcUa_Null)
            {
                OpcUa_Free((OpcUa_Void*)pStringInt->strContent);
            }
        }

        pStringInt->strContent      = strRaw;
        pStringInt->bFreeSecondMem  = OpcUa_True;
        pStringInt->uLength         = uiLimitLen;
    }

    return OpcUa_Good;
}


/*============================================================================
 * Initialize a string.
 *===========================================================================*/
OpcUa_StatusCode OpcUa_String_Initialize( /*  bi */ OpcUa_String* a_pString )
{
    OpcUa_pStringInternal   pStringInt  = (OpcUa_pStringInternal)   a_pString;

    pStringInt->uMagic          = OpcUa_uiMagic;
    pStringInt->bFreeSecondMem  = OpcUa_True;
    /*pStringInt->uReserved       = 0;*/
    pStringInt->uLength         = 0;
    pStringInt->strContent      = OpcUa_Null;

    return OpcUa_Good;
}

/*============================================================================
* Create a new OpcUa_String
*===========================================================================*/
OpcUa_StatusCode OpcUa_String_CreateNewString(   /*  in */ OpcUa_StringA            a_strSource,
                                                 /*  in */ OpcUa_UInt32             a_uLength,
                                                 /*  in */ OpcUa_UInt32             a_uBufferSize,
                                                 /*  in */ OpcUa_Boolean            a_bDoCopy,
                                                 /*  in */ OpcUa_Boolean            a_bFreeOnClear,
                                                 /* out */ OpcUa_String**           a_ppNewString)
{
    OpcUa_pStringInternal   pStringInt  = OpcUa_Null;
    OpcUa_StatusCode        retVal      = OpcUa_Good;
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_String);

OpcUa_ReturnErrorIfArgumentNull(a_ppNewString);

   /* ToDo: maybe remove this parameter; its unlikely to be used at all */
    OpcUa_ReferenceParameter(a_uBufferSize);

    /* clar if prior string exists */
    *a_ppNewString = OpcUa_Null;

    /* Use length of source, must be \0ed */
    if(a_uLength == OPCUA_STRINGLENZEROTERMINATED)
    {
        if(a_strSource != OpcUa_Null )
        {
            a_uLength = OpcUa_P_String_StrLen(a_strSource);
        }
        else
        {
            return OpcUa_BadInvalidArgument;
        }
    } /* else use parameter value */

    /* parameter consistency check */
    if(a_strSource == OpcUa_Null && a_uLength != 0)
    {
        return OpcUa_BadInvalidArgument;
    }

    /* append content to string struct if a copy is wished AND the source is not null */
    if(a_bDoCopy != OpcUa_False && a_strSource != OpcUa_Null)
    {
        pStringInt = (OpcUa_pStringInternal) OpcUa_Alloc(a_uLength + 1 + sizeof(OpcUa_StringInternal));
        OpcUa_ReturnErrorIfAllocFailed(pStringInt);
#ifdef OPCUA_STRING_PARANOID_MEMORY
        OpcUa_MemSet(pStringInt, 0, (a_uLength + 1 + sizeof(OpcUa_StringInternal)));
#endif /* OPCUA_STRING_PARANOID_MEMORY */
        pStringInt->strContent = (OpcUa_StringA) pStringInt + sizeof(OpcUa_StringInternal);
    }
    else
    {
        pStringInt = (OpcUa_pStringInternal) OpcUa_Alloc(sizeof(OpcUa_StringInternal));
        OpcUa_ReturnErrorIfAllocFailed(pStringInt);
    }

    pStringInt->uMagic          = OpcUa_uiMagic;
    /*pStringInt->uReserved       = 0;*/
    pStringInt->uLength         = a_uLength;

    if(a_strSource != OpcUa_Null)
    {
        if(a_bDoCopy != OpcUa_False)
        {
            /* we're copying */
            pStringInt->bFreeSecondMem  = OpcUa_False;

            if(a_uLength != 0)
            {
                retVal = OpcUa_P_String_StrnCpy(    pStringInt->strContent,
                                                    a_uLength,
                                                    a_strSource,
                                                    a_uLength);
            }
        }
        else
        {
            /* do not copy and free external source if wanted */
            pStringInt->bFreeSecondMem  = a_bFreeOnClear;
            pStringInt->strContent      = a_strSource;
        }
    }
    else
    {
        pStringInt->strContent = OpcUa_Null;
        pStringInt->bFreeSecondMem  = OpcUa_False;
    }

    (*a_ppNewString) = (OpcUa_String*)pStringInt;

    return retVal;
}


/*============================================================================
*
*===========================================================================*/
OpcUa_StatusCode OpcUa_String_AttachToString(  /* in */ OpcUa_StringA              a_strSource,
                                               /* in */ OpcUa_UInt32               a_uLength,
                                               /* in */ OpcUa_UInt32               a_uBufferSize,
                                               /* in */ OpcUa_Boolean              a_bDoCopy,
                                               /* in */ OpcUa_Boolean              a_bFreeOnClear,
                                               /* bi */ OpcUa_String*              a_pNewString)
{
    OpcUa_pStringInternal   pStringInt = (OpcUa_pStringInternal)a_pNewString;
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_String);

    OpcUa_ReferenceParameter(a_uBufferSize);

    OpcUa_ReturnErrorIfArgumentNull(a_strSource);
    OpcUa_ReturnErrorIfArgumentNull(a_pNewString);

    OpcUa_String_Clear(a_pNewString);

    if(a_uLength == OPCUA_STRINGLENZEROTERMINATED)
    {
        a_uLength = OpcUa_P_String_StrLen(a_strSource);
    }

    pStringInt->uMagic          = OpcUa_uiMagic;
    /*pStringInt->uReserved       = 0;*/
    pStringInt->bFreeSecondMem  = a_bFreeOnClear;
    pStringInt->uLength         = a_uLength;

    if(a_bDoCopy != OpcUa_False)
    {
        /* attach copied string, free it on clearing */
        pStringInt->bFreeSecondMem = OpcUa_True;
        pStringInt->strContent     = (OpcUa_StringA)OpcUa_Alloc(a_uLength+1);

        OpcUa_ReturnErrorIfAllocFailed(pStringInt->strContent);

        OpcUa_MemCpy(    pStringInt->strContent,
                                a_uLength,
                                a_strSource,
                                a_uLength);

        pStringInt->strContent[a_uLength]   = '\0';
    }
    else
    {
        /* attach external string; memory does get freed on request! */
        pStringInt->bFreeSecondMem  = a_bFreeOnClear;
        pStringInt->strContent      = a_strSource;
    }


    a_pNewString = (OpcUa_String*)pStringInt;

    return OpcUa_Good;
}


/*============================================================================
 * Free memory for string
 *===========================================================================*/
OpcUa_Void OpcUa_String_Delete(OpcUa_String** a_ppString)
{
    OpcUa_pStringInternal pStringInt = OpcUa_Null;

    if(a_ppString == OpcUa_Null)
    {
        return;
    }

    /* cast to internal representation */
    pStringInt = (OpcUa_pStringInternal)*a_ppString;

    /* if it isnt a OpcUa_String object, leave... */
    if(_OpcUa_IsUaString((OpcUa_StringA)pStringInt) == OpcUa_False)
    {
        return;
    }

    /* free referenced memory if needed and possible. */
    if(pStringInt->bFreeSecondMem != OpcUa_False && pStringInt->strContent != OpcUa_Null)
    {
        OpcUa_Free(pStringInt->strContent);
    }

    OpcUa_Free(pStringInt);

    /* reset pointer */
    (*a_ppString) = OpcUa_Null;
}


/*============================================================================
*
*===========================================================================*/
OpcUa_Void OpcUa_String_Clear(OpcUa_String* a_pString)
{
    OpcUa_pStringInternal pStringInt = (OpcUa_pStringInternal) a_pString;

    if(_OpcUa_IsUaString( (OpcUa_StringA) a_pString) == OpcUa_False)
    {
        return;
    }

    if(pStringInt->bFreeSecondMem != OpcUa_False)
    {
        OpcUa_Free((OpcUa_Void*)(pStringInt->strContent));
#ifdef OPCUA_STRING_PARANOID_MEMORY
        pStringInt->strContent = OpcUa_Null;
#endif /* OPCUA_STRING_PARANOID_MEMORY */
    }

    OpcUa_String_Initialize(a_pString);
}


/*============================================================================
* Get pointer to internal raw string.
*===========================================================================*/
OpcUa_CharA* OpcUa_String_GetRawString(const OpcUa_String* a_pString)
{
    return _OpcUa_String_GetRawString(a_pString);
}

/*============================================================================
* Check if string is empty.
*===========================================================================*/
OpcUa_Boolean OpcUa_String_IsEmpty(const OpcUa_String* a_pString)
{
    OpcUa_pStringInternal   pStringInt  = (OpcUa_pStringInternal)a_pString;

    if(pStringInt == OpcUa_Null)
    {
        return OpcUa_False; /* a null string is not empty! */
    }

    if(((OpcUa_StringA)pStringInt)[0] == 0x00)
    {
        if(pStringInt->strContent == OpcUa_Null)
        {
            return OpcUa_False; /* a null string is not empty! */
        }

        if(pStringInt->uLength == 0)
        {
            return OpcUa_True;
        }
    }
    return OpcUa_False;
}


/*============================================================================
* Check if string is null.
*===========================================================================*/
OpcUa_Boolean OpcUa_String_IsNull(const OpcUa_String* a_pString)
{
    OpcUa_pStringInternal pStringInt  = (OpcUa_pStringInternal)a_pString;

    if(pStringInt == OpcUa_Null)
    {
        return OpcUa_True;
    }

    if(((OpcUa_StringA)pStringInt)[0] == 0x00)
    {
        if(pStringInt->strContent == OpcUa_Null)
        {
            return OpcUa_True;
        }
    }
    return OpcUa_False;
}


/*============================================================================
* Get number of bytes.
*===========================================================================*/
OpcUa_UInt32 OpcUa_String_StrSize(const OpcUa_String* a_pString)
{
    OpcUa_pStringInternal pStringInt = (OpcUa_pStringInternal)a_pString;

    if(pStringInt == OpcUa_Null)
    {
        return 0;
    }

    if( _OpcUa_IsUaString(a_pString) != OpcUa_False )
    {
        if(pStringInt->strContent == OpcUa_Null)
        {
            return 0;
        }
        return pStringInt->uLength;
    }

    return OpcUa_P_String_StrLen((OpcUa_StringA)a_pString);
}


/*============================================================================
* Get number of characters.
*===========================================================================*/
OpcUa_UInt32 OpcUa_String_StrLen(const OpcUa_String*  a_pString)
{
    OpcUa_UInt32            uRawLen         = 0;
    OpcUa_StringA           strRawString    = OpcUa_Null;
    OpcUa_UInt32            uCount          = 0;
    OpcUa_UInt32            uiOctetCount    = 0;
    OpcUa_UInt32            uLen            = 0;
    OpcUa_Byte              byMask          = 0;

    static const OpcUa_UCharA   byUTF8Mask[6] = {   0x00,                  /* Mask for 1 Byte UTF 0x0xxxxxxx */
                                                    0xC0,                  /* Mask for 2 Byte UTF 0x110xxxxx */
                                                    0xE0,                  /* Mask for 3 Byte UTF 0x1110xxxx */
                                                    0xF0,                  /* Mask for 4 Byte UTF 0x11110xxx */
                                                    0xF8,                  /* Mask for 5 Byte UTF 0x111110xx */
                                                    0xFC                   /* Mask for 6 Byte UTF 0x1111110x */
                                                };

    if(a_pString == OpcUa_Null)
    {
        return 0;
    }

    strRawString = _OpcUa_String_GetRawString(a_pString);

    if(strRawString == OpcUa_Null)
    {
        return 0;
    }

    uRawLen = OpcUa_String_StrSize(a_pString);
    uLen = 0;
    for(uCount = 0; uCount < uRawLen; uCount++)
    {
        if ((strRawString[uCount]&0x80) !=0 )
        {
            uiOctetCount = 7;  /* Maximum BYTES of UTF Char ist 6, we ne 1 one, cause we make a -1 in the next step. */
            do
            {
                uiOctetCount--;
                byMask = (OpcUa_Byte) ((0xff >> (8 - uiOctetCount)) << (8 - uiOctetCount));
            } while ((uiOctetCount>1) && ((strRawString[uCount] & byMask) != byUTF8Mask[uiOctetCount-1]));
            if (uiOctetCount==1)
            {
                /* Todo OpcUa_BadSyntaxError might be better, but is not yet defined */
                /* OpcUa_BadInvalidArgument; */
            }
            uCount+=uiOctetCount-1;
        }
        uLen++;
    }
    return uLen;
}

/*============================================================================
* Copy string.
*===========================================================================*/
OpcUa_StatusCode OpcUa_String_StrnCpy( OpcUa_String*       a_pDestString,
                                       const OpcUa_String* a_pSrcString,
                                       OpcUa_UInt32        a_uLength)
{
    OpcUa_StringA           strRawSrc   = OpcUa_Null;
    OpcUa_UInt32            uiSrcLen    = 0;
    OpcUa_StringInternal*   pStringInt  = (OpcUa_StringInternal*)a_pSrcString;

    OpcUa_StatusCode        uStatus     = OpcUa_Good;

    OpcUa_ReturnErrorIfArgumentNull(a_pDestString);

#if 0 /* deactivating check, because it is not right here */
      /* user would have to enforce type casting and would provoke an error */
    if(_OpcUa_IsUaString(a_pDestString) == OpcUa_False)
    {
        return OpcUa_BadInvalidArgument;
    }
#endif

    /* check src string */

    OpcUa_String_Clear(a_pDestString);

    if(OpcUa_String_IsNull(a_pSrcString))
    {
        return OpcUa_Good;
    }

    strRawSrc  = _OpcUa_String_GetRawString(a_pSrcString);

    if(a_uLength == OPCUA_STRING_LENDONTCARE)
    {
        /* size in bytes */
        uiSrcLen  = OpcUa_String_StrSize(a_pSrcString);
    }
    else
    {
        /* min of given maximum number of bytes and the real length */
        uiSrcLen  = (a_uLength < pStringInt->uLength)?a_uLength:pStringInt->uLength;
    }

    uStatus = OpcUa_String_AttachToString(  strRawSrc,
                                            uiSrcLen,
                                            0,
                                            OpcUa_True,
                                            OpcUa_True, /* since we copy, this is irrelevant... */
                                            a_pDestString);
    return uStatus;
}

/*============================================================================
* Append string.
*===========================================================================*/
OpcUa_StatusCode OpcUa_String_StrnCat(  OpcUa_String*       a_pDestString,
                                        const OpcUa_String* a_pSrcString,
                                        OpcUa_UInt32        a_uLength)
{
    OpcUa_pStringInternal   pStringInt  = (OpcUa_pStringInternal) a_pDestString;
    OpcUa_StringA           strRawSrc   = OpcUa_Null;
    OpcUa_StringA           strRawDest  = OpcUa_Null;
    OpcUa_UInt32            uiSrcLen    = 0;
    OpcUa_UInt32            uiDestLen   = 0;
    OpcUa_UInt32            uiTempLen   = OpcUa_ProxyStub_g_Configuration.iSerializer_MaxStringLength;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_String);
    OpcUa_ReturnErrorIfArgumentNull(a_pDestString);

#if 0 /* deactivating check, because it is not right here */
    if(_OpcUa_IsUaString(a_pDestString)==OpcUa_False)
    {
        return OpcUa_BadInvalidArgument;
    }
#endif

    if( a_pSrcString == OpcUa_Null || OpcUa_String_IsNull(a_pSrcString) || OpcUa_String_IsEmpty(a_pSrcString) || a_uLength == 0)
    {
        return OpcUa_Good;
    }

    uiSrcLen = OpcUa_String_StrSize( a_pSrcString);
    uiDestLen = OpcUa_String_StrSize(a_pDestString);

    /* use source length if complete source should be appended or the given size is larger than the source */
    if(a_uLength == OPCUA_STRING_LENDONTCARE || a_uLength > uiSrcLen)
    {
        a_uLength = uiSrcLen;
    }

    OpcUa_ReturnErrorIfBad( _OpcUa_ExpandString(    a_pDestString,             /* string to expand */
                                                    a_uLength + uiDestLen + 1, /* new len = dest len + length of bytes to append */
                                                    &uiTempLen,                /* limit len and new buffer len */
                                                    OpcUa_True)                /* do it for real */
                           );
    strRawSrc  = _OpcUa_String_GetRawString(a_pSrcString);
    strRawDest = _OpcUa_String_GetRawString(a_pDestString);

    OpcUa_P_String_StrnCat( strRawDest, /* target */
                            uiTempLen,  /* length of target buffer (max for operation) */
                            strRawSrc,  /* source */
                            a_uLength); /* lenght of source to append */

    pStringInt->uLength = a_uLength + uiDestLen;

    return OpcUa_Good;
}


/*============================================================================
 * Compare two OpcUa_Strings
 *===========================================================================*/
OpcUa_Int32 OpcUa_String_StrnCmp(   const OpcUa_String* a_pLeftString,
                                    const OpcUa_String* a_pRightString,
                                    OpcUa_UInt32        a_uLength,
                                    OpcUa_Boolean       a_bIgnoreCase )
{
    OpcUa_StringA   strRawLeft  = OpcUa_Null;
    OpcUa_StringA   strRawRight = OpcUa_Null;
    OpcUa_UInt32    uiLeftLen   = 0;
    OpcUa_UInt32    uiRightLen  = 0;
    OpcUa_UInt32    uiTempLen   = 0;
    OpcUa_Int32     nRetVal     = 0;

    /* '0' a_pLeftString identical to a_pRightString  */
    if(a_pLeftString  == OpcUa_Null && a_pRightString  == OpcUa_Null) return 0;
    /* '< 0' a_pLeftString less than a_pRightString */
    if(a_pLeftString == OpcUa_Null) return -1;
    /* '> 0' a_pLeftString greater than a_pRightString */
    if(a_pRightString == OpcUa_Null) return 1;

    strRawLeft  = _OpcUa_String_GetRawString(a_pLeftString);
    strRawRight = _OpcUa_String_GetRawString(a_pRightString);

    /* '0' a_pLeftString identical to a_pRightString  */
    if(strRawLeft == strRawRight) return 0;
    /* '< 0' a_pLeftString less than a_pRightString */
    if(strRawLeft == OpcUa_Null) return -1;
    /* '> 0' a_pLeftString greater than a_pRightString */
    if(strRawRight == OpcUa_Null) return 1;

    uiLeftLen  = OpcUa_String_StrSize(a_pLeftString);
    uiRightLen = OpcUa_String_StrSize(a_pRightString);

    /* Re-Calculate Minimum length */
    uiTempLen = (uiLeftLen > uiRightLen) ? uiRightLen : uiLeftLen;

    if(uiTempLen >= a_uLength)
    {
        uiTempLen = a_uLength;
    }
    else
    {
        /* the compare length is longer than at least one of the given strings */
        /* we can check the lengths a priori */
        if(uiLeftLen < uiRightLen) return -1;
        if(uiLeftLen > uiRightLen) return 1;
    }

    if(a_uLength == OPCUA_STRING_LENDONTCARE)
    {
        /* we can check the lengths a priori */
        if(uiLeftLen < uiRightLen) return -1;
        if(uiLeftLen > uiRightLen) return 1;
    }

    /* need to provide strnicmp with valid lengths, because the raw strings may not be zero terminated! */
    if(a_bIgnoreCase  != OpcUa_False)
    {
        nRetVal = OpcUa_P_String_StrniCmp(strRawLeft, strRawRight, uiTempLen);
    }
    else
    {
        nRetVal = OpcUa_P_String_StrnCmp(strRawLeft, strRawRight, uiTempLen);
    }

    return nRetVal;
}



/*============================================================================
 * OpcUa_String_AttachReadOnly
 *===========================================================================*/
OpcUa_StatusCode OpcUa_String_AttachReadOnly(OpcUa_String* a_pDst, const OpcUa_StringA a_pSrc)
{
    OpcUa_StatusCode uStatus = OpcUa_String_AttachToString(
        a_pSrc,
        OPCUA_STRINGLENZEROTERMINATED,
        0,
        OpcUa_False,  /* attach the source without copying */
        OpcUa_False,  /* do not free the attached value */
        a_pDst);

    return uStatus;
}

/*============================================================================
 * OpcUa_String_AttachCopy
 *===========================================================================*/
OpcUa_StatusCode OpcUa_String_AttachCopy(OpcUa_String* a_pDst, const OpcUa_StringA a_pSrc)
{
    OpcUa_StatusCode uStatus = OpcUa_String_AttachToString(
        a_pSrc,
        OPCUA_STRINGLENZEROTERMINATED,
        0,
        OpcUa_True, /* copy the given string */
        OpcUa_True, /* irrelevant due to copy; could be OpcUa_False as well */
        a_pDst);

    return uStatus;
}

/*============================================================================
 * OpcUa_String_AttachWithOwnership
 *===========================================================================*/
OpcUa_StatusCode OpcUa_String_AttachWithOwnership(OpcUa_String* a_pDst, OpcUa_StringA a_pSrc)
{
    OpcUa_StatusCode uStatus = OpcUa_String_AttachToString(
        a_pSrc,
        OPCUA_STRINGLENZEROTERMINATED,
        0,
        OpcUa_False, /* attach the source without copying */
        OpcUa_True,  /* and free the attached string on clearing */
        a_pDst);

    return uStatus;
}

