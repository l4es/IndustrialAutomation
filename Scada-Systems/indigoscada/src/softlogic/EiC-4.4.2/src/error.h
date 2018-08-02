#ifndef ERRORH_
#define ERRORH_

/* error handling  error.c
---------------------------*/
void EiC_error(char *, ...);
int EiC_match(int t, char * m);
void EiC_warningerror(char *, ...);
void EiC_clear_err_msgs(void);
void EiC_formatMessage(char *,...);


/* error handling  globals
---------------------------*/
extern int EiC_ParseError;          /* if 1 an error has occured */
extern int EiC_ErrorRecover;       /* if 1 attempting to recover from error*/
extern int EiC_errs;           /* Number of errors and warnings */

extern void (*EiC_messageDisplay)(char *);  /* pointer function to
					     * message Display
					     * function
					     */

#endif
