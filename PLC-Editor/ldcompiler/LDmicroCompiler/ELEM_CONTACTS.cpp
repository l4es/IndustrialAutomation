#include "StdAfx.h"
#include "ELEM.h"

ELEM_OPEN::ELEM_OPEN(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
}

void ELEM_OPEN::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
    sw->WriteLine("{0} {1}", Indent, Marker());
}

void ELEM_OPEN::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	Compiler->vmComment("");
	Compiler->vmComment(Marker());
	Compiler->vmClearBit(StateInOut);
}

void ELEM_OPEN::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), "");
	Compiler->AddAlias("$StateInOut", StateInOut);

	Compiler->OutputC("ClearBit($StateInOut);",		"\t// $StateInOut = false");
	Compiler->End(Marker(), "");
}

ELEM_SHORT::ELEM_SHORT(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
}

void ELEM_SHORT::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
    sw->WriteLine("{0} {1}", Indent, Marker());
}

void ELEM_SHORT::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	Compiler->vmComment("");
	Compiler->vmComment(Marker());
}

void ELEM_SHORT::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->Begin(Marker(), "");
	Compiler->OutputC("", "\t// do nothing...");
	Compiler->End(Marker(), "");
}


ELEM_CONTACTS::ELEM_CONTACTS(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	name = data[1];
	negated = (data[2]->Contains("1") || data[2]->Contains("negated"));
}

void ELEM_CONTACTS::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	if (negated)
	  sw->WriteLine("{0} {1} {2} {3}", Indent, Marker(), name, "negated");
	else
	  sw->WriteLine("{0} {1} {2} {3}", Indent, Marker(), name, "normal");
}

void ELEM_CONTACTS::CompilePass1(PLC_COMPILER^ Compiler) 
{
	Compiler->AllocateBit(name);
}

void ELEM_CONTACTS::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	Compiler->vmComment("");
	Compiler->vmComment(Marker() + " " + name);
    if(negated) 
		Compiler->vmSkipIfBitClear(name, String::Format("NC : {0}", name));
    else 
		Compiler->vmSkipIfBitSet(name, String::Format("NO : {0}", name));
	Compiler->vmClearBit(StateInOut);
}

void ELEM_CONTACTS::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), name);
	Compiler->AddAlias("$StateInOut", StateInOut);
	Compiler->AddAlias("$name", name);

	if (negated) 
		Compiler->OutputC("if ( GetBit($name) ) ClearBit($StateInOut);",	"\t// NC : if($name) $StateInOut = false");
	else 
		Compiler->OutputC("if ( !GetBit($name) ) ClearBit($StateInOut);",	"\t// NO : if(!$name) $StateInOut = false"); 
	Compiler->End(Marker(), name);
}