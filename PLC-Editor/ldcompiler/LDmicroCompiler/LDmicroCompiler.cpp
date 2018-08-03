// LDmicroCompiler.cpp : fichier projet principal.

#include "stdafx.h"
#include "ELEM.h"

using namespace System;
using namespace System::IO;

/**
 * Output format
 */

/**
 * @{
 */
#define COMPILE_NONE		0
#define COMPILE_C_OUTPUT	1
#define COMPILE_VM_OUTPUT	2
#define COMPILE_CALM_OUTPUT 3
#define COMPILE_CONVERT		9
/** @} */

/**
 * return true if arg begins with '-' or '/' and is starting part of opt
 */
bool IsOptionArg(System::String^ arg)
{
	return ((arg[0] == '-') || (arg[0] == '/'));
}

/**
 * Determine if arg is an option qualifier
 * and is starting uppercase part of opt
 */
bool MatchOptionArg(System::String^ opt, System::String^ arg)
{
	int MinLength;
	MinLength = 0;
	// Count Uppercase chars of opt;
	while ((MinLength < opt->Length) && (!Char::IsLetter(opt[MinLength]) || Char::IsUpper(opt[MinLength]))) MinLength++;

	if ((arg->Length < 2) || !IsOptionArg(arg)) return false;
	if (arg->Substring(1)->Length < MinLength) return false; // not enought chars to compare....

	return opt->StartsWith(arg->Substring(1), StringComparison::InvariantCultureIgnoreCase);
}

/**
 * Find if command line has this option
 */
bool HasOptionInArgs(System::String^ opt, array<System::String ^> ^args)
{
	int i;
	i = 0;
	while (i < args->Length) {
		if (MatchOptionArg(opt, args[i])) return true;
		i++;
	}
	return false;
}

/**
 * Display help at command prompt
 */
void DisplayHelp(void)
{
	array<String^>^ args = Environment::GetCommandLineArgs();

	Console::WriteLine("\nLDmicro Compiler usage : {0} <inputfile> [<outputfile>] [/options]\n\n", Path::GetFileNameWithoutExtension(args[0]));
	Console::WriteLine("\n!! All output files will be overwritted without warning !!");
	Console::WriteLine("\nInput file format accepted :");
	Console::WriteLine("   .ld2 new file format");
	Console::WriteLine("   .ld LDmicro file format (see http://cq.cx/ladder.pl)");

	Console::WriteLine("\nOptions can begin with '-' or '/' :");
	Console::WriteLine("   /? or /help  : display this help screen");
	Console::WriteLine("   /f:c         : output C language skeleton");
	Console::WriteLine("   /f:vm        : output vmMachine code");
	Console::WriteLine("   /f:calm      : output CALM assembly source");
	Console::WriteLine("   /f:ld2       : convert .ld file format to .ld2 format");

	Console::WriteLine("\nOutput file format for /f:c output");
	Console::WriteLine("   <outputfile>.c  c code file is created");

	Console::WriteLine("\nExperimental version of LDmicroCompiler");
	Console::WriteLine("Feel free to join us and contribute to develop at");
	Console::WriteLine("http://sourceforge.net/projects/ldcompiler/");
}

void WaitKey(void)
{
//	Console::WriteLine("... press any key");
// 	Console::ReadKey();
}


void Compile2VM(String^ InputFileName, String^ OutputFileName, array<System::String ^> ^args)
{
	PLC^ plc =  gcnew PLC;
	plc->LoadFromFile(InputFileName);

	TextWriter^ swMain = File::CreateText(OutputFileName);
	PLC_COMPILER_VM^ Compiler = gcnew PLC_COMPILER_VM(swMain);

	Compiler->Reset();
	plc->CompilePass1VM(Compiler);
	Compiler->DumpVarsTable(swMain);
	Compiler->DumpBitsTable(swMain);
	Compiler->DumpIOTable(swMain);
	plc->CompilePass2VM(Compiler, "$mcr");

	swMain->Close();
	plc->~PLC();
}

void Compile2CALM(String^ InputFileName, String^ OutputFileName, array<System::String ^> ^args)
{
	PLC^ plc =  gcnew PLC;
	plc->LoadFromFile(InputFileName);

	TextWriter^ swMain = File::CreateText(OutputFileName);
	PLC_COMPILER_CALM^ Compiler = gcnew PLC_COMPILER_CALM(swMain);

	Compiler->Reset();
	plc->CompilePass1VM(Compiler);
	Compiler->CalmHeader(swMain);
	Compiler->DumpVarsTable(swMain);
	Compiler->DumpBitsTable(swMain);
	Compiler->CalmBeforeCompileCode(swMain);

	plc->CompilePass2VM(Compiler, "$mcr");

	Compiler->CalmAfterCompileCode(swMain);
	swMain->Close();
	plc->~PLC();
}

void Compile2C(String^ InputFileName, String^ OutputFileName, array<System::String ^> ^args)
{
	PLC^ plc =  gcnew PLC;
	plc->LoadFromFile(InputFileName);

	TextWriter^ swC = File::CreateText(OutputFileName);
	swC->WriteLine("#include \"ladder.h\"");
	swC->WriteLine("//#include \"{0}.h\"", Path::GetFileNameWithoutExtension(InputFileName));
	swC->WriteLine("");
	PLC_COMPILER_C^ Compiler = gcnew PLC_COMPILER_C(swC);
	Compiler->Reset();
	plc->CompilePass1C(Compiler);
	plc->CompilePass2C(Compiler, "$mcr");
	swC->Close();
	plc->~PLC();
}

void ConvertLD2(String^ InputFileName, String^ OutputFileName, array<System::String ^> ^args)
{
	PLC^ plc =  gcnew PLC;
	plc->LoadFromFile(InputFileName);
	Console::WriteLine("Convert to .ld2 file format : {0}", OutputFileName);
	plc->SaveToFile(OutputFileName);
	plc->~PLC();
}


/**
 * Main entry point
 */
int main(array<System::String ^> ^args)
{
	String ^InputFileName;
	String ^OutputFileName;
	int OutputFormat;

	// Needs to display help ?
	if ((args->Length < 2) || MatchOptionArg("Help", args[0]) || MatchOptionArg("?", args[0])) {
		DisplayHelp();
		WaitKey();
		return -1;
	}

	// Check for valid input file
	if (IsOptionArg(args[0])) {
		Console::WriteLine("Error : expected <inputfilename> instead option {0}", args[0]);
		WaitKey();
		return -1;
	}

	InputFileName = Path::GetFullPath(args[0]);
	if (!File::Exists(InputFileName)) {
		Console::WriteLine("Error : {0} file not exists", InputFileName);
		WaitKey();
		return -1;
	}
	
	// find output format
	OutputFormat = COMPILE_NONE;
	if (HasOptionInArgs("F:C", args)) OutputFormat = COMPILE_C_OUTPUT;
	else if (HasOptionInArgs("F:VM", args)) OutputFormat = COMPILE_VM_OUTPUT;
	else if (HasOptionInArgs("F:CALM", args)) OutputFormat = COMPILE_CALM_OUTPUT;
	else if (HasOptionInArgs("F:LD2", args)) OutputFormat = COMPILE_CONVERT;
	else {
		Console::WriteLine("Error : no output file format /f:xxx specified");
		WaitKey();
		return -1;
	}

	// Set output file name
	if (IsOptionArg(args[1])) {
		switch (OutputFormat) {
			case COMPILE_C_OUTPUT	: OutputFileName = Path::ChangeExtension(InputFileName, "C"); break;
			case COMPILE_VM_OUTPUT	: OutputFileName = Path::ChangeExtension(InputFileName, "VM"); break;
			case COMPILE_CALM_OUTPUT: OutputFileName = Path::ChangeExtension(InputFileName, "ASM"); break;
			case COMPILE_CONVERT	: OutputFileName = Path::ChangeExtension(InputFileName, "LD2"); break;
			default : OutputFileName = Path::ChangeExtension(InputFileName, "txt");
		}
	} else
		OutputFileName = args[1];

	// We have enought to process...
	switch (OutputFormat) {
		case COMPILE_C_OUTPUT	: Compile2C(InputFileName, OutputFileName, args); break;
		case COMPILE_VM_OUTPUT	: Compile2VM(InputFileName, OutputFileName, args); break;
		case COMPILE_CALM_OUTPUT: Compile2CALM(InputFileName, OutputFileName, args); break;
		case COMPILE_CONVERT	: ConvertLD2(InputFileName, OutputFileName, args); break;
	}
	WaitKey();
	
    return 0;
}
