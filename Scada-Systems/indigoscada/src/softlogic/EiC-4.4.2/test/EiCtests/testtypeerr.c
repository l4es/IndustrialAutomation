char c = 'f';
unsigned char uc = 'g';
short s = -2;
unsigned short us = 3;
int i = -4;
unsigned int ui = 7;
long l = 7998;
unsigned long ul = 987535;
float f = -5.2;
double d = 7234.33e-10;
void *vp = 0;
char *cp = &c;
int *ip = &i;
float *fp = &f;


typedef struct {int i; char c; float f;} icftype;
icftype icf = {3,'g',4.3,};	/* error: expected {		    */

if (c==i) "c == i"; else "c <> i";

c = i;
s = ui;
i = s;
uc = ul;
us = d;
c = f;

fp--;
fp++;

vp = cp - ip;			/* error: mixed pointer operation    */
fp = fp + fp;			/* error: incompatible types         */
fp -= fp;		      	/* warning: Suspicious pointer conversion*/
fp = fp - fp;			/* warning: Suspicious pointer conversion*/
ip = vp - 1;                    /* error: illegal pointer operation  */
ip = fp - 1; 
f = 3.1 % f;         /* error: illegal binary operation  to `%' */
ip = (int*) d;
d = !f+!f;
d = ~f;
i = -ip;
c = (char) +cp;














