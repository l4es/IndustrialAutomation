#define CLOCKS_PER_SEC 100000

struct  tm {
        int     tm_sec;
        int     tm_min;
        int     tm_hour;
        int     tm_mday;
        int     tm_mon;
        int     tm_year;
        int     tm_wday;
        int     tm_yday;
        int     tm_isdst;
        char    *tm_zone;
        long    tm_gmtoff;
};

char *asctime(struct tm * tm);
clock_t clock(void);
char *ctime(time_t * clock);
double difftime(time_t t1, time_t t2);
struct tm *gmtime(time_t *clock);
struct tm *localtime(time_t *clock);
/*time_t mktime(struct tm * tm);*/
size_t strftime(char * buf, size_t bufsize, char * fmt,struct tm *tm);
time_t time(time_t * tloc);


/**********      None standard SUNOS functions not implemented in EiC
char *strptime(char *buf, char *fmt, struct tm * tm);
time_t timegm(struct tm *tm);
time_t timelocal(struct tm *tm);
void tzset();
void tzsetwall();
int dysize(int y);
***********/








