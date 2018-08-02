DBGFLAGS = -O2 -g0
ifeq ($(DEBUG),1)
DBGFLAGS = -O0 -g3 -DNOSPLASH
endif
ifeq ($(PROFILE),1)
DBGFLAGS = -O2 -g3
endif

CC      = gcc
CFLAGS  = -Wall $(DBGFLAGS) -DENV_LINUX -D_REENTRANT -L /usr/lib -L .. -I /usr/include -I .. -I . -I../../plugins -I../../liboapc

LDFLAGS  = -lm -loapc -lrt

PNAME=ilPLC

OBJ = main.o isconnect.o
SRC = main.c isconnect.c

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(PNAME) $(OBJ) $(LDFLAGS)

clean:
	rm *.o
	rm $(PNAME)
