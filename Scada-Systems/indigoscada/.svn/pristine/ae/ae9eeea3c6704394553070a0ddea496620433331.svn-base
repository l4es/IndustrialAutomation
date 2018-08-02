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

/* This are the UA Proxy/Stub internal data type definitions! */

/* This is the Linux version of this file! */

#ifndef _OpcUa_P_Types_H_
#define _OpcUa_P_Types_H_ 1

#define OPCUA_P_NATIVE64 1

/*============================================================================
* Type definitions for basic data types.
*===========================================================================*/

typedef int                 OpcUa_Int;
typedef unsigned int        OpcUa_UInt;
typedef void                OpcUa_Void;
typedef void*               OpcUa_Handle;
typedef unsigned char       OpcUa_Boolean;
typedef signed char         OpcUa_SByte;
typedef unsigned char       OpcUa_Byte;
typedef short               OpcUa_Int16;
typedef unsigned short      OpcUa_UInt16;
typedef int                 OpcUa_Int32;
typedef unsigned int        OpcUa_UInt32;


#if OPCUA_P_NATIVE64
typedef long long           OpcUa_Int64;
typedef unsigned long long  OpcUa_UInt64;
#else
struct _OpcUa_Int64 {
    OpcUa_UInt32 dwLowQuad;
    OpcUa_UInt32 dwHighQuad;
};
typedef struct _OpcUa_Int64 OpcUa_Int64;
struct _OpcUa_UInt64 {
    OpcUa_UInt32 dwLowQuad;
    OpcUa_UInt32 dwHighQuad;
};
typedef struct _OpcUa_UInt64 OpcUa_UInt64;
#endif

typedef float               OpcUa_Float;
typedef double              OpcUa_Double;
typedef char                OpcUa_CharA;
typedef unsigned char       OpcUa_UCharA;
typedef OpcUa_CharA*        OpcUa_StringA;
typedef unsigned short      OpcUa_Char;


struct _OpcUa_DateTime
{
    OpcUa_UInt32 dwLowDateTime;
    OpcUa_UInt32 dwHighDateTime;
};

typedef struct _OpcUa_DateTime OpcUa_DateTime;


/**
 * @brief OpcUa_SocketManager Type
 */
typedef OpcUa_Void* OpcUa_SocketManager;

/**
 * @brief OpcUa_Socket Type
 */
typedef OpcUa_Void* OpcUa_Socket;

/**
 * @brief OpcUa_Thread Type
 */
typedef OpcUa_Void* OpcUa_Thread;


/**
 * @brief Internally used thread main entry function.
 */
typedef OpcUa_Void      (OpcUa_PfnInternalThreadMain)(OpcUa_Void* pArguments);

/**
 * @brief The handle for the platform thread.
 */
typedef OpcUa_UInt32        OpcUa_StatusCode;

/**
 * @brief The handle for the mutex.
 */
typedef OpcUa_Void*     OpcUa_Mutex;

/**
 * @brief The handle for the semaphore.
 */
typedef OpcUa_Void*     OpcUa_Semaphore;

/**
 * @brief The handle for a timer.
 */
typedef OpcUa_Void*     OpcUa_Timer;

/**
 * @brief A function used to compare elements when sorting or searching.
 *
 * @param pContext  [in] The context passed to the sorting/searching function.
 * @param pElement1 [in] The first element to compare.
 * @param pElement2 [in] The second element to compare.
 *
 * @return Zero if elements are equal, < 0 if element1 is less that element2.
 */
typedef OpcUa_Int (OPCUA_CDECL OpcUa_PfnCompare)(   const OpcUa_Void* pElement1,
                                                const OpcUa_Void* pElement2);

/*============================================================================
* Type definitions for data types on the wire.
*===========================================================================*/
typedef OpcUa_Boolean       OpcUa_Boolean_Wire;
typedef OpcUa_SByte         OpcUa_SByte_Wire;
typedef OpcUa_Byte          OpcUa_Byte_Wire;
typedef OpcUa_Int16         OpcUa_Int16_Wire;
typedef OpcUa_UInt16        OpcUa_UInt16_Wire;
typedef OpcUa_Int32         OpcUa_Int32_Wire;
typedef OpcUa_UInt32        OpcUa_UInt32_Wire;
typedef OpcUa_Int64         OpcUa_Int64_Wire;
typedef OpcUa_UInt64        OpcUa_UInt64_Wire;
typedef OpcUa_Float         OpcUa_Float_Wire;
typedef OpcUa_Double        OpcUa_Double_Wire;
typedef OpcUa_CharA         OpcUa_Char_Wire;
typedef OpcUa_Char_Wire*    OpcUa_String_Wire;
typedef OpcUa_DateTime      OpcUa_DateTime_Wire;

/*============================================================================
* Type definitions for structured data types.
*===========================================================================*/
#define OPCUA_GUID_STATICINITIALIZER {0, 0, 0, {0,0,0,0,0,0,0,0}}
typedef struct _OpcUa_Guid
{
    OpcUa_UInt32    Data1;
    OpcUa_UInt16    Data2;
    OpcUa_UInt16    Data3;
    OpcUa_UCharA    Data4[8];
} OpcUa_Guid, *OpcUa_pGuid, OpcUa_Guid_Wire, *pOpcUa_Guid_Wire;

#define OPCUA_STRING_STATICINITIALIZER {0, 0, OpcUa_Null}
#ifdef _DEBUG
typedef struct _OpcUa_String
{
    OpcUa_UInt16 flags;
    OpcUa_UInt32 uLength;
    OpcUa_CharA* strContent;
} OpcUa_String, *OpcUa_pString;
#else
typedef struct _OpcUa_String
{
    OpcUa_UInt16        uReserved1;     /* Content is private to String Implementation */
    OpcUa_UInt32        uReserved2;     /* Content is private to String Implementation */
    OpcUa_Void*         uReserved4;     /* Content is private to String Implementation */
} OpcUa_String, *OpcUa_pString;
#endif

#define OPCUA_BYTESTRING_STATICINITIALIZER {-1, OpcUa_Null}
typedef struct _OpcUa_ByteString
{
    OpcUa_Int32 Length;
    OpcUa_Byte* Data;
} OpcUa_ByteString;

#define OPCUA_DATETIME_STATICINITIALIZER {0, 0}
typedef struct _OpcUa_FileTime
{
    OpcUa_UInt32 uiLowDateTime;
    OpcUa_UInt32 uiHighDateTime;
} OpcUa_FileTime, *OpcUa_pFileTime;

/**
* @brief Holds a time value with a maximum resolution of micro seconds.
*/
typedef struct _OpcUa_TimeVal OpcUa_TimeVal;

struct _OpcUa_TimeVal
{
    /** @brief The number of full seconds since 1970. */
    OpcUa_Int64 uintSeconds;
    /** @brief The fraction of the last second. */
    OpcUa_UInt32 uintMicroSeconds;
};


/*============================================================================
* constant definitions.
*===========================================================================*/
#define OpcUa_Ignore        0           /* Ignore signal */
#define OpcUa_Infinite      0xFFFFFFFF  /* Infinite timeout */


#define OpcUa_False         0
#define OpcUa_True          (!OpcUa_False)
#define OpcUa_Null          0
#define OpcUa_SByte_Min     (OpcUa_SByte)-128
#define OpcUa_SByte_Max     (OpcUa_SByte)127
#define OpcUa_Byte_Min      (OpcUa_Byte)0
#define OpcUa_Byte_Max      (OpcUa_Byte)255
#define OpcUa_Int16_Min     (OpcUa_Int16)-32768
#define OpcUa_Int16_Max     (OpcUa_Int16)32767
#define OpcUa_UInt16_Min    (OpcUa_UInt16)0
#define OpcUa_UInt16_Max    (OpcUa_UInt16)65535
#define OpcUa_Int32_Min     (OpcUa_Int32)(-2147483647L-1)
#define OpcUa_Int32_Max     (OpcUa_Int32)2147483647
#define OpcUa_UInt32_Min    (OpcUa_UInt32)0
#define OpcUa_UInt32_Max    (OpcUa_UInt32)4294967295U
#define OpcUa_Int64_Max     (OpcUa_Int64)9223372036854775807LL
#define OpcUa_Int64_Min     (OpcUa_Int64)(-OpcUa_Int64_Max - 1LL)
#define OpcUa_UInt64_Min    (OpcUa_UInt64)0
#define OpcUa_UInt64_Max    (OpcUa_UInt64)18446744073709551615ULL
/* defined as FLT_MIN in "%ProgramFiles\Microsoft Visual Studio 8\VC\include\float.h" */
/* #define FLT_MIN         1.175494351e-38F */
#define OpcUa_Float_Min     (OpcUa_Float)1.175494351e-38F
/* defined as FLT_MAX in "%ProgramFiles\Microsoft Visual Studio 8\VC\include\float.h" */
/* #define FLT_MAX         3.402823466e+38F */
#define OpcUa_Float_Max     (OpcUa_Float)3.402823466e+38F
/* defined as DBL_MIN in "%ProgramFiles\Microsoft Visual Studio 8\VC\include\float.h" */
/* #define DBL_MIN         2.2250738585072014e-308 */
#define OpcUa_Double_Min    (OpcUa_Double)2.2250738585072014e-308
/* defined as DBL_MAX in "%ProgramFiles\Microsoft Visual Studio 8\VC\include\float.h" */
/* #define DBL_MAX         1.7976931348623158e+308 */
#define OpcUa_Double_Max    (OpcUa_Double)1.7976931348623158e+308

#define OpcUa_DateTime_Min  0
#define OpcUa_DateTime_Max  3155378975999999999

/* set to OPCUA_CONFIG_YES to use the untested optimized byteswap */
#define OPCUA_SWAP_ALTERNATIVE OPCUA_CONFIG_NO

#if OPCUA_SWAP_ALTERNATIVE

#if BYTE_ORDER == LITTLE_ENDIAN

    /* this is the wire format */

    #define OpcUa_SwapBytes_2(xDst, xSrc) \
    { \
        *xDst = *xSrc; \
    }

    #define OpcUa_SwapBytes_4(xDst, xSrc) \
    { \
        *xDst = *xSrc; \
    }

    #define OpcUa_SwapBytes_8(xDst, xSrc) \
    { \
        *xDst = *xSrc; \
    }

#elif BYTE_ORDER == BIG_ENDIAN

    #define OpcUa_SwapBytes_2(xDst, xSrc) \
    { \
        ((unsigned char*)xDst)[0] = ((unsigned char*)xSrc)[1]; \
        ((unsigned char*)xDst)[1] = ((unsigned char*)xSrc)[0]; \
    }

    #define OpcUa_SwapBytes_4(xDst, xSrc) \
    { \
        ((unsigned char*)xDst)[0] = ((unsigned char*)xSrc)[3]; \
        ((unsigned char*)xDst)[1] = ((unsigned char*)xSrc)[2]; \
        ((unsigned char*)xDst)[2] = ((unsigned char*)xSrc)[1]; \
        ((unsigned char*)xDst)[3] = ((unsigned char*)xSrc)[0]; \
    }

    #define OpcUa_SwapBytes_8(xDst, xSrc) \
    { \
        ((unsigned char*)xDst)[0] = ((unsigned char*)xSrc)[7]; \
        ((unsigned char*)xDst)[1] = ((unsigned char*)xSrc)[6]; \
        ((unsigned char*)xDst)[2] = ((unsigned char*)xSrc)[5]; \
        ((unsigned char*)xDst)[3] = ((unsigned char*)xSrc)[4]; \
        ((unsigned char*)xDst)[4] = ((unsigned char*)xSrc)[3]; \
        ((unsigned char*)xDst)[5] = ((unsigned char*)xSrc)[2]; \
        ((unsigned char*)xDst)[6] = ((unsigned char*)xSrc)[1]; \
        ((unsigned char*)xDst)[7] = ((unsigned char*)xSrc)[0]; \
    }

#endif

#else /* OPCUA_SWAP_ALTERNATIVE */

#if BYTE_ORDER == LITTLE_ENDIAN
    #define OpcUa_SwapBytes(xDst, xSrc, xCount) \
    { \
        memcpy(xDst, xSrc, xCount); \
    }
#elif BYTE_ORDER == BIG_ENDIAN
    #define OpcUa_SwapBytes(xDst, xSrc, xCount) \
    { \
        OpcUa_UInt32 ii = 0; \
        OpcUa_UInt32 jj = xCount-1; \
        OpcUa_Byte* dst = (OpcUa_Byte*)xDst; \
        OpcUa_Byte* src = (OpcUa_Byte*)xSrc; \
        \
        for (; ii < xCount; ii++, jj--) \
        { \
            dst[ii] = src[jj]; \
        } \
    }
#endif

#endif /* OPCUA_SWAP_ALTERNATIVE */

#endif /* _OpcUa_PlatformDefs_H_ */
/*----------------------------------------------------------------------------------------------------*\
|   End of File                                                                          End of File   |
\*----------------------------------------------------------------------------------------------------*/


