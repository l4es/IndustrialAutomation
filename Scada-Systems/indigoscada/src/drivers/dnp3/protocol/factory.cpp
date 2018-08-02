//
// $Id: factory.cpp 23 2007-04-16 18:46:37Z sparky1194 $
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

#include "assert.h"
#include "stdio.h"
#include "common.hpp"
#include "stats.hpp"
#include "factory.hpp"

#pragma warning (disable : 4786) //apa+++ 12-06-2012

const uint8_t ObjectHeader::PACKED_WITHOUT_A_PREFIX = 0;
const uint8_t ObjectHeader::ONE_OCTET_INDEX         = 1;
const uint8_t ObjectHeader::TWO_OCTET_INDEX         = 2;
const uint8_t ObjectHeader::FOUR_OCTET_INDEX        = 3;
const uint8_t ObjectHeader::ONE_OCTET_SIZE          = 4;
const uint8_t ObjectHeader::TWO_OCTET_SIZE          = 5;
const uint8_t ObjectHeader::FOUR_OCTET_SIZE         = 6;

// range specifier codes
const uint8_t ObjectHeader::ONE_OCTET_START_STOP_INDEXES               = 0;
const uint8_t ObjectHeader::TWO_OCTET_START_STOP_INDEXES               = 1;
const uint8_t ObjectHeader::FOUR_OCTET_START_STOP_INDEXES              = 2;
const uint8_t ObjectHeader::ONE_OCTET_START_STOP_VIRTUAL_ADDRESSES     = 3;
const uint8_t ObjectHeader::TWO_OCTET_START_STOP_VIRTUAL_ADDRESSES     = 4;
const uint8_t ObjectHeader::FOUR_OCTET_START_STOP_VIRTUAL_ADDRESSES    = 5;
const uint8_t ObjectHeader::NO_RANGE_FIELD                             = 6;
const uint8_t ObjectHeader::ONE_OCTET_COUNT_OF_OBJECTS                 = 7;
const uint8_t ObjectHeader::TWO_OCTET_COUNT_OF_OBJECTS                 = 8;
const uint8_t ObjectHeader::FOUR_OCTET_COUNT_OF_OBJECTS                = 9;
const uint8_t ObjectHeader::ONE_OCTET_COUNT_OF_OBJECTS_VARIABLE_FORMAT =0xb;

ObjectHeader::ObjectHeader(uint8_t group,
			   uint8_t variation,
			   uint8_t qualifier,
			   uint32_t countOfObjects,
			   uint32_t startIndex,
			   uint32_t stopIndex ) :
  grp(group), var(variation), qual(qualifier), count(countOfObjects),
  start(startIndex), stop(stopIndex)
{
    indexSize = (qual & 0x70) >> 4;
    rangeSpecifier = qual & 0x0f;
}

char* ObjectHeader::str( char* buf, int len) const
{
    sprintf( buf, "Oh: Grp:%d,Var=%d",grp,var);
    return buf;
}

void ObjectHeader::encode( Bytes& data) const
{
    data.push_back( grp);
    data.push_back( var);
    data.push_back( qual);

    if (rangeSpecifier == ONE_OCTET_START_STOP_INDEXES)
    {
		// range size is two bytes
		data.push_back( start);
		data.push_back( stop);
    }
    else if (rangeSpecifier == TWO_OCTET_START_STOP_INDEXES)
    {
		// range size is four bytes
		appendUINT16(data, start);
		appendUINT16(data, stop);
    }
    else if (rangeSpecifier == ONE_OCTET_COUNT_OF_OBJECTS)
    {
		// range size is one byte
		data.push_back(  count);
    }
    else if (rangeSpecifier == TWO_OCTET_COUNT_OF_OBJECTS)
    {
		// range size is two bytes
		appendUINT16(data, count);
    }
    else if (rangeSpecifier == ONE_OCTET_COUNT_OF_OBJECTS_VARIABLE_FORMAT)
    {
		data.push_back( count);
    }
    else if (rangeSpecifier == NO_RANGE_FIELD)
    {
    }
    else
    {
		Stats::log(0, 0, "Unsupported Qualifier code 0x%x", qual);
		assert(0);
    }
}

void ObjectHeader::decode( Bytes& data, Stats& stats) throw(int)
{
    grp = data[0]; data.pop_front();
    var = data[0]; data.pop_front();

    // this next byte contains both the index size and the range specifier
    qual = data[0]; data.pop_front();
    indexSize = (qual & 0x70) >> 4;
    rangeSpecifier = qual & 0x0f;

    if (rangeSpecifier == ONE_OCTET_START_STOP_INDEXES)
    {
		// range size is two bytes
		start = data[0]; data.pop_front();
		stop = data[0]; data.pop_front();
    }
    else if (rangeSpecifier == TWO_OCTET_START_STOP_INDEXES)
    {
		// range size is four bytes
		start = data[0] + (data[1] << 8);
		data.pop_front();
		data.pop_front();

		stop = data[0] + (data[1] << 8);
		data.pop_front();
		data.pop_front();
    }
    else if (rangeSpecifier == NO_RANGE_FIELD)
    {
		// range size is zero bytes
    }
    else if (rangeSpecifier == ONE_OCTET_COUNT_OF_OBJECTS)
    {
		// range size is one byte
		count = data[0]; data.pop_front();
		}
		else if (rangeSpecifier == TWO_OCTET_COUNT_OF_OBJECTS)
		{
		// range size is two bytes
		count = data[0] + (data[1] << 8);
		data.pop_front();
		data.pop_front();
    }
    else if (rangeSpecifier == ONE_OCTET_COUNT_OF_OBJECTS_VARIABLE_FORMAT)
    {
		count = data[0]; data.pop_front();
    }
    else
    {
		stats.logAbnormal(0, "Unsupported Qualifier code 0x%x", qual);
		throw(__LINE__);
    }
}

Factory::DnpObjectMap Factory::objectMap;

Factory::Factory(EventInterface* eventInterface_p) :
  db_p(eventInterface_p),
  cto(0)
{
    // 1,2 is used when decoding 1,1 and 3,1
    // this is because the object is less than one byte
    objectMap[ key(  1,  2)] = new BinaryInputWithStatus();
    objectMap[ key(  2,  1)] = new BinaryInputEventNoTime();
    objectMap[ key(  2,  2)] = new BinaryInputEvent();
    objectMap[ key(  2,  3)] = new BinaryInputEventRelativeTime();
    objectMap[ key( 10,  2)] = new BinaryOutputStatus();
    objectMap[ key( 12,  1)] = new ControlOutputRelayBlock();
    objectMap[ key( 20,  1)] = new Bit32BinaryCounter();
    objectMap[ key( 20,  2)] = new Bit16BinaryCounter();
    objectMap[ key( 20,  3)] = new Bit32DeltaCounter();
    objectMap[ key( 20,  4)] = new Bit16DeltaCounter();
    objectMap[ key( 20,  5)] = new Bit32BinaryCounterNoFlag();
    objectMap[ key( 20,  6)] = new Bit16BinaryCounterNoFlag();
    objectMap[ key( 20,  7)] = new Bit32DeltaCounterNoFlag();
    objectMap[ key( 20,  8)] = new Bit16DeltaCounterNoFlag();
    objectMap[ key( 22,  1)] = new Bit32CounterEventNoTime();
    objectMap[ key( 22,  2)] = new Bit16CounterEventNoTime();
    objectMap[ key( 22,  3)] = new Bit32DeltaCounterEventNoTime();
    objectMap[ key( 22,  4)] = new Bit16DeltaCounterEventNoTime();
    objectMap[ key( 30,  1)] = new Bit32AnalogInput();
    objectMap[ key( 30,  2)] = new Bit16AnalogInput();
    objectMap[ key( 30,  3)] = new Bit32AnalogInputNoFlag();       
    objectMap[ key( 30,  4)] = new Bit16AnalogInputNoFlag();
    objectMap[ key( 32,  1)] = new Bit32AnalogEventNoTime();
    objectMap[ key( 32,  2)] = new Bit16AnalogEventNoTime();
    objectMap[ key( 40,  2)] = new Bit16AnalogOutputStatus();
    objectMap[ key( 41,  2)] = new Bit16AnalogOutput();
    objectMap[ key( 50,  1)] = new TimeAndDate();
    objectMap[ key( 51,  1)] = new TimeAndDateCTO();
    objectMap[ key( 51,  2)] = new UnsyncronizedTimeAndDateCTO();
    objectMap[ key( 52,  1)] = new TimeDelayCoarse();
    objectMap[ key( 52,  2)] = new TimeDelayFine();
    objectMap[ key(120,  1)] = new Challenge();
    objectMap[ key(120,  2)] = new Reply();
    objectMap[ key(120,  3)] = new AggressiveModeRequest();
    objectMap[ key(120,  4)] = new SessionKeyStatusReq();
    objectMap[ key(120,  5)] = new SessionKeyStatus();
    objectMap[ key(120,  6)] = new SessionKeyChange();
    objectMap[ key(120,  7)] = new AuthenticationError();

    selfTest();
}

// this method could be expanded to include tests for the variable
// sized and other more complex objects
void Factory::selfTest()
{
    DnpObjectMap::iterator iter;

    // the simple encode and decode test only works for objects that
    // have implemented these methods.

    ObjectKey lastObjectToTest =  key(52,2);

    for(iter = objectMap.begin(); iter->first != lastObjectToTest; iter++)
    {
		assert (iter != objectMap.end());

		Bytes data;
		DnpObject* o = iter->second;

		try
		{
			o->encode( data);
			o->decode( data);
		}
		catch (int e)
		{
			printf ("Testing grp=%d var=%d\n", ((iter->first & 0xff00) >> 8),
				iter->first & 0xff);
			printf ("Exception line# %d\n", e);
			assert(0);
		}
    }
}

Factory::ObjectKey Factory::key(uint8_t group, uint8_t variation)
{
    return ( (group<<8) + variation);
}


DnpObject* Factory::decode(const ObjectHeader& oh, Bytes& data,
			     DnpAddr_t addr,
			     Stats& stats) throw(int)
{
    uint32_t num;

    if (oh.rangeSpecifier == ObjectHeader::TWO_OCTET_START_STOP_INDEXES ||
	oh.rangeSpecifier == ObjectHeader::ONE_OCTET_START_STOP_INDEXES    )
    {
		if (oh.indexSize == ObjectHeader::PACKED_WITHOUT_A_PREFIX)
		{
			num = oh.stop - oh.start + 1;
			stats.logNormal( "Decoding: %d object(s)", num);
			createObjects(oh.grp, oh.var, data, oh.start, oh.stop,addr,stats);
		}
		else
		{
			stats.logAbnormal(0, "Rx Unsupported Qualifier %d", oh.qual);
			throw(__LINE__);
		}
    }
    else if (oh.rangeSpecifier == ObjectHeader::TWO_OCTET_COUNT_OF_OBJECTS ||
	     oh.rangeSpecifier == ObjectHeader::ONE_OCTET_COUNT_OF_OBJECTS )
    {
		uint32_t index;
		stats.logNormal("Decoding: %d object(s)", oh.count);
		for (unsigned int i=0; i<oh.count; i++)
		{
			if (oh.indexSize == 2)
			{
				index = removeUINT16(data);
			}
			else if (oh.indexSize == 1)
			{
				index = removeUINT8(data);
			}
			else if (oh.indexSize == 0)
				index = NO_INDEX;
			else
			{
				// something has gone wrong
				stats.logAbnormal(0,
						  "Rx Unsupported Index Size %d",oh.indexSize);
				throw(__LINE__);
			}

			createObjects(oh.grp, oh.var, data, index, index, addr, stats);
		}
    }
    else if (oh.rangeSpecifier ==
	ObjectHeader::ONE_OCTET_COUNT_OF_OBJECTS_VARIABLE_FORMAT )
    {
		uint32_t objectSize;

		for(unsigned int i = 0; i < oh.count; i++)
		{
			// index size here is really used for the size of the object size
			if (oh.indexSize == ObjectHeader::ONE_OCTET_SIZE)
				objectSize = removeUINT8(data);
			else if (oh.indexSize == ObjectHeader::TWO_OCTET_SIZE)
				objectSize = removeUINT16(data);
			else if (oh.indexSize == ObjectHeader::FOUR_OCTET_SIZE)
				objectSize = removeUINT32(data);
			else
			{
				stats.logAbnormal(0,"Unsupported Qualifier code 0x%x",oh.qual);
				throw(__LINE__);
			}

			stats.logNormal( "Decoding: 1 object of size %d", objectSize);

			createObjects(oh.grp, oh.var, data,
				  NO_INDEX,
				  NO_INDEX,
				  addr,
				  stats,
				  objectSize );
		}
    }
    else
    {	
		stats.logAbnormal(0, "Rx Unsupported Qualifier %d", oh.qual);
		throw(__LINE__);
    }

    return lastObjectParsed;
}

void Factory::createObjects(uint8_t grp, uint8_t var, Bytes& data,
			    uint32_t startIndex, uint32_t stopIndex,
			    DnpAddr_t addr, Stats& stats,
			    uint32_t objectSize) throw(int)
{
    uint32_t  i;
    DnpObject* obj_p = NULL; // we should never return NULL
    // if we do have toubl parseing with should throw an exception

    // Binary Input is a special case because it is packed 
    if(grp == 1 && var == 1)
    {
 		uint8_t bitMask = 0x01;
		uint8_t flag;

 		for(i = startIndex; i < stopIndex + 1; i++)
		{
			BinaryInputWithStatus bi;

			if (data[0] & bitMask)
				flag = 0x81;
			else
				flag = 0x01;

			bi = BinaryInputWithStatus( flag);

			db_p->changePoint( addr, i,
					   bi.pointType,
					   bi.value,
					   bi.timestamp,
					   bi.flag,
					   grp,
					   var);

			if ((bitMask == 0x80) || (i == stopIndex))
				data.pop_front();

			if (bitMask == 0x80)
				bitMask = 0x01;
			else
				bitMask = bitMask << 1;
		}
    }
    // Double bit Binary Input is a special case because it is packed 
    else if (grp == 3 && var == 1)
    {
 		uint8_t bitMask = 0x03;
		uint8_t shift   = 0x00;
		uint8_t flag;

 		for(i = startIndex; i < stopIndex + 1; i++)
		{
			BinaryInputWithStatus bi;

			if ( ((data[0] & bitMask) >> shift) == 0x02)
				flag = 0x81;
			else
				flag = 0x01;

			bi = BinaryInputWithStatus( flag);

			db_p->changePoint( addr, i,
					   bi.pointType,
					   bi.value,
					   bi.timestamp,
					   bi.flag,
					   grp,
					   var);

			if ((bitMask == 0xC0) || (i == stopIndex))
				data.pop_front();

			if (bitMask == 0xC0)
			{
				bitMask = 0x03;
				shift = 0;
			}
			else
			{
				bitMask = bitMask << 2;
				shift += 2;
			}
		}
    }
    else
    {
		if (objectMap.count( key( grp, var)) == 0)
		{
			stats.logAbnormal(0,"Rx Unsupported Object grp=%d var=%d",grp,var);
			throw(__LINE__);
		}

		obj_p = objectMap[ key( grp, var)];

		if(startIndex == NO_INDEX)
		{
			if (objectSize > 0)
			{
				// this is a variable sized object and the object size
				// was specified in the header
				obj_p->decode(data, objectSize);
			}
			else
			{
				obj_p->decode(data);    // init instance
			}
		}
		else
		{
			for(i = startIndex; i < stopIndex + 1; i++)
			{
				obj_p->decode(data);    // init instance

				if ((grp == 2) && (var == 2))
					// handle another special case
					// we need to add the CTO to get a dnp time
					obj_p->timestamp += cto;
				
				db_p->changePoint(addr, i,
						  obj_p->pointType,
						  obj_p->value,
						  obj_p->timestamp,
						  obj_p->flag,
						  grp,
						  var);
			}
		}
    }

    lastObjectParsed = obj_p;
}

void Factory::setCTO( DnpTime_t newCTO)
{
    cto = newCTO;
}
