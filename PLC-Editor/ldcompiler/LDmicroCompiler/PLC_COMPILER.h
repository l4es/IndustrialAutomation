#pragma once

using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;


/**
 * Symbol table in Ladder
 */
/**
 * @{
 */
/** Label addressing space */
#define PLC_LABEL_SPACE		0
/** Bit addressing space */
#define PLC_BIT_IO			0
#define PLC_BIT_INTERNAL	1
#define PLC_BIT_EXTERNAL	2
#define PLC_BIT_SPECIAL		3
/** Var addressing space */
#define PLC_LITERAL			0
#define PLC_VAR_INTERNAL	1
#define PLC_VAR_EXTERNAL	2
#define PLC_VAR_SPECIAL		3
/** @} */

#define PLC_IO_OUTPUT		1
#define PLC_IO_INPUT		2
#define PLC_IO_PWM			3

#define PLCSYSBITS			-1
#define PLCBITS				-2


ref class PLC_SYMBOL
{
public:
	String^	SymbolName;
	int		AddrsSpace;
public:
    PLC_SYMBOL(String^ Name, int AddressingSpace);
	bool Match(String^ Name);								//!< return true if name == BitName
};

ref class PLC_LABEL_SYMBOL :
public PLC_SYMBOL
{
public:
	PLC_LABEL_SYMBOL(String^ LabelName) : PLC_SYMBOL(LabelName, PLC_LABEL_SPACE) {};
};

ref class PLC_BIT_SYMBOL :
public PLC_SYMBOL
{
public:
	String^ varName;										//!< 8 bits variable that contains the bit
	String^ bitName;										//!< Bit name in this variable
	int		bit;											//!< Bit position
public:
	PLC_BIT_SYMBOL(String^ SymbolName, int AddressingSpace, String^ VarName, String^ BitName, int Bit);
};

ref class PLC_VAR_SYMBOL :
public PLC_SYMBOL
{
public:
	int 	varSize;										//!< number of bits, including sign !!
	bool	varUnsigned;									//!< true if unsigned value
	bool	varInit;										//!< true if compiler must generate code to preset this var
	int		varInitialValue;
	String^	InitComment;
public:
    PLC_VAR_SYMBOL(String^ SymbolName, int AddressingSpace, int VarSize, bool VarUnsigned);
	void	MinSize(int VarSize);							//!< fix minimum size in bits
	void	InitialValue(int value, String^ Comment);	
};

/**
 * Used for IO initialization
 */
ref class PLC_IO 
{
public:
	String^ marker;
	String^ name;
	int		bit;
	int		type;
	PLC_IO(String^ Marker, String^ Name, int Bit, int ioType);
};

ref class PLC_IO_PORT 									
{
public:
	String^ name;
	List<PLC_IO^>	items;
	PLC_IO_PORT(String^ PortName);
	void AddIO(String^ Marker, String^ ioName, int Bit, int ioType);
	bool Match(String^ Name);								//!< return true if name == Name
};

/**
 * Used by the function outputing code. Provide read facility for writing
 * target code without eavy {0} {1}... format replacement
 * see CompilePass() of Leaf elements to have sample of use.
 * guidelines :
 * first clear the current list with ClearAliases()
 * add local aliases with AddAlias()
 * the ouput function of compiler will replace all aliases with the associated value
 * note : CompilePass() are nested in LIST elements... you must re-initialze aliases after return from child
 */
ref class PLC_ALIAS
{
public:
	String^ aliasName;
	String^ aliasValue;
	PLC_ALIAS(String^ AName, String^ AValue);
};

/**
 * Container for different value, options, ...
 */
ref class PLC_VALUE 									
{
public:
	String^ name;
	String^ value;
	String^ comment;
	PLC_VALUE(String^ Name, String^ Value, String^ Comment);
	bool Match(String^ Name);								//!< return true if name == Name
};


ref class PLC_COMPILER
{
protected:
	String^	IndentTabs;
	List<PLC_LABEL_SYMBOL^>	Labels;
	List<PLC_BIT_SYMBOL^>	Bits;
	List<PLC_IO_PORT^>	    PortsIO;
	List<PLC_ALIAS^>		Aliases;
	List<PLC_VALUE^>		Values;
public:
	bool AllowSameSymbolNames;		//!< Allow same name for Bit, Var or Label
	int BitAllocationCounter;		//!< for AllocateBit()
	List<PLC_VAR_SYMBOL^>	Vars;
protected:
	bool AllowSymbolAllocation(String^ Name);
	bool IsLabelAllocated(String^ Name);					//!< return true if symbol is already allocated
	bool IsBitAllocated(String^ Name);						//!< return true if symbol is already allocated
	bool IsVarAllocated(String^ Name);						//!< return true if symbol is already allocated
	bool FindBit(String^ Name, PLC_BIT_SYMBOL^ &Bit);
	bool FindVar(String^ Name, PLC_VAR_SYMBOL^ &Var);
	bool FindValue(String^ Name, PLC_VALUE^ &Value);
	// Syntax depedent functions
public:
	PLC_COMPILER(void);
	void Indent(int Tabs);
	void Reset(void);
	virtual String^ BitSyntaxName(String^ Name);
	virtual String^ VarSyntaxName(String^ Name);
	
	virtual String^ CompileError(String^ Msg);
	virtual String^ CompileWarning(String^ Msg);

	void AddLabel(String^ Name);
	String^ AllocateLabelOneShot(String^ Prefix);

	void AddBit(String^ Name, int AddressingSpace, String^ VarName, String^ BitName, int Bit);
	void AllocateBit(String^ Name);
	String^ AllocateBitOneShot(String^ Prefix);

	void AddVar(String^ Name, int AddressingSpace, int VarSize, bool Unsigned);
	void AddVar(String^ Name, int AddressingSpace, int VarSize);
	void AllocateVar(String^ Name, int VarSize);
	void AllocateVar(String^ Name, int VarSize, bool Unsigned);
	void PresetVar(String^ varName, int Value, String^ Comment);

	void AddIO(String^ Marker, String^ Name, String^ Port, int Bit, int ioType);

	void ClearAliases(void);
	void AddAlias(String^ Name, String^ Value);
	String^ ReplaceAliases(String^ CodeStr);	//!< Replace aliases in a line of code

	void SetValue(String^ Name, String^ Value, String^ Comment);
	void SetValue(String^ Name, int Value, String^ Comment);
	void SetValue(String^ Name, bool Value, String^ Comment);

	String^ GetValue(String^ Name, String^ Default);
	String^ GetValue(String^ Name);
	int GetIntValue(String^ Name, int Default);
	int GetIntValue(String^ Name);
	bool GetBoolValue(String^ Name, bool Default);
	bool GetBoolValue(String^ Name);
	String^ GetCommentValue(String^ Name);

//	virtual void McuTarget(String^ Mcu, int cycleTime, int mcuClock, int baudRate);

	virtual void plcRungBegin(int Rung);
	virtual void plcRungEnd(int Rung);
	virtual void plcGenerateReset(void);
	virtual void DumpVarsTable(TextWriter^ sw);
	virtual void DumpBitsTable(TextWriter^ sw);
	virtual void DumpIOTable(TextWriter^ sw);
};

ref class PLC_COMPILER_C :
public PLC_COMPILER
{
private:
	TextWriter^ swC;
public:
	bool ExpandReplaceMacroBitIO;
	bool ExpandReplaceMacroBitInternal;
	bool ExpandReplaceMacroBitExternal;
	bool ExpandReplaceMacroBitSpecial;
protected:
	virtual String^ MacroGetBit(String^ src, bool Expand);
	virtual String^ MacroAssignBit(String^ dest, String^ src, bool Expand);
	virtual String^ MacroSetBit(String^ dest, bool Expand);
	virtual String^ MacroClearBit(String^ dest, bool Expand);
public:
	PLC_COMPILER_C(TextWriter^ OutputC);
	virtual String^ BitSyntaxName(String^ Name) override;
	virtual void OutputC(String^ CodeStr);
	virtual void OutputC(String^ CodeStr, String^ Comment);
	virtual String^ ExpandMacros(String^ CodeStr);
	virtual void Begin(String^ BlockMarker, String^ BlockName);
	virtual void End(String^ BlockMarker, String^ BlockName);
	virtual void Code_MacroDefines(void);
	virtual void Code_VarGlobals(void);
	virtual void Code_plcTimerTicks(void);
	virtual void Code_plcReset(void);
	virtual void Code_plcDebug(void);
};

ref class PLC_COMPILER_VM :
public PLC_COMPILER
{
protected:
	TextWriter^ swMain;
protected:
	String^ vmBitSuffix(String^ bitName);
	String^ vmVarSuffix(String^ varName);
public:
	PLC_COMPILER_VM(TextWriter^ OutputMain);
	virtual String^ BitSyntaxName(String^ Name) override;
	// VM MACHINE CODE
	void vmWriteLine(String^ mnemonic, String^ argument, String^ comment);		//!< stream output function
	void vmComment(String^ msg);												//!< comment code
	void vmWarning(String^ msg);
	void vmError(String^ msg);
	// bit manipulation
    void vmSetBit(String^ bitName);
    void vmSetBit(String^ bitName, String^ comment);
    void vmClearBit(String^ bitName);
    void vmClearBit(String^ bitName, String^ comment);
    void vmGetBit(String^ bitName);
    void vmGetBit(String^ bitName, String^ comment);
    void vmPutBit(String^ bitName);
    void vmPutBit(String^ bitName, String^ comment);
    void vmSkipIfBitSet(String^ bitName);
    void vmSkipIfBitSet(String^ bitName, String^ comment);
    void vmSkipIfBitClear(String^ bitName);
    void vmSkipIfBitClear(String^ bitName, String^ comment);
	// Number manipulation
	void vmClear(String^ varDest, String^ comment);
	void vmClear(String^ varDest);
	void vmLoadA(String^ varSrc, String^ comment);
	void vmLoadA(int value, String^ comment);
	void vmLoadA(String^ varSrc);
	void vmLoadA(int value);
	void vmLoadB(String^ varSrc, String^ comment);
	void vmLoadB(int value, String^ comment);
	void vmLoadB(String^ varSrc);
	void vmLoadB(int value);
	void vmLoadR(String^ varSrc, String^ comment);
	void vmLoadR(int value, String^ comment);
	void vmLoadR(String^ varSrc);
	void vmLoadR(int value);
	void vmStoreR(String^ varDest, String^ comment);
	void vmStoreR(String^ varDest);
	void vmAdd(String^ varDest, String^ comment);
	void vmAdd(String^ varDest);
	void vmSub(String^ varDest, String^ comment);
	void vmSub(String^ varDest);
	void vmMul(String^ varDest, String^ comment);
	void vmMul(String^ varDest);
	void vmDiv(String^ varDest, String^ comment);
	void vmDiv(String^ varDest);
	void vmInc(String^ varDest, String^ comment);
	void vmInc(String^ varDest);
	void vmSelTmr(String^ varTimerTicks, String^ comment);
	void vmSelTmr(String^ varTimerTicks);
	void vmIncTmr(String^ varDest, String^ comment);
	void vmIncTmr(String^ varDest);
	void vmSkipIfFlagSet(String^ flags, String^ comment);
	void vmSkipIfFlagSet(String^ flags);
	void vmLabel(String^ label);
	void vmGoto(String^ label, String^ comment);
	void vmGoto(String^ label);
//	virtual void McuTarget(String^ Mcu, int cycleTime, int mcuClock, int baudRate) override;
	virtual void plcGenerateReset(void) override;
	virtual void DumpVarsTable(TextWriter^ sw) override;
	virtual void DumpBitsTable(TextWriter^ sw) override;
};

ref class PLC_COMPILER_CALM :
public PLC_COMPILER_VM
{
public:
	PLC_COMPILER_CALM(TextWriter^ OutputMain);
//	virtual void McuTarget(String^ Mcu, int cycleTime, int mcuClock, int baudRate) override;
	virtual void DumpVarsTable(TextWriter^ sw) override;
	virtual void DumpBitsTable(TextWriter^ sw) override;
	virtual void DumpIOTable(TextWriter^ sw) override;
	virtual void plcRungBegin(int Rung) override;
	virtual void plcRungEnd(int Rung) override;
	void CalmHeader(TextWriter^ sw);
	void CalmBeforeCompileCode(TextWriter^ sw);
	void CalmAfterCompileCode(TextWriter^ sw);
};