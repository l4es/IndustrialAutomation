#include <stdio.h>


/* The following string, which was pilfered from a CH demo module, is
 * larger than EiC's lexem buffer
 */

char usage[] = "double getNumber( double f ):\n\
\n\
    This function is passed a default value then requests a\n\
    number from stdin.  If a RETURN is entered, the default\n\
    value is returned.  If a new number is entered, the new\n\
    number is returned. However, if an invalid number is\n\
    entered, the function asks for a number again.\n\
\n\
    This function is passed a default value then requests a\n\
    number from stdin.  If a RETURN is entered, the default\n\
    value is returned.  If a new number is entered, the new\n\
    number is returned. However, if an invalid number is\n\
    entered, the function asks for a number again.\n\
\n\
    This function is passed a default value then requests a\n\
    number from stdin.  If a RETURN is entered, the default\n\
    value is returned.  If a new number is entered, the new\n\
    number is returned. However, if an invalid number is\n\
    entered, the function asks for a number again.\n\
\n\
\n\
    This function is passed a default value then requests a\n\
    number from stdin.  If a RETURN is entered, the default\n\
    value is returned.  If a new number is entered, the new\n\
    number is returned. However, if an invalid number is\n\
    entered, the function asks for a number again.\n\
\n\
    This function is passed a default value then requests a\n\
    number from stdin.  If a RETURN is entered, the default\n\
    value is returned.  If a new number is entered, the new\n\
    number is returned. However, if an invalid number is\n\
    entered, the function asks for a number again.\n\
\n\
    This function is passed a default value then requests a\n\
    number from stdin.  If a RETURN is entered, the default\n\
    value is returned.  If a new number is entered, the new\n\
    number is returned. However, if an invalid number is\n\
    entered, the function asks for a number again.\n\
\n\
";

/* the following string is exactly equal to the LEXEM buffer size */
char lexem[] = "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678";

/* the following string is a multiple of the LEXEM buffer size */
char lexemM[] =
"012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
"012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"


"012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"


"012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
;

int main(void)
{
    puts(usage);
    puts(lexem);
    puts("\n");
    puts(lexemM);
    puts("\n");
    return 0;
}

#ifdef EiCTeStS
main();
#endif

