#ifndef _ARGS_H_
#define _ARGS_H_
/**
 *@file greek.h
 *@brief Header file to be included in programms using args.c
**/
#define ARGS	48
#define MAXARG	80
extern char arg_s[ARGS][MAXARG];
extern int arg_n;

int args(char *);
int inv_args(char *);
int caps(char *s);
int smalls(char *s);
void rmquotes(char *s);
void rmnl(char *s);

#endif //_ARGS_H_
