/*
 * MatPLC project - classicladder linkage
 *
 * based on vars_access.c from 
 *   Classic Ladder Project
 *   Copyright (C) 2001 Marc Le Douarain 
 *   mavati@club-internet.fr
 *   http:  www.multimania.com mavati classicladder 
 *   February 2001
 *
 *
 * changes Copyright (C) 2001 Jiri Baum
 */

/* ---------------------------------------------------- */
/* Access a MatPLC variable for reading / writing */
/* ---------------------------------------------------- */

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

#include <gtk/gtk.h>
#include <stdio.h>
#include "classicladder.h"
#include "global.h"

#include "../../lib/plc.h"

void InitVars(int argc,char *argv[])
{
    int NumVar;
    char name[10]; /* will hold "B1" through to "B500" */

    plc_init("classicladder", argc, argv);

    for (NumVar=0; NumVar<NBR_BITS; NumVar++) {
        sprintf(name, "B%d",NumVar);
	VarArray[NumVar] = plc_pt_by_name(name);
	if (!VarArray[NumVar].valid) {
	    VarArray[NumVar] = plc_pt_null();
	} else {
	    printf("got point %s %s\n", name, plc_pt_rw_s(VarArray[NumVar]));
	}
    }
}

char ReadVar(int TypeVar,int Offset)
{
    switch(TypeVar)
    {
        case VAR_MEM_BIT:
            return plc_get(VarArray[Offset]);
        case VAR_TIMER_DONE:
            return TimerArray[Offset].OutputDone;
        case VAR_TIMER_RUNNING:
            return TimerArray[Offset].OutputRunning;
        case VAR_MONOSTABLE_RUNNING:
            return MonostableArray[Offset].OutputRunning;
        default:
            printf("!!! Error : Type not found in ReadVar()\n");
    }
    return 0;
}

void WriteVar(int TypeVar,int NumVar,char Value)
{
    switch(TypeVar)
    {
        case VAR_MEM_BIT:
            plc_set(VarArray[NumVar], Value);
            break;
        default:
            printf("!!! Error : Type not found in WriteVar()\n");
            break;
    }
	switch(TypeVar)
	{
		case VAR_MEM_BIT:
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
void DoneVars(void) {
    plc_done();
}
void CycleStart(void) {
    plc_scan_beg();
    plc_update();
}
void CycleEnd(void) {
    plc_update();
    plc_scan_end();
}

