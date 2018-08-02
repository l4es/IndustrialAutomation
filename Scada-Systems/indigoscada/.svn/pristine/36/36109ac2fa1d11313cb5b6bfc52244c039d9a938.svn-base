/* error.c
 *
 *	(C) Copyright Apr 15 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */



#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "lexer.h"
#include "xalloc.h"
#include "preproc.h"
#include "error.h"

#include <stdarg.h>

int EiC_ParseError, EiC_ErrorRecover;
int EiC_errs = 0;


static void messageDisplay(char *msg);

void (*EiC_messageDisplay)(char *) = messageDisplay;



void EiC_setMessageDisplay(void (*t)(char *msg))
{
    EiC_messageDisplay = t;
}


               /*CUT match.cut*/
int EiC_match(int t, char *m)
{
    extern token_t *token;
    int lookahead;
    char message[80];

    lookahead = EiC_lexan();
    if (EiC_ErrorRecover) {		/* error recovery method */
	while ((lookahead != t && lookahead != ';') && lookahead != DONE) {
	    if(lookahead == STR) 
		xfree(token->Val.p.p);
	    lookahead = EiC_lexan();
	}
	if (lookahead == t || lookahead == ';') {
	    EiC_ErrorRecover = 0;
	    if (lookahead != t)
		retractlexan();
	    return 1;
	} else
	    return 0;
    }
    if (lookahead != t) {

	sprintf(message, "Expected %s", m);
	EiC_error(message);
	return 0;
    }
    return 1;
}
              /*END CUT*/


void EiC_clear_err_msgs(void)
{
    EiC_errs = 0;
}

static void messageDisplay(char *msg)
{
    fprintf(stderr,msg);
}

void EiC_formatMessage(char *msg, ...)
{
    va_list args;
    char buff2[1024];
    va_start(args,msg);


    vsprintf(buff2,msg,args);
    EiC_messageDisplay(buff2);

    va_end(args);
}

void EiC_error(char *msg, ...)
{
    char *buff1, *buff2;
    va_list args;
    int ln;
    
    va_start(args,msg);

    ln = strlen(msg);
    buff1 = malloc(ln + 256);
    buff2 = malloc(ln + 512);
    
    if (!EiC_ErrorRecover) {
	EiC_errs++;
	EiC_ErrorRecover = EiC_ParseError = 1;
	sprintf(buff1,"Error in %s near line %d: %s\n",
		CurrentFileName(),
		CurrentLineNo(),
		msg);
	vsprintf(buff2,buff1,args);
	EiC_messageDisplay(buff2);
    }
    free(buff1);
    free(buff2);
    va_end(args);
}

void EiC_warningerror(char *msg, ...)
{
    va_list args;
    char *buff1, *buff2;
    int ln;


    va_start(args,msg);

    ln = strlen(msg);
    buff1 = malloc(ln + 256);
    buff2 = malloc(ln + 512);

    EiC_errs++;
    sprintf(buff1,"Warning: in %s near line %d: %s\n",
	    CurrentFileName(),
	    CurrentLineNo(),
	    msg);
    vsprintf(buff2,buff1,args);
    EiC_messageDisplay(buff2);

    free(buff1);
    free(buff2);
    va_end(args);
}






