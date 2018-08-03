#include "StdAfx.h"
#include "PLC_COMPILER.h"

PLC_SYMBOL::PLC_SYMBOL(String^ Name, int AddressingSpace)
{
	SymbolName = Name;
	AddrsSpace = AddressingSpace;
}

bool PLC_SYMBOL::Match(String^ Name) // for finding
{
	return String::Equals(SymbolName, Name, StringComparison::InvariantCultureIgnoreCase);
}

//---------------------------------------------------------------------------------------------------------------------//

PLC_BIT_SYMBOL::PLC_BIT_SYMBOL(String^ SymbolName, int AddressingSpace, String^ VarName, String^ BitName, int Bit) :
	PLC_SYMBOL(SymbolName, AddressingSpace)
{
	varName = VarName;
	bitName = BitName;
	bit		= Bit;
}

//----------------------------------------------------------------------------------------------------------------------

PLC_VAR_SYMBOL::PLC_VAR_SYMBOL(String^ SymbolName, int AddressingSpace, int VarSize, bool Unsigned) :
	PLC_SYMBOL(SymbolName, AddressingSpace)	
{
	varSize = VarSize;
	varUnsigned = Unsigned;
}

void PLC_VAR_SYMBOL::MinSize(int VarSize)
{
	if (varSize < VarSize) varSize = VarSize;
}

void PLC_VAR_SYMBOL::InitialValue(int value, String^ Comment)
{
	varInit = true;
	varInitialValue = value;
	InitComment = Comment;
}


//----------------------------------------------------------------------------------------------------------------------

PLC_IO::PLC_IO(String^ Marker, String^ Name, int Bit, int ioType)
{
	marker = Marker;
	name = Name;
	bit = Bit;
	type = ioType;
}

PLC_IO_PORT::PLC_IO_PORT(String^ PortName)
{
	name = PortName;
}

bool PLC_IO_PORT::Match(String^ Name) // for finding
{
	return String::Equals(name, Name, StringComparison::InvariantCultureIgnoreCase);
}

void PLC_IO_PORT::AddIO(String^ Marker, String^ ioName, int Bit, int ioType)
{
	items.Add(gcnew PLC_IO(Marker, ioName, Bit, ioType));
}


//----------------------------------------------------------------------------------------------------------------------

PLC_ALIAS::PLC_ALIAS(String^ AName, String^ AValue)
{
	aliasName = AName;
	aliasValue = AValue;
}

//----------------------------------------------------------------------------------------------------------------------

PLC_VALUE::PLC_VALUE(String^ Name, String^ Value, String^ Comment)
{
	name = Name;
	value = Value;
	comment = Comment;
}

bool PLC_VALUE::Match(String^ Name)
{
	return String::Equals(name, Name, StringComparison::InvariantCultureIgnoreCase);
}

//----------------------------------------------------------------------------------------------------------------------

PLC_COMPILER::PLC_COMPILER(void)
{
	AllowSameSymbolNames = false;
	IndentTabs = "";
}

void PLC_COMPILER::Indent(int Tabs)
{
	if (Tabs < 0) IndentTabs = IndentTabs->Remove(0, -Tabs);
	if (Tabs > 0) IndentTabs = IndentTabs->PadRight(IndentTabs->Length+Tabs, '\t');
}


void PLC_COMPILER::Reset(void)
{
	BitAllocationCounter = 0; //
	Labels.Clear();
	Bits.Clear();
	Vars.Clear();

	// following bits are global and not allocated as memory position
	AddVar("vmStatus", PLC_VAR_SPECIAL, 8, true);
	PresetVar("vmStatus", 0, "Clear all status flags");
	AddBit("$carry",		PLC_BIT_SPECIAL, "vmStatus",	"CARRY",	7);
	AddBit("$zero",			PLC_BIT_SPECIAL, "vmStatus",	"ZERO",		6);
	AddBit("$overflow",		PLC_BIT_SPECIAL, "vmStatus",	"OVERFLOW",	5);
	AddBit("$sgnA",			PLC_BIT_SPECIAL, "vmStatus",	"SgnA",		4);
	AddBit("$sgnB",			PLC_BIT_SPECIAL, "vmStatus",	"SgnB",		3);
	AddBit("$sgnR",			PLC_BIT_SPECIAL, "vmStatus",	"SgnR",		2);
	AddBit("$bit",			PLC_BIT_SPECIAL, "vmStatus",	"BIT",		1);

	AddVar("vmRegAL",		PLC_VAR_SPECIAL, 8);
	AddVar("vmRegAH",		PLC_VAR_SPECIAL, 8);
	AddVar("vmRegBL",		PLC_VAR_SPECIAL, 8);
	AddVar("vmRegBH",		PLC_VAR_SPECIAL, 8);
	AddVar("vmRegRL",		PLC_VAR_SPECIAL, 8);
	AddVar("vmRegRH",		PLC_VAR_SPECIAL, 8);

	AddVar("vmTmrSel", PLC_VAR_SPECIAL, 8);

	AddVar("plcTimerTicks", PLC_VAR_SPECIAL, 8);
	
	AddVar("plcSysBits",	PLC_VAR_SPECIAL, PLCSYSBITS);
	AddBit("$mcr",			PLC_BIT_SPECIAL, "plcSysBits",	"MCR",		7);
	AddBit("$rung_top",		PLC_BIT_SPECIAL, "plcSysBits",	"RungTop",	6);
	AddBit("$scratch",		PLC_BIT_SPECIAL, "plcSysBits",	"Scratch",	5);
	AddBit("$tmr_ovrf",		PLC_BIT_SPECIAL, "plcSysBits", "TmrOvrf",	4);

	AllocateVar("plcTaskRung", 8, true);
}

String^ PLC_COMPILER::CompileError(String^ Msg)
{
	Console::WriteLine("Error : {0}", Msg);
	return Msg;
}

String^ PLC_COMPILER::CompileWarning(String^ Msg)
{
	Console::WriteLine("Warning : {0}", Msg);
	return Msg;
}

bool PLC_COMPILER::AllowSymbolAllocation(String^ Name)
{
	if (AllowSameSymbolNames) return true;
	if (IsLabelAllocated(Name)) return false;
	if (IsBitAllocated(Name)) return false;
	if (IsVarAllocated(Name)) return false;
	return true;
}

bool PLC_COMPILER::IsLabelAllocated(String^ Name)
{
	for each(PLC_LABEL_SYMBOL^ item in Labels) {
		if (item->Match(Name)) return true;
	}
	return false;
}

void PLC_COMPILER::AddLabel(String^ Name)
{
	if (AllowSymbolAllocation(Name)) 
		Labels.Add(gcnew PLC_LABEL_SYMBOL(Name));
	else 
		CompileError(String::Format("Duplicate symbol {0}", Name));
}

String^ PLC_COMPILER::AllocateLabelOneShot(String^ Prefix)
{
	int i;
	String^ SymbolName;
	i = 0;
	while (IsLabelAllocated(String::Format("{0}{1}", Prefix, i))) i++;
	SymbolName = String::Format("{0}{1}", Prefix, i);
	AddLabel(SymbolName);
	return SymbolName;
}

bool PLC_COMPILER::FindBit(String^ Name, PLC_BIT_SYMBOL^ &Bit)
{
	for each(PLC_BIT_SYMBOL^ item in Bits) {
		if (item->Match(Name)) {
			Bit = item;
			return true;
		}
	}
	return false;
}

bool PLC_COMPILER::FindVar(String^ Name, PLC_VAR_SYMBOL^ &Var)
{
	for each(PLC_VAR_SYMBOL^ item in Vars) {
		if (item->Match(Name)) {
			Var = item;
			return true;
		}
	}
	return false;
}

bool PLC_COMPILER::FindValue(String^ Name, PLC_VALUE^ &Value)
{
	for each(PLC_VALUE^ item in Values) {
		if (item->Match(Name)) {
			Value = item;
			return true;
		}
	}
	return false;
}


bool PLC_COMPILER::IsBitAllocated(String^ Name)
{
	PLC_BIT_SYMBOL^ Bit;
	return FindBit(Name, Bit);
}

String^ PLC_COMPILER::BitSyntaxName(String^ Name)
{
	return Name;
}

String^ PLC_COMPILER::VarSyntaxName(String^ Name)
{
	return Name;
}


void PLC_COMPILER::AddBit(String^ Name, int AddressingSpace, String^ VarName, String^ BitName, int Bit)
{
	if (AllowSymbolAllocation(Name)) 
		Bits.Add(gcnew PLC_BIT_SYMBOL(Name, AddressingSpace, VarName, BitName, Bit));
	else 
		CompileError(String::Format("Duplicate symbol {0}", Name));
}

void PLC_COMPILER::AllocateBit(String^ Name)
{
	if (IsBitAllocated(Name)) return;
	int bit;
	int byte;
    byte = BitAllocationCounter / 8;
	bit = BitAllocationCounter % 8;
	BitAllocationCounter++;
	if (bit == 0) AllocateVar(String::Format("plcBits[{0}]", byte), PLCBITS, true);
	AddBit(Name, PLC_BIT_INTERNAL, String::Format("plcBits[{0}]", byte), String::Format("b{0}", bit), bit);
}

String^ PLC_COMPILER::AllocateBitOneShot(String^ Prefix)
{
	int i;
	String^ SymbolName;
	i = 0;
	while (IsBitAllocated(String::Format("{0}{1}", Prefix, i))) i++;
	SymbolName = String::Format("{0}{1}", Prefix, i);
	AllocateBit(SymbolName);
	return SymbolName;
}



bool PLC_COMPILER::IsVarAllocated(String^ Name)
{
	for each(PLC_VAR_SYMBOL^ item in Vars) {
		if (item->Match(Name)) return true;
	}
	return false;
}

void PLC_COMPILER::AddVar(String^ Name, int AddressingSpace, int VarSize, bool Unsigned)
{
	if (AllowSymbolAllocation(Name)) 
		Vars.Add(gcnew PLC_VAR_SYMBOL(Name, AddressingSpace, VarSize, Unsigned));
	else 
		CompileError(String::Format("Duplicate symbol {0}", Name));
}

void PLC_COMPILER::AddVar(String^ Name, int AddressingSpace, int VarSize)
{
	AddVar(Name, AddressingSpace, VarSize, false); 	
}

void PLC_COMPILER::AllocateVar(String^ Name, int VarSize, bool Unsigned)
{
	for each(PLC_VAR_SYMBOL^ item in Vars) {
		if (item->Match(Name)) {
			item->MinSize(VarSize);
			return;
		}
	}
	AddVar(Name, PLC_VAR_INTERNAL, VarSize, Unsigned);
}

void PLC_COMPILER::AllocateVar(String^ Name, int VarSize)
{
	AllocateVar(Name, VarSize, false);
}

void PLC_COMPILER::PresetVar(String^ varName, int Value, String^ Comment)
{
	PLC_VAR_SYMBOL^ var;
	if (FindVar(varName, var))
		var->InitialValue(Value, Comment);
	else
		CompileError(String::Format("Preset Var {0} = {1} ({2}) : var not declared", varName, Value, Comment));
}


void PLC_COMPILER::AddIO(String^ Marker, String^ Name, String^ Port, int Bit, int ioType)
{
	for each(PLC_IO_PORT^ item in PortsIO) {
		if (item->Match(Port)) {
			item->AddIO(Marker, Name, Bit, ioType);
			AddBit(Name, PLC_BIT_IO, Port, Name, Bit);
			return;
		}
	}
	PLC_IO_PORT^ item = gcnew PLC_IO_PORT(Port);
	PortsIO.Add(item);
	item->AddIO(Marker, Name, Bit, ioType);
	AddBit(Name, PLC_BIT_IO, Port, Name, Bit);
}


void PLC_COMPILER::ClearAliases(void)
{
	Aliases.Clear();
}

void PLC_COMPILER::AddAlias(String^ Name, String^ Value)
{
	Aliases.Add(gcnew PLC_ALIAS(Name, Value));
}

String^ PLC_COMPILER::ReplaceAliases(String^ CodeStr)
{
	for each(PLC_ALIAS^ item in Aliases) {
//		if (item->aliasName->Contains("$par")) Console::Write("Replace {0} with {1} in {2}", item->aliasName, item->aliasValue, CodeStr);
		CodeStr = CodeStr->Replace(item->aliasName, item->aliasValue);
//		if (item->aliasName->Contains("$par")) Console::WriteLine("=> {0}", CodeStr);
	};
	return CodeStr;
}

void PLC_COMPILER::SetValue(String^ Name, String^ Value, String^ Comment)
{
	PLC_VALUE^ AValue;
	if (FindValue(Name, AValue)) {
		AValue->value = Value;
		AValue->comment = Comment;		
	} else {
		Values.Add(gcnew PLC_VALUE(Name, Value, Comment));
	}
	Console::WriteLine("Compiler Value {0} = {1} {2}", Name, Value, Comment); 
}

void PLC_COMPILER::SetValue(String^ Name, int Value, String^ Comment)
{
	SetValue(Name, Value.ToString(), Comment);
}

void PLC_COMPILER::SetValue(String^ Name, bool Value, String^ Comment)
{
	SetValue(Name, Value.ToString(), Comment);
}

String^ PLC_COMPILER::GetValue(String^ Name, String^ Default)
{
	PLC_VALUE^ AValue;
	if (FindValue(Name, AValue)) 
		return AValue->value;
	else
		return Default;
}

String^ PLC_COMPILER::GetValue(String^ Name)
{
	return GetValue(Name, "");
}

int PLC_COMPILER::GetIntValue(String^ Name, int Default)
{
	return Convert::ToUInt32(GetValue(Name, Default.ToString()));
}

int PLC_COMPILER::GetIntValue(String^ Name)
{
	return GetIntValue(Name, 0);
}

bool PLC_COMPILER::GetBoolValue(String^ Name, bool Default)
{
	return Convert::ToBoolean(GetValue(Name, Default.ToString()));
}

bool PLC_COMPILER::GetBoolValue(String^ Name)
{
	return GetBoolValue(Name, false);
}

String^ PLC_COMPILER::GetCommentValue(String^ Name)
{
	PLC_VALUE^ AValue;
	if (FindValue(Name, AValue)) 
		return AValue->comment;
	else
		return "";
}


/*void PLC_COMPILER::McuTarget(String^ Mcu, int cycleTime, int mcuClock, int baudRate)
{
}*/

void PLC_COMPILER::plcRungBegin(int Rung)
{
}

void PLC_COMPILER::plcRungEnd(int Rung)
{
}


void PLC_COMPILER::plcGenerateReset(void)
{
}


void PLC_COMPILER::DumpVarsTable(TextWriter^ sw)
{
	sw->WriteLine(";=== Vars Table ===");
	for each(PLC_VAR_SYMBOL^ item in Vars) {
		sw->WriteLine("; {0} = {1}", item->SymbolName, VarSyntaxName(item->SymbolName));
	}
}

void PLC_COMPILER::DumpBitsTable(TextWriter^ sw)
{
	sw->WriteLine(";=== Bits Table ===");
	for each(PLC_BIT_SYMBOL^ item in Bits) {
		sw->WriteLine("; {0} = {1}", item->SymbolName, BitSyntaxName(item->SymbolName));
	}
}

void PLC_COMPILER::DumpIOTable(TextWriter^ sw)
{
	sw->WriteLine(";=== IO Table ===");
	for each(PLC_IO_PORT^ item in PortsIO) {
		sw->WriteLine("; {0}", item->name);
	}
}

//----------------------------------------------------------------------------------------------------------------------

PLC_COMPILER_C::PLC_COMPILER_C(TextWriter^ OutputC) :
	PLC_COMPILER()
{
	swC = OutputC;
	ExpandReplaceMacroBitIO = true;
	ExpandReplaceMacroBitInternal = true;
	ExpandReplaceMacroBitExternal = true;
	ExpandReplaceMacroBitSpecial = true;
}

void PLC_COMPILER_C::OutputC(String^ CodeStr)
{
	swC->WriteLine("{0}{1}", IndentTabs, ExpandMacros(ReplaceAliases(CodeStr)));	
}

void PLC_COMPILER_C::OutputC(String^ CodeStr, String^ Comment)
{
	swC->WriteLine("{0}{1}{2}", IndentTabs, ExpandMacros(ReplaceAliases(CodeStr)), ReplaceAliases(Comment));	
}

String^ PLC_COMPILER_C::ExpandMacros(String^ CodeStr)
{
	PLC_BIT_SYMBOL^ Bit;
	int i, p1, p2, p3;
	String^ result;
	String^ s1;
	String^ bitName;
	i = 0;
	result = "";
	while ( i < CodeStr->Length ) {
		s1 = CodeStr->Substring(i);
		if ( s1->StartsWith("GetBit(") ) {
			p1 = s1->IndexOf('(')+1;
			p2 = IndexOfClosingParenthesis(s1);
			bitName = s1->Substring(p1, p2-p1)->Trim();
			if (FindBit(bitName, Bit))
				switch (Bit->AddrsSpace) {
					case PLC_BIT_IO			: result = result + MacroGetBit(bitName, ExpandReplaceMacroBitIO); break;
					case PLC_BIT_INTERNAL	: result = result + MacroGetBit(bitName, ExpandReplaceMacroBitInternal); break;
					case PLC_BIT_EXTERNAL	: result = result + MacroGetBit(bitName, ExpandReplaceMacroBitExternal); break;
					case PLC_BIT_SPECIAL	: result = result + MacroGetBit(bitName, ExpandReplaceMacroBitSpecial); break;
					default	: result = result + MacroGetBit(bitName, ExpandReplaceMacroBitSpecial);
				}
			else
				result = result + CompileError(String::Format("unknow Bit {0}", bitName));	
			i += p2+1;
		} // if "GetBit"
		else if ( s1->StartsWith("SetBit(") ) {
			p1 = s1->IndexOf('(')+1;
			p2 = IndexOfClosingParenthesis(s1);
			bitName = s1->Substring(p1, p2-p1)->Trim();
			if (FindBit(bitName, Bit))
				switch (Bit->AddrsSpace) {
					case PLC_BIT_IO			: result = result + MacroSetBit(bitName, ExpandReplaceMacroBitIO); break;
					case PLC_BIT_INTERNAL	: result = result + MacroSetBit(bitName, ExpandReplaceMacroBitInternal); break;
					case PLC_BIT_EXTERNAL	: result = result + MacroSetBit(bitName, ExpandReplaceMacroBitExternal); break;
					case PLC_BIT_SPECIAL	: result = result + MacroSetBit(bitName, ExpandReplaceMacroBitSpecial); break;
					default	: result = result + MacroSetBit(bitName, ExpandReplaceMacroBitSpecial);
				}
			else
				result = result + CompileError(String::Format("unknow Bit {0}", bitName));	
			i += p2+1;
		} // if "SetBit"
		else if ( s1->StartsWith("ClearBit(") ) {
			p1 = s1->IndexOf('(')+1;
			p2 = IndexOfClosingParenthesis(s1);
			bitName = s1->Substring(p1, p2-p1)->Trim();
			if (FindBit(bitName, Bit))
				switch (Bit->AddrsSpace) {
					case PLC_BIT_IO			: result = result + MacroClearBit(bitName, ExpandReplaceMacroBitIO); break;
					case PLC_BIT_INTERNAL	: result = result + MacroClearBit(bitName, ExpandReplaceMacroBitInternal); break;
					case PLC_BIT_EXTERNAL	: result = result + MacroClearBit(bitName, ExpandReplaceMacroBitExternal); break;
					case PLC_BIT_SPECIAL	: result = result + MacroClearBit(bitName, ExpandReplaceMacroBitSpecial); break;
					default	: result = result + MacroClearBit(bitName, ExpandReplaceMacroBitSpecial);
				}
			else
				result = result + CompileError(String::Format("unknow Bit {0}", bitName));	
			i += p2+1;
		} // if "ClearBit"
		else if ( s1->StartsWith("AssignBit(") ) {
			p1 = s1->IndexOf('(')+1;
			p2 = IndexOfClosingParenthesis(s1);
			p3 = s1->IndexOf(',');
			bitName = s1->Substring(p1, p3-p1)->Trim();
			if (FindBit(bitName, Bit))
				switch (Bit->AddrsSpace) {
					case PLC_BIT_IO			: result = result + MacroAssignBit(bitName, ExpandMacros(s1->Substring(p3+1, p2-p3-1)->Trim()), ExpandReplaceMacroBitIO); break;
					case PLC_BIT_INTERNAL	: result = result + MacroAssignBit(bitName, ExpandMacros(s1->Substring(p3+1, p2-p3-1)->Trim()), ExpandReplaceMacroBitInternal); break;
					case PLC_BIT_EXTERNAL	: result = result + MacroAssignBit(bitName, ExpandMacros(s1->Substring(p3+1, p2-p3-1)->Trim()), ExpandReplaceMacroBitExternal); break;
					case PLC_BIT_SPECIAL	: result = result + MacroAssignBit(bitName, ExpandMacros(s1->Substring(p3+1, p2-p3-1)->Trim()), ExpandReplaceMacroBitSpecial); break;
					default	: result = result + MacroAssignBit(bitName, ExpandMacros(s1->Substring(p3+1, p2-p3-1)->Trim()), ExpandReplaceMacroBitSpecial);
				}
			else
				result = result + CompileError(String::Format("unknow Bit {0}", bitName));	
			i += p2+1;
		} // if "AssignBit"
		else {
			result = result + s1[0];
			i++;
		}
	} // while 
	return result;
}


String^ PLC_COMPILER_C::BitSyntaxName(String^ Name)
{
	PLC_BIT_SYMBOL^ Bit;
	if (FindBit(Name, Bit))
		if (Bit->AddrsSpace != PLC_BIT_IO) // IO bits always have a #define varName ... for easy changes
			return String::Format("{0}.{1}", Bit->varName, Bit->bitName);
		else
			return String::Format("{0}", Bit->bitName);
	else
		return CompileError(String::Format("unknow Bit {0}", Name));
}

String^ PLC_COMPILER_C::MacroGetBit(String^ src, bool Expand)
{
	if (Expand)
		return BitSyntaxName(src);
	else
		return String::Format("plcGetBit_{0}", RemoveDollar(src));
}

String^ PLC_COMPILER_C::MacroAssignBit(String^ dest, String^ src, bool Expand)
{
	if (Expand)
		return String::Format("{0} = {1}", BitSyntaxName(dest), src);
	else
		return String::Format("plcAssignBit_{0}({1})", RemoveDollar(dest), src);
}

String^ PLC_COMPILER_C::MacroSetBit(String^ dest, bool Expand)
{
	if (Expand)
		return String::Format("{0} = true", BitSyntaxName(dest));
	else
		return String::Format("plcSetBit_{0}", RemoveDollar(dest));
}

String^ PLC_COMPILER_C::MacroClearBit(String^ dest, bool Expand)
{
	if (Expand)
		return String::Format("{0} = false", BitSyntaxName(dest));
	else
		return String::Format("plcClearBit_{0}", RemoveDollar(dest));
}

void PLC_COMPILER_C::Begin(String^ BlockMarker, String^ BlockName)
{
//	OutputC("");
	OutputC("",	String::Format("// BEGIN {0} {1}", BlockMarker, BlockName));
}

void PLC_COMPILER_C::End(String^ BlockMarker, String^ BlockName)
{
	OutputC("", String::Format("// END {0} {1}", BlockMarker, BlockName));
}

void PLC_COMPILER_C::Code_MacroDefines(void)
{
	// Find we have some ReplaceMacro to define...
	for each(PLC_BIT_SYMBOL^ item in Bits) {
		if ((item->AddrsSpace == PLC_BIT_IO) && !ExpandReplaceMacroBitIO ||
			(item->AddrsSpace == PLC_BIT_INTERNAL) && !ExpandReplaceMacroBitInternal || 
			(item->AddrsSpace == PLC_BIT_EXTERNAL) && !ExpandReplaceMacroBitExternal ||
			(item->AddrsSpace == PLC_BIT_SPECIAL) && !ExpandReplaceMacroBitSpecial) {	
				swC->WriteLine();
				swC->WriteLine("/**");
				swC->WriteLine(" *  PLC bits access");
				swC->WriteLine(" *  The following macro regulate the bits access");
				swC->WriteLine(" *  as generated, macro will expand as direct bit access like : bitname = value");
				swC->WriteLine(" *  you can change this if you need to access bit by functions :");
				swC->WriteLine(" *  comment the plcGetBit_xxx and plcAssign_xxx statement");
				swC->WriteLine(" *  and provide external functions");
				swC->WriteLine(" */");
				swC->WriteLine();

				for each(PLC_BIT_SYMBOL^ item in Bits) {
					if ((item->AddrsSpace == PLC_BIT_IO) && !ExpandReplaceMacroBitIO ||
						(item->AddrsSpace == PLC_BIT_INTERNAL) && !ExpandReplaceMacroBitInternal || 
						(item->AddrsSpace == PLC_BIT_EXTERNAL) && !ExpandReplaceMacroBitExternal ||
						(item->AddrsSpace == PLC_BIT_SPECIAL) && !ExpandReplaceMacroBitSpecial) 
						{
							swC->WriteLine("");
							swC->WriteLine("#define plcGetBit_{0} {1}", RemoveDollar(item->SymbolName), BitSyntaxName(item->SymbolName));
							swC->WriteLine("#define plcAssignBit_{0}(x) {1} = x", RemoveDollar(item->SymbolName), BitSyntaxName(item->SymbolName));
							swC->WriteLine("#define plcSetBit_{0} plcAssignBit_{0}(true)", RemoveDollar(item->SymbolName));
							swC->WriteLine("#define plcClearBit_{0} plcAssignBit_{0}(false)", RemoveDollar(item->SymbolName));
						}
				}
				break;
		}
	}
}


void PLC_COMPILER_C::Code_VarGlobals(void)
{
	swC->WriteLine();
	swC->WriteLine("/** PLC globals */");

	swC->WriteLine("#pragma udata access USER_ACCESS_DATA");
	swC->WriteLine("near PLCSYSBITS plcSysBits;"); 
	swC->WriteLine("near INT8 plcTimerTicks;"); 
	swC->WriteLine("#pragma udata");
	

	for each(PLC_VAR_SYMBOL^ item in Vars) {
		if ((item->AddrsSpace == PLC_VAR_INTERNAL) && (item->varSize > 0)) {
			if (item->varUnsigned)
				swC->WriteLine("UINT{0} {1};", item->varSize, VarSyntaxName(item->SymbolName));
			else
				swC->WriteLine("INT{0} {1};", item->varSize, VarSyntaxName(item->SymbolName));
		} // if
	} // for

	swC->WriteLine("PLCBITS plcBits[{0}];", (BitAllocationCounter / 8) + 1);
}

void PLC_COMPILER_C::Code_plcTimerTicks(void)
{
	swC->WriteLine("");
	swC->WriteLine("/*************************************************");
	swC->WriteLine(" * user must provide the following function depending of timer implementation"); 
	swC->WriteLine(" * the value set by plcTimerTicksOccured() produce these behaviors :");
	swC->WriteLine(" *    -1 : no timer ticks occured, plcTask() will exit without executing rungs");
	swC->WriteLine(" *     0 : no timer ticks occured, plcTask() will execute rungs");
	swC->WriteLine(" *    >0 : timer ticks occured, PLC timers are updated, plcTask() execute rungs");
	swC->WriteLine(" */");
	swC->WriteLine("// #define plcTimerTicksOccured(x) plcTimerTicks = 1;");
	swC->WriteLine("");
	swC->WriteLine("#ifndef plcTimerTicksOccured");
	swC->WriteLine("#pragma udata");
	swC->WriteLine("TICK plcPreviousTick;\t//!< Hold previous Tick to compute difference");
	swC->WriteLine("");
	swC->WriteLine("#pragma code");
	OutputC("void plcTimerTicksOccured(void)");
	OutputC("{");
	Indent(+1);
	OutputC("plcTimerTicks = TickGet()-plcPreviousTick;");
	OutputC("plcPreviousTick = TickGet();");
	Indent(-1);
	OutputC("} // void plcTimerTicksOccured(void)");
	swC->WriteLine("#endif");
}

void PLC_COMPILER_C::Code_plcReset(void)
{
	swC->WriteLine("");
	swC->WriteLine("/*************************************************");
	swC->WriteLine(" * Reset the PLC"); 
	swC->WriteLine(" */");
	swC->WriteLine("");
	swC->WriteLine("#pragma code");
	OutputC("void plcReset(void)");
	OutputC("{");
	Indent(+1);
	OutputC("plcTaskRung = 0; \t// force rung 0 to execute");
	OutputC("SetBit($mcr);", "\t// Start PLC");
	OutputC("// PLC rung 0 will initailize the PLC");

	Indent(-1);
	OutputC("} // void plcReset(void)");
}

void PLC_COMPILER_C::Code_plcDebug(void)
{
	int i;
	String^ format = "";
	swC->WriteLine("");
	swC->WriteLine("/*************************************************");
	swC->WriteLine(" * Debug PLC"); 
	swC->WriteLine(" */");
	swC->WriteLine("");
	swC->WriteLine("#pragma code");
	OutputC("void plcDebug(FILE * f)");
	OutputC("{");
	Indent(+1);

    OutputC("fprintf(f, \"plcSysBits %08hhb\\r\\n\", plcSysBits.val);");	
    OutputC("fprintf(f, \"plcTimerTicks %0hhd\\r\\n\", plcTimerTicks);");	

	OutputC("");
	for (i=0; i <= BitAllocationCounter / 8; i++) OutputC(String::Format("fprintf(f, \"plcBits[{0}] %08b\\r\\n\", plcBits[{0}].val);", i));
	for each(PLC_VAR_SYMBOL^ item in Vars) {
		if ((item->AddrsSpace == PLC_VAR_INTERNAL) && (item->varSize > 0)) {
			switch (item->varSize) {
				case  8 : if (item->varUnsigned)	format = "% 3hhu";  else format = "% 3hhd"; break;
				case 16 : if (item->varUnsigned)	format = "% 5hu";   else format = "% 5hd"; break;
				case 24 : if (item->varUnsigned)	format = "% 8Hu";   else format = "% 8Hd"; break;
				case 32 : if (item->varUnsigned)	format = "% 10lu";  else format = "% 10ld"; break;
				default : format = "%#u";
			}
			OutputC(String::Format("fprintf(f, \"{0} {1}\\r\\n\", {0});  // varSize : {2}", VarSyntaxName(item->SymbolName), format, item->varSize));	
		} // if
	} // for
	Indent(-1);
	OutputC("} // void plcDebug(FILE * f)");
}

//----------------------------------------------------------------------------------------------------------------------


PLC_COMPILER_VM::PLC_COMPILER_VM(TextWriter^ OutputMain) :
	PLC_COMPILER()
{
	swMain = OutputMain;
}


String^ PLC_COMPILER_VM::BitSyntaxName(String^ Name)
{
	PLC_BIT_SYMBOL^ Bit;
	if (FindBit(Name, Bit))
		return String::Format("{0}:#{1}", Bit->varName, Bit->bitName);
	else
		return CompileError(String::Format("unknow Bit {0}", Name));
}

String^ PLC_COMPILER_VM::vmBitSuffix(String^ bitName) 
{
	PLC_BIT_SYMBOL^ Bit;
	if (FindBit(bitName, Bit))
		switch (Bit->AddrsSpace) {
			case PLC_BIT_IO			: return ".io"; break;
			case PLC_BIT_INTERNAL	: return ".r"; break;
			case PLC_BIT_EXTERNAL	: return ".x"; break;
			case PLC_BIT_SPECIAL	: return ".s"; break;
			default	: return "";
		}
	else
		return "";
}

String^ PLC_COMPILER_VM::vmVarSuffix(String^ varName)
{
	PLC_VAR_SYMBOL^ Var;
	String^ result;
	if (FindVar(varName, Var)) {
		switch (Var->AddrsSpace) {
			case PLC_LITERAL		: result = ".l"; break;
			case PLC_VAR_INTERNAL	: result = ".r"; break;
			case PLC_VAR_EXTERNAL	: result = ".x"; break;
			case PLC_VAR_SPECIAL	: result = ".s"; break;
			default	: result = "";
		}
		return result + Var->varSize.ToString();
	} else
		return "";
}


void PLC_COMPILER_VM::vmWriteLine(String^ mnemonic, String^ argument, String^ comment)
{
	if (String::IsNullOrEmpty(comment))
		swMain->WriteLine("\t\t{0}\t\t{1}", mnemonic, argument);
	else
		swMain->WriteLine("\t\t{0}\t\t{1}\t\t; {2}", mnemonic, argument, comment);
}

void PLC_COMPILER_VM::vmComment(String^ msg)
{
	swMain->WriteLine("\t\t; {0}", msg);
}

void PLC_COMPILER_VM::vmWarning(String^ msg)
{
	Console::WriteLine("Warning : {0}", msg);
	swMain->WriteLine("; !!! Warning : {0}", msg);
}

void PLC_COMPILER_VM::vmError(String^ msg)
{
	Console::WriteLine("*ERROR* {0}", msg);
	swMain->WriteLine("!!! Error : {0}", msg);
}

void PLC_COMPILER_VM::vmLabel(String^ label)
{
	swMain->Write("{0}:", label);
}


void PLC_COMPILER_VM::vmSetBit(String^ bitName, String^ comment)
{
	vmWriteLine("vmSetBit"+vmBitSuffix(bitName), BitSyntaxName(bitName), comment);
}

void PLC_COMPILER_VM::vmSetBit(String^ bitName)
{
	vmSetBit(bitName, bitName);
}


void PLC_COMPILER_VM::vmClearBit(String^ bitName, String^ comment)
{
	vmWriteLine("vmClearBit"+vmBitSuffix(bitName), BitSyntaxName(bitName), comment);
}

void PLC_COMPILER_VM::vmClearBit(String^ bitName)
{
	vmClearBit(bitName, bitName);
}

void PLC_COMPILER_VM::vmGetBit(String^ bitName, String^ comment)
{
	vmWriteLine("vmGetBit"+vmBitSuffix(bitName), BitSyntaxName(bitName), comment);
}

void PLC_COMPILER_VM::vmGetBit(String^ bitName)
{
	vmGetBit(bitName, bitName);
}

void PLC_COMPILER_VM::vmPutBit(String^ bitName, String^ comment)
{
	vmWriteLine("vmPutBit"+vmBitSuffix(bitName), BitSyntaxName(bitName), comment);
}

void PLC_COMPILER_VM::vmPutBit(String^ bitName)
{
	vmPutBit(bitName, bitName);
}


void PLC_COMPILER_VM::vmSkipIfBitClear(String^ bitName, String^ comment)
{
	vmWriteLine("vmSkipIfBitClear"+vmBitSuffix(bitName), BitSyntaxName(bitName), comment);
}

void PLC_COMPILER_VM::vmSkipIfBitClear(String^ bitName)
{
	vmSkipIfBitClear(bitName, bitName);
}

void PLC_COMPILER_VM::vmSkipIfBitSet(String^ bitName, String^ comment)
{
	vmWriteLine("vmSkipIfBitSet"+vmBitSuffix(bitName), BitSyntaxName(bitName), comment);
}

void PLC_COMPILER_VM::vmSkipIfBitSet(String^ bitName)
{
	vmSkipIfBitSet(bitName, bitName);
}


void PLC_COMPILER_VM::vmClear(System::String^ varDest, String^ comment)
{
	vmWriteLine("vmClear"+vmVarSuffix(varDest), VarSyntaxName(varDest), comment);
}

void PLC_COMPILER_VM::vmClear(System::String^ varDest)
{
	vmClear(varDest, varDest);
}

void PLC_COMPILER_VM::vmLoadA(System::String^ varSrc, String^ comment)
{
	if (IsNumber(varSrc))
		vmLoadA(Convert::ToInt32(varSrc), comment);
	else
		vmWriteLine("vmLoadA"+vmVarSuffix(varSrc), VarSyntaxName(varSrc), comment);
}

void PLC_COMPILER_VM::vmLoadA(int value, String^ comment)
{
	vmWriteLine("vmLoadLiteralA", value.ToString(), comment);
}

void PLC_COMPILER_VM::vmLoadA(System::String^ varSrc)
{
	vmLoadA(varSrc, varSrc);
}

void PLC_COMPILER_VM::vmLoadA(int value)
{
	vmLoadA(value, "");
}

void PLC_COMPILER_VM::vmLoadB(System::String^ varSrc, String^ comment)
{
	if (IsNumber(varSrc))
		vmLoadB(Convert::ToInt32(varSrc), comment);
	else
		vmWriteLine("vmLoadB"+vmVarSuffix(varSrc), VarSyntaxName(varSrc), comment);
}

void PLC_COMPILER_VM::vmLoadB(int value, String^ comment)
{
	vmWriteLine("vmLoadLiteralB", value.ToString(), comment);
}

void PLC_COMPILER_VM::vmLoadB(System::String^ varSrc)
{
	vmLoadB(varSrc, varSrc);
}

void PLC_COMPILER_VM::vmLoadB(int value)
{
	vmLoadB(value, "");
}


void PLC_COMPILER_VM::vmLoadR(System::String^ varSrc, String^ comment)
{
	if (IsNumber(varSrc))
		vmLoadR(Convert::ToInt32(varSrc), comment);
	else
		vmWriteLine("vmLoadR"+vmVarSuffix(varSrc), VarSyntaxName(varSrc), comment);
}

void PLC_COMPILER_VM::vmLoadR(int value, String^ comment)
{
	vmWriteLine("vmLoadLiteralR", value.ToString(), comment);
}

void PLC_COMPILER_VM::vmLoadR(System::String^ varSrc)
{
	vmLoadR(varSrc, varSrc);
}

void PLC_COMPILER_VM::vmLoadR(int value)
{
	vmLoadR(value, "");
}

void PLC_COMPILER_VM::vmStoreR(String^ varDest, String^ comment)
{
	vmWriteLine("vmStoreR"+vmVarSuffix(varDest), VarSyntaxName(varDest), comment);
}

void PLC_COMPILER_VM::vmStoreR(String^ varDest)
{
	vmStoreR(varDest, varDest);
}

void PLC_COMPILER_VM::vmAdd(String^ varDest, String^ comment)
{
	vmWriteLine("vmAdd"+vmVarSuffix(varDest), VarSyntaxName(varDest), comment);
}

void PLC_COMPILER_VM::vmAdd(String^ varDest)
{
	vmAdd(varDest, varDest);
}


void PLC_COMPILER_VM::vmSub(String^ varDest, String^ comment)
{
	vmWriteLine("vmSub"+vmVarSuffix(varDest), VarSyntaxName(varDest), comment);
}

void PLC_COMPILER_VM::vmSub(String^ varDest)
{
	vmSub(varDest, varDest);
}

void PLC_COMPILER_VM::vmMul(String^ varDest, String^ comment)
{
	vmWriteLine("vmMul"+vmVarSuffix(varDest), VarSyntaxName(varDest), comment);
}

void PLC_COMPILER_VM::vmMul(String^ varDest)
{
	vmMul(varDest, varDest);
}

void PLC_COMPILER_VM::vmDiv(String^ varDest, String^ comment)
{
	vmWriteLine("vmDiv"+vmVarSuffix(varDest), VarSyntaxName(varDest), comment);
}

void PLC_COMPILER_VM::vmDiv(String^ varDest)
{
	vmSub(varDest, varDest);
}


void PLC_COMPILER_VM::vmSkipIfFlagSet(String^ flags, String^ comment)
{
	vmWriteLine("vmSkipIfFlagSet", flags, comment);
}

void PLC_COMPILER_VM::vmSkipIfFlagSet(String^ flags)
{
	vmSkipIfFlagSet(flags, "");
}

void PLC_COMPILER_VM::vmGoto(String^ label, String^ comment)
{
	vmWriteLine("vmGoto", label, comment);
}

void PLC_COMPILER_VM::vmGoto(String^ label)
{
	vmGoto(label, "");
}

void PLC_COMPILER_VM::vmInc(String^ varDest, String^ comment)
{
	vmWriteLine("vmInc"+vmVarSuffix(varDest), VarSyntaxName(varDest), comment);
}

void PLC_COMPILER_VM::vmInc(String^ varDest)
{
	vmInc(varDest, varDest);
}


void PLC_COMPILER_VM::vmSelTmr(String^ varTimerTicks, String^ comment)
{
	vmWriteLine("vmSelTmr"+vmVarSuffix(varTimerTicks), VarSyntaxName(varTimerTicks), comment);
}

void PLC_COMPILER_VM::vmSelTmr(String^ varTimerTicks)
{
	vmSelTmr(varTimerTicks, varTimerTicks);
}

void PLC_COMPILER_VM::vmIncTmr(String^ varDest, String^ comment)
{
	vmWriteLine("vmIncTmr"+vmVarSuffix(varDest), VarSyntaxName(varDest), comment);
}

void PLC_COMPILER_VM::vmIncTmr(String^ varDest)
{
	vmIncTmr(varDest, varDest);
}

/*void PLC_COMPILER_VM::McuTarget(String^ Mcu, int cycleTime, int mcuClock, int baudRate)
{
	vmComment(String::Format("Target : {0}", Mcu));
	vmComment(String::Format("Cycle  : {0} uS", cycleTime));
	vmComment(String::Format("Clock  : {0} Hz", mcuClock));
	vmComment(String::Format("Baud   : {0}", baudRate));
}*/

void PLC_COMPILER_VM::plcGenerateReset(void)
{
	swMain->WriteLine("");
	swMain->WriteLine("plcRESET:\t\t; Reset the PLC");
	swMain->WriteLine(";\tClear memory between PlcBeginVar and PlcEndVar");
	swMain->WriteLine("\t\tvmFillChar PlcBeginVar, PlcEndVar-PlcBeginVar, 0");
	swMain->WriteLine(";\tInit PLC variables");
	for each(PLC_VAR_SYMBOL^ item in Vars) {
		if (item->varInit) {
			vmLoadR(item->varInitialValue, item->InitComment);
			vmStoreR(item->SymbolName);
		}
	}
	swMain->WriteLine("");
	swMain->WriteLine(";\tInit Port IO");
}


void PLC_COMPILER_VM::DumpVarsTable(TextWriter^ sw)
{
	sw->WriteLine("");
	sw->WriteLine(";=== Vars Table ===");
	for each(PLC_VAR_SYMBOL^ item in Vars) {
		sw->WriteLine("; {1}\t\t{0} ({2} bits)", item->SymbolName, VarSyntaxName(item->SymbolName), item->varSize);
	}
}

void PLC_COMPILER_VM::DumpBitsTable(TextWriter^ sw)
{
	sw->WriteLine("");
	sw->WriteLine(";=== Bits definition ===");
	for each(PLC_BIT_SYMBOL^ item in Bits) {
		sw->WriteLine("; {0}\t= {1}\t\t; {2} ({3})", item->bitName, item->bit, item->SymbolName, BitSyntaxName(item->SymbolName));
	}
}

//********************************************************************************************************************************************************

PLC_COMPILER_CALM::PLC_COMPILER_CALM(TextWriter^ OutputMain) :
	PLC_COMPILER_VM(OutputMain)
{
}

void PLC_COMPILER_CALM::DumpVarsTable(TextWriter^ sw)
{
	sw->WriteLine("");
	sw->WriteLine("\\var:==> Variables <===");		
	sw->WriteLine("; Special access var");
	sw->WriteLine(".APC\t{0}", "ACCESSBANK");
	sw->WriteLine("");
	for each(PLC_VAR_SYMBOL^ item in Vars) {
		if (item->AddrsSpace == PLC_VAR_SPECIAL) {
			if (item->varSize <= 8)
				sw->WriteLine("{1}:\t.BLK.16 1\t\t; {0}", item->SymbolName, VarSyntaxName(item->SymbolName));
			else
				sw->WriteLine("{1}:\t.BLK.16 2\t\t; {0}", item->SymbolName, VarSyntaxName(item->SymbolName));
		}
	}
	sw->WriteLine("");
	sw->WriteLine("; Plc vars ");
	sw->WriteLine(".APC\t{0}", "BANK0");
	sw->WriteLine("");
	sw->WriteLine("PlcBeginVar:\t\t; !!! All memory location between PlcBeginVar and PLCEndVar will be preset");
	for each(PLC_VAR_SYMBOL^ item in Vars) {
		if (item->AddrsSpace == PLC_VAR_INTERNAL) {
			if (item->varSize <= 8)
				sw->WriteLine("{1}:\t.BLK.16 1\t\t; {0}", item->SymbolName, VarSyntaxName(item->SymbolName));
			else
				sw->WriteLine("{1}:\t.BLK.16 2\t\t; {0}", item->SymbolName, VarSyntaxName(item->SymbolName));
		}
	}
	sw->WriteLine("PlcEndVar:\t\t; !!! All memory location between PlcBeginVar and PLCEndVar will be preset");
}

void PLC_COMPILER_CALM::DumpBitsTable(TextWriter^ sw)
{
	int i;
	sw->WriteLine("");
	sw->WriteLine("\\const:==> Bit access <===");		
	for (i = 0; i<=7; i++) {
		sw->WriteLine("Bit{0}\t= {0}", i);
	}
	for each(PLC_BIT_SYMBOL^ item in Bits) {
		if (!(item->bitName->StartsWith("Bit") && (item->bitName->Length == 4)))
			sw->WriteLine("{0}\t= {1}\t\t; {2} ({3})", item->bitName, item->bit, item->SymbolName, BitSyntaxName(item->SymbolName));
	}
}

void PLC_COMPILER_CALM::DumpIOTable(TextWriter^ sw)
{
	sw->WriteLine("");
	sw->WriteLine(";=== IO Table ===");
	for each(PLC_IO_PORT^ item in PortsIO) {
		sw->WriteLine("; {0}", item->name);
		for each(PLC_IO^ io in item->items) {
			sw->WriteLine(";\t{0}:#{1}\t-> {2} ({3})", item->name, io->bit, io->name, io->marker);
		}
		
	}
}

void PLC_COMPILER_CALM::plcRungBegin(int Rung)
{
	swMain->WriteLine("PlcRungBegin {0}", Rung);
}

void PLC_COMPILER_CALM::plcRungEnd(int Rung)
{
	swMain->WriteLine("PlcRungEND {0}", Rung);
}

void PLC_COMPILER_CALM::CalmHeader(TextWriter^ sw)
{
	sw->WriteLine(".PROC\t{0}", GetValue("PROC"));
	sw->WriteLine(".REF\t{0}", GetValue("PROC"));
	DumpIOTable(sw);
	sw->WriteLine("");
	sw->WriteLine(".LIST FALSE");
	sw->WriteLine(".INS Commons.ASI");
	sw->WriteLine("InitCommons \"16F877\"");
	sw->WriteLine("");
	sw->WriteLine(".INS PLC.ASI");
	sw->WriteLine(".INS vmMachine.ASI");
	sw->WriteLine(".ENDLIST");
}

void PLC_COMPILER_CALM::CalmBeforeCompileCode(TextWriter^ sw)
{
	sw->WriteLine("");		
	sw->WriteLine("\\prog:==> PLC Boot section <===");		
	sw->WriteLine("\t\tplcBoot");

	sw->WriteLine("");		
	sw->WriteLine("\\prog:==> PLC ISR section <===");		
	sw->WriteLine("\t\tplcISR");

	sw->WriteLine("");		
	sw->WriteLine("\\prog:==> PLC Main section <===");		
	sw->WriteLine("PlcMAIN:");
	sw->WriteLine("\t\tLongCall plcINIT");		
	sw->WriteLine("PlcLoop:\t\tLongCall plcTASK");		
	sw->WriteLine("\t\tLongJump PlcLoop");		

	sw->WriteLine("");		
	sw->WriteLine("\\prog:==> PLC Task section <===");		
}

void PLC_COMPILER_CALM::CalmAfterCompileCode(TextWriter^ sw)
{
	sw->WriteLine("");	
	sw->WriteLine("\t\tplcAllRungsDone");
	sw->WriteLine("");	
	sw->WriteLine("");		
	sw->WriteLine("\\prog:==> PLC Init section <===");		
	plcGenerateReset();
	swMain->WriteLine("");
	swMain->WriteLine("\t\t;Init PLC Timers");
	swMain->WriteLine("\t\tplcInitTimer2 {0}, {1}, {2}, {3}, {4}", 
		GetIntValue("TimerPeriod"), GetIntValue("mcuClock"), 16, 125, 5);
	sw->WriteLine("\t\tRet");		

	swMain->WriteLine("");
	sw->WriteLine("\\prog:==> Include Libraries <===");		
//	sw->WriteLine(".LIST FALSE");		 		
	sw->WriteLine(".INS vmMachine.ASM");
//	sw->WriteLine(".ENDLIST");
}
