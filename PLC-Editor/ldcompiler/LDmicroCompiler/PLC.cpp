#include "StdAfx.h"

#include "ELEM.h"


PLC::PLC(void)
{
	targets = gcnew ELEM_TARGET_LIST(this, nullptr); 
	program = gcnew ELEM_PROGRAM(this, nullptr);

	System::Console::WriteLine("PLC created");
}

PLC::~PLC(void)
{
}


bool PLC::LoadFromFile(String^ path)
{
   bool LoadOK = true;
   int LineCounter = 0;
   int RungCounter = 0;
   int i;
   ELEM_TARGET^ newTarget; // for LDmicro0.1 file format

   ELEM_RUNG_INTERNAL^ rung0 = gcnew ELEM_RUNG_INTERNAL(this, program);
   ELEM_RUNG_INTERNAL^ lastrung = gcnew ELEM_RUNG_INTERNAL(this, program);
   ELEM_RUNG_INTERNAL^ internalRung;


   // Open the file to read from.
   Console::WriteLine( "Reading source " + path );
   StreamReader^ sr = File::OpenText( path );
   try
   {
      String^ s = "";
	  while ( s = sr->ReadLine() )
      {
		  LineCounter++;
		  s = s->Trim();
		  if(s->CompareTo("LDmicro0.1")==0) {
			  // LDmicro0.1 format
			  newTarget = gcnew ELEM_TARGET(this, targets);
//			  selTarget = newTarget;
			  newTarget->LoadFromTextStream(sr, LineCounter);
			  targets->Add(newTarget); // only one target in LDmicro0.1 file format
			  // Load PROGRAM
			  program->LoadFromTextStream(sr, LineCounter);
			  continue;
		  } else if(s->CompareTo("LDmicro2.0")==0) {
			  Console::WriteLine("Ladder File Format {0}", s);
			  continue;
		  } else if(s->CompareTo("TARGET_LIST")==0) {
			  targets->LoadFromTextStream(sr, LineCounter); 			  
		  } else if(s->CompareTo("PROGRAM")==0) {
			  program->LoadFromTextStream(sr, LineCounter);
			  continue;
		  } else 
			if (s->Length!=0) Console::WriteLine( "Ignored : " + s );
      }
   }
   catch (...)
   {
	   Console::WriteLine("Error near line {0}", LineCounter);
	   LoadOK=false;
   }
   finally
   {
      if ( sr )
            delete (IDisposable^)sr;
   }
   Console::WriteLine( "Source closed");

   RungCounter = program->Count();

   // Add Internal reset code
   rung0->Add(gcnew ELEM_INTERNAL_RESET(this, program));
//   program->Add(rung0);
   program->items.Insert(0, rung0);

   // Add internal special code here
	internalRung = gcnew ELEM_RUNG_INTERNAL(this, program);
	internalRung->Add(gcnew ELEM_INTERNAL_UART(this, program));
	program->Add(internalRung);

   // Add plcTimerTicks update code : ALWAYS LAST !!!
   lastrung->Add(gcnew ELEM_INTERNAL_UPDATE_TIMERTICKS(this, program));
   program->Add(lastrung); // Update plcTimerTick rung

   Console::WriteLine("  {0} TARGETS read", targets->Count());
   Console::WriteLine("  {0} RUNGS   read ( {1} Internals added )", RungCounter, program->Count()-RungCounter);


   StreamWriter^ sw = File::CreateText("Test.ld");
   WriteToTextStream(sw, "");
   if ( sw )
	   delete (IDisposable^)sw;

	i = 0;
	for each(ELEM_TARGET^ item in targets->items) {
		if (i == targets->selected) {
			selTarget = item;
			Console::WriteLine("Selected Target : {0}", selTarget->micro);
		}
		i++;
	}
   return LoadOK;
}

void PLC::WriteToTextStream(StreamWriter^ sw, String^ Indent)
{
	sw->WriteLine(Indent+"LDmicro2.0");
	targets->WriteToTextStream(sw, Indent);
	program->WriteToTextStream(sw, Indent);
}

void PLC::SaveToFile(String^ path)
{
	StreamWriter^ sw = File::CreateText( path );
	WriteToTextStream(sw, "");
	sw->Close();
}


int PLC::CycleTime(void)
{
  ELEM_TARGET^ selTarget;
//  selTarget = (ELEM_TARGET^)targets->items[1];
  return 10000;

  return selTarget->cycleTime;
}

void PLC::CompilePass1C(PLC_COMPILER_C^ Compiler)
{
	selTarget->CompilePass1C(Compiler);
	program->CompilePass1C(Compiler);
}

void PLC::CompilePass1VM(PLC_COMPILER_VM^ Compiler)
{
	selTarget->CompilePass1VM(Compiler);
	program->CompilePass1VM(Compiler);
}

void PLC::CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut)
{
	selTarget->CompilePass2VM(Compiler, StateInOut);
	program->CompilePass2VM(Compiler, StateInOut);
}

void PLC::CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut)
{
	if (program->items.Count > 255) Compiler->AllocateVar("plcTaskRung", 16, true); // update size

	selTarget->CompilePass2C(Compiler, StateInOut);

	Compiler->Code_MacroDefines();
	Compiler->Code_VarGlobals();
	Compiler->Code_plcTimerTicks();
	Compiler->Code_plcReset();
	Compiler->Code_plcDebug();

	program->CompilePass2C(Compiler, "$mcr");

	Compiler->ClearAliases();
}
