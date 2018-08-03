#include "StdAfx.h"
#include "ELEM.h"
#include "mcutable.h"

int mcuOldFormat0_1; // for converting pin to port:#bit while .ld file reading

void FindOldMcu(String^ name)
{
	int i;
	bool identical;
	mcuOldFormat0_1 = NUM_SUPPORTED_MCUS;
	while (mcuOldFormat0_1-- >= 0) {
		i = 0;
		identical = true;
		while ((i < name->Length) && identical) {
			if (name[i] != SupportedMcus[mcuOldFormat0_1].mcuName[i]) identical = false;
			i++;
		}
		if (identical) return;
	}
}

ELEM_IO_LIST::ELEM_IO_LIST(PLC^ APlc, ELEM^ AParent) : ELEM_LIST(APlc, AParent)
{
}



ELEM_TARGET::ELEM_TARGET(PLC^ APlc, ELEM^ AParent) : ELEM(APlc, AParent)
{
	io_list = gcnew ELEM_IO_LIST(APlc, AParent); 
}

void ELEM_TARGET::LoadFromTextStream(StreamReader^ sr, int &LineCounter)
{
   // this routine can load LDmicro0.1 file format

   String^ s = "";
   while ( s = sr->ReadLine() )
   {
      LineCounter++;
	  s = s->Trim();
	  if(s->CompareTo("IO LIST")==0) {io_list->LoadFromTextStream(sr, LineCounter); continue;} // LDmicro0.1
	  else if(s->CompareTo("IO_LIST")==0) {io_list->LoadFromTextStream(sr, LineCounter); continue;} // LDmicro2.0
	  else if(s->StartsWith("MICRO=")) {
		  micro=s->Substring(s->IndexOf("=")+1);
		  FindOldMcu(micro);
		  continue;}
	  else if(s->StartsWith("CYCLE=")) {cycleTime=Convert::ToUInt32( s->Substring(s->IndexOf("=")+1) ); continue;}
	  else if(s->StartsWith("CRYSTAL=")) {mcuClock=Convert::ToUInt32( s->Substring(s->IndexOf("=")+1) ); continue;}
	  else if(s->StartsWith("BAUD=")) {baudRate=Convert::ToUInt32( s->Substring(s->IndexOf("=")+1) ); continue;}
	  else if(s->StartsWith("COMPILED=")) {compiled=s->Substring(s->IndexOf("=")+1); continue;}
	  else if(s->CompareTo("END")==0) break; // LDmicro2.0
	  else if(s->CompareTo("PROGRAM")==0) break; // LDmicro0.1 
      else 
        if (s->Length!=0) Console::WriteLine( "Ignored : " + s );
   }
}

void ELEM_TARGET::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
//  ELEM::WriteToTextStream(sw, Indent); 
  // LDmicro2.0 file format
	sw->WriteLine(Indent+Marker());
	sw->WriteLine(Indent+"    MICRO="+micro);
	sw->WriteLine(Indent+"    CYCLE={0}",cycleTime);
	sw->WriteLine(Indent+"    CRYSTAL={0}", mcuClock);
	sw->WriteLine(Indent+"    BAUD={0}", baudRate);
	sw->WriteLine(Indent+"    COMPILED="+compiled);
	io_list->WriteToTextStream(sw, Indent+"    ");
  sw->WriteLine(Indent+"END"); // LDmicro2.0 file format
}

void ELEM_TARGET::CompilePass1VM(PLC_COMPILER_VM^ Compiler)
{
	array<String^>^ data;
	if (!String::IsNullOrEmpty(micro))
		data = micro->Split(' ');
	else {
		micro = "No target selected,XXXprocessor not selected";
		data = micro->Split(',');
	}
	
	Compiler->SetValue("MCU", micro, "Target MCU");
	Compiler->SetValue("PROC", data[1]->Substring(3), "Target processor");
	Compiler->SetValue("TimerPeriod", cycleTime, "PLC Timer Period [us]");
	Compiler->SetValue("mcuClock", mcuClock, "MCU Clock [MHz]");
	Compiler->SetValue("baudRate", baudRate, "Baud Rate");
	io_list->CompilePass1VM(Compiler);
}

void ELEM_TARGET::CompilePass1C(PLC_COMPILER_C^ Compiler)
{
	array<String^>^ data;
	if (!String::IsNullOrEmpty(micro))
		data = micro->Split(' ');
	else {
		micro = "No target selected,XXXprocessor not selected";
		data = micro->Split(',');
	}
	Compiler->SetValue("MCU", micro, "Target MCU");
	Compiler->SetValue("PROC", data[1]->Substring(3), "Target processor");
	Compiler->SetValue("TimerPeriod", cycleTime, "PLC Timer Period [us]");
	Compiler->SetValue("mcuClock", mcuClock, "MCU Clock [MHz]");
	Compiler->SetValue("baudRate", baudRate, "Baud Rate");
	io_list->CompilePass1C(Compiler);
}


void ELEM_TARGET::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	io_list->CompilePass2VM(Compiler, StateInOut);
}

void ELEM_TARGET::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->OutputC("/**");
	Compiler->OutputC(String::Format(" * MICRO    = {0}", micro));
	Compiler->OutputC(String::Format(" * CYCLE    = {0}", cycleTime));
	Compiler->OutputC(String::Format(" * CRYSTAL  = {0}", mcuClock));
	Compiler->OutputC(String::Format(" * BAUD     = {0}", baudRate));
	Compiler->OutputC(String::Format(" * COMPILED = {0}", compiled));
	Compiler->OutputC("*/");
	Compiler->OutputC("");
	Compiler->OutputC("// IO list");
	io_list->CompilePass2C(Compiler, StateInOut);
}

ELEM_IO::ELEM_IO(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM(APlc, AParent)
{
	array<String^>^ data;
	data = str->Split(' ');
	name = data[1];
	port = data[2];
	bit = System::Convert::ToInt32(data[3]);
    pullup = (data[4]->Contains("1") || (data[4]->IndexOf("PullUp", StringComparison::InvariantCultureIgnoreCase) != -1));
}

void ELEM_IO::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine(Indent+Marker()+" "+name+" {0} {1} {2}", port, bit, BoolToString(pullup, "PullUp", "NoPullUp"));
}

void ELEM_IO::CompilePass1(PLC_COMPILER^ Compiler)
{
	Compiler->AddBit(name, PLC_BIT_IO, String::Format("{0}", port), name, bit); 
}

void ELEM_IO::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
}


void ELEM_IO::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->OutputC(String::Format("#define {0} PORT{1}bits.R{1}{2}", name, port[port->Length-1], bit));
}



ELEM_IO_INPUT::ELEM_IO_INPUT(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_IO(APlc, AParent, str)
{
	ELEM_IO(APlc, AParent, str);
}

void ELEM_IO_INPUT::CompilePass1C(PLC_COMPILER_C^ Compiler)
{
	Compiler->AddIO(Marker(), name, port, bit, PLC_IO_INPUT);
}

void ELEM_IO_INPUT::CompilePass1VM(PLC_COMPILER_VM^ Compiler)
{
	Compiler->AddIO(Marker(), name, port, bit, PLC_IO_INPUT);
}


ELEM_IO_OUTPUT::ELEM_IO_OUTPUT(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_IO(APlc, AParent, str)
{
	ELEM_IO(APlc, AParent, str);
}

void ELEM_IO_OUTPUT::CompilePass1C(PLC_COMPILER_C^ Compiler)
{
	Compiler->AddIO(Marker(), name, port, bit, PLC_IO_OUTPUT);
}

void ELEM_IO_OUTPUT::CompilePass1VM(PLC_COMPILER_VM^ Compiler)
{
	Compiler->AddIO(Marker(), name, port, bit, PLC_IO_OUTPUT);
}

ELEM_IO_ADC::ELEM_IO_ADC(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_IO(APlc, AParent, str)
{
	//ELEM_ADC(APlc, AParent, str);
}

void ELEM_IO_ADC::CompilePass1C(PLC_COMPILER_C^ Compiler)
{
//	Compiler->AddIO(Marker(), name, port, bit, PLC_IO_ADC);
}

void ELEM_IO_ADC::CompilePass1VM(PLC_COMPILER_VM^ Compiler)
{
//	Compiler->AddIO(Marker(), name, port, bit, PLC_IO_ADC);
}

ELEM_IO_RELAY::ELEM_IO_RELAY(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_IO(APlc, AParent, str)
{
	ELEM_IO(APlc, AParent, str);
}

ELEM_IO_RCIN::ELEM_IO_RCIN(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_IO(APlc, AParent, str)
{
	ELEM_IO(APlc, AParent, str);
}

ELEM_IO_RCOUT::ELEM_IO_RCOUT(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_IO(APlc, AParent, str)
{
	ELEM_IO(APlc, AParent, str);
}

ELEM_TARGET_LIST::ELEM_TARGET_LIST(PLC^ APlc, ELEM^ AParent) : ELEM_LIST(APlc, AParent)
{
	selected = 0;
}

void ELEM_TARGET_LIST::LoadFromTextStream(StreamReader^ sr, int &LineCounter)
{
   ELEM_TARGET^ newTarget;
   String^ s = "";
   while ( s = sr->ReadLine() )
   {
      LineCounter++;
	  s = s->Trim();
	  if(s->CompareTo("TARGET")==0)
	  {
        newTarget = gcnew ELEM_TARGET(plc, this);
		newTarget->LoadFromTextStream(sr, LineCounter);
		Add(newTarget);
	  }
	  else if(s->StartsWith("SELECTED=")) {selected=Convert::ToUInt32( s->Substring(s->IndexOf("=")+1) ); continue;}
	  else if(s->CompareTo("END")==0) break; 
   }
}


void ELEM_IO_LIST::OldFormat0_1_IO(String^ str)
{
	int		i;
	String^	name;
	int		pin;
	String^	port;
	int		bit;
	int		adc;
//ELEM_IO^ newio;

	name	= str->Substring(0, str->IndexOf(" "));
	pin		= System::Convert::ToInt32(str->Substring(str->LastIndexOf(" "))); // LDmicro0.1 format
	port	= "UnknowPort";
	bit		= 0;
	if (mcuOldFormat0_1 >=0 ) 
	{
		// Find Port & bit
		i = SupportedMcus[mcuOldFormat0_1].pinCount;
		while (i-- >= 0) {if (SupportedMcus[mcuOldFormat0_1].pinInfo[i].pin == pin) break;}
		if (i >= 0) 
		{
			port = System::String::Format("Port{0}", Char(SupportedMcus[mcuOldFormat0_1].pinInfo[i].port));
			bit = SupportedMcus[mcuOldFormat0_1].pinInfo[i].bit;
		}
		// Find ADC channel
		i = SupportedMcus[mcuOldFormat0_1].adcCount;
		while (i-- >= 0) {if (SupportedMcus[mcuOldFormat0_1].adcInfo[i].pin == pin) break;}
		if (i >= 0) adc = SupportedMcus[mcuOldFormat0_1].adcInfo[i].muxRegValue;
	}

	// Dertermine IO type from name 

	if (name->StartsWith("YRCOUT", StringComparison::InvariantCultureIgnoreCase)) 
		Add(gcnew ELEM_IO_RCOUT(plc, this, String::Format("RCOUT {0} {1} {2} 0", name, port, bit)));
	else if (name->StartsWith("XRCIN", StringComparison::InvariantCultureIgnoreCase)) 
		Add(gcnew ELEM_IO_RCIN(plc, this, String::Format("RCIN {0} {1} {2} 0", name, port, bit)));
	else
		switch (str[0]) {
			case 'R' : Add(gcnew ELEM_IO_RELAY(plc, this, String::Format("RELAY {0} {1} {2} 0", name, port, bit))); break;
			case 'Y' : Add(gcnew ELEM_IO_OUTPUT(plc, this, String::Format("OUTPUT {0} {1} {2} 0", name, port, bit)));	break;
			case 'X' : Add(gcnew ELEM_IO_INPUT(plc, this, String::Format("INPUT {0} {1} {2} 0", name, port, bit)));	break;
			case 'A' : Add(gcnew ELEM_IO_ADC(plc, this, String::Format("ADC {0} {1} {2} 0 {3}", name, port, bit, adc)));	break;
			default: {
				//	throw ApplicationException("Unknow first char in IO name");
				return;
			}
		}
/*  newio->name = name;
  newio->pin = System::Convert::ToInt32(str->Substring(str->LastIndexOf(" "))); // LDmicro0.1 format
  if (mcuOldFormat0_1 >=0 ) {
	  i = SupportedMcus[mcuOldFormat0_1].pinCount;
	  while (i-- >= 0) {if (SupportedMcus[mcuOldFormat0_1].pinInfo[i].pin == newio->pin) break;}
	  if (i >= 0) {
		  newio->port = System::String::Format("Port{0}", Char(SupportedMcus[mcuOldFormat0_1].pinInfo[i].port));
		  newio->bit = SupportedMcus[mcuOldFormat0_1].pinInfo[i].bit;
	  }
  }
  Add(newio); */
}


void ELEM_IO_LIST::LoadFromTextStream(StreamReader^ sr, int &LineCounter)
{
   String^ s = "";
   while ( s = sr->ReadLine() )
   {
      LineCounter++;
	  s = s->Trim();
      if (s->IndexOf(" at ")!=-1) OldFormat0_1_IO(s); // Old LDmicro0.1 file format
	  else if(s->StartsWith("INPUT")) Add(gcnew ELEM_IO_INPUT(plc, this, s));
	  else if(s->StartsWith("OUTPUT")) Add(gcnew ELEM_IO_OUTPUT(plc, this, s));
	  else if(s->StartsWith("RELAY")) Add(gcnew ELEM_IO_RELAY(plc, this, s));
	  else if(s->StartsWith("RCIN")) Add(gcnew ELEM_IO_RCIN(plc, this, s));
	  else if(s->StartsWith("RCOUT")) Add(gcnew ELEM_IO_RCOUT(plc, this, s));
	  else if(s->CompareTo("END")==0) break;
      else 
        if (s->Length!=0) Console::WriteLine( "Ignored : " + s );
   }
}

