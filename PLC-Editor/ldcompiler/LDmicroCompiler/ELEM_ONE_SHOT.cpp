#include "StdAfx.h"
#include "ELEM.h"

ELEM_ONE_SHOT_RISING::ELEM_ONE_SHOT_RISING(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
}

void ELEM_ONE_SHOT_RISING::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2}", Indent, Marker(), name);
}

void ELEM_ONE_SHOT_RISING::CompilePass1(PLC_COMPILER^ Compiler)
{
	name = Compiler->AllocateBitOneShot("$osr");
}


void ELEM_ONE_SHOT_RISING::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	String^ storeName = Compiler->AllocateBitOneShot("StoreOSR");
	Compiler->vmComment("");
	Compiler->vmComment(Marker()+ " "+storeName);
	Compiler->vmGetBit(StateInOut);
	Compiler->vmSkipIfBitClear(storeName);
	Compiler->vmClearBit(StateInOut);
	Compiler->vmPutBit(storeName);
}

void ELEM_ONE_SHOT_RISING::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), name);

	Compiler->AddAlias("$StateInOut", StateInOut);
	Compiler->AddAlias("$OSR", name);

	Compiler->OutputC("AssignBit($scratch, GetBit($StateInOut));",		"\t// $scratch = $StateInOut");
	Compiler->OutputC("if ( GetBit($OSR) ) ClearBit($StateInOut);",		"\t// if ($scratch) $StateInOut = false");
	Compiler->OutputC("AssignBit($OSR, GetBit($scratch));",				"\t// $OSR = $scratch");
	Compiler->End(Marker(), name);
}


ELEM_ONE_SHOT_FALLING::ELEM_ONE_SHOT_FALLING(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
}

void ELEM_ONE_SHOT_FALLING::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2}", Indent, Marker(), name);
}

void ELEM_ONE_SHOT_FALLING::CompilePass1(PLC_COMPILER^ Compiler)
{
	name = Compiler->AllocateBitOneShot("$osf");
}


void ELEM_ONE_SHOT_FALLING::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	String^ storeName = Compiler->AllocateBitOneShot("StoreOSF");
	String^ labelIsSet = Compiler->AllocateLabelOneShot("OSF_Lbl");
	String^ labelEnd = Compiler->AllocateLabelOneShot("OSF_End");
	Compiler->vmComment("");
	Compiler->vmComment(Marker()+" "+storeName);
	Compiler->vmGetBit(StateInOut);
	Compiler->vmSkipIfBitClear(StateInOut);
	Compiler->vmGoto(labelIsSet);
	Compiler->vmSkipIfBitClear(storeName, "line is clear");
	Compiler->vmSetBit(StateInOut);
	Compiler->vmGoto(labelEnd);
	Compiler->vmLabel(labelIsSet);
	Compiler->vmClearBit(StateInOut, "line is set");
	Compiler->vmLabel(labelEnd);
	Compiler->vmPutBit(storeName);
}

void ELEM_ONE_SHOT_FALLING::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), name);

	Compiler->AddAlias("$StateInOut", StateInOut);
	Compiler->AddAlias("$OSF", name);

	Compiler->OutputC("AssignBit($scratch, GetBit($StateInOut));",		"\t// $scratch = $StateInOut");
	Compiler->OutputC("if ( !GetBit($StateInOut) ) {",					"\t// if (!$StateInOut)");
	Compiler->OutputC("\tif ( GetBit($OSF) ) SetBit($StateInOut);",		"\t// if($OSF) $StateInOut = true");
	Compiler->OutputC("\t} else");
	Compiler->OutputC("ClearBit($StateInOut);",							"\t// $StateInOut = false");
	Compiler->OutputC("AssignBit($OSF, GetBit($scratch));",				"\t// $OSF = $scratch");
	Compiler->End(Marker(), name);
}
