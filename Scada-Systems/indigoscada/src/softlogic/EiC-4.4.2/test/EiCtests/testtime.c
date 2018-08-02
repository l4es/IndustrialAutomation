#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


int main()
{
    char buf[32];
    clock_t tc = clock();
    struct tm ts1;
    time_t tt1, tt2;

    static char * dstr = "Sun Dec  2 06:55:15 1979\n";

    tt1 = time(&tt2);
    assert(tt1 == tt2);

    ts1.tm_sec = 15;
    ts1.tm_min = 55;
    ts1.tm_hour = 6;
    ts1.tm_mday = 2;
    ts1.tm_mon = 11;
    ts1.tm_year = 79;
    ts1.tm_isdst = -1;

#if !defined(_SUNOS)  /* hack for bloody non standard SUNOS system */
    tt1 = mktime(&ts1);
    assert(ts1.tm_wday == 0);
    assert(ts1.tm_yday == 335);
    ++ts1.tm_sec;
    tt2 = mktime(&ts1);
#else
    tt1 = 312926115;
    tt2 = 312926116;
#endif
    
    assert(difftime(tt1,tt2) < 0.0);
    assert(strcmp(asctime(localtime(&tt1)),dstr) == 0);
    assert(strftime(buf, sizeof(buf),"%S",gmtime(&tt2)) == 2);
    assert(strcmp(buf,"16") == 0);
    assert(tc <= clock());

    /*
    fputs("Current date -- ", stdout);
    time(&tt1);
    fputs(ctime(&tt1),stdout);
    */
    return 0;
}

#ifdef EiCTeStS
main();
#endif

