//
// $Id: master.cpp 24 2007-04-16 18:48:12Z sparky1194 $
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
#include "master.hpp"
#include "object.hpp"
#include "security.hpp"

Master::Master( MasterConfig&                 masterConfig,
		Datalink::DatalinkConfig&     datalinkConfig,
	        Station::StationConfig        stationConfig[],
	        int                           numStations,
		EventInterface*               eventInterface_p,
		TimerInterface*               timerInterface_p)
  : Application( masterConfig.debugLevel_p, masterConfig.addr,
		 masterConfig.userNum, datalinkConfig,
		 eventInterface_p, timerInterface_p ),
    maxConsecutive( masterConfig.consecutiveTimeoutsForCommsFail),
    integrityPollInterval_p( masterConfig.integrityPollInterval_p)
{
    int i;
    StationInfo info;

    // create each outstation object and store it in a map 
    for (i=0; i<numStations; i++)
    {
	    Station *op;
	    stationConfig[i].master_p = this;
	    op = new Station(masterConfig.addr, stationConfig[i], db_p);
	    stationMap[op->addr] = op;

	    // fill in the station info to be used by the transport function
	    info.session_p = &op->session;
	    info.stats_p = &op->stats;
	    info.addr = op->addr;
	    stationInfoMap[ op->addr] = info;
    }

    currentStationPair = stationMap.begin();
    stn_p = currentStationPair->second; // stn_p is the pair's 2nd part

    // initialize the transport function
    tf_p = new TransportFunction(dl, stationInfoMap);

}

void Master::enableSecureAuthentication(DnpAddr_t stationAddr, bool enable)
{
    assert (stationMap.count(stationAddr) > 0);

    if (enable)
    {
    	stationMap[ stationAddr]->secAuth.init();
    }

    secureAuthenticationEnabled = enable;
}

DnpStat_t Master::getStat( DnpAddr_t stationAddr, int index)
{
    assert (stationMap.count( stationAddr) > 0);
    return stationMap[ stationAddr]->stats.get( index);
}

DnpStat_t Master::getSecAuthStat( DnpAddr_t stationAddr, int index)
{
    assert (stationMap.count( stationAddr) > 0);
    return stationMap[ stationAddr]->secAuth.stats.get( index);
}

DnpStat_t Master::getState() const
{
    return stn_p->stats.get(Station::STATE);
}

DnpStat_t Master::getSecAuthState() const
{
    return stn_p->secAuth.stats.get(SecureAuthentication::STATE);
}

DnpStat_t Master::rxData(Bytes* buf, Uptime_t timeRxd)
{
    // put the buf into a Bytes container
//     Bytes data(buf, buf+buf_len);

    while (buf->size() > 0)
    {
	    Lpdu::UserData& segment = dl.rxData( *buf);

	    if (segment.data.size() > 0)
	    {
	        // this data has completed a segment
	        DnpAddr_t addr;

	        if (timer_p->isActive(TimerInterface::RESPONSE))
		    // reset the timer so that large multi segment responses
		    // will have a chance to get through
		    timer_p->activate(TimerInterface::RESPONSE);

	        addr = tf_p->rxSegment( segment);
	        if (addr != TransportFunction::FRAGMENT_NOT_FOUND)
	        {
		        // this data had completed a fragment
		        lastRxdAsdu = stn_p->session.rxFragment;
		        stn_p = stationMap[addr];
		        processRxdFragment();
	        }
	    }
    }

    return stn_p->stats.get(Station::STATE);
}

DnpStat_t Master::timeout(TimerInterface::TimerId t)
{

    if (t == TimerInterface::RESPONSE)
    {
	    if (secureAuthenticationEnabled)
	    {
	        DnpStat_t state;
	        state = stn_p->secAuth.stats.get( SecureAuthentication::STATE);

	        if ((state != SecureAuthentication::MASTER_IDLE) &&
		    (state != SecureAuthentication::INIT))
	        {
		        // we must be waiting for something so the response
		        // timeout will apply to the secure auth object
		        stn_p->secAuth.responseTimeout();
	        }
	    }

	    // were we waiting for anything?
	    if (stn_p->stats.get(Station::STATE) != Station::IDLE)
	    {
	        stn_p->stats.increment( Station::RESPONSE_TIMEOUT);
	        stn_p->stats.increment( Station::CONSECUTIVE_TIMEOUT);

	        if (stn_p->stats.get(Station::CONSECUTIVE_TIMEOUT)>=maxConsecutive)
	        {       
		        if (stn_p->stats.get(Station::COMMUNICATION) == 1)
		        {
		            stn_p->stats.set(Station::COMMUNICATION, 0);
		            // /* notify database */
		            /* we should know attempt integrity polls */
		            stn_p->sendIntegrityPoll = 1;
		        
		            // 		    if (sendKeepAlives)
		            // 			if endPoint.connectionIsUp()
		        }
	        }

	        // don't bother waiting any longer for a response
	        completedTransaction();
	    }
    }
    else if (t == TimerInterface::KEY_CHANGE)
    {
    	stn_p->secAuth.keyChangeTimeout();
    }
    else if (t == TimerInterface::CHALLENGE)
    {
	    stn_p->secAuth.challengeTimeout();
    }
    else
    {
	    assert(0);
    }

    return stn_p->stats.get(Station::STATE);
}


bool Master::checkAppHeader()
{
    bool                    returnValue    = false;
    uint8_t                 seqNum;
    bool                    multiFragment  = false;
    bool                    rxingMultiFragmentResponse = false;
    AppHeader::FunctionCode fn         = ah.getFn();
    bool                    fin        = ah.getFinal();
    bool                    fir        = ah.getFirst();

    stn_p->stats.logNormal("Rx %s", ah.str(strbuf, sizeof(strbuf)));

    if (fn == AppHeader::CONFIRM)
    {
	// as a master we never request a confirm
	stn_p->stats.increment(Station::RX_UNEXP_CONFIRM);
    }
    else if (fn == AppHeader::UNSOLICITED_RESPONSE)
    {
        // if the fragment is unsolicited
        // they use a different set of seqeunce numbers
	multiFragment = stn_p->rxingMultiFragmentUnsolicitedResponse;
        seqNum = stn_p->nextRxUnsolicitedSeqNum;
	if (seqNum == AppHeader::UNDEFINED_SEQ_NUM)
	    // this must be our first unsolicted response from this
	    // device - simply accept whatever seq num they give us
	    seqNum = ah.getSeqNum(); // this will pass the check
	
	stn_p->stats.increment(Station::RX_UNSOLICITED);
    }
    else if (fn == AppHeader::RESPONSE)
    {
	DnpStat_t state = stn_p->stats.get(Station::STATE);
	DnpStat_t secState = stn_p->secAuth.stats.get(MasterSecurity::STATE);
	bool sec = false; // is security waiting for a response?
	if (secureAuthenticationEnabled)
	    if (secState !=  MasterSecurity::MASTER_IDLE &&
		secState !=  MasterSecurity::INIT)
		sec = true;

	if ( (state == Station::IDLE) && ( sec == false) )
	{
	    // maybe this is a late response?
	    stn_p->stats.increment(Station::RX_UNEXP_RESPONSE);
	}
	else
	{
            multiFragment = stn_p->rxingMultiFragmentResponse;

            if (fir)
		// if this is the first fragment of a response
		// we will expect it to have the same seq num
                // as the request
                seqNum = stn_p->lastTxSeqNum;
            else
                seqNum = stn_p->nextRxSeqNum;
	    returnValue = true;
	    stn_p->stats.increment(Station::RX_RESPONSE);
	}
    }
    else if (fn == AppHeader::AUTHENTICATION_CHALLENGE)
    {
	multiFragment = false;
	seqNum = stn_p->lastTxSeqNum;
	// need to add in some checks here
	returnValue = true;
    }
    else
	stn_p->stats.increment(Station::RX_UNSUPPORTED_FN);

    if (returnValue == true)
    {
        // we should always check for seq #s
        if (ah.getSeqNum() != seqNum)
	{
	    AppSeqNum::decrement( seqNum);
            if (ah.getSeqNum() == seqNum)
                // do byte by byte compare from the previously
                // received fragment
                if (lastRxdAsdu == stn_p->session.rxFragment)
		    // this must be a retried attempt
		    // do not reprocess this data 
		    stn_p->stats.increment(Station::RX_RESEND);
	            // don't bother sending a retry 
		else
		    stn_p->stats.increment(Station::RX_BAD_AH_SEQ_NUM);
	    else
		stn_p->stats.increment(Station::RX_BAD_AH_SEQ_NUM);
	    returnValue = false;
	}
    }

    if (returnValue == true)
    {
        if (fir)
	{
	    stn_p->nextRxSeqNum = stn_p->lastTxSeqNum;

            if (multiFragment)
                stn_p->stats.increment(Station::RX_MISSED_AH_FIN);
                // not much we can do lets just assume this one is good
                // and assume the previous one was bogus
	    if (fin)
	    {
		rxingMultiFragmentResponse = false;
	    }
	    else
		rxingMultiFragmentResponse = true;

	}
	else
	{
	    // check that session exists
	    if (multiFragment == false)
	    {
		stn_p->stats.increment(Station::RX_MISSED_AH_FIR);
		rxingMultiFragmentResponse = false;
		returnValue = false;
	    }
	    else
	    {
		if (fin)
		{
		    rxingMultiFragmentResponse = false;
		}
		else
		{
		}

	    }
	}
    }
                       
    // now update the variables regardless of whether we feel it was good
    if (fn == AppHeader::UNSOLICITED_RESPONSE)
    {
	stn_p->rxingMultiFragmentUnsolicitedResponse = rxingMultiFragmentResponse;
	AppSeqNum::increment(stn_p->nextRxUnsolicitedSeqNum);
    }
    else
    {
	stn_p->rxingMultiFragmentResponse = rxingMultiFragmentResponse;
	AppSeqNum::increment(stn_p->nextRxSeqNum);
    }

    return returnValue;
}
  

void Master::processRxdFragment()
{
    // set to zero if we have touble parsing objects
    // used to determine if we should send a confirm
    // and to update our communication stats
    bool parseOk = false;
    bool okToProcess = true;

    if (secureAuthenticationEnabled)
    {
	okToProcess = stn_p->secAuth.rxAsdu(stn_p->session.rxFragment);
    }

    if (okToProcess == false)
	return;

    ah.decode( stn_p->session.rxFragment);
    bool fin = ah.getFinal();
    
    if ( checkAppHeader() == 1)
    {
	unsigned int ii;
	unsigned int oldii;
	unsigned int ch;  // changed ii bits
	unsigned int fn;

	// check II bits and update stats if changed
	ii = ah.getIIN();
	oldii = stn_p->stats.get(Station::IIN);
	if  (ii != oldii)
	{
	    stn_p->stats.set(Station::IIN, ii);
	    // what are the bits that changed?
	    ch = oldii^ii;  //XOR to find out
	    // count only the 'on' transitions
	    ch &= ii;

	    if (ch & InternalIndications::NEED_TIME)
		stn_p->stats.increment(Station::RX_IIN_NEED_TIME);

	    if (ch & InternalIndications::DEVICE_TROUBLE)
		stn_p->stats.increment(Station::RX_IIN_TROUBLE);

	    if (ch & InternalIndications::DEVICE_RESTART)
	    {
		stn_p->stats.increment(Station::RX_IIN_RESTART);
		// we don't know what this will be
		stn_p->nextRxUnsolicitedSeqNum = -1;
		// outdate any previous delay measurement
		if (stn_p->stats.get(Station::DELAY_MEASUREMENT) != 0)
		    stn_p->stats.set(Station::DELAY_MEASUREMENT, 0);
	    }

	    if (ch & InternalIndications::FUNCTION_UNKNOWN)
		stn_p->stats.increment(Station::RX_IIN_FUNCTION_UNKOWN);

	    if (ch & InternalIndications::OBJECT_UNKNOWN)
		stn_p->stats.increment(Station::RX_IIN_OBJECT_UNKNOWN);

	    if (ch & InternalIndications::PARAMETER_ERROR)
		stn_p->stats.increment(Station::RX_IIN_PARAMETER_ERR);

	    if (ch & InternalIndications::BUFFER_OVERFLOW)
		stn_p->stats.increment(Station::RX_IIN_BUFFER_OVERFLOW);

	    if (ch & InternalIndications::BAD_CONFIG)
		stn_p->stats.increment(Station::RX_IIN_BAD_CONFIG);
	}

	fn = ah.getFn();

	// note that checkapp header would have returned 0 if it was a confirm

	if (fn == AppHeader::RESPONSE)
	{
	    DnpStat_t state = stn_p->stats.get(Station::STATE);

	    // check control response before we parse (decode) the fragment
	    // so that the fragment is left in tack
	    if (state == Station::SELECT_RESP)
	    {
		if (verifyControlResp())
		{
		    operate();

		    return; // to avoid calling completedTransaction
		}
		else
		    stn_p->stats.increment(Station::RX_BAD_SELECT_RESP);

	    }
	    else if (state == Station::OPERATE_RESP)
	    {
		if (!verifyControlResp())
		    stn_p->stats.increment(Station::RX_BAD_OPERATE_RESP);
	    }

	    // we even parse the control responses because
	    // we would like to see the returned status
	    parseOk = parseResponseObjects(stn_p->session.rxFragment);
	    
	    if ((parseOk) && (state == Station::POLL_RESP))
	    {
		stn_p->sendIntegrityPoll = 0;
	    }
	    else if (state == Station::CLEAR_RESTART_BIT_RESP)
	    {
		// make sure the appropriate bits have been
		// cleared and that the object portion is null
		if ((stn_p->session.rxFragment.size() != 0) ||
		    ((stn_p->stats.get(Station::IIN) &
		      InternalIndications::DEVICE_RESTART) != 0))
		    stn_p->stats.increment(Station::RX_BAD_WRITE_RESP);
	    }
	}
	else if (fn == AppHeader::AUTHENTICATION_CHALLENGE)
	{
	    // should realy only accept a challenge object here but for 
	    // the prototype we won't do any checks
	    parseOk = parseResponseObjects(stn_p->session.rxFragment);
	}
	else
	{
	    // must be unsolicited, if not asdu should have discarded
	    assert (fn == AppHeader::UNSOLICITED_RESPONSE);
	    // parseOk = parseResponseObjects();
	}

	if (parseOk)
	{
	    if (stn_p->stats.get(Station::COMMUNICATION) != 1)
	    {
		stn_p->stats.set(Station::COMMUNICATION, 1);

		if (stn_p->stats.get(Station::CONSECUTIVE_TIMEOUT) != 0)
		    stn_p->stats.set(Station::CONSECUTIVE_TIMEOUT, 0);
	    }

	    if (ah.getConfirm() == 1)  // send a confirm?
		sendConfirm( ah.getSeqNum());
	}

	if ((fin) &&
	    ((getSecAuthState() == SecureAuthentication::MASTER_IDLE) ||
	     (getSecAuthState() == SecureAuthentication::INIT)) &&
	    (fn != 131))
	    completedTransaction();
    }
}

void Master::sendConfirm( AppSeqNum_t txSeqNum)
{

    // updating the txSeqNum here ensures the correct txSeqNum
    // will be calculated in the send for
    // the next request after a multi fragment rx from an ied
    // it is also used by the initRequest
    stn_p->lastTxSeqNum = txSeqNum;

    // a confirm is simply a header with the confirm function code
    // no IIN bytes
    // first & final = 1, confirm & unsol = 0
    initRequest( AppHeader::CONFIRM);
    stn_p->stats.increment(Station::TX_CONFIRM);
    transmit();
}

// this method is called when it is time to do a poll or a command
// it will invoke the normal polling routine
DnpStat_t Master::startNewTransaction()
{
    unsigned int ii;
    //assert (stn_p->stats.get(Station::STATE) == Station::IDLE);
	if(stn_p->stats.get(Station::STATE) != Station::IDLE)
	{
		return false;
	}

    ii = stn_p->stats.get(Station::IIN);
    if ( ii & InternalIndications::DEVICE_RESTART)
    {
		clearRestartBit();
    }
//     else if ( ii & InternalIndications::NEED_TIME)
//     {
//         if (stn_p->stats.get(Station::DELAY_MEASUREMENT) == 0)
// 	{
// 	    // delayMeasurement();
// 	}
// 	else
// 	{
// 	    //writeTimeAndDate();
// 	}
//     }
    else
    {
		poll(AUTO);
    }

    return stn_p->stats.get(Station::STATE);
}

// this method must be called upon receipt of a response
// or a timeout signifiying the close of a transaction
void Master::completedTransaction()
{       
    timer_p->cancel(TimerInterface::RESPONSE);
    // we are no longer waiting for anything
    stn_p->changeState( Station::IDLE);
    stn_p->stats.logNormal("End Transaction --------------------------------");

    // now that we have serviced this outstation, in the round robin
    // fashion determine the next one to begin a transaction with
        
    currentStationPair++;
    if (currentStationPair == stationMap.end())
	currentStationPair = stationMap.begin();
    stn_p = currentStationPair->second;

}

DnpStat_t Master::control(ControlOutputRelayBlock& cb)
{
    //assert (stn_p->stats.get(Station::STATE) == Station::IDLE);
	if(stn_p->stats.get(Station::STATE) != Station::IDLE)
	{
		return false;
	}

    // perform the select

    initRequest( AppHeader::SELECT);

    oh = ObjectHeader(12, 1, 0x28, 1); // we send only one control at a time
    oh.encode( stn_p->txFragment);
    appendUINT16( stn_p->txFragment, cb.index);
    cb.encode( stn_p->txFragment);
    stn_p->lastControlFrag = stn_p->txFragment;
    stn_p->stats.increment(Station::TX_SELECT);
    transmit();
    stn_p->changeState( Station::SELECT_RESP);
    stn_p->cb = cb; // remember the control to be operated
    timer_p->activate(TimerInterface::RESPONSE);
    return stn_p->stats.get(Station::STATE);
}


DnpStat_t Master::poll( PollType pollType)
{
    // don't call this method if we are in the middle of a transaction
    //assert(stn_p->stats.get(Station::STATE) == Station::IDLE);

	if(stn_p->stats.get(Station::STATE) != Station::IDLE)
	{
		return false;
	}

    stn_p->stats.logNormal("Begin Transaction ------------------------------");

    initRequest( AppHeader::READ);

    if(pollType == AUTO)
    {
		// check to see if this should be an event poll or integrity
		if ((stn_p->stats.get(Station::TX_READ_REQUEST) %
			 *integrityPollInterval_p) == 0)
		{
			// note that for other reasons (startup) this may already be 1
			stn_p->sendIntegrityPoll = 1;
		}
	}
	else if (pollType == INTEGRITY)
		stn_p->sendIntegrityPoll = 1;
	else // (pollType == EVENT)
		stn_p->sendIntegrityPoll = 0;
        
	if (stn_p->sendIntegrityPoll == 1)
	{
		appendIntegrityPoll();
		stn_p->stats.increment(Station::TX_INTEGRITY_POLL);
	}
	else
	{
		appendEventPoll();
		stn_p->stats.increment(Station::TX_EVENT_POLL);
	}

    stn_p->stats.increment(Station::TX_READ_REQUEST);

    transmit();

    stn_p->changeState( Station::POLL_RESP);

    timer_p->activate(TimerInterface::RESPONSE);

    return stn_p->stats.get(Station::STATE);
}

void Master::initRequest(  AppHeader::FunctionCode fn)
{ 
    stn_p->txFragment.clear();

    if ((fn != AppHeader::CONFIRM) &&
	( fn != AppHeader::AUTHENTICATION_REQUEST) &&
	( fn != AppHeader::AUTHENTICATION_REPLY) )
		AppSeqNum::increment( stn_p->lastTxSeqNum);

    // master requests will always be a single fragment and
    // we will never ask for a confrim and it won't be
    // an unsolicited response
    // fir, fin=1, con, uns=0
    ah = AppHeader( 1, 1, 0, 0, stn_p->lastTxSeqNum, fn);
    stn_p->stats.logNormal(ah.str(strbuf, sizeof(strbuf)));

    // now use this app header as the first part of our request
    ah.encode(stn_p->txFragment);
}

void Master::clearRestartBit()
{
    initRequest( AppHeader::WRITE);

    oh = ObjectHeader(80, 1, ObjectHeader::ONE_OCTET_START_STOP_INDEXES,
		      0,    //   count is not used
		      7,    //   start index (in bits)
		      7 );  //   stop index (in bits)

    oh.encode( stn_p->txFragment);

    // the value we want to write to the restart bit is 0
    appendUINT8( stn_p->txFragment, 0);
     
    stn_p->stats.increment(Station::TX_WRITE_REQUEST);
    stn_p->changeState( Station::CLEAR_RESTART_BIT_RESP);
    transmit();
}

void Master::appendIntegrityPoll()
{
    //for an integrity poll we want to read:
    //   1. all event data
    //   2. all static data
    appendEventPoll();
    oh = ObjectHeader(60, 1, ObjectHeader::NO_RANGE_FIELD); // all static data
    stn_p->stats.logNormal(oh.str(strbuf, sizeof(strbuf)));
    oh.encode(stn_p->txFragment);
}

void Master::appendEventPoll()
{
    // all class 1 data
    oh = ObjectHeader(60, 2, ObjectHeader::NO_RANGE_FIELD);
    stn_p->stats.logNormal(oh.str(strbuf, sizeof(strbuf)));
    oh.encode(stn_p->txFragment);
    // all class 2 data
    oh = ObjectHeader(60, 3, ObjectHeader::NO_RANGE_FIELD);
    stn_p->stats.logNormal(oh.str(strbuf, sizeof(strbuf)));
    oh.encode(stn_p->txFragment);
    // all class 3 data
    oh = ObjectHeader(60, 4, ObjectHeader::NO_RANGE_FIELD);
    stn_p->stats.logNormal(oh.str(strbuf, sizeof(strbuf)));
    oh.encode(stn_p->txFragment);
}


bool Master::verifyControlResp()
{
    // remove the app header from the tx fragment
    removeUINT16(stn_p->lastControlFrag);
    // the rx fragment header has already been destreamed
    return (stn_p->lastControlFrag == stn_p->session.rxFragment);
}

void Master::operate()
{
    initRequest( AppHeader::OPERATE);

    oh = ObjectHeader(12, 1, 0x28, 1); // we send only one control at a time
    oh.encode( stn_p->txFragment);
    appendUINT16( stn_p->txFragment, stn_p->cb.index);
    stn_p->cb.encode( stn_p->txFragment);

    stn_p->lastControlFrag = stn_p->txFragment;

    stn_p->stats.increment(Station::TX_OPERATE);
    transmit();
    stn_p->changeState( Station::OPERATE_RESP);
}

void Master::transmit()
{
    // hand it off to the transport function
    tf_p->transmit(stn_p->addr, stn_p->txFragment, stn_p->stats);
}

bool Master::parseResponseObjects(Bytes &data)
{
    bool parseOk = true;
    DnpObject* obj_p = NULL;
    // catch exceptions if object data is malformed
    while (data.size() > 0)
    {
	try
	{
	    oh.decode( data, stn_p->stats );
	    stn_p->stats.logNormal(oh.str(strbuf, sizeof(strbuf)));
	    obj_p = of.decode(oh, data, stn_p->addr, stn_p->stats);
	}
	catch (int e)
	{
	    stn_p->stats.increment(Station::RX_UNPARSABLE_DATA);
	    stn_p->stats.logAbnormal(0, "Caught exception line# %d", e);
	    parseOk = false;
	    break;
	}

	// handle special cases
	// note we make the assume that all special case objects will
	// come one at a time, eg. one object per object header
	if (oh.grp == 51)
	{ // CTO
	    of.setCTO( obj_p->timestamp);
	}
	else if (oh.grp == 120)
	{
	    if (oh.var == 5)
	    {
		stn_p->secAuth.rxKeyStatus((SessionKeyStatus*) obj_p);
	    }
	    else if (oh.var == 1)
	    {
		stn_p->secAuth.rxChallenge((Challenge*) obj_p);
	    }
	}
 
    }

    // CTOs are not valid across fragments
    of.setCTO( 0);

    return parseOk;

}

void Master::appendVariableSizedObject( const ObjectHeader& h,
					const DnpObject& o)
{
    stn_p->stats.logNormal( h.str( strbuf, sizeof(strbuf)));
    h.encode(stn_p->txFragment);
    appendUINT16(stn_p->txFragment, o.size()); 
    o.encode(stn_p->txFragment);
}
