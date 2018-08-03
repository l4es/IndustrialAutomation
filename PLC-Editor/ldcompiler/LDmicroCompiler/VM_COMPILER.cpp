#include "StdAfx.h"
#include "VM_COMPILER.h"

VM_OP::VM_OP(int op, int bin, String^ genLine)
{
	fop = op;
	fbin = bin;
	fgenLine = genLine;
	fused = false;
}

bool VM_OP::Match(int op)
{
	return (fop == op);
}

void VM_OP::Generate(VM_COMPILER^ vmc, TextWriter^ sw, array<System::String ^> ^args)
{
	String^ argsFormat = "";
	int i = 0;
	while (i < args->Length) {
		argsFormat = argsFormat->Clone()+", {"+ i.ToString() +"}";
		i++;
	}

	if (vmc->NeedsToPopLabel) {
		String^ label = vmc->PopLabel();
		sw->Write("{0}: {1:x4}", label, fbin);
	} else 
		sw->Write("          {0:x4}", fbin);

	sw->Write(argsFormat, args);
	sw->Write(" ; ");
	sw->WriteLine(fgenLine, args);
}



// -----------------------------------------------------------------------------------------------------------------


VM_COMPILER::VM_COMPILER(TextWriter^ swMain)
{
	this->swMain = swMain;
	ConvertIfElse = true; // convert IF ELSE with SKIP and GOTO intructions
	// always provide -1 code for unknown OP
	mnemo.Add(gcnew VM_OP(-1, -1, "*!!! Unknown op !!!* {0} {1} {2} {3} {4}"));
	// always provide -2 code for label
	mnemo.Add(gcnew VM_OP(INT_LABEL, INT_LABEL, "{0}:"));
	swMain->WriteLine("; LDmicro Compiler version {0}", LDversion());
}

int  VM_COMPILER::LDversion(void)
{
	return 2;
}


void VM_COMPILER::Mnemo0_1(void)
{
	mnemo.Add(gcnew VM_OP(INT_SET_BIT, INT_SET_BIT, "SETBIT {0}"));
	mnemo.Add(gcnew VM_OP(INT_CLEAR_BIT, INT_CLEAR_BIT, "CLEARBIT {0}"));
	mnemo.Add(gcnew VM_OP(INT_COPY_BIT, INT_COPY_BIT, "COPYBIT {0}, {1}"));
	mnemo.Add(gcnew VM_OP(INT_BIT_IF_SET, INT_BIT_IF_SET, "IF {0} THEN "));
	mnemo.Add(gcnew VM_OP(INT_BIT_IF_CLEAR, INT_BIT_IF_CLEAR, "IF NOT {0} THEN"));

	mnemo.Add(gcnew VM_OP(INT_INCREMENT_VARIABLE, INT_INCREMENT_VARIABLE, "INC {0}"));
	mnemo.Add(gcnew VM_OP(INT_SET_VARIABLE_TO_VARIABLE, INT_SET_VARIABLE_TO_VARIABLE, "MOVE {0}, {1}"));
	mnemo.Add(gcnew VM_OP(INT_SET_VARIABLE_TO_LITERAL, INT_SET_VARIABLE_TO_LITERAL, "MOVE {0}, {1}"));

	mnemo.Add(gcnew VM_OP(INT_ELSE, INT_ELSE, "ELSE"));
	mnemo.Add(gcnew VM_OP(INT_END_IF, INT_END_IF, "END IF"));

	mnemo.Add(gcnew VM_OP(INT_GOTO, INT_GOTO, "GOTO {0}"));
	mnemo.Add(gcnew VM_OP(INT_SKIP_IF_BIT_IS_SET, INT_SKIP_IF_BIT_IS_SET, "SkipIfBitSet {0}"));
	mnemo.Add(gcnew VM_OP(INT_SKIP_IF_BIT_IS_CLEAR, INT_SKIP_IF_BIT_IS_CLEAR, "SkipIfBitClear {0}"));
}

void VM_COMPILER::Mnemo2_0(void)
{
}

// return op data, else unknow op
VM_OP^ VM_COMPILER::FindOp(int op)
{
	VM_OP^ unknown;
	for each(VM_OP^ item in mnemo) {
		if (item->Match(-1)) unknown = item;
		if (item->Match(op)) return item;
	}
	return unknown;
}


void VM_COMPILER::Op(int op, array<System::String ^> ^args)
{
	VM_OP^ vmOP = FindOp(op);
	if (vmOP->Match(-1))
		Console::WriteLine("* Unknown : op = {0}", op);
	else
		vmOP->Generate(this, Console::Out, args);
}



bool VM_COMPILER::RequestOP(int op)
{
	VM_OP^ vmOP = FindOp(op);
	if (vmOP->Match(-1)) return false;
	return true;
}

void VM_COMPILER::Oops(String^ msg)
{
	Console::WriteLine("Oops : {0}", msg);
}

void VM_COMPILER::Indent(int count)
{
	indentCount = indentCount + count;
	if (indentCount < 0) indentCount = 0;
}

String^ VM_COMPILER::GenSymbOneShot(void)
{
	return "$oneShot_"+(GenSymbCountOneShot++).ToString("x");
}

String^ VM_COMPILER::GenLabelOneShot(String^ prefix)
{
	String^ label = prefix+(LabelCountOneShot++).ToString("x");
	return label;
}



String^ VM_COMPILER::PopLabel(void)
{
	String^ label;
	NeedsToPopLabel = false;
	if (if_labels.Count != 0) {
	  label = if_labels[if_labels.Count-1];
	  if_labels.Remove(label);
	} else
      label = "*ERROR* nested if else endif";
	return label;
}

void VM_COMPILER::vmWriteLine(String^ mnemonic, String^ argument, String^ comment)
{
	if (comment->Empty)
		swMain->WriteLine("\t\t{0}\t{1}", mnemonic, argument);
	else
		swMain->WriteLine("\t\t{0}\t{1}\t\t{2}", mnemonic, argument, comment);
}

void VM_COMPILER::vmComment(String^ msg)
{
	swMain->WriteLine("; {0}", msg);
}

void VM_COMPILER::vmWarning(String^ msg)
{
	Console::WriteLine("Warning : {0}", msg);
	swMain->WriteLine("; !!! Warning : {0}", msg);
}

void VM_COMPILER::vmError(String^ msg)
{
	Console::WriteLine("*ERROR* {0}", msg);
	swMain->WriteLine("!!! Error : {0}", msg);
}



void VM_COMPILER::vmInitVar(String^ varName, int value, String^ comment)
{
	vmWarning("This code will be called once PLC run");
}

void VM_COMPILER::vmLabel(String^ label)
{
	swMain->Write("{0}:", label);
}


void VM_COMPILER::vmSetBit(String^ bitName, String^ comment)
{
//	vmWriteLine("vmSetBit"+BitSuffix(bitName), BitSymbol(bitName), comment);
}

void VM_COMPILER::vmSetBit(String^ bitName)
{
	vmSetBit(bitName, "");
}


void VM_COMPILER::vmClearBit(String^ bitName, String^ comment)
{
//	vmWriteLine("vmClearBit"+BitSuffix(bitName), BitSymbol(bitName), comment);
}

void VM_COMPILER::vmClearBit(String^ bitName)
{
	vmClearBit(bitName, "");
}

void VM_COMPILER::vmGetBit(String^ bitName, String^ comment)
{
//	vmWriteLine("vmGetBit"+BitSuffix(bitName), BitSymbol(bitName), comment);
}

void VM_COMPILER::vmGetBit(String^ bitName)
{
	vmGetBit(bitName, "");
}

void VM_COMPILER::vmPutBit(String^ bitName, String^ comment)
{
//	vmWriteLine("vmPutBit"+BitSuffix(bitName), BitSymbol(bitName), comment);
}

void VM_COMPILER::vmPutBit(String^ bitName)
{
	vmPutBit(bitName, "");
}


void VM_COMPILER::vmSkipIfBitClear(String^ bitName, String^ comment)
{
//	vmWriteLine("vmSkipIfBitClear"+BitSuffix(bitName), BitSymbol(bitName), comment);
}

void VM_COMPILER::vmSkipIfBitClear(String^ bitName)
{
	vmSkipIfBitClear(bitName, "");
}

void VM_COMPILER::vmSkipIfBitSet(String^ bitName, String^ comment)
{
//	vmWriteLine("vmSkipIfBitSet"+BitSuffix(bitName), BitSymbol(bitName), comment);
}

void VM_COMPILER::vmSkipIfBitSet(String^ bitName)
{
	vmSkipIfBitSet(bitName, "");
}


void VM_COMPILER::vmClear(System::String^ varDest, String^ comment)
{
//	vmWriteLine("vmClear"+VarSuffix(varDest), VarSymbol(varDest), comment);
}

void VM_COMPILER::vmClear(System::String^ varDest)
{
	vmClear(varDest, "");
}

void VM_COMPILER::vmLoadA(System::String^ varSrc, String^ comment)
{
	vmWriteLine("vmLoadA", varSrc, comment);
}

void VM_COMPILER::vmLoadA(int value, String^ comment)
{
	vmWriteLine("vmLoadLiteralA", value.ToString(), comment);
}

void VM_COMPILER::vmLoadA(System::String^ varSrc)
{
	vmLoadA(varSrc, "");
}

void VM_COMPILER::vmLoadA(int value)
{
	vmLoadA(value, "");
}

void VM_COMPILER::vmLoadB(System::String^ varSrc, String^ comment)
{
	vmWriteLine("vmLoadB", varSrc, comment);
}

void VM_COMPILER::vmLoadB(int value, String^ comment)
{
	vmWriteLine("vmLoadLiteralB", value.ToString(), comment);
}

void VM_COMPILER::vmLoadB(System::String^ varSrc)
{
	vmLoadB(varSrc, "");
}

void VM_COMPILER::vmLoadB(int value)
{
	vmLoadB(value, "");
}


void VM_COMPILER::vmLoadR(System::String^ varSrc, String^ comment)
{
	vmWriteLine("vmLoadR", varSrc, comment);
}

void VM_COMPILER::vmLoadR(int value, String^ comment)
{
	vmWriteLine("vmLoadLiteralR", value.ToString(), comment);
}

void VM_COMPILER::vmLoadR(System::String^ varSrc)
{
	vmLoadR(varSrc, "");
}

void VM_COMPILER::vmLoadR(int value)
{
	vmLoadR(value, "");
}

void VM_COMPILER::vmStoreR(String^ varDest, String^ comment)
{
	vmWriteLine("vmStoreR", varDest, comment);
}

void VM_COMPILER::vmStoreR(String^ varDest)
{
	vmStoreR(varDest, "");
}

void VM_COMPILER::vmSub(String^ varDest, String^ comment)
{
	vmWriteLine("vmSub", varDest, comment);
}

void VM_COMPILER::vmSub(String^ varDest)
{
	vmSub(varDest, "");
}

void VM_COMPILER::vmSkipIfFlagSet(String^ flags, String^ comment)
{
	vmWriteLine("vmSkipIfFlagSet", flags, comment);
}

void VM_COMPILER::vmSkipIfFlagSet(String^ flags)
{
	vmSkipIfFlagSet(flags, "");
}

void VM_COMPILER::vmGoto(String^ label, String^ comment)
{
	vmWriteLine("vmGoto", label, comment);
}

void VM_COMPILER::vmGoto(String^ label)
{
	vmGoto(label, "");
}

void VM_COMPILER::vmInc(String^ varDest, String^ comment)
{
	vmWriteLine("vmInc", varDest, comment);
}

void VM_COMPILER::vmInc(String^ varDest)
{
	vmInc(varDest, "");
}

void VM_COMPILER::vmIncTmr(String^ varDest, String^ comment)
{
	vmWriteLine("vmIncTmr", varDest, comment);
}

void VM_COMPILER::vmIncTmr(String^ varDest)
{
	vmIncTmr(varDest, "");
}

void VM_COMPILER::INCREMENT(String^ varName)
{
	if (IsNumber(varName)) {
		vmError(String::Format("{0} is not a variable", varName));
		return;
	}
	if (!RequestOP(INT_INCREMENT_VARIABLE)) {Oops("INCREMENT required"); return;}
	array<String^> ^args = {varName};
	Op(INT_INCREMENT_VARIABLE, args);
}

void VM_COMPILER::ASSIGN(String^ varDest, String^ src)
{
	if (IsNumber(varDest)) {
		vmError(String::Format("{0} is not a variable", varDest));
		return;
	}
	if (IsNumber(src)) ASSIGN(varDest, Convert::ToInt32(src));
	else {
	  if (!RequestOP(INT_SET_VARIABLE_TO_VARIABLE)) {Oops("SET_VARIABLE_TO_VARIABLE required"); return;}
      array<String^> ^args = {varDest, src};
	  Op(INT_SET_VARIABLE_TO_VARIABLE, args);
	}
}

void VM_COMPILER::ASSIGN(String^ varDest, int value)
{
	if (IsNumber(varDest)) {
		vmError(String::Format("{0} is not a variable", varDest));
		return;
	}
	if (!RequestOP(INT_SET_VARIABLE_TO_LITERAL)) {Oops("SET_LITERAL_TO_VARIABLE required"); return;}
	array<String^> ^args = {varDest, value.ToString()};
	Op(INT_SET_VARIABLE_TO_LITERAL, args);
}


void VM_COMPILER::IF_LES(String^ op1, String^ op2)
{
	vmError(String::Format("Must implement  IF {0} < {1}", op1, op2));
}

void VM_COMPILER::IF_LES(String^ op1, int op2)
{
	vmError(String::Format("Must implement  IF {0} < {1}", op1, op2));
}
