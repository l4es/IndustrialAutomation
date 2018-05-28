/******************************************************************************
 *
 *  Copyright (C) 2008-2009  Andreas Stewering-Bone, Ingenieurgemeinschaft IgH
 *
 *  This file is part of the IgH EtherLAB Scicos Toolbox.
 *  
 *  The IgH EtherLAB Scicos Toolbox is free software; you can
 *  redistribute it and/or modify it under the terms of the GNU Lesser General
 *  Public License as published by the Free Software Foundation; version 2.1
 *  of the License.
 *
 *  The IgH EtherLAB Scicos Toolbox is distributed in the hope that
 *  it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the IgH EtherLAB Scicos Toolbox. If not, see
 *  <http://www.gnu.org/licenses/>.
 *  
 *  ---
 *  
 *  The license mentioned above concerns the source code only. Using the
 *  EtherCAT technology and brand is only permitted in compliance with the
 *  industrial property and similar rights of Beckhoff Automation GmbH.
 *
 *****************************************************************************/

/* hier die Userbibliotheken */
#include <linux/a.out.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <linux/errno.h>  /* error codes */


//MSR
#include "msr_reg_scicos.h"
#include "rt_lib/include/msr_attributelist.h"
#include <etl_scicos_data_info.h>
//Scicos
#include <scicos_block4.h>

#define DBG 0

#define MSR_CALC_ADR(_START,_DATASIZE,_ORIENTATION,_RNUM,_CNUM)   \
do {                                                              \
switch(_ORIENTATION) {                                            \
    case si_vector:                                               \
	p = _START + (r + c)*_DATASIZE;  		          \
        break;                                                    \
    case si_matrix_col_major:                                     \
	p = _START + (r + c * _RNUM)*_DATASIZE;	                  \
	break;                                                    \
    case si_matrix_row_major:                                     \
	p = _START + (_CNUM * r + c)*_DATASIZE;	                  \
	break;                                                    \
    default:                                                      \
        p = _START;                                               \
	break;                                                    \
}                                                                 \
} while (0)


extern default_sampling_red;

enum enum_var_typ SCICOS_to_MSR(unsigned int datatyp) {

  if(DBG > 0){printf("Scicos to MSR Datacode %i \n",datatyp);}
  switch(datatyp)
    {
    case SCSREAL_N:
      if(DBG > 0){printf("Scicos to MSR Type SCSREAL_N -> TDBL\n");}
      return TDBL;
      break;
    case SCSCOMPLEX_N:
      if(DBG > 0){printf("Scicos to MSR Type SCSCOMPLEX_N -> Unknown\n");}
      return 0; //Unkown
      break;
    case SCSINT_N:
      if(DBG > 0){printf("Scicos to MSR Type SCSINT_N -> TINT\n");}
      return TINT;
      break;
    case SCSINT8_N:
      if(DBG > 0){printf("Scicos to MSR Type SCSINT8_N -> TCHAR\n");}
      return TCHAR;
      break;
    case SCSINT16_N:
      if(DBG > 0){printf("Scicos to MSR Type SCSINT16_N -> TSHORT\n");}
      return TSHORT;
      break;
    case SCSINT32_N:
      if(DBG > 0){printf("Scicos to MSR Type SCSINT32_N -> TINT\n");}
      return TINT;
      break;
    case SCSUINT_N:
      if(DBG > 0){printf("Scicos to MSR Type SCSUINT_N -> TUINT\n");}
      return TUINT;
      break;
    case SCSUINT8_N:
      if(DBG > 0){printf("Scicos to MSR Type SCSUINT8_N -> TUCHAR\n");}
      return TUCHAR;
      break;
    case SCSUINT16_N:
      if(DBG > 0){printf("Scicos to MSR Type SCSUINT16_N -> TUSHORT\n");}
      return TUSHORT;
      break;
    case SCSUINT32_N:
      if(DBG > 0){printf("Scicos to MSR Type SCSUINT32_N -> TUINT\n");}
      return TUINT;
      break;
    case SCSUNKNOW_N:
      if(DBG > 0){printf("Scicos to MSR Type SCSUNKNOWN_N -> Unknown\n");}
      return 0;
      break;
    default:
      if(DBG > 0){printf("Scicos to MSR Type default -> Unknown\n");}
      return 0;
      break;
    }
}


unsigned int SCICOS_size(unsigned int datatyp) {

  switch(datatyp)
    {
    case SCSREAL_N:
      return sizeof(double);
      break;
    case SCSCOMPLEX_N:
      return 0; //Unkown
      break;
    case SCSINT_N:
      return sizeof(int);
      break;
    case SCSINT8_N:
      return sizeof(char);
      break;
    case SCSINT16_N:
      return sizeof(short int);
      break;
    case SCSINT32_N:
      return sizeof(int);
      break;
    case SCSUINT_N:
      return sizeof(unsigned int);
      break;
    case SCSUINT8_N:
      return sizeof(unsigned char);
      break;
    case SCSUINT16_N:
      return sizeof(unsigned short int);
      break;
    case SCSUINT32_N:
      return sizeof(unsigned int);
      break;
    case SCSUNKNOW_N:
      return 0;
      break;
    default:
      return 0;
      break;
    }
}


//RTW ersetzt \n durch Leerzeichen, diese werden hier entfernt
#define RTWPATHTRIM(c)                \
do {                                                             \
    int i,j=0;                                                     \
    for(i=0;i<strlen(c)+1;i++)  {                                   \
	if (i<strlen(c)-1 && ((c[i] == '/' && c[i+1] == '/') || (c[i] == ' ' && c[i+1] == '/') || (c[i] == '/' && c[i+1] == ' ')))    \
        ; \
/* do nothing*/ \
        else                                                    \
	    c[j++] = c[i];                                       \
   }            \
} while(0)







int msr_reg_scicos_param( const char *model_name,
                       const char *path, const char *name, const char *alias,
                       const char *cTypeName,
		       void *data,
		       unsigned int rnum, unsigned int cnum,
		       enum si_datatype_t dataType, 
                       enum si_orientation_t orientation,
		       unsigned int dataSize){
    char *buf;
    char *rbuf,*info;
    char *upath;        //unique name
    char *value;
    int result=1;
    int dohide = 0;
    int isstring = 0;
    unsigned int pflag = 0;

    struct talist *alist = NULL;

    if(DBG > 0){printf("Parameterbaseadress: %X\n",data);}
    
    if(DBG > 0) printf("reg_scicos_param:%s|%s|%s, rnum: %i, cnum: %i\n",model_name,path,name,rnum,cnum);

    //Hilfspuffer

    buf = (char *)getmem(
            strlen(model_name) + strlen(path) + strlen(name) + 4
            +2+20);

    //erstmal den Namen zusammensetzten zum einem gültigen Pfad
    if (strlen(path))
        sprintf(buf, "/%s/%s/%s", model_name, path, name);
    else {
        sprintf(buf, "/%s/%s", model_name, name);
	if (DBG >0) printf("xxxxxxxxxxxxxxxxx\n");
    }
    //jetzt alle Ausdrücke, die im Pfad in <> stehen extrahieren und interpretieren
    rbuf = extractalist(&alist,buf);

    RTWPATHTRIM(rbuf);
    if(rbuf[strlen(rbuf)-1] == '/') 
	rbuf[strlen(rbuf)-1] = 0;

    info = alisttostr(alist);

    //und registrieren, falls gewünscht
    if(hasattribute(alist,"hide")) {
	value = getattribute(alist,"hide");
	if (value[0] == 0 || value [0] == 'p') {
	    printf("Hiding Parameter: %s\n",buf);
	    dohide = 1;
	}
    }


    if(hasattribute(alist,"aic"))
      pflag = MSR_R | MSR_AIC;  //Input Channels sind ro und werden über <wpic> beschrieben
    else 
      pflag = MSR_R | MSR_W;
    

    //upath = msr_unique_channel_name(rbuf);
    upath = rbuf;

    if(!dohide) {
	if(hasattribute(alist,"isstring") && (dataType == TUCHAR || dataType == TCHAR)) {
	    result = msr_cfi_reg_param(upath,"",data,rnum,cnum,orientation,TSTR,info,pflag,NULL,NULL);
	    isstring = 1;
	}
	else
	    result = msr_cfi_reg_param(upath,"",data,rnum,cnum,orientation,dataType,info,pflag,NULL,NULL);

	    //jetzt noch zusätzlich die einzelnen Elemente registrieren, da das MSR-Protokoll das so haben will... 
	if (!hasattribute(alist,"hideelements") && isstring == 0) {
		if(rnum+cnum > 2) { 
		    int r,c;
		    void *p;
		    char *ibuf = (char *)getmem(40+3); //warum 40+3 ?? 2 Indizes a max 64bit in dezimal sind 20 Stelle je, dann 2 mal / und die 0
		    for (r = 0; r < rnum; r++) {
			for (c = 0; c < cnum; c++) {
			    MSR_CALC_ADR((void *)data,dataSize,orientation,rnum,cnum);
			    //neuen Namen
			    if (rnum == 1 || cnum == 1)  //Vektor
				sprintf(ibuf,"/%i",r+c);
			    else                         //Matrize
				sprintf(ibuf,"/%i/%i",r,c);
			    //p wird in MSR_CALC_ADR berechnet !!!!!!!!!!!
			    result = msr_cfi_reg_param(upath,ibuf,p,1,1,orientation,dataType,info,pflag | MSR_DEP,NULL,NULL);

			}
		    }
		    freemem(ibuf);
		}
	    }

    }
    freemem(info);
    freemem(rbuf);
    freemem(buf);
    //freemem(upath); //uncommented because ubuf = rbuf 
    freealist(&alist);
    return result;

}


int msr_reg_scicos_signal( const char* model_name, 
                        const char *path, const char *name, const char *alias,
                        const char *cTypeName,
			unsigned int baseadr,                                              // !!!
			unsigned int rnum, unsigned int cnum,
			enum si_datatype_t dataType, 
                        enum si_orientation_t orientation,
			unsigned int dataSize,
			int outputno,
			int anzoutput){

    char *buf;
    char *rbuf,*info;
    char *upath;          //unique Pathname
    char *value;
    int result = 1,r,c;
    int dohide = 0;

    void *p;

    struct talist *alist = NULL;

    if(DBG > 0){printf("Kanalbaseadress: %X\n",baseadr);}
    //Hilfspuffer

    if(DBG > 0) printf("reg_scicos_signal:%s|%s|%s, rnum: %i, cnum: %i\n",model_name,path,name,rnum,cnum);

    buf = (char *)getmem(
            strlen(model_name) + strlen(path) + strlen(name) + 4
            +2+20);


    //erstmal den Namen zusammensetzten zum einem gültigen Pfad

   if (anzoutput > 1)
     {
       if (strlen(path))
	 sprintf(buf, "/%s/%s/%s/out%i", model_name, path, name,outputno);
       else
	 sprintf(buf, "/%s/%s/out%i", model_name, name,outputno);
     }
   else
     {
       if (strlen(path))
	 sprintf(buf, "/%s/%s/%s", model_name, path, name);
       else
	 sprintf(buf, "/%s/%s", model_name, name);
     }
    rbuf = extractalist(&alist,buf);

    RTWPATHTRIM(rbuf);

    if(rbuf[strlen(rbuf)-1] == '/') 
	rbuf[strlen(rbuf)-1] = 0;


    info = alisttostr(alist);

    //und registrieren (hier aber für Vektoren und Matrizen einen eigenen Kanal)


    //und registrieren, falls gewünscht
    if(hasattribute(alist,"hide")) {
	value = getattribute(alist,"hide");
	if (value[0] == 0 || value [0] == 's' || value [0] == 'k')  {//signal oder kanal
	    printf("Hiding Channel: %s\n",rbuf);
	    dohide = 1;
	}
    }

    //upath = msr_unique_channel_name(rbuf);
    upath = rbuf;
    
    if (strcmp(upath,rbuf) != 0) printf("%s->%s\n",rbuf,upath);   


    if(!dohide) {
	if(rnum+cnum > 2) {
	    char *ibuf = (char *)getmem(40+3); //warum 40+3 ?? 2 Indizes a max 64bit in dezimal sind 20 Stelle je, dann 2 mal / und die 0
	    for (r = 0; r < rnum; r++) {
		for (c = 0; c < cnum; c++) {
		    MSR_CALC_ADR((void *)baseadr,dataSize,orientation,rnum,cnum);
		    //neuen Namen
		    if (rnum == 1 || cnum == 1)  //Vektor
			sprintf(ibuf,"%s/%i",upath,r+c);
		    else                         //Matrize
			sprintf(ibuf,"%s/%i/%i",upath,r,c);
		    //p wird in MSR_CALC_ADR berechnet !!!!!!!!!!!
		    result |= msr_reg_kanal3(ibuf,(char *)alias,"",p,dataType,info,default_sampling_red);
		}
	    }
	    freemem(ibuf);
	}
	else {  //ein Sklarer Kanal
	    result |= msr_reg_kanal3(upath,(char *)alias,"",(void *)baseadr,dataType,info,default_sampling_red);
	}

    }

    freemem(info);
    freemem(rbuf);
    freemem(buf);
    //freemem(upath); /comment out because of comment out msr_unique_...
    freealist(&alist);

    return result;

}

enum si_orientation_t get_orientation(int rows, int colls)
{

  if((rows * colls) <= 1) //Scalar
    {
      return si_scalar;
    }
  
  if(((rows == 1)&&(colls > 1)) || ((colls == 1)&&(rows > 1))) //Vector
    {
      return si_vector;
    }
  if((rows > 1) && (colls > 1)) //Matrix
    {
      return si_matrix_row_major;
    }
  // si_matrix_coll_major_nd is ignored
}

int  msr_register_blocks(const char * modelname, char *blockname,scicos_block *block)
{
  char *namebuf;
  int result;
  int j;
  int noout = GetNout(block);
  int noipar = GetNipar(block);
  int norpar = GetNrpar(block);
  int noopar = GetNopar(block);
  int *baseadr;
  int *targetadr;
  double *baseadr_r;
  double *targetadr_r;
  void *baseadr_o;
  void *targetadr_o;

  if(DBG > 0){printf("Register Scicos Block %s Number of Outputs %i, Integer Parameter %i, Real Parameter %i, Object Parameter %i\n",blockname,noout,noipar,norpar,noopar);}

  //first register channels
  if(noout>0)
    {
      for(j=1; j <= noout;j++)
	{
	  result = msr_reg_scicos_signal(
					 modelname,                 //Modelname
					 "",                        //Path
					 blockname,                 //Blockname
					 "",                        //Alias
					 "",                        //CTypeName
					 GetOutPortPtrs(block,j),   //Offset-Adress
					 GetOutPortRows(block,j),   //Anz-Rows
					 GetOutPortCols(block,j),   //Anz-Colls
					 SCICOS_to_MSR(GetOutType(block,j)), 
					 get_orientation(GetOutPortSize(block,j,1),GetOutPortSize(block,j,2)),
					 SCICOS_size(GetOutType(block,j)),
					 j,                          // Num Out
					 noout);
	  if(result < 0)
	    {
	      printf("Error Signal registration: %s, Output %i \n",blockname,j);
	      return result;
	    }
	} 
    }
  //Integer Parameter
  if(noipar > 0)
    {
      for(j=1; j <= noipar; j++)
	{
	  baseadr = GetIparPtrs(block);
	  targetadr = &baseadr[j-1];
	  if(targetadr != NULL)
	    {
	      if(DBG > 0){printf("Parameter Baseadress %X, Targetadress %X \n",baseadr,targetadr);}
	      //Hilfspuffer
	      namebuf = (char *)malloc(strlen(blockname) + 20);
	      sprintf(namebuf, "/%s/IPAR%3i", blockname,j);
	      result = msr_reg_scicos_param( modelname,              //Modelname
					     "",                     //Path
					     namebuf,              //Blockname
					     "",                     //Alias
					     "",                     //cTypeName
					     targetadr,              //Data
					     1,                      //RNUM
					     1,                      //CNUM
					     TINT,                   //Type
					     si_scalar,              //Orientation
					     SCICOS_size(SCSINT_N)   //Datasize
					     );
	      if(result < 0)
		{
		  printf("Error Integer Parameter registration: %s, Number %i \n",blockname,j);
		  return result;
		}
	    }
	  else
	    {
	      printf("Skipping Integer Parameter Block %s Number %i \n",blockname, j);
	    }
	}

    }

  //Real Parameter
  if(norpar > 0)
    {
      for(j=1; j <= norpar; j++)
	{
	  baseadr_r = GetRparPtrs(block);
	  targetadr_r = &baseadr_r[j-1];
	  if(targetadr_r != NULL)
	    {
	      if(DBG > 0){printf("Parameter Baseadress %X, Targetadress %X \n",baseadr_r,targetadr_r);}
	      //Hilfspuffer
	      namebuf = (char *)malloc(strlen(blockname) + 20);
	      sprintf(namebuf, "/%s/RPAR%3i", blockname,j);
	      result = msr_reg_scicos_param( modelname,              //Modelname
					     "",                     //Path
					     namebuf,              //Blockname
					     "",                     //Alias
					     "",                     //cTypeName
					     targetadr_r,            //Data
					     1,                      //RNUM
					     1,                      //CNUM
					     TDBL,                   //Type
					     si_scalar,              //Orientation
					     SCICOS_size(SCSREAL_N)  //Datasize
					     );
	      if(result < 0)
		{
		  printf("Error Real Parameter registration: %s, Number %i \n",blockname,j);
		  return result;
		}
	    }
	  else
	    {
	      printf("Skipping Real Parameter Block %s Number %i \n",blockname, j);
	    }
	}

    }

  //Object Parameter
  if(noopar > 0)
    {
      for(j=1; j <= noopar; j++)
	{
	  if(GetOparSize(block,j,1)*GetOparSize(block,j,2)>0) //Check for Matrix size is non zero (SDO Entries for example)
	    {
	      baseadr_o = GetOparPtrs(block,1);
	      targetadr_o = GetOparPtrs(block,j);
	      if(targetadr_o != NULL)
		{
		  if(DBG > 0){printf("Parameter Baseadress %X, Targetadress %X \n",baseadr_o,targetadr_o);}
		  //Hilfspuffer
		  
		  namebuf = (char *)malloc(strlen(blockname) + 20);
		  sprintf(namebuf, "/%s/OPAR%3i", blockname,j);
		  result = msr_reg_scicos_param( modelname,              //Modelname
						 "",                     //Path
						 namebuf,              //Blockname
						 "",                     //Alias
						 "",                     //cTypeName
						 targetadr_o,            //Data
						 GetOparSize(block,j,1), //RNUM
						 GetOparSize(block,j,2), //CNUM
						 SCICOS_to_MSR(GetOparType(block,j)),//Type
						 get_orientation(GetOparSize(block,j,1),GetOparSize(block,j,1)), //Orientation
						 SCICOS_size(GetOparType(block,j))  //Datasize
						 );
		  if(result < 0)
		    {
		      printf("Error Object Parameter registration: %s, Number %i \n",blockname,j);
		      return result;
		    }
		}
	      else
		{
		  printf("Skipping Object Parameter Block %s Number %i \n",blockname, j);
		}
	    }
	  else
	    {
	      if(DBG > 0){printf("OPAR Parameter Skipped because of Matix size equals zero\n");}
	    }
	}

    }

  return 0;
}

