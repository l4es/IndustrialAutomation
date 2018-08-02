//
// $Id: object.cpp 19 2007-04-13 21:35:41Z sparky1194 $
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
#include "stdio.h"
#include "common.hpp"
#include "stats.hpp"
#include "object.hpp"

const uint16_t InternalIndications :: ALL_STATIONS               = 0x0001;
const uint16_t InternalIndications :: CLASS1                     = 0x0002;
const uint16_t InternalIndications :: CLASS2                     = 0x0004;
const uint16_t InternalIndications :: CLASS3                     = 0x0008;
const uint16_t InternalIndications :: NEED_TIME                  = 0x0010;
const uint16_t InternalIndications :: LOCAL                      = 0x0020;
const uint16_t InternalIndications :: DEVICE_TROUBLE             = 0x0040;
const uint16_t InternalIndications :: DEVICE_RESTART             = 0x0080;
const uint16_t InternalIndications :: FUNCTION_UNKNOWN           = 0x0100;
const uint16_t InternalIndications :: OBJECT_UNKNOWN             = 0x0200;
const uint16_t InternalIndications :: PARAMETER_ERROR            = 0x0400;
const uint16_t InternalIndications :: BUFFER_OVERFLOW            = 0x0800;
const uint16_t InternalIndications :: BAD_CONFIG                 = 0x2000;


DnpObject::DnpObject(float                  val,
		     uint8_t                        flags,
		     DnpIndex_t                     indx, 
		     EventInterface::PointType_t    pt,
		     DnpTime_t                      time ) :
  value(val), flag(flags), index(indx), pointType(pt), timestamp(time)
{

}

void DnpObject::encode(Bytes& data) const
{
    // must be implemented by the derived class
    assert(0);
}

void DnpObject::decode(Bytes& data) throw(int)
{
    // must be implemented by the derived class
    throw(__LINE__);
}

void DnpObject::decode(Bytes& data, uint32_t objectSize) throw(int)
{
    // must be implemented by the derived class
    throw(__LINE__);
}

unsigned int DnpObject::size() const
{
    // must be implemented by the derived class
    assert(0);
	return 0;
}

DnpObject::~DnpObject()
{
}

// Binary Inputs //////////////////////////////////////////////


BinaryInputWithStatus::BinaryInputWithStatus(uint8_t flag, DnpIndex_t index) :
  DnpObject( ((flag & 0x80) >> 7), flag, index, EventInterface::BI)
{
}

void BinaryInputWithStatus::encode(Bytes& data) const
{
    appendUINT8(data, flag);
}

void BinaryInputWithStatus::decode(Bytes& data) throw(int)
{
    flag = removeUINT8(data);
    value = (flag & 0x80) >> 7;
}

BinaryInputEvent::BinaryInputEvent(uint8_t flag, DnpIndex_t index,
				   DnpTime_t time) :
  DnpObject( ((flag & 0x80) >> 7), flag, index, EventInterface::BI, time)
{
}

void BinaryInputEvent::encode(Bytes& data) const
{
    appendUINT8(data, flag);
    appendUINT48(data, timestamp);
}

void BinaryInputEvent::decode(Bytes& data) throw(int)
{
    flag = removeUINT8(data);
    value = (flag & 0x80) >> 7;
    timestamp = removeUINT48(data);
}

BinaryInputEventRelativeTime::BinaryInputEventRelativeTime(uint8_t flag,
							   DnpIndex_t index,
							   DnpTime_t time) :
  DnpObject( ((flag & 0x80) >> 7), flag, index, EventInterface::BI, time)
{
}

void BinaryInputEventRelativeTime::encode(Bytes& data) const
{
    appendUINT8(data, flag);
    appendUINT16(data, timestamp);
}

void BinaryInputEventRelativeTime::decode(Bytes& data) throw(int)
{
    flag = removeUINT8(data);
    value = (flag & 0x80) >> 7;
    timestamp = removeUINT16(data);
}


// Binary Outputs //////////////////////////////////////////////

BinaryOutputStatus::BinaryOutputStatus(uint8_t flag, DnpIndex_t index) :
  BinaryInputWithStatus( flag, index)
{
    // the only difference between BinaryOutputStatus and BinaryInputWithStatus
    pointType = EventInterface::BO;
}

ControlOutputRelayBlock::ControlOutputRelayBlock(Code       code,
						 DnpIndex_t index,
						 uint32_t   on,
						 uint32_t   off,
						 Status     st) :
  DnpObject( 0, 0, index, EventInterface::BO),
  outputCode( code),
  count(1),
  onTime(on),
  offTime(off),
  status(st)
{
}

void ControlOutputRelayBlock::encode(Bytes& data) const
{
    // since queue, clear, tripClose flags are not used and are always 0
    // the first byte is simply the code
    appendUINT8  (data, outputCode);
    appendUINT8  (data, count);
    appendUINT32 (data, onTime);
    appendUINT32 (data, offTime);
    appendUINT8  (data, status);
}

void ControlOutputRelayBlock::decode(Bytes& data) throw(int)
{
    outputCode = (Code) removeUINT8(data);
    count      = removeUINT8(data);
    onTime     = removeUINT32(data);
    offTime    = removeUINT32(data);
    status     = (Status) removeUINT8(data);
}

// Analog Inputs ///////////////////////////////////////////////

Bit32AnalogInput::Bit32AnalogInput(int32_t v, uint8_t flag,DnpIndex_t index)
  :  DnpObject( v, flag, index, EventInterface::AI)
{
}

void Bit32AnalogInput::encode(Bytes& data) const
{
    appendUINT8(data, flag);
    appendINT32(data, value);
}

void Bit32AnalogInput::decode(Bytes& data) throw(int)
{
    flag  = removeUINT8(data);
    value = removeINT32(data);
	//value = removeFLOAT(data); //apa+++ 25-06-2012
}

Bit16AnalogInput::Bit16AnalogInput(int16_t v, uint8_t flag,DnpIndex_t index)
  :  DnpObject( v, flag, index, EventInterface::AI)
{
}

void Bit16AnalogInput::encode(Bytes& data) const
{
    appendUINT8(data, flag);
    appendINT16(data, value);
}

void Bit16AnalogInput::decode(Bytes& data) throw(int)
{
    flag  = removeUINT8(data);
    value = removeINT16(data);
}

Bit32AnalogInputNoFlag::Bit32AnalogInputNoFlag(float v, DnpIndex_t index)
  :  DnpObject( v, ONLINE, index, EventInterface::AI)
{
}

void Bit32AnalogInputNoFlag::encode(Bytes& data) const
{
    appendINT32(data, value);
}

void Bit32AnalogInputNoFlag::decode(Bytes& data) throw(int)
{
    flag  = ONLINE;
    value = removeINT32(data);
}

Bit16AnalogInputNoFlag::Bit16AnalogInputNoFlag(int16_t v, DnpIndex_t index)
  :  DnpObject( v, ONLINE, index, EventInterface::AI)
{
}

void Bit16AnalogInputNoFlag::encode(Bytes& data) const
{
    appendINT16(data, value);
}

void Bit16AnalogInputNoFlag::decode(Bytes& data) throw(int)
{
    flag  = ONLINE;
    value = removeINT16(data);
}

// Counter Inputs //////////////////////////////////////////////

Bit32BinaryCounter::Bit32BinaryCounter(uint32_t v,
				       uint8_t flag, DnpIndex_t index) :
  Bit32AnalogInput( (int32_t)v, flag, index)
{
    pointType = EventInterface::CI;
}

Bit16BinaryCounter::Bit16BinaryCounter(uint16_t v,
				       uint8_t flag, DnpIndex_t index) :
  Bit16AnalogInput( (int16_t)v, flag, index)
{
    pointType = EventInterface::CI;
}

Bit32BinaryCounterNoFlag::Bit32BinaryCounterNoFlag(uint32_t v,DnpIndex_t index)
  : Bit32AnalogInputNoFlag( (float)v, index)
{
    pointType = EventInterface::CI;
}

Bit16BinaryCounterNoFlag::Bit16BinaryCounterNoFlag(uint16_t v,DnpIndex_t index)
  : Bit16AnalogInputNoFlag( (int16_t)v, index)
{
    pointType = EventInterface::CI;
}

// Analog Outputs /////////////////////////////////////////////


Bit16AnalogOutput::Bit16AnalogOutput(uint16_t requestedValue,
				     DnpIndex_t index,
				     Status st) :
  DnpObject( 0, 0, index, EventInterface::AO),
  request( requestedValue),
  status(st)
{
}

void Bit16AnalogOutput::encode(Bytes& data) const
{
    appendUINT16 (data, request);
    appendUINT8  (data, status);
}

void Bit16AnalogOutput::decode(Bytes& data) throw(int)
{
    request = removeUINT16(data);
    status  = (Status) removeUINT8(data);
}

// Other Objects //////////////////////////////////////////////

TimeAndDate::TimeAndDate(DnpTime_t time)
  :  DnpObject( 0, 0, 0, EventInterface::NONE, time)
{
}

void TimeAndDate::encode(Bytes& data) const
{
    appendUINT48(data, timestamp);
}

void TimeAndDate::decode(Bytes& data) throw(int)
{
    timestamp = removeUINT48(data);
}

TimeDelayCoarse::TimeDelayCoarse(uint16_t delay)
  :  DnpObject( delay, 0, 0, EventInterface::NONE)
{
}

void TimeDelayCoarse::encode(Bytes& data) const
{
    appendUINT16(data, value);
}

void TimeDelayCoarse::decode(Bytes& data) throw(int)
{
    value = removeUINT16(data);
}


// security objects /////////////////////////////////////////


Challenge::Challenge(uint32_t challengeSeqNum, UserNumber_t num, 
		     HMACAlgorithm algorithm, ChallengeReason reason) :
  seqNum(challengeSeqNum), userNum(num), hmacAlgorithm(algorithm), 
  challengeReason(reason)
{
    randGen(challengeData, MIN_CHALLENGE_SIZE);
}

void Challenge::decode(Bytes& data, uint32_t objectSize) throw(int)
{
    seqNum           = removeUINT24(data);
    userNum          = removeUINT8(data);
    hmacAlgorithm    = (HMACAlgorithm)   removeUINT8(data);
    challengeReason  = (ChallengeReason) removeUINT8(data);
    moveBytes(data, challengeData, objectSize - 6 );
}

void Challenge::encode(Bytes& data) const
{
    appendUINT24(data, seqNum);
    appendUINT8(data, userNum);
    appendUINT8(data, hmacAlgorithm);
    appendUINT8(data, challengeReason);
    appendBytes(data, challengeData);
}

unsigned int Challenge::size() const
{
    return 6 + challengeData.size();
}

Reply::Reply(uint32_t challengeSeqNum, UserNumber_t num, Bytes& hmac) :
  seqNum(challengeSeqNum), userNum(num), hmacValue(hmac)
{
}

void Reply::decode(Bytes& data, uint32_t objectSize) throw(int)
{
    seqNum = removeUINT24(data);
    userNum = removeUINT8(data);
    hmacValue.resize(0); // init hmac
    moveBytes(data, hmacValue, objectSize - 4);
}

void Reply::encode(Bytes& data) const
{
    appendUINT24(data, seqNum);
    appendUINT8(data, userNum);
    appendBytes(data, hmacValue);
}

unsigned int Reply::size() const
{
    return 4 + hmacValue.size();
}


SessionKeyStatusReq::SessionKeyStatusReq(UserNumber_t num) :
    userNum(num)
{
}

void SessionKeyStatusReq::decode(Bytes& data) throw(int)
{
    userNum = removeUINT8(data);
}

void SessionKeyStatusReq::encode(Bytes& data) const
{
    appendUINT8(data, userNum);
}

SessionKeyStatus::SessionKeyStatus( uint32_t seqNum, UserNumber_t num, 
				    KeyWrapAlgorithm algorithm,
				    KeyStatus status,
				    Bytes& data) :
  keyChangeSeqNum(seqNum), userNum(num), keyWrapAlgorithm(algorithm),
  keyStatus(status), challengeData(data)
{
}


void SessionKeyStatus::decode(Bytes& data, uint32_t objectSize) throw(int)
{
    keyChangeSeqNum  = removeUINT24(data);
    userNum          = removeUINT8(data);
    keyWrapAlgorithm = (KeyWrapAlgorithm) removeUINT8(data);
    keyStatus        = (KeyStatus) removeUINT8(data);
    moveBytes(data, challengeData, objectSize-6);
}

void SessionKeyStatus::encode(Bytes& data) const
{
    appendUINT24(data, keyChangeSeqNum);
    appendUINT8(data, userNum);
    appendUINT8(data, keyWrapAlgorithm);
    appendUINT8(data, keyStatus);
    appendBytes(data, challengeData);
}

bool SessionKeyStatus::operator==(const SessionKeyStatus &other) const
{
    if ((keyChangeSeqNum    == other.keyChangeSeqNum)    &&
	(userNum            == other.userNum)            &&
	(keyWrapAlgorithm   == other.keyWrapAlgorithm)   &&
	(keyStatus          == other.keyStatus)          &&
	(challengeData      == other.challengeData))
	return true;
    else
	return false;
}

bool SessionKeyStatus::operator!=(const SessionKeyStatus &other) const
{
    return !(*this == other);
}

unsigned int SessionKeyStatus::size() const
{
    return 6 + challengeData.size();
}

SessionKeyChange::SessionKeyChange(uint32_t seqNum, UserNumber_t num,
				   Bytes& keyData) :
    keyChangeSeqNum(seqNum),
    userNum(num),
    wrappedKeyData(keyData)
{
}

void SessionKeyChange::decode(Bytes& data, uint32_t objectSize) throw(int)
{
    keyChangeSeqNum  = removeUINT24(data);
    userNum          = removeUINT8(data);
    moveBytes(data, wrappedKeyData, objectSize-4);
}

void SessionKeyChange::encode(Bytes& data) const
{
    appendUINT24(data, keyChangeSeqNum);
    appendUINT8(data, userNum);
    appendBytes(data, wrappedKeyData);
}

unsigned int SessionKeyChange::size() const
{
    return 4 + wrappedKeyData.size();
}

AuthenticationError::AuthenticationError(uint32_t sequenceNum, 
					 UserNumber_t num,
					 ErrorReason errReason,
					 Bytes& text) :
  seqNum(sequenceNum), userNum(num), errorReason(errReason), errorText(text)
{
}

void AuthenticationError::decode(Bytes& data, uint32_t objectSize) throw(int)
{
    seqNum       = removeUINT24(data);
    userNum      = removeUINT8(data);
    errorReason  = (ErrorReason) removeUINT8(data);
    moveBytes(data, errorText, objectSize - 5);
}

void AuthenticationError::encode(Bytes& data) const
{
    appendUINT24(data, seqNum);
    appendUINT8(data, userNum);
    appendUINT8(data, errorReason);

    appendBytes(data, errorText);
}

unsigned int AuthenticationError::size() const
{
    return 5 + errorText.size();
}
