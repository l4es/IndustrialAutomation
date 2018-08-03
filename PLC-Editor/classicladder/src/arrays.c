/* Classic Ladder Project */
/* Copyright (C) 2001-2017 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* February 2001 */
/* --------------------------- */
/* Alloc/free global variables */
/* --------------------------- */
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
#include <stdlib.h>
#endif

#ifndef RT_SUPPORT
#include <signal.h>
#else
#include <linux/string.h>
#endif

#include "classicladder.h"
#include "files.h"
#include "calc.h"
#include "vars_access.h"
#include "manager.h"
#include "calc_sequential.h"
#include "symbols.h"
#include "vars_system.h"
#include "time_and_rtc.h"
#include "protocol_modbus_master.h"
#ifdef COMPLETE_PLC
#include "log_events.h"
#include "modem.h"
#endif
#include "tasks.h"
#include "hardware.h"

#if defined( MODULE )
#include <linux/string.h>
#include "module_rtlinux.h"
#define debug_printf rt_debug_printf
#else
#include <string.h>
#endif

#ifdef GTK_INTERFACE
#include <gtk/gtk.h>
#include "classicladder_gtk.h"
#include "manager_gtk.h"
#include "symbols_gtk.h"
//#include <gtk/gtk.h>
#endif

#ifdef MAT_CONNECTION
#include "../../lib/plc.h"
#endif


#if defined( RTLINUX ) || defined ( __RTL__ )
#include "/usr/rtlinux/include/mbuff.h"
#endif

#if defined(RTAI)
#if !defined(MODULE)
#include <stddef.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include "rtai.h"
#include "rtai_shm.h"
#define mbuff_alloc(a, b) rtai_malloc(nam2num(a), b)
#define mbuff_free(a, b) rtai_free(nam2num(a), b)
#else
#include <linux/module.h>
#include "rtai.h"
#include "rtai_shm.h"
#define mbuff_alloc(a, b) rtai_kmalloc(nam2num(a), b)
#define mbuff_free(a, b) rtai_kfree(nam2num(a))
#endif
#endif

StrRung * RungArray;
TYPE_FOR_BOOL_VAR * VarArray;
unsigned char * VarSetArray;
unsigned char * LogVarArray;
int * VarWordArray;
#ifdef OLD_TIMERS_MONOS_SUPPORT
StrTimer * TimerArray;
StrMonostable * MonostableArray;
#endif
StrCounter * CounterArray;
StrTimerIEC * NewTimerArray;
StrRegister * RegisterArray;
int * RegistersListsDatas;
StrArithmExpr * ArithmExpr;
StrInfosGene * InfosGene;
StrInfosGUI * InfosGUI;
StrSection * SectionArray;
#ifdef SEQUENTIAL_SUPPORT
StrSequential * Sequential;
#endif
StrSymbol * SymbolArray;
StrInputFilterAndState * InputFilterAndStateArray;

#ifdef GTK_INTERFACE
/* used for the editor */
StrEditRung EditDatas;
StrArithmExpr * EditArithmExpr;
#endif


// Default sizes values
// and variable used to store parameters before malloc()/init_rt_thread done!
// After init, we used it again to store the current configuration modifications !!!
// The real values allocated are in InfosGene->GeneralParams.SizesInfos...
// The file load/save only use this mirror...
// The periodic tasks parameters only use this mirror everywhere.
StrGeneralParams GeneralParamsMirror;
/* IN COMMENT!!!! = {
	.SizesInfos.nbr_rungs = NBR_RUNGS_DEF,
	.SizesInfos.nbr_bits = NBR_BITS_DEF,
	.SizesInfos.nbr_words = NBR_WORDS_DEF,
#ifdef OLD_TIMERS_MONOS_SUPPORT
	.SizesInfos.nbr_timers = NBR_TIMERS_DEF,
	.SizesInfos.nbr_monostables = NBR_MONOSTABLES_DEF,
#endif
	.SizesInfos.nbr_counters = NBR_COUNTERS_DEF,
	.SizesInfos.nbr_timers_iec = NBR_TIMERS_IEC_DEF,
	.SizesInfos.nbr_registers = NBR_REGISTERS_DEF,
	.SizesInfos.register_list_size = REGISTER_LIST_SIZE_DEF,
	.SizesInfos.nbr_phys_inputs = NBR_PHYS_INPUTS_DEF,
	.SizesInfos.nbr_phys_outputs = NBR_PHYS_OUTPUTS_DEF,
	.SizesInfos.nbr_arithm_expr = NBR_ARITHM_EXPR_DEF,
	.SizesInfos.nbr_sections = NBR_SECTIONS_DEF,
	.SizesInfos.nbr_symbols = NBR_SYMBOLS_DEF,
	.SizesInfos.nbr_phys_words_inputs = NBR_PHYS_WORDS_INPUTS_DEF,
	.SizesInfos.nbr_phys_words_outputs = NBR_PHYS_WORDS_OUTPUTS_DEF,
	.PeriodMilliSecsTaskLogic = PERIODIC_REFRESH_MS_DEF,
	.PeriodMilliSecsTaskScanInputs = PERIODIC_REFRESH_MS_INPUTS_DEF,
	.RealInputsOutputsOnlyOnTarget = FALSE
};*/

//v0.9.5, created a function to be called before loading another project
//(important to have correct default values, if some parameters not saved in older project...)
void InitGeneralParamsMirror( void )
{
	GeneralParamsMirror.SizesInfos.nbr_rungs = NBR_RUNGS_DEF;
	GeneralParamsMirror.SizesInfos.nbr_bits = NBR_BITS_DEF;
	GeneralParamsMirror.SizesInfos.nbr_words = NBR_WORDS_DEF;
#ifdef OLD_TIMERS_MONOS_SUPPORT
	GeneralParamsMirror.SizesInfos.nbr_timers = NBR_TIMERS_DEF;
	GeneralParamsMirror.SizesInfos.nbr_monostables = NBR_MONOSTABLES_DEF;
#endif
	GeneralParamsMirror.SizesInfos.nbr_counters = NBR_COUNTERS_DEF;
	GeneralParamsMirror.SizesInfos.nbr_timers_iec = NBR_TIMERS_IEC_DEF;
	GeneralParamsMirror.SizesInfos.nbr_registers = NBR_REGISTERS_DEF;
	GeneralParamsMirror.SizesInfos.register_list_size = REGISTER_LIST_SIZE_DEF;
	GeneralParamsMirror.SizesInfos.nbr_phys_inputs = NBR_PHYS_INPUTS_DEF;
	GeneralParamsMirror.SizesInfos.nbr_phys_outputs = NBR_PHYS_OUTPUTS_DEF;
	GeneralParamsMirror.SizesInfos.nbr_arithm_expr = NBR_ARITHM_EXPR_DEF;
	GeneralParamsMirror.SizesInfos.nbr_sections = NBR_SECTIONS_DEF;
	GeneralParamsMirror.SizesInfos.nbr_symbols = NBR_SYMBOLS_DEF;
	GeneralParamsMirror.SizesInfos.nbr_phys_words_inputs = NBR_PHYS_WORDS_INPUTS_DEF;
	GeneralParamsMirror.SizesInfos.nbr_phys_words_outputs = NBR_PHYS_WORDS_OUTPUTS_DEF;
	GeneralParamsMirror.PeriodMilliSecsTaskLogic = PERIODIC_REFRESH_MS_DEF;
	GeneralParamsMirror.PeriodMilliSecsTaskScanInputs = PERIODIC_REFRESH_MS_INPUTS_DEF;
	GeneralParamsMirror.RealInputsOutputsOnlyOnTarget = FALSE;
	GeneralParamsMirror.AutomaticallyAdjustSummerWinterTime = FALSE;
}

void InitProjectProperties( void )
{
	char BuffCurrTime[ 30 ];
	GetCurrentAscTime( BuffCurrTime );
	InfosGene->ProjectProperties.ProjectName[ 0 ] = '\0';
	InfosGene->ProjectProperties.ProjectSite[ 0 ] = '\0';
	strcpy( InfosGene->ProjectProperties.ParamVersion, "0" );
	InfosGene->ProjectProperties.ParamAuthor[ 0 ] = '\0';
	InfosGene->ProjectProperties.ParamCompany[ 0 ] = '\0';
	strcpy( InfosGene->ProjectProperties.ParamCreaDate, BuffCurrTime );
	strcpy( InfosGene->ProjectProperties.ParamModifDate, BuffCurrTime );
	InfosGene->ProjectProperties.ParamComment[ 0 ] = '\0';
}

//v0.9.20, function created... to avoid crash sometimes when refreshing in GUI project loaded...
void InitInfosGeneRungDisplayGUI( void )
{
	InfosGene->TopRungDisplayed = 0;
	InfosGene->OffsetHiddenTopRungDisplayed = 0;
	InfosGene->OffsetCurrentRungDisplayed = 0;
	InfosGene->VScrollValue = 0;
	InfosGene->HScrollValue = 0;
}

void InitInfosGene( void )
{
	int Scan;
	InfosGene->LadderState = STATE_LOADING;
	InfosGene->UnderCalculationPleaseWait = FALSE;
	InfosGene->LadderStoppedToRunBack = FALSE;

	InfosGene->CmdRefreshVarsBits = FALSE;

	InfosGene->BlockWidth = BLOCK_WIDTH_DEF;
	InfosGene->BlockHeight = BLOCK_HEIGHT_DEF;
	InfosGene->PageWidth = 0;
	InfosGene->PageHeight = 0;
	InitInfosGeneRungDisplayGUI( );
	InfosGene->HeaderLabelCommentHeight = BLOCK_HEIGHT_DEF*65/100;

	InfosGene->DurationOfLastScan = -1; // unknow value, to do not display!
	InfosGene->MaxScanDuration = -1;
	InfosGene->NbrTicksMissed = -1; // the same...
	InfosGene->CurrentSection = 0;

	InfosGene->AskConfirmationToQuit = FALSE;
	InfosGene->AskConfirmationToCancel = FALSE;
	InfosGene->HasBeenModifiedForExitCode = FALSE;
	InfosGene->ErrorMsgStringToDisplay[ 0 ] = '\0'; //no error for now!
	InfosGene->ErrorTitleStringToDisplay[ 0 ] = '\0';
//	InfosGene->DisplaySymbols = TRUE;

	InfosGene->AskToConfHardInputs = FALSE;
	InfosGene->AskToConfHardOutputs = FALSE;
	InfosGene->LogContentModified = FALSE;
	InfosGene->DefaultLogListModified = FALSE;
	InfosGene->BackgroundSaveLogEventsData = FALSE;
	
	InfosGene->BackgroundSaveSetVarsList = FALSE;
	InfosGene->DoNotSaveDuringLoadingSetVarsList = FALSE;
	
	InfosGene->LifeLedState = FALSE;
	for( Scan=0; Scan<NBR_USERS_LEDS; Scan++)
		InfosGene->UserLedVar[ Scan ] = FALSE;
	
	InfosGene->TransmitAlarmsGlobalFlagForSms = FALSE;
	InfosGene->TransmitAlarmsGlobalFlagForEmails = FALSE;
}

void InitTargetMonitorAsks( void )
{
	InfosGUI->TargetMonitor.AskTargetToSetState = -1;
	InfosGUI->TargetMonitor.AskTargetToReset = -1;
	InfosGUI->TargetMonitor.AskTargetToSetClockTime = -1;
	InfosGUI->TargetMonitor.AskTargetToReboot = -1;
	InfosGUI->TargetMonitor.AskTargetToGetTargetInfos = -1;
	InfosGUI->TargetMonitor.AskTargetToGetProjectProperties = -1;
	InfosGUI->TargetMonitor.AskTargetToWriteVarType = -1;
	InfosGUI->TargetMonitor.AskTargetToWriteVarNum = -1;
	InfosGUI->TargetMonitor.AskTargetToWriteVarNewValue = -1;
	InfosGUI->TargetMonitor.AskTargetToSetOrUnsetVarType = -1;
	InfosGUI->TargetMonitor.AskTargetToSetOrUnsetVarNum = -1;
	InfosGUI->TargetMonitor.AskTargetToSetOrUnsetVarValue = -1;
	InfosGUI->TargetMonitor.AskTargetToGetNetworkConfig = -1;
	InfosGUI->TargetMonitor.AskTargetToWriteNetworkConfig = -1;
	InfosGUI->TargetMonitor.AskTargetToGetMonitorSerialConfig = -1;
	InfosGUI->TargetMonitor.AskTargetToWriteMonitorSerialConfig = -1;
	InfosGUI->TargetMonitor.AskTargetToCleanUp = -1;
	
	InfosGUI->TargetMonitor.AskTargetForThisManualFrame = NULL;
}

void InitInfosGUI( void )
{
	int ScanIt;
	InfosGUI->TargetMonitor.RemoteConnected = FALSE;
	InfosGUI->TargetMonitor.PreviousRemoteConnected = FALSE;
	InfosGUI->TargetMonitor.RemoteAdrIP[ 0 ] = '\0';
	InfosGUI->TargetMonitor.RemoteWithSerialPort[ 0 ] = '\0';
	InfosGUI->TargetMonitor.RemoteWithSerialSpeed = 38400 /*115200*/;
	InfosGUI->TargetMonitor.TimeOutWaitReply = 300;
	InfosGUI->TargetMonitor.RemoteWithSerialModem = FALSE;
	InfosGUI->TargetMonitor.RemoteTelephoneNumber[ 0 ] = '\0';
	
	InfosGUI->TargetMonitor.RemoteFileTransfer = FALSE;
	InfosGUI->TargetMonitor.PreviousRemoteFileTransfer = FALSE;
	InfosGUI->TargetMonitor.TransferFileNum = -1;
	InfosGUI->TargetMonitor.TransferFileSubNum = 0;
	InfosGUI->TargetMonitor.TransferFileSelectedName[ 0 ] = '\0';
	InfosGUI->TargetMonitor.TransferFileIsSend = FALSE;
	
	InfosGUI->TargetMonitor.LatestTargetState = -1;
	
	InitTargetMonitorAsks( );
	
	InfosGUI->TargetMonitor.CmdDisplayTargetInfosVersion = FALSE;
	InfosGUI->TargetMonitor.CmdDisplayTargetProjectProperties = FALSE;
	InfosGUI->TargetMonitor.CmdDisplayTargetNetworkConfig = FALSE;
	
	InfosGUI->TargetMonitor.CmdDisplayTargetMonitorSerialConfig = FALSE;
	InfosGUI->TargetMonitor.TargetSlaveOnSerialPort[ 0 ] = '\0';
	InfosGUI->TargetMonitor.TargetSlaveSerialSpeed = 38400;

	for( ScanIt=0; ScanIt<NBR_TYPE_BOOLS_SPY; ScanIt++ )
		InfosGUI->ValOffsetBoolVar[ ScanIt ] = 0;
	for( ScanIt=0; ScanIt<NBR_FREE_VAR_SPY; ScanIt++ )
	{
		InfosGUI->FreeVarSpy[ ScanIt ].VarType = VAR_MEM_WORD;
		InfosGUI->FreeVarSpy[ ScanIt ].VarNum = ScanIt;
		InfosGUI->FreeVarSpyDisplayFormat[ ScanIt ] = 0;
	}
	InfosGUI->NbrRungsDisplayedToMonitor = 0;
}

void * MyMalloc( char * Name, int Size )
{
#ifndef RT_SUPPORT
	return malloc( Size );
#else
	return mbuff_alloc( Name, Size );
#endif
}
void MyFree( char * Name, void * DatasPtr )
{
#ifndef RT_SUPPORT
	free( DatasPtr );
#else
	mbuff_free( Name, DatasPtr );
#endif
}

/* return TRUE if okay */
int ClassicLadder_AllocAllInfos( void )
{
	InfosGene = (StrInfosGene *)MyMalloc( "InfosGene", sizeof(StrInfosGene) );
	if (!InfosGene)
	{
#ifndef RT_SUPPORT
		debug_printf("Failed to alloc InfosGene !\n");
#else
		rt_debug_printf("Failed to alloc InfosGene shared memory !\n");
		rt_debug_printf("The real-time module must be inserted before...\n");
		rt_debug_printf("See the README to know how you must launch the real-time version !\n");
#endif
		return FALSE;
	}
// before alloc/free rework here set sizes values before making next allocs...

#ifdef GTK_INTERFACE
	InfosGUI = (StrInfosGUI *)malloc( sizeof(StrInfosGUI) );
	if (!InfosGUI)
	{
		debug_printf("Failed to alloc InfosGUI !\n");
		return FALSE;
	}
#else
	InfosGUI = NULL;
#endif

	InitInfosGene( );
#ifdef GTK_INTERFACE
	InitInfosGUI( );
#endif

	InitGeneralParamsMirror( );
//////	if ( !ClassicLadder_AllocProjectDatas( ) )
//////		return FALSE;

	return TRUE;
}

int ClassicLadder_AllocNewProjectDatas( void )
{
	int Result;
	printf("In %s()...\n",__FUNCTION__);
	ClassicLadder_FreeProjectDatas( );
	InitGeneralParamsMirror( );
	Result = ClassicLadder_AllocProjectDatas();
//moved here in v0.9.100
InfosGene->CurrentProjectFileName[0] = '\0'; // no current project name
InfosGene->AskConfirmationToQuit = TRUE;
InfosGene->HasBeenModifiedForExitCode = TRUE;
//added in v0.9.100, else in "loading" no display...
InfosGene->LadderState = STATE_STOP;
	return Result; 
}

/* return TRUE if okay */
int ClassicLadder_AllocProjectDatas( void )
{
	// set sizes values before making the next allocs...
	memcpy( &InfosGene->GeneralParams, &GeneralParamsMirror, sizeof( StrGeneralParams ) );
#ifdef GTK_INTERFACE
	UpdateSizesOfConvVarNameTable( );
#endif
#ifdef RT_SUPPORT
	// get RT module parameters
	CopySizesInfosFromModuleParams( );
#endif

printf("[[[ In %s(), will alloc '%d' Rungs... ]]]\n", __FUNCTION__, NBR_RUNGS );
	RungArray = (StrRung *)MyMalloc( "Rungs", NBR_RUNGS * sizeof(StrRung) );
	if (!RungArray)
	{
		debug_printf("Failed to alloc RungArray !\n");
		return FALSE;
	}
#ifdef OLD_TIMERS_MONOS_SUPPORT
	TimerArray = (StrTimer *)MyMalloc( "Timers", NBR_TIMERS * sizeof(StrTimer) );
	if (!TimerArray)
	{
		debug_printf("Failed to alloc TimerArray !\n");
		return FALSE;
	}
	MonostableArray = (StrMonostable *)MyMalloc( "Monostables", NBR_MONOSTABLES * sizeof(StrMonostable) );
	if (!MonostableArray)
	{
		debug_printf("Failed to alloc MonostableArray !\n");
		return FALSE;
	}
#endif
	CounterArray = (StrCounter *)MyMalloc( "Counters", NBR_COUNTERS * sizeof(StrCounter) );
	if (!CounterArray)
	{
		debug_printf("Failed to alloc CounterArray !\n");
		return FALSE;
	}
printf("[[[ In %s(), will alloc '%d' TimersIEC... ]]]\n", __FUNCTION__, NBR_TIMERS_IEC );
	NewTimerArray = (StrTimerIEC *)MyMalloc( "TimersIEC", NBR_TIMERS_IEC * sizeof(StrTimerIEC) );
	if (!NewTimerArray)
	{
		debug_printf("Failed to alloc NewTimerArray !\n");
		return FALSE;
	}
	RegisterArray = (StrRegister *)MyMalloc( "Registers", NBR_REGISTERS * sizeof(StrRegister) );
	if (!RegisterArray)
	{
		debug_printf("Failed to alloc RegisterArray !\n");
		return FALSE;
	}
printf("[[[ In %s(), will alloc '%d' datas for %d registers... ]]]\n", __FUNCTION__, NBR_REGISTERS * REGISTER_LIST_SIZE * sizeof(int), NBR_REGISTERS );
	RegistersListsDatas = (int *)MyMalloc( "RegistersLists", NBR_REGISTERS * REGISTER_LIST_SIZE * sizeof(int) );
	if (!RegistersListsDatas)
	{
		debug_printf("Failed to alloc RegisterListDatas !\n");
		return FALSE;
	}
	VarArray = (TYPE_FOR_BOOL_VAR *)MyMalloc( "VarsBits", SIZE_VAR_ARRAY * sizeof(TYPE_FOR_BOOL_VAR) );
	if (!VarArray)
	{
		debug_printf("Failed to alloc VarArray !\n");
		return FALSE;
	}
printf("[[[ In %s(), will alloc '%d' VarsSets... ]]]\n", __FUNCTION__, SIZE_VAR_SET_ARRAY );
	VarSetArray = (unsigned char *)MyMalloc( "VarsSets", SIZE_VAR_SET_ARRAY * sizeof(unsigned char) );
	if (!VarSetArray)
	{
		debug_printf("Failed to alloc VarSetArray !\n");
		return FALSE;
	}
	LogVarArray = (unsigned char *)MyMalloc( "LogVarsBits", SIZE_VAR_ARRAY * sizeof(unsigned char) );
	if (!LogVarArray)
	{
		debug_printf("Failed to alloc LogVarArray !\n");
		return FALSE;
	}
	VarWordArray = (int *)MyMalloc( "VarWords", SIZE_VAR_WORD_ARRAY * sizeof(int) );
	if (!VarWordArray)
	{
		debug_printf("Failed to alloc VarWordArray !\n");
		return FALSE;
	}
	ArithmExpr = (StrArithmExpr *)MyMalloc( "ArithmExpr", NBR_ARITHM_EXPR * sizeof(StrArithmExpr) );
	if (!ArithmExpr)
	{
		debug_printf("Failed to alloc ArithmExpr !\n");
		return FALSE;
	}
printf("[[[ In %s(), will alloc '%d' Sections... ]]]\n", __FUNCTION__, NBR_SECTIONS );
	SectionArray = (StrSection *)MyMalloc( "Sections", NBR_SECTIONS * sizeof(StrSection) );
	if (!SectionArray)
	{
		debug_printf("Failed to alloc SectionArray !\n");
		return FALSE;
	}
#ifdef SEQUENTIAL_SUPPORT
	Sequential = (StrSequential *)MyMalloc( "Sequential", sizeof(StrSequential) );
	if (!Sequential)
	{
		debug_printf("Failed to alloc Sequential !\n");
		return FALSE;
	}
#endif
	SymbolArray = (StrSymbol *)MyMalloc( "Symbols", NBR_SYMBOLS * sizeof(StrSymbol) );
	if (!SymbolArray)
	{
		debug_printf("Failed to alloc SymbolArray !\n");
		return FALSE;
	}
	InputFilterAndStateArray = (StrInputFilterAndState *)MyMalloc( "InputFilterAndState", NBR_PHYS_INPUTS * sizeof(StrInputFilterAndState) );
	if (!InputFilterAndStateArray)
	{
		debug_printf("Failed to alloc InputFilterAndStateArray !\n");
		return FALSE;
	}

#ifdef GTK_INTERFACE
	EditArithmExpr = (StrArithmExpr *)malloc( NBR_ARITHM_EXPR * sizeof(StrArithmExpr) );
	if (!EditArithmExpr)
	{
		debug_printf("Failed to alloc EditArithmExpr !\n");
		return FALSE;
	}
#endif

	ClassicLadder_InitProjectDatas( );
	InitSetsVars( );
	//v0.9.20
	InitInfosGeneRungDisplayGUI( );
	
	return TRUE;
}

void ClassicLadder_FreeAllInfos( char CleanAndRemoveTmpDir )
{
	InfosGene->LadderState = STATE_LOADING;
//////	ClassicLadder_FreeProjectDatas( );
#ifdef GTK_INTERFACE
	if ( InfosGUI )
		free( InfosGUI );
	InfosGUI = NULL;
#endif
	if (InfosGene)
		MyFree("InfosGene",InfosGene);
	InfosGene = NULL;

	if ( CleanAndRemoveTmpDir )
		CleanTmpLadderDirectory( TRUE/*RemoveTmpDirAtEnd*/ );

}

void ClassicLadder_FreeProjectDatas( void )
{

	InfosGene->LadderState = STATE_LOADING;
	if (RungArray)
		MyFree("Rungs",RungArray);
	RungArray = NULL;
#ifdef OLD_TIMERS_MONOS_SUPPORT
	if (TimerArray)
		MyFree("Timers",TimerArray);
	TimerArray = NULL;
	if (MonostableArray)
		MyFree("Monostables",MonostableArray);
	MonostableArray = NULL;
#endif
	if (CounterArray)
		MyFree("Counters",CounterArray);
	CounterArray = NULL;
	if (NewTimerArray)
		MyFree("TimersIEC",NewTimerArray);
	NewTimerArray = NULL;
	if (RegisterArray)
		MyFree("Registers",RegisterArray);
	RegisterArray = NULL;
	if (RegistersListsDatas)
		MyFree("RegistersLists",RegistersListsDatas);
	RegistersListsDatas = NULL;
	if (VarArray)
		MyFree("VarsBits",VarArray);
	VarArray = NULL;
	if (VarSetArray)
		MyFree("VarsSets",VarSetArray);
	VarSetArray = NULL;
	if (LogVarArray)
		MyFree("LogVarsBits",LogVarArray);
	LogVarArray = NULL;
	if (VarWordArray)
		MyFree("VarWords",VarWordArray);
	VarWordArray = NULL;
	if (ArithmExpr)
		MyFree("ArithmExpr",ArithmExpr);
	ArithmExpr = NULL;
	if (SectionArray)
		MyFree("Sections",SectionArray);
	SectionArray = NULL;
#ifdef SEQUENTIAL_SUPPORT
	if (Sequential)
		MyFree("Sequential",Sequential);
	Sequential = NULL;
#endif
	if (SymbolArray)
		MyFree("Symbols",SymbolArray);
	SymbolArray = NULL;
	if (InputFilterAndStateArray)
		MyFree("InputFilterAndState",InputFilterAndStateArray);
	InputFilterAndStateArray = NULL;

#ifdef GTK_INTERFACE
	if (EditArithmExpr)
		free( EditArithmExpr );
	EditArithmExpr = NULL;
#endif
}

void ClassicLadder_InitProjectDatas( void )
{
	InitProjectProperties( );
	InitIOConf( );
	InitVars( FALSE/*DoLogEvents*/ );
#ifdef OLD_TIMERS_MONOS_SUPPORT
	InitTimers();
	InitMonostables();
#endif
	InitCounters();
	InitTimersIEC();
	InitRegisters();
	InitArithmExpr();
	InitRungs();
	InitSections( );
#ifdef SEQUENTIAL_SUPPORT
	InitSequential( );
#endif
	//added in v0.9.1, was forgotten...
	InitModbusMasterParams( );
	InitSymbols( );
	InitSystemVars( TRUE );

#ifdef COMPLETE_PLC
	InitLogConfig( );
	InitRemoteAlarmsConfig( );
	InitVarsArrayLogTags( );
	InitModem( );
#endif

	InfosGene->AskConfirmationToQuit = FALSE;
	InfosGene->AskConfirmationToCancel = FALSE;
	InfosGene->HasBeenModifiedForExitCode = FALSE;

}

int ClassicLadder_FreeAllocInitProjectDatas( void )
{
	ClassicLadder_FreeProjectDatas( );
	if ( !ClassicLadder_AllocProjectDatas( ) )
		return FALSE;
	return TRUE;
}
