/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2012 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#ifndef RFC_1006_IMP_H
#define RFC_1006_IMP_H

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif
#include <stdio.h>
#include <time.h>
#include "itrace.h"

////////////////////////////Middleware///////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

class rfc1006_imp
{
	public:
	char plc_server_prog_id[100];
	int g_dwNumItems;
	struct rfc1006Item* Config_db;
	/////////////////////Middleware/////////////////////////
	///////////////////////////////////Middleware///////////
	bool fExit;
	int  g_dwUpdateRate;
	double dead_band_percent;
	char		ServerIPAddress[80];
	char		ServerPort[80];
	int local_server;
	double		pollingTime;
	
	rfc1006_imp(char* rfc1006server_address, char*rfc1006server_port, char* line_number, int polling_time);
	~rfc1006_imp();
	int AddItems(void);
	void CreateSqlConfigurationFile(char* sql_file_name, char* opc_path);
	int Async2Update();
	int check_connection_to_server(void);
	int RfcStart(char* RfcServerProgID, char* RfcclassId, char* RfcUpdateRate, char* RfcPercentDeadband);
	void LogMessage(int* error = 0, const char* name = NULL);
	int RfcStop();
	int GetStatus(WORD *pwMav, WORD *pwMiv, WORD *pwB, LPWSTR *pszV);
	time_t epoch_from_cp56time2a(const struct cp56time2a* time);
	void epoch_to_cp56time2a(cp56time2a *time, signed __int64 epoch_in_millisec);
	void SendEvent2(void);
	signed __int64 epoch_from_FILETIME(const FILETIME *fileTime);
	short rescale_value(double V, double Vmin, double Vmax, int* error);
	double rescale_value_inv(double A, double Vmin, double Vmax, int* error);
	
	////////////////////Middleware//////////////////////
	void check_for_commands(struct iec_item *item);
	void alloc_command_resources(void);
	void free_command_resources(void);
	void get_utc_host_time(struct cp56time2a* time);
	////////////////////////////////////////////////
};

#endif //RFC_1006_IMP_H