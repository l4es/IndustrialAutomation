//
// $Id: common.hpp 4 2007-04-10 22:55:27Z sparky1194 $
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


#ifndef DNP_COMMON_H
#define DNP_COMMON_H

#include <deque>
#include "config.h"

#pragma warning (disable : 4290)

// use a deque instead of vector because we will often be removing
// bytes from the beginning
typedef std::deque<unsigned char> Bytes;

// timing releated internal to the protocol.
// it is better to use because the OS time can be changed
// due to time sync etc.
// use something > 4 bytes to avoid a rollover
typedef uint64_t Uptime_t;

// DNP time is a six byte unsigned int representing the number of milli-seconds
// since midnight UTC Jan 1, 1970 (does not include leap seconds)
typedef uint64_t DnpTime_t;

typedef uint16_t DnpAddr_t;
typedef uint8_t  UserNumber_t;
typedef uint32_t DnpIndex_t;
typedef uint32_t DnpStat_t;

// returns the overall version
const char* version();

// returns the license text in html format
const char* x11_license();

char* hex_repr( const Bytes& ab, char* buf, unsigned int buf_len);

// Append len bytes of random data to the end of 'data'
void randGen(Bytes& data, int len);

// These functions assume data is in or should be in little endian order
// This is normal for DNP. (DNP does not use network byte order)
// Exceptions:
// None known.

// Append a 48-bit word to the end of data
void appendUINT48(Bytes& data, uint64_t val);
// Append a 32-bit word to the end of data
void appendUINT32(Bytes& data, uint32_t val);
// Append a 24-bit word to the end of data
void appendUINT24(Bytes& data, uint32_t val);
// Append a 16-bit word to the end of data
void appendUINT16(Bytes& data, uint16_t val);
// Append a 8-bit word to the end of data
void appendUINT8(Bytes& data, uint8_t val);

// Append a 32-bit word to the end of data
void appendINT32(Bytes& data, int32_t val);
// Append a 16-bit word to the end of data
void appendINT16(Bytes& data, int16_t val);


// Append val to the end of data
void appendBytes(Bytes& data, const Bytes& val);

uint64_t removeUINT48(Bytes& data) throw(int);
uint32_t removeUINT32(Bytes& data) throw(int);
uint32_t removeUINT24(Bytes& data) throw(int);
uint16_t removeUINT16(Bytes& data) throw(int);
uint8_t removeUINT8(Bytes& data) throw(int);

int32_t removeINT32(Bytes& data) throw(int);
int16_t removeINT16(Bytes& data) throw(int);

float removeFLOAT(Bytes& data)  throw(float); //apa+++

// takes the first len bytes of data and adds them to the end of val
void moveBytes(Bytes& data, Bytes& val, unsigned int len) throw(int);

#endif //DNP_COMMON_H
