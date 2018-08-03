#include "StdAfx.h"
#include "ELEM.h"

ELEM_RES::ELEM_RES(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	name = data[1];
}

void ELEM_RES::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	  sw->WriteLine("{0} {1} {2}", Indent, Marker(), name);
}

void ELEM_RES::CompilePass1(PLC_COMPILER^ Compiler) 
{
	Compiler->AllocateVar(name, 8);
}

void ELEM_RES::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	Compiler->vmComment("");
	Compiler->vmComment(Marker());
	Compiler->vmSkipIfBitClear(StateInOut);
	Compiler->vmClear(name);
}

void ELEM_RES::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), name);

	Compiler->AddAlias("$StateInOut", StateInOut);
	Compiler->AddAlias("VAR", Compiler->VarSyntaxName(name));

	Compiler->OutputC("if ( GetBit($StateInOut) )",		"\t// if ($StateInOut)");
	Compiler->OutputC("\tVAR = 0;",						"\t// VAR = 0");
	Compiler->End(Marker(), name);
}
