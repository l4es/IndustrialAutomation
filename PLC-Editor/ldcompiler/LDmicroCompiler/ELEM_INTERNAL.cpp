#include "StdAfx.h"
#include "ELEM.h"

//--------------------------------------------------------------------------------------------------
// ELEM_INTERNAL is the ancestor of all ELEM_INTERNAL_xxx that don't appear in .LD file
//--------------------------------------------------------------------------------------------------

ELEM_INTERNAL::ELEM_INTERNAL(PLC^ APlc, ELEM^ AParent) : ELEM(APlc, AParent)
{
}

void ELEM_INTERNAL::WriteToTextStream(StreamWriter^ sw, String^ Indent) 
{
	// Do nothing, Internals dosn't appear in .ld files
}

void ELEM_INTERNAL::LoadFromTextStream(StreamReader^ sr, int &LineCounter)
{
	Console::WriteLine("Illegal read of Internal ELEM {0}", Marker());
}

//--------------------------------------------------------------------------------------------------
// ELEM_INTERNAL_RESET Reset the PLC, normaly in RUNG 0
//--------------------------------------------------------------------------------------------------

ELEM_INTERNAL_RESET::ELEM_INTERNAL_RESET(PLC^ APlc, ELEM^ AParent) : ELEM_INTERNAL(APlc, AParent)
{
}

void ELEM_INTERNAL_RESET::CompilePass1(PLC_COMPILER^ Compiler)
{
}

void ELEM_INTERNAL_RESET::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) 
{
	int i;
	Compiler->ClearAliases();

	Compiler->Begin(Marker(), "");
	Compiler->OutputC("/**");
	Compiler->OutputC(" * Reset the PLC"); 
	Compiler->OutputC(" *   - set internal variable to inital value");
	Compiler->OutputC(" *   - set plcTick to current value (used by timers)");
	Compiler->OutputC(" *   - clear all bits");
	Compiler->OutputC(" *   - set MCR");
	Compiler->OutputC(" */");

	for each(PLC_VAR_SYMBOL^ item in Compiler->Vars) {
		if ((item->AddrsSpace == PLC_VAR_INTERNAL) && (item->varSize > 0) && (item->varInit)) {
			Compiler->OutputC(String::Format("{0} = {1};", Compiler->VarSyntaxName(item->SymbolName), item->varInitialValue), item->InitComment);
		} // if
	} // for

	Compiler->OutputC("// Comment following define to save space if array is greater than 32 bytes");
	Compiler->OutputC("#define OPTIMIZED_CLEAR_PLCBITS");
	Compiler->OutputC("#ifdef OPTIMIZED_CLEAR_PLCBITS");	
	for (i=0; i <= Compiler->BitAllocationCounter / 8; i++) {
		Compiler->OutputC(String::Format("plcBits[{0}].val = 0;", i)); 
	}
	Compiler->OutputC("#else");
	Compiler->OutputC(String::Format("for (i=0; i <= {0}; i++) plcBits[i].val = 0;", Compiler->BitAllocationCounter / 8)); 
	Compiler->OutputC("#endif");
	Compiler->OutputC("");

	Compiler->OutputC("// Following will preset timer variables");
	Compiler->OutputC("plcTimerTicksOccured();",	"\t// to ovoid Tmr_Ovrf error");
	Compiler->OutputC("plcTimerTicks = 0;",		"\t// initial value for first cycle");

	Compiler->OutputC("");
	Compiler->OutputC("plcSysBits.val = 0;", "\t// Clear all flags");
	Compiler->OutputC("SetBit($mcr);", "\t// Keep PLC running");

	Compiler->End(Marker(), "");
}

void ELEM_INTERNAL_RESET::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) 
{
	Console::WriteLine("Not implemented {0}", Marker());
}

//--------------------------------------------------------------------------------------------------
// ELEM_INTERNAL_UPDATE_TIMERTICKS update plcTimerTicks and return to RUNG 1, normaly in last RUNG
//--------------------------------------------------------------------------------------------------

ELEM_INTERNAL_UPDATE_TIMERTICKS::ELEM_INTERNAL_UPDATE_TIMERTICKS(PLC^ APlc, ELEM^ AParent) : ELEM_INTERNAL(APlc, AParent)
{
}

void ELEM_INTERNAL_UPDATE_TIMERTICKS::CompilePass1(PLC_COMPILER^ Compiler)
{
}


void ELEM_INTERNAL_UPDATE_TIMERTICKS::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) 
{
	Compiler->Begin(Marker(), "");
	Compiler->OutputC("/**");
	Compiler->OutputC(" * Update plcTimerTicks"); 
	Compiler->OutputC(" */");

	Compiler->OutputC("plcTimerTicksOccured();  // Update plcTimerTicks");
	Compiler->OutputC("if (( plcTimerTicks >= 0 ) && GetBit($mcr) ) plcTaskRung = 1;", "\t// execute rungs only if a tick occured ( see plcTimerTicksOccured() )");
	Compiler->OutputC("return;", "\t//!> !!! ALWAYS EXIT FROM plcTask()");
	Compiler->End(Marker(), "");
}

void ELEM_INTERNAL_UPDATE_TIMERTICKS::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) 
{
	Console::WriteLine("Not implemented {0}", Marker());
}


//--------------------------------------------------------------------------------------------------
// ELEM_INTERNAL_UART special elem for UART
//--------------------------------------------------------------------------------------------------

ELEM_INTERNAL_UART::ELEM_INTERNAL_UART(PLC^ APlc, ELEM^ AParent) : ELEM_INTERNAL(APlc, AParent)
{
}

void ELEM_INTERNAL_UART::CompilePass1(PLC_COMPILER^ Compiler)
{
}


void ELEM_INTERNAL_UART::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) 
{
	Compiler->Begin(Marker(), "");
	Compiler->OutputC("/**");
	Compiler->OutputC(" * UART"); 
	Compiler->OutputC(" */");

	Compiler->OutputC("// Not implemented");

	Compiler->End(Marker(), "");
}

void ELEM_INTERNAL_UART::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) 
{
	Console::WriteLine("Not implemented {0}", Marker());
}
