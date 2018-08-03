/* Classic Ladder Project */
/* Copyright (C) 2001-2017 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* February 2011 */
/* ------------------------------------------------------ */
/* Monitor protocol - All the requests...                 */
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
#include <time.h>
//#include <sys/time.h>
#ifdef GTK_INTERFACE
#include <libintl.h> // i18n
#include <locale.h> // i18n
#endif

#include "classicladder.h"
#include "global.h"
#include "vars_access.h"
#include "monitor_protocol.h"
#include "monitor_transfer.h"
#include "tasks.h"
#include "files.h"
#include "preferences.h"
//#include "vars_system.h"
#include "time_and_rtc.h"
#include "files_project.h"
#include "socket_modbus_master.h"
#include "log_events.h"
#include "frames_log_buffers.h"
#ifdef GTK_INTERFACE
#include <gtk/gtk.h>
#include "menu_and_toolbar_gtk.h"
#include "classicladder_gtk.h"
#include "monitor_windows_gtk.h"
#else
#include <sys/types.h>
#include <signal.h>
#endif
#ifndef __WIN32__
// for statfs() function
#include <sys/vfs.h>
// for reboot()
#include <unistd.h>
//#include <linux/reboot.h>
#include <sys/reboot.h>
#endif

// look at interesting MONITOR_TEST_ONLY_NO_RESPONSES_USE in classicladder.h to debug frames
// with just one executable connecting to itself...

// optional ID for each frame exchanged
#define DEBUG_FRAME_ID_ADDED

#ifdef DEBUG_FRAME_ID_ADDED
int NumFrame = 0;
#endif

// SlaveMode = 0 if "master", 1 if "slave/IP", 2 if "slave/serial"...
char * MonitorParseGeneralResponseToSwitch( char * TextReq, char SlaveMode )
{
	char * TextReqWithValues = NULL;
	cJSON *JsonRoot;
	JsonRoot = cJSON_Parse(TextReq);
	if ( JsonRoot )
	{
		char RequestErrorFromSlave = FALSE;
		if ( !SlaveMode )
		{
			cJSON * CommandReqErrorObj = cJSON_GetObjectItem( JsonRoot,"ReqError" );
			if ( CommandReqErrorObj )
			{
				printf("monmaster: ****** RECEIVE ERROR EXCEPTION FROM THE SLAVE (ERROR=%d) ******\n",CommandReqErrorObj->valueint);
#ifdef GTK_INTERFACE
				MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, "monmaster: ****** RECEIVE ERROR EXCEPTION FROM THE SLAVE ******\n" );
#endif
				RequestErrorFromSlave = TRUE;
			}
		}
		if ( !RequestErrorFromSlave )
		{
			cJSON * CommandReqObj = cJSON_GetObjectItem( JsonRoot,"ReqCL" );
			if ( CommandReqObj )
			{
				char UnformattedToBeMoreCompact = FALSE;
				char * CommandReq = CommandReqObj->valuestring;
				if ( strcmp( CommandReq, "RdVars" )==0 )
				{
					MonitorParseResponseReadVars( JsonRoot, SlaveMode );
					UnformattedToBeMoreCompact = TRUE;
				}
				else if ( strcmp( CommandReq, "RdRung" )==0 )
				{
					MonitorParseResponseReadRung( JsonRoot, SlaveMode );
					UnformattedToBeMoreCompact = TRUE;
				}
				else if ( strcmp( CommandReq, "RdSeq" )==0 )
				{
					MonitorParseResponseReadSequential( JsonRoot, SlaveMode );
					UnformattedToBeMoreCompact = TRUE;
				}
				else if ( strcmp( CommandReq, "SetRunStopState" )==0 )
				{
					MonitorParseResponseSetRunStopState( JsonRoot, SlaveMode );
				}
				else if ( strcmp( CommandReq, "ResetTarget" )==0 )
				{
					MonitorParseResponseResetTarget( JsonRoot, SlaveMode );
				}
				else if ( strcmp( CommandReq, "SetClockTime" )==0 )
				{
					MonitorParseResponseSetClockTime( JsonRoot, SlaveMode );
				}
				else if ( strcmp( CommandReq, "GetTargetInfosVersion" )==0 )
				{
					MonitorParseResponseGetTargetInfosVersion( JsonRoot, SlaveMode );
				}
				else if ( strcmp( CommandReq, "GetProjectInfos" )==0 )
				{
					MonitorParseResponseGetProjectProperties( JsonRoot, SlaveMode );
				}
				else if ( strcmp( CommandReq, "Transfer" )==0 )
				{
					MonitorParseResponseFileTransfer( JsonRoot, SlaveMode );
				}
				else if ( strcmp( CommandReq, "RebootTarget" )==0 )
				{
					MonitorParseResponseRebootTarget( JsonRoot, SlaveMode );
				}
				else if ( strcmp( CommandReq, "WriteVarValue" )==0 )
				{
					MonitorParseResponseWriteVarValue( JsonRoot, SlaveMode );
				}
				else if ( strcmp( CommandReq, "SetOrUnsetVar" )==0 )
				{
					MonitorParseResponseSetOrUnsetVar( JsonRoot, SlaveMode );
				}
				else if ( strcmp( CommandReq, "GetNetworkConfig" )==0 )
				{
					MonitorParseResponseGetNetworkConfig( JsonRoot, SlaveMode );
				}
				else if ( strcmp( CommandReq, "WriteNetworkConfig" )==0 )
				{
					MonitorParseResponseWriteNetworkConfig( JsonRoot, SlaveMode );
				}
				else if ( strcmp( CommandReq, "GetMonitorSerialConfig" )==0 )
				{
					MonitorParseResponseGetMonitorSerialConfig( JsonRoot, SlaveMode );
				}
				else if ( strcmp( CommandReq, "WriteMonitorSerialConfig" )==0 )
				{
					MonitorParseResponseWriteMonitorSerialConfig( JsonRoot, SlaveMode );
				}
				else if ( strcmp( CommandReq, "GetStatsModbusM" )==0 )
				{
					MonitorParseResponseGetModbusMasterStats( JsonRoot, SlaveMode );
					UnformattedToBeMoreCompact = TRUE;
				}
				else if ( strcmp( CommandReq, "CleanUp" )==0 )
				{
					MonitorParseResponseCleanUp( JsonRoot, SlaveMode );
				}
				else
				{
					printf("mon: ****** FAILED TO FIND REQUEST TO PARSE !!! ******\n");
					FrameLogString( NUM_FRAME_LOG_FOR_MON(SlaveMode), 'i', "****** FAILED TO FIND REQUEST TO PARSE !!! ******" );
					cJSON_AddNumberToObject( JsonRoot, "ReqError", 1/*UnknownRequest!*/ );
				}

				if ( SlaveMode )
				{
					if ( UnformattedToBeMoreCompact )
						TextReqWithValues = cJSON_PrintUnformatted( JsonRoot );
					else
						TextReqWithValues = cJSON_Print( JsonRoot );
				}
			}
			else
			{
				printf( "mon: ****** FAILED TO FIND 'ReqCL' KEYWORD TAG !!! ******\n" );
				FrameLogString( NUM_FRAME_LOG_FOR_MON(SlaveMode), 'i', "****** FAILED TO FIND 'ReqCL' KEYWORD TAG !!! ******" );
			}
		}
		cJSON_Delete( JsonRoot );
	}
	else
	{
		printf( "mon: ****** FAILED TO PARSE JSON (for %s) !!! ******\n", SlaveMode?"slave":"master" );
		FrameLogString( NUM_FRAME_LOG_FOR_MON(SlaveMode), 'i', "****** ****** FAILED TO PARSE JSON !!! ******" );
	}
	return TextReqWithValues;
}


// used to exchange the "free vars spy" + system time vars + functions blocks displayed on screen...
#define NBR_FREE_VARS_READ ( NBR_FREE_VAR_SPY + 3 + NBR_FUNCTIONS_BLOCKS_VARS_TO_MONITOR )
//for the master, to ask to read variables values
char * MonitorCreateRequestReadVars( void )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq;
	int ScanColumn = 0;
	int ArrayFirstsBoolsAndNbr[ 6 ];
	int FreeVarsType[ NBR_FREE_VARS_READ ];
	int FreeVarsNum[ NBR_FREE_VARS_READ ];
	int NbrFreeVars = 0;
	int ScanVar;
	
	JsonRoot = cJSON_CreateObject();
	
	cJSON_AddStringToObject( JsonRoot, "ReqCL", "RdVars" );
	
#ifdef DEBUG_FRAME_ID_ADDED
	cJSON_AddNumberToObject( JsonRoot, "FrameID", NumFrame++ );
#endif

	// to read all bools vars (bools, inputs & outputs)
	ArrayFirstsBoolsAndNbr[ ScanColumn ] = InfosGUI->ValOffsetBoolVar[ ScanColumn ];
	ScanColumn++;
	ArrayFirstsBoolsAndNbr[ ScanColumn ] = InfosGUI->ValOffsetBoolVar[ ScanColumn ];
	ScanColumn++;
	ArrayFirstsBoolsAndNbr[ ScanColumn ] = InfosGUI->ValOffsetBoolVar[ ScanColumn ];
	ScanColumn++;
	ArrayFirstsBoolsAndNbr[ ScanColumn++ ] = NBR_BOOLS_VAR_SPY;
	ArrayFirstsBoolsAndNbr[ ScanColumn++ ] = NBR_BOOLS_VAR_SPY;
	ArrayFirstsBoolsAndNbr[ ScanColumn++ ] = NBR_BOOLS_VAR_SPY;
	cJSON_AddItemToObject( JsonRoot, "Bools", cJSON_CreateIntArray( ArrayFirstsBoolsAndNbr, ScanColumn ) );

	// to read the "free vars spy" values
	for( ScanVar=0; ScanVar<NBR_FREE_VAR_SPY; ScanVar++ )
	{
		FreeVarsType[ NbrFreeVars ] = InfosGUI->FreeVarSpy[ ScanVar ].VarType;
		FreeVarsNum[ NbrFreeVars ] = InfosGUI->FreeVarSpy[ ScanVar ].VarNum;
		NbrFreeVars++;
	}
	// to read 3 system time variables
	for( ScanVar=0; ScanVar<3; ScanVar++ )
	{
		FreeVarsType[ NbrFreeVars ] = VAR_WORD_SYSTEM;
		FreeVarsNum[ NbrFreeVars ] = ScanVar;
		NbrFreeVars++;
	}
	// to read functions blocks displayed on screen...
	for( ScanVar=0; ScanVar<InfosGUI->NbrFunctionsBlocksVarsToMonitor; ScanVar++ )
	{
		if ( NbrFreeVars<NBR_FREE_VARS_READ )
		{
			FreeVarsType[ NbrFreeVars ] = InfosGUI->FunctionsBlocksVarsToMonitor[ ScanVar ].VarType;
			FreeVarsNum[ NbrFreeVars ] = InfosGUI->FunctionsBlocksVarsToMonitor[ ScanVar ].VarNum;
			NbrFreeVars++;
		}
	}

	cJSON_AddItemToObject( JsonRoot, "SpyT", cJSON_CreateIntArray( FreeVarsType, NbrFreeVars ) );
	cJSON_AddItemToObject( JsonRoot, "SpyN", cJSON_CreateIntArray( FreeVarsNum, NbrFreeVars ) );
	
	// Print to text, Delete the cJSON, use it, release the string.
//	TextReq = cJSON_Print( JsonRoot );
	TextReq = cJSON_PrintUnformatted( JsonRoot );
	cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	return TextReq;
}
#ifdef GTK_INTERFACE
gboolean TargetStateForGtk;
#endif
void MonitorParseResponseReadVars( cJSON *JsonRoot, char SlaveMode )
{
	int ScanVar;
	int ColumnVar = 0;
	int VarBoolType[ NBR_TYPE_BOOLS_SPY ] = { VAR_MEM_BIT, VAR_PHYS_INPUT, VAR_PHYS_OUTPUT };
	int FirstBoolVar[ NBR_TYPE_BOOLS_SPY ];
	int NbrBoolVar[ NBR_TYPE_BOOLS_SPY ];
	int NbrFreeVars = cJSON_GetArraySize( cJSON_GetObjectItem( JsonRoot,"SpyT" ) );
	FirstBoolVar[ 0 ] = cJSON_GetArrayItem( cJSON_GetObjectItem( JsonRoot,"Bools" ), ColumnVar++ )->valueint;
	FirstBoolVar[ 1 ] = cJSON_GetArrayItem( cJSON_GetObjectItem( JsonRoot,"Bools" ), ColumnVar++ )->valueint;
	FirstBoolVar[ 2 ] = cJSON_GetArrayItem( cJSON_GetObjectItem( JsonRoot,"Bools" ), ColumnVar++ )->valueint;
	NbrBoolVar[ 0 ] = cJSON_GetArrayItem( cJSON_GetObjectItem( JsonRoot,"Bools" ), ColumnVar++ )->valueint;
	NbrBoolVar[ 1 ] = cJSON_GetArrayItem( cJSON_GetObjectItem( JsonRoot,"Bools" ), ColumnVar++ )->valueint;
	NbrBoolVar[ 2 ] = cJSON_GetArrayItem( cJSON_GetObjectItem( JsonRoot,"Bools" ), ColumnVar++ )->valueint;
	
	if ( !SlaveMode )
	{
		// get values from the response of the slave...
		// --------------------------------------------
		int TargetState;
		cJSON * JsonLastDuration;
		cJSON * JsonMaxDuration;
		cJSON * JsonNbrTicksMissed;
		cJSON * JsonArrayBools = cJSON_GetObjectItem( JsonRoot,"BoolsVal" );
		
		for( ColumnVar=0; ColumnVar<NBR_TYPE_BOOLS_SPY; ColumnVar++ )
		{
			char * BoolsValues = cJSON_GetArrayItem( JsonArrayBools, ColumnVar )->valuestring;
#ifndef MONITOR_TEST_ONLY_NO_RESPONSES_USE
			for( ScanVar=0; ScanVar<NbrBoolVar[ ColumnVar ]&&BoolsValues[ ScanVar ]!='\0'; ScanVar++ )
			{
				int BoolVal = BoolsValues[ ScanVar ];
//printf("write bool var %d/%d with %d\n", VarBoolType[ColumnVar],FirstBoolVar[ ColumnVar ]+ScanVar,BoolVal);
				// should not be usefull, as only inputs & outputs can be set...
				if ( ColumnVar==0 )
					BoolVal = BoolVal & 1;
				if ( BoolVal&2 )
				{
					char DoSetVar = FALSE;
					if ( !IsVarSet( VarBoolType[ColumnVar], FirstBoolVar[ ColumnVar ]+ScanVar ) )
						DoSetVar = TRUE;
					else if ( ReadVar( VarBoolType[ColumnVar], FirstBoolVar[ ColumnVar ]+ScanVar )!=(BoolVal&1) )
						DoSetVar = TRUE;
					if ( DoSetVar )
						SetVar( VarBoolType[ColumnVar], FirstBoolVar[ ColumnVar ]+ScanVar, BoolVal&1 );
				}
				else
				{
					if ( ColumnVar>0 )
					{
						if ( IsVarSet( VarBoolType[ColumnVar], FirstBoolVar[ ColumnVar ]+ScanVar ) )
							UnsetVar( VarBoolType[ColumnVar], FirstBoolVar[ ColumnVar ]+ScanVar );
					}
					WriteVar( VarBoolType[ColumnVar], FirstBoolVar[ ColumnVar ]+ScanVar, BoolVal&1 );
				}
			}
#endif
		}
		
		for( ScanVar=0; ScanVar<NbrFreeVars; ScanVar++ )
		{
			int VarType = cJSON_GetArrayItem( cJSON_GetObjectItem( JsonRoot,"SpyT" ), ScanVar )->valueint;
			int VarNum = cJSON_GetArrayItem( cJSON_GetObjectItem( JsonRoot,"SpyN" ), ScanVar )->valueint;
			int VarValue = cJSON_GetArrayItem( cJSON_GetObjectItem( JsonRoot,"SpyVal" ), ScanVar )->valueint;
//			printf("freevar%d (%d/%d) = %d\n", ScanVar, VarType, VarNum, VarValue );
#ifndef MONITOR_TEST_ONLY_NO_RESPONSES_USE
			WriteVar( VarType, VarNum, VarValue );
#endif
		}
		
		TargetState = cJSON_GetObjectItem( JsonRoot,"State" )->valueint;
#ifdef GTK_INTERFACE
printf("TARGET STATE IS = %s (currenttarget=%s)\n", _ConvStateInString(TargetState),_ConvStateInString(InfosGUI->TargetMonitor.LatestTargetState) );
		if ( TargetState!=InfosGUI->TargetMonitor.LatestTargetState )
		{
			printf("ask to refresh run/stop menu...\n");
			TargetStateForGtk = TargetState==STATE_RUN;
			g_idle_add( (GSourceFunc)SetGtkMenuStateForRunStopSwitch, &TargetStateForGtk );
		}
#endif
		InfosGUI->TargetMonitor.LatestTargetState = TargetState;
		JsonLastDuration = cJSON_GetObjectItem( JsonRoot, "LastDurRT" );
		if ( JsonLastDuration )
			InfosGene->DurationOfLastScan = JsonLastDuration->valueint;
		JsonMaxDuration = cJSON_GetObjectItem( JsonRoot, "MaxDurRT" );
		if ( JsonMaxDuration )
			InfosGene->MaxScanDuration = JsonMaxDuration->valueint;
		JsonNbrTicksMissed = cJSON_GetObjectItem( JsonRoot, "TicksMiss" );
		if ( JsonNbrTicksMissed )
			InfosGene->NbrTicksMissed = JsonNbrTicksMissed->valueint;
//printf("debug read (%s)/DurationOfLastScan:%d (%s)/NbrTicksMissed:%d\n", JsonLastDuration?"PRES":"UNPRES", InfosGene->DurationOfLastScan, JsonNbrTicksMissed?"PRES":"UNPRES", InfosGene->NbrTicksMissed );
	}
	else
	{
		// adds values responses for the response to the master !
		// ------------------------------------------------------
		char BuffStateBools[ NBR_TYPE_BOOLS_SPY ][ 50+1 ];
		const char * ArrayBools[ NBR_TYPE_BOOLS_SPY ];
		for( ColumnVar=0; ColumnVar<NBR_TYPE_BOOLS_SPY; ColumnVar++ )
		{
			char * pBuff = BuffStateBools[ ColumnVar ];
			char BuffVal[ 2 ];
			BuffVal[1] = '\0';
			pBuff[0] = '\0';
			for( ScanVar=0; ScanVar<NbrBoolVar[ ColumnVar ]&&ScanVar<50; ScanVar++ )
			{
				BuffVal[0] = ReadVar( VarBoolType[ColumnVar], FirstBoolVar[ ColumnVar ]+ScanVar );
				if ( ColumnVar>0 )
				{
					if ( IsVarSet( VarBoolType[ColumnVar], FirstBoolVar[ ColumnVar ]+ScanVar ) )
						BuffVal[0] = BuffVal[0]|2;
				}
				BuffVal[0] = BuffVal[0]+'0';
				strcat( pBuff, BuffVal );
			}
			ArrayBools[ ColumnVar ] = BuffStateBools[ ColumnVar ];
		}
		cJSON_AddItemToObject( JsonRoot, "BoolsVal", cJSON_CreateStringArray( ArrayBools, NBR_TYPE_BOOLS_SPY ) );
		
		int FreeVarsValues[ NBR_FREE_VARS_READ ];
		for( ScanVar=0; ScanVar<NbrFreeVars; ScanVar++ )
		{
			int VarType = cJSON_GetArrayItem( cJSON_GetObjectItem( JsonRoot,"SpyT" ), ScanVar )->valueint;
			int VarNum = cJSON_GetArrayItem( cJSON_GetObjectItem( JsonRoot,"SpyN" ), ScanVar )->valueint;
			FreeVarsValues[ ScanVar ] = ReadVar( VarType, VarNum );
		}
		cJSON_AddItemToObject( JsonRoot, "SpyVal", cJSON_CreateIntArray( FreeVarsValues, NbrFreeVars ) );
		// current run/stop state of the target !
		cJSON_AddNumberToObject( JsonRoot, "State", InfosGene->LadderState );
#if defined( RT_SUPPORT ) || defined( __XENO__ ) || defined( __COBALT__ )
		cJSON_AddNumberToObject( JsonRoot, "LastDurRT", InfosGene->DurationOfLastScan );
		cJSON_AddNumberToObject( JsonRoot, "MaxDurRT", InfosGene->MaxScanDuration );
		cJSON_AddNumberToObject( JsonRoot, "TicksMiss", InfosGene->NbrTicksMissed );
#endif
	}
}


//for the master, to ask to read a rung displayed
char * MonitorCreateRequestReadRung( int NumRung )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq;
	
	JsonRoot = cJSON_CreateObject();
	
	cJSON_AddStringToObject( JsonRoot, "ReqCL", "RdRung" );

#ifdef DEBUG_FRAME_ID_ADDED
	cJSON_AddNumberToObject( JsonRoot, "FrameID", NumFrame++ );
#endif

	cJSON_AddNumberToObject( JsonRoot, "Rung", NumRung );
	
	// Print to text, Delete the cJSON, use it, release the string.
//	TextReq = cJSON_Print( JsonRoot );
	TextReq = cJSON_PrintUnformatted( JsonRoot );
	cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	return TextReq;
}

void MonitorParseResponseReadRung( cJSON *JsonRoot, char SlaveMode )
{
	int ScanX,ScanY;
	int NumRung = cJSON_GetObjectItem( JsonRoot,"Rung" )->valueint;
	StrRung * PtrRung = &RungArray[ NumRung ];
	if ( !SlaveMode )
	{
		// get values from the response of the slave...
		// --------------------------------------------
		cJSON * JsonArrayLine = cJSON_GetObjectItem( JsonRoot,"Line" );
		int NbrLinesRead = cJSON_GetArraySize( JsonArrayLine );
		for (ScanY=0;ScanY<NbrLinesRead;ScanY++)
		{
			char * StringValues = cJSON_GetArrayItem( JsonArrayLine, ScanY )->valuestring;
//printf("ValueReadForLine%d=%s\n", ScanY, StringValues);
#ifndef MONITOR_TEST_ONLY_NO_RESPONSES_USE
			for(ScanX=0;ScanX<RUNG_WIDTH;ScanX++)
			{
				if ( StringValues[ ScanX ]=='\0' )
					break;
				char Val = StringValues[ ScanX ];
				if ( Val>='A' )
					Val = Val-'A'+10;
				else
					Val = Val-'0';
				PtrRung->Element[ScanX][ScanY].DynamicState = (Val&1)?1:0;
				PtrRung->Element[ScanX][ScanY].DynamicInput = (Val&2)?1:0;
				PtrRung->Element[ScanX][ScanY].DynamicOutput = (Val&4)?1:0;
				PtrRung->Element[ScanX][ScanY].DynamicVarSetted = (Val&8)?1:0;
			}
#endif
		}
	}
	else
	{
		// adds values responses for the response to the master !
		// ------------------------------------------------------
		char BuffDynamicLines[ RUNG_HEIGHT ][ RUNG_WIDTH+1 ];
		const char * ArrayLines[ RUNG_HEIGHT ];
		for (ScanY=0;ScanY<PtrRung->NbrLinesUsed;ScanY++)
		{
			char * pBuff = BuffDynamicLines[ ScanY ];
			for(ScanX=0;ScanX<RUNG_WIDTH;ScanX++)
			{
				char Val = 0;
				if ( PtrRung->Element[ScanX][ScanY].DynamicState )
					Val |= 1;
				if ( PtrRung->Element[ScanX][ScanY].DynamicInput )
					Val |= 2;
				if ( PtrRung->Element[ScanX][ScanY].DynamicOutput )
					Val |= 4;
				if ( PtrRung->Element[ScanX][ScanY].DynamicVarSetted )
					Val |= 8;
				// if one day, one more bit required, use base 32! (char 'G' to ....)
				if ( Val>=10 )
					Val = Val-10+'A';
				else
					Val = Val+'0';
				*pBuff = Val;
				pBuff++;
			}
			*pBuff = '\0';
//printf("ValueCreatedFor%d=%s\n", ScanY, BuffDynamicLines[ ScanY ]);
			ArrayLines[ ScanY ] = BuffDynamicLines[ ScanY ];
		}
		cJSON_AddItemToObject( JsonRoot, "Line", cJSON_CreateStringArray( ArrayLines, PtrRung->NbrLinesUsed ) );
	}
}

//for the master, to ask to read a sequential page displayed
char * MonitorCreateRequestReadSequential( int NumPage )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq;
	
	JsonRoot = cJSON_CreateObject();
	
	cJSON_AddStringToObject( JsonRoot, "ReqCL", "RdSeq" );
	
#ifdef DEBUG_FRAME_ID_ADDED
	cJSON_AddNumberToObject( JsonRoot, "FrameID", NumFrame++ );
#endif

	cJSON_AddNumberToObject( JsonRoot, "Page", NumPage );
	
	// Print to text, Delete the cJSON, use it, release the string.
//	TextReq = cJSON_Print( JsonRoot );
	TextReq = cJSON_PrintUnformatted( JsonRoot );
	cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	return TextReq;
}

void MonitorParseResponseReadSequential( cJSON *JsonRoot, char SlaveMode )
{
	int NbrStepsDisp = 0;
	int StepNumDisp[ NBR_STEPS ];
	char StepState[ NBR_STEPS ];
	int NbrTransisDisp = 0;
	int TransiNumDisp[ NBR_TRANSITIONS ];
	char TransiState[ NBR_TRANSITIONS ];
	int ScanStep,ScanTransi;
	StrStep * pStep;
	StrTransition * pTransi;
	int NumPageDisplayed = cJSON_GetObjectItem( JsonRoot,"Page" )->valueint;
	if ( !SlaveMode )
	{
		// get values from the response of the slave...
		// --------------------------------------------
		char * StringStepState = cJSON_GetObjectItem( JsonRoot, "StVal" )->valuestring;
		char * StringTransiState = cJSON_GetObjectItem( JsonRoot, "TrVal" )->valuestring;
		NbrStepsDisp = cJSON_GetArraySize( cJSON_GetObjectItem( JsonRoot,"StNum" ) );
		for( ScanStep=0; ScanStep<NbrStepsDisp; ScanStep++ )
		{
			int StepNum = cJSON_GetArrayItem( cJSON_GetObjectItem( JsonRoot,"StNum" ), ScanStep )->valueint;
			int StepState = (StringStepState[ ScanStep ]=='1')?1:0;
#ifndef MONITOR_TEST_ONLY_NO_RESPONSES_USE
			Sequential->Step[ StepNum ].Activated = StepState;
#endif
		}
		NbrTransisDisp = cJSON_GetArraySize( cJSON_GetObjectItem( JsonRoot,"TrNum" ) );
		for( ScanTransi=0; ScanTransi<NbrStepsDisp; ScanTransi++ )
		{
			int TransiNum = cJSON_GetArrayItem( cJSON_GetObjectItem( JsonRoot,"TrNum" ), ScanTransi )->valueint;
			int TransiState = (StringTransiState[ ScanStep ]=='1')?1:0;;
#ifndef MONITOR_TEST_ONLY_NO_RESPONSES_USE
			Sequential->Transition[ TransiNum ].Activated = TransiState;
#endif
		}
	}
	else
	{
		// adds values responses for the response to the master !
		// ------------------------------------------------------
		
		// scan for steps and transitions displayed on this page
		for( ScanStep=0; ScanStep<NBR_STEPS; ScanStep++ )
		{
			pStep = &Sequential->Step[ ScanStep ];
			if ( pStep->NumPage==NumPageDisplayed )
			{
				StepNumDisp[ NbrStepsDisp ] = ScanStep;
				StepState[ NbrStepsDisp ] = pStep->Activated?'1':'0';
				NbrStepsDisp++;
			}
		}
		StepState[ NbrStepsDisp ] = '\0';
		cJSON_AddItemToObject( JsonRoot, "StNum", cJSON_CreateIntArray( StepNumDisp, NbrStepsDisp ) );
		cJSON_AddStringToObject( JsonRoot, "StVal", StepState );
		for( ScanTransi=0; ScanTransi<NBR_TRANSITIONS; ScanTransi++ )
		{
			pTransi = &Sequential->Transition[ ScanTransi ];
			if ( pTransi->NumPage==NumPageDisplayed )
			{
				TransiNumDisp[ NbrTransisDisp ] = ScanTransi;
				TransiState[ NbrTransisDisp ] = pTransi->Activated?'1':'0';
				NbrTransisDisp++;
			}
		}
		TransiState[ NbrTransisDisp ] = '\0';
		cJSON_AddItemToObject( JsonRoot, "TrNum", cJSON_CreateIntArray( TransiNumDisp, NbrTransisDisp ) );
		cJSON_AddStringToObject( JsonRoot, "TrVal", TransiState );
	}
}

//for the master, to ask to set run/stop state
char * MonitorCreateRequestSetRunStopState( int RunStopStateWanted )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq;
	
	JsonRoot = cJSON_CreateObject();
	
	cJSON_AddStringToObject( JsonRoot, "ReqCL", "SetRunStopState" );
	
	cJSON_AddNumberToObject( JsonRoot, "State", RunStopStateWanted );
	
	// Print to text, Delete the cJSON, use it, release the string.
	TextReq = cJSON_Print( JsonRoot );
	cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	return TextReq;
}

void MonitorParseResponseSetRunStopState( cJSON *JsonRoot, char SlaveMode )
{
	if ( !SlaveMode )
	{
		// get values from the response of the slave...
		// --------------------------------------------
		// nothing interesting to read back...
		InfosGUI->TargetMonitor.AskTargetToSetState = -1; //action request to target done
	}
	else
	{
		// get value wanted per the master
		int TargetStateWanted = cJSON_GetObjectItem( JsonRoot,"State" )->valueint;
printf("RECV ORDER STOP/RUN/FREEZE/RUNONECYLE Target=%d - Wanted=%d\n",InfosGene->LadderState,TargetStateWanted);
		if ( ( TargetStateWanted==STATE_RUN && InfosGene->LadderState!=STATE_RUN )
			|| ( TargetStateWanted==STATE_STOP && InfosGene->LadderState!=STATE_STOP ) )
		{
printf("EXEC ORDER FLIP RUN/STOP\n");
			DoFlipFlopRunStop( );
		}
		else if ( ( TargetStateWanted==STATE_RUN_FREEZE && InfosGene->LadderState!=STATE_RUN_FREEZE )
			|| TargetStateWanted==STATE_RUN_FOR_ONE_CYCLE )
		{
printf("EXEC ORDER FLIP RUN_ONE_CYCLE/FREEZE\n");
			DoFlipFlopRunOneCycleFreeze( );
		}

		// adds values responses for the response to the master !
		// ------------------------------------------------------
		// nothing usefull for the master...
	}
}

//for the master, to ask to reset the target
char * MonitorCreateRequestResetTarget( void )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq;
	
	JsonRoot = cJSON_CreateObject();
	
	cJSON_AddStringToObject( JsonRoot, "ReqCL", "ResetTarget" );
	
	// Print to text, Delete the cJSON, use it, release the string.
	TextReq = cJSON_Print( JsonRoot );
	cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	return TextReq;
}

void MonitorParseResponseResetTarget( cJSON *JsonRoot, char SlaveMode )
{
	if ( !SlaveMode )
	{
		// get values from the response of the slave...
		// --------------------------------------------
		// nothing interesting to read back...
		InfosGUI->TargetMonitor.AskTargetToReset = -1; //action request to target done
	}
	else
	{
printf("RECV/EXEC ORDER Reset Target\n");
		DoReset( );
		// adds values responses for the response to the master !
		// ------------------------------------------------------
		// nothing usefull for the master...
	}
}

// The functions gettimeofday() and settimeofday() can get and set the time as well as a timezone. The tv argument is a struct timeval (as specified in <sys/time.h>):
//for the master, to ask to set its clock time with current PC one
char * MonitorCreateRequestSetClockTime( )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq = NULL;
//	struct timeval tv;
	char BuffCurrTime[ 30 ];
	GetCurrentAscTimeUTC( BuffCurrTime );
	
//	if ( gettimeofday( &tv, NULL )==0 )
//	{
		JsonRoot = cJSON_CreateObject();
	
		cJSON_AddStringToObject( JsonRoot, "ReqCL", "SetClockTime" );
		cJSON_AddStringToObject( JsonRoot, "AscTime", BuffCurrTime );
	
		// Print to text, Delete the cJSON, use it, release the string.
		TextReq = cJSON_Print( JsonRoot );
		cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
//	}
	return TextReq;
}

void MonitorParseResponseSetClockTime( cJSON *JsonRoot, char SlaveMode )
{
	if ( !SlaveMode )
	{
		// get values from the response of the slave...
		// --------------------------------------------
		// nothing interesting to read back...
		InfosGUI->TargetMonitor.AskTargetToSetClockTime = -1; //action request to target done
	}
	else
	{
		int Done = 0;
		// get value wanted per the master
		//Dec.2016, seen with MSYS2/Win32
#ifndef __WIN32__
		cJSON * CommandTimeVal = cJSON_GetObjectItem( JsonRoot,"AscTime" );
		if ( CommandTimeVal )
		{
			time_t IntTimeVal = ConvertAscUtcToIntTime( CommandTimeVal->valuestring );
			Done = SetTimeClock( IntTimeVal, Preferences.UseRtcDevice );
printf("RECV/EXEC ORDER Set Clock Time=%d / Done=%d\n",(int)IntTimeVal,Done);
		}
#endif
		// adds values responses for the response to the master !
		// ------------------------------------------------------
		cJSON_AddNumberToObject( JsonRoot, "Done", Done );
	}
}

char * MonitorCreateRequestGetTargetInfosVersion( void )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq;
	
	JsonRoot = cJSON_CreateObject();
	
	cJSON_AddStringToObject( JsonRoot, "ReqCL", "GetTargetInfosVersion" );
	
	// Print to text, Delete the cJSON, use it, release the string.
	TextReq = cJSON_Print( JsonRoot );
	cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	return TextReq;
}

void ReadKernelVersion( char * Buff )
{
	FILE * pFile;
	strcpy( Buff, "???" );
	pFile = fopen( "/proc/version", "rt" );
	if ( pFile )
	{
		fread( Buff, 1, 50, pFile );
		fclose( pFile );
		Buff[ 49 ] = '\0';
		// extract version number from string
		char * pStart = Buff;
		while( *pStart!='\0' && ( *pStart<'0' || *pStart>'9' || *pStart==' ' ) )
			pStart++;
		if ( *pStart!='\0' )
		{
			char * pEnd = pStart;
			while( *pEnd!=' ' && *pEnd!='\0' )
				pEnd++;
			if ( *pEnd!='\0' )
			{
				*pEnd = '\0';
				strcpy( Buff, pStart );
			}
		}
	}
}
void ReadDistribVersion( char * Buff )
{
	FILE * File;
	char Line[300];
	char * LineOk;
	strcpy( Buff, "???" );
	File = fopen( "/etc/lsb-release","rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				char * pParameter;
				pParameter = "DISTRIB_DESCRIPTION=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
				{
					if ( Line[ strlen( pParameter) ]=='\"' )
					{
						strcpy( Buff, &Line[ strlen( pParameter)+1 ] );
						Buff[ strlen( Buff )-1 ] = '\0';
					}
					else
					{
						strcpy( Buff, &Line[ strlen( pParameter) ] );
					}
				}
			}
		}
		while(LineOk);
		fclose(File);
	}
	else
	{
		File = fopen( "/etc/debian_version","rt");
		if (File)
		{
			cl_fgets(Buff,300,File);
			fclose(File);
		}
	}	
}
void ReadXenomaiVersion( char * Buff )
{
	FILE * pFile;
	Buff[ 0 ] = '\0';
	pFile = fopen( "/proc/xenomai/version", "rt" );
	if ( pFile )
	{
		fread( Buff, 1, 50, pFile );
		fclose( pFile );
		Buff[ 49 ] = '\0';
		// extract version number from string
		char * pEnd = Buff;
		while( *pEnd!='\0' && *pEnd!='\n' && *pEnd!='\r' )
			pEnd++;
		if ( *pEnd!='\0' )
			*pEnd = '\0';
	}
}
#define KILOBYTE_STATFS 1024
void ReadDiskStats( char * Disk, char * Buff )
{
	Buff[ 0 ] = '\0';
#ifndef __WIN32__
	struct statfs s;
	long UsedBlocks;
	long PercentUsedBlocks;

	if (statfs("/", &s) == 0)
	{
		if (s.f_blocks > 0)
		{
			UsedBlocks = s.f_blocks - s.f_bfree;
			if(UsedBlocks == 0)
				PercentUsedBlocks = 0;
			else
			{
				PercentUsedBlocks = (long)
				  (UsedBlocks * 100.0 / (UsedBlocks + s.f_bavail) + 0.5);
			}
			sprintf( Buff, "U=%ldk (%ld%%)  F=%ldk S=%ldk",
				(long) ((s.f_blocks - s.f_bfree)*(s.f_bsize/(double)KILOBYTE_STATFS)),
				PercentUsedBlocks,
				(long) (s.f_bavail * (s.f_bsize / (double)KILOBYTE_STATFS)),
				(long) (s.f_blocks * (s.f_bsize / (double)KILOBYTE_STATFS))
				);
		}
		else
		{
			sprintf( Buff, "Error block size for %s.", Disk );
		}
	}
	else
	{
		sprintf( Buff, "StatFs Error for %s.", Disk );
	}
#endif
}
void MonitorParseResponseGetTargetInfosVersion( cJSON *JsonRoot, char SlaveMode )
{
	if ( !SlaveMode )
	{
		// get values from the response of the slave...
		// --------------------------------------------
		cJSON * JsonInfo;
		char * VersionSoft = cJSON_GetObjectItem( JsonRoot, "SoftVersion" )->valuestring;
printf("Version soft on target=%s\n",VersionSoft);
		strcpy( InfosGUI->TargetMonitor.InfoSoftVersion, VersionSoft );
		InfosGUI->TargetMonitor.InfoKernelVersion[ 0 ] = '\0';
		JsonInfo = cJSON_GetObjectItem( JsonRoot, "KernelVersion" );
		if ( JsonInfo )
			strcpy( InfosGUI->TargetMonitor.InfoKernelVersion, JsonInfo->valuestring );
		InfosGUI->TargetMonitor.InfoLinuxDistribVersion[ 0 ] = '\0';
		JsonInfo = cJSON_GetObjectItem( JsonRoot, "LinuxDistribVersion" );
		if ( JsonInfo )
			strcpy( InfosGUI->TargetMonitor.InfoLinuxDistribVersion, JsonInfo->valuestring );
		InfosGUI->TargetMonitor.InfoXenomaiVersion[ 0 ] = '\0';
		JsonInfo = cJSON_GetObjectItem( JsonRoot, "XenomaiVersion" );
		if ( JsonInfo )
			strcpy( InfosGUI->TargetMonitor.InfoXenomaiVersion, JsonInfo->valuestring );
		InfosGUI->TargetMonitor.InfoDiskStats[ 0 ] = '\0';
		JsonInfo = cJSON_GetObjectItem( JsonRoot, "DiskStats" );
		if ( JsonInfo )
			strcpy( InfosGUI->TargetMonitor.InfoDiskStats, JsonInfo->valuestring );
		InfosGUI->TargetMonitor.AskTargetToGetTargetInfos = -1; //action request to target done
		InfosGUI->TargetMonitor.CmdDisplayTargetInfosVersion = TRUE;
	}
	else
	{
		// get value wanted per the master
		// --- nothing ---
		// adds values responses for the response to the master !
		// ------------------------------------------------------
		char * Info = (char *)malloc( 300 );
		if ( Info )
		{
			strcpy( Info, CL_RELEASE_VER_STRING " ("CL_RELEASE_DATE_STRING")" );
#if defined(__XENO__) || defined(__COBALT__)
			strcat( Info, " / Xenomai" );
#endif
			cJSON_AddStringToObject( JsonRoot, "SoftVersion", Info );
			ReadKernelVersion( Info );
			cJSON_AddStringToObject( JsonRoot, "KernelVersion", Info );
			ReadDistribVersion( Info );
			cJSON_AddStringToObject( JsonRoot, "LinuxDistribVersion", Info );
			ReadXenomaiVersion( Info );
			cJSON_AddStringToObject( JsonRoot, "XenomaiVersion", Info );
			ReadDiskStats( EMBEDDED_DISK_DEVICE, Info );
			cJSON_AddStringToObject( JsonRoot, "DiskStats", Info );
			free( Info );
		}
	}
}

char * MonitorCreateRequestGetProjectProperties( void )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq;
	
	JsonRoot = cJSON_CreateObject();
	
	cJSON_AddStringToObject( JsonRoot, "ReqCL", "GetProjectInfos" );
	
	// Print to text, Delete the cJSON, use it, release the string.
	TextReq = cJSON_Print( JsonRoot );
	cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	return TextReq;
}

void MonitorParseResponseGetProjectProperties( cJSON *JsonRoot, char SlaveMode )
{
	if ( !SlaveMode )
	{
		// get values from the response of the slave...
		// --------------------------------------------
		// just some values are used to test if same project between local already loaded and the target !
		char * ProjectName = cJSON_GetObjectItem( JsonRoot, "ProjectName" )->valuestring;
		char * ParamVersion = cJSON_GetObjectItem( JsonRoot, "ParamVersion" )->valuestring;
		char * ParamModifDate = cJSON_GetObjectItem( JsonRoot, "ParamModifDate" )->valuestring;
printf("Target project name=%s version=%s modifdate=%s\n",ProjectName,ParamVersion,ParamModifDate);
		if ( strcmp( InfosGene->ProjectProperties.ProjectName,ProjectName)!=0 || strcmp( InfosGene->ProjectProperties.ParamVersion,ParamVersion)!=0 
			|| strcmp( InfosGene->ProjectProperties.ParamModifDate,ParamModifDate)!=0 )
		{
//			strcpy( InfosGene->ErrorMsgStringToDisplay, "Mismatch detected between local parameters and target parameters...\nPerhaps you should disconnect!" );
#ifdef GTK_INTERFACE
			ShowMessageBoxInBackground( NULL, _("Mismatch detected between local parameters and target parameters...\nPerhaps you should disconnect!") );
#endif
		}
		InfosGUI->TargetMonitor.AskTargetToGetProjectProperties = -1; //action request to target done
		InfosGUI->TargetMonitor.CmdDisplayTargetInfosVersion = TRUE;
	}
	else
	{
		// get value wanted per the master
		// --- nothing ---
		// adds values responses for the response to the master !
		// ------------------------------------------------------
//		char * pCommentWithoutNewLines;
		cJSON_AddStringToObject( JsonRoot, "ProjectName", InfosGene->ProjectProperties.ProjectName );
		cJSON_AddStringToObject( JsonRoot, "ProjectSite", InfosGene->ProjectProperties.ProjectSite ); // not used after...
		cJSON_AddStringToObject( JsonRoot, "ParamVersion", InfosGene->ProjectProperties.ParamVersion );
		cJSON_AddStringToObject( JsonRoot, "ParamAuthor", InfosGene->ProjectProperties.ParamAuthor );  // not used after...
		cJSON_AddStringToObject( JsonRoot, "ParamCompany", InfosGene->ProjectProperties.ParamCompany ); // not used after...
		cJSON_AddStringToObject( JsonRoot, "ParamCreaDate", InfosGene->ProjectProperties.ParamCreaDate ); // not used after...
		cJSON_AddStringToObject( JsonRoot, "ParamModifDate", InfosGene->ProjectProperties.ParamModifDate ); // not used after...
//		pCommentWithoutNewLines = replace_str( InfosGene->ProjectProperties.ParamComment, "\n", "\\n" );
//		cJSON_AddStringToObject( JsonRoot, "ParamComment", pCommentWithoutNewLines ); // not used after...
//		free( pCommentWithoutNewLines );
	}
}

//for the master, to ask to reset the target
char * MonitorCreateRequestRebootTarget( void )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq;
	
	JsonRoot = cJSON_CreateObject();
	
	cJSON_AddStringToObject( JsonRoot, "ReqCL", "RebootTarget" );
	if ( InfosGUI->TargetMonitor.AskTargetToReboot!=1 )
		cJSON_AddNumberToObject( JsonRoot, "Type", InfosGUI->TargetMonitor.AskTargetToReboot );
		
	// Print to text, Delete the cJSON, use it, release the string.
	TextReq = cJSON_Print( JsonRoot );
	cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	return TextReq;
}

void MonitorParseResponseRebootTarget( cJSON *JsonRoot, char SlaveMode )
{
	if ( !SlaveMode )
	{
		// get values from the response of the slave...
		// --------------------------------------------
		// nothing interesting to read back...
		InfosGUI->TargetMonitor.AskTargetToReboot = -1; //action request to target done
	}
	else
	{
#ifndef GTK_INTERFACE
		FILE * pTestTelinitFile;
#endif
		int RebootType = 1;
		// get value wanted per the master
		cJSON * JsonRebootType = cJSON_GetObjectItem( JsonRoot,"Type" );
		if ( JsonRebootType )
			RebootType = JsonRebootType->valueint;
printf("RECV/EXEC ORDER Reboot/Halt (%d) of the system\n", RebootType);
#ifndef GTK_INTERFACE
		// if telinit command available, use it instead of signal on kill...
		pTestTelinitFile = fopen( "/sbin/telinit", "rb" );
		if ( pTestTelinitFile )
		{
			fclose( pTestTelinitFile );
			LaunchExternalCommand( "/sbin/telinit" );
			LaunchExternalCommand( (RebootType==2)?"0\t":"6\t" ); // '\t' = end of command list !
//test			LaunchExternalCommand( "/bin/cp" );
//test			LaunchExternalCommand( "/etc/lsb-release" );
//test			LaunchExternalCommand( "/tmp/toto.txt\t" ); // '\t' = end of command list !
//test2			LaunchExternalCommand( "/bin/cp" );
//test2			LaunchExternalCommand( "/etc/debian_version" );
//test2			LaunchExternalCommand( "/tmp/toto.txt\t" ); // '\t' = end of command list !
		}
		else
		{
			kill(1/*initpid*/, (RebootType==2)?SIGUSR1:SIGTERM);
		}
//		sync();
////		reboot( LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, (RebootType==2)?LINUX_REBOOT_CMD_POWER_OFF:LINUX_REBOOT_CMD_RESTART, NULL );
//		reboot( (RebootType==2)?RB_POWER_OFF:RB_AUTOBOOT );
#endif
		// adds values responses for the response to the master !
		// ------------------------------------------------------
		// nothing usefull for the master...
	}
}

char * MonitorCreateRequestWriteVarValue( )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq = NULL;

	JsonRoot = cJSON_CreateObject();
	
	cJSON_AddStringToObject( JsonRoot, "ReqCL", "WriteVarValue" );

	cJSON_AddNumberToObject( JsonRoot, "VarType", InfosGUI->TargetMonitor.AskTargetToWriteVarType );
	cJSON_AddNumberToObject( JsonRoot, "VarNum", InfosGUI->TargetMonitor.AskTargetToWriteVarNum );
	cJSON_AddNumberToObject( JsonRoot, "Value", InfosGUI->TargetMonitor.AskTargetToWriteVarNewValue );

	// Print to text, Delete the cJSON, use it, release the string.
	TextReq = cJSON_Print( JsonRoot );
	cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	return TextReq;
}

void MonitorParseResponseWriteVarValue( cJSON *JsonRoot, char SlaveMode )
{
	if ( !SlaveMode )
	{
		// get values from the response of the slave...
		// --------------------------------------------
		// nothing interesting to read back...
		InfosGUI->TargetMonitor.AskTargetToWriteVarType = -1; //action request to target done
		InfosGUI->TargetMonitor.AskTargetToWriteVarNum = -1;
	}
	else
	{
		// get values wanted per the master
		int VarType = cJSON_GetObjectItem( JsonRoot,"VarType" )->valueint;
		int VarNum = cJSON_GetObjectItem( JsonRoot,"VarNum" )->valueint;
		int NewValue = cJSON_GetObjectItem( JsonRoot,"Value" )->valueint;
#ifndef MONITOR_TEST_ONLY_NO_RESPONSES_USE
		WriteVar( VarType, VarNum, NewValue );
#else
		printf( "Monitor: simul but should WriteVar%d/%d=%d...\n",VarType, VarNum, NewValue );
#endif
		// adds values responses for the response to the master !
		// ------------------------------------------------------
		// nothing usefull for the master...
	}
}

char * MonitorCreateRequestSetOrUnsetVar( )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq = NULL;

	JsonRoot = cJSON_CreateObject();
	
	cJSON_AddStringToObject( JsonRoot, "ReqCL", "SetOrUnsetVar" );

	cJSON_AddNumberToObject( JsonRoot, "VarType", InfosGUI->TargetMonitor.AskTargetToSetOrUnsetVarType );
	cJSON_AddNumberToObject( JsonRoot, "VarNum", InfosGUI->TargetMonitor.AskTargetToSetOrUnsetVarNum );
	cJSON_AddNumberToObject( JsonRoot, "Value", InfosGUI->TargetMonitor.AskTargetToSetOrUnsetVarValue );

	// Print to text, Delete the cJSON, use it, release the string.
	TextReq = cJSON_Print( JsonRoot );
	cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	return TextReq;
}

void MonitorParseResponseSetOrUnsetVar( cJSON *JsonRoot, char SlaveMode )
{
	if ( !SlaveMode )
	{
		// get values from the response of the slave...
		// --------------------------------------------
		// nothing interesting to read back...
		InfosGUI->TargetMonitor.AskTargetToSetOrUnsetVarType = -1; //action request to target done
		InfosGUI->TargetMonitor.AskTargetToSetOrUnsetVarNum = -1;
	}
	else
	{
		// get values wanted per the master
		int VarType = cJSON_GetObjectItem( JsonRoot,"VarType" )->valueint;
		int VarNum = cJSON_GetObjectItem( JsonRoot,"VarNum" )->valueint;
		int SetUnsetValue = cJSON_GetObjectItem( JsonRoot,"Value" )->valueint;
#ifndef MONITOR_TEST_ONLY_NO_RESPONSES_USE
		switch( SetUnsetValue )
		{
			case 2: SetVar( VarType, VarNum, 0 ); break;
			case 1: SetVar( VarType, VarNum, 1 ); break;
			case 0: UnsetVar( VarType, VarNum ); break;
		}
#else
		printf( "Monitor: simul but should SetOrUnsetVar%d/%d=%d...\n",VarType, VarNum, SetUnsetValue );
#endif
		// adds values responses for the response to the master !
		// ------------------------------------------------------
		// nothing usefull for the master...
	}
}

char * MonitorCreateRequestGetNetworkConfig( void )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq;
	
	JsonRoot = cJSON_CreateObject();
	
	cJSON_AddStringToObject( JsonRoot, "ReqCL", "GetNetworkConfig" );
	
	// Print to text, Delete the cJSON, use it, release the string.
	TextReq = cJSON_Print( JsonRoot );
	cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	return TextReq;
}

void MonitorParseResponseGetNetworkConfig( cJSON *JsonRoot, char SlaveMode )
{
	if ( !SlaveMode )
	{
		// get values from the response of the slave...
		// --------------------------------------------
		strcpy( NetworkConfigDatas.IpAddr, cJSON_GetObjectItem( JsonRoot, "IpAddr" )->valuestring );
		strcpy( NetworkConfigDatas.NetMask, cJSON_GetObjectItem( JsonRoot, "NetMask" )->valuestring );
		strcpy( NetworkConfigDatas.Route, cJSON_GetObjectItem( JsonRoot, "Route" )->valuestring );
		if ( cJSON_GetObjectItem( JsonRoot, "ServerDNS1" ) )
			strcpy( NetworkConfigDatas.ServerDNS1, cJSON_GetObjectItem( JsonRoot, "ServerDNS1" )->valuestring );
		else
			NetworkConfigDatas.ServerDNS1[0] = '\0';
		if ( cJSON_GetObjectItem( JsonRoot, "ServerDNS2" ) )
			strcpy( NetworkConfigDatas.ServerDNS2, cJSON_GetObjectItem( JsonRoot, "ServerDNS2" )->valuestring );
		else
			NetworkConfigDatas.ServerDNS2[0] = '\0';
		if ( cJSON_GetObjectItem( JsonRoot, "HostName" ) )
			strcpy( NetworkConfigDatas.HostName, cJSON_GetObjectItem( JsonRoot, "HostName" )->valuestring );
		else
			NetworkConfigDatas.HostName[0] = '\0';
		InfosGUI->TargetMonitor.AskTargetToGetNetworkConfig = -1; //action request to target done
		InfosGUI->TargetMonitor.CmdDisplayTargetNetworkConfig = TRUE;
	}
	else
	{
		// get value wanted per the master
		// --- nothing ---
		// adds values responses for the response to the master !
		// ------------------------------------------------------
		char ReadConfigOk = LoadNetworkConfigDatas( );
		char ReadHosNameOk = LoadHostNameFile( );
		cJSON_AddStringToObject( JsonRoot, "IpAddr", ReadConfigOk?NetworkConfigDatas.IpAddr:"???" );
		cJSON_AddStringToObject( JsonRoot, "NetMask", ReadConfigOk?NetworkConfigDatas.NetMask:"???" );
		cJSON_AddStringToObject( JsonRoot, "Route", ReadConfigOk?NetworkConfigDatas.Route:"???" );
		cJSON_AddStringToObject( JsonRoot, "ServerDNS1", ReadConfigOk?NetworkConfigDatas.ServerDNS1:"???" );
		cJSON_AddStringToObject( JsonRoot, "ServerDNS2", ReadConfigOk?NetworkConfigDatas.ServerDNS2:"???" );
		cJSON_AddStringToObject( JsonRoot, "HostName", ReadHosNameOk?NetworkConfigDatas.HostName:"???" );
	}
}

char * MonitorCreateRequestWriteNetworkConfig( void )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq;
	
	JsonRoot = cJSON_CreateObject();
	
	cJSON_AddStringToObject( JsonRoot, "ReqCL", "WriteNetworkConfig" );

	cJSON_AddStringToObject( JsonRoot, "IpAddr", NetworkConfigDatas.IpAddr );
	cJSON_AddStringToObject( JsonRoot, "NetMask", NetworkConfigDatas.NetMask );
	cJSON_AddStringToObject( JsonRoot, "Route", NetworkConfigDatas.Route );
	cJSON_AddStringToObject( JsonRoot, "ServerDNS1", NetworkConfigDatas.ServerDNS1 );
	cJSON_AddStringToObject( JsonRoot, "ServerDNS2", NetworkConfigDatas.ServerDNS2 );
	cJSON_AddStringToObject( JsonRoot, "HostName", NetworkConfigDatas.HostName );
	
	// Print to text, Delete the cJSON, use it, release the string.
	TextReq = cJSON_Print( JsonRoot );
	cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	return TextReq;
}

void MonitorParseResponseWriteNetworkConfig( cJSON *JsonRoot, char SlaveMode )
{
	if ( !SlaveMode )
	{
		// get values from the response of the slave...
		// --------------------------------------------
#ifdef GTK_INTERFACE
		if ( cJSON_GetObjectItem( JsonRoot,"Done" )->valueint==0 )
			ShowMessageBoxInBackground( NULL, _("Failed to send network config on target!") );
		else
			ShowMessageBoxInBackground( _("Info target"), _("Network config successfully send to target.") );
#endif
		InfosGUI->TargetMonitor.AskTargetToWriteNetworkConfig = -1; //action request to target done
	}
	else
	{
		// get value wanted per the master
		strcpy( NetworkConfigDatas.IpAddr, cJSON_GetObjectItem( JsonRoot, "IpAddr" )->valuestring );
		strcpy( NetworkConfigDatas.NetMask, cJSON_GetObjectItem( JsonRoot, "NetMask" )->valuestring );
		strcpy( NetworkConfigDatas.Route, cJSON_GetObjectItem( JsonRoot, "Route" )->valuestring );
		if ( cJSON_GetObjectItem( JsonRoot, "ServerDNS1" ) )
			strcpy( NetworkConfigDatas.ServerDNS1, cJSON_GetObjectItem( JsonRoot, "ServerDNS1" )->valuestring );
		if ( cJSON_GetObjectItem( JsonRoot, "ServerDNS2" ) )
			strcpy( NetworkConfigDatas.ServerDNS2, cJSON_GetObjectItem( JsonRoot, "ServerDNS2" )->valuestring );
		if ( cJSON_GetObjectItem( JsonRoot, "HostName" ) )
			strcpy( NetworkConfigDatas.HostName, cJSON_GetObjectItem( JsonRoot, "HostName" )->valuestring );
				// adds values responses for the response to the master !
		// ------------------------------------------------------
		char WriteConfigOk = TRUE;
#ifndef MONITOR_TEST_ONLY_NO_RESPONSES_USE
		RemountFileSystemRW( );
		if ( NetworkConfigDatas.IpAddr[0]!='?' )
			WriteConfigOk = SaveNetworkConfigDatas( );
		if ( WriteConfigOk )
		{
			if ( NetworkConfigDatas.HostName[0]!='?' )
				WriteConfigOk = SaveHostNameFile( );
		}
		RemountFileSystemRO( );
#else
		printf( "Monitor: simul but should write network/hostname config...\n" );
#endif
		cJSON_AddNumberToObject( JsonRoot, "Done", WriteConfigOk?1:0 );
	}
}

char * MonitorCreateRequestGetMonitorSerialConfig( void )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq;
	
	JsonRoot = cJSON_CreateObject();
	
	cJSON_AddStringToObject( JsonRoot, "ReqCL", "GetMonitorSerialConfig" );
	
	// Print to text, Delete the cJSON, use it, release the string.
	TextReq = cJSON_Print( JsonRoot );
	cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	return TextReq;
}

void MonitorParseResponseGetMonitorSerialConfig( cJSON *JsonRoot, char SlaveMode )
{
	if ( !SlaveMode )
	{
		// get values from the response of the slave...
		// --------------------------------------------
		strcpy( InfosGUI->TargetMonitor.TargetSlaveOnSerialPort, cJSON_GetObjectItem( JsonRoot, "PortName" )->valuestring );
		InfosGUI->TargetMonitor.TargetSlaveSerialSpeed = cJSON_GetObjectItem( JsonRoot, "Speed" )->valueint;
		InfosGUI->TargetMonitor.AskTargetToGetMonitorSerialConfig = -1; //action request to target done
		InfosGUI->TargetMonitor.CmdDisplayTargetMonitorSerialConfig = TRUE;
	}
	else
	{
		// get value wanted per the master
		// --- nothing ---
		// adds values responses for the response to the master !
		// ------------------------------------------------------
		cJSON_AddStringToObject( JsonRoot, "PortName", Preferences.MonitorSlaveOnSerialPort );
		cJSON_AddNumberToObject( JsonRoot, "Speed", Preferences.MonitorSlaveSerialSpeed );
	}
}

char * MonitorCreateRequestWriteMonitorSerialConfig( void )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq;
	
	JsonRoot = cJSON_CreateObject();
	
	cJSON_AddStringToObject( JsonRoot, "ReqCL", "WriteMonitorSerialConfig" );

	cJSON_AddStringToObject( JsonRoot, "PortName", InfosGUI->TargetMonitor.TargetSlaveOnSerialPort );
	cJSON_AddNumberToObject( JsonRoot, "Speed", InfosGUI->TargetMonitor.TargetSlaveSerialSpeed );
	
	// Print to text, Delete the cJSON, use it, release the string.
	TextReq = cJSON_Print( JsonRoot );
	cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	return TextReq;
}

void MonitorParseResponseWriteMonitorSerialConfig( cJSON *JsonRoot, char SlaveMode )
{
	if ( !SlaveMode )
	{
		// get values from the response of the slave...
		// --------------------------------------------
#ifdef GTK_INTERFACE
		if ( cJSON_GetObjectItem( JsonRoot,"Done" )->valueint==0 )
			ShowMessageBoxInBackground( NULL, _("Failed to send monitor serial config on target!") );
		else
			ShowMessageBoxInBackground( _("Info target"), _("Monitor serial config successfully send to target.") );
#endif
		InfosGUI->TargetMonitor.AskTargetToWriteMonitorSerialConfig = -1; //action request to target done
	}
	else
	{
		// get value wanted per the master
		strcpy( Preferences.MonitorSlaveOnSerialPort, cJSON_GetObjectItem( JsonRoot, "PortName" )->valuestring );
		Preferences.MonitorSlaveSerialSpeed = cJSON_GetObjectItem( JsonRoot, "Speed" )->valueint;
				// adds values responses for the response to the master !
		// ------------------------------------------------------
		char WriteConfigOk = 0;
#ifndef MONITOR_TEST_ONLY_NO_RESPONSES_USE
		RemountFileSystemRW( );
		WriteConfigOk = SavePreferences( );
		RemountFileSystemRO( );
#else
		printf( "Monitor: simul but should write monitor serial config...\n" );
#endif
		cJSON_AddNumberToObject( JsonRoot, "Done", WriteConfigOk?1:0 );
	}
}

char * MonitorCreateRequestGetModbusMasterStats( int SlaveNbr)
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq = NULL;

	JsonRoot = cJSON_CreateObject();
	
	cJSON_AddStringToObject( JsonRoot, "ReqCL", "GetStatsModbusM" );

	cJSON_AddNumberToObject( JsonRoot, "Slave", SlaveNbr );

	// Print to text, Delete the cJSON, use it, release the string.
//	TextReq = cJSON_Print( JsonRoot );
	TextReq = cJSON_PrintUnformatted( JsonRoot );
	cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	return TextReq;
}

void MonitorParseResponseGetModbusMasterStats( cJSON *JsonRoot, char SlaveMode )
{
	int SlaveNbr = cJSON_GetObjectItem( JsonRoot,"Slave" )->valueint;
	StrModbusSlave * pSlaveAsked = &ModbusSlaveList[ SlaveNbr ];
	if ( !SlaveMode )
	{
		// get values from the response of the slave...
		// --------------------------------------------
//////		printf( "*** ModbusMasterStats Slave%d : ***\n", SlaveNbr );
//////		printf( "*** %s ***\n", cJSON_GetObjectItem( JsonRoot, "Stats" )->valuestring );
		cJSON * StatsVal = cJSON_GetObjectItem( JsonRoot,"NbrFrames" );
		if ( StatsVal )
			pSlaveAsked->StatsNbrFrames = StatsVal->valueint;
		StatsVal = cJSON_GetObjectItem( JsonRoot,"NbrErrNoResp" );
		if ( StatsVal )
			pSlaveAsked->StatsNbrErrorsNoResponse = StatsVal->valueint;
		StatsVal = cJSON_GetObjectItem( JsonRoot,"NbrErrTreat" );
		if ( StatsVal )
			pSlaveAsked->StatsNbrErrorsModbusTreat = StatsVal->valueint;
		StatsVal = cJSON_GetObjectItem( JsonRoot,"StartTime" );
		if ( StatsVal )
			pSlaveAsked->StatsStartTime = StatsVal->valueint;
		StatsVal = cJSON_GetObjectItem( JsonRoot,"CurrentTime" );
		if ( StatsVal )
			pSlaveAsked->StatsCurrentTime = StatsVal->valueint;
#ifdef GTK_INTERFACE
		g_idle_add( (GSourceFunc)StatisticsModbusSlaveSelectedRefreshInfosGtk, NULL );
#endif
	}
	else
	{
//////		char BuffValue[ 100 ];
		// get values wanted per the master
		// (already done at the top !)
		// adds values responses for the response to the master !
		// ------------------------------------------------------
//////		GetSocketModbusMasterStats( SlaveNbr, BuffValue );
		// keep this to avoid crash with olders versions doing cJSON_GetObjectItem( JsonRoot, "Stats" )->valuestring !
		cJSON_AddStringToObject( JsonRoot, "Stats", "OLD_VERSION!" );
		cJSON_AddNumberToObject( JsonRoot, "NbrFrames", pSlaveAsked->StatsNbrFrames );
		cJSON_AddNumberToObject( JsonRoot, "NbrErrNoResp", pSlaveAsked->StatsNbrErrorsNoResponse );
		cJSON_AddNumberToObject( JsonRoot, "NbrErrTreat", pSlaveAsked->StatsNbrErrorsModbusTreat );
		cJSON_AddNumberToObject( JsonRoot, "StartTime", pSlaveAsked->StatsStartTime );
		cJSON_AddNumberToObject( JsonRoot, "CurrentTime", pSlaveAsked->StatsCurrentTime );
	}
}

//for the master, to ask to cleanup the events log or frames logs
char * MonitorCreateRequestCleanUp( void )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq;
	
	JsonRoot = cJSON_CreateObject();
	
	cJSON_AddStringToObject( JsonRoot, "ReqCL", "CleanUp" );
	cJSON_AddNumberToObject( JsonRoot, "What", InfosGUI->TargetMonitor.AskTargetToCleanUp );
	
	// Print to text, Delete the cJSON, use it, release the string.
	TextReq = cJSON_Print( JsonRoot );
	cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	return TextReq;
}

void MonitorParseResponseCleanUp( cJSON *JsonRoot, char SlaveMode )
{
	int What = cJSON_GetObjectItem( JsonRoot,"What" )->valueint;
	if ( !SlaveMode )
	{
		InfosGUI->TargetMonitor.AskTargetToCleanUp = -1; //action request to target done
		if ( What==10 )
			InfosGene->LogContentModified = TRUE;
		// get values from the response of the slave...
		// --------------------------------------------
		// nothing interesting to read back...
	}
	else
	{
		if ( What==10 )
			InitLogDatas( );
		else if ( What>=20 && What<20+NBR_FRAMES_LOG_BUFFERS )
			FrameLogCleanupAsked( What-20 );
		// adds values responses for the response to the master !
		// ------------------------------------------------------
		// nothing usefull for the master...
	}
}
