/* ========================================================================
 * Copyright (c) 2005-2016 The OPC Foundation, Inc. All rights reserved.
 *
 * OPC Foundation MIT License 1.00
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * The complete license agreement can be found here:
 * http://opcfoundation.org/License/MIT/1.00/
 * ======================================================================*/
 
#ifndef _readservice_
#define _readservice_


#define RESET_SESSION_COUNTER	msec_counter=0; 


OpcUa_StatusCode my_Read(
							OpcUa_Endpoint             a_hEndpoint,
							OpcUa_Handle               a_hContext,
							const OpcUa_RequestHeader* a_pRequestHeader,
							OpcUa_Double               a_nMaxAge,
							OpcUa_TimestampsToReturn   a_eTimestampsToReturn,
							OpcUa_Int32                a_nNoOfNodesToRead,
							const OpcUa_ReadValueId*   a_pNodesToRead,
							OpcUa_ResponseHeader*      a_pResponseHeader,
							OpcUa_Int32*               a_pNoOfResults,
							OpcUa_DataValue**          a_pResults,
							OpcUa_Int32*               a_pNoOfDiagnosticInfos,
							OpcUa_DiagnosticInfo**     a_pDiagnosticInfos);
OpcUa_StatusCode  fill_Variant_for_value_attribute(_VariableKnoten_*  , OpcUa_String* , OpcUa_DataValue* );

OpcUa_StatusCode fill_datatype_arraytype_in_my_Variant(OpcUa_DataValue* ,OpcUa_Byte, OpcUa_Byte,OpcUa_Int);

OpcUa_StatusCode assigne_Timestamp(OpcUa_DataValue* ,OpcUa_TimestampsToReturn );



#endif /*_readservice_*/