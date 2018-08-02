/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2014 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#ifndef MODBUS_IMP_H
#define MODBUS_IMP_H

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif
#include <stdio.h>
#include <time.h>
#include "itrace.h"
#include <modbus.h>

#include "iec_item_type.h" //Middleware
////////////////////////////Middleware///////////////////////////////////////////////////////
extern void onRegFail(void *param);
extern void recvCallBack(const ORTERecvInfo *info,void *vinstance, void *recvCallBackParam); 
/////////////////////////////////////////////////////////////////////////////////////////////

struct modbusContext
{
	int use_context;
	//////////////////MODBUS TCP context///////////////////////
	char modbus_server_address[40];
	char modbus_server_port[40];
	//////////////////MODBUS RTU context///////////////////////
	//The device argument specifies the name of the serial port handled by the OS,
	//eg. '/dev/ttyS0'. On Windows, it's necessary to prepend COM
	//name with '\\.\' for COM number greater than 9, eg. '\\\\.\\COM10'. See
	//http://msdn.microsoft.com/en-us/library/aa365247(v=vs.85).aspx for details

	char serial_device[40];
	/* Bauds: 9600, 19200, 57600, 115200, etc */
    int baud;
    /* Data bit, eg. 8 */
    uint8_t data_bit;
    /* Stop bit, eg. 1 */
    uint8_t stop_bit;
    /* Parity: 'N', 'O', 'E' */
    char parity;
};

enum {
    TCP,
    TCP_PI,
    RTU
};

/* Modbus function codes */
#define FC_READ_COILS                0x01
#define FC_READ_DISCRETE_INPUTS      0x02
#define FC_READ_HOLDING_REGISTERS    0x03
#define FC_READ_INPUT_REGISTERS      0x04
#define FC_WRITE_SINGLE_COIL         0x05
#define FC_WRITE_SINGLE_REGISTER     0x06
#define FC_READ_EXCEPTION_STATUS     0x07
#define FC_WRITE_MULTIPLE_COILS      0x0F
#define FC_WRITE_MULTIPLE_REGISTERS  0x10
#define FC_REPORT_SLAVE_ID           0x11
#define FC_WRITE_AND_READ_REGISTERS  0x17


class modbus_imp
{
	public:
	///////////////////configuration database//////////////////////
	char database_name[MAX_PATH];
	int db_n_rows;
	int db_m_columns;
	/////////////////////////////////////////////////
	int g_dwNumItems;
	struct modbusDbRecord* Config_db;
	/////////////////////Middleware/////////////////////////
	ORTEDomain              *domain;
	ORTEPublication			*publisher;
	ORTESubscription        *subscriber;
	iec_item_type			instanceSend;
	iec_item_type		    instanceRecv;
	//////////////////////////////end//Middleware///////////

	/////////////Middleware///////////////////////////////
	u_int n_msg_sent_monitor_dir;
	u_int n_msg_sent_control_dir;
	//////////////////////////////////////////////////////
	bool fExit;
	unsigned long pollingTime;
	struct modbusContext my_modbus_context;
	bool general_interrogation;
	int loops;
    int comm_error_counter;
	bool is_connected;
	////////////////Modbus specific/////////////////
	modbus_t *ctx; //context
	uint8_t *tab_rp_bits;
    uint16_t *tab_rp_registers;
	uint16_t *stored_tab_rp_registers;
	int nb_points;
	////////////////////////////////////////////////
	int lineNumber;
	
	modbus_imp(struct modbusContext* my_ctx, char* line_number, int polling_time);
	~modbus_imp();
	int AddItems(void);
	int PollServer(void);
	int Start(void);
	void LogMessage(int* error = 0, const char* name = NULL);
	int Stop(void);
	time_t epoch_from_cp56time2a(const struct cp56time2a* time);
	int PollItems(void);
	short rescale_value(double V, double Vmin, double Vmax, int* error);
	double rescale_value_inv(double A, double Vmin, double Vmax, int* error);
	
	////////////////////Middleware//////////////////////
	void check_for_commands(struct iec_item *item);
	void get_utc_host_time(struct cp56time2a* time);
	void get_local_host_time(struct cp56time2a* time);
	////////////////////////////////////////////////
};

#endif //MODBUS_IMP_H