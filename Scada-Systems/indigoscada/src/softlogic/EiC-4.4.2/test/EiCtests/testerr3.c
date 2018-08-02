
/* more errors that EiC should pick up and recover from */

char *pstr[] = "hello";  /* Error: initialisation error */

; // force error alignment

void constErrors()
{
  const int i = 5;

  ++i;
  i++;
  --i;
  i--;
}

;  // force error alignment


/* error from unsafe and safe pointers */

void f1(char * * p) {1;}
void f2(char * *unsafe q) {1;}

char * * p;
char * *unsafe q;
f1(p);   // okay
f1(q);   // error
f2(p);   // error
f2(q);   // okay

;  // force error alignment

/* error from goto's and labels */
lab1 :;  // error 
;
goto lab1;   // error
;

void f3() 
{

    5;
    
    goto 5;  // error
   
    goto fooey;  // error
}


/* Illegal array domains specifications */

char xx[5][];
char aa, yy[5][0];
char vv[5][0] = {"aa"};

























