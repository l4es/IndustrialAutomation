/* Classic Ladder Project */
/* Copyright (C) 2001-2015 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* December 2013 */
/* --------------------------------------- */
/* Modem (Init, config, call, hangup, ...) */
/* --------------------------------------- */
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

#include "classicladder.h"
#include "global.h"
#include "serial_common.h"
#include "modem.h"
//#ifdef GTK_INTERFACE
//#include "monitor_windows_gtk.h"
//#endif
#include "frames_log_buffers.h"
#include "tasks.h"
#include "log_events.h"
StrModem Modem;

#define CHAR_RETURN '\r'

#define MODEM_NBR_STRINGS 8
#define MODEM_NUM_STR_OK 0
#define MODEM_NUM_STR_ERROR 1
#define MODEM_NUM_STR_CONNECT 2
#define MODEM_NUM_STR_RING 3
#define MODEM_NUM_STR_NO_CARRIER 4
#define MODEM_NUM_STR_NO_DIALTONE 5
#define MODEM_NUM_STR_NO_ANSWER 6
#define MODEM_NUM_STR_BUSY 7

char ModemStrings[ MODEM_NBR_STRINGS ][ MODEM_STRING_LGT ];
int IndexSearchString[ MODEM_NBR_STRINGS ];

static char ModemReturnsOk = FALSE;
static char ModemReturnsError = FALSE;
static char ModemIsConnected = FALSE;

static char ModemInitAndConfigOk = FALSE;

static int SerialNumPortForModem = -1;

static char BuffMsgDebug[ 400 ];
static char BuffSendCmd[ 350 ];

void InitModem( void )
{
	int InitStr;
	Modem.ModemUsed = FALSE;
	strcpy( Modem.StrInitSequence, "ATZ" );
	strcpy( Modem.StrConfigSequence, "ATE0;ATS0=2" );
	strcpy( Modem.StrCallSequence, "ATDT" );
	Modem.StrCodePIN[ 0 ] = '\0';
	strcpy( ModemStrings[ MODEM_NUM_STR_OK ], "OK" );
	strcpy( ModemStrings[ MODEM_NUM_STR_ERROR ], "ERROR" );
	strcpy( ModemStrings[ MODEM_NUM_STR_CONNECT ], "CONNECT*" );
	strcpy( ModemStrings[ MODEM_NUM_STR_RING ], "RING" );
	strcpy( ModemStrings[ MODEM_NUM_STR_NO_CARRIER ], "NO CARRIER" );
	strcpy( ModemStrings[ MODEM_NUM_STR_NO_DIALTONE ], "NO DIALTONE" );
	strcpy( ModemStrings[ MODEM_NUM_STR_NO_ANSWER ], "NO ANSWER" );
	strcpy( ModemStrings[ MODEM_NUM_STR_BUSY ], "BUSY" );
	for( InitStr=0; InitStr<MODEM_NBR_STRINGS; InitStr++ )
		IndexSearchString[ InitStr ] = 0;
}

void ModemDebugMsg( char Type, char * Msg ) 
{
	FrameLogString( (SerialNumPortForModem==IDX_SERIAL_MON_MASTER)?FRAMES_LOG_MONITOR_MASTER:FRAMES_LOG_MONITOR_SLAVE_SERIAL, Type, Msg );
/*#ifdef GTK_INTERFACE
	if ( SerialNumPortForModem==IDX_SERIAL_MON_MASTER )
		MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, Msg );
	else
#endif
	printf( "%s", Msg );*/
}

void AnalyseCharsReceivedFromModem( char * StrReceived, int StrLgt )
{
	int ScanChar;
	int ScanModemStr;
	for( ScanChar=0; ScanChar<StrLgt; ScanChar++ )
	{
		char CharReceived = StrReceived[ ScanChar ];
//printf("modem: char recv=%c (%02X)\n", CharReceived, CharReceived);
		for( ScanModemStr=0; ScanModemStr<MODEM_NBR_STRINGS; ScanModemStr++ )
		{
			char CurrentCharInString = ModemStrings[ ScanModemStr ][ IndexSearchString[ ScanModemStr ] ];
			if ( CharReceived==CurrentCharInString
				|| ( CurrentCharInString=='*' && CharReceived!=CHAR_RETURN ) )
			{
				if ( CurrentCharInString!='*' )
				{
					IndexSearchString[ ScanModemStr ]++;
//printf("modem: advance pos%d in string%d\n", IndexSearchString[ ScanModemStr ], ScanModemStr );
				}
			}
			else
			{
				if ( CharReceived==CHAR_RETURN )
				{
//printf("modem: seen RETURN\n");
					if ( CurrentCharInString=='\0' || CurrentCharInString=='*')
					{
						sprintf(BuffMsgDebug, "Seen string%d: %s...", ScanModemStr, ModemStrings[ ScanModemStr ] );
						ModemDebugMsg( 'i', BuffMsgDebug );

						switch( ScanModemStr )
						{
							case MODEM_NUM_STR_OK: ModemReturnsOk = TRUE; break;
							case MODEM_NUM_STR_ERROR: ModemReturnsError = TRUE; break;
							case MODEM_NUM_STR_CONNECT: ModemIsConnected = TRUE; printf( "[[ModemIsConnected=%d]]\n",ModemInitAndConfigOk ); break;
							case MODEM_NUM_STR_RING: break;
							case MODEM_NUM_STR_NO_CARRIER: ModemReturnsError = TRUE; ModemIsConnected = FALSE; break;
							case MODEM_NUM_STR_NO_DIALTONE: ModemReturnsError = TRUE; ModemIsConnected = FALSE; break;
							case MODEM_NUM_STR_NO_ANSWER: ModemReturnsError = TRUE; break;
							case MODEM_NUM_STR_BUSY: ModemReturnsError = TRUE; break;
						}
					}
				}
				IndexSearchString[ ScanModemStr ] = 0;
//printf("modem: reset pos in string%d\n", ScanModemStr );
			}
		}
	}
}

#define MODEM_PAUSE_TIME 50
// if ReturnTested==NULL, no test
char SendCommandModemNbrTriesParam( char * StrCommand, char * ReturnTested, int TimeOutSeconds, int NbrTries )
{
	char CommandResultOk = FALSE;
	int NbrErrors = 0;
	if ( SerialNumPortForModem!=-1 )
	{
		char * StartCommand = StrCommand;
		char * CurrentPos = StrCommand;
		char EndOfList = TRUE;
		// in case of received an old "OK" before...
		if ( ReturnTested )
			*ReturnTested = FALSE;
printf( "modem: CommandToSend='%s' - Timeout=%d\n", StrCommand, TimeOutSeconds );
		do
		{
			char CopyReturnTested = FALSE;
			while( *CurrentPos!='\0' && *CurrentPos!=';' )
				CurrentPos++;
			EndOfList = *CurrentPos!=';';
			// usefull not to write, if StrCommand is a const char array...
			if ( *CurrentPos!='\0' )
				*CurrentPos++ = '\0';
			do
			{
				int WaitTime = (TimeOutSeconds*1000)/MODEM_PAUSE_TIME;
				ModemReturnsError = FALSE;
				sprintf( BuffMsgDebug, "Send:%s (size=%d) (wait=%dms)", StartCommand, strlen( StartCommand ), WaitTime*MODEM_PAUSE_TIME );
				ModemDebugMsg( '<', BuffMsgDebug );
				if ( strcmp( StartCommand, "+++" )==0 || StartCommand[ strlen(StartCommand)-1 ]==0x1A /*End marker for SMS*/ )
					strcpy( BuffSendCmd, StartCommand );
				else
					sprintf( BuffSendCmd, "%s%c", StartCommand, CHAR_RETURN );
				SerialSend( SerialNumPortForModem, BuffSendCmd, strlen( BuffSendCmd ) );
				do
				{
					int LgtDatasRecv = SerialReceive( SerialNumPortForModem, BuffSendCmd, 100 );
					if ( LgtDatasRecv>0 )
					{
						BuffSendCmd[ LgtDatasRecv ] = '\0';
printf( "modem: recv:%s\n", BuffSendCmd );
						AnalyseCharsReceivedFromModem( BuffSendCmd, LgtDatasRecv );
					}
					else
					{
						DoPauseMilliSecs( MODEM_PAUSE_TIME );
						WaitTime--;
					}
					if ( ReturnTested )
						CopyReturnTested = *ReturnTested;
					else
						CopyReturnTested = TRUE;
				}
				while( WaitTime>0 && CopyReturnTested/**ReturnTested*/==FALSE && !ModemReturnsError );
				if ( ModemReturnsError )
				{
					NbrErrors++;
					ModemDebugMsg( '!', "*** ERROR RECEIVED ! ***" );
				}
				else if ( CopyReturnTested/**ReturnTested*/==FALSE )
				{
					NbrErrors++;
					ModemDebugMsg( '!', "*** TIMEOUT ! ***" );
				}
				else if ( ReturnTested==NULL )
				{
					ModemDebugMsg( '!', "*** NO REPLY CONTROL... ***" );
				}
				else
				{
					NbrErrors = 0;
					ModemDebugMsg( 'i', "*** REPLY IS OK ! ***" );
				}
			}
			while( NbrErrors<NbrTries && CopyReturnTested/**ReturnTested*/==FALSE );
			if ( !EndOfList && NbrErrors<NbrTries )
			{
				StartCommand = CurrentPos;
				if ( ReturnTested )
					*ReturnTested = FALSE;
printf("modem: go next!\n");
			}
		}
		while ( !EndOfList && NbrErrors<NbrTries );
		if ( ReturnTested )
		{
			CommandResultOk = *ReturnTested;
			*ReturnTested = FALSE;
		}
		ModemReturnsError = FALSE;
	}
printf( "modem: result=%d!\n", CommandResultOk );
	return CommandResultOk;
}
char SendCommandModem( char * StrCommand, char * ReturnTested, int TimeOutSeconds )
{
	return SendCommandModemNbrTriesParam( StrCommand, ReturnTested, TimeOutSeconds, 3/*NbrTries*/ );
}

char InitAndConfigModem( StrModem * pModemParameters )
{
	char Buff[ 100 ];
	char ModemInitOk = FALSE;
	ModemInitAndConfigOk = FALSE;
	sprintf( Buff, "%s", pModemParameters->StrInitSequence );
	if ( SendCommandModem( Buff, &ModemReturnsOk, 10/*TimeOutSeconds*/ ) )
	{
		sprintf( Buff, "%s", pModemParameters->StrConfigSequence );
		ModemInitOk = SendCommandModem( Buff, &ModemReturnsOk, 3/*TimeOutSeconds*/ );
		if ( pModemParameters->StrCodePIN[ 0 ]!='\0' )
		{
			sprintf( Buff, "AT+CPIN=%s", pModemParameters->StrCodePIN );
			ModemInitOk = SendCommandModemNbrTriesParam( Buff, &ModemReturnsOk, 3/*TimeOutSeconds*/, 2/*NbrTries*/ );
			if ( !ModemInitOk )
			{
				printf("modem: *** MODEM ERROR WITH CODE PIN - CHAT WITH MODEM SUSPENDED... ***\n");
				ModemDebugMsg( '!', "*** MODEM ERROR WITH CODE PIN - CHAT WITH MODEM SUSPENDED... ***" );
				// to avoid to block SIM with too much tries...
				while( TRUE )
					DoPauseMilliSecs( 1000 );
			}
		}
	}
	ModemInitAndConfigOk = ModemInitOk;
printf( "[[ModemInitAndConfigOk=%d]]\n",ModemInitAndConfigOk );
	if ( ModemInitOk )
		ModemDebugMsg( 'i', "*** Modem Init & Config OK ! ***" );
	return ModemInitOk;
}

char CallModem( char * CallStringCommand, char * StrTelephoneNumber )
{
	char Buff[ 100 ];
	sprintf( Buff, "%s%s", CallStringCommand, StrTelephoneNumber );
	return SendCommandModem( Buff, &ModemIsConnected, 30 );
}

void HangUpModem( )
{
	char Buff[ 100 ];
	SendCommandModem( "+++", &ModemReturnsOk, 2 );
	sprintf( Buff, "%s", "ATH" );
	SendCommandModem( Buff, &ModemReturnsOk, 2 );
	ModemIsConnected = FALSE;
}

void SetModemSerialNumPort( int NumPort )
{
	SerialNumPortForModem = NumPort;
}

char GetIfModemIsConnected( )
{
	return ModemIsConnected;
}
char GetIfModemInitAndConfigOk( )
{
	return ModemInitAndConfigOk;
}

void ModemAlarmsTransmitForSms( void )
{
	if ( InfosGene->TransmitAlarmsGlobalFlagForSms )
	{
		InfosGene->TransmitAlarmsGlobalFlagForSms = FALSE; // immediately, could be setted up before end of this function...
printf("In %s()\n",__FUNCTION__ );
		
		RemoteAlarmsTransmitScanAllEvents( RemoteAlarmsTransmitModemSendSmsIfEventConcerned );
	}
}

void RemoteAlarmsTransmitModemSendSmsIfEventConcerned( int ScanAlarmSlot, char ForEnd, StrEventLog * pEvent )
{
	// alarm slot flagged ?
	if ( RemoteAlarmsCheckTransmitFlag( ScanAlarmSlot, ForEnd, pEvent ) )
	{
		// alarm slot is of SMS type ?
		if ( RemoteAlarmsConfig.AlarmType[ ScanAlarmSlot ]==ALARMS_TYPE_SMS )
		{
			char * BuffMsgSms = malloc( 200 );
			if ( BuffMsgSms )
			{
				char BuffCommandModem[ 50 ];
	printf("In %s() log alarm (%s) to send\n",__FUNCTION__, ForEnd?"END":"BEGIN" );
				// SMS in text format
				if ( SendCommandModem( "AT+CMGF=1", &ModemReturnsOk, 2 ) )
				{
					sprintf( BuffCommandModem, "AT+CSCA=\"%s\"", RemoteAlarmsConfig.CenterServerSMS );
					if ( SendCommandModem( BuffCommandModem, &ModemReturnsOk, 2 ) )
					{
//						sprintf( BuffCommandModem, "AT+CMGS=\"%s\"", RemoteAlarmsConfig.TelephoneNumber[ ScanAlarmSlot ] );
//						SendCommandModem( BuffCommandModem, NULL/*NotTested*/, 1 );
						sprintf( BuffMsgSms, "AT+CMGS=\"%s\"%c", RemoteAlarmsConfig.TelephoneNumber[ ScanAlarmSlot ], CHAR_RETURN );
						// create event text message
//						CreateAlarmEventMessage( ALARMS_TYPE_SMS, pEvent, ForEnd, BuffMsgSms, NULL/*BuffMsgSubject*/ );
						CreateAlarmEventMessage( ALARMS_TYPE_SMS, pEvent, ForEnd, &BuffMsgSms[ strlen(BuffMsgSms ) ], NULL/*BuffMsgSubject*/ );
						strcat( BuffMsgSms, "\x1A" ); //Substitute char to signal end of SMS...
						// send it...
//						SendCommandModem( BuffMsgSms, NULL/*NotTested*/, 1 );
						if ( SendCommandModem( BuffMsgSms, &ModemReturnsOk, 10 ) )
						{
							RemoteAlarmsResetTransmitFlag( ScanAlarmSlot, ForEnd, pEvent->iIdEvent );
						}
					}
				}
//				RemoteAlarmsResetTransmitFlag( ScanAlarmSlot, ForEnd, pEvent->iIdEvent );
				free( BuffMsgSms );
			}
		}
	}
}

