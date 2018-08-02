#include <stdio.h>
#include <ctype.h>
#include <limits.h>

char * names[] = {
    "isalnum",
    "isalpha",
    "iscntrl",
    "isdigit",
    "isgraph",
    "islower",
    "isprint",
    "ispunct",
    "isspace",
    "isupper",
    "isxdigit"};

int main()
{
    enum{
	ISALNUM,
	ISALPHA,
	ISCNTRL,
	ISDIGIT,
	ISGRAPH,
	ISLOWER,
	ISPRINT,
	ISPUNCT,
	ISSPACE,
	ISUPPER,
	ISXDIGIT};

    int i,c;
    char * s;

    for(i=0;i<=ISXDIGIT;++i) {
	printf("\t%s\n",names[i]);
	for(c=EOF; c <=UCHAR_MAX;c++) 
	    switch(i) {
	      case ISALNUM:
		if(isalnum(c))
		    printf("[%c,%d]",c, isalnum(c));
		break;
	      case ISALPHA:
		if(isalpha(c))
		    printf("[%c,%d]",c, isalpha(c));
		break;
	      case ISCNTRL:
		if(iscntrl(c))
		    printf("[%c,%d]",c, iscntrl(c));
		break;
	      case ISDIGIT:
		if(isdigit(c))
		    printf("[%c,%d]",c, isdigit(c));
		break;
	      case ISGRAPH:
		if(isgraph(c))
		    printf("[%c,%d]",c, isgraph(c));
		break;
	      case ISLOWER:
		if(islower(c))
		    printf("[%c,%d]",c, islower(c));
		break;
	      case ISPRINT:
		if(isprint(c))
		    printf("[%c,%d]",c, isprint(c));
		break;
	      case ISPUNCT:
		if(ispunct(c))
		    printf("[%c,%d]",c, ispunct(c));
		break;
	      case ISSPACE:
		if(isspace(c))
		    printf("[%c,%d]",c, isspace(c));
		break;
	      case ISUPPER:
		if(isupper(c))
		    printf("[%c,%d]",c, isupper(c));
		break;
	      case ISXDIGIT:
		if(isxdigit(c))
		    printf("[%c,%d]",c, isxdigit(c));
		break;
		
	    }
	putchar('\n');
    }

    
    for(s = "0123456789abcdefABCDEF"; *s != 0;s++)
	if(!isxdigit(*s))
	    printf("error isxdigit(%c)\n",*s);
	else
	    printf("+");
    putchar('\n');
    for(s = "0123456789"; *s != 0;s++)
	if(!isdigit(*s))
	    printf("error isdigit(%c)\n",*s);
	else
	    printf("+");
    putchar('\n');
    for(s = "abcdefghigklmnopqrstuvwxyz"; *s != 0;s++)
	if(!islower(*s))
	    printf("error islower(%c)\n",*s);
	else
	    printf("+");
    putchar('\n');
    for(s = "abcdefghigklmnopqrstuvwxyzABCDEFGHIGKLMNOPQRSTUVWXYZ"; *s
	!= 0;s++)
	if(!isalpha(*s))
	    printf("error isalpha(%c)\n",*s);
	else
	    printf("+");
    putchar('\n');
    for(s =
	"0123456789abcdefghigklmnopqrstuvwxyzABCDEFGHIGKLMNOPQRSTUVWXYZ";
	*s !=0;s++)
	if(!isalnum(*s))
	    printf("error isalnum(%c)\n",*s);
	else
	    printf("+");
    putchar('\n');
    return 0;
}


#ifdef EiCTeStS
main();
" to Clean   :rm names";
#endif



