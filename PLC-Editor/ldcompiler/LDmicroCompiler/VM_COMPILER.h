#pragma once

using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;

// Labels don't code and don't increment PC counter !!
#define INT_LABEL                               -2

// Bit functions
#define INT_SET_BIT                              1
#define INT_CLEAR_BIT                            2
#define INT_COPY_BIT                             3
#define INT_SET_VARIABLE_TO_LITERAL              4
#define INT_SET_VARIABLE_TO_VARIABLE             5   
#define INT_INCREMENT_VARIABLE                   6
#define INT_SET_VARIABLE_ADD                     7
#define INT_SET_VARIABLE_SUBTRACT                8
#define INT_SET_VARIABLE_MULTIPLY                9
#define INT_SET_VARIABLE_DIVIDE                 10

#define INT_READ_ADC                            11
#define INT_SET_PWM                             12
#define INT_UART_SEND                           13
#define INT_UART_RECV                           14
#define INT_EEPROM_BUSY_CHECK                   15
#define INT_EEPROM_READ                         16
#define INT_EEPROM_WRITE                        17

#define INT_IF_GROUP(x) (((x) >= 50) && ((x) < 60))
#define INT_BIT_IF_SET                          50
#define INT_BIT_IF_CLEAR                        51
#define INT_IF_VARIABLE_LES_LITERAL             52
#define INT_IF_VARIABLE_EQUALS_VARIABLE         53
#define INT_IF_VARIABLE_GRT_VARIABLE            54

#define INT_ELSE                                60
#define INT_END_IF                              61
// used to reduce IF ELSE END statments
#define INT_GOTO                                63
#define INT_SKIP_IF_BIT_IS_SET                  64
#define INT_SKIP_IF_BIT_IS_CLEAR                65

#define INT_SIMULATE_NODE_STATE                 80

#define INT_COMMENT                            100

// Only used for the interpretable code.
#define INT_END_OF_PROGRAM                     255

ref class VM_COMPILER;

ref class VM_OP
{
private:
	int fop;
	int fbin;
	String^ fgenLine;
	bool fimplemented;
	bool fused;
public:
	VM_OP(int op, int bin, String^ genLine);
    bool Match(int op);
	void Generate(VM_COMPILER^ vmc, TextWriter^ sw, array<System::String ^> ^args);
};


ref class VM_COMPILER
{
private:
	TextWriter^ swMain;
	int GenSymbCountOneShot;
	int LabelCountOneShot;
	int BitAllocationCounter;
	int indentCount;
	bool ConvertIfElse;
	List<VM_OP^> mnemo;
	List<String^> if_labels; // contains nested else/endif labels for converting if then else endif statements
protected:
	VM_OP^ FindOp(int op); // find the data for the given op
	void Op(int op, array<System::String ^> ^args);
public:
	bool NeedsToPopLabel;
public:
	VM_COMPILER(TextWriter^ swMain);
	int  LDversion(void);
	void Mnemo0_1(void);
	void Mnemo2_0(void);
	void Oops(String^ msg);
	void Indent(int count);
    bool RequestOP(int op);
	String^ GenSymbOneShot(void);
	String^ GenLabelOneShot(String^ prefix);
	String^ PopLabel(void);

	// VM MACHINE CODE
	void vmWriteLine(String^ mnemonic, String^ argument, String^ comment);		//!< stream output function
	void vmComment(String^ msg);												//!< comment code
	void vmWarning(String^ msg);
	void vmError(String^ msg);
	void vmInitVar(String^ varName, int value, String^ comment);
	void vmLabel(String^ label);
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
	void vmSub(String^ varDest, String^ comment);
	void vmSub(String^ varDest);
	void vmInc(String^ varDest, String^ comment);
	void vmInc(String^ varDest);
	void vmIncTmr(String^ varDest, String^ comment);
	void vmIncTmr(String^ varDest);
	void vmSkipIfFlagSet(String^ flags, String^ comment);
	void vmSkipIfFlagSet(String^ flags);
	void vmGoto(String^ label, String^ comment);
	void vmGoto(String^ label);
	// avoid using this
	void INCREMENT(String^ varName);
	void ASSIGN(String^ varName, String^ src);
	void ASSIGN(String^ varName, int value);
	void IF_LES(String^ op1, String^ op2);
	void IF_LES(String^ op1, int op2);
};
