/* test signal functions */

#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <limits.h>

        /* static data */
int sigs[] = {
     SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM};
static void (*rets[])(int) = {SIG_DFL, SIG_ERR, SIG_IGN};
static sig_atomic_t atomic;

void tick(int i) { printf("tick\n"); return;}
void tock(int i) { printf("tock\n"); return;}

void myexit(int x) { printf("Exiting via myexit\n"); exit(0); }

void myraised1(int x) { printf("raised 1\n"); }
void myraised2(int x) { printf("raised 2\n"); }

void T1()
{
    int i = 0; 
    int cnt = 0;

    while(1) {
	signal(SIGALRM,tick); 
	sleep(1);
	raise(SIGALRM);
	
	signal(SIGALRM,tock); 
	alarm(1);
	pause(); 
	if(i++==cnt) 
	    break; 
	
    }

    signal(SIGALRM,SIG_DFL);
}

void T2()
{
    int a = 0;
    void (*h)(int);
    h = signal(SIGFPE,myraised1);
    raise(SIGFPE);

    signal(SIGFPE,myraised2);
    raise(SIGFPE);

    signal(SIGFPE,h);
    
}

void catch_function(int);

void T3(void)
{
    if(signal(SIGINT, catch_function)==SIG_ERR)
    {
        printf("An error occured while setting a signal handler.\n");
        exit(0);
    }

    printf("Raising the interactive attention signal.\n");
    if(raise(SIGINT)!=0)
    {
        printf("Error raising the signal.\n");
        exit(0);
    }
    printf("leaving interactive attention signal.\n");
}

void catch_function(int signal)
{
    printf("Interactive attention signal caught.\n");
}

/* This flag controls termination of the T4 loop. */
volatile sig_atomic_t keep_going = 1;

/* The signal handler just clears the flag and re-enables itself. */
void  catch_alarm (int sig)
{
  keep_going = 0;
  signal (sig, catch_alarm);
  printf("MY ALARM\n");
}

void  do_stuff (void)
{
  if(keep_going)
      puts ("Doing stuff while waiting for alarm....");
}

void T4 (void)
{
    unsigned int k;
  /* Establish a handler for SIGALRM signals. */
  signal (SIGALRM, catch_alarm);

  /* Set an alarm to go off in a little while. */
  alarm (1);

  /* Check the flag once in a while to see when to quit. */
  while (keep_going) {
      for(k=0;k<SHRT_MAX;++k)
	  ;
    do_stuff ();
  }

  return;
}

int main()
{

    T1();
    T2();


    T3();
    T4();

    signal(SIGTERM,myexit);
    raise(SIGTERM);

    return 0;
}

#ifdef EiCTeStS
main();
#endif






