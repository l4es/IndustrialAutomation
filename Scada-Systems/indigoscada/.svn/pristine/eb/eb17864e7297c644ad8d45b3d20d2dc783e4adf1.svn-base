#include <stdio.h>
#include <stdlib.h>

#define tmp "tabc11"

int main()
{

    system("ln -s tst1.c " tmp);
    system("eic filetype.c .. filetype.c " tmp);
    system("eic unlink.c ");


    system("touch foo bar");
    system("eic changemod.c");
    system("rm foo bar");

    
    remove(tmp);
    return 0;
}
