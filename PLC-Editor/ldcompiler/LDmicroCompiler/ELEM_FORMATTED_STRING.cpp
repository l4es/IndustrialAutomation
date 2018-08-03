#include "StdAfx.h"
#include "ELEM.h"

/**
 *
 */
ELEM_FORMATTED_STRING::ELEM_FORMATTED_STRING(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	int i;
	int c;
	bool backslash;
	data = str->Split(' ');
	name = data[1];
	digits = 0;
	minus = false;

	if (data[2][0] == '"') { // LDmicro2.0
		format = str->Substring(str->IndexOf('"'));
		format->Remove(format->Length, 1);
	} else { // LDmicro1.0
		format = "";
		backslash = false;
		i = 3; // Ignore Length
		while (i <= data->GetUpperBound(0)) {
			c = Convert::ToInt16(data[i]);
			if ((c < 32) || (c >= 127) || (c == '"')) 
				format = format+String::Format("\\x{0}", c.ToString("x2"));		// we have control or special char, convert to hex format
			else {
				if (backslash) {
					if (c == '-')
						minus = true;			// signed numeric format
					else  {
						if ((c >= '0') && (c <= '9')) {
							digits = (c - '0');			// number of digits without sign
							format = format + "{0}";	// the numeric format with be inserted here
						} else
							format = format+"\\"+Convert::ToChar(c);
						backslash = false;				// terminate escape sequence decoding
					}
				}
				else
					if (c == '\\')
						backslash = true;
					else
						format = format+Convert::ToChar(c);				
			}
			i++;
		}
	}
}

void ELEM_FORMATTED_STRING::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3} {4} \"{5}\"", Indent, Marker(), name, digits, minus, format);
}

void ELEM_FORMATTED_STRING::CompilePass1(PLC_COMPILER^ Compiler)
{
//	Compiler->AllocateBit(name);
    Compiler->AllocateVar(name, 16);
}


void ELEM_FORMATTED_STRING::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	Console::WriteLine("Not implemented {0}", Marker());
}

void ELEM_FORMATTED_STRING::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	String^ numFmt;
	Compiler->ClearAliases();
	Compiler->AddAlias("$StateInOut", StateInOut);
	Compiler->AddAlias("$name", name);

	// Build microchip C18 numeric format
	numFmt = "%";
	if (minus) 
		numFmt = String::Format("%+{0}i", digits);
	else
		numFmt = String::Format("% {0}i", digits);

	Compiler->Begin(Marker(), name);

	if (format->Contains("{0}"))
		Compiler->OutputC(String::Format("fprintf(_H_USART, \"{0}\", {1});", String::Format(format, numFmt), name));
	else
		Compiler->OutputC(String::Format("fprintf(_H_USART, \"{0}\", {1});", format, name));

	Compiler->End(Marker(), name);
}