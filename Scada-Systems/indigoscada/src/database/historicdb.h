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

/*
*Header For: global databases
*Purpose:
*/

#ifndef include_historic_db_hpp
#define include_historic_db_hpp

#ifdef USING_GARRET
// we are using the GigaBASE SQL server and FastDB SQL server
// this runs on Win32 and Linux and other Unices
#include "qsgigabase.h"
#define HISTORIC_DB QSGigabase
#endif
//
//
// open the databases
QSEXPORT bool OpenHistoricConnections();

QSEXPORT bool ConnectToHistoricDatabases();
// closethem
QSEXPORT void CloseHistoricConnections();
QSEXPORT void DisconnectFromHistoricDatabases();		
//
//
typedef std::map<QString, HISTORIC_DB*, std::less<QString> > HistoricDbDict; // the database dictionary
//
QSEXPORT QSDatabase *GetHistoricResultDb ();
QSEXPORT HistoricDbDict& GetHistoricDbDict();

#endif

