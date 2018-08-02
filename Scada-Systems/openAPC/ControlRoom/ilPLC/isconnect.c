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

#ifndef ENV_WINDOWS
#include <strings.h>
#else
#define snprintf _snprintf
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liboapc.h"
#include "ildefs.h"
#include "isconnect.h"


static struct var_entry *new_var_entry(struct var_entry *paramList,char *vname)
{
	struct var_entry *newVarEntry;
	
   newVarEntry=malloc(sizeof(struct var_entry));
   if (!newVarEntry)
	{
      return NULL;
   }
	memset(newVarEntry,0,sizeof(struct var_entry));
   strncpy(newVarEntry->varName,vname,MAX_VAR_LEN);
	newVarEntry->next=paramList;
   return newVarEntry;
}



void ispace_callback0i(void *handle,char *nodeName,unsigned int cmd,unsigned int ios,void *values[MAX_NUM_IOS])
{
	unsigned int                  bits =0x00010101;
	int                           i;
	struct oapc_digi_value_block *digiValue;
	struct oapc_num_value_block  *numValue;

   isDatai[0].fromIS.type=TYPE_UNDEFINED;
   for (i=0; i<MAX_NUM_IOS; i++)
  	{
   	if (i==isDatai[0].fromISIO)
	   {
		   if (ios & bits & OAPC_DIGI_IO_MASK)
  			{
   			digiValue=(struct oapc_digi_value_block*)values[i];
	   		isDatai[0].fromIS.type=ilBOOL;
		   	isDatai[0].fromIS.val=digiValue->digiValue;
  			}
   		else if (ios & bits & OAPC_NUM_IO_MASK)
	   	{
		   	numValue=(struct oapc_num_value_block*)values[i];
			   isDatai[0].fromIS.type=LREAL;
  				isDatai[0].fromIS.vald=oapc_util_block_to_dbl(numValue);
   		}
	   	else if (ios & bits & OAPC_CHAR_IO_MASK)
		   {
  				isDatai[0].fromIS.type=STRING;
   			isDatai[0].fromIS.vals=strdup(values[i]);
	   	}
		   break;
  		}
   	bits=bits<<1;
   }
	isDatai[0].fromISDone=1;
}



void ispace_callback0m(void *handle,char *nodeName,unsigned int cmd,unsigned int ios,void *values[MAX_NUM_IOS])
{
	unsigned int                  bits;
	int                           i;
	struct oapc_digi_value_block *digiValue;
	struct oapc_num_value_block  *numValue;

 	struct var_entry *currVarEntry;
   	
  	currVarEntry=firstGVarEntry;
  	while (currVarEntry)
  	{
  		if ((currVarEntry->isHandle==&isDatam[0]) && (!strncmp(currVarEntry->isNodeName,nodeName,MAX_NODENAME_LENGTH)))
  		{
  			bits=0x00010101;
  			
            for (i=0; i<MAX_NUM_IOS; i++)
        	{
               if (i==currVarEntry->isIO)
	           {
     		      if (ios & bits & OAPC_DIGI_IO_MASK)
        		  {
 	   	      	     if (currVarEntry->type==ilBOOL)
 	   	      	     {
         		        digiValue=(struct oapc_digi_value_block*)values[i];
		   	            currVarEntry->val=digiValue->digiValue;
 	   	      	     }
        		  }
         		  else if (ios & bits & OAPC_NUM_IO_MASK)
	   	          {
     			     numValue=(struct oapc_num_value_block*)values[i];
                     if (currVarEntry->type==USINT) currVarEntry->val=(unsigned char)oapc_util_block_to_dbl(numValue);
                     else if (currVarEntry->type==SINT) currVarEntry->val=(char)oapc_util_block_to_dbl(numValue);
                     else if (currVarEntry->type==ilUINT) currVarEntry->val=(unsigned short)oapc_util_block_to_dbl(numValue);
                     else if (currVarEntry->type==ilINT) currVarEntry->val=(short)oapc_util_block_to_dbl(numValue);
                     else if (currVarEntry->type==UDINT) currVarEntry->val=(unsigned int)oapc_util_block_to_dbl(numValue);
                     else if (currVarEntry->type==DINT) currVarEntry->val=(int)oapc_util_block_to_dbl(numValue);
                     else if (currVarEntry->type==ULINT) currVarEntry->val=(int64_t)oapc_util_block_to_dbl(numValue);
                     else if (currVarEntry->type==LINT) currVarEntry->val=(int64_t)oapc_util_block_to_dbl(numValue);
                     else if (currVarEntry->type==REAL) currVarEntry->vald=(float)oapc_util_block_to_dbl(numValue);
                     else if (currVarEntry->type==LREAL) currVarEntry->vald=oapc_util_block_to_dbl(numValue);
     	   		  }
	         	  else if (ios & bits & OAPC_CHAR_IO_MASK)
		          {
        			 if (currVarEntry->type==STRING)
         			  currVarEntry->vals=strdup(values[i]);
		          }
		          break;
	           }
     	   	   bits=bits<<1;
            }
  		}   		
  		currVarEntry=currVarEntry->next;
  	}
}



void ispace_callback0(void *handle,char *nodeName,unsigned int cmd,unsigned int ios,void *values[MAX_NUM_IOS])
{
	unsigned int                  bits =0x00010101;
	int                           i;
	struct var_entry             *newVarEntry,*digiParamList=NULL,*numParamList=NULL,*charParamList=NULL;
	char                          vname[101];
	struct oapc_digi_value_block *digiValue;
	struct oapc_num_value_block  *numValue;

	for (i=0; i<MAX_NUM_IOS; i++)
	{
		if ((isData[0].digiFBlock>0) && (ios & bits & OAPC_DIGI_IO_MASK))
		{
			snprintf(vname,100,"digi%d",i);
			newVarEntry=new_var_entry(digiParamList,vname);
			if (newVarEntry)
			{
				digiParamList=newVarEntry;
				newVarEntry->type=ilBOOL;
				digiValue=(struct oapc_digi_value_block*)values[i];
				newVarEntry->val=digiValue->digiValue;
			//int 
			}
		}
		else if ((isData[0].numFBlock>0) && (ios & bits & OAPC_NUM_IO_MASK))
		{
			snprintf(vname,100,"num%d",i);
			newVarEntry=new_var_entry(numParamList,vname);
			if (newVarEntry)
			{
				numParamList=newVarEntry;
				newVarEntry->type=LREAL;
				numValue=(struct oapc_num_value_block*)values[i];
				newVarEntry->vald=oapc_util_block_to_dbl(numValue);
			//int 
			}
		}
		if ((isData[0].charFBlock>0) && (ios & bits & OAPC_CHAR_IO_MASK))
		{
			snprintf(vname,100,"char%d",i);
			newVarEntry=new_var_entry(charParamList,vname);
			if (newVarEntry)
			{
				charParamList=newVarEntry;
				newVarEntry->type=STRING;
				newVarEntry->vals=strdup(values[i]);
			//int 
			}
		}
		
		bits=bits<<1;
	}
	if (digiParamList)
	{
		newVarEntry=new_var_entry(digiParamList,"ios");
		if (newVarEntry)
		{
			digiParamList=newVarEntry;
			newVarEntry->type=UDINT;
			newVarEntry->val=(ios & OAPC_DIGI_IO_MASK);
   		newVarEntry=new_var_entry(digiParamList,"nodeName");
   		if (newVarEntry)
	   	{
		   	digiParamList=newVarEntry;
   			newVarEntry->type=STRING;
			   newVarEntry->vals=strdup(nodeName);
   		   execute_il(isData[0].digiFBlock,1,digiParamList,__FILE__,__LINE__);
	   	}
		}
	}
	if (numParamList)
	{
		newVarEntry=new_var_entry(numParamList,"ios");
		if (newVarEntry)
		{
			numParamList=newVarEntry;
			newVarEntry->type=UDINT;
			newVarEntry->val=(ios & OAPC_NUM_IO_MASK);
   		newVarEntry=new_var_entry(numParamList,"nodeName");
   		if (newVarEntry)
	   	{
		   	numParamList=newVarEntry;
   			newVarEntry->type=STRING;
			   newVarEntry->vals=strdup(nodeName);
      		execute_il(isData[0].numFBlock,1,numParamList,__FILE__,__LINE__);
	   	}
		}
	}
	if (charParamList)
	{
		newVarEntry=new_var_entry(charParamList,"ios");
		if (newVarEntry)
		{
			charParamList=newVarEntry;
			newVarEntry->type=UDINT;
			newVarEntry->val=(ios & OAPC_CHAR_IO_MASK);
   		newVarEntry=new_var_entry(charParamList,"nodeName");
   		if (newVarEntry)
	   	{
		   	charParamList=newVarEntry;
   			newVarEntry->type=STRING;
			   newVarEntry->vals=strdup(nodeName);
      		execute_il(isData[0].charFBlock,1,charParamList,__FILE__,__LINE__);
	   	}
		}
	}
}

