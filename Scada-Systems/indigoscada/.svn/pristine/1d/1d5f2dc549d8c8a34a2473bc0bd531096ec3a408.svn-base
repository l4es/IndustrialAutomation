//
// $Id: app.hpp 4 2007-04-10 22:55:27Z sparky1194 $
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

#ifndef APP_H
#define APP_H

#include "common.hpp"
#include "asdu.hpp"
#include "datalink.hpp"
#include "factory.hpp"
#include "transport.hpp"
#include "stats.hpp"
#include "event_interface.hpp"
#include "timer_interface.hpp"

// Base class for Master and Outstation implementations
class Application
{
  public:
    virtual ~Application();
    TimerInterface*        timer_p;

  protected:
    friend class SecureAuthentication;
    friend class MasterSecurity;
    friend class OutstationSecurity;

    Application(int*                       debugLevel_p,
		DnpAddr_t                  dnpAddr,
		UserNumber_t               num,
		Datalink::DatalinkConfig&  datalinkConfig,
		EventInterface*            eventInterface_p,
		TimerInterface*            timerInterface_p );

    int*                   debug_p;
    DnpAddr_t              addr;
    UserNumber_t           userNum;
    Datalink               dl;
    TransportFunction*     tf_p;
    AppHeader              ah;
    ObjectHeader           oh;
    Factory                of;
    EventInterface*        db_p;
    bool                   secureAuthenticationEnabled;

    Bytes                  lastRxdAsdu; // used for by security

    // should be adequate for the app and object header strs
    char             strbuf[80];

    virtual void transmit()=0;

    virtual void appendVariableSizedObject(const ObjectHeader& h,
					   const DnpObject& o)=0;
};

#endif
