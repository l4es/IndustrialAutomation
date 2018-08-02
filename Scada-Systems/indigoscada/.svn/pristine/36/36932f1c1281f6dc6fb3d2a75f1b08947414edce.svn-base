//
// $Id: timer_interface.hpp 25 2007-04-16 18:48:46Z sparky1194 $
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

// abstract class that defines the interface for timers
// this abstraction is so that timers can be manually expired during
// unittesting, system testing or debugging
// as well as to accomodate various platform dependent timer implemenations

#ifndef TIMER_INTERFACE_H
#define TIMER_INTERFACE_H

#include "common.hpp"

// implementors of the timer interface must call the application's (either
// master or outstation) timeout method when a timeout occurs

class TimerInterface
{

  public:
    enum TimerId   { RESPONSE,      // used by normal protocol and security
		     KEY_CHANGE,    // used by master security
		     CHALLENGE,     // used by master and outstation security
		     SESSION_KEY,   // used by outstation security
		     NUM_TIMERS };

    virtual void activate( TimerId timerId)=0;
    virtual void cancel( TimerId timerId)=0;
    virtual bool isActive( TimerId timerId)=0;

    virtual ~TimerInterface();
};

#endif
