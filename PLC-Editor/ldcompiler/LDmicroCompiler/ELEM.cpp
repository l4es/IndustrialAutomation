#include "StdAfx.h"
#include "ELEM.h"


ELEM::ELEM(PLC^ APlc, ELEM^ AParent)
{
	plc = APlc;
	parent = AParent;
}

void ELEM::LoadFromTextStream(StreamReader^ sr, int &LineCounter)
{
   Console::WriteLine( "*ERROR* ELEM LoadFromTextStream : can't be used" );
}

void ELEM::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine(Indent + Marker()+ "*ERROR* ELEM WriteToTextStream : can't be used");
}

void ELEM::CompilePass1(PLC_COMPILER^ Compiler)
{
}

void ELEM::CompilePass1C(PLC_COMPILER_C^ Compiler)
{
	CompilePass1(Compiler);
}

void ELEM::CompilePass1VM(PLC_COMPILER_VM^ Compiler)
{
	CompilePass1(Compiler);
}


void ELEM::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->CompileError(String::Format("ELEM Compile Pass2 not programmed", Marker()));
}

void ELEM::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	Compiler->CompileError(String::Format("ELEM Compile Pass2 not programmed", Marker()));
}


ELEM_LIST::ELEM_LIST(PLC^ APlc, ELEM^ AParent) : ELEM(APlc, AParent)
{ 
	selected = -1;
}


int ELEM_LIST::Count(void)
{
	return items.Count;
}

void ELEM_LIST::Add(ELEM^ item)
{
	items.Add(item);
}

void ELEM_LIST::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine(Indent+Marker());
	if (selected != -1) sw->WriteLine(Indent+"    SELECTED={0}", selected);
	for each(ELEM^ item in items)
	{
		item->WriteToTextStream(sw, Indent+"    ");
	}
	sw->WriteLine(Indent+"END");
}

long ELEM_LIST::CountLeafs(bool ExcludeComments)
{
	long result;
	result = 0;
	for each(ELEM^ item in items)
	{
		result += item->CountLeafs(ExcludeComments);
	}
	return result;
}

void ELEM_LIST::CompilePass1C(PLC_COMPILER_C^ Compiler)
{
	for each(ELEM^ item in items)
	{
		item->CompilePass1C(Compiler);
	}
}

void ELEM_LIST::CompilePass1VM(PLC_COMPILER_VM^ Compiler)
{
	for each(ELEM^ item in items)
	{
		item->CompilePass1VM(Compiler);
	}
}


void ELEM_LIST::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	for each(ELEM^ item in items)
	{
		item->CompilePass2C(Compiler, StateInOut);
	}
}

void ELEM_LIST::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	for each(ELEM^ item in items)
	{
		item->CompilePass2VM(Compiler, StateInOut);
	}
}




ELEM_RUNG::ELEM_RUNG(PLC^ APlc, ELEM^ AParent) : ELEM_LIST(APlc, AParent)
{
}


void ELEM_RUNG::LoadFromTextStream(StreamReader^ sr, int &LineCounter)
{
   String^ s = "";
   ELEM_LIST^ newList;
//   Console::WriteLine( "Rung : LoadFromTextStream" );
   while ( s = sr->ReadLine() )
   {
      LineCounter++;
	  s = s->Trim();
	  if(s->CompareTo("END")==0) break;
	  else if(s->CompareTo("PARALLEL")==0) { 
		  newList = gcnew ELEM_PARALLEL(plc, this);
		  newList->LoadFromTextStream(sr, LineCounter);
		  Add( newList ); 
		  continue;
      } else if(s->CompareTo("SERIES")==0) { 
		  newList = gcnew ELEM_SERIES(plc, this);
		  newList->LoadFromTextStream(sr, LineCounter);
		  Add( newList ); 
		  continue;
      } 
	  else if(s->StartsWith("COMMENT ")) { Add( gcnew ELEM_COMMENT(plc, this, s)); continue;}
	  else if(s->StartsWith("CONTACTS ")) { Add( gcnew ELEM_CONTACTS(plc, this, s)); continue;}
	  else if(s->StartsWith("COIL ")) { Add( gcnew ELEM_COIL(plc, this, s)); continue;}
	  else if(s->StartsWith("TON ")) { Add( gcnew ELEM_TON(plc, this, s)); continue;}
	  else if(s->StartsWith("TOF ")) { Add( gcnew ELEM_TOF(plc, this, s)); continue;}
	  else if(s->StartsWith("RTO ")) { Add( gcnew ELEM_RTO(plc, this, s)); continue;}
	  else if(s->StartsWith("CTU ")) { Add( gcnew ELEM_CTU(plc, this, s)); continue;}
	  else if(s->StartsWith("CTD ")) { Add( gcnew ELEM_CTD(plc, this, s)); continue;}
	  else if(s->StartsWith("CTC ")) { Add( gcnew ELEM_CTC(plc, this, s)); continue;}
	  else if(s->StartsWith("MOVE ")) { Add( gcnew ELEM_MOVE(plc, this, s)); continue;}
	  else if(s->StartsWith("ADD ")) { Add( gcnew ELEM_ADD(plc, this, s)); continue;}
	  else if(s->StartsWith("SUB ")) { Add( gcnew ELEM_SUB(plc, this, s)); continue;}
	  else if(s->StartsWith("MUL ")) { Add( gcnew ELEM_MUL(plc, this, s)); continue;}
	  else if(s->StartsWith("DIV ")) { Add( gcnew ELEM_DIV(plc, this, s)); continue;}
	  else if(s->StartsWith("EQU ")) { Add( gcnew ELEM_EQU(plc, this, s)); continue;}
	  else if(s->StartsWith("GRT ")) { Add( gcnew ELEM_GRT(plc, this, s)); continue;}
	  else if(s->StartsWith("GEQ ")) { Add( gcnew ELEM_GEQ(plc, this, s)); continue;}
	  else if(s->StartsWith("LES ")) { Add( gcnew ELEM_LES(plc, this, s)); continue;}
	  else if(s->StartsWith("LEQ ")) { Add( gcnew ELEM_LEQ(plc, this, s)); continue;}
	  else if(s->StartsWith("NEQ ")) { Add( gcnew ELEM_NEQ(plc, this, s)); continue;}
	  else if(s->StartsWith("RES")) { Add( gcnew ELEM_RES(plc, this, s)); continue;}
	  else if(s->StartsWith("FORMATTED_STRING INLINE")) { Add( gcnew ELEM_INLINE_CODE(plc, this, s)); continue;}	// Catch INLINE code before FORMATED_STRING  
	  else if(s->StartsWith("FORMATTED_STRING")) { Add( gcnew ELEM_FORMATTED_STRING(plc, this, s)); continue;}	  
	  else if(s->CompareTo("OPEN")==0) { Add( gcnew ELEM_OPEN(plc, this, s)); continue;}
	  else if(s->CompareTo("SHORT")==0) { Add( gcnew ELEM_SHORT(plc, this, s)); continue;}
	  else if(s->CompareTo("OSR")==0) { Add( gcnew ELEM_ONE_SHOT_RISING(plc, this, s)); continue;}
	  else if(s->CompareTo("OSF")==0) { Add( gcnew ELEM_ONE_SHOT_FALLING(plc, this, s)); continue;}
	  else if(s->CompareTo("MASTER_RELAY")==0) { Add( gcnew ELEM_MASTER_RELAY(plc, this, s)); continue;}
      else if (s->Length!=0) Console::WriteLine( "Ignored : " + s );
   }
}


void ELEM_RUNG::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	Compiler->vmGetBit(StateInOut);
	Compiler->vmPutBit("$rung_top");
	ELEM_LIST::CompilePass2VM(Compiler, "$rung_top");
}

void ELEM_RUNG::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->AddAlias("$StateInOut", StateInOut);

//	Compiler->OutputC("AssignBit($rung_top, GetBit($StateInOut));",	"\t// $rung_top = $StateInOut");
	ELEM_LIST::CompilePass2C(Compiler, "$rung_top");
}

ELEM_RUNG_INTERNAL::ELEM_RUNG_INTERNAL(PLC^ APlc, ELEM^ AParent) : ELEM_RUNG(APlc, AParent)
{
}

void ELEM_RUNG_INTERNAL::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	// Do nothing
}

void ELEM_RUNG_INTERNAL::LoadFromTextStream(StreamReader^ sr, int &LineCounter)
{
	Console::WriteLine( "{0} Illegal LoadFromTextStream  : Line {1}", Marker(), LineCounter);
}


ELEM_SERIES::ELEM_SERIES(PLC^ APlc, ELEM^ AParent) : ELEM_RUNG(APlc, AParent)
{
}

void ELEM_SERIES::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	Compiler->vmComment("= SERIE =");
	Compiler->Indent(+1);
	ELEM_LIST::CompilePass2VM(Compiler, StateInOut);
	Compiler->Indent(-1);
}

void ELEM_SERIES::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->Begin(Marker(), "");
	Compiler->Indent(+1);
	ELEM_LIST::CompilePass2C(Compiler, StateInOut);
	Compiler->Indent(-1);
	Compiler->End(Marker(), "");
}


ELEM_PARALLEL::ELEM_PARALLEL(PLC^ APlc, ELEM^ AParent) : ELEM_RUNG(APlc, AParent)
{
}

void ELEM_PARALLEL::CompilePass1C(PLC_COMPILER_C^ Compiler) 
{
	parThis = Compiler->AllocateBitOneShot("$parallelThis"); // not use same name than alias !!!
	parOut  = Compiler->AllocateBitOneShot("$parallelOut");

	ELEM_RUNG::CompilePass1C(Compiler);
}

void ELEM_PARALLEL::CompilePass1VM(PLC_COMPILER_VM^ Compiler) 
{
	parThis = Compiler->AllocateBitOneShot("$parThis");
	parOut  = Compiler->AllocateBitOneShot("$parOut");
	ELEM_RUNG::CompilePass1VM(Compiler);
}

void ELEM_PARALLEL::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	Compiler->vmComment("== PARALLEL ==");
	Compiler->vmClearBit(parOut);

	for each(ELEM^ item in items)
	{
		Compiler->Indent(+1);
		Compiler->vmGetBit(StateInOut);
		Compiler->vmPutBit(parThis);
		item->CompilePass2VM(Compiler, parThis);
		Compiler->vmSkipIfBitClear(parThis);
		Compiler->vmSetBit(parOut);
		Compiler->Indent(-1);
	}
	Compiler->vmGetBit(parOut);
	Compiler->vmPutBit(StateInOut);
}

void ELEM_PARALLEL::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->ClearAliases();
	Compiler->Begin(Marker(), "");

	Compiler->AddAlias("$StateInOut", StateInOut);
	Compiler->AddAlias("$parThis", parThis);
	Compiler->AddAlias("$parOut", parOut);
	Compiler->OutputC("ClearBit($parOut);",	"\t // $parOut = false");
	for each(ELEM^ item in items)
	{
		Compiler->ClearAliases();
		Compiler->AddAlias("$StateInOut", StateInOut);
		Compiler->AddAlias("$parThis", parThis);
		Compiler->Indent(+1);
		Compiler->OutputC("AssignBit($parThis, GetBit($StateInOut));",	"\t// $parThis = $StateInOut");
		item->CompilePass2C(Compiler, parThis);

		// we need to actualize Aliases
		Compiler->ClearAliases();
		Compiler->AddAlias("$StateInOut", StateInOut);
		Compiler->AddAlias("$parThis", parThis);
		Compiler->AddAlias("$parOut", parOut);

		Compiler->OutputC("if ( GetBit($parThis) ) SetBit($parOut);",	"\t// if ($parThis) $parOut = true");
		Compiler->Indent(-1);
	}
	Compiler->OutputC("AssignBit($StateInOut, GetBit($parOut));",	"\t// $StateInOut = $parOut");
	Compiler->End(Marker(), "");
}


ELEM_PROGRAM::ELEM_PROGRAM(PLC^ APlc, ELEM ^ AParent) : ELEM_LIST(APlc, AParent)
{
}

void ELEM_PROGRAM::LoadFromTextStream(StreamReader^ sr, int &LineCounter)
{
   ELEM_RUNG^ newRung;
   String^ s = "";
   while ( s = sr->ReadLine() )
   {
      LineCounter++;
	  s = s->Trim();
	  if(s->CompareTo("RUNG")==0)
	  {
        newRung = gcnew ELEM_RUNG(plc, this);
		newRung->LoadFromTextStream(sr, LineCounter);
		Add(newRung);
	  }
   }
}


void ELEM_PROGRAM::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	int i;
	Compiler->vmLabel("plcTASK");
	Compiler->vmComment("plc Implementation");
	Compiler->Indent(+1);
	
	i = 0;
	for each(ELEM_RUNG^ item in items)
	{
		Compiler->plcRungBegin(i);
		Compiler->vmLabel(String::Format("plcRung{0}", i));
		Compiler->Indent(+1);
		item->CompilePass2VM(Compiler, StateInOut);
		Compiler->Indent(-1);
		Compiler->plcRungEnd(i);
		i++;
	}
}

void ELEM_PROGRAM::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	int i;
	Compiler->ClearAliases();
	Compiler->AddAlias("$StateInOut", StateInOut);

	Compiler->OutputC("");
	Compiler->OutputC("", "// plc Implementation : 1 rung is executed at each call of plcTask()");

	Compiler->OutputC("");
	Compiler->OutputC("void plcTask(void)");
	Compiler->OutputC("{");
	Compiler->Indent(+1);
	Compiler->OutputC("if ( !GetBit($StateInOut) ) return;",	"\t// Master relay is off... exit task");
	Compiler->OutputC("SetBit($rung_top);",	"\t// $rung_top = $StateInOut, change this if you modify above line");

	Compiler->OutputC(String::Format("// {0} rungs to execute", items.Count));
	Compiler->OutputC("switch ( plcTaskRung ) {");
	i = 0;
	for each(ELEM_RUNG^ item in items) {
		Compiler->plcRungBegin(i);
		Compiler->OutputC(String::Format("\tcase {0} : ", i));
		Compiler->Indent(+2);
		item->CompilePass2C(Compiler, StateInOut);
		Compiler->Indent(-2);
		Compiler->OutputC("\t\tbreak;");
		Compiler->plcRungEnd(i);
		i++;
	}
	Compiler->OutputC("}", "\t// switch( plcTaskRung )");
	Compiler->OutputC("if ( GetBit($StateInOut) ) plcTaskRung++;", "\t// test needed if plcReset() from interrupt service");
	Compiler->Indent(-1);
	Compiler->OutputC("} // void plcTask(void)");
}



ELEM_LEAF::ELEM_LEAF(PLC^ APlc, ELEM^ AParent) : ELEM(APlc, AParent)
{
}

ELEM_COMMENT::ELEM_COMMENT(PLC^ APlc, ELEM^ AParent, String^ str) : ELEM_LEAF(APlc, AParent)
{	
	comment = str;
}

void ELEM_COMMENT::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine(Indent+Marker()+" "+comment);
}

long ELEM_COMMENT::CountLeafs(bool ExcludeComments)
{
	if (ExcludeComments)
		return 0;
	else
		return 1;
}

void ELEM_COMMENT::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	Compiler->OutputC("", "// "+comment);
}
