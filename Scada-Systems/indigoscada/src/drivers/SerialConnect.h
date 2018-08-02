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

#ifndef SERIAL_CONNECT_H
#define SERIAL_CONNECT_H
#include "common.h"
#include "IndentedTrace.h"

// Description:
// This is a general Serial interface class
// Under Unix we just open a serial port put it into raw mode
// and mark it as No Delay
// Under Windows 16bit we use the OpenComm, ReadComm, WriteComm, CloseComm and DCB stuff
// Under Windows 32 bit we  use Createfile, ReadFile WriteFile  and DeviceIoControl
//
// *********************************
//
// are we Unix
#ifdef UNIX
#include <termio.h>
#endif

#define SOCKET_BUF_SIZE (8*1024)
#define MAX_OUTQUEUE_SIZE SOCKET_BUF_SIZE
#define MAX_INQUEUE_SIZE SOCKET_BUF_SIZE
#define WAIT_FOREVER                 ((time_t)-1)
//
// we have to unify the constants used 
// Windows uses one set Unix another
//
enum
{
	#ifdef UNIX	 
	SC_50 = 1,
	SC_75,
	SC_110 ,
	#endif
	#ifdef WIN32
	SC_110 = 1,
	#endif
	SC_300,
	SC_600,
	SC_1200,
	SC_2400,
	SC_4800,
	SC_9600,
	SC_19200,
	SC_38400,
	SC_57600,
	SC_115200
};
//
// other constants set to useful values
//
#define SC_5DATA 5
#define SC_6DATA 6
#define SC_8DATA 8
#define SC_7DATA 7
#define SC_NOPARITY 'N'
#define SC_EVENPARITY 'E'
#define SC_ODDPARITY 'O'
#define SC_MARK 'M'
#define SC_SPACE 'S'
#define SC_1STOP 1
#define SC_2STOP 2
#define SC_FLOW_XON_XOFF 1
#define SC_FLOW_HARD 2
#define SC_FLOW_NONE 3
//
#ifdef WIN32
#define SC_INVALID_FD (INVALID_HANDLE_VALUE)
#else
#define SC_INVALID_FD (-1)
#endif
//
//
// error codes
//
enum
{
	SC_SUCCESS = 0,
	SC_INVALID_BAUDRATE, // not a valid baudrate
	SC_INVALID_DATASIZE, // not a valid data size
	SC_INVALID_PARITY, // not a valid parity
	SC_INVALID_STOPBITS, // not a valid number of stop bits
	SC_CANNOT_OPEN, // cannot open the port
	SC_CANNOT_CONFIGURE, // cannot configure the port
	SC_INVALID_FLOW,  // invalid flow control option
	SC_FD_INVALID  // the FD is not valid
};

// number of supported serial ports
#define QS_N_SERIAL_PORTS 8

class QSEXPORT SerialConnect : public QObject// we assume the user task/thread locks before use
{
	Q_OBJECT

	enum { ss_open, ss_shutdown, ss_close } state;
	enum error_codes 
	{
		ok = 0,
		not_opened = -1,
		broken_pipe = -2,
		timeout_expired = -3
    };

	enum socket_signals 
	{
        RD,  // receive data
        TD,  // transfer data
		RE	 // receive event
    };
 
	BYTE* RXBuff;
	BYTE* cAnswerBuff;
	unsigned int cAnswerBuff_size;
	
	#ifdef WIN32
    HANDLE      Signal[3];
    #endif
	int Error;
	bool fFail;
	///
	#ifdef UNIX
	int fd; // serial port fd
	#endif
	// Under Windows a DCB is used to hold the COM port configuratiom
	#ifdef WIN32
	DCB Config;
	HANDLE fd; // we have HANDLE for 32
	#endif
	//
	#ifdef UNIX
	// Linux termio configuration
	struct termios Config;
	#endif
	QString Name;
	bool fLocked; // true if this port is locked		
	QMutex mLock; // we lock this port for use as and when
	public:
	//
	// make life easy store the last settings publicly
	int _baudrate; // baud rate index
	int _data;
	int _stop;
	int _parity;
	int _flow;
	//
	// configure from given values
	// 
	#ifdef WIN32
	int Configure(int baud, int data = SC_8DATA,int stop = SC_1STOP, int parity = SC_NOPARITY, int flow = SC_FLOW_NONE,
								 DWORD ReadIntervalTimeout = MAXDWORD, 
								 DWORD ReadTotalTimeoutMultiplier = 0,
								 DWORD ReadTotalTimeoutConstant = 40);
	#else
	int Configure(int baud, int data = SC_8DATA,int stop = SC_1STOP, int parity = SC_NOPARITY, int flow = SC_FLOW_NONE);
	#endif
	//
	int SetBaudRate(int baud); // set the baud rate (baud = index not actual rate)
	void FlushTransmit(); // flush the Tx buffer
	void FlushReceive(); // flush the receive buffer
	void FlowStart(); // enable receive
	void FlowStop(); // Disable Receive
	//
	static SerialConnect Ports[QS_N_SERIAL_PORTS]; // all the ports are created closed
	//
	static SerialConnect * GetSerialPort(const QString &s); // find an open serial port
	//
	SerialConnect(const char *pName = 0); // construct with an optional mode string
	~SerialConnect();
	//
	int Open(const char *pName);  // open a connection using a mode string
	void Close(); // close a connection
	//
	int Read(BYTE **buf, time_t timeout = WAIT_FOREVER); // read from serial port 
	int Write(void *buf,int len = 1);// write to a serial port

	#ifdef WIN32
	int read(BYTE** buf, size_t* min_size,time_t timeout = WAIT_FOREVER);
	bool write(const void* buf, size_t size);
	//
	enum
	{
		dtr_handshake		=  0x01,
		rts_handshake		=  0x02,
		cts_monitor			=  0x04,
		dsr_monitor			=  0x08,
		software			=  0x10	
	};

	bool setHandshake (const int);
	//
	void Shutdown();
	//
	#endif

	const QString &GetName() { return Name;}; // get the object's name
	//
	int GetChar() // get the next character
	{
		IT_IT("SerialConnect::GetChar");

		char* c;
		if(Read((BYTE **)&c) == 1)
		{ 
			return *c;
		};
		return -1;          
	};
	void PutChar(int c) // write a character
	{
		IT_IT("SerialConnect::PutChar");

		Write(&c,1);
	};
	bool Lock()  // lock the serial port 
	{
		IT_IT("SerialConnect::Lock");
		bool ret = false;
		mLock.lock(); 
		if(!fLocked )
		{
			fLocked = true; 
			ret = true;
		};
		mLock.unlock(); 
		return ret;
	}; // claim the port
	void Unlock() 
	{
		IT_IT("SerialConnect::Unlock");
		mLock.lock(); 
		fLocked = false; 
		mLock.unlock();
	}; // release the port
	//
	static QString TranslateSerialName(const QString &s); // convert from Unix to Windows name - very platform dependent
	//
	bool Ok();
};

#endif	//SERIAL_CONNECT_H