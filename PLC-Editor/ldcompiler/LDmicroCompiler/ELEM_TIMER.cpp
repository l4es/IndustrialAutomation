#include "StdAfx.h"
#include "ELEM.h"


ELEM_TIMER::ELEM_TIMER(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	name = data[1];
	delay = Convert::ToInt32(data[2]);
}

void ELEM_TIMER::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine("{0} {1} {2} {3}", Indent, Marker(), name, delay);
}

int ELEM_TIMER::Period(void)
{
	// return computed period = delay / cycleTime;
	if (plc->CycleTime() != 0) 
		return delay / plc->CycleTime();
	else
		return 0;
}

void ELEM_TIMER::CompilePass1(PLC_COMPILER^ Compiler) 
{
	if (Period() < 256)
		Compiler->AllocateVar(name, 8);
	else
		Compiler->AllocateVar(name, 16);
	Compiler->PresetVar(name, 0, "");
}




ELEM_RTO::ELEM_RTO(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_TIMER(APlc, AParent, str)
{
	array<String^>^ data;
	data = str->Split(' ');
	name = data[1];
	delay = Convert::ToInt32(data[2]);
}



void ELEM_RTO::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	String^ labelElapsed = Compiler->AllocateLabelOneShot("RTO_Elapsed");
	String^ labelEnd = Compiler->AllocateLabelOneShot("RTO_End");
	Compiler->vmComment("");
	Compiler->vmComment(Marker() + String::Format(" {0} delay={1} period={2}", name, delay, Period()) );
	Compiler->vmLoadA(Period(),"inverted");
	Compiler->vmLoadB(name);
	Compiler->vmSub("", String::Format("inverted : {0} <= {1}", name, Period()));
	Compiler->vmSkipIfFlagSet("GE", "is elapsed ?"); 
	Compiler->vmGoto(labelElapsed, "yes");
	Compiler->vmSelTmr("PlcTimerTicks");
	Compiler->vmSkipIfBitClear(StateInOut, "not elapsed");
	Compiler->vmIncTmr(name);
	Compiler->vmClearBit(StateInOut);
	Compiler->vmGoto(labelEnd);
	Compiler->vmLabel(labelElapsed);
	Compiler->vmSetBit(StateInOut, "elapsed, turn ON"); 
	Compiler->vmLabel(labelEnd);
}

void ELEM_RTO::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} delay={1} period={2}", name, delay, Period()));
	Compiler->AddAlias("RTO", Compiler->VarSyntaxName(name));
	Compiler->AddAlias("Period", String::Format("{0}", Period()));
	Compiler->AddAlias("$StateInOut", StateInOut);

	Compiler->OutputC("if ( RTO < Period ) {",								"\t// if ( RTO < Period ) ");
	Compiler->OutputC("\tif ( GetBit($StateInOut) ) RTO += plcTimerTicks;",	"\t// if($StateInOut) RTO = RTO+plcTimerTicks");
	Compiler->OutputC("\t\tClearBit($StateInOut);",							"\t// $StateInOut = false");
	Compiler->OutputC("} else");
	Compiler->OutputC("\t\tSetBit($StateInOut);",							"\t// $StateInOut = true");
	Compiler->ClearAliases();
	Compiler->End(Marker(), name);
}


ELEM_TON::ELEM_TON(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_TIMER(APlc, AParent, str)
{
	array<String^>^ data;
	data = str->Split(' ');
	name = data[1];
	delay = Convert::ToInt32(data[2]);
}



void ELEM_TON::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	String^ label1 = Compiler->AllocateLabelOneShot("TON_Lbl");
	String^ labelEnd = Compiler->AllocateLabelOneShot("TON_End");

	Compiler->vmComment("");
	Compiler->vmComment(Marker() + String::Format(" {0} delay={1} period={2}", name, delay, Period()) );
	Compiler->vmSkipIfBitSet(StateInOut);
	Compiler->vmGoto(label1);
	Compiler->vmLoadA(name);
	Compiler->vmLoadB(Period());
	Compiler->vmSub("");
	Compiler->vmSkipIfFlagSet("GE");
	Compiler->vmGoto(labelEnd);
	Compiler->vmSelTmr("PlcTimerTicks");
	Compiler->vmIncTmr(name);
	Compiler->vmClearBit(StateInOut);
	Compiler->vmGoto(labelEnd);
	Compiler->vmLabel(label1);
	Compiler->vmClear(name);
	Compiler->vmLabel(labelEnd);
}

void ELEM_TON::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} delay={1} period={2}", name, delay, Period()));

	Compiler->AddAlias("TON", Compiler->VarSyntaxName(name));
	Compiler->AddAlias("Period", String::Format("{0}", Period()));
	Compiler->AddAlias("$StateInOut", StateInOut);


	Compiler->OutputC("if ( GetBit($StateInOut) ) {",	"\t// if ($StateInOut)");
	Compiler->OutputC("\tif ( TON < Period ) {",		"\t// if (TON < Period)");
	Compiler->OutputC("\t\tTON += plcTimerTicks;",		"\t// TON = TON + plcTimerTicks");
	Compiler->OutputC("\t\tClearBit($StateInOut);",		"\t// $StateInOut = false");
	Compiler->OutputC("\t}");
	Compiler->OutputC("} else");
	Compiler->OutputC("\tTON = 0;",						"\t// TON = 0");
	Compiler->ClearAliases();
	Compiler->End(Marker(), name);
}



ELEM_TOF::ELEM_TOF(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_TIMER(APlc, AParent, str)
{
	array<String^>^ data;
	data = str->Split(' ');
	name = data[1];
	delay = Convert::ToInt32(data[2]);
	antiGlitchName = "$"+name+"_antiglitch";
}

void ELEM_TOF::CompilePass1(PLC_COMPILER^ Compiler) 
{
	ELEM_TIMER::CompilePass1(Compiler); 
	Compiler->AllocateBit(antiGlitchName);
}



void ELEM_TOF::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	Compiler->vmComment("");
	Compiler->vmComment(Marker() + String::Format(" {0} delay={1} period={2}", name, delay, Period()) );
    // All variables start at zero by default, so by default the
    // TOF timer would start out with its output forced HIGH, until
    // it finishes counting up. This does not seem to be what
    // people expect, so add a special case to fix that up.
	String^ label1 = Compiler->AllocateLabelOneShot("TOF_Lbl");
	String^ label2 = Compiler->AllocateLabelOneShot("TOF_End");
	Compiler->AllocateBit(antiGlitchName);
	Compiler->vmWarning("replace antiGlitch with vmInitVar()");
	Compiler->vmSkipIfBitClear(antiGlitchName);
	Compiler->vmGoto(label1);
	Compiler->vmLoadR(Period());
	Compiler->vmStoreR(name);
	Compiler->vmLabel(label1);
	Compiler->vmSetBit(antiGlitchName);
	Compiler->vmSkipIfBitClear(StateInOut);
	Compiler->vmGoto(label2);
	Compiler->vmLoadA(name);
	Compiler->vmLoadB(Period());
	Compiler->vmSub("");
	Compiler->vmSkipIfFlagSet("LE");
	Compiler->vmGoto(label2);
	Compiler->vmSelTmr("PlcTimerTicks");
	Compiler->vmIncTmr(name);
	Compiler->vmSetBit(StateInOut);
	Compiler->vmLabel(label2);
	Compiler->vmClear(name);
}

void ELEM_TOF::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), String::Format("{0} delay={1} period={2}", name, delay, Period()));

	Compiler->AddAlias("TOF", Compiler->VarSyntaxName(name));
	Compiler->AddAlias("Period", String::Format("{0}", Period()));
	Compiler->AddAlias("$StateInOut", StateInOut);
	Compiler->AddAlias("$AntiGlitch",antiGlitchName);

	Compiler->OutputC("if ( !GetBit($AntiGlitch) ) TOF = Period;",		"\t// if (!$AntiGlitch) TOF = Period"); 
	Compiler->OutputC("SetBit($AntiGlitch);",							"\t// $AntiGlitch = true");
	Compiler->OutputC("if ( !GetBit($StateInOut) ) {",					"\t// if(!$StateInOut)");
	Compiler->OutputC("\tif ( TOF < Period ) {",						"\t// if (TOF < Period)");
	Compiler->OutputC("\t\tTOF += plcTimerTicks;",						"\t// TOF = TOF+plcTimerTicks");
	Compiler->OutputC("\t\tSetBit($StateInOut);",						"\t// $StateInOut = true");
	Compiler->OutputC("\t}");
	Compiler->OutputC("} else");
	Compiler->OutputC("\tTOF = 0;",										"\t// TOF = 0");
	Compiler->ClearAliases();
	Compiler->End(Marker(), name);
}
