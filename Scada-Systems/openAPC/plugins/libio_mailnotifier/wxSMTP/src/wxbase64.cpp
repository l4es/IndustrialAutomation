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
 $Log: wxbase64.cpp,v $
 Revision 1.5  2003/11/21 12:36:46  tavasti
 - Makefilet -Wall optioilla
 - Korjattu 'j‰rkev‰t' varoitukset pois (J‰‰nyt muutama joita ei saa
   kohtuudella poistettua)

 Revision 1.4  2003/11/14 15:43:09  tavasti
 Sending email with alternatives works

 Revision 1.3  2003/11/13 17:12:15  tavasti
 - Muutettu tiedostojen nimet wx-alkuisiksi

 Revision 1.2  2003/11/07 09:17:40  tavasti
 - K‰‰ntyv‰ versio, fileheaderit lis‰tty.


****************************************************************************/

//static char cvs_id[] = "$Header: /v/CVS/olive/notifier/wxSMTP/src/wxbase64.cpp,v 1.1.1.1 2004/08/27 23:12:14 paul Exp $";


//*********************************************************************
//* Base64 - a simple base64 encoder and decoder.
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

#include "wxbase64.h"

const wxChar fillchar = '=';

                        // 00000000001111111111222222
                        // 01234567890123456789012345
static wxString     cvt = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ"

                        // 22223333333333444444444455
                        // 67890123456789012345678901
                          _T("abcdefghijklmnopqrstuvwxyz")

                        // 555555556666
                        // 234567890123
                          _T("0123456789+/"));


wxString wxBase64::Encode(const wxChar* pData, size_t len)
{
    size_t c;
    wxString ret;
	ret.Alloc(len * 4 / 3 + len * 2 / 50);
	size_t resultLen = 0;
	wxString cr(_T("\x00d\x00a"));
	//wxString cr("\n");

    for (size_t i = 0; i < len; ++i)
    {
        c = (pData[i] >> 2) & 0x3f;
        ret.Append(cvt[c], 1);
		if (++resultLen == 72) { ret += cr; resultLen = 0; }
        c = (pData[i] << 4) & 0x3f;
        if (++i < len)
            c |= (pData[i] >> 4) & 0x0f;

        ret.Append(cvt[c], 1);
		if (++resultLen == 72) { ret += cr; resultLen = 0; }
        if (i < len)
        {
            c = (pData[i] << 2) & 0x3f;
            if (++i < len)
                c |= (pData[i] >> 6) & 0x03;

            ret.Append(cvt[c], 1);
        }
        else
        {
            ++i;
            ret.Append(fillchar, 1);
        }
		if (++resultLen == 72) { ret += cr; resultLen = 0; }

        if (i < len)
        {
            c = pData[i] & 0x3f;
            ret.Append(cvt[c], 1);
        }
        else
        {
            ret.Append(fillchar, 1);
        }
		if (++resultLen == 72) { ret += cr; resultLen = 0; }
    }

    return ret;
}

wxString wxBase64::Decode(const wxString& data)
{
    int c;
    int c1;
    size_t len = data.Length();
    wxString ret;
	ret.Alloc(data.Length() * 3 / 4);

    for (size_t i = 0; i < len; ++i)
    {
		// TODO: check all Find results for -1 as result of wrong input data for release build
        c = cvt.Find(data[i]);
		wxASSERT_MSG(c >= 0, _T("invalid base64 input"));
        ++i;
        c1 = cvt.Find(data[i]);
		wxASSERT_MSG(c1 >= 0, _T("invalid base64 input"));
        c = (c << 2) | ((c1 >> 4) & 0x3);
        ret.Append((char)c, 1);
        if (++i < len)
        {
            c = data[i];
            if (fillchar == c)
                break;

            c = cvt.Find((char)c);
			wxASSERT_MSG(c >= 0, _T("invalid base64 input"));
            c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
            ret.Append((char)c1, 1);
        }

        if (++i < len)
        {
            c1 = data[i];
            if (fillchar == c1)
                break;

            c1 = cvt.Find((char)c1);
			wxASSERT_MSG(c1 >= 0, _T("invalid base64 input"));
            c = ((c << 6) & 0xc0) | c1;
            ret.Append((char)c, 1);
        }
    }

    return ret;
}
