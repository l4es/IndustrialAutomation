/* Classic Ladder Project */
/* Copyright (C) 2001-2015 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* August 2005 */
/* ------------------------------------------- */
/* Socket for modbus master (Distributed I/O)  */
/* + making call to low-level serial functions */
/* if this is the mode used                    */
/* ------------------------------------------- */

/* This library is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU Lesser General Public */
/* License as published by the Free Software Foundation; either */
/* version 2.1 of the License, or (at your option) any later version. */

/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU */
/* Lesser General Public License for more details. */

/* You should have received a copy of the GNU Lesser General Public */
/* License along with this library; if not, write to the Free Software */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __WIN32__
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#endif
#include <errno.h>
#include <time.h>

#include "classicladder.h"
#include "global.h"
#include "socket_modbus_master.h"
#include "protocol_modbus_master.h"
#include "serial_common.h"
#include "frames_log_buffers.h"

#ifdef __WIN32__
#define SOCK_FD SOCKET
#define SOCK_INVALID SOCKET_ERROR
HANDLE ThreadHandleClient = NULL;
DWORD ThreadIdClient;
#else
#define SOCK_FD unsigned int
#define SOCK_INVALID -1
pthread_t thread_socket_client;
#endif

#define BUF_SIZE 512


int ClientSocketRunning = 0;

SOCK_FD ClientSockDescrip[ NBR_MODBUS_SLAVES ]; // SOCK_INVALID if slave not opened

static char BuffDebugModbusMasterSocket[ 400 ];

void InitSocketModbusMaster( )
{

#ifndef __WIN32__
	int Error;
#endif
	int ScanClientSock;

	// WSAStartup for Windows already done for socket server...

	SerialGeneralInit( );
	InitModbusMasterParams( );
	for( ScanClientSock=0; ScanClientSock<NBR_MODBUS_SLAVES; ScanClientSock++ )
	{
//		ClientSocketOpened[ ScanClientSock ] = -1;
		ClientSockDescrip[ ScanClientSock ] = SOCK_INVALID;
	}

	ClientSocketRunning = 1;
#ifdef __WIN32__
	ThreadHandleClient = CreateThread( NULL/*no security attributes*/, 16*1024L/*default stack size*/,                                                   
			(LPTHREAD_START_ROUTINE)SocketModbusMasterLoop/* thread function*/, 
			NULL/*argument to thread function*/,                
			THREAD_QUERY_INFORMATION/*use default creation flags*/,                           
			&ThreadIdClient/* returns the thread identifier*/);                
	if ( ThreadHandleClient==NULL )
#else
	Error = pthread_create( &thread_socket_client, NULL, (void *(*)(void *))SocketModbusMasterLoop, (void *)NULL );
	if (Error)
#endif
	{
		debug_printf(DBG_HEADER_ERR "Failed to create thread Modbus I/O module master...\n");
		CloseSocketModbusMaster( );
	}
//v0.9.9, now done in HardwareActionsAfterProjectLoaded()	else
//v0.9.9	{
//v0.9.9		ConfigSerialModbusMaster( );
//v0.9.9	}
}

void ConfigSerialModbusMaster( void )
{
//	int Error = 0;
	if ( ModbusConfig.ModbusSerialPortNameUsed[ 0 ]!='\0' )
	{
#ifdef GTK_INTERFACE
		if ( !GeneralParamsMirror.RealInputsOutputsOnlyOnTarget )
#endif
		{
			SerialConfigDebugAndRts( IDX_SERIAL_MODBUS, ModbusConfig.ModbusDebugLevel, ModbusConfig.ModbusSerialUseRtsToSend );
			if ( !SerialOpen( IDX_SERIAL_MODBUS, ModbusConfig.ModbusSerialPortNameUsed, ModbusConfig.ModbusSerialSpeed, ModbusConfig.ModbusSerialDataBits, ModbusConfig.ModbusSerialParity, ModbusConfig.ModbusSerialStopBits ) )
			{
//				Error = -1;
				sprintf( &InfosGene->ErrorMsgStringToDisplay[1], "ailed to open serial port for Modbus I/O module master: %s !!!", ModbusConfig.ModbusSerialPortNameUsed );
				InfosGene->ErrorMsgStringToDisplay[0] = 'F'; // first char tested in multitask...
				FrameLogString( FRAMES_LOG_MODBUS_MASTER, '!', "Failed to open serial port" );
			}
			else
			{
				debug_printf(DBG_HEADER_INFO "Modbus I/O module master (serial) init ok !\n" );
			}
		}
	}
	else
	{
		debug_printf(DBG_HEADER_INFO "Modbus I/O module master (Ethernet) init ok !\n" );
	}
}


/* if not already connected => connect to slave... */
char VerifyConnection( int SlaveIndex )
{
	char Connected = FALSE;
printf("VerifyConnection slave%d, before sockok=%d\n", SlaveIndex, ClientSockDescrip[ SlaveIndex ]!=SOCK_INVALID?1:0 );
	if ( ClientSockDescrip[ SlaveIndex ]==SOCK_INVALID )
	{
		StrModbusSlave * pSlave = &ModbusSlaveList[ SlaveIndex ];
		struct sockaddr_in io_module_addr;          // Server Internet address

		if( ModbusConfig.ModbusDebugLevel>=2 )
		{
			debug_printf(DBG_HEADER_INFO "Modbus I/O module master - Init socket for slave %d.\n", SlaveIndex);	
		}
		SOCK_FD client_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if ( client_s==SOCK_INVALID )
		{
			strcpy( BuffDebugModbusMasterSocket, "Failed to open socket for Modbus I/O module master..." );
			debug_printf( DBG_HEADER_ERR "Modbus I/O module master - %s\n", BuffDebugModbusMasterSocket );
			FrameLogString( FRAMES_LOG_MODBUS_MASTER, '!', BuffDebugModbusMasterSocket );
		}
		else
		{
			int NumPort = 502; // default modbus port
			char * PosiSep;
			memset(&io_module_addr, 0, sizeof(io_module_addr));     /* Zero out structure */
			io_module_addr.sin_family = AF_INET;             /* Internet address family */
				
			// verify if port given in string ?
			PosiSep = strchr( pSlave->SlaveAdr, ':' );
			if ( PosiSep==NULL )
			{
				io_module_addr.sin_addr.s_addr = inet_addr( pSlave->SlaveAdr );   /* Server IP address */
			}
			else
			{
				static char Address[ 50 ];
				strcpy( Address, pSlave->SlaveAdr );
				Address[ PosiSep-pSlave->SlaveAdr ] = '\0';
				NumPort = atoi( PosiSep+1 );
				io_module_addr.sin_addr.s_addr = inet_addr( Address );   /* Server IP address */
			}
			io_module_addr.sin_port = htons( NumPort ); /* Server port */

			if ( !pSlave->UseUdpInsteadOfTcp )
			{
				if( ModbusConfig.ModbusDebugLevel>=2 )
				{
					debug_printf(DBG_HEADER_INFO "Modbus I/O module master - Connecting slave %d...\n", SlaveIndex);
				}
				/* Establish the connection with the I/O module */
				if (connect(client_s, (struct sockaddr *) &io_module_addr, sizeof(io_module_addr)) >= 0)
				{
					Connected = TRUE;
					ClientSockDescrip[ SlaveIndex ] = client_s;
				}
				else
				{
					strcpy( BuffDebugModbusMasterSocket, "Failed  to connect to slave!" ); 
					debug_printf( DBG_HEADER_ERR "Modbus I/O module master - %s\n", BuffDebugModbusMasterSocket );
					FrameLogString( FRAMES_LOG_MODBUS_MASTER, '!', BuffDebugModbusMasterSocket );
ClientSockDescrip[ SlaveIndex ] = client_s;
CloseSockSlave( SlaveIndex );
				}
			}
			else
			{
				Connected = TRUE;
				pSlave->SlavePortIP = NumPort;
			}
		}
	}
	else
	{
		Connected = TRUE;
	}

printf("VerifyConnection slave%d, after sockok=%d ; cnx=%d\n", SlaveIndex, ClientSockDescrip[ SlaveIndex ]!=SOCK_INVALID?1:0, Connected );
	return Connected;
}

int SendSocketModbusMaster( int SlaveIndex, char * Frame, int LgtFrame )
{
	int Status = -1;
	int LgtSend;
//TODO: for UDP, get port value from this struct...
//	StrModbusSlave * pSlave = &ModbusSlaveList[ SlaveIndex ];
	if ( VerifyConnection( SlaveIndex ) )
	{
		if( ModbusConfig.ModbusDebugLevel>=2 )
		{
			debug_printf(DBG_HEADER_INFO "Modbus I/O module master - Sending frame to slave %d...\n",SlaveIndex);
		}
		/* Send the modbus frame */
		LgtSend = send( ClientSockDescrip[ SlaveIndex ], Frame, LgtFrame, 0 );
		if ( LgtSend==LgtFrame )
		{
			Status = 0;
		}
		else
		{
			debug_printf(DBG_HEADER_ERR "Modbus I/O module master - FAILED TO SEND ON SOCKET !!!(LgtSend=%d)\n",LgtSend);
		}
	}
	return Status;
}

int WaitRespSocketModbusMaster( int SlaveIndex, char * Buff, int BuffSize, int TimeOutResponseMilliSecs )
{
	int ResponseSize = 0;
	if ( ClientSockDescrip[ SlaveIndex ]!=SOCK_INVALID )
	{
		int recep_descrip;
		fd_set myset;
		struct timeval tv;
		FD_ZERO( &myset);
		// add descrip to survey and set time-out wanted !
		FD_SET( ClientSockDescrip[ SlaveIndex ], &myset );
		tv.tv_sec = 0; //seconds
		tv.tv_usec = TimeOutResponseMilliSecs*1000; //micro-seconds
		recep_descrip = select( /*16*/ClientSockDescrip[ SlaveIndex ]+1, &myset, NULL, NULL, &tv );
		if ( recep_descrip>0 )
		{
			int bytesRcvd;
			if( ModbusConfig.ModbusDebugLevel>=2 )
			{
				debug_printf(DBG_HEADER_INFO "Modbus I/O module master - Waiting response for slave %d...\n",SlaveIndex);
			}
			if ((bytesRcvd = recv( ClientSockDescrip[ SlaveIndex ], Buff, BuffSize, 0 )) > 0)
				ResponseSize = bytesRcvd;
		}
	}
	return ResponseSize;
}

void CloseSockSlave( int SlaveIndex )
{
//	if ( ClientSocketOpened[ SlaveIndex ]!=-1 )
	if ( ClientSockDescrip[ SlaveIndex ]!=SOCK_INVALID )
	{
		if( ModbusConfig.ModbusDebugLevel>=2 )
		{
			debug_printf(DBG_HEADER_INFO "Modbus I/O module master - Closing slave %d.\n", SlaveIndex);
		}
#ifdef __WIN32__
			closesocket( ClientSockDescrip[ SlaveIndex ] );
#else
			close( ClientSockDescrip[ SlaveIndex ] );
#endif
//		ClientSocketOpened[ SlaveIndex ] = -1;
		ClientSockDescrip[ SlaveIndex ] = SOCK_INVALID;
	}
}

void CloseSocketModbusMaster( void )
{
	int ScanClientSock;
	ClientSocketRunning = 0;
#ifdef __WIN32__
	if ( ThreadHandleClient )
		TerminateThread( ThreadHandleClient, 0);
#endif	
	// close sockets
	for( ScanClientSock=0; ScanClientSock<NBR_MODBUS_SLAVES; ScanClientSock++ )
	{
		CloseSockSlave( ScanClientSock );
	}
	if ( ModbusConfig.ModbusSerialPortNameUsed[ 0 ]!='\0' )
		SerialClose( IDX_SERIAL_MODBUS );
	debug_printf(DBG_HEADER_INFO "Modbus I/O module master closed!\n");
}

void SocketModbusMasterLoop( void )
{
	int CurrentSlave = -1;
	int SizeQuestionToAsk;
	static char QuestionFrame[ 800 ];
	int ResponseSize;
	static char ResponseFrame[ 800 ];
	int SendResultStatus = 0;

#ifdef __XENO__
	pthread_set_name_np(pthread_self(), __FUNCTION__);
#endif
#ifdef __COBALT__
	pthread_setname_np(pthread_self(), __FUNCTION__);
#endif

	while( ClientSocketRunning )
	{
		char DoSleep = FALSE;
// TODO: added for XENO... not found why required for now...
// (task suspended otherwise with the watchdog, seen with dmesg!)
DoPauseMilliSecs( 10 );

		if ( InfosGene==NULL )
			DoSleep = TRUE;
		else if (InfosGene->LadderState!=STATE_RUN || ( ModbusConfig.ModbusSerialPortNameUsed[ 0 ]!='\0' && !SerialPortIsOpened( IDX_SERIAL_MODBUS ) ) )
			DoSleep = TRUE;

		if ( DoSleep )
		{
//			int ScanSlaveList;
			DoPauseMilliSecs( ModbusConfig.ModbusTimeInterFrame );
//			for( ScanSlaveList=0; ScanSlaveList<NBR_MODBUS_SLAVES ;ScanSlaveList++ )
//				InitStatsForSlave( ScanSlaveList );
//////printf("Modbus I/O module master: sleeping (not run, or serial not open...)\n");
		}
		else
		{
			SizeQuestionToAsk = ModbusMasterAsk( &CurrentSlave, (unsigned char*)QuestionFrame );
			if ( SizeQuestionToAsk>0 )
			{
				StrModbusSlave * pModbusSlave = &ModbusSlaveList[ CurrentSlave ];
				pModbusSlave->StatsNbrFrames++;
				pModbusSlave->StatsCurrentTime = time( NULL );
				if ( pModbusSlave->StatsStartTime==0 )
					pModbusSlave->StatsStartTime = pModbusSlave->StatsCurrentTime;
//printf("Modbus: request lgt to send %d\n", SizeQuestionToAsk );
				if ( ModbusConfig.ModbusSerialPortNameUsed[ 0 ]=='\0' )
				{
					SendResultStatus = SendSocketModbusMaster( CurrentSlave, QuestionFrame, SizeQuestionToAsk );
					if ( SendResultStatus==-1 )
						CloseSockSlave( CurrentSlave );
				}
				else
				{
					int NbrCharsWaited = 1/*adr*/+GetModbusResponseLenghtToReceive()+2/*crc*/;
					// before sending question, set size of frame that will be to receive after! 
//					if( ModbusConfig.ModbusDebugLevel>=3 )
//						debug_printf(DBG_HEADER_INFO "I/O modbus master - SetResponseSize, NbrCharsToReceive=%d\n",NbrCharsWaited);
					SerialSetResponseSize( IDX_SERIAL_MODBUS, NbrCharsWaited, ModbusConfig.ModbusTimeOutReceipt );
//TEMP TEST USB-RS485 converter...
//SerialSend( "\0\0", 2 );
					SerialSend( IDX_SERIAL_MODBUS, QuestionFrame, SizeQuestionToAsk );
				}

				ResponseSize = 0;
				if ( SendResultStatus==0 )
				{
					if ( ModbusConfig.ModbusTimeAfterTransmit>0 )
					{
						// usefull for USB-RS485 dongle...
						if( ModbusConfig.ModbusDebugLevel>=3 )
						{
							debug_printf(DBG_HEADER_INFO "Modbus I/O module master - Delay after transmit now...\n");
						}
						DoPauseMilliSecs( ModbusConfig.ModbusTimeAfterTransmit );
					}
					
					if ( ModbusConfig.ModbusSerialPortNameUsed[ 0 ]=='\0' )
						ResponseSize = WaitRespSocketModbusMaster( CurrentSlave, ResponseFrame, 800, ModbusConfig.ModbusTimeOutReceipt );
					else
						ResponseSize = SerialReceive( IDX_SERIAL_MODBUS, ResponseFrame, 800 );
					if ( ResponseSize==0 )
					{
						sprintf( BuffDebugModbusMasterSocket, "NO RESPONSE (Errs=%d/%d) !!!!????", ++pModbusSlave->StatsNbrErrorsNoResponse, pModbusSlave->StatsNbrFrames);
						debug_printf( DBG_HEADER_ERR "Modbus I/O module master - %s\n", BuffDebugModbusMasterSocket );
						FrameLogString( FRAMES_LOG_MODBUS_MASTER, '!', BuffDebugModbusMasterSocket );
					}
				}
				// we have to called this function even if no response received.
				if ( !TreatModbusMasterResponse( (unsigned char *)ResponseFrame, ResponseSize ) )
				{
					if ( ResponseSize>0 )
						pModbusSlave->StatsNbrErrorsModbusTreat++;
					// trouble? => cleanup all pending chars (perhaps we can receive now responses for old asks
					// and we are shifted between ask/resp...)
					if ( ModbusConfig.ModbusSerialPortNameUsed[ 0 ]!='\0' )
					{
						SerialPurge( IDX_SERIAL_MODBUS );
					}
					else
					{
						if ( SendResultStatus==0 )
						{
							do
							{
								ResponseSize = WaitRespSocketModbusMaster( CurrentSlave, ResponseFrame, 800, 10/*ModbusConfig.ModbusTimeOutReceipt*/ );
							}
							while( ResponseSize>0 );
						}
					}
				}
				if ( ResponseSize==0 && ModbusConfig.ModbusSerialPortNameUsed[ 0 ]=='\0' )
					CloseSockSlave( CurrentSlave );
				if ( ModbusConfig.ModbusTimeInterFrame>0 )
					DoPauseMilliSecs( ModbusConfig.ModbusTimeInterFrame );
			}
			else
			{
//				sleep( 1 );
				DoPauseMilliSecs( 1000 );
//////printf("Modbus I/O module master: sleeping (no request to ask...)\n");
			}
		}
	
	}
#ifndef __WIN32__
	pthread_exit(NULL);
#endif

}

void GetSocketModbusMasterStats( int SlaveToDisplay, char * Buff )
{
	if ( SlaveToDisplay>=0 && SlaveToDisplay<NBR_MODBUS_SLAVES )
	{
		StrModbusSlave * pSlaveDisplayed = &ModbusSlaveList[ SlaveToDisplay ];
		int CalcNbrFramesPerSecondX10 = 0;
		if ( pSlaveDisplayed->StatsNbrFrames>0 && pSlaveDisplayed->StatsStartTime!=0 && pSlaveDisplayed->StatsCurrentTime!=pSlaveDisplayed->StatsStartTime )
		{
			CalcNbrFramesPerSecondX10 = (pSlaveDisplayed->StatsNbrFrames*10)/(pSlaveDisplayed->StatsCurrentTime-pSlaveDisplayed->StatsStartTime);
			printf("Calc modbus/master speed: DiffTime=%d, Speed=%d.%d frames/secs\n", pSlaveDisplayed->StatsCurrentTime-pSlaveDisplayed->StatsStartTime, CalcNbrFramesPerSecondX10/10,CalcNbrFramesPerSecondX10%10 );
		}
		sprintf( Buff, "FramesSend=%d - NoResp=%d - BadResps=%d - %d.%d frames/sec", pSlaveDisplayed->StatsNbrFrames, pSlaveDisplayed->StatsNbrErrorsNoResponse, pSlaveDisplayed->StatsNbrErrorsModbusTreat, CalcNbrFramesPerSecondX10/10, CalcNbrFramesPerSecondX10%10 );
	}
	else
	{
		strcpy( Buff, "Bad modbus slave for stats !" );
	}
}

