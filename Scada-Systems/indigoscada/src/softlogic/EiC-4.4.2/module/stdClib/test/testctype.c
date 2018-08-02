#include "stdio.h"
#include "ctype.h"

int main(void)
{
    
    char *s;
    for(s = "0123456789abcdefABCDEF"; *s;s++)
	if(!isxdigit(*s))
	    printf("error isxdigit(%c)\n",*s);
	else
	    printf("+");
    putchar('\n');
    for(s = "0123456789"; *s;s++)
	if(!isdigit(*s))
	    printf("error isdigit(%c)\n",*s);
	else
	    printf("+");
    putchar('\n');
    for(s = "abcdefghigklmnopqrstuvwxyz"; *s;s++)
	if(!islower(*s))
	    printf("error islower(%c)\n",*s);
	else
	    printf("+");
    putchar('\n');
    for(s = "abcdefghigklmnopqrstuvwxyzABCDEFGHIGKLMNOPQRSTUVWXYZ"; *s;s++)
	if(!isalpha(*s))
	    printf("error isalpha(%c)\n",*s);
	else
	    printf("+");
    putchar('\n');
    for(s = "0123456789abcdefghigklmnopqrstuvwxyzABCDEFGHIGKLMNOPQRSTUVWXYZ"; *s;s++)
	if(!isalnum(*s))
	    printf("error isalnum(%c)\n",*s);
	else
	    printf("+");
    putchar('\n');
    return 0;
}
