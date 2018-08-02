#ifndef ILDEFS_H
#define ILDEFS_H

#define FCOMMON_NAME "ilPLC Instruction List Interpreter"
#define FCOMMON_VERSION 0.1

#define MAX_IL_LINES 1000
#define MAX_VAR_LEN   100

#define WARN_VAR_NOT_INIT           1001
#define WARN_AKKU_TYPE_OUT_OF_RANGE 1002
#define WARN_OP_TYPE_OUT_OF_RANGE   1003
#define WARN_PARAM_LIST             1004

#define ERR_UNKNOWN_LABEL         2001
#define ERR_NO_VAR_END            2002
#define ERR_VAR_DOUBLE_DEFINED    2003
#define ERR_UNKNOWN_TYPE          2004
#define ERR_UNKNOWN_VAR           2005
#define ERR_REAL_TYPE_ERROR       2006
#define ERR_UNINITIALIZED_VAR     2007
#define ERR_AKKU_DATATYPE         2008
#define ERR_OP_DATATYPE           2009
#define ERR_STRING_TYPE_ERROR     2010
#define ERR_VAR_GLOBAL            2011
#define ERR_VAR_LOCAL             2012
#define ERR_VAR_NOT_IN_FCT        2013
#define ERR_UNKNOWN_FCT           2014
#define ERR_NO_STRING_TERM        2015
#define ERR_FCT_DOUBLE_DEFINED    2016
#define ERR_NO_STRING_OP          2017
#define ERR_NO_NODENAME           2018

#define IERR_NO_VAR_MEM   3001
#define IERR_NO_MEM       3002

enum var_type
{
	TYPE_UNDEFINED=0,
	ilBOOL,
	USINT,
	SINT,
	ilUINT,
	ilINT,
	UDINT,  // 32 Bit
	DINT,   // 32 Bit
	ULINT,  // 64 Bit
	LINT,   // 64 Bit
	REAL,   // 32 Bit
	LREAL,  // 64 Bit
	STRING, // ASCII
};

#define TYPE_BOOL   0x0001
#define TYPE_INT    0x0002
#define TYPE_REAL   0x0004
#define TYPE_STRING 0x0008



struct var_entry
{
	char              varName[MAX_VAR_LEN+1];
	enum var_type     type;
	int64_t           val;
	double            vald;
	char             *vals;
	struct is_data   *isHandle;   // the interlock connection this varable is mapped to
	char             *isNodeName; // the name of the node within the interlock server
	char              isIO;       // the IO number       
	struct var_entry *next;
};



struct label_entry
{
	char                labelName[MAX_VAR_LEN+1];
   int                 line;
   struct label_entry *next;	
};



struct fct_entry
{
	char              fctName[MAX_VAR_LEN+1];
   int               line;
   struct fct_entry *next;	
};



extern struct var_entry *firstGVarEntry;

extern int execute_il(int currLine,char is_function,struct var_entry *firstVarEntry,char *file,int line);

#endif
