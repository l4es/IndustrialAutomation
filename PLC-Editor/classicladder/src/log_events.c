/* Classic Ladder Project */
/* Copyright (C) 2001-2017 Marc Le Douarain */
/* http://www.multimania.com/mavati/classicladder */
/* http://www.sourceforge.net/projects/classicladder */
/* July 2009 */
/* ------------------------- */
/* Log of events parametered */
/* ------------------------- */
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

#ifndef MODULE
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <zlib.h>
#include <stdlib.h>
#endif

#include "classicladder.h"
#include "global.h"
#include "log_events.h"
#include "tasks.h"
#include "modem.h"
//#include "vars_system.h"
#include "time_and_rtc.h"
#include "vars_access.h"

StrLog Log;
char LogEventsDataFile[ 400 ];
StrConfigEventLog ConfigEventLog[ NBR_CONFIG_EVENTS_LOG ]; 

unsigned char ListCurrentDefType[ NBR_CURRENT_DEFS_MAX ];
int ListCurrentDefParam[ NBR_CURRENT_DEFS_MAX ];

StrRemoteAlarms RemoteAlarmsConfig;

void InitLogDatas( void )
{
	Log.LastEvent = -1;
	Log.NbrEvents = 0;
}
void InitLogConfig( void )
{
	int Scan;
	StrConfigEventLog * pCfgEvtLog;
	for( Scan=0; Scan<NBR_CONFIG_EVENTS_LOG; Scan++ )
	{
		pCfgEvtLog = &ConfigEventLog[ Scan ];
		pCfgEvtLog->FirstVarNum = -1;
		pCfgEvtLog->NbrVars = 0;
		pCfgEvtLog->EventLevel = 0;
		pCfgEvtLog->Symbol[ 0 ] = '\0';
		pCfgEvtLog->Text[ 0 ] = '\0';
		pCfgEvtLog->RemoteAlarmsForwardSlot = 0x00; // none alarms slots selected
	}
}
void InitRemoteAlarmsConfig( void )
{
	int ScanAlarmSlot;
	StrRemoteAlarms * pAlarms = &RemoteAlarmsConfig;
	pAlarms->GlobalEnabled = FALSE;
	for( ScanAlarmSlot=0; ScanAlarmSlot<NBR_ALARMS_SLOTS; ScanAlarmSlot++ )
	{
		pAlarms->SlotName[ ScanAlarmSlot ][ 0 ] = '\0';
		pAlarms->AlarmType[ ScanAlarmSlot ] = ALARMS_TYPE_NONE;
		pAlarms->TelephoneNumber[ ScanAlarmSlot ][ 0 ] = '\0';
		pAlarms->EmailAddress[ ScanAlarmSlot ][ 0 ] = '\0';
	}
	pAlarms->CenterServerSMS[ 0 ] = '\0';
	pAlarms->SmtpServerForEmails[ 0 ] = '\0';
	pAlarms->SmtpServerUserName[ 0 ] = '\0';
	pAlarms->SmtpServerPassword[ 0 ] = '\0';
	pAlarms->EmailSenderAddress[ 0 ] = '\0';
}
void AdvanceLast( void )
{
	Log.LastEvent++;
	if (Log.LastEvent>=NBR_EVENTS_IN_LOG)
		Log.LastEvent = 0;
}
char AdvanceInLog( void )
{
	char EventSlotFound = FALSE;
	// no problem if not reached the maxima
	if (Log.NbrEvents<NBR_EVENTS_IN_LOG)
	{
		Log.NbrEvents++;
		AdvanceLast( );
		EventSlotFound = TRUE;
	}
	else
	{
		StrEventLog * pEvent;
		int LoopSecurity = 0;
		do
		{
			AdvanceLast( );
			LoopSecurity++;
			pEvent = &Log.Event[ Log.LastEvent ];
			// verify here that we can not destroy this old event
			// because it is not finished...
			if ( pEvent->EndTime==0 )
			{
				pEvent->OldEventThatCouldNotBeDestroyed = TRUE;
			}
			else
			{
				// just in case, it wasn't the case before... ;-)
				pEvent->OldEventThatCouldNotBeDestroyed = FALSE;
			}
		}while( pEvent->OldEventThatCouldNotBeDestroyed && LoopSecurity<NBR_EVENTS_IN_LOG );
		if ( pEvent->OldEventThatCouldNotBeDestroyed==FALSE )
			EventSlotFound = TRUE;
	}
	return EventSlotFound;
}
/*DoModeSwitchInXenomai! TypeTime GetCurrentTime( void )
{
	return (TypeTime)time( NULL );
}*/

void CreateEventLog( int ConfigArrayNumber, int ParameterValue, char StartNewOne )
{
//	TypeTime TimeNow = GetCurrentTime( );
	TypeTime TimeNow = GetCopyCurrentIntTime( );
	StrEventLog * pEvent;
	LockTheMutex( MUTEX_LOG_EVENTS );
	if ( StartNewOne )
	{
		// Create a new event
		int iIdEvent = 0;
		// value of last index ?
		if ( Log.NbrEvents>0 )
			iIdEvent = Log.Event[ Log.LastEvent ].iIdEvent;
		iIdEvent++;
		// Whaoowwhh so much logged!?
		if ( iIdEvent<=0 )
			iIdEvent = 1;

		// Go to next slot event available
		if ( AdvanceInLog( ) )
		{
//NoPrintfBetweenMutex!!!
//printf("log a new\n");
			// Fill event datas
			pEvent = &Log.Event[ Log.LastEvent ];
			pEvent->iIdEvent = iIdEvent;
			pEvent->StartTime = TimeNow;
			pEvent->StartTimeMilliSecs = 0; //not used for now
			pEvent->EndTime = 0; //means not finished !
			pEvent->EndTimeMilliSecs = 0; //not used for now
			pEvent->ConfigArrayNum = ConfigArrayNumber;
			pEvent->Parameter = ParameterValue;
			pEvent->OldEventThatCouldNotBeDestroyed = FALSE;
			pEvent->StartTransmitAlarmsFlags = 0x00;
			pEvent->EndTransmitAlarmsFlags = 0x00;
			if ( RemoteAlarmsConfig.GlobalEnabled )
			{
				pEvent->StartTransmitAlarmsFlags = ConfigEventLog[ ConfigArrayNumber ].RemoteAlarmsForwardSlot;
				SetTransmitAlarmsGlobalFlag( pEvent->StartTransmitAlarmsFlags );
			}
		}
	}
	else
	{
		// End an event (first search for its start...)
		// It not event one even, the search is fast!
		if (Log.NbrEvents>0)
		{
			char FoundIt = FALSE;
			int NbrEventsRemaining = Log.NbrEvents;
			int ScanEvent = Log.LastEvent;
			do
			{
			    pEvent = &Log.Event[ ScanEvent ];
				// the same ? and not already disapeared ?
				if ( pEvent->ConfigArrayNum==ConfigArrayNumber && pEvent->Parameter==ParameterValue && pEvent->EndTime==0 )
					FoundIt = TRUE;
				NbrEventsRemaining--;
				ScanEvent--;
				if (ScanEvent<0)
					ScanEvent = NBR_EVENTS_IN_LOG-1;
			}
			while( !FoundIt && NbrEventsRemaining>0 );
			if (FoundIt)
			{
//NoPrintfBetweenMutex!!!
//printf("log the end\n");
				// Fill event datas
				pEvent->EndTime = TimeNow;
				if ( RemoteAlarmsConfig.GlobalEnabled )
				{
					pEvent->EndTransmitAlarmsFlags = ConfigEventLog[ ConfigArrayNumber ].RemoteAlarmsForwardSlot;
					SetTransmitAlarmsGlobalFlag( pEvent->EndTransmitAlarmsFlags );
				}
			}
		}
	}
	UnlockTheMutex( MUTEX_LOG_EVENTS );
	InfosGene->BackgroundSaveLogEventsData = TRUE;
}

void SetTransmitAlarmsGlobalFlag( unsigned char SlotsSelected )
{
	int ScanAlarmSlot;
//	printf("TEMP DEBUG!!!!! Verify if global flag log alarm to set\n");
	for( ScanAlarmSlot=0; ScanAlarmSlot<NBR_ALARMS_SLOTS; ScanAlarmSlot++ )
	{
		if ( SlotsSelected & (1<<ScanAlarmSlot ) )
		{
//NoPrintfBetweenMutex!!!
			if ( RemoteAlarmsConfig.AlarmType[ ScanAlarmSlot ]==ALARMS_TYPE_SMS )
			{
				InfosGene->TransmitAlarmsGlobalFlagForSms = TRUE;
//NoPrintfBetweenMutex!!!
//		printf("TEMP DEBUG!!!!! Setted global flag log alarm for SMS\n");
			}
			if ( RemoteAlarmsConfig.AlarmType[ ScanAlarmSlot ]==ALARMS_TYPE_EMAIL )
			{
				InfosGene->TransmitAlarmsGlobalFlagForEmails = TRUE;
//NoPrintfBetweenMutex!!!
//				printf("TEMP DEBUG!!!!! Setted global flag log alarm for Emails\n");
			}
		}
	}
}

// enquire the vars that user want to log
// to immediately in WriteVar( ) know that VarStateChanged has to been called...
void InitVarsArrayLogTags( void )
{
	int ScanLogVar;
	int ScanConfigNum;
	StrConfigEventLog * pCfgEvtLog;
	//none to log for now...
	for( ScanLogVar=0; ScanLogVar<SIZE_VAR_ARRAY; ScanLogVar++ )
		LogVarArray[ ScanLogVar ] = 0;
	//set true the ones that must be logged
	for( ScanConfigNum=0; ScanConfigNum<NBR_CONFIG_EVENTS_LOG; ScanConfigNum++ )
	{
		pCfgEvtLog = &ConfigEventLog[ ScanConfigNum ];
		for( ScanLogVar=pCfgEvtLog->FirstVarNum; ScanLogVar<pCfgEvtLog->FirstVarNum+pCfgEvtLog->NbrVars; ScanLogVar++ )
			LogVarArray[ ScanLogVar ] = 1;
	}
}

//Type not used, see why in classicladder.h
void VarStateChanged( int Type, int Num, int NewStateValue )
{
	int ScanConfigNum = 0;
	char FoundIt = FALSE;
	StrConfigEventLog * pCfgEvtLog;
//ToAvoidModeSwitchInXenomai! printf("state of var B%d changed\n", Num);
	do
	{
		pCfgEvtLog = &ConfigEventLog[ ScanConfigNum ];
		if ( pCfgEvtLog->FirstVarNum!=-1 && pCfgEvtLog->NbrVars>0 )
		{
			if ( pCfgEvtLog->FirstVarNum<=Num && Num<pCfgEvtLog->FirstVarNum+pCfgEvtLog->NbrVars )
				FoundIt = TRUE;
		}
		if ( !FoundIt )
			ScanConfigNum++;
	}
	while( ScanConfigNum<NBR_CONFIG_EVENTS_LOG && !FoundIt );
	if ( FoundIt )
	{
		int ParameterValue = -1;
		if ( pCfgEvtLog->NbrVars>1 )
			ParameterValue = Num-pCfgEvtLog->FirstVarNum;
//ToAvoidModeSwitchInXenomai! printf("will now log it (param=%d)!\n",ParameterValue);
		CreateEventLog( ScanConfigNum, ParameterValue, NewStateValue );
		//Ask to update log window display !
		InfosGene->LogContentModified = TRUE;
		// if it is a default even type, ask to update the display list !
		if ( pCfgEvtLog->EventLevel>0 )
			InfosGene->DefaultLogListModified = TRUE;
	}
	else
	{
//ToAvoidModeSwitchInXenomai! printf("var (%d/%d) not found in config log array... strange...\n",Type,Num);
	}
}


void CleanupEventsLog( void )
{
	StrEventLog * pEvent;
	StrConfigEventLog * pCfgEvtLog;
//	TypeTime TimeNow = GetCurrentTime( );
/*	TypeTime TimeNow = GetCurrentIntTime( );*/
	int NbrEventsRemaining = Log.NbrEvents;
	int ScanEvent = Log.LastEvent;
	char EmptyLog = TRUE;
	if ( NbrEventsRemaining>0  )
	{
printf("%s: analyzing log content...\n", __FUNCTION__);
		do
		{
		    pEvent = &Log.Event[ ScanEvent ];
			pCfgEvtLog = &ConfigEventLog[ pEvent->ConfigArrayNum ];
		    // this event do no more exist in the config? we kill it !
		    if( pCfgEvtLog->FirstVarNum==-1 || pCfgEvtLog->NbrVars==0 || pEvent->Parameter>=pCfgEvtLog->NbrVars )
		    {
printf("%s: kill event id%d - for old config %d(value=%d)\n", __FUNCTION__, pEvent->iIdEvent, pEvent->ConfigArrayNum, pEvent->Parameter);
				pEvent->StartTime = 0;
				pEvent->EndTime = 0;
				pEvent->StartTransmitAlarmsFlags = 0x00;
				pEvent->EndTransmitAlarmsFlags = 0x00;
			}
/*NoMoreNow: keep and then set corresponding var to 1...			else
			{
				// if this event not finished, finish it now!
				// (we suppose all variables start with 0 value, so no pending event)
				if ( pEvent->EndTime==0 )
				{
printf("%s: end event id%d not previously finished - config %s(value=%d)\n", __FUNCTION__, pEvent->iIdEvent, pCfgEvtLog->Symbol, pEvent->Parameter );
					pEvent->EndTime = TimeNow;
					pEvent->EndTransmitAlarmsFlags = 0x00;
				}
			}*/
			NbrEventsRemaining--;
			ScanEvent--;
			if (ScanEvent<0)
				ScanEvent = NBR_EVENTS_IN_LOG-1;
		}
		while( NbrEventsRemaining>0 );

		// verify if all killed, so that we reinit all
		NbrEventsRemaining = Log.NbrEvents;
		ScanEvent = Log.LastEvent;
		do
		{
		    pEvent = &Log.Event[ ScanEvent ];
		    if ( pEvent->StartTime!=0 )
				EmptyLog = FALSE;
			NbrEventsRemaining--;
			ScanEvent--;
			if (ScanEvent<0)
				ScanEvent = NBR_EVENTS_IN_LOG-1;
		}
		while( NbrEventsRemaining>0 && EmptyLog );
		if ( EmptyLog )
		{
printf("%s: see empty, so init all.\n", __FUNCTION__);
			Log.LastEvent = -1;
			Log.NbrEvents = 0;
		}
		//Ask to update log window display !
		InfosGene->LogContentModified = TRUE;
	}
}
// added in v0.9.101, to set var to 1 if the event associated is pending...
// (allow to save var state...)
void WriteBoolVarsTo1FromPendingEventsLog( void )
{
	StrEventLog * pEvent;
	StrConfigEventLog * pCfgEvtLog;
	int NbrEventsRemaining = Log.NbrEvents;
	int ScanEvent = Log.LastEvent;
	if ( NbrEventsRemaining>0  )
	{
printf("%s: analyzing log content...\n", __FUNCTION__);
		do
		{
		    pEvent = &Log.Event[ ScanEvent ];
			pCfgEvtLog = &ConfigEventLog[ pEvent->ConfigArrayNum ];
			// if this event not finished, set boolean var associated to 1
			if ( pEvent->StartTime!=0 && pEvent->EndTime==0 )
			{
				int NumVar = pCfgEvtLog->FirstVarNum;
				// beware of parameter=-1 if only one var !
				if ( pCfgEvtLog->NbrVars>1 )
					NumVar = NumVar+pEvent->Parameter;
				WriteBoolVarWithoutLog( NumVar, 1 );
printf("%s: set var from pending event id%d (not previously finished) - config %s(var=%%B%d)\n", __FUNCTION__, pEvent->iIdEvent, pCfgEvtLog->Symbol, NumVar );
			}
			NbrEventsRemaining--;
			ScanEvent--;
			if (ScanEvent<0)
				ScanEvent = NBR_EVENTS_IN_LOG-1;
		}
		while( NbrEventsRemaining>0 );
	}
}

StrLog * GetCopyLogDatasToFreeAfterUse( void )
{
	StrLog * pLogCopy = malloc( sizeof( StrLog ) );
	if ( pLogCopy )
	{
		LockTheMutex( MUTEX_LOG_EVENTS );
		memcpy( (void *)pLogCopy, (void *)&Log, sizeof( StrLog ) );
		UnlockTheMutex( MUTEX_LOG_EVENTS );
	}
	return pLogCopy;
}

int FindCurrentDefaults( void )
{
	int NbrDefsFound = 0;
	StrEventLog * pEvent;
	StrConfigEventLog * pCfgEvtLog;
	if (Log.NbrEvents>0)
	{
//		LockTheMutex( MUTEX_LOG_EVENTS );
//TODO, should copy the log datas in a local mirror allocated used after...
//		int NbrEventsRemaining = Log.NbrEvents;
//		int ScanEvent = Log.LastEvent;
//		UnlockTheMutex( MUTEX_LOG_EVENTS );
		// work on a log datas copy, so that real log can continue to live safely...
		StrLog * pLogLocalCopy = GetCopyLogDatasToFreeAfterUse( );
		if ( pLogLocalCopy )
		{
			int NbrEventsRemaining = pLogLocalCopy->NbrEvents;
			int ScanEvent = pLogLocalCopy->LastEvent;
			do
			{
				pEvent = &pLogLocalCopy->Event[ ScanEvent ];
				pCfgEvtLog = &ConfigEventLog[ pEvent->ConfigArrayNum ];
				// not disapeared ? -and- is a default type ?
				if ( pEvent->EndTime==0 && pCfgEvtLog->EventLevel>0 )
				{
					ListCurrentDefType[ NbrDefsFound ] = pEvent->ConfigArrayNum;
					ListCurrentDefParam[ NbrDefsFound++ ] = pEvent->Parameter;
				}
				NbrEventsRemaining--;
				ScanEvent--;
				if (ScanEvent<0)
					ScanEvent = NBR_EVENTS_IN_LOG-1;
			}
			while( NbrEventsRemaining>0 && NbrDefsFound<NBR_CURRENT_DEFS_MAX );
			free( pLogLocalCopy );
		}
	}
	return NbrDefsFound;
}

void LoadLogEventsData( void )
{
	FILE * File;
	char BuffHeaderVer[ 10 ];
	File = fopen(LogEventsDataFile,"rb");
	if (File)
	{
		fread( BuffHeaderVer, 3, 1, File);
		if ( BuffHeaderVer[ 0 ]=='V' && BuffHeaderVer[ 1 ]=='0' && BuffHeaderVer[ 2 ]=='0' )
		{
			fread(&Log, sizeof(StrLog), 1, File);
		}
		fclose(File);
	}
}
void SaveLogEventsData( void )
{
	FILE * File;
	StrLog * pLogLocalCopy = GetCopyLogDatasToFreeAfterUse( );
	if ( pLogLocalCopy )
	{
		File = fopen(LogEventsDataFile,"wb");
		if (File)
		{
			fwrite("V00", 3, 1, File);
			fwrite(pLogLocalCopy, sizeof(StrLog), 1, File);
			fclose(File);
		}
		else
		{
			printf("FAILED to open log file to write !!!\n");
		}
		free( pLogLocalCopy );
	}
}

char SaveLogEventsTextFile( char * File, char Compressed )
{
	gzFile pLogFileGz = NULL;
	FILE * pLogFile = NULL;
	if ( Compressed )
		pLogFileGz = gzopen( File, "wt" );
	else
		pLogFile = fopen( File, "wt" );
	if ( pLogFileGz || pLogFile )
	{
		char Buff[ 200 ];
		StrLog * pLogLocalCopy = GetCopyLogDatasToFreeAfterUse( );
		if ( pLogLocalCopy )
		{
			StrEventLog * pEvent;
			StrConfigEventLog * pCfgEvtLog;
			int NbrEventsRemaining = pLogLocalCopy->NbrEvents;
			int ScanEvent = pLogLocalCopy->LastEvent;
			if ( NbrEventsRemaining>0  )
			{
				char BuffTemp[ 30 ];
				do
				{
					pEvent = &pLogLocalCopy->Event[ ScanEvent ];
					pCfgEvtLog = &ConfigEventLog[ pEvent->ConfigArrayNum ];
					sprintf( Buff, "%d,", pEvent->iIdEvent );
					ConvertIntTimeToAsc( pEvent->StartTime, BuffTemp, TRUE/*WithDate*/, FALSE/*InUTC*/ );
					strcat( Buff, BuffTemp );
					strcat( Buff, "," );
					if ( pEvent->EndTime!=0 )
						ConvertIntTimeToAsc( pEvent->EndTime, BuffTemp, TRUE/*WithDate*/, FALSE/*InUTC*/ );
					else
						strcpy( BuffTemp, "***not finished***" );
					strcat( Buff, BuffTemp );
					strcat( Buff, "," );
					strcat( Buff, pCfgEvtLog->Symbol );
					strcat( Buff, "," );
					sprintf( BuffTemp, "%d,", pEvent->Parameter );
					strcat( Buff, BuffTemp );
					strcat( Buff, pCfgEvtLog->Text );
					strcat( Buff, "," );
					sprintf( BuffTemp, "%d", pCfgEvtLog->EventLevel );
					strcat( Buff, BuffTemp );
					strcat( Buff, "\n" );
					if ( Compressed )
						gzputs( pLogFileGz, Buff );
					else
						fputs( Buff, pLogFile );
printf("SaveCsvLog: %s\n", Buff );

					NbrEventsRemaining--;
					ScanEvent--;
					if (ScanEvent<0)
						ScanEvent = NBR_EVENTS_IN_LOG-1;
				}
				while( NbrEventsRemaining>0 );
			}
			else
			{
				strcpy( Buff, "0,---,---,---Empty Log---,0,---,0" );
				if ( Compressed )
					gzputs( pLogFileGz, Buff );
				else
					fputs( Buff, pLogFile );
			}
			free( pLogLocalCopy );
		}
		else
		{
			strcpy( Buff, "0,---,---,---Memory error---,0,---,0" );
			if ( Compressed )
				gzputs( pLogFileGz, Buff );
			else
				fputs( Buff, pLogFile );
		}
		if ( Compressed )
			gzclose( pLogFileGz );
		else
			fclose( pLogFile );
	}
	return ( pLogFileGz!=NULL || pLogFile!=NULL );
}

char ConvertCompressedCsvToUncompressedCsv( char * FileGz, char * NewFileCsv )
{
	gzFile pLogFileGz;
	FILE * pNewLogFileCsv;
	char Buff[ 200 ];
	pLogFileGz = gzopen( FileGz, "rt" );
	if ( pLogFileGz==NULL )
		return FALSE;

	pNewLogFileCsv = fopen( NewFileCsv, "wt" );
	if ( pNewLogFileCsv==NULL )
	{
		gzclose( pLogFileGz );
		return FALSE;
	}

	while( gzgets( pLogFileGz, Buff, 200 ) )
	{
		fputs( Buff, pNewLogFileCsv );
	}
	fclose( pNewLogFileCsv );
	gzclose( pLogFileGz );
	return TRUE;
}

void CreateAlarmEventMessage( int AlarmType, StrEventLog * pEvent, char ForEnd, char * BuffMsg, char * BuffMsgSubject )
{
	char BuffTemp[ 80 ];
	char BuffParam[ 20 ];
	const char * EndOfLine = (AlarmType==ALARMS_TYPE_SMS)?" \r\n":"\n";
	BuffMsg[ 0 ] = '\0';
	if ( BuffMsgSubject )
		BuffMsgSubject[ 0 ] = '\0';
	if ( InfosGene->ProjectProperties.ProjectName[0]!='\0' )
	{
		strcat( BuffMsg, InfosGene->ProjectProperties.ProjectName );
		strcat( BuffMsg, EndOfLine );
		if ( BuffMsgSubject )
		{
			strcat( BuffMsgSubject, InfosGene->ProjectProperties.ProjectName );
			strcat( BuffMsgSubject, " " );
		}
	}
	if ( InfosGene->ProjectProperties.ProjectSite[0]!='\0' )
	{
		strcat( BuffMsg, InfosGene->ProjectProperties.ProjectSite );
		strcat( BuffMsg, EndOfLine );
		if ( BuffMsgSubject )
		{
			strcat( BuffMsgSubject, InfosGene->ProjectProperties.ProjectSite );
			strcat( BuffMsgSubject, " " );
		}
	}
	ConvertIntTimeToAsc( ForEnd?pEvent->EndTime:pEvent->StartTime, BuffTemp, TRUE/*WithDate*/, FALSE/*InUTC*/ );
	strcat( BuffMsg, BuffTemp );
	strcat( BuffMsg, EndOfLine );
	BuffParam[ 0 ] = '\0';
	if ( pEvent->Parameter!=-1 )
		sprintf( BuffParam, "/%d", pEvent->Parameter );
	sprintf( BuffTemp, "%s %s%s", ForEnd?"End":"Start", ConfigEventLog[ pEvent->ConfigArrayNum ].Symbol, BuffParam );
	strcat( BuffMsg, BuffTemp );
	strcat( BuffMsg, EndOfLine );
	if ( BuffMsgSubject )
	{
		sprintf( BuffTemp, "%s %s%s", ForEnd?"End":"Start", ConfigEventLog[ pEvent->ConfigArrayNum ].Symbol, BuffParam );
		strcat( BuffMsgSubject, BuffTemp );
		strcat( BuffMsgSubject, " " );
	}
	strcat( BuffMsg, ConfigEventLog[ pEvent->ConfigArrayNum ].Text );
printf("%s(): strlen_msg=%d, strlen_subject=%d\n", __FUNCTION__, strlen(BuffMsg), BuffMsgSubject?strlen(BuffMsgSubject):-1);
}


void RemoteAlarmsTransmitScanAllEvents( FunctionAlarmTransmitToCall FunctionToCall )
{
	// scan all events, for SMS events to transmit
	if (Log.NbrEvents>0)
	{
		// work on a log datas copy, so that real log can continue to live safely...
		StrLog * pLogLocalCopy = GetCopyLogDatasToFreeAfterUse( );
		if ( pLogLocalCopy )
		{
			
			int NbrEventsRemaining = pLogLocalCopy->NbrEvents;
			int ScanEvent = pLogLocalCopy->LastEvent;
printf("In %s() search log alarms\n",__FUNCTION__ );
			do
			{
				StrEventLog * pEvent = &pLogLocalCopy->Event[ ScanEvent ];
				// to speed up, just see if this event has something... in detail, later!
				if ( pEvent->StartTransmitAlarmsFlags!=0 || pEvent->EndTransmitAlarmsFlags!=0 )
				{
					int ScanAlarmSlot;
					for( ScanAlarmSlot=0; ScanAlarmSlot<NBR_ALARMS_SLOTS; ScanAlarmSlot++ )
					{
						FunctionToCall( ScanAlarmSlot, FALSE/*ForEnd*/, pEvent );
						FunctionToCall( ScanAlarmSlot, TRUE/*ForEnd*/, pEvent );
//						ModemAlarmsTransmitSendSmsIfEventConcerned( ScanAlarmSlot, FALSE/*ForEnd*/, pEvent, BuffMsgSms );
//						ModemAlarmsTransmitSendSmsIfEventConcerned( ScanAlarmSlot, TRUE/*ForEnd*/, pEvent, BuffMsgSms );
					}
				}
				NbrEventsRemaining--;
				ScanEvent--;
				if (ScanEvent<0)
					ScanEvent = NBR_EVENTS_IN_LOG-1;
			}
			while( NbrEventsRemaining>0 );
			free( pLogLocalCopy );
			
		}
	}
}

char RemoteAlarmsCheckTransmitFlag( int ScanAlarmSlot, char ForEnd, StrEventLog * pEvent )
{
	unsigned char TransmitAlarmsFlags;
	if ( ForEnd )
		TransmitAlarmsFlags = pEvent->EndTransmitAlarmsFlags;
	else
		TransmitAlarmsFlags = pEvent->StartTransmitAlarmsFlags;
	
	// alarm slot flagged ?
	if ( TransmitAlarmsFlags & (1<<ScanAlarmSlot ) )
		return TRUE;
	else
		return FALSE;
}

// as we work on a log datas copy, we must reset flag on real Log datas (by searching IdEvent)
void RemoteAlarmsResetTransmitFlag( int ScanAlarmSlot, char ForEnd, int iIdEventSearched )
{
	LockTheMutex( MUTEX_LOG_EVENTS );
	int NbrEventsRemaining = Log.NbrEvents;
	int ScanEvent = Log.LastEvent;
	char EventFound = FALSE;
	do
	{
		StrEventLog * pEvent = &Log.Event[ ScanEvent ];
		if ( pEvent->iIdEvent==iIdEventSearched )
		{
			EventFound = TRUE;
//printf("In %s() log alarm (%s) found real event id%d to reset flag\n",__FUNCTION__, ForEnd?"END":"BEGIN", iIdEventSearched );
			if ( ForEnd )
				pEvent->EndTransmitAlarmsFlags = pEvent->EndTransmitAlarmsFlags & (~(1<<ScanAlarmSlot));
			else
				pEvent->StartTransmitAlarmsFlags = pEvent->StartTransmitAlarmsFlags & (~(1<<ScanAlarmSlot));
		}
		else
		{
			NbrEventsRemaining--;
			ScanEvent--;
			if (ScanEvent<0)
				ScanEvent = NBR_EVENTS_IN_LOG-1;
		}
	}
	while( NbrEventsRemaining>0 && !EventFound );
	UnlockTheMutex( MUTEX_LOG_EVENTS );
	if ( EventFound )
	{
		InfosGene->BackgroundSaveLogEventsData = TRUE;
	}
	else
	{
		//Debug Printf after real-time Xenomai Mutex released!!!
		printf("!!!ERROR in %s() log alarm (%s) NOT FOUND real event id%d to reset flag\n",__FUNCTION__, ForEnd?"END":"BEGIN", iIdEventSearched );
	}
}


void RemoteAlarmsTransmitForEmails( void )
{
	if ( InfosGene->TransmitAlarmsGlobalFlagForEmails )
	{
		InfosGene->TransmitAlarmsGlobalFlagForEmails = FALSE; // immediately, could be setted up before end of this function...
printf("In %s()\n",__FUNCTION__ );
		
		RemoteAlarmsTransmitScanAllEvents( RemoteAlarmsTransmitSendEmailIfEventConcerned );
	}
}
void RemoteAlarmsTransmitSendEmailIfEventConcerned( int ScanAlarmSlot, char ForEnd, StrEventLog * pEvent )
{
	// alarm slot flagged ?
	if ( RemoteAlarmsCheckTransmitFlag( ScanAlarmSlot, ForEnd, pEvent ) )
	{
		// alarm slot is of Email type ?
		if ( RemoteAlarmsConfig.AlarmType[ ScanAlarmSlot ]==ALARMS_TYPE_EMAIL )
		{
			char * BuffMsgText = malloc( 300 );
			if ( BuffMsgText )
			{
				char * BuffMsgSubject = malloc( 200 );
				if ( BuffMsgSubject )
				{
					char SiteName[ 20+LGT_STR_INFO];
					StrRemoteAlarms * pAlarms = &RemoteAlarmsConfig;
					strcpy( SiteName, "ClassicLadder" );
					if ( InfosGene->ProjectProperties.ProjectSite[0]!='\0' )
					{
						strcat( SiteName, " " );
						strcat( SiteName, InfosGene->ProjectProperties.ProjectSite );
					}
					// create event text message
					CreateAlarmEventMessage( ALARMS_TYPE_EMAIL, pEvent, ForEnd, BuffMsgText, BuffMsgSubject );
					
printf("Preparing command to send email...\n");
					LaunchExternalCommand( "/usr/bin/mailsend" );
					LaunchExternalCommand( "-name" );
					LaunchExternalCommand( SiteName );
					LaunchExternalCommand( "-f" );
					LaunchExternalCommand( pAlarms->EmailSenderAddress );
					LaunchExternalCommand( "-smtp" );
					LaunchExternalCommand( pAlarms->SmtpServerForEmails );
					if ( pAlarms->SmtpServerUserName[0]!='\0' )
					{
						LaunchExternalCommand( "-auth" );
						LaunchExternalCommand( "-user" );
						LaunchExternalCommand( pAlarms->SmtpServerUserName );
						LaunchExternalCommand( "-pass" );
						LaunchExternalCommand( pAlarms->SmtpServerPassword );
					}
					LaunchExternalCommand( "-t" );
					LaunchExternalCommand( pAlarms->EmailAddress[ ScanAlarmSlot ] );
					LaunchExternalCommand( "+bc" );
					LaunchExternalCommand( "+cc" );
					LaunchExternalCommand( "-sub" );
					LaunchExternalCommand( BuffMsgSubject );
					LaunchExternalCommand( "-M" );
					strcat( BuffMsgText, "\t" ); // = end of command list !
					LaunchExternalCommand( BuffMsgText );

					RemoteAlarmsResetTransmitFlag( ScanAlarmSlot, ForEnd, pEvent->iIdEvent );
					free( BuffMsgSubject );
				}
				free( BuffMsgText );
			}
		}
	}
}
