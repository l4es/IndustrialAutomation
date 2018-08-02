/******************************************************************************

This file is part of ControlRoom process control/HMI software.

ControlRoom is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

ControlRoom is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
ControlRoom. If not, see http://www.gnu.org/licenses/

*******************************************************************************

For different licensing and/or usage of the sources apart from GPL or any other
open source license, please contact us at https://openapc.com/contact.php

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#ifndef ENV_WINDOWS
 #include <unistd.h>
 #include <inttypes.h> // QNX?
#else
 #pragma warning(disable:4996)
 #include <windows.h>
 #ifdef ENV_WINDOWSCE
  #include <winsock2.h>
 #endif
#endif

#ifndef ENV_QNX
#include "getopt.h"
#endif

#include "liboapc.h"
#include "oapc_libio.h"
#include "ildefs.h"
#include "isconnect.h"
#include "globaldefs.h"

static char               *ilBuffer=NULL,*ilFName=NULL;
static char               *ilLines[MAX_IL_LINES+1];
static int                 ilLNumber[MAX_IL_LINES+1];
static int                 lastLine=0;
       struct var_entry   *firstGVarEntry=NULL;
       struct is_data      isData[MAX_IS_CONNECTIONS];    // handler to access callback function blocks
       struct is_data      isDatai[MAX_IS_CONNECTIONS];   // handler for LDISx and STISx commands
       struct is_data      isDatam[MAX_IS_CONNECTIONS];   // handler for MAP_ISx-mapped variables
static void               *g_isMutex[MAX_IS_CONNECTIONS];
       struct label_entry *firstLabelEntry=NULL;
       struct fct_entry   *firstFctEntry=NULL;
static char                verbose=0;
       
//#ifndef ENV_LINUX
char isnumber(char *in)
{
	if (((in[0]>='0') && (in[0]<='9')) || (in[0]=='-') || (in[0]=='.')) return 1;
	return 0;
}
//#endif



/** definitions for the command line options */
static char shortopts[] = "vVhf:";
#ifndef ENV_QNX
static struct option const longopts[] = {
   {"scriptfile",required_argument, NULL, 'f'},
   {"version",   no_argument,       NULL, 'v'},
   {"verbose",   no_argument,       NULL, 'V'},
   {"help",      no_argument,       NULL, 'h'},
  {NULL, no_argument, NULL, 0}
};
#endif



/** definitions for the command line help output */
#ifdef ENV_QNX
static char const *const option_help[] = {
  " -f  the IL script file that has to be executed",
  " -V  print out some debugging information",
  " -v  show version info",
  " -h  show this help",
  0
};
#else
static char const *const option_help[] = {
  " -f  --file     the IL script file that has to be executed",
  " -V  --verbose  print out some debugging information",
  " -v  --version  show version info",
  " -h  --help     show this help",
  0
};
#endif


/** print out version information */
static void version(void)
{
   printf ("%s %.1f %s\n%s\n", FCOMMON_NAME,FCOMMON_VERSION,FCOMMON_URL,FCOMMON_COPYRIGHT);
}



/** print out command line help information */
static void usage (char *pname)
{
   printf ("usage: %s [OPTIONS]\n",pname);
   printf (" --help for more information\n");
   return;
}



/**
 * Get the switches and related values out of the command line parameters
 * @param argc the number of arguments as handed over by main()
 * @param argv the arguments as handed over by main()
 */
static void getSwitches(int argc,char *argv[])
{
int                optc;
char const *const *p;

if (optind == argc) return;
#ifndef ENV_WINDOWS
while ((optc = getopt(argc, argv, shortopts)) != -1)
#else
while ((optc = getopt_long (argc, argv, shortopts, longopts, (int *) 0)) != -1)
#endif
   {
   switch (optc)
      {
      case 'f':
         ilFName=strdup(optarg);
         break;
      case 'V':
         verbose=true;
         break;
      case 'v':
         version();
         exit (0);
         break;
      case 'h':
         usage (argv[0]);
         for (p = option_help; *p; p++) printf ("%s\n", *p);
         exit (0);
         break;
      default:
         usage (argv[0]);
      }
   }
}



static struct var_entry *get_var_entry(struct var_entry *firstVarEntry,char *name)
{
	int i;
	
	for (i=0; i<2; i++)
	{
   	while (firstVarEntry) 
	   {
		   if (!strncmp(firstVarEntry->varName,name,MAX_VAR_LEN)) return firstVarEntry;
   		firstVarEntry=firstVarEntry->next;
	   }
	   firstVarEntry=firstGVarEntry; // check if variable exists globally
	}
	return NULL;
}



static void free_varlist(struct var_entry *firstVarEntry)
{
	struct var_entry *currVarEntry=NULL;
	
	while (firstVarEntry)
	{
		if (currVarEntry)
		{
			if (currVarEntry->vals) free(currVarEntry->vals);
			if (currVarEntry->isNodeName) free(currVarEntry->isNodeName);
			free(currVarEntry);
		}
		currVarEntry=firstVarEntry;
		firstVarEntry=firstVarEntry->next;
	}
	if (currVarEntry) free(currVarEntry);
}



static void print_err(int code,int line)
{
	switch (code)
	{
		case WARN_VAR_NOT_INIT:
		   printf("Warning %d: variable not initialized and not used before at line %d\n",code,line);
		   break;
		case WARN_AKKU_TYPE_OUT_OF_RANGE:
		   printf("Warning %d: possible loss of data, akku type value out of range at line %d\n",code,line);
		   break;
		case WARN_OP_TYPE_OUT_OF_RANGE:
		   printf("Warning %d: possible loss of data, operand type value out of range at line %d\n",code,line);
		   break;
		case WARN_PARAM_LIST:
		   printf("Warning %d: incomplete function parameter list at line %d\n",code,line);
		   break;
		   
		case ERR_UNKNOWN_LABEL:
		   printf("Error %d: jump command with unknown label at line %d\n",code,line);
		   break;		
		case ERR_NO_VAR_END:
		   printf("Error %d: missing VAR_END statement after variable declaration at line %d\n",code,line);
		   break;		
		case ERR_VAR_DOUBLE_DEFINED:
		   printf("Error %d: variable double defined at line %d\n",code,line);
		   break;		
		case ERR_UNKNOWN_TYPE:
		   printf("Error %d: unknown data type specifier at line %d\n",code,line);
		   break;		
		case ERR_UNKNOWN_VAR:
		   printf("Error %d: unknown variable used at line %d\n",code,line);
		   break;		
		case ERR_REAL_TYPE_ERROR:
		   printf("Error %d: can't use REAL datatype for logic operation at line %d\n",code,line);
		   break;		
		case ERR_UNINITIALIZED_VAR:
		   printf("Error %d: using uninitialized variable at line %d\n",code,line);
		   break;		
		case ERR_AKKU_DATATYPE:
		   printf("Error %d: akku has wrong data type for operation at line %d\n",code,line);
		   break;		
		case ERR_OP_DATATYPE:
		   printf("Error %d: operand has wrong data type for operation at line %d\n",code,line);
		   break;		
		case ERR_STRING_TYPE_ERROR:
		   printf("Error %d: can't assign STRING values to non STRING datatypes at line %d\n",code,line);
		   break;		
		case ERR_VAR_GLOBAL:		
		   printf("Error %d: global variable definition not allowed at line %d\n",code,line);
		   break;		
		case ERR_VAR_LOCAL:
		   printf("Error %d: local variable definition not allowed at line %d\n",code,line);
		   break;		
		case ERR_VAR_NOT_IN_FCT:
  			printf("Error %d: illegal call not from within a function at line %d\n",code,line);
  			break;
		case ERR_UNKNOWN_FCT:
		   printf("Error %d: call command with unknown function at line %d\n",code,line);
		   break;		
		case ERR_NO_STRING_TERM:
		   printf("Error %d: string value not terminated at line %d\n",code,line);
		   break;		
		case ERR_FCT_DOUBLE_DEFINED:
		   printf("Error %d: function name double defined at line %d\n",code,line);
		   break;		
		case ERR_NO_STRING_OP:
		   printf("Error %d: STRING operand expected at line %d\n",code,line);
		   break;		
		case ERR_NO_NODENAME:
		   printf("Error %d: no valid node name in operand at line %d\n",code,line);
		   break;
		   
		case IERR_NO_VAR_MEM:
  			printf("Internal Error %d: no more memory for variable at line %d\n",code,line);
  			break;
      case IERR_NO_MEM:
  			printf("Internal Error %d: not nough memory for operation at line %d\n",code,line);
  			break;

		default:
		   printf("Internal error 3999: unknown error event at line %d\n",line);
	}	
}



static char is_out_of_range(struct var_entry *a)
{
	char doWarn=0;
	
	if (!a) return 0;
	if ((a->type==USINT) && ((a->val>255) || (a->val<0)))
	{
		doWarn=1;
		a->val=(unsigned char)(a->val);
	}
	else if ((a->type==SINT) && ((a->val>128) || (a->val<-127)))
	{
		doWarn=1;
		a->val=(char)a->val;
	}
	else if ((a->type==ilUINT) && ((a->val>65535) || (a->val<0)))
	{
		doWarn=1;
		a->val=(unsigned short)a->val;
	}
	else if ((a->type==ilINT) && ((a->val>32768) || (a->val<-32767)))
	{
		doWarn=1;
		a->val=(short)a->val;
	}
#ifndef ENV_WINDOWS
	else if ((a->type==UDINT) && ((a->val>(int64_t)4294967296LL) || (a->val<0)))
#else
	else if ((a->type==UDINT) && ((a->val>(int64_t)4294967296L) || (a->val<0)))
#endif
   {
		doWarn=1;
		a->val=(unsigned int)a->val;
	}
#ifndef ENV_WINDOWS
	else if ((a->type==DINT) && ((a->val>(int64_t)2147483648LL) || (a->val<(int64_t)-2147483647)))
#else
	else if ((a->type==DINT) && ((a->val>(int64_t)2147483648L) || (a->val<(int64_t)-2147483647)))
#endif
	{
		doWarn=1;
		a->val=(int)a->val;
	}
	else if (a->type==REAL)
	{
		doWarn=1;
		a->vald=(float)a->vald;
	}
	return doWarn;
}



static void check_range(struct var_entry *a,char warna,struct var_entry *o,char warno,int line)
{
	if ((is_out_of_range(a)) && (warna)) print_err(WARN_AKKU_TYPE_OUT_OF_RANGE,line);
	if ((is_out_of_range(o)) && (warno)) print_err(WARN_OP_TYPE_OUT_OF_RANGE,line);
}



static char is_wrong_datatype(struct var_entry *a,int atype)
{
   if (a)
   {
   	if ((atype & TYPE_BOOL) && (a->type==ilBOOL)) return 0;
   	else if ((atype & TYPE_INT) &&  ((a->type==USINT) || (a->type==SINT) || 
   	                                 (a->type==ilUINT) ||  (a->type==ilINT) || 
   	                                 (a->type==UDINT) || (a->type==DINT) ||
   	                                 (a->type==ULINT) || (a->type==LINT))) return 0;
   	else if ((atype & TYPE_REAL) && ((a->type==REAL) || (a->type==LREAL))) return 0;
   	else if ((atype & TYPE_STRING) && (a->type==STRING)) return 0;
   	return 1;
   }
   return 0;
}


static char check_datatype(struct var_entry *a,int atype,struct var_entry *o,int otype,int line)
{
	if (is_wrong_datatype(a,atype))
	{
      print_err(ERR_AKKU_DATATYPE,line);
      return 0;
   }
	if (is_wrong_datatype(o,otype))
	{
      print_err(ERR_OP_DATATYPE,line);
      return 0;
   }
	return 1;
}


static int find_label_pos(char *label)
{
	struct label_entry *currLabelEntry;
	
	currLabelEntry=firstLabelEntry;
	while (currLabelEntry)
	{
		if (!strncmp(currLabelEntry->labelName,label,MAX_VAR_LEN+1)) return currLabelEntry->line;
		currLabelEntry=currLabelEntry->next;
	}
	return -1;
}



static int find_fct_pos(char *fct)
{
	struct fct_entry *currFctEntry;
	char              fctName[MAX_VAR_LEN+1];
	char             *c;
	
	strncpy(fctName,fct,MAX_VAR_LEN);
	c=strstr(fctName,"(");
	if (c) *c=0;
	currFctEntry=firstFctEntry;
	while (currFctEntry)
	{
		if (!strncmp(currFctEntry->fctName,fctName,MAX_VAR_LEN+1)) return currFctEntry->line;
		currFctEntry=currFctEntry->next;
	}
	return -1;
}



static char setVarType(struct var_entry *newVarEntry,char *vtype)
{
   if (strstr(vtype,"BOOL")==vtype) newVarEntry->type=ilBOOL;
   else if (strstr(vtype,"USINT")==vtype) newVarEntry->type=USINT;
   else if (strstr(vtype,"SINT")==vtype) newVarEntry->type=SINT;
   else if (strstr(vtype,"UINT")==vtype) newVarEntry->type=ilUINT;
   else if (strstr(vtype,"INT")==vtype) newVarEntry->type=ilINT;
   else if (strstr(vtype,"UDINT")==vtype) newVarEntry->type=UDINT;
   else if (strstr(vtype,"DINT")==vtype) newVarEntry->type=DINT;
   else if (strstr(vtype,"ULINT")==vtype) newVarEntry->type=ULINT;
   else if (strstr(vtype,"LINT")==vtype) newVarEntry->type=LINT;
   else if (strstr(vtype,"REAL")==vtype) newVarEntry->type=REAL;
   else if (strstr(vtype,"LREAL")==vtype) newVarEntry->type=LREAL;
   else if (strstr(vtype,"STRING")==vtype) newVarEntry->type=STRING;
   else return 0;
   return 1;
}


static struct var_entry *create_new_variable(struct var_entry *firstVarEntry,char *name,int line)
{
	struct var_entry *currVarEntry,*newVarEntry;
	char              vtype[MAX_VAR_LEN+1],vname[MAX_VAR_LEN+1];
	
	sscanf(name,"%s : %s",vname,vtype);
	while ((strlen(vtype)>0) && ((vtype[strlen(vtype)-1]==' ') || (vtype[strlen(vtype)-1]=='\t')))
	 vtype[strlen(vtype)-1]=0;
	while ((strlen(vname)>0) && ((vname[strlen(vname)-1]==' ') || (vname[strlen(vname)-1]=='\t')))
	 vname[strlen(vname)-1]=0;

   currVarEntry=get_var_entry(firstVarEntry,vname);
   if (currVarEntry)
   {
	   print_err(ERR_VAR_DOUBLE_DEFINED,line);
	   return NULL;
   }	
	
	newVarEntry=malloc(sizeof(struct var_entry));
	if (!newVarEntry)
	{
		print_err(IERR_NO_VAR_MEM,line);
		return NULL;
	}
	memset(newVarEntry,0,sizeof(struct var_entry));
	strncpy(newVarEntry->varName,vname,MAX_VAR_LEN);
	newVarEntry->next=firstVarEntry;
   if (!setVarType(newVarEntry,vtype))
	{
		free(newVarEntry);
		print_err(ERR_UNKNOWN_TYPE,line);
		return NULL;
	}
	return newVarEntry;
}



static struct var_entry *create_map_variable(struct is_data *isHandle,struct var_entry *firstVarEntry,char *name,int line)
{
	struct var_entry *currVarEntry,*newVarEntry;
	char              nodeName[MAX_NODENAME_LENGTH+1],vname[MAX_VAR_LEN+1],vtype[MAX_VAR_LEN+1];
	int               nodeIO;
	
	sscanf(name,"%s : %s : %s",vname,vtype,nodeName);
	while ((strlen(nodeName)>0) && ((nodeName[strlen(nodeName)-1]==' ') || (nodeName[strlen(nodeName)-1]=='\t')))
	 nodeName[strlen(nodeName)-1]=0;
	while ((strlen(vname)>0) && ((vname[strlen(vname)-1]==' ') || (vname[strlen(vname)-1]=='\t')))
	 vname[strlen(vname)-1]=0;
	
	if ((strlen(nodeName)<6) || (nodeName[0]!='/'))
	{
      print_err(ERR_NO_NODENAME,line);
     	return NULL;	
	}

   nodeIO=atoi(nodeName+strlen(nodeName)-1);
	nodeName[strlen(nodeName)-2]=0;	

   currVarEntry=get_var_entry(firstVarEntry,vname);
   if (currVarEntry)
   {
	   print_err(ERR_VAR_DOUBLE_DEFINED,line);
	   return NULL;
   }	
	
	newVarEntry=malloc(sizeof(struct var_entry));
	if (!newVarEntry)
	{
		print_err(IERR_NO_VAR_MEM,line);
		return NULL;
	}
	memset(newVarEntry,0,sizeof(struct var_entry));
	strncpy(newVarEntry->varName,vname,MAX_VAR_LEN);
	newVarEntry->next=firstVarEntry;
   if (!setVarType(newVarEntry,vtype))
   {
      free(newVarEntry);
      print_err(ERR_UNKNOWN_TYPE,line);
      return NULL;
   }
	newVarEntry->isIO=nodeIO;
	newVarEntry->isNodeName=strdup(nodeName);
	newVarEntry->isHandle=isHandle;
	return newVarEntry;
}



static struct var_entry *create_param_list(const char *param_str,struct var_entry *firstVarEntry,int line)
{
   char             *c,*vname,*param_copy;
   struct var_entry *paramList=NULL,*newVarEntry,*currVarEntry;
   
   if (!param_str)
   {
   	// no parameters with function call
   	return NULL;
   }
   param_copy=strdup(param_str+1);
   c=param_copy;
   while ((c) && (*c!=0) && (*c!=')'))
   {
   	vname=c;
   	c=strstr(vname,":=");
   	if (!c)
   	{
   		print_err(WARN_PARAM_LIST,line);
   		free(param_copy);
	      return paramList;
   	}
   	*c=0;
   	c+=2;
   	newVarEntry=malloc(sizeof(struct var_entry));
   	if (!newVarEntry)
	   {
		   print_err(IERR_NO_VAR_MEM,line);
   		free(param_copy);
		   return paramList;
   	}
	   memset(newVarEntry,0,sizeof(struct var_entry));
   	strncpy(newVarEntry->varName,vname,MAX_VAR_LEN);
	   newVarEntry->next=paramList;
      paramList=newVarEntry;

		if (isnumber(c))
		{
			newVarEntry->vald=oapc_util_atof(c);
			newVarEntry->val=atol(c);
			if ((newVarEntry->vald>newVarEntry->val) || (newVarEntry->vald<newVarEntry->val)) newVarEntry->type=LREAL;
			else newVarEntry->type=DINT;
		}
		else if (*c=='"')
		{
			char *end;
			
		   newVarEntry->vals=strdup(c+1);
		   if (!newVarEntry->vals)
		   {
     			print_err(IERR_NO_MEM,line);
      		free(param_copy);
      	   return paramList;
		   }
		   end=strstr(newVarEntry->vals,"\"");
		   if (!end)
		   {
     			print_err(ERR_NO_STRING_TERM,line);
      		free(param_copy);
      	   return paramList;
		   }
		   else *end=0;
			newVarEntry->type=STRING;
		}
		else // variable
		{
			char  opvname[MAX_VAR_LEN+1];
			char *end;
			
			strncpy(opvname,c,MAX_VAR_LEN);
			end=strstr(opvname,",");
			if (end) *end=0;
			else
			{
   			end=strstr(opvname,")");
	   		if (end) *end=0;
			}
  			currVarEntry=get_var_entry(firstVarEntry,opvname);
   		if (!currVarEntry)
   		{
   			print_err(ERR_UNKNOWN_VAR,line);
      		free(param_copy);
   		   return paramList;	
   		}
   		if (currVarEntry->type==TYPE_UNDEFINED) print_err(ERR_UNINITIALIZED_VAR,line);
   		newVarEntry->type=currVarEntry->type;
	   	if (currVarEntry->type==STRING)
	   	{
	   		if (!currVarEntry->vals) print_err(ERR_UNINITIALIZED_VAR,line);
	   		else newVarEntry->vals=strdup(currVarEntry->vals);
	   	}
	   	else
	   	{  
		   	newVarEntry->val=currVarEntry->val;
  		   	newVarEntry->vald=currVarEntry->vald;
	   	}
		   check_range(newVarEntry,1,NULL,0,line);
		}
		c=strstr(c,",");
		if (c) c++;
   }	
	if (param_copy) free(param_copy);
   return paramList;
}



/**
 * @param currLine the line to begin/continue execution with
 * @param is_function the calling context, 0 in case the bottom level of the script is executed, 1 in case
 *        the code within a function block is done
 * @param firstVarEntry valid only in case of function block scope, here the parameters are handed over and are used
 *        as local variables automatically
 */
int execute_il(int currLine,char is_function,struct var_entry *firstVarEntry,char *file,int line)
{
	struct var_entry  akku;
	int    offset;
	struct var_entry *currVarEntry=NULL;
	
	memset(&akku,0,sizeof(struct var_entry));
	while (1)
	{
      if (verbose) printf("%d:\t%s\n",ilLNumber[currLine],ilLines[currLine]);
		if (ilLines[currLine]==0) currLine=0;
		if ((strstr(ilLines[currLine],"LD ")==ilLines[currLine]) || (strstr(ilLines[currLine],"LDN ")==ilLines[currLine]))
		{
		   if (strstr(ilLines[currLine],"LD ")==ilLines[currLine]) offset=3;
		   else offset=4;
		   
		   if (akku.vals) free(akku.vals);
		   akku.vals=NULL;
		   akku.type=TYPE_UNDEFINED;	
			if (isnumber(ilLines[currLine]+offset))
			{
				if (offset==3)
				{
					akku.vald=oapc_util_atof(ilLines[currLine]+offset);
					akku.val=atol(ilLines[currLine]+offset);
					if ((akku.vald>akku.val) || (akku.vald<akku.val)) akku.type=LREAL;
					else akku.type=LINT;
				}
				else
				{
					akku.val=~atol(ilLines[currLine]+offset);
					akku.type=LINT;
				}
			}
			else if (ilLines[currLine][offset]=='"')
			{
				akku.vals=strdup(ilLines[currLine]+offset+1);
			   if (!akku.vals)
			   {
     			   free_varlist(firstVarEntry);
        			print_err(IERR_NO_MEM,ilLNumber[currLine]);
  	      	   return OAPC_ERROR;	
			   }
				akku.vals[strlen(akku.vals)-1]=0;
				akku.type=STRING;
			}
			else if (strstr(ilLines[currLine]+offset,"TRUE")==ilLines[currLine]+offset)
			{
				if (offset==3) akku.val=1;
				else akku.val=0;
				akku.type=ilBOOL;
			}
			else if (strstr(ilLines[currLine]+offset,"FALSE")==ilLines[currLine]+offset)
			{
				if (offset==3) akku.val=0;
				else akku.val=1;
				akku.type=ilBOOL;
			}
			else
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+offset);
	   		if (!currVarEntry)
	   		{
   			   free_varlist(firstVarEntry);
	   			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
	   		   return OAPC_ERROR;	
	   		}
	   		if (currVarEntry->type==TYPE_UNDEFINED)
	   		{
   			   free_varlist(firstVarEntry);
	   			print_err(ERR_UNINITIALIZED_VAR,ilLNumber[currLine]);
	   		   return OAPC_ERROR;	
	   		}
	   		akku.type=currVarEntry->type;
			   if (offset==3)
			   {
			   	if (currVarEntry->type==STRING)
			   	{
			   		if (!currVarEntry->vals)
			   		{
         			   free_varlist(firstVarEntry);
	         			print_err(ERR_UNINITIALIZED_VAR,ilLNumber[currLine]);
	   		         return OAPC_ERROR;	
			   		}
			   		akku.vals=strdup(currVarEntry->vals);
			   	}
			   	else
			   	{  
   			   	akku.val=currVarEntry->val;
	   		   	akku.vald=currVarEntry->vald;
			   	}
			   }
			   else
			   {
			   	if ((akku.type==REAL) || (akku.type==LREAL))
			   	{
      			   free_varlist(firstVarEntry);
	      			print_err(ERR_REAL_TYPE_ERROR,ilLNumber[currLine]);
	      		   return OAPC_ERROR;				   		
			   	}			   	
			   	else if (akku.type==STRING)
			   	{
      			   free_varlist(firstVarEntry);
	      			print_err(ERR_STRING_TYPE_ERROR,ilLNumber[currLine]);
	      		   return OAPC_ERROR;				   		
			   	}			   	
			   	akku.val=~currVarEntry->val;
			   }
			   check_range(&akku,1,NULL,0,ilLNumber[currLine]);
			}
		}
		else if ((strstr(ilLines[currLine],"ST ")==ilLines[currLine]) || (strstr(ilLines[currLine],"STN ")==ilLines[currLine]))
		{
		   if (strstr(ilLines[currLine],"ST ")==ilLines[currLine]) offset=3;
		   else offset=4;
		   	
			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+offset);
   		if (!currVarEntry)
   		{
  			   free_varlist(firstVarEntry);
   			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
   		   return OAPC_ERROR;	
   		}
			if (offset==3)
			{
				if (currVarEntry->vals) free(currVarEntry->vals);
				currVarEntry->vals=NULL;
				if ((akku.type==STRING) || (currVarEntry->type==STRING))
				{
					if (akku.type!=currVarEntry->type)
					{
          			print_err(ERR_STRING_TYPE_ERROR,ilLNumber[currLine]);
						return OAPC_ERROR;
					}
					else currVarEntry->vals=strdup(akku.vals);
				}
				else
				{
   				if (((currVarEntry->type==REAL) || (currVarEntry->type==LREAL)) && ((akku.type==REAL) || (akku.type==LREAL)))
                currVarEntry->vald=akku.vald;
		   		else if ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL))
                currVarEntry->vald=(double)akku.val;
				   else if ((akku.type==REAL) || (akku.type==LREAL))
                currVarEntry->val=(int64_t)OAPC_ROUND(akku.vald,0);
               else
                currVarEntry->val=akku.val;
				}
			}
			else
		   {
		   	if ((akku.type==REAL) || (akku.type==LREAL))
		   	{
     			   free_varlist(firstVarEntry);
      			print_err(ERR_REAL_TYPE_ERROR,ilLNumber[currLine]);
      		   return OAPC_ERROR;				   		
		   	}			   	
		   	else if (akku.type==STRING)
		   	{
     			   free_varlist(firstVarEntry);
      			print_err(ERR_STRING_TYPE_ERROR,ilLNumber[currLine]);
      		   return OAPC_ERROR;				   		
		   	}			   	
		   	currVarEntry->val=~akku.val;
		   }			

         if (currVarEntry->isHandle) // check if variable has to be synchronized with the Interlock Server
         {
         	char                         toIO;
         	unsigned int                 ios;
         	void                        *values[MAX_NUM_IOS]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
         	struct oapc_digi_value_block digiValue;
         	struct oapc_num_value_block  numValue;
         	double                       d;

			   toIO=currVarEntry->isIO;
   			if (currVarEntry->type==STRING)
   			{
	   			ios=OAPC_CHAR_IO0;
		   		values[(int)toIO]=currVarEntry->vals;
			   }
   			else if (currVarEntry->type==ilBOOL)
	   		{
		   		ios=OAPC_DIGI_IO0;
			   	digiValue.digiValue=(unsigned char)currVarEntry->val;
				   values[(int)toIO]=&digiValue.digiValue;
   			}
	   		else
		   	{
			   	ios=OAPC_NUM_IO0;
				   if ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL)) d=currVarEntry->vald;
   				else d=(double)currVarEntry->val;
   				oapc_util_dbl_to_block(d,&numValue);
   				values[(int)toIO]=&numValue;
	   		}
		   	while (toIO>0)
			   {
				   toIO--;
   				ios=ios<<1;
	   		}
            oapc_ispace_set_data(currVarEntry->isHandle->handle,currVarEntry->isNodeName,ios,values,NULL);
         }

		   check_range(NULL,0,currVarEntry,1,ilLNumber[currLine]);
		}
		else if ((strstr(ilLines[currLine],"S ")==ilLines[currLine]) || (strstr(ilLines[currLine],"R ")==ilLines[currLine]))
		{
			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+2);
   		if (!currVarEntry)
   		{
  			   free_varlist(firstVarEntry);
   			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
   		   return OAPC_ERROR;	
   		}
   		if (!check_datatype(&akku,TYPE_BOOL,currVarEntry,TYPE_BOOL,ilLNumber[currLine])) return OAPC_ERROR;				   		
		   if (strstr(ilLines[currLine],"S ")==ilLines[currLine]) currVarEntry->val=(akku.val!=0);
			else currVarEntry->val=!(akku.val==0);
		}
		else if ((strstr(ilLines[currLine],"AND ")==ilLines[currLine]) || (strstr(ilLines[currLine],"ANDN ")==ilLines[currLine]))
		{
		   if (strstr(ilLines[currLine],"AND ")==ilLines[currLine]) offset=4;
		   else offset=5;
		   	
     		if (!check_datatype(&akku,TYPE_BOOL|TYPE_INT,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
			if (isnumber(ilLines[currLine]+offset))
			{
				if (offset==4) akku.val&=atol(ilLines[currLine]+offset);
				else akku.val&=~atol(ilLines[currLine]+offset);
			}
			else
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+offset);
      		if (!currVarEntry)
      		{
  		   	   free_varlist(firstVarEntry);
   		   	print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
      		   return OAPC_ERROR;	
      		}
       		if (!check_datatype(NULL,0,currVarEntry,TYPE_BOOL|TYPE_INT,ilLNumber[currLine])) return OAPC_ERROR;				   		
			   if (offset==4) akku.val&=currVarEntry->val;
			   else akku.val&=~currVarEntry->val;
			}
		}
		else if ((strstr(ilLines[currLine],"OR ")==ilLines[currLine]) || (strstr(ilLines[currLine],"ORN ")==ilLines[currLine]))
		{
		   if (strstr(ilLines[currLine],"OR ")==ilLines[currLine]) offset=3;
		   else offset=4;
		   	
     		if (!check_datatype(&akku,TYPE_BOOL|TYPE_INT,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
			if (isnumber(ilLines[currLine]+offset))
			{
				if (offset==3) akku.val|=atol(ilLines[currLine]+offset);
				else akku.val|=~atol(ilLines[currLine]+offset);
			}
			else
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+offset);
      		if (!currVarEntry)
      		{
  		   	   free_varlist(firstVarEntry);
   		   	print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
      		   return OAPC_ERROR;	
      		}
       		if (!check_datatype(NULL,0,currVarEntry,TYPE_BOOL|TYPE_INT,ilLNumber[currLine])) return OAPC_ERROR;				   		
			   if (offset==3) akku.val|=currVarEntry->val;
			   else akku.val|=~currVarEntry->val;
			}
		   check_range(&akku,1,NULL,0,ilLNumber[currLine]);
		}
		else if ((strstr(ilLines[currLine],"XOR ")==ilLines[currLine]) || (strstr(ilLines[currLine],"XORN ")==ilLines[currLine]))
		{

		   if (strstr(ilLines[currLine],"XOR ")==ilLines[currLine]) offset=4;
		   else offset=5;
		   	
     		if (!check_datatype(&akku,TYPE_BOOL|TYPE_INT,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
			if (isnumber(ilLines[currLine]+offset))
			{
				if (offset==4) akku.val^=atol(ilLines[currLine]+offset);
				else akku.val^=~atol(ilLines[currLine]+offset);
			}
			else
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+offset);
      		if (!currVarEntry)
      		{
  		   	   free_varlist(firstVarEntry);
   		   	print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
      		   return OAPC_ERROR;	
      		}
       		if (!check_datatype(NULL,0,currVarEntry,TYPE_BOOL|TYPE_INT,ilLNumber[currLine])) return OAPC_ERROR;				   		
			   if (offset==4) akku.val^=currVarEntry->val;
			   else akku.val^=~currVarEntry->val;
			}
		   check_range(&akku,1,NULL,0,ilLNumber[currLine]);
		}
		else if ((!strcmp(ilLines[currLine],"NOT")) || (!strcmp(ilLines[currLine],"!")))
		{
     		if (!check_datatype(&akku,TYPE_BOOL|TYPE_INT,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
			akku.val=~akku.val;
		   check_range(&akku,0,NULL,0,ilLNumber[currLine]);
		}
		else if (!strcmp(ilLines[currLine],"BOOL"))
		{
     		if (!check_datatype(&akku,TYPE_REAL|TYPE_INT,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;
     		if ((akku.type==REAL) || (akku.type==LREAL)) akku.val=(int64_t)OAPC_ROUND(akku.vald,0);
     		if (akku.val!=0) akku.val=1;				   		
			akku.type=ilBOOL;
		   check_range(&akku,0,NULL,0,ilLNumber[currLine]);
		}
		else if (!strcmp(ilLines[currLine],"USINT"))
		{
     		if (!check_datatype(&akku,TYPE_REAL|TYPE_INT,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;
     		if ((akku.type==REAL) || (akku.type==LREAL)) akku.val=(int64_t)OAPC_ROUND(akku.vald,0);	
			akku.type=USINT;
		   check_range(&akku,0,NULL,0,ilLNumber[currLine]);
		}
		else if (!strcmp(ilLines[currLine],"SINT"))
		{
     		if (!check_datatype(&akku,TYPE_REAL|TYPE_INT,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
     		if ((akku.type==REAL) || (akku.type==LREAL)) akku.val=(int64_t)OAPC_ROUND(akku.vald,0);
			akku.type=SINT;
		   check_range(&akku,0,NULL,0,ilLNumber[currLine]);
		}
		else if (!strcmp(ilLines[currLine],"UINT"))
		{
     		if (!check_datatype(&akku,TYPE_REAL|TYPE_INT,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
     		if ((akku.type==REAL) || (akku.type==LREAL)) akku.val=(int64_t)OAPC_ROUND(akku.vald,0);
			akku.type=ilUINT;
		   check_range(&akku,0,NULL,0,ilLNumber[currLine]);
		}
		else if (!strcmp(ilLines[currLine],"INT"))
		{
     		if (!check_datatype(&akku,TYPE_REAL|TYPE_INT,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
     		if ((akku.type==REAL) || (akku.type==LREAL)) akku.val=(int64_t)OAPC_ROUND(akku.vald,0);
			akku.type=ilINT;
		   check_range(&akku,0,NULL,0,ilLNumber[currLine]);
		}
		else if (!strcmp(ilLines[currLine],"UDINT"))
		{
     		if (!check_datatype(&akku,TYPE_REAL|TYPE_INT,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
     		if ((akku.type==REAL) || (akku.type==LREAL)) akku.val=(int64_t)OAPC_ROUND(akku.vald,0);
			akku.type=UDINT;
		   check_range(&akku,0,NULL,0,ilLNumber[currLine]);
		}
		else if (!strcmp(ilLines[currLine],"DINT"))
		{
     		if (!check_datatype(&akku,TYPE_REAL|TYPE_INT,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
     		if ((akku.type==REAL) || (akku.type==LREAL)) akku.val=(int64_t)OAPC_ROUND(akku.vald,0);
			akku.type=DINT;
		   check_range(&akku,0,NULL,0,ilLNumber[currLine]);
		}
		else if (!strcmp(ilLines[currLine],"ULINT"))
		{
     		if (!check_datatype(&akku,TYPE_REAL|TYPE_INT,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
     		if ((akku.type==REAL) || (akku.type==LREAL)) akku.val=(int64_t)OAPC_ROUND(akku.vald,0);
			akku.type=ULINT;
		   check_range(&akku,0,NULL,0,ilLNumber[currLine]);
		}
		else if (!strcmp(ilLines[currLine],"LINT"))
		{
     		if (!check_datatype(&akku,TYPE_REAL|TYPE_INT,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
     		if ((akku.type==REAL) || (akku.type==LREAL)) akku.val=(int64_t)OAPC_ROUND(akku.vald,0);
			akku.type=LINT;
		   check_range(&akku,0,NULL,0,ilLNumber[currLine]);
		}
		else if (!strcmp(ilLines[currLine],"REAL"))
		{
     		if (!check_datatype(&akku,TYPE_REAL|TYPE_INT,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
     		if ((akku.type!=REAL) && (akku.type!=LREAL)) akku.vald=(double)akku.val;				   		
			akku.type=REAL;
		   check_range(&akku,0,NULL,0,ilLNumber[currLine]);
		}
		else if (!strcmp(ilLines[currLine],"LREAL"))
		{
     		if (!check_datatype(&akku,TYPE_REAL|TYPE_INT,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
     		if ((akku.type!=REAL) && (akku.type!=LREAL)) akku.vald=(double)akku.val;				   		
			akku.type=LREAL;
		}
		else if (!strcmp(ilLines[currLine],"SIN"))
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
	   	if ((akku.type==REAL) || (akku.type==LREAL)) akku.vald=sin(akku.vald);
	   	else
	   	{
	   		akku.vald=sin((double)akku.val);
	   		akku.type=LREAL;
	   	}
		}
		else if (!strcmp(ilLines[currLine],"COS"))
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
	   	if ((akku.type==REAL) || (akku.type==LREAL)) akku.vald=cos(akku.vald);
	   	else
	   	{
	   		akku.vald=cos((double)akku.val);
	   		akku.type=LREAL;
	   	}
		}
		else if (!strcmp(ilLines[currLine],"LOG"))
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
	   	if ((akku.type==REAL) || (akku.type==LREAL)) akku.vald=log10(akku.vald);
	   	else
	   	{
	   		akku.vald=log10((double)akku.val);
	   		akku.type=LREAL;
	   	}
		}
		else if (!strcmp(ilLines[currLine],"LN"))
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
	   	if ((akku.type==REAL) || (akku.type==LREAL)) akku.vald=log(akku.vald);
	   	else
	   	{
	   		akku.vald=log((double)akku.val);
	   		akku.type=LREAL;
	   	}
		}
		else if (!strcmp(ilLines[currLine],"SQRT"))
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
	   	if ((akku.type==REAL) || (akku.type==LREAL)) akku.vald=sqrt(akku.vald);
	   	else
	   	{
	   		akku.vald=sqrt((double)akku.val);
	   		akku.type=LREAL;
	   	}
		}
		else if (strstr(ilLines[currLine],"POW ")==ilLines[currLine])
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
			if (isnumber(ilLines[currLine]+4))
			{
				if ((akku.type==REAL) || (akku.type==LREAL)) akku.vald=pow(akku.vald,oapc_util_atof(ilLines[currLine]+4));
				else akku.val=(int64_t)OAPC_ROUND(pow((double)akku.val,oapc_util_atof(ilLines[currLine]+4)),0);
			}
			else
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+4);
	   		if (!currVarEntry)
	   		{
     			   free_varlist(firstVarEntry);
      			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
   	   	   return OAPC_ERROR;	
	   		}
       		if (!check_datatype(NULL,0,currVarEntry,TYPE_INT|TYPE_REAL,ilLNumber[currLine])) return OAPC_ERROR;				   		
				if (((currVarEntry->type==REAL) || (currVarEntry->type==LREAL)) && ((akku.type==REAL) || (akku.type==LREAL)))
             akku.vald=pow(akku.vald,currVarEntry->vald);
				else if ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL))
             akku.val=(int64_t)OAPC_ROUND(pow((double)akku.val,currVarEntry->vald),0);
				else if ((akku.type==REAL) || (akku.type==LREAL))
             akku.vald=pow(akku.vald,(double)currVarEntry->val);
            else
             akku.val=(int64_t)OAPC_ROUND(pow((double)akku.val,(double)currVarEntry->val),0);
			}
		   check_range(&akku,1,NULL,0,ilLNumber[currLine]);
		}
		else if (!strcmp(ilLines[currLine],"STP"))
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;
     		oapc_thread_sleep((int)akku.val);
		}
		else if (strstr(ilLines[currLine],"ADD ")==ilLines[currLine])
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
			if (isnumber(ilLines[currLine]+4))
			{
				if ((akku.type==REAL) || (akku.type==LREAL)) akku.vald+=oapc_util_atof(ilLines[currLine]+4);
				else akku.val=(int64_t)OAPC_ROUND(akku.val+oapc_util_atof(ilLines[currLine]+4),0);
			}
			else
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+4);
	   		if (!currVarEntry)
	   		{
     			   free_varlist(firstVarEntry);
      			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
   	   	   return OAPC_ERROR;	
	   		}
       		if (!check_datatype(NULL,0,currVarEntry,TYPE_INT|TYPE_REAL,ilLNumber[currLine])) return OAPC_ERROR;				   		
				if (((currVarEntry->type==REAL) || (currVarEntry->type==LREAL)) && ((akku.type==REAL) || (akku.type==LREAL)))
             akku.vald+=currVarEntry->vald;
				else if ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL))
             akku.val=(int64_t)OAPC_ROUND(akku.val+currVarEntry->vald,0);
				else if ((akku.type==REAL) || (akku.type==LREAL))
             akku.vald+=currVarEntry->val;
            else
             akku.val+=currVarEntry->val;
			}
		   check_range(&akku,1,NULL,0,ilLNumber[currLine]);
		}
		else if (strstr(ilLines[currLine],"SUB ")==ilLines[currLine])
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
			if (isnumber(ilLines[currLine]+4))
			{
				if ((akku.type==REAL) || (akku.type==LREAL)) akku.vald-=oapc_util_atof(ilLines[currLine]+4);
				else akku.val=(int64_t)OAPC_ROUND(akku.val-oapc_util_atof(ilLines[currLine]+4),0);
			}
			else
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+4);
	   		if (!currVarEntry)
	   		{
     			   free_varlist(firstVarEntry);
      			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
   	   	   return OAPC_ERROR;	
	   		}
       		if (!check_datatype(NULL,0,currVarEntry,TYPE_INT|TYPE_REAL,ilLNumber[currLine])) return OAPC_ERROR;				   		
				if (((currVarEntry->type==REAL) || (currVarEntry->type==LREAL)) && ((akku.type==REAL) || (akku.type==LREAL)))
             akku.vald-=currVarEntry->vald;
				else if ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL))
             akku.val=(int64_t)OAPC_ROUND(akku.val-currVarEntry->vald,0);
				else if ((akku.type==REAL) || (akku.type==LREAL))
             akku.vald-=currVarEntry->val;
            else
             akku.val-=currVarEntry->val;
			}
		   check_range(&akku,1,NULL,0,ilLNumber[currLine]);
		}
		else if (strstr(ilLines[currLine],"MUL ")==ilLines[currLine])
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
			if (isnumber(ilLines[currLine]+4))
			{
				if ((akku.type==REAL) || (akku.type==LREAL)) akku.vald*=oapc_util_atof(ilLines[currLine]+4);
				else akku.val=(int64_t)OAPC_ROUND(akku.val*oapc_util_atof(ilLines[currLine]+4),0);
			}
			else //kiu98
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+4);
	   		if (!currVarEntry)
	   		{
     			   free_varlist(firstVarEntry);
      			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
   	   	   return OAPC_ERROR;	
	   		}
       		if (!check_datatype(NULL,0,currVarEntry,TYPE_INT|TYPE_REAL,ilLNumber[currLine])) return OAPC_ERROR;				   		
				if (((currVarEntry->type==REAL) || (currVarEntry->type==LREAL)) && ((akku.type==REAL) || (akku.type==LREAL)))
             akku.vald*=currVarEntry->vald;
				else if ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL))
             akku.val=(int64_t)OAPC_ROUND(akku.val*currVarEntry->vald,0);
				else if ((akku.type==REAL) || (akku.type==LREAL))
             akku.vald*=currVarEntry->val;
            else
             akku.val*=currVarEntry->val;
			}
		   check_range(&akku,1,NULL,0,ilLNumber[currLine]);
		}
		else if (strstr(ilLines[currLine],"DIV ")==ilLines[currLine])
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
			if (isnumber(ilLines[currLine]+4))
			{
				if ((akku.type==REAL) || (akku.type==LREAL)) akku.vald/=oapc_util_atof(ilLines[currLine]+4);
				else akku.val=(int64_t)OAPC_ROUND(akku.val/oapc_util_atof(ilLines[currLine]+4),0);
			}
			else
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+4);
	   		if (!currVarEntry)
	   		{
     			   free_varlist(firstVarEntry);
      			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
   	   	   return OAPC_ERROR;	
	   		}
       		if (!check_datatype(NULL,0,currVarEntry,TYPE_INT|TYPE_REAL,ilLNumber[currLine])) return OAPC_ERROR;				   		
				if (((currVarEntry->type==REAL) || (currVarEntry->type==LREAL)) && ((akku.type==REAL) || (akku.type==LREAL)))
             akku.vald/=currVarEntry->vald;
				else if ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL))
             akku.val=(int64_t)OAPC_ROUND(currVarEntry->vald,0);
				else if ((akku.type==REAL) || (akku.type==LREAL))
             akku.vald/=currVarEntry->val;
            else
             akku.val/=currVarEntry->val;
			}
		}
		else if (strstr(ilLines[currLine],"MOD ")==ilLines[currLine])
		{
     		if (!check_datatype(&akku,TYPE_INT,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
			if (isnumber(ilLines[currLine]+4))
			{
				akku.val%=atol(ilLines[currLine]+4);
			}
			else
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+4);
	   		if (!currVarEntry)
	   		{
     			   free_varlist(firstVarEntry);
      			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
   	   	   return OAPC_ERROR;	
	   		}
       		if (!check_datatype(NULL,0,currVarEntry,TYPE_INT,ilLNumber[currLine])) return OAPC_ERROR;				   		
            akku.val%=currVarEntry->val;
			}
		}
		else if (strstr(ilLines[currLine],"GT ")==ilLines[currLine])
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
			if (isnumber(ilLines[currLine]+3))
			{
				if ((akku.type==REAL) || (akku.type==LREAL)) akku.vald=(akku.vald>oapc_util_atof(ilLines[currLine]+3));
				else akku.val=(akku.val>atol(ilLines[currLine]+3));
   			akku.type=ilBOOL;
			}
			else
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+3);
	   		if (!currVarEntry)
	   		{
     			   free_varlist(firstVarEntry);
      			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
   	   	   return OAPC_ERROR;	
	   		}
       		if (!check_datatype(NULL,0,currVarEntry,TYPE_INT|TYPE_REAL,ilLNumber[currLine])) return OAPC_ERROR;				   		
	   		if (((akku.type==REAL) || (akku.type==LREAL)) && ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL)))
	   		 akku.val=(akku.vald>currVarEntry->vald);
	   		else if ((akku.type==REAL) || (akku.type==LREAL))
	   		 akku.val=(akku.vald>currVarEntry->val);
	   		else if ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL))
	   		 akku.val=(akku.val>currVarEntry->vald);
	   		else
	   		 akku.val=(akku.val>currVarEntry->val);
   			akku.type=ilBOOL;
			}
		}
		else if (strstr(ilLines[currLine],"GE ")==ilLines[currLine])
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
			if (isnumber(ilLines[currLine]+3))
			{
				if ((akku.type==REAL) || (akku.type==LREAL)) akku.vald=(akku.vald>=oapc_util_atof(ilLines[currLine]+3));
				else akku.val=(akku.val>=atol(ilLines[currLine]+3));
   			akku.type=ilBOOL;
			}
			else
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+3);
	   		if (!currVarEntry)
	   		{
     			   free_varlist(firstVarEntry);
      			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
   	   	   return OAPC_ERROR;	
	   		}
       		if (!check_datatype(NULL,0,currVarEntry,TYPE_INT|TYPE_REAL,ilLNumber[currLine])) return OAPC_ERROR;				   		
	   		if (((akku.type==REAL) || (akku.type==LREAL)) && ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL)))
	   		 akku.val=(akku.vald>=currVarEntry->vald);
	   		else if ((akku.type==REAL) || (akku.type==LREAL))
	   		 akku.val=(akku.vald>=currVarEntry->val);
	   		else if ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL))
	   		 akku.val=(akku.val>=currVarEntry->vald);
	   		else
	   		 akku.val=(akku.val>=currVarEntry->val);
   			akku.type=ilBOOL;
			}
		}
		else if (strstr(ilLines[currLine],"EQ ")==ilLines[currLine])
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL|TYPE_STRING,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;
     		if (akku.type==STRING)
     		{
     			if (ilLines[currLine][3]=='"') // local constant value
     			{
     				char *c;
     				
				   c=strdup(ilLines[currLine]+3+1);
				   if (!c)
				   {
        			   free_varlist(firstVarEntry);
         			print_err(IERR_NO_MEM,ilLNumber[currLine]);
   	      	   return OAPC_ERROR;	
				   }
				   c[strlen(c)-1]=0;
          		if (!strcmp(akku.vals,c)) akku.val=1;
          		else akku.val=0;				   		
   			   akku.type=ilBOOL;
               free(c);
     			}
     			else // variable
     			{
   		   	currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+3);
	   		   if (!currVarEntry)
   	   		{
        			   free_varlist(firstVarEntry);
         			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
   	      	   return OAPC_ERROR;	
	   		   }
          		if (!check_datatype(NULL,0,currVarEntry,TYPE_STRING,ilLNumber[currLine])) return OAPC_ERROR;
		   		if (!currVarEntry->vals)
		   		{
        			   free_varlist(firstVarEntry);
         			print_err(ERR_UNINITIALIZED_VAR,ilLNumber[currLine]);
   		         return OAPC_ERROR;	
		   		}
          		if (!strcmp(akku.vals,currVarEntry->vals)) akku.val=1;
          		else akku.val=0;				   		
   			   akku.type=ilBOOL;
     			}
     		}
     		else
     		{				   		
            if (isnumber(ilLines[currLine]+3))
   			{
	   			if ((akku.type==REAL) || (akku.type==LREAL)) akku.vald=(akku.vald==oapc_util_atof(ilLines[currLine]+3));
		   		else akku.val=(akku.val==atol(ilLines[currLine]+3));
   		   	akku.type=ilBOOL;
   			}
	   		else
		   	{
   		   	currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+3);
	   		   if (!currVarEntry)
   	   		{
        			   free_varlist(firstVarEntry);
         			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
   	      	   return OAPC_ERROR;	
	   		   }
          		if (!check_datatype(NULL,0,currVarEntry,TYPE_INT|TYPE_REAL,ilLNumber[currLine])) return OAPC_ERROR;				   		
	      		if (((akku.type==REAL) || (akku.type==LREAL)) && ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL)))
	      		 akku.val=(akku.vald==currVarEntry->vald);
	   	   	else if ((akku.type==REAL) || (akku.type==LREAL))
	   		    akku.val=(akku.vald==currVarEntry->val);
   	   		else if ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL))
	      		 akku.val=(akku.val==currVarEntry->vald);
	      		else
	   	   	 akku.val=(akku.val==currVarEntry->val);
   			   akku.type=ilBOOL;
		   	}
			}
		}
		else if (strstr(ilLines[currLine],"NE ")==ilLines[currLine])
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL|TYPE_STRING,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;
     		if (akku.type==STRING)
     		{
     			if (ilLines[currLine][3]=='"') // local constant value
     			{
     				char *c;
     				
				   c=strdup(ilLines[currLine]+3+1);
				   if (!c)
				   {
        			   free_varlist(firstVarEntry);
         			print_err(IERR_NO_MEM,ilLNumber[currLine]);
   	      	   return OAPC_ERROR;	
				   }
				   c[strlen(c)-1]=0;
          		if (strcmp(akku.vals,c)) akku.val=1;
          		else akku.val=0;				   		
   			   akku.type=ilBOOL;
               free(c);
     			}
     			else // variable
     			{
   		   	currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+3);
		   		if (!currVarEntry->vals)
		   		{
        			   free_varlist(firstVarEntry);
         			print_err(ERR_UNINITIALIZED_VAR,ilLNumber[currLine]);
   		         return OAPC_ERROR;	
		   		}
	   		   if (!currVarEntry)
   	   		{
        			   free_varlist(firstVarEntry);
         			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
   	      	   return OAPC_ERROR;	
	   		   }
          		if (!check_datatype(NULL,0,currVarEntry,TYPE_STRING,ilLNumber[currLine])) return OAPC_ERROR;
          		if (strcmp(akku.vals,currVarEntry->vals)) akku.val=1;
          		else akku.val=0;				   		
   			   akku.type=ilBOOL;
     			}
     		}
     		else
     		{				   		
            if (isnumber(ilLines[currLine]+3))
   			{
	   			if ((akku.type==REAL) || (akku.type==LREAL)) akku.vald=(akku.vald!=oapc_util_atof(ilLines[currLine]+3));
		   		else akku.val=(akku.val!=atol(ilLines[currLine]+3));
   		   	akku.type=ilBOOL;
   			}
	   		else
		   	{
   		   	currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+3);
	   		   if (!currVarEntry)
   	   		{
        			   free_varlist(firstVarEntry);
         			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
   	      	   return OAPC_ERROR;	
	   		   }
          		if (!check_datatype(NULL,0,currVarEntry,TYPE_INT|TYPE_REAL,ilLNumber[currLine])) return OAPC_ERROR;				   		
	      		if (((akku.type==REAL) || (akku.type==LREAL)) && ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL)))
	      		 akku.val=(akku.vald!=currVarEntry->vald);
	   	   	else if ((akku.type==REAL) || (akku.type==LREAL))
	   		    akku.val=(akku.vald!=currVarEntry->val);
   	   		else if ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL))
	      		 akku.val=(akku.val!=currVarEntry->vald);
	      		else
	   	   	 akku.val=(akku.val!=currVarEntry->val);
   			   akku.type=ilBOOL;
		   	}
			}
		}
		else if (strstr(ilLines[currLine],"LE ")==ilLines[currLine])
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
			if (isnumber(ilLines[currLine]+3))
			{
				if ((akku.type==REAL) || (akku.type==LREAL)) akku.vald=(akku.vald<=oapc_util_atof(ilLines[currLine]+3));
				else akku.val=(akku.val<=atol(ilLines[currLine]+3));
   			akku.type=ilBOOL;
			}
			else
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+3);
	   		if (!currVarEntry)
	   		{
     			   free_varlist(firstVarEntry);
      			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
   	   	   return OAPC_ERROR;	
	   		}
       		if (!check_datatype(NULL,0,currVarEntry,TYPE_INT|TYPE_REAL,ilLNumber[currLine])) return OAPC_ERROR;				   		
	   		if (((akku.type==REAL) || (akku.type==LREAL)) && ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL)))
	   		 akku.val=(akku.vald<=currVarEntry->vald);
	   		else if ((akku.type==REAL) || (akku.type==LREAL))
	   		 akku.val=(akku.vald<=currVarEntry->val);
	   		else if ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL))
	   		 akku.val=(akku.val<=currVarEntry->vald);
	   		else
	   		 akku.val=(akku.val<=currVarEntry->val);
   			akku.type=ilBOOL;
			}
		}
		else if (strstr(ilLines[currLine],"LT ")==ilLines[currLine])
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_REAL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   		
			if (isnumber(ilLines[currLine]+3))
			{
				if ((akku.type==REAL) || (akku.type==LREAL)) akku.vald=(akku.vald<oapc_util_atof(ilLines[currLine]+3));
				else akku.val=(akku.val<atol(ilLines[currLine]+3));
   			akku.type=ilBOOL;
			}
			else
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+3);
	   		if (!currVarEntry)
	   		{
     			   free_varlist(firstVarEntry);
      			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
   	   	   return OAPC_ERROR;	
	   		}
       		if (!check_datatype(NULL,0,currVarEntry,TYPE_INT|TYPE_REAL,ilLNumber[currLine])) return OAPC_ERROR;				   		
	   		if (((akku.type==REAL) || (akku.type==LREAL)) && ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL)))
	   		 akku.val=(akku.vald<currVarEntry->vald);
	   		else if ((akku.type==REAL) || (akku.type==LREAL))
	   		 akku.val=(akku.vald<currVarEntry->val);
	   		else if ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL))
	   		 akku.val=(akku.val<currVarEntry->vald);
	   		else
	   		 akku.val=(akku.val<currVarEntry->val);
   			akku.type=ilBOOL;
			}
		}
		else if (strstr(ilLines[currLine],"JMP ")==ilLines[currLine])
		{
			int newLine;
			
			newLine=find_label_pos(ilLines[currLine]+4);
			if (newLine<0)
			{
  			   free_varlist(firstVarEntry);
     			print_err(ERR_UNKNOWN_LABEL,ilLNumber[currLine]);
   		   return OAPC_ERROR;	
			}
			currLine=newLine;
		}
		else if (strstr(ilLines[currLine],"JMPC ")==ilLines[currLine])
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_BOOL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   					
			if (akku.val!=0)
			{
   			int newLine;
			
	   		newLine=find_label_pos(ilLines[currLine]+5);
		   	if (newLine<0)
			   {
  			      free_varlist(firstVarEntry);
        			print_err(ERR_UNKNOWN_LABEL,ilLNumber[currLine]);
      		   return OAPC_ERROR;	
		   	}
			   currLine=newLine;
			}
		}
		else if (strstr(ilLines[currLine],"JMPCN ")==ilLines[currLine])
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_BOOL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   					
			if (akku.val==0)
			{
   			int newLine;
			
	   		newLine=find_label_pos(ilLines[currLine]+6);
		   	if (newLine<0)
			   {
  			      free_varlist(firstVarEntry);
        			print_err(ERR_UNKNOWN_LABEL,ilLNumber[currLine]);
      		   return OAPC_ERROR;	
		   	}
			   currLine=newLine;
			}
		}
		else if (strstr(ilLines[currLine],"CAL ")==ilLines[currLine])
		{
			int               newLine,rc;
			struct var_entry *paramList=NULL;
			
			newLine=find_fct_pos(ilLines[currLine]+4);
			if (newLine<0)
			{
  			   free_varlist(firstVarEntry);
     			print_err(ERR_UNKNOWN_FCT,ilLNumber[currLine]);
   		   return OAPC_ERROR;	
			}
			paramList=create_param_list(strstr(ilLines[currLine],"("),firstVarEntry,ilLNumber[currLine]);
			rc=execute_il(newLine,1,paramList,__FILE__,__LINE__);
			if (rc!=OAPC_OK) return rc;
		}
		else if (strstr(ilLines[currLine],"CALC ")==ilLines[currLine])
		{
			int               newLine,rc;
			struct var_entry *paramList=NULL;
			
     		if (!check_datatype(&akku,TYPE_INT|TYPE_BOOL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   					
			if (akku.val!=0)
			{
   			newLine=find_fct_pos(ilLines[currLine]+5);
	   		if (newLine<0)
		   	{
  			      free_varlist(firstVarEntry);
     			   print_err(ERR_UNKNOWN_FCT,ilLNumber[currLine]);
      		   return OAPC_ERROR;	
	   		}
	   		paramList=create_param_list(strstr(ilLines[currLine],"("),firstVarEntry,ilLNumber[currLine]);
  		   	rc=execute_il(newLine,1,paramList,__FILE__,__LINE__);
 			   if (rc!=OAPC_OK) return rc;
			}
		}
		else if (strstr(ilLines[currLine],"CALCN ")==ilLines[currLine])
		{
			int               newLine,rc;
			struct var_entry *paramList=NULL;
			
     		if (!check_datatype(&akku,TYPE_INT|TYPE_BOOL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;				   					
			if (akku.val==0)
			{
   			newLine=find_fct_pos(ilLines[currLine]+6);
	   		if (newLine<0)
		   	{
  			      free_varlist(firstVarEntry);
     			   print_err(ERR_UNKNOWN_FCT,ilLNumber[currLine]);
      		   return OAPC_ERROR;	
	   		}
	   		paramList=create_param_list(strstr(ilLines[currLine],"("),firstVarEntry,ilLNumber[currLine]);
		   	rc=execute_il(newLine,1,paramList,__FILE__,__LINE__);
			   if (rc!=OAPC_OK) return rc;
			}
		}
		else if (!strcmp(ilLines[currLine],"MAP_IS0"))
		{
			int startLine=ilLNumber[currLine];
			
			if (is_function)
			{
   		       free_varlist(firstGVarEntry);
	   		   print_err(ERR_VAR_GLOBAL,ilLNumber[currLine]);
	   	       return OAPC_ERROR;
			}
			currLine++;
			while (strcmp(ilLines[currLine],"END_MAP"))
			{
			   struct var_entry *newVar;
				
    		   if (ilLines[currLine]==0)
    		   {
    			  free_varlist(firstGVarEntry);
	    		  print_err(ERR_NO_VAR_END,startLine);
	   		      return OAPC_ERROR;
   			   }
   			   newVar=create_map_variable(&isDatam[0],firstGVarEntry,ilLines[currLine],ilLNumber[currLine]);
   			   if (!newVar)
   			   {
   			      free_varlist(firstGVarEntry);
	   		      return OAPC_ERROR;
   			   }
   			   else firstGVarEntry=newVar;
   			   currLine++;
		    }
		 }
		else if (!strcmp(ilLines[currLine],"VAR_GLOBAL"))
		{
			int startLine=ilLNumber[currLine];
			
			if (is_function)
			{
   		   free_varlist(firstGVarEntry);
	   		print_err(ERR_VAR_GLOBAL,ilLNumber[currLine]);
	   	   return OAPC_ERROR;	
			}
			currLine++;
			while (strcmp(ilLines[currLine],"END_VAR"))
			{
				struct var_entry *newVar;
				
   			if (ilLines[currLine]==0)
   			{
   			   free_varlist(firstGVarEntry);
	   			print_err(ERR_NO_VAR_END,startLine);
	   		   return OAPC_ERROR;	
   			}
   			newVar=create_new_variable(firstGVarEntry,ilLines[currLine],ilLNumber[currLine]);
   			if (!newVar)
   			{
   			   free_varlist(firstGVarEntry);
	   		   return OAPC_ERROR;	
   			}
   			else firstGVarEntry=newVar;
   			currLine++;
			}
		}
		else if (!strcmp(ilLines[currLine],"VAR_LOCAL"))
		{
			struct var_entry *newVar;
			int               startLine=ilLNumber[currLine];
			
			if (!is_function)
			{
   		   free_varlist(firstGVarEntry);
	   		print_err(ERR_VAR_LOCAL,startLine);
	   	   return OAPC_ERROR;	
			}
			currLine++;
			while (strcmp(ilLines[currLine],"END_VAR"))
			{
   			if (ilLines[currLine]==0)
   			{
   			   free_varlist(firstVarEntry);
	   			print_err(ERR_NO_VAR_END,startLine);
	   		   return OAPC_ERROR;	
   			}
   			newVar=create_new_variable(firstVarEntry,ilLines[currLine],ilLNumber[currLine]);
   			if (!newVar)
   			{
   			   free_varlist(firstVarEntry);
	   		   return OAPC_ERROR;	
   			}
   			else firstVarEntry=newVar;
   			currLine++;
			}
		}		
		else if ((!is_function) && (strstr(ilLines[currLine],"FUNCTION_BLOCK ")==ilLines[currLine])) // drop the function block on top level
		{
			currLine++;
			while (strcmp(ilLines[currLine],"END_FUNCTION")) currLine++;
		}		
		else if (!strcmp(ilLines[currLine],"PRINT")) // no operand
		{
			if (akku.type==ilBOOL)
			{
				if (akku.val) printf("TRUE\n");
				else printf("FALSE\n");
			}
			else if ((akku.type==REAL) || (akku.type==LREAL)) printf("%f\n",akku.vald);
			else if (akku.type==STRING)
			{
	   		if (!akku.vals)
	   		{
     			   free_varlist(firstVarEntry);
        			print_err(ERR_UNINITIALIZED_VAR,ilLNumber[currLine]);
  		         return OAPC_ERROR;	
	   		}
				printf("\"%s\"\n",akku.vals);
			}
			else printf("%lld\n",(int64_t)OAPC_ROUND(akku.val,0));
		}
		else if (strstr(ilLines[currLine],"PRINT ")==ilLines[currLine])
		{
			struct var_entry opVar;

			if (isnumber(ilLines[currLine]+6))
			{
				opVar.vald=oapc_util_atof(ilLines[currLine]+6);
				opVar.val=atol(ilLines[currLine]+6);
				if ((opVar.vald>opVar.val) || (opVar.vald<opVar.val)) printf("%f\n",opVar.vald);
				else printf("%lld\n",(int64_t)OAPC_ROUND(opVar.val,0));
			}
			else if (ilLines[currLine][6]=='"')
			{
				opVar.vals=strdup(ilLines[currLine]+6+1);
			   if (!opVar.vals) print_err(IERR_NO_MEM,ilLNumber[currLine]);
			   else
			   {
			   	opVar.vals[strlen(opVar.vals)-1]=0;
			   	printf("%s\n",opVar.vals);
			   	free(opVar.vals);
			   }
			}
			else
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+6);
	   		if (!currVarEntry)
	   		{
   			   free_varlist(firstVarEntry);
	   			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
	   		   return OAPC_ERROR;	
	   		}
	   		if (currVarEntry->type==TYPE_UNDEFINED)
	   		{
   			   free_varlist(firstVarEntry);
	   			print_err(ERR_UNINITIALIZED_VAR,ilLNumber[currLine]);
	   		   return OAPC_ERROR;	
	   		}
		   	if (currVarEntry->type==STRING)
		   	{
		   		if (!currVarEntry->vals)
		   		{
        			   free_varlist(firstVarEntry);
         			print_err(ERR_UNINITIALIZED_VAR,ilLNumber[currLine]);
   		         return OAPC_ERROR;	
		   		}
		   		printf("%s\n",currVarEntry->vals);
		   	}
		   	else if (currVarEntry->type==ilBOOL)
   			{
	   			if (currVarEntry->val) printf("TRUE\n");
				   else printf("FALSE\n");
			   }
			   else if ((currVarEntry->type==REAL) || (currVarEntry->type==LREAL)) printf("%f\n",currVarEntry->vald);
			   else printf("%lld\n",(int64_t)OAPC_ROUND(currVarEntry->val,0));
			}			
		}
		else if (!strcmp(ilLines[currLine],"EXIT"))
		{
			free_varlist(firstVarEntry);
			return OAPC_ERROR; // it is not an error but we have to leave the complete program flow here
		}		
		else if ((!strcmp(ilLines[currLine],"RET")) || (!strcmp(ilLines[currLine],"END_FUNCTION")))
		{
			if (!is_function)
			{
   		   free_varlist(firstGVarEntry);
	   		print_err(ERR_VAR_NOT_IN_FCT,ilLNumber[currLine]);
	   	   return OAPC_ERROR;	
			}
			free_varlist(firstVarEntry);
			return OAPC_OK;
		}		
		else if (!strcmp(ilLines[currLine],"RETC"))
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_BOOL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;
			if (!is_function)
			{
   		   free_varlist(firstGVarEntry);
	   		print_err(ERR_VAR_NOT_IN_FCT,ilLNumber[currLine]);
	   	   return OAPC_ERROR;	
			}
     		if (akku.val!=0)
     		{
     			free_varlist(firstVarEntry);
			   return OAPC_OK;
     		}
		}		
		else if (!strcmp(ilLines[currLine],"RETCN"))
		{
     		if (!check_datatype(&akku,TYPE_INT|TYPE_BOOL,NULL,0,ilLNumber[currLine])) return OAPC_ERROR;
			if (!is_function)
			{
   		   free_varlist(firstGVarEntry);
	   		print_err(ERR_VAR_NOT_IN_FCT,ilLNumber[currLine]);
	   	   return OAPC_ERROR;	
			}
     		if (akku.val==0)
     		{
     			free_varlist(firstVarEntry);
			   return OAPC_OK;
     		}
		}		
		else if (!strcmp(ilLines[currLine],"IS0C"))
		{
			int rc;
			
		   if (isData[0].handle) oapc_ispace_disconnect(isData[0].handle);
		   isData[0].handle=oapc_ispace_get_instance();
		   if (isDatai[0].handle) oapc_ispace_disconnect(isDatai[0].handle);
		   isDatai[0].handle=oapc_ispace_get_instance();
		   if (isDatam[0].handle) oapc_ispace_disconnect(isDatam[0].handle);
		   isDatam[0].handle=oapc_ispace_get_instance();
		   akku.val=0;
		   akku.type=ilBOOL;
		   if ((isData[0].handle) && (isDatai[0].handle) && (isDatam[0].handle))
		   {
   	      rc=oapc_ispace_set_recv_callback(isDatam[0].handle,ispace_callback0m);
   	      if (rc==OAPC_OK)
	         {
               rc=oapc_ispace_connect(isDatam[0].handle,NULL,0,NULL);
               if (rc==OAPC_OK)
               {
      				if (oapc_ispace_request_all_data(isDatam[0].handle,NULL)==OAPC_OK)
      				{
      					
         		      rc=oapc_ispace_set_recv_callback(isDatai[0].handle,ispace_callback0i);
		               if (rc==OAPC_OK)
         		      {
                        rc=oapc_ispace_connect(isDatai[0].handle,NULL,0,NULL);
                        if (rc==OAPC_OK)
                        {
               		      rc=oapc_ispace_set_recv_callback(isData[0].handle,ispace_callback0);
		                     if (rc==OAPC_OK)
         	      	      {
                              rc=oapc_ispace_connect(isData[0].handle,NULL,0,NULL);
                              if (rc==OAPC_OK)
                              {
                              	akku.val=1;
                              }
         	      	      }
                        }
                     }
		            }
               }
		      }
		   }
		   else
		   {
  			   free_varlist(firstVarEntry);
    			print_err(IERR_NO_MEM,ilLNumber[currLine]);
      	   return OAPC_ERROR;			   	
		   }
/*   OAPC_EXT_API int   oapc_ispace_set_data(void *handle,const char *nodeName,unsigned int ios,void *values[MAX_NUM_IOS],struct oapc_ispace_auth *auth);
   OAPC_EXT_API int   oapc_ispace_request_data(void *handle,const char *nodeName,struct oapc_ispace_auth *auth);
   OAPC_EXT_API int      */
		
		}		
		else if (!strcmp(ilLines[currLine],"IS0RA"))
		{
			akku.val=0;
			akku.type=ilBOOL;
			if (isData[0].handle)
			{
				if (oapc_ispace_request_all_data(isData[0].handle,NULL)==OAPC_OK)
             akku.val=1;
			}
		}
		else if (strstr(ilLines[currLine],"LDIS0 ")==ilLines[currLine])
		{
			char nodeName[MAX_NODENAME_LENGTH+1];
				
		   if (akku.vals) free(akku.vals);
		   akku.vals=NULL;	
			if (isnumber(ilLines[currLine]+6))
			{
  			   free_varlist(firstVarEntry);
   			print_err(ERR_NO_STRING_OP,ilLNumber[currLine]);
   		   return OAPC_ERROR;	
			}
			else if (ilLines[currLine][6]=='"') // node name given directly
			{
				strncpy(nodeName,ilLines[currLine]+6+1,MAX_NODENAME_LENGTH);
				nodeName[strlen(nodeName)-1]=0;
			}
			else // node name stored in variable
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+6);
	   		if (!currVarEntry)
	   		{
   			   free_varlist(firstVarEntry);
	   			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
	   		   return OAPC_ERROR;	
	   		}
	   		if (currVarEntry->type==TYPE_UNDEFINED)
	   		{
   			   free_varlist(firstVarEntry);
	   			print_err(ERR_UNINITIALIZED_VAR,ilLNumber[currLine]);
	   		   return OAPC_ERROR;	
	   		}
		   	if (currVarEntry->type==STRING)
		   	{
		   		if (!currVarEntry->vals)
		   		{
        			   free_varlist(firstVarEntry);
         			print_err(ERR_UNINITIALIZED_VAR,ilLNumber[currLine]);
   		         return OAPC_ERROR;	
		   		}
		   		strncpy(nodeName,currVarEntry->vals,MAX_NODENAME_LENGTH);
		   	}
		   	else
		   	{
     			   free_varlist(firstVarEntry);
      			print_err(ERR_NO_STRING_OP,ilLNumber[currLine]);
   	   	   return OAPC_ERROR;	
		   	}		   		
			}
			if ((strlen(nodeName)<6) || (nodeName[0]!='/'))
			{
     	      free_varlist(firstVarEntry);
            print_err(ERR_NO_NODENAME,ilLNumber[currLine]);
   	   	return OAPC_ERROR;	
			}
			oapc_thread_mutex_lock(g_isMutex[0]);
   		if (isDatai[0].fromIS.vals) free(isDatai[0].fromIS.vals);
   		isDatai[0].fromIS.vals=NULL;
			isDatai[0].fromISIO=atoi(nodeName+strlen(nodeName)-1);
        	isDatai[0].fromISDone=0;
			nodeName[strlen(nodeName)-2]=0;
			if (oapc_ispace_request_data(isDatai[0].handle,nodeName,NULL)==OAPC_OK)
			{
				int cnt;
				
				for (cnt=0; cnt<400; cnt++) // max ten seconds
				{
					if (isDatai[0].fromISDone) break;
					oapc_thread_sleep(25);
				}
				if (isDatai[0].fromISDone)
				{
					akku.val=isDatai[0].fromIS.val;
					akku.vald=isDatai[0].fromIS.vald;
					if (isDatai[0].fromIS.vals) akku.vals=strdup(isDatai[0].fromIS.vals);
					akku.type=isDatai[0].fromIS.type;
				}
				else
				{
					// implement error handler here?
				}
			}
			oapc_thread_mutex_unlock(g_isMutex[0]);
		}
		else if (strstr(ilLines[currLine],"STIS0 ")==ilLines[currLine])
		{
			char                         nodeName[MAX_NODENAME_LENGTH+1];
			int                          toIO;
			unsigned int                 ios;
         void                        *values[MAX_NUM_IOS]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
         struct oapc_digi_value_block digiValue;
         double                       d;
         struct oapc_num_value_block  numValue;
         				
			if (isnumber(ilLines[currLine]+6))
			{
  			   free_varlist(firstVarEntry);
   			print_err(ERR_NO_STRING_OP,ilLNumber[currLine]);
   		   return OAPC_ERROR;	
			}
			else if (ilLines[currLine][6]=='"') // node name given directly
			{
				strncpy(nodeName,ilLines[currLine]+6+1,MAX_NODENAME_LENGTH);
				nodeName[strlen(nodeName)-1]=0;
			}
			else // node name given in variable
			{
   			currVarEntry=get_var_entry(firstVarEntry,ilLines[currLine]+6);
	   		if (!currVarEntry)
	   		{
   			   free_varlist(firstVarEntry);
	   			print_err(ERR_UNKNOWN_VAR,ilLNumber[currLine]);
	   		   return OAPC_ERROR;	
	   		}
	   		if (currVarEntry->type==TYPE_UNDEFINED)
	   		{
   			   free_varlist(firstVarEntry);
	   			print_err(ERR_UNINITIALIZED_VAR,ilLNumber[currLine]);
	   		   return OAPC_ERROR;	
	   		}
		   	if (currVarEntry->type==STRING)
		   	{
		   		if (!currVarEntry->vals)
		   		{
        			   free_varlist(firstVarEntry);
         			print_err(ERR_UNINITIALIZED_VAR,ilLNumber[currLine]);
   		         return OAPC_ERROR;	
		   		}
		   		strncpy(nodeName,currVarEntry->vals,MAX_NODENAME_LENGTH);
		   	}
		   	else
		   	{
     			   free_varlist(firstVarEntry);
      			print_err(ERR_NO_STRING_OP,ilLNumber[currLine]);
   	   	   return OAPC_ERROR;	
		   	}		   						
			}
			if ((strlen(nodeName)<6) || (nodeName[0]!='/'))
			{
     	      free_varlist(firstVarEntry);
            print_err(ERR_NO_NODENAME,ilLNumber[currLine]);
   	   	return OAPC_ERROR;	
			}
			toIO=atoi(nodeName+strlen(nodeName)-1);
			nodeName[strlen(nodeName)-2]=0;

			if (akku.type==STRING)
			{
				ios=OAPC_CHAR_IO0;
				values[toIO]=akku.vals;
			}
			else if (akku.type==ilBOOL)
			{
				ios=OAPC_DIGI_IO0;
				digiValue.digiValue=(unsigned char)akku.val;
				values[toIO]=&digiValue.digiValue;
			}
			else
			{
				ios=OAPC_NUM_IO0;
				if ((akku.type==REAL) || (akku.type==LREAL)) d=akku.vald;
				else d=(double)akku.val;
  				oapc_util_dbl_to_block(d,&numValue);
  				values[(int)toIO]=&numValue;
			}
			while (toIO>0)
			{
				toIO--;
				ios=ios<<1;
			}
         akku.type=ilBOOL;
         
         toIO=oapc_ispace_set_data(isDatai[0].handle,nodeName,ios,values,NULL);
         if (toIO==OAPC_OK) akku.val=1;
         else
         {
         	akku.val=0;
         }
		}
		else if (!strcmp(ilLines[currLine],"IS0D"))
		{
			if (isData[0].handle) oapc_ispace_disconnect(isData[0].handle);
			isData[0].handle=NULL; 
			if (isDatai[0].handle) oapc_ispace_disconnect(isDatai[0].handle);
			isDatai[0].handle=NULL; 
		}
		else
		{
			if (ilLines[currLine][strlen(ilLines[currLine])-1]!=':') // it is not a label
			{
   			free_varlist(firstVarEntry);
	   		printf("Error: unknown command \"%s\" or missing operand at line %d\n",ilLines[currLine],ilLNumber[currLine]);
		   	return OAPC_ERROR;
			}
		}
		currLine++;
	}
	
	
}



static int load_il(char *fname)
{
   FILE *FHandle;
   char  line[500+1];
   int   len,lnCnt=0;
   char *c,*t;
   
   lastLine=0;
   FHandle=fopen(fname,"r");
   if (FHandle)
   {
   	fseek(FHandle,0,SEEK_END);
   	len=ftell(FHandle);
   	fseek(FHandle,0,SEEK_SET);
   	ilBuffer=malloc(len);
   	if (!ilBuffer)
   	{
   		fclose(FHandle);
   		return OAPC_ERROR_NO_MEMORY;
   	}
   	memset(ilBuffer,0,len);
   	t=ilBuffer;
   	while (fgets(line,500,FHandle))
   	{
   		lnCnt++;
   		c=line;
   		while ((*c==' ') || (*c=='\t')) c++;
   		while ((strlen(c)>0) && 
   		       ((c[strlen(c)-1]=='\r') || (c[strlen(c)-1]=='\n') || 
   		        (c[strlen(c)-1]==' ') || (c[strlen(c)-1]=='\t'))) c[strlen(c)-1]=0;
   		if ((*c!=0) && (*c!='#'))
   		{
   			if ((strlen(c)>1) && (c[strlen(c)-1]==':')) // it is a label, put it into the label table
   			{
   				struct label_entry *newLabelEntry;
   				
   				newLabelEntry=malloc(sizeof(struct label_entry));
   				if (!newLabelEntry)
   				{
      				printf("Fatal error: not enough memory to create label table!\n");
   					return OAPC_ERROR_NO_MEMORY;
   				}
   				c[strlen(c)-1]=0;
   				strncpy(newLabelEntry->labelName,c,MAX_VAR_LEN);
   				newLabelEntry->line=lastLine-1;
   				newLabelEntry->next=firstLabelEntry;
   				firstLabelEntry=newLabelEntry;
   			}
   			else
   			{
               if (strstr(c,"FUNCTION_BLOCK")==c)
      			{
      				char             *f;
   	   			struct fct_entry *newFctEntry;
   				
   		   		f=c+15;
         	   	while ((strlen(f)>0) && 
   	      	          ((f[strlen(f)-1]=='\r') || (f[strlen(f)-1]=='\n') || 
   		                 (f[strlen(f)-1]==' ') || (f[strlen(f)-1]=='\t'))) f[strlen(f)-1]=0;
   				
      				newFctEntry=malloc(sizeof(struct fct_entry));
      				if (!newFctEntry)
   	   			{
      	   			printf("Fatal error: not enough memory to create function block table!\n");
   			   		return OAPC_ERROR_NO_MEMORY;
   				   }
      				strncpy(newFctEntry->fctName,f,MAX_VAR_LEN);
      				f=strstr(newFctEntry->fctName,"(");
   	   			if (f) *f=0;
   		   		newFctEntry->line=lastLine+1;
   			   	newFctEntry->next=firstFctEntry;
   				   firstFctEntry=newFctEntry;
      			}
   				
      			strcpy(t,c);
      			ilLines[lastLine]=t;
   	   		ilLNumber[lastLine]=lnCnt;
   		   	if (strstr(ilLines[lastLine],"is0_cb_digi_data"))
   			   {
   				   if (isData[0].digiFBlock!=0)
      				{
         	   		print_err(ERR_FCT_DOUBLE_DEFINED,lnCnt);
          				return OAPC_ERROR;
   		   		}
   			   	else isData[0].digiFBlock=lastLine+1;
      			}
      			else if (strstr(ilLines[lastLine],"is0_cb_num_data"))
   	   		{
   		   		if (isData[0].numFBlock!=0)
   			   	{
         	   		print_err(ERR_FCT_DOUBLE_DEFINED,lnCnt);
          				return OAPC_ERROR;
   	   			}
   		   		else isData[0].numFBlock=lastLine+1;
   			   }
      			else if (strstr(ilLines[lastLine],"is0_cb_char_data"))
      			{
   	   			if (isData[0].charFBlock!=0)
   		   		{
      	      		print_err(ERR_FCT_DOUBLE_DEFINED,lnCnt);
       			   	return OAPC_ERROR;
      				}
      				else isData[0].charFBlock=lastLine+1;
   	   		}
      			lastLine++;
   			}
   			if (lastLine>=MAX_IL_LINES)
   			{
   				printf("Fatal error: maximum number of lines exceeded!\n");
   				return OAPC_ERROR_RESOURCE;
   			}
   			t+=strlen(c)+1;
   		} 
   	}
   	ilLines[lastLine]=NULL;
   }
   else
   {
   	printf("Fatal error: could not open file %s!\n",fname);
   	return OAPC_ERROR_RESOURCE;
   }
   return OAPC_OK;
}



int main(int argc, char *argv[])
{
	int i;
#ifdef ENV_WINDOWS
   WSADATA      wsaData;

   WSAStartup((MAKEWORD(1, 1)),&wsaData);
#endif
	
	for (i=0; i<MAX_IS_CONNECTIONS; i++)
	{
		memset(&isData[i],0,sizeof(struct is_data));
		memset(&isDatai[i],0,sizeof(struct is_data));
		isDatai[i].fromISDone=1;
		g_isMutex[i]=oapc_thread_mutex_create();
	}
	
   if (argc<2)
   {
      printf("ERROR: missing parameters\n\n");
      usage(argv[0]);
      return -1;
   }
   getSwitches(argc,argv);
   if (!ilFName)
   {
      printf("ERROR: no IL file specified\n\n");
      usage(argv[0]);
      return -1;
   }
	if (load_il(ilFName)==OAPC_OK)
	{
	   free(ilFName);
	   execute_il(0,0,NULL,__FILE__,__LINE__);
	}
	if (ilBuffer) free(ilBuffer);
	for (i=0; i<MAX_IS_CONNECTIONS; i++)
	{
	   oapc_thread_mutex_release(g_isMutex[i]);
	}
#ifdef ENV_WINDOWS
   WSACleanup();
#endif	
	return 0;
}


