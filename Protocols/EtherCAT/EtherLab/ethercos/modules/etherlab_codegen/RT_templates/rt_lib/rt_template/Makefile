
#Comment/uncomment the following line to disable/enable debugging
#DEBUG = y

#Modul laeuft im Simulationsmodus
SIMULATION = y
#UML=y

#RTAI ?
#RTAI = y

TIMERSOURCE = TQ

#TIMERSOURCE = TIMER_LINUX      #Zeitgeber ist der Timerinterrupt des Betriebssystems 
                               #der muß hierfür Sharable sein
#TIMERSOURCE = TIMER_APCI3120   #Zeitgeber ist der Timer der APCI3120

# KERNELDIR bei Simulation der aktuelle Kernel ansonsten Waldikernel
	PFAD=/vol/projekte/
#	PFAD=/vol/projekte/
ifeq ($(SIMULATION),y)
	KERNELDIR = linux
#	KERNELDIR = /vol/projekte/msr_messen_steuern_regeln/linux/kernel/2.4.20/include/linux-2.4.20.CX1100-rthal5
	INCLUDEDIR = $(KERNELDIR)    #/include

	RTAIDIR = /usr/src/rtai
#	INCLUDEDIR = /lib/modules/`uname -r`/build/include

	ifeq ($(UML),y)
	KERNELDIR = /home/ab/uml/linux-2.4.27
	INCLUDEDIR = $(KERNELDIR)/include  -I $(KERNELDIR)/arch/um/include -I $(KERNELDIR)/arch/um/kernel/tt/include #Headers from Kernel
	endif
#	KERNELDIR = $(PFAD)lhd_luk_hochdynamik/software/kernel/linux
#	RTAIDIR = $(PFAD)lhd_luk_hochdynamik/software/kernel/rtai
#	INCLUDEDIR = $(KERNELDIR)/include  #Headers from Kernel
else
#	KERNELDIR = /usr/src/linux-2.4.20.Waldi
#	INCLUDEDIR = /usr/src/linux-2.4.20.Waldi/include  #Headers from Kernel
#       Pfadangaben muessen hier absolut sein, da sie nach rt_lib exportiert werden
	KERNELDIR = /usr/src/linux
	RTAIDIR = /usr/src/rtai
#	KERNELDIR = $(PFAD)lhd_luk_hochdynamik/software/kernel/linux
#	RTAIDIR = $(PFAD)lhd_luk_hochdynamik/software/kernel/rtai
	INCLUDEDIR = $(KERNELDIR)/include  #Headers from Kernel

endif	

#includeverzeichnis der rtlib

RTINCLUDE = rt_lib/msr-include

# CFLAGS: all assignments to CFLAGS are incremental, so you can specify
# the initial flags on the command line or environment, if needed.
	CFLAGS = -O2
	CFLAGS +=  -Wall -Wuninitialized -D__KERNEL__ -DMODULE -I$(INCLUDEDIR) -I$(RTINCLUDE)

ifdef CONFIG_SMP
	CFLAGS += -D__SMP__ -DSMP
endif

# Prepend modversions.h if we're running with versioning.
ifdef CONFIG_MODVERSIONS
	CFLAGS += -DMODVERSIONS -include $(KERNELDIR)/include/linux/modversions.h
endif

# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DMSR_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

CFLAGS += $(DEBFLAGS)
#CFLAGS += -I..

# Add Simulationflag
ifeq ($(SIMULATION),y)
 CFLAGS += -D_SIMULATION
endif

ifeq ($(UML),y)
	CFLAGS += -D_UML
endif

# Add RTAIflag
ifeq ($(RTAI),y)
 CFLAGS += -D_RTAI -I$(RTAIDIR)/include
endif

# Fi: libm.a in /usr/lib (no shared object!)
LDFLAGS =  -L/usr/lib -lm


TARGET = msr_modul
MODULE = $(TARGET).o


#Source mit simulierter Hardware
#ifeq ($(SIMULATION),y)
# TIMERSOURCE = TIMER_LINUX
#endif

SRC = msr_module.c msr_control.c msr_io.c $PRJ$_globals.c  $PRJ$_statemaschine.c

# Add Timersource
CFLAGS += -D_$(TIMERSOURCE)

#Suchpfad für die Dateien aus dem RT-Lib-Verzeichnis
VPATH = rt_lib/msr-core:rt_lib/msr-control:rt_lib/msr-hwdriver:rt_lib/msr-math:rt_lib/msr-misc:rt_lib/msr-utils

#Datei aus dem RT-Libverzeichnis für dies Projekt
RTSRC = msr_main.c msr_lists.c msr_charbuf.c msr_reg.c msr_interpreter.c msr_utils.c msr_messages.c msr_functiongen.c msr_base64.c msr_watchdog.c msr_proc.c msr_error_reg.c

ALLSRC = $(SRC) $(RTSRC)

OBJS = $(ALLSRC:.c=.o)  


all: .depend $(TARGET).o Makefile

$(TARGET).o: $(SRC:.c=.o) $(RTSRC:.c=.o)
	$(LD) -r $(OBJS)  -o $@ $(LDFLAGS)

install:
	cp $(TARGET).o /opt/msr/lib/modules

clean:
	rm -f *.o *~ core .depend

mrproper:
	rm -f *.o *~ core .depend


depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > $@@




ifeq (.depend,$(wildcard .depend))
include .depend
endif












