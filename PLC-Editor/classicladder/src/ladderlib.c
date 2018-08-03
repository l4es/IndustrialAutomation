/* Classic Ladder Project */
/* Copyright (C) 2001-2003 Marc Le Douarain */
/* mavati@club-internet.fr */
/* http://www.multimania.com/mavati/classicladder */
/* May 2003 */
/* Part written by Thomas Gleixner */
/* ----------------------------------- */
/* To build a library of ClassicLadder */
/* ----------------------------------- */
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
#include <string.h>
#include <stdlib.h>

#include "ladderlib.h"

#ifdef __RTL__
#include <rtl_printf.h>      // for RTLinux module
#else
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#endif

#ifdef DYNAMIC_PLCSIZE
int ladder_init(char *fname, plc_sizeinfo_s *sinfo)
{
//	plc_sizeinfo = sinfo;
	memcpy( &InfosGene->SizesInfos, &sinfo, sizeof( plc_sizeinfo_s ) );
#else
int ladder_init(char *fname)
{
#endif
	VerifyDirectorySelected(fname);

	if (!ClassicLadder_AllocAll()) {
		ClassicLadder_FreeAll();
		return -ENOMEM;
	}

        LoadProjectFiles( CurrentProjectFileName );
        InfosGene->LadderState = STATE_RUN;

    	return 0;
}

void InitVars(void)
{
	memset (VarArray, 0, SIZE_VAR_ARRAY * sizeof (TYPE_FOR_BOOL_VAR));
	memset (VarWordArray, 0, SIZE_VAR_WORD_ARRAY * sizeof (int));
    	/* to tell the GTK application to refresh the bits */
    	InfosGene->CmdRefreshVarsBits = TRUE;
}

int ReadVar(int TypeVar,int Offset)
{
	switch(TypeVar)
	{
		case VAR_MEM_BIT:
			return VarArray[Offset];
		case VAR_TIMER_DONE:
			return TimerArray[Offset].OutputDone;
		case VAR_TIMER_RUNNING:
			return TimerArray[Offset].OutputRunning;
		case VAR_MONOSTABLE_RUNNING:
			return MonostableArray[Offset].OutputRunning;
#ifdef SEQUENTIAL_SUPPORT
		case VAR_STEP_ACTIVITY:
//            return Sequential->Step[ Offset ].Activated;
			return VarArray[NBR_BITS+Offset];
#endif
		case VAR_PHYS_INPUT:
			return readplc_input (Offset);
		case VAR_PHYS_OUTPUT:
			return readplc_output (Offset);
		case VAR_MEM_WORD:
			return VarWordArray[Offset];
#ifdef SEQUENTIAL_SUPPORT
	        case VAR_STEP_TIME:
//            return Sequential->Step[ Offset ].TimeActivated/1000;
			return VarWordArray[NBR_WORDS+Offset];
#endif
        default:
            debug_printf("!!! Error : Type not found in ReadVar()\n");
    }
    return 0;
}

void WriteVar(int TypeVar,int NumVar,int Value)
{
	switch(TypeVar)
	{
		case VAR_MEM_BIT:
			VarArray[NumVar] = Value;
			break;
		case VAR_PHYS_INPUT:
			writeplc_input (NumVar, Value);
			break;
		case VAR_PHYS_OUTPUT:
			writeplc_output (NumVar, Value);
			break;
#ifdef SEQUENTIAL_SUPPORT
		case VAR_STEP_ACTIVITY:
			VarArray[NBR_BITS+NumVar] = Value;
			break;
#endif
		case VAR_MEM_WORD:
			VarWordArray[NumVar] = Value;
			break;
#ifdef SEQUENTIAL_SUPPORT
	        case VAR_STEP_TIME:
			VarWordArray[NBR_WORDS+NumVar] = Value;
			break;
#endif
		default:
			debug_printf("!!! Error : Type not found in WriteVar()\n");
			break;
	}
	switch(TypeVar)
	{
		case VAR_MEM_BIT:
		case VAR_PHYS_INPUT:
		case VAR_PHYS_OUTPUT:
#if defined( GTK_INTERFACE )
			RefreshOneBoolVar( TypeVar, NumVar, Value );
#else
			/* to tell the GTK application to refresh the bits */
			InfosGene->CmdRefreshVarsBits = TRUE;
#endif
			break;
	}
}

/* these are only useful for the MAT-connected version */
void DoneVars(void) {}
void CycleStart(void) {}
void CycleEnd(void) {}


