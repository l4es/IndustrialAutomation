#ifndef _MSR_FLOAT_H_
#define _MSR_FLOAT_H_
/*  FLOAT */

/*  IN INTERRUPT HANDLERS */

/*  In control applications, using a PC and Linux, there are many cases in which it can be useful to use floating point operations in interrupts handlers. Instances of such cases are a few digital controls, even complex ones, that requires a periodic timing with some form of communication with front end supervisory Linux processes. In such a situation one can save the overhead of using a hard real time scheduler, e.g. those made available by RTAI or NMT-RTL (http://www.rtlinux.org/), and do all what is needed from an interrupt handler, timed by an external source or, as shown in examples available in the RTAI distribution, by the PC 8254 timer, or local APIC timers when available.  */
/*  There can clearly be many other situations that can require the use of floating point operations within interrupt handlers.  */
/*  Moreover what is explained below can be useful also in writing kernel modules. Do not get mad in doing strange scaling operations to use integer arithmetic in a module that has to do floating point calculations, use the Floating Point Unit (FPU)!  */

/*  In all such cases the point to be cared of is that, on an INTEL and compatible PC, Linux uses the hard task switching capability available for INTEL x86 CPUs. Such a method causes the setting of the TS (task switched) flag bit in register CR0. When such a bit is set any following hard floating point instruction results in the activation of trap 7 (device not available) until TS is cleared. Linux uses that trap to understand that a newly switched process wants to use the FPU, and thus can clear the TS flag and set up the process environment appropriately by saving the FPU environment, if it was engaged by any previously running process, and restoring ir to that of the currently running process.  */

/*  Thus to use the FPU within an interrupt handler one must do the same things that Linux does, to avoid being caught in endless trap 7 activation and to not disrupt the Linux processing.  */

/*  The following macros make it easier to take appropriate actions to support hard floating point operations in the coding of an interrupt handler.  */

/*  #define save_cr0_and_clts(x) __asm__ __volatile__ ("movl %%cr0,%0; clts" :"=r" (x))   */

/*  #define restore_cr0(x)       __asm__ __volatile__ ("movl %0,%%cr0": :"r" (x));   */

/*  #define save_fpenv(x)        __asm__ __volatile__ ("fnsave %0" : "=m" (x))   */

/*  #define restore_fpenv(x)     __asm__ __volatile__ ("frstor %0" : "=m" (x));   */

/*  Then a typical programming sequence to be used is the following:  */

/*  unsigned long cr0;   */

/*  unsigned long linux_fpe[27];   */

/*  unsigned long task_fpe[27];   */

 
  


/*  save_cr0_and_clts(cr0);  */
/*  # To save Linux cr0 state. Always to be done. */


/*  save_fpenv(linux_fpe); */
/*  # To save Linux FPU environment. Needed only if  */
/*  # any Linux process uses the FPU. */


/*  restore_fpenv(task_fpe); */
/*  # To restore your FPU environment. Needed only if  */
/*  # it can be interrupted or if you left some  */
/*  # intermediate results in it. */

/*  >> PUT HERE ALL YOUR INTERRUPT SERVICE ROUTINE FLOATING POINT CALCULATIONS. <<  */
 
  


/*  save_fpenv(task_fpe); */
/*  # To save your FPU environment. Needed only if there  */
/*  # is a suspect that any intermediate result, to be  */
/*  # used at the next interrupts service, can be left  */
/*  # in it. It should never happen, but maybe the  */
/*  # compiler can do strange things while optimizing. */


/*  restore_fpenv(linux_fpe); */
/*  # To restore the previously saved Linux FPU environment. */


/*  restore_cr0(cr0); */
/*  # To restore Linux cr0. Always to be done. */

/*  Just not to have it blindly.  */
/*  The most important thing is the clts (clear task switched flag) instruction. Save/restores of the FPU are required only if Linux itself uses the FPU, clts always. Recall that the TS flag is always set whenever Linux switches processes and is almost shure that, sooner or later, Linux is going to swap any of its processes between your interrupts requests handling. Assuming it will not can be lethal.  */

/*  Register CR0 must be saved/restored, otherwise your clts can hide to Linux that a process, just switched to, but interrupted by you before Linux is informed that its process needs the FPU, will never be appropriately set when your interrupt handler returns to Linux, after having done  its clts, thus hiding the TS flag to Linux itself.  */

/*  Note also that fnsave clears and initializes the FPU to take default actions in case of illegal operations, e.g. divides by zero and square roots of negative numbers, so no further initialization, e.g.. fninit, is required after an fnsave. Clearly you must be content with the default processing of fault operations, otherwise you must be able to setup the system to trap them with an appropriate routine. That is a whole new problem, entailing an interaction with the corresponding Linux service, and we do not know how, and do not want,  to solve it. In this way you clearly will never know that something went wrong in your data and must include appropriate tests to verify them.  */

/*  About the timing, I can roughly remember that the couple fnsave/frstor is about 1 us on a 233 Mhz PII, i.e. 200 CPU cycles at most. Save restore of CR0 is about 20/30 cycles. So the above is about 2 us overall.  */

/*  Recall again that, while keeping the clts instruction, you can avoid save/restoring Linux FPU if no Linux foreground process does not use it, and generally you know what you are running. Moreover there is often no need to restore/save your FPU registers in the interrupt  handler, if each time you complete your calculations and do not leave anything pending in the FPU. That is likely to be always the case, except for possible and strange compiler optimizations that could use the internal FPU registers to save intermediate results. If you suspect a similar occurance then the assembler generated by the compiler should be checked and/or appropriate volatile declarations used. We never found such an instance.  */

/*  Thus, in the case Linux uses the FPU and you are sure that no meaningful intermediate result was left in the FPU registers at the previous interrupt , you can use the shorter sequence:  */
  
  


/*  save_cr0_and_clts(cr0);  */
/*  # To save Linux cr0 state. Always to be done. */


/*  save_fpenv(linux_fpe); */
/*  # To save Linux FPU environment. Needed only if  */
/*  # any Linux process uses the FPU. */

/*  >> PUT HERE ALL YOUR INTERRUPT SERVICE ROUTINE FLOATING POINT CALCULATIONS. <<  */
  
  


/*  restore_fpenv(linux_fpe); */
/*  # To restore the previously saved Linux FPU environment. */


/*  restore_cr0(cr0); */
/*  # To restore Linux cr0. Always to be done. */

/*  If, beside fulfilling the no intermediate result in the FPU, you are sure that Linux does not, and never will, use the FPU, the all stuff can shrink to the simplest:  */
  
  


/*  save_cr0_and_clts(cr0); */
/*  # To save Linux cr0 state. Always to be done. */

/*  >> PUT HERE ALL YOUR INTERRUPT SERVICE ROUTINE FLOATING POINT CALCULATIONS. <<  */
  

/*  Finally, if Linux does not use the FPU and you suspect that intermediate results are left in the  */
  


/*  restore_cr0(cr0); */
/*  # To restore Linux cr0. Always to be done. */

/*  FPU in your interrupts use:  */
  
  


/*  save_cr0_and_clts(cr0);  */
/*  # To save Linux cr0 state. Always to be done. */


/*  restore_fpenv(task_fpe); */
/*  # To restore your FPU environment. Needed only if  */
/*  # it can be interrupted or if you left some  */
/*  # intermediate results in it. */

/*  >> PUT HERE ALL YOUR INTERRUPT SERVICE ROUTINE FLOATING POINT CALCULATIONS. <<  */
  
  


/*  save_fpenv(task_fpe); */
/*  # To save your FPU environment. Needed only if there  */
/*  # is a suspect that any intermediate result, to be  */
/*  # used at the next interrupts service, can be left  */
/*  # in it. It should never happen, but maybe the  */
/*  # compiler can do strange things while optimizing. */


/*  restore_cr0(cr0); */
/*  # To restore Linux cr0. Always to be done. */

/*  I'll hope there are no typos in the above and we are glad to help you in a matter that costed us a lot of "blood shedding" just to discover, once solved, that was that simple, and could be easily found by "rgrep"ping the Linux kernel sources appropriately.  */
/*  A thank to Daniele Lugli, he was fundamental in pointing out such a crucial problem in relation to both NMT-RTL and our variant of the original RTLinux (available at RTLinux-Variant or ftp.llp.fu-berlin.de/LINUX-LAB/RTAPPS/paolo/)  */




/*  Document written and copyrighted by: P. Mantegazza.  */
/*  Dipartimento di Ingegneria Aerospaziale  */
/*  Politecnico di Milano  */
/*  e-mail: mantegazza@aero.polimi.it */


#ifdef save_cr0_and_clts
#undef save_cr0_and_clts
#undef restore_cr0    
#undef save_fpenv
#undef restore_fpenv
#endif


#define save_cr0_and_clts(x) __asm__ __volatile__ ("movl %%cr0,%0; clts" :"=r" (x))  

#define restore_cr0(x)       __asm__ __volatile__ ("movl %0,%%cr0": :"r" (x));  

#define save_fpenv(x)        __asm__ __volatile__ ("fnsave %0" : "=m" (x))  

#define restore_fpenv(x)     __asm__ __volatile__ ("frstor %0" : "=m" (x));  



#define MSR_FLOAT_DEF                      \
unsigned long cr0;                         \
unsigned long linux_fpe[27]

#define MSR_FLOAT_START                    \
save_cr0_and_clts(cr0);                    \
save_fpenv(linux_fpe)



#define MSR_FLOAT_STOP                     \
restore_fpenv(linux_fpe);                  \
restore_cr0(cr0)                          

#endif

