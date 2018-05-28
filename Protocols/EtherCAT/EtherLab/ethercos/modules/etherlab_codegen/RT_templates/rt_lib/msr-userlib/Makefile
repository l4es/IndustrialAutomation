
#Makefile um die MSR-LIB für Usermode zu kompilieren

#includeverzeichnis der rtlib
RTINCLUDE = rt_lib/include
INCLUDEDIR = 

# CFLAGS: all assignments to CFLAGS are incremental, so you can specify
# the initial flags on the command line or environment, if needed.
CFLAGS = -O2
CFLAGS +=  -Wall -Wuninitialized -I$(RTINCLUDE)


#TARGET = msr-usr-lib
TARGET = main
OBJS = $(TARGET).o

#Datei aus dem RT-Libverzeichnis für dies Projekt
SRC =  usr-main.c \
       rt_lib/msr-core/msr_main.c \
       rt_lib/msr-core/msr_charbuf.c \
       rt_lib/msr-core/msr_reg.c \
       rt_lib/msr-core/msr_interpreter.c \
       rt_lib/msr-utils/msr_utils.c \
       rt_lib/msr-core/msr_messages.c \
       rt_lib/msr-core/msr_error_reg.c \
       rt_lib/msr-core/msr.c \
       rt_lib/msr-math/msr_base64.c 


all: .depend $(TARGET)


$(TARGET).o: $(SRC:.c=.o)
	$(LD) -r $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(SRC:.c=.o) *.o *~ core .depend

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > $@


ifeq (.depend,$(wildcard .depend))
include .depend
endif









