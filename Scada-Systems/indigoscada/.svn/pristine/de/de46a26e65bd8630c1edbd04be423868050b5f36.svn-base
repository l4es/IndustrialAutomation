/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "custom.hpp"
#include "station.hpp"
#include "outstation.hpp"
#include "datalink.hpp"

int main( int argc, char **argv )
{
	int socket = 0;
    Outstation* o_p;
    CustomDb db;
    int debugLevel = -1;
    CustomInter masterTx(&debugLevel, 'M', 'S', socket);
    CustomInter outstationTx(&debugLevel, 'O', 'S', socket);
    CustomTimer masterTimer;
    CustomTimer outstationTimer;
    int integrityPollInterval = 10;

    Datalink::DatalinkConfig      datalinkConfig;
    Station::StationConfig        stationConfig;

    stationConfig.addr = 2;
    stationConfig.debugLevel_p = &debugLevel;

    Outstation::OutstationConfig outstationConfig;
    outstationConfig.addr           = stationConfig.addr;
    outstationConfig.masterAddr     = 1;
    outstationConfig.userNum        = 5;
    outstationConfig.debugLevel_p   = &debugLevel;

    datalinkConfig.addr             = stationConfig.addr;
    datalinkConfig.isMaster         = 0;
    datalinkConfig.tx_p             = &outstationTx;

    o_p = new Outstation( outstationConfig, datalinkConfig, &db,
			  &outstationTimer);
	return 0;
}
