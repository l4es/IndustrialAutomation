/* emitter.c
 *
 *	(C) Copyright Apr 15 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "MachSet.h"
#include "global.h"


static void showjmptab(InsT_t * inst, int i, int spaces)
{
  struct {
    int n;
    val_t *loc;
  } *p;
  int j, k;
  p = inst->val.p.p;
  printf("\n");
  for (k = 0; k < spaces; k++)
    printf(" ");
  printf("     default  %d\n", i + p->loc[0].ival);
  for (j = 1; j < p->n; j += 2) {
    for (k = 0; k < spaces; k++)
      printf(" ");

    printf("     case %d: %d\n", p->loc[j].ival,
	   i + p->loc[j + 1].ival);
  }
}





static void showcode(InsT_t * inst,int i)
{
  static char str[30], *s;

  s = NULL;
  switch (inst->opcode) {
      case __eiclongjmp: s = "__eiclongjmp"; break;
      case __eicsetjmp: s = "__eicsetjmp"; break;
    case jmptab: s = "jmptab"; break;
    case lval: sprintf(str, "lval   %d %d",
		       inst->val.ival,
		       inst->ext); break;

    case ixa: sprintf(str, "ixa      %d  %d",inst->val.ival, inst->ext);
      break;
    case lda: sprintf(str, "lda      %d  %d",inst->val.ival, inst->ext);
      break;

    case refchar: s = "refchar"; break;
    case drefuchar: s = "drefuchar"; break;  
    case drefchar: s = "drefchar"; break;
    case rvaluchar: sprintf(str, "rvaluchar  %d %d",
			  inst->val.ival,
			  inst->ext); break;
    case rvalchar: sprintf(str, "rvalchar  %d %d",
			  inst->val.ival,
			  inst->ext); break;
    case stochar: sprintf(str, "stochar %d %d",
			   inst->val.ival,
			   inst->ext); break;

    case refshort: s = "refshort"; break;
    case drefushort: s = "drefushort"; break;  
    case drefshort: s = "drefshort"; break;
    case rvalushort: sprintf(str, "rvalushort  %d %d",
			  inst->val.ival,
			  inst->ext); break;
    case rvalshort: sprintf(str, "rvalshort  %d %d",
			  inst->val.ival,
			  inst->ext); break;
    case stoshort: sprintf(str, "stoshort %d %d",
			   inst->val.ival,
			   inst->ext); break;
      

    case reffloat: s = "reffloat"; break;
    case dreffloat: s = "dreffloat"; break;
    case rvalfloat: sprintf(str, "rvalfloat  %d %d",
			  inst->val.ival,
			  inst->ext); break;
    case stofloat: sprintf(str, "stofloat %d %d",
			   inst->val.ival,
			   inst->ext); break;
      
    case jmpFint: sprintf(str, "jmpFint  %d", i + inst->val.ival); break;
    case jmpFlng: sprintf(str, "jmpFlng  %d", i + inst->val.ival); break;
    case jmpFllng: sprintf(str, "jmpFllng  %d", i + inst->val.ival); break;
    case jmpFdbl: sprintf(str, "jmpFdbl  %d", i + inst->val.ival); break;
    case jmpFptr: sprintf(str, "jmpFptr  %d", i + inst->val.ival); break;
      
    case jmpTint: sprintf(str, "jmpTint  %d", i + inst->val.ival); break;
    case jmpTlng: sprintf(str, "jmpTlng  %d", i + inst->val.ival); break;
    case jmpTllng: sprintf(str, "jmpTllng  %d", i + inst->val.ival); break;
    case jmpTdbl: sprintf(str, "jmpTdbl  %d", i + inst->val.ival); break;
    case jmpTptr: sprintf(str, "jmpTptr  %d", i + inst->val.ival); break;

    case jmpu: sprintf(str, "jmpU     %d", i + inst->val.ival); break;
    case halt: s = "halt"; break;
    case dupval: sprintf(str,"dupval   %d",inst->val.ival); break;
    case bump:   sprintf(str,"bump     %d", inst->val.ival); break;
    case call: s = "call"; break;
      
    case neguchar: s = "neguchar"; break;
      
    case stodouble: sprintf(str, "stodouble %d %d",
			   inst->val.ival,
			   inst->ext); break;
    case pushdouble: sprintf(str, "pushdouble  %g", inst->val.dval); break;
    case rvaldouble: sprintf(str, "rvaldouble  %d %d",
			    inst->val.ival,
			    inst->ext); break;

    case incdouble: s = "incdouble"; break;
    case decdouble: s = "decdouble"; break;
    case refdouble: s = "refdouble"; break;
    case drefdouble: s = "drefdouble "; break;
    case negdouble: s = "negdouble"; break;
    case ltdouble: s = "ltdouble"; break;
    case ledouble: s = "ledouble"; break;
    case eqdouble: s = "eqdouble"; break;
    case nedouble: s = "nedouble"; break;
    case gtdouble: s = "gtdouble"; break;
    case gedouble: s = "gedouble"; break;
    case lordouble: s = "lordouble"; break;
    case landdouble: s = "landdouble"; break;
    case notdouble: s = "notdouble"; break;
    case adddouble: s = "adddouble"; break;
    case subdouble: s = "subdouble"; break;
    case divdouble: s = "divdouble"; break;
    case multdouble: s = "multdouble"; break;
    case double2int: s = "double2int"; break;
    case double2long: s = "double2long"; break;
    case double2llong: s = "double2llong"; break;
    case double2float: s = "double2float"; break;
      
    case stoint: sprintf(str, "stoint   %d %d", 
			 inst->val.ival,
			 inst->ext); break;
    case pushint: sprintf(str, "pushint  %d", inst->val.ival); break;
    case rvalint: sprintf(str, "rvalint  %d %d",
			  inst->val.ival,
			  inst->ext); break;
    case drefint: s = "drefint   "; break;
    case refint: s = "refint   "; break;
    case decint: sprintf(str, "decint   %d", inst->val.ival); break;
    case incint: sprintf(str, "incint   %d", inst->val.ival); break;
    case negint: s = "negint"; break;
    case ltint: s = "ltint"; break;
    case leint: s = "leint"; break;
    case eqint: s = "eqint"; break;
    case neint: s = "neint"; break;
    case gtint: s = "gtint"; break;
    case geint: s = "geint"; break;
    case lorint: s = "lorint"; break;
    case landint: s = "landint"; break;
    case notint: s = "notint"; break;
    case borint: s = "borint"; break;
    case xorint: s = "xorint"; break;
    case andint: s = "andint"; break;
    case addint: s = "addint"; break;
    case subint: s = "subint"; break;
    case divint: s = "divint"; break;
    case multint: s = "multint"; break;
    case modint: s = "modint"; break;
    case lshtint: s = "lshtint"; break;
    case rshtint: s = "rshtint"; break;
    case compint: s = "compint"; break;
    case int2ptr: s = "int2ptr"; break;
    case int2uchar: s = "int2uchar"; break;
    case int2ushort: s = "int2ushort"; break;
      
    case int2double: s = "int2double"; break;
    case int2llong: s = "int2llong"; break;
    case int2long: s = "int2llong"; break;
      
      
    case stouint: sprintf(str, "stouint   %d %d",
			  inst->val.ival,
			  inst->ext); break;
    case pushuint: sprintf(str, "pushuint  %u", inst->val.ival); break;
    case rvaluint: sprintf(str, "rvaluint  %d %d",
			   inst->val.ival,
			   inst->ext); break;
    case drefuint: s = "drefuint   "; break;
    case refuint: s = "refuint   "; break;
    case decuint: sprintf(str, "decuint   %d", inst->val.ival); break;
    case incuint: sprintf(str, "incuint   %d", inst->val.ival); break;
    case neguint: s = "neguint"; break;
    case ltuint: s = "ltuint"; break;
    case leuint: s = "leuint"; break;
    case equint: s = "equint"; break;
    case neuint: s = "neuint"; break;
    case gtuint: s = "gtuint"; break;
    case geuint: s = "geuint"; break;
    case loruint: s = "loruint"; break;

    case notuint: s = "notuint"; break;
    case boruint: s = "boruint"; break;
    case xoruint: s = "xoruint"; break;
    case anduint: s = "anduint"; break;
    case adduint: s = "adduint"; break;
    case subuint: s = "subuint"; break;
    case divuint: s = "divuint"; break;
    case multuint: s = "multuint"; break;
    case moduint: s = "moduint"; break;
    case lshtuint: s = "lshtuint"; break;
    case rshtuint: s = "rshtuint"; break;
    case compuint: s = "compuint"; break;
      
    case uint2ptr: s = "uint2ptr"; break;
    case uint2long: s = "uint2long"; break;
    case uint2llong: s = "uint2llong"; break;
    case uint2double: s = "uint2double"; break;
      
    case stolong: sprintf(str, "stolong   %d %d",
			  inst->val.ival,
			  inst->ext); break;
    case stollong: sprintf(str, "stollong   %d %d",
			  inst->val.ival,
			  inst->ext); break;
    case pushlong: sprintf(str, "pushlong  %ld", inst->val.lval); break;
    case pushllong: sprintf(str, "pushllong  %ld", inst->val.lval); break;
    case rvallong: sprintf(str, "rvallong  %d %d",
			   inst->val.ival,
			   inst->ext); break;
    case rvalllong: sprintf(str, "rvalllong  %d %d",
			   inst->val.ival,
			   inst->ext); break;

    case dreflong: s = "dreflong   "; break;
    case reflong: s = "reflong   "; break;
    case declong: sprintf(str, "declong   %d", inst->val.ival); break;
    case inclong: sprintf(str, "inclong   %d", inst->val.ival); break;
    case neglong: s = "neglong"; break;
    case ltlong: s = "ltlong"; break;
    case lelong: s = "lelong"; break;
    case eqlong: s = "eqlong"; break;
    case nelong: s = "nelong"; break;
    case gtlong: s = "gtlong"; break;
    case gelong: s = "gelong"; break;
    case lorlong: s = "lorlong"; break;
    case landlong: s = "landlong"; break;
    case notlong: s = "notlong"; break;
    case borlong: s = "borlong"; break;
    case xorlong: s = "xorlong"; break;
    case andlong: s = "andlong"; break;
    case addlong: s = "addlong"; break;
    case sublong: s = "sublong"; break;
    case divlong: s = "divlong"; break;
    case multlong: s = "multlong"; break;
    case modlong: s = "modlong"; break;
    case lshtlong: s = "lshtlong"; break;
    case rshtlong: s = "rshtlong"; break;
    case complong: s = "complong"; break;
      

    case drefllong: s = "drefllong   "; break;
    case refllong: s = "refllong   "; break;
    case decllong: sprintf(str, "decllong   %d", inst->val.ival); break;
    case incllong: sprintf(str, "incllong   %d", inst->val.ival); break;
    case negllong: s = "negllong"; break;
    case ltllong: s = "ltllong"; break;
    case lellong: s = "lellong"; break;
    case eqllong: s = "eqllong"; break;
    case nellong: s = "nellong"; break;
    case gtllong: s = "gtllong"; break;
    case gellong: s = "gellong"; break;
    case lorllong: s = "lorllong"; break;
    case landllong: s = "landllong"; break;
    case notllong: s = "notllong"; break;
    case borllong: s = "borllong"; break;
    case xorllong: s = "xorllong"; break;
    case andllong: s = "andllong"; break;
    case addllong: s = "addllong"; break;
    case subllong: s = "subllong"; break;
    case divllong: s = "divllong"; break;
    case multllong: s = "multllong"; break;
    case modllong: s = "modllong"; break;
    case lshtllong: s = "lshtllong"; break;
    case rshtllong: s = "rshtllong"; break;
    case compllong: s = "compllong"; break;
      


    case long2ptr: s = "long2ptr"; break;
    case long2int: s = "long2int"; break;
    case long2double: s = "long2double"; break;

    case llong2ptr: s = "llong2ptr"; break;
    case llong2int: s = "llong2int"; break;
    case llong2double: s = "llong2double"; break;
      
    case stoulong: sprintf(str, "stoulong %d %d",
			   inst->val.ival,
			   inst->ext); break;
    case pushulong: sprintf(str, "pushulong  %u", inst->val.ival); break;
    case rvalulong: sprintf(str, "rvalulong  %d %d",
			    inst->val.ival,
			    inst->ext); break;
    case drefulong: s = "drefulong"; break;
    case refulong: s = "refulong"; break;
    case deculong: sprintf(str, "deculong %d", inst->val.ival); break;
    case inculong: sprintf(str, "inculong %d", inst->val.ival); break;
    case negulong: s = "negulong"; break;
    case ltulong: s = "ltulong"; break;
    case leulong: s = "leulong"; break;
    case equlong: s = "equlong"; break;
    case neulong: s = "neulong"; break;
    case gtulong: s = "gtulong"; break;
    case geulong: s = "geulong"; break;
    case lorulong: s = "lorulong"; break;

    case notulong: s = "notulong"; break;
    case borulong: s = "borulong"; break;
    case xorulong: s = "xorulong"; break;
    case andulong: s = "andulong"; break;
    case addulong: s = "addulong"; break;
    case subulong: s = "subulong"; break;
    case divulong: s = "divulong"; break;
    case multulong: s = "multulong"; break;
    case modulong: s = "modulong"; break;
    case lshtulong: s = "lshtulong"; break;
    case rshtulong: s = "rshtulong"; break;
    case compulong: s = "compulong"; break;
      
    case ulong2ptr: s = "ulong2ptr"; break;
    case ulong2int: s = "ulong2int"; break;
    case ulong2double: s = "ulong2double"; break;
      
    case drefptr: s = "drefptr"; break;
    case drefuptr: s = "drefuptr"; break;
    case refptr: s = "refptr"; break;
    case refuptr: s = "refuptr"; break;
    
  case stouptr:  sprintf(str, "stouptr   %d %d",
			 inst->val.ival,
			 inst->ext); break;

    case stoptr: sprintf(str, "stoptr   %d %d",
			 inst->val.ival,
			 inst->ext); break;
    case pushptr: sprintf(str, "pushptr  %p", inst->val.p.p); break;

  case rvaluptr: sprintf(str, "rvaluptr  %d %d",
			  inst->val.ival,
			  inst->ext); break;
    case rvalptr: sprintf(str, "rvalptr  %d %d",
			  inst->val.ival,
			  inst->ext); break;
    case incptr: sprintf(str, "incptr   %d", inst->val.ival); break;
    case decptr: sprintf(str, "decptr   %d", inst->val.ival); break;
    case ltptr: s = "ltptr"; break;
    case leptr: s = "leptr"; break;
    case eqptr: s = "eqptr"; break;
    case neptr: s = "neptr"; break;
    case gtptr: s = "gtptr"; break;
    case geptr: s = "geptr"; break;
    case lorptr: s = "lorptr"; break;
    case landptr: s = "landptr"; break;
    case notptr: s = "notptr"; break;
    case addptr2int: s = "addptr2int"; break;
    case addint2ptr: s = "addint2ptr"; break;
    case subptrint: s = "subptrint"; break;
    case subptr: s = "subptr"; break;
    case divptr: sprintf(str, "divptr   %d", inst->val.ival); break;
    case ptr2int: s = "ptr2int"; break;
    case ptr2long: s = "ptr2long"; break;
      
    case eiccall: s = "eiccall"; break;
    case eicreturn: s = "eicreturn"; break;
      
    case assigntype: sprintf(str, "assigntype %d",
			     inst->ext); break;
      
    case stoval: s = "stoval"; break;
    case pushval: s = "pushval"; break;  
    case reducear: sprintf(str, "reducear %d",
			   inst->val.ival); break;
      
    case checkar: sprintf(str, "checkar  %d %d",
			  inst->val.ival,
			  inst->ext); break;
      
    case refmem: sprintf(str, "refmem   %d",
			 inst->val.ival); break;
    case massign: sprintf(str, "massign  %d %d",
			  inst->val.ival,
			  inst->ext); break;
      
    case fmem: sprintf(str, "fmem     %d %d",
		       inst->val.ival,
		       inst->ext); break;
    case minit: sprintf(str, "mcpy    %p %d",
		       inst->val.p.p,
		       inst->ext); break;

    case empty: s = "empty"; break;
      
      
    default:
      sprintf(str, "UNKNOWN %d", inst->opcode);
  }
  if (s)
      printf("%-20.20s", s);
  else
      printf("%-20.20s", str);
}

    
void EiC_listcode(code_t * CODE)
{
    extern int EiC_lineNums;
    int i, j, t, k, cols, rows, sp;
    if (CODE->nextinst < 11)
	cols = 1;
    else if (CODE->nextinst < 21)
	cols = 2;
    else
	cols = 3;
    
    rows = CODE->nextinst / cols;
    if (CODE->nextinst % cols)
	rows++;

    if(EiC_lineNums)
	sp = 30;
    else
	sp = 25;
    
    for (j = 0; j < rows; ++j) {
	for (t = 0, i = j; i < CODE->nextinst; i += rows, t++) {
	    if(EiC_lineNums)
		printf("%4d:", CODE->inst[i].line);
	    printf("%4d:", i);
	    showcode(&CODE->inst[i], i);
	    if (CODE->inst[i].opcode == jmptab) {
		showjmptab(&CODE->inst[i], i, t * sp);
		t = (t + 1) * sp;
		for (k = 0; k < t; k++)
		    printf(" ");
	    }
	}
	printf("\n");
    }
    printf("\n");
}

/* END EMITTER.C
   -----------------
   ----------------- */








