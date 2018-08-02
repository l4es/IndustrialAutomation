extern void module_stdio(void);
extern void module_math(void);
extern void module_fcntl(void);
extern void module_string(void);
extern void module_ctype(void);
extern void module_dirent(void);
extern void module_assert(void);
extern void module_time(void);
extern void module_unistd(void);
extern void module_stat(void);
extern void module_stdarg(void);
extern void module_errno(void);
extern void module_stdlib(void);
extern void module_termios(void);
extern void module_signal(void);

void stdClib(void)
{

    module_stdio();
    module_math();
    module_fcntl();
    module_string();
    module_ctype();
    module_dirent();
    module_assert();
    module_time();
    module_unistd();
    module_stat();
    module_stdarg();
    module_errno();
    module_stdlib();
#if 1
    module_signal();
#endif

#ifndef NO_TERMIOS
    module_termios();
#endif
}



