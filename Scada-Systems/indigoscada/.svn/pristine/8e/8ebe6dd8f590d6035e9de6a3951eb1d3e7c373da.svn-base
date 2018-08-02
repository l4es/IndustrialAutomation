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

//general_defines.h

#ifndef GENERAL_DEFINES
#define GENERAL_DEFINES

///////////////////////////////////////////////////////////
#define MONITOR_CAN_RUN_AS_ROOT

#define USE_STD_MAP //if we use the STL std::map 
#define NO_LOGIN //uncomment for development

///////////HMI appearence//////////////

//#define HMI_BACKGROUND_COLOR QColor(102, 139, 153)
//#define HMI_BACKGROUND_COLOR QColor(255, 255, 255)
//#define HMI_BACKGROUND_COLOR QColor(0, 0, 0)

///////////DATABASES///////////////////////////////////

//Important parameters of the client side of GigaBASE and FastDB
#define MAX_LENGHT_OF_STRING 2500 //bytes, max lenght of field of type string 
//#define MAX_LENGHT_OF_QUERY 4500 //bytes, max lenght of query string
#define MAX_LENGHT_OF_QUERY 50000 //bytes, max lenght of query string
#define MAX_NUMBER_OF_RECORD_FETCHED 500 //max # of initial records fetched by this client
#define COMMIT_FREQ 500 //After COMMIT_FREQ precommits the client commits the transactions
#define NMAX_FIELDS_IN_TABLE 256
///////////////////////////////////////////////////////////////////////////////////////////////////////
//Dispatcher Sono ammessi un massimo numero di tags per punto di lettura:
#define MAX_NUM_OF_TAGS_FOR_SAMPLE_POINT 10

////////////////////general system names/////////////
//Suppliers company names

//Supplier
#define SUPPLIER_NAME_SHORT ""
#define SUPPLIER_NAME_LONG ""
#define LOGO_SUPPLIER QPixmap((const char **)logo_blank_30)

//IndigoSCADA
#define SYSTEM_NAME_HISTOR_SERVER "IndigoSCADA.historical.server" //SCADA historical data server
#define SYSTEM_NAME_RTSERVER "IndigoSCADA.real_time.server" //SSCADA data real time server
#define SYSTEM_NAME_MONITOR "IndigoSCADA.front_end" //SCADA front end
#define SYSTEM_NAME_DISPATCHER "IndigoSCADA.dispatcher.server" //SCADA data real time server
#define SYSTEM_NAME_MANAGER "IndigoSCADA.manager" //manager of the SCADA processes
#define SYSTEM_NAME_ARCHIVER "IndigoSCADA.archiver" //SCADA archiver
#define SYSTEM_NAME_HMI	"HMInterface-SCADA" //SCADA interface
#define SYSTEM_NAME "IndigoSCADA"
#define SYSTEM_NAME_SYSTEM_ADMINISTRATOR "IndigoSCADA - System Administrator"
#define SYSTEM_NAME_OPERATOR "IndigoSCADA - Operator"
#define SYSTEM_NAME_STARTING "IndigoSCADA Starting"

//IndigoSCADA software demo
#define CUSTOMER_NAME_SHORT ""
#define CUSTOMER_NAME_LONG ""
#define LOGO_CUSTOMER QPixmap((const char **)logo_blank_30)

//////////////////////////////////////////////////////////////////
//start tests section 
//////////////////////////////////////////////////////////////////

#endif//GENERAL_DEFINES



