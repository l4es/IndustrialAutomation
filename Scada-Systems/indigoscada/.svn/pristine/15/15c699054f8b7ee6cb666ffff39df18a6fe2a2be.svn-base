#ifndef _SIGNALH
#define _SIGNALH

#pragma push_safeptr

#include "unistd.h"
#include "sys/signal.h"

int kill(pid_t id, int sig);

typedef int    sig_atomic_t;

void (*signal(int sig, void (*func)(int a))) (int a);


#define raise(sig)   kill(getpid(),sig);

#pragma pop_ptr

#endif
