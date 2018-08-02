//
// $Id: security.cpp 28 2007-04-16 21:32:17Z sparky1194 $
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


#include<algorithm>
#include <assert.h>
#include <stdio.h>
#include "common.hpp"
#include "app.hpp"
#include "master.hpp"
#include "outstation.hpp"
#include "security.hpp"
#ifdef USE_CRIPTO
#include "wrap.h"
#include "sha1.h"
#endif

const int Key::MIN_KEY_SIZE = 16;

const char* SecureAuthentication::stateStrings[ NUM_STATES] =
{
    "Init",
    "Idle",
    "Idle",
    "Wait For Response",
    "Wait For Response",
    "Wait For Key Status",
    "Wait For Key Confirmation",
    "Wait For Key Change"
};

void Key::initKey(Key_t& key, int len)
{
    randGen(key, len);
}

void Key::initKey(Key_t& key, const unsigned char* data, int len)
{
    assert (data != NULL);
    assert (len >= Key::MIN_KEY_SIZE);
    
    for(int i=0; i<len; i++)
	key.push_back(data[i]);
}


SeqNum24::SeqNum24( uint32_t initialValue) : value(initialValue)
{
}

uint32_t SeqNum24::get() const
{
    return value;
}

void SeqNum24::increment()
{
    value++;
    value &= 0xffffff;
}

SecureAuthenicationState::SecureAuthenicationState( SecureAuthentication*
						    secureAuth_p,
						    DnpStat_t state) :
  sa_p( secureAuth_p),
  id(state)
{
    // SecureAuthenicationState is an abstract class and must be initialized
    // with the enum corresponding to the derived state
    assert(id != 0);
}

SecureAuthenicationState::~SecureAuthenicationState()
{
}

void SecureAuthenicationState::challengeTimeout()
{
    //    assert(0);
    sa_p->stats.logNormal( "Challenge Timer expiration action not implemented yet");
}

void SecureAuthenicationState::responseTimeout()
{
    assert(0);
}

void SecureAuthenicationState::keyChangeTimeout()
{
    assert(0);
}

void SecureAuthenicationState::sessionKeyTimeout()
{
    assert(0);
}

bool SecureAuthenicationState::rxNonCriticalAsdu()
{
    // if not defined let the normal protocol parse
    return true;
}

bool SecureAuthenicationState::rxCriticalAsdu()
{
    // if not defined let the normal protocol parse
    return true;
}

bool SecureAuthenicationState::rxResponse()
{
    assert(0);
	return 0;
}

void SecureAuthenicationState::errorMsg()
{
    // error object will have already been logged
    // no need to do anything further unless we support more than one
    // HMAC algorithm
}

void SecureAuthenicationState::maxInvalidRespOrLinkFail()
{
    // do nothing, the spec does not say what happens in the cases
    // other than Idle and WaitForResponse
}

void SecureAuthenicationState::rxKeyStatusOk()
{
    assert(0);
}

void SecureAuthenicationState::rxKeyStatusNotOk()
{
    assert(0);
}

void SecureAuthenicationState::rxChallengeMsg()
{
    assert(0);
}

void SecureAuthenicationState::rxKeyChange()
{
    assert(0);
}

void SecureAuthenicationState::rxKeyStatusReq()
{
    assert(0);
}


  // Idle //////////////////////////////



Idle::Idle(SecureAuthentication* secureAuth_p) :
  SecureAuthenicationState(secureAuth_p, SecureAuthentication::MASTER_IDLE)
{
    // The id defaults to MASTER_IDLE. If this object is to be used for
    // an outstation it must manually set the id to OUTSTATION_IDLE.
    // This is to handle the cases where the IDLE state machine differs
    // between master and outstation.
}

bool Idle::rxNonCriticalAsdu()
{
    return true;
}

bool Idle::rxCriticalAsdu()
{
    sa_p->challengeSeqNum.increment();
    sa_p->txChallengeMsg( Challenge::CRITICAL);
    sa_p->changeState( &sa_p->waitForResponse);
    return false;
}

void Idle::challengeTimeout()
{
    sa_p->challengeSeqNum.increment();
    sa_p->txChallengeMsg( Challenge::PERIODIC);
    sa_p->changeState( &sa_p->waitForResponse);
}

bool Idle::rxResponse()
{
    sa_p->stats.increment( AuthenticationError::UNEXPECTED_RESPONSE);
    return false;
}

void Idle::maxInvalidRespOrLinkFail()
{
    if (id == SecureAuthentication::MASTER_IDLE)
    {
	((MasterSecurity*) sa_p)->txKeyStatusReq();
	sa_p->changeState( &((MasterSecurity*) sa_p)->waitForKeyStatus);
    }
    else
    {
	sa_p->changeState( &((OutstationSecurity*) sa_p)->waitForKeyChange);
	((OutstationSecurity*)sa_p)->stats.set(OutstationSecurity::KEY_STATUS,
					       SessionKeyStatus::LINK_FAIL);
    }
}

void Idle::keyChangeTimeout()
{
    assert (id == SecureAuthentication::MASTER_IDLE);
    sa_p->changeState( &((MasterSecurity*) sa_p)->waitForKeyConfirmation);
}

void Idle::rxChallengeMsg()
{
    sa_p->txResponseMsg();
}

void  Idle::rxKeyStatusReq()
{
    ((OutstationSecurity*) sa_p)->txKeyStatus();
}

  // WaitForResponse //////////////////////////////


WaitForResponse::WaitForResponse(SecureAuthentication* secureAuth_p) :
  SecureAuthenicationState(secureAuth_p,
			   SecureAuthentication::MASTER_WAIT_FOR_RESPONSE)
{
    // The id defaults to MASTER_WAIT_FOR_RESPONSE.
    // If this object is to be used for an 
    // outstation it must manually set the id to OUTSTATION_WAIT_FOR_RESPONSE.
    // This is to handle the cases where the IDLE state machine differs
    // between master and outstation.
}

bool WaitForResponse::rxNonCriticalAsdu()
{
    sa_p->txError( AuthenticationError::UNEXPECTED_RESPONSE);
    sa_p->changeState( &sa_p->idle);
    return false;
}

bool WaitForResponse::rxCriticalAsdu()
{
    sa_p->txError( AuthenticationError::UNEXPECTED_RESPONSE);
    sa_p->changeState( &sa_p->idle);
    return false;
}

void WaitForResponse::challengeTimeout()
{
    sa_p->app_p->timer_p->activate(TimerInterface::CHALLENGE);
    sa_p->changeState( &sa_p->waitForResponse);
}

bool WaitForResponse::rxResponse()
{
    bool returnValue = false;

    // don't cancel the response timer incase the app layer was waiting
    // sa_p->app_p->timer_p->cancel(TimerInterface::RESPONSE);

    if ( sa_p->checkResponse())
    {
	sa_p->stats.increment( SecureAuthentication::RX_VALID_AUTH_RESP);
	sa_p->stats.reset( SecureAuthentication::SESSION_ERROR);
	sa_p->nextRxdAsduCritical = false;
	returnValue= true;
    }
    else
    {
	sa_p->stats.increment( SecureAuthentication::RX_INVALID_AUTH_RESP);
	sa_p->txError( AuthenticationError::AUTHENTICATION_FAILED);
    }

    if ( sa_p->stats.get( SecureAuthentication::SESSION_ERROR) > 
	 sa_p->maxErrCount)
    {
	maxInvalidRespOrLinkFail();
    }
    else
	sa_p->changeState( &sa_p->idle);

    return returnValue;
}

void WaitForResponse::responseTimeout()
{
    sa_p->txError( AuthenticationError::NO_RESPONSE);

    if ( sa_p->stats.get( SecureAuthentication::SESSION_ERROR) >
	 sa_p->maxErrCount)
    {
	maxInvalidRespOrLinkFail();
    }
    else
	sa_p->changeState( &sa_p->idle);
}

void WaitForResponse::maxInvalidRespOrLinkFail()
{
    if (id == SecureAuthentication::MASTER_WAIT_FOR_RESPONSE)
    {
	((MasterSecurity*) sa_p)->txKeyStatusReq();
	sa_p->changeState( &((MasterSecurity*) sa_p)->waitForKeyStatus);
    }
    else
    {
	sa_p->changeState( &((OutstationSecurity*) sa_p)->waitForKeyChange);
	((OutstationSecurity*)sa_p)->stats.set(OutstationSecurity::KEY_STATUS,
					       SessionKeyStatus::LINK_FAIL);
    }
}

void WaitForResponse::keyChangeTimeout()
{
    assert (id == SecureAuthentication::MASTER_IDLE);
    // queue the event for processing next time we enter security idle
    sa_p->keyChangeTimedOut = true;
}




  // WaitForKeyStatus //////////////////////////////


WaitForKeyStatus::WaitForKeyStatus( MasterSecurity* secureAuth) :
  SecureAuthenicationState( secureAuth, MasterSecurity::WAIT_FOR_KEY_STATUS)
{
}

void  WaitForKeyStatus::rxKeyStatusNotOk()
{
    ((MasterSecurity*) sa_p)->txKeyChangeReq();
    sa_p->changeState( &((MasterSecurity*) sa_p)->waitForKeyConfirmation);
}

void  WaitForKeyStatus::rxKeyStatusOk()
{
    ((MasterSecurity*) sa_p)->txKeyChangeReq();
    sa_p->changeState( &((MasterSecurity*) sa_p)->waitForKeyConfirmation);
}

void  WaitForKeyStatus::responseTimeout()
{
    ((MasterSecurity*) sa_p)->txKeyStatusReq();
}

void  WaitForKeyStatus::keyChangeTimeout()
{
    ((MasterSecurity*) sa_p)->txKeyStatusReq();
}

void  WaitForKeyStatus::rxChallengeMsg()
{
    sa_p->txError( AuthenticationError::AUTHENTICATION_FAILED);
}



  // WaitForKeyConfirmation /////////////////////

WaitForKeyConfirmation::WaitForKeyConfirmation( MasterSecurity* secureAuth_p)
  : SecureAuthenicationState( secureAuth_p,
			      MasterSecurity::WAIT_FOR_KEY_CONFIRMATION)
{
}

void  WaitForKeyConfirmation::rxKeyStatusNotOk()
{
    ((MasterSecurity*) sa_p)->txKeyStatusReq();
    sa_p->changeState( &((MasterSecurity*) sa_p)->waitForKeyStatus);
}

void  WaitForKeyConfirmation::rxKeyStatusOk()
{
    sa_p->nextRxdAsduCritical = true;
    sa_p->app_p->timer_p->activate(TimerInterface::CHALLENGE);
    sa_p->app_p->timer_p->activate(TimerInterface::KEY_CHANGE);
    sa_p->stats.reset( MasterSecurity::KEY_CHANGE_COUNTER);
    sa_p->changeState( &sa_p->idle);
}

void  WaitForKeyConfirmation::responseTimeout()
{
    ((MasterSecurity*) sa_p)->txKeyStatusReq();
    sa_p->changeState( &((MasterSecurity*) sa_p)->waitForKeyStatus);
}

void  WaitForKeyConfirmation::keyChangeTimeout()
{
    ((MasterSecurity*) sa_p)->txKeyStatusReq();
    sa_p->changeState( &((MasterSecurity*) sa_p)->waitForKeyStatus);
}

void  WaitForKeyConfirmation::rxChallengeMsg()
{
    sa_p->txError( AuthenticationError::AUTHENTICATION_FAILED);
}




  // WaitForKeyChange /////////////////////

WaitForKeyChange::WaitForKeyChange( OutstationSecurity* secureAuth_p)
  : SecureAuthenicationState( secureAuth_p,
			      OutstationSecurity::WAIT_FOR_KEY_CHANGE)
{
}

void  WaitForKeyChange::rxKeyStatusReq()
{
    ((OutstationSecurity*) sa_p)->incrementKeyChangeSeqNum();
    ((OutstationSecurity*) sa_p)->txKeyStatus();
}

void  WaitForKeyChange::rxKeyChange()
{
    OutstationSecurity* p = (OutstationSecurity*) sa_p;

    Bytes             controlKey;
    Bytes             monitorKey;
    Bytes             keyStatusEncoded;

    p->aes128KeyUnwrap( p->lastSessionKeyChange.wrappedKeyData,
			controlKey,
			monitorKey,
			keyStatusEncoded);

    SessionKeyStatus rxdKeyStatus;
    rxdKeyStatus.decode( keyStatusEncoded, keyStatusEncoded.size());

    if (p->sessionKeyStatus == rxdKeyStatus)
    {
	p->stats.increment( OutstationSecurity::RX_VALID_KEY_CHANGE);
 	p->monitoringDirectionKey = controlKey;
 	p->controlDirectionKey = monitorKey;

	if (p->stats.get( OutstationSecurity::KEY_STATUS) !=
	    SessionKeyStatus::OK)
	    p->stats.set( OutstationSecurity::KEY_STATUS,
			  SessionKeyStatus::OK);
 	p->nextRxdAsduCritical = 1;
	p->changeState( &p->idle);

	sa_p->app_p->timer_p->activate(TimerInterface::CHALLENGE);
	sa_p->app_p->timer_p->activate(TimerInterface::SESSION_KEY);

    }
    else
    {
	p->stats.increment( OutstationSecurity::RX_INVALID_KEY_CHANGE);
	if (p->stats.get(OutstationSecurity::KEY_STATUS) !=
	    SessionKeyStatus::AUTH_FAIL)
	    p->stats.set(OutstationSecurity::KEY_STATUS,
			 SessionKeyStatus::AUTH_FAIL);
    }

    p->incrementKeyChangeSeqNum();
    p->txKeyStatus();
}

void  WaitForKeyChange::rxChallengeMsg()
{
    sa_p->txError( AuthenticationError::AUTHENTICATION_FAILED);
}

void  WaitForKeyChange::sessionKeyTimeout()
{
    ((OutstationSecurity*) sa_p)->stats.set(OutstationSecurity::KEY_STATUS,
					    SessionKeyStatus::NOT_INIT);
}



  // SecureAuthentication ////////////////////////



SecureAuthentication::SecureAuthentication(Application* parent_p, 
					   bool aggressiveMode,
					   DnpStat_t maxErrorCount) :
  idle(this),
  waitForResponse(this),
  state_p(NULL),
  app_p(parent_p),
  aggressive(aggressiveMode), 
  maxErrCount( maxErrorCount),
  nextRxdAsduCritical(1),
  keyChangeTimedOut(false)
{
    int i;

    // hard-coded pre-shared update key for prototype: 0,1,2,3,4,5,...
    Key::initKey( updateKey,
		  (unsigned char*) DEFAULT_UPDATE_KEY,
		  sizeof(DEFAULT_UPDATE_KEY));

    assert( aggressive == false); // not supported yet

    for (i=0; i<=AppHeader::MAX_FUNCTION_CODE_INDEX; i++)
	critical[i] = false;

    critical[ AppHeader::WRITE]                       = true;
    critical[ AppHeader::SELECT]                      = true;
    critical[ AppHeader::OPERATE]                     = true;
    critical[ AppHeader::DIR_OPERATE]                 = true;
    critical[ AppHeader::DIR_OPERATE_NO_RESP]         = true;
    critical[ AppHeader::COLD_RESTART]                = true;
    critical[ AppHeader::WARM_RESTART]                = true;
    critical[ AppHeader::INITIALIZE_APPLICATION]      = true;
    critical[ AppHeader::START_APPLICATION]           = true;
    critical[ AppHeader::STOP_APPLICATION]            = true;
    critical[ AppHeader::ENABLE_UNSOLICITED]          = true;
    critical[ AppHeader::DISABLE_UNSOLICITED]         = true;
    critical[ AppHeader::RECORD_CURRENT_TIME]         = true;
    critical[ AppHeader::AUTHENTICATE_FILE]           = true;
    critical[ AppHeader::ACTIVATE_CONFIGURATION]      = true;
}

void  SecureAuthentication::init()
{
    // the master class needs to override this method and the outstation
    // should never call it.
    assert(0);
}

void SecureAuthentication::changeState( SecureAuthenicationState* newState_p)
{
    if (state_p != NULL)
	stats.logNormal("Sec Auth state change: %s -> %s",
			stateStrings[ state_p->id],
			stateStrings[ newState_p->id]);
    state_p = newState_p;
    if (stats.get( STATE) != state_p->id)
	stats.set( STATE, state_p->id);
    stats.reset( SESSION_ERROR);
    if ((state_p->id == MASTER_IDLE) && (keyChangeTimedOut == true))
    {
	state_p->keyChangeTimeout();
	keyChangeTimedOut = false;
    }
}

bool SecureAuthentication::checkResponse()
{
    bool auth = true;

    if (lastRxdResponse.seqNum != lastTxdChallenge.seqNum)
    {
	auth = false;
	stats.logAbnormal(0, "Challenge=%d Response=%d seq nums don't match",
			  lastTxdChallenge.seqNum, lastRxdResponse.seqNum);
    }
    else if (lastRxdResponse.userNum != lastTxdChallenge.userNum)
    {
	auth = false;
	stats.logAbnormal(0, "Challenge=%d Response=%d user nums don't match",
			  lastTxdChallenge.userNum, lastRxdResponse.userNum );
    }
    else
    {
	Bytes hmac;
	if ( lastTxdChallenge.challengeReason == Challenge::CRITICAL)
	    calculateHmac( hmac, lastTxdChallengeAsdu, queuedAsdu);
	else
	    calculateHmac( hmac, lastTxdChallengeAsdu);
	if (lastRxdResponse.hmacValue != hmac)
	{
	    auth = false;
	    stats.logAbnormal(0,"hmacs don't match:");
	    stats.logAbnormal(0,"Last: %s", hex_repr(lastRxdResponse.hmacValue,
						     app_p->strbuf,
						     sizeof(app_p->strbuf)));
	    stats.logAbnormal(0,"This: %s", hex_repr(hmac,
						     app_p->strbuf,
						     sizeof(app_p->strbuf)));
	}
    }

    return auth;
}

// this mehtod needs to be refactored
bool SecureAuthentication::rxAsdu( Bytes& asdu)
{
    AppHeader::FunctionCode fn = AppHeader::getFn(asdu);

    if ( (fn == AppHeader::AUTHENTICATION_REPLY) ||
    ((fn == AppHeader::RESPONSE) && (state_p->id == MASTER_WAIT_FOR_RESPONSE)))
    {
	bool parseOk = true;
	DnpObject* obj_p = NULL;

	// strip off header
	app_p->ah.decode( asdu);

	try
	{
	    app_p->oh.decode( asdu, stats );
	    stats.logNormal(app_p->oh.str(app_p->strbuf,
					  sizeof(app_p->strbuf)));
	    obj_p = app_p->of.decode(app_p->oh, asdu, app_p->addr, stats);
	}
	catch (int e)
	{
	    stats.logAbnormal(0, "Caught exception line# %d", e);
	    parseOk = false;
	}

	// must be
	if ((app_p->oh.grp == 120) && (app_p->oh.var == 2))
	{
	    if (asdu.size() > 0)
	    {
		// only the one object should be present
		stats.logAbnormal(0, "Format not expected");
		parseOk = false;
	    }
	}
	else
	{
	    stats.logAbnormal(0, "Reply object not found");
	    parseOk = false;
	}

	if (parseOk)
	{
	    lastRxdResponse = *((Reply*)obj_p);
	}
	else
	{
	    lastRxdResponse = Reply();
	}

	if (state_p->rxResponse())
	{
	    // replace the challenge response asdu with the queued on
	    asdu = queuedAsdu;
	    return true;
	}
	else
	{
	    return false;
	}

    }
    else if (fn == AppHeader::AUTHENTICATION_ERROR_NO_ACK)
    {
	// log the object
	state_p->errorMsg();
	return false;
    }
    else if (fn == AppHeader::AUTHENTICATION_REQUEST)
    {
	// normal app layer knows nothing of this function code
	bool parseOk = true;
	DnpObject* obj_p = NULL;

	// strip off header
	app_p->ah.decode( asdu);

	try
	{
	    app_p->oh.decode( asdu, stats );
	    stats.logNormal(app_p->oh.str(app_p->strbuf,
					  sizeof(app_p->strbuf)));
	    obj_p = app_p->of.decode(app_p->oh, asdu, app_p->addr, stats);
	}
	catch (int e)
	{
	    stats.logAbnormal(0, "Caught exception line# %d", e);
	    parseOk = false;
	}

	// must be
	if ((app_p->oh.grp == 120) && (app_p->oh.var == 1))
	{
	    // only the one object should be present
	    if (asdu.size() > 0)
	    {
		stats.logAbnormal(0, "Format not expected");
		parseOk = false;
	    }
	}
	else
	{
	    stats.logAbnormal(0, "Challenge object not found");
	    parseOk = false;
	}

	if (parseOk)
	{
	    rxChallenge((Challenge*) obj_p);
	}
	return false;
    }
    else if ((fn == AppHeader::AUTHENTICATION_CHALLENGE) ||
	     (fn == AppHeader::UNSOLICITED_AUTHENTICATION_CHALLENGE))
    {	
	// the challenge object may be embedded with regular objects
	return true;
    }
    else if (( critical[fn]) || nextRxdAsduCritical)
    {
	stats.increment( RX_CRITICAL_ASDU);
	queuedAsdu = asdu;
	return state_p->rxCriticalAsdu();
    }
    else
	return state_p->rxNonCriticalAsdu();
}

void  SecureAuthentication::challengeTimeout()
{
    stats.increment( CHALLENGE_TIMEOUT);
    state_p->challengeTimeout();
}

void  SecureAuthentication::responseTimeout()
{
    stats.increment( RESPONSE_TIMEOUT);
    state_p->responseTimeout();
}

void  SecureAuthentication::keyChangeTimeout()
{
    stats.increment( MasterSecurity::KEY_CHANGE_TIMEOUT);
    state_p->keyChangeTimeout();
}

void  SecureAuthentication::sessionKeyTimeout()
{
    stats.increment( OutstationSecurity::SESSION_KEY_TIMEOUT);
    state_p->sessionKeyTimeout();
}

void  SecureAuthentication::linkFailed()
{
    state_p->maxInvalidRespOrLinkFail();
}



void SecureAuthentication::rxChallenge(const Challenge* p)
{
    lastRxdChallenge = *p;
    lastRxdChallengeAsdu = app_p->lastRxdAsdu;
    stats.increment(RX_CHALLENGE_MSG);
    state_p->rxChallengeMsg();
}



// SecureAuthentication protected methods

void SecureAuthentication::txError( AuthenticationError::ErrorReason reason)
{
    ObjectHeader* oh_p = &app_p->oh;

    stats.increment( reason);
    stats.increment( SESSION_ERROR);
    if (stats.get( SESSION_ERROR) <= maxErrCount)
    {
	// consists of one single object
	*oh_p = ObjectHeader(120, 7, ObjectHeader::ONE_OCTET_COUNT_OF_OBJECTS,
			     1);
	stats.logNormal( oh_p->str( app_p->strbuf,
				    sizeof(app_p->strbuf)));
	// oh_p->encode(app_p->stn_p->txFragment);

	stats.increment( ERRORS_TXD);
	app_p->transmit();
    }
}


void SecureAuthentication::calculateHmac(Bytes& hmac,
					 const Bytes& challengeAsdu,
					 const Bytes& challengedAsdu )
{
	#ifdef USE_CRIPTO
    unsigned char key[updateKey.size()];
    unsigned char input[challengeAsdu.size() + challengedAsdu.size()];
    unsigned char output[20];

//     printf( "Ch: %s\n", hex_repr( challengeAsdu, app_p->strbuf,
// 				   sizeof(app_p->strbuf)));
//     printf( "Cd: %s\n", hex_repr( challengedAsdu, app_p->strbuf,
// 				sizeof(app_p->strbuf)));


    copy( updateKey.begin(), updateKey.end(), key);
    copy( challengeAsdu.begin(), challengeAsdu.end(), input);
    copy( challengedAsdu.begin(), challengedAsdu.end(),
	  &input[challengeAsdu.size()]);

    sha1_hmac( key, sizeof(key), input, sizeof(input), output);

    // take left most 8 octects assuming we are going over TCP
    hmac = Bytes( output, output+8);
	#endif
}

void SecureAuthentication::calculateHmac(Bytes& hmac,
					 const Bytes& challengeAsdu )
{
	#ifdef USE_CRIPTO
    unsigned char key[updateKey.size()];
    unsigned char input[challengeAsdu.size()];
    unsigned char output[20];

    copy( updateKey.begin(), updateKey.end(), key);
    copy( challengeAsdu.begin(), challengeAsdu.end(), input);

    sha1_hmac( key, sizeof(key), input, sizeof(input), output);

    // take left most 8 octects assuming we are going over TCP
    hmac = Bytes( output, output+8);
	#endif
}


SecureAuthentication::~SecureAuthentication()
{
}

MasterSecurity::MasterSecurity(Master* app_p, bool aggressiveMode) :
  SecureAuthentication(app_p, aggressiveMode),
  waitForKeyStatus(this),
  waitForKeyConfirmation(this)
{
    char name[MAX_USER_NAME_LEN];
    Stats::Element temp[] =
    {
	// Error Reasons
	{ AuthenticationError::NOT_USED,                         
	  "Not Used", Stats::ABNORMAL,  0, 0 },
	{ AuthenticationError::AUTHENTICATION_FAILED,
	  "Auth Failed", Stats::ABNORMAL,  0, 0 },
	{ AuthenticationError::UNEXPECTED_RESPONSE,
	  "Unexpected Response", Stats::ABNORMAL,  0, 0 },
	{ AuthenticationError::NO_RESPONSE, 
	  "No Response", Stats::ABNORMAL,  0, 0 },
	{ AuthenticationError::AGGRESSIVE_MODE_NOT_PERMITTED,
	  "Aggressive No Permit", Stats::ABNORMAL,  0, 0 },
	{ AuthenticationError::HMAC_ALGORITHM_NOT_PERMITTED,
	  "HMAC Alg No Permit", Stats::ABNORMAL,  0, 0 },
	{ AuthenticationError::KEY_WRAP_ALGORITHM_NOT_PERMITTED,
	  "Key Wrap Alg No Permit", Stats::ABNORMAL,  0, 0 },

	// Normal base stats
        { STATE,                "State",                 Stats::NORMAL,
	  INIT, INIT },
        { RX_CRITICAL_ASDU,     "Rx Critical ASDU",     Stats::NORMAL, 0, 0 },
        { RESPONSE_TIMEOUT,     "Response Timeout",     Stats::NORMAL, 0, 0 },
        { CHALLENGE_TIMEOUT,    "Challenge Timeout",    Stats::NORMAL, 0, 0 },
        { TX_CHALLENGE_MSG,     "Tx Challenge Msg",     Stats::NORMAL, 0, 0 },
        { RX_CHALLENGE_MSG,     "Rx Challenge Msg",     Stats::NORMAL, 0, 0 },
        { TX_AUTH_RESPONSE,     "Tx Auth Response",     Stats::NORMAL, 0, 0 },
        { RX_VALID_AUTH_RESP,"Rx Valid Auth Resp",     Stats::NORMAL, 0, 0 },
        { RX_INVALID_AUTH_RESP,"Rx Invalid Auth Resp",  Stats::NORMAL, 0, 0 },

	// Abnormal base stats
        { ERRORS_TXD,           "Tx Errors",            Stats::ABNORMAL,0, 0 },
        { KEYS_INVALID,         "Keys Valid",           Stats::ABNORMAL,0, 0 },
        { SESSION_ERROR,        "Session Error",        Stats::ABNORMAL,0, 0 },

	// Master stats
        { TX_KEY_STATUS_REQUEST,"Tx Key Status Requests",Stats::NORMAL, 0, 0 },
	{ TX_KEY_CHANGE_MSG,    "Tx Key Change Msgs",   Stats::NORMAL,  0, 0 },
	{ RX_KEY_STATUS_OK,     "Rx Key Status Ok",   Stats::NORMAL,  0, 0 },
	{ RX_KEY_STATUS_NOT_OK, "Rx Key Status Not Ok",Stats::ABNORMAL, 0, 0 },
	{ KEY_CHANGE_TIMEOUT,   "Tx Key Change Timeout",Stats::NORMAL,  0, 0 },
        { KEY_CHANGE_COUNTER,   "Key Change Counter",   Stats::NORMAL, 0, 0 },

    };
    assert (sizeof(temp)/sizeof(Stats::Element) == NUM_STATS);
    memcpy(statElements, temp, sizeof(temp));
    // MA - Master Authentication
    sprintf(name, "MA %6d ", app_p->addr);
    stats = Stats( name, app_p->addr, app_p->debug_p, statElements, NUM_STATS,
		   app_p->db_p, EventInterface::SA_AB_ST);
}

void MasterSecurity::init()
{
    txKeyStatusReq();
    changeState( &waitForKeyStatus);
}

void  MasterSecurity::txKeyStatusReq()
{
    ObjectHeader* oh_p = &app_p->oh;
    ((Master*)app_p)->initRequest( AppHeader::READ);
 
    *oh_p = ObjectHeader(120, 4, 0x07, 1);
    stats.logNormal( oh_p->str( app_p->strbuf,
			        sizeof(app_p->strbuf)));
    oh_p->encode(((Master*)app_p)->stn_p->txFragment);
    ((Master*)app_p)->stn_p->txFragment.push_back( app_p->userNum);

    stats.increment( TX_KEY_STATUS_REQUEST);
    app_p->transmit();

    // start timer
    app_p->timer_p->activate(TimerInterface::RESPONSE);
}

void MasterSecurity::txKeyChangeReq()
{
    Bytes& frag = ((Master*)app_p)->stn_p->txFragment;

    ObjectHeader oh(120, 6, 0x5b, 1);
    ((Master*)app_p)->initRequest( AppHeader::WRITE);
    stats.logNormal( oh.str( app_p->strbuf, sizeof(app_p->strbuf)));
    oh.encode(frag);

    Bytes lastKeyStatusEncoded;
    Bytes wrappedKeyData;

    Key::initKey( monitoringDirectionKey);
    Key::initKey( controlDirectionKey);
    lastKeyStatus.encode(lastKeyStatusEncoded);

    aes128KeyWrap( wrappedKeyData, controlDirectionKey, 
		   monitoringDirectionKey, lastKeyStatusEncoded);

    SessionKeyChange obj( lastKeyStatus.keyChangeSeqNum, 
			  lastKeyStatus.userNum,
			  wrappedKeyData);

    Bytes objEncode;
    obj.encode(objEncode);
    appendUINT16(frag, objEncode.size());
    moveBytes(objEncode, frag, objEncode.size());

    stats.increment( TX_KEY_CHANGE_MSG);
    app_p->transmit();

    // start timer
    app_p->timer_p->activate(TimerInterface::RESPONSE);
}

void MasterSecurity::txChallengeMsg( Challenge::ChallengeReason reason)
{
    Master* m_p = (Master*) app_p;
    Bytes& frag = ((Master*)app_p)->stn_p->txFragment;
    Challenge     obj( challengeSeqNum.get(), app_p->userNum, Challenge::SHA_1,
		       reason);

    m_p->initRequest( AppHeader::AUTHENTICATION_REQUEST);

    // consists of one single object
    ObjectHeader oh(120, 1, 0x5b, 1);
    stats.logNormal( oh.str( app_p->strbuf, sizeof(app_p->strbuf)));
    oh.encode( frag);

    Bytes objEncode;
    obj.encode(objEncode);
    appendUINT16( frag, objEncode.size());
    moveBytes(objEncode, frag, objEncode.size());

    lastTxdChallenge = obj;
    lastTxdChallengeAsdu = frag;

    stats.increment( TX_CHALLENGE_MSG);
    app_p->transmit();

    if (reason == Challenge::PERIODIC)
	app_p->timer_p->activate(TimerInterface::CHALLENGE);

    app_p->timer_p->activate(TimerInterface::RESPONSE);
}

void MasterSecurity::txResponseMsg()
{
    Master* m_p = (Master*) app_p;
    Bytes hmac;

    if ( lastRxdChallenge.challengeReason == Challenge::CRITICAL)
	calculateHmac( hmac, lastRxdChallengeAsdu, m_p->stn_p->txFragment);
    else
	calculateHmac( hmac, lastRxdChallengeAsdu);	

    ObjectHeader oh(  120, 2, 0x5b, 1);
    Reply        obj( lastRxdChallenge.seqNum,
		      lastRxdChallenge.userNum,
		      hmac);

    m_p->initRequest( AppHeader::AUTHENTICATION_REPLY);

    m_p->appendVariableSizedObject( oh, obj);
    stats.increment( SecureAuthentication::TX_AUTH_RESPONSE);
    app_p->transmit();
}

void MasterSecurity::txError( AuthenticationError::ErrorReason)
{
}

void MasterSecurity::rxKeyStatus(const SessionKeyStatus* p)
{
    // save a copy of this object
    lastKeyStatus = *p;

    if (p->keyStatus == SessionKeyStatus::OK)
    {
        stats.increment( MasterSecurity::RX_KEY_STATUS_OK);
	state_p->rxKeyStatusOk();
    }
    else
    {
	stats.increment( MasterSecurity::RX_KEY_STATUS_NOT_OK);
	state_p->rxKeyStatusNotOk();
    }
}


void MasterSecurity::aes128KeyWrap( Bytes&       wrappedKeyData,
				    const Bytes& controlKey,
				    const Bytes& monitorKey,
				    const Bytes& keyStatus)
{
	#ifdef USE_CRIPTO
    unsigned char input[AES_MAX_PLAIN_LEN];
    unsigned char output[AES_MAX_WRAPPED_LEN];
    unsigned char key[AES_128_SIZE];

    int len = controlKey.size() + monitorKey.size() + keyStatus.size();
    assert (len <= AES_MAX_PLAIN_LEN);

    copy( updateKey.begin(), updateKey.end(), key);

    copy( controlKey.begin(), controlKey.end(), input);
    copy( monitorKey.begin(), monitorKey.end(), &input[controlKey.size()]);
    copy( keyStatus.begin(),  keyStatus.end(),
	  &input[controlKey.size()+monitorKey.size()]);

    // convert len from bytes to number of 64 bit blocks
    if (len % 8 == 0)
    {
	len /= 8;
    }
    else
    {
	len /= 8;
	len++;
    }

    aes_wrap( key, len, input, output);

    // put the char data into a Bytes container
    wrappedKeyData = Bytes( output, output+((len+1)*8));
	#endif
}

OutstationSecurity::OutstationSecurity(Outstation* app_p, bool aggressiveMode):
  SecureAuthentication(app_p, aggressiveMode),
  waitForKeyChange(this)
{
    // workaround to handle the differences in state behaviour between
    // master and outstation
    idle.id = SecureAuthentication::OUTSTATION_IDLE;
    waitForResponse.id = SecureAuthentication::OUTSTATION_WAIT_FOR_RESPONSE;

    char name[MAX_USER_NAME_LEN];
    Stats::Element temp[] =
    {
	// Error Reasons
	{ AuthenticationError::NOT_USED,                         
	  "Not Used", Stats::ABNORMAL,  0, 0 },
	{ AuthenticationError::AUTHENTICATION_FAILED,
	  "Auth Failed", Stats::ABNORMAL,  0, 0 },
	{ AuthenticationError::UNEXPECTED_RESPONSE,
	  "Unexpected Response", Stats::ABNORMAL,  0, 0 },
	{ AuthenticationError::NO_RESPONSE, 
	  "No Response", Stats::ABNORMAL,  0, 0 },
	{ AuthenticationError::AGGRESSIVE_MODE_NOT_PERMITTED,
	  "Aggressive No Permit", Stats::ABNORMAL,  0, 0 },
	{ AuthenticationError::HMAC_ALGORITHM_NOT_PERMITTED,
	  "HMAC Alg No Permit", Stats::ABNORMAL,  0, 0 },
	{ AuthenticationError::KEY_WRAP_ALGORITHM_NOT_PERMITTED,
	  "Key Wrap Alg No Permit", Stats::ABNORMAL,  0, 0 },

	// Normal base stats
        { STATE,                "State",                 Stats::NORMAL,
	  WAIT_FOR_KEY_CHANGE, WAIT_FOR_KEY_CHANGE },
        { RX_CRITICAL_ASDU,     "Rx Critical ASDU",     Stats::NORMAL, 0, 0 },
        { RESPONSE_TIMEOUT,     "Response Timeout",     Stats::NORMAL, 0, 0 },
        { CHALLENGE_TIMEOUT,    "Challenge Timeout",    Stats::NORMAL, 0, 0 },
        { TX_CHALLENGE_MSG,     "Tx Challenge Msg",     Stats::NORMAL, 0, 0 },
        { RX_CHALLENGE_MSG,     "Rx Challenge Msg",     Stats::NORMAL, 0, 0 },
        { TX_AUTH_RESPONSE,     "Tx Auth Response",     Stats::NORMAL, 0, 0 },
        { RX_VALID_AUTH_RESP,"Rx Valid Auth Resp",     Stats::NORMAL, 0, 0 },
        { RX_INVALID_AUTH_RESP,"Rx Invalid Auth Resp",  Stats::NORMAL, 0, 0 },

	// Abnormal base stats
        { ERRORS_TXD,           "Tx Errors",            Stats::ABNORMAL,0, 0 },
        { KEYS_INVALID,         "Keys Valid",           Stats::ABNORMAL,0, 0 },
        { SESSION_ERROR,        "Session Error",        Stats::ABNORMAL,0, 0 },

	// outstation stats
        { TX_KEY_STATUS,        "Tx Key Status",        Stats::NORMAL, 0, 0 },
        { RX_KEY_STATUS_REQ,    "Rx Key Status Req",    Stats::NORMAL, 0, 0 },
        { RX_VALID_KEY_CHANGE,  "Rx Valid Key Change",  Stats::NORMAL, 0, 0 },
        { RX_INVALID_KEY_CHANGE,"Rx Invalid Key Change",Stats::NORMAL, 0, 0 },
        { SESSION_KEY_TIMEOUT,  "Session Key Timeouts", Stats::NORMAL, 0, 0 },
        { KEY_STATUS,    "Key Status", Stats::ABNORMAL,
	  SessionKeyStatus::NOT_INIT, SessionKeyStatus::NOT_INIT },
        { KEY_CHANGE_SEQ_NUM,   "Key Change Seq Num",   Stats::NORMAL, 0, 0 },

    };
    assert (sizeof(temp)/sizeof(Stats::Element) == NUM_STATS);
    memcpy(statElements, temp, sizeof(temp));
    // OA - Outstation Authentication
    sprintf(name, "OA %6d ", app_p->addr);
    stats = Stats( name, app_p->addr, app_p->debug_p, statElements, NUM_STATS,
		   app_p->db_p, EventInterface::SA_AB_ST);

    // changing state will use the stats so it must be called after stat init
    changeState( &waitForKeyChange);

}

void OutstationSecurity::rxKeyStatusReq( SessionKeyStatusReq* p)
{
    stats.increment( RX_KEY_STATUS_REQ);
    lastKeyStatusReq = *p;
    state_p->rxKeyStatusReq();
}


void OutstationSecurity::txKeyStatus()
{
    Bytes pseudoRandomChallengeData;
    Bytes objBytes;
    randGen(pseudoRandomChallengeData, 8);
    ObjectHeader oh(120, 5, 0x5b, 1);

    sessionKeyStatus = SessionKeyStatus( stats.get(KEY_CHANGE_SEQ_NUM),
					 app_p->userNum,
					 SessionKeyStatus::AES_128,
	   (SessionKeyStatus::KeyStatus) stats.get(KEY_STATUS),
					 pseudoRandomChallengeData);


    stats.logNormal( oh.str( app_p->strbuf, sizeof(app_p->strbuf)));

    ((Outstation*)app_p)->initResponse( 1,1,0,0);
    oh.encode(((Outstation*)app_p)->txFragment);
    // because this object is of variable size we must add the size first
    sessionKeyStatus.encode(objBytes);
    appendUINT16(((Outstation*)app_p)->txFragment, objBytes.size());
    moveBytes( objBytes, ((Outstation*)app_p)->txFragment, objBytes.size());
    app_p->transmit();
    stats.increment( TX_KEY_STATUS);
}

void OutstationSecurity::txChallengeMsg( Challenge::ChallengeReason reason)
{
    Outstation* o_p = (Outstation*) app_p;
    ObjectHeader oh = ObjectHeader(120, 1, 0x5b, 1);
    Challenge    obj( challengeSeqNum.get(), app_p->userNum, Challenge::SHA_1,
		      reason);


    o_p->initResponse( 1,1,0,0,0, AppHeader::AUTHENTICATION_CHALLENGE);

    o_p->appendVariableSizedObject( oh, obj);

    stats.increment( TX_CHALLENGE_MSG);
    lastTxdChallenge = obj;
    lastTxdChallengeAsdu = o_p->txFragment;
    app_p->transmit();

    app_p->timer_p->activate(TimerInterface::RESPONSE);

}

void OutstationSecurity::txResponseMsg()
{
    Outstation* o_p = (Outstation*) app_p;
    Bytes hmac;

    if ( lastRxdChallenge.challengeReason == Challenge::CRITICAL)
	calculateHmac( hmac, lastRxdChallengeAsdu, o_p->txFragment);
    else
	calculateHmac( hmac, lastRxdChallengeAsdu);

    ObjectHeader oh(  120, 2, 0x5b, 1);
    Reply        obj( lastRxdChallenge.seqNum,
		      lastRxdChallenge.userNum,
		      hmac);

    o_p->initResponse( 1,1,0,0,0, AppHeader::RESPONSE);

    o_p->appendVariableSizedObject( oh, obj);
    stats.increment( SecureAuthentication::TX_AUTH_RESPONSE);
    app_p->transmit();
}

void OutstationSecurity::txError( AuthenticationError::ErrorReason)
{
}


void OutstationSecurity::rxKeyChange(const SessionKeyChange* p)
{
    // save a copy of this object
    lastSessionKeyChange = *p;
    state_p->rxKeyChange();
}

void OutstationSecurity::incrementKeyChangeSeqNum()
{
    stats.increment( KEY_CHANGE_SEQ_NUM);
    if (stats.get(KEY_CHANGE_SEQ_NUM) > 0xffffff)
	stats.reset(KEY_CHANGE_SEQ_NUM);
}

void OutstationSecurity::aes128KeyUnwrap( const Bytes&       wrappedKeyData,
					  Bytes&             controlKey,
					  Bytes&             monitorKey,
					  Bytes&             keyStatusEncoded)
{
	#ifdef USE_CRIPTO
    unsigned char input[AES_MAX_WRAPPED_LEN];
    unsigned char output[AES_MAX_PLAIN_LEN];
    unsigned char key[AES_128_SIZE];

    int len = wrappedKeyData.size();
    assert (len <= AES_MAX_WRAPPED_LEN);

    copy( updateKey.begin(), updateKey.end(), key);

    copy( wrappedKeyData.begin(), wrappedKeyData.end(), input);

    if (len % 8 != 0)
    {
	assert(0); // wrapped data is always in 64 bit blocks
    }

    // convert len from bytes to number of 64 bit blocks
    len /= 8;
    len--; // we pass in the length of the unwrapped data
    
    aes_unwrap( key, len, input, output);

    // put the output into the correct Bytes container
    // we need to know the key sizes and the key status size to do this
    // for the prototype we know the master is using the min key size
    // for the prototype we'll assume the key status size is the same
    // as we sent. note that these are bad assumptions but we have no other
    // way to know the sizes
    controlKey = Bytes( output, output + Key::MIN_KEY_SIZE);
    unsigned char* output_p = &output[Key::MIN_KEY_SIZE];
    monitorKey = Bytes( output_p, output_p + Key::MIN_KEY_SIZE);
    output_p += Key::MIN_KEY_SIZE;
    keyStatusEncoded = Bytes( output_p, output_p + sessionKeyStatus.size());
	#endif
}
