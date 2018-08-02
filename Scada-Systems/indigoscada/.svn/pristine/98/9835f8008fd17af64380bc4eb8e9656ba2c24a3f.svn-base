//
// $Id: stats.cpp 21 2007-04-16 17:55:56Z sparky1194 $
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <syslog.h>
#include <errno.h>
#include <assert.h>
#include <time.h>

#include "stats.hpp"

  Stats::Stats( char* userName, DnpAddr_t dnpAddr,
		int* debugLevel_p,
		Element *elements, int num_elements,
		EventInterface* eventInterface_p,
		EventInterface::PointType_t pType) :
    db_p( eventInterface_p), addr( dnpAddr), pointType(pType)
{
    if (userName != NULL)
    {
	strncpy(name, userName, MAX_USER_NAME_LEN);
	sprintf(buf, "%s", name );
    }
    nameLen = strlen(buf);
    outputLevel_p = debugLevel_p;
    num           = num_elements;
    x             = elements;
    for (int i=0; i<num; i++)
    {
	assert (x[i].index == i);
	db_p->registerName( addr, i,
	     (EventInterface::PointType_t) ((int) pointType + x[i].normal),
			    x[i].name, x[i].initialValue);
    }
    reset();
}

void Stats::reset( int index)
{
    assert (index < num);
    int i;
    if (index==-1)
    {
	// reset them all
	for(i=0; i<num; i++)
	{
	    x[i].value = x[i].initialValue;
	}
    }
    else
    {
	x[index].value = x[index].initialValue;
    }
}

unsigned int Stats::get( int index) const
{
    return x[index].value;
}

void Stats::increment( int index)
{

    assert (index < num);
    assert (num > 0);
    x[index].value++;
    logChangeInStatValue(index);
}

  void Stats::set( int index, unsigned int v)
{

    assert (index < num);
    assert (num > 0);
    assert (x[index].value != v);
    x[index].value = v;
    logChangeInStatValue(index);
}

void Stats::logChangeInStatValue(int index)
{
    if (db_p != NULL)
 	db_p->changePoint(addr, index,
	 (EventInterface::PointType_t) ((int)pointType+ x[index].normal),
			  x[index].value);

    // always log secure authentication mesages for prototype
    if ((x[index].normal) && (pointType != EventInterface::SA_AB_ST))
	logNormal     ("%s = %d", x[index].name, x[index].value);
    else
	logAbnormal(0, "%s = %d", x[index].name, x[index].value);
}

void Stats::logNormal(const char *format, ...)
{
    va_list        ap;
    int n;

    if (*outputLevel_p >= 1)
    {
	n = nameLen;
	sprintf(buf+n, " Info: ");
	n += 7;
        va_start(ap,format);
#ifdef HAVE_VSNPRINTF
        vsnprintf(buf+n, sizeof(buf)-n, format, ap);
#else
	vsprintf(buf+n, format, ap);
#endif
	va_end(ap);
	
	strcat(buf,"\n");

	log(buf);
    }
}

void Stats::logAbnormal(int use_errno, const char *format, ...)
{
    va_list       ap;
    int           n;
    int           old_errno = errno; // save the errno so it isn't  overwritten

    if (*outputLevel_p >= 0)
    {
	n = nameLen;
	sprintf(buf+n, " Warn: ");
	n += 7;
	va_start(ap,format);
#ifdef HAVE_VSNPRINTF
	vsnprintf(buf+n, sizeof(buf)-n, format, ap);
#else
	vsprintf(buf+n, format, ap);
#endif
	va_end(ap);

	n = strlen(buf);
	if(use_errno)
	    sprintf(buf+n, ": %s", strerror(old_errno));
	
	strcat(buf,"\n");

	log(buf);
    }
}

void Stats::log( const char* buf, int logToSysLog)
{
    char s[20];
    time_t t = time(NULL);
    strftime( s, 20, "%F %H:%M:%S", localtime( &t));
    fprintf(stderr, "%s  %s", s, buf);
//     if (logToSysLog)
// 	syslog(LOG_INFO, buf);
}
void Stats::log( int logToSysLog, int use_errno, const char *format, ...)
{
    char                   buf[MAX_LOG_LEN];
    char                   s[20];
    time_t                 t = time(NULL);
    va_list       ap;
    int           old_errno = errno; // save the errno so it isn't  overwritten

    strftime( s, 20, "%F %H:%M:%S", localtime( &t));

    va_start(ap,format);
#ifdef HAVE_VSNPRINTF
    vsnprintf(buf, sizeof(buf), format, ap);
#else
    vsprintf(buf, format, ap);
#endif
    va_end(ap);

    if(use_errno)
	sprintf(buf, "%s", strerror(old_errno));
	
    strcat(buf,"\n");

    fprintf(stderr, "%s  %s", s, buf);
//     if (logToSysLog)
// 	syslog(LOG_INFO, buf);
}
