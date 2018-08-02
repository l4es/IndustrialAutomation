//
// $Id: asdu.hpp 4 2007-04-10 22:55:27Z sparky1194 $
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

#ifndef ASDU_H
#define ASDU_H

#include "common.hpp"

typedef uint8_t AppSeqNum_t;

// this is not meant to be instantiated
class AppSeqNum
{
  public:
    static void increment(AppSeqNum_t& seqNum);
    static void decrement(AppSeqNum_t& seqNum);
  private:
    AppSeqNum();
};

class AppHeader
{

public:

    enum FunctionCode  {
	// these function codes are valid only for requests
	CONFIRM                                = 0x00,
	READ                                   = 0x01,
	WRITE                                  = 0x02,
	SELECT                                 = 0x03,
	OPERATE                                = 0x04,
	DIR_OPERATE                            = 0x05,
	DIR_OPERATE_NO_RESP                    = 0x06,
	FREEZE                                 = 0x07,
	FREEZE_NO_RESP                         = 0x08,
	FREEZE_CLEAR                           = 0x09,
	FREEZE_CLEAR_NO_RESP                   = 0x0A,
	FREEZE_AT_TIME                         = 0x0B,
	FREEZE_AT_TIME_NO_RESP                 = 0x0C,
	COLD_RESTART                           = 0x0D,
	WARM_RESTART                           = 0x0E,
	INITIALIZE_DATA                        = 0x0F,
	INITIALIZE_APPLICATION                 = 0x10,
	START_APPLICATION                      = 0x11,
	STOP_APPLICATION                       = 0x12,
	SAVE_CONFIGURATION                     = 0x13,
	ENABLE_UNSOLICITED                     = 0x14,
	DISABLE_UNSOLICITED                    = 0X15,
	ASSIGN_CLASS                           = 0x16,
	DELAY_MEASUREMENT                      = 0x17,
	RECORD_CURRENT_TIME                    = 0x18,
	OPEN_FILE                              = 0x19,
	CLOSE_FILE                             = 0x1A,
	DELETE_FILE                            = 0X1B,
	GET_FILE_INFORMATION                   = 0x1C,
	AUTHENTICATE_FILE                      = 0x1D,
	ABORT_FILE                             = 0x1E,
	ACTIVATE_CONFIGURATION                 = 0x1F,
	AUTHENTICATION_REQUEST                 = 0x20,
	AUTHENTICATION_REPLY                   = 0x21,
	AUTHENTICATION_ERROR_NO_ACK            = 0x22,
	RESPONSE                               = 0x81,
	UNSOLICITED_RESPONSE                   = 0x82,
	AUTHENTICATION_CHALLENGE               = 0x83,
	UNSOLICITED_AUTHENTICATION_CHALLENGE   = 0x84,
	MAX_FUNCTION_CODE_INDEX                = 0x84
    };

    AppHeader() {};
    AppHeader( bool fir, bool fin,
	       bool con, bool uns, AppSeqNum_t seq,
	       FunctionCode fc, uint16_t iin=0 );

    // maximum app seq num is 15
    static uint8_t UNDEFINED_SEQ_NUM;
    void encode( Bytes& fragment);
    void decode( Bytes& fragment);
    void setConfirm( bool con);
    bool getFirst();
    bool getFinal();    
    bool getConfirm();
    bool getUnsolicited();
    AppSeqNum_t getSeqNum();
    FunctionCode getFn();
    static FunctionCode getFn( const Bytes& fragment);
    static AppSeqNum_t getSeqNum( const Bytes& fragment);
    static uint16_t getIIN( const Bytes& fragment);
    uint16_t getIIN();
    // text representation of the object
    // must pass in buffer and length of buffer
    char* str( char* buf, int len);

private:
    uint8_t       a[4];
    int           len;  // for requests len = 2 for repsonse len = 4

};

#endif
