/* Classic Ladder Project */
/* Copyright (C) 2001-2017 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* February 2001 */
/* --------------------------------------- */
/* Access a variable for reading / writing */
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

#ifdef GTK_INTERFACE
#include <gtk/gtk.h>
#include "classicladder_gtk.h"
#endif

#if defined(MODULE) && defined(RTAI)
#include <linux/kernel.h>
#include <linux/module.h>
#include "rtai.h"
#else
#include <stdio.h>
#include <stdlib.h>
#endif
#include "classicladder.h"
#include "global.h"
#include "vars_access.h"
#ifdef COMPLETE_PLC
#include "log_events.h"
#endif

#define MASK_VAR_SETTED 0x80

void InitVars( char DoLogEvents )
{
	int NumVar;
	for (NumVar=0; NumVar<SIZE_VAR_ARRAY; NumVar++)
	{
		if ( DoLogEvents )
			WriteVar( VAR_MEM_BIT, NumVar, FALSE );
		else
			VarArray[NumVar] = FALSE;
	}
	for (NumVar=0; NumVar<SIZE_VAR_WORD_ARRAY; NumVar++)
		VarWordArray[NumVar] = 0;
	
	for (NumVar=0; NumVar<NBR_PHYS_INPUTS; NumVar++)
	{
		StrInputFilterAndState * pInput = &InputFilterAndStateArray[ NumVar ];
		pInput->FilterMask = 0x7;
		pInput->BuffRawInput = 0;
		pInput->InputStateForLogic = -1; // no "real" filtered value for now
	}
	
	/* to tell the GTK application to refresh the bits */
	InfosGene->CmdRefreshVarsBits = TRUE;
}
void InitSetsVars( void )
{
	int NumVar;
printf("-->init VarSetArray[] here...\n");
	for (NumVar=0; NumVar<SIZE_VAR_SET_ARRAY; NumVar++)
		VarSetArray[ NumVar ] = 0;
}

int GetNbrVarsForType( int TypeVar )
{
	switch(TypeVar)
	{
		case VAR_MEM_BIT:
			return NBR_BITS;
		case VAR_PHYS_INPUT:
			return NBR_PHYS_INPUTS;
		case VAR_PHYS_OUTPUT:
			return NBR_PHYS_OUTPUTS;
		case VAR_USER_LED:
			return NBR_USERS_LEDS;
		case VAR_SYSTEM:
			return NBR_VARS_SYSTEM;
#ifdef SEQUENTIAL_SUPPORT
		case VAR_STEP_ACTIVITY:
			return NBR_STEPS;
#endif
#ifdef OLD_TIMERS_MONOS_SUPPORT
		case VAR_TIMER_DONE:
			return NBR_TIMERS;
		case VAR_TIMER_RUNNING:
			return NBR_TIMERS;
		case VAR_MONOSTABLE_RUNNING:
			return NBR_MONOSTABLES;
#endif
		case VAR_COUNTER_DONE:
			return NBR_COUNTERS;
		case VAR_COUNTER_EMPTY:
			return NBR_COUNTERS;
		case VAR_COUNTER_FULL:
			return NBR_COUNTERS;
		case VAR_TIMER_IEC_DONE:
			return NBR_TIMERS_IEC;
		case VAR_REGISTER_EMPTY:
		case VAR_REGISTER_FULL:
			return NBR_REGISTERS;
		case VAR_MEM_WORD:
			return NBR_WORDS;
		case VAR_PHYS_WORD_INPUT:
			return NBR_PHYS_WORDS_INPUTS;
		case VAR_PHYS_WORD_OUTPUT:
			return NBR_PHYS_WORDS_OUTPUTS;
		case VAR_WORD_SYSTEM:
			return NBR_VARS_WORDS_SYSTEM;
#ifdef SEQUENTIAL_SUPPORT
		case VAR_STEP_TIME:
			return NBR_STEPS;
#endif
#ifdef OLD_TIMERS_MONOS_SUPPORT
		case VAR_TIMER_PRESET:
			return NBR_TIMERS;
		case VAR_TIMER_VALUE:
			return NBR_TIMERS;
		case VAR_MONOSTABLE_PRESET:
			return NBR_MONOSTABLES;
		case VAR_MONOSTABLE_VALUE:
			return NBR_MONOSTABLES;
#endif
		case VAR_COUNTER_PRESET:
			return NBR_COUNTERS;
		case VAR_COUNTER_VALUE:
			return NBR_COUNTERS;
		case VAR_TIMER_IEC_PRESET:
			return NBR_TIMERS_IEC;
		case VAR_TIMER_IEC_VALUE:
			return NBR_TIMERS_IEC;
		case VAR_REGISTER_IN_VALUE:
		case VAR_REGISTER_OUT_VALUE:
		case VAR_REGISTER_NBR_VALUES:
			return NBR_REGISTERS;
		default:
			rt_debug_printf("!!! Error : Type (=%d) not found in %s(%d)\n", TypeVar, __FUNCTION__, TypeVar);
	}
	return -1;
}

int ReadVar(int TypeVar,int Offset)
{
	switch(TypeVar)
	{
		case VAR_MEM_BIT:
			return VarArray[Offset];
		case VAR_PHYS_INPUT:
			if ( VarSetArray[ Offset ]&MASK_VAR_SETTED )
				return VarSetArray[ Offset ]&1;
			else
				return VarArray[NBR_BITS+Offset];
		case VAR_PHYS_OUTPUT:
			if ( VarSetArray[NBR_PHYS_INPUTS+Offset]&MASK_VAR_SETTED )
				return VarSetArray[NBR_PHYS_INPUTS+Offset]&1;
			else
				return VarArray[NBR_BITS+NBR_PHYS_INPUTS+Offset];
		case VAR_USER_LED:
			return InfosGene->UserLedVar[Offset];
		case VAR_SYSTEM:
			return VarArray[NBR_BITS+NBR_PHYS_INPUTS+NBR_PHYS_OUTPUTS+Offset];
#ifdef SEQUENTIAL_SUPPORT
		case VAR_STEP_ACTIVITY:
//			return Sequential->Step[ Offset ].Activated;
			return VarArray[NBR_BITS+NBR_PHYS_INPUTS+NBR_PHYS_OUTPUTS+NBR_VARS_SYSTEM+Offset];
#endif
#ifdef OLD_TIMERS_MONOS_SUPPORT
		case VAR_TIMER_DONE:
			return TimerArray[Offset].OutputDone;
		case VAR_TIMER_RUNNING:
			return TimerArray[Offset].OutputRunning;
		case VAR_MONOSTABLE_RUNNING:
			return MonostableArray[Offset].OutputRunning;
#endif
		case VAR_COUNTER_DONE:
			return CounterArray[Offset].OutputDone;
		case VAR_COUNTER_EMPTY:
			return CounterArray[Offset].OutputEmpty;
		case VAR_COUNTER_FULL:
			return CounterArray[Offset].OutputFull;
		case VAR_TIMER_IEC_DONE:
			return NewTimerArray[Offset].Output;
		case VAR_REGISTER_EMPTY:
			return RegisterArray[Offset].OutputEmpty;
		case VAR_REGISTER_FULL:
			return RegisterArray[Offset].OutputFull;
		case VAR_MEM_WORD:
			return VarWordArray[Offset];
		case VAR_PHYS_WORD_INPUT:
			return VarWordArray[NBR_WORDS+Offset];
		case VAR_PHYS_WORD_OUTPUT:
			return VarWordArray[NBR_WORDS+NBR_PHYS_WORDS_INPUTS+Offset];
		case VAR_WORD_SYSTEM:
			return VarWordArray[NBR_WORDS+NBR_PHYS_WORDS_INPUTS+NBR_PHYS_WORDS_OUTPUTS+Offset];
#ifdef SEQUENTIAL_SUPPORT
		case VAR_STEP_TIME:
//			return Sequential->Step[ Offset ].TimeActivated/1000;
			return VarWordArray[NBR_WORDS+NBR_PHYS_WORDS_INPUTS+NBR_PHYS_WORDS_OUTPUTS+NBR_VARS_WORDS_SYSTEM+Offset];
#endif
#ifdef OLD_TIMERS_MONOS_SUPPORT
		case VAR_TIMER_PRESET:
			return TimerArray[Offset].Preset/TimerArray[Offset].Base;
		case VAR_TIMER_VALUE:
			return TimerArray[Offset].Value/TimerArray[Offset].Base;
		case VAR_MONOSTABLE_PRESET:
			return MonostableArray[Offset].Preset/MonostableArray[Offset].Base;
		case VAR_MONOSTABLE_VALUE:
			return MonostableArray[Offset].Value/MonostableArray[Offset].Base;
#endif
		case VAR_COUNTER_PRESET:
			return CounterArray[Offset].Preset;
		case VAR_COUNTER_VALUE:
			return CounterArray[Offset].Value;
		case VAR_TIMER_IEC_PRESET:
			return NewTimerArray[Offset].Preset;
		case VAR_TIMER_IEC_VALUE:
			return NewTimerArray[Offset].Value;
		case VAR_REGISTER_IN_VALUE:
			return RegisterArray[Offset].ValueIn;
		case VAR_REGISTER_OUT_VALUE:
			return RegisterArray[Offset].ValueOut;
		case VAR_REGISTER_NBR_VALUES:
			return RegisterArray[Offset].NbrValuesSaved;
		default:
			rt_debug_printf("!!! Error : Type (=%d) not found in ReadVar(%d,%d)\n", TypeVar, TypeVar, Offset);
	}
	return 0;
}

void WriteVar(int TypeVar,int NumVar,int Value)
{
	switch(TypeVar)
	{
		case VAR_MEM_BIT:
#ifdef COMPLETE_PLC
			if (InfosGene->LadderState==STATE_RUN)
			{
				// log of this variable wanted ?
				if ( LogVarArray[ NumVar ] )
				{
					// a new state ?
					if ( VarArray[NumVar]!=Value )
						VarStateChanged( TypeVar, NumVar, Value );
				}
			}
#endif
			VarArray[NumVar] = Value;
			break;
		case VAR_PHYS_INPUT:
			VarArray[NBR_BITS+NumVar] = Value;
			break;
		case VAR_PHYS_OUTPUT:
			VarArray[NBR_BITS+NBR_PHYS_INPUTS+NumVar] = Value;
			break;
		case VAR_USER_LED:
			InfosGene->UserLedVar[NumVar] = Value;
			break;
		case VAR_SYSTEM:
			VarArray[NBR_BITS+NBR_PHYS_INPUTS+NBR_PHYS_OUTPUTS+NumVar] = Value;
			break;
#ifdef SEQUENTIAL_SUPPORT
		case VAR_STEP_ACTIVITY:
			VarArray[NBR_BITS+NBR_PHYS_INPUTS+NBR_PHYS_OUTPUTS+NBR_VARS_SYSTEM+NumVar] = Value;
			break;
#endif
		case VAR_COUNTER_DONE:
			CounterArray[NumVar].OutputDone = Value;
			break;
		case VAR_COUNTER_EMPTY:
			CounterArray[NumVar].OutputEmpty = Value;
			break;
		case VAR_COUNTER_FULL:
			CounterArray[NumVar].OutputFull = Value;
			break;
		case VAR_TIMER_IEC_DONE:
			NewTimerArray[NumVar].Output = Value;
			break;
		case VAR_REGISTER_EMPTY:
			RegisterArray[NumVar].OutputEmpty = Value;
			break;
		case VAR_REGISTER_FULL:
			RegisterArray[NumVar].OutputFull = Value;
			break;
		case VAR_MEM_WORD:
			VarWordArray[NumVar] = Value;
			break;
		case VAR_PHYS_WORD_INPUT:
			VarWordArray[NBR_WORDS+NumVar] = Value;
			break;
		case VAR_PHYS_WORD_OUTPUT:
			VarWordArray[NBR_WORDS+NBR_PHYS_WORDS_INPUTS+NumVar] = Value;
			break;
		case VAR_WORD_SYSTEM:
			VarWordArray[NBR_WORDS+NBR_PHYS_WORDS_INPUTS+NBR_PHYS_WORDS_OUTPUTS+NumVar] = Value;
			break;
#ifdef SEQUENTIAL_SUPPORT
		case VAR_STEP_TIME:
			VarWordArray[NBR_WORDS+NBR_PHYS_WORDS_INPUTS+NBR_PHYS_WORDS_OUTPUTS+NBR_VARS_WORDS_SYSTEM+NumVar] = Value;
			break;
#endif
#ifdef OLD_TIMERS_MONOS_SUPPORT
		case VAR_TIMER_PRESET:
			TimerArray[NumVar].Preset = Value * TimerArray[NumVar].Base;
			break;
		case VAR_MONOSTABLE_PRESET:
			MonostableArray[NumVar].Preset = Value * MonostableArray[NumVar].Base;
			break;
#endif
		case VAR_COUNTER_PRESET:
			CounterArray[NumVar].Preset = Value;
			break;
		case VAR_COUNTER_VALUE:
			CounterArray[NumVar].Value = Value;
			break;
		case VAR_TIMER_IEC_PRESET:
			NewTimerArray[NumVar].Preset = Value;
			break;
		case VAR_TIMER_IEC_VALUE:
			NewTimerArray[NumVar].Value = Value;
			break;
		case VAR_REGISTER_IN_VALUE:
			RegisterArray[NumVar].ValueIn = Value;
			break;
		case VAR_REGISTER_OUT_VALUE:
			RegisterArray[NumVar].ValueOut = Value;
			break;
		case VAR_REGISTER_NBR_VALUES:
			RegisterArray[NumVar].NbrValuesSaved = Value;
			break;
		default:
			rt_debug_printf("!!! Error : Type (=%d) not found in WriteVar(%d,%d)\n", TypeVar, TypeVar, NumVar);
			break;
	}

	switch(TypeVar)
	{
		case VAR_MEM_BIT:
		case VAR_PHYS_INPUT:
		case VAR_PHYS_OUTPUT:
// with a thread for all versions, do no more call a gtk function from this thread !
//////			// for Xenomai, do not do it now to avoid a domain mode switch !
//////if defined( GTK_INTERFACE ) && !defined( __XENO__ )
//////			RefreshOneBoolVar( TypeVar, NumVar, Value );
//////else
			/* to tell the GTK application to refresh the bits */
			InfosGene->CmdRefreshVarsBits = TRUE;
//////#endif
			break;
	}
}
void WriteBoolVarWithoutLog( int NumVar,int Value )
{
	VarArray[NumVar] = Value;
	/* to tell the GTK application to refresh the bits */
	InfosGene->CmdRefreshVarsBits = TRUE;
}

void SetVar( int TypeVar, int NumVar, int SetValue )
{
	switch( TypeVar )
	{
		case VAR_PHYS_INPUT:
			VarSetArray[ NumVar ] = SetValue|MASK_VAR_SETTED;
			InfosGene->BackgroundSaveSetVarsList = TRUE;
			/* to tell the GTK application to refresh the bits */
			InfosGene->CmdRefreshVarsBits = TRUE;
			break;
		case VAR_PHYS_OUTPUT:
			VarSetArray[NBR_PHYS_INPUTS+NumVar] = SetValue|MASK_VAR_SETTED;
			InfosGene->BackgroundSaveSetVarsList = TRUE;
			/* to tell the GTK application to refresh the bits */
			InfosGene->CmdRefreshVarsBits = TRUE;
			break;
		default:
			rt_debug_printf("!!! Error : Unsupported type in %s(%d,%d)\n", __FUNCTION__, TypeVar, NumVar);
			break;
	}
}
void UnsetVar( int TypeVar, int NumVar )
{
	switch( TypeVar )
	{
		case VAR_PHYS_INPUT:
			VarSetArray[ NumVar ] = 0;
			InfosGene->BackgroundSaveSetVarsList = TRUE;
			/* to tell the GTK application to refresh the bits */
			InfosGene->CmdRefreshVarsBits = TRUE;
			break;
		case VAR_PHYS_OUTPUT:
			VarSetArray[NBR_PHYS_INPUTS+NumVar] = 0;
			InfosGene->BackgroundSaveSetVarsList = TRUE;
			/* to tell the GTK application to refresh the bits */
			InfosGene->CmdRefreshVarsBits = TRUE;
			break;
		default:
			rt_debug_printf("!!! Error : Unsupported type in %s(%d,%d)\n", __FUNCTION__, TypeVar, NumVar);
			break;
	}
}
char IsVarSet( int TypeVar, int NumVar )
{
	char IsSet = FALSE;
	switch( TypeVar )
	{
		case VAR_PHYS_INPUT:
			IsSet = VarSetArray[ NumVar ]&MASK_VAR_SETTED;
			break;
		case VAR_PHYS_OUTPUT:
			IsSet = VarSetArray[NBR_PHYS_INPUTS+NumVar]&MASK_VAR_SETTED;
			break;
		default:
			rt_debug_printf("!!! Error : Unsupported type in %s(%d,%d)\n", __FUNCTION__, TypeVar, NumVar);
			break;
	}
	return IsSet;
}

void DumpVars(void)
{
	int NumVar;
	for (NumVar=0; NumVar<20; NumVar++)
		rt_debug_printf("Var %d=%d\n",NumVar,ReadVar(VAR_MEM_BIT,NumVar));
}

/* these are only useful for the MAT-connected version */
void DoneVars(void) {}
void CycleStart(void) {}
void CycleEnd(void) {}

