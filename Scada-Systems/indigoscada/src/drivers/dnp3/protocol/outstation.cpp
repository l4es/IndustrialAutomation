//
// $Id: outstation.cpp 17 2007-04-13 15:30:36Z sparky1194 $
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


// The Oustation class contains all of the outstation specific DNP
// application layer code.
//

#include <assert.h>
#include <stdio.h>
#include "object.hpp"
#include "security.hpp"
#include "outstation.hpp"

const char* Outstation::stateStrings[ Outstation::NUM_STATES] =
{
    "Idle",
    "Waiting For Event Confirm"
};

Outstation::Outstation( OutstationConfig&             outstationConfig,
			Datalink::DatalinkConfig&     datalinkConfig,
			EventInterface*               eventInterface_p,
			TimerInterface*               timerInterface_p)
  : Application( outstationConfig.debugLevel_p, outstationConfig.addr,
		 outstationConfig.userNum, datalinkConfig,
		 eventInterface_p, timerInterface_p ),
    secAuth(this),
    masterAddr(outstationConfig.masterAddr),
    broadcast(false)
{

    // create stationInfo and stationInfoMap to init the transport function
    StationInfo stationInfo;
    stationInfo.session_p = &session;
    stationInfo.stats_p   = &stats;
    stationInfo.addr      = masterAddr;

    StationInfoMap stationInfoMap;
    // as an outstation device we accept broadcast messages
    stationInfoMap[ masterAddr] = stationInfo;

    // initialize the transport function
    tf_p = new TransportFunction(dl, stationInfoMap);


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
        { STATE,                "State",             Stats::NORMAL,IDLE,IDLE },
        { TX_RESPONSE,          "Tx Response",         Stats::NORMAL,  0, 0 },
        { TX_NULL_RESPONSE,     "Tx Null Repsonse",    Stats::NORMAL,  0, 0 },
        { TX_CONFIRM,           "Tx Confirm",          Stats::NORMAL,  0, 0 },
        { TX_UNSOLICITED,       "Tx Unsolicited",      Stats::NORMAL,  0, 0 },
        { TX_SELECT_RESP,       "Tx Select Resp",      Stats::NORMAL,  0, 0 },
        { TX_OPERATE_RESP,      "Tx Operate Resp",     Stats::NORMAL,  0, 0 },
        { TX_FRAGMENT,          "Tx Fragment",         Stats::NORMAL,  0, 0 },
        { TX_TIME_DELAY_FINE,   "Tx Time Delay Fine",  Stats::NORMAL,  0, 0 },
        { TX_TIME_DELAY_COARSE, "Tx Time Delay Coasrse",Stats::NORMAL,  0, 0 },
        { RX_WRITE_TIME,        "Rx Write Time",       Stats::NORMAL,  0, 0 },
        { RX_DELAY_MEASUREMENT, "Rx Delay Measure",    Stats::NORMAL,  0, 0 },
        { RX_CONFIRM,           "Rx Confirm",          Stats::NORMAL,  0, 0 },
        { RX_CLASS_O_POLL,      "Rx Class 0 Poll",     Stats::NORMAL,  0, 0 },
        { RX_CLASS_1_POLL,      "Rx Class 1 Poll",     Stats::NORMAL,  0, 0 },
        { RX_CLASS_2_POLL,      "Rx Class 2 Poll",     Stats::NORMAL,  0, 0 },
        { RX_CLASS_3_POLL,      "Rx Class 3 Poll",     Stats::NORMAL,  0, 0 },
        { RX_READ,              "Rx Read",             Stats::NORMAL,  0, 0 },
        { RX_WRITE,             "Rx Write",            Stats::NORMAL,  0, 0 },
        { RX_SELECT,            "Rx Select",           Stats::NORMAL,  0, 0 },
        { RX_OPERATE,           "Rx Operate",          Stats::NORMAL,  0, 0 },
        { RX_DIR_OP,            "Rx Direct Operate",   Stats::NORMAL,  0, 0 },
        { RX_DIR_OP_NO_ACK,     "Rx Dir Op No Ack",    Stats::NORMAL,  0, 0 },
        { RX_BROADCAST,         "Rx Broadcast",        Stats::NORMAL,  0, 0 },

	// Abnormal stats.
        { IIN,                  "Internal Indications",Stats::ABNORMAL,0, 0 },
	{ NO_CONFIRM,           "Confirm Timeout",     Stats::ABNORMAL,0, 0 },
	{ RX_CONFIRM_TOO_LATE,  "Rx Confirm Too Late", Stats::ABNORMAL,0, 0 },
	{ RX_RESEND,            "Rx Resend",     Stats::ABNORMAL,0, 0 },
	{ RX_COLD_RESTART,      "Rx Cold Restart",     Stats::ABNORMAL,0, 0 },
	{ RX_WARM_RESTART,      "Rx Warm Restart",     Stats::ABNORMAL,0, 0 },
	{ RX_UNPARSABLE_DATA,   "Rx Unparsable Data",  Stats::ABNORMAL,0, 0 },
	{ RX_UNEXPECTED_CONFIRM,"Rx Unexpected Confirm",Stats::ABNORMAL,0, 0 },
       { RX_BAD_CONFIRM_SEQ_NUM,"Rx Bad Confirm Seq Num",Stats::ABNORMAL,0,0 },
	{ TX_OBJECT_UNKNOWN,    "Tx Object Unknown",   Stats::ABNORMAL,0, 0 },
	{ TX_PARAMETER_ERROR,   "Tx Parameter Error",  Stats::ABNORMAL,0, 0 },
	{ TX_FUNCTION_UNKNOWN,  "Tx Function Unknown", Stats::ABNORMAL,0, 0 },
    };

    assert (sizeof(temp)/sizeof(Stats::Element) == NUM_STATS);
    memcpy(statElements, temp, sizeof(temp));

    char name[MAX_USER_NAME_LEN];
    sprintf(name, "OS  %5d ", addr);
    stats = Stats( name, addr, outstationConfig.debugLevel_p,
		   statElements, NUM_STATS, eventInterface_p,
		   EventInterface::AP_AB_ST);

}

void Outstation::enableSecureAuthentication( bool enable)
{
    secureAuthenticationEnabled = enable;
}

DnpStat_t Outstation::getStat( int index)
{
    return stats.get( index);
}

DnpStat_t Outstation::getSecAuthStat( int index)
{
    return secAuth.stats.get( index);
}

DnpStat_t Outstation::getState() const
{
    return (State) stats.get( STATE);
}

DnpStat_t Outstation::getSecAuthState() const
{
    return secAuth.stats.get(SecureAuthentication::STATE);
}

void Outstation::changeState(State state)
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

DnpStat_t Outstation::rxData(Bytes* buf, Uptime_t timeRxd)
{
    while (buf->size() > 0)
    {
	Lpdu::UserData& segment = dl.rxData( *buf);
	if (segment.data.size() > 0)
	{
	    // this data has completed a segment
	    addr = tf_p->rxSegment( segment);
	    if (addr != TransportFunction::FRAGMENT_NOT_FOUND)
	    {
		// this data had completed a fragment
		lastRxdAsdu = session.rxFragment;
		// send any responses to this address
		destAddr = segment.src;
		processRxdFragment();
	    }
	}
    }
    return waitingFor;
}

DnpStat_t Outstation::timeout(TimerInterface::TimerId t)
{
    if (t == TimerInterface::RESPONSE)
    {
	if (waitingFor == EVENT_CONFIRM)
	{
	    stats.increment(NO_CONFIRM);
	}
    }
    return waitingFor;
}


void Outstation::processRxdFragment()
{
    AppHeader::FunctionCode fn = AppHeader::getFn(session.rxFragment);
    lastRxSeqNum = AppHeader::getSeqNum(session.rxFragment);

    if ( secAuth.rxAsdu( session.rxFragment) == false)
	return;

    ah.decode( session.rxFragment);
    // secAuth could have replaced the asdu with a queued asdu so we need
    // to get the fn again
    fn = ah.getFn();


    if (fn == AppHeader::CONFIRM)
    {
	if (lastTxSeqNum == ah.getSeqNum())
	{
	    stats.increment(RX_CONFIRM);
	    //processConfirm
	}
	else
	    stats.increment(RX_BAD_CONFIRM_SEQ_NUM);
    }
    else
    {
	if (waitingFor == EVENT_CONFIRM)
	    stats.increment(NO_CONFIRM);

	// regardless, process the new request
	if (fn == AppHeader::READ)
	{
	    stats.increment(RX_READ);
	    read();
	}
	else if (fn == AppHeader::WRITE)
	{
	    stats.increment(RX_WRITE);
	    write();
	}
	else if (fn == AppHeader::SELECT)
	{
	    stats.increment(RX_SELECT);
	    control( fn);
	}
	else if (fn == AppHeader::OPERATE)
	{
	    stats.increment(RX_OPERATE);
	    control( fn);
	}
	else if (fn == AppHeader::AUTHENTICATION_REPLY)
	{
	    // should have been handled by secAuth
	    assert(0);
	}
	else
	{
	    if (!broadcast)
	    {
		sendFunctionUnknown();
	    }
	}
    }
}

void Outstation::sendConfirm()
{
    // a confirm is simply a header with the confirm function code
    // no IIN bytes
    // first & final = 1, confirm & unsol = 0
    initResponse( 1, 1, 0, 0, 0, AppHeader::CONFIRM);
    transmit();
    stats.increment(Outstation::TX_CONFIRM);
}


void Outstation::initResponse( bool fir, bool fin, bool con, bool uns,
			       uint16_t additionalIin,
			       AppHeader::FunctionCode fn)
{ 
    txFragment.clear();
    AppHeader a(fir,fin,con,uns,lastRxSeqNum,fn,stats.get(IIN)|additionalIin);
    a.encode(txFragment);
}

void Outstation::transmit()
{
    // hand it off to the transport function
    tf_p->transmit( destAddr, txFragment, stats);
}


void Outstation::read()
{
    DnpObject* obj_p = NULL;
    bool parseOk = true;
    bool sendStaticData = false;
    bool sendEvents = false;
    // app header has already been stripped off
    while (session.rxFragment.size() > 0)
    {
	try
	{
	    oh.decode( session.rxFragment, stats );
	    stats.logNormal(oh.str(strbuf, sizeof(strbuf)));
	    if (oh.grp != 60)
		obj_p = of.decode(oh, session.rxFragment, addr, stats);
	}
	catch (int e)
	{
	    stats.increment( RX_UNPARSABLE_DATA);
	    stats.logAbnormal(0, "Caught exception line# %d", e);
	    parseOk = false;
	    break;
	}

	// handle special cases
	if (oh.grp == 120)
	{
	    // these objects are all supposed to come one at a time
	    // with no other objects in the fragment
	    if (session.rxFragment.size() > 0)
	    {
		stats.logAbnormal(0, "Format not expected");
		parseOk = false;
		break;
	    }

	    if (oh.var == 4)
	    {
		secAuth.rxKeyStatusReq((SessionKeyStatusReq*) obj_p);
	    }
	    else if (oh.var == 1)
	    {
		secAuth.rxChallenge((Challenge*) obj_p);
	    }
	}
	else if (oh.grp == 60)
	{
	    if  (oh.var==1)
		sendStaticData = true;
	    else
		sendEvents = true;
	}
    }

    if (!parseOk)
    {
	sendParameterError();
    }
    else if (sendStaticData)
    {
	// for prototype send 3 online binary inputs
	initResponse( 1, 1, 0, 0);
	oh = ObjectHeader(1,2,0,0,0,2);
	oh.encode(txFragment);
	BinaryInputWithStatus obj = BinaryInputWithStatus(1);
	obj.encode(txFragment);
	obj.encode(txFragment);
	obj = BinaryInputWithStatus(0);
	obj.encode(txFragment);

	transmit();
	stats.increment(TX_RESPONSE);
    }
    else if (sendEvents)
    {
	sendNullResponse();
    }
	

}

void Outstation::write()
{
    DnpObject* obj_p = NULL;
    bool parseOk = true;
    // app header has already been stripped off
    while (session.rxFragment.size() > 0)
    {
	try
	{
	    oh.decode( session.rxFragment, stats );
	    stats.logNormal(oh.str(strbuf, sizeof(strbuf)));
	    if (oh.grp == 120)
		obj_p = of.decode(oh, session.rxFragment, addr, stats);
	}
	catch (int e)
	{
	    stats.increment( RX_UNPARSABLE_DATA);
	    stats.logAbnormal(0, "Caught exception line# %d", e);
	    parseOk = false;
	    break;
	}

	// handle special cases
	if (oh.grp == 120)
	{
	    // these objects are all supposed to come one at a time
	    // with no other objects in the fragment
	    if (session.rxFragment.size() > 0)
	    {
		stats.logAbnormal(0, "Format not expected");
		parseOk = false;
		break;
	    }

	    if (oh.var == 6)
	    {
		secAuth.rxKeyChange((SessionKeyChange*) obj_p);
	    }
	}
	else if ((oh.grp == 80) && (oh.var == 1))
	{
	    if ((oh.qual == 0) && (oh.start==7) && (oh.stop==7))
	    {
		uint16_t iin = stats.get(IIN);
		// decode the value - it should always be 0;
		removeUINT8(session.rxFragment);
		if (iin & InternalIndications::DEVICE_RESTART)
		{
		    iin &= ~(InternalIndications::DEVICE_RESTART);
		    stats.set(IIN, iin);
		}
	    }
	    else
	    {
		parseOk = false;
		break;
	    }
	}
    }

    if (!parseOk)
    {
	sendParameterError();
    }
}

void Outstation::control(AppHeader::FunctionCode fn)
{
    initResponse( 1, 1, 0, 0);
    // append a copy of the control request (which happens to be the
    // remaining portion of the request)
    appendBytes( txFragment, session.rxFragment);

    transmit();

    if (fn == AppHeader::SELECT)
	stats.increment(TX_SELECT_RESP);
    else if (fn == AppHeader::OPERATE)
	stats.increment(TX_OPERATE_RESP);
    else
	assert(0);
}


void Outstation::sendNullResponse()
{
    initResponse( 1, 1, 0, 0);
    transmit();
    stats.increment(TX_NULL_RESPONSE);
    stats.increment(TX_RESPONSE);
}

void Outstation::sendFunctionUnknown()
{
    initResponse( 1, 1, 0, 0, InternalIndications::FUNCTION_UNKNOWN);
    transmit();
    stats.increment(TX_FUNCTION_UNKNOWN);
    stats.increment(TX_RESPONSE);
}

void Outstation::sendParameterError()
{
    initResponse( 1, 1, 0, 0, InternalIndications::PARAMETER_ERROR);
    transmit();
    stats.increment(TX_PARAMETER_ERROR);
    stats.increment(TX_RESPONSE);
}


void Outstation::appendVariableSizedObject( const ObjectHeader& h,
					    const DnpObject& o)
{
    stats.logNormal( h.str( strbuf, sizeof(strbuf)));    
    h.encode( txFragment);
    appendUINT16( txFragment, o.size()); 
    o.encode( txFragment);
}
