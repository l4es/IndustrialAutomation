#include "StdAfx.h"
#include "ELEM.h"

ELEM_MOVE::ELEM_MOVE(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	dest = data[1];
	src = data[2];
}

void ELEM_MOVE::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3}", Indent, Marker(), dest, src);
}

void ELEM_MOVE::CompilePass1(PLC_COMPILER^ Compiler)
{
	Compiler->AllocateVar(dest, 16);
	Compiler->PresetVar(dest, 0, "");
}


void ELEM_MOVE::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	String^ labelEnd = Compiler->AllocateLabelOneShot("MOVE_End");

	Compiler->vmComment("");
	Compiler->vmComment(Marker() + " " + dest + " = " + src);
    if(IsNumber(dest)) {
	   Compiler->vmError(String::Format("Move instruction: {0} not a valid destination.", dest));
	   return;
     }
	Compiler->vmSkipIfBitSet(StateInOut);
	Compiler->vmGoto(labelEnd);
	Compiler->vmLoadR(src);
	Compiler->vmStoreR(dest);
	Compiler->vmLabel(labelEnd);
}

void ELEM_MOVE::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} = {1}", dest, src));

	Compiler->AddAlias("$StateInOut",StateInOut);
	if (IsNumber(src))Compiler->AddAlias("src", src); else	Compiler->AddAlias("src", Compiler->VarSyntaxName(src));

	if(IsNumber(dest)) {
		Compiler->CompileError(String::Format("Move instruction: {0} not a valid destination.", dest));
	   return;
     }

	Compiler->AddAlias("dest", Compiler->VarSyntaxName(dest));

	Compiler->OutputC("if ( GetBit($StateInOut) ) dest = src;"); 
	Compiler->End(Marker(), "");
}

//-------------------------------------------------------------------------------------------------------------------------------------

ELEM_ADD::ELEM_ADD(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	dest = data[1];
	op1 = data[2];
	op2 = data[3];
}

void ELEM_ADD::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3} {4}", Indent, Marker(), dest, op1, op2);
}


void ELEM_ADD::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	String^ labelEnd = Compiler->AllocateLabelOneShot("ADD_End");

	Compiler->vmComment("");
	Compiler->vmComment(Marker() + String::Format("{0} = {1} + {2}", dest, op1, op2));
    if(IsNumber(dest)) {
	   Compiler->vmError(String::Format("Add instruction: {0} not a valid destination.", dest));
	   return;
     }
	Compiler->vmSkipIfBitSet(StateInOut);
	Compiler->vmGoto(labelEnd);
	Compiler->vmLoadA(op1);
	Compiler->vmLoadB(op2);
	Compiler->vmAdd(dest);
	Compiler->vmLabel(labelEnd);
}

void ELEM_ADD::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} = {1} + {2}", dest, op1, op2));

	Compiler->AddAlias("$StateInOut", StateInOut);
	if (IsNumber(op1)) Compiler->AddAlias("op1", op1); else Compiler->AddAlias("op1", Compiler->VarSyntaxName(op1));
	if (IsNumber(op2)) Compiler->AddAlias("op2", op2); else Compiler->AddAlias("op2", Compiler->VarSyntaxName(op2));

	if(IsNumber(dest)) {
		Compiler->CompileError(String::Format("Add instruction: {0} not a valid destination.", dest));
	   return;
     }

	Compiler->AddAlias("dest", Compiler->VarSyntaxName(dest));

	Compiler->OutputC("if (GetBit($StateInOut)) dest = op1 + op2;");
	Compiler->End(Marker(), "");
}

//-------------------------------------------------------------------------------------------------------------------------------------

ELEM_SUB::ELEM_SUB(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	dest = data[1];
	op1 = data[2];
	op2 = data[3];
}

void ELEM_SUB::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3} {4}", Indent, Marker(), dest, op1, op2);
}


void ELEM_SUB::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	String^ labelEnd = Compiler->AllocateLabelOneShot("SUB_End");

	Compiler->vmComment("");
	Compiler->vmComment(Marker() + String::Format("{0} = {1} - {2}", dest, op1, op2));
    if(IsNumber(dest)) {
	   Compiler->vmError(String::Format("Sub instruction: {0} not a valid destination.", dest));
	   return;
     }
	Compiler->vmSkipIfBitSet(StateInOut);
	Compiler->vmGoto(labelEnd);
	Compiler->vmLoadA(op1);
	Compiler->vmLoadB(op2);
	Compiler->vmSub(dest);
	Compiler->vmLabel(labelEnd);
}

void ELEM_SUB::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} = {1} - {2}", dest, op1, op2));

	Compiler->AddAlias("$StateInOut", StateInOut);
	if (IsNumber(op1)) Compiler->AddAlias("op1", op1); else Compiler->AddAlias("op1", Compiler->VarSyntaxName(op1));
	if (IsNumber(op2)) Compiler->AddAlias("op2", op2); else Compiler->AddAlias("op2", Compiler->VarSyntaxName(op2));

	if(IsNumber(dest)) {
		Compiler->CompileError(String::Format("Sub instruction: {0} not a valid destination.", dest));
	   return;
     }

	Compiler->AddAlias("dest", Compiler->VarSyntaxName(dest));

	Compiler->OutputC("if (GetBit($StateInOut)) dest = op1 - op2;");
	Compiler->End(Marker(), "");
}

//-------------------------------------------------------------------------------------------------------------------------------------

ELEM_MUL::ELEM_MUL(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	dest = data[1];
	op1 = data[2];
	op2 = data[3];
}

void ELEM_MUL::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3} {4}", Indent, Marker(), dest, op1, op2);
}


void ELEM_MUL::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	String^ labelEnd = Compiler->AllocateLabelOneShot("MUL_End");

	Compiler->vmComment("");
	Compiler->vmComment(Marker() + String::Format("{0} = {1} * {2}", dest, op1, op2));
    if(IsNumber(dest)) {
	   Compiler->vmError(String::Format("Mul instruction: {0} not a valid destination.", dest));
	   return;
     }
	Compiler->vmSkipIfBitSet(StateInOut);
	Compiler->vmGoto(labelEnd);
	Compiler->vmLoadA(op1);
	Compiler->vmLoadB(op2);
	Compiler->vmMul(dest);
	Compiler->vmLabel(labelEnd);
}

void ELEM_MUL::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} = {1} * {2}", dest, op1, op2));

	Compiler->AddAlias("$StateInOut", StateInOut);
	if (IsNumber(op1)) Compiler->AddAlias("op1", op1); else Compiler->AddAlias("op1", Compiler->VarSyntaxName(op1));
	if (IsNumber(op2)) Compiler->AddAlias("op2", op2); else Compiler->AddAlias("op2", Compiler->VarSyntaxName(op2));

	if(IsNumber(dest)) {
		Compiler->CompileError(String::Format("Mul instruction: {0} not a valid destination.", dest));
	   return;
     }

	Compiler->AddAlias("dest", Compiler->VarSyntaxName(dest));

	Compiler->OutputC("if (GetBit($StateInOut)) dest = op1 * op2;");
	Compiler->End(Marker(), "");
}

//-------------------------------------------------------------------------------------------------------------------------------------

ELEM_DIV::ELEM_DIV(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	dest = data[1];
	op1 = data[2];
	op2 = data[3];
}

void ELEM_DIV::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3} {4}", Indent, Marker(), dest, op1, op2);
}


void ELEM_DIV::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	String^ labelEnd = Compiler->AllocateLabelOneShot("DIV_End");

	Compiler->vmComment("");
	Compiler->vmComment(Marker() + String::Format("{0} = {1} / {2}", dest, op1, op2));
    if(IsNumber(dest)) {
	   Compiler->vmError(String::Format("Div instruction: {0} not a valid destination.", dest));
	   return;
     }
	Compiler->vmSkipIfBitSet(StateInOut);
	Compiler->vmGoto(labelEnd);
	Compiler->vmLoadA(op1);
	Compiler->vmLoadB(op2);
	Compiler->vmDiv(dest);
	Compiler->vmLabel(labelEnd);
}

void ELEM_DIV::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} = {1} / {2}", dest, op1, op2));

	Compiler->AddAlias("$StateInOut", StateInOut);
	if (IsNumber(op1)) Compiler->AddAlias("op1", op1); else Compiler->AddAlias("op1", Compiler->VarSyntaxName(op1));
	if (IsNumber(op2)) Compiler->AddAlias("op2", op2); else Compiler->AddAlias("op2", Compiler->VarSyntaxName(op2));

	if(IsNumber(dest)) {
		Compiler->CompileError(String::Format("Div instruction: {0} not a valid destination.", dest));
	   return;
     }

	Compiler->AddAlias("dest", Compiler->VarSyntaxName(dest));

	Compiler->OutputC("if (GetBit($StateInOut)) dest = op1 / op2;");
	Compiler->End(Marker(), "");
}

//-------------------------------------------------------------------------------------------------------------------------------------

ELEM_EQU::ELEM_EQU(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	op1 = data[1];
	op2 = data[2];
}

void ELEM_EQU::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3}", Indent, Marker(), op1, op2);
}


void ELEM_EQU::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	String^ labelEnd = Compiler->AllocateLabelOneShot("EQ_End");

	Compiler->vmComment("");
	Compiler->vmComment(Marker() + " " + op1 + " == " + op2);
	Compiler->vmSkipIfBitSet(StateInOut);
	Compiler->vmGoto(labelEnd);
	Compiler->vmLoadA(op1);
	Compiler->vmLoadB(op2);
	Compiler->vmSub("");
	Compiler->vmSkipIfFlagSet("EQ");
	Compiler->vmClear(StateInOut);
	Compiler->vmLabel(labelEnd);
}

void ELEM_EQU::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} == {1}", op1, op2));

	Compiler->AddAlias("$StateInOut", StateInOut);
	if (IsNumber(op1)) Compiler->AddAlias("op1", op1); else Compiler->AddAlias("op1", Compiler->VarSyntaxName(op1));
	if (IsNumber(op2)) Compiler->AddAlias("op2", op2); else Compiler->AddAlias("op2", Compiler->VarSyntaxName(op2));

	Compiler->OutputC("if (GetBit($StateInOut) && !( op1 == op2)) ClearBit($StateInOut);");
	Compiler->End(Marker(), "");
}

//-------------------------------------------------------------------------------------------------------------------------------------

ELEM_GRT::ELEM_GRT(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	op1 = data[1];
	op2 = data[2];
}

void ELEM_GRT::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3}", Indent, Marker(), op1, op2);
}


void ELEM_GRT::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	String^ labelEnd = Compiler->AllocateLabelOneShot("GR_End");

	Compiler->vmComment("");
	Compiler->vmComment(Marker() + " " + op1 + " > " + op2);
	Compiler->vmSkipIfBitSet(StateInOut);
	Compiler->vmGoto(labelEnd);
	Compiler->vmLoadA(op2);
	Compiler->vmLoadB(op1);
	Compiler->vmSub("", "inverted : op2-op1");
	Compiler->vmSkipIfFlagSet("LT", "inverted");
	Compiler->vmClear(StateInOut);
	Compiler->vmLabel(labelEnd);
}

void ELEM_GRT::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} > {1}", op1, op2));

	Compiler->AddAlias("$StateInOut", StateInOut);
	if (IsNumber(op1)) Compiler->AddAlias("op1", op1); else Compiler->AddAlias("op1", Compiler->VarSyntaxName(op1));
	if (IsNumber(op2)) Compiler->AddAlias("op2", op2); else Compiler->AddAlias("op2", Compiler->VarSyntaxName(op2));

	Compiler->OutputC("if (GetBit($StateInOut) && !( op1 > op2)) ClearBit($StateInOut);");
	Compiler->End(Marker(), "");
}

//-------------------------------------------------------------------------------------------------------------------------------------

ELEM_GEQ::ELEM_GEQ(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	op1 = data[1];
	op2 = data[2];
}

void ELEM_GEQ::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3}", Indent, Marker(), op1, op2);
}


void ELEM_GEQ::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	String^ labelEnd = Compiler->AllocateLabelOneShot("GE_End");

	Compiler->vmComment("");
	Compiler->vmComment(Marker() + " " + op1 + " >= " + op2);
	Compiler->vmSkipIfBitSet(StateInOut);
	Compiler->vmGoto(labelEnd);
	Compiler->vmLoadA(op1);
	Compiler->vmLoadB(op2);
	Compiler->vmSub("");
	Compiler->vmSkipIfFlagSet("GE");
	Compiler->vmClear(StateInOut);
	Compiler->vmLabel(labelEnd);
}

void ELEM_GEQ::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} >= {1}", op1, op2));

	Compiler->AddAlias("$StateInOut", StateInOut);
	if (IsNumber(op1)) Compiler->AddAlias("op1", op1); else Compiler->AddAlias("op1", Compiler->VarSyntaxName(op1));
	if (IsNumber(op2)) Compiler->AddAlias("op2", op2); else Compiler->AddAlias("op2", Compiler->VarSyntaxName(op2));

	Compiler->OutputC("if (GetBit($StateInOut) && !( op1 >= op2)) ClearBit($StateInOut);");
	Compiler->End(Marker(), "");
}

//-------------------------------------------------------------------------------------------------------------------------------------

ELEM_LES::ELEM_LES(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	op1 = data[1];
	op2 = data[2];
}

void ELEM_LES::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3}", Indent, Marker(), op1, op2);
}


void ELEM_LES::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	String^ labelEnd = Compiler->AllocateLabelOneShot("LT_End");

	Compiler->vmComment("");
	Compiler->vmComment(Marker() + " " + op1 + " < " + op2);
	Compiler->vmSkipIfBitSet(StateInOut);
	Compiler->vmGoto(labelEnd);
	Compiler->vmLoadA(op1);
	Compiler->vmLoadB(op2);
	Compiler->vmSub("");
	Compiler->vmSkipIfFlagSet("LT");
	Compiler->vmClear(StateInOut);
	Compiler->vmLabel(labelEnd);
}

void ELEM_LES::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} < {1}", op1, op2));

	Compiler->AddAlias("$StateInOut", StateInOut);
	if (IsNumber(op1)) Compiler->AddAlias("op1", op1); else Compiler->AddAlias("op1", Compiler->VarSyntaxName(op1));
	if (IsNumber(op2)) Compiler->AddAlias("op2", op2); else Compiler->AddAlias("op2", Compiler->VarSyntaxName(op2));

	Compiler->OutputC("if (GetBit($StateInOut) && !( op1 < op2)) ClearBit($StateInOut);");
	Compiler->End(Marker(), "");
}

//-------------------------------------------------------------------------------------------------------------------------------------

ELEM_LEQ::ELEM_LEQ(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	op1 = data[1];
	op2 = data[2];
}

void ELEM_LEQ::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3}", Indent, Marker(), op1, op2);
}


void ELEM_LEQ::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	String^ labelEnd = Compiler->AllocateLabelOneShot("LE_End");

	Compiler->vmComment("");
	Compiler->vmComment(Marker() + " " + op1 + " <= " + op2);
	Compiler->vmSkipIfBitSet(StateInOut);
	Compiler->vmGoto(labelEnd);
	Compiler->vmLoadA(op1);
	Compiler->vmLoadB(op2);
	Compiler->vmSub("", "inverted : op2-op1");
	Compiler->vmSkipIfFlagSet("GE", "inverted");
	Compiler->vmClear(StateInOut);
	Compiler->vmLabel(labelEnd);
}

void ELEM_LEQ::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} <= {1}", op1, op2));

	Compiler->AddAlias("$StateInOut", StateInOut);
	if (IsNumber(op1)) Compiler->AddAlias("op1", op1); else Compiler->AddAlias("op1", Compiler->VarSyntaxName(op1));
	if (IsNumber(op2)) Compiler->AddAlias("op2", op2); else Compiler->AddAlias("op2", Compiler->VarSyntaxName(op2));

	Compiler->OutputC("if (GetBit($StateInOut) && !( op1 <= op2)) ClearBit($StateInOut);");
	Compiler->End(Marker(), "");
}

//-------------------------------------------------------------------------------------------------------------------------------------

ELEM_NEQ::ELEM_NEQ(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	op1 = data[1];
	op2 = data[2];
}

void ELEM_NEQ::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3}", Indent, Marker(), op1, op2);
}


void ELEM_NEQ::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	String^ labelEnd = Compiler->AllocateLabelOneShot("NE_End");

	Compiler->vmComment("");
	Compiler->vmComment(Marker() + " " + op1 + " != " + op2);
	Compiler->vmSkipIfBitSet(StateInOut);
	Compiler->vmGoto(labelEnd);
	Compiler->vmLoadA(op1);
	Compiler->vmLoadB(op2);
	Compiler->vmSub("");
	Compiler->vmSkipIfFlagSet("NE");
	Compiler->vmClear(StateInOut);
	Compiler->vmLabel(labelEnd);
}

void ELEM_NEQ::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} != {1}", op1, op2));

	Compiler->AddAlias("$StateInOut", StateInOut);
	if (IsNumber(op1)) Compiler->AddAlias("op1", op1); else Compiler->AddAlias("op1", Compiler->VarSyntaxName(op1));
	if (IsNumber(op2)) Compiler->AddAlias("op2", op2); else Compiler->AddAlias("op2", Compiler->VarSyntaxName(op2));

	Compiler->OutputC("if (GetBit($StateInOut) && !( op1 != op2)) ClearBit($StateInOut);");
	Compiler->End(Marker(), "");
}
