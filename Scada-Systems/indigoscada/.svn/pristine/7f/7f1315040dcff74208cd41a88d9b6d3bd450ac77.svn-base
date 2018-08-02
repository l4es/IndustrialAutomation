#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <unistd.h>

void do_fileflags()
{
    system("eic fileflags.c fileflags.c");
    system("eic fileflags.c  0 < /dev/tty");
    system("eic fileflags.c  1 > temp.foo");
    system("cat temp.foo");
    system("eic fileflags.c  2 2>>temp.foo");
    remove("temp.foo");
}

void do_filetype()
{
    system("eic filetype.c filetype.c /usr /etc");
}


void do_access()
{
    system("eic access.c");
    system("eic access.c access.c");
}

void do_unistd()
{
    system("eic seek.c < seek.c");
    system("eic hole.c");
    system("od -c file.hole");
    sleep(1);
    remove("file.hole");
    system("eic mycat.c < mycat.c");
}
int main(void)
{
    do_fileflags();
    do_filetype();
    do_access();
    do_unistd();
    return 0;
}

#ifdef EiCTeStS
main();
#endif












