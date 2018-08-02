//
// $Id: station.cpp 12 2007-04-12 22:17:18Z sparky1194 $
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

#include <time.h>
#include <stdio.h>
#include <string.h>  // for memcpy
#include <assert.h>
#include "station.hpp"
#include "transport.hpp"

const char* Station::stateStrings[ Station::NUM_STATES] =
{
    "Idle",
    "Waiting for Poll Response",
    "Waiting for Clear Restart Bit Respsonse",
    "Waiting for Select Response",
    "Waiting for Operate Response",
    "Waiting for Write Time Repsonse",
    "Waiting for Delay Measurement"
};

Station::Station( DnpAddr_t masterAddr, StationConfig& config,
		  EventInterface* eventInterface_p) :
  addr(config.addr),
  sendIntegrityPoll(1),
  secAuth(config.master_p)
{
    //   the master will never implement retries and
    //   therefor this seqNum will be incremented for
    //   every transmission to this ied
    //   see TBs 9804-001, 9804-002, and 2000-002
    lastTxSeqNum = 4;//  start something other than 0 or 1 to stand out
    nextRxSeqNum                 = AppHeader::UNDEFINED_SEQ_NUM;
    nextRxUnsolicitedSeqNum      = AppHeader::UNDEFINED_SEQ_NUM;
    rxingMultiFragmentResponse   = false;
    rxingMultiFragmentUnsolicitedResponse = false;
    timeSent                     = 0;


    // for use by the transport function
    session.inProgress           = false;
    session.nextSeqNum           = AppHeader::UNDEFINED_SEQ_NUM;

    Stats::Element temp[] =
    {
	// the following stats are incremented by the TransportFunction class.
	// We are initializing them here because it is simpler to
	// have one stats class for the outstation regardless of which
	// class is incrementing the stat
	{ TransportStats::TX_FRAGMENT,"Tx Fragments"  ,Stats::NORMAL,  0, 0 },
	{ TransportStats::TX_SEGMENT,"Tx Segments"    ,Stats::NORMAL,  0, 0 },
	{ TransportStats::RX_FRAGMENT,"Rx Fragments"  ,Stats::NORMAL,  0, 0 },
	{ TransportStats::RX_SEGMENT,"Rx Segments"    ,Stats::NORMAL,  0, 0 },
	{ TransportStats::RX_UNAUTH_SEG,"Rx Unauth Seg",Stats::ABNORMAL,0, 0 },
	{ TransportStats::RX_ROUGE_SEG,"Rx Rouge Seg"  ,Stats::ABNORMAL,0, 0 },
	{ TransportStats::RX_BAD_TH_SEQ_NUM,"Rx Bad Th SeqNum",
	                                               Stats::ABNORMAL,0, 0 },
	// Normal stats
        { RESPONSE_TIMEOUT,    "Response Timeouts",    Stats::NORMAL,  0, 0 },
        { STATE,               "State",               Stats::NORMAL,IDLE,IDLE},
	{ CONSECUTIVE_TIMEOUT, "Consecutive Timeouts", Stats::NORMAL,  0, 0 },
	{ DELAY_MEASUREMENT,   "Delay Measurement",    Stats::NORMAL,  0, 0 },
	{ IIN,                 "Current IIN Bits",     Stats::NORMAL,  0, 0 },
	{ RX_IIN_LOCAL,        "Rx IIN Locals",        Stats::NORMAL,  0, 0 },
	{ RX_IIN_NEED_TIME,    "Rx IIN Need Times",    Stats::NORMAL,  0, 0 },
	{ RX_UNSOLICITED,      "Rx Unsolicited",       Stats::NORMAL,  0, 0 },
	{ RX_RESPONSE,         "Rx Responses",         Stats::NORMAL,  0, 0 },
	{ TX_INTEGRITY_POLL,   "Tx Integrity Polls",   Stats::NORMAL,  0, 0 },
	{ TX_EVENT_POLL,       "Tx Event Polls",       Stats::NORMAL,  0, 0 },
	{ TX_READ_REQUEST,     "Tx Read Requests",     Stats::NORMAL,  0, 0 },
	{ TX_WRITE_REQUEST,    "Tx Write Requests",    Stats::NORMAL,  0, 0 },
	{ TX_DELAY_MEASUREMENT,"Tx Delay Measurement", Stats::NORMAL,  0, 0 },
	{ TX_TIME_AND_DATE,    "Tx Time and Date",     Stats::NORMAL,  0, 0 },
	{ TX_SELECT,           "Tx Selects",           Stats::NORMAL,  0, 0 },
	{ TX_OPERATE,          "Tx Operates",          Stats::NORMAL,  0, 0 },
	{ TX_CONFIRM,          "Tx Confirms",          Stats::NORMAL,  0, 0 },

	// Abnormal stats.
	// on startup communications state is unknown so set to Failed (0)
	// and a log will be printed when communications are Ok (1)
	{ COMMUNICATION,       "Communication",        Stats::ABNORMAL,0, 0 },
	{ RX_IIN_RESTART,      "Rx IIN Restarts",      Stats::ABNORMAL,0, 0 },
	{ RX_IIN_TROUBLE,      "Rx IIN Troubles",      Stats::ABNORMAL,0, 0 },
	{ RX_IIN_BAD_CONFIG,   "Rx IIN Bad Configs",   Stats::ABNORMAL,0, 0 },
	{ RX_IIN_BUFFER_OVERFLOW,"Rx IIN Buffer Ovflws",Stats::ABNORMAL,0, 0 },
	{ RX_IIN_PARAMETER_ERR,"Rx IIN Parameter Err", Stats::ABNORMAL,0, 0 },
	{ RX_IIN_OBJECT_UNKNOWN,"Rx IIN Object Unknwn",Stats::ABNORMAL,0, 0 },
	{ RX_IIN_FUNCTION_UNKOWN,"Rx IIN Func Unknwn", Stats::ABNORMAL,0, 0 },
	{ RX_RESEND,           "Rx Resends",           Stats::ABNORMAL,0, 0 },
	{ RX_UNPARSABLE_DATA,  "Rx Unparsable Data",   Stats::ABNORMAL,0, 0 },
	{ RX_ROUGE_FRAGMENT,   "Rx Rouge Fragments",   Stats::ABNORMAL,0, 0 },
	{ RX_UNEXP_RESPONSE,   "Rx Unexp Responses",   Stats::ABNORMAL,0, 0 },
	{ RX_UNSUPPORTED_FN,   "Rx Unsupported Fn",    Stats::ABNORMAL,0, 0 },
	{ RX_CORRUPT_DATA,     "Rx Corrupt Data",      Stats::ABNORMAL,0, 0 },
	{ RX_CONFIRM,          "Rx Confirms",          Stats::ABNORMAL,0, 0 },
	{ RX_UNEXP_CONFIRM,    "Rx Unexp Confirms",    Stats::ABNORMAL,0, 0 },
	{ RX_BAD_AH_SEQ_NUM,   "Rx Bad AH Seq Nums",   Stats::ABNORMAL,0, 0 },
	{ RX_BAD_WRITE_RESP,   "Rx Bad Write Resp",    Stats::ABNORMAL,0, 0 },
	{ RX_BAD_SELECT_RESP,  "Rx Bad Select Resp",   Stats::ABNORMAL,0, 0 },
	{ RX_BAD_OPERATE_RESP, "Rx Bad Operate Resp",  Stats::ABNORMAL,0, 0 },
	{ RX_BAD_DELAY_MEASUREMENT,"Rx Bad Delay Msrmnt",Stats::ABNORMAL,0,0},
	{ RX_MISSED_AH_FIN,    "Rx Missed AH FIN",     Stats::ABNORMAL,0, 0 },
	{ RX_MISSED_AH_FIR,    "Rx Missed AH FIR",     Stats::ABNORMAL,0, 0 },

    };

    assert (sizeof(temp)/sizeof(Stats::Element) == NUM_STATS);
    memcpy(statElements, temp, sizeof(temp));

    char name[MAX_USER_NAME_LEN];
    sprintf(name, "MS (%5d)", addr);
    stats = Stats( name, addr, config.debugLevel_p, statElements, NUM_STATS,
		   eventInterface_p,
		   EventInterface::AP_AB_ST);

}

void Station::changeState(State state)
{
    State old = (State) stats.get(STATE);
    if ( old != state)
    {
	stats.logNormal("State change: %s -> %s",
			stateStrings[ old],
			stateStrings[ state] );
	stats.set( STATE, state);
    }
}
