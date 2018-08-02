//==============================================================================
// TITLE: OpcHda_Error.h
//
// CONTENTS:
// 
// Defines error codes for the Historial Data Access specifications.
//
// (c) Copyright 2000-2003 The OPC Foundation
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
// 2000/01/17 OPC   Created.
// 2003/01/06 RSA   Updated formatting. Added messages to proxy/stub resource block.
//

#ifndef __OPCHDAERROR_H
#define __OPCHDAERROR_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// The 'Facility' is set to the standard for COM interfaces or FACILITY_ITF (i.e. 0x004)
// The 'Code' is set in the range defined OPC Commmon for DA (i.e. 0x1000 to 0x10FF)

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
// MessageId: OPC_E_MAXEXCEEDED
//
// MessageText:
//
//  The maximum number of values requested exceeds the server's limit.
//
#define OPC_E_MAXEXCEEDED                ((HRESULT)0xC0041001L)

//
// MessageId: OPC_S_NODATA
//
// MessageText:
//
//  There is no data within the specified parameters.
//
#define OPC_S_NODATA                     ((HRESULT)0x40041002L)

//
// MessageId: OPC_S_MOREDATA
//
// MessageText:
//
//  There is more data satisfying the query than was returned.
//
#define OPC_S_MOREDATA                   ((HRESULT)0x40041003L)

//
// MessageId: OPC_E_INVALIDAGGREGATE
//
// MessageText:
//
//  The aggregate requested is not valid.
//
#define OPC_E_INVALIDAGGREGATE           ((HRESULT)0xC0041004L)

//
// MessageId: OPC_S_CURRENTVALUE
//
// MessageText:
//
//  The server only returns current values for the requested item attributes.
//
#define OPC_S_CURRENTVALUE               ((HRESULT)0x40041005L)

//
// MessageId: OPC_S_EXTRADATA
//
// MessageText:
//
//  Additional data satisfying the query was found.
//
#define OPC_S_EXTRADATA                  ((HRESULT)0x40041006L)

//
// MessageId: OPC_W_NOFILTER
//
// MessageText:
//
//  The server does not support this filter.
//
#define OPC_W_NOFILTER                   ((HRESULT)0x80041007L)

//
// MessageId: OPC_E_UNKNOWNATTRID
//
// MessageText:
//
//  The server does not support this attribute.
//
#define OPC_E_UNKNOWNATTRID              ((HRESULT)0xC0041008L)

//
// MessageId: OPC_E_NOT_AVAIL
//
// MessageText:
//
//  The requested aggregate is not available for the specified item.
//
#define OPC_E_NOT_AVAIL                  ((HRESULT)0xC0041009L)

//
// MessageId: OPC_E_INVALIDDATATYPE
//
// MessageText:
//
//  The supplied value for the attribute is not a correct data type.
//
#define OPC_E_INVALIDDATATYPE            ((HRESULT)0xC004100AL)

//
// MessageId: OPC_E_DATAEXISTS
//
// MessageText:
//
//  Unable to insert - data already present.
//
#define OPC_E_DATAEXISTS                 ((HRESULT)0xC004100BL)

//
// MessageId: OPC_E_INVALIDATTRID
//
// MessageText:
//
//  The supplied attribute ID is not valid.
//
#define OPC_E_INVALIDATTRID              ((HRESULT)0xC004100CL)

//
// MessageId: OPC_E_NODATAEXISTS
//
// MessageText:
//
//  The server has no value for the specified time and item ID.
//
#define OPC_E_NODATAEXISTS               ((HRESULT)0xC004100DL)

//
// MessageId: OPC_S_INSERTED
//
// MessageText:
//
//  The requested insert occurred.
//
#define OPC_S_INSERTED                   ((HRESULT)0x4004100EL)

//
// MessageId: OPC_S_REPLACED
//
// MessageText:
//
//  The requested replace occurred.
//
#define OPC_S_REPLACED                   ((HRESULT)0x4004100FL)

#endif // __OPCHDAERROR_H
