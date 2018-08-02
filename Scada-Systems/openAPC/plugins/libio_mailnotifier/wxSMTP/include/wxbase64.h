/****************************************************************************

 Project     : 
 Author      : 
 Description : 

 VERSION INFORMATION:
 File    : $Source$
 Version : $Revision$
 Date    : $Date$
 Author  : $Author$

 History: 
 $Log: wxbase64.h,v $
 Revision 1.2  2003/11/07 09:17:40  tavasti
 - K‰‰ntyv‰ versio, fileheaderit lis‰tty.


****************************************************************************/

//*********************************************************************
//* C_Base64 - a simple base64 encoder and decoder.
//*
//*     Copyright (c) 1999, Bob Withers - bwit@pobox.com
//*
//* This code may be freely used for any purpose, either personal
//* or commercial, provided the authors copyright notice remains
//* intact.
//*********************************************************************
//
// converted to wxWindows by Frank Buﬂ
//

#ifndef BASE64_H
#define BASE64_H

#include <wx/wx.h>

/**
 * Helper class for encoding / decoding base64.
 */
class wxBase64
{
public:
	/**
	 * Converts a byte array to a base64 encoded string.
	 * \param pData The byte array.
	 * \param length The length of the byte array.
	 * \return The base64 representation of the byte array.
	 */
	static wxString Encode(const wxChar* pData, size_t length);

	/**
	 * Decodes a base64 encoded string.
	 * \param data The base64 encoded string.
	 * \return The decoded binary string (may contain 0).
	 */
    static wxString Decode(const wxString& data);
};

#endif
