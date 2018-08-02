#define CLOCKS_PER_SEC 100

#ifndef _EiC
#define unsafe 
#endif

#define _need_time_t
#define _need_clock_t
#include "sys/stdtypes.h"
#undef _need_clock_t
#undef _need_time_t

typedef struct timespec {
	time_t	tv_sec;
	long	tv_nsec;
} timespec_t;

struct tm {
        int     tm_sec;		/* seconds after the minute [0-60] */
        int     tm_min;		/* minutes after the hour [0-59] */
        int     tm_hour;	/* hours since midnight [0-23] */
        int     tm_mday;	/* day of the month [1-31] */
        int     tm_mon;		/* months since January [0-11] */
        int     tm_year;	/* years since 1900 */
        int     tm_wday;	/* days since Sunday [0-6] */
        int     tm_yday;	/* days since January 1 [0-365] */
        int     tm_isdst;	/* Daylight Savings Time flag */
        long    tm_gmtoff;	/* offset from CUT in seconds */
        char    *unsafe tm_zone; /* timezone abbreviation */
};

char *asctime(struct tm *);
clock_t clock(void);
char *ctime(const time_t *);
double difftime(time_t, time_t);
struct tm *gmtime(const time_t *);
struct tm *localtime(const time_t *);
time_t mktime(struct tm *);
size_t strftime(char *, size_t bufsize, const char *, const struct tm *);
time_t time(time_t *);

#ifndef _EiC
#undef unsafe 
#endif
