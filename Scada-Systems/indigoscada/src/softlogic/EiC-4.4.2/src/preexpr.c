/* preexpr.c
 *
 *	(C) Copyright Feb  2 1996, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

#ifndef _STANDALONE

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "xalloc.h"
#include "preproc.h"

#endif


/*ppint expr(int k);*/

/* typedef unsigned long ppint;   the preprocessor integer */

typedef struct {
    union {
	long s;           /* signed value */
	unsigned long u;  /* unsigned value */
    }v;
    int type;
}ppint;
/* methods */
#define sval(x)  ((x).v.s)
#define uval(x)  ((x).v.u)
#define tval(x)  ((x).type)

#define USIGN  1
#define SIGN   2

static ppint do_binary(int tk, ppint left, ppint right);
static int getTok(int k);
static ppint EiC_expr_unary(void);
static ppint get_number(void);
static ppint EiC_ifexpr(int k);


enum { LOR = 1, LAND, BOR,  XOR,   AND,    EQ,  NEQ,  LT, LEQ,
        GT, GEQ, LSHFT, RSHFT,PLUS, MINUS, TIMES, DIV, MOD};

static char *S;
static int TK =0;

#define _StrSz_ 100

static void replaceDefines(char *S)
{
    char str[50];
    int cmode = 0;
    char *p;
    p = S;
    while(*S != '\0') {
        if(!cmode && (isalpha(*S) || *S == '_')) {
            if(S[0] == 'd' && S[1] == 'e' &&
               S[2] == 'f' && S[3] == 'i' &&
               S[4] == 'n' && S[5] == 'e' &&
               S[6] == 'd' && !isalpha(S[7]) &&
               S[7] != '_') {
                int br = 0;
                int i;
                S+=7;
                skipfws(S);
                if(*S=='(') {
                    S++;
                    br = 1;
                    skipfws(S);
                }
                i = 0;
                while(i < 50 && (isalpha(*S) || *S == '_' || isdigit(*S)))
                    str[i++] = *S++;
                str[i] = '\0';
                if(br) {
                    skipfws(S);
                    if(*S != ')') 
                        EiC_pre_error("Missing ')'");
                    else
                        S++;
                }
                if(str[0] != '\0') {
                    if(EiC_ismacroid(str) > -1)
                        *p = '1';
                    else
                        *p = '0';
                    ++p;
                } else
                    EiC_pre_error("Missing identifier");
                continue;
            } 
            do 
                *p++ = *S++;
            while(isalpha(*S) || *S == '_' );
	    continue;
        } if(*S == '\'')
            cmode = !cmode;
        *p++ = *S++;
    }
    *p = '\0';
}

static void replaceIdentifiers(char *S)
{
    int i;
    char str[50];
    char *p;
    int cmode = 0;

    p = S;    
    
    while(1) {
	while(isspace(*S))
	    *p++ = *S++;
	if(!*S)
	    return;
	if(!isalpha(*S) && *S != '_') {
	    if(isdigit(*S) || *S == '\'') { /* skip throu numbers or literals*/
		while(*S && !isspace(*S))
		    *p++ = *S++;
	    } else 
		while(*S && !isspace(*S) && *S != '_' && ispunct(*S))
		    *p++ =  *S++;
	    continue;
	}
	if(!cmode) {
	    i = 0;
	    while(i < 50 && (isalpha(*S) || *S == '_' || isdigit(*S)) )
		str[i++] = *S++;
	    str[i] = '\0';
	    if(strcmp(str,"sizeof") == 0)
		EiC_pre_error("Illegal sizeof operator");
	    else
		*p++ = '0';
	} else
	    *p++ = *S++;
    }
}


int EiC_cpp_parse(char *s)
{

    ppint res;
    S = s;
    replaceDefines(S);

#ifdef DEBUG
    printf("return [%s]\n",S);
#endif

    S = s  = EiC_process2(S,0,0);

    replaceIdentifiers(S);

#ifdef DEBUG
    printf("return [%s]\n",S);
#endif

    res = EiC_ifexpr(0);

    if(s)
        xfree(s);

    if(tval(res) == SIGN)
	return sval(res);
    else
	return uval(res);
}

static ppint EiC_ifexpr(int k)
{
    ppint res;
    int k1, tk;
    res = EiC_expr_unary();
    for(k1 = 10; k1 >= k; k1--)
	while((tk = getTok(k1))) 
	    res = do_binary(tk, res,EiC_ifexpr(k1>8?k1:k1+1));
    return res;
}

#define eval(a,l,r,op)\
{\
     if(tval(l) == SIGN)\
	 sval(a) = sval(l) op sval(r);\
     else\
	 uval(a) = uval(l) op uval(r);\
}
		 

static ppint do_binary(int tk, ppint left, ppint right)
{
    ppint r;

    if(tval(left) == USIGN || tval(right) == USIGN)
	tval(r) = tval(left) = tval(right) = USIGN;
    else
	tval(r) = tval(left) = tval(right) = SIGN;
    

	
    switch(tk) {
      case BOR: eval(r,left,right, | ); break;
      case XOR: eval(r,left,right, ^ ); break;
      case AND: eval(r,left,right, & ); break;
      case LT:  eval(r,left,right, < ); break;
      case LEQ: eval(r,left,right, <= ); break;
      case EQ:  eval(r,left,right,  == ); break;
      case NEQ: eval(r,left,right,  != ); break;
      case GT:  eval(r,left,right, > ); break;
      case GEQ: eval(r,left,right, >= ); break;
      case LOR: eval(r,left,right, || ); break;
      case LAND: eval(r,left,right, && ); break;
      case LSHFT: eval(r,left,right, << ); break;
      case RSHFT: eval(r,left,right, >> ); break; 	
      case PLUS: eval(r,left,right, + ); break;
      case MINUS: eval(r,left,right, - ); break;
      case TIMES: eval(r,left,right, * ); break;
      case DIV: eval(r,left,right, / ); break;
      case MOD: eval(r,left,right, % ); break;
    }
    return r;
}

static int getTok(int k)
{
    TK = 0;
    
    while(isspace(*S))
	S++;
	
    switch(k) {
      case 1: /* LOR */
	if(*S == '|' && *(S+1) == '|') {S+=2; TK = LOR;}
	break;
      case 2: /* LAND */
	if(*S == '&' && *(S+1) == '&') {S+=2; TK = LAND;}
      case 3: /* BOR */
	if(*S == '|' && *(S+1) != '|') {S++; TK = BOR;}
	break;
      case 4: /* XOR */
	if(*S == '^') {S++; TK = GEQ;}
	break;
      case 5: /* AND */
	if(*S == '&' && *(S+1) != '&') {S++; TK = GEQ;}
	break;
      case 6: /* EQ, NEQ */
	if(*S == '=' && *(S+1) == '=') {S+=2; TK = EQ;}
	else if(*S == '!' && *(S+1) == '=') {S+=2; TK = NEQ;}
	break;
      case 7: /* LT, LEQ, GT, GEQ */
	if(*S == '<') {
	    S++;
	    if(*S == '='){S++;TK = LEQ;}
	    else TK = LT;
	} else if(*S == '>') {
	    S++;
	    if(*S == '='){S++; TK = GEQ;}
	    else TK = GT;
	}
	break;
      case 8:  /* LSHFT, RSHFT */
	if(*S == '<' && *(S+1) == '<') {S+=2; TK = LSHFT;}
	else if(*S == '>' && *(S+1) == '>') {S+=2; TK = RSHFT;}
	break;
      case 9: /* PLUS, MINUS */
	if(*S == '-') {S++;TK = MINUS;}
	else if(*S == '+') {S++; TK = PLUS;}
	break;
      case 10: /* TIMES, DIV, MOD */
	if(*S == '*') {S++;TK = TIMES;}
	else if(*S == '/') {S++; TK = DIV;}
	else if(*S == '%') {S++; TK = MOD;}
	break;
    }
    return TK;
}

static int get_oct(int x)
{
    return x>='0'&&x<='7'? x-'0':-1;
}

static int get_hex(int x)
{
    
    if (x >= '0' && x <= '9')
	x -= '0';
    else if (x >= 'a' && x <= 'f')
	 x = x - 'a' + 10;
    else if (x >= 'A' && x <= 'F')
	x = x - 'A' + 10;
    else
	x = -1;
    return x;
}

static int get_dec(int x)
{
    return x >= '0' && x <= '9' ? x-'0':-1;
}


static ppint get_number()   /* collect hex, octal and decimal integers */
{
    int (*f)(int x);
    int radix,val;
    ppint res = {{0},SIGN};

    if(*S == '0') {
	S++;
	if(*S == 'x' || *S == 'X') { /* get hex number */
	    S++;
	    radix = 16;
	    f = get_hex;
	} else { /* get octal number */
	    radix = 8;
	    f = get_oct;
	}
    } else { /* get decimal number */
	radix = 10;
	f = get_dec;
    }
    while((val = (*f)(*S++)) >= 0)
	uval(res) = uval(res) * radix + val;
    S--;

    if(uval(res) > LONG_MAX)
	tval(res) = USIGN;
    
    /* check for prefix */    
    if(*S=='u' || *S=='U') {
	S++;
	tval(res) = USIGN;
    } if(*S=='l' || *S=='L')
	S++;
    return res;
}
	

static int get_charConst()
{
    
    int c;
    switch (*S) {
      case 'n': c = '\n'; break; /* newline */
      case 't': c = '\t'; break; /* tabspace */
      case 'v': c = '\v'; break; /* vertical tab */
      case 'b': c = '\b'; break; /* backspace */
      case 'r': c = '\r'; break; /* carriage return */
      case 'f': c = '\f'; break; /* formfeed */
      case 'a': c = '\a'; break; /* bell */
      case '\\': c = '\\'; break; /* backslash */
      case '\'': c = '\''; break; /* single quote */
      case '"': c = '\"'; break; /* double quote */
      case 'x':			/* string of hex characters */
      case 'X':{
	  int i, val = 0;
	  S++;
	  while ((i = get_hex(*S)) > -1) {
	      S++;
	      val = val * 16 + i;
	  }
	  if (val > 255)
	      EiC_pre_error("Illegal character hex value");
	  c = val;
      }
	break;
      default:
	if (isdigit(*S)) {	/* treat as octal characters */
	    int i, val = 0;
	    while ((i = get_oct(*S)) > -1) {
		val = val * 8 + i;
		S++;
	    }
	    if (val > 255)
		EiC_pre_error("Illegal character octal value");
	    c = val;
	} else {
	    EiC_pre_error("Illegal character escape sequence `\\%c'", *S);
	    c = *S++;
	}
	break;
    }
    return c;
}

static ppint EiC_expr_unary()
{
    ppint res;
    
    while(isspace(*S))
	S++;
    if(isdigit(*S)) {
	res = get_number();
    } else if( *S == '(') {
	S++;
	res = EiC_ifexpr(0);
	if(*S != ')')
	    EiC_pre_error("Unbalanced parenthesis");
	S++;
    } else if(*S == '!') {
	S++;
	res = EiC_expr_unary();
	uval(res) = !uval(res);
    } else if(*S == '-') {
	S++;
	if(*S == '-')
	    EiC_pre_error("-- not allowed in operand of #if");
	res = EiC_expr_unary();	
	tval(res) = SIGN;
	sval(res) = -uval(res);
    } else if(*S == '+') {
	S++;
	if(*S == '+')
	    EiC_pre_error("++ not allowed in operand of #if");
	res = EiC_expr_unary();
    } else if(*S == '~') {
	S++;
	res = EiC_expr_unary();
	uval(res) = ~uval(res);
    } else if(*S == '\'') { /* char constants */
	S++;
	if(*S == '\\') {
	    S++;
	    uval(res) = get_charConst();
	} else
	    uval(res) = *S++;
	if(*S != '\'')
	    EiC_pre_error("Missing closing single quote '");
	else
	    S++;
	tval(res) = SIGN;
    } else
	EiC_pre_error("Illegal constant expression");
    return res;
}








