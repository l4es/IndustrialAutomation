#include "StdAfx.h"
#include "ELEM.h"

/**
 *
 */
ELEM_COIL::ELEM_COIL(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	name = data[1];
    negated = (data[2]->Contains("1") || (data[2]->IndexOf("negated", StringComparison::InvariantCultureIgnoreCase) != -1));
    setOnly = (data[3]->Contains("1") || (data[3]->IndexOf("setOnly", StringComparison::InvariantCultureIgnoreCase) != -1));
    resetOnly = (data[4]->Contains("1") || (data[4]->IndexOf("resetOnly", StringComparison::InvariantCultureIgnoreCase) != -1));
}

void ELEM_COIL::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3} {4} {5}", Indent, Marker(), name, negated, setOnly, resetOnly);
}

void ELEM_COIL::CompilePass1(PLC_COMPILER^ Compiler)
{
	Compiler->AllocateBit(name);
}


void ELEM_COIL::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	Compiler->vmComment("");
	Compiler->vmComment(Marker() + " " + name);
    if(negated) {
		Compiler->vmClearBit("$Bit", "set $Bit used by GetBit/PutBit");
		Compiler->vmSkipIfBitSet(StateInOut);
		Compiler->vmSetBit("$Bit");
		Compiler->vmPutBit(name);
     } else if(setOnly) {
		Compiler->vmSkipIfBitClear(StateInOut);
		Compiler->vmSetBit(name, "set only");
     } else if(resetOnly) {
		Compiler->vmSkipIfBitClear(StateInOut);
		Compiler->vmClearBit(name, "reset only");
     } else {
		 Compiler->vmGetBit(StateInOut);
		 Compiler->vmPutBit(name);
     }
}

void ELEM_COIL::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->AddAlias("$StateInOut", StateInOut);
	Compiler->AddAlias("$name", name);

	Compiler->Begin(Marker(), name);
    if(negated) {
		Compiler->OutputC("if ( GetBit($StateInOut) )",		"\t// Negated Coil : if($StateInOut)");
		Compiler->OutputC("\tClearBit($name);",				"\t// $name = false");
		Compiler->OutputC("else");
		Compiler->OutputC("\tSetBit($name);"				"\t// $name = true");
     } else if(setOnly) {
		Compiler->OutputC("if ( GetBit($StateInOut) ) SetBit($name);",	"\t// SetOnly Coil : if($StateInOut) $name = true");
     } else if(resetOnly) {
		Compiler->OutputC("if ( GetBit($StateInOut) ) ClearBit($name);","\t// ResetOnly Coil : if($StateInOut) $name = false");
     } else {
		Compiler->OutputC("AssignBit($name, GetBit($StateInOut));",		"\t// $name = $StateInOut");
     }
	Compiler->End(Marker(), name);
}