//
// $Id: security.hpp 22 2007-04-16 17:56:45Z sparky1194 $
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

#ifndef SECURITY_H
#define SECURITY_H

#include "asdu.hpp"
#include "object.hpp"
#include "stats.hpp"
#include "factory.hpp"


// forward declaractions for classes external to this file
class Application;
class Master;
class Outstation;

// forward declatactions for classes that exist in this file
class SecureAuthentication;
class MasterSecurity;
class OutstationSecurity;

#define DEFAULT_UPDATE_KEY "0123456789ABCDEF"

typedef Bytes Key_t;

// this is not meant to be instantiated
class Key{
  public:
    //static const int MIN_KEY_SIZE = 16;
	static const int MIN_KEY_SIZE;
    // Fill the key with random data of minimum length
    static void initKey(Key_t& key, int len=MIN_KEY_SIZE);
    // Fill the key with binary data of length 'len'
    static void initKey(Key_t& key, const unsigned char* data, int len);
  private:
    Key();
};

class SeqNum24
{
  public:
    SeqNum24(uint32_t initialValue=0);
    void increment();
    uint32_t get() const;

  private:
    uint32_t value;
};

// Base class for all the states
class SecureAuthenicationState
{
  public:
    SecureAuthenicationState( SecureAuthentication* secureAuth_p,
			      DnpStat_t state=0);
    virtual void challengeTimeout();
    virtual void responseTimeout();
    virtual void keyChangeTimeout();
    virtual void sessionKeyTimeout();
    virtual bool rxNonCriticalAsdu();
    virtual bool rxCriticalAsdu();
    virtual bool rxResponse();
    virtual void maxInvalidRespOrLinkFail();
    virtual void errorMsg();
//     virtual void backwardCompatibilityError();
//     virtual void rxValidAggressiveModeRequest();
//     virtual void rxInvalidAggressiveModeRequest();
    virtual void rxKeyStatusOk();
    virtual void rxKeyStatusNotOk();
    virtual void rxChallengeMsg();
    virtual void rxKeyChange();
    virtual void rxKeyStatusReq();

    SecureAuthentication* sa_p;
    DnpStat_t id;

    virtual ~SecureAuthenicationState();
};

class Idle : public SecureAuthenicationState
{
  public:
    // The id in the constructor defaults to MASTER_IDLE.
    // If this object is to be used for
    // an outstation it must manually set the id to OUTSTATION_IDLE.
    // This is to handle the cases where the IDLE state machine differs
    // between master and outstation.
    Idle(SecureAuthentication* secureAuth_p);
    bool rxNonCriticalAsdu();
    bool rxCriticalAsdu();
    void challengeTimeout();
    bool rxResponse();
    void maxInvalidRespOrLinkFail();
    void keyChangeTimeout();   // for master only
    void rxChallengeMsg();
    void rxKeyStatusReq();     // outstation only


};

class WaitForResponse : public SecureAuthenicationState
{
  public:
    // If this object is to be used for
    // an outstation it must manually set the id to OUTSTATION_IDLE.
    // This is to handle the cases where the IDLE state machine differs
    // between master and outstation.
    WaitForResponse(SecureAuthentication* secureAuth_p);
    bool rxNonCriticalAsdu();
    bool rxCriticalAsdu();
    void challengeTimeout();
    bool rxResponse();
    void maxInvalidRespOrLinkFail();
    void responseTimeout();
    void keyChangeTimeout();
};

  // master only
class WaitForKeyStatus : public SecureAuthenicationState
{
  public:
    WaitForKeyStatus(MasterSecurity* secureAuth_p);
    void rxKeyStatusOk();
    void rxKeyStatusNotOk();
    void responseTimeout();
    void keyChangeTimeout();
    void rxChallengeMsg();
};

  // master only
class WaitForKeyConfirmation : public SecureAuthenicationState
{
  public:
    WaitForKeyConfirmation(MasterSecurity* secureAuth_p);
    void rxKeyStatusOk();
    void rxKeyStatusNotOk();
    void responseTimeout();
    void keyChangeTimeout();
    void rxChallengeMsg();
};

  // outstation only
class WaitForKeyChange : public SecureAuthenicationState
{
  public:
    WaitForKeyChange(OutstationSecurity* secureAuth_p);
    void rxKeyStatusReq();
    void rxKeyChange();
    void rxChallengeMsg();
    void sessionKeyTimeout();

};

// abstract class
class SecureAuthentication
{
  public:
    enum BaseStatIndex  {   STATE  = AuthenticationError::NUM_ERROR_REASONS,
			    RX_CRITICAL_ASDU,
			    RESPONSE_TIMEOUT,
			    CHALLENGE_TIMEOUT,
			    TX_CHALLENGE_MSG,
			    RX_CHALLENGE_MSG,
			    TX_AUTH_RESPONSE,
			    RX_VALID_AUTH_RESP,
			    RX_INVALID_AUTH_RESP,

			    //abnormal
			    ERRORS_TXD,
			    KEYS_INVALID,
			    SESSION_ERROR,
  			    BASE_NUM_STATS };

    // to be used for the state statistic
    enum States         {   INIT                           = 0,
		            MASTER_IDLE                    = 1,
		            OUTSTATION_IDLE                = 2,
			    MASTER_WAIT_FOR_RESPONSE       = 3,
			    OUTSTATION_WAIT_FOR_RESPONSE   = 4,
			    WAIT_FOR_KEY_STATUS            = 5, // master
			    WAIT_FOR_KEY_CONFIRMATION      = 6, // master
                            WAIT_FOR_KEY_CHANGE            = 7, // outstation
			    NUM_STATES};

    static const char* stateStrings[ NUM_STATES];

    SecureAuthentication( Application* parent_p, bool aggressiveMode = false,
			  DnpStat_t maxErrorCount=2);

    // only used by the Master class
    virtual void init();
    // will return true if the ASDU should be processed according to the
    // normal protocol
    bool rxAsdu( Bytes& asdu);
    void challengeTimeout();
    void responseTimeout();
    void keyChangeTimeout();
    void sessionKeyTimeout();
    // when the communication fails the security object must be notified
    void linkFailed();
    void rxChallenge(const Challenge* p);

    virtual ~SecureAuthentication();

    Stats stats;
   

  protected:
    // declare a friends so that they can call
    // SecureAuthentication methods, eg changeState
    friend class WaitForKeyConfirmation;
    friend class WaitForKeyStatus;
    friend class Idle;
    friend class WaitForResponse;
    friend class WaitForKeyChange;

    Idle idle;
    WaitForResponse waitForResponse;

    void changeState( SecureAuthenicationState* newState);

    // returns true if the response to a challenge ...
    bool checkResponse();

    // thses methods are different depending on whether on not
    // it is a master or outstation security class
    virtual void txChallengeMsg( Challenge::ChallengeReason reason)=0;
    virtual void txResponseMsg()=0;
    virtual void txError( AuthenticationError::ErrorReason)=0;

    void calculateHmac(Bytes& hmac,
		       const Bytes& challengeAsdu,
		       const Bytes& challengedAsdu );

    void calculateHmac(Bytes& hmac,
		       const Bytes& challengeAsdu );

    SecureAuthenicationState* state_p;

    Application*    app_p;
    bool            aggressive;
    DnpStat_t       maxErrCount;
    bool            nextRxdAsduCritical;
    bool            keyChangeTimedOut;
    SeqNum24        challengeSeqNum;
    SeqNum24        keyChangeSeqNum;
    Bytes           queuedAsdu;  // we will only ever need to queue one
    Reply           lastRxdResponse;
    Challenge       lastRxdChallenge;
    Bytes           lastRxdChallengeAsdu;
    Challenge       lastTxdChallenge;
    Bytes           lastTxdChallengeAsdu;
    Key_t           monitoringDirectionKey;
    Key_t           controlDirectionKey;
    Key_t           updateKey;

    bool critical[ AppHeader::MAX_FUNCTION_CODE_INDEX+1];

};

class MasterSecurity : public SecureAuthentication
{
  public:

    enum StatIndex     { TX_KEY_STATUS_REQUEST = BASE_NUM_STATS,
			 TX_KEY_CHANGE_MSG,
			 RX_KEY_STATUS_OK,
			 RX_KEY_STATUS_NOT_OK,
			 KEY_CHANGE_TIMEOUT,
			 KEY_CHANGE_COUNTER,
		         NUM_STATS };

    MasterSecurity( Master* app_p, bool aggressiveMode = false);
    void init();
    void txKeyStatusReq();
    void txKeyChangeReq();

    void txChallengeMsg( Challenge::ChallengeReason reason);
    void txResponseMsg();
    void txError( AuthenticationError::ErrorReason);

    void rxKeyStatus(const SessionKeyStatus* p);

    void aes128KeyWrap( Bytes&       wrappedKeyData,
			const Bytes& controlKey,
			const Bytes& monitorKey,
			const Bytes& keyStatus);

    WaitForKeyStatus         waitForKeyStatus;
    WaitForKeyConfirmation   waitForKeyConfirmation;

private:

    Stats::Element      statElements[NUM_STATS];

    SessionKeyStatus    lastKeyStatus;
};

class OutstationSecurity : public SecureAuthentication
{
  public:

    enum StatIndex        { TX_KEY_STATUS = BASE_NUM_STATS,
			    RX_KEY_STATUS_REQ,
			    RX_VALID_KEY_CHANGE,
			    RX_INVALID_KEY_CHANGE,
			    SESSION_KEY_TIMEOUT,
			    KEY_STATUS,
			    KEY_CHANGE_SEQ_NUM,
  		            NUM_STATS };

    OutstationSecurity( Outstation* app_p, bool aggressiveMode = false);

    void rxKeyStatusReq(SessionKeyStatusReq* p);
    void txKeyStatus();
    void txChallengeMsg( Challenge::ChallengeReason reason);
    void txResponseMsg();
    void txError( AuthenticationError::ErrorReason);

    void incrementKeyChangeSeqNum();

    void rxKeyChange(const SessionKeyChange* p);

    void aes128KeyUnwrap( const Bytes&       wrappedKeyData,
			  Bytes&             controlKey,
			  Bytes&             monitorKey,
			  Bytes&             keyStatus);

    WaitForKeyChange             waitForKeyChange;

    SessionKeyStatus             sessionKeyStatus;

    SessionKeyStatusReq          lastKeyStatusReq;
    SessionKeyChange             lastSessionKeyChange;

private:

    Stats::Element      statElements[NUM_STATS];


};


#endif
