//
// $Id: stats.hpp 4 2007-04-10 22:55:27Z sparky1194 $
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

// Purpose: A generic class keep track of statistics

// The abonormal stats are not necceessarily errors they are just
// events that occour on an infrequent basis that can
// be logged for uncluttered informational/debugging purposes

// outputLevel =  1 - all normal and abnormal stats sent to output
// outputLevel =  0 - only abnormal stats are sent to output
// outputLevel = -1 - nothing is sent to output

#ifndef STATS_H
#define STATS_H

#include <stdlib.h> // for NULL
#include "event_interface.hpp"
 
class Stats
{
public:

	#define MAX_STAT_NAME_LEN    30
	#define MAX_USER_NAME_LEN    sizeof("AP (12345)") // incl. null
	// two chars for hex digit and one for space + one for null
	#define MAX_LOG_LEN          3*2048 + 1

    enum OutputLevel { ABNORMAL = 0, NORMAL = 1 };

    typedef struct {
	char           index;
	char           name[30+1];
	OutputLevel    normal;
	DnpStat_t      initialValue;
	DnpStat_t      value;
    } Element;


    // a debug level of:  -1 will not log anything
    //                     0 will log all abnormalities
    //                     1 will log everything
    // -1 is only used for unit tests where abnormalities are
    // forces and we do not want to clutter the output
    Stats( char* userName = NULL,
	   DnpAddr_t dnpAddr=0,
	   int* debugLevel_p=NULL,
	   Element* elements=NULL, int num_elements=0,
	   EventInterface* eventInterface_p=NULL,
	   EventInterface::PointType_t pType=EventInterface::ST);

    void reset(int index=-1);
    void increment(int index);
    void decrement(int index);
    DnpStat_t get(int index) const;
    void set(int index, DnpStat_t v);
    void logNormal(const char *format, ...);
    void logAbnormal(int use_errno, const char *format, ...);

    // To be used by objects that do not have a reference to a Stats object
    static void log(int logToSysLog, int use_errno, const char *format, ...);

private:

    char                   name[MAX_USER_NAME_LEN];
    int                    nameLen;
    char                   buf[MAX_LOG_LEN];
    int*                   outputLevel_p;
    int                    num;           // number of stats
    Element                *x;            // ptr to array of elements
    void logChangeInStatValue( int index);
    // buf must be null terminated string
    void log( const char* buf, int logToSysLog=0);
    EventInterface* db_p;
    DnpAddr_t addr;
    EventInterface::PointType_t pointType;

};


#endif
