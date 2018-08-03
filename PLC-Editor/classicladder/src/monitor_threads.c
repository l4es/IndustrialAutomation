/* Classic Ladder Project */
/* Copyright (C) 2001-2016 Marc Le Douarain */
/* http://membres.lycos.fr/mavati/classicladder/ */
/* http://www.sourceforge.net/projects/classicladder */
/* February 2011 */
/* ------------------------------------------------------ */
/* Monitor protocol - Threads for both slave/master       */
/* ------------------------------------------------------ */
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
#ifdef GTK_INTERFACE
#include <libintl.h> // i18n
#include <locale.h> // i18n
#endif

#ifdef __WIN32__
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "classicladder.h"
#include "global.h"
#include "monitor_sockets_udp.h"
#include "monitor_protocol.h"
#include "serial_common.h"
#include "monitor_protocol_adds_serial.h"
#ifdef GTK_INTERFACE
#include <gtk/gtk.h>
#include "menu_and_toolbar_gtk.h"
#include "classicladder_gtk.h"
#include "monitor_windows_gtk.h"
#include "spy_vars_gtk.h"
#endif
#include "monitor_threads.h"
#include "monitor_transfer.h"
#include "vars_access.h"
#include "vars_system.h"
#include "calc.h"
#include "modem.h"
#include "frames_log_buffers.h"

#define LGT_BUFF_RX 1280
#define LGT_BUFF_TX 1280

#ifdef __WIN32__
HANDLE ThreadHandleMonitorSlaveIP = NULL;
DWORD ThreadIdMonitorSlaveIP;
HANDLE ThreadHandleMonitorSlaveSerial = NULL;
DWORD ThreadIdMonitorSlaveSerial;
HANDLE ThreadHandleMonitorMaster = NULL;
DWORD ThreadIdMonitorMaster;
#else
pthread_t ThreadMonitorSlaveIP;
pthread_t ThreadMonitorSlaveSerial;
pthread_t ThreadMonitorMaster;
#endif

char MonitorSlaveRunning[ 2 ]; // 0=IP, 1=Serial
char MonitorMasterRunning = TRUE;

// for serial (both slave & master, to permit to have master/slave running at both time with one same executable ! very usefull for debug...)
extern StrMonitorSerialBuffer MonitorSerialBuffer[ NBR_MONITOR_SERIALS_BUFFERS ];


void InitGeneralMonitor( int ListenPortForSlave, char InitOnlyTheSlave )
{
	int Error = FALSE;
	InitMonitorTransferFile( );
	MonitorSlaveRunning[ 0 ] = TRUE;
	MonitorSlaveRunning[ 1 ] = TRUE;
	MonitorSerialBuffer[ 0 ].CurrLgtRecv = -1;
	MonitorSerialBuffer[ 1 ].CurrLgtRecv = -1;
	InitMonitorSocketsUdp( 9000, InitOnlyTheSlave );

	if ( IsMonitorSocketUdpInitOk( MONSOCKSLAVE ) )
	{
		if ( Preferences.MonitorSlaveOnSerialPort[0]!='\0' )
			SerialOpen( IDX_SERIAL_MON_SLAVE, Preferences.MonitorSlaveOnSerialPort, Preferences.MonitorSlaveSerialSpeed, 8, 0, 1 );
#ifdef __WIN32__
		ThreadHandleMonitorSlaveIP = CreateThread( NULL/*no security attributes*/, 16*1024L/*default stack size*/,
				(LPTHREAD_START_ROUTINE)MonitorSlaveLoopThread/* thread function*/, 
				0/*argument to thread function*/,                
				THREAD_QUERY_INFORMATION/*use default creation flags*/,                           
				&ThreadIdMonitorSlaveIP/* returns the thread identifier*/);                
		if ( Preferences.MonitorSlaveOnSerialPort[0]!='\0' )
		{
			ThreadHandleMonitorSlaveSerial = CreateThread( NULL/*no security attributes*/, 16*1024L/*default stack size*/,
					(LPTHREAD_START_ROUTINE)MonitorSlaveLoopThread/* thread function*/, 
					1/*argument to thread function*/,                
					THREAD_QUERY_INFORMATION/*use default creation flags*/,                           
					&ThreadIdMonitorSlaveSerial/* returns the thread identifier*/);                
		}
		if ( ThreadHandleMonitorSlaveIP==NULL || ( ThreadHandleMonitorSlaveSerial==NULL && Preferences.MonitorSlaveOnSerialPort[0]!='\0' ) )
#else
		Error = pthread_create( &ThreadMonitorSlaveIP, NULL, (void *(*)(void *))MonitorSlaveLoopThread, (void *)NULL /*0*/ );
		if ( !Error )
		{
			if ( Preferences.MonitorSlaveOnSerialPort[0]!='\0' )
				Error = pthread_create( &ThreadMonitorSlaveSerial, NULL, (void *(*)(void *))MonitorSlaveLoopThread, (void *)1 );
		}
		if (Error)
#endif
		{
			debug_printf(DBG_HEADER_ERR "Failed to create thread(s) monitor slave...\n");
			EndMonitorSocketsUdp( );
			Error = TRUE; //for Windows...
		}
	}
	if ( !Error )
	{
		
#ifdef GTK_INTERFACE
		if ( !InitOnlyTheSlave )
		{
#ifdef __WIN32__
			ThreadHandleMonitorMaster = CreateThread( NULL/*no security attributes*/, 16*1024L/*default stack size*/,
					(LPTHREAD_START_ROUTINE)MonitorMasterLoopThread/* thread function*/, 
					NULL/*argument to thread function*/,                
					THREAD_QUERY_INFORMATION/*use default creation flags*/,                           
					&ThreadIdMonitorMaster/* returns the thread identifier*/);                
			if ( ThreadHandleMonitorMaster==NULL )
#else
			Error = pthread_create( &ThreadMonitorMaster, NULL, (void *(*)(void *))MonitorMasterLoopThread, (void *)NULL );
			if (Error)
#endif
			{
				debug_printf(DBG_HEADER_ERR "Failed to create thread monitor master...\n");
				EndGeneralMonitor( );
				EndMonitorSocketsUdp( );
			}
		
		}
#endif
	}
}

void EndGeneralMonitor( void )
{
	MonitorSlaveRunning[ 0 ] = FALSE;
	MonitorSlaveRunning[ 1 ] = FALSE;
	MonitorMasterRunning = FALSE;
#ifdef __WIN32__
	if ( ThreadHandleMonitorSlaveSerial )
		TerminateThread( ThreadHandleMonitorSlaveSerial, 0);
	if ( ThreadHandleMonitorSlaveIP )
		TerminateThread( ThreadHandleMonitorSlaveIP, 0);
	if ( ThreadHandleMonitorMaster )
		TerminateThread( ThreadHandleMonitorMaster, 0);
#endif
	EndMonitorSocketsUdp( );
	if ( Preferences.MonitorSlaveOnSerialPort!='\0' )
		SerialClose( IDX_SERIAL_MON_SLAVE );
}

//SlaveThreadForSerial=0 for IP, 1 for serial...
void MonitorSlaveLoopThread( int SlaveThreadForSerial )
{
	char * RequestReceived;
	char * SerialPartReceived;
	char * SerialResponseToSend;
	int SerialCountInitModem = 0;

#ifdef __XENO__
	pthread_set_name_np(pthread_self(), SlaveThreadForSerial?"MonitorSlaveSerial":"MonitorSlaveIP");
#endif
#ifdef __COBALT__
	pthread_setname_np(pthread_self(), SlaveThreadForSerial?"MonitorSlaveSerial":"MonitorSlaveIP");
#endif

	RequestReceived = malloc( LGT_BUFF_RX );
	SerialPartReceived = malloc( LGT_BUFF_RX );
	SerialResponseToSend = malloc( LGT_BUFF_TX );
	if ( RequestReceived==NULL || SerialPartReceived==NULL || SerialResponseToSend==NULL )
	{
		printf("out of mem in Thread Slave Monitor\n");
		MonitorSlaveRunning[ SlaveThreadForSerial ] = FALSE;
	}
	
printf("Starting Thread Slave Monitor (%s)\n", SlaveThreadForSerial?"Serial":"Udp" );
	while( MonitorSlaveRunning[ SlaveThreadForSerial ] )
	{
		int ReqReceivedLgt;
		int SerialPartLgt;
//		printf("monslave: wait request...\n" );
		if ( SlaveThreadForSerial )
		{
			if ( Modem.ModemUsed )
			{
				if ( SerialCountInitModem==0 )
				{
					SetModemSerialNumPort( IDX_SERIAL_MON_SLAVE );
					if ( InitAndConfigModem( &Modem ) )
						SerialCountInitModem++;
				}
				else
				{
					//periodic init modem
					SerialCountInitModem++;
					if ( SerialCountInitModem>(30*60*1000)/50 && !GetIfModemIsConnected( ) )
						SerialCountInitModem = 0;
				}
				ModemAlarmsTransmitForSms( );
			}
			
			SerialPartLgt = SerialReceive( IDX_SERIAL_MON_SLAVE, SerialPartReceived, LGT_BUFF_RX );
			if ( SerialPartLgt>0 )
			{
				SerialPartReceived[ SerialPartLgt ] = '\0';
				if ( Modem.ModemUsed )
					AnalyseCharsReceivedFromModem( SerialPartReceived, SerialPartLgt );
printf("SERIAL SLAVE PART FRAME=%s\n", SerialPartReceived);
FrameLogString( SlaveThreadForSerial?FRAMES_LOG_MONITOR_SLAVE_SERIAL:FRAMES_LOG_MONITOR_SLAVE_IP, 'p', SerialPartReceived );
			}
			ReqReceivedLgt = MonitorSerialDatasReceivedIsEnd( MONITOR_SERIAL_SLAVE, SerialPartReceived, SerialPartLgt, RequestReceived );
		}
		else
		{
			ReqReceivedLgt = ReceiveMonitorFrameSocketUdp( MONSOCKSLAVE, RequestReceived, LGT_BUFF_RX, 0/*infinite*/ );
		}
		if ( ReqReceivedLgt>0 )
		{
			RequestReceived[ ReqReceivedLgt ] = '\0'; //else will show end part of previous big request, very strange to see!
			printf("monslave: request received (lgt=%d) : %s \n", ReqReceivedLgt, RequestReceived );
//			printf("monslave: request received (lgt=%d)!\n", ReqReceivedLgt );
			FrameLogString( SlaveThreadForSerial?FRAMES_LOG_MONITOR_SLAVE_SERIAL:FRAMES_LOG_MONITOR_SLAVE_IP, '<', RequestReceived );
			char * ReplyToSend = MonitorParseGeneralResponseToSwitch( RequestReceived, /*SlaveMode*/1+SlaveThreadForSerial );
			if ( ReplyToSend )
			{
				printf("monslave: send response (lgt=%d) : %s \n", strlen(ReplyToSend), ReplyToSend );
//				printf("monslave: send response (lgt=%d)!\n", strlen(ReplyToSend) );
				FrameLogString( SlaveThreadForSerial?FRAMES_LOG_MONITOR_SLAVE_SERIAL:FRAMES_LOG_MONITOR_SLAVE_IP, '>', ReplyToSend );
				if ( SlaveThreadForSerial )
				{
					int FinalLength;
					int PureLength = strlen(ReplyToSend);
					// let first two chars availables for STX & @ in serial frame
					memcpy( (void*)&SerialResponseToSend[2], (void*)ReplyToSend, PureLength );
					FinalLength = MonitorAddSerialEncapsul( SerialResponseToSend, PureLength );
					SerialSend( IDX_SERIAL_MON_SLAVE, SerialResponseToSend, FinalLength );
				}
				else
				{
					SendMonitorFrameSocketUdp( MONSOCKSLAVE, ReplyToSend, strlen(ReplyToSend), 0, 0 );
				}
				//free here cJSON_Print() !
				free( ReplyToSend );
			}
		}
		else
		{
			if ( SlaveThreadForSerial && SerialPartLgt==0 )
				DoPauseMilliSecs( 50 );
		}
	}
	
	free( RequestReceived );
	free( SerialPartReceived );
	free( SerialResponseToSend );

#ifndef __WIN32__
	pthread_exit(NULL);
#endif
}

#ifdef GTK_INTERFACE
int SchedAskType = 0; // 0=get vars, 1=get rungs, 2=get grafcet displayed
int SchedAskNbr = 0; // get state of each rung displayed
static char DebugMsgMaster[ LGT_BUFF_TX+100 ];
int DecptAskModbusMasterStats = -1;
#ifndef MONITOR_TEST_ONLY_NO_RESPONSES_USE
#define NBR_FRAMES_TO_ASK_MODBUS_MASTER_STATS 100
#else
#define NBR_FRAMES_TO_ASK_MODBUS_MASTER_STATS 10
#endif
char * MonitorMasterSchedulerRequestToAsk( int * pReplyTimeOutWanted )
{
	char * ReqToAsk = NULL;
//TODO possibility for file transfer when connected (ask a frame transfer some time to time)
	if ( InfosGUI->TargetMonitor.RemoteFileTransfer )
	{
		*pReplyTimeOutWanted = *pReplyTimeOutWanted*16; //5000;
		ReqToAsk = MonitorCreateRequestFileTransfer( );
	}
	else if ( InfosGUI->TargetMonitor.AskTargetForThisManualFrame!=NULL )
	{
		ReqToAsk = InfosGUI->TargetMonitor.AskTargetForThisManualFrame;
		sprintf(DebugMsgMaster,"MANUAL FRAME TO SEND (seen req=%s)! ====\n",ReqToAsk);
		MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, DebugMsgMaster );
	}
	else if ( InfosGUI->TargetMonitor.AskTargetToSetState!=-1 )
	{
		ReqToAsk = MonitorCreateRequestSetRunStopState( InfosGUI->TargetMonitor.AskTargetToSetState );
	}
	else if ( InfosGUI->TargetMonitor.AskTargetToReset!=-1 )
	{
		ReqToAsk = MonitorCreateRequestResetTarget( );
	}
	else if ( InfosGUI->TargetMonitor.AskTargetToSetClockTime!=-1 )
	{
		*pReplyTimeOutWanted = *pReplyTimeOutWanted*6;
		ReqToAsk = MonitorCreateRequestSetClockTime( );
	}
	else if ( InfosGUI->TargetMonitor.AskTargetToGetTargetInfos!=-1 )
	{
		ReqToAsk = MonitorCreateRequestGetTargetInfosVersion( );
	}
	else if ( InfosGUI->TargetMonitor.AskTargetToGetProjectProperties!=-1 )
	{
		ReqToAsk = MonitorCreateRequestGetProjectProperties( );
	}
	else if ( InfosGUI->TargetMonitor.AskTargetToReboot!=-1 )
	{
		ReqToAsk = MonitorCreateRequestRebootTarget( );
	}
	else if ( InfosGUI->TargetMonitor.AskTargetToWriteVarType!=-1 && InfosGUI->TargetMonitor.AskTargetToWriteVarNum!=-1 )
	{
		ReqToAsk = MonitorCreateRequestWriteVarValue( );
	}
	else if ( InfosGUI->TargetMonitor.AskTargetToSetOrUnsetVarType!=-1 && InfosGUI->TargetMonitor.AskTargetToSetOrUnsetVarNum!=-1 )
	{
		ReqToAsk = MonitorCreateRequestSetOrUnsetVar( );
	}
	else if ( InfosGUI->TargetMonitor.AskTargetToGetNetworkConfig!=-1 )
	{
		ReqToAsk = MonitorCreateRequestGetNetworkConfig( );
	}
	else if ( InfosGUI->TargetMonitor.AskTargetToWriteNetworkConfig!=-1 )
	{
		ReqToAsk = MonitorCreateRequestWriteNetworkConfig( );
	}
	else if ( InfosGUI->TargetMonitor.AskTargetToGetMonitorSerialConfig!=-1 )
	{
		ReqToAsk = MonitorCreateRequestGetMonitorSerialConfig( );
	}
	else if ( InfosGUI->TargetMonitor.AskTargetToWriteMonitorSerialConfig!=-1 )
	{
		ReqToAsk = MonitorCreateRequestWriteMonitorSerialConfig( );
	}
	else if ( InfosGUI->TargetMonitor.AskTargetToCleanUp!=-1 )
	{
		ReqToAsk = MonitorCreateRequestCleanUp( );
	}
	else
	{
		
		if ( DecptAskModbusMasterStats>0 )
			DecptAskModbusMasterStats--;
		if ( DecptAskModbusMasterStats==0 )
		{
			ReqToAsk = MonitorCreateRequestGetModbusMasterStats( GetCurrentNumSlaveForStats() );
			if ( FindNextSlaveForStats(FALSE)!=-1 )
			{
				DecptAskModbusMasterStats = NBR_FRAMES_TO_ASK_MODBUS_MASTER_STATS/GetNbrTotalSlaveForStats();
			}
		}
		else
		{
//TEMP!!!!!! only ladder, interesting shorter frames to debug...
//////SchedAskType=1;

			int NumRungToRead;
	sprintf( DebugMsgMaster, "monmaster_sched: <<< type=%d, num=%d >>>\n", SchedAskType, SchedAskNbr );
	MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, DebugMsgMaster );
			switch( SchedAskType )
			{
				case 0:
					ReqToAsk = MonitorCreateRequestReadVars( );
					break;
				case 1:
					NumRungToRead = InfosGUI->NumRungDisplayedToMonitor[ SchedAskNbr ];
					if ( NumRungToRead>=0 && NumRungToRead<NBR_RUNGS )
						ReqToAsk = MonitorCreateRequestReadRung( NumRungToRead );
					break;
				case 2:
					ReqToAsk = MonitorCreateRequestReadSequential( SectionArray[ InfosGene->CurrentSection ].SequentialPage );
					break;
			}
			// next...
			if ( SchedAskType==0 || SchedAskType==2 )
			{
				int iCurrentLanguage = SectionArray[ InfosGene->CurrentSection ].Language;
				SchedAskType++;
				// if no rung displayed, go for sequential
				if ( iCurrentLanguage==SECTION_IN_SEQUENTIAL )
					SchedAskType++;
			}
			else if ( SchedAskType==1 )
			{
				SchedAskNbr++;
				if ( SchedAskNbr>=InfosGUI->NbrRungsDisplayedToMonitor )
					SchedAskType = 99;
			}
			else if ( SchedAskType==2 )
			{
				SchedAskType = 99;
			}
			if ( SchedAskType>2 )
			{
				SchedAskType = 0;
				SchedAskNbr = 0;
			}
		}
	}
	return ReqToAsk;
}

void EndMasterCnxOnError( void )
{
	//added in v9.0.100 to avoid for example a waiting reboot at next connexion ! :-(
	InitTargetMonitorAsks( );
	if ( InfosGUI->TargetMonitor.RemoteConnected )
	{
// added because also called from this thread (not gtk main) !
//gdk_threads_enter();
//						DoFlipFlopConnectDisconnectTargetInGtk( );
//gdk_threads_leave();
//simpler... g_idle_add( DoDisconnectTargetInGtkWithLock, NULL );
g_idle_add( (GSourceFunc)DoFlipFlopConnectDisconnectTargetInGtk, NULL );
	}
	if ( InfosGUI->TargetMonitor.RemoteFileTransfer )
	{
		InfosGUI->TargetMonitor.RemoteFileTransfer = FALSE;
		g_idle_add( (GSourceFunc)FileTransferUpdateInfosGtk, NULL );
	}
}

void MonitorMasterLoopThread( void )
{
	char * RequestReceived;
	char * SerialPartReceived;
	char * SerialRequestToAsk;
	int NbrSendTry;
	char LastRequestInTimeOutError = FALSE;
	int MasterStatsNbrFramesSend, MasterStatsNbrFramesErrors;
	RequestReceived = malloc( LGT_BUFF_RX );
	SerialPartReceived = malloc( LGT_BUFF_RX );
	SerialRequestToAsk = malloc( LGT_BUFF_TX );
	if ( RequestReceived==NULL || SerialPartReceived==NULL || SerialRequestToAsk==NULL )
	{
		printf("out of mem in Thread Master Monitor\n");
		MonitorMasterRunning = FALSE;
	}

	while( MonitorMasterRunning )
	{
		char ErrorToConnect = FALSE;
		// state change on cnx / discnx ?
		if ( InfosGUI->TargetMonitor.PreviousRemoteConnected!=InfosGUI->TargetMonitor.RemoteConnected || ( InfosGUI->TargetMonitor.PreviousRemoteFileTransfer!=InfosGUI->TargetMonitor.RemoteFileTransfer && !InfosGUI->TargetMonitor.RemoteConnected ) )
		{
			InfosGUI->TargetMonitor.PreviousRemoteConnected = InfosGUI->TargetMonitor.RemoteConnected;
			InfosGUI->TargetMonitor.PreviousRemoteFileTransfer = InfosGUI->TargetMonitor.RemoteFileTransfer;
			NbrSendTry = 0;
			MasterStatsNbrFramesSend = 0;
			MasterStatsNbrFramesErrors = 0;
			
			if ( InfosGUI->TargetMonitor.RemoteConnected )
			{
				// at start of cnx, try to get projects info version to detect parameters mismatch between local and target
				InfosGUI->TargetMonitor.AskTargetToGetProjectProperties = 1;
//done before stopping engine, moved below				InitVars( TRUE/*DoLogEvents*/ );
//				InitSystemVars( FALSE );
//				PrepareAllDatasBeforeRun( );
				if ( FindNextSlaveForStats(TRUE)!=-1 )
					DecptAskModbusMasterStats = NBR_FRAMES_TO_ASK_MODBUS_MASTER_STATS/GetNbrTotalSlaveForStats();
				else
					DecptAskModbusMasterStats = -1; //disabled!
			}
			else
			{
				InfosGene->DurationOfLastScan = -1; // hide it now disconnected...
				InfosGene->NbrTicksMissed = -1; // idem
			}

			// monitor on serial ?
			if ( InfosGUI->TargetMonitor.RemoteWithSerialPort[ 0 ]!='\0' )
			{
sprintf( DebugMsgMaster, "MONITOR MASTER FOR SERIAL (%s)...\n", ( InfosGUI->TargetMonitor.RemoteConnected || InfosGUI->TargetMonitor.RemoteFileTransfer )?"Open":"Close" );
printf( "%s",DebugMsgMaster );
MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, DebugMsgMaster );
				if ( InfosGUI->TargetMonitor.RemoteConnected || InfosGUI->TargetMonitor.RemoteFileTransfer )
				{
					// try to open serial port
					if ( !SerialOpen( IDX_SERIAL_MON_MASTER, InfosGUI->TargetMonitor.RemoteWithSerialPort, InfosGUI->TargetMonitor.RemoteWithSerialSpeed, 8, 0, 1 ) )
					{
//						strcpy( InfosGene->ErrorMsgStringToDisplay, "Failed to open this serial port..." );
						ShowMessageBoxInBackground( NULL, _("Failed to open this serial port...") );
						EndMasterCnxOnError( );
						ErrorToConnect = TRUE;
					}
					if (  InfosGUI->TargetMonitor.RemoteWithSerialModem )
					{
						SetModemSerialNumPort( IDX_SERIAL_MON_MASTER );
						if ( !InitAndConfigModem( &Preferences.ModemForMasterMonitor ) )
						{
							ShowMessageBoxInBackground( NULL, _("Failed to init and configure modem...") );
							EndMasterCnxOnError( );
							ErrorToConnect = TRUE;
						}
						else
						{
							if ( !CallModem( Preferences.ModemForMasterMonitor.StrCallSequence, InfosGUI->TargetMonitor.RemoteTelephoneNumber ) )
							{
								ShowMessageBoxInBackground( NULL, _("Failed to call telephone number...") );
								EndMasterCnxOnError( );
								ErrorToConnect = TRUE;
							}
						}
					}
				}
				else
				{
					if (  InfosGUI->TargetMonitor.RemoteWithSerialModem )
						HangUpModem( );
					// close the serial port
					SerialClose( IDX_SERIAL_MON_MASTER );
				}
			}
		}//if ( InfosGUI->TargetMonitor.PreviousRemoteConnected!=InfosGUI->TargetMonitor.RemoteConnected || InfosGUI->TargetMonitor.PreviousRemoteFileTransfer!=InfosGUI->TargetMonitor.RemoteFileTransfer )
		
		if ( ( InfosGUI->TargetMonitor.RemoteConnected || InfosGUI->TargetMonitor.RemoteFileTransfer ) && !ErrorToConnect )
		{
			int ReqReceivedLgt;
			char * RequestToSend = NULL;
			int ReplyTimeOutValueInMs = InfosGUI->TargetMonitor.TimeOutWaitReply; //300; //250;
			RequestToSend = MonitorMasterSchedulerRequestToAsk( &ReplyTimeOutValueInMs );
//MonitorWindowAddText( "monmaster: target to ask.\n" );
			if ( RequestToSend!=NULL )
			{
				int PureLength = strlen(RequestToSend);
				if ( InfosGUI->TargetMonitor.RemoteWithSerialPort[ 0 ]!='\0' )
				{
					int FinalLength;
					sprintf( DebugMsgMaster, "monmaster: pure frame to send (lgt=%d):%s \n", PureLength, RequestToSend );
					MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, DebugMsgMaster );
					// let first two chars availables for STX & @ in serial frame
					memcpy( (void*)&SerialRequestToAsk[2], (void*)RequestToSend, PureLength );
					FinalLength = MonitorAddSerialEncapsul( SerialRequestToAsk, PureLength );
					sprintf( DebugMsgMaster, "monmaster: send serial (lgt=%d):%s \n", FinalLength, SerialRequestToAsk );
					MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, DebugMsgMaster );
					if ( LastRequestInTimeOutError )
					{
						SerialPurge( IDX_SERIAL_MON_MASTER );
						LastRequestInTimeOutError = FALSE;
					}
					SerialSend( IDX_SERIAL_MON_MASTER, SerialRequestToAsk, FinalLength );
				}
				else
				{
					sprintf( DebugMsgMaster, "monmaster: send UDP (lgt=%d, host=%s):%s \n", PureLength, InfosGUI->TargetMonitor.RemoteAdrIP, RequestToSend );
					MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, DebugMsgMaster );
					SendMonitorFrameSocketUdp( MONSOCKMASTER, RequestToSend, PureLength, InfosGUI->TargetMonitor.RemoteAdrIP, 9000 );
				}
				//free here cJSON_Print() !
				free( RequestToSend );
				// put to NULL if it was a manual frame to send !
				if ( InfosGUI->TargetMonitor.AskTargetForThisManualFrame!=NULL )
					InfosGUI->TargetMonitor.AskTargetForThisManualFrame = NULL;
				MasterStatsNbrFramesSend++;

				if ( InfosGUI->TargetMonitor.RemoteWithSerialPort[ 0 ]!='\0' )
				{
					// in serial we can/will receive the reply in many differents blocks
					int iNbrSerialBlockWait = ReplyTimeOutValueInMs/50;
					do
					{
						int SerialPartLgt = SerialReceive( IDX_SERIAL_MON_MASTER, SerialPartReceived, LGT_BUFF_RX );
if ( SerialPartLgt>0 )
{
SerialPartReceived[ SerialPartLgt ] = '\0';
sprintf(DebugMsgMaster,"SERIAL MASTER PART RESPONSE FRAME=%s\n", SerialPartReceived);
MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, DebugMsgMaster );
}
						ReqReceivedLgt = MonitorSerialDatasReceivedIsEnd( MONITOR_SERIAL_MASTER, SerialPartReceived, SerialPartLgt, RequestReceived );
						if ( ReqReceivedLgt==0 )
							DoPauseMilliSecs( 50 );
						iNbrSerialBlockWait--;
					}
					while( iNbrSerialBlockWait>0 && ReqReceivedLgt==0 );
				}
				else
				{
					ReqReceivedLgt = ReceiveMonitorFrameSocketUdp( MONSOCKMASTER, RequestReceived, LGT_BUFF_RX, ReplyTimeOutValueInMs );
				}
				
				if ( ReqReceivedLgt>0 )
				{
					RequestReceived[ ReqReceivedLgt ] = '\0'; //else will show end part of previous big request, very strange to see!
					sprintf( DebugMsgMaster, "monmaster: received (lgt=%d) :%s \n", ReqReceivedLgt, RequestReceived );
					MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, DebugMsgMaster );
					MonitorParseGeneralResponseToSwitch( RequestReceived, /*SlaveMode*/0 );
					NbrSendTry = 0;
				}
				else
				{
					MonitorSerialBuffer[MONITOR_SERIAL_MASTER].CurrLgtRecv = -1; // no frame under receive.
					LastRequestInTimeOutError = TRUE;
					NbrSendTry++;
					MasterStatsNbrFramesErrors++;
					sprintf( DebugMsgMaster, "monmaster: ##### TIMEOUT!!! (%d/%d) #####\n", MasterStatsNbrFramesErrors, MasterStatsNbrFramesSend );
					MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, DebugMsgMaster );
					if ( NbrSendTry>3 )
					{
//						strcpy( InfosGene->ErrorMsgStringToDisplay, "Too much timeouts errors with remote target..." );
						ShowMessageBoxInBackground( NULL, _("Too much timeouts errors with remote target...") );
						MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, "monmaster: ##### too much timeouts errors... disconnect! #####\n" );
						EndMasterCnxOnError( );
					}
				}
			}
#ifndef MONITOR_TEST_ONLY_NO_RESPONSES_USE
			// little pause for faster transfer with big files... (usefull for soft archives...)
			StrFileTransfer * pFileTrans = GetPtrFileTransForMaster( );
			if ( InfosGUI->TargetMonitor.RemoteFileTransfer && pFileTrans->NbrTotalBlocks>50 )
				DoPauseMilliSecs( 20 );
			else
				DoPauseMilliSecs( 100 );
//////DoPauseMilliSecs( 1000 );
#else
			DoPauseMilliSecs( 1000 );
#endif
		}
		else
		{
			DoPauseMilliSecs( 2000 );
		}
	}

	if ( RequestReceived )
		free( RequestReceived );
	if ( SerialPartReceived )
		free( SerialPartReceived );
	if ( SerialRequestToAsk )
		free( SerialRequestToAsk );
	
#ifndef __WIN32__
	pthread_exit(NULL);
#endif
}
#endif


#ifdef GTK_INTERFACE
//////gboolean DoDisconnectTargetInGtkWithLock( void )
//////{
//////gdk_threads_enter();
//////	DoFlipFlopConnectDisconnectTargetInGtk( );
//////gdk_threads_leave();
//////	return FALSE; //usefull when called with g_idle_add (just one time)
//////}

gboolean StateForGtk;
gboolean DoFlipFlopConnectDisconnectTargetInGtk( void )
{
	if (InfosGUI->TargetMonitor.RemoteConnected)
	{
		InfosGUI->TargetMonitor.RemoteConnected = FALSE;
		MessageInStatusBar( _("Target disconnected.") );
		SetGtkMenuStateForConnectDisconnectSwitch( FALSE );
		UpdateWindowTitleWithProjectName( );
// added in v0.9.10
InitVars( FALSE/*DoLogEvents*/ );
InitSystemVars( FALSE );
InitSetsVars( );
PrepareAllDatasBeforeRun( );
// calls required as not refreshed if local engine not running!
RefreshAllBoolsVars();
DisplayFreeVarSpy();
		//set back local run/stop state...
		StateForGtk = (InfosGene->LadderState==STATE_RUN)?TRUE:FALSE;
		SetGtkMenuStateForRunStopSwitch( &StateForGtk );
	}
	else
	{
		InfosGUI->TargetMonitor.TransferFileNum = -1;
		InfosGUI->TargetMonitor.LatestTargetState = -1;
//		if ( ShowEnterMessageBox( "Target to connect", "Enter IP address or hostname", InfosGUI->TargetMonitor.RemoteAdrIP ) )
//		if ( ShowEnterMessageBox( "Target to connect", "Enter serial port com...", InfosGUI->TargetMonitor.RemoteWithSerialPort ) )
		if ( OpenDialogMonitorConnect( ) )
		{
			InitVars( TRUE/*DoLogEvents*/ );
			InitSystemVars( FALSE );
			InitSetsVars( );
			PrepareAllDatasBeforeRun( );
//for that test, it is better that the motor is running, to see the JSON frames contents changing!
#ifndef MONITOR_TEST_ONLY_NO_RESPONSES_USE
			// stop local motor engine if actually running...
			StopRunIfRunning( );
#endif
			InfosGUI->TargetMonitor.RemoteConnected = TRUE;
			MessageInStatusBar( _("Target connected") ); 
			SetGtkMenuStateForConnectDisconnectSwitch( TRUE );
			UpdateWindowTitleWithProjectName( );
		}
	}
printf("flipflop cnx/decnx target = %d\n", InfosGUI->TargetMonitor.RemoteConnected );
	return FALSE; //usefull when called with g_idle_add (just one time)
}
#endif
