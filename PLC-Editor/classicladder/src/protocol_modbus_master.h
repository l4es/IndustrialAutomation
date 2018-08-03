
#ifndef _PROTOCOL_MODBUS_MASTER_H
#define _PROTOCOL_MODBUS_MASTER_H

/* Modbus requests list available for the user */
#define MODBUS_REQ_INPUTS_READ 0
#define MODBUS_REQ_COILS_WRITE 1
#define MODBUS_REQ_INPUT_REGS_READ 2
#define MODBUS_REQ_HOLD_REGS_WRITE 3
#define MODBUS_REQ_COILS_READ 4
#define MODBUS_REQ_HOLD_REGS_READ 5
#define MODBUS_REQ_READ_STATUS 6
#define MODBUS_REQ_DIAGNOSTICS 7

#define LGT_SLAVE_ADR 25
#define LGT_SLAVE_NAME 40

#define NBR_MODBUS_MASTER_REQ 20 /*50: problem with GTK config window: adding vertical scroll else required*/
#define NBR_MODBUS_SLAVES 10

typedef struct StrModbusSlave
{
	/* String with "IP address" or "IP:port" or "slave number" for serial mode */
	/* if '\0' => req not defined */
	char SlaveAdr[ LGT_SLAVE_ADR ];
	int SlavePortIP; // extracted from SlaveAdr string before (stored here for UDP use later...)
	/* For IP slaves, UDP is a nice interesting option (no cnx required before ask!) */
	char UseUdpInsteadOfTcp;
	char NameInfo[ LGT_SLAVE_NAME ];
	/* Statistics on that slave */
	int StatsNbrErrorsNoResponse;
	int StatsNbrErrorsModbusTreat;
	int StatsNbrFrames;
	// times to calc average nbr frames per second...
	int StatsStartTime;
	int StatsCurrentTime;
}StrModbusSlave;

typedef struct StrModbusMasterReq
{
	int SlaveListNum;
	char TypeReq; /* see MODBUS_REQ_ list */
	int FirstModbusElement;
	int NbrModbusElements;
	char LogicInverted;
	int OffsetVarMapped;
}StrModbusMasterReq;

typedef struct StrModbusConfig
{
	// if '\0' => IP mode used for I/O modbus modules
	char ModbusSerialPortNameUsed[ 30 ];
	int ModbusSerialSpeed;
	int ModbusSerialDataBits;   // Number of data bits (5, 6, 7, 8)
	int ModbusSerialParity;     // Parity (00 = None, 01 = Odd, 02 = Even, 03 = Mark, 04 = Space)
	int ModbusSerialStopBits;   // Number of stop bits (1 or 2)
	int ModbusSerialUseRtsToSend;
	int ModbusTimeInterFrame;
	int ModbusTimeOutReceipt;
	int ModbusTimeAfterTransmit;
	// classic modbus offset (0 in frames = 1 in doc, or 0 everywhere: often too much simple to be used...)
	int ModbusEleOffset; 
	int ModbusDebugLevel;
	// types of vars to map for each modbus request
	int MapTypeForReadInputs;
	int MapTypeForReadCoils;
	int MapTypeForWriteCoils;
	int MapTypeForReadInputRegs;
	int MapTypeForReadHoldRegs;
	int MapTypeForWriteHoldRegs;
}StrModbusConfig;

void InitModbusMasterBeforeReadConf( void );
void InitModbusMasterParams( void );
void InitStatsForSlave( int Slave );
int GetModbusResponseLenghtToReceive( void );
unsigned short CRC16(unsigned char *puchMsg, unsigned short usDataLen);
int ModbusMasterAsk( int * pCurrentSlaveIndex, unsigned char * Question );
char TreatModbusMasterResponse( unsigned char * Response, int LgtResponse );

void SetVarFromModbus( StrModbusMasterReq * ModbusReq, int ModbusNum, int Value );
int GetVarForModbus( StrModbusMasterReq * ModbusReq, int ModbusNum );
void SetErrorVarModbusSlave( int iSlaveNbr, int iErrorValue );
int GetErrorVarModbusSlave( int iSlaveNbr );

int FindNextSlaveForStats( char Start );
int GetCurrentNumSlaveForStats( void );
int GetNbrTotalSlaveForStats( void );
#endif

