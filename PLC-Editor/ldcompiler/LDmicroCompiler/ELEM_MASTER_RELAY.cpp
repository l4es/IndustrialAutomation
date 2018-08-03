#include "StdAfx.h"
#include "ELEM.h"

ELEM_MASTER_RELAY::ELEM_MASTER_RELAY(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
}

void ELEM_MASTER_RELAY::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1}", Indent, Marker());
}

void ELEM_MASTER_RELAY::CompilePass1(PLC_COMPILER^ Compiler) 
{
}

void ELEM_MASTER_RELAY::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	Compiler->vmComment("");
	Compiler->vmComment(Marker());
	Compiler->vmGetBit(StateInOut);
	Compiler->vmSkipIfBitSet("$mcr");
	Compiler->vmSetBit("$bit");
	Compiler->vmPutBit("$mcr");
}

void ELEM_MASTER_RELAY::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), "");
            // Tricky: must set the master control relay if we reach this
            // instruction while the master control relay is cleared, because
            // otherwise there is no good way for it to ever become set
            // again.

	Compiler->AddAlias("$StateInOut", StateInOut);

	Compiler->OutputC("if ( !GetBit($mcr) )",	"\t// if (!$mcr)");
	Compiler->OutputC("\tSetBit($mcr);",			"\t// $mcr = true");
	Compiler->OutputC("else");
	Compiler->OutputC("\tAssignBit($mcr, GetBit($StateInOut));",	"\t// $mcr = $StateInOut");
	Compiler->End(Marker(), "");
}
