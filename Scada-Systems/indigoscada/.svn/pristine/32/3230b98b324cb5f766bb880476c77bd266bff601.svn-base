//
// $Id: outstation.hpp 12 2007-04-12 22:17:18Z sparky1194 $
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

#ifndef OUTSTATION_H
#define OUTSTATION_H

#include "common.hpp"
#include "app.hpp"
#include "asdu.hpp"
#include "object.hpp"
#include "security.hpp"

class Outstation : Application
{
public:

    typedef struct
    {
	DnpAddr_t        addr;
	DnpAddr_t        masterAddr;
	UserNumber_t     userNum;
	int*             debugLevel_p;
    } OutstationConfig;

    // state can either be waiting for something or not
    enum State         {  IDLE = 0,
			  EVENT_CONFIRM,
			  NUM_STATES  };

    void changeState(State state);

    static const char* stateStrings[ NUM_STATES];

    Outstation( OutstationConfig&          outstationConfig,
		Datalink::DatalinkConfig&  datalinkConfig,
		EventInterface*            eventInterface_p,
		TimerInterface*            timerInterface_p );


    DnpStat_t rxData(Bytes* buf, Uptime_t timeRxd=0);
    DnpStat_t timeout(TimerInterface::TimerId t);

    DnpStat_t getState() const;
    DnpStat_t getSecAuthState() const;

    void enableSecureAuthentication(bool enable=true);

    enum StatIndex     {  STATE  = TransportStats::NUM_STATS,
			  TX_RESPONSE,
			  TX_NULL_RESPONSE,
			  TX_CONFIRM,
			  TX_UNSOLICITED,
			  TX_SELECT_RESP,
			  TX_OPERATE_RESP,
			  TX_FRAGMENT,
			  TX_TIME_DELAY_FINE,
			  TX_TIME_DELAY_COARSE,
			  RX_WRITE_TIME,
			  RX_DELAY_MEASUREMENT,
			  RX_CONFIRM,
			  RX_CLASS_O_POLL,
			  RX_CLASS_1_POLL,
			  RX_CLASS_2_POLL,
			  RX_CLASS_3_POLL,
			  RX_READ,
			  RX_WRITE,
			  RX_SELECT,
			  RX_OPERATE,
			  RX_DIR_OP,
			  RX_DIR_OP_NO_ACK,
			  RX_BROADCAST,
			  IIN,

			  // Abnormal
			  NO_CONFIRM,
			  RX_CONFIRM_TOO_LATE,
			  RX_RESEND,
			  RX_COLD_RESTART,
			  RX_WARM_RESTART,
			  RX_UNPARSABLE_DATA,
			  RX_UNEXPECTED_CONFIRM,
			  RX_BAD_CONFIRM_SEQ_NUM,
			  TX_OBJECT_UNKNOWN,
			  TX_PARAMETER_ERROR,
			  TX_FUNCTION_UNKNOWN,
			  NUM_STATS };

    Stats::Element      statElements[NUM_STATS];
    Stats               stats;

    // primarity for unit testing
    DnpStat_t getStat( int index);
    DnpStat_t getSecAuthStat( int index);

private:
    friend class SecureAuthentication;
    friend class OutstationSecurity;

    // call when a new fragment has been rxd by the transport function
    void processRxdFragment();

    void read();
    void write();
    void control( AppHeader::FunctionCode fn);
    void controlNoResp( AppHeader::FunctionCode fn);
    void sendNullResponse();
    void sendFunctionUnknown();
    void sendObjectUnknown();
    void sendParameterError();

    // this method begins the construction of a response by
    // starting with the application header
    void initResponse(bool fir, bool fin, bool con, bool uns,
		      uint16_t additionalIin = 0,
		      AppHeader::FunctionCode fn = AppHeader::RESPONSE);

    void sendConfirm();

    void sendFragments();
    void sendNextFragment();
    void transmit();
    void appendVariableSizedObject(const ObjectHeader& h, const DnpObject& o);

    State                   waitingFor;
    Bytes                   txFragment;
    TransportSession        session;       // used by the transport function
    OutstationSecurity      secAuth;

    AppSeqNum_t             lastRxSeqNum;
    AppSeqNum_t             lastTxSeqNum;
    DnpAddr_t               masterAddr;
    DnpAddr_t               destAddr;     // addr to send responses to
    bool                    broadcast; // true if last rxd frag was broadcast
};

#endif
