//
// $Id: transport.cpp 9 2007-04-12 20:31:20Z sparky1194 $
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

// The whole point of the transport function in DNP is:
// If the application layer fragment is greater than MAX_APP_DATA_SIZE then the
// transport function splits it into smaller segments (and vice versa.)

// one Transport Function object per DNP application layer object

#include <assert.h>
#include <stdio.h>
#include "stats.hpp"
#include "transport.hpp"


TransportHeader::TransportHeader( bool fin, bool fir, uint8_t seqNum)
{
    b = seqNum;
    if (fin)
	b |= 0x80;
    if (fir)
	b |= 0x40;
}

TransportHeader::TransportHeader( uint8_t th)
{
    b = th;
}

void TransportHeader::incrementSeqNum( uint8_t &seqNum)
{
    seqNum++;
    seqNum &= 0x3f;  // th uses 6 bits for seq num 
}
    
bool TransportHeader::getFirst()
{
    return ((b & 0x40) == 0x40) ? true : false;
}

bool TransportHeader::getFinal()
{
    return ((b & 0x80) == 0x80) ? true : false;
}

uint8_t TransportHeader::getSeqNum()
{
    return b & 0x3f;
}

char* TransportHeader::str(char* buf, int len)
{
    sprintf( buf, 
	     "Th: FIN:%d,FIR=%d,Seq:%d",
	     getFinal(), getFirst(), getSeqNum());
    return buf;
}


TransportFunction::TransportFunction( Datalink&               datalink,
				      const StationInfoMap&   stationInfoMap)
  : dl(datalink), stnInfoMap(stationInfoMap)
{
    StationInfoMap::iterator iter;
    for(iter=stnInfoMap.begin(); iter != stnInfoMap.end(); iter++)
	iter->second.session_p->inProgress = false;
}

// returns the time that the first bit was sent
Uptime_t TransportFunction::transmit( DnpAddr_t         txAddr,
				      const Bytes&      fragment,
				      Stats&            stats)
{
    Bytes            segment;
    bool             fir = true;
    uint8_t          seqNum = 0;

    Bytes::const_iterator i = fragment.begin();

    while (i != fragment.end())
    {
	segment.clear();
	if (fragment.end() - i <= MAX_APP_DATA)
	{
	    // last segment in the sequence
	    TransportHeader th(true, fir, seqNum);
	    stats.logNormal("Tx %s",th.str(strbuf, sizeof(strbuf)));
	    segment.push_back(th.b);
	    segment.insert( segment.end(), i, fragment.end());
	    i = fragment.end();
	    stats.increment(TransportStats::TX_FRAGMENT);
	    stats.increment(TransportStats::TX_SEGMENT);
	    return dl.transmit( txAddr, segment);
	}
	else
	{
	    // not the last segment in the sequence
	    TransportHeader th(false, fir, seqNum);
	    stats.logNormal("Tx %s",th.str(strbuf, sizeof(strbuf)));
	    segment.push_back(th.b);
	    segment.insert( segment.end(), i, i+ MAX_APP_DATA);
	    i += MAX_APP_DATA;
	    stats.increment(TransportStats::TX_SEGMENT);
	    dl.transmit( txAddr, segment);
	    fir = false; // all subsequent segments will not be the firstx

	    // Sequence numbers only have to be consistent with the segments
	    // within a fragment. They do not have to be consistent between
	    // fragments. This allows us not to keep track of sequence
	    // numbers for each device we talk to.
	    th.incrementSeqNum( seqNum);
	}
    }
    
    assert(0);
    return 0; //apa+++
}

const DnpAddr_t TransportFunction::FRAGMENT_NOT_FOUND = 0xfff0;

DnpAddr_t TransportFunction::rxSegment( const Lpdu::UserData& segment)
{
    const Bytes& data = segment.data;
    StationInfo info;
    DnpAddr_t returnValue = FRAGMENT_NOT_FOUND; 

    // we only accept segments from configured outstations/masters
    if (stnInfoMap.count(segment.src) == 0)
    {
	// we don't know who this is so just use the first one
	// for stats purposes
	info = stnInfoMap.begin()->second;  // pair of (addr, info)
        info.stats_p->increment(TransportStats::RX_UNAUTH_SEG);
    }
    else
    {   
	info = stnInfoMap[segment.src];
	info.stats_p->increment(TransportStats::RX_SEGMENT);

        // transport header is the first byte
        TransportHeader th(data[0]);
        bool fin = th.getFinal();
        bool fir = th.getFirst();

	info.stats_p->logNormal("Rx %s", th.str(strbuf, sizeof(strbuf)));

        if (fir)
	{
	    info.session_p->rxFragment.clear();

            // check no session exists
            if (info.session_p->inProgress)
	    {
                info.stats_p->increment(TransportStats::RX_ROUGE_SEG);
                // discard existing erroneus session
                info.session_p->inProgress = false;
	    }

	    // add the app part of the segment to our app fragment
	    info.session_p->rxFragment.insert(info.session_p->rxFragment.end(),
					      ++data.begin(), data.end());

	    if (fin)
		// this is the first and last segment in the transaction
		// so it is a complete fragment
		returnValue = info.addr;
	    else
	    {
		// fin is 0
		// create new session
		// note this is the only place where we add new sessions
		info.session_p->inProgress = true;
		info.session_p->nextSeqNum = th.getSeqNum();
		th.incrementSeqNum( info.session_p->nextSeqNum);
	    }
	}
        else
	{
	    // fir is 0
            // check session exists
            if (!info.session_p->inProgress)
	    {
                info.stats_p->increment(TransportStats::RX_ROUGE_SEG);
		info.stats_p->logAbnormal(0,"fir=0, fin=%d, session not found",
					  fin);
                // discard existing erroneus session
                info.session_p->inProgress = false;
		info.session_p->rxFragment.clear();
	    }
	    else
	    {
		// check sequenceNumber
		if (info.session_p->nextSeqNum == th.getSeqNum())
		{
		    // sequence number is good
		    // add the app part of the segment to our app fragment
		    info.session_p->rxFragment.insert(
					      info.session_p->rxFragment.end(),
					      ++data.begin(), data.end());

		    if (fin)
		    {
			// fir = 0 and fin = 1
			// close session, this is final segment of the fragment
			info.session_p->inProgress = false;
			returnValue = info.addr;
		    }
		    else
		    {
			// update expected seqNum
			info.session_p->nextSeqNum = th.getSeqNum();
			th.incrementSeqNum( info.session_p->nextSeqNum);
		    }
		}
		else
		{
		    info.stats_p->increment(TransportStats::RX_BAD_TH_SEQ_NUM);
		    info.stats_p->logAbnormal( 0,
		      "fir=0, fin=%d, seqNum not right. Expexted %d, got: %d",
		      fin, info.session_p->nextSeqNum, th.getSeqNum());
		    // discard existing erroneus session
		    info.session_p->inProgress = false;
		    info.session_p->rxFragment.clear();
		}
	    }
	}
    }

    if ( returnValue != FRAGMENT_NOT_FOUND)
	info.stats_p->increment(TransportStats::RX_FRAGMENT);

    return returnValue;
}


