#ifndef LEXERH_
#define LEXERH_

/* lexer.h
 *
 *	(C) Copyright May  7 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */
enum{
  NUM = 350, HEX, OCTAL, STR,
  CHAR,UCHAR,
  SHORT, USHORT,
  INT,UINT,
  LONG, ULONG,
  FLOAT,DOUBLE,

  ID,TYPENAME, FUNCTION,
  INC,		/* ++ */
  DEC,		/* -- */
  RARROW,	/* -> */
  LSHT,		/* << */
  RSHT,		/* >> */

  MISC, /* dummy for EiC_lexan */

  RELOP,  /* relational operators */
  LT,	/* < */
  LE,	/* <= */
  EQ,	/* == */
  NE,	/* != */
  GT,	/* >  */
  GE,	/* >= */

  LOR,	/* || */
  BOR, 	/* | */
  XOR,	/* ^ */

  LAND,	/* && */
  AND,	/* & */

  LOGOP,	/* logical operators */
  NOT,		/* ! */

  ASSOP,	/* assignment operators */
  ASS,		/* =  */
  ADDEQ,	/* += */
  SUBEQ,	/* -= */
  MULEQ,	/* *= */
  DIVEQ,	/* /= */
  MODEQ,	/* %= */
  RSHTEQ,	/* >>= */
  LSHTEQ,	/* <<= */
  ANDEQ,	/* &= */
  BOREQ,	/* |= */
  XOREQ	/* ^= */

};

void EiC_initlex(char *str);
int EiC_lexan(void);
char EiC_nextchar(void);

#if 0

extern unsigned short STOKEN;
#define retractlexan()  STOKEN=token->Tok

#else

/* the following is to provide EiC with
 *  N token lookahead parser
 */

#define ILOOKAHEAD
#define MAX_TOKENS 3

extern token_t EiC_TokenArray[];
extern token_t *token;
extern int EiC_TokenI, EiC_TokenP, EiC_TokenR;
extern char *lex_buff;
extern char EiC_LEXEM[];

#define retractlexan()   do\
{\
     if(EiC_TokenI) {\
	 EiC_TokenR++;EiC_TokenI--;\
         if(EiC_TokenP==0) EiC_TokenP = MAX_TOKENS;\
         EiC_TokenP--;\
	 if(EiC_TokenP) token=&EiC_TokenArray[EiC_TokenP-1];\
         else token=&EiC_TokenArray[MAX_TOKENS-1];\
     }\
}while(0)

#endif

#endif  /* LEXERH_ */







