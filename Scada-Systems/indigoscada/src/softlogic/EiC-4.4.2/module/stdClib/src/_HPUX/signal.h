#ifndef _SYSSIGNALH
#define _SYSSIGNALH

#  define _SIGHUP       1       /* floating point exception */
#  define SIGINT        2       /* Interrupt */
#  define _SIGQUIT      3       /* quit */
#  define SIGILL        4       /* Illegal instruction (not reset when
                                   caught) */
#  define _SIGTRAP      5       /* trace trap (not reset when caught) */
#  define SIGABRT       6       /* Process abort signal */
#  define _SIGIOT       SIGABRT /* IOT instruction */
#  define _SIGEMT       7       /* EMT instruction */
#  define SIGFPE        8       /* Floating point exception */
#  define _SIGKILL      9       /* kill (cannot be caught of ignored) */
#  define _SIGBUS       10      /* bus error */
#  define SIGSEGV       11      /* Segmentation violation */
#  define _SIGSYS       12      /* bad argument to system call */
#  define _SIGPIPE      13      /* write on a pipe with no one to read it */
#  define _SIGALRM      14      /* alarm clock */
#  define SIGTERM       15      /* Software termination signal from kill */
#  define _SIGUSR1      16      /* user defined signal 1 */
#  define _SIGUSR2      17      /* user defined signal 2 */
#  define _SIGCHLD      18      /* Child process terminated or stopped */
#  define _SIGCLD       _SIGCHLD        /* death of a child */
#  define _SIGPWR       19      /* power state indication */
#  define _SIGVTALRM    20      /* virtual timer alarm */
#  define _SIGPROF      21      /* profiling timer alarm */
#  define _SIGIO        22      /* asynchronous I/O */
#  define _SIGPOLL      _SIGIO  /* for HP-UX hpstreams signal */
#  define _SIGWINCH     23      /* window size change signal */
#  define _SIGWINDOW    _SIGWINCH /* added for compatibility reasons */
#  define _SIGSTOP      24      /* Stop signal (cannot be caught or ignored) */
#  define _SIGTSTP      25      /* Interactive stop signal */
#  define _SIGCONT      26      /* Continue if stopped */
#  define _SIGTTIN      27      /* Read from control terminal attempted by a
                                   member of a background process group */
#  define _SIGTTOU      28      /* Write to control terminal attempted by a
                                   member of a background process group */
#  define _SIGURG       29      /* urgent condition on IO channel */
#  define _SIGLOST      30      /* remote lock lost  (NFS)        */
#  define _SIGRESERVE   31      /* Save for future use */
#  define _SIGDIL       32      /* DIL signal */

#  define SIGUSR1      _SIGUSR1
#  define SIGUSR2      _SIGUSR2
#  define SIGBUS       _SIGBUS
#  define SIGTSTP      _SIGTSTP

/* signal() args & returns */
#define SIG_ERR ((void (*) (__harg))-1)
#define SIG_DFL ((void (*) (__harg))0)
#define SIG_IGN     ((void (*) (__harg))1)

#endif

