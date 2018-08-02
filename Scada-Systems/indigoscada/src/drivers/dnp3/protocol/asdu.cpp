//
// $Id: asdu.cpp 4 2007-04-10 22:55:27Z sparky1194 $
//
// Copyright (C) 2007 Turner Technolgoies Inc. http://www.turner.ca
//
// Permission is hereby granted, free of charge, to any person 
// obtaining a copy of this software and associated documentation 
// files (the "Software"), to deal in the Software without 
// restriction, including without limitation the rights to use, 
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following 
// conditions:
//      
// The above copyright notice and this permission notice shall be 
// included in all copies or substantial portions of the Software. 
//      
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
// OTHER DEALINGS IN THE SOFTWARE.

// Modified by Enscada limited http://www.enscada.com

#include <assert.h>
#include <stdio.h>
#include "asdu.hpp"

uint8_t AppHeader::UNDEFINED_SEQ_NUM = 15;

void AppSeqNum::increment( AppSeqNum_t& seqNum)
{
    seqNum++;
    seqNum &= 0x0f;  // max app seq num = 15 
}

void AppSeqNum::decrement( AppSeqNum_t& seqNum)
{
    if (seqNum == 0)
	seqNum = 0x0f;
    else
	seqNum--;
}

AppHeader::AppHeader( bool fir, bool fin,
		      bool con, bool uns, AppSeqNum_t seq,
		      FunctionCode fc, uint16_t iin)
{
	UNDEFINED_SEQ_NUM = 0xff;

    if (fc & 0x80)
    {
	// must be a repsonse
	a[0] = ((fin<<6) | (fir<<7) | (con<<5) | (uns<<4) | (seq));
	a[1] = fc;
	a[2] = iin & 0xff;
	a[3] = (iin >> 8) & 0xff;
	len = 4;
    }
    else
    {
	// must be a request
	a[0] = ((fin<<6) | (fir<<7) | (con<<5) | (uns<<4) | (seq));
	a[1] = fc;
	len = 2;
    }
}

void AppHeader::encode( Bytes& fragment)
{
    int i;

    // app headers go at the beginning
    assert(fragment.size() == 0);
    fragment.resize(len);

    for (i=0; i<len; i++)
	fragment[i] = a[i];
}

void AppHeader::decode( Bytes& fragment)
{
    assert(fragment.size() >= 2);

    // for a response the app header is 4 bytes and for a request 2 bytes
    if (fragment[1] & 0x80)
    {
	assert(fragment.size() >= 4);
	len = 4; //  this is a response
        a[0] = fragment[0];
        a[1] = fragment[1];
        a[2] = fragment[2];
        a[3] = fragment[3];
    }
    else
    {
	len = 2;  /* this is a request */
        a[0] = fragment[0];
        a[1] = fragment[1];
    }

    fragment.erase(fragment.begin(), fragment.begin()+len); 
}


void AppHeader::setConfirm( bool con)
{
    if (con)
	a[0] &= 0xdf;
    else 
	a[0] |= 0x20;
}

bool AppHeader::getFirst()
{
    return ((a[0] & 0x80) == 0x80) ? true : false;
}

bool AppHeader::getFinal()
{
    return ((a[0] & 0x40) == 0x40) ? true : false;
}

bool AppHeader::getConfirm()
{
    return ((a[0] & 0x20) == 0x20) ? true : false;
}

bool AppHeader::getUnsolicited()
{
    return ((a[0] & 0x10) == 0x10) ? true : false;
}
    
AppSeqNum_t AppHeader::getSeqNum()
{
    return a[0] & 0x0f;
}

AppHeader::FunctionCode AppHeader::getFn()
{
    return (FunctionCode) a[1];
}

AppHeader::FunctionCode AppHeader::getFn( const Bytes& fragment)
{
    return (FunctionCode) fragment[1];
}

AppSeqNum_t AppHeader::getSeqNum( const Bytes& fragment)
{
    return fragment[0] & 0x0f;
}

uint16_t AppHeader::getIIN( const Bytes& fragment)
{
    return fragment[2] + (fragment[3] << 8);
}

uint16_t AppHeader::getIIN()
{
    return a[2] + (a[3] << 8);
}

char* AppHeader::str( char* buf, int len)
{
    FunctionCode fn = getFn();
    if (fn & 0x80)
    {
	sprintf( buf, 
                 "Ap: Rsp [Fn:%d,FIR=%d,FIN=%d,CON=%d,UNS=%d,SEQ=%d,II=%04x]",
		  fn, getFirst(), getFinal(),
		  getConfirm(), getUnsolicited(),
		  getSeqNum(), getIIN());
    }
    else
    {
	sprintf( buf, 
		  "Ap: Req [Fn:%d,FIR=%d,FIN=%d,CON=%d,UNS=%d,SEQ=%d]",
		  fn, getFirst(), getFinal(),
		  getConfirm(), getUnsolicited(),
		  getSeqNum() );
    }
    return buf;
}

