/* Classic Ladder Project */
/* Copyright (C) 2001-2015 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* February 2001 */
/* -------------- */
/* Refresh a rung */
/* -------------- */
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

#ifdef MODULE
#include <linux/string.h>
#ifdef RTAI
#include "rtai.h"
#include "rtai_sched.h"
#endif
#else
#include <stdio.h>
#include <string.h>
#include <zlib.h>
#endif
#ifdef __RTL__
#include <rtlinux_signal.h>
#endif

#include "classicladder.h"
#include "global.h"
#include "vars_access.h"
#include "arithm_eval.h"
#include "manager.h"
#ifdef SEQUENTIAL_SUPPORT
#include "calc_sequential.h"
#endif
#include "calc.h"

void InitRungs()
{
	int NumRung;
	for (NumRung=0;NumRung<NBR_RUNGS;NumRung++)
		InitRung( &RungArray[NumRung] );
	// the rung used in the default section created per default
	InfosGene->FirstRung = 0;
	InfosGene->LastRung = 0;
	InfosGene->CurrentRung = 0;
	RungArray[0].Used = TRUE;
}
void InitRung( StrRung * pRung )
{
	int x,y;
	for (y=0;y<RUNG_HEIGHT;y++)
	{
		for(x=0;x<RUNG_WIDTH;x++)
		{
			pRung->Element[x][y].Type = ELE_FREE;
			pRung->Element[x][y].ConnectedWithTop = 0;
			pRung->Element[x][y].VarType = 0;
			pRung->Element[x][y].VarNum = 0;
			pRung->Element[x][y].IndexedVarType = -1; /* undefined */
			pRung->Element[x][y].IndexedVarNum = 0;
			pRung->Element[x][y].DynamicInput = 0;
			pRung->Element[x][y].DynamicState = 0;
			pRung->Element[x][y].DynamicVarBak = 0;
			pRung->Element[x][y].DynamicOutput = 0;
			pRung->Element[x][y].DynamicVarSetted = 0;
		}
	}
	pRung->Label[0] = '\0';
	pRung->Comment[0] = '\0';
	pRung->Used = FALSE;
	pRung->NbrLinesUsed = RUNG_HEIGHT;
	pRung->PrevRung = -1;
	pRung->NextRung = -1;
}
/* Set DynamicVarBak (Element) to the right value before calculating the rungs */
/* for detecting rising/falling edges used in some elements */
void PrepareRungs()
{
	int NumRung;
	int x,y;
	char StateElement;
	for (NumRung=0;NumRung<NBR_RUNGS;NumRung++)
	{
		for (y=0;y<RUNG_HEIGHT;y++)
		{
			for(x=0;x<RUNG_WIDTH;x++)
			{
				if ( (RungArray[NumRung].Element[x][y].Type==ELE_RISING_INPUT)
					|| (RungArray[NumRung].Element[x][y].Type==ELE_FALLING_INPUT) )
				{
					StateElement = ReadVarForElement( &RungArray[NumRung].Element[x][y], NULL );
					if (RungArray[NumRung].Element[x][y].Type==ELE_FALLING_INPUT)
						StateElement = !StateElement;
					RungArray[NumRung].Element[x][y].DynamicVarBak = StateElement;
				}
			}
		}
	}
}
#ifdef OLD_TIMERS_MONOS_SUPPORT
void InitTimers()
{
	int NumTimer;
	for (NumTimer=0; NumTimer<NBR_TIMERS; NumTimer++)
	{
		TimerArray[NumTimer].Preset = 0;
		TimerArray[NumTimer].Base = TIME_BASE_SECS;
		strcpy( TimerArray[NumTimer].DisplayFormat, "%.1fs" );
	}
}
void PrepareTimers()
{
	int NumTimer;
	for (NumTimer=0; NumTimer<NBR_TIMERS; NumTimer++)
	{
		TimerArray[NumTimer].Value = TimerArray[NumTimer].Preset;
		TimerArray[NumTimer].InputEnable = 0;
		TimerArray[NumTimer].InputControl = 0;
		TimerArray[NumTimer].OutputDone = 0;
		TimerArray[NumTimer].OutputRunning = 0;
	}
}
void InitMonostables()
{
	int NumMonostable;
	for (NumMonostable=0; NumMonostable<NBR_MONOSTABLES; NumMonostable++)
	{
		MonostableArray[NumMonostable].Preset = 0;
		MonostableArray[NumMonostable].Base = TIME_BASE_SECS;
		strcpy( MonostableArray[NumMonostable].DisplayFormat, "%.1fs" );
	}
}
void PrepareMonostables()
{
	int NumMonostable;
	for (NumMonostable=0; NumMonostable<NBR_MONOSTABLES; NumMonostable++)
	{
		MonostableArray[NumMonostable].Value = 0;
		MonostableArray[NumMonostable].Input = 0;
		MonostableArray[NumMonostable].OutputRunning = 0;
		MonostableArray[NumMonostable].InputBak = 0;
	}
}
#endif
void InitCounters()
{
	int NumCounter;
	for (NumCounter=0; NumCounter<NBR_COUNTERS; NumCounter++)
	{
		CounterArray[NumCounter].Preset = 0;
	}
}
void PrepareCounters()
{
	int NumCounter;
	for (NumCounter=0; NumCounter<NBR_COUNTERS; NumCounter++)
	{
		CounterArray[NumCounter].Value = 0;
		CounterArray[NumCounter].ValueBak = 0;
		CounterArray[NumCounter].InputReset = 0;
		CounterArray[NumCounter].InputPreset = 0;
		CounterArray[NumCounter].InputCountUp = 0;
		CounterArray[NumCounter].InputCountUpBak = 0;
		CounterArray[NumCounter].InputCountDown = 0;
		CounterArray[NumCounter].InputCountDownBak = 0;
		CounterArray[NumCounter].OutputDone = 0;
		CounterArray[NumCounter].OutputEmpty = 0;
		CounterArray[NumCounter].OutputFull = 0;
	}
}
void InitTimersIEC()
{
	int NumTimer;
	for (NumTimer=0; NumTimer<NBR_TIMERS_IEC; NumTimer++)
	{
		NewTimerArray[NumTimer].Preset = 0;
		NewTimerArray[NumTimer].Base = TIME_BASE_SECS;
		NewTimerArray[NumTimer].TimerMode = TIMER_IEC_MODE_ON;
		strcpy( NewTimerArray[NumTimer].DisplayFormat, "%.1fs" );
	}
}
void PrepareTimersIEC()
{
	int NumTimer;
	for (NumTimer=0; NumTimer<NBR_TIMERS_IEC; NumTimer++)
	{
		NewTimerArray[NumTimer].Value = 0;
		NewTimerArray[NumTimer].Input = 0;
		NewTimerArray[NumTimer].InputBak = 0;
		NewTimerArray[NumTimer].Output = 0;
		NewTimerArray[NumTimer].TimerStarted = 0;
		NewTimerArray[NumTimer].ValueToReachOneBaseUnit = 0;
	}
}
void InitRegisters()
{
	int NumRegister;
	for (NumRegister=0; NumRegister<NBR_REGISTERS; NumRegister++)
	{
		StrRegister * pRegister = &RegisterArray[ NumRegister ];
		pRegister->RegisterMode = REGISTER_MODE_UNDEF;
	}
}
void PrepareRegisters()
{
	int NumRegister;
	for (NumRegister=0; NumRegister<NBR_REGISTERS; NumRegister++)
	{
		StrRegister * pRegister = &RegisterArray[ NumRegister ];
		pRegister->ValueIn = 0;
		pRegister->ValueOut = 0;
		pRegister->InputReset = 0;
		pRegister->InputIn = 0;
		pRegister->InputInBak = 0;
		pRegister->InputOut = 0;
		pRegister->InputOutBak = 0;
		pRegister->OutputEmpty = 0;
		pRegister->OutputFull = 0;
		pRegister->NbrValuesSaved = 0;
		pRegister->FirstValueIndex = 0;
		pRegister->LatestValueIndex = 0;
	}
}

void PrepareAllDatasBeforeRun( )
{
#ifdef OLD_TIMERS_MONOS_SUPPORT
	PrepareTimers( );
	PrepareMonostables( );
#endif
	PrepareCounters( );
	PrepareTimersIEC( );
	PrepareRegisters( );
	PrepareRungs( );
#ifdef SEQUENTIAL_SUPPORT
	PrepareSequential( );
#endif
}

void InitArithmExpr()
{
    int NumExpr;
    for (NumExpr=0; NumExpr<NBR_ARITHM_EXPR; NumExpr++)
        strcpy(ArithmExpr[NumExpr].Expr,"");
}

int ReadVarForElement( StrElement * pElem, char * pInfoVarIsSet )
{
	int VarType = pElem->VarType;
	int VarOffset = pElem->VarNum;
	// is an indexed one ?
	if ( pElem->IndexedVarType!=-1 )
	{
		// add index value from content of the index variable
		int IndexValue = ReadVar( pElem->IndexedVarType, pElem->IndexedVarNum );
		VarOffset += IndexValue;
	}
	if ( pInfoVarIsSet && ( VarType==VAR_PHYS_INPUT || VarType==VAR_PHYS_OUTPUT ) )
		*pInfoVarIsSet = IsVarSet( VarType, VarOffset );
	return ReadVar( VarType, VarOffset );
}
void WriteVarForElement( StrElement *pElem, int Value )
{
	int VarType = pElem->VarType;
	int VarOffset = pElem->VarNum;
	// is an indexed one ?
	if ( pElem->IndexedVarType!=-1 )
	{
		// add index value from content of the index variable
		int IndexValue = ReadVar( pElem->IndexedVarType, pElem->IndexedVarNum );
		VarOffset += IndexValue;
	}
	WriteVar( VarType, VarOffset, Value );
}

char StateOnLeft(int x,int y,StrRung * TheRung)
{
    char State = 0;
    int PosY;
    char StillConnected;
    // in case of...
    if ( x<0 )
    {
		printf("Abnormal test in StateOnLeft for x=%d,y=%d\n", x, y );
		return 0;
	}
    // directly connected to the "left"? if yes, ON !
    if (x==0)
        return 1;
    /* Direct on left */
    if (TheRung->Element[x-1][y].DynamicOutput)
        State = 1;
    /* Up */
    PosY = y;
    StillConnected = TheRung->Element[x][PosY].ConnectedWithTop;
    while( (PosY>0) && StillConnected)
    {
        PosY--;
        if (TheRung->Element[x-1][PosY].DynamicOutput)
            State = 1;
        if ( !(TheRung->Element[x][PosY].ConnectedWithTop) )
            StillConnected = FALSE;
    }
    /* Down */
    if (y<RUNG_HEIGHT-1)
    {
        PosY = y+1;
        StillConnected = TheRung->Element[x][PosY].ConnectedWithTop;
        while( (PosY<RUNG_HEIGHT) && StillConnected)
        {
            if (TheRung->Element[x-1][PosY].DynamicOutput)
                State = 1;
            PosY++;
            if (PosY<RUNG_HEIGHT)
            {
                if ( !(TheRung->Element[x][PosY].ConnectedWithTop) )
                    StillConnected = FALSE;
            }
        }
    }
    return State;
}

/* Elements : -| |- and -|/|- */
char CalcTypeInput(int x,int y,StrRung * UpdateRung,char IsNot,char OnlyFronts)
{
    char State;
    char StateElement;
    char StateVar;

    StateElement = ReadVarForElement( &UpdateRung->Element[x][y], &UpdateRung->Element[x][y].DynamicVarSetted );
    if (IsNot)
        StateElement = !StateElement;
    StateVar = StateElement;
    if (OnlyFronts)
    {
        if (StateElement && UpdateRung->Element[x][y].DynamicVarBak)
            StateElement = 0;
    }
    UpdateRung->Element[x][y].DynamicState = StateElement;
    if (x==0)
    {
        State = StateElement;
    }
    else
    {
        UpdateRung->Element[x][y].DynamicInput = StateOnLeft(x,y,UpdateRung);
        State = StateElement && UpdateRung->Element[x][y].DynamicInput;
    }
    UpdateRung->Element[x][y].DynamicOutput = State;
    UpdateRung->Element[x][y].DynamicVarBak = StateVar;
    return State;
}
/* Element : --- */
char CalcTypeConnection(int x,int y,StrRung * UpdateRung)
{
    char State;
    char StateElement;
    StateElement = 1;
    if (x==0)
    {
        State = StateElement;
    }
    else
    {
        UpdateRung->Element[x][y].DynamicInput = StateOnLeft(x,y,UpdateRung);
        State = StateElement && UpdateRung->Element[x][y].DynamicInput;
    }
    UpdateRung->Element[x][y].DynamicState = State;
    UpdateRung->Element[x][y].DynamicOutput = State;
    return State;
}
/* Elements : -( )- and -(/)- */
char CalcTypeOutput(int x,int y,StrRung * UpdateRung,char IsNot)
{
    char State;
    char StateElementIfSetted;
    //v0.9.100, added just to see if this output is "setted"
	StateElementIfSetted = ReadVarForElement( &UpdateRung->Element[x][y], &UpdateRung->Element[x][y].DynamicVarSetted );
    State = StateOnLeft(x,y,UpdateRung);
    UpdateRung->Element[x][y].DynamicInput = State;
// moved below in v0.9.3 (Display bug found by Heli!)    UpdateRung->Element[x][y].DynamicState = State;
    if (IsNot)
        State = !State;
    WriteVarForElement( &UpdateRung->Element[x][y], State );
    //v0.9.100, to see "setted" state used !
    if ( UpdateRung->Element[x][y].DynamicVarSetted )
		State = StateElementIfSetted;
    UpdateRung->Element[x][y].DynamicState = State;
    return State;
}
/* Elements : -(S)- and -(R)- */
char CalcTypeOutputSetReset(int x,int y,StrRung * UpdateRung,char IsReset)
{
    char State;
    UpdateRung->Element[x][y].DynamicInput = StateOnLeft(x,y,UpdateRung);
    State = UpdateRung->Element[x][y].DynamicInput;
    UpdateRung->Element[x][y].DynamicState = State;
    if (State)
    {
        if (IsReset)
            State = 0;  /* reset */
        else
            State = 1;  /* set */
        WriteVarForElement( &UpdateRung->Element[x][y], State );
    }
    return State;
}
/* Element : -(J)- */
int CalcTypeOutputJump(int x,int y,StrRung * UpdateRung)
{
    char State;
    int Goto = -1;
    State = StateOnLeft(x,y,UpdateRung);
    if (State)
        Goto = UpdateRung->Element[x][y].VarNum;
    UpdateRung->Element[x][y].DynamicInput = State;
    UpdateRung->Element[x][y].DynamicState = State;
    return Goto;
}
/* Element : -(C)- */
int CalcTypeOutputCall(int x,int y,StrRung * UpdateRung)
{
    char State;
    int CallSrSection = -1;
    State = StateOnLeft(x,y,UpdateRung);
    if (State)
        CallSrSection = SearchSubRoutineWithItsNumber( UpdateRung->Element[x][y].VarNum );
    UpdateRung->Element[x][y].DynamicInput = State;
    UpdateRung->Element[x][y].DynamicState = State;
    return CallSrSection;
}
#ifdef OLD_TIMERS_MONOS_SUPPORT
/* Element : Timer (2x2 Blocks) */
void CalcTypeTimer(int x,int y,StrRung * UpdateRung)
{
    StrTimer * Timer;
    Timer = &TimerArray[UpdateRung->Element[x][y].VarNum];
// v0.9.2, for monitor, added refresh DynamicInput + fixed x>0 test...
    // 2 blocks width, with "alive" one is right, should never have x=0 here...
    if (x>=1)
    {
        Timer->InputEnable = UpdateRung->Element[x-1][y].DynamicInput = StateOnLeft(x-1,y,UpdateRung);
#ifdef FORCE_INP_CONTROL_OLD_TIMERS
        Timer->InputControl = UpdateRung->Element[x-1][y+1].DynamicInput = 1;
#else
        Timer->InputControl = UpdateRung->Element[x-1][y+1].DynamicInput = StateOnLeft(x-1,y+1,UpdateRung);
#endif
    }
    if (!Timer->InputEnable)
    {
        Timer->OutputRunning = 0;
        Timer->OutputDone = 0;
        Timer->Value = Timer->Preset;
    }
    else
    {
        if (Timer->Value>0)
        {
            if ( Timer->InputControl )
            {
                Timer->Value = Timer->Value - GeneralParamsMirror./*InfosGene->GeneralParams.*/PeriodMilliSecsTaskLogic;
                Timer->OutputRunning = 1;
                Timer->OutputDone = 0;
            }
        }
        else
        {
            Timer->OutputRunning = 0;
            Timer->OutputDone = 1;
        }
    }
    UpdateRung->Element[x][y].DynamicOutput = Timer->OutputDone;
    UpdateRung->Element[x][y+1].DynamicOutput = Timer->OutputRunning;
}
/* Element : Monostable (2x2 Blocks) */
void CalcTypeMonostable(int x,int y,StrRung * UpdateRung)
{
    StrMonostable * Monostable;
    Monostable = &MonostableArray[UpdateRung->Element[x][y].VarNum];
// v0.9.2, for monitor, added refresh DynamicInput + fixed x>0 test...
    // 2 blocks width, with "alive" one is right, should never have x=0 here...
    if (x>=1)
    {
        Monostable->Input = UpdateRung->Element[x-1][y].DynamicInput = StateOnLeft(x-1,y,UpdateRung);
    }
    /* detecting impulse on input, the monostable is not retriggerable */
    if (Monostable->Input && !Monostable->InputBak && (Monostable->Value==0) )
    {
        Monostable->OutputRunning = 1;
        Monostable->Value = Monostable->Preset;
    }
    if (Monostable->Value>0)
        Monostable->Value = Monostable->Value - GeneralParamsMirror./*InfosGene->GeneralParams.*/PeriodMilliSecsTaskLogic;
    else
        Monostable->OutputRunning = 0;
    Monostable->InputBak = Monostable->Input;
    UpdateRung->Element[x][y].DynamicOutput = Monostable->OutputRunning;
}
#endif
/* Element : Counter (2x4 Blocks) */
void CalcTypeCounter(int x,int y,StrRung * UpdateRung)
{
	int CounterNbr = UpdateRung->Element[x][y].VarNum;
	StrCounter * Counter = &CounterArray[ CounterNbr ];
	char DoneResult, EmptyResult, FullResult;
	int PresetValue = ReadVar( VAR_COUNTER_PRESET, CounterNbr );
	int CurrentValue = ReadVar( VAR_COUNTER_VALUE, CounterNbr );
	int ValueSave = CurrentValue; /* to detect value changed from user... */
	int CurrentValueNow;
// v0.9.2, for monitor, added refresh DynamicInput + fixed x>0 test...
    // 2 blocks width, with "alive" one is right, should never have x=0 here...
    if (x>=1)
	{
		Counter->InputReset = UpdateRung->Element[x-1][y].DynamicInput = StateOnLeft(x-1,y,UpdateRung);
		Counter->InputPreset = UpdateRung->Element[x-1][y+1].DynamicInput = StateOnLeft(x-1,y+1,UpdateRung);
		Counter->InputCountUp = UpdateRung->Element[x-1][y+2].DynamicInput = StateOnLeft(x-1,y+2,UpdateRung);
		Counter->InputCountDown = UpdateRung->Element[x-1][y+3].DynamicInput = StateOnLeft(x-1,y+3,UpdateRung);
	}
	if ( Counter->InputCountUp && Counter->InputCountUpBak==0 )
	{
		Counter->ValueBak = CurrentValue;
		CurrentValue++;
		if ( CurrentValue>9999 )
			CurrentValue = 0;
	}
	if ( Counter->InputCountDown && Counter->InputCountDownBak==0 )
	{
		Counter->ValueBak = CurrentValue;
		CurrentValue--;
		if ( CurrentValue<0 )
			CurrentValue = 9999;
	}
	if ( Counter->InputPreset )
	{
		Counter->ValueBak = CurrentValue;
		CurrentValue = PresetValue;
	}
	if ( Counter->InputReset )
	{
		Counter->ValueBak = CurrentValue;
		CurrentValue = 0;
	}
	Counter->InputCountUpBak = Counter->InputCountUp;
	Counter->InputCountDownBak = Counter->InputCountDown;

	DoneResult = ( CurrentValue==PresetValue )?1:0;
	EmptyResult = ( CurrentValue==9999 && Counter->ValueBak==0 )?1:0;
	FullResult = ( CurrentValue==0 && Counter->ValueBak==9999 )?1:0;
	UpdateRung->Element[x][y].DynamicOutput = EmptyResult;
	UpdateRung->Element[x][y + 1].DynamicOutput = DoneResult;
	UpdateRung->Element[x][y + 2].DynamicOutput = FullResult;

	// detect current value changed from user between start and here? not sure...?
	CurrentValueNow = ReadVar( VAR_COUNTER_VALUE, CounterNbr );
	if ( ValueSave!=CurrentValueNow )
		CurrentValue = CurrentValueNow;
	// now update public vars
	// (we could have directly written in the counter structure)
	// (but on another project, vars can be mapped in another way)
	WriteVar( VAR_COUNTER_VALUE, CounterNbr, CurrentValue );
//	WriteVar( VAR_COUNTER_PRESET, CounterNbr, PresetValue );
	WriteVar( VAR_COUNTER_DONE, CounterNbr, DoneResult );
	WriteVar( VAR_COUNTER_EMPTY, CounterNbr, EmptyResult );
	WriteVar( VAR_COUNTER_FULL, CounterNbr, FullResult );
}
/* Element : New IEC Timer with many modes (2x2 Blocks) */
void CalcTypeTimerIEC(int x,int y,StrRung * UpdateRung)
{
	int TimerNbr = UpdateRung->Element[x][y].VarNum;
	StrTimerIEC * TimerIEC = &NewTimerArray[ TimerNbr ];
	int CurrentValue = ReadVar( VAR_TIMER_IEC_VALUE, TimerNbr );
	int PresetValue = ReadVar( VAR_TIMER_IEC_PRESET, TimerNbr );
	char OutputResult = ReadVar( VAR_TIMER_IEC_DONE, TimerNbr );

	char DoIncTime = FALSE;
// v0.9.2, for monitor, added refresh DynamicInput + fixed x>0 test...
    // 2 blocks width, with "alive" one is right, should never have x=0 here...
    if (x>=1)
	{
		TimerIEC->Input = UpdateRung->Element[x-1][y].DynamicInput = StateOnLeft(x-1,y,UpdateRung);
	}
	switch( TimerIEC->TimerMode )
	{
		case TIMER_IEC_MODE_ON:
			if (!TimerIEC->Input)
			{
				OutputResult = 0;
				CurrentValue = 0;
			}
			else
			{
				if (CurrentValue<PresetValue)
					DoIncTime = TRUE;
				else
					OutputResult = 1;
			}
			break;
		case TIMER_IEC_MODE_OFF:
			if ( TimerIEC->Input )
			{
				OutputResult = 1;
				CurrentValue = 0;
				TimerIEC->TimerStarted = 0;
			}
			else
			{
				/* detecting falling edge on input */
				if ( !TimerIEC->Input && TimerIEC->InputBak )
					TimerIEC->TimerStarted = 1;
			}
			break;
		case TIMER_IEC_MODE_PULSE:
			/* detecting rising edge on input, the monostable is not retriggerable */
			if (TimerIEC->Input && !TimerIEC->InputBak && TimerIEC->TimerStarted==0 )
			{
				OutputResult = 1;
				CurrentValue = 0;
				TimerIEC->TimerStarted = 1;
			}
			break;
	}
	if ( TimerIEC->TimerMode==TIMER_IEC_MODE_OFF || TimerIEC->TimerMode==TIMER_IEC_MODE_PULSE )
	{
		if (TimerIEC->TimerStarted )
		{
			if (CurrentValue<PresetValue)
			{
				DoIncTime = TRUE;
			}
			else
			{
				OutputResult = 0;
				CurrentValue = 0;
				TimerIEC->TimerStarted = 0;
			}
		}
	}
	if ( DoIncTime )
	{
		TimerIEC->ValueToReachOneBaseUnit = TimerIEC->ValueToReachOneBaseUnit+GeneralParamsMirror./*InfosGene->GeneralParams.*/PeriodMilliSecsTaskLogic;
		if ( TimerIEC->ValueToReachOneBaseUnit>=TimerIEC->Base )
		{
			CurrentValue++;
			// do not lost the little too-much time part...
			TimerIEC->ValueToReachOneBaseUnit = TimerIEC->ValueToReachOneBaseUnit-TimerIEC->Base;
		}
	}
	TimerIEC->InputBak = TimerIEC->Input;
	UpdateRung->Element[x][y].DynamicOutput = OutputResult;
	// now update public vars
	// (we could have directly written in the IEC Timer structure)
	// (but on another project, vars can be mapped in another way)
	WriteVar( VAR_TIMER_IEC_DONE, TimerNbr, OutputResult );
	WriteVar( VAR_TIMER_IEC_PRESET, TimerNbr, PresetValue );
	WriteVar( VAR_TIMER_IEC_VALUE, TimerNbr, CurrentValue );
}
/* Element : Register (2x3 Blocks) */
void CalcTypeRegister(int x,int y,StrRung * UpdateRung)
{
	int RegisterNbr = UpdateRung->Element[x][y].VarNum;
	StrRegister * pRegister = &RegisterArray[ RegisterNbr ];
	char EmptyResult, FullResult;
    // 2 blocks width, with "alive" one is right, should never have x=0 here...
    if (x>=1)
	{
		pRegister->InputReset = UpdateRung->Element[x-1][y].DynamicInput = StateOnLeft(x-1,y,UpdateRung);
		pRegister->InputIn = UpdateRung->Element[x-1][y+1].DynamicInput = StateOnLeft(x-1,y+1,UpdateRung);
		pRegister->InputOut = UpdateRung->Element[x-1][y+2].DynamicInput = StateOnLeft(x-1,y+2,UpdateRung);
	}
	if ( pRegister->InputIn && pRegister->InputInBak==0 )
	{
		if ( pRegister->NbrValuesSaved<REGISTER_LIST_SIZE )
		{
			pRegister->NbrValuesSaved++;
			RegistersListsDatas[ RegisterNbr*REGISTER_LIST_SIZE+pRegister->FirstValueIndex ] = ReadVar( VAR_REGISTER_IN_VALUE, RegisterNbr );
			pRegister->FirstValueIndex++;
			if ( pRegister->FirstValueIndex>REGISTER_LIST_SIZE )
				pRegister->FirstValueIndex = 0;
		}
	}
	if ( pRegister->InputOut && pRegister->InputOutBak==0 )
	{
		if ( pRegister->NbrValuesSaved>0)
		{
			pRegister->NbrValuesSaved--;
			if ( pRegister->RegisterMode==REGISTER_MODE_FIFO )
			{
				WriteVar( VAR_REGISTER_OUT_VALUE, RegisterNbr, RegistersListsDatas[ RegisterNbr*REGISTER_LIST_SIZE+pRegister->LatestValueIndex ] );
				pRegister->LatestValueIndex++;
				if ( pRegister->LatestValueIndex>REGISTER_LIST_SIZE )
					pRegister->LatestValueIndex = 0;
			}
			else if ( pRegister->RegisterMode==REGISTER_MODE_LIFO )
			{
				pRegister->FirstValueIndex--;
				if ( pRegister->FirstValueIndex<0 )
					pRegister->FirstValueIndex = REGISTER_LIST_SIZE-1;
				WriteVar( VAR_REGISTER_OUT_VALUE, RegisterNbr, RegistersListsDatas[ RegisterNbr*REGISTER_LIST_SIZE+pRegister->FirstValueIndex ] );
			}

		}
	}
	if ( pRegister->InputReset )
	{
		pRegister->NbrValuesSaved = 0;
		pRegister->FirstValueIndex = 0;
		pRegister->LatestValueIndex = 0;
		WriteVar( VAR_REGISTER_OUT_VALUE, RegisterNbr, 0 );
	}
	pRegister->InputInBak = pRegister->InputIn;
	pRegister->InputOutBak = pRegister->InputOut;

	EmptyResult = ( pRegister->NbrValuesSaved==0 )?1:0;
	FullResult = ( pRegister->NbrValuesSaved==REGISTER_LIST_SIZE )?1:0;
	UpdateRung->Element[x][y].DynamicOutput = EmptyResult;
	UpdateRung->Element[x][y + 1].DynamicOutput = FullResult;
	// now update public vars
	WriteVar( VAR_REGISTER_EMPTY, RegisterNbr, EmptyResult );
	WriteVar( VAR_REGISTER_FULL, RegisterNbr, FullResult );
}

/* Element : Compar (3 Horizontal Blocks) */
char CalcTypeCompar(int x,int y,StrRung * UpdateRung)
{
    char State;
    char StateElement;

    StateElement = EvalCompare(ArithmExpr[UpdateRung->Element[x][y].VarNum].Expr);
    UpdateRung->Element[x][y].DynamicState = StateElement;
    if (x==2)
    {
        State = StateElement;
    }
    else
    {
        UpdateRung->Element[x-2][y].DynamicInput = StateOnLeft(x-2,y,UpdateRung);
        State = StateElement && UpdateRung->Element[x-2][y].DynamicInput;
    }
    UpdateRung->Element[x][y].DynamicOutput = State;
    return State;
}

/* Element : Operate (3 Horizontal Blocks) */
char CalcTypeOutputOperate(int x,int y,StrRung * UpdateRung)
{
    char State;
    State = StateOnLeft(x-2,y,UpdateRung);
    if (State)
        MakeCalc(ArithmExpr[UpdateRung->Element[x][y].VarNum].Expr,FALSE /* verify mode */);
    UpdateRung->Element[x][y].DynamicInput = State;
    UpdateRung->Element[x][y].DynamicState = State;
    return State;
}


int RefreshRung(StrRung * Rung, int * JumpTo)
{
	int x = 0, y = 0;
	int JumpToRung = -1;
	int SectionToCall = -1;

	do
	{
		do
		{
			switch(Rung->Element[x][y].Type)
			{
				/* MLD,16/5/2001,V0.2.8 , fixed for drawing */
				case ELE_FREE:
				case ELE_UNUSABLE:
					if (StateOnLeft(x,y,Rung))
						Rung->Element[x][y].DynamicInput = 1;
					else
						Rung->Element[x][y].DynamicInput = 0;
					break;
				/* End fix */
				case ELE_INPUT:
					CalcTypeInput(x,y,Rung,FALSE,FALSE);
					break;
				case ELE_INPUT_NOT:
					CalcTypeInput(x,y,Rung,TRUE,FALSE);
					break;
				case ELE_RISING_INPUT:
					CalcTypeInput(x,y,Rung,FALSE,TRUE);
					break;
				case ELE_FALLING_INPUT:
					CalcTypeInput(x,y,Rung,TRUE,TRUE);
					break;
				case ELE_CONNECTION:
					CalcTypeConnection(x,y,Rung);
					break;
#ifdef OLD_TIMERS_MONOS_SUPPORT
				case ELE_TIMER:
					CalcTypeTimer(x,y,Rung);
					break;
				case ELE_MONOSTABLE:
					CalcTypeMonostable(x,y,Rung);
					break;
#endif
				case ELE_COUNTER:
					CalcTypeCounter(x,y,Rung);
					break;
				case ELE_TIMER_IEC:
					CalcTypeTimerIEC(x,y,Rung);
					break;
				case ELE_REGISTER:
					CalcTypeRegister(x,y,Rung);
					break;
				case ELE_COMPAR:
					CalcTypeCompar(x,y,Rung);
					break;
				case ELE_OUTPUT:
					CalcTypeOutput(x,y,Rung,FALSE);
					break;
				case ELE_OUTPUT_NOT:
					CalcTypeOutput(x,y,Rung,TRUE);
					break;
				case ELE_OUTPUT_SET:
					CalcTypeOutputSetReset(x,y,Rung,FALSE);
					break;
				case ELE_OUTPUT_RESET:
					CalcTypeOutputSetReset(x,y,Rung,TRUE);
					break;
				case ELE_OUTPUT_JUMP:
					JumpToRung = CalcTypeOutputJump(x,y,Rung);
					// we will now abort the refresh of the rung immediately...
					break;
				case ELE_OUTPUT_CALL:
					SectionToCall = CalcTypeOutputCall(x,y,Rung);
					if ( SectionToCall!=-1 )
					{
						StrSection * pSubRoutineSection = &SectionArray[ SectionToCall ];
						if ( pSubRoutineSection->Used && pSubRoutineSection->SubRoutineNumber>=0 )
							RefreshASection( pSubRoutineSection ); //recursive call! ;-)
						else
							rt_debug_printf("Refresh rungs aborted - call to a sub-routine undefined or programmed as main !!!");
					}
					break;
				case ELE_OUTPUT_OPERATE:
					CalcTypeOutputOperate(x,y,Rung);
					break;
			}
			y++;
		}while( y<Rung->NbrLinesUsed/*RUNG_HEIGHT*/ && JumpToRung==-1 );
		y = 0;
		x++;
	}
	while( x<RUNG_WIDTH && JumpToRung==-1 );

	*JumpTo = JumpToRung;
	return TRUE;
}


// we refresh all the rungs of this section.
// we can (J)ump to another rung in this section.
// we can arrive here with a sub-routine (C)all coil (another section, recursively) !
void RefreshASection( StrSection * pSection )
{
	int Goto;
	int Done = FALSE;
	int NumRung = pSection->FirstRung;
	int MadLoopBreak = 0;
	do
	{
		RefreshRung(&RungArray[NumRung], &Goto);

		if ( Goto!=-1 )
		{
			if (!RungArray[Goto].Used)
			{
				Done = TRUE;
				rt_debug_printf("Refresh rungs aborted - jump to an undefined rung found in rung No%d...\n",Goto);
			}
			NumRung = Goto;
			MadLoopBreak++;
			if ( MadLoopBreak>99999 ) //value to set here?... or else measuring time?
			{
				Done = TRUE;
				rt_debug_printf("Refresh rungs aborted - mad loop jump detected - STOPPED...!\n");
				InfosGene->LadderState = STATE_STOP;
			}
		}
		else
		{
			if (NumRung == pSection->LastRung)
				Done = TRUE;
			else
				NumRung = RungArray[ NumRung ].NextRung;
		}
	}
	while(!Done);
}

// All the sections 'main' are refreshed in the order defined.
#define SR_STACK 25
void ClassicLadder_RefreshAllSections()
{
	int ScanMainSection;
	StrSection * pScanSection;

//TODO: times measures should be moved directly in the module task
#ifdef __RTL__
	long StartTime = gethrtime();
#endif
#if defined( RTAI ) && defined( MODULE )
	long StartTime = rt_get_cpu_time_ns();
#endif
	CycleStart();

	for ( ScanMainSection=0; ScanMainSection<NBR_SECTIONS; ScanMainSection++ )
	{

		pScanSection = &SectionArray[ ScanMainSection ];

		// current section defined and is a main-section (not a sub-routine)
		// and in Ladder language ?
		if ( pScanSection->Used && pScanSection->SubRoutineNumber==-1 && pScanSection->Language==SECTION_IN_LADDER )
		{
			RefreshASection( pScanSection );
		}

#ifdef SEQUENTIAL_SUPPORT
		// current section defined and is in sequential language
		if ( pScanSection->Used && pScanSection->Language==SECTION_IN_SEQUENTIAL )
		{
			RefreshSequentialPage( pScanSection->SequentialPage );
		}
#endif

	}// for( )

	CycleEnd();
//TODO: times measures should be moved directly in the module task
#ifdef __RTL__
	InfosGene->DurationOfLastScan = gethrtime() - StartTime;
//    rtl_printf("Time elapsed for refresh = %u nsec\n",gethrtime()-StartTime);
#endif
#if defined( RTAI ) && defined ( MODULE )
	InfosGene->DurationOfLastScan = rt_get_cpu_time_ns() - StartTime;
#endif
	if ( InfosGene->DurationOfLastScan>InfosGene->MaxScanDuration )
		InfosGene->MaxScanDuration = InfosGene->DurationOfLastScan;
}

void CopyRungToRung(StrRung * RungSrc,StrRung * RungDest)
{
    memcpy(RungDest,RungSrc,sizeof(StrRung));
}

void CopyElementToElement( StrElement * pEleSrc, StrElement *pEleDest )
{
	pEleDest->Type = pEleSrc->Type;
	pEleDest->ConnectedWithTop = pEleSrc->ConnectedWithTop;
	pEleDest->VarType = pEleSrc->VarType;
	pEleDest->VarNum = pEleSrc->VarNum;
	pEleDest->IndexedVarType = pEleSrc->IndexedVarType;
	pEleDest->IndexedVarNum = pEleSrc->IndexedVarNum;
}

char SaveRegisterFunctionBlockContent( int RegisterNbr, char * File, char Compressed )
{
	if ( RegisterNbr>=0 && RegisterNbr<NBR_REGISTERS )
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
				StrRegister * pRegister = &RegisterArray[ RegisterNbr ];
printf("SaveRegisterContent: File=%s\n", File );
			sprintf( Buff, "Register%d\n", RegisterNbr );
			if ( Compressed )
				gzputs( pLogFileGz, Buff );
			else
				fputs( Buff, pLogFile );
			
			int NbrValues = pRegister->NbrValuesSaved;
			if ( NbrValues==0 )
			{
				strcpy( Buff, "Empty!\n" );
				if ( Compressed )
					gzputs( pLogFileGz, Buff );
				else
					fputs( Buff, pLogFile );
			}
			else
			{
				char BuffTemp[ 30 ];
				int ValueIndex = 0;
				if ( pRegister->RegisterMode==REGISTER_MODE_FIFO )
					ValueIndex = pRegister->LatestValueIndex;
				else if ( pRegister->RegisterMode==REGISTER_MODE_LIFO )
					ValueIndex = pRegister->FirstValueIndex;
				while( NbrValues>0 )
				{
					int DataValue = -1;
					Buff[ 0 ] = '\0';
					if ( pRegister->RegisterMode==REGISTER_MODE_FIFO )
					{
						DataValue = RegistersListsDatas[ RegisterNbr*REGISTER_LIST_SIZE+ValueIndex ];
						ValueIndex++;
						if ( ValueIndex>REGISTER_LIST_SIZE )
							ValueIndex = 0;
					}
					else if ( pRegister->RegisterMode==REGISTER_MODE_LIFO )
					{
						ValueIndex--;
						if ( ValueIndex<0 )
							ValueIndex = REGISTER_LIST_SIZE-1;
						DataValue = RegistersListsDatas[ RegisterNbr*REGISTER_LIST_SIZE+ValueIndex ];
					}
					sprintf( BuffTemp, "%d" , DataValue );
					strcat( Buff, BuffTemp );
					strcat( Buff, "\n" );
					if ( Compressed )
						gzputs( pLogFileGz, Buff );
					else
						fputs( Buff, pLogFile );
printf("SaveRegisterContent: %s", Buff );
					NbrValues--;
				}
			}
		}
		if ( Compressed )
			gzclose( pLogFileGz );
		else
			fclose( pLogFile );
		return ( pLogFileGz!=NULL || pLogFile!=NULL );
	}
	else
	{
		return FALSE;
	}
}
