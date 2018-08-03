#include "StdAfx.h"
#include "ELEM.h"

ELEM_CTU::ELEM_CTU(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	name = data[1];
	max = Convert::ToInt32(data[2]);
}

void ELEM_CTU::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3}", Indent, Marker(), name, max);
}

void ELEM_CTU::CompilePass1(PLC_COMPILER^ Compiler)
{
	storeName = Compiler->AllocateBitOneShot(name);
	if ( max < 256 )
	  Compiler->AllocateVar(name, 8);
	else
	  Compiler->AllocateVar(name, 16);
	Compiler->PresetVar(name, 0, "");
}

void ELEM_CTU::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	Console::WriteLine("Not implemented {0}", Marker());
}

void ELEM_CTU::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} max = {1}", name, max));

	Compiler->AddAlias("$StateInOut",  StateInOut);
	Compiler->AddAlias("$storeName",  storeName);
	Compiler->AddAlias("CTU", Compiler->VarSyntaxName(name));
	Compiler->AddAlias("Max", String::Format("{0}", max));

	Compiler->OutputC("if ( GetBit($StateInOut) )",			"\t// check rising edge : if ($StateInOut)");
	Compiler->OutputC("\tif ( !GetBit($storeName) ) CTU++;",		"\t// if (!$storeName) CTU++");
	Compiler->OutputC("AssignBit($storeName, GetBit($StateInOut));","\t// $storeName = $StateInOut");
	Compiler->OutputC("if ( CTU <=  Max)",					"\t// if (CTU < Max)");
	Compiler->OutputC("\tSetBit($StateInOut);",						"\t// <= Max");
	Compiler->OutputC("else ");
	Compiler->OutputC("\tClearBit($StateInOut);",						"\t// > Max");

	Compiler->ClearAliases();
	Compiler->End(Marker(), name);
}

ELEM_CTD::ELEM_CTD(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	name = data[1];
	max = Convert::ToInt32(data[2]);
}

void ELEM_CTD::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3}", Indent, Marker(), name, max);
}

void ELEM_CTD::CompilePass1(PLC_COMPILER^ Compiler)
{
	storeName = Compiler->AllocateBitOneShot(name);
	if ( max > 255 )
	  Compiler->AllocateVar(name, 8);
	else
	  Compiler->AllocateVar(name, 16);
	Compiler->PresetVar(name, 0, "");
}

void ELEM_CTD::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	Console::WriteLine("Not implemented {0}", Marker());
}

void ELEM_CTD::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} max = {1}", name, max));

	Compiler->AddAlias("$StateInOut",  StateInOut);
	Compiler->AddAlias("$storeName",  storeName);
	Compiler->AddAlias("CTD", Compiler->VarSyntaxName(name));
	Compiler->AddAlias("Max", String::Format("{0}", max));

	Compiler->OutputC("if ( GetBit($StateInOut) )",			"\t// check rising edge : if ($StateInOut)");
	Compiler->OutputC("\tif ( !GetBit($storeName) ) CTD--;",		"\t// if (!$storeName) CTD--");
	Compiler->OutputC("AssignBit($storeName, GetBit($StateInOut));","\t// $storeName = $StateInOut");
	Compiler->OutputC("if ( CTD <=  Max)",					"\t// if (CTD < Max)");
	Compiler->OutputC("\tSetBit($StateInOut);",						"\t// <= Max");
	Compiler->OutputC("else ");
	Compiler->OutputC("\tClearBit($StateInOut);",						"\t// > Max");

	Compiler->ClearAliases();
	Compiler->End(Marker(), name);
}



ELEM_CTC::ELEM_CTC(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	name = data[1];
	max = Convert::ToInt32(data[2]);
}

void ELEM_CTC::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3}", Indent, Marker(), name, max);
}

void ELEM_CTC::CompilePass1(PLC_COMPILER^ Compiler)
{
	storeName = Compiler->AllocateBitOneShot(name);
	if ( max < 256 )
	  Compiler->AllocateVar(name, 8);
	else
	  Compiler->AllocateVar(name, 16);
	Compiler->PresetVar(name, 0, "");
}

void ELEM_CTC::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	Compiler->vmComment("");
	Compiler->vmComment(Marker() + " " + name);
	String^ labelEnd = Compiler->AllocateLabelOneShot("CTC_End");
	String^ storeName = Compiler->AllocateBitOneShot(name);
	Compiler->vmSkipIfBitClear(StateInOut);
	Compiler->vmGoto(labelEnd);
	Compiler->vmSkipIfBitClear(storeName);
	Compiler->vmGoto(labelEnd);
	Compiler->vmInc(name);
	Compiler->vmLoadA(name);
	Compiler->vmLoadB(max);
	Compiler->vmSub("", "up ?");
	Compiler->vmSkipIfFlagSet("GT");
	Compiler->vmClear(name, "restart from 0");
	Compiler->vmLabel(labelEnd);
	Compiler->vmGetBit(StateInOut);
	Compiler->vmPutBit(storeName);
}

void ELEM_CTC::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} max = {1}", name, max));

	Compiler->AddAlias("$StateInOut",  StateInOut);
	Compiler->AddAlias("$storeName",  storeName);
	Compiler->AddAlias("CTC", Compiler->VarSyntaxName(name));
	Compiler->AddAlias("Max", String::Format("{0}", max));

	Compiler->OutputC("if ( GetBit($StateInOut) )",			"\t// check rising edge : if ($StateInOut)");
	Compiler->OutputC("\tif ( !GetBit($storeName) )",		"\t// if (!$storeName)");
	Compiler->OutputC("\t\tif ( CTC <  Max)",				"\t// if (CTC < Max)");
	Compiler->OutputC("\t\t\tCTC++;",						"\t// CTC++");
	Compiler->OutputC("\t\telse ");
	Compiler->OutputC("\t\t\tCTC = 0;",						"\t// CTC = 0");
	Compiler->OutputC("AssignBit($storeName, GetBit($StateInOut));","\t// $storeName = $StateInOut");
	Compiler->ClearAliases();
	Compiler->End(Marker(), name);
}
