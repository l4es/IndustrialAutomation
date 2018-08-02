//
// $Id: datalink.cpp 4 2007-04-10 22:55:27Z sparky1194 $
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
#include <string.h>  // for memcpy
#include <stdio.h>
#include "datalink.hpp"

Datalink::Datalink( DatalinkConfig& config,
		    EventInterface* eventInterface_p)			  
{
    char name[MAX_USER_NAME_LEN];

    isMaster                = config.isMaster;
    addr                    = config.addr;
    keepAliveInterval_ms    = config.keepAliveInterval_ms;
    tx_p                    = config.tx_p;

    linkReset = 0; // set to 1 upon receipt of a link reset
    rxFcb = 1;     // starts at 1 then alternates between good frames
    linkNeedsToBeReset = 1;

    state = IDLE;

//     if (dl->tx.tcp)
//     {
// 	/* then we need to use keep alive msgs */
// 	dl->lastRx = time();
// 	dl->keepAliveInterval = keepAliveInterval;
// 	dl->timeout = -1;
// 	dl->responseTimeout = responseTimeout; /* for keep alives */
// 	dl->dest = dest; /* only for keep alives (request link status) */
//     }


    // created a temp element array for initialiation
    // eg avoid having to do statElement[i].name ... for each stat
    Stats::Element   temp[] =
    {
	// the following stats are incremented by the LPDU class.
	// We are initializing them here because it is simpler to
	// have one stats class for the datalink regardless of which
	// class is incrementing the stat
	{ Lpdu::RX_START_OCTETS, "Rx Start Octets",         Stats::NORMAL,0,0},
	{ Lpdu::LOST_BYTES,      "Lost Bytes",            Stats::ABNORMAL,0,0},
	{ Lpdu::CRC_ERRORS,      "CRC Errors",            Stats::ABNORMAL,0,0},

	{ RX_FRAMES,             "Rx Frames"              , Stats::NORMAL,0,0},
	{ RX_ACKS,               "Rx Acks"                , Stats::NORMAL,0,0},
	{ RX_LINK_STATUS,        "Rx Link Status"         , Stats::NORMAL,0,0},
	{ RX_RESET_LINK,         "Rx Reset Link"          , Stats::NORMAL,0,0},
	{ RX_TEST_LINK,          "Rx Test Link"           , Stats::NORMAL,0,0},
	{ RX_USER_DATA_CONFIRM,  "Rx User Data (Confirm)" , Stats::NORMAL,0,0},
	{ RX_USER_DATA_NO_CONFIRM,"Rx User Data (No Confirm)",
	                                                    Stats::NORMAL,0,0},
	{ RX_REQUEST_LINK_STATUS,"Rx Request Link Status" , Stats::NORMAL,0,0},
	{ TX_FRAMES,             "Tx Frames"              , Stats::NORMAL,0,0},
	{ TX_ACKS,               "Tx Acks"                , Stats::NORMAL,0,0},
	{ TX_LINK_STATUS,        "Tx Link Status"         , Stats::NORMAL,0,0},
	{ TX_RESET_LINK,         "Tx Reset Link"          , Stats::NORMAL,0,0},
	{ TX_TEST_LINK,          "Tx Test Link"           , Stats::NORMAL,0,0},
	{ TX_USER_DATA_CONFIRM,  "Tx User Data (Confirm)" , Stats::NORMAL,0,0},
	{ TX_USER_DATA_NO_CONFIRM,"Tx User Data (No Confirm)",
                                                            Stats::NORMAL,0,0},
	{ TX_REQUEST_LINK_STATUS,"Tx Request Link Status" , Stats::NORMAL,0,0},
	
	{ TIMEOUTS_ACK,          "Timeouts - Ack"       , Stats::ABNORMAL,0,0},
	{ TIMEOUTS_lINK_STATUS,  "Timeouts - Link Status",Stats::ABNORMAL,0,0},
	{ RX_NACKS,              "Rx Nacks"              ,Stats::ABNORMAL,0,0},
	{ RX_NOT_SUPPORTED,      "Rx Not Supported"      ,Stats::ABNORMAL,0,0},
	{ RX_FN_CODE_ERROR,      "Rx Function Code Error",Stats::ABNORMAL,0,0},
	{ RX_UNEXPECTED_ACKS,    "Rx Unexpected Acks"    ,Stats::ABNORMAL,0,0},
	{ RX_UNEXPECTED_LINK_STATUS,"Rx Unexpected Link Status",
                                                          Stats::ABNORMAL,0,0},
	{ RX_FCB_INCORRECT,      "Rx FCB Incorrect"      ,Stats::ABNORMAL,0,0},
	{ RX_FCV_INCORRECT,      "Rx FCV Incorrect"      ,Stats::ABNORMAL,0,0},
	{ RX_OTHER_ADDRESSES,    "Rx Other Addresses"    ,Stats::ABNORMAL,0,0},
	{ TX_NACKS,              "Tx Nacks"              ,Stats::ABNORMAL,0,0},
	{ TX_NOT_SUPPORTED,      "Tx Not Supported"      ,Stats::ABNORMAL,0,0}
    };

    assert(sizeof(temp)/sizeof(Stats::Element) == NUM_STATS);
    memcpy(statElements, temp, sizeof(temp));
    sprintf(name, "DL %6d ", addr);
    stats = Stats( name, addr, config.debugLevel_p, statElements, NUM_STATS,
		   eventInterface_p, EventInterface::DL_AB_ST );
    rxLpdu = Lpdu( &stats);
}

// we never use datalink confirms
Uptime_t Datalink::transmit( DnpAddr_t dest, Bytes& data)
{
    assert(state == IDLE);
    txLpdu.build( isMaster,                     // dir
		  1      ,                      // prm
		  0,                            // fcb
		  0,                            // fcv/dfc
		  UNCONFIRMED_USER_DATA,        // fc
		  dest, addr, data);

    stats.increment(TX_USER_DATA_NO_CONFIRM);
    stats.increment(TX_FRAMES);
    return tx_p->transmit( txLpdu);
}

Lpdu::UserData& Datalink::rxData(Bytes& data)
{
    bool              lpduComplete;
    userData.data.clear();
    lpduComplete = rxLpdu.buildFromBytes(data);
    if (lpduComplete)
    {
	// this method will fill in userData if there is any valid user data
	processRxdLpdu();
	// we have finished processing this LPDU, reset so we can use again
	rxLpdu.reset(); 
    }
    return userData;
}

void Datalink::processRxdLpdu()
{
    DnpAddr_t      dest = rxLpdu.getDest();
    unsigned int   func = rxLpdu.getFunctionCode();
    int            broadcast;

    stats.increment(RX_FRAMES);

    // is it a broadcast?
    if (dest == 0xffff || dest ==0xfffe || dest == 0xfffd)
	broadcast = 1;
    else
        broadcast = 0;
                
    // check to see if it addressed to us or it is a broadcast
    if (dest == addr || broadcast)
    {
	// this LPDU is for us
	if ( rxLpdu.getPrimaryBit() == 0)
	{
	    // this is a response - check to see if we are expecting anything
	    // it should be one of four vaild response function codes
	    if (func == ACK)
	    {
		stats.increment( RX_ACKS);
		// check to see if we are expecting an ACK
		if (state == WAITING_FOR_ACK)
		{
		    state = IDLE;
		}
		else if ( state == WAITING_FOR_ACK_TO_LINK_RESET)
		{
		    linkNeedsToBeReset = 0;
		    state = IDLE;
		}
		else
		{
		    stats.increment( RX_UNEXPECTED_ACKS);
		}
	    }
	    else if (func == NACK)
	    {
		stats.increment( RX_NACKS);
		// this is the case no matter what
		linkNeedsToBeReset = 1;
		state = IDLE;
	    }
	    else if (func == LINK_STATUS)
	    {
		stats.increment( RX_LINK_STATUS);
		if ( state == WAITING_FOR_LINK_STATUS)
		{
		    state = IDLE;
		    // timeout = keepAliveInterval_ms;
		}
		else
		{
		    stats.increment( RX_UNEXPECTED_LINK_STATUS);
		}
	    }                                
	    else if (func == NOT_SUPPORTED_L)
	    {
		// this device can't support what we asked for
		stats.increment( RX_NOT_SUPPORTED);
	    }
	    else
	    {
		// invalid function for a response
		stats.increment( RX_FN_CODE_ERROR);
	    }
	}
	else // PRM = 1
	{
	    // this is an initiated transaction 
	    // it should be one of five vaild function codes
	    switch( func)
	    {
		case RESET_LINK:

		    stats.increment( RX_RESET_LINK);

		    if (rxLpdu.getFcv() == 0)
		    {
			rxFcb = 1; // we should expect this next
			linkReset = 1;
			if (!broadcast)
			    ack( rxLpdu.getSrc());
			else
			    stats.increment( RX_FCV_INCORRECT);
		    }
		    break;

		case TEST_LINK:

		    stats.increment( RX_TEST_LINK);

		    if ( linkReset == 1)
			if ( rxLpdu.getFcv() == 1)
			    if ( rxFcb == rxLpdu.getFcb())
			    {
				if (!broadcast)
				{
				    ack( rxLpdu.getSrc());
				    rxFcb = rxFcb^1;  // alternate
				}
			    }
			    else 
				stats.increment( RX_FCB_INCORRECT);
			else
			    stats.increment( RX_FCV_INCORRECT);
		    else
			if (!broadcast)
			    nack( rxLpdu.getSrc());
		    break;

		case CONFIRMED_USER_DATA:

		    stats.increment( RX_USER_DATA_CONFIRM);

		    if ( linkReset == 1)
			if ( rxLpdu.getFcv() == 1)
			{
			    // send an ack even if FCB bad
			    if (!broadcast)
				ack( rxLpdu.getSrc());

			    if ( rxFcb == rxLpdu.getFcb())
			    {
				rxFcb = rxFcb^1;  // alternate 
				// fill in user data for application layer use
				rxLpdu.getUserData(userData);
			    }
			    else
				stats.increment( RX_FCB_INCORRECT);
			}
			else
			    stats.increment( RX_FCB_INCORRECT);
		    else 
			if (!broadcast)
			    nack( rxLpdu.getSrc());
		    break;
                                
		case UNCONFIRMED_USER_DATA:

		    stats.increment( RX_USER_DATA_NO_CONFIRM);
		    if (rxLpdu.getFcv() == 0)
			// fill in user data for application layer use
			rxLpdu.getUserData(userData);
		    else
			stats.increment( RX_FCV_INCORRECT);

		    break;

		case REQUEST_LINK_STATUS:

		    stats.increment( RX_REQUEST_LINK_STATUS);

		    if ( rxLpdu.getFcv() == 0)
		    {
			txLpdu.build( isMaster, 0, 0, 0, LINK_STATUS,
				      rxLpdu.getSrc(), addr);
			stats.increment( TX_FRAMES);
                        stats.increment( TX_LINK_STATUS);
			tx_p->transmit( txLpdu);

		    }
		    else
			stats.increment( RX_FCV_INCORRECT);

		    break;
                        
		default:
		    // invalid function for a response
		    stats.increment( RX_FN_CODE_ERROR);

	    } // end switch func

	} // end PRM = 1

    } // end if (addr == dl->addr || broadcast)
    else  
    {
        // this LPDU is not for us
	stats.increment( RX_OTHER_ADDRESSES);
    }
}


void Datalink::ack( DnpAddr_t dest)
{
    txLpdu.build( isMaster, 0, 0, 0, ACK, dest, addr);
    stats.increment(TX_ACKS);
    tx_p->transmit( txLpdu);
}

void Datalink::nack( DnpAddr_t dest)
{
    txLpdu.build( isMaster, 0, 0, 0, NACK, dest, addr);
    stats.increment(TX_NACKS);
    tx_p->transmit( txLpdu);
}
        
void Datalink::sendRequestLinkStatus( DnpAddr_t dest)
{
    txLpdu.build( isMaster, 0, 0, 0, NACK, dest, addr);
    stats.increment(TX_REQUEST_LINK_STATUS);
    tx_p->transmit( txLpdu);
}

#if 0
//     def run(self):

//         lpdu = LinkProtocolDataUnit()  # we will use this class to create LPDUs
//         linkNeedsToBeReset = 1 # reset the link before we initiate any comms
//         pendingItem = None
//         state = e.IDLE
//         stats = self.stats
//         s = self.getName() + " Thread started, PID=" + str(getpid())
//         msglog.log("Dnp", msglog.types.INFO, s)
        
//         while 1:

//             if state == e.IDLE:
//                 if pendingItem == None:
//                     item = None
//                 else:
//                     item = pendingItem
//                     pendingItem = None
//             else:
//                 item = None

/* maybe move this entire function to the master app? */ 
void
timeout( DatalinkInfo_t *dl)
{
    assert (dl->tx.tcp);/*the only reason for this function is for keepalives*/
    if ( tx_connectionIsUp( dl->remoteIp))
    {
	if (dl->state == WAITING_FOR_LINK_STATUS)
	{
	    tx_closeConnection( dl->remoteIp);
	    dl->state = IDLE;
	}
	else
	{
	    /* we have not received anything for a while */
	    dl_sendRequestLinkStatus( dl->dest);
	    dl->state = WAITING_FOR_LINK_STATUS;
	    /* need to hear back soon */
	    dl->timeout = dl->responseTimeout;
	}
    }
    else
    {
	dl->state = IDLE; 
	dl->timeout = dl->keepAliveInterval;
    }
}


#endif
