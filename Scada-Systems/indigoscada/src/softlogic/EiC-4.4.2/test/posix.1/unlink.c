#include <stdlib.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	"ourhdr.h"

int
main(void)
{

    system("touch tempfile");
    if (open("tempfile", O_RDWR) < 0)
	err_sys("open error");

    if (unlink("tempfile") < 0)
	err_sys("unlink error");

    printf("file unlinked\n");
    printf("done\n");

    return 0;
}


