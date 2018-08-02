//
// $Id: object.hpp 8 2007-04-12 17:25:30Z sparky1194 $
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

#ifndef DNP_OBJECT_H
#define DNP_OBJECT_H

#include "common.hpp"
#include "event_interface.hpp"
#include "config.h"

#define ONLINE                0x01

class DnpObject
{
  public:
    #define NO_INDEX           0xffffffff

    // flag octect bits common to many DNP objects
    //#define ONLINE                0x01;

    DnpObject(float val=0,
	      uint8_t flags=0,
	      DnpIndex_t indx=0,
	      EventInterface::PointType_t pt = EventInterface::ST,
	      DnpTime_t time=0);

    // the following methods are not pure virtuals because it
    // is not required that every single object use them
    virtual void encode(Bytes& data) const;
    virtual void decode(Bytes& data) throw(int);
    virtual void decode(Bytes& data, uint32_t objectSize) throw(int);

    // returns the number of bytes it will encode too
    // not supported on all objects
    virtual unsigned int size() const;

    virtual ~DnpObject();

    // each object can have an index, value, flag and timestamp
    // but does not need to have any.
    // all objects to be updated by the database 
    // must have a value and flag.

    //int32_t                     value; //apa--- on 19-06-2012
	float                     value; //apa+++ on 19-06-2012

    uint8_t                     flag;
    DnpIndex_t                  index; // not used by anyone?
    EventInterface::PointType_t pointType;
    DnpTime_t                   timestamp;
};

// Binary Inputs //////////////////////////////////////////////

class BinaryInputWithStatus : public DnpObject
{
  public:
    BinaryInputWithStatus(uint8_t flag=0x01, DnpIndex_t index=0);
    void encode(Bytes& data) const;
    void decode(Bytes& data) throw(int);
};

typedef BinaryInputWithStatus BinaryInputEventNoTime;


class BinaryInputEvent : public DnpObject
{
  public:
    BinaryInputEvent(uint8_t flag=0x01, DnpIndex_t index=0,DnpTime_t time=0);
    void encode(Bytes& data) const;
    void decode(Bytes& data) throw(int);
};

class BinaryInputEventRelativeTime : public DnpObject
{
  public:
    BinaryInputEventRelativeTime(uint8_t flag=0x01, DnpIndex_t index=0,
				 DnpTime_t time=0);
    void encode(Bytes& data) const;
    void decode(Bytes& data) throw(int);
};

// Binary Outputs //////////////////////////////////////////////

// equivalent except for the type
class BinaryOutputStatus : public BinaryInputWithStatus
{
  public:
    BinaryOutputStatus(uint8_t flag=0x01, DnpIndex_t index=0);
};

// queue, clear, tripClose flags are not used and are always 0
// count is always 1
class ControlOutputRelayBlock : public DnpObject
{
  public:
    enum Code      { NUL       = 0,
		     PULSE_ON  = 1,
		     PULSE_OFF = 2,
		     LATCH_ON  = 3,
		     LATCH_OFF = 4,
		     TRIP      = 0x40,
		     CLOSE     = 0x80 };


    enum Status    { ACCEPTED       = 0,
		     ARM_TIMEOUT    = 1,
		     NO_SELECT      = 2,
		     FORMAT_ERROR   = 3,
		     NOT_SUPPORTED  = 4,
		     ALREADY_ACTIVE = 5,
		     HARDWARE_ERROR = 6,
		     LOCAL          = 7,
		     TOO_MANY_OPS   = 8,
		     NOT_AUTHORIZED = 9,
		     UNDEFINED      = 127 };

    ControlOutputRelayBlock(Code code=TRIP, DnpIndex_t index=0,
			    uint32_t on=1000, uint32_t off=1000,
			    Status st=ACCEPTED);
    void encode(Bytes& data) const;
    void decode(Bytes& data) throw(int);
    Code      outputCode;
    uint8_t   count;
    uint32_t  onTime;
    uint32_t  offTime;
    Status    status;
};

// Analog Inputs ///////////////////////////////////////////////

class Bit32AnalogInput : public DnpObject
{
  public:
    Bit32AnalogInput(int32_t v=0, uint8_t flag=0x01, DnpIndex_t index=0);
    void encode(Bytes& data) const;
    void decode(Bytes& data) throw(int);
};

class Bit16AnalogInput : public DnpObject
{
  public:
    Bit16AnalogInput(int16_t v=0, uint8_t flag=0x01, DnpIndex_t index=0);
    void encode(Bytes& data) const;
    void decode(Bytes& data) throw(int);
};

class Bit32AnalogInputNoFlag : public DnpObject
{
  public:
    Bit32AnalogInputNoFlag(float v = 0.0, DnpIndex_t index=0);
    void encode(Bytes& data) const;
    void decode(Bytes& data) throw(int);
};

class Bit16AnalogInputNoFlag : public DnpObject
{
  public:
    Bit16AnalogInputNoFlag(int16_t v=0, DnpIndex_t index=0);
    void encode(Bytes& data) const;
    void decode(Bytes& data) throw(int);
};

typedef Bit32AnalogInput Bit32AnalogEventNoTime;
typedef Bit16AnalogInput Bit16AnalogEventNoTime;


// Counter Inputs //////////////////////////////////////////////


// equivalent except for the type
// and that the value is a uint32 rather than an int32
class Bit32BinaryCounter : public Bit32AnalogInput
{
  public:
    Bit32BinaryCounter(uint32_t v=0,uint8_t flag=0x01, DnpIndex_t index=0);
};

// equivalent except for the type
// and that the value is a uint16 rather than an int16
class Bit16BinaryCounter : public Bit16AnalogInput
{
  public:
    Bit16BinaryCounter(uint16_t v=0,uint8_t flag=0x01, DnpIndex_t index=0);
};

typedef Bit32BinaryCounter Bit32DeltaCounter;
typedef Bit16BinaryCounter Bit16DeltaCounter;

// equivalent except for the type
// and that the value is a uint32 rather than an int32
class Bit32BinaryCounterNoFlag : public Bit32AnalogInputNoFlag
{
  public:
    Bit32BinaryCounterNoFlag(uint32_t v=0, DnpIndex_t index=0);
};

// equivalent except for the type
// and that the value is a uint16 rather than an int16
class Bit16BinaryCounterNoFlag : public Bit16AnalogInputNoFlag
{
  public:
    Bit16BinaryCounterNoFlag(uint16_t v=0, DnpIndex_t index=0);
};

typedef Bit32BinaryCounterNoFlag Bit32DeltaCounterNoFlag;
typedef Bit16BinaryCounterNoFlag Bit16DeltaCounterNoFlag;

typedef Bit32BinaryCounter Bit32CounterEventNoTime;
typedef Bit16BinaryCounter Bit16CounterEventNoTime;

typedef Bit32BinaryCounter Bit32DeltaCounterEventNoTime;
typedef Bit16BinaryCounter Bit16DeltaCounterEventNoTime;


// Analog Outputs /////////////////////////////////////////////

typedef Bit16AnalogInput Bit16AnalogOutputStatus;

class Bit16AnalogOutput : public DnpObject
{
  public:
    enum Status    { ACCEPTED       = 0,
		     ARM_TIMEOUT    = 1,
		     NO_SELECT      = 2,
		     FORMAT_ERROR   = 3,
		     NOT_SUPPORTED  = 4,
		     ALREADY_ACTIVE = 5,
		     HARDWARE_ERROR = 6,
		     LOCAL          = 7,
		     TOO_MANY_OPS   = 8,
		     NOT_AUTHORIZED = 9,
		     UNDEFINED      = 127 };

    Bit16AnalogOutput(uint16_t requestedValue=0, DnpIndex_t index=0,
		      Status st=ACCEPTED);
    void encode(Bytes& data) const;
    void decode(Bytes& data) throw(int);
    uint16_t  request;
    Status    status;
};



// Other Objects //////////////////////////////////////////////



class TimeAndDate : public DnpObject
{
  public:
    TimeAndDate(DnpTime_t time=0);
    void encode(Bytes& data) const;
    void decode(Bytes& data) throw(int);
};

typedef TimeAndDate TimeAndDateCTO;
typedef TimeAndDateCTO UnsyncronizedTimeAndDateCTO;

class TimeDelayCoarse : public DnpObject
{
  public:
    TimeDelayCoarse(uint16_t delay=0);
    void encode(Bytes& data) const;
    void decode(Bytes& data) throw(int);
};

typedef TimeDelayCoarse TimeDelayFine;



// security objects /////////////////////////////////////////



class Challenge : public DnpObject
{
  public:

    enum HMACAlgorithm      { SHA_256  = 1,
                              SHA_1    = 2 };

    enum ChallengeReason    { INIT     = 1,
			      CRITICAL = 2,
			      PERIODIC = 3 };

    #define MIN_CHALLENGE_SIZE  4

    Challenge() {};
    Challenge(uint32_t challengeSeqNum, UserNumber_t num, 
	      HMACAlgorithm algorithm, ChallengeReason reason);

    void decode(Bytes& data, uint32_t objectSize) throw(int);
    void encode(Bytes& data) const;
 
    unsigned int size() const;
 
    uint32_t                                seqNum;
    UserNumber_t                            userNum;
    HMACAlgorithm                           hmacAlgorithm;
    ChallengeReason                         challengeReason;
    Bytes                                   challengeData;
    
};

class Reply : public DnpObject
{
  public:
    Reply() {};
    Reply(uint32_t challengeSeqNum, UserNumber_t num, Bytes& hmac);
    
    void decode(Bytes& data, uint32_t objectSize) throw(int);
    void encode(Bytes& data) const;
    unsigned int size() const;

    uint32_t                                seqNum;
    UserNumber_t                            userNum;
    Bytes                                   hmacValue;
    
};

typedef Reply AggressiveModeRequest;

class SessionKeyStatusReq : public DnpObject
{
  public:
    SessionKeyStatusReq(UserNumber_t num=0);

    void decode(Bytes& data) throw(int);
    void encode(Bytes& data) const;

    UserNumber_t                            userNum;

};

class SessionKeyStatus : public DnpObject
{
  public:
    enum KeyStatus        { OK        = 1,
		   	    NOT_INIT  = 2,
			    LINK_FAIL = 3,
			    AUTH_FAIL = 4 };

    enum KeyWrapAlgorithm { AES_128   = 1,
		       	    AES_256   = 2 };

    SessionKeyStatus() {};
    SessionKeyStatus( uint32_t seqNum, UserNumber_t num, 
		      KeyWrapAlgorithm algorithm,
		      KeyStatus status,
		      Bytes& data);

    void decode(Bytes& data, uint32_t objectSize) throw(int);
    void encode(Bytes& data) const;

    bool operator==(const SessionKeyStatus &other) const;
    bool operator!=(const SessionKeyStatus &other) const;

    // returns the number of bytes it will encode too
    unsigned int size() const;
 
    uint32_t                                keyChangeSeqNum;
    UserNumber_t                            userNum;
    KeyWrapAlgorithm                        keyWrapAlgorithm;
    KeyStatus                               keyStatus;
    Bytes                                   challengeData;
};

class SessionKeyChange : public DnpObject
{
  public:
    SessionKeyChange() {};
    SessionKeyChange(uint32_t seqNum, UserNumber_t num, Bytes& keyData);

    void decode(Bytes& data, uint32_t objectSize) throw(int);
    void encode(Bytes& data) const;
    unsigned int size() const;
    
    uint32_t                                keyChangeSeqNum;
    UserNumber_t                            userNum;
    Bytes                                   wrappedKeyData;
};

class AuthenticationError : public DnpObject
{
  public:   

    // all these errors are considered abnormal for stats purposes
    enum ErrorReason    {   NOT_USED              = 0,
			    AUTHENTICATION_FAILED = 1,
			    UNEXPECTED_RESPONSE   = 2,
			    NO_RESPONSE           = 3,
			    AGGRESSIVE_MODE_NOT_PERMITTED = 4,
			    HMAC_ALGORITHM_NOT_PERMITTED = 5,
			    KEY_WRAP_ALGORITHM_NOT_PERMITTED = 6,
			    NUM_ERROR_REASONS = 7};

    AuthenticationError() {};
    AuthenticationError(uint32_t sequenceNum, UserNumber_t num,
			ErrorReason errReason, Bytes& text);

    void decode(Bytes& data, uint32_t objectSize) throw(int);
    void encode(Bytes& data) const;
    unsigned int size() const;

    uint32_t                                seqNum;
    UserNumber_t                            userNum;
    ErrorReason                             errorReason;
    Bytes                                   errorText;

};


class InternalIndications : DnpObject
{

public:
	/*
    static const uint16_t ALL_STATIONS               = 0x0001;
    static const uint16_t CLASS1                     = 0x0002;
    static const uint16_t CLASS2                     = 0x0004;
    static const uint16_t CLASS3                     = 0x0008;
    static const uint16_t NEED_TIME                  = 0x0010;
    static const uint16_t LOCAL                      = 0x0020;
    static const uint16_t DEVICE_TROUBLE             = 0x0040;
    static const uint16_t DEVICE_RESTART             = 0x0080;
    static const uint16_t FUNCTION_UNKNOWN           = 0x0100;
    static const uint16_t OBJECT_UNKNOWN             = 0x0200;
    static const uint16_t PARAMETER_ERROR            = 0x0400;
    static const uint16_t BUFFER_OVERFLOW            = 0x0800;
    static const uint16_t BAD_CONFIG                 = 0x2000;
	*/
    static const uint16_t ALL_STATIONS;
    static const uint16_t CLASS1;
    static const uint16_t CLASS2;
    static const uint16_t CLASS3;
    static const uint16_t NEED_TIME;
    static const uint16_t LOCAL;
    static const uint16_t DEVICE_TROUBLE;
    static const uint16_t DEVICE_RESTART;
    static const uint16_t FUNCTION_UNKNOWN;
    static const uint16_t OBJECT_UNKNOWN;
    static const uint16_t PARAMETER_ERROR;
    static const uint16_t BUFFER_OVERFLOW;
    static const uint16_t BAD_CONFIG;

    void str( char*buf);
};

#endif
