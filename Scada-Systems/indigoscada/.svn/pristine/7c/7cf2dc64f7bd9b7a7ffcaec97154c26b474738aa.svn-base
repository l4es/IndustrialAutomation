//
// $Id: transport.hpp 4 2007-04-10 22:55:27Z sparky1194 $
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

#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <map>
#include "common.hpp"
#include "lpdu.hpp"
#include "datalink.hpp"
#include "stats.hpp"

#pragma warning(disable: 4786)

typedef struct {
    bool          inProgress;
    Bytes         rxFragment;
    uint8_t       nextSeqNum;
} TransportSession;

typedef struct {
    TransportSession*  session_p;
    Stats*             stats_p;
    DnpAddr_t          addr;     // address of the receiving station
                                 // which can be a broadcast address
} StationInfo;

typedef std::map<DnpAddr_t, StationInfo> StationInfoMap;

class TransportStats
{
public:
    enum StatIndex { TX_FRAGMENT = 0,
		     TX_SEGMENT,
		     RX_FRAGMENT,
		     RX_SEGMENT,
		     RX_UNAUTH_SEG,
		     RX_ROUGE_SEG,
		     RX_BAD_TH_SEQ_NUM,
		     NUM_STATS };
};


class TransportFunction
{

public:
    TransportFunction( Datalink&               datalink,
		       const StationInfoMap&   stationInfoMap);

    Uptime_t transmit( DnpAddr_t         txAddr,
		       const Bytes&      fragment,
		       Stats&            stats);

    // this is a reserved DNP address that we will use internally
    // if the DNP protocol is ammended to use this address for something
    // we may have to change the way we return from the rxSegment method
    // when a fragment is not found

    //static const DnpAddr_t FRAGMENT_NOT_FOUND = 0xfff0;
	static const DnpAddr_t FRAGMENT_NOT_FOUND;

    // returns the receiving station's dnp address
    // when a complete fragment is found
    // returns FRAGMENT_NOT_FOUND when incomplete
    DnpAddr_t rxSegment( const Lpdu::UserData& segment);

private:

    Datalink&            dl;
    StationInfoMap       stnInfoMap;

    // should be more than adequate for the header str
    char             strbuf[80];
};


class TransportHeader
{

public:
    TransportHeader(uint8_t th=0);
    TransportHeader(bool fin, bool fir, uint8_t seqNum);
    bool getFirst();
    bool getFinal();
    uint8_t getSeqNum();
    char* str( char* buf, int len);
    static void incrementSeqNum(uint8_t &seqNum);
    void decode( Bytes& segment);

    uint8_t b; // th is really just a single byte of data
};


#endif 
