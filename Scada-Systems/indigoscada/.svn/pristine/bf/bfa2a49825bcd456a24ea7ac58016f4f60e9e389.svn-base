#define CLOCKS_PER_SEC	91

#ifndef _EiC
#define unsafe
#endif

struct tm {
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
  char * unsafe __tm_zone;
  int __tm_gmtoff;
};

char *		asctime(const struct tm *_tptr);
clock_t		clock(void);
char *		ctime(const time_t *_cal);
double		difftime(time_t _t1, time_t _t0);
struct tm *	gmtime(const time_t *_tod);
struct tm *	localtime(const time_t *_tod);
time_t		mktime(struct tm *_tptr);
size_t		strftime(char *_s, size_t _n, const char *_format, const struct tm *_tptr);
time_t		time(time_t *_tod);

#ifndef _EiC
#undef unsafe
#endif
