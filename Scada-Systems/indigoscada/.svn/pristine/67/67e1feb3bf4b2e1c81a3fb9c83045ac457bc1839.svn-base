#ifndef STDLIBLOCALH_
#define STDLIBLOCALH_

char * strrev(char *s);
char *utoa(unsigned int n, char *s, int radix);
char *itoa(int n, char *s, int radix);
char *ultoa(unsigned long n, char *s, int radix);
char *ltoa(long n, char *s, int radix);

char *fftoa(double d, char *str, int p, char type, int trunc);

int charout_(int c, char **buf);
int charin_(char **buf);
int charback_(int ch, char **buf);

int strround(char *s,int len);


#define setEp(P,sz)  (P).ep = (char*)(P).p + (sz)

#define getptrarg(x,y)  y = arg(x,getargs(),ptr_t)
#define checkEp(P,s)  if((char*)P.p + (s) > (char *)P.ep)\
                             EiC_Mod_Error("Access beyond array\n");

extern void EiC_Mod_Error(char *fmt, ...);

#endif
