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

#include "SerialConnect.h"
#include <time.h>

/*
*Function:SerialConnect::GetSerialPort
*Inputs:COM port name COM1 - COM8
*Outputs:none
*Returns:serial port interface or 0 on failure
*/
SerialConnect SerialConnect::Ports[QS_N_SERIAL_PORTS];  // serial ports - we allow upto 8
// these are opened and configured on startup by the monitoring program
// 
SerialConnect * SerialConnect::GetSerialPort(const QString &s)
{
	IT_IT("SerialConnect::GetSerialPort");
	
	for(int i = 0; i < QS_N_SERIAL_PORTS; i++)
	{   
		if(s == SerialConnect::Ports[i].GetName())
		{
			return &Ports[i]; // return the serial port
		}
	};
	return 0; // found nothing
};
//
/*
*Function: TranslateSerialName
*Inputs:COMn name
*Outputs:none
*Returns:unix name
*/
QString SerialConnect::TranslateSerialName(const QString &s) // translate the serial port's name
{
	IT_IT("SerialConnect::TranslateSerialName");

	#ifdef UNIX
	// we expect COM1 - COM9
	QString r = s.right(1); // extract the number
	// COM1 = /dev/ttyS0 so 1 -> 0
	r.sprintf("/dev/ttyS%d",r.toInt() - 1);
	return r;
	#else
	return s;
	#endif
};
//
// Function: SerialConnect
// Purpose:To construct a SerialCOnnect object
// Inputs: name of port to open
// Outputs: none
// Returns: none
//
//
SerialConnect::SerialConnect(const char *pName) // construct with an optional mode string
{
	IT_IT("SerialConnect::SerialConnect");
	
	fd = SC_INVALID_FD;
	fLocked = 0;
	state = ss_close;
	Error = 0;
	fFail = false;
	memset(&Config,0,sizeof(Config)); // initialise the config structure
	#ifdef WIN32
	Config.DCBlength = sizeof(Config);
	#endif
	if(pName) // if none null name open the port
	{
		Open(pName);
	};

	#ifdef WIN32
	for (int i = RD; i <= RE; i++) 
	{
		Signal[i] = 0;
	}
	#endif

	RXBuff = 0;
	cAnswerBuff = 0;
	cAnswerBuff_size = 5000;
};
//
// Function: ~SerialConnect
// Purpose: To destroy a serial conenction
// Inputs: none
// Outputs: none
// ReturnsL: none
//
//
SerialConnect::~SerialConnect()
{
	IT_IT("SerialConnect::~SerialConnect");
};
//
// baud rate constant translation tables
#ifdef WIN32
static int TransBaud[] =
{
	9600,
	CBR_110,
	CBR_300,
	CBR_600,
	CBR_1200,
	CBR_2400,
	CBR_4800,
	CBR_9600,
	CBR_19200,
	CBR_38400,
	CBR_56000,
	CBR_128000
};
#endif
#ifdef UNIX
static int TransBaud[] =
{
	B0,
	B50,
	B75,
	B110 ,
	B300,
	B600,
	B1200,
	B2400,
	B4800,
	B9600,
	B19200,
	B38400,
	B57600,
	B115200
};
#endif
//
// Function: Configure
// Purpose:To configure a serial port
// Inputs: baudrate, data size, parity , stop bits and flowe control
// Outputs: none
// Returns: R_SUCCESS if completed successfully
//
//
#ifdef WIN32
int SerialConnect::Configure(int baud, int data, int stop, int parity, int flow,
							 DWORD ReadIntervalTimeout, 
							 DWORD ReadTotalTimeoutMultiplier,
							 DWORD ReadTotalTimeoutConstant)
#else
int SerialConnect::Configure(int baud, int data, int stop, int parity, int flow)
#endif
{
	IT_IT("SerialConnect::Configure");

	int status = false;
	if(fd != SC_INVALID_FD)
	{
		int ok = 1; // if ok is zeo we have an error
		// Validate baudrate
		switch(baud)
		{
			#ifdef UNIX
			case SC_50:
			case SC_75:
			#endif
			case SC_110:
			case SC_300:
			case SC_600:
			case SC_1200:
			case SC_2400:
			case SC_4800:
			case SC_9600:
			case SC_19200:
			case SC_38400:
			case SC_57600:
			case SC_115200:
			break;
			default:
			ok = 0;
			break;
		};
		if(ok)
		{
			switch(data)
			{
				case SC_5DATA:
				case SC_6DATA:
				case SC_8DATA:
				case SC_7DATA:
				break;
				default:
				ok = 0;
				break;
			};
		};
		if(ok)
		{
			switch(parity)
			{
				case SC_NOPARITY:
				case SC_EVENPARITY:
				case SC_ODDPARITY:
				break;
				default:
				ok = 0;
				break;
			};
		};
		if(ok)
		{
			switch(stop)
			{
				case SC_1STOP:
				case SC_2STOP:
				break;
				default:
				ok = 0;
				break;
			};
		};
		if(ok)
		{
			switch(flow)
			{
				case SC_FLOW_XON_XOFF:
				case SC_FLOW_HARD:
				case SC_FLOW_NONE:
				break;
				default:
				ok = 0;
				break;
			};
		};
		if(ok)
		{
			_baudrate = baud;
			_data = data;
			_stop = stop;
			_parity = parity;
			_flow = flow;
			#ifdef WIN32
			Config.DCBlength = sizeof(Config);
			//BuildCommDCB("COM1: baud=9600 parity=N data=8 stop=1",&Config);
			
			// set the baud rate index
			Config.BaudRate = TransBaud[baud];
			//

			switch(data)
			{
				case SC_5DATA:
					Config.ByteSize = 5;
				break;
				case SC_6DATA:
					Config.ByteSize = 6;
				break;
				case SC_7DATA:
					Config.ByteSize = 7;
				break;
				case SC_8DATA:
					Config.ByteSize = 8;
				break;
				default:
				break;
			};

			switch(parity)
			{
				case SC_NOPARITY:
				Config.Parity = NOPARITY;
				break;
				case SC_EVENPARITY:
				Config.Parity = EVENPARITY;
				break;
				case SC_ODDPARITY:
				Config.Parity = ODDPARITY;
				break;
				default:
				break;
			};
			switch(stop)
			{
				case SC_1STOP:
				Config.StopBits = ONESTOPBIT;
				break;
				case SC_2STOP:
				Config.StopBits = TWOSTOPBITS;
				break;
				default:
				break;
			};

			// set the flow control
			switch(flow)
			{
				case SC_FLOW_XON_XOFF:
				Config.fDtrControl  =  DTR_CONTROL_DISABLE;
				Config.fRtsControl = RTS_CONTROL_DISABLE;
				Config.fOutxCtsFlow = Config.fOutxDsrFlow = FALSE;
				Config.fOutX = TRUE;
				Config.fInX  = TRUE;
				break;
				case SC_FLOW_HARD:
				Config.fDtrControl  =  DTR_CONTROL_ENABLE;
				Config.fRtsControl = RTS_CONTROL_ENABLE;
				Config.fOutxCtsFlow = Config.fOutxDsrFlow = TRUE;
				Config.fOutX = FALSE;
				Config.fInX  = FALSE;
				break;
				case SC_FLOW_NONE:
				Config.fDtrControl  =  DTR_CONTROL_DISABLE;
				Config.fRtsControl = RTS_CONTROL_DISABLE;
				Config.fOutxCtsFlow = Config.fOutxDsrFlow = FALSE;
				Config.fOutX = FALSE;
				Config.fInX  = FALSE;
				break;
				default:
				break;
			};
			// set the configuration
			status = SetCommState(fd,&Config);
			status = status & SetCommMask(fd,EV_RXCHAR);
			
			//non-blocking i/o

			COMMTIMEOUTS ct;
			memset(&ct,0,sizeof(ct)); 
			ct.ReadIntervalTimeout = ReadIntervalTimeout;
			ct.ReadTotalTimeoutMultiplier = ReadTotalTimeoutMultiplier;
			ct.ReadTotalTimeoutConstant = ReadTotalTimeoutConstant;

			status = status & SetCommTimeouts(fd,&ct);

			#endif
			//
			//
			// ******************************
			//
			#ifdef UNIX
			//
			// start by clearing c_cflag
			//
			Config.c_cflag = CLOCAL | CREAD ;
			Config.c_iflag = IGNBRK;
			Config.c_oflag = 0; // no processing
			Config.c_lflag = 0; // default line status
			Config.c_line = N_TTY;
			//
			switch(data)
			{
				case SC_5DATA:
				Config.c_cflag |= CS5;
				break;
				case SC_6DATA:
				Config.c_cflag |= CS6;
				break;
				case SC_8DATA:
				Config.c_cflag |= CS8;
				break;
				case SC_7DATA:
				Config.c_cflag |= CS7;
				break;
				default:
				break;
			};
			switch(parity)
			{
				case SC_NOPARITY:
				break;
				case SC_EVENPARITY:
				Config.c_cflag |= PARENB;
				break;
				case SC_ODDPARITY:
				Config.c_cflag |= (PARENB | PARODD);
				break;
				default:
				break;
			};
			switch(stop)
			{
				case SC_1STOP:
				break;
				case SC_2STOP:
				Config.c_cflag |= CSTOPB;
				break;
				default:
				break;
			};
			switch(flow)
			{
				case SC_FLOW_XON_XOFF:
				Config.c_iflag |= IXON | IXOFF;
				break;
				case SC_FLOW_HARD:
				Config.c_cflag |= CRTSCTS;
				break;
				case SC_FLOW_NONE:
				break;
				default:
				break;
			};
			//
			// configure the port
			//
			cfsetospeed(&Config,TransBaud[baud]); // set the baud rate (output)
			cfsetispeed(&Config,TransBaud[baud]); // set the baud rate (input)
			if(!tcsetattr(fd,TCSANOW,&Config))
			{
				tcflow(fd,TCOON); // enable output
				FlowStart();
				status =true;
			};
			#endif
		};
	};
	return status;
};
//
// Function:SetBaudRate
// Purpose:To set the baud rate of a serial port
// Inputs: baud rate index (use SC_xxx constants)
// Outputs: none
// Returns: R_SUCCESS on success
//
//
int SerialConnect::SetBaudRate(int baud)
{
	IT_IT("SerialConnect::SetBaudRate");
	
	int status = false;
	if(fd != SC_INVALID_FD)
	{
		int ok = 1;
		switch(baud)
		{
			case 300:
			baud = SC_300;
			break;
			case 600:
			baud = SC_600;
			break;
			case 1200:
			baud = SC_1200;
			break;
			case 2400:
			baud = SC_2400;
			break;
			case 4800:
			baud = SC_4800;
			break;
			case 9600:
			baud = SC_9600;
			break;
			case 19200:
			baud = SC_19200;
			break;
			case 38400:
			baud = SC_38400;
			break;
			case 57600:
			baud = SC_57600;
			break;
			case 115200:
			baud = SC_115200;
			break;
			default:
			ok = 0;
			break;
		};
		if(ok)
		{
			_baudrate = baud;
			#ifdef WIN32
			Config.BaudRate = TransBaud[baud];
			
			status = SetCommState(fd,&Config);
			
			#endif
			#ifdef UNIX
			// set the baud rate field and configure the port
			Config.c_cflag &= ~(CBAUD);
			Config.c_cflag |= TransBaud[baud];
			if(!tcsetattr(fd,TCSANOW,&Config))
			{
				status = true;
			};
			#endif
		};
	};
	return status;
}; // set the baud rate
//
// Function:FlushTransmit
// Purpose:To flush the transmit buffer
// Inputs: none
// Outputs: none
// Returns: none
//
//
void SerialConnect::FlushTransmit()
{
	IT_IT("SerialConnect::FlushTransmit");
	
	if(fd != SC_INVALID_FD)
	{
		#ifdef WIN32
		PurgeComm(fd,PURGE_TXCLEAR);
		#endif
		#ifdef UNIX
		tcflush(fd,TCOFLUSH);
		#endif
	};
};
//
// Function: FlushReceive
// Purpose:To flush the receive buffer
// Inputs: none
// Outputs: none
// Returns: R_SUCCESS
//
//
void SerialConnect::FlushReceive()
{
	IT_IT("SerialConnect::FlushReceive");
	
	if(fd != SC_INVALID_FD)
	{
		#ifdef WIN32
		PurgeComm(fd,PURGE_RXCLEAR);
		#endif
		//
		//
		#ifdef UNIX
		tcflush(fd,TCIFLUSH);
		#endif
	};
};
//
// Function:Flow Start
// Purpose:To enable input
// Inputs
// Outputs
// Returns
//
//
void  SerialConnect::FlowStart()
{
	IT_IT("SerialConnect::FlowStart");
	
	if(fd != SC_INVALID_FD)
	{
		#ifdef WIN32
		EscapeCommFunction(fd,SETDTR);
		EscapeCommFunction(fd,SETRTS);
		#endif
		#ifdef UNIX
		tcflow(fd,TCION); // enable receive
		#endif
	};
};
//
// Function: FlowStop
// Purpose: To disable input via flow control
// Inputs: none
// Outputs: none
// Returns: R_SUCCESS if successful
//
//
void SerialConnect::FlowStop() // disable transmit and receive
{
	IT_IT("SerialConnect::FlowStop");
	
	if(fd != SC_INVALID_FD)
	{
		#ifdef WIN32
		EscapeCommFunction(fd,CLRDTR);
		EscapeCommFunction(fd,CLRRTS);
		#endif
		#ifdef UNIX
		tcflow(fd,TCIOFF);
		#endif
	};
};
//
// Function: Open
// Purpose:To open a serial port
// Inputs: port name
// Outputs: none
// Returns: R_SUCCESS on success
//
//
//
int SerialConnect::Open(const char *pName)  // open a connection using a mode string
{
	IT_IT("SerialConnect::Open");
	
	Name = QString(pName);
	QString local = TranslateSerialName(Name);

	if(pName)
	{
		#ifdef WIN32

		fd = CreateFile((const char *)local,
		GENERIC_READ | GENERIC_WRITE,
		0,    /* comm devices must be opened w/exclusive-access */
		0, /* no security attrs */
		OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
		FILE_FLAG_OVERLAPPED,    /*overlapped I/O */
		0  
		);
		//
		//
		if(!(fd == SC_INVALID_FD))
		{
			SetupComm(fd,MAX_INQUEUE_SIZE,MAX_OUTQUEUE_SIZE); 

			//non-blocking i/o

			COMMTIMEOUTS ct;
			memset(&ct,0,sizeof(ct)); 
			ct.ReadIntervalTimeout = MAXDWORD;
			ct.ReadTotalTimeoutMultiplier = 0;
			ct.ReadTotalTimeoutConstant = 40;

			SetCommTimeouts(fd,&ct);
			//
			if(!Configure(SC_9600))
			{
				Close();
				return false;
			};
		}
		else
		{
			fd = SC_INVALID_FD;
		};
		#endif
		#ifdef UNIX
		fd = open((const char *)local,O_RDWR | O_NDELAY);
		if(fd > 0)
		{
			tcgetattr(fd,&Config); // get the current attributes
		}
		else
		{
			fd = SC_INVALID_FD;
			return false;
		};
		#endif

		#ifdef WIN32
		int  i;
		for (i = RD; i <= RE; i++) 
		{
			char buf[256];
			sprintf(buf, "%s.%c", (const char *)local, i + '0');
			// Create the event (MANUAL reset, initially NOT signalled state)
			Signal[i] = CreateEventA(NULL, true, false, buf);
			if (GetLastError() == ERROR_ALREADY_EXISTS) 
			{
				WaitForSingleObject(Signal[i], 0);
			}
			if (!Signal[i]) 
			{
				Error = GetLastError();
				while (--i >= 0) 
				{
					CloseHandle(Signal[i]);
				}
				return false;
			}
		}
		#endif

		if(RXBuff)
		{
			free(RXBuff);
			RXBuff = NULL;
		}
		RXBuff = (BYTE*)malloc(MAX_INQUEUE_SIZE); 

		if(cAnswerBuff)
		{
			free(cAnswerBuff);
			cAnswerBuff = NULL;
		}
		cAnswerBuff = (BYTE*)malloc(cAnswerBuff_size);
		memset(cAnswerBuff,0x00,cAnswerBuff_size);

		if (!RXBuff) 
		{
			#ifdef WIN32
			Error = GetLastError();
			for (i = RD; i <= RE; i++) 
			{
				CloseHandle(Signal[i]);
			}
			#endif

			return false;
		}

		Error = ok;
		state = ss_open;
	};
	return true;
};
//
// Function:Close
// Purpose:To close a serial port
// Inputs: none
// Outputs: none
// Returns: Connection::Close
//
//
void SerialConnect::Close() // close a connection
{
	IT_IT("SerialConnect::Close");

	if (state != ss_close) 
	{
		#ifdef WIN32
		SetEvent(Signal[RD]);//Is it good to close the reading in this way?
		SetEvent(Signal[TD]);//is it correct to do stop the WRITING for mission critical applications?
		SetEvent(Signal[RE]);
		#endif
		state = ss_close;
		if(fd != SC_INVALID_FD)
		{
			#ifdef WIN32
			CloseHandle(fd); // Win 32
			#endif
			#ifdef UNIX
			close(fd);
			#endif
		};
		fd = SC_INVALID_FD;
		fLocked = 0;

		#ifdef WIN32
		for (int i = RD; i <= RE; i++) 
		{
			if(Signal[i])
				CloseHandle(Signal[i]);
		}
		#endif

		if(RXBuff)
		{
			free(RXBuff); 
			RXBuff = NULL;
		}
		if(cAnswerBuff)
		{
			free(cAnswerBuff);
			cAnswerBuff = NULL;
		}
	}
};
//
// Function: Read
// Purpose:To read from the serial port
// Inputs: dest buffer, buffer length
// Outputs: data lenght read
// timeout: wait com event timeout under WIN32
// Returns: R_SUCCESS on success
//
//
int SerialConnect::Read(BYTE **pDest, time_t timeout)
{
	IT_IT("SerialConnect::Read");
	
	int nDataRead = 0;

	if(fd != SC_INVALID_FD)
	{
		#ifdef WIN32
		nDataRead = read(pDest, &cAnswerBuff_size, timeout);
		#endif
		//
		//
		#ifdef UNIX
		int n = read(fd,*pDest,cAnswerBuff_size); // the only error we should get is no data
		nDataRead = (n >= 0)?n:0;
		#endif
	};
	return nDataRead;

};
//
// Function: Write
// Purpose:To write to the serial port
// Inputs: source data, data lenth
// Outputs: data length written
// Returns: R_SUCCESS if successful
//
//
int SerialConnect::Write(void *pSrc,int nSrcLen)
{
	IT_IT("SerialConnect::Write");

	int nSrcWrite = 0;
	if(fd != SC_INVALID_FD) 
	{
		#ifdef WIN32
		return write(pSrc, nSrcLen);
		#endif
		#ifdef UNIX
		int n = write(fd,pSrc,nSrcLen);
		nSrcWrite = (n >= 0)?n:0;
		#endif
	};
	return nSrcWrite;

};

#ifdef WIN32
int SerialConnect::read(BYTE** buf, size_t* min_size, time_t timeout)
{
    IT_IT("SerialConnect::read");
		
	memset(cAnswerBuff, 0x00, *min_size);
	*buf = cAnswerBuff;
	size_t size = 0;
    Error = ok;

	DWORD sEvent;

	OVERLAPPED overlappedRead = {0};
	overlappedRead.hEvent = Signal[RD];

	OVERLAPPED overlappedEvent = {0};
	overlappedEvent.hEvent = Signal[RE];

	unsigned long partial_read = 0;
	unsigned int total_read = 0;
	
	bool first_loop = true;
	bool second_loop = true;
		
    while(first_loop && state == ss_open) 
	{
        sEvent = 0;
		ResetEvent(Signal[RE]);

		if(!HasOverlappedIoCompleted(&overlappedEvent)){ fFail = true; return -1;}
		
		if (!WaitCommEvent(fd,&sEvent,&overlappedEvent))
		{
			//overlapped wait com event
			Error = GetLastError();
			if (Error != ERROR_IO_PENDING)
			{			
				fFail = true;
				return -1;
			}

			IT_COMMENT("wait serial com event");
			int rc = WaitForSingleObject(Signal[RE], timeout);

			if (rc != WAIT_OBJECT_0) 
			{
				if(rc == WAIT_TIMEOUT)
				{
					CancelIo(fd);
					IT_COMMENT("wait serial com event timeout");
					fFail = true;
					return -1;
				}
				else
				{
					Error = GetLastError();
					fFail = true;
					return -1;
				}
			}
		}
				
		if (sEvent & EV_RXCHAR)
		{
			while(second_loop)
			{
				ResetEvent(Signal[RD]);
				partial_read = 0;

				if(!HasOverlappedIoCompleted(&overlappedRead)){fFail = true; return -1;}

				if (!ReadFile(fd, RXBuff, MAX_INQUEUE_SIZE, &partial_read, &overlappedRead))
				{
					IT_COMMENT("overlapped i/o read");

					unsigned int LastError = GetLastError();

					if (LastError == ERROR_IO_PENDING 
						|| LastError == ERROR_FILE_NOT_FOUND)
					{															 // ||
						//wait until the Signal[RD] is signaled, because            \/   bWait is set ot TRUE
						if (!GetOverlappedResult(fd,&overlappedRead,&partial_read,TRUE))
						{
							Error = GetLastError();
							IT_COMMENT("ReadFile - overlapped completed without result");
							fFail = true;
							return -1;
						}
					}
					else
					{
						Error = LastError;
						fFail = true;
						return -1;
					}
				}
				else
				{
					IT_COMMENT("not overlapped i/o read");
				}

				IT_COMMENT1("Partial recv = %d", partial_read);
					
				if (partial_read > 0)
				{
					first_loop = false;
					
					//IT_COMMENT1("RXBuff = %s", RXBuff);
					IT_DUMP_STR((const char*)RXBuff, partial_read);
															
					if(total_read + partial_read <= *min_size)
					{
						memcpy(cAnswerBuff + total_read, RXBuff, partial_read);
						total_read += partial_read;
					}
					else
					{
						*min_size *=2;
						BYTE* newdata = (BYTE*)malloc(*min_size);	
						memcpy(newdata, cAnswerBuff, total_read);
						free(cAnswerBuff);
						cAnswerBuff = newdata;
						memcpy(cAnswerBuff + total_read, RXBuff, partial_read);
					}
 				}
				else
				{
					break;
				}

			}//while(second_loop)
		}
		else if(sEvent & EV_RX80FULL)
		{
			fFail = true;
			return -1;
		}
		else if(sEvent & EV_ERR)
		{
			fFail = true;
			return -1;
		}
	}
	
	IT_COMMENT1("Byte(s) reveived = %d", total_read);
	PurgeComm(fd, PURGE_RXCLEAR | PURGE_RXABORT);
	return (int)total_read;
}


bool SerialConnect::write(const void* buf, size_t size)
{
    IT_IT("SerialConnect::write");
	
	if(state != ss_open) { fFail = true; return false;}
	if(size > MAX_OUTQUEUE_SIZE) {fFail = true; return false;}

	BYTE* src = (BYTE*)buf;
	unsigned long written = 0;
    Error = ok;

	PurgeComm(fd, PURGE_TXCLEAR | PURGE_TXABORT);
	
	OVERLAPPED overlappedWrite = {0};
	overlappedWrite.hEvent = Signal[TD];
	
	ResetEvent(Signal[TD]);
	
	if (!WriteFile(fd, src, size, &written, &overlappedWrite))
	{
		//overlapped i/o
		int LastError = GetLastError();
		if (LastError != ERROR_IO_PENDING)
		{
			Error = LastError;
			fFail = true;
			return false;
		}
															 //   ||
		//wait until the Signal[TD] is signaled, because          \/   bWait is set ot TRUE
		if (!GetOverlappedResult(fd, &overlappedWrite, &written, TRUE))
		{
			Error = GetLastError();
			IT_COMMENT("WriteFile - overlapped completed without result");
			fFail = true;
			return false;
		}

		IT_COMMENT1("Byte(s) written: %d", written);
		IT_DUMP_STR((const char*)src, written);
		IT_COMMENT("overlapped i/o write");
		return size == written;
	}
	else
	{
		//not overlapped i/o
		IT_COMMENT1("Byte(s) written: %d", written);
		IT_DUMP_STR((const char*)src, written);
		IT_COMMENT("not overlapped i/o write");
		return size == written;
	}

}

bool SerialConnect::setHandshake (const int handshake)
{
	IT_IT("SerialConnect::setHandshake");
	
	int status = false;
		
	status = GetCommState(fd,&Config);
			
	Config.fDtrControl = DTR_CONTROL_DISABLE;		
	Config.fRtsControl = RTS_CONTROL_DISABLE;		
	Config.fOutxCtsFlow = false;					
	Config.fOutxDsrFlow = false;					
	Config.fOutX = false;							
	Config.fInX = false;							
		
	if(handshake & dtr_handshake)
	{
		Config.fDtrControl = DTR_CONTROL_HANDSHAKE;	
	}
	
	if(handshake & rts_handshake)
	{
		Config.fRtsControl = RTS_CONTROL_HANDSHAKE;	
	}
	
	if(handshake & cts_monitor)
	{
		Config.fOutxCtsFlow = true;					
	}

	if(handshake & dsr_monitor)
	{
		Config.fOutxDsrFlow = true;					
	}

	if(handshake & software)
	{
		Config.fOutX = true;							
		Config.fInX = true;							
	}
	
	status = status & SetCommState(fd,&Config);
	
	return status;
}

void SerialConnect::Shutdown()
{
	IT_IT("SerialConnect::Shutdown");

	state = ss_shutdown;
	SetEvent(Signal[RD]);//Is it good to close the reading in this way?
	SetEvent(Signal[TD]);//is it correct to do stop the WRITING for mission critical applications?
	SetEvent(Signal[RE]);//
}

#endif //WIN32

bool SerialConnect::Ok()
{
	return !fFail;
}

