#include <stdio.h>


enum { CHAR, INT, DOUBLE };

enum rgb { RED, GREEN = 13, BLUE };

enum {Ed = 1, CLAIRE, SARAH = 1, ALEX, GEORGIA};

enum Rem;
enum b { abc };
/* EiC should allow for the re-declaration of 
   enumeration types */
enum b { abc };

extern enum rgb colour;


enum Test_e { One = 1, Two, Three };

void SubA( enum Test_e *T1 ) 
{

  enum Test_e T2 = *T1;

  switch( T2 ) {
  case One : printf( "One\n" ); T2 = Two; break;
  case Two : printf( "Two\n" ); T2 = Three; break;
  case Three : printf( "Three\n" ); T2 = One; break;
  default  : printf( "Unknown\n" ); T2 = One; break;
  }
  *T1 = T2;
}


int main()
{
    enum Test_e T1 = Two;

    enum rgb colour = RED;
    enum rgb RGB = 5;
    {    int shep = 13;
        {
	    enum {farm, axe=3, shep};
	    printf("shep = %d :-> shep = 4\n",shep);
	}
	 printf("shep = %d :-> shep = 13\n",shep);
     }    
    printf("colour = %d\n",colour);
    printf("RGB = %d\n",RGB);
    printf("Ed = %d, CLAIRE = %d, SARAH = %d, ALEX %d, GEORGIA = %d\n",
	   Ed,CLAIRE, SARAH,ALEX, GEORGIA);
    printf ("RED = %d,GREEN = %d,BLUE = %d "
	    ":-> RED = 0,GREEN = 13,BLUE = 14  \n", RED, GREEN, BLUE);
    printf ("CHAR = %d,INT = %d,DOUBLE = %d "
	    ":-> CHAR = 0,INT = 1,DOUBLE = 2 \n", CHAR, INT, DOUBLE);


    SubA( &T1 );
    
    return 0;
}

#ifdef EiCTeStS
main();
#endif







