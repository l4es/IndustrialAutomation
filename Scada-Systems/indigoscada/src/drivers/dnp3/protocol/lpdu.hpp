//
// $Id: lpdu.hpp 4 2007-04-10 22:55:27Z sparky1194 $
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

#ifndef DNP_LPDU_H
#define DNP_LPDU_H

#include "common.hpp"
#include "stats.hpp"

class Lpdu
{

public:
    // includes start, len and crc bytes
	#define HEADER_SIZE       10
    // includes start, len and crc bytes
    #define  MAX_LEN                    292
    // doesn't incl dl header or crc bytes
    #define  MAX_USER_DATA              234
    // doesn't include transport header
    #define  MAX_APP_DATA               233

    enum StatIndex { RX_START_OCTETS = 0,
		     LOST_BYTES,
		     CRC_ERRORS,
		     NUM_STATS };

    typedef struct
    {
	Bytes          data;
	DnpAddr_t      src;
	DnpAddr_t      dest;
    } UserData;

    Lpdu(Stats* datalinkStats_p = NULL);

    // fills in a structure with the user data (no crcs)
    void         getUserData(UserData& u) const;

    unsigned int getLenField() const;
    DnpAddr_t    getSrc() const;
    DnpAddr_t    getDest() const;
    unsigned int getFunctionCode() const;
    unsigned int getDirectionBit() const;
    unsigned int getPrimaryBit() const;
    unsigned int getFcv() const; // frame count valid
    unsigned int getFcb() const; // fcb should alternate if valid
    unsigned int getDataFlowControlBit() const;
    char*        str(char* buf, int len) const;  // descriptive string
    char*        hex(char* buf, int len) const;  // hex string

    // Builds an Lpdu from incoming bytes. Can be called several times if
    // all byte buffers must be sequential (received in order) to build an lpdu
    // Returns:  true if a complete lpdu has been found.
    //           The Lpdu structure will be filled in.
    //           and lpdus if found returns the number of unused bytes
    //           in the buffer if 1 a complete lpdu was foud
    // bytes that are used to build the lpdu will be removed from the
    // beginning of the data.

    bool buildFromBytes(Bytes& data);

    // Remember:
    // Master->Slave Direction Bit (DIR) = 1
    // From initiating station Primary (PRM) = 1
    void build( uint8_t dir, uint8_t prm,
		uint8_t fcb, uint8_t fcvDfc,
		uint8_t fc,
		DnpAddr_t dest, DnpAddr_t src,
		const Bytes& data);

    // for building header only frames
    void build( uint8_t dir, uint8_t prm,
		uint8_t fcb, uint8_t fcvDfc,
		uint8_t fc,
		DnpAddr_t dest, DnpAddr_t src);

    // exposed for unit testing but otherwise would be private
    void            reset();
    unsigned int    getStat( int index);

    Bytes           ab;

private:

    Stats*          stats_p;

    // for testing/debug to convert values to human readable strings 
    //          [Primary Bit][Function Code]
    static const char             fcMap[2][16][25];
    static const unsigned short   crcTable[256];

    unsigned int                  expectedLen;// valid after header is read

    unsigned int expectedLpduLength( unsigned int lengthField);
    unsigned int calculateLengthField( int userDataLength);
    int          checkDatalinkHeader();
    int          checkCrc( Bytes::const_iterator j, int len);
    unsigned int calculateCrc( Bytes::const_iterator j, int len);

};



#endif
