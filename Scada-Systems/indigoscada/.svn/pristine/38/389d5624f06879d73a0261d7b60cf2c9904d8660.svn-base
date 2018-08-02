/* lexer.c
 *
 *	(C) Copyright Apr 15 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 *
 * History:
 *           First written V1.0 1984 (E.J.B)
 *           Revised       V1.2 1994 (E.J.B)
 *           Revised       V2.0 1995 (E.J.B)
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "global.h"
#include "xalloc.h"
#include "error.h"
#include "symbol.h"
#include "lexer.h"
#include "preproc.h"

static void retract(char c);
static int fail(int ival, int c);
static int charliteral(int c);
static void EiC_stringliteral(void);
static void success(int ival);

char nextchar(void);
char *nextproline(void);
void retract(char c);
int fail(int ival, int c);
int charliteral(int c);
void stringliteral(void);
void success(int ival);


#define NEWLINE(ch)  (ch  == '\n' ? TRUE: FALSE)
#define LETTER(ch)   ((ch >=  'a'  && ch   <=  'z') ? TRUE :\
				 ((ch >=  'A'  && ch   <=  'Z') ? TRUE :\
				 ((ch ==  '_'   || ch   ==  '$')? TRUE : FALSE)))

#define DIGIT(ch)   ((ch  >=  '0'   && ch   <= '9') ? TRUE : FALSE)
#define WHITE(ch)   ((ch  == ' '    || ch   == '\t')? TRUE : FALSE)



keyword_t cwords[] =
{
    {"__eiclongjmp",eiclongjmpsym,},
    {"__eicsetjmp",eicsetjmpsym,},

  {"auto", autosym,},
  {"break", breaksym,},
  {"case", casesym,},
  {"char", charsym,},
  {"const", constsym,},
  {"continue", continuesym,},
  {"default", defaultsym,},
  {"do", dosym,},
  {"double", doublesym,},
  {"else", elsesym,},
  {"enum", enumsym,},
  {"extern", externsym,},
  {"float", floatsym,},
  {"for", forsym,},
  {"goto",gotosym,},
  {"if", ifsym,},
  {"int", intsym,},
  {"long", longsym,},
  {"register", registersym,},
  {"return", returnsym,},
  {"safe", safesym,},
  {"short", shortsym,},
  {"signed", signedsym,},
  {"sizeof", sizeofsym,},
  {"static", staticsym,},
  {"struct", structsym,},
  {"switch", switchsym,},
  {"typedef", typedefsym,},
  {"union", unionsym,},
  {"unsafe", unsafesym,},
  {"unsigned", unsignedsym,},
  {"void", voidsym,},
  {"volatile", volatilesym,},
  {"while", whilesym,},
};



#define NOTOKEN 0

char EiC_LEXEM[BSIZE+1];
static token_t TOK;
token_t *token = &TOK;

#ifdef ILOOKAHEAD

token_t EiC_TokenArray[MAX_TOKENS];
int EiC_TokenI = 0, EiC_TokenP = 0, EiC_TokenR=0;

#else

unsigned short STOKEN;

#endif




unsigned lex_lineno, lex_lineprev, lex_linepos, lex_linelen, lex_curpos = 0,
 lex_lastpos;
char *lex_buff;

static int state = 0;
static unsigned long lexival;
static double lexfval;
static char Lseen,  /* long seen */
            Fseen,  /* floating point seen */
            Useen,  /* Unsigned specifier seen */
            Hseen;  /* Hex or Octal value seen */

#define getoct(x)  (((x)>='0'&&(x)<='7')? (x)-'0':-1)


static int gethex(int c)
{
    if (c >= '0' && c <= '9')
	return c - '0';
    if (c >= 'a' && c <= 'f')
	return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
	return c - 'A' + 10;
    return -1;
}


void EiC_initlex(char *str)
{
    lex_buff = str;
    lex_curpos = 0;
    lex_lineprev = 0;
    lex_lineno = 1;

#ifdef ILOOKAHEAD
    EiC_TokenR = EiC_TokenP = EiC_TokenI = 0;
    
#else

    STOKEN = NOTOKEN;
#endif
    
}

char EiC_nextchar()
{
    char *EiC_nextproline();
    if (lex_buff[lex_curpos] == EOF) {
	return 0;
    }
    else if (lex_buff[lex_curpos] == '\0') {
      
	if (lex_curpos != lex_lastpos)
	    return 0;

	lex_buff = EiC_nextproline();
	lex_lastpos = lex_curpos = 0;
	lex_lineprev = 0;
	lex_lineno = 1;
    }
    if (lex_lineprev != lex_lineno) {
	lex_linepos = lex_curpos;
	lex_lineprev = lex_lineno;
    }
    return (lex_buff[lex_curpos++]);
}

static void checkExt(int c)
{				/* check for unsigned and long suffix */
    Lseen = Useen = Fseen = 0;
    if (c == 'f' || c == 'F')
	Fseen = 1;
    else if (c == 'u' || c == 'U') {
	Useen = 1;
	if ((c = EiC_nextchar()) == 'l' || c == 'L')
	    Lseen = 1;
	else
	    retract(c);
    } else if (c == 'l' || c == 'L')
	Lseen = 1;
    else
	retract(c);
}

static void retract(char c)
{
    if (c != '\0') {
	lex_curpos--;
	if (lex_curpos < lex_lastpos)
	    lex_lastpos = lex_curpos;
    }
}


static int fail(int ival, int c)
{
    retract(c);
    switch (ival) {
      case RELOP: return (10);
      case ID:    return (20);
      case FLOAT:
      case INT: return (100);
    }
    return 0;
}

static void setfloatval(void)
{

    if (Lseen || lexfval > FLT_MAX  || !Fseen ) {
	token->Val.dval = lexfval;
	token->Tok = DOUBLE;
    } else {
	/* mandatory conversion to float */
	float f = lexfval;
	token->Val.dval = f;
	token->Tok = FLOAT;
    }

}

static void setintval(void)
{
    if (Fseen) {
	lexfval = lexival;
	setfloatval();
	return;
    }
    if (Useen) {
	if (Lseen || lexival > UINT_MAX) {
	    token->Tok = ULONG;
	    token->Val.ulval = lexival;
	} else {
	    token->Tok = UINT;
	    token->Val.uival = (unsigned) lexival;
	}
    } else if (Lseen || lexival > UINT_MAX) {
	if (lexival > ULONG_MAX) {
	    token->Tok = ULONG;
	    token->Val.ulval = lexival;
	} else if (lexival >= ULONG_MAX) {
	    token->Tok = ULONG;
	    token->Val.ulval = lexival;
	} else {
	    token->Val.lval = lexival;
	    token->Tok = LONG;
	}
    } else {
	if (lexival <= INT_MAX) {
	    token->Val.ival = (int) lexival;
	    token->Tok = INT;
	} else if(Hseen && lexival <= UINT_MAX) {
	    token->Tok = UINT;
	    token->Val.uival = (unsigned) lexival;
	} else if(lexival <= LONG_MAX) {	    
	    token->Val.lval = (long) lexival;
	    token->Tok = LONG;
	} else {
	   token->Tok = ULONG;
	   token->Val.ulval = lexival;
       }
    }
}

static void success(int ival)
{
    int i, size;

    size = (int) (lex_curpos - lex_lastpos);
    memcpy(EiC_LEXEM, &lex_buff[lex_lastpos], size);
    EiC_LEXEM[size] = '\0';
    if (Lseen) size--;
    if (Useen) size--;
    if (Fseen) size--;

    Hseen = 0;
    switch (ival) {
	case ID:
	    if ((token->Tok = EiC_iskeyword(cwords, EiC_LEXEM,
					sizeof(cwords) / sizeof(keyword_t))) == 0) {
		token->Tok = ID;
		/* search for id in various name spaces */
		if ((token->Val.sym = EiC_lookup(EiC_work_tab, EiC_LEXEM)) == NULL)
		    token->Val.sym = EiC_insertLUT(EiC_work_tab, EiC_LEXEM, ID);
		if (token->Val.sym)
		    if (token->Val.sym->sclass == c_typedef)
			token->Tok = TYPENAME;
	    }
	    break;
	case OCTAL:
	    if (Fseen)
		EiC_error("Declaration syntax error");
	    for (lexival = 0, i = 0; i < size; i++)
		lexival = lexival * 8 + getoct(EiC_LEXEM[i]);
	    Hseen = 1;
	    setintval();
	    break;
	case HEX:
	    for (lexival = 0, i = 2; i < size; i++)
		lexival = lexival * 16 + gethex(EiC_LEXEM[i]);
	    Hseen = 1;
	    setintval();
	    break;
	case INT:
	    for (lexival = 0, i = 0; i < size; i++)
		lexival = lexival * 10 + EiC_LEXEM[i] - '0';
	    setintval();
	    break;
	case FLOAT:
	    if (Useen)
		EiC_error("Declaration syntax error");
	    lexfval = atof(EiC_LEXEM);
	    setfloatval();
	    break;
	case RELOP:
	case MISC:
	    break;
    }
}


static int WASLITERAL;
static int charliteral(int c)
{
    if (c == '\\') {
	switch ((c = EiC_nextchar())) {
	case 'n': c = '\n'; break;     /* newline */
	case 't': c = '\t'; break;     /* tabspace */
	case 'v': c = '\v'; break;     /* vertical tab */
	case 'b': c = '\b'; break;     /* backspace */
	case 'r': c = '\r'; break;     /* carriage return */
	case 'f': c = '\f'; break;     /* formfeed */
	case 'a': c = '\a'; break;     /* bell */
	case '\\': c = '\\'; break;    /* backslash */
	case '\'': c = '\''; break;    /* single quote */
	case '"': c = '\"'; break;     /* double quote */
        case '?': c = '\?'; break;     /* question mark */
	case 'x':		       /* string of hex characters */
	case 'X':{
	    int i, val = 0;
	    while ((i = gethex((c = EiC_nextchar()))) > -1) {
		val = val * 16 + i;
	    }
	    retract(c);
	    if (val > 255)
		EiC_error("Illegal character hex value");
	    c = val;
	}
	break;
	default:
	    if (getoct(c) > -1) {		/* octal characters */
		int i, val = 0;
		while ((i = getoct(c)) > -1) {
		    val = val * 8 + i;
		    c = EiC_nextchar();
		}
		retract(c);
		if (val > 255)
		    EiC_error("Illegal character octal value");
		c = val;
	    } else
		EiC_error("Illegal character escape sequence `\\%c'", c);
	    break;
	}
	WASLITERAL = 1;
    } else
	WASLITERAL = 0;
    return ((signed char )c);
}

static void EiC_stringliteral(void)
{
    unsigned size, lastsize = 0, c;
    char *p=NULL;
    lex_lastpos = lex_curpos;
    do {

	for (size = 0; ((c = charliteral(EiC_nextchar())) != '\0' || WASLITERAL) &&
	     !(c == '"' && !WASLITERAL)  && size < BSIZE; size++) 
	    EiC_LEXEM[size] = c;
    

	if (lastsize)
	    p = (char *) xrealloc(p, lastsize + size + 1);
	else
	    p = (char *) xcalloc(size + 1, sizeof(char));

	memcpy(&p[lastsize], EiC_LEXEM, size);
	lastsize += size;

	if(c != '"' && size == BSIZE) {
	    p[lastsize++] = c;
	    continue;
	}
	
	if (c != '"')
	    EiC_error("String literal error");

	do {
	    c = EiC_nextchar();
	    if (c == '\n')
		lex_lastpos++, lex_lineno++;
	} while (WHITE(c) || c == '\n');

	lex_lastpos = lex_curpos;
	if (!c)
	    do
		c = EiC_nextchar();
	    while (WHITE(c));
    } while (c == '"' || size == BSIZE);
    retract(c);
    p[lastsize] = '\0';
    token->Val.p.sp = token->Val.p.p = p;
    token->Val.p.ep = p + lastsize + 1;
}

extern int EiC_lexan(void)
{
    int t=0, loop; char c=0, EiC_nextchar();

#ifdef ILOOKAHEAD

    token = &EiC_TokenArray[EiC_TokenP];

    if(EiC_TokenR > 0) {
	EiC_TokenR--;
	EiC_TokenI++;
	EiC_TokenP=(EiC_TokenP+1)%MAX_TOKENS;
	return token->Tok;
    }


#else

    if (STOKEN != NOTOKEN) {
	STOKEN = NOTOKEN;
	return token->Tok;
    }

#endif
    
    loop  = 1;
    state = 0;
    while (loop) {
	switch (state) {
	  case 0: lex_lastpos = lex_curpos; c = EiC_nextchar();
	    state = (WHITE(c) ? 0 :
		    (c == '\n' ? lex_lineno++, 0 :
		    (c == '<' ? t = LT, 1 :
		    (c == '>' ? t = GT, 2 :
		    (c == '+' ? t = '+', 3 :
		    (c == '-' ? t = '-', 4 :
		    (c == '|' ? t = BOR, 5 :
		    (c == '&' ? t = AND, 6 :
		    (c == '\''? 7 :
		    (c == '"' ? 8 :
		    (c == '.' ? 9 :  
		    (c == '/' ? t = '/', c = EiC_nextchar(), 50 :
		    (c == '%' ? t = '%', c = EiC_nextchar(), 50 :
		    (c == '*' ? t = '*', c = EiC_nextchar(), 50 :
		    (c == '=' ? t = ASS, c = EiC_nextchar(), 50 :
		    (c == '!' ? t = NOT, c = EiC_nextchar(), 50 :
		    (c == '^' ? t = XOR, c = EiC_nextchar(), 50 :
		     fail(RELOP, c))))))))))))))))));
	    break;
	  case 1: /* get <,  <= and << */
	    if ((c = EiC_nextchar()) == '<') t = LSHT;
	    else state = 50;
	    break;
	  case 2: /* get >, >= and >> */
	    if ((c = EiC_nextchar()) == '>') t = RSHT;
	    else state = 50;
	    break;
	  case 3: c = EiC_nextchar();                         /* get +, += or ++ */
	    if (c == '+') t = INC, state = 60;
	    else state = 50;
	    break;
	  case 4: c = EiC_nextchar();                            /* get -, -= -- */
	    state = 60;
	    if (c == '-') t = DEC;
	    else if (c == '>') t = RARROW;
	    else state = 50;
	    break;
	  case 5: c = EiC_nextchar();                         /* get |, |= or || */
	    if (c == '|') t = LOR, state = 60;
	    else state = 50;
	    break;
	  case 6: c = EiC_nextchar();                         /* get &, &= or && */
	    if (c == '&') t = LAND, state = 60;
	    else state = 50;
	    break;
	  case 7:token->Val.ival = charliteral(EiC_nextchar()); /* char_constants */
	    t = CHAR;
	    if (EiC_nextchar() != '\'')
		EiC_error("Missing single quote '");
	    state = 60;
	    break;
	  case 8: EiC_stringliteral();                        /* string literals */
	    token->Tok = STR;
	    /*return STR;*/ loop = 0; break;
	  case 9: c = EiC_nextchar();
	    t = '.';
	    if(DIGIT(c)) 
		state = 22;
	    else
		state = 60;
	    retract(c);
	    break;
	  case 10: c = EiC_nextchar();              /* identifiers and  keywords */
	    state = (LETTER(c) ? 11 :
		    (c == '_' ? 11 : fail(ID, c)));
	    break;
	  case 11: c = EiC_nextchar();
	    state = (LETTER(c) ? 11 :
		    (DIGIT(c) ? 11 :
		    (c == '_' ? 11 : 12)));
	    break;
	  case 12: retract(c); success(ID); /*return (token->Tok);*/ loop = 0; break;

	  case 20: c = EiC_nextchar();                     /* integers and reals */
	    state = (c == '0' ? 30 :
		    (DIGIT(c) ? 21 : fail(INT, c)));
	    break;
	  case 21: c = EiC_nextchar();
	    state = (DIGIT(c) ? 21 :
		    (c == '.' ? 22 :
		    (c == 'e' ? 23 :
		    (c == 'E' ? 23 : 25))));
	    break;
	  case 22: c = EiC_nextchar();
	    state = (DIGIT(c) ? 22 :
		    (c == 'e' ? 23 :
		    (c == 'E' ? 23 : 26)));
	    break;
	  case 23: c = EiC_nextchar();
	    state = (c == '+' ? 24 :
		    (c == '-' ? 24 :
		    (DIGIT(c) ? 24 : fail(FLOAT, c) /* ??? */ )));
	    break;
	  case 24: c = EiC_nextchar();
	    state = (DIGIT(c) ? 24 : 26);
	    break;
	  case 25: checkExt(c); success(INT); /*return (token->Tok);*/ loop = 0; break;
	  case 26: checkExt(c); success(FLOAT); /*return (token->Tok);*/ loop = 0; break;
	  case 27: checkExt(c); success(HEX);   /*return (token->Tok);*/ loop = 0; break;
	  case 28: checkExt(c); success(OCTAL); /*return (token->Tok);*/ loop = 0; break;
	  case 30:			  /* check for octal and hex numbers */
	    if ((c = EiC_nextchar()) == 'x' || c == 'X') {
		while (gethex((c = EiC_nextchar())) > -1);
		state = 27;
		break;
	    }
	    if (c != '.' && c != 'e' && c != 'E') {
		while (getoct(c) > -1)
		    c = EiC_nextchar();
		state = 28;
		break;
	    }
	    retract(c); state = 21; break;
	  case 50:                                      /* mix with equal's  */
	    if (c == '=')
		switch (t) {
		  case '+': t = ADDEQ;  break;		/* += */
		  case '-': t = SUBEQ;  break;		/* -= */
		  case '/': t = DIVEQ;  break;		/* /= */
		  case '*': t = MULEQ;  break;		/* *= */
		  case '%': t = MODEQ;  break;		/* %= */
		  case ASS: t = EQ;     break;		/* == */
		  case GT:  t = GE;     break;		/* >= */
		  case LT:  t = LE;     break;		/* <= */
		  case NOT: t = NE;     break;		/* != */
		  case RSHT:t = RSHTEQ; break;		/* >>= */
		  case LSHT:t = LSHTEQ; break;		/* <<= */
		  case AND: t = ANDEQ;  break;		/* &= */
		  case BOR: t = BOREQ;  break;		/* |= */
		  case XOR: t = XOREQ;  break;		/* ^= */
		  default: retract(c);
	    } else retract(c);
	    state = 60;
	    break;
	  case 60: success(MISC); token->Tok = t; /*return (token->Tok);*/ loop = 0; break;
	  case 100: token->Tok = EiC_nextchar(); /*return (token->Tok);*/ loop = 0; break;
	}
    }

#ifdef ILOOKAHEAD

    if(EiC_TokenI<MAX_TOKENS)
	EiC_TokenI++;

    EiC_TokenP = (EiC_TokenP +1)%MAX_TOKENS;

#endif

    return token->Tok;


}




