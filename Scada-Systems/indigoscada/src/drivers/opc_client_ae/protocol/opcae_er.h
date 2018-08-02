//==============================================================================
// TITLE: opcae_er.h
//
// CONTENTS:
// 
// Defines error codes for the OPC Alarms & Events specification.
//
// (c) Copyright 1997-2003 The OPC Foundation
// ALL RIGHTS RESERVED.
//
// DISCLAIMER:
//  This code is provided by the OPC Foundation solely to assist in 
//  understanding and use of the appropriate OPC Specification(s) and may be 
//  used as set forth in the License Grant section of the OPC Specification.
//  This code is provided as-is and without warranty or support of any sort
//  and is subject to the Warranty and Liability Disclaimers which appear
//  in the printed OPC Specification.
//
// MODIFICATION LOG:
//
// Date       By    Notes
// ---------- ---   -----
// 2002/10/02 JL    Updated for Version 1.1.
// 2003/01/06 RSA   Fixed formatting to comply with coding guidelines.
//

#ifndef __OPCAE_ER_H
#define __OPCAE_ER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// The 'Facility' is set to the standard for COM interfaces or FACILITY_ITF (i.e. 0x004)
// The 'Code' is set in the range defined OPC Commmon for AE (i.e. 0x0200 to 0x02FF)

//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: OPC_S_ALREADYACKED
//
// MessageText:
//
//  The condition has already been acknowleged.
//
#define OPC_S_ALREADYACKED               ((HRESULT)0x00040200L)

//
// MessageId: OPC_S_INVALIDBUFFERTIME
//
// MessageText:
//
//  The buffer time parameter was invalid.
//
#define OPC_S_INVALIDBUFFERTIME          ((HRESULT)0x00040201L)

//
// MessageId: OPC_S_INVALIDMAXSIZE
//
// MessageText:
//
//  The max size parameter was invalid.
//
#define OPC_S_INVALIDMAXSIZE             ((HRESULT)0x00040202L)

//
// MessageId: OPC_S_INVALIDKEEPALIVETIME
//
// MessageText:
//
//  The KeepAliveTime parameter was invalid.
//
#define OPC_S_INVALIDKEEPALIVETIME       ((HRESULT)0x00040203L)

//
// MessageId: OPC_E_INVALIDBRANCHNAME
//
// MessageText:
//
//  The string was not recognized as an area name.
//
#define OPC_E_INVALIDBRANCHNAME          ((HRESULT)0xC0040203L)

//
// MessageId: OPC_E_INVALIDTIME
//
// MessageText:
//
//  The time does not match the latest active time.
//
#define OPC_E_INVALIDTIME                ((HRESULT)0xC0040204L)

//
// MessageId: OPC_E_BUSY
//
// MessageText:
//
//  A refresh is currently in progress.
//
#define OPC_E_BUSY                       ((HRESULT)0xC0040205L)

//
// MessageId: OPC_E_NOINFO
//
// MessageText:
//
//  Information is not available.
//
#define OPC_E_NOINFO                     ((HRESULT)0xC0040206L)

#endif // __OPCAE_ER_H
