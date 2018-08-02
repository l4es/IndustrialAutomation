//
// $Id: master.hpp 4 2007-04-10 22:55:27Z sparky1194 $
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

#ifndef DNP_MASTER_H
#define DNP_MASTER_H

#include "common.hpp"
#include "app.hpp"
#include "asdu.hpp"
#include "station.hpp"
#include "object.hpp"
#include "security.hpp"

#pragma warning (disable : 4786) //apa+++ 12-06-2012

// public methods can be called by the owner of the object
// in any order
// none of the states or methods are timing dependent to facilitate
// debugging
// this class is NOT reentrant
class Master : Application
{
public:

    typedef struct
    {
	    DnpAddr_t        addr;
	    int              consecutiveTimeoutsForCommsFail;
	    int*             integrityPollInterval_p;
	    UserNumber_t     userNum;
	    int*             debugLevel_p;
    } MasterConfig;

    Master( MasterConfig&              masterConfig,
	    Datalink::DatalinkConfig&  datalinkConfig,
	    Station::StationConfig     outstationConfig[],
	    int                        numOustations,
	    EventInterface*            eventInterface_p,
	    TimerInterface*            timerInterface_p );

    enum PollType  {  AUTO,
		      INTEGRITY,
		      EVENT };

    // this class will decide what type of transaction to begin eg. event poll
    // under normal mode of operation this method should be called on a 
    // regular basis, eg. each second
    DnpStat_t startNewTransaction();
    DnpStat_t poll( PollType pollType);
    DnpStat_t control( ControlOutputRelayBlock& cb);
    DnpStat_t timeout(TimerInterface::TimerId t);
    DnpStat_t rxData(Bytes* buf, Uptime_t timeRxd=0);

    void enableSecureAuthentication(DnpAddr_t stationAddr, bool enable = true);

    DnpStat_t getState() const;
    DnpStat_t getSecAuthState() const;

    DnpStat_t getStat( DnpAddr_t stationAddr, int index);
    DnpStat_t getSecAuthStat( DnpAddr_t stationAddr, int index);


private:
    friend class SecureAuthentication;
    friend class MasterSecurity;
    friend class TestSecurity;
    friend class TestMaster;


    unsigned int           maxConsecutive;
    int*                   integrityPollInterval_p;

    typedef std::map<DnpAddr_t, Station*> StationMap;
    // dnp address is the key
    StationMap             stationMap;
    // the active station
    StationMap::iterator   currentStationPair;

    // all of the private methods below will operate on the 
    // outstation specified by this pointer. So it is crucial that
    // this pointer is updated to the correct value before any
    // of the private methods are called
    Station*            stn_p;

    // for the transport function
    StationInfoMap stationInfoMap;

    // call when a new fragment has been rxd by the transport function
    void processRxdFragment();

    void completedTransaction();    
    // this method begins the construction of a request by
    // starting with the application header
    void initRequest( AppHeader::FunctionCode fn);

    // appends the intgrity poll object headers to the request
    void appendIntegrityPoll();

    // appends the event poll object headers to the request
    void appendEventPoll();

    void clearRestartBit();

    bool parseResponseObjects(Bytes &data);

    // checks that the fir and fin bits and the seq # make sense
    // if seq # from one before will resend
    // Returns 1 if good, and  0 if application should not process
    bool checkAppHeader();

    // checks if the data portions of the tx and rx fragments are identical
    bool verifyControlResp();
    void operate();

    void sendConfirm(AppSeqNum_t txSeqNum);
    void transmit();

    void appendVariableSizedObject( const ObjectHeader& h,
				    const DnpObject& o);
};

#endif
