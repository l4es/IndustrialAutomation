#include "StdAfx.h"
#include "ELEM.h"

/**
 *
 */
ELEM_INLINE_CODE::ELEM_INLINE_CODE(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	int i;
	int c;
	data = str->Split(' ');

	if (data[1][0] == '"') { // LDmicro2.0
		code = str->Substring(str->IndexOf('"'));
		code->Remove(code->Length, 1);
	} else { // LDmicro1.0
		i = 3; // ignore Length
		while (i <= data->GetUpperBound(0)) {  
			c = Convert::ToInt16(data[i]);
			code = code+Convert::ToChar(c);				
			i++;
		}
		i = code->LastIndexOf("\\3"); // remove all data following this
		if (i != -1) code = code->Remove(i);
	}
}

void ELEM_INLINE_CODE::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} \"{2}\"", Indent, Marker(), code);
}

void ELEM_INLINE_CODE::CompilePass1(PLC_COMPILER^ Compiler)
{
}


void ELEM_INLINE_CODE::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	Console::WriteLine("Not implemented {0}", Marker());
}

void ELEM_INLINE_CODE::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	String^ numFmt;
	Compiler->ClearAliases();
	Compiler->AddAlias("$StateInOut", StateInOut);

	if (!code->StartsWith("//")) {
		Compiler->Begin(Marker(), "");
		Compiler->OutputC(String::Format("if ( GetBit($StateInOut) ) AssignBit($StateInOut, {0});", code));
		Compiler->End(Marker(), "");
	} else
	  Compiler->OutputC(String::Format("{0}", code));

}