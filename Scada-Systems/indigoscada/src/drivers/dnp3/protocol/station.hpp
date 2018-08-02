//
// $Id: station.hpp 19 2007-04-13 21:35:41Z sparky1194 $
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

#ifndef STATION_H
#define STATION_H

#include "common.hpp"
#include "stats.hpp"
#include "asdu.hpp"
#include "event_interface.hpp"
#include "security.hpp"
#include "transport.hpp"

#pragma warning (disable : 4786) //apa+++ 12-06-2012

class Master;

class Station
{

public:

    typedef struct
    {
          DnpAddr_t      addr;
	//    DnpControlBlock_t   controlBlock;
    } ControlRequest;

    typedef struct
    {
	      DnpAddr_t      addr;
	      int*           debugLevel_p;
	      Master*        master_p;
    } StationConfig;


    Station( DnpAddr_t masterAddr, StationConfig& config,
	     EventInterface* eventInterface_p);

    // state can either be waiting for a response or not
    enum State         {  IDLE = 0,
			  POLL_RESP,
			  CLEAR_RESTART_BIT_RESP,
			  SELECT_RESP,
			  OPERATE_RESP,
			  WRITE_TIME_AND_DATE_RESP,
			  DELAY_MEASUREMENT_RESP,
			  NUM_STATES   };

    void changeState(State state);

    static const char* stateStrings[ NUM_STATES];

    enum StatIndex     {  RESPONSE_TIMEOUT  = TransportStats::NUM_STATS,
			  STATE,
			  CONSECUTIVE_TIMEOUT,
			  DELAY_MEASUREMENT,
			  IIN,
			  RX_IIN_LOCAL,
			  RX_IIN_NEED_TIME,
			  RX_UNSOLICITED,
			  RX_RESPONSE,
			  TX_INTEGRITY_POLL,
			  TX_EVENT_POLL,
			  TX_READ_REQUEST,
			  TX_WRITE_REQUEST,
			  TX_DELAY_MEASUREMENT,
			  TX_TIME_AND_DATE,
			  TX_SELECT,
			  TX_OPERATE,
			  TX_CONFIRM,
			  // Abnormal
			  COMMUNICATION,
			  // TX_RESEND, master does not support retries
			  RX_IIN_RESTART,
			  RX_IIN_TROUBLE,
			  RX_IIN_BAD_CONFIG,
			  RX_IIN_BUFFER_OVERFLOW,
			  RX_IIN_PARAMETER_ERR,
			  RX_IIN_OBJECT_UNKNOWN,
			  RX_IIN_FUNCTION_UNKOWN,
			  RX_RESEND,
			  RX_UNPARSABLE_DATA,
			  RX_ROUGE_FRAGMENT,
			  RX_UNEXP_RESPONSE,
			  RX_UNSUPPORTED_FN,
			  RX_CORRUPT_DATA,
			  RX_CONFIRM,
			  RX_UNEXP_CONFIRM,
			  RX_BAD_AH_SEQ_NUM,
			  RX_BAD_WRITE_RESP,
			  RX_BAD_SELECT_RESP,
			  RX_BAD_OPERATE_RESP,
			  RX_BAD_DELAY_MEASUREMENT,
			  RX_MISSED_AH_FIN,
			  RX_MISSED_AH_FIR,
			  NUM_STATS };

    DnpAddr_t           addr;
    Stats               stats;
    int                 sendIntegrityPoll;

    //  note that these are app seqNums
    // (the transport seqNums are in the session structure)
    AppSeqNum_t         lastTxSeqNum;
    AppSeqNum_t         nextRxSeqNum;
    AppSeqNum_t         nextRxUnsolicitedSeqNum;
    int                 rxingMultiFragmentResponse;
    int                 rxingMultiFragmentUnsolicitedResponse;
    Uptime_t            timeRxd; // time the last fragment was received
    Bytes               txFragment;
    Bytes               lastControlFrag; // needed because sec auth can
                                         // overwrite the last control request
    Uptime_t            timeSent;
    Uptime_t            uptime;
    ControlOutputRelayBlock cb;
    Stats::Element      statElements[NUM_STATS];

    TransportSession    session;       // used by the transport function

    MasterSecurity      secAuth;
};


#endif
