
// first serial for modbus master, second/third for monitor
// to have easier debug, it is possible to connect to the same "one" sofware also in serial...
#define IDX_SERIAL_MODBUS 0
#define IDX_SERIAL_MON_SLAVE 1
#define IDX_SERIAL_MON_MASTER 2
#define NBR_SERIAL_PORTS 3

#define SERIAL_PARITY_NONE 0
#define SERIAL_PARITY_ODD 1
#define SERIAL_PARITY_EVEN 2

void SerialGeneralInit( void );
char SerialOpen( int PortIndex, const char * SerialPortName, int SerialSpeed, int DataBits, int Parity, int StopBits );
void SerialClose( int PortIndex );
char SerialPortIsOpened( int PortIndex );
void SerialSetRTS( int PortIndex, int State );
void SerialSend( int PortIndex, char * Buff, int BuffLength );
void SerialSetResponseSize( int PortIndex, int Size, int TimeOutResp );
int SerialReceive( int PortIndex, char * Buff, int MaxBuffLength );
void SerialPurge( int PortIndex );
void SerialConfigDebugAndRts( int PortIndex, int DbgLvl, char RtsToUse );
