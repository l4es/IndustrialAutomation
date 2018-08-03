#pragma once

#include "PLC_COMPILER.h"

using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;

ref class PLC;
//--------------------------------------------------------------------------------------------------
// ELEM is the ancestor of all ELEM_xxx
//--------------------------------------------------------------------------------------------------
ref class ELEM 
{
public:
	PLC^ plc;			//!< Owner of this element
	ELEM^ parent;		//!< Parent of this element
public:
	ELEM(PLC^ APlc, ELEM^ AParent);
	virtual String^ Marker(void) {return "*Error* ELEM Marker";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent);
	virtual void LoadFromTextStream(StreamReader^ sr, int &LineCounter);
	virtual long CountLeafs(bool ExcludeComments) {return -1;};
	virtual void CompilePass1(PLC_COMPILER^ Compiler);
	virtual void CompilePass1C(PLC_COMPILER_C^ Compiler);
	virtual void CompilePass1VM(PLC_COMPILER_VM^ Compiler);
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut);
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut);
};

//--------------------------------------------------------------------------------------------------
// ELEM_INTERNAL is the ancestor of all ELEM_INTERNAL_xxx that don't appear in .LD file
//--------------------------------------------------------------------------------------------------
ref class ELEM_INTERNAL :
public ELEM
{
public:
	ELEM_INTERNAL(PLC^ APlc, ELEM^ AParent);
	virtual String^ Marker(void) override {return "*Error* ELEM_INTERNAL Marker";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void LoadFromTextStream(StreamReader^ sr, int &LineCounter) override;
};

//--------------------------------------------------------------------------------------------------
// ELEM_INTERNAL_RESET Reset the PLC, normaly in RUNG 0
//--------------------------------------------------------------------------------------------------
ref class ELEM_INTERNAL_RESET :
public ELEM_INTERNAL
{
public:
	ELEM_INTERNAL_RESET(PLC^ APlc, ELEM^ AParent);
	virtual String^ Marker(void) override {return "INTERNAL_RESET";};
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

//--------------------------------------------------------------------------------------------------
// ELEM_INTERNAL_UPDATE_TIMERTICKS update plcTimerTicks and return to RUNG 1, normaly in last RUNG
//--------------------------------------------------------------------------------------------------
ref class ELEM_INTERNAL_UPDATE_TIMERTICKS :
public ELEM_INTERNAL
{
public:
	ELEM_INTERNAL_UPDATE_TIMERTICKS(PLC^ APlc, ELEM^ AParent);
	virtual String^ Marker(void) override {return "INTERNAL_UPDATE_TIMERTICKS";};
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

//--------------------------------------------------------------------------------------------------
// ELEM_INTERNAL_UART special elem for UART
//--------------------------------------------------------------------------------------------------
ref class ELEM_INTERNAL_UART :
public ELEM_INTERNAL
{
public:
	ELEM_INTERNAL_UART(PLC^ APlc, ELEM^ AParent);
	virtual String^ Marker(void) override {return "INTERNAL_UART";};
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

//--------------------------------------------------------------------------------------------------
// ELEM_LIST : main container, ancestor for RUNG, SERIE, PARALLEL elements
//--------------------------------------------------------------------------------------------------
ref class ELEM_LIST :
public ELEM
{
public:
	int selected;
	List<ELEM^> items;
public:
	ELEM_LIST(PLC^ APlc, ELEM^ AParent);
	virtual String^ Marker(void) override {return "LIST";};
	int Count(void);
	void Add(ELEM^ item);
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual long CountLeafs(bool ExcludeComments) override;
	virtual void CompilePass1C(PLC_COMPILER_C^ Compiler) override;
	virtual void CompilePass1VM(PLC_COMPILER_VM^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};


//--------------------------------------------------------------------------------------------------
// ELEM_IO_LIST
//--------------------------------------------------------------------------------------------------
ref class ELEM_IO_LIST :
public ELEM_LIST
{
public:
	ELEM_IO_LIST(PLC^ APlc, ELEM^ AParent);
	virtual String^ Marker(void) override {return "IO_LIST";};
	virtual void LoadFromTextStream(StreamReader^ sr, int &LineCounter) override;
	void OldFormat0_1_IO(String^ str);
};

//--------------------------------------------------------------------------------------------------
// ELEM_TARGET
//--------------------------------------------------------------------------------------------------
ref class ELEM_TARGET :
public ELEM
{
public:
	ELEM_TARGET(PLC^ APlc, ELEM^ AParent);
	virtual String^ Marker(void) override {return "TARGET";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void LoadFromTextStream(StreamReader^ sr, int &LineCounter) override;
	virtual void CompilePass1C(PLC_COMPILER_C^ Compiler) override;
	virtual void CompilePass1VM(PLC_COMPILER_VM^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
    String^		micro;
    int         cycleTime;
    int         mcuClock;
    int         baudRate;
	String^		compiled;
	ELEM_IO_LIST^  io_list;
};

//--------------------------------------------------------------------------------------------------
// ELEM_TARGET_LIST
//--------------------------------------------------------------------------------------------------
ref class ELEM_TARGET_LIST :
public ELEM_LIST
{
public:
	ELEM_TARGET_LIST(PLC^ APlc, ELEM^ AParent);
	virtual String^ Marker(void) override {return "TARGET_LIST";};
	virtual void LoadFromTextStream(StreamReader^ sr, int &LineCounter) override;
};

//--------------------------------------------------------------------------------------------------
// ELEM_IO contain I/O maping info for target device
//--------------------------------------------------------------------------------------------------
ref class ELEM_IO :
public ELEM
{
public:
	String^	name;
	String^ port;
	int		size;
	int		pin; // old format
	int		bit;
	bool	pullup;
public:
	ELEM_IO(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "IO";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_IO_INPUT :
public ELEM_IO
{
public:
	ELEM_IO_INPUT(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "INPUT";};
	virtual void CompilePass1C(PLC_COMPILER_C^ Compiler) override;
	virtual void CompilePass1VM(PLC_COMPILER_VM^ Compiler) override;
};

ref class ELEM_IO_OUTPUT :
public ELEM_IO
{
public:
	ELEM_IO_OUTPUT(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "OUTPUT";};
	virtual void CompilePass1C(PLC_COMPILER_C^ Compiler) override;
	virtual void CompilePass1VM(PLC_COMPILER_VM^ Compiler) override;
};

ref class ELEM_IO_ADC :
public ELEM_IO
{
public:
	ELEM_IO_ADC(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "ADC";};
	virtual void CompilePass1C(PLC_COMPILER_C^ Compiler) override;
	virtual void CompilePass1VM(PLC_COMPILER_VM^ Compiler) override;
};

ref class ELEM_IO_RCIN :
public ELEM_IO
{
public:
	ELEM_IO_RCIN(PLC^ APlc, ELEM^ AParent, String^ str);
};

ref class ELEM_IO_RCOUT :
public ELEM_IO
{
public:
	ELEM_IO_RCOUT(PLC^ APlc, ELEM^ AParent, String^ str);
};

ref class ELEM_IO_RELAY :
public ELEM_IO
{
public:
	ELEM_IO_RELAY(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "RELAY";};
};

//--------------------------------------------------------------------------------------------------
// ELEM_RUNG : Top level element of PLC program
//--------------------------------------------------------------------------------------------------
ref class ELEM_RUNG :
public ELEM_LIST
{
public:
	ELEM_RUNG(PLC^ APlc, ELEM^ AParent);
	virtual String^ Marker(void) override {return "RUNG";};
	virtual void LoadFromTextStream(StreamReader^ sr, int &LineCounter) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_RUNG_INTERNAL :
public ELEM_RUNG
{
public:
	ELEM_RUNG_INTERNAL(PLC^ APlc, ELEM^ AParent);
	virtual String^ Marker(void) override {return "RUNG_INTERNAL";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void LoadFromTextStream(StreamReader^ sr, int &LineCounter) override;
};


ref class ELEM_SERIES :
public ELEM_RUNG
{
public:
	ELEM_SERIES(PLC^ APlc, ELEM^ AParent);
	virtual String^ Marker(void) override {return "SERIES";};
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_PARALLEL :
public ELEM_RUNG
{
public:
	String^ parThis;
	String^ parOut;
public:
	ELEM_PARALLEL(PLC^ APlc, ELEM^ AParent);
	virtual String^ Marker(void) override {return "PARALLEL";};
	virtual void CompilePass1C(PLC_COMPILER_C^ Compiler) override;
	virtual void CompilePass1VM(PLC_COMPILER_VM^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_PROGRAM :
public ELEM_LIST
{
public:
	ELEM_PROGRAM(PLC^ APlc, ELEM^ AParent);
	virtual String^ Marker(void) override {return "PROGRAM";};
	virtual void LoadFromTextStream(StreamReader^ sr, int &LineCounter) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class PLC 
{
private:
	ELEM_TARGET_LIST^ targets;
	ELEM_PROGRAM^ program;
	ELEM_TARGET^ selTarget;
public:
	PLC(void);
	~PLC(void);
	bool LoadFromFile(String^ path);
	void SaveToFile(String^ path);
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent);
	int CycleTime(void);
	virtual void CompilePass1C(PLC_COMPILER_C^ Compiler); 
	virtual void CompilePass1VM(PLC_COMPILER_VM^ Compiler); 
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut);
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut);
};


ref class ELEM_LEAF :
public ELEM
{
public:
	ELEM_LEAF(PLC^ APlc, ELEM^ AParent);
	virtual long CountLeafs(bool ExcludeComments) override {return 1;};
};


ref class ELEM_COMMENT :
public ELEM_LEAF
{
private:
    String^ comment;
public:
	ELEM_COMMENT(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "COMMENT";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual long CountLeafs(bool ExcludeComments) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
};



ref class ELEM_OPEN :
public ELEM_LEAF
{
public:
	ELEM_OPEN(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "OPEN";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_SHORT :
public ELEM_LEAF
{
public:
	ELEM_SHORT(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "SHORT";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};


ref class ELEM_CONTACTS :
public ELEM_LEAF
{
private:
    String^ name;
    bool negated;
public:
	ELEM_CONTACTS(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "CONTACTS";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};


ref class ELEM_COIL :
public ELEM_LEAF
{
private:
	String^ name;
    bool    negated;
    bool    setOnly;
    bool    resetOnly;
public:
	ELEM_COIL(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "COIL";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_TIMER :
public ELEM_LEAF
{
public:
	String^ name;
	int delay;
public:
	ELEM_TIMER(PLC^ APlc, ELEM^ AParent, String^ str);
	int Period(void); // return computed period = delay / cycleTime;
	virtual String^ Marker(void) override {return "TIMER";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
};

ref class ELEM_RTO :
public ELEM_TIMER
{
public:
	ELEM_RTO(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "RTO";};
//	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_TON :
public ELEM_TIMER
{
public:
	ELEM_TON(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "TON";};
//	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_TOF :
public ELEM_TIMER
{
public:
	String^ antiGlitchName;
public:
	ELEM_TOF(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "TOF";};
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_CTU :
public ELEM_LEAF
{
private:
    String^ name;
	String^ storeName;
    int max;
public:
	ELEM_CTU(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "CTU";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_CTD :
public ELEM_LEAF
{
private:
    String^ name;
	String^ storeName;
    int max;
public:
	ELEM_CTD(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "CTD";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};


ref class ELEM_CTC :
public ELEM_LEAF
{
private:
    String^ name;
	String^ storeName;
    int max;
public:
	ELEM_CTC(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "CTC";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};


ref class ELEM_ONE_SHOT_RISING :
public ELEM_LEAF
{
private:
    String^ name;
public:
	ELEM_ONE_SHOT_RISING(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "OSR";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_ONE_SHOT_FALLING :
public ELEM_LEAF
{
private:
    String^ name;
public:
	ELEM_ONE_SHOT_FALLING(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "OSF";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_MOVE :
public ELEM_LEAF
{
private:
	String^ src;
	String^ dest;
public:
	ELEM_MOVE(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "MOVE";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_ADD :
public ELEM_LEAF
{
private:
	String^ dest;
	String^ op1;
	String^ op2;
public:
	ELEM_ADD(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "ADD";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_SUB :
public ELEM_LEAF
{
private:
	String^ dest;
	String^ op1;
	String^ op2;
public:
	ELEM_SUB(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "SUB";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_MUL :
public ELEM_LEAF
{
private:
	String^ dest;
	String^ op1;
	String^ op2;
public:
	ELEM_MUL(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "MUL";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_DIV :
public ELEM_LEAF
{
private:
	String^ dest;
	String^ op1;
	String^ op2;
public:
	ELEM_DIV(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "DIV";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};


ref class ELEM_EQU :
public ELEM_LEAF
{
private:
	String^ op1;
	String^ op2;
public:
	ELEM_EQU(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "EQU";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_GRT :
public ELEM_LEAF
{
private:
	String^ op1;
	String^ op2;
public:
	ELEM_GRT(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "GRT";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};
ref class ELEM_GEQ :
public ELEM_LEAF
{
private:
	String^ op1;
	String^ op2;
public:
	ELEM_GEQ(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "GEQ";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};
ref class ELEM_LES :
public ELEM_LEAF
{
private:
	String^ op1;
	String^ op2;
public:
	ELEM_LES(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "LES";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};
ref class ELEM_LEQ :
public ELEM_LEAF
{
private:
	String^ op1;
	String^ op2;
public:
	ELEM_LEQ(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "LEQ";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};
ref class ELEM_NEQ :
public ELEM_LEAF
{
private:
	String^ op1;
	String^ op2;
public:
	ELEM_NEQ(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "NEQ";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_RES :
public ELEM_LEAF
{
private:
    String^ name;
public:
	ELEM_RES(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "RES";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_MASTER_RELAY :
public ELEM_LEAF
{
public:
	ELEM_MASTER_RELAY(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "MASTER_RELAY";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_FORMATTED_STRING :
public ELEM_LEAF
{
private:
    String^ name;
	String^ format;
	int		digits;
	bool	minus;
public:
	ELEM_FORMATTED_STRING(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "FORMATTED_STRING";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};

ref class ELEM_INLINE_CODE :
public ELEM_LEAF
{
private:
    String^ code;
public:
	ELEM_INLINE_CODE(PLC^ APlc, ELEM^ AParent, String^ str);
	virtual String^ Marker(void) override {return "INLINE_CODE";};
	virtual void WriteToTextStream(StreamWriter^ sw, String^ Indent) override;
	virtual void CompilePass1(PLC_COMPILER^ Compiler) override;
	virtual void CompilePass2C(PLC_COMPILER_C^ Compiler, String^ StateInOut) override;
	virtual void CompilePass2VM(PLC_COMPILER_VM^ Compiler, String^ StateInOut) override;
};
