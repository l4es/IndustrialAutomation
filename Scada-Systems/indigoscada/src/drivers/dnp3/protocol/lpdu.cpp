//
// $Id: lpdu.cpp 4 2007-04-10 22:55:27Z sparky1194 $
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
#include <string.h>
#include <stdio.h>
#include "common.hpp"
#include "lpdu.hpp"
#include <deque>

const char Lpdu::fcMap[2][16][25] = 
{
    { "ACK"                   , /* [0][0]   */
      "NACK"                  , /* [0][1]   */
      "Undefined"             , /* [0][2]   */
      "Undefined"             , /* [0][3]   */
      "Undefined"             , /* [0][4]   */
      "Undefined"             , /* [0][5]   */
      "Undefined"             , /* [0][6]   */
      "Undefined"             , /* [0][7]   */
      "Undefined"             , /* [0][8]   */
      "Undefined"             , /* [0][9]   */
      "Undefined"             , /* [0][10]  */
      "Link Status"           , /* [0][11]  */
      "Undefined"             , /* [0][12]  */
      "Undefined"             , /* [0][13]  */
      "Undefined"             , /* [0][14]  */
      "Undefined"             , /* [0][15]  */ },
    { "Undefined"             , /* [1][0]   */
      "Reset User Process"    , /* [1][1]   */
      "Test Link"             , /* [1][2]   */
      "User Data (Confirm)"   , /* [1][3]   */
      "User Data (No Confirm)", /* [1][4]   */
      "Undefined"             , /* [1][5]   */
      "Undefined"             , /* [1][6]   */
      "Undefined"             , /* [1][7]   */
      "Undefined"             , /* [1][8]   */
      "Request Link Status"   , /* [1][9]   */
      "Undefined"             , /* [1][10]  */
      "Undefined"             , /* [1][11]  */
      "Undefined"             , /* [1][12]  */
      "Undefined"             , /* [1][13]  */
      "Undefined"             , /* [1][14]  */
      "Undefined"               /* [1][15]  */}
};
  
  /*****************************************************************/
  /*                                                               */
  /* CRC LOOKUP TABLE                                              */
  /* ================                                              */
  /* The following CRC lookup table was generated automagically    */
  /* by the Rocksoft^tm Model CRC Algorithm Table Generation       */
  /* Program V1.0 using the following model parameters:            */
  /*                                                               */
  /*    Width   : 2 bytes.                                         */
  /*    Poly    : 0x3D65                                           */
  /*    Reverse : TRUE.                                            */
  /*                                                               */
  /* For more information on the Rocksoft^tm Model CRC Algorithm,  */
  /* see the document titled "A Painless Guide to CRC Error        */
  /* Detection Algorithms" by Ross Williams                        */
  /* (ross@guest.adelaide.edu.au.). This document is likely to be  */
  /* in the FTP archive "ftp.adelaide.edu.au/pub/rocksoft".        */
  /*                                                               */
  /*****************************************************************/    
  
const unsigned short Lpdu::crcTable[256] =
{
    0x0000, 0x365E, 0x6CBC, 0x5AE2, 0xD978, 0xEF26, 0xB5C4, 0x839A,
    0xFF89, 0xC9D7, 0x9335, 0xA56B, 0x26F1, 0x10AF, 0x4A4D, 0x7C13,
    0xB26B, 0x8435, 0xDED7, 0xE889, 0x6B13, 0x5D4D, 0x07AF, 0x31F1,
    0x4DE2, 0x7BBC, 0x215E, 0x1700, 0x949A, 0xA2C4, 0xF826, 0xCE78,
    0x29AF, 0x1FF1, 0x4513, 0x734D, 0xF0D7, 0xC689, 0x9C6B, 0xAA35,
    0xD626, 0xE078, 0xBA9A, 0x8CC4, 0x0F5E, 0x3900, 0x63E2, 0x55BC,
    0x9BC4, 0xAD9A, 0xF778, 0xC126, 0x42BC, 0x74E2, 0x2E00, 0x185E,
    0x644D, 0x5213, 0x08F1, 0x3EAF, 0xBD35, 0x8B6B, 0xD189, 0xE7D7,
    0x535E, 0x6500, 0x3FE2, 0x09BC, 0x8A26, 0xBC78, 0xE69A, 0xD0C4,
    0xACD7, 0x9A89, 0xC06B, 0xF635, 0x75AF, 0x43F1, 0x1913, 0x2F4D,
    0xE135, 0xD76B, 0x8D89, 0xBBD7, 0x384D, 0x0E13, 0x54F1, 0x62AF,
    0x1EBC, 0x28E2, 0x7200, 0x445E, 0xC7C4, 0xF19A, 0xAB78, 0x9D26,
    0x7AF1, 0x4CAF, 0x164D, 0x2013, 0xA389, 0x95D7, 0xCF35, 0xF96B,
    0x8578, 0xB326, 0xE9C4, 0xDF9A, 0x5C00, 0x6A5E, 0x30BC, 0x06E2,
    0xC89A, 0xFEC4, 0xA426, 0x9278, 0x11E2, 0x27BC, 0x7D5E, 0x4B00,
    0x3713, 0x014D, 0x5BAF, 0x6DF1, 0xEE6B, 0xD835, 0x82D7, 0xB489,
    0xA6BC, 0x90E2, 0xCA00, 0xFC5E, 0x7FC4, 0x499A, 0x1378, 0x2526,
    0x5935, 0x6F6B, 0x3589, 0x03D7, 0x804D, 0xB613, 0xECF1, 0xDAAF,
    0x14D7, 0x2289, 0x786B, 0x4E35, 0xCDAF, 0xFBF1, 0xA113, 0x974D,
    0xEB5E, 0xDD00, 0x87E2, 0xB1BC, 0x3226, 0x0478, 0x5E9A, 0x68C4,
    0x8F13, 0xB94D, 0xE3AF, 0xD5F1, 0x566B, 0x6035, 0x3AD7, 0x0C89,
    0x709A, 0x46C4, 0x1C26, 0x2A78, 0xA9E2, 0x9FBC, 0xC55E, 0xF300,
    0x3D78, 0x0B26, 0x51C4, 0x679A, 0xE400, 0xD25E, 0x88BC, 0xBEE2,
    0xC2F1, 0xF4AF, 0xAE4D, 0x9813, 0x1B89, 0x2DD7, 0x7735, 0x416B,
    0xF5E2, 0xC3BC, 0x995E, 0xAF00, 0x2C9A, 0x1AC4, 0x4026, 0x7678,
    0x0A6B, 0x3C35, 0x66D7, 0x5089, 0xD313, 0xE54D, 0xBFAF, 0x89F1,
    0x4789, 0x71D7, 0x2B35, 0x1D6B, 0x9EF1, 0xA8AF, 0xF24D, 0xC413,
    0xB800, 0x8E5E, 0xD4BC, 0xE2E2, 0x6178, 0x5726, 0x0DC4, 0x3B9A,
    0xDC4D, 0xEA13, 0xB0F1, 0x86AF, 0x0535, 0x336B, 0x6989, 0x5FD7,
    0x23C4, 0x159A, 0x4F78, 0x7926, 0xFABC, 0xCCE2, 0x9600, 0xA05E,
    0x6E26, 0x5878, 0x029A, 0x34C4, 0xB75E, 0x8100, 0xDBE2, 0xEDBC,
    0x91AF, 0xA7F1, 0xFD13, 0xCB4D, 0x48D7, 0x7E89, 0x246B, 0x1235
};

Lpdu::Lpdu(Stats* datalinkStats_p)
{
    stats_p = datalinkStats_p;
    reset();
}

void Lpdu::reset()
{
    ab.clear();
}

unsigned int Lpdu::getLenField() const
{
    return ab[2];
}

DnpAddr_t Lpdu::getSrc() const
{
    return (ab[7]<<8) + ab[6];
}

DnpAddr_t Lpdu::getDest() const
{
    return (ab[5]<<8) + ab[4];
}

unsigned int Lpdu::getFunctionCode() const
{
    return ab[3] & 0x0f;
}

unsigned int Lpdu::getDirectionBit() const
{
    return (ab[3] & 0x80) >> 7;
}

unsigned int Lpdu::getPrimaryBit() const
{
    return (ab[3] & 0x40) >> 6;
}

unsigned int Lpdu::getFcv() const
{
    return (ab[3] & 0x10) >> 4;
}

unsigned int Lpdu::getFcb() const
{
    assert ((ab[3] & 0x10) != 0x10); // don't call this if fcb invalid
    return ((ab[3] & 0x20) >> 5);
}

unsigned int Lpdu::getDataFlowControlBit() const
{
    assert ((ab[3] & 0x20) != 0x00); // don't call this if dfc invalid
    return ((ab[3] & 0x10) >> 4);
}

void Lpdu::getUserData( UserData &u) const
{
    Bytes::const_iterator i;

    u.data.clear();

    for (i=ab.begin()+HEADER_SIZE; i<ab.end(); i+=18)
    {
        if (ab.end()-i >= 18)
        {
	    // don't incl the 2 crc bytes
	    u.data.insert(u.data.end(), i, i+16);
        }
        else
        {
	    // don't incl the last 2 crc bytes
	    u.data.insert(u.data.end(), i, ab.end()-2);
        }
    }
    // fill in the src and dest
    u.src = getSrc();
    u.dest = getDest();
}

unsigned int Lpdu::getStat( int i)
{
    return stats_p->get(i);
}

char* Lpdu::str(char* buf, int len) const
{
    int prm = getPrimaryBit();
    int fc  = getFunctionCode();
    // for all possible values of prm and fc have defined a string
    char* fcStr = (char*) fcMap[prm][fc];
    int dir = getDirectionBit();
    int fcv = getFcv();
    int lenField = getLenField();
    int dest = getDest();
    int src = getSrc();
    sprintf(buf, 
	  "%s [Dir:%d,Prm:%d,Fcv:%d] Len=%d, Dest=%d, Src=%d\n",
	     fcStr, dir, prm, fcv, lenField, dest, src);
    return buf;
}

char* Lpdu::hex(char* buf, int buf_len) const
{
    return hex_repr( ab, buf, buf_len);
}

bool Lpdu::buildFromBytes(Bytes& data)
{
    bool  lpduComplete     = false;
    Bytes::iterator i      = data.begin();
    Bytes::iterator j; 

    while ( data.size() > 0)
    {
        if (ab.size() == 1)
        {
            /*  we've already detected the first octet, 0x05 */
            if (*i == 0x64)
            {
		ab.push_back(*i);
		i++;
		data.pop_front();
                stats_p->increment( RX_START_OCTETS);
            }
            else
            {
                /* still searching for the start octets */
                reset();
            }
        } 

        if (ab.size() == 0)
        {
            if (data.size() == 1)
            {
                if (*i == 0x05)
                {
		    ab.push_back(*i);
		    data.pop_front();
                }
                else
                {
                    stats_p->increment( LOST_BYTES);
	        }
		/* no matter what this byte was we are done with it */
                i = data.end();
                break;  
            }
            else
            {
                /* search for the start octects - always 0x0564 */
                while (i != data.end())
                {
	            if (*i == 0x05)
                    {
                        if (data.end()-i == 1)
                        {
	                    /* the start octets could be split accross reads */
			    ab.push_back(*i);
			    i = data.end();
                        }
                        else if (*(i+1) == 0x64)
                        {
	                    /* found the start octets */
			    ab.push_back(*i);
			    i++;
			    ab.push_back(*i);
			    i++;
			    data.pop_front();
			    data.pop_front();
                            stats_p->increment( RX_START_OCTETS);
                            break;
	                }
                        else
	                {
			    /* only the 0x05 found */
                            stats_p->increment( LOST_BYTES);
                            /* keep trying with the remaining slice */
			    i++;
			    data.pop_front();
                        }
                    } /* end if 0x05 */
                    else
	            {
                        stats_p->increment( LOST_BYTES);
                        /* keep trying with the remaining slice */
			i++;
			data.pop_front();
	            }

	        } /* end while i!=data.end() (the search for start octects*/
  
            } /* end else data size == 1 */

        } /* end if ab.size() = 0 */
        else
        {
            if ((ab.size() > 1) &&
                (ab.size() < HEADER_SIZE) &&
	        (data.size() > 0) )
            {
                /* start octets found and data still left in buf but we still
                 * have not fully read in the header yet ...
                 * so now read in as much of the header as we can */

                int bytesToCompleteHeader = HEADER_SIZE - ab.size();
                int bytesToAdd = data.end()-i;
                if (bytesToAdd > bytesToCompleteHeader)
                {
                    bytesToAdd = bytesToCompleteHeader;
                }

		j = i+bytesToAdd;
		ab.insert(ab.end(), i, j);
		data.erase(i, j);
		i = j;

                if (ab.size() == HEADER_SIZE)
	        {
                    int headerOk = checkDatalinkHeader();
                    if (headerOk)
	            {
                        if (expectedLen == ab.size())
	                {
                            /* do not process reaming buf, save it for later */
	                    lpduComplete = true;  
			    break;
	                }
	            }
                    else
	            {
                        reset(); /* corrupt message - start over */
                    }
	        } /* end if (len == HEADER_SIZE) */
            } /* end if haven't read in full header */

            if ((ab.size() >= HEADER_SIZE) && ( data.size() > 0 ))
            {
                /* we've read in the header, now read in the the user data
                 * read in as much of the rest of the LPDU as we can */
                int bytesToCompleteLpdu = expectedLen - ab.size();
                int bytesToAdd = data.end()-i;
                if (bytesToAdd > bytesToCompleteLpdu)
	        {
                    bytesToAdd = bytesToCompleteLpdu;
	        }

		j = i + bytesToAdd;
		ab.insert(ab.end(), i, j);
		i = data.erase(i, j);

                /* check the CRCs if the end of the complete frame */
                if (ab.size() == expectedLen)
                {
                    int remainderBytes =(ab.size() - HEADER_SIZE) % 18;

                    /* check CRCs on all 16 byte data chunks of the message */
	            for (j=ab.begin()+HEADER_SIZE; j<ab.end()-18; j+=18)
                    { 
                        if (checkCrc( j, 18) == 0)
                        {
                            stats_p->increment( CRC_ERRORS);
                            reset();
                            break;
                        }
                    }

	            /* check the CRC on the last variable block of bytes */
                    if (remainderBytes > 0)
	            {
                        /* because of the way we calculated expected length 
                         * remainder bytes will never be
                         * less than 3 (1 data 2 crc) */
			assert(remainderBytes >= 3);
			if(!checkCrc(ab.end()-remainderBytes,remainderBytes))
                        { 
                            stats_p->increment( CRC_ERRORS);
                            reset();
                            break;
	                }
	            }
          
                    lpduComplete = true;
                    break; /* do not process anymore data, save it for later */

	        } /* end if end of complete frame */

            } /* end if header has been read */

	} /* end else lpdu ab.size() > 0 */

    } /* closes while *buf_len > 0 */

    return lpduComplete;
} 

void Lpdu::build(uint8_t dir, uint8_t prm,
		 uint8_t fcb, uint8_t fcvDfc,
		 uint8_t fc,
		 DnpAddr_t dest, DnpAddr_t src )
{
    Bytes data;
    build(dir, prm, fcb, fcvDfc, fc, dest, src, data);
}

// constructs the entire Lpdu from the inputs provided
// and adds in the length field and all CRCs */
void Lpdu::build(uint8_t dir, uint8_t prm,
		 uint8_t fcb, uint8_t fcvDfc,
		 uint8_t fc,
		 DnpAddr_t dest, DnpAddr_t src,
		 const Bytes& data)
{      
    /* contruct the Data Link Control Field (DLC) */
    uint8_t dlc = (dir<<7) | (prm<<6) | (fcb<<5) | (fcvDfc<<4) | (fc);
    Bytes::const_iterator i;
    int remainderBytes;
    unsigned int crc;

    remainderBytes = data.size() % 16;

    ab.resize(4);
    /* first add in the start octects which are always 05 64 */
    ab[0] = 0x05;
    ab[1] = 0x64;
    ab[2] = calculateLengthField( data.size());
    ab[3] = dlc;
    /* add the destination and source address */
    appendUINT16( ab, dest);
    appendUINT16( ab, src);
    /* calculate the CRC on the Datalink Header */
    crc = calculateCrc( ab.begin(), ab.size());
    appendUINT16( ab, crc);

    /* add in blocks of 16 bytes from the application Layer and add the CRC */
    for (i=data.begin(); i<data.end()-remainderBytes; i+=16)
    {
	ab.insert(ab.end(), i, i+16);
	crc = calculateCrc( i, 16);
	appendUINT16( ab, crc);
    }

    /* adding the remaining bytes from the application layer and the CRC */
    if (remainderBytes > 0) /* only add the app data if there is any */
    {
	ab.insert(ab.end(), i, i+remainderBytes);
	crc = calculateCrc( i, remainderBytes);
	appendUINT16( ab, crc);
    }
}

/* calculates the total Lpdu length from the length field in the header */
unsigned int Lpdu::expectedLpduLength(unsigned int lengthField)
{
    int l; /* 3 + lengthField + crcs */
    if ((lengthField-5) % 16 == 0)
    { 
        l = 5 + lengthField + (((lengthField-5)/16) * 2);
    }
    else
    {
        /* add an extra two bytes for the data < 16 bytes */
        l = 7 + lengthField + (((lengthField-5)/16) * 2);
    }
  
    return l;
}

/* determines the value that should appear in the length field */
unsigned int Lpdu::calculateLengthField( int userDataLength)
{
    unsigned int l =  userDataLength + 5;
    assert (l <= 255);
    return l;
}

int Lpdu::checkDatalinkHeader()
{
    int result = 1;
    uint8_t lenField = ab[2];
    expectedLen = expectedLpduLength(lenField);
    if (lenField < 5) /*  length must be > 5 and < 255 */
        result = 0;
    if (checkCrc(ab.begin(), HEADER_SIZE) == 0)
    {
        stats_p->increment( CRC_ERRORS);
        result = 0;
    }
    return result;
}

/* the last two bytes of the array are the CRC */
int Lpdu::checkCrc( Bytes::const_iterator j, int len)
{
    unsigned int crc1 = 0;
    unsigned int crc2 = 0;
    assert (len > 2); /* can't check crc on nothing */
    crc1 = calculateCrc( j, len-2);
    crc2 = ((*(j+len-1)) << 8) + (*(j+len-2));
    if (crc1 == crc2)
        return 1;
    else
	return 0;
}

unsigned int Lpdu::calculateCrc( Bytes::const_iterator j, int len)
{
    unsigned int crc = 0;
    int i;
    for (i=0; i<len; i++, j++)
    {
        crc = (crc >> 8) ^ crcTable[(crc^(*j)) & 0x00ff];
    }
    return ( ~crc & 0xffff); /* this is 16 bit CRC */
}
